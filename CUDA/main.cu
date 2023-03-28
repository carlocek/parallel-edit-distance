#include <stdlib.h>
#include <iostream>
#include <cuda.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <chrono>

#define MIN(x,y) ((x) < (y) ? (x) : (y))

using namespace std;

char* generateRandString(char* str, int size)
{
	const int ch_MAX = 4;
//  char alphabet[ch_MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
//                          'h', 'i', 'j', 'k', 'l', 'm', 'n',
//                          'o', 'p', 'q', 'r', 's', 't', 'u',
//                          'v', 'w', 'x', 'y', 'z' };
	char alphabet[ch_MAX] = {'a', 'b', 'c', 'd'};
    for(int i = 0; i < size; i++)
        str[i] = alphabet[rand() % ch_MAX];
    str[size] = '\0';
    return str;
}

__global__ void editDistKernel(char* devA, char* devB, int lenA, int lenB, unsigned int* d_prevprevDiagPtr, unsigned int* d_prevDiagPtr, unsigned int* d_currDiagPtr, int d)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int bd = blockDim.x;
	int j = lenA+d-tid;
	__shared__ extern unsigned int ds_currDiag[];

	if(d < 1)
	{
		if(tid == 0)
			ds_currDiag[0] = min(lenA+d, lenB+1);
		else if(tid == lenA+d)
			ds_currDiag[(lenA+d) % bd] = min(lenA+d, lenB+1);
	}
	if(tid < min(lenA+d, lenB+1) && tid > max(0, d-1))
	{
		if(devA[j-1] != devB[tid-1])
			ds_currDiag[tid % bd] = 1 + MIN(d_prevDiagPtr[tid], MIN(d_prevDiagPtr[tid-1], d_prevprevDiagPtr[tid-1]));
		else
			ds_currDiag[tid % bd] = d_prevprevDiagPtr[tid-1];
	}
//	__threadfence();
	__syncthreads();
	if(tid < lenA+1)
	{
		d_currDiagPtr[tid] = ds_currDiag[tid % bd];
	}

//	__threadfence();
//	if(tid % blockDim.x == 0)
//	{
//		unsigned int* tmp = d_prevprevDiagPtr;
//		d_prevprevDiagPtr = d_prevDiagPtr;
//		d_prevDiagPtr = d_currDiagPtr;
//		d_currDiagPtr = tmp;
//	}
//	__threadfence();
//	if(tid == 0)
//	{
//		for(int i = 0; i < lenA+1; i++)
//			printf("%d, ", d_currDiagPtr[i]);
//		printf("\n");
//	}
}

int main()
{
	for(int it = 0; it < 20; it++)
	{
	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> time_span;

	int lenA = 10000;
	int lenB = 10000;
	char* A = new char[lenA+1];
	char* B = new char[lenB+1];
//	char A[lenA] = "sundayy";
//	char B[lenB] = "sfnlayz";
	char* devA;
	char* devB;
	cudaMalloc((void**)&devA, (lenA+1)*sizeof(char));
	cudaMalloc((void**)&devB, (lenB+1)*sizeof(char));
	generateRandString(A, lenA);
	generateRandString(B, lenB);
	cudaMemcpy((void*)devA, (void*)A, (lenA+1)*sizeof(char), cudaMemcpyHostToDevice);
	cudaMemcpy((void*)devB, (void*)B, (lenB+1)*sizeof(char), cudaMemcpyHostToDevice);

	for(int i = 0; i < lenA; i++)
		printf("%c", A[i]);
	printf("\n");
	for(int i = 0; i < lenB; i++)
		printf("%c", B[i]);
	printf("\n");

	t1 = chrono::high_resolution_clock::now();

	unsigned int* d_currDiag;
	unsigned int* d_prevDiag;
	unsigned int* d_prevprevDiag;

	unsigned int* d_currDiagPtr;
	unsigned int* d_prevDiagPtr;
	unsigned int* d_prevprevDiagPtr;

	unsigned int* currDiag = new unsigned int[lenA+1];
	unsigned int* prevDiag = new unsigned int[lenA+1];
	unsigned int* prevprevDiag = new unsigned int[lenA+1];
	cudaMalloc((void**)&d_currDiag, (lenA+1)*sizeof(unsigned int));
	cudaMalloc((void**)&d_prevDiag, (lenA+1)*sizeof(unsigned int));
	cudaMalloc((void**)&d_prevprevDiag, (lenA+1)*sizeof(unsigned int));

	prevprevDiag[0] = 0;
	prevDiag[0] = 1;
	prevDiag[1] = 1;
	cudaMemcpy((void*)d_prevprevDiag, (void*)prevprevDiag, (lenA+1)*sizeof(unsigned int), cudaMemcpyHostToDevice);
	cudaMemcpy((void*)d_prevDiag, (void*)prevDiag, (lenA+1)*sizeof(unsigned int), cudaMemcpyHostToDevice);
//	cudaMemcpy((void*)d_currDiag, (void*)currDiag, (lenA+1)*sizeof(unsigned int), cudaMemcpyHostToDevice);

	d_prevprevDiagPtr = d_prevprevDiag;
	d_prevDiagPtr = d_prevDiag;
	d_currDiagPtr = d_currDiag;

	int bDim = 256;
	int gDim = ceil((float)(lenA+1)/bDim);
	int dmin = 2-lenA;
	int dmax = lenB+1;
	int ed;
	for(int d = dmin; d < dmax; d++)
	{
//		int imax = min(lenA+d+1, lenB+1);
//		int gDim = ceil((float)(imax)/bDim);
		editDistKernel<<<gDim, bDim, (bDim)*sizeof(unsigned int)>>>(devA, devB, lenA, lenB, d_prevprevDiagPtr, d_prevDiagPtr, d_currDiagPtr, d);
		cudaDeviceSynchronize();

//		cudaMemcpy((void*)currDiag, (void*)d_currDiagPtr, (lenA+1)*sizeof(unsigned int), cudaMemcpyDeviceToHost);
//		for(int i = 0; i < lenA+1; i++)
//			printf("%d, ", currDiag[i]);
//		printf("\n");
//		printf("end of iteration\n");
		if(d == dmax-1)
		{
			cudaMemcpy((void*)&ed, (void*)&d_currDiagPtr[lenA], 1*sizeof(unsigned int), cudaMemcpyDeviceToHost);
		}
		unsigned int* tmp = d_prevprevDiagPtr;
		d_prevprevDiagPtr = d_prevDiagPtr;
		d_prevDiagPtr = d_currDiagPtr;
		d_currDiagPtr = tmp;
	}
//	cudaMemcpy((void*)currDiag, (void*)d_currDiagPtr, (lenA+1)*sizeof(unsigned int), cudaMemcpyDeviceToHost);

	t2 = chrono::high_resolution_clock::now();
	time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	printf("\nelapsed time: %f\n", time_span.count());
	printf("edit distance: %d\n", ed);
	ed = 0;
	}
	return 0;
}
