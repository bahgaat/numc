#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "../src/matrix_optimized.h"
#include "../src/matrix.h"
#include <stdio.h>


/* Test Suite set_optimizedup and cleanup functions: */
int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

void mul_square_optimized_test(void) {
  matrix_optimized *result = NULL;
  matrix_optimized *mat1 = NULL;
  matrix_optimized *mat2 = NULL;
  allocate_matrix_optimized(&result, 3, 3);
  allocate_matrix_optimized(&mat1, 3, 3);
  allocate_matrix_optimized(&mat2, 3, 3);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      set_optimized(mat1, i, j, i * 3 + j + 1);
      set_optimized(mat2, i, j, i * 3 + j + 1);
    }
  }
  mul_matrix_optimized(result, mat1, mat2);
  get_optimized(result, 0, 0);//, 30);
  get_optimized(result, 0, 1);//, 36);
  get_optimized(result, 0, 2);//, 42);
  get_optimized(result, 1, 0);//, 66);
  get_optimized(result, 1, 1);//, 81);
  get_optimized(result, 1, 2);//, 96);
  get_optimized(result, 2, 0);//, 102);
  get_optimized(result, 2, 1);//, 126);
  get_optimized(result, 2, 2);//, 150);
  deallocate_matrix_optimized(result);
  deallocate_matrix_optimized(mat1);
  deallocate_matrix_optimized(mat2);
}

void mul_non_square_optimized_test_large(void) {
  matrix_optimized *result = NULL;
  matrix_optimized *mat1 = NULL;
  matrix_optimized *mat2 = NULL;
  allocate_matrix_optimized(&result, 3, 4);
  allocate_matrix_optimized(&mat1, 3, 10);
  allocate_matrix_optimized(&mat2, 10, 4);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 10; j++) {
      set_optimized(mat1, i, j, i * 3 + j + 1);
    }
  }
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 4; j++) {
      set_optimized(mat2, i, j, i * 3 + j + 4);
    }
  }
  mul_matrix_optimized(result, mat1, mat2);
  double i = get_optimized(result, 0, 0);
  double y = get_optimized(result, 0, 1);
  double e = get_optimized(result, 0, 2);
  double x = get_optimized(result, 0, 3);
  double t = get_optimized(result, 1, 0);
  double f = get_optimized(result, 1, 1);
  double q = get_optimized(result, 1, 2);
  double s = get_optimized(result, 1, 3);
  double d = get_optimized(result, 2, 0);
  double l = get_optimized(result, 2, 1);
  double o = get_optimized(result, 2, 2);
  double p = get_optimized(result, 2, 3);
  deallocate_matrix_optimized(result);
  deallocate_matrix_optimized(mat1);
  deallocate_matrix_optimized(mat2);
}

void mul_square_optimized_test_double(void) {
  matrix_optimized *result = NULL;
  matrix_optimized *mat1 = NULL;
  matrix_optimized *mat2 = NULL;
  allocate_matrix_optimized(&result, 4, 4);
  allocate_matrix_optimized(&mat1, 4, 4);
  allocate_matrix_optimized(&mat2, 4, 4);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      set_optimized(mat1, i, j, i * 1.2 + j * 2.3);
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      set_optimized(mat2, i, j, i * 1.5 + j * 9.3);
    }
  }
  mul_matrix_optimized(result, mat1, mat2);
  double i = get_optimized(result, 0, 0);
  double x = get_optimized(result, 0, 1); //
  double z = get_optimized(result, 1, 0); //
  double f = get_optimized(result, 1, 1);
  deallocate_matrix_optimized(result);
  deallocate_matrix_optimized(mat1);
  deallocate_matrix_optimized(mat2);
}

void mul_non_square_optimized_test_very_large(void) {
  matrix_optimized *result = NULL;
  matrix_optimized *mat1 = NULL;
  matrix_optimized *mat2 = NULL;
  allocate_matrix_optimized(&result, 5, 4);
  allocate_matrix_optimized(&mat1, 5, 12);
  allocate_matrix_optimized(&mat2, 12, 4);
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 12; j++) {
      set_optimized(mat1, i, j, i * 3 + j - 9);
    }
  }
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 3; j++) {
      set_optimized(mat2, i, j, i * -2 + j + 4);
    }
  }
  mul_matrix_optimized(result, mat1, mat2);
  double i = get_optimized(result, 0, 0); //8
  double y = get_optimized(result, 0, 1); //-34
  double e = get_optimized(result, 0, 2); //-76
  double t = get_optimized(result, 1, 0); //-244
  double f = get_optimized(result, 1, 1); //-250
  double q = get_optimized(result, 1, 2); //-256
  double d = get_optimized(result, 2, 0); //-496
  double l = get_optimized(result, 2, 1); //-466
  double o = get_optimized(result, 2, 2); //-436
  double h = get_optimized(result, 3, 0); //-748
  double iw = get_optimized(result, 3, 1); //-682
  double sw = get_optimized(result, 3, 2); //-616
  double ll = get_optimized(result, 4, 0); //-1000
  double kk = get_optimized(result, 4, 1); //-898
  double vf = get_optimized(result, 4, 2); //-796
  deallocate_matrix_optimized(result);
  deallocate_matrix_optimized(mat1);
  deallocate_matrix_optimized(mat2);
}
/*
void test_runtime_mul(void) {
  matrix_optimized *result_optimized = NULL;
  matrix_optimized *mat1_optimized = NULL;
  matrix_optimized *mat2_optimized = NULL;
  allocate_matrix_optimized(&result_optimized, 2000, 2000);
  allocate_matrix_optimized(&mat1_optimized, 2000, 2000);
  allocate_matrix_optimized(&mat2_optimized, 2000, 2000);
  clock_t start_optimized = clock();
  mul_matrix_optimized(result_optimized, mat1_optimized, mat2_optimized);
  clock_t end_optimized = clock();
  printf("Time taken: %Lf s\n", (long double)(end_optimized - start_optimized)); // data-level parallelism achieved about
  // 9 times the speed of the typical mat
  deallocate_matrix_optimized(result_optimized);
  deallocate_matrix_optimized(mat1_optimized);
  deallocate_matrix_optimized(mat2_optimized);
}
*/



/************* Test Runner Code goes here **************/

int main (void)
{
  mul_square_optimized_test();
  return 0;
}
