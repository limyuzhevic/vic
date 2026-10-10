# NLM Python Package
# This package provides a Python interface to the NLM neural simulation framework

[metadata]
name = pynlm
version = 0.1.0
description = Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework
description-file = README.md
long-description = file: README.md
long-description-content-type = text/markdown
author = NLM Authors
author-email = nlm-authors@example.com
license = MIT
keywords = neural simulation spiking brain learning
classifiers = 
    Development Status :: 3 - Alpha
    Intended Audience :: Science/Research
    License :: OSI Approved :: MIT License
    Programming Language :: Python :: 3
    Programming Language :: Python :: 3.8
    Programming Language :: Python :: 3.9
    Programming Language :: Python :: 3.10
    Programming Language :: Python :: 3.11
    Programming Language :: Python :: 3.12
    Topic :: Scientific/Engineering :: Artificial Intelligence

[options]
packages = find:
install_requires =
python_requires = >=3.8
include_package_data = True

[options.packages.find]
where = .
exclude = 
    tests*
    build*
    dist*

[options.entry_points]
console_scripts =
    nlm-demo = pynlm.brain:main

[tool:pytest]
testpaths = tests
python_files = test_*.py
addopts = -v --tb=short

[build-system]
requires = setuptools>=42, wheel
build-backend = setenv

[setenv]
PYTHONPATH = {root:workspace}/.build/lib
