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

int main()
{
	int size = 30000;
	string a = generateRandString(size);
	string b = generateRandString(size);
//	cout << a << endl << b << endl;

	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> time_span;

	int nthreads = 8;
	omp_set_dynamic(0);
	omp_set_num_threads(nthreads);

	t1 = chrono::high_resolution_clock::now();

	int M = a.size();
	int N = b.size();

//	int** D = new int*[M+1];
//	for (int i = 0; i < M+1; i++)
//		D[i] = new int[N+1];
//
//	for(int i = 0; i < M+1; i++)
//		D[i][0] = i;
//	for(int j = 1; j < N+1; j++)
//		D[0][j] = j;

	int* D = new int[(M+1)*(N+1)];


	for(int i = 0; i < M+1; i++)
		D[i] = i;
	for(int j = 1; j < N+1; j++)
		D[j*(M+1)] = j;



//	#pragma omp parallel default(none) shared(D, a, b, M, N)
//	{
//		#pragma omp master
//		{
//			int dmin = 1-(M);
//			int dmax = N+1;
//			int imin;
//			int imax;
//			for(int d = dmin; d < dmax; d++)
//			{
//				imin = max(1, d);
//				imax = min(M + d, N+1);
//				int j;
//				for(int i = imin; i < imax; i++)
//				{
//					#pragma omp task
//					{
//						j = (M) + d - i;
//						if(a[i-1] != b[j-1])
//						{
//							D[i][j] = 1 + min({D[i][j-1], D[i-1][j], D[i-1][j-1]});
//						}
//						else
//						{
//							D[i][j] = D[i-1][j-1];
//						}
//					}
//				}
//				#pragma omp taskwait
//			}
//		}
//	}

	#pragma omp parallel default(none) shared(D, a, b, M, N)
	{
		#pragma omp master
		{
			int dmin = 1-M;
			int dmax = N+1;
			int imin;
			int imax;
			for(int d = dmin; d < dmax; d++)
			{
				imin = max(1, d);
				imax = min(M + d, N+1);
				int j;
				#pragma omp taskloop
				for(int i = imin; i < imax; i++)
				{

						j = M + d - i;
						if(a[i-1] != b[j-1])
						{
							int m = min({D[i*(M+1) + j-1], D[(i-1)*(M+1) + j], D[(i-1)*(M+1) + j-1]});
							D[i*(M+1) + j] = 1 + m;
						}
						else
						{
							int m = D[(i-1)*(M+1) + j-1];
							D[i*(M+1) + j] = m;
						}

				}
//				#pragma omp taskwait
			}
		}
	}
	t2 = chrono::high_resolution_clock::now();
	time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	printf("\nelapsed time: %f\n", time_span.count());
	fflush(stdout);

//	printMatrix(D, M+1, N+1);
	cout << D[N*(M+1) + M] << endl;

	return 0;
}




