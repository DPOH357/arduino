#include <RotaryEncoder.h>
#include <FastLED.h>

//---------------------------------------------------------------------------------------------------------

//#define ENABLE_PRINT

#ifdef ENABLE_PRINT
  unsigned int printStep = 0;
  #define PRINT_INIT(VALUE) Serial.begin(VALUE);
  #define PRINT_STEP ++printStep; if(printStep > 20) printStep = 0;
  #define PRINT_VALUE(VALUE) if(printStep == 0) { Serial.print(VALUE); Serial.print(" | "); }
  #define PRINT_LN if(printStep == 0) Serial.println();
#else
  #define PRINT_INIT(VALUE)
  #define PRINT_STEP
  #define PRINT_VALUE(VALUE)
  #define PRINT_LN
#endif

template <class T>
struct RgbData
{
  T r = 0;
  T g = 0;
  T b = 0;
};

using ButtonsState = RgbData<bool>;

ButtonsState bs;

class HoldButton
{
public:
  HoldButton(const int pin)
    : m_pin(pin)
    , m_statePrev(LOW)
    , m_bEnabled(false)
  {
    pinMode(m_pin, INPUT_PULLUP);
    digitalWrite(m_pin, HIGH);

    m_statePrev = digitalRead(m_pin);
  }

  void setEnabled(const bool bEnabled)
  {
    m_bEnabled = bEnabled;
  }

  bool isEnabled()
  {
    const int state = digitalRead(m_pin);
    if(state != m_statePrev)
    {
      m_statePrev = state;
      m_bEnabled = !m_bEnabled;
    }

    return m_bEnabled;    
  }

private:
  const int m_pin;
  int m_statePrev;
  bool m_bEnabled;
};

//---------------------------------------------------------------------------------------------------------

class EncoderPosition
{
public:
  EncoderPosition(const int pin1, const int pin2)
    : m_encoder(pin1, pin2)
  {
  }

  void reset()
  {
    m_encoder.setPosition(0);
  }

  int getPosition()
  {
    m_encoder.tick();
    return m_encoder.getPosition();
  }

private:
  RotaryEncoder m_encoder;
};

//---------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------

class Sensor
{
  enum class Mode
  {
    Default,
    PowerOn,
    Check
  };
  
public:
  Sensor(
    const int pinSensor,
    const int pinSensorPower)
    : m_pinSensor(pinSensor)
    , m_pinSensorPower(pinSensorPower)
    , m_bSensor(false)
    , m_timer(ms_timePeriodCheck)
    , m_mode(Mode::Default)
  {
    pinMode(m_pinSensor, INPUT);
    m_bSensor = (digitalRead(m_pinSensor) == LOW);

    pinMode(m_pinSensorPower, OUTPUT);
    digitalWrite(m_pinSensorPower, LOW);
  }

  void process()
  {
    switch(m_mode)
    {
      case Mode::PowerOn:
      {
        if(m_timer.isComplete())
        {
          const bool bSensor = (digitalRead(m_pinSensor) == LOW);

          if(bSensor)
          {
            m_timer.restart(ms_timePeriodFastCheck);
            m_mode = Mode::Check;
          }
          else
          {
            m_timer.restart(ms_timePeriodCheck);
            digitalWrite(m_pinSensorPower, LOW);
            m_mode = Mode::Default;
            m_bSensor = false;
          }
        }
      }
      break;

      case Mode::Check:
      {
        if(m_timer.isComplete())
        {
          m_bSensor = (digitalRead(m_pinSensor) == LOW);
          m_timer.restart(ms_timePeriodCheck);
          digitalWrite(m_pinSensorPower, LOW);
          m_mode = Mode::Default;
        }        
      }
      break;

      case Mode::Default:
      default:
      {
        if(m_timer.isComplete())
        {
          digitalWrite(m_pinSensorPower, HIGH);
          m_mode = Mode::PowerOn;
          m_timer.restart(ms_timePeriodPowerOn);
        }
      }
      break;
    }
  }

  bool check() const
  {
    return m_bSensor;
  }

private:
  static const uint32_t ms_timePeriodCheck = 2000;
  static const uint32_t ms_timePeriodFastCheck = 100;
  static const uint32_t ms_timePeriodPowerOn = 100;
  const int m_pinSensor;
  const int m_pinSensorPower;
  bool m_bSensor;
  CountdownTimer m_timer;
  Mode m_mode;
};

//---------------------------------------------------------------------------------------------------------

class FunManager
{
public:
  FunManager(
    const int pinSensor,
    const int pinSensorPower,
    const int pinFun)
    : m_pinFun(pinFun)
    , m_timer(ms_timeCheck)
    , m_sensor(pinSensor, pinSensorPower)
    , m_bSensorPrev(false)
  {
    pinMode(m_pinFun, OUTPUT);

    digitalWrite(m_pinFun, LOW);
  }

