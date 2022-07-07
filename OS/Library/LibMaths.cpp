#include "../../Compiler/NativeCompiler/NativeCompiler.h"
#include "math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /**< pi */
#endif

double call_MATHS_power(double v1, double v2)
{
    return pow(v2, v1);
}

double call_MATHS_ln(double v)
{
    return log(v);
}

double call_MATHS_log(double v)
{
    return log10(v);
}

double call_MATHS_exp(double v)
{
    return exp(v);
}

double call_MATHS_sin(double v)
{
    return sin(v);
}

double call_MATHS_asn(double v)
{
    return asin(v);
}

double call_MATHS_cos(double v)
{
    return cos(v);
}

double call_MATHS_acs(double v)
{
    return acos(v);
}

double call_MATHS_tan(double v)
{
    return tan(v);
}

double call_MATHS_atn(double v)
{
    return atan(v);
}

double call_MATHS_rad(double v)
{
    return (v * M_PI) / 180.0;
}

double call_MATHS_deg(double v)
{
    return (v * 180.0) / M_PI;
}

int64_t call_MATHS_sgn(double v)
{
    if (v < 0.0)
        return -1;
    else if (v == 0.0)
        return 0;
    else
        return 1;
}
