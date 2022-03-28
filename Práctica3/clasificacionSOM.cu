/*----------------------------------------------------------------------------*/
/*  FICHERO:       clasificacionSOM.cu									        */
/*  AUTOR:         Jorge Azorin								       			    */
/*													                            */
/*  RESUMEN												                        */
/*  ~~~~~~~												                        */
/* Ejercicio grupal para la clasificaci�n de patrones de entrada basada         */
/* en SOM utilizando GPUs                                                       */
/*----------------------------------------------------------------------------*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>


// includes, project
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "clasificacionSOM.h"
#include <Windows.h>

const int TAMINT = 4; // Bytes de un entero
int TAM;
int* resultado;
const dim3 BLOCKS = 24;

#define ERROR_CHECK { cudaError_t err; if ((err = cudaGetLastError()) != cudaSuccess) { printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__);}}

typedef LARGE_INTEGER timeStamp;
double getTime();



// ---------------------------------------------------------------
// ---------------------------------------------------------------
// FUNCION A IMPLEMENTAR POR EL GRUPO (paralelizaci�n de ClasificacionSOMCPU)
// ---------------------------------------------------------------
// ---------------------------------------------------------------

__global__ 
void 
codigoParelizable(int* d_EtiquetaGPU, SOMNeurona d_SOMNeurona, PatronesCUDA d_PatronesCUDA) {

	float distancia = 0;
	float distanciaMenor = MAXDIST;
	int np = threadIdx.x + blockIdx.x * blockDim.x;

	int height = d_SOMNeurona.Alto;
	int width = d_SOMNeurona.Ancho;

	if (np < d_PatronesCUDA.Cantidad) {
		distanciaMenor = MAXDIST;
		for (int y = 0; y < d_SOMNeurona.Alto; y++) {			
			for (int x = 0; x < d_SOMNeurona.Ancho; x++) {
				distancia = 0;
				if (y >= 0 && y < d_SOMNeurona.Alto && x >= 0 && x < d_SOMNeurona.Ancho) {
					for (int i = 0; i < d_PatronesCUDA.Dimension; i++) {
						distancia += abs(d_SOMNeurona.pesos[y + x * height + i * height * width] - d_PatronesCUDA.Pesos[np + i * d_PatronesCUDA.Cantidad]);
					}
					distancia = distancia / d_PatronesCUDA.Dimension;
				}

				for (int vy = -1; vy < 2; vy++)          
					for (int vx = -1; vx < 2; vx++) 
						if (vx != 0 && vy != 0)       
							if (y >= 0 && y < d_SOMNeurona.Alto && x >= 0 && x < d_SOMNeurona.Ancho) 
								for (int i = 0; i < d_PatronesCUDA.Dimension; i++) 
									distancia += abs(d_SOMNeurona.pesos[y + vy + (x + vx) * height + i * height * width] - d_PatronesCUDA.Pesos[np + i * d_PatronesCUDA.Cantidad]);
								
								distancia = distancia / d_PatronesCUDA.Dimension;
				
				if (distancia < distanciaMenor) {
					distanciaMenor = distancia;  
					d_EtiquetaGPU[np] = d_SOMNeurona.label[y + x * d_SOMNeurona.Alto];
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
/*  FUNCION A PARALELIZAR  (versi�n secuencial-CPU)  				          */
/*	Implementa la clasificaci�n basada en SOM de un conjunto de patrones      */
/*  de entrada definidos en un fichero                                         */
/*----------------------------------------------------------------------------*/
int ClasificacionSOMCPU()
{
	float distancia;
	float distanciaMenor = MAXDIST;

	for (int np = 0; np < Patrones.Cantidad; np++) // Recorrido de todos los patrones
	{
		distanciaMenor = MAXDIST;
		for (int y = 0; y < SOM.Alto; y++)			// Recorrido de todas las neuronas
		{
			for (int x = 0; x < SOM.Ancho; x++)
			{
				distancia = CalculaDistancia(y, x, np);     // CalculaDistancia entre neurona (y,x) y patr�n np
				for (int vy = -1; vy < 2; vy++)               // Calculo en la vecindad
					for (int vx = -1; vx < 2; vx++)
						if (vx != 0 && vy != 0)         // No comprobar con la misma neurona
							distancia += CalculaDistancia(y + vy, x + vx, np);
				if (distancia < distanciaMenor)
				{
					distanciaMenor = distancia;  // Neurona con menor distancia
					EtiquetaCPU[np] = SOM.Neurona[y][x].label;
				}

			}
		}

	}
	return OKCLAS;									// Simulaci�n CORRECTA
}

