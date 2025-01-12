#pragma once
#include "common.h"
#include <string>

using namespace std;

namespace common
{
    class character;
};

class timeCharacter : public common::character
{
public:
    timeCharacter() = default;

    timeCharacter(string strPlace, string strFrom, string strRole, string strName, string strOther, bool bMale)
        : character(strFrom, strRole, strName, strOther, bMale)
        , m_strPlace(strPlace)
    {

    }
    void setTime(string strStartTime = "", string strEndTime = "")
    {
        if (!strStartTime.empty())
        {
            m_strStartTime = strStartTime;
        }
        if (!strEndTime.empty())
        {
            m_strEndTime = strEndTime;
        }
    }

    // ���л�����
    template<class Archive>
    void serialize(Archive& ar)
    {
        character::serialize(ar);
        ar(
            CEREAL_NVP(m_strPlace),
            CEREAL_NVP(m_strStartTime),
            CEREAL_NVP(m_strEndTime)
        );
    }

private:

protected:
    virtual string detail()
    {
        stringstream buf;
        buf << " ����" << m_strFrom << "��ְҵΪ" << m_strRole << "��" << m_strName << "," << personalPronoun() << "���ص���:" << m_strOther;
        buf << ", "<< personalPronoun();
        if (!m_strStartTime.empty())
        {
            buf << "��" << m_strStartTime;
        }
        buf <<"����" << m_strPlace <<"��";
        if (!m_strEndTime.empty())
        {
            buf << "��" << m_strEndTime << "�뿪��";
        }
        return buf.str();
    }

protected:
    // ��ǰ��ص�
    string m_strPlace;
    // ʱ���
    string m_strStartTime;
    // ʱ���
    string m_strEndTime;
};