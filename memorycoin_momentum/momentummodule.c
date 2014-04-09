#include <Python.h>

#include "momentum.h"

static const signed char phexdigit[256] =
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, };

unsigned char *ParseHex(const char* psz, int len)
{
    // convert hex dump to vector
    unsigned char *vch = malloc(len);
    int i;
    for(i = 0;i < len;i++)
    {
        while (isspace(*psz))
            psz++;
        signed char c = phexdigit[(unsigned char)*psz++];
        if (c == (signed char)-1)
            break;
        unsigned char n = (c << 4);
        c = phexdigit[(unsigned char)*psz++];
        if (c == (signed char)-1)
            break;
        n |= c;
        vch[i] = n;
    }
    return vch;
}

static PyObject *momentum_check(PyObject *self, PyObject *args)
{
	char *midhash;
	
	
	//int size;
	int size;
	unsigned int a, b;
	if(!PyArg_ParseTuple(args, "s#II", &midhash, &size, &a, &b))
		return NULL;
	
	
	
	if(size != 64)
		return NULL;
	
	unsigned char * data = ParseHex(midhash, 32);
	
	if(momentum_verify(data, a, b))
		return Py_True;
	else
		return Py_False;
}

static PyMethodDef MomentumMethods[] = {
    { "checkMomentum", momentum_check, METH_VARARGS, "Verifies birthday values of the Memorycoin block header" },
    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initmemorycoin_momentum(void) {
    (void) Py_InitModule("memorycoin_momentum", MomentumMethods);
}