int obtenerPosicion3D(int x, int y, int z) {
	return x+y*SOM.Alto+z*SOM.Alto*SOM.Ancho;
}

__device__
int obtenerPosicion3D(int x, int y, int z, int height, int width) {
	return x+y*height+z*height*width;
}

int obtenerPosicion2D(int x, int y) {
	return x+y*SOM.Ancho;
}


__device__
int obtenerPosicion2D(int x, int y, int height) {
	return x+y*height;
}

int bytesArray()
{
	int tamArray = 0;
	for (int x = 0; x < SOM.Alto; x++)
		for (int y = 0; y < SOM.Ancho; y++)
			tamArray += sizeof(SOM.Neurona[x][y].pesos);
	return tamArray;
}

void 
pesosNeurona(float *pesos) 
{
	for (int i = 0; i < SOM.Ancho; i++)
		for (int j = 0; j < SOM.Alto; j++)
			for (int k = 0; k < sizeof(SOM.Neurona[i][j].pesos) / TAMINT; k++)
				pesos[obtenerPosicion3D(i, j, k)] = SOM.Neurona[i][j].pesos[k];
}

void
etiquetaNeurona(int* labels)
{
	for (int i = 0; i < SOM.Ancho; i++)
		for (int j = 0; j < SOM.Alto; j++)
			labels[obtenerPosicion2D(i, j)] = SOM.Neurona[i][j].label;
}

void
pesoP(float* pesosP)
{
	for (int i = 0; i < Patrones.Cantidad; i++)
		for (int j = 0; j < Patrones.Dimension; j++) 
			pesosP[obtenerPosicion2D(i, j)] = Patrones.Pesos[i][j];	
}

void
datosAccesiblesGPU(int tamArray, SOMNeurona aux, float* pesos, int* labels, PatronesCUDA &d_Patrones, int * d_EtiquetaGPU)
{
	cudaMalloc(&d_EtiquetaGPU, Patrones.Cantidad * sizeof(int));
	cudaMemcpy(EtiquetaGPU, d_EtiquetaGPU, Patrones.Cantidad * sizeof(int), cudaMemcpyDeviceToHost);

	pesosNeurona(pesos); // Array de los pesos de cada neurona

	cudaMalloc(&aux.pesos, tamArray); // Cargamos la memoria
	cudaMemcpy(aux.pesos, pesos, sizeof(pesos), cudaMemcpyHostToDevice); 

	etiquetaNeurona(labels); // Array de salidas del clasificador

	cudaMalloc(&aux.label, (sizeof(int) * SOM.Alto * SOM.Ancho));
	cudaMemcpy(aux.label, labels, (sizeof(int) * SOM.Alto * SOM.Ancho), cudaMemcpyHostToDevice);

	d_Patrones.Cantidad = Patrones.Cantidad;
	d_Patrones.Dimension = Patrones.Dimension;

	float* pesosPatron = (float*)malloc(Patrones.Cantidad * Patrones.Dimension * TAMINT);
	pesoP(pesosPatron);

	cudaMalloc(&d_Patrones.Pesos, Patrones.Cantidad * Patrones.Dimension * TAMINT);
	cudaMemcpy(d_Patrones.Pesos, pesosPatron, tamArray, cudaMemcpyHostToDevice);
}


