#define DEBUG
//#define TRANSMITTER

#include <DPOH.h>
#include <DS1307RTC.h>
#include <Nrf24L01.h>
#include <RemoteVillageCommon.h>

#define PIN_LAMP 5

#define PIN_RADIO_CE 8
#define PIN_RADIO_CSN 9
#define RADIO_CHANNEL 1

#define PIN_SD 7

//---------------------------------------------------------

class X
{
public:
  X()
  {
    pinMode(PIN_SD, OUTPUT);
    digitalWrite(PIN_SD, HIGH);

    pinMode(PIN_RADIO_CE, OUTPUT);
    digitalWrite(PIN_RADIO_CE, HIGH);
  }
};

X x;

Nrf24L01 radio(PIN_RADIO_CE, PIN_RADIO_CSN);

void setup()
{ 
    INIT_TRACE;

    TRACE("Starting...");

    Vector<Nrf24L01::PipeId> vecInputPipes;
    Vector<Nrf24L01::PipeId> vecOutputPipes;

#ifdef TRANSMITTER

    vecOutputPipes.push_back(PIPE_1_OUT);

#else

    vecInputPipes.push_back(PIPE_1_OUT);

    pinMode(PIN_LAMP, OUTPUT);
    analogWrite(PIN_LAMP, 0);

#endif

    if(!radio.begin(RADIO_CHANNEL, vecInputPipes, vecOutputPipes))
    {
        TRACE("Radio is not ready");
    }

    TRACE("Start");
}

void loop()
{
  return;
#ifdef TRANSMITTER

    static int nLamp = 0;
    RadioData data(RadioCommand::SetLamp, nLamp);
    radio.send(PIPE_1_OUT, data);
    ++nLamp;
    if(nLamp > 255)
    {
      nLamp = 0;
    }
    delay(100);

#else

    UniquePtr<RadioData> pData;
    if (radio.receive(pData))
    {
        TRACE(String("Receive: ") + (int)pData->command + " value: " + pData->value);
        switch (pData->command)
        {
        case RadioCommand::SetLamp:
        {
            analogWrite(PIN_LAMP, constrain(pData->value, 0, 255));
        }
        break;
        default:
            break;
        }
    }

#endif
}
