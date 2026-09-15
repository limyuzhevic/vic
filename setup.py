from setuptools import setup
from setuptools.command.build_ext import build_ext
import subprocess
import sys
import os

class Pybind11BuildExt(build_ext):
    def run(self):
        try:
            # Try to build with scikit-build-core (modern approach)
            subprocess.check_call([sys.executable, "-m", "build", "--wheel"], cwd=os.path.dirname(__file__))
        except:
            # Fallback to traditional setup.py
            self.build_py()

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    author="NLM Authors",
    license="MIT",
    packages=[],
    ext_modules=[],
    cmdclass={"build_ext": Pybind11BuildExt},
    python_requires=">=3.8",
    install_requires=[],
    zip_safe=False,
)
