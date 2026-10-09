# Python package configuration for NLM Python bindings
# This package provides access to the enhanced NLM neural learning machine

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import os

class PyNLMBuildExt(build_ext):
    """Custom build extension for NLM Python bindings."""
    
    def build_extension(self, ext):
        # Add include directories for NLM headers
        ext.include_dirs = [
            '../include',
            '../src',
            '../src/core',
            '../src/brain',
            '../src/agent',
            '../src/memory',
            '../src/prediction',
            '../src/cognition',
            '../src/neuromodulation',
            '../src/sensory',
            '../src/motor',
            '../src/environment',
            '../src/world',
            '../src/plasticity',
            '../src/dynamics',
            '../src/development',
            '../src/experiments',
            '../src/visualization',
            '../src/performance'
        ]
        
        # Add compiler flags
        ext.extra_compile_args = ['-std=c++20', '-O2', '-DNDEBUG']
        
        super().build_extension(ext)

# Define the extension module
nlm_extension = Extension(
    'pynlm._native',
    sources=[
        '../src/brain/Brain.cpp',
        '../src/agent/AgentBrain.cpp',
        '../src/world/SimpleWorld.cpp',
        '../src/memory/NeuralWorkingMemory.cpp',
        '../src/memory/NeuralEpisodicMemory.cpp',
        '../src/prediction/PredictionSystem.cpp',
        '../src/cognition/NeuralPlanner.cpp',
        '../src/cognition/ConceptFormation.cpp',
        '../src/cognition/AttentionalSelection.cpp',
        '../src/neuromodulation/Dopamine.cpp',
        '../src/neuromodulation/Curiosity.cpp',
        '../src/neuromodulation/Novelty.cpp',
        '../src/neuromodulation/PredictionError.cpp',
        '../src/neuromodulation/Neuromodulator.cpp',
        '../src/neuromodulation/Reward.cpp',
        '../src/neuromodulation/Curiosity.cpp',
        '../src/neuromodulation/Novelty.cpp',
        '../src/neuromodulation/PredictionError.cpp',
        '../src/core/Config.cpp',
        '../src/core/Logger/Logger.cpp',
        '../src/core/Random/Random.cpp',
        '../src/core/SimulationClock/SimulationClock.cpp',
        '../src/brain/Neuron.cpp',
        '../src/brain/Synapse.cpp',
        '../src/brain/NeuralPopulation.cpp',
        '../src/brain/NeuralRegion.cpp',
        '../src/dynamics/SpikeSystem.cpp',
        '../src/dynamics/NeuralDynamics.cpp',
        '../src/plasticity/PlasticityRule.cpp',
        '../src/plasticity/STDP.cpp',
        '../src/plasticity/Hebbian.cpp',
        '../src/plasticity/StructuralPlasticity.cpp',
        '../src/development/DevelopmentSystem.cpp',
        '../src/development/Synaptogenesis.cpp',
        '../src/development/Pruning.cpp',
        '../src/development/Maturation.cpp',
        '../src/sensory/SensoryInput.cpp',
        '../src/sensory/Vision.cpp',
        '../src/sensory/Audio.cpp',
        '../src/sensory/InternalSignals.cpp',
        '../src/motor/MotorSystem.cpp',
        '../src/motor/Action.cpp',
        '../src/environment/Environment.cpp',
        '../src/environment/Observation.cpp',
        '../src/experiments/Experiment.cpp',
        '../src/experiments/ExperimentRunner.cpp',
        '../src/experiments/Metrics.cpp',
        '../src/experiments/Phase3Experiment.cpp',
        '../src/experiments/Phase4Experiment.cpp',
        '../src/experiments/Phase5Experiment.cpp',
        '../src/experiments/Phase6IntegratedExperiment.cpp',
        '../src/experiments/Phase3Demo.cpp',
        '../src/experiments/Phase4Demo.cpp',
        '../src/experiments/Phase5Demo.cpp',
        '../src/experiments/Phase6Demo.cpp',
        '../src/visualization/VisualizationInterface.cpp',
        '../src/world/Observation.cpp',
        '../src/benchmark.cpp'
    ],
    include_dirs=[
        '../include',
        '../src',
        '../src/core',
        '../src/brain',
        '../src/agent',
        '../src/memory',
        '../src/prediction',
        '../src/cognition',
        '../src/neuromodulation',
        '../src/sensory',
        '../src/motor',
        '../src/environment',
        '../src/world',
        '../src/plasticity',
        '../src/dynamics',
        '../src/development',
        '../src/experiments',
        '../src/visualization',
        '../src/performance'
    ],
try:
    with open('../README.md', 'r') as f:
        long_description = f.read()
except:
    long_description = "NLM (熙然) - Advanced Neural Learning Machine"

# Read version from pyproject.toml
try:
    with open('../pyproject.toml', 'r') as f:
        import re
        content = f.read()
        version_match = re.search(r'version\s*=\s*"([^"]+)"', content)
        version = version_match.group(1) if version_match else "0.1.0"
except:
    version = "0.1.0"

setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    author_email="research@nlm.org",
    description="Python bindings for NLM (Neural Learning Machine) - Advanced Artificial Neural Simulation",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=['pynlm'],
    ext_modules=[nlm_extension],
    cmdclass={'build_ext': PyNLMBuildExt},
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neural Networks",
        "Topic :: Software Development :: Libraries :: Python Modules"
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
        "pybind11>=2.11.0",
    ],
    setup_requires=["pybind11>=2.11.0"],
    tests_require=["pytest"],
    test_suite="tests",
    keywords="neural simulation spiking neural network artificial intelligence cognitive modeling memory learning prediction",
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source": "https://github.com/nlm-project/nlm",
        "Tracker": "https://github.com/nlm-project/nlm/issues"
    },
    license="MIT",
    zip_safe=False,
    options={'build_ext': {'compiler_args': ['-std=c++20', '-O2']}}
)