import setuptools
import sys
import os

# Check if we're building the Python extension
if "build_ext" in sys.argv or "bdist_wheel" in sys.argv:
    # Build the C++ extension
    from setuptools.command.build_ext import build_ext
    from pybind11.setup_helpers import Pybind11Extension, parallel_build_ext

    # Define the extension module
    extensions = [
        Pybind11Extension(
            "pynlm",
            sources=[
                "python/bindings.cpp",
                "src/brain/Brain.cpp",
                "src/brain/Neuron.cpp",
                "src/brain/Synapse.cpp",
                "src/brain/NeuralRegion.cpp",
                "src/brain/NeuralPopulation.cpp",
                "src/core/Config/Config.cpp",
                "src/core/Random/Random.cpp",
                "src/core/Logger/Logger.cpp",
                "src/core/SimulationClock/SimulationClock.cpp",
                "src/core/Types/Types.cpp",
                "src/sensory/Vision.cpp",
                "src/sensory/SensoryInput.cpp",
                "src/sensory/InternalSignals.cpp",
                "src/sensory/Audio.cpp",
                "src/motor/Action.cpp",
                "src/motor/MotorSystem.cpp",
                "src/environment/Environment.cpp",
                "src/world/SimpleWorld.cpp",
                "src/world/Observation.cpp",
                "src/agent/AgentBrain.cpp",
                "src/agent/SensoryPercept.cpp",
                "src/neuromodulation/Neuromodulator.cpp",
                "src/neuromodulation/Dopamine.cpp",
                "src/neuromodulation/Curiosity.cpp",
                "src/neuromodulation/Novelty.cpp",
                "src/neuromodulation/PredictionError.cpp",
                "src/prediction/PredictionSystem.cpp",
                "src/prediction/NeuralPrediction.cpp",
                "src/cognition/NeuralPlanner.cpp",
                "src/cognition/ConceptFormation.cpp",
                "src/memory/NeuralWorkingMemory.cpp",
                "src/memory/NeuralEpisodicMemory.cpp",
                "src/plasticity/STDP.cpp",
                "src/plasticity/Hebbian.cpp",
                "src/plasticity/StructuralPlasticity.cpp",
                "src/development/DevelopmentSystem.cpp",
                "src/dynamics/SpikeSystem.cpp",
                "src/dynamics/NeuralDynamics.cpp",
                "src/experiments/Phase6IntegratedExperiment.cpp",
            ],
            include_dirs=["src", "include", "/usr/include/python3.12"],
            define_macros=[("VERSION_INFO", "1.0")],
            cxx_std=17,
        )
    ]

    class CustomBuildExt(build_ext):
        def run(self):
            # Use parallel build for faster compilation
            parallel_build_ext(self)
            super().run()

    setup(
        name="pynlm",
        version="0.1.0",
        author="NLM Authors",
        description="Python bindings for NLM (Neural Learning Machine)",
        long_description="A Python binding for the NLM C++ neural simulation framework.",
        long_description_content_type="text/markdown",
        ext_modules=extensions,
        cmdclass={"build_ext": CustomBuildExt},
        zip_safe=False,
        python_requires=">=3.8",
        install_requires=["numpy>=1.20"],
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Intended Audience :: Science/Research",
            "License :: OSI Approved :: MIT License",
            "Programming Language :: Python :: 3",
            "Programming Language :: C++",
        ],
    )
else:
    # For documentation purposes only
    setup(
        name="pynlm",
        version="0.1.0",
        author="NLM Authors",
        description="Python bindings for NLM (Neural Learning Machine)",
    )
