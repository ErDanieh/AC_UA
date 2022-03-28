/*
#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

#define MAX 6
#define _BYTE 4 // Flaot

void inversaMatTest();
void inversaMat();
void mostrarMatriz2(float MatB[MAX][MAX]);


int main()
{
	int iter1 = 10000000;
	int iter2 = 1;

	auto start = clock();

	for (unsigned i = 0; i < iter1; i++)
		inversaMatTest();

	auto end = clock();
	cout <<  (float) (end - start) / CLOCKS_PER_SEC << endl;

	return 0;
}




void inversaMatTest()
{
	float MatB[MAX][MAX] =
	{
		{1, 1, 4, 24, 21},
		{1, 3, 1, 17, 52},
		{4, 1, 7, 31, 3},
		{12, 3, 1, 11, 1},
		{3, 12, 11, 6, 3}
	};

	int _size = 5;
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
			movss [MatB + 4 * eax], xmm6

			jmp _cp4

		_j2 :
			mov eax, ecx
			mul ebx // [*][]
			add eax, esi // [][*]

			movss xmm6, [MatB + 4 * eax]
			divss xmm6, pivot

			movss [MatB + 4 * eax], xmm6
			jmp _ij2

		_ij2 :
			inc esi
			cmp esi, _size
			jl _j2
			pop esi
			jmp _cp3

		_i2:
			cmp esi, ebx
			je incremento_i_2
			mov j, 0
			jmp loop_j_1

		continuar_i_2 :
			jmp incremento_i_2


		loop_j_1:
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
			movss [MatB + 4 * eax], xmm6

			jmp incremento_j_1

		incremento_j_1:
			inc j
			mov eax, _size

			cmp j, eax
			jl loop_j_1

			jmp continuar_i_2

		incremento_i_2:
			inc esi
			cmp esi, _size
			jl _i2

			pop esi
			jmp _cp2

		_i1:
			mov eax, ecx
			mul esi // [*][]
			add eax, ebx // [][*]

			movss xmm6, [MatB + 4 * eax]
			divss xmm6, pivot
			mulss xmm6, unoNegativo

			movss [MatB + 4 * eax], xmm6

			jmp incremento_i_1

		incremento_i_1:
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

			fin:
	}


	//mostrarMatriz2(MatB);

}

void mostrarMatriz2(float MatB[MAX][MAX])
{
	if (det == 0.0f)
	{

		cout << endl << "#############################################################" << endl;
		cout << "#                    _              ESA MATRIZ ES BASURA    #" << endl;
		cout << "#                  -=\\`\\                                    #" << endl;
		cout << "#              |\\ ____\\_\\__			  	    #" << endl;
		cout << "#            -=\\c``````````````)                            #" << endl;
		cout << "#               `~~~~~/ /~~`\                                #" << endl;
		cout << "#                 -==/ /                                    #" << endl;
		cout << "#                   '-'                                     #" << endl;
		cout << "#                  _  _            Determinante = 0         #" << endl;
		cout << "#                 ( `   )_                                  #" << endl;
		cout << "#                (    )    `)                               #" << endl;
		cout << "#              (_   (_ .  _) _)                             #" << endl;
		cout << "#                                             _             #" << endl;
		cout << "#   No existe inversa para esta matriz.      (  )           #" << endl;
		cout << "#             _ .                         ( `  ) . )        #" << endl;
		cout << "#           (  _ )_                      (_, _(  ,_)_)      #" << endl;
		cout << "#         (_  _(_ ,)                                        #" << endl;
		cout << "#############################################################" << endl;
	}



	for (int z = 0; z < MAX - 1; z++)
	{
		for (int y = 0; y < MAX - 1; y++)
		{
			printf("%f ", MatB[z][y]);

		}
		printf("\n");
	}

}
*/