#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <clocale>
#include <cstdio>

#define MIN 100
#define MAX 20000000
#define NUMBER_OF_NUMBERS MAX-MIN+1 //Optymalizacja rozmiaru tablicy liczb
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

	int firstNumberMultiplication = findFirstMultiplicationInRange(min, number);
	for (int multiplication = firstNumberMultiplication; multiplication <= max; multiplication += number)
	{
		//	std::cout << multiplication << std::endl << std::flush;;
		arr->arr[multiplication - min] = true;
	}

}

void findMultiplicationsToRemove(int number, int min, int max, int* indexesToRemove, int &indexesToRemoveCurrentIndex)
{

	int firstNumberMultiplication = findFirstMultiplicationInRange(min, number);
	for (int multiplication = firstNumberMultiplication; multiplication <= max; multiplication += number)
	{
		//	std::cout << multiplication << std::endl << std::flush;;
		//arr->arr[multiplication - min] = true;

		indexesToRemove[indexesToRemoveCurrentIndex] = multiplication - min;
		indexesToRemoveCurrentIndex++;
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
		int* indexesToRemove = new int[200000000];
		int indexesToRemoveCurrentIndex = 0;

		int localMin = min;
		int localMax = max;
		int localSqrtOfMax = sqrtOfMax;

#pragma omp for
		for (int i = 2; i < localSqrtOfMax; i++)
		{


			int number = i;
			if (!globalBaseNumbers->arr[i - 2])
			{
				//std::cout << "------" << min << "--------" << number << "--------" << max << "--------\n"<<std::flush;
				//removeMultiplications(number, numbers, min, max);

				findMultiplicationsToRemove(number, localMin, localMax, indexesToRemove, indexesToRemoveCurrentIndex);
			}

		}
#pragma omp for
		for (int i = 0; i < indexesToRemoveCurrentIndex; i++)
		{
			if (!numbers->arr[indexesToRemove[i]])
			{
				numbers->arr[indexesToRemove[i]] = true;
			}
		}


		delete[] indexesToRemove;
	}
	

}

int main() {

	std::setlocale(LC_ALL, "pl_PL.UTF-8");

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
		omp_set_num_threads(1);
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