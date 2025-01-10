#include "common.h"

namespace common
{
    int random(const int& nLimit1, const int& nLimit2)
    {
        int nMax = std::max(nLimit1, nLimit2);
        int nMin = std::min(nLimit1, nLimit2);

        // ��������棬ʹ��Ĭ�ϵ����������
        std::default_random_engine engine(std::random_device{}());

        // �ֲ�������ʹ�þ��ȷֲ�
        std::uniform_int_distribution<int> distribution(nMin, nMax);
        // ���������
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
