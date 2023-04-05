#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <numeric>
#include <cmath>
#include <omp.h>

using namespace std;

void printMatrix(int *D, int lenA, int lenB)
{
    int i, j;
    for (i = 0; i < lenB+1; i++)
    {
        for (j = 0; j < lenA+1; j++)
        {
            printf("%d, ", D[(i*(lenA+1)) + j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
}

string generateRandString(int size)
{
	const int ch_MAX = 4;
//  char alphabet[ch_MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
//                          'h', 'i', 'j', 'k', 'l', 'm', 'n',
//                          'o', 'p', 'q', 'r', 's', 't', 'u',
//                          'v', 'w', 'x', 'y', 'z' };

	char alphabet[ch_MAX] = {'a', 'b', 'c', 'd'};
    string result = "";
    for (int i = 0; i < size; i++)
        result += alphabet[rand() % ch_MAX];

    return result;
}

void computeTile(int I, int J, unsigned int lenA, unsigned int lenB, string A, string B, unsigned int* D, int tileWidth)
{
	//compute start row and column indexes of D from tile coordinates in the "tiled matrix"
	I = I * tileWidth + 1;
	J = J * tileWidth + 1;
	for(int i = I; i < lenB+1 && i < I+tileWidth; i++)
	{
		for(int j = J; j < lenA+1 && j < J+tileWidth; j++)
		{
			if(A[j - 1] != B[i - 1])
			{
				D[i * (lenA+1) + j] = 1 + min({D[i * (lenA+1) + j - 1], D[(i - 1) * (lenA+1) + j], D[(i - 1) * (lenA+1) + j - 1]});
			}
			else
			{
				D[i * (lenA+1) + j] = D[(i - 1) * (lenA+1) + j - 1];
			}
		}
	}
}

int main()
{
	int size = 30000;
	string A = generateRandString(size);
	string B = generateRandString(size);

//	string A = "saturday";
//	string B = "sunday";
//	cout << A << endl << B << endl;

	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> time_span;

	int nthreads = 1;
	omp_set_dynamic(0);
	omp_set_num_threads(nthreads);
	int tileWidth = 500;

//	for(int it = 0; it < 10; it++)
//	{
	t1 = chrono::high_resolution_clock::now();

	unsigned int lenA = A.size();
	unsigned int lenB = B.size();

	unsigned int* D = new unsigned int[(lenA+1)*(lenB+1)];

	//initialize first row and first column
	for(int i = 0; i < lenA+1; i++)
		D[i] = i;
	for(int j = 1; j < lenB+1; j++)
		D[j*(lenA+1)] = j;

	int tilesA = ceil((float)(lenA)/tileWidth);
	int tilesB = ceil((float)(lenB)/tileWidth);
	cout << tilesA << endl << tilesB << endl;

	#pragma omp parallel default(none) shared(D, A, B, lenA, lenB, tilesA, tilesB, tileWidth)
	{
		//compute number of antidiagonals of tiles to be computed with synchronization in between
		int dmin = 1-tilesA;
		int dmax = tilesB;
		#pragma omp master
		{
			for(int d = dmin; d < dmax; d++)
			{
				//compute number of tiles in the current antidiagonal to be computed in parallel
				int imin = max(0, d);
				int imax = min(tilesA + d, tilesB);
				#pragma omp taskloop
				for(int i = imin; i < imax; i++)
				{
					int j = tilesA + d - i - 1;
					computeTile(i, j, lenA, lenB, A, B, D, tileWidth);
				}
			}
		}
	}
	t2 = chrono::high_resolution_clock::now();
	time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	printf("\nelapsed time: %f\n", time_span.count());
	fflush(stdout);

	cout << "parallel edit distance: " << D[lenB*(lenA+1) + lenA] << endl;
//	}
	return 0;
}













