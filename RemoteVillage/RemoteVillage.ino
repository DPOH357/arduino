#define DEBUG

#include <DS1307RTC.h>
#include <SD.h>
#include <Sim800L.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Wire.h>

#define PIN_SD_SELECT 8
#define PIN_RX  A1 /*green*/
#define PIN_TX  A0 /*yellow*/

#define LOG_FILE "log.txt"
#define TICK_PERIOD 60000

#define MSG_REQUEST "req"

const String validPhones("+79104828275 +79773927957");

//---------------------------------------------------------

class CountdownTimer
{
public:
  CountdownTimer(const uint32_t timePeriod)
    : m_timePeriod(timePeriod)
    , m_timeBegin(millis())
  {

  }

  void restart(const uint32_t timePeriod = 0)
  {
    if (timePeriod)
    {
      m_timePeriod = timePeriod;
    }

    m_timeBegin = millis();
  }

  bool isComplete() const
  {
    return (millis() - m_timeBegin) > m_timePeriod;
  }

private:
  uint32_t m_timeBegin;
  uint32_t m_timePeriod;
};

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

    TRACE("Ready");
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
        
        if(validPhones.indexOf(*pPhoneNumber) != -1
        && pMessage->equalsIgnoreCase(MSG_REQUEST))
        {
            TRACE("Send message");
            //sim800l.sendSms(*pPhoneNumber, *pMessage + "?, I am robot! :)");
        }
    }

    if(timer.isComplete())
    {
        timer.restart();
        writeLog("Tick");
    }
}
