#pragma once
#include "common.h"
#include <string>

using namespace std;

namespace common
{
    class character;
};

class load
{
public:
    load();

protected:
    string m_strInit;
    string m_strInitRole;
    string m_strInitPlace;
    string m_strInitWay;
    string m_strLoadAim;
};

class mainCharacter : public common::character
{
public:
    mainCharacter() = default;

    mainCharacter(string strPlace, string strFrom, string strRole, string strName, string strOther)
        : character(strFrom, strRole, strName, strOther)
        , m_strPlace(strPlace)
    {

    }

    // 序列化函数
    template<class Archive>
    void serialize(Archive& ar)
    {
        character::serialize(ar);
        ar(
            CEREAL_NVP(m_strPlace)
        );
    }
protected:
    virtual string detail()
    {
        stringstream buf;
        buf << "地点:" << m_strPlace << " 来自:" << m_strFrom << "角色:" << m_strRole << "名字:" << m_strName << "其他:" << m_strOther;
        return buf.str();
    }

protected:
    string m_strPlace;
};