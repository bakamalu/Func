#include "common.h"

namespace common
{
    int random(const int& nLimit1, const int& nLimit2)
    {
        int nMax = std::max(nLimit1, nLimit2);
        int nMin = std::min(nLimit1, nLimit2);

        // 随机数引擎，使用默认的随机数种子
        std::default_random_engine engine(std::random_device{}());

        // 分布，这里使用均匀分布
        std::uniform_int_distribution<int> distribution(nMin, nMax);
        // 生成随机数
        int nRandom = distribution(engine);
        return nRandom;
    }

    /// <summary>
    /// character
    /// </summary>

    void character::describe()
    {
        std::cout << detail() << endl;
    }
};
