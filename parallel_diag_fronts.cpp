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

void printMatrix(int *D, int lenA, int lenB)
{
    int i, j;
    for (i = 0; i < lenA+1; i++)
    {
        for (j = 0; j < lenB+1; j++)
        {
            printf("%d, ", D[(i*lenA) + j]);
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

	char alpha[ch_MAX] = {'a', 'b', 'c', 'd'};
    string result = "";
    for (int i = 0; i < size; i++)
        result += alpha[rand() % ch_MAX];

    return result;
}

struct cache_storage
{
    int data;
    char pad[CACHE_LINE_SIZE];
};

int main()
{
	int size = 70000;
	string A = generateRandString(size);
	string B = generateRandString(size);

//	string A = "bdcb";
//	string B = "aaaa";
//	cout << A << endl << B << endl;

	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> time_span;

	int nthreads = 16;
	omp_set_dynamic(0);
	omp_set_num_threads(nthreads);

	for(int it = 0; it < 10; it++)
	{
	t1 = chrono::high_resolution_clock::now();

	unsigned int lenA = A.size();
	unsigned int lenB = B.size();

	unsigned int* currDiag = new unsigned int[lenA+1];
	unsigned int* prevDiag = new unsigned int[lenA+1];
	unsigned int* prevprevDiag = new unsigned int[lenA+1];

	prevprevDiag[0] = 0;
	prevDiag[0] = 1;
	prevDiag[1] = 1;

	#pragma omp parallel default(none) shared(prevprevDiag, prevDiag, currDiag, lenA, lenB, A, B)
	{
		#pragma omp master
		{
			int dmin = 2-lenA;
			int dmax = lenB+1;
			for(int d = dmin; d < dmax; d++)
			{
				int imin = max(0, d);
				int imax = min(lenA + d, lenB);
				int i;
				int j;
				#pragma omp taskloop
				for(i = imin; i <= imax; i++)
				{
//					printf("aaa: %d\n", omp_get_thread_num());
//					fflush(stdout);
					j = lenA + d - i;
					if(j-1 < 0 || i-1 < 0)
					{
						currDiag[i] = imax;
					}
					else if(A[j-1] != B[i-1])
						currDiag[i] = 1 + min({prevDiag[i], prevDiag[i-1], prevprevDiag[i-1]});
					else
						currDiag[i] = prevprevDiag[i-1];
				}
//				#pragma omp taskwait
//				printf("%d, %d , %d, %d, %d\n", currDiag[0], currDiag[1], currDiag[2], currDiag[3], currDiag[4]);
//				fflush(stdout);

				unsigned int* tmp = prevprevDiag;
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

	cout << currDiag[lenA] << endl;
	}

	return 0;
}









