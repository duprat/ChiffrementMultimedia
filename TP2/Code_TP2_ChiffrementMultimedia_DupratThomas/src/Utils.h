#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include <vector>
#include <unistd.h>
#include <set>
#include <fstream>
#include <experimental/random>
#include <bits/stdc++.h> 
#include <math.h>
#include "image_ppm.h"
#include "Eigen/Dense"

class Utils
{
public:
    static bool isPrime(int n)
    {
        int i;
        bool isPrime = true;

        for (i = 2; i <= n / 2; ++i)
        {
            if (n % i == 0)
            {
                isPrime = false;
                break;
            }
        }

        return isPrime;
    };

    static bool arePrime(int a, int b)
    {
        return euclide(a, b) == 1;
    };

    static int euclide(int a, int b)
    {
        if (b == 0)
        {
            return a;
        }
        return euclide(b, a % b);
    };

    static std::vector<int> exposants_de_chiffrement(int phi)
    {
        std::vector<int> exposants;

        for (int i = 3; i < phi; i++)
        {
            if (arePrime(phi, i))
            {
                exposants.push_back(i);
            }
        }

        return exposants;
    };

    static int exposant_de_dechiffrement(int little, int big)
    {
        if (!arePrime(little, big))
        {
            std::cerr << "We need Prime Numbers\n";
            exit(1);
        }

        int r = little;
        int r_prim = big;
        int u = 1;
        int v = 0;
        int u_prim = 0;
        int v_prim = 1;
        int q, r_temp, u_temp, v_temp;

        while (r_prim != 0)
        {
            q = r / r_prim;

            r_temp = r;
            u_temp = u;
            v_temp = v;

            r = r_prim;
            u = u_prim;
            v = v_prim;

            r_prim = r_temp - (q * r_prim);
            u_prim = u_temp - (q * u_prim);
            v_prim = v_temp - (q * v_prim);
        }
        return u;
    };

    /* Iterative Function to calculate (x^y)%p in O(log y) */
    static int exponentiation_modulaire(int number, int exponent, int modulo)
    {
        int res = 1;
        number = number % modulo;

        if (number == 0)
            return 0;

        while (exponent > 0)
        {
            if (exponent & 1)
                res = (res * number) % modulo;

            exponent = exponent >> 1;
            number = (number * number) % modulo;
        }
        return res;
    }
};

#endif