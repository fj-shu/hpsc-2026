#include <cstdio>
#include <cstdlib>
#include <vector>
#include <omp.h>

int main() {
  int n = 50;
  int range = 5;
  std::vector<int> key(n);

  for (int i=0; i<n; i++) {
    key[i] = rand() % range;
    printf("%d ", key[i]);
  }
  printf("\n");

  std::vector<int> bucket(range, 0);
  int num_threads = omp_get_max_threads();
  std::vector<std::vector<int>> local_bucket(num_threads, std::vector<int>(range, 0));

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();

    #pragma omp for
    for (int i=0; i<n; i++)
      local_bucket[tid][key[i]]++;
  }

  #pragma omp parallel for
  for (int i=0; i<range; i++) {
    for (int t=0; t<num_threads; t++)
      bucket[i] += local_bucket[t][i];
  }

  std::vector<int> offset(range, 0);
  for (int i=1; i<range; i++) 
    offset[i] = offset[i-1] + bucket[i-1];

  #pragma omp parallel for
  for (int i=0; i<range; i++) {
    int j = offset[i];
    int count = bucket[i];  

    for (int k = 0; k < count; k++) {
      key[j + k] = i;
    }
  }

  for (int i=0; i<n; i++) {
    printf("%d ", key[i]);
  }
  printf("\n");
}
