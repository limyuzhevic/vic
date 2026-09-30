# NLM Python Package Configuration
# This file defines the structure and dependencies for the pynlm Python package

import os
import sys
import platform
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.version import LooseVersion
import subprocess
import warnings

# Try to read the README for long description
try:
    with open("README.md", "r", encoding="utf-8") as f:
        long_description = f.read()
except FileNotFoundError:
    long_description = "Python bindings for the NLM (Neural Learning Machine) neural simulation framework"

# Package metadata
NAME = "pynlm"
VERSION = "0.1.0"
DESCRIPTION = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"
AUTHOR = "NLM Authors"
EMAIL = "contact@nlm-project.org"
URL = "https://github.com/nlm-project/nlm"
LICENSE = "MIT"

# Dependencies
REQUIRED = [
    "numpy>=1.20",
]

DEV_REQUIRED = [
    "pytest>=7.0",
    "numpy>=1.20",
    "scikit-build-core>=0.5.0",
    "pybind11>=2.11.0",
]

# Classifiers for PyPI
CLASSIFIERS = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: C++",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Scientific/Engineering :: Neuroscience",
    "Topic :: Software Development :: Libraries",
    "Topic :: System :: Emulators",
    "Operating System :: POSIX :: Linux",
    "Operating System :: MacOS",
    "Operating System :: Microsoft :: Windows",
]

# Python package structure
PACKAGES = [
    "pynlm",
    "pynlm.test",
]

# Package data
PACKAGE_DATA = {
    "pynlm": [
        "*.py",
        "*.pyx",
        "*.pxd",
    ],
}

# Entry points for command line utilities
ENTRY_POINTS = {
    "console_scripts": [
        "nlm-explore = pynlm.cli:main",
        "nlm-sim = pynlm.cli:run_simulation",
        "nlm-analyze = pynlm.cli:analyze_results",
    ],
}

# Custom build extension to check for C++ compiler
class BuildExtension(build_ext):
    """Custom build extension with C++ compiler checks."""
    
    def run(self):
        # Check for C++ compiler
        compiler = self._get_compiler()
        if not compiler:
            raise RuntimeError(
                "C++ compiler not found. Please install a C++ compiler "
                "(e.g., g++ on Linux, clang on macOS, or MSVC on Windows)"
            )
        
        # Check CMake version
        try:
            subprocess.run(
                ["cmake", "--version"],
                check=True,
                capture_output=True,
                text=True
            )
        except (subprocess.CalledProcessError, FileNotFoundError):
            warnings.warn(
                "CMake not found. Build may fail without CMake. "
                "Consider installing CMake if you encounter build errors.",
                UserWarning
            )
        
        # Call parent implementation
        super().run()
    
    def _get_compiler(self):
        """Detect available C++ compiler."""
        # Check environment variables
        if "CXX" in os.environ:
            return os.environ["CXX"]
        
        # Try to detect compiler based on platform
        if platform.system() == "Windows":
            # Check for MSVC
            try:
                subprocess.run(
                    ["cl", "/?"],
                    shell=True,
                    check=False,
                    capture_output=True,
                    text=True
                )
                return "cl"
            except:
                pass
        else:
            # Unix-like systems (Linux, macOS)
            for compiler in ["g++", "clang++"]:
                try:
                    subprocess.run(
                        [compiler, "--version"],
                        check=True,
                        capture_output=True,
                        text=True
                    )
                    return compiler
                except (subprocess.CalledProcessError, FileNotFoundError):
                    continue
        
        return None

# Custom install to set up additional files
class InstallCommand(install):
    """Custom install command to set up additional files."""
    
    def run(self):
        # Call parent install
        super().run()
        
        # Create example scripts
        self._create_examples()
    
    def _create_examples(self):
        """Create example scripts for users."""
        examples_dir = os.path.join(self.package_dir, "pynlm", "examples")
        
        if not os.path.exists(examples_dir):
            os.makedirs(examples_dir)
        
        # Example 1: Basic brain simulation
        basic_example = '''#!/usr/bin/env python3
"""Basic NLM brain simulation example."""

import pynlm
import time

def main():
    print("=== Basic NLM Brain Simulation ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"Step {step}: {firing} neurons firing, "
                  f"{rate:.2f} Hz average rate")
    
    print("Simulation complete!")

if __name__ == "__main__":
    main()
'''
        
        with open(os.path.join(examples_dir, "basic_simulation.py"), "w") as f:
            f.write(basic_example)
        
        # Example 2: Agent in world
        agent_example = '''#!/usr/bin/env python3
"""NLM agent simulation example."""

import pynlm
import time

def main():
    print("=== NLM Agent Simulation ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running agent simulation...")
    
    # Run simulation
    for step in range(200):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log periodically
        if step % 50 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Agent simulation complete!")

if __name__ == "__main__":
    main()
'''
        
        with open(os.path.join(examples_dir, "agent_simulation.py"), "w") as f:
            f.write(agent_example)
        
        # Make scripts executable
        os.chmod(os.path.join(examples_dir, "basic_simulation.py"), 0o755)
        os.chmod(os.path.join(examples_dir, "agent_simulation.py"), 0o755)
        
        print(f"Created example scripts in {examples_dir}/")

# Platform-specific configuration
if platform.system() == "Windows":
    # Windows-specific compiler flags
    EXTRA_COMPILE_ARGS = [
        "/W4",  # Level 4 warnings
        "/permissive-",  # Strict C++ standard compliance
    ]
    
    EXTRA_LINK_ARGS = [
        "/LIBPATH:%s" % os.environ.get("LIB", ""),
    ]
else:
    # Unix-like systems compiler flags
    EXTRA_COMPILE_ARGS = [
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-std=c++20",
        "-O3",  # Optimization
    ]
    
    EXTRA_LINK_ARGS = [
        "-lm",  # Math library
    ]

# Readme for PyPI
try:
    with open("README.md", "r", encoding="utf-8") as f:
        long_description = f.read()
except FileNotFoundError:
    long_description = DESCRIPTION

# Main setup function
setup(
    name=NAME,
    version=VERSION,
    description=DESCRIPTION,
    long_description=long_description,
    long_description_content_type="text/markdown",
    author=AUTHOR,
    author_email=EMAIL,
    url=URL,
    license=LICENSE,
    packages=PACKAGES,
    package_data=PACKAGE_DATA,
    install_requires=REQUIRED,
    extras_require={
        "dev": DEV_REQUIRED,
        "test": DEV_REQUIRED,
    },
    classifiers=CLASSIFIERS,
    cmdclass={
        "build_ext": BuildExtension,
        "install": InstallCommand,
    },
    ext_modules=[],  # Extensions are built by scikit-build
    zip_safe=True,
    entry_points=ENTRY_POINTS,
    python_requires=">=3.8",
    project_urls={
        "Bug Reports": f"{URL}/issues",
        "Documentation": f"{URL}/blob/main/python/README.md",
        "Source Code": URL,
        "Discussions": f"{URL}/discussions",
    },
    keywords=[
        "neural", "simulation", "spiking", "brain", "learning",
        "artificial intelligence", "neuroscience", "computational neuroscience",
        "neural networks", "brain simulation", "neuromorphic"
    ],
)
