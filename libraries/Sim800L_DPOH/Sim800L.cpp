#include "Sim800L.h"

SoftwareSerialWrapper::SoftwareSerialWrapper(const int pinRx, const int pinTx)
    : m_softserial(pinRx, pinTx)
{

}

void SoftwareSerialWrapper::begin()
{
    m_softserial.begin(9600); // Скорость обмена данными с модемом
    m_softserial.setTimeout(2000);
}

bool SoftwareSerialWrapper::isAvailable() const
{
    return m_softserial.available();
}

void SoftwareSerialWrapper::request(const String& command)
{
    m_softserial.println(command);
}

String SoftwareSerialWrapper::requestWaitResponce(const String& command)
{
    m_softserial.println(command);

    return waitResponce();
}

void SoftwareSerialWrapper::requestWaitResponce(const String& command, const String& responce)
{
    String responceCurrent = "";
    do
    {
      responceCurrent = requestWaitResponce(command);
      responceCurrent.trim();
    }
    while(responceCurrent != responce);
}

String SoftwareSerialWrapper::waitResponce(const String& command = "")
{
    String responce = "";
    const long timeout = millis() + 10000;

    while(millis() < timeout)
    {
        const int avail = m_softserial.available();
        TRACE("SoftwareSerialWrapper - Available " + avail);

        if(avail)
        {
            responce += m_softserial.readString();

            // Убираем из ответа дублирующуюся команду
            if(command != "" && responce.startsWith(command))
            {
                responce = responce.substring(responce.indexOf("\r", command.length()) + 2);
            }

            responce.trim();

            break;
        }
        delay(200);
    }

    return responce;
}

//---------------------------------------------------------

Sim800L::Sim800L(const int pinRx, const int pinTx)
    : m_ssw(pinRx, pinTx)
{

}

void Sim800L::begin()
{
    TRACE("Sim800L beginning...");

    m_ssw.begin();

    m_ssw.requestWaitResponce("AT");             // Автонастройка скорости
    //m_ssw.requestWaitResponce("AT+CLVL?");       // Запрашиваем громкость динамикаSMS
    m_ssw.requestWaitResponce("AT+CMGF=1");      // Включить TextMode для SMS
    //m_ssw.requestWaitResponce("AT+DDET=1,0,1");  // Включить DTMF
    //m_ssw.requestWaitResponce("AT+CLIP=1");      // Включить АОН

    //m_ssw.requestWaitResponce("AT+CMGDA=\"DEL SENT\"");
    TRACE("Sim800L begin");
}

void Sim800L::loop()
{
    // !!! добавить периодический опрос сохранённых событий

    if (m_ssw.isAvailable())
    {
        const String responce = m_ssw.waitResponce();

        TRACE ("Sim800L::loop: '" + responce + "'");

        // Пришло сообщение об отправке SMS
        if(responce.startsWith("+CMTI:"))
        {
            TRACE ("Sim800L::loop 1");

            const int i = responce.lastIndexOf(",");   // Находим последнюю запятую, перед индексом
            TRACE ("Sim800L::loop 2 - " + i);
            String index = responce.substring(i + 1, responce.length()); // Получаем индекс
            index.trim(); // Убираем пробельные символы в начале/конце
            m_ssw.request("AT+CMGR=" + index); // Получить содержимое SMS
        }
        else
        // получили содержимое SMS
        if(responce.startsWith("AT+CMGR"))
        {
            TRACE ("Sim800L::loop: inputSmsHandler");
            inputSmsHandler(responce);
        }

        TRACE ("Sim800L::loop: responce processed");
    }
}

void Sim800L::getSms(
    UniquePtr<String>& pPhoneNumber,
    UniquePtr<String>& pMessage)
{
    m_pPhoneNumber.moveTo(pPhoneNumber);
    m_pMessage.moveTo(pMessage);
}

void Sim800L::sendSms(
    const String& phoneNumber,
    const String& message)
{
    m_ssw.requestWaitResponce("AT+CMGS=\"" + phoneNumber + "\"");
    m_ssw.requestWaitResponce(message + (char)26);
}

void Sim800L::inputSmsHandler(const String& responce)
{
    TRACE("inputSmsHandler 1");
    const int indexStart = responce.indexOf("+CMGR: ");
    const String msg = responce.substring(indexStart);

    //const int okIndex = msg.lastIndexOf("OK");

    //const int indexStart = message.indexOf("+CMGR: ");
    //parseSMS(message.substring(indexStart));                      // Распарсить SMS на элементы
    m_ssw.requestWaitResponce("AT+CMGDA=\"DEL ALL\""); // Удалить все прочитанные сообщения, чтобы не забивали память модуля
    //m_ssw.requestWaitResponce("AT+CMGD=" + index);

    const int indexHeaderEnd = msg.indexOf("\n");

    {
        TRACE("inputSmsHandler 2");
        const int indexStart = msg.indexOf("\",\"") + 3;
        const int indexEnd = msg.indexOf("\",\"", indexStart);

        m_pPhoneNumber.reset("");
        *m_pPhoneNumber = msg.substring(indexStart, indexEnd);
        TRACE("Phone: \"" + *m_pPhoneNumber + "\"");
    }

    {
        TRACE("inputSmsHandler 3");
        const int indexEnd = msg.lastIndexOf("\r\n\r\nOK");
        m_pMessage.reset("");
        *m_pMessage = msg.substring(indexHeaderEnd + 1, indexEnd);
        TRACE("Message: \"" + *m_pMessage + "\"");
    }
}
