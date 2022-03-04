#include "pynl.hpp"
#include <iostream>
using uint = unsigned int;
// #include "nonlinear.cpp"

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
    // mat x0;
    // mat params;
    uint n_points;
    double step;
    std::string method = "euler";
    PyObject *list;
    if (PyArg_ParseTuple(args, "dO", &t0, list))
    {
        std::cout << t0 << std::endl;
        // Log::Print() << t0 << endl;
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