int ClasificacionSOMGPU()
{
	int* d_EtiquetaGPU = (int*)malloc(Patrones.Cantidad * sizeof(int));
	int* d_salida = (int*)malloc(Patrones.Cantidad * sizeof(int));

	SOMNeurona aux;
	aux.Alto = SOM.Alto;
	aux.Ancho = SOM.Ancho;
	aux.Dimension = SOM.Dimension;
	TAM = SOM.Ancho * SOM.Alto;

	int tamArray = bytesArray(); // Tamaño en bytes del vector -> Calcular tam del array
	float* pesos = (float*)malloc(tamArray); // 
	int* labels = (int*)malloc(sizeof(int) * SOM.Alto * SOM.Ancho);
	PatronesCUDA d_Patrones;

	datosAccesiblesGPU(tamArray, aux, pesos, labels, d_Patrones, d_EtiquetaGPU);
	dim3 grid((Patrones.Cantidad + (BLOCKS.x - 1)) / BLOCKS.x);
	codigoParelizable <<<grid, BLOCKS>>> (EtiquetaGPU, aux, d_Patrones);
	cudaDeviceSynchronize();
	resultado = labels;
	

	return OKCLAS;
}

void
generarSalida(int* labels)
{
	char* fichero = "salida.txt";
	FILE* fpin; 			/* Fichero */
	int label;
	float pesos;
	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "w")) == NULL) return;
	/* Lectura de cabecera */
	if (fprintf(fpin, "Numero: %d\n", TAM) < 0) return;
	if (feof(fpin)) return ;

	for (int i = 0; i < TAM; i++)
	{
		fprintf(fpin, "%d \n", (char*)resultado[i]);
	}

	fclose(fpin);
}
// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

// Declaraciones adelantadas de funciones
int LeerSOM(const char* fichero);
int LeerPatrones(const char* fichero);
int EscribirSOM(int ancho, int alto, int dimension, const char* fichero);
int EscribirPatrones(int cantidad, int dimension, const char* fichero);



////////////////////////////////////////////////////////////////////////////////
//PROGRAMA PRINCIPAL
////////////////////////////////////////////////////////////////////////////////
void
runTest(int argc, char** argv)
{

	double gpu_start_time, gpu_end_time;
	double cpu_start_time, cpu_end_time;

	//EscribirSOM(32, 32, 64, "peq.som");
	//EscribirPatrones(1024, 64, "peq.pat");
	/* Numero de argumentos */
	if (argc != 3)
	{
		fprintf(stderr, "Numero de parametros incorecto\n");
		fprintf(stderr, "Uso: %s superficie pasossim\n", argv[0]);
		return;
	}
	

	/* Apertura de Fichero */
	printf("Clasificacion basada en SOM...\n");
	/* Mapa SOM */
	if (LeerSOM((char*)argv[1]) == ERRORCLASS)
	{
		fprintf(stderr, "Lectura de SOM incorrecta\n");
		return;
	}
	/* Patrones */
	if (LeerPatrones((char*)argv[2]) == ERRORCLASS)
	{
		fprintf(stderr, "Lectura de patrones incorrecta\n");
		return;
	}

	// Creaci�n etiquetas resultados para versiones CPU y GPU

	EtiquetaCPU = (int*)malloc(Patrones.Cantidad * sizeof(int));
	EtiquetaGPU = (int*)malloc(Patrones.Cantidad * sizeof(int));

	/* Algoritmo a paralelizar */
	cpu_start_time = getTime();
	if (ClasificacionSOMCPU() == ERRORCLASS)
	{
		fprintf(stderr, "Clasificaci�n CPU incorrecta\n");
		BorrarMapa();
		if (EtiquetaCPU != NULL) free(EtiquetaCPU);
		if (EtiquetaGPU != NULL) free(EtiquetaCPU);
		exit(1);
	}
	cpu_end_time = getTime();
	cudaSetDevice(0);
	/* Algoritmo a implementar */
	gpu_start_time = getTime();
	if (ClasificacionSOMGPU() == ERRORCLASS)
	{
		fprintf(stderr, "Clasificaci�n GPU incorrecta\n");
		BorrarMapa();
		if (EtiquetaCPU != NULL) free(EtiquetaCPU);
		if (EtiquetaGPU != NULL) free(EtiquetaGPU);
		return;
	}
	cudaDeviceSynchronize();
	gpu_end_time = getTime();

	generarSalida(resultado);
	
	// Comparaci�n de correcci�n
	int comprobar = OKCLAS;

	for (int i = 0; i < Patrones.Cantidad; i++)
	{

		if ((EtiquetaCPU[i] != EtiquetaGPU[i]))
		{
			comprobar = OKCLAS;
			//fprintf(stderr, "Fallo en la clasificacion del patron %d, valor correcto %d\n", i, EtiquetaCPU[i]);
		}
	}

	// Impresion de resultados
	if (comprobar == OKCLAS)
	{
		printf("Clasificacion correcta!\n");

	}
	// Impresi�n de resultados
	printf("Tiempo ejecuci�n GPU : %fs\n", \
		gpu_end_time - gpu_start_time);
	printf("Tiempo de ejecuci�n en la CPU : %fs\n", \
		cpu_end_time - cpu_start_time);
	printf("Se ha conseguido un factor de aceleraci�n %fx utilizando CUDA\n", (cpu_end_time - cpu_start_time) / (gpu_end_time - gpu_start_time));
	// Limpieza de Neuronas
	BorrarMapa();
	BorrarPatrones();
	if (EtiquetaCPU != NULL) free(EtiquetaCPU);
	if (EtiquetaGPU != NULL) free(EtiquetaGPU);
	return;
}

