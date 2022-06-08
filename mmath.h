//
// Created by Andrei Gheorghiu on 5/24/2022.
//

#ifndef DATABASE_UNIVERSITA2022_MMATH_H
#define DATABASE_UNIVERSITA2022_MMATH_H

#include <random>

namespace mmath{
    double floor(double num) {
        auto n = (long long)num;
        auto d = (double)n;
        if (d == num || num >= 0)
            return d;
        else
            return d - 1;
    }

    int ceil(float num) {
        int inum = (int)num;
        if (num == (float)inum) {
            return inum;
        }
        return inum + 1;
    }

    int abs(int v)
    {
        return v * ((v>0) - (v<0));
    }
}
#endif //DATABASE_UNIVERSITA2022_MMATH_H
