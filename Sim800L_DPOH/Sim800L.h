#ifndef DPOH_SIM_800_L_H 
#define DPOH_SIM_800_L_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#ifdef DEBUG
    #define INIT_TRACE Serial.begin(9600);               // Скорость обмена данными с компьютером
    #define TRACE(MSG) Serial.println(MSG);
#else
    #define INIT_TRACE
    #define TRACE(MSG)
#endif

//---------------------------------------------------------

template <class T>
class UniquePtr
{
public:
    UniquePtr(T* ptr = nullptr)
        : m_ptr(ptr)
    {

    }

    ~UniquePtr()
    {
        reset();
    }

    void moveFrom(UniquePtr<T>& uptr)
    {
        reset(uptr.m_ptr);
        uptr.m_ptr = nullptr;
    }

    void moveTo(UniquePtr<T>& uptr)
    {
        uptr.reset(m_ptr);
        m_ptr = nullptr;
    }

    operator bool() const
    {
        return m_ptr;
    }

    T* operator->()
    {
        return m_ptr;
    }

    const T* operator->() const
    {
        return m_ptr;
    }

    T& operator*()
    {
        return *m_ptr;
    }

    const T& operator*() const
    {
        return *m_ptr;
    }

    void reset(T* ptr = nullptr)
    {
        if(m_ptr)
        {
            delete m_ptr;
        }

        m_ptr = ptr;
    }

    void reset(const T& val)
    {
        if(m_ptr)
        {
            delete m_ptr;
        }

        m_ptr = new T(val);
    }

private:
    UniquePtr(UniquePtr<T>& uptr)
        : m_ptr(uptr.m_ptr)
    {
        uptr.m_ptr = nullptr;
    }

private:
    T* m_ptr;
};

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
