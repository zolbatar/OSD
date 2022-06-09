#pragma once

void call_END();
void call_YIELD();
void call_PRINT_NL();
void call_PRINT_Tabbed();
void call_PRINT_TabbedOff();
void call_PRINT_integer(int64_t v);
void call_PRINT_real(double v);
void call_PRINT_string(int64_t idx);
void call_PRINT_SPC(int64_t v);
void call_PRINT_TAB(int64_t v);
double call_MATHS_power(double v1, double v2);
double call_MATHS_ln(double v);
double call_MATHS_log(double v);
double call_MATHS_exp(double v);
double call_MATHS_sin(double v);
double call_MATHS_asn(double v);
double call_MATHS_cos(double v);
double call_MATHS_acs(double v);
double call_MATHS_tan(double v);
double call_MATHS_atn(double v);
double call_MATHS_rad(double v);
double call_MATHS_deg(double v);
int64_t call_MATHS_sgn(double v);
int64_t call_STRING_equal(int64_t s1, int64_t s2);
int64_t call_STRING_notequal(int64_t s1, int64_t s2);
int64_t call_STRING_less(int64_t s1, int64_t s2);
int64_t call_STRING_lessequal(int64_t s1, int64_t s2);
int64_t call_STRING_greater(int64_t s1, int64_t s2);
int64_t call_STRING_greaterequal(int64_t s1, int64_t s2);
int64_t call_STRING_add(int64_t s1, int64_t s2);
int64_t call_STRING_asc(int64_t s);
int64_t call_STRING_chrs(int64_t s);
int64_t call_STRING_instr(int64_t s1, int64_t s2, int64_t s3);
int64_t call_STRING_lefts(int64_t s1, int64_t s2);
int64_t call_STRING_mids(int64_t s1, int64_t s2, int64_t s3);
int64_t call_STRING_rights(int64_t s1, int64_t s2);
int64_t call_STRING_len(int64_t s);
int64_t call_STRING_strings(int64_t s1, int64_t s2);
double call_STRING_stringToFloat(int64_t s);
int64_t call_STRING_stringToInt(int64_t s);
int64_t call_STRING_realtostring(double v);
int64_t call_STRING_inttostring(int64_t v);
void call_STRING_makepermanent(int64_t s);
void call_STRING_freepermanent(int64_t s);

int64_t call_TIME();
int64_t call_TIMES();

int64_t call_2D_screenwidth();
int64_t call_2D_screenheight();
void call_2D_colour(int64_t r, int64_t g, int64_t b);
void call_2D_colourbg(int64_t r, int64_t g, int64_t b);
void call_2D_cls();
void call_2D_plot(int64_t x, int64_t y);
void call_2D_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w);
void call_2D_triangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w);
void call_2D_trianglefilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w);

void call_DATA_integer(int64_t v);
void call_DATA_float(double v);
void call_DATA_string(int64_t v);
int64_t call_READ_integer();
double call_READ_float();
int64_t call_READ_string();
void call_DATA_label(int64_t v);
void call_DATA_restore(int64_t v);

int64_t call_INKEY(int64_t);

int64_t call_RNDI(int64_t upper_bound);
double call_RNDF(double upper_bound);