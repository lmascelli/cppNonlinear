#include <Python.h>
#include "nonlinear.hpp"

static void system_destroy(PyObject *system)
{
    delete (SystemDescriptor *)PyCapsule_GetPointer(system, "system_test.system");
}

static PyObject *getSystemDescriptor(PyObject *self, PyObject *args)
{
    SystemDescriptor *system = new SystemDescriptor(2);

    // Implement system functions here
    //***********************************************

    system->AddFunction(
        [](mat x, mat param)
        {
        const double a = 0.04;
        const double b = 5;
        const double c = 150;
        mat ret{a * x(0) * x(0) + b * x(0) + c - x(1),
                param(0) * (param(1) * x(0) - x(1))};
        ret = ret.t();
        return ret; },
        SystemDescriptor::EQUATION);

    /**
     * @brief reset rule map
     *  x -> -55
     *  y -> y + 4
     */
    system->AddFunction(
        [](mat x, mat param)
        {
            const double d = -55;
            const double f = 4;
            mat ret{d, x(1) + f};
            return (mat)ret.t();
        },
        SystemDescriptor::MAP);

    /**
     * @brief the manifold of reset
     *
     *  x >= 30
     */
    system->manifold = [](mat x)
    {
        const double xTH = 30;
        uint ret = x(0) >= xTH ? 1 : 0;
        return ret;
    };

    /**
     * @brief the gradient of the manifold
     * manifold(x, y) = x - 30
     * gradient -> (1, 0)
     */
    system->manifold_gradient = [](mat x, mat params)
    {
        mat ret{1, 0};
        return ret;
    };

    /**
     * @brief the time derivative is always 0
     *
     */
    system->manifold_time_derivative = [](mat x, mat params)
    {
        mat ret = arma::zeros(1, 1);
        return ret;
    };

    //***********************************************

    return PyCapsule_New((void *)system, "system_test.system", system_destroy);
}

static PyMethodDef System_testMethods[] = {
    {"GetSystemDescriptor", getSystemDescriptor, METH_VARARGS,
     "Return the system descriptor"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef system_testmodule = {
    PyModuleDef_HEAD_INIT,
    "system_test",
    NULL,
    -1,
    System_testMethods};

PyMODINIT_FUNC
PyInit_system_test(void)
{
    return PyModule_Create(&system_testmodule);
}