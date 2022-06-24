#include <Python.h>

typedef struct matrix_optimized {
    int rows; // number of rows
    int cols; // number of columns
    double* data; // pointer to rows * columns doubles
    int ref_cnt; // How many slices/matrices are referring to this matrix_optimized's data
    struct matrix_optimized *parent; // NULL if matrix_optimized is not a slice, else the parent matrix_optimized of the slice
} matrix_optimized;

double rand_double(double low, double high);
void rand_matrix_optimized(matrix_optimized *result, unsigned int seed, double low, double high);
int allocate_matrix_optimized(matrix_optimized **mat, int rows, int cols);
int allocate_matrix_optimized_ref(matrix_optimized **mat, matrix_optimized *from, int offset, int rows, int cols);
void deallocate_matrix_optimized(matrix_optimized *mat);
double get_optimized(matrix_optimized *mat, int row, int col);
void set_optimized(matrix_optimized *mat, int row, int col, double val);
void fill_matrix_optimized(matrix_optimized *mat, double val);
int add_matrix_optimized(matrix_optimized *result, matrix_optimized *mat1, matrix_optimized *mat2);
int sub_matrix_optimized(matrix_optimized *result, matrix_optimized *mat1, matrix_optimized *mat2);
int mul_matrix_optimized(matrix_optimized *result, matrix_optimized *mat1, matrix_optimized *mat2);
int pow_matrix_optimized(matrix_optimized *result, matrix_optimized *mat, int pow);
int neg_matrix_optimized(matrix_optimized *result, matrix_optimized *mat);
int abs_matrix_optimized(matrix_optimized *result, matrix_optimized *mat);
