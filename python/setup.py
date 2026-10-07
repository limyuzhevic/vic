#!/usr/bin/env python3

import os
import sys
import platform
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.command.build import build
import subprocess

# Package metadata
PACKAGE_NAME = "nlm"
PACKAGE_VERSION = "0.1.0"
PACKAGE_DESCRIPTION = "Python bindings for the Neural Learning Machine (NLM) neural simulation framework"
PACKAGE_AUTHOR = "NLM Development Team"
PACKAGE_EMAIL = "contact@nlm.org"
PACKAGE_URL = "https://github.com/nlmlab/nlm"
PACKAGE_LICENSE = "MIT"

# Check Python version
if sys.version_info < (3, 7):
    sys.exit("NLM requires Python 3.7 or higher")

# Try to detect the C++ compiler
if platform.system() == "Linux":
    CXX_COMPILER = "g++"
elif platform.system() == "Darwin":
    CXX_COMPILER = "clang++"
elif platform.system() == "Windows":
    CXX_COMPILER = "cl"
else:
    CXX_COMPILER = "g++"

# Read the C++ source files for compilation
NLM_SOURCE_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "src")

if not os.path.exists(NLM_SOURCE_DIR):
    sys.exit(f"Error: NLM source directory not found at {NLM_SOURCE_DIR}")

# Collect all source files
def collect_cpp_sources(directory, extensions=('.cpp', '.cc', '.cxx')):
    sources = []
    for root, dirs, files in os.walk(directory):
        # Skip build directories
        if '__pycache__' in root or 'build' in root:
            continue
        for file in files:
            if file.endswith(extensions):
                sources.append(os.path.join(root, file))
    return sources

# Try to find NLM CMake build
nlm_build_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "build")
if os.path.exists(os.path.join(nlm_build_dir, "CMakeCache.txt")):
    # Use the existing CMake build
    print("Using existing NLM CMake build...")
else:
    # Try to build NLM
    print("Building NLM from source...")
    try:
        subprocess.check_call(["cmake", "-B", nlm_build_dir, "-S", os.path.dirname(os.path.abspath(__file__))])
        subprocess.check_call(["cmake", "--build", nlm_build_dir, "--config", "Release", "-j4"])
    except subprocess.CalledProcessError as e:
        sys.exit(f"Error building NLM: {e}")

# Define the extension module
ext_modules = [
    {
        'name': 'nlm._pynlm',
        'sources': [
            # We need to actually compile the C++ bindings
            # For now, we'll use the CMake-built library
        ],
        'include_dirs': [
            NLM_SOURCE_DIR,
            os.path.join(os.path.dirname(os.path.abspath(__file__)), "python"),
        ],
        'language': 'c++',
        'cxx_std': 20,
        'extra_compile_args': [
            '-std=c++20',
            '-Wall',
            '-Wextra',
            '-pedantic',
        ],
        'extra_link_args': [],
        'libraries': [],
        'library_dirs': [nlm_build_dir],
        'define_macros': [
            ('NLM_VERSION', PACKAGE_VERSION),
        ],
    }
]

# Custom build extension to handle the C++ dependencies
def build_extension(cmd):
    # Ensure the NLM library is built
    nlm_build_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "build")
    if not os.path.exists(os.path.join(nlm_build_dir, "libnlm_core.a")):
        print("Building NLM core library...")
        try:
            subprocess.check_call(["cmake", "--build", nlm_build_dir, "--config", "Release", "-j4"])
        except subprocess.CalledProcessError as e:
            sys.exit(f"Error building NLM core library: {e}")
    
    # Call the parent build_extension method
    super(build_ext, cmd).run()

# Custom install command to set up the Python package
class InstallCommand(install):
    def run(self):
        # Run the standard install command
        super().run()
        
        # Create symlinks or copy files if needed
        print("NLM Python package installed successfully!")
        print("To use NLM in Python:")
        print("  import nlm")
        print("  from nlm import NLM, create_agent")
        print("  # Create an agent")
        print("  agent = create_agent(config_file='my_config.json')")
        print("  # Or create a complete simulation")
        print("  sim = nlm.NLM(config_file='my_config.json')")

# Custom build command
setup(
    name=PACKAGE_NAME,
    version=PACKAGE_VERSION,
    description=PACKAGE_DESCRIPTION,
    author=PACKAGE_AUTHOR,
    author_email=PACKAGE_EMAIL,
    url=PACKAGE_URL,
    license=PACKAGE_LICENSE,
    packages=find_packages(),
    package_dir={
        '': 'python',
        'nlm': 'python/nlm',
        'nlm.core': 'python/nlm/core',
        'nlm.brain': 'python/nlm/brain',
        'nlm.agent': 'python/nlm/agent',
        'nlm.world': 'python/nlm/world',
        'nlm.sensory': 'python/nlm/sensory',
        'nlm.motor': 'python/nlm/motor',
        'nlm.environment': 'python/nlm/environment',
        'nlm.logging': 'python/nlm/logging',
        'nlm.config': 'python/nlm/config',
        'nlm.exceptions': 'python/nlm/exceptions',
    },
    ext_modules=ext_modules,
    cmdclass={
        'build_ext': build_extension,
        'install': InstallCommand,
    },
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Topic :: Scientific/Engineering',
        'Topic :: Software Development :: Libraries',
    ],
    python_requires='>=3.7',
    install_requires=[
        'pybind11>=2.6.0',
    ],
    extras_require={
        'dev': [
            'pybind11>=2.6.0',
            'cmake>=3.16',
            'ninja',
        ],
        'test': [
            'pytest',
            'pytest-cov',
            'coverage',
        ],
    },
    long_description=open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "python", "README.md"), "r").read() if os.path.exists(os.path.join(os.path.dirname(os.path.abspath(__file__)), "python", "README.md")) else PACKAGE_DESCRIPTION,
    long_description_content_type='text/markdown' if os.path.exists(os.path.join(os.path.dirname(os.path.abspath(__file__)), "python", "README.md")) and os.path.join(os.path.dirname(os.path.abspath(__file__)), "python", "README.md").endswith('.md') else 'text/plain',
    keywords='neuroscience neural-networks brain simulation AI machine-learning',
    project_urls={
        'Documentation': 'https://docs.nlm.org',
        'Source': PACKAGE_URL,
        'Tracker': f'{PACKAGE_URL}/issues',
    },
)