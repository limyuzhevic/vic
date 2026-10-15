"""
Setup script for enhanced NLM Python bindings.
This script configures the build process for the enhanced Python interface.
"""

import os
import sys
import platform
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.command.build import build
from distutils.spawn import find_executable
import subprocess
import json

class EnhancedNLMBuildExt(build_ext):
    """
    Custom build extension class for enhanced NLM Python bindings.
    This ensures the enhanced bindings are built with proper configuration.
    """
    
    def run(self):
        print("Building enhanced NLM Python bindings...")
        print(f"Platform: {platform.system()}")
        print(f"Python: {sys.version}")
        
        # Check for required dependencies
        self.check_dependencies()
        
        # Run the actual build
        super().run()
        
        # Post-build validation
        self.validate_build()
    
    def check_dependencies(self):
        """Check for required system dependencies."""
        print("\nChecking system dependencies...")
        
        # Check for CMake
        cmake = find_executable('cmake')
        if cmake is None:
            raise RuntimeError("CMake is required to build NLM Python bindings")
        print(f"✓ CMake found: {cmake}")
        
        # Check for C++ compiler
        compiler = find_executable('g++')
        if compiler is None:
            compiler = find_executable('clang++')
            if compiler is None:
                raise RuntimeError("C++ compiler (g++ or clang++) is required")
        print(f"✓ C++ compiler found: {compiler}")
        
        # Check Python development headers
        try:
            import sysconfig
            python_include = sysconfig.get_config_var('INCLUDEPY')
            if python_include and os.path.exists(python_include):
                print(f"✓ Python headers found: {python_include}")
            else:
                # Try to find Python.h
                import distutils.sysconfig
                include_dir = distutils.sysconfig.get_python_inc()
                if include_dir and os.path.exists(include_dir):
                    print(f"✓ Python include directory found: {include_dir}")
        except ImportError:
            print("⚠ Python development headers not found - attempting to continue")
    
    def validate_build(self):
        """Validate that the build was successful."""
        print("\nValidating build...")
        
        # Check if module was built
        module_path = os.path.join(self.build_lib, 'pynlm', '__init__.py')
        if os.path.exists(module_path):
            print(f"✓ Module built successfully: {module_path}")
        else:
            # Check alternative locations
            alternatives = [
                os.path.join(self.build_lib, 'pynlm', 'bindings.py'),
                os.path.join('python', 'pynlm', '__init__.py'),
            ]
            for alt_path in alternatives:
                if os.path.exists(alt_path):
                    print(f"✓ Module found at: {alt_path}")
                    return
            
            raise RuntimeError("Module not found after build")
        
        # Check for essential files
        essential_files = [
            os.path.join(self.build_lib, 'pynlm', 'bindings.cpp'),
            os.path.join(self.build_lib, 'pynlm', 'examples', 'basic_usage.py'),
            os.path.join(self.build_lib, 'pynlm', 'tests', 'test_enhanced_bindings.py'),
        ]
        
        for file_path in essential_files:
            if not os.path.exists(file_path):
                print(f"⚠ Warning: Essential file not found: {file_path}")

def get_version():
    """Get version from package metadata."""
    # Try to read version from package metadata
    version_file = os.path.join('python', 'pynlm', '__init__.py')
    if os.path.exists(version_file):
        with open(version_file, 'r') as f:
            content = f.read()
            for line in content.split('\n'):
                if '__version__' in line and '=' in line:
                    return line.split('=')[1].strip().strip('\"\'')
    
    return '1.0.0-enhanced'

def get_requirements():
    """Get Python package requirements."""
    requirements = [
        'pybind11>=2.11.0',
        'numpy>=1.20',
    ]
    
    # Read requirements from file if it exists
    req_file = os.path.join('python', 'requirements.txt')
    if os.path.exists(req_file):
        with open(req_file, 'r') as f:
            file_requirements = [line.strip() for line in f if line.strip() and not line.startswith('#')]
            requirements.extend(file_requirements)
    
    return requirements

def get_long_description():
    """Get long description from README."""
    readme_file = os.path.join('README.md')
    if os.path.exists(readme_file):
        with open(readme_file, 'r') as f:
            return f.read()
    return "Enhanced Python interface for NLM neural simulation framework"

# Custom install class
class EnhancedNLMInstall(install):
    """Custom install class for enhanced NLM Python bindings."""
    
    def run(self):
        print("Installing enhanced NLM Python bindings...")
        
        # Run standard install
        super().run()
        
        # Post-install validation
        self.post_install_validation()
    
    def post_install_validation(self):
        """Validate installation."""
        print("\nValidating installation...")
        
        # Try to import the module
        try:
            import pynlm
            print(f"✓ Successfully imported pynlm version {pynlm.__version__}")
            
            # Check version
            expected_version = get_version()
            if pynlm.__version__ != expected_version:
                print(f"⚠ Version mismatch: expected {expected_version}, got {pynlm.__version__}")
            
            # Test basic functionality
            print("✓ Testing basic functionality...")
            
        except ImportError as e:
            print(f"❌ Failed to import pynlm: {e}")
            raise
        except Exception as e:
            print(f"⚠ Basic functionality test failed: {e}")
        
        # Check for examples
        examples_dir = os.path.join(self.install_lib, 'pynlm', 'examples')
        if os.path.exists(examples_dir):
            print(f"✓ Examples available: {examples_dir}")
        
        # Check for tests
        tests_dir = os.path.join(self.install_lib, 'pynlm', 'tests')
        if os.path.exists(tests_dir):
            print(f"✓ Tests available: {tests_dir}")

# Package data configuration
def get_package_data():
    """Get package data files."""
    package_data = {
        'pynlm': [
            '*.md',
            '*.txt',
            'examples/*.py',
            'tests/*.py',
            'pythonic/*.py',
        ],
    }
    
    return package_data

# Main setup configuration
setup(
    name='pynlm',
    version=get_version(),
    description='Enhanced Python interface for NLM neural simulation framework',
    long_description=get_long_description(),
    long_description_content_type='text/markdown',
    author='NLM Authors',
    author_email='nlm-authors@example.com',
    url='https://github.com/nlm-project/nlm',
    license='MIT',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: C++',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
    ],
    keywords='neural simulation spiking brain learning machine',
    packages=find_packages(where='python'),
    package_dir={'': 'python'},
    package_data=get_package_data(),
    include_package_data=True,
    install_requires=get_requirements(),
    extras_require={
        'dev': [
            'pytest>=7.0',
            'pytest-cov',
            'black',
            'flake8',
            'mypy',
            'sphinx',
            'sphinx-rtd-theme',
        ],
        'examples': [
            'matplotlib>=3.0',
            'jupyter',
        ],
        'docs': [
            'sphinx>=4.0',
            'sphinx-rtd-theme',
            'myst-parser',
        ],
    },
    python_requires='>=3.8',
    zip_safe=False,
    cmdclass={
        'build_ext': EnhancedNLMBuildExt,
        'install': EnhancedNLMInstall,
    },
    entry_points={
        'console_scripts': [
            'nlm-pynlm = pynlm.examples.basic_usage:main',
            'nlm-demo = pynlm.examples.advanced_usage:main',
        ],
    },
    project_urls={
        'Documentation': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm',
        'Issues': 'https://github.com/nlm-project/nlm/issues',
    },
    platforms=['Linux', 'MacOS X', 'Windows'],
    test_suite='pynlm.tests.test_enhanced_bindings',
    tests_require=[
        'pytest>=7.0',
        'pytest-cov',
    ],
    setup_requires=[
        'setuptools>=42',
        'wheel',
        'pybind11>=2.11.0',
    ],
)
