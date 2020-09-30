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

        const unsigned int vecInputPipesSize = vecInputPipes.size();
        if (vecInputPipesSize)
        {
            for (unsigned int i = 0; i < vecInputPipesSize; ++i)
            {
                m_radio.openReadingPipe(1, vecInputPipes[i]);
            }

            m_bListening = true;
            m_radio.startListening();
        }

        const unsigned int vecOutputPipesSize = vecOutputPipes.size();
        if (vecOutputPipesSize)
        {
            for (unsigned int i = 0; i < vecOutputPipesSize; ++i)
            {
                m_radio.openWritingPipe(vecOutputPipes[i]);
            }
        }
    }

    return bSuccess;
}

bool Nrf24L01::send(const PipeId pipeId, const void* pData, const unsigned int dataSize)
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

Nrf24L01::PipeId Nrf24L01::receive(void* pData, const unsigned int dataSize) const
{
    PipeId pipeId = 0;
    if(m_radio.available(&pipeId))
    {
        m_radio.read(pData, dataSize);
    }

    return pipeId;
}
