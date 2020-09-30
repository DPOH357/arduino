#ifndef DPOH_SIM_800_L_H 
#define DPOH_SIM_800_L_H

#include <Arduino.h>
#include <DPOH.h>
#include <SoftwareSerial.h>

//---------------------------------------------------------

class SoftwareSerialWrapper
{
public:
    SoftwareSerialWrapper(const int pinRx, const int pinTx);

    void begin();

    bool isAvailable() const;

    void request(const String& command);

    String requestWaitResponce(const String& command);

    void requestWaitResponce(const String& command, const String& responce);

    String waitResponce(const String& command = "");

private:
    SoftwareSerial m_softserial;
};

//---------------------------------------------------------

class Sim800L
{
public:
    Sim800L(const int pinRx, const int pinTx);

    void begin();

    void loop();

    void getSms(
        UniquePtr<String>& pPhoneNumber,
        UniquePtr<String>& pMessage);

    void sendSms(
        const String& phoneNumber,
        const String& message);

private:
    void inputSmsHandler(const String& responce);

private:
    SoftwareSerialWrapper m_ssw;

    UniquePtr<String> m_pPhoneNumber;
    UniquePtr<String> m_pMessage;
};

#endif // DPOH_SIM_800_L_H
