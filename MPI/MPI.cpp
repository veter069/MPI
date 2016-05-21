// MPI.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <stdio.h>
#include <math.h>

double f(double a)
{
	return (4.0 / (1.0 + a*a));
}

int main(int argc, char *argv[])
{
	int myid, numprocs;
	long long n, k;
	double PI25DT = 3.141592653589793238462643;
	double mypi, pi, h, sum, x;
	double startwtime = 0.0, endwtime;
	int  namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv); // Функция инициализации работы с MPI. В качестве аргументов этой функции передаются параметры
	//командной строки, поступающие в функцию main().
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // Получаем размер кластера (число потоков в группе)
	MPI_Comm_rank(MPI_COMM_WORLD, &myid); // Функция позволяет процессу узнать свой порядковый номер в группе. Второй параметр выходной
	// содержащим номер процесса в указанной в первом параметре группе.
	MPI_Get_processor_name(processor_name, &namelen);

	fprintf(stderr, "Process %d on %s\n",
		myid, processor_name);

	n = 0;
	while (true)
	{
		if (myid == 0)
		{
			if (n == 0) n = 1e9; else n = 0;

			startwtime = MPI_Wtime();
		}
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (n == 0)
			break;

		h = 0.5;
		sum = 0.0; int k8 = 0;
		for (k = myid; k <= n; k += numprocs) { // Распаралеливание чередованием
			k8 = k * 8;
			sum += 1.0 / pow(16.0, k)* (
				8.0 / (k8 + 2.0) + 
				4.0 / (k8 + 3.0) + 
				4.0 / (k8 + 4.0) - 
				1.0 / (k8 + 7.0));
		}
		mypi = h * sum;

		MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // Нераспараллеливаемая часть, складывает переменнные mypi в pi

		if (myid == 0)
		{
			printf("pi is approximately %.16f, Error is %.16f\n",
				pi, fabs(pi - PI25DT));
			endwtime = MPI_Wtime();
			printf("wall clock time = %f\n",
				endwtime - startwtime);
		}

	}
	MPI_Finalize(); // Функция окончания работы с MPI

	return 0;
}