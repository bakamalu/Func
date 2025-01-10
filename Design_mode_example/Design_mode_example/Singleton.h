#pragma once

#include <iostream>
#include <mutex>

// Singleton class declaration
class Singleton {
public:
    static Singleton& getInstance() {
        std::call_once(initInstanceFlag, &Singleton::initSingleton);
        return *instance;
    }

    Singleton(const Singleton&) = delete; // Prevent copy construction
    Singleton& operator=(const Singleton&) = delete; // Prevent assignment

    // Example method
    void printMessage() const {
        std::cout << "Hello from the Singleton instance." << std::endl;
    }

private:
    Singleton() {} // Private constructor to prevent instantiation outside of class

    static void initSingleton() {
        instance = new Singleton();
    }

    static std::once_flag initInstanceFlag;
    static Singleton* instance;
};

std::once_flag Singleton::initInstanceFlag;
Singleton* Singleton::instance = nullptr;

//2
template<class T>
class SingletonT
{
public:
    static T* instance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new T();
        }
        return s_pInstance;
    }

    static void release()
    {
        if (NULL != s_pInstance)
        {
            delete s_pInstance;
            s_pInstance = NULL;
        }
    }

protected:
    static T* s_pInstance;
};

class SingletonT_test : public SingletonT<SingletonT_test>
{
public:
    SingletonT_test(){}

    void print()
    {
        std::cout << "SingletonT_test id:" << nId;
    }
private:
    int nId = 0;
};


// 使用法2时，必须先用该宏声明, 不然编译报错
#define IMPL_SINGLETON_CLASS(subClass)  \
    template<> subClass* SingletonT<subClass>::s_pInstance = NULL; \
    template class SingletonT<subClass>