#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>

static PyObject *pynl_test(PyObject *self, PyObject *args);
static PyObject *pynl_traiectory(PyObject *self, PyObject *args);
static PyObject *pynl_vector_field_2d(PyObject *self, PyObject *args);