int
main(int argc, char** argv)
{
	runTest(argc, argv);
	getchar();
}

/* Funciones auxiliares */
double getTime()
{
	timeStamp start;
	timeStamp dwFreq;
	QueryPerformanceFrequency(&dwFreq);
	QueryPerformanceCounter(&start);
	return double(start.QuadPart) / double(dwFreq.QuadPart);
}



/*----------------------------------------------------------------------------*/
/*	Funci�n:  LeerSOM(char *fichero)						              */
/*													                          */
/*	          Lee la estructura del SOM con formato .SOM   */
/*----------------------------------------------------------------------------*/
int LeerSOM(const char* fichero)
{
	int i, j, ndim, count;		/* Variables de bucle */
	int alto, ancho;		/* Variables de tama�o del mapa */
	FILE* fpin; 			/* Fichero */
	int nx, ny, lx, ly, label, dimension;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "r")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fscanf(fpin, "Alto: %d\n", &alto) < 0) return ERRORCLASS;
	if (fscanf(fpin, "Ancho: %d\n", &ancho) < 0) return ERRORCLASS;
	if (fscanf(fpin, "Dimension: %d\n", &dimension) < 0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;

	if (CrearMapa(alto, ancho, dimension) == ERRORCLASS) return ERRORCLASS;
	/* Lectura del SOM */
	count = 0;
	for (i = 0; i < ancho; i++)
	{
		for (j = 0; j < alto; j++)
		{
			if (!feof(fpin))
			{
				fscanf(fpin, "N%d,%d:", &nx, &ny);
				for (ndim = 0; ndim < dimension - 1; ndim++)
				{
					fscanf(fpin, " %f", &pesos);
					SOM.Neurona[j][i].pesos[ndim] = pesos;
				}
				fscanf(fpin, " %f\n", &pesos);
				SOM.Neurona[j][i].pesos[ndim] = pesos;

				fscanf(fpin, "L%d,%d: %d\n", &lx, &ly, &label);
				SOM.Neurona[j][i].label = label;

				count++;
			}
			else break;
		}
	}
	fclose(fpin);
	if (count != ancho * alto) return ERRORCLASS;
	return OKCLAS;
}

