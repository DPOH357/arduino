#ifndef DPOH_H
#define DPOH_H

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

    T* get()
    {
        return m_ptr;
    }

    const T* get() const
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

template <class T>
class Vector
{
public:
    Vector()
        : m_arrSize(0)
    {
        m_dataSize = 4;
        m_ptr = new T[m_dataSize];
    }

    ~Vector()
    {
        delete [] m_ptr;
    }

    void reserve(const uint32_t size)
    {
        if (size > m_dataSize)
        {
            T* ptr = new T[size];
            for (uint32_t i = 0; i < m_arrSize; ++i)
            {
                ptr[i] = m_ptr[i];
            }
            delete [] m_ptr;
            m_ptr = ptr;
        }
    }

    bool empty() const
    {
        return (m_arrSize == 0);
    }

    uint32_t size() const
    {
        return m_arrSize;
    }

    const T& operator[](const uin32_t index) const
    {
        return m_ptr[index];
    }

    T& operator[](const uin32_t index)
    {
        return m_ptr[index];
    }

    void clear()
    {
        m_arrSize = 0;
    }

    void push_back(const T& data)
    {
        m_ptr[m_arrSize] = data;
        ++m_arrSize;
    }

    const T& back() const
    {
        return m_ptr[m_arrSize];
    }

    T& back()
    {
        return m_ptr[m_arrSize];
    }

    void pop_back()
    {
        if(m_arrSize > 0)
        {
            --m_arrSize;
        }
    }

private:
    T* m_ptr;
    uint32_t m_dataSize;
    uint32_t m_arrSize;
};

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

#endif // DPOH_H
