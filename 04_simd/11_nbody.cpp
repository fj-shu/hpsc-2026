#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <immintrin.h>

int main() {
  const int N = 16;
  float x[N], y[N], m[N], fx[N], fy[N];
  for(int i=0; i<N; i++) {
    x[i] = drand48();
    y[i] = drand48();
    m[i] = drand48();
    fx[i] = fy[i] = 0;
  }
  for(int i=0; i<N; i++) {
    __m512 xi = _mm512_set1_ps(x[i]);
    __m512 yi = _mm512_set1_ps(y[i]);
    __m512 xj = _mm512_loadu_ps(x);
    __m512 yj = _mm512_loadu_ps(y);
    __m512 mj = _mm512_loadu_ps(m);

    __m512 rx = _mm512_sub_ps(xi, xj);
    __m512 ry = _mm512_sub_ps(yi, yj);
    __m512 r2 = _mm512_add_ps(_mm512_mul_ps(rx, rx), _mm512_mul_ps(ry, ry));
    __m512 r = _mm512_sqrt_ps(r2);
    __m512 r3 = _mm512_mul_ps(r2, r);

    __mmask16 mask = (__mmask16)(0xffffu ^ (1u << i));
    __m512 inv_r3_m = _mm512_maskz_div_ps(mask, mj, r3);
    __m512 dfx = _mm512_mul_ps(rx, inv_r3_m);
    __m512 dfy = _mm512_mul_ps(ry, inv_r3_m);

    fx[i] -= _mm512_reduce_add_ps(dfx);
    fy[i] -= _mm512_reduce_add_ps(dfy);
    printf("%d %g %g\n",i,fx[i],fy[i]);
  }
}
