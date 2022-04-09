#include "pynl.hpp"
#include "nonlinear.hpp"
#include <armadillo>

using arma::mat;

const char *pynl_doc = "";

/**
 * @brief return a PyTuple with the system traiectory at given params
 *
 * @param self
 * @param args
 * @return PyObject*
 */
static PyObject *pynl_traiectory(PyObject *self, PyObject *args)
{
  printf("Here++");
  mat x0;
  mat params;
  uint n_points;
  double step;
  const char *method = "euler";
  PyObject *x0_list, *params_list, *tmat_ret, *system_capsule;
  tmat_ret = Py_None;
  if (PyArg_ParseTuple(args, "OOOId", &system_capsule, &x0_list, &params_list,
                       &n_points, &step))
  {
    uint system_size = PyList_Size(x0_list);
    x0 = mat(system_size, 1);
    for (uint i = 0; i < system_size; i++)
    {
      x0(i, 0) = PyFloat_AsDouble(PyList_GetItem(x0_list, i));
    }
    uint params_size = PyList_Size(params_list);
    params = mat(params_size, 1);
    for (uint i = 0; i < params_size; i++)
    {
      params(i, 0) = PyFloat_AsDouble(PyList_GetItem(params_list, i));
    }

    SystemDescriptor *system = (SystemDescriptor *)PyCapsule_GetPointer(
        system_capsule, "system_test.system");
    double *data;
    mat *t = traiectory(*system, x0, params, n_points, step, "euler", &data);
    for (uint i = 0; i < 22; i++)
      printf("%lf ", data[i]);
    npy_intp dims[2];
    dims[0] = static_cast<npy_intp>(t->n_rows);
    dims[1] = static_cast<npy_intp>(t->n_cols);
    tmat_ret = PyArray_SimpleNewFromData(2, dims, NPY_DOUBLE, (void *)data);
  }
  else
  {
    printf("ERROR: pynl_traiectory --- failed parsing function parameters.");
  }
  return tmat_ret;
}

static PyObject *pynl_vector_field_2d(PyObject *self, PyObject *args)
{
  PyObject *ret = Py_None;
  PyObject *system_capsule, *params_list;
  double xmin, xmax, ymin, ymax;
  uint x_points, y_points;
  mat params;
  if (PyArg_ParseTuple(args, "OddddIIO", &system_capsule, &xmin, &xmax, &ymin,
                       &ymax, &x_points, &y_points, &params_list))
  {
    SystemDescriptor *system = (SystemDescriptor *)PyCapsule_GetPointer(
        system_capsule, "system_test.system");
    uint params_size = PyList_Size(params_list);
    params = mat(1, params_size);
    for (uint i = 0; i < params_size; i++)
      params(0, i) = PyFloat_AsDouble(PyList_GET_ITEM(params_list, i));

    mat **vf = vector_field_2d(*system, xmin, xmax, ymin, ymax, x_points,
                               y_points, params);
    ret = PyList_New(x_points * y_points);
    PyObject **points = new PyObject *[x_points * y_points];

    for (uint i = 0; i < x_points * y_points; i++)
    {
      points[i] = PyList_New(2);
      PyList_SET_ITEM(points[i], 0, PyFloat_FromDouble(vf[i]->at(0)));
      PyList_SET_ITEM(points[i], 1, PyFloat_FromDouble(vf[i]->at(1)));
      PyList_SET_ITEM(ret, i, points[i]);
    }
  }
  else
  {
    printf("ERROR: pynl_vector_field_2d"
           " --- failed parsing function parameters.");
  }
  return ret;
}

static PyMethodDef PynlMethods[] = {
    {"traiectory", pynl_traiectory, METH_VARARGS, "Compute a traiectory."},
    {"vector_field_2d", pynl_vector_field_2d, METH_VARARGS,
     "Compute a vector field."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef pynlmodule = {PyModuleDef_HEAD_INIT, "pynl", pynl_doc,
                                        -1, PynlMethods};

PyMODINIT_FUNC PyInit_pynl(void) { return PyModule_Create(&pynlmodule); }
