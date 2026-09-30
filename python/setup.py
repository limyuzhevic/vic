/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d36b4516-9598-423b-a111-045888d2ef5f/python/setup.py
Name: Setup script for pynlm package
Summary: Python setup script for building and distributing pynlm

from setuptools import setup, find_packages
from pathlib import Path

# Read the README file
this_directory = Path(__file__).parent
readme_path = this_directory / 'pynlm' / 'README.md'

if readme_path.exists():
    with open(readme_path, 'r') as f:
        long_description = f.read()
else:
    long_description = """
Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework.

This package provides a comprehensive Python interface for runtime control of the NLM brain system, including:
- Core Python API for brain control and monitoring
- Real-time brain state access and manipulation
- Configuration management from Python
- Event-driven programming with callbacks
- Asynchronous operations for non-blocking control
- Brain simulation control (step, pause, reset, save/load)
- Neural network manipulation (inject stimuli, monitor neurons)
- Memory system access and control
- Neuromodulation system control
- Plasticity rule management
- Interactive REPL for exploration
- Visualization and plotting capabilities
- Real-time monitoring with customizable dashboards
- Plugin system for custom Python modules
- Integration with NumPy, Matplotlib, Pandas
- Support for Jupyter notebooks and interactive environments
- Database integration for persistent brain state
- Cloud connectivity for distributed computing
- Sandboxed execution for safety
- Access control and permissions
- Input validation and error handling
- Resource limits and monitoring
"""

setup(
    name='pynlm',
    version='0.1.0',
    description='Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='NLM Authors',
    author_email='',
    url='https://github.com/nlm-project/nlm',
    license='MIT',
    packages=find_packages(where='python/pynlm'),
    package_dir={'': 'python'},
    install_requires=[
        'numpy>=1.20',
        'matplotlib>=3.0',
        'pandas>=1.0',
        'pybind11>=2.11.0',
        'scikit-build-core>=0.5.0',
        'pytest>=7.0',
    ],
    extras_require={
        'dev': [
            'pytest>=7.0',
            'numpy>=1.20',
            'matplotlib>=3.0',
            'pandas>=1.0',
            'jupyter',
            'ipywidgets',
        ],
        'full': [
            'numpy>=1.20',
            'matplotlib>=3.0',
            'pandas>=1.0',
            'scipy',
            'seaborn',
            'plotly',
            'jupyter',
            'ipywidgets',
            'sqlalchemy',
            'pymongo',
            'redis',
        ],
    },
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    python_requires='>=3.8',
    keywords=['neural', 'simulation', 'spiking', 'brain', 'learning'],
    project_urls={
        'Documentation': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm.git',
        'Issues': 'https://github.com/nlm-project/nlm/issues',
        'Funding': 'https://github.com/sponsors/nlm-project',
    },
)
