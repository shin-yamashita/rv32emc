//
// rcp.c
// float reciprocal, inv sqrt
//


//---------------------------------------------------------
typedef union {float f; unsigned u;} fu_t;

float rcp(float x)      // reciprocal  return 1/x
{
        unsigned id = 0x7f000000 - ((fu_t)x).u;  // initial approx
        float y = ((fu_t)id).f;

        // Newton-Raphson Yn+1 = 2*Yn - X * Y^2
        y = y * (2.0f - x * y);         // iter 1
        return y * (2.0f - x * y);      // iter 2
}

/*----
float invsqrt(float a)
{
    unsigned ix = (0xbe800000 - ((fu_t)a).u) >> 1;      // initial approx
    float x = ((fu_t)ix).f;

    // Newton-Raphson Xn+1 = Xn * (3 - A * Xn^2) / 2
    x = x * (3.0f - a * x * x) / 2;
    x = x * (3.0f - a * x * x) / 2;
//    x = x * (3.0f - a * x * x) / 2;
    return x;
}
----*/
//---------------------------------------------------------

