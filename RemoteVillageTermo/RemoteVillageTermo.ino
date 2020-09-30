#define DEBUG

#include <DPOH.h>
#include <DS1307RTC.h>
#include <Nrf24L01.h>
#include <RemoteVillageCommon.h>

#define PIN_LAMP 5

#define PIN_RADIO_CE 9
#define PIN_RADIO_CSN 10
#define RADIO_CHANNEL 1

//---------------------------------------------------------

Nrf24L01 radio(PIN_RADIO_CE, PIN_RADIO_CSN);

void setup()
{
    INIT_TRACE;

    TRACE("Starting...");

    pinMode(PIN_LAMP, OUTPUT);
    analogWrite(PIN_LAMP, 0);

    Vector<Nrf24L01::PipeId> vecInputPipes;
    vecInputPipes.push_back(PIPE_1_OUT);

    Vector<Nrf24L01::PipeId> vecOutputPipes;

    if(radio.begin(RADIO_CHANNEL, vecInputPipes, vecOutputPipes))
    {
        TRACE("Radio is not ready");
    }

    TRACE("Start");
}

void loop()
{
    UniquePtr<RadioData> pData;
    if (radio.receive(pData))
    {
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
}
