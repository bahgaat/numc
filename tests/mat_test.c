#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "../src/matrix.h"
#include <stdio.h>

/* Test Suite setup and cleanup functions: */
int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/

void get_test(void) {
  matrix *mat = NULL;
  allocate_matrix(&mat, 2, 2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat, i, j, i * 2 + j);
    }
  }
  CU_ASSERT_EQUAL(get(mat, 0, 0), 0);
  CU_ASSERT_EQUAL(get(mat, 0, 1), 1);
  CU_ASSERT_EQUAL(get(mat, 1, 0), 2);
  CU_ASSERT_EQUAL(get(mat, 1, 1), 3);
  deallocate_matrix(mat);
}

void set_test(void) {
  matrix *mat = NULL;
  allocate_matrix(&mat, 2, 2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat, i, j, i * 2 + j);
    }
  }
  CU_ASSERT_EQUAL(get(mat, 0, 0), 0);
  CU_ASSERT_EQUAL(get(mat, 0, 1), 1);
  CU_ASSERT_EQUAL(get(mat, 1, 0), 2);
  CU_ASSERT_EQUAL(get(mat, 1, 1), 3);
  deallocate_matrix(mat);
}

void alloc_fail_test(void) {
  matrix *mat = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 0, 0), -1);
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 0, 1), -1);
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 1, 0), -1);
}

void alloc_success_test(void) {
  matrix *mat = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 3, 2), 0);
  CU_ASSERT_EQUAL(mat->parent, NULL);
  CU_ASSERT_EQUAL(mat->ref_cnt, 1);
  CU_ASSERT_EQUAL(mat->rows, 3);
  CU_ASSERT_EQUAL(mat->cols, 2);
  CU_ASSERT_NOT_EQUAL(mat->data, NULL);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      CU_ASSERT_EQUAL(get(mat, i, j), 0);
    }
  }
  deallocate_matrix(mat);
}

void alloc_ref_fail_test(void) {
  matrix *mat = NULL;
  matrix *from = NULL;
  CU_ASSERT_EQUAL(allocate_matrix_ref(&mat, from, 0, 0, 0), -1);
  CU_ASSERT_EQUAL(allocate_matrix_ref(&mat, from, 0, 0, 1), -1);
  CU_ASSERT_EQUAL(allocate_matrix_ref(&mat, from, 0, 1, 0), -1);
}

void alloc_ref_success_test(void) {
  matrix *mat = NULL;
  matrix *from = NULL;
  allocate_matrix(&from, 3, 2);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      set(from, i, j, i * 2 + j);
    }
  }
  CU_ASSERT_EQUAL(allocate_matrix_ref(&mat, from, 2, 2, 2), 0);
  CU_ASSERT_PTR_EQUAL(mat->data, from->data + 2);
  CU_ASSERT_PTR_EQUAL(mat->parent, from);
  CU_ASSERT_EQUAL(mat->parent->ref_cnt, 2);
  CU_ASSERT_EQUAL(mat->rows, 2);
  CU_ASSERT_EQUAL(mat->cols, 2);
  deallocate_matrix(from);
  deallocate_matrix(mat);
}

void dealloc_null_test(void) {
  matrix *mat = NULL;
  deallocate_matrix(mat); // Test the null case doesn't crash
}

void add_test(void) {
  matrix *result = NULL;
  matrix *mat1 = NULL;
  matrix *mat2 = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&result, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat1, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat2, 2, 2), 0);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat1, i, j, i * 2 + j);
      set(mat2, i, j, i * 2 + j);
    }
  }
  //clock_t start = clock();
  add_matrix(result, mat1, mat2);
  //clock_t end = clock();
  //printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      CU_ASSERT_EQUAL(get(result, i, j), 2 * (i * 2 + j));
    }
  }
  deallocate_matrix(result);
  deallocate_matrix(mat1);
  deallocate_matrix(mat2);
}
/*
void test_runtime_mul(void) {
    matrix *result = NULL;
    matrix *mat1 = NULL;
    matrix *mat2 = NULL;
    allocate_matrix(&result, 2000, 2000);
    allocate_matrix(&mat1, 2000, 2000);
    allocate_matrix(&mat2, 2000, 2000);
    clock_t start = clock();
    mul_matrix(result, mat1, mat2);
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) );
    deallocate_matrix(result);
    deallocate_matrix(mat1);
    deallocate_matrix(mat2);
}
*/


