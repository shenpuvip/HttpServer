#ifndef HS_SQUEUE_H
#define HS_SQUEUE_H

#include <mutex>
#include <condition_variable>

namespace SS
{
    template<typename T>
    class SQueue
    {
    public:
        SQueue(int maxSize = 1000)
        {
            maxSize = maxSize <= 0 ? 1000 : maxSize;

            m_maxSize = maxSize;
            m_array = new T[m_maxSize];
            m_size = 0;
            m_front = -1;
            m_back = -1;
        }

        void clear()
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            m_size = 0;
            m_front = -1;
            m_back = -1;
        }

        ~SQueue()
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            if(m_array != NULL)
                delete [] m_array;
        }

        bool full()
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            if(m_size >= m_maxSize)
                return true;
            return false;
        }

        bool empty()
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            if(m_size <= 0)
                return true;
            return false;
        }

        bool front(T &value)
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            if(m_size == 0) return false;
            value = m_array[m_front];
            return true;
        }

        bool back(T &value)
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            if(m_size == 0)
                return false;
            value = m_array[m_back];
            return true;
        }

        int size()
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            return m_size;
        }

        int maxSize()
        {
            return m_maxSize;
        }

        bool push(const T &item)
        {
            std::lock_guard <std::mutex> lock(m_mutex);
            if(m_size >= m_maxSize)
            {
                m_cond.notify_all();
                return false;
            }
            m_back = (m_back + 1) % m_maxSize;
            m_array[m_back] = item;
            ++m_size;
            m_cond.notify_all();
            return true;
        }

        bool pop(T &item)
        {
            std::unique_lock <std::mutex> lock{m_mutex};
            m_cond.wait(lock, [this]()
            {
                return m_size > 0;
            });
            m_front = (m_front + 1) % m_maxSize;
            item = m_array[m_front];
            --m_size;
            return true;
        }

        bool pop(T &item, int waitMs)
        {
            std::unique_lock <std::mutex> lock{m_mutex};
            if(m_size <= 0)
            {
                if(m_cond.wait_for(lock, std::chrono::microseconds(waitMs)) == std::cv_status::timeout)
                    return false;
            }
            if(m_size <= 0) return false;
            m_front = (m_front + 1) % m_maxSize;
            item = m_array[m_front];
            --m_size;
            return true;
        }

    private:
        T *m_array;
        int m_maxSize;
        int m_size;
        int m_front;
        int m_back;
        std::mutex m_mutex;
        std::condition_variable m_cond;
    };
}

#endif //HS_SQUEUE_H
