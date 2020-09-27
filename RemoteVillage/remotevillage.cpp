
#if 0

#include <sim800l.h>

#define PIN_LAMP 13

//----------------------------------------------------------------------------

void parseSMS(String msg)
{
  Serial.println("Parsing string: \"" + msg + "\"");

  const int indexHeaderEnd = msg.indexOf("\n");

  String phone;
  {
    const int indexStart = msg.indexOf("\",\"") + 3;
    const int indexEnd = msg.indexOf("\",\"", indexStart);
    phone = msg.substring(indexStart, indexEnd);
  }

  String text;
  {
    const int indexEnd = msg.lastIndexOf("\r\n\r\nOK");
    text = msg.substring(indexHeaderEnd + 1, indexEnd);
  }

  Serial.println("Phone: \"" + phone + "\"");
  Serial.println("Message: \"" + text + "\"");

  if(phone == "+79773927957")
  {
    const int value = (text == "1" ? HIGH : LOW);
    digitalWrite(c_pinLamp, value);
  }
}

//----------------------------------------------------------------------------

const int c_pinRx = 8;
const int c_pinTx = 9;

SoftwareSerial sim800(c_pinRx, c_pinTx);

void setup()
{
  pinMode(c_pinLamp, OUTPUT);

  Serial.begin(9600);               // Скорость обмена данными с компьютером
  Serial.println("Starting...");

  sim800.begin(9600);               // Скорость обмена данными с модемом
  sim800.setTimeout(2000);

  Sim800Cmd command(sim800);
  command.SendAndWait("AT");             // Автонастройка скорости
  //command.SendAndWait("AT+CLVL?");       // Запрашиваем громкость динамикаSMS
  command.SendAndWait("AT+CMGF=1");      // Включить TextMode для
  //command.SendAndWait("AT+DDET=1,0,1");  // Включить DTMF
  //command.SendAndWait("AT+CLIP=1");      // Включить АОН

  Serial.println("Ready");
}

void loop()
{
  Sim800Cmd command(sim800);

  if (sim800.available())
  {
    const String responce = command.WaitResponce();
    Serial.println(responce);

    //return;

    // Пришло сообщение об отправке SMS
    if(responce.startsWith("+CMTI:"))
    {
      const int i = responce.lastIndexOf(",");   // Находим последнюю запятую, перед индексом
      String index = responce.substring(i + 1, responce.length()); // Получаем индекс
      index.trim();                            // Убираем пробельные символы в начале/конце
      command.Send("AT+CMGR=" + index); // Получить содержимое SMS
    }
    else
    if(responce.startsWith("AT+CMGR"))
    {
      const int indexStart = responce.indexOf("+CMGR: ");
      const String text = responce.substring(indexStart);

      const int okIndex = text.lastIndexOf("OK");
      Serial.print("OK = ");
      Serial.println(okIndex);

      //const int indexStart = message.indexOf("+CMGR: ");
      //parseSMS(message.substring(indexStart));                      // Распарсить SMS на элементы
      command.SendAndWait("AT+CMGDA=\"DEL ALL\""); // Удалить все сообщения, чтобы не забивали память модуля
      //command.SendAndWait("AT+CMGD=" + index);

      parseSMS(text);
    }
  }


  if (Serial.available())
  {
    const String request = Serial.readString();
    if(request.indexOf("???") == 0)
    {
      command.Send("AT+CMGR=1,1");
    }
    else
    {
      sim800.println(request);
    }
  }
}

#endif 