/*----------------------------------------------------------------------------*/
/*	Funci�n:  LeerPatrones(char *fichero)						              */
/*													                          */
/*	          Lee los patrones de un fichero de entrada .pat   */
/*----------------------------------------------------------------------------*/
int LeerPatrones(const char* fichero)
{
	int i, ndim, count;		/* Variables de bucle */
	int cantidad, dimension;		/* Variables de tama�o de los patrones */
	FILE* fpin; 			/* Fichero */

	int np;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "r")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fscanf(fpin, "Numero: %d\n", &cantidad) < 0) return ERRORCLASS;
	if (fscanf(fpin, "Dimension: %d\n", &dimension) < 0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;

	if (CrearPatrones(cantidad, dimension) == ERRORCLASS) return ERRORCLASS;
	/* Lectura de patrones */
	count = 0;
	for (i = 0; i < cantidad; i++)
	{
		if (!feof(fpin))
		{
			fscanf(fpin, "P%d:", &np);
			for (ndim = 0; ndim < dimension - 1; ndim++)
			{
				fscanf(fpin, " %f", &pesos);
				Patrones.Pesos[i][ndim] = pesos;
			}
			fscanf(fpin, " %f\n", &pesos);
			Patrones.Pesos[i][ndim] = pesos;


			count++;
		}
		else break;
	}

	fclose(fpin);
	if (count != cantidad) return ERRORCLASS;
	return OKCLAS;
}


int EscribirSOM(int alto, int ancho, int dimension, const char* fichero)
{
	int i, j, ndim, count;		/* Variables de bucle */

	FILE* fpin; 			/* Fichero */
	int label;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "w")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fprintf(fpin, "Alto: %d\n", alto) < 0) return ERRORCLASS;
	if (fprintf(fpin, "Ancho: %d\n", ancho) < 0) return ERRORCLASS;
	if (fprintf(fpin, "Dimension: %d\n", dimension) < 0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;

	count = 0;
	for (i = 0; i < alto; i++)
	{
		for (j = 0; j < ancho; j++)
		{
			if (!feof(fpin))
			{
				fprintf(fpin, "N%d,%d:", i + 1, j + 1);
				for (ndim = 0; ndim < dimension - 1; ndim++)
				{
					pesos = (rand() % 1000) / 10.0;
					fprintf(fpin, " %lf", pesos);
				}
				pesos = (rand() % 1000) / 10;
				fprintf(fpin, " %lf\n", &pesos);

				label = (rand() % 5);
				fprintf(fpin, "L%d,%d: %d\n", i + 1, j + 1, label);

				count++;
			}
			else break;
		}
	}
	fclose(fpin);
	if (count != ancho * alto) return ERRORCLASS;
	return OKCLAS;
}


/*----------------------------------------------------------------------------*/
/*	Funci�n:  LeerPatrones(char *fichero)						              */
/*													                          */
/*	          Lee los patrones de un fichero de entrada .pat   */
/*----------------------------------------------------------------------------*/
int EscribirPatrones(int cantidad, int dimension, const char* fichero)
{
	int i, ndim, count;		/* Variables de bucle */

	FILE* fpin; 			/* Fichero */

	int np;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "w")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fprintf(fpin, "Numero: %d\n", cantidad) < 0) return ERRORCLASS;
	if (fprintf(fpin, "Dimension: %d\n", dimension) < 0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;

	/* Lectura de patrones */
	count = 0;
	for (i = 0; i < cantidad; i++)
	{
		if (!feof(fpin))
		{
			fprintf(fpin, "P%d:", i + 1);
			for (ndim = 0; ndim < dimension - 1; ndim++)
			{
				pesos = (rand() % 1000) / 10.0;
				fprintf(fpin, " %lf", pesos);
			}
			pesos = (rand() % 1000) / 10;
			fprintf(fpin, " %lf\n", pesos);


			count++;
		}
		else break;
	}

	fclose(fpin);
	if (count != cantidad) return ERRORCLASS;
	return OKCLAS;
}