  void process()
  {
    m_sensor.process();
    if(m_timer.isComplete())
    {
      const bool bSensor = m_sensor.check();
      if(bSensor && !m_bSensorPrev)
      {
        digitalWrite(m_pinFun, HIGH);
        m_timer.restart(ms_timeFunOn);
      }
      else
      {
        digitalWrite(m_pinFun, LOW);
        m_timer.restart(ms_timeCheck);
      }
      m_bSensorPrev = bSensor;
    }
  }

private:
  static const unsigned int ms_timeFunOn = 10 * 1000;
  static const unsigned int ms_timeCheck = 2000;
  const int m_pinFun;
  CountdownTimer m_timer;
  Sensor m_sensor;
  bool m_bSensorPrev;
};

//---------------------------------------------------------------------------------------------------------

template <size_t LAMP_COUNT>
class LedLine
{
  enum class Mode
  {
    Present,
    Default
  };
  
public:
  LedLine(float pos)
    : m_mode(Mode::Default)
    , m_speed(0)
    , m_pos(pos)
    , m_bEnabled(true)
  {
    for(size_t i = 0; i < LAMP_COUNT; ++i)
    {
      m_arrLamp[i] = 0;    
    }
    
    setPos(pos);
  }

  void setPos(const float pos)
  {
    if(pos > LAMP_COUNT - 1)
    {
      m_pos = LAMP_COUNT - 1;
    }
    else
    if(pos < 0)
    {
      m_pos = 0;
    }
    else
    {
      m_pos = pos;
    }
  }

  void setSpeed(const float speed)
  {
    if(m_bEnabled)
    {
      m_speed = speed;
    }
  }

  void setPresent()
  {
    m_mode = Mode::Present;

    for(size_t i = 0; i < LAMP_COUNT; ++i)
    {
      m_arrLamp[i] = 0;
    }
  }
  
  void setEnabled(const bool bEnabled)
  {
    m_bEnabled = bEnabled;

    if(!bEnabled)
    {
      m_speed = 0;
      m_mode = Mode::Default;
    }
  }

  void process()
  {    
    switch(m_mode)
    {
      case Mode::Present:
      {
        if(m_bEnabled)
        {          
          static const float delta = 0.3;
          for(size_t i = 0; i < LAMP_COUNT; ++i)
          {
            float& lamp = m_arrLamp[i];
            lamp += delta;
            if(lamp > 255.0)
            {
              lamp = 255;
              m_mode = Mode::Default;
            }
          } 
        }
      }
      break;

      default:
      {        
        static const float delta = 0.5;
        for(size_t i = 0; i < LAMP_COUNT; ++i)
        {
          float& lamp = m_arrLamp[i];
          lamp -= delta;

          if(lamp > 255.0)
          {
            lamp = 255;
          }
          else if(lamp < 0)
          {
            lamp = 0;
          }
        }
    
        if(m_bEnabled)
        {
          m_pos -= m_speed;
          while(m_pos > LAMP_COUNT)
          {
            m_pos -= (float)LAMP_COUNT;
          }
      
          while(m_pos < 0.0)
          {
            m_pos += (float)LAMP_COUNT;
          }
      
          const size_t nPos1 = m_pos;
          const size_t nPos2 = nPos1 + 1 < LAMP_COUNT ? nPos1 + 1 : 0;
          const float prop = m_pos - (float)nPos1;
      
          const float value1 = 255.0 * (1.0 - prop);
          const float value2 = 255.0 * prop;
      
          m_arrLamp[nPos1] = value1 > m_arrLamp[nPos1] ? value1 : m_arrLamp[nPos1];
          m_arrLamp[nPos2] = value2 > m_arrLamp[nPos2] ? value2 : m_arrLamp[nPos2]; 
        }   
      }
    }        
  }

  unsigned char getLight(const size_t num)
  {
    return m_arrLamp[num]; 
  }

private:
  Mode m_mode;
  float m_arrLamp[LAMP_COUNT];
  float m_speed;
  float m_pos;
  bool m_bEnabled;
};

//---------------------------------------------------------------------------------------------------------

template <int PIN_LED, size_t LED_COUNT>
class LedRing
{
public:
  LedRing()
    : m_ledLineRed(0)
    , m_ledLineGreen(0)
    , m_ledLineBlue(0)
  {
    FastLED.addLeds<WS2812B, PIN_LED, GRB>(m_leds, LED_COUNT);
    FastLED.setBrightness(30);
  
    m_arrLedLine[0] = &m_ledLineRed;
    m_arrLedLine[1] = &m_ledLineGreen;
    m_arrLedLine[2] = &m_ledLineBlue;
  }

