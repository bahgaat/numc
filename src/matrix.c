#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif


/* Generates a random double between low and high */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/* Generates a random matrix */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Returns the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid. Note that the matrix is in row-major order.
 */
double get(matrix *mat, int row, int col) {
    int columns = mat->cols;
    int position = row * columns + col;
    return mat->data[position];
}

/*
 * set the value at the given row and column to val. You may assume `row` and
 * `col` are valid. Note that the matrix is in row-major order.
 */
void set(matrix *mat, int row, int col, double val) {
    int columns = mat->cols;
    int position = row * columns + col;
    mat->data[position] = val;
}

/*
 * Allocates space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. `parent` should be set to NULL to indicate that
 * this matrix is not a slice. You should also set `ref_cnt` to 1.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
      return -1;
    }
    int size = rows * cols;
    matrix* matrix = malloc(size * sizeof(*matrix));
    if (matrix == NULL) {
      return -2;
    }
    matrix->data = (double*) calloc(rows * cols, sizeof(double));
    if (matrix->data == NULL) {
      return -2;
    }
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->parent = NULL;
    matrix->ref_cnt = 1;
    *mat = matrix;
    return 0;
}



/*
 * You need to make sure that you only free `mat->data` if `mat` is not a slice and has no existing slices,
 * or that you free `mat->parent->data` if `mat` is the last existing slice of its parent matrix and its parent
 * matrix has no other references (including itself).
 */
void deallocate_matrix(matrix *mat) {
    if (mat == NULL) {
      return;
    } else if (mat->parent == NULL) {
        mat->ref_cnt --;
        if (mat->ref_cnt == 0) {
          free(mat->data);
          free(mat);
        }
    } else {
      deallocate_matrix(mat->parent);
      free(mat);
    }
  }

/*
 * Allocates space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * Its data should point to the `offset`th entry of `from`'s data (you do not need to allocate memory)
 * for the data field. `parent` should be set to `from` to indicate this matrix is a slice of `from`
 * and the reference counter for `from` should be incremented. Lastly, do not forget to setthe
 * matrix's row and column values as well.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 * NOTE: Here we're allocating a matrix struct that refers to already allocated data, so
 * there is no need to allocate space for matrix data.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int offset, int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
      return -1;
    }
    int size = rows * cols;
    matrix* matrix = malloc(size * sizeof(*matrix));
    if (matrix == NULL) {
      return -2;
    }
    matrix->data = from->data + offset;
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->parent = from;
    from->ref_cnt ++;
    *mat = matrix;
    return 0;
  }

/*
 * set all entries in mat to val. Note that the matrix is in row-major order.
 */
