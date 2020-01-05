#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <iostream>

#define MIN 1000
#define MAX 2000
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

	int firstNumberMultiplication = findFirstMultiplicationInRange(min, number);
	for (int multiplication = firstNumberMultiplication; multiplication <= max; multiplication += number)
	{
		//std::cout << multiplication << std::endl;
		arr->arr[multiplication - min] = true;
	}

}



void findPrimeNumbers(int min, int max, DynamicArrayBool* numbers)
{

	//Warunek stopu - usunięcie liczb parzystych
	if (max == 2)
	{
		removeMultiplications(2, numbers, min, max);
		return;
	}

	findPrimeNumbers(2, sqrt(max), globalBaseNumbers);

	for (int i = 2; i < sqrt(max); i++)
	{
		int number = i;
		if (!globalBaseNumbers->arr[i - 2])
		{
			//std::cout << "------" << min << "--------" << number << "--------" << max << "--------\n";
			removeMultiplications(number, numbers, min, max);
		}

	}

}

int main() {

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

	//Rozpoczęcie poszukiwania liczb pierwszych
	findPrimeNumbers(MIN, MAX, globalNumbers);


	//Wyświetlanie wyników
	if (SHOW_RESULTS)
	{
		for (int i = 0; i < globalNumbers->length; i++)
		{
			if (!globalNumbers->arr[i])
			{
				std::cout << i + MIN << std::endl;
			}

		}
	}

}