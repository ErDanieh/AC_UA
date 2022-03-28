#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

#define MAX 7
#define _BYTE 4 // Flaot

float inversaMatSSE()
{
	float MatB[MAX][MAX] =
	{
		{2, 3, 3, 5, 9, 11 },
		{1, 6, 1, 7, 5, 33},
		{5, 9, 7, 6, 4, 5},
		{7, 9, 8, 0, 7, 8},
		{4, 8, 4, 4, 9, 9},
		{56, 677, 5, 4, 555, 6}
	};


	int _size = MAX-1;
	float pivot = 0;
	float  det = 1.0f;

	int j = 0;

	float compCero = 0.0;

	float unoNegativo = -1.0f;
	float unoPositivo = 1.0f;
	float _precision = 0.001f;

	__asm
	{
		mov esi, 0 // esi = p
		mov edi, _size // size
		movss xmm4, unoPositivo // xmm4 = det -> Return determiannte
		mov ecx, MAX

		jmp _p

		_p:								// Bucle principal
			mov eax, ecx
			mul esi
			add eax, esi

			VZEROALL

			// Pivot = MatB[p][p];
			movss xmm0, [MatB + 4 * eax] 
			movss pivot, xmm0
			mulss xmm4, pivot

			// compCero = |pivot|;
			fld pivot
			fabs
			fstp compCero

			// if (pivot < 1e-5) -> FIN:
			mov eax, _precision
			cmp compCero, eax
			jl fin

			mov ebx, esi	// ebx = p
			push esi		// esi = i 
			mov esi, 0

			jmp _i1

		_cp1 :
			mov ebx, esi		// ebx = p
			push esi
			mov esi, 0			// esi = i
			jmp _i2

		_cp2 :
			mov ebx, esi // ebx == p
			push esi     // esi = j
			mov esi, 0
			jmp _j2

		_cp3 :
			jmp _lo

		_cp4 :
			jmp _ip

		_lo :
			// MatB[p][p] = 1 / pivot;
			mov eax, ecx
			mul esi
			add eax, esi

			movss xmm6, unoPositivo
			divss xmm6, pivot
			movss[MatB + 4 * eax], xmm6

			jmp _cp4

		_j2 :
			// for (j = 0; j < size; j++)
			//	MatB[p][j] = MatB[p][j] / pivot;
			mov eax, ecx
			mul ebx // [*][]
			add eax, esi // [][*]

			movss xmm6, [MatB + 4 * eax]
			divss xmm6, pivot

			movss[MatB + 4 * eax], xmm6
			jmp _ij2

		_ij2 :
			inc esi
			cmp esi, _size
			jl _j2
			pop esi
			jmp _cp3

		_i2 :
			// Bucle i2
			// for (i = 0; i < size; i++) // i2
			//    if (i != p)
			cmp esi, ebx
			je incremento_i_2
			mov j, 0
			jmp loop_j_1

		continuar_i_2 :
			jmp incremento_i_2


		loop_j_1 :
			// for (j = 0; j < size; j++) // j1
			//	if (j != p)
			//		MatB[i][j] = MatB[i][j] + MatB[p][j] * MatB[i][p];
			cmp j, ebx
			je incremento_j_1

			mov eax, ecx
			mul ebx // [*][]
			add eax, j // [][*]

			movss xmm6, [MatB + 4 * eax]

			mov eax, ecx
			mul esi // [*][]
			add eax, ebx // [][*]

			movss xmm3, [MatB + 4 * eax]
			mulss xmm3, xmm6

			mov eax, ecx
			mul esi // [*][]
			add eax, j // [][*]

			movss xmm6, [MatB + 4 * eax]
			addss xmm6, xmm3
			movss[MatB + 4 * eax], xmm6

			jmp incremento_j_1

		incremento_j_1 :
			inc j
			mov eax, _size

			cmp j, eax
			jl loop_j_1

			jmp continuar_i_2

		incremento_i_2 :
			inc esi
			cmp esi, _size
			jl _i2

			pop esi
			jmp _cp2

		_i1 :
			// MatB[i][p] = -MatB[i][p] / pivot;
			mov eax, ecx
			mul esi // [*][]
			add eax, ebx // [][*]

			movss xmm6, [MatB + 4 * eax]
			divss xmm6, pivot
			mulss xmm6, unoNegativo

			movss[MatB + 4 * eax], xmm6

			jmp incremento_i_1

		incremento_i_1 :
			// i++
			inc esi
			cmp esi, edi
			jle _i1

			pop esi
			jmp _cp1

		_ip :
			inc esi
			cmp esi, edi
			jl _p
			jmp fin

		fin :
			movss det, xmm4
	}

	return det;
}
