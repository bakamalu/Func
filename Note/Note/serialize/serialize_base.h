#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <fstream>
#include <typeinfo>

// 非模板的接口类
class ISerializable {
public:
    virtual ~ISerializable() = default;

    // Cereal 的友元函数声明
    friend class cereal::access;
//     // 获取类型名的方法
//     virtual std::string getTypeName() const = 0;
};

//class SerializableFactory;
template <typename Derived>
class SerializableBase : public ISerializable {
protected:
    /// <summary>
    /// 子类需要实现serialize, 然后便可用multiBinSave和multiBinLoad来实现本地化存储
    /// </summary>
    SerializableBase() = default;

public:
    static void multiBinSave(const std::vector<Derived>& vecSrcDerived, std::string strDstFileName)
    {
        std::ofstream ofs(strDstFileName, std::ios::binary);
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open file for writting.");
        }
        cereal::BinaryOutputArchive oarchive(ofs); // 创建二进制输出归档
        for (const Derived& param : vecSrcDerived)
        {
            oarchive(param); // 序列化自定义结构
        }
    }

    static void multiBinLoad(std::string strSrcFileName, std::vector<Derived>& vecDstDerived)
    {
        std::ifstream ifs(strSrcFileName, std::ios::binary);
        if (!ifs.is_open()) {
            throw std::runtime_error("Failed to open file for reading.");
        }
        cereal::BinaryInputArchive iarchive(ifs); // 创建二进制输入归档
        while (!ifs.eof())
        {
            try
            {
                Derived param;
                iarchive(param); // 反序列化自定义结构
                vecDstDerived.emplace_back(param);
            }
            catch (const std::exception& e) {
                //std::cerr << "General error occurred: " << e.what() << std::endl;
                break;
            }
            catch (...) {
                std::cerr << "Unknown error occurred." << std::endl;
                break;
            }
        }
    }

    // 序列化到文件
//     void save(const std::string& filename)
//     {
//         std::ofstream ofs(filename, std::ios::binary);
//         cereal::BinaryOutputArchive oarchive(ofs); // 创建二进制输出归档
//         oarchive(static_cast<const Derived*>(this)->getTypeName());
// 
//         // 直接调用派生类的序列化方法
//         static_cast<Derived*>(this)->serialize(oarchive);
//     }

    // 从文件反序列化
//     static std::shared_ptr<Derived> load(const std::string& filename)
//     {
//         std::ifstream ifs(filename, std::ios::binary);
//         if (!ifs.is_open()) {
//             throw std::runtime_error("Unable to open file for reading.");
//         }
//         cereal::BinaryInputArchive iarchive(ifs); // 创建二进制输入归档
//         std::string typeName;
//         iarchive(typeName);
// 
//         // 动态创建子类实例
//         auto obj = std::dynamic_pointer_cast<Derived>(SerializableFactory::createInstance(typeName));
//         if (!obj) {
//             throw std::runtime_error("Failed to cast object to correct type.");
//         }
//         obj->serialize(iarchive); // 反序列化到派生类实例
//         return obj;
//     }

    template<class Archive>
    void serialize(Archive& ar) {
        // 子类无该函数实现
        throw std::runtime_error("Missing subclass implementation");
    }
};

class tell
{
protected:
    tell() = default;

    virtual std::string detail() = 0;

public:
    virtual void describe()
    {
        std::cout << detail() << std::endl;
    }
};

class cProperty
{
protected:
    cProperty() = default;

public:
    void setProperty()
    {

    }

protected:
    // 力量
    int m_nPower;
    // 速度
    int m_nSpeed;
    // 法力值(精神力)
    int m_nMP;
    // 体力(防御力)
    int m_nHp;
    // 技能/特性
    string m_strSkill;
};

// 工厂类，用于根据类型名创建子类实例
// class SerializableFactory 
// {
// private:
//     using Creator = std::function<std::shared_ptr<ISerializable>()>;
//     static std::map<std::string, Creator>& creators() {
//         static std::map<std::string, Creator> instance;
//         return instance;
//     }
// 
// public:
//     template<typename T>
//     static void registerType(const std::string& typeName)
//     {
//         if (creators().find(typeName) != creators().end()) {
//             throw std::runtime_error("Type already registered: " + typeName);
//         }
//         creators()[typeName] = []() -> std::shared_ptr<ISerializable> {
//             return std::make_shared<T>();
//         };
//     }
// 
//     // 一个对象只存一次
//     static std::shared_ptr<ISerializable> createInstance(const std::string& typeName)
//     {
//         auto it = creators().find(typeName);
//         if (it == creators().end()) {
//             throw std::runtime_error("Unknown type: " + typeName);
//         }
//         return it->second();
//     }
// };