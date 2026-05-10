#include <cstdio>
#include <cstdlib>

__global__ void clearBucket(int *bucket, int range) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < range)
    bucket[i] = 0;
}

__global__ void fillBucket(int *key, int *bucket, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n)
    atomicAdd(&bucket[key[i]], 1);
}

__global__ void scanBucket(int *bucket, int *offset, int range) {
  if (blockIdx.x == 0 && threadIdx.x == 0) {
    int sum = 0;
    for (int i=0; i<range; i++) {
      offset[i] = sum;
      sum += bucket[i];
    }
  }
}

__global__ void fillKey(int *key, int *bucket, int *offset, int range) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i >= range)
    return;

  int j = offset[i];
  for (int count=0; count<bucket[i]; count++)
    key[j + count] = i;
}

int main() {
  int n = 50;
  int block_size = 256;
  int range = 5;
  int *key, *bucket, *offset;

  cudaMallocManaged(&key, n*sizeof(int));
  cudaMallocManaged(&bucket, range*sizeof(int));
  cudaMallocManaged(&offset, range*sizeof(int));

  for (int i=0; i<n; i++) {
    key[i] = rand() % range;
    printf("%d ",key[i]);
  }
  printf("\n");

  clearBucket<<<(range+block_size-1)/block_size, block_size>>>(bucket, range);
  fillBucket<<<(n+block_size-1)/block_size, block_size>>>(key, bucket, n);
  scanBucket<<<1, 1>>>(bucket, offset, range);
  fillKey<<<(range+block_size-1)/block_size, block_size>>>(key, bucket, offset, range);
  cudaDeviceSynchronize();

  for (int i=0; i<n; i++) {
    printf("%d ",key[i]);
  }
  printf("\n");

  cudaFree(key);
  cudaFree(bucket);
  cudaFree(offset);
}
