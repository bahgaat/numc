#include "matrix_optimized.h"

/*
 * Defines the struct that represents the object
 * Has the default PyObject_HEAD so it can be a python object
 * It also has the matrix_optimized that is being wrapped
 * is of type PyObject
 */
typedef struct {
    PyObject_HEAD
    matrix_optimized* mat;
    PyObject *shape;
} matrix_optimized61c;

/* Function definitions */
static int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low, double high);
static int init_fill(PyObject *self, int rows, int cols, double val);
static int init_1d(PyObject *self, int rows, int cols, PyObject *lst);
static int init_2d(PyObject *self, PyObject *lst);
static void matrix_optimized61c_dealloc(matrix_optimized61c *self);
static PyObject *matrix_optimized61c_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int matrix_optimized61c_init(PyObject *self, PyObject *args, PyObject *kwds);
static PyObject *matrix_optimized61c_to_list(matrix_optimized61c *self);
static PyObject *matrix_optimized61c_repr(PyObject *self);
static PyObject *matrix_optimized61c_set_value(matrix_optimized61c *self, PyObject* args);
static PyObject *matrix_optimized61c_get_value(matrix_optimized61c *self, PyObject* args);
static PyObject *matrix_optimized61c_add(matrix_optimized61c* self, PyObject* args);
static PyObject *matrix_optimized61c_sub(matrix_optimized61c* self, PyObject* args);
static PyObject *matrix_optimized61c_multiply(matrix_optimized61c* self, PyObject *args);
static PyObject *matrix_optimized61c_neg(matrix_optimized61c* self);
static PyObject *matrix_optimized61c_abs(matrix_optimized61c *self);
static PyObject *matrix_optimized61c_pow(matrix_optimized61c *self, PyObject *pow, PyObject *optional);