void sub_test(void) {
  matrix *result = NULL;
  matrix *mat1 = NULL;
  matrix *mat2 = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&result, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat1, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat2, 2, 2), 0);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat1, i, j, i * 2 + j);
      set(mat2, i, j, (i * 2 + j) * 3);
    }
  }
  sub_matrix(result, mat1, mat2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      CU_ASSERT_EQUAL(get(result, i, j), (-2) * (i * 2 + j));
    }
  }
  deallocate_matrix(result);
  deallocate_matrix(mat1);
  deallocate_matrix(mat2);
}


void neg_test(void) {
  matrix *result = NULL;
  matrix *mat = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&result, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 2, 2), 0);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat, i, j, i * 2 + j);
    }
  }
  neg_matrix(result, mat);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      CU_ASSERT_EQUAL(get(result, i, j), -(i * 2 + j));
    }
  }
  deallocate_matrix(result);
  deallocate_matrix(mat);
}

void abs_test(void) {
  matrix *result = NULL;
  matrix *mat = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&result, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 2, 2), 0);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      if (j % 2 == 0)
        set(mat, i, j, i * 2 + j);
      else
        set(mat, i, j, -(i * 2 + j));
    }
  }
  abs_matrix(result, mat);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      CU_ASSERT_EQUAL(get(result, i, j), i * 2 + j);
    }
  }
  deallocate_matrix(result);
  deallocate_matrix(mat);
}

void mul_square_test(void) {
  matrix *result = NULL;
  matrix *mat1 = NULL;
  matrix *mat2 = NULL;
  allocate_matrix(&result, 3, 3);
  allocate_matrix(&mat1, 3, 3);
  allocate_matrix(&mat2, 3, 3);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      set(mat1, i, j, i * 3 + j + 1);
      set(mat2, i, j, i * 3 + j + 1);
    }
  }
  mul_matrix(result, mat1, mat2);
  get(result, 0, 0);//, 30);
  get(result, 0, 1);//, 36);
  get(result, 0, 2);// 42);
  get(result, 1, 0);//66);
  get(result, 1, 1);//, 81);
  get(result, 1, 2);// 96);
  get(result, 2, 0);// 102);
  get(result, 2, 1);// 126);
  get(result, 2, 2);// 150);
  deallocate_matrix(result);
  deallocate_matrix(mat1);
  deallocate_matrix(mat2);
}

void mul_square_test2(void) {
  matrix *result = NULL;
  matrix *mat1 = NULL;
  matrix *mat2 = NULL;
  allocate_matrix(&result, 2, 2);
  allocate_matrix(&mat1, 2, 2);
  allocate_matrix(&mat2, 2, 2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat1, i, j, i * 2 + 9);
    }
  }
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat2, i, j, i * 3 + 6 + 1);
    }
  }
  mul_matrix(result, mat1, mat2);
  int i = get(result, 0, 0);
  int x = get(result, 0, 1); //
  int z = get(result, 1, 0); //
  int f = get(result, 1, 1);
  deallocate_matrix(result);
  deallocate_matrix(mat1);
  deallocate_matrix(mat2);
}

void mul_non_square_test(void) {
  matrix *result = NULL;
  matrix *mat1 = NULL;
  matrix *mat2 = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&result, 3, 3), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat1, 3, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat2, 2, 3), 0);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      set(mat1, i, j, i * 2 + j + 1);
    }
  }
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      set(mat2, i, j, i * 3 + j + 1);
    }
  }
  mul_matrix(result, mat1, mat2);
  CU_ASSERT_EQUAL(get(result, 0, 0), 9);
  CU_ASSERT_EQUAL(get(result, 0, 1), 12);
  CU_ASSERT_EQUAL(get(result, 0, 2), 15);
  CU_ASSERT_EQUAL(get(result, 1, 0), 19);
  CU_ASSERT_EQUAL(get(result, 1, 1), 26);
  CU_ASSERT_EQUAL(get(result, 1, 2), 33);
  CU_ASSERT_EQUAL(get(result, 2, 0), 29);
  CU_ASSERT_EQUAL(get(result, 2, 1), 40);
  CU_ASSERT_EQUAL(get(result, 2, 2), 51);
  deallocate_matrix(result);
  deallocate_matrix(mat1);
  deallocate_matrix(mat2);
}

