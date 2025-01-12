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

// ��ģ��Ľӿ���
class ISerializable {
public:
    virtual ~ISerializable() = default;

    // Cereal ����Ԫ��������
    friend class cereal::access;
//     // ��ȡ�������ķ���
//     virtual std::string getTypeName() const = 0;
};

//class SerializableFactory;
template <typename Derived>
class SerializableBase : public ISerializable {
protected:
    /// <summary>
    /// ������Ҫʵ��serialize, Ȼ������multiBinSave��multiBinLoad��ʵ�ֱ��ػ��洢
    /// </summary>
    SerializableBase() = default;

public:
    static void multiBinSave(const std::vector<Derived>& vecSrcDerived, std::string strDstFileName)
    {
        std::ofstream ofs(strDstFileName, std::ios::binary);
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open file for writting.");
        }
        cereal::BinaryOutputArchive oarchive(ofs); // ��������������鵵
        for (const Derived& param : vecSrcDerived)
        {
            oarchive(param); // ���л��Զ���ṹ
        }
    }

    static void multiBinLoad(std::string strSrcFileName, std::vector<Derived>& vecDstDerived)
    {
        std::ifstream ifs(strSrcFileName, std::ios::binary);
        if (!ifs.is_open()) {
            throw std::runtime_error("Failed to open file for reading.");
        }
        cereal::BinaryInputArchive iarchive(ifs); // ��������������鵵
        while (!ifs.eof())
        {
            try
            {
                Derived param;
                iarchive(param); // �����л��Զ���ṹ
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

    // ���л����ļ�
//     void save(const std::string& filename)
//     {
//         std::ofstream ofs(filename, std::ios::binary);
//         cereal::BinaryOutputArchive oarchive(ofs); // ��������������鵵
//         oarchive(static_cast<const Derived*>(this)->getTypeName());
// 
//         // ֱ�ӵ�������������л�����
//         static_cast<Derived*>(this)->serialize(oarchive);
//     }

    // ���ļ������л�
//     static std::shared_ptr<Derived> load(const std::string& filename)
//     {
//         std::ifstream ifs(filename, std::ios::binary);
//         if (!ifs.is_open()) {
//             throw std::runtime_error("Unable to open file for reading.");
//         }
//         cereal::BinaryInputArchive iarchive(ifs); // ��������������鵵
//         std::string typeName;
//         iarchive(typeName);
// 
//         // ��̬��������ʵ��
//         auto obj = std::dynamic_pointer_cast<Derived>(SerializableFactory::createInstance(typeName));
//         if (!obj) {
//             throw std::runtime_error("Failed to cast object to correct type.");
//         }
//         obj->serialize(iarchive); // �����л���������ʵ��
//         return obj;
//     }

    template<class Archive>
    void serialize(Archive& ar) {
        // �����޸ú���ʵ��
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
    // ����
    int m_nPower;
    // �ٶ�
    int m_nSpeed;
    // ����ֵ(������)
    int m_nMP;
    // ����(������)
    int m_nHp;
    // ����/����
    string m_strSkill;
};

// �����࣬���ڸ�����������������ʵ��
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
//     // һ������ֻ��һ��
//     static std::shared_ptr<ISerializable> createInstance(const std::string& typeName)
//     {
//         auto it = creators().find(typeName);
//         if (it == creators().end()) {
//             throw std::runtime_error("Unknown type: " + typeName);
//         }
//         return it->second();
//     }
// };