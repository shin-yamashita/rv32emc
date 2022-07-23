//
// mathsf.h
// 
// Single float (32bit) elementary functions
//  for embedded software
//
// 2016/08
// 組み込み用途向けに演算量を最小限に抑えた初等関数
// 標準ライブラリの float 関数より精度は若干劣る
// また、値域のチェックは行っていないので、注意
// 

float logsf(float x);
float expsf(float x);
float powsf(float x, float y);


float invsqrt(float a);
float sqrtsf(float x);
void  sincossf(float x, float *s, float *c);
float sinsf(float x);
float cossf(float x);


