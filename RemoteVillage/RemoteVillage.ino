#define DEBUG

#include <DS1307RTC.h>
#include <SD.h>
#include <Sim800L.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Wire.h>

#define PIN_SD_SELECT 8
#define PIN_RX  A1
#define PIN_TX  A0

#define VALID_PHONES "+79104828275 +79773927957"
#define LOG_FILE "log.txt"

#define MSG_REQUEST "req"

Sim800L sim800l(PIN_RX, PIN_TX);

//---------------------------------------------------------

void writeLog(const String& text)
{
    tmElements_t time;
    RTC.read(time);

    const String logText = time.Day + "." + time.Month + "." time.Year
        + " " + time.Hour + ":" + time.Minute + ":" + time.Second;
        + ": " + text;

    File dataFile = SD.open(LOG_FILE, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile)
    {
        dataFile.println(logText);
        dataFile.close();
    }

    return dataFile;
}

//---------------------------------------------------------

void setup()
{
    INIT_TRACE;

    TRACE("Starting...");

    sim800l.begin();

    #ifdef DEBUG
    // get the date and time the compiler was run
    if (getDate(__DATE__) && getTime(__TIME__))
    {
        tmElements_t tm;
        RTC.write(tm);
    }

    #endif

    const bool bSdReady = SD.begin(PIN_SD_SELECT);

    TRACE("Ready");
    writeLog("Ready");
}

void loop()
{
    sim800l.loop();

    UniquePtr<String> pPhoneNumber;
    UniquePtr<String> pMessage;

    sim800l.getSms(pPhoneNumber, pMessage);
    if(pPhoneNumber
    && pMessage
    && VALID_PHONES.indexOf(*pPhoneNumber) != -1)
    {
        TRACE(*pMessage);
        pMessage->trim();
        const String logText = "Receive SMS from " + *pPhoneNumber
            + ", message: \"" + *pMessage + "\"";

        const bool bLogSuccess = writeLog(logText);

        if(pMessage->equalsIgnoreCase(MSG_REQUEST))
        {
            //sim800l.sendSms(*pPhoneNumber, *pMessage + "?, I am robot! :)");
        }
    }
}