void mul_additional_test(void) {
    matrix *result = NULL;
    matrix *mat1 = NULL;
    CU_ASSERT_EQUAL(allocate_matrix(&result, 2, 2), 0);
    CU_ASSERT_EQUAL(allocate_matrix(&mat1, 2, 2), 0);
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        if (i == 1 && j == 1) {
          set(mat1, i, j, 0);
        } else {
          set(mat1, i, j, 1);
        }
      }
    }
    mul_matrix(result, mat1, mat1);
    CU_ASSERT_EQUAL(get(result, 0, 0), 2);
    CU_ASSERT_EQUAL(get(result, 0, 1), 1);
    CU_ASSERT_EQUAL(get(result, 0, 2), 1);
    CU_ASSERT_EQUAL(get(result, 1, 0), 1);
    deallocate_matrix(result);
    deallocate_matrix(mat1);
}

void pow_test(void) {
  matrix *result = NULL;
  matrix *mat = NULL;
  CU_ASSERT_EQUAL(allocate_matrix(&result, 2, 2), 0);
  CU_ASSERT_EQUAL(allocate_matrix(&mat, 2, 2), 0);
  set(mat, 0, 0, 1);
  set(mat, 0, 1, 1);
  set(mat, 1, 0, 1);
  set(mat, 1, 1, 0);
  pow_matrix(result, mat, 2);
  CU_ASSERT_EQUAL(get(result, 0, 0), 2);
  CU_ASSERT_EQUAL(get(result, 0, 1), 1);
  CU_ASSERT_EQUAL(get(result, 1, 0), 1);
  CU_ASSERT_EQUAL(get(result, 1, 1), 1);
  pow_matrix(result, mat, 3);
  CU_ASSERT_EQUAL(get(result, 0, 0), 3);
  CU_ASSERT_EQUAL(get(result, 0, 1), 2);
  CU_ASSERT_EQUAL(get(result, 1, 0), 2);
  CU_ASSERT_EQUAL(get(result, 1, 1), 1);
  pow_matrix(result, mat, 10);
  CU_ASSERT_EQUAL(get(result, 0, 0), 89);
  CU_ASSERT_EQUAL(get(result, 0, 1), 55);
  CU_ASSERT_EQUAL(get(result, 1, 0), 55);
  CU_ASSERT_EQUAL(get(result, 1, 1), 34);
  deallocate_matrix(result);
  deallocate_matrix(mat);
}


void mul_non_square_test_large(void) {
  matrix *result = NULL;
  matrix *mat1 = NULL;
  matrix *mat2 = NULL;
  allocate_matrix(&result, 3, 4);
  allocate_matrix(&mat1, 3, 10);
  allocate_matrix(&mat2, 10, 4);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 10; j++) {
      set(mat1, i, j, i * 3 + j + 1);
    }
  }
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 4; j++) {
      set(mat2, i, j, i * 3 + j + 4);
    }
  }
  mul_matrix(result, mat1, mat2);
  double i = get(result, 0, 0);
  double y = get(result, 0, 1);
  double e = get(result, 0, 2);
  double x = get(result, 0, 3);
  double t = get(result, 1, 0);
  double f = get(result, 1, 1);
  double q = get(result, 1, 2);
  double s = get(result, 1, 3);
  double d = get(result, 2, 0);
  double l = get(result, 2, 1);
  double o = get(result, 2, 2);
  double p = get(result, 2, 3);
  deallocate_matrix(result);
  deallocate_matrix(mat1);
  deallocate_matrix(mat2);
}

/************* Test Runner Code goes here **************/

int main (void)
{
  mul_square_test();
}
