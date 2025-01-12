#include "common.h"

namespace common
{
    int random(const int& nLimit1, const int& nLimit2, int nType /*= 0*/)
    {
        int nMax = std::max(nLimit1, nLimit2);
        int nMin = std::min(nLimit1, nLimit2);

        // 随机数引擎，使用默认的随机数种子
        std::default_random_engine engine(std::random_device{}());

        // 分布，这里使用均匀分布
        std::uniform_int_distribution<int> distribution(nMin, nMax);
        // 生成随机数
        int nRandom = distribution(engine);
        switch (nType)
        {
        case E_RANDOM_RUNNING_HOST:
            std::cout << "主持人偷偷丢了个骰子，你们这些笨蛋是看不见的(欸嘿~), 那么得到的结果是:" << std::endl;
            std::cout << nRandom;
            break;
        default:
            break;
        }
        return nRandom;
    }
};
