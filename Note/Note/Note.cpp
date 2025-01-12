#include "Note.h"

int main()
{
    std::vector<timeCharacter> vecWrite = {
        timeCharacter{"异世界", "地球", "勇者", "橘子","很矮", false},
        timeCharacter{"上海", "北京", "魔王", "橙子","很高", true}
    };
    vecWrite[0].setTime("2025年1月1日", "2025年12月31日");

    std::vector<timeCharacter> vecRead;
    string strFileName = "main.bin";


//     for (timeCharacter& param : vecWrite)
//     {
//         param.describe();
//     }

    SerializableBase<timeCharacter>::multiBinSave(vecWrite, strFileName);
    SerializableBase<timeCharacter>::multiBinLoad(strFileName, vecRead);

    for (timeCharacter& param : vecRead)
    {
        param.describe();
    }

    int nRand = common::random(0, 100, common::E_RANDOM_RUNNING_HOST);
    return 0;
}
