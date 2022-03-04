#include "pynl.hpp"
#include <iostream>
#include "nonlinear.hpp"
using std::cout, std::endl;

// extern SystemDescriptor _system;

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
    PyObject *x0_list, *params_list;
    if (PyArg_ParseTuple(args, "dOOd", &t0, &x0_list, &params_list, &step))
    {
        cout << "t0: " << t0 << endl;
        uint system_size = PyList_Size(x0_list);
        x0 = mat(system_size, 1);
        for (uint i = 0; i < system_size; i++)
        {
            x0(i, 0) = PyFloat_AsDouble(PyList_GetItem(x0_list, i));
        }
        cout << "x0: \n"
             << x0 << endl;
        uint params_size = PyList_Size(params_list);
        params = mat(params_size, 1);
        for (uint i = 0; i < params_size; i++)
        {
            params(i, 0) = PyFloat_AsDouble(PyList_GetItem(params_list, i));
        }
        cout << "params: \n"
             << params << endl;
        cout << "step: " << step << endl;
    }
    return Py_None;
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