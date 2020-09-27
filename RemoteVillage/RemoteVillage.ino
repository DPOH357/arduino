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
    if(pMessage->equalsIgnoreCase("fun"))
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
