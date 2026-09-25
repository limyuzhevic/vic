from setuptools import setup, find_packages
import os
import platform
import sys
import subprocess

# Read version from file
with open('pynlm/__init__.py', 'r') as f:
    content = f.read()
    import re
    version_match = re.search(r"__version__ = \"([0-9.]+)\"", content)
    if version_match:
        version = version_match.group(1)
    else:
        version = '2.0.0'

# Check if pybind11 extension can be built
try:
    # Try to build the extension
    import pybind11
    from pybind11.setup_helpers import Pybind11Extension, build_ext
    
    # Define the extension module
    ext_modules = [
        Pybind11Extension(
            'pynlm._pynlm',
            sources=[
                'pynlm/_pynlm.cpp',
                'src/brain/Neuron.cpp',
                'src/brain/Synapse.cpp',
                'src/brain/NeuralRegion.cpp',
                'src/brain/NeuralPopulation.cpp',
                'src/brain/Brain.cpp',
                'src/core/Config/Config.cpp',
                'src/core/Random/Random.cpp',
                'src/core/Logger/Logger.cpp',
                'src/core/Error/Error.cpp',
                'src/core/Error/ErrorUtilities.cpp',
                'src/core/Factory/Factory.cpp',
                'src/dynamics/SpikeSystem.cpp',
                'src/plasticity/STDP.cpp',
                'src/plasticity/Hebbian.cpp',
                'src/plasticity/StructuralPlasticity.cpp',
                'src/neuromodulation/Dopamine.cpp',
                'src/neuromodulation/Curiosity.cpp',
                'src/neuromodulation/Novelty.cpp',
                'src/neuromodulation/PredictionError.cpp',
                'src/memory/NeuralWorkingMemory.cpp',
                'src/memory/NeuralEpisodicMemory.cpp',
                'src/memory/NeuralAssociativeMemory.cpp',
                'src/cognition/NeuralPlanner.cpp',
                'src/cognition/ConceptFormation.cpp',
                'src/cognition/AttentionalSelection.cpp',
                'src/development/DevelopmentSystem.cpp',
                'src/development/Maturation.cpp',
                'src/development/Pruning.cpp',
                'src/development/Synaptogenesis.cpp',
                'src/world/SimpleWorld.cpp',
                'src/world/Observation.cpp',
                'src/agent/AgentBrain.cpp',
                'src/agent/SensoryPercept.cpp',
                'src/agent/AgentBody.cpp',
                'src/environment/Environment.cpp',
                'src/environment/Observation.cpp',
                'src/visualization/VisualizationInterface.cpp',
                'src/experiments/ExperimentRunner.cpp',
                'src/experiments/Phase3Experiment.cpp',
                'src/experiments/Phase4Experiment.cpp',
                'src/experiments/Phase5Experiment.cpp',
                'src/experiments/Phase6IntegratedExperiment.cpp',
            ],
            include_dirs=[
                'src',
                os.path.join(pybind11.get_include(), 'pybind11'),
                os.path.join(pybind11.get_include(), 'pybind11/stl'),
            ],
            language='c++',
            cxx_std=20,
            py_limited_api=True,
        ),
    ]
    
    setup(
        name='pynlm',
        version=version,
        author='NLM Authors',
        description='Python bindings for NLM (Neural Learning Machine) - a brain-inspired spiking neural network simulator',
        long_description=open('README.md').read(),
        long_description_content_type='text/markdown',
        url='https://github.com/nlm-project/nlm',
        packages=find_packages(),
        ext_modules=ext_modules,
        install_requires=[
            'numpy>=1.20',
        ],
        setup_requires=[
            'pybind11>=2.11.0',
        ],
        python_requires='>=3.8',
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
            'Programming Language :: C++',
            'Topic :: Scientific/Engineering :: Artificial Intelligence',
        ],
        keywords='neural simulation spiking brain learning',
        ext_modules=ext_modules,
        cmdclass={'build_ext': build_ext},
    )
    
except ImportError:
    # If pybind11 is not available, create a minimal setup for documentation purposes
    setup(
        name='pynlm',
        version=version,
        author='NLM Authors',
        description='Python bindings for NLM (Neural Learning Machine) - a brain-inspired spiking neural network simulator',
        long_description='This package requires pybind11 and C++ compiler to build the core extension.',
        url='https://github.com/nlm-project/nlm',
        packages=find_packages(),
        python_requires='>=3.8',
        classifiers=[
            'Development Status :: 3 - Alpha',
            'Intended Audience :: Science/Research',
            'License :: OSI Approved :: MIT License',
            'Programming Language :: Python :: 3',
        ],
    )
