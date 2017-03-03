#include <stdio.h>
#include "cuda.h"

#define N 128

int assigned_device;
int used_device;

// Data on the host system
int HostA[N];
int HostB[N];
int HostC[N];

// Pointers to data on the device
int *DeviceA;
int *DeviceB;
int *DeviceC;

//----------------------------------------------------------
__global__ void AddVectors(int* a, int* b, int* c) {
	int i = threadIdx.x;
	c[i] = a[i] + b[i];
}

//----------------------------------------------------------
int main(int argc, char** argv) {

	// Define the device to use:
	if (argc < 2) {
		printf("Error: device number is absent\n");
		return 100;
	}
	assigned_device = atoi(argv[1]);
	if (strlen(argv[1]) > 1 or (assigned_device == 0 and strcmp(argv[1], "0") != 0)) {
		printf("Error: device number is incorrect\n");
		return 110;
	}

	// Select the used device:
	if (cudaSetDevice(assigned_device) != cudaSuccess or
		cudaGetDevice(&used_device) != cudaSuccess or
		used_device != assigned_device
		) {
		printf("Error: unable to set device %d\n", assigned_device);
		return 120;
	}
	printf("Used device: %d\n", used_device);

	// Initialize summands:
	for (int i = 0; i<N; i++) {
		HostA[i] = i * 2;
		HostB[i] = i * 3;
	}

	// Allocate memory on the device:
	cudaMalloc((void**)&DeviceA, N * sizeof(int));
	cudaMalloc((void**)&DeviceB, N * sizeof(int));
	cudaMalloc((void**)&DeviceC, N * sizeof(int));

	// Copy summands from host to device:
	cudaMemcpy(DeviceA, HostA, N * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(DeviceB, HostB, N * sizeof(int), cudaMemcpyHostToDevice);

	// Execute kernel:
	AddVectors << <1, N >> >(DeviceA, DeviceB, DeviceC);

	// Copy result from device to host:
	cudaMemcpy(HostC, DeviceC, N * sizeof(int), cudaMemcpyDeviceToHost);

	// Show result:
	for (int i = 0; i<N; i++) {
		printf("%d + %d = %d\n", HostA[i], HostB[i], HostC[i]);
	}
}

