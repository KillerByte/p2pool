from distutils.core import setup, Extension

memorycoin_momentum_module = Extension('memorycoin_momentum',
                               libraries = ['ssl', 'crypto'],
                               sources = ['momentummodule.c',
                                          'momentum.c'],
                               include_dirs=['.'])

setup (name = 'memorycoin_momentum',
       version = '1.0',
       description = 'Bindings for AES-NI momentum proof of work used by Memorycoin',
       ext_modules = [memorycoin_momentum_module])
