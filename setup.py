# pynlm - Python bindings for NLM (Neural Learning Machine)
#
# This is the setup.py for building the NLM Python package
#

from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.version import LooseVersion
import os
import sys
import subprocess
import platform

class PyNLMBuildExt(build_ext):
    """Custom build extension to compile NLM Python bindings."""
    
    def run(self):
        # Ensure CMake is available
        try:
            subprocess.check_call(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build NLM Python bindings")
        
        # Build the C++ extension
        self.build_extension()
    
    def build_extension(self):
        """Build the _pynlm_core extension module."""
        # Determine build directory
        build_dir = os.path.abspath(self.build_temp)
        source_dir = os.path.abspath(self.build_source)
        
        # Configure CMake
        cmake_args = [
            '-DCMAKE_BUILD_TYPE=Release',
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            '-DBUILD_PYTHON_BINDINGS=ON',
            '-DBUILD_SHARED_LIBS=ON',
            '-DBUILD_TESTING=OFF',
            '-DBUILD_EXAMPLES=OFF'
        ]
        
        # Configure
        subprocess.check_call(['cmake', source_dir] + cmake_args, cwd=build_dir)
        
        # Build
        subprocess.check_call(['cmake', '--build', '.', '--config', 'Release'], cwd=build_dir)
        
        # Copy the built module to the package directory
        if platform.system() == 'Windows':
            ext_filename = 'pynlm.pyd'
        else:
            ext_filename = 'pynlm.so'
        
        build_module_path = os.path.join(build_dir, ext_filename)
        package_module_path = os.path.join(
            self.get_ext_fullpath('_pynlm_core')
        )
        
        if os.path.exists(build_module_path):
            self.copy_file(build_module_path, package_module_path)
        else:
            # Try to find the module in a different location
            import glob
            matches = glob.glob(os.path.join(build_dir, '**', ext_filename), recursive=True)
            if matches:
                self.copy_file(matches[0], package_module_path)

class PyNLMInstall(install):
    """Custom install class."""
    
    def run(self):
        # Build extensions first
        self.run_command('build_ext')
        super().run()

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) - An experimental artificial developmental brain",
    long_description="""
NLM (熙然, meaning "serene flow") is an experimental computational brain project. 
The long-term goal is to create a neural system that begins in a primitive developmental 
state and acquires increasingly complex abilities through interaction with an environment.

This package provides Python bindings for the NLM C++ neural simulation framework,
allowing users to create and interact with virtual brains using a simple, intuitive API.

Key Features:
- Real spiking neural computation (LIF neurons, event-driven dynamics)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Prediction systems for temporal sequence learning
- Developmental stages that modulate plasticity
- Sensorimotor interaction with simulated environments
- Full control over neural connectivity and plasticity rules
- Checkpoint saving and loading for reproducible experiments

The API is designed to be both beginner-friendly with simple examples and
powerful for advanced users who need fine-grained control over brain parameters.
""",
    long_description_content_type="text/markdown",
    author="NLM Project",
    author_email="nlm-project@example.com",
    url="https://github.com/nlm-project/nlm",
    license="MIT",
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Libraries",
        "Topic :: Education",
    ],
    keywords=["neural", "simulation", "spiking", "brain", "learning", "AI", "neuromorphic"],
    python_requires=">=3.8",
    packages=find_packages(),
    package_data={
        "pynlm": [
            "*.so",
            "*.pyd",
            "*.txt",
            "*.md"
        ],
    },
    include_package_data=True,
    ext_modules=[],
    cmdclass={
        'build_ext': PyNLMBuildExt,
        'install': PyNLMInstall,
    },
    install_requires=[],
    setup_requires=[],
    zip_safe=False,
    entry_points={
        "console_scripts": [
            "nlm-run = pynlm.cli:main",
            "nlm-explore = pynlm.explore:main",
            "nlm-demo = pynlm.demo:main",
        ],
    },
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm",
        "Issues": "https://github.com/nlm-project/nlm/issues",
        "Discussions": "https://github.com/nlm-project/nlm/discussions",
    },
)
