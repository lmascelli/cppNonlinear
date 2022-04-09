#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <numpy/ndarrayobject.h>

static PyObject *pynl_traiectory(PyObject *self, PyObject *args);
static PyObject *pynl_vector_field_2d(PyObject *self, PyObject *args);
