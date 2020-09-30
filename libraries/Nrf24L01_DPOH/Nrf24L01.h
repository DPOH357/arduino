#ifndef NRF24L01_DPOH_H
#define NRF24L01_DPOH_H

#include <DPOH.h>
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

class Nrf24L01
{
public:
    using PipeId = uint8_t;

    Nrf24L01(
        const int pinCe,
        const int pinCsn);

    bool begin(
        const uint8_t channel,
        const Vector<PipeId>& vecInputPipes,
        const Vector<PipeId>& vecOutputPipes);

    bool send(
        const PipeId pipeId,
        const void* pData,
        const uint32_t dataSize);

    template <class T>
    bool send(const PipeId pipeId, const T& data)
    {
        return send(pipeId, &data, sizeof(T));
    }

    // if PipeId not 0 then data received
    PipeId receive(
        void* pData,
        const uint32_t dataSize) const;

    template <class T>
    PipeId receive(UniquePtr<T>& pData)
    {
        if(!pData)
        {
            pData.reset(new T());
        }
        return receive(pData.get(), sizeof(T));
    }

private:
    RF24 m_radio;
    bool m_bListening;
}

#endif // NRF24L01_DPOH_H