void fill_matrix(matrix *mat, double val) {
    int size = mat->rows * mat->cols;
    double* array = mat->data;
    __m256d fill_vector =  _mm256_set1_pd (val);
    #pragma omp parallel for
    for (int i = 0; i < size / 4 * 4; i += 4) {
       _mm256_storeu_pd (array + i, fill_vector);
    }
    // tail case when size is not mulitple of 4
    for (int i = size / 4 * 4; i < size; i++) {
      array[i] = val;
    }
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int abs_matrix(matrix *result, matrix *mat) {
    int size = result->rows * result->cols;
    double* resultArray = result->data;
    double* matArray = mat->data;
    __m256d mask = _mm256_set1_pd (-1);
    #pragma omp parallel for
    for (int i = 0; i < size / 4 * 4; i += 4) {
       __m256d vector_matArray = _mm256_loadu_pd (matArray + i);
       __m256d negation = _mm256_mul_pd (vector_matArray , mask);
       __m256d abs_vector = _mm256_max_pd (negation, vector_matArray);
       _mm256_storeu_pd (resultArray + i, abs_vector);
    }
    // tail case when size is not multiple of 4
    for (int i = size / 4 * 4; i < size; i++) {
       resultArray[i] = fabs(matArray[i]);
    }
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int neg_matrix(matrix *result, matrix *mat) {
    int size = result->rows * result->cols;
    double* resultArray = result->data;
    double* matArray = mat->data;
    __m256d negation = _mm256_set1_pd(-1);
    #pragma omp parallel for
    for (int i = 0; i < size / 4 * 4; i += 4) {
      __m256d vector = _mm256_loadu_pd (matArray + i);
      __m256d negated_vector = _mm256_mul_pd (negation, vector);
      _mm256_storeu_pd (resultArray + i, negated_vector);
    }
    // handle tail case
    for (int i = size / 4 * 4; i < size; i++) {
      resultArray[i] = matArray[i] * -1;
    }
    return 0;
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    int size = result->rows * result->cols;
    double* resultArray = result->data;
    double* mat1Array = mat1->data;
    double* mat2Array = mat2->data;
    #pragma omp parallel for
    for (int i = 0; i < size / 4 * 4; i += 4) {
      __m256d vector_mat1Array = _mm256_loadu_pd (mat1Array + i);
      __m256d vector_mat2Array = _mm256_loadu_pd (mat2Array + i);
      __m256d vector_answer = _mm256_add_pd (vector_mat1Array, vector_mat2Array);
      _mm256_storeu_pd (resultArray + i, vector_answer);
    }
    for (int i = size / 4 * 4; i < size; i++) {
      resultArray[i] = mat1Array[i] + mat2Array[i];
    }
    return 0;
}

/*
 *
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    int size = result->rows * result->cols;
    double* resultArray = result->data;
    double* mat1Array = mat1->data;
    double* mat2Array = mat2->data;
    #pragma omp parallel for
    for (int i = 0; i < size / 4 * 4; i += 4) {
      __m256d vector_mat1Array = _mm256_loadu_pd (mat1Array + i);
      __m256d vector_mat2Array = _mm256_loadu_pd (mat2Array + i);
      __m256d vector_answer = _mm256_sub_pd (vector_mat1Array, vector_mat2Array);
      _mm256_storeu_pd (resultArray + i, vector_answer);
    }
    for (int i = size / 4 * 4; i < size; i++) {
      resultArray[i] = mat1Array[i] - mat2Array[i];
    }
    return 0;
}

void transpose(matrix *dst, matrix *src) {
    for (int x = 0; x < src->rows; x++) {
        for (int y = 0; y < src->cols; y++) {
            double value = get(src, x, y);
            set(dst, y, x, value);
        }
    }
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 * You may assume `mat1`'s number of columns is equal to `mat2`'s number of rows.
 * Note that the matrix is in row-major order.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    matrix *mat2_transposed = NULL;
    allocate_matrix(&mat2_transposed, mat2->cols, mat2->rows);
    transpose(mat2_transposed, mat2);
    #pragma omp parallel for
    for (int row = 0; row < result->rows; row ++) {
      int k = row * mat1->cols;
      int i = 0;
      for (int col = 0; col < result->cols; col ++) {
        __m256d value = _mm256_set1_pd(0);
        double resultArray[4];
        for (int j = 0; j < mat1->cols / 4 * 4; j += 4) {
          __m256d mat1_vector = _mm256_loadu_pd (mat1->data + (j + k));
          __m256d mat2_vector = _mm256_loadu_pd (mat2_transposed->data + (j + i));
          __m256d  multipled_vector = _mm256_mul_pd (mat1_vector, mat2_vector);
          value = _mm256_add_pd (value, multipled_vector);
        }
        _mm256_storeu_pd (resultArray, value);
         // Handle tail case
        for (int j = mat1->cols / 4 * 4; j < mat1->cols; j++) {
          resultArray[0] += get(mat1, row, j) * get(mat2_transposed, col, j);
        }
        double finalResult = resultArray[0] + resultArray[1] + resultArray[2] + resultArray[3];
        set(result, row, col, finalResult);
        i += mat2_transposed->cols;
      }
    }
    deallocate_matrix(mat2_transposed);
    return 0;
}

void set_to_identity_matrix(matrix *result) {
   int size = result->rows * result->cols;
   int slide = result->cols + 1;
   for (int i = 0; i < size; i++) {
     if (i % slide == 0) {
       result->data[i] = 1;
     } else {
       result->data[i] = 0;
     }
   }
 }

/* copy data from mat matrix and put it in result matrix */
 void copy_matrix(matrix *result, matrix *mat) {
   int size = mat->rows * mat->cols;
   for (int i = 0; i < size; i++) {
     result->data[i] = mat->data[i];
   }
 }

/*
  * Store the result of raising mat to the (pow)th power to `result`.
  * Return 0 upon success.
  * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
  * You may assume `mat` is a square matrix and `pow` is a non-negative integer.
  * Note that the matrix is in row-major order.
*/
int pow_matrix(matrix *result, matrix *mat, int pow) {
    if (pow == 0) {
      set_to_identity_matrix(result);
    } else if (pow == 1) {
      copy_matrix(result, mat);
    }
    matrix *mat_helper = NULL;
    allocate_matrix(&mat_helper, mat->rows, mat->cols);
    copy_matrix(mat_helper, mat);
    matrix *identity_matrix = NULL;
    allocate_matrix(&identity_matrix, mat->rows, mat->cols);
    set_to_identity_matrix(identity_matrix);
    matrix *identity_matrix_helper = NULL;
    allocate_matrix(&identity_matrix_helper, mat->rows, mat->cols);
    set_to_identity_matrix(identity_matrix_helper);
    while (pow > 1) {
      if (pow % 2 == 0) {
        mul_matrix(result, mat_helper, mat_helper);
        copy_matrix(mat_helper, result);
        pow = pow / 2;
      } else {
        mul_matrix(identity_matrix, identity_matrix_helper, mat_helper);
        copy_matrix(identity_matrix_helper, identity_matrix);
        mul_matrix(result, mat_helper, mat_helper);
        copy_matrix(mat_helper, result);
        pow = (pow - 1) / 2;
      }
    }
    mul_matrix(result, mat_helper, identity_matrix);
    deallocate_matrix(mat_helper);
    deallocate_matrix(identity_matrix);
    deallocate_matrix(identity_matrix_helper);
    return 0;
}
