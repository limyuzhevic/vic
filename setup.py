from setuptools import setup
from setuptools.extension import Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext
import pybind11
import os

# Define the extension module
ext_modules = [
    Pybind11Extension(
        "pynlm",
        [
            "python/bindings.cpp",
        ],
        include_dirs=[
            "python/include",
            "src",
            os.path.join(os.path.dirname(__file__), "src"),
        ],
        cxx_std=20,
        extra_compile_args=["-std=c++20"],
        language="c++",
    ),
]

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) neural simulation framework",
    ext_modules=ext_modules,
    setup_requires=["pybind11>=2.11.0"],
    python_requires=">=3.8",
    author="NLM Authors",
    license="MIT",
)
