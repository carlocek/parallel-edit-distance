#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <numeric>
#include <cmath>
#include <omp.h>

#define CACHE_LINE_SIZE 64

using namespace std;

void printMatrix(int *D, int M, int N)
{
    int i, j;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%d, ", D[(i*M) + j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
}

string generateRandString(int size)
{
	const int ch_MAX = 4;
//  char alpha[ch_MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
//                          'h', 'i', 'j', 'k', 'l', 'm', 'n',
//                          'o', 'p', 'q', 'r', 's', 't', 'u',
//                          'v', 'w', 'x', 'y', 'z' };

	char alpha[ch_MAX] = { 'a', 'b', 'c', 'd'};
    string result = "";
    for (int i = 0; i < size; i++)
        result += alpha[rand() % ch_MAX];

    return result;
}

template<typename T>
struct alignas(CACHE_LINE_SIZE) cache_line_storage
{
    alignas(CACHE_LINE_SIZE) T data;
    char pad[CACHE_LINE_SIZE > sizeof(T) ? CACHE_LINE_SIZE - sizeof(T) : 1];
};

int main()
{
	int size = 70000;
	string a = generateRandString(size);
	string b = generateRandString(size);
//	string a = "bdcb";
//	string b = "aaaa";
//	cout << a << endl << b << endl;

	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> time_span;

	int nthreads = 16;
	omp_set_dynamic(0);
	omp_set_num_threads(nthreads);

	for(int it = 0; it < 10; it++)
	{
	t1 = chrono::high_resolution_clock::now();

	int M = a.size();
	int N = b.size();

//	int* D = new int[(M+1)*(N+1)];
//
//	for(int i = 0; i < M+1; i++)
//		D[i] = i;
//	for(int j = 1; j < N+1; j++)
//		D[j*(M+1)] = j;

	int* currDiag = new int[M+1]();
	int* prevDiag = new int[M+1]();
	int* prevprevDiag = new int[M+1]();

//	cache_line_storage<int>* currDiag = new cache_line_storage<int>[M+1];
//	cache_line_storage<int>* prevDiag = new cache_line_storage<int>[M+1];
//	cache_line_storage<int>* prevprevDiag = new cache_line_storage<int>[M+1];

	prevprevDiag[0] = 0;
	prevDiag[0] = 1;
	prevDiag[1] = 1;

	#pragma omp parallel default(none) shared(prevprevDiag, prevDiag, currDiag, M , N, a, b)
	{
		#pragma omp master
		{
			int dmin = 1-M;
			int dmax = N+1;
			for(int d = dmin+1; d < dmax; d++)
			{
				int id = 0;
				int imin = max(0, d);
				int imax = min(M + d, N);
				int i;
				int j;
				#pragma omp taskloop
				for(i = imin; i <= imax; i++)
				{
//					printf("aaa: %d\n", omp_get_thread_num());
//					fflush(stdout);
					j = M + d - i;
					if(j-1 < 0 || i-1 < 0)
					{
						currDiag[i] = imax;
					}
					else if(a[j-1] != b[i-1])
						currDiag[i] = 1 + min({prevDiag[i], prevDiag[i-1], prevprevDiag[i-1]});
					else
						currDiag[i] = prevprevDiag[i-1];
					id++;
				}
//				#pragma omp taskwait
//				printf("%d, %d , %d, %d, %d\n", currDiag[0], currDiag[1], currDiag[2], currDiag[3], currDiag[4]);
//				fflush(stdout);

				int* tmp = prevprevDiag;
				prevprevDiag = prevDiag;
				prevDiag = currDiag;
				currDiag = tmp;

			}
		}

	}
	t2 = chrono::high_resolution_clock::now();
	time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	printf("\nelapsed time: %f\n", time_span.count());
	fflush(stdout);

	cout << currDiag[M] << endl;
	}

	return 0;
}









