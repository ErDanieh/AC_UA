
#include <stdio.h>
#include <math.h>
#include <chrono>
#include <iostream>

// El tamaño de MatB tiene que ser al menos 1 mayor que el real, no se porque
// Size es el real.

// Calcula el determiannate y la mtariz inversa de una matriz. Si no hay determianante posible devuelve 0

using namespace std;

/*
int inversaMatC()
{
    float MatB[6][6] =
    {
        {1, 1, 4, 24, 21},
        {1, 3, 1, 17, 52},
        {4, 1, 7, 31, 3},
        {12, 3, 1, 11, 1},
        {3, 12, 11, 6, 3}
    };

    int size = 5;

    float pivot, det = 1.0;
    int i, j, p;

    float compCero = 0.0;

    for (p = 0; p < size; p++)
    {
        pivot = MatB[p][p];
        det = det * pivot;

        if (fabs(pivot) < 1e-5)	return 0;

        for (i = 0; i <= size; i++) // i1
            MatB[i][p] = -MatB[i][p] / pivot;

        for (i = 0; i < size; i++) // i2
            if (i != p)
                for (j = 0; j < size; j++) // j1
                    if (j != p)
                        MatB[i][j] = MatB[i][j] + MatB[p][j] * MatB[i][p];

        for (j = 0; j < size; j++)
            MatB[p][j] = MatB[p][j] / pivot;
        MatB[p][p] = 1 / pivot;
    }

    // Para mostrarla por pantalla 
    
    for (int z = 0; z < size; z++)
    {
        for (int y = 0; y < size; y++)
        {
            printf("%f ", MatB[z][y]);

        }
        printf("\n");
    }

    return 1;

}
/*
void main()
{
    auto start = clock();

    for (unsigned i = 0; i < 10000000; i++)
        inversaMatC();

    auto end = clock();
    cout << (float)(end - start) / CLOCKS_PER_SEC << endl;
}*/