#pragma once
#include <string>
#include <iostream>
#include <random>
#include "serialize/serialize_base.h"

using namespace std;

namespace common
{
    enum E_RANDOM_TELL_TYPE
    {
        E_RANDOM_RUNNING_HOST = 1,
    };
    int random(const int& nLimit1, const int& nLimit2, int nType = 0);


    class place : public SerializableBase<place>, public tell
    {
    public:
        place() = default;

    protected:
        string m_strName;
        string m_strPos;
        string m_strOther;

    public:
        // ���л�����
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(
                CEREAL_NVP(m_strName),
                CEREAL_NVP(m_strPos),
                CEREAL_NVP(m_strOther)
            );
        }
    };

    class character : public SerializableBase<character> , public tell , public cProperty
    {
    protected:
        character() = default;
        character(string strFrom, string strRole, string strName, string strOther, bool bMale)
            : m_strFrom(strFrom)
            , m_strRole(strRole)
            , m_strName(strName)
            , m_strOther(strOther)
            , m_bMale(bMale)
        {

        }

        string personalPronoun()
        {
            return m_bMale ? "��" : "��";
        }

    protected:
        // ����ص�
        string m_strFrom;
        // ְҵ
        string m_strRole;
        // ����
        string m_strName;
        // ����
        string m_strOther;
        // �Ƿ�Ϊ����
        bool m_bMale;

    public:
        // ���л�����
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
    };
};