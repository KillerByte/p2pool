#include <Python.h>

#include "momentum.h"

static PyObject *momentum_check(PyObject *self, PyObject *args)
{
	const char *midstate;
	int size;
	unsigned int a, b;
	if(!PyArg_ParseTuple(args, "s#II", &midstate, &size, &a, &b))
		return NULL;
	
	if(size != 32)
		return NULL;
	
	if(momentum_verify(midstate, a, b))
		return Py_False;
	else
		return Py_True;
}

static PyMethodDef MomentumMethods[] = {
    { "checkMomentum", momentum_check, METH_VARARGS, "Verifies birthday values of the Memorycoin block header" },
    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initmemorycoin_momentum(void) {
    (void) Py_InitModule("memorycoin_momentum", MomentumMethods);
}
