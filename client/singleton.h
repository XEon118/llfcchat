#ifndef SINGLETON_H
#define SINGLETON_H
#include <global.h>
template <typename T>
class Singleton{
protected:
    Singleton() = default;  //子类继承时能构造，protected
    Singleton(const Singleton<T>&) = delete; //不允许拷贝构造
    Singleton& operator = (const Singleton<T>& st) = delete;    //不允许拷贝赋值
    static std::shared_ptr<T> _instance;    //静态
public:
    static std::shared_ptr<T> GetInstance()
    {
        static std::once_flag s_flag; //只初始化一次（第一次调用）
        std::call_once(s_flag,[&](){
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddress(){
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton(){
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;   //类外初始化（实例化）

#endif // SINGLETON_H
