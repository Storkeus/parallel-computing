﻿#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <clocale>
#include <cstdio>

#define MIN 100
#define MAX 2000000000
#define NUMBER_OF_NUMBERS MAX-MIN //Optymalizacja rozmiaru tablicy liczb
#define SHOW_RESULTS 1


struct DynamicArrayBool
{
	int length;
	bool* arr;
};

DynamicArrayBool* globalNumbers = new DynamicArrayBool;
DynamicArrayBool* globalBaseNumbers = new DynamicArrayBool;

int findFirstMultiplicationInRange(int min, int number)
{
	//wielokrotność liczby najbliższa minimum
	int multiplication = (min / number) * number;




	while (multiplication < min || multiplication == number)
	{
		multiplication += number;
	}




	return multiplication;
}

void removeMultiplications(int number, DynamicArrayBool* arr, int min, int max)
{
	//printf("wykonal:%d\n",omp_get_thread_num());
	int firstNumberMultiplication = findFirstMultiplicationInRange(min, number);
	for (int multiplication = firstNumberMultiplication; multiplication <= max; multiplication += number)
	{
		//	std::cout << multiplication << std::endl << std::flush;;
		if (!arr->arr[multiplication - min])
		{
			arr->arr[multiplication - min] = true;
		}

	}

}



void findPrimeNumbers(int min, int max, DynamicArrayBool* numbers)
{


	//Warunek stopu - usunięcie liczb parzystych
	if (max <= 2)
	{
		removeMultiplications(2, numbers, min, max);
		return;
	}
	int sqrtOfMax = sqrt(max);
	findPrimeNumbers(2, sqrtOfMax, globalBaseNumbers);

#pragma omp parallel
	{
#pragma omp for schedule(dynamic)
		for (int i = 2; i < sqrtOfMax; i++)
		{

			int number = i;
			if (!globalBaseNumbers->arr[i - 2])
			{
				//std::cout << "------" << min << "--------" << number << "--------" << max << "--------\n"<<std::flush;
				removeMultiplications(number, numbers, min, max);
			}

		}
	}

}

int main() {

	std::setlocale(LC_ALL, "pl_PL.UTF-8");

	omp_set_num_threads(4);

	//Tablica, której indeksy odpowiadają liczbom z zakresu <min;max>
	//indeks zadanej liczby = liczba-min
	//wartość true = liczba usunięta
	globalNumbers->arr = new bool[NUMBER_OF_NUMBERS] {};
	globalNumbers->length = NUMBER_OF_NUMBERS;

	//Tablica, której indekst odpowiadają liczbom z zakresu <2;sqrt(max)>
	//indeks zadanej liczby = liczba-2
	//wartość true = liczba usunięta
	globalBaseNumbers->arr = new bool[(int)sqrt(MAX) - 2]{};
	globalBaseNumbers->length = (int)sqrt(MAX) - 2;

	clock_t start, stop;
	start = clock();

	//Rozpoczęcie poszukiwania liczb pierwszych
	findPrimeNumbers(MIN, MAX, globalNumbers);



	stop = clock();


	//Wyświetlanie wyników
	if (SHOW_RESULTS)
	{
		printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));

		FILE* file = fopen("results.txt", "w");


		unsigned long long resultsCount = 0;

		for (int i = 0; i < globalNumbers->length; i++)
		{
			if (!globalNumbers->arr[i])
			{
				resultsCount++;
			}
		}

		fprintf(file, "%d %d %d \n", MIN, MAX, resultsCount);

		int counter = 0;

		for (int i = 0; i < globalNumbers->length; i++)
		{
			if (!globalNumbers->arr[i])
			{
				fprintf(file, "%d ", i + MIN);
				counter++;

				if (counter == 10) {
					fprintf(file, "\n");
					counter = 0;
				}
			}

		}
		fclose(file);
		printf("Ilosc liczb pierwszych w przedziale %d : %d >: %d", MIN, MAX, resultsCount);
	}

}