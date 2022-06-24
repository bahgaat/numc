#include "numc_optimized.h"
#include <structmember.h>

// numc_optimized.c mainly handles possible errors and unpacking the variables to then later use the functions
// from matrix_optimized.c. To get a better understanding of the Python-C interface function calls that are used
// in the code below, have a look at the spec.

static PyTypeObject matrix_optimized61cType;

/* Below are some helper functions for throwing errors */
static int number_methods_err(const char *op, PyObject* args, matrix_optimized61c *self, matrix_optimized61c *other) {
    /* PyObject_TypeCheck returns True if args is a subtype of matrix_optimized61cType */
    char err_msg[200];
    /* First checks t make sure self is a subtype of matrix_optimized 61c type */
    if (!PyObject_TypeCheck(self, &matrix_optimized61cType)) {
        sprintf(err_msg, "numc_optimized.matrix_optimized does not support %s with other types", op);
        PyErr_SetString(PyExc_TypeError, err_msg);
        return 1;
    }
    /* Secondly checks to make sure the args are a subtype of matrix_optimized61c Type*/
    if (!PyObject_TypeCheck(args, &matrix_optimized61cType)) {
        /* Unpack the tuple into other*/
        if (!PyArg_ParseTuple(args, "O", &other)) {
            sprintf(err_msg, "numc_optimized.matrix_optimized does not support %s with other types", op);
            PyErr_SetString(PyExc_TypeError, err_msg);
            return 1;
        }
        /* check to make sure that other is a sutype of matrix_optimized61cType */
        if (!PyObject_TypeCheck(other, &matrix_optimized61cType)) {
            sprintf(err_msg, "numc_optimized.matrix_optimized does not support %s with other types", op);
            PyErr_SetString(PyExc_TypeError, err_msg);
            return 1;
        }
        return 0;
    }
    return 0;
}

/* Helper function to either create a new matrix_optimized61C object with the given new_mat matrix_optimized if op_result is non negative */
static PyObject *op_err(matrix_optimized *new_mat, int op_result){
    if (op_result < 0) {
        deallocate_matrix_optimized(new_mat);
        return NULL;
    } else {
        matrix_optimized61c* rv = (matrix_optimized61c*) matrix_optimized61c_new(&matrix_optimized61cType, NULL, NULL);
        rv->mat = new_mat;
        rv->shape = PyTuple_Pack(2, PyLong_FromLong(new_mat->rows), PyLong_FromLong(new_mat->cols));
        return (PyObject*)rv;
    }
}


/* Helper functions for initalization of matrices and vectors */
/* matrix_optimized(rows, cols, low, high). Fill a matrix_optimized random double values */
static int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low, double high) {
    matrix_optimized *new_mat;
    int alloc_failed = allocate_matrix_optimized(&new_mat, rows, cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return alloc_failed;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return alloc_failed;
    }
    rand_matrix_optimized(new_mat, seed, low, high);
    ((matrix_optimized61c *)self)->mat = new_mat;
    ((matrix_optimized61c *)self)->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    return 0;
}

/* matrix_optimized(rows, cols, val). Fill a matrix_optimized of dimension rows * cols with val*/
static int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix_optimized *new_mat;
    int alloc_failed = allocate_matrix_optimized(&new_mat, rows, cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return alloc_failed;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return alloc_failed;
    }
    else {
        fill_matrix_optimized(new_mat, val);
        ((matrix_optimized61c *)self)->mat = new_mat;
        ((matrix_optimized61c *)self)->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }
    return 0;
}

/* matrix_optimized(rows, cols, 1d_list). Fill a matrix_optimized with dimension rows * cols with 1d_list values */
static int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_TypeError, "Incorrect number of elements in list");
        return -1;
    }
    matrix_optimized *new_mat;
    int alloc_failed = allocate_matrix_optimized(&new_mat, rows, cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return alloc_failed;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return alloc_failed;
    }
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set_optimized(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((matrix_optimized61c *)self)->mat = new_mat;
    ((matrix_optimized61c *)self)->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    return 0;
}

