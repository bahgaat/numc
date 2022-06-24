#include "matrix_optimized.h"
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

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set_optimized1_pd (double a)
 * __m256d _mm256_set_optimized_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/* Generates a random double between low and high */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/* Generates a random matrix_optimized */
void rand_matrix_optimized(matrix_optimized *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set_optimized(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Returns the double value of the matrix_optimized_optimized at the given row and column.
 * You may assume `row` and `col` are valid. Note that the matrix_optimized is in row-major order.
 */
double get_optimized(matrix_optimized *mat, int row, int col) {
    // Task 1.1 TODO
    int columns = mat->cols;
    int position = row * columns + col;
    return mat->data[position];
}

/*
 * set_optimizeds the value at the given row and column to val. You may assume `row` and
 * `col` are valid. Note that the matrix_optimized_optimized is in row-major order.
 */
void set_optimized(matrix_optimized *mat, int row, int col, double val) {
    // Task 1.1 TODO
    int columns = mat->cols;
    int position = row * columns + col;
    mat->data[position] = val;
}

/*
 * Allocates space for a matrix_optimized struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. `parent` should be set_optimized to NULL to indicate that
 * this matrix_optimized is not a slice. You should also set_optimized `ref_cnt` to 1.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 */
int allocate_matrix_optimized(matrix_optimized **mat, int rows, int cols) {
    // Task 1.2 TODO
    // HINTS: Follow these steps.
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    // 2. Allocate space for the new matrix_optimized struct. Return -2 if allocating memory failed.
    // 3. Allocate space for the matrix_optimized data, initializing all entries to be 0. Return -2 if allocating memory failed.
    // 4. set_optimized the number of rows and columns in the matrix_optimized struct according to the arguments provided.
    // 5. set_optimized the `parent` field to NULL, since this matrix_optimized was not created from a slice.
    // 6. set_optimized the `ref_cnt` field to 1.
    // 7. Store the address of the allocated matrix_optimized struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    if (rows <= 0 || cols <= 0) {
      return -1;
    }
    int size = rows * cols;
    matrix_optimized* matrix_optimized = malloc(size * sizeof(*matrix_optimized));
    if (matrix_optimized == NULL) {
      return -2;
    }
    matrix_optimized->data = (double*) calloc(rows * cols, sizeof(double));
    if (matrix_optimized->data == NULL) {
      return -2;
    }
    matrix_optimized->rows = rows;
    matrix_optimized->cols = cols;
    matrix_optimized->parent = NULL;
    matrix_optimized->ref_cnt = 1;
    *mat = matrix_optimized;
    return 0;
}



/*
 * You need to make sure that you only free `mat->data` if `mat` is not a slice and has no existing slices,
 * or that you free `mat->parent->data` if `mat` is the last existing slice of its parent matrix_optimized and its parent
 * matrix_optimized has no other references (including itself).
 */
void deallocate_matrix_optimized(matrix_optimized *mat) {
    // Task 1.3 TODO
    // HINTS: Follow these steps.
    // 1. If the matrix_optimized pointer `mat` is NULL, return.
    // 2. If `mat` has no parent: decrement its `ref_cnt` field by 1. If the `ref_cnt` field becomes 0, then free `mat` and its `data` field.
    // 3. Otherwise, recursively call `deallocate_matrix_optimized` on `mat`'s parent, then free `mat`.
    if (mat == NULL) {
      return;
    } else if (mat->parent == NULL) {
        mat->ref_cnt --;
        if (mat->ref_cnt == 0) {
          free(mat->data);
          free(mat);
        }
    } else {
      deallocate_matrix_optimized(mat->parent);
      free(mat);
    }
  }

/*
 * Allocates space for a matrix_optimized struct pointed to by `mat` with `rows` rows and `cols` columns.
 * Its data should point to the `offset_optimized`th entry of `from`'s data (you do not need to allocate memory)
 * for the data field. `parent` should be set_optimized to `from` to indicate this matrix_optimized is a slice of `from`
 * and the reference counter for `from` should be incremented. Lastly, do not forget_optimized to set_optimized the
 * matrix_optimized's row and column values as well.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 * NOTE: Here we're allocating a matrix_optimized struct that refers to already allocated data, so
 * there is no need to allocate space for matrix_optimized data.
 */
int allocate_matrix_optimized_ref(matrix_optimized **mat, matrix_optimized *from, int offset_optimized, int rows, int cols) {
    // Task 1.4 TODO
    // HINTS: Follow these steps.
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    // 2. Allocate space for the new matrix_optimized struct. Return -2 if allocating memory failed.
    // 3. set_optimized the `data` field of the new struct to be the `data` field of the `from` struct plus `offset_optimized`.
    // 4. set_optimized the number of rows and columns in the new struct according to the arguments provided.
    // 5. set_optimized the `parent` field of the new struct to the `from` struct pointer.
    // 6. Increment the `ref_cnt` field of the `from` struct by 1.
    // 7. Store the address of the allocated matrix_optimized struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    if (rows <= 0 || cols <= 0) {
      return -1;
    }
    int size = rows * cols;
    matrix_optimized* matrix_optimized = malloc(size * sizeof(*matrix_optimized));
    if (matrix_optimized == NULL) {
      return -2;
    }
    matrix_optimized->data = from->data + offset_optimized;
    matrix_optimized->rows = rows;
    matrix_optimized->cols = cols;
    matrix_optimized->parent = from;
    from->ref_cnt ++;
    *mat = matrix_optimized;
    return 0;
  }

