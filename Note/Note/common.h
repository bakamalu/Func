#pragma once
#include <string>
#include <iostream>
#include <random>
#include "serialize/serialize_base.h"

using namespace std;

namespace common
{
    int random(const int& nLimit1, const int& nLimit2);

    class character : SerializableBase<character>
    {
    protected:
        character() = default;
        character(string strFrom, string strRole, string strName, string strOther)
            : m_strFrom(strFrom)
            , m_strRole(strRole)
            , m_strName(strName)
            , m_strOther(strOther)
        {

        }

    protected:
        string m_strFrom;
        string m_strRole;
        string m_strName;
        string m_strOther;

    public:
        // ÐòÁÐ»¯º¯Êý
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(
                CEREAL_NVP(m_strFrom),
                CEREAL_NVP(m_strRole),
                CEREAL_NVP(m_strName),
                CEREAL_NVP(m_strOther)
            );
        }

    public:
        virtual void describe();

    protected:
        virtual string detail() = 0;
    };
};