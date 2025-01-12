#include "common.h"

namespace common
{
    int random(const int& nLimit1, const int& nLimit2, int nType /*= 0*/)
    {
        int nMax = std::max(nLimit1, nLimit2);
        int nMin = std::min(nLimit1, nLimit2);

        // ��������棬ʹ��Ĭ�ϵ����������
        std::default_random_engine engine(std::random_device{}());

        // �ֲ�������ʹ�þ��ȷֲ�
        std::uniform_int_distribution<int> distribution(nMin, nMax);
        // ���������
        int nRandom = distribution(engine);
        switch (nType)
        {
        case E_RANDOM_RUNNING_HOST:
            std::cout << "������͵͵���˸����ӣ�������Щ�����ǿ�������(�G��~), ��ô�õ��Ľ����:" << std::endl;
            std::cout << nRandom;
            break;
        default:
            break;
        }
        return nRandom;
    }
};
