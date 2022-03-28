#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

#define MAX 11
#define _BYTE 4 // Flaot


float inversaMatBench()
{
	float MatB[MAX][MAX] =
	{ {1, 3, 7, 8, 7, 0, 3, 0, 1, 8},
		{0, 9, 4, 7, 3, 7, 6, 3, 9, 4},
		{2, 8, 8, 8, 6, 9, 6, 0, 9, 9},
		{3, 8, 9, 1, 6, 2, 3, 3, 7, 4},
		{8, 8, 2, 8, 4, 9, 9, 4, 9, 2},
		{5, 2, 3, 7, 1, 5, 7, 3, 2, 7},
		{0, 8, 0, 7, 1, 2, 3, 6, 0, 6},
		{9, 4, 2, 3, 9, 0, 3, 4, 6, 3},
		{3, 7, 5, 6, 2, 4, 7, 9, 8, 9},
		{1, 4, 9, 4, 0, 4, 3, 1, 6, 7}
	};

	int _size = MAX - 1;
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
		mov esi, 0 // esi = p
		mov edi, _size // size

		jmp _p

		_p :
		mov eax, MAX
			mul esi
			add eax, esi

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

			_cp1 :		// WARNING
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
		// pivot = MatB[p][p]; p = esi
		mov eax, MAX
			mul esi
			add eax, esi

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0

			fld unoPositivo
			fdiv pivot
			fstp[MatB + 4 * eax]

			jmp _cp4

			_j2 :
		mov eax, MAX
			mul ebx // [*][]
			add eax, esi // [][*]

			movss xmm0, [MatB + 4 * eax]
			movss aux1, xmm0

			fld aux1
			fdiv pivot
			fstp[MatB + 4 * eax]

			jmp _ij2

			_ij2 :
		inc esi

			cmp esi, _size
			jl _j2

			pop esi
			jmp _cp3

			_i2 :
		cmp esi, ebx
			je incremento_i_2

			mov j, 0
			jmp loop_j_1

			continuar_i_2 :
		jmp incremento_i_2


			loop_j_1 :
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
			fstp[MatB + 4 * eax]

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
			fstp[MatB + 4 * eax]

			jmp incremento_i_1

			incremento_i_1 :
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

float inversaMatBenchSSEBench()
{
	float MatB[MAX][MAX] =
	{ {1, 3, 7, 8, 7, 0, 3, 0, 1, 8},
		{0, 9, 4, 7, 3, 7, 6, 3, 9, 4},
		{2, 8, 8, 8, 6, 9, 6, 0, 9, 9},
		{3, 8, 9, 1, 6, 2, 3, 3, 7, 4},
		{8, 8, 2, 8, 4, 9, 9, 4, 9, 2},
		{5, 2, 3, 7, 1, 5, 7, 3, 2, 7},
		{0, 8, 0, 7, 1, 2, 3, 6, 0, 6},
		{9, 4, 2, 3, 9, 0, 3, 4, 6, 3},
		{3, 7, 5, 6, 2, 4, 7, 9, 8, 9},
		{1, 4, 9, 4, 0, 4, 3, 1, 6, 7}
	};


	int _size = MAX - 1;
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
		//movss xmm7, pivot // 1
		mov ecx, MAX

		jmp _p

		_p :

		mov eax, ecx
			mul esi
			add eax, esi

			VZEROALL

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

			_cp1 :		// WARNING
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
		mov eax, ecx
			mul esi
			add eax, esi


			movss xmm6, unoPositivo
			divss xmm6, pivot
			movss[MatB + 4 * eax], xmm6

			jmp _cp4

			_j2 :
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
		cmp esi, ebx
			je incremento_i_2
			mov j, 0
			jmp loop_j_1

			continuar_i_2 :
		jmp incremento_i_2


			loop_j_1 :
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
		mov eax, ecx
			mul esi // [*][]
			add eax, ebx // [][*]

			movss xmm6, [MatB + 4 * eax]
			divss xmm6, pivot
			mulss xmm6, unoNegativo

			movss[MatB + 4 * eax], xmm6

			jmp incremento_i_1

			incremento_i_1 :
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


	//mostrarMatriz2(MatB);

	return det;
}

int inversaMatCBench()
{
	float MatB[MAX][MAX] =
	{ {1, 3, 7, 8, 7, 0, 3, 0, 1, 8},
	{0, 9, 4, 7, 3, 7, 6, 3, 9, 4},
	{2, 8, 8, 8, 6, 9, 6, 0, 9, 9},
	{3, 8, 9, 1, 6, 2, 3, 3, 7, 4},
	{8, 8, 2, 8, 4, 9, 9, 4, 9, 2},
	{5, 2, 3, 7, 1, 5, 7, 3, 2, 7},
	{0, 8, 0, 7, 1, 2, 3, 6, 0, 6},
	{9, 4, 2, 3, 9, 0, 3, 4, 6, 3},
	{3, 7, 5, 6, 2, 4, 7, 9, 8, 9},
	{1, 4, 9, 4, 0, 4, 3, 1, 6, 7}
	};

	int size = MAX-1;

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

	return 1;
}

void mostrarMatriz(float MatB[MAX][MAX])
{
	for (int z = 0; z < MAX - 1; z++)
	{
		for (int y = 0; y < MAX - 1; y++)
		{
			printf("%f ", MatB[z][y]);
		}
		printf("\n");
	}
}

int main()
{
	int iter1 = 10000000;
	int iter2 = 1;


	auto start = clock();

	//cout << "Tiempo en C\tTiempo en x86\tTiempo en SSE" << endl;

	for (unsigned i = 0; i < iter1; i++)
	{
		inversaMatCBench();
	}

	auto end = clock();
	cout << (float)(end - start) / CLOCKS_PER_SEC << " \t\t";


	start = clock();

	for (unsigned i = 0; i < iter1; i++)
	{
		inversaMatBench();
	}

	end = clock();
	cout  << (float)(end - start) / CLOCKS_PER_SEC << " \t\t";


	start = clock();

	for (unsigned i = 0; i < iter1; i++)
	{
		inversaMatBenchSSEBench();
	}

	end = clock();
	cout << (float)(end - start) / CLOCKS_PER_SEC << endl;


	return 0;
}

