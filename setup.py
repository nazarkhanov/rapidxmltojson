from setuptools import setup, find_packages, Extension
from Cython.Build import cythonize


extension = Extension(
    name='rapidxmltojson',
    sources=['source/rapidxmltojson.pyx'],
    package_data={'source': ['py.typed']},
    include_dirs=['include'],
    language='c++',
)


setup(
    name='rapidxmltojson',
    packages=find_packages(),
    ext_modules=cythonize([extension]),
)
