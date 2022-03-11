#include "pynl.hpp"
#include <armadillo>
#include "nonlinear.hpp"

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
    mat x0;
    mat params;
    uint n_points;
    double step;
    const char *method = "euler";
    PyObject *x0_list, *params_list, *tmat_ret, *system_capsule;
    tmat_ret = Py_None;
    if (PyArg_ParseTuple(args, "OOOId", &system_capsule,
                         &x0_list, &params_list, &n_points, &step))
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

        SystemDescriptor *system =
            (SystemDescriptor *)PyCapsule_GetPointer(system_capsule,
                                                     "system_test.system");
        mat t = traiectory(*system, x0, params, n_points, step);
        PyObject *t_ret;
        PyObject **x_ret = new PyObject *[system_size];
        tmat_ret = PyList_New(1 + system_size);
        t_ret = PyList_New(t.n_cols);
        for (uint row = 0; row < system_size; row++)
            x_ret[row] = PyList_New(t.n_cols);
        for (uint col = 0; col < t.n_cols; col++)
        {
            PyList_SET_ITEM(t_ret, col, PyFloat_FromDouble(t(0, col)));
            for (uint row = 0; row < system_size; row++)
                PyList_SET_ITEM(x_ret[row], col,
                                PyFloat_FromDouble(t(row + 1, col)));
        }
        PyList_SetItem(tmat_ret, 0, t_ret);
        for (uint row = 0; row < system_size; row++)
            PyList_SetItem(tmat_ret, row + 1, x_ret[row]);
    }
    else
    {
        printf(
            "ERROR: pynl_traiectory --- failed parsing function parameters.");
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
    if (PyArg_ParseTuple(args, "OddddIIO", &system_capsule,
                         &xmin, &xmax, &ymin, &ymax, &x_points, &y_points,
                         &params_list))
    {
        SystemDescriptor *system = (SystemDescriptor *)
            PyCapsule_GetPointer(system_capsule, "system_test.system");
        uint params_size = PyList_Size(params_list);
        params = mat(1, params_size);
        for (uint i = 0; i < params_size; i++)
            params(0, i) = PyFloat_AsDouble(PyList_GET_ITEM(params_list, i));

        mat **vf = vector_field_2d(*system, xmin, xmax, ymin, ymax,
                                   x_points, y_points, params);
        ret = PyList_New(x_points * y_points);
        PyObject **points = new PyObject *[x_points * y_points];

        for (uint i = 0; i < x_points * y_points; i++)
        {
            points[i] = PyList_New(2);
            PyList_SET_ITEM(points[i], 0, PyFloat_FromDouble((*vf)[i](0, 0)));
            printf("%f\n", (*vf)[i](0, 0));
            PyList_SET_ITEM(points[i], 1, PyFloat_FromDouble((*vf)[i](1, 0)));
            printf("%f\n", (*vf)[i](1, 0));
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
    {"traiectory", pynl_traiectory, METH_VARARGS,
     "Compute a traiectory."},
    {"vector_field_2d", pynl_vector_field_2d, METH_VARARGS,
     "Compute a vector field."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef pynlmodule = {
    PyModuleDef_HEAD_INIT,
    "pynl",
    pynl_doc,
    -1,
    PynlMethods};

PyMODINIT_FUNC
PyInit_pynl(void)
{
    return PyModule_Create(&pynlmodule);
}