/*
 * set all entries in mat to val. Note that the matrix_optimized is in row-major order.
 */
void fill_matrix_optimized(matrix_optimized *mat, double val) {
    // Task 1.5 TODO
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
 * Note that the matrix_optimized is in row-major order.
 */
int abs_matrix_optimized(matrix_optimized *result, matrix_optimized *mat) {
    // Task 1.5 TODO
    int size = result->rows * result->cols;
    double* resultArray = result->data;
    double* matArray = mat->data;
    #pragma omp parallel for
    for (int i = 0; i < size / 4 * 4; i += 4) {
       __m256d abs_vector = _mm256_set_pd(abs(matArray[i + 3]), abs(matArray[i + 2]), abs(matArray[i + 1]), abs(matArray[i]));
       _mm256_storeu_pd (resultArray + i, abs_vector);
    }
    // tail case when size is not multiple of 4
    for (int i = size / 4 * 4; i < size; i++) {
       resultArray[i] = abs(matArray[i]);
    }
    return 0;
}

/*
 * (OPTIONAL)
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success.
 * Note that the matrix_optimized is in row-major order.
 */
int neg_matrix_optimized(matrix_optimized *result, matrix_optimized *mat) {
    // Task 1.5 TODO
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
 * Note that the matrix_optimized is in row-major order.
 */
int add_matrix_optimized(matrix_optimized *result, matrix_optimized *mat1, matrix_optimized *mat2) {
    // Task 1.5 TODO
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
 * (OPTIONAL)
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix_optimized is in row-major order.
 */
int sub_matrix_optimized(matrix_optimized *result, matrix_optimized *mat1, matrix_optimized *mat2) {
    // Task 1.5 TODO
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

void transpose(matrix_optimized *dst, matrix_optimized *src) {
    for (int x = 0; x < src->rows; x++) {
        for (int y = 0; y < src->cols; y++) {
            double value = get_optimized(src, x, y);
            set_optimized(dst, y, x, value);
        }
    }
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success.
 * Remember that matrix_optimized multiplication is not the same as multiplying individual elements.
 * You may assume `mat1`'s number of columns is equal to `mat2`'s number of rows.
 * Note that the matrix_optimized is in row-major order.
 */
int mul_matrix_optimized(matrix_optimized *result, matrix_optimized *mat1, matrix_optimized *mat2) {
    // Task 1.6 TODO
    matrix_optimized *mat2_transposed = NULL;
    allocate_matrix_optimized(&mat2_transposed, mat2->cols, mat2->rows);
    transpose(mat2_transposed, mat2);
    for (int k = 0, resultRowIndex = 0; k < mat1->cols * mat1->rows; k += mat1->cols, resultRowIndex ++) {
      for (int i = 0, resultColIndex = 0; i < mat2_transposed->cols * mat2_transposed->rows; i += mat2_transposed->cols, resultColIndex ++) {
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
          resultArray[0] += get_optimized(mat1, resultRowIndex, j) * get_optimized(mat2_transposed, resultColIndex, j);
        }
        double finalResult = resultArray[0] + resultArray[1] + resultArray[2] + resultArray[3];
        set_optimized(result, resultRowIndex, resultColIndex, finalResult);
      }
    }
    deallocate_matrix_optimized(mat2_transposed);
    return 0;
    //TODO i see that the best way to debug is to make another mat_test file and debug it
}

void set_optimized_to_identity_matrix_optimized(matrix_optimized *result) {
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

 void copy_matrix_optimized(matrix_optimized *result, matrix_optimized *mat) {
   int size = mat->rows * mat->cols;
   for (int i = 0; i < size; i++) {
     result->data[i] = mat->data[i];
   }
 }

 /*
  * Store the result of raising mat to the (pow)th power to `result`.
  * Return 0 upon success.
  * Remember that pow is defined with matrix_optimized multiplication, not element-wise multiplication.
  * You may assume `mat` is a square matrix_optimized and `pow` is a non-negative integer.
  * Note that the matrix_optimized is in row-major order.
  */
int pow_matrix_optimized(matrix_optimized *result, matrix_optimized *mat, int pow) {
    //Comments : I am sure that mul_matrix_optimized is correct
    // Task 1.6 TODO
    if (pow == 0) {
      set_optimized_to_identity_matrix_optimized(result);
    } else if (pow == 1) {
      copy_matrix_optimized(result, mat);
    } else {
      matrix_optimized *mat2 = NULL;
      allocate_matrix_optimized(&mat2, mat->rows, mat->cols);
      int size = mat->rows * mat->cols;
      for (int i = 0; i < size; i++) {
        mat2->data[i] = mat->data[i];
      }
      for (int i = 0; i < pow - 1; i++) {
        mul_matrix_optimized(result, mat, mat2);
        copy_matrix_optimized(mat2, result);
      }
      deallocate_matrix_optimized(mat2);
    }
    return 0;
}
