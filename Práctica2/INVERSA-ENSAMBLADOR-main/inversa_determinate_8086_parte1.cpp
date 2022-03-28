#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

#define MAX 6
#define _BYTE 4 // Flaot

float inversaMat()
{
	float MatB[MAX][MAX] =
	{
		{1, 1, 4, 24, 21},
		{1, 3, 1, 17, 52},
		{4, 1, 7, 31, 3},
		{12, 3, 1, 11, 1},
		{3, 12, 11, 6, 3}
	};

	int _size = MAX-1;
	float pivot = 0;
	float  det = 1.0f;

	int j = 0;

	float compCero = 0.0;

	float aux1 = 0.0f;
	float aux2 = 0.0f;

	float unoNegativo = -1.0f;
	float unoPositivo = 1.0f;
	float _precision = 0.001f;

	__asm
	{
		mov esi, 0			// esi = p
		mov edi, _size		// size

		jmp _p

		_p :				// Parte principal
			mov eax, MAX	// 
			mul esi
			add eax, esi

			// Pivot = MatB[p][p];
			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0

			movss pivot, xmm0

			// det = det * pivot
			fld pivot
			fmul det
			fstp det

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
			mov eax, MAX
			mul esi
			add eax, esi

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0

			fld unoPositivo
			fdiv pivot
			fstp [MatB + 4 * eax]

			jmp _cp4

		_j2 :
			// for (j = 0; j < size; j++)
			//	MatB[p][j] = MatB[p][j] / pivot;
			mov eax, MAX
			mul ebx // [*][]
			add eax, esi // [][*]

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0

			fld aux1
			fdiv pivot
			fstp [MatB + 4 * eax]

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

			mov eax, MAX
			mul ebx // [*][]
			add eax, j // [][*]

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0 // aux1 = MatB[p][j]

			mov eax, MAX
			mul esi // [*][]
			add eax, ebx // [][*]

			movss xmm0, [MatB + 4 * eax]
			movss aux2, xmm0 // aux2 = MatB[i][p]

			fld aux1 // st(0)
			fmul aux2 // st(1)
			fstp aux2 //

			mov eax, MAX
			mul esi // [*][]
			add eax, j // [][*]

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0 // aux2 = MatB[i][p]

			fld aux2
			fadd aux1
			fstp [MatB + 4 * eax]

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
			mov eax, MAX
			mul esi // [*][]
			add eax, ebx // [][*]

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0

			//  -MatB[i][p] / pivot
			fld aux1
			fdiv pivot
			fstp aux1

			fld aux1
			fmul unoNegativo
			fstp [MatB + 4 * eax]

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
	}

	return det;
	//mostrarMatriz(MatB);
	
}


