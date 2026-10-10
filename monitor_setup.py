import sys
import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# Pybind11 extension configuration
ext_modules = []

# Add core extension
if os.path.exists("monitor_bindings.py"):
    # Try to import the monitor bindings module
    try:
        import monitor_bindings
        ext_modules.append(Extension(
            'pynlm.monitor_bindings',
            sources=[],
            libraries=[],
            library_dirs=[],
            extra_compile_args=[],
            extra_link_args=[]
        ))
    except ImportError:
        print("Warning: monitor_bindings module not found")

# Add optional extensions based on configuration
if os.path.exists("setup.py"):  # This should not be the case
    # Check for optional extensions
    if os.path.exists("monitor_bindings.py"):  # This is a placeholder
        pass

setup(
    name='pynlm-monitor',
    version='0.1.0',
    description='NLM Performance Monitoring Tools Python Bindings',
    author='NLM Team',
    packages=['pynlm'],
    ext_modules=ext_modules,
    install_requires=[],
    python_requires='>=3.8',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
    ],
    zip_safe=False,
)
