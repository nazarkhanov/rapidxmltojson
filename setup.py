from setuptools import setup, Extension
from Cython.Build import cythonize


extension = Extension(
    name='rapidxmltojson',
    sources=['rapidxmltojson.pyx'],
    include_dirs=['include'],
    language='c++',
)


setup(
    ext_modules=cythonize([extension]),
)
