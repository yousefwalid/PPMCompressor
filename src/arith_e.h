#include <cstdint>
#include <string>
#define NUM_BITS 32

class ArithmeticEncoder
{
private:
    uint32_t l, u;

    bool inLowerOrUpperHalves(int l, int u)
    {
        return ((l >> (NUM_BITS - 1)) & 1) == ((u >> (NUM_BITS - 1) & 1));
    }

    bool inMiddleHalf(int l, int u)
    {
        return (((l >> (NUM_BITS - 2)) & 1) != ((u >> (NUM_BITS - 2)) & 1));
    }

public:
    ArithmeticEncoder()
    {
        l = 0;
        u = -1;
    }

    std::string encodeSymbol(int count, int cumCount, int totalCount)
    {
        std::string streamBits = "";

        int diff = (u - l + 1);
        uint8_t new_l = l + (diff * cumCount) / totalCount;
        uint8_t new_u = l + (diff * (cumCount + count)) / totalCount;

        int middleHalfCounter = 0;

        while (inLowerOrUpperHalves(l, u) || inMiddleHalf(l, u))
        {
            if (inLowerOrUpperHalves(l, u))
            {
                uint8_t b = (l >> (NUM_BITS - 1)) & 1;
                streamBits += b + '0';
                l <<= 1;
                u <<= 1;
                u += 1;
                while (middleHalfCounter)
                {
                    streamBits += (b == 0) ? '1' : '0';
                    middleHalfCounter--;
                }
            }

            if (inMiddleHalf(l, u))
            {
                l <<= 1;
                u <<= 1;
                u += 1;
                l ^= (1 << (NUM_BITS - 1));
                u ^= (1 << (NUM_BITS - 1));
                middleHalfCounter++;
            }
        }

        return streamBits;
    }
};