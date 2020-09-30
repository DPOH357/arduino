#define DEBUG

#include <DS1307RTC.h>
#include <Nrf24L01.h>
#include <RemoteVillageCommon.h>

#include <SD.h>
#include <Sim800L.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Wire.h>

#define PIN_SD_SELECT 8
#define PIN_RX  A1 /*green*/
#define PIN_TX  A0 /*yellow*/

#define PIN_RADIO_CE 9
#define PIN_RADIO_CSN 10
#define RADIO_CHANNEL 1

#define LOG_FILE "log.txt"
#define TICK_PERIOD 60000

#define MSG_REQUEST "req"
#define MSG_TEST "test"

const String validPhones("+79104828275 +79773927957");

//---------------------------------------------------------

String getCurrentTime()
{
    tmElements_t tm;
    RTC.read(tm);

    char textBuffer[21];
    sprintf(textBuffer, "%02d.%02d.%04d %02d:%02d:%02d", tm.Day, tm.Month, tmYearToCalendar(tm.Year), tm.Hour, tm.Minute, tm.Second);
    
    return textBuffer;
}

bool writeLog(const String& text)
{
    File dataFile = SD.open(LOG_FILE, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile)
    {
        dataFile.print(getCurrentTime());
        dataFile.print(": ");
        dataFile.println(text);
        dataFile.close();
    }

    return dataFile;
}

//---------------------------------------------------------

Sim800L sim800l(PIN_RX, PIN_TX);
CountdownTimer timer(TICK_PERIOD);
Nrf24L01 radio(PIN_RADIO_CE, PIN_RADIO_CSN);

void setup()
{
    INIT_TRACE;

    TRACE("Starting...");

    sim800l.begin();

#if 0
    if (getDate(__DATE__) && getTime(__TIME__)) 
    {
        RTC.write(tm);
    }
#endif

    SD.begin(PIN_SD_SELECT);

    {
        Vector<Nrf24L01::PipeId> vecInputPipes;
        Vector<Nrf24L01::PipeId> vecOutputPipes;
        vecOutputPipes.push_back(PIPE_1_OUT);
        if(radio.begin(RADIO_CHANNEL, vecInputPipes, vecOutputPipes))
        {
            TRACE("Radio is not ready");
        }
    }

    TRACE("Start");
    writeLog("Start");
}

void loop()
{
    sim800l.loop();

    UniquePtr<String> pPhoneNumber;
    UniquePtr<String> pMessage;

    sim800l.getSms(pPhoneNumber, pMessage);
    if(pPhoneNumber
    && pMessage)
    {
        pMessage->trim();
        const String logText = "Receive SMS from " + *pPhoneNumber
            + ", message: \"" + *pMessage + "\"";

        TRACE(logText);
            
        writeLog(logText);
        
        if(validPhones.indexOf(*pPhoneNumber) != -1)
        {
            if(pMessage->equalsIgnoreCase(MSG_REQUEST))
            {
                TRACE("Send message");
                //sim800l.sendSms(*pPhoneNumber, *pMessage + "?, I am robot! :)");
            }
            else
            if(pMessage->equalsIgnoreCase(MSG_TEST))
            {
                const int indexSpace = pMessage->indexOf(" ");
                if (indexSpace != -1)
                {
                    const String strNumber = pMessage->substring(indexSpace + 1);
                    TRACE("Set lamp: '" + strNumber + "'");
                    Data data(Command::SetLamp, strNumber.toInt());
                    radio.send(data);
                }
            }
        }
    }

    if(timer.isComplete())
    {
        timer.restart();
        writeLog("Tick");
    }
}
