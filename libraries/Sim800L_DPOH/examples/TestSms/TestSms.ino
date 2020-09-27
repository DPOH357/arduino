/*
 * DPOH electronics
 * 
 * Программа работает с модулем SIM800L, подключенному к PIN_RX и PIN_TX.
 * При получении СМС от номера VALID_PHONE с текстом "lamp" (независимо от регистра)
 * изменяет значение на противоположенное на PIN_LAMP. В случае получении иного текста 
 * сообщения - отправляет СМС с текстом "<текст принятого сообщения>?, I am robot! :)".
 */

//#define DEBUG

#include <Sim800L.h>
          
#define PIN_LAMP 13
#define PIN_RX  8
#define PIN_TX  9

#define VALID_PHONE "+79773927957"


Sim800L sim800l(PIN_RX, PIN_TX);
bool bLamp = false;

void setup()
{
  INIT_TRACE;

  TRACE("Starting...");

  sim800l.begin();

  pinMode(PIN_LAMP, OUTPUT);
  digitalWrite(PIN_LAMP, LOW);

  TRACE("Ready");
}

void loop()
{
  sim800l.loop();

  UniquePtr<String> pPhoneNumber;
  UniquePtr<String> pMessage;

  sim800l.getSms(pPhoneNumber, pMessage);
  if(pPhoneNumber 
  && pMessage
  && *pPhoneNumber == VALID_PHONE)
  {
    TRACE(*pMessage);
    pMessage->trim();
    if(pMessage->equalsIgnoreCase("lamp"))
    {
      bLamp = !bLamp;
      const int funMode = bLamp ? HIGH : LOW;
      digitalWrite(PIN_LAMP, funMode);
    }
    else
    {
      sim800l.sendSms(*pPhoneNumber, *pMessage + "?, I am robot! :)");
    }
  }
}