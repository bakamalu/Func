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

    // 序列化函数
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
        buf << " 来自" << m_strFrom << "的职业为" << m_strRole << "的" << m_strName << "," << personalPronoun() << "的特点是:" << m_strOther;
        buf << ", "<< personalPronoun();
        if (!m_strStartTime.empty())
        {
            buf << "在" << m_strStartTime;
        }
        buf <<"来到" << m_strPlace <<"。";
        if (!m_strEndTime.empty())
        {
            buf << "于" << m_strEndTime << "离开。";
        }
        return buf.str();
    }

protected:
    // 当前大地点
    string m_strPlace;
    // 时间段
    string m_strStartTime;
    // 时间段
    string m_strEndTime;
};