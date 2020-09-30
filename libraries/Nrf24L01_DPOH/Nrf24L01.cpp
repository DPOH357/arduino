#include <Nrf24L01.h>

Nrf24L01::Nrf24L01(
    const int pinCe,
    const int pinCsn)
    : m_radio(pinCe, pinCsn)
    , m_bListening(false)
{

}

bool Nrf24L01::begin(
    const uint8_t channel,
    const Vector<PipeId>& vecInputPipes,
    const Vector<PipeId>& vecOutputPipes)
{
    m_radio.begin();
    const bool bSuccess = m_radio.isPVariant();
    if(bSuccess)
    {
        m_radio.setChannel(channel);
        m_radio.setDataRate(RF24_1MBPS);
        m_radio.setPALevel(RF24_PA_HIGH);

        const uint32_t vecInputPipesSize = vecInputPipes.size();
        if (vecInputPipesSize)
        {
            for (uint32_t i = 0; i < vecInputPipesSize; ++i)
            {
                m_radio.openReadingPipe(1, vecInputPipesSize[i]);
            }

            m_bListening = true;
            m_radio.startListening();
        }

        const uint32_t vecOutputPipesSize = vecOutputPipes.size();
        if (vecOutputPipesSize)
        {
            for (uint32_t i = 0; i < vecOutputPipesSize; ++i)
            {
                m_radio.openWritingPipe(vecOutputPipes[i]);
            }
        }
    }

    return bSuccess;
}

bool Nrf24L01::send(const PipeId pipeId, const void* pData, const uint32_t dataSize)
{
    if (m_bListening)
    {
        m_radio.stopListening();
    }

    const bool bSuccess = m_radio.write(pData, dataSize);

    if (m_bListening)
    {
        m_radio.startListening();
    }

    return bSuccess;
}

PipeId Nrf24L01::receive(void* pData, const uint32_t dataSize) const
{
    PipeId pipeId = 0;
    if(radio.available(&pipeId))
    {
        m_radio.read(pData, dataSize);
    }

    return pipeId;
}
