#include "serialize_base.h"
#include <iostream>

// ���軹��һ�� Address ��
class Address : public SerializableBase<Address> 
{
public:
    std::string street;
    std::string city;

//     std::string getTypeName() const override {
//         return "Address";
//     }

public:
    Address(std::string s = "", std::string c = "") : street(std::move(s)), city(std::move(c)) {}

    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(street), CEREAL_NVP(city));
    }
};

// ������һ�� Person ��
class Person : public SerializableBase<Person> 
{
public:
    Person(std::string n = "", int a = 0) : name(std::move(n)), age(a) {}

//     // ��ȡ�������ķ���
//     std::string getTypeName() const override {
//         return "Person";
//     }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(name), CEREAL_NVP(age));
    }
public:
    std::string name;
    int age;
};

// ע��������֪������
// void registerTypes() {
//     SerializableFactory::registerType<Person>("Person");
//     SerializableFactory::registerType<Address>("Address");
// }

int test_main() 
{
    //registerTypes();

//     // ���������� Person ����
//     Person person{ "Alice", 30 };
//     person.save("person.bin");
// 
//     // ���� Person ����
//     auto loaded_person = Person::load("person.bin");
//     std::cout << "Loaded Person: " << loaded_person->name << ", " << loaded_person->age << std::endl;
// 
//     // ���������� Address ����
//     Address address{ "123 Main St", "Wonderland" };
//     address.save("address.bin");
// 
//     // ���� Address ����
//     auto loaded_address = Address::load("address.bin");
//     std::cout << "Loaded Address: " << loaded_address->street << ", " << loaded_address->city << std::endl;

    std::vector<Person> vecPerson = { 
        Person {"sss2",50},
        Person{"sss3",60} 
    };

    std::string strMultiFile = "test_multi.bin"; 
    Person::multiBinSave(vecPerson, strMultiFile);


    std::vector<Person> vecPerson2;
    Person::multiBinLoad(strMultiFile, vecPerson2);

    for (const Person& param : vecPerson2)
    {
        std::cout << "Loaded Person: " << param.name << ", " << param.age << std::endl;
    }

    return 0;
}