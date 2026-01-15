// SIMD example without avx512
// docs:
// http://web.archive.org/web/20260114052208/https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int find(int n, int *a, int needle) {
  for (int i = 0; i < n; i++) {
    if (a[i] == needle) return i;
  }
  return -1;
}

int find_simd(int n, int *a, int needle) {
  __m256i bneedle = _mm256_set1_epi32(needle);
  
  int i = 0;
  for (; i + 7 < n; i += 8) {
    __m256i hspart = _mm256_loadu_si256((__m256i*)(a + i));
    
    #ifdef DEBUG
    printf("hspart[%d] = %d\n", 0, _mm256_extract_epi32(hspart, 0));
    printf("hspart[%d] = %d\n", 1, _mm256_extract_epi32(hspart, 1));
    printf("hspart[%d] = %d\n", 2, _mm256_extract_epi32(hspart, 2));
    printf("hspart[%d] = %d\n", 3, _mm256_extract_epi32(hspart, 3));
    printf("hspart[%d] = %d\n", 4, _mm256_extract_epi32(hspart, 4));
    printf("hspart[%d] = %d\n", 5, _mm256_extract_epi32(hspart, 5));
    printf("hspart[%d] = %d\n", 6, _mm256_extract_epi32(hspart, 6));
    printf("hspart[%d] = %d\n", 7, _mm256_extract_epi32(hspart, 7));
    puts("----");
    #endif
    
    __m256i cmpres = _mm256_cmpeq_epi32(bneedle, hspart);
    int cmpmask = _mm256_movemask_epi8(cmpres);
    
    if (__builtin_popcount(cmpmask)) {
      return i + (__builtin_ctz(cmpmask) >> 2);
    }
  }
  
  for (; i < n; i++) {
    if (a[i] == needle) return i;
  }
  
  return -1;
}

int main() {
  srand(time(0));
  
  int n;
  scanf("%d", &n);
  
  int *a = malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) a[i] = rand() % 100;
  for (int i = 0; i < n; i++) printf("%d ", a[i]);
  puts("");
  
  int needle;
  scanf("%d", &needle);
  
  TIME_IT ( printf("SISD find: %d\n", find(n, a, needle)); );
  TIME_IT ( printf("SIMD find: %d\n", find_simd(n, a, needle)); );
  
  return 0;
}