  void setPresent()
  {
    for(size_t i = 0; i < 3; ++i)
    {
      m_arrLedLine[i]->setPresent();
    }
  }

  void setPosition(
    const float posRed = 0, 
    const float posGreen = 0, 
    const float posBlue = 0)
  {
    m_ledLineRed.setPos(posRed);
    m_ledLineGreen.setPos(posGreen);
    m_ledLineBlue.setPos(posBlue);
  }

  void setSpeed(
    const float speedRed,
    const float speedGreen,
    const float speedBlue)
  {
    m_ledLineRed.setSpeed(speedRed);
    m_ledLineGreen.setSpeed(speedGreen);
    m_ledLineBlue.setSpeed(speedBlue);
  }

  void setEnabled(
    const bool bRed,
    const bool bGreen,
    const bool bBlue)
  {
    m_ledLineRed.setEnabled(bRed);
    m_ledLineGreen.setEnabled(bGreen);
    m_ledLineBlue.setEnabled(bBlue);
  }

  void process()
  {
    for(size_t i = 0; i < 3; ++i)
    {
      m_arrLedLine[i]->process();
    }

    for(size_t i = 0; i < LED_COUNT; ++i) 
    {
      m_leds[i] = CRGB(m_ledLineRed.getLight(i), m_ledLineGreen.getLight(i), m_ledLineBlue.getLight(i));
    }
  
    FastLED.show();
  }

private:
  CRGB m_leds[LED_COUNT];

  LedLine<LED_COUNT> m_ledLineRed;
  LedLine<LED_COUNT> m_ledLineGreen;
  LedLine<LED_COUNT> m_ledLineBlue;

  LedLine<LED_COUNT>* m_arrLedLine[3];
};

//---------------------------------------------------------------------------------------------------------

const int c_pinSwitch = 8;

const int c_numLeds = 16;
const int c_pinLed = 13;

HoldButton buttonRed(10);
HoldButton buttonGreen(11);
HoldButton buttonBlue(12);

EncoderPosition encoderRed(2, 5);
EncoderPosition encoderGreen(3, 6);
EncoderPosition encoderBlue(4, 7);

LedRing<c_pinLed, 16> ledRing;

FunManager funManager(A1, A4, A5);

const float c_posLedRed = 0;
const float c_posLedGreen = 5;
const float c_posLedBlue = 10;

bool bSwitcherPrev = false;

void setup() 
{
  PRINT_INIT(9600);

  pinMode(c_pinSwitch, INPUT);
  bSwitcherPrev = (digitalRead(c_pinSwitch) == HIGH);

  if(bSwitcherPrev)
  {
    ledRing.setPosition(c_posLedRed, c_posLedGreen, c_posLedBlue);
    ledRing.setPresent();
  }
  else
  {
    ledRing.setEnabled(false, false, false);
  }
}

void loop() 
{
  const bool bSwitcher = (digitalRead(c_pinSwitch) == HIGH);

  PRINT_VALUE(bSwitcher);

  if(bSwitcher != bSwitcherPrev)
  {
    if(bSwitcher)
    {
      ledRing.setPosition(c_posLedRed, c_posLedGreen, c_posLedBlue);
      ledRing.setPresent();

      buttonRed.setEnabled(true);
      buttonGreen.setEnabled(true);
      buttonBlue.setEnabled(true);
  
      encoderRed.reset();
      encoderGreen.reset();
      encoderBlue.reset();
    }
    else
    {
      ledRing.setEnabled(false, false, false);   
    }

    bSwitcherPrev = bSwitcher;
  }

  if(bSwitcher)
  {
    const float mn = 0.002;
    const float encRed = encoderRed.getPosition() * mn;
    const float encGreen = encoderGreen.getPosition() * mn;
    const float encBlue = encoderBlue.getPosition() * mn;
  
    const bool bRed = buttonRed.isEnabled();  
    const bool bGreen = buttonGreen.isEnabled();
    const bool bBlue = buttonBlue.isEnabled();
    
    PRINT_VALUE(encRed);
    PRINT_VALUE(encGreen);
    PRINT_VALUE(encBlue);
  
    PRINT_VALUE(bRed);
    PRINT_VALUE(bGreen);
    PRINT_VALUE(bBlue);
  
    PRINT_LN;

    ledRing.setEnabled(bRed, bGreen, bBlue);
    ledRing.setSpeed(encRed, encGreen, encBlue);  
  }

  ledRing.process();

  funManager.process();

  PRINT_STEP;
}
