#define TRANSMITER

//---------------------------------------------------------------------------------

const int c_channel = 1;
const int c_pipe = 0x7878787878LL;

#ifdef TRANSMITER

#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

const int c_pinPot = 0;
const int c_pinCE = 10;
const int c_pinCSN = 9;

RF24 radio(c_pinCE, c_pinCSN);

void setup() 
{
  Serial.begin(9600);
    
  pinMode(c_pinPot, INPUT);
  
  radio.begin();
  if(radio.isPVariant())
  {
    Serial.println("Radio ready");
    radio.setChannel(c_channel);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openWritingPipe(c_pipe);
  }
  else
  {
    Serial.println("Radio not ready");
  }
}

void loop() 
{
  const int value = analogRead(c_pinPot);
  if(radio.write(&value, sizeof(value)))
  {
    Serial.print("Send value: ");
    Serial.println(value);

    delay(500);
  }
  else
  {
    Serial.println("Data not delivered");
    delay(1000);
  }
}

#else // #ifdef TRANSMITER

//---------------------------------------------------------------------------------
#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

const int c_pinLamp = 3;
const int c_pinCE = 10;
const int c_pinCSN = 9;

RF24 radio(c_pinCE, c_pinCSN);

void setup() 
{
  Serial.begin(9600);
    
  pinMode(c_pinLamp, OUTPUT);
  
  radio.begin();
  if(radio.isPVariant())
  {
    Serial.println("Radio ready");
    radio.setChannel(c_channel);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openReadingPipe(1, c_pipe);
    radio.startListening();
  }
  else
  {
        Serial.println("Radio not ready");
  }
}

void loop() 
{
  if(radio.available())
  {
    int data = 0;
    radio.read(&data, sizeof(data));
    const int value = map(data, 0, 1023, 0, 255);
    analogWrite(c_pinLamp, value);
    
    Serial.print(data);
    Serial.print(" - ");
    Serial.println(value);

    delay(500);
  }
  else
  {
    Serial.println("Radio is not available");
    delay(1000);
  }
}

#endif // #ifndef TRANSMITER
