#ifndef SINGLETON_H
#define SINGLETON_H

#include <QAtomicPointer>
#include <QObject>
#include <mutex>

template <typename T>
class Singleton
{
public:
    static T& instance()
    {
        // 第一重检查
        while(m_instance.testAndSetOrdered(nullptr, nullptr))
        {
            std::call_once(m_once_flag, [](){
                // 第二重检查，如果new抛出异常，不会反转once_flag,将尝试重新调用lambda
                m_instance.testAndSetOrdered(nullptr, new T);
            });
        }

        return *m_instance;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    static QAtomicPointer<T> m_instance;
    static std::once_flag m_once_flag;
};

template <class T>
QAtomicPointer<T> Singleton<T>::m_instance = nullptr;

template <class T>
std::once_flag Singleton<T>::m_once_flag;


#endif // SINGLETON_H