/* matrix_optimized(2d_list). Fill a matrix_optimized with dimension len(2d_list) * len(2d_list[0]) */
static int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_TypeError, "Cannot initialize numc_optimized.matrix_optimized with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_TypeError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) || PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_TypeError, "List values not valid");
            return -1;
        }
    }
    matrix_optimized *new_mat;
    int alloc_failed = allocate_matrix_optimized(&new_mat, rows, cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return alloc_failed;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return alloc_failed;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set_optimized(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((matrix_optimized61c *)self)->mat = new_mat;
    ((matrix_optimized61c *)self)->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    return 0;
}

/* This deallocation function is called when reference count is 0*/
static void matrix_optimized61c_dealloc(matrix_optimized61c *self) {
    deallocate_matrix_optimized(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
static PyObject *matrix_optimized61c_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    matrix_optimized61c *self = (matrix_optimized61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/* This matrix_optimized61c type is mutable, so needs init function. Return 0 on success otherwise -1 */
static int matrix_optimized61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low, double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3) || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            }
            else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* matrix_optimized(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* matrix_optimized(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* matrix_optimized(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/* List of lists representations for matrices */
static PyObject *matrix_optimized61c_to_list(matrix_optimized61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = PyList_New(rows);
    for (int i = 0; i < rows; i++) {
        PyList_SetItem(py_lst, i, PyList_New(cols));
        PyObject *curr_row = PyList_GetItem(py_lst, i);
        for (int j = 0; j < cols; j++) {
            PyList_SetItem(curr_row, j, PyFloat_FromDouble(get_optimized(self->mat, i, j)));
        }
    }
    return py_lst;
}

/* Class function which acts as a helper function to matrix_optimized61c_to_list */
static PyObject *matrix_optimized61c_class_to_list(matrix_optimized61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &matrix_optimized61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc_optimized.matrix_optimized!");
            return NULL;
        }
        matrix_optimized61c* mat61c = (matrix_optimized61c*)mat;
        return matrix_optimized61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}


/* Add class methods */
static PyMethodDef matrix_optimized61c_class_methods[] = {
    {"to_list", (PyCFunction)matrix_optimized61c_class_to_list, METH_VARARGS, "Returns a list representation of numc_optimized.matrix_optimized"},
    {NULL, NULL, 0, NULL}
};


/* matrix_optimized61c string representation. For printing purposes. */
static PyObject *matrix_optimized61c_repr(PyObject *self) {
    PyObject *py_lst = matrix_optimized61c_to_list((matrix_optimized61c *)self);
    return PyObject_Repr(py_lst);
}

/* For __getitem__. (e.g. mat[0])
Uses allocate_matrix_optimized_ref to return a new matrix_optimized from the original as a slice if we are dealing with multiple dimension matrix_optimized
*/
static PyObject *matrix_optimized61c_subscript(matrix_optimized61c* self, PyObject* key) {
	if (!PyLong_Check(key)) {
		PyErr_SetString(PyExc_TypeError, "Key is not valid");
        return NULL;
	}
	int index = PyLong_AsLong(key);
	if (index >= self->mat->rows || index < 0) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
        return NULL;
	}
	matrix_optimized *new_mat;
    int ref_failed = allocate_matrix_optimized_ref(&new_mat, self->mat, index * self->mat->cols, self->mat->cols, 1);
    if (ref_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return NULL;
    }else if (ref_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return NULL;
    }
    matrix_optimized61c* rv = (matrix_optimized61c*) matrix_optimized61c_new(&matrix_optimized61cType, NULL, NULL);
    rv->mat = new_mat;
    rv->shape = PyTuple_Pack(2, PyLong_FromLong(new_mat->rows), PyLong_FromLong(1));
    if (new_mat->rows == 1) { // if one single number, unwrap from list
    	return PyFloat_FromDouble(new_mat->data[0]);
    }
    return (PyObject*)rv;
}

/* For __setitem__ (e.g. mat[0] = 1) */
static int matrix_optimized61c_set_subscript(matrix_optimized61c* self, PyObject *key, PyObject *v) {
	if (!PyLong_Check(key)) {
		PyErr_SetString(PyExc_TypeError, "Key is not valid");
        return -1;
	}
	int index = PyLong_AsLong(key);
	if (index >= self->mat->rows || index < 0) {
		PyErr_SetString(PyExc_IndexError, "Index out of range");
        return -1;
	}
	int cols = self->mat->cols;
	if (cols == 1) {
		if (!PyFloat_Check(v) && !PyLong_Check(v)) {
			PyErr_SetString(PyExc_TypeError, "Value is not valid");
	        return -1;
		}
		double val = PyFloat_AsDouble(v);
		set_optimized(self->mat, index, 0, val);
		return 0;
	} else {
		if (!PyList_Check(v)) {
			PyErr_SetString(PyExc_TypeError, "Value is not valid");
	        return -1;
		}
		for (int i = 0; i < cols; i++) {
			if (!PyFloat_Check(PyList_GetItem(v, i)) && !PyLong_Check(PyList_GetItem(v, i))) {
				PyErr_SetString(PyExc_TypeError, "Value is not valid");
	        	return -1;
			}
			set_optimized(self->mat, index, i, PyFloat_AsDouble(PyList_GetItem(v, i)));
		}
		return 0;
	}
	return -1;
}

static PyMappingMethods matrix_optimized61c_mapping = {
	NULL,
	(binaryfunc) matrix_optimized61c_subscript,
	(objobjargproc) matrix_optimized61c_set_subscript,
};


/*
 * Adds two numc_optimized.matrix_optimized (matrix_optimized61c) objects together. The first operand is self, and
 * the second operand can be obtained by casting `args`.
 */
static PyObject *matrix_optimized61c_add(matrix_optimized61c* self, PyObject* args) {
    matrix_optimized61c* other = NULL;
    int args_invalid = number_methods_err("+", args, self, other);
    if (args_invalid)
        return NULL;
    else {
        other = (matrix_optimized61c*)args;
    }
    matrix_optimized *new_mat;
    matrix_optimized *mat1 = self->mat;
    matrix_optimized *mat2 = other->mat;
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        PyErr_SetString(PyExc_ValueError, "Arguments' dimensions invalid");
        return NULL;
    }
    int alloc_failed = allocate_matrix_optimized(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return NULL;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return NULL;
    }

    int add_result = add_matrix_optimized(new_mat, self->mat, other->mat);
    return op_err(new_mat, add_result);
}

/*
 * Subtracts the second numc_optimized.matrix_optimized (matrix_optimized61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
static PyObject *matrix_optimized61c_sub(matrix_optimized61c* self, PyObject* args) {
    matrix_optimized61c* other = NULL;
    int args_invalid = number_methods_err("-", args, self, other);
    if (args_invalid)
        return NULL;
    else {
        other = (matrix_optimized61c*)args;
    }

    matrix_optimized *new_mat;
    matrix_optimized *mat1 = self->mat;
    matrix_optimized *mat2 = other->mat;
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        PyErr_SetString(PyExc_ValueError, "Arguments' dimensions invalid");
        return NULL;
    }
    int alloc_failed = allocate_matrix_optimized(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return NULL;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return NULL;
    }
    int sub_result = sub_matrix_optimized(new_mat, self->mat, other->mat);
    return op_err(new_mat, sub_result);
}

/*
 * Multiplies two numc_optimized.matrix_optimized (matrix_optimized61c) objects together. The first operand is self, and
 * the second operand can be obtained by casting `args`.
 */
static PyObject *matrix_optimized61c_multiply(matrix_optimized61c* self, PyObject *args) {
    matrix_optimized61c* other = NULL;
    int args_invalid = number_methods_err("*", args, self, other);
    if (args_invalid)
        return NULL;
    else {
        other = (matrix_optimized61c*)args;
    }

    matrix_optimized *new_mat;
    matrix_optimized *mat1 = self->mat;
    matrix_optimized *mat2 = other->mat;
    if (mat1->cols != mat2->rows) {
        PyErr_SetString(PyExc_ValueError, "Arguments' dimensions invalid");
        return NULL;
    }
    int alloc_failed = allocate_matrix_optimized(&new_mat, self->mat->rows, other->mat->cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return NULL;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return NULL;
    }
    int mul_result = mul_matrix_optimized(new_mat, self->mat, other->mat);
    return op_err(new_mat, mul_result);
}

/*
 * Negates the given numc_optimized.matrix_optimized (matrix_optimized61c).
 */
static PyObject *matrix_optimized61c_neg(matrix_optimized61c* self) {
    matrix_optimized *new_mat;
    int alloc_failed = allocate_matrix_optimized(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return NULL;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return NULL;
    }
    int neg_result = neg_matrix_optimized(new_mat, self->mat);
    return op_err(new_mat, neg_result);
}

/*
 * Take the element-wise absolute value of this numc_optimized.matrix_optimized (matrix_optimized61c).
 */
static PyObject *matrix_optimized61c_abs(matrix_optimized61c *self) {
    matrix_optimized *new_mat;
    int alloc_failed = allocate_matrix_optimized(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed == -1){
        PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
        return NULL;
    }else if (alloc_failed == -2){
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
        return NULL;
    }
    int abs_result = abs_matrix_optimized(new_mat, self->mat);
    return op_err(new_mat, abs_result);
}

/*
 * Raise numc_optimized.matrix_optimized (matrix_optimized61c) to the `pow`th power. You can ignore the argument `optional`.
 */
static PyObject *matrix_optimized61c_pow(matrix_optimized61c *self, PyObject *pow, PyObject *optional) {
    if (self->mat->rows != self->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "matrix_optimized must be square");
        return NULL;
    }
    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "Exponent must be of type integer");
        return NULL;
    } else {
        int pow_c = PyLong_AsLong(pow);
        if (pow_c < 0) {
            PyErr_SetString(PyExc_ValueError, "Exponent must be positive");
            return NULL;
        }
        matrix_optimized *new_mat;
        int alloc_failed = allocate_matrix_optimized(&new_mat, self->mat->rows, self->mat->cols);
        if (alloc_failed == -1){
            PyErr_SetString(PyExc_ValueError, "Dimensions must be positive");
            return NULL;
        }else if (alloc_failed == -2){
            PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix_optimized");
            return NULL;
        }
        int pow_result = pow_matrix_optimized(new_mat, self->mat, pow_c);
        return op_err(new_mat, pow_result);
    }
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods
 * defined as above.
 * You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
static PyNumberMethods matrix_optimized61c_as_number = {
   .nb_add = (binaryfunc)matrix_optimized61c_add, // binaryfunc nb_add;
   .nb_subtract = (binaryfunc)matrix_optimized61c_sub, // binaryfunc nb_subtract;
   .nb_multiply = (binaryfunc)matrix_optimized61c_multiply, // binaryfunc nb_multiply;
   .nb_power = (ternaryfunc)matrix_optimized61c_pow, // ternaryfunc nb_power;
   .nb_negative = (unaryfunc)matrix_optimized61c_neg, // unaryfunc nb_negative;
   .nb_absolute = (unaryfunc)matrix_optimized61c_abs, // unaryfunc nb_absolute;
};


/* INSTANCE METHODS */
/*
 * Given a numc_optimized.matrix_optimized self, parse `args` to (int) row, (int) col, and (double) val.
 * Throw a type error if the number of arguments parsed from args is not 3 or if the arguments
 * are of the wrong types. Throw an index error if the parsed row and col are out of range.
 * This function should return None in Python.
 */
static PyObject *matrix_optimized61c_set_value(matrix_optimized61c *self, PyObject* args) {
    int row, col;
    double val;
    if (!PyArg_ParseTuple(args, "iid", &row, &col, &val)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
    if (row < 0 || col < 0 || row >= self->mat->rows || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "row or column index out of range");
        return NULL;
    }
    set_optimized(self->mat, row, col, val);
    return Py_BuildValue("");
}


/*
 * Given a numc_optimized.matrix_optimized `self`, parse `args` to (int) row, (int) col, and (double) val.
 * Throw a type error if the number of arguments parsed from args is not 3 or if the arguments
 * are of the wrong types. Throw an index error if the parsed row and col are out of range.
 * This function should return the value at the `row`th row and `col`th column, which is a Python
 * float.
 */
static PyObject *matrix_optimized61c_get_value(matrix_optimized61c *self, PyObject* args) {
    int row, col;
    if (!PyArg_ParseTuple(args, "ii", &row, &col)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
    if (row < 0 || col < 0 || row >= self->mat->rows || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "row or column index out of range");
        return NULL;
    }
    return PyFloat_FromDouble(get_optimized(self->mat, row, col));
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to matrix_optimized61c_get_value as "get" and matrix_optimized61c_set_value
 * as "set".
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
static PyMethodDef matrix_optimized61c_methods[] = {
    {"set", (PyCFunction)matrix_optimized61c_set_value, METH_VARARGS,
    "Change the value at a specific row and column index"},
    {"get", (PyCFunction)matrix_optimized61c_get_value, METH_VARARGS,
    "Get the value at a specific row and column index"},
    {NULL}  /* Sentinel */
};

/* INSTANCE ATTRIBUTES*/
static PyMemberDef matrix_optimized61c_members[] = {
    {"shape", T_OBJECT_EX, offsetof(matrix_optimized61c, shape), 0,
     "(rows, cols)"},
    {NULL}  /* Sentinel */
};
/* INSTANCE ATTRIBUTES */
static PyTypeObject matrix_optimized61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc_optimized.matrix_optimized",
    .tp_basicsize = sizeof(matrix_optimized61c),
    .tp_dealloc = (destructor)matrix_optimized61c_dealloc,
    .tp_repr = (reprfunc)matrix_optimized61c_repr,
    .tp_as_number = &matrix_optimized61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc_optimized.matrix_optimized objects",
    .tp_methods = matrix_optimized61c_methods,
    .tp_members = matrix_optimized61c_members,
    .tp_as_mapping = &matrix_optimized61c_mapping,
    .tp_init = (initproc)matrix_optimized61c_init,
    .tp_new = matrix_optimized61c_new
};

/* Information about the Cython Module */
static struct PyModuleDef numc_optimizedmodule = {
    PyModuleDef_HEAD_INIT,
    "numc_optimized",
    "numc_optimized matrix_optimized operations",
    -1,
    matrix_optimized61c_class_methods
};

/* Initialize the numc_optimized module */
PyMODINIT_FUNC PyInit_numc_optimized(void) {
    PyObject* m;

    if (PyType_Ready(&matrix_optimized61cType) < 0)
        return NULL;

    m = PyModule_Create(&numc_optimizedmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&matrix_optimized61cType);
    PyModule_AddObject(m, "matrix_optimized", (PyObject *)&matrix_optimized61cType);
    printf("CS61C Project 4: numc_optimized imported!\n");
    fflush(stdout);
    return m;
}
