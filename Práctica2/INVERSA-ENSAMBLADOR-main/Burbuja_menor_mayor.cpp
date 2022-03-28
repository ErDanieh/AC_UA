
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int burbujaMenor() {

	// Declaración e inicialización del vector 
	int vector[20] = { 27, 2, 45, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 };

	__asm 
	{
		// Cargamos en un registro, posición inicial del bucle
		mov esi, 0
		// Cargamos en otro registro, posición final del bucle
		mov edi, 19

		bucle:
			mov edx, [vector + 4 * esi]
			add esi, 1
			mov eax, [vector + 4 * esi]
			sub esi, 1
			// edx = 2 , eax = 30

			cmp eax, edx // 
			jl intercambio // if (edx < eax)

			jmp incremento

		intercambio: // Intercambiamos los datos de los elementos del vector
			
			mov [vector + 4 * esi], eax // El valor en la pos 0 -> eax = valor en la pos 1
			add esi, 1
			mov[vector + 4 * esi], edx // El valor en la pos 1 -> eax = valor en la pos 0
			sub esi, 1

			jmp incremento

		incremento: // Incrementamos esi, para recorrer el vector
			add esi, 1 // Sumamos uno a esi

			cmp edi, esi // Si edi > esi, ya hemos recorrido todo el bucle

			jle decremento // Decrementarmos el tam de elementos a ordenar
			jmp bucle // Sino volvemos al bucle

		decremento: // Decrementaos edi, ya que sabeque el ultimo elemn esta ordenado
			mov esi, 0 // Inicializamos esi a 0
			sub edi, 1 // Restamos uno a edi, nuestra n

			cmp edi, 0 // Miramos si edi > 0
			jg bucle // Si es mayor que 0, volvemos a empezar

			jmp fin // Si no es mayor terminamo

		fin:

	}

	//Mostramos el resultado final del vector por pantalla
	for (int i = 0; i < 20; i++)
		printf_s(" %d ", vector[i]);
}