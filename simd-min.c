// SIMD example without avx512
// docs:
// http://web.archive.org/web/20260114052208/https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// ============================================

#include <immintrin.h>
#pragma GCC target("avx2")

#define TIME_IT(code_block) do { \
    clock_t start = clock(); \
    code_block; \
    clock_t end = clock(); \
    printf("%f sec.\n", (double)(end - start) / CLOCKS_PER_SEC); \
} while(0)

// ============================================

int min(int n, int *a) {
  int mn = INT_MAX;
  for (int i = 0; i < n; i++) {
    if (a[i] < mn) mn = a[i];
  }
  return mn;
}

int min_simd(int n, int *a) {
  __m256i vecmin = _mm256_set1_epi32(INT_MAX);
  
  int i = 0;
  for (; i + 8 <= n; i += 8) {
    __m256i part = _mm256_loadu_si256((__m256i*)(a + i));
    vecmin = _mm256_min_epi32(vecmin, part);
  }
  
  int tmp[8];
  _mm256_storeu_si256((__m256i*)tmp, vecmin);
  
  int mn = INT_MAX;
  for (int k = 0; k < 8; k++) {
    if (tmp[k] < mn) mn = tmp[k];
  }
  
  for (; i < n; i++) {
    if (a[i] < mn) mn = a[i];
  }
  return mn;
}

int main() {
  srand(time(0));
  
  int n;
  scanf("%d", &n);
  
  int *a = malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) a[i] = rand() % 100;
  for (int i = 0; i < n; i++) printf("%d ", a[i]);
  puts("");
  
  TIME_IT ( printf("SISD find: %d\n", min(n, a)); );
  TIME_IT ( printf("SIMD find: %d\n", min_simd(n, a)); );
  
  return 0;
}

