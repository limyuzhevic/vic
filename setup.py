from setuptools import setup, find_packages
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext
import sys
import os

# Determine if we're building the Python extension
ext_modules = []
if not os.path.exists('build'):
    # Try to build the Python extension if possible
    try:
        from skbuild import get_config
        from skbuild import setup as skbuild_setup
        
        # Use scikit-build for building the extension
        setup(
            name="pynlm",
            version="0.1.0",
            description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
            author="NLM Authors",
            author_email="",
            url="https://github.com/nlm-project/nlm",
            packages=find_packages(),
            package_data={
                "": ["*.py", "*.txt", "*.md"],
                "python": ["*.py"]
            },
            include_package_data=True,
            classifiers=[
                "Development Status :: 3 - Alpha",
                "Intended Audience :: Science/Research",
                "License :: OSI Approved :: MIT License",
                "Programming Language :: Python :: 3",
                "Programming Language :: Python :: 3.8",
                "Programming Language :: Python :: 3.9",
                "Programming Language :: Python :: 3.10",
                "Programming Language :: Python :: 3.11",
                "Programming Language :: Python :: 3.12",
                "Topic :: Scientific/Engineering :: Artificial Intelligence",
            ],
            python_requires=">=3.8",
            install_requires=[],
            extras_require={
                "dev": ["pytest>=7.0", "numpy>=1.20"],
            },
            zip_safe=False,
            setup_requires=["scikit-build-core>=0.5.0", "pybind11>=2.11.0"],
        )
        
    except ImportError:
        # Fallback to simple setup for development without extension
        setup(
            name="pynlm",
            version="0.1.0",
            description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
            author="NLM Authors",
            author_email="",
            url="https://github.com/nlm-project/nlm",
            packages=find_packages(),
            package_data={
                "": ["*.py", "*.txt", "*.md"],
                "python": ["*.py"]
            },
            include_package_data=True,
            classifiers=[
                "Development Status :: 3 - Alpha",
                "Intended Audience :: Science/Research",
                "License :: OSI Approved :: MIT License",
                "Programming Language :: Python :: 3",
                "Programming Language :: Python :: 3.8",
                "Programming Language :: Python :: 3.9",
                "Programming Language :: Python :: 3.10",
                "Programming Language :: Python :: 3.11",
                "Programming Language :: Python :: 3.12",
                "Topic :: Scientific/Engineering :: Artificial Intelligence",
            ],
            python_requires=">=3.8",
            install_requires=[],
            extras_require={
                "dev": ["pytest>=7.0", "numpy>=1.20"],
            },
            zip_safe=False,
        )
else:
    # Use scikit-build if extension is already built
    from skbuild import setup as skbuild_setup
    
    skbuild_setup(
        name="pynlm",
        version="0.1.0",
        description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
        author="NLM Authors",
        author_email="",
        url="https://github.com/nlm-project/nlm",
        packages=find_packages(),
        package_data={
            "": ["*.py", "*.txt", "*.md"],
            "python": ["*.py"]
        },
        include_package_data=True,
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Intended Audience :: Science/Research",
            "License :: OSI Approved :: MIT License",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3.8",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
            "Programming Language :: Python :: 3.12",
            "Topic :: Scientific/Engineering :: Artificial Intelligence",
        ],
        python_requires=">=3.8",
        install_requires=[],
        extras_require={
            "dev": ["pytest>=7.0", "numpy>=1.20"],
        },
        zip_safe=False,
        setup_requires=["scikit-build-core>=0.5.0", "pybind11>=2.11.0"],
    )