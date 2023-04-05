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

using namespace std;

void printMatrix(int **D, int lenA, int lenB)
{
    int i, j;
    for (i = 0; i < lenA+1; i++)
    {
        for (j = 0; j < lenB+1; j++)
        {
            printf("%i, ", D[i][j]);
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
	int size = 1000;
	string A = generateRandString(size);
	string B = generateRandString(size);
//	cout << A << endl << B << endl;

	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> time_span;

//	for(int it = 0; it < 30; it++)
//	{
	t1 = chrono::high_resolution_clock::now();

	unsigned int lenA = A.size();
	unsigned int lenB = B.size();

	unsigned int** D = new unsigned int*[lenA+1];
	for (int i = 0; i < lenA+1; i++)
	    D[i] = new unsigned int[lenB+1];

	for(int i = 0; i < lenA+1; i++)
		D[i][0] = i;
	for(int j = 1; j < lenB+1; j++)
		D[0][j] = j;

	for(int i = 1; i < lenA+1; i++)
	{
		for(int j = 1; j < lenB+1; j++)
		{
			if(A[i-1] != B[j-1])
			{
				D[i][j] = 1 + min({D[i][j-1], D[i-1][j], D[i-1][j-1]});
			}
			else
			{
				D[i][j] = D[i-1][j-1];
			}
		}
	}
	t2 = chrono::high_resolution_clock::now();
	time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	printf("\nelapsed time: %f\n", time_span.count());
	fflush(stdout);

//	printMatrix(D, lenA, lenB);
	cout << "sequential edit distance: " << D[lenA][lenB] << endl;
//	}

	return 0;
}



