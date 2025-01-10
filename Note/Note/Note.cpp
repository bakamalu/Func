#include "Note.h"

int main()
{
    std::vector<mainCharacter> vecWrite = {
        mainCharacter{"上海", "北京", "交易员", "大名","无"},
        mainCharacter{"上海", "北京", "交易员", "小名","无2"}
    };

    std::vector<mainCharacter> vecRead;
    string strFileName = "main.bin";


    for (mainCharacter& param : vecWrite)
    {
        param.describe();
    }

    SerializableBase<mainCharacter>::multiBinSave(vecWrite, strFileName);
    SerializableBase<mainCharacter>::multiBinLoad(strFileName, vecRead);

    for (mainCharacter& param : vecRead)
    {
        param.describe();
    }

    cout << common::random(0, 100);
    return 0;
}
