#include "pynl.hpp"
#include <iostream>
#include "test.hpp"

using std::cout, std::endl;

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
    double t0;
    mat x0;
    mat params;
    uint n_points;
    double step;
    std::string method = "euler";
    PyObject *x0_list, *params_list, *tmat_ret, *system_capsule;
    tmat_ret = Py_None;
    if (PyArg_ParseTuple(args, "OdOOId", &system_capsule, &t0, &x0_list, &params_list, &n_points, &step))
    {
        std::cout << "t0: " << t0 << std::endl;
        uint system_size = PyList_Size(x0_list);
        cout << system_size << endl;
        x0 = mat(system_size, 1);
        for (uint i = 0; i < system_size; i++)
        {
            x0(i, 0) = PyFloat_AsDouble(PyList_GetItem(x0_list, i));
        }
        std::cout << "x0: \n"
                  << x0 << std::endl;
        uint params_size = PyList_Size(params_list);
        params = mat(params_size, 1);
        for (uint i = 0; i < params_size; i++)
        {
            params(i, 0) = PyFloat_AsDouble(PyList_GetItem(params_list, i));
        }
        std::cout << "params: \n"
                  << params << std::endl
                  << "n_points: " << n_points << std::endl
                  << "step: " << step << std::endl;

        SystemDescriptor *system =
            (SystemDescriptor *)PyCapsule_GetPointer(system_capsule, "system_test.system");
        if (!system)
            cout << "Error loading system" << endl;
        mat t = traiectory(*system, t0, x0, params, n_points, step);
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
                PyList_SET_ITEM(x_ret[row], col, PyFloat_FromDouble(t(row + 1, col)));
        }
        PyList_SetItem(tmat_ret, 0, t_ret);
        for (uint row = 0; row < system_size; row++)
            PyList_SetItem(tmat_ret, row + 1, x_ret[row]);
        cout << "Here" << endl;
    }
    return tmat_ret;
}

static PyMethodDef PynlMethods[] = {
    {"traiectory", pynl_traiectory, METH_VARARGS,
     "Plot a traiectory."},
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