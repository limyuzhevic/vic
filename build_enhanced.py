#!/usr/bin/env python3
"""
Setup script for building enhanced NLM Python bindings with error handling and validation.

This script builds the enhanced pynlm module with:
- Comprehensive error handling and type safety
- Parameter validation and bounds checking
- Memory safety and resource management
- Better documentation and examples
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def check_python_version():
    """Check if Python version is supported."""
    version = sys.version_info
    if version.major != 3 or version.minor < 8:
        print(f"Error: Python 3.8+ required, found {version.major}.{version.minor}.{version.micro}")
        sys.exit(1)

def check_required_tools():
    """Check if required build tools are available."""
    # Check for CMake
    try:
        subprocess.run(['cmake', '--version'], check=True, capture_output=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("Error: CMake is required but not found in PATH")
        sys.exit(1)

    # Check for C++ compiler
    compiler = 'cl' if platform.system() == 'Windows' else 'g++'
    try:
        subprocess.run([compiler, '--version'], check=True, capture_output=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print(f"Error: {compiler} compiler is required but not found in PATH")
        sys.exit(1)

def build_enhanced_bindings():
    """Build the enhanced Python bindings."""
    print("Building enhanced NLM Python bindings with error handling and validation...")

    # Create build directory
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)

    # Run CMake configuration
    cmake_args = [
        'cmake', '..', 
        '-DCMAKE_BUILD_TYPE=Release',
        '-DCMAKE_CXX_STANDARD=20',
        '-DCMAKE_CXX_STANDARD_REQUIRED=ON',
        '-DPYTHON_EXECUTABLE=' + sys.executable,
        '-DPYTHON_MODULE_NAME=pynlm_enhanced'
    ]

    try:
        print(f"Running CMake configuration: {' '.join(cmake_args)}")
        subprocess.run(cmake_args, cwd=build_dir, check=True, capture_output=True, text=True)
        print("CMake configuration successful")
    except subprocess.CalledProcessError as e:
        print(f"CMake configuration failed: {e.stderr}")
        sys.exit(1)

    # Build the module
    try:
        print("Building Python module...")
        subprocess.run(['cmake', '--build', '.', '--config', 'Release', '--', '-j4'], 
                      cwd=build_dir, check=True, capture_output=True, text=True)
        print("Module build successful")
    except subprocess.CalledProcessError as e:
        print(f"Module build failed: {e.stderr}")
        sys.exit(1)

def install_enhanced_module():
    """Install the enhanced module."""
    print("Installing enhanced NLM Python module...")
    
    # Copy the built module to a location where Python can import it
    build_dir = Path("build")
    lib_dir = Path("src")
    
    # Find the built module
    if platform.system() == "Windows":
        module_name = "pynlm_enhanced.cp311-win_amd64.pyd"
    elif platform.system() == "Darwin":
        module_name = "pynlm_enhanced.cpython-311-darwin.so"
    else:
        module_name = "pynlm_enhanced.cpython-311-x86_64-linux-gnu.so"
    
    module_path = build_dir / "pynlm_enhanced" / module_name
    
    if not module_path.exists():
        # Try to find any .so or .pyd file
        for ext in ['*.so', '*.pyd']:
            matches = list(build_dir.rglob(ext))
            if matches:
                module_path = matches[0]
                break
    
    if not module_path.exists():
        print(f"Error: Built module not found at {module_path}")
        sys.exit(1)
    
    # Create the installation directory
    install_dir = Path("src/pynlm")
    install_dir.mkdir(parents=True, exist_ok=True)
    
    # Copy the module
    dest_path = install_dir / module_name
    dest_path.write_bytes(module_path.read_bytes())
    print(f"Module installed to: {dest_path}")

def create_enhanced_python_module():
    """Create the enhanced Python module files."""
    print("Creating enhanced Python module files...")
    
    # Create __init__.py for the enhanced module
    init_content = '''"""
Enhanced NLM (Neural Learning Machine) Python Bindings

This module provides enhanced Python bindings for the NLM C++ neural simulation framework
with comprehensive error handling, type safety, and validation.

Key Features:
- Comprehensive error handling with descriptive messages
- Parameter validation and bounds checking
- Type safety for all Python-C++ interactions
- Memory safety and resource management
- Better documentation and examples

Usage:
    import pynlm_enhanced as nlm
    
    # Create and initialize a brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
"""

    # Create errors.py for custom exceptions
    errors_content = '''"""
Custom exception classes for NLM Python bindings.
"""

class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Exception raised for configuration errors."""
    pass

class NeuronError(NLMError):
    """Exception raised for neuron-related errors."""
    pass

class BrainError(NLMError):
    """Exception raised for brain-related errors."""
    pass

class AgentError(NLMError):
    """Exception raised for agent-related errors."""
    pass

class WorldError(NLMError):
    """Exception raised for world-related errors."""
    pass
'''

    # Create utils.py for validation and helper functions
    utils_content = '''"""
Utility functions for enhanced NLM Python bindings.
"""

import sys
from typing import List, Dict, Any, Union

def validate_range(value: float, min: float, max: float, param_name: str) -> None:
    """Validate that a value is within specified range."""
    if not isinstance(value, (int, float)):
        raise TypeError(f"Parameter {param_name} must be numeric, got {type(value).__name__}")
    if min <= value <= max:
        return
    raise ValueError(f"Parameter {param_name} must be in range [{min}, {max}], got {value}")

def validate_positive(value: Union[int, float], param_name: str) -> None:
    """Validate that a value is positive."""
    if not isinstance(value, (int, float)):
        raise TypeError(f"Parameter {param_name} must be numeric, got {type(value).__name__}")
    if value > 0:
        return
    raise ValueError(f"Parameter {param_name} must be positive, got {value}")

def validate_non_negative(value: Union[int, float], param_name: str) -> None:
    """Validate that a value is non-negative."""
    if not isinstance(value, (int, float)):
        raise TypeError(f"Parameter {param_name} must be numeric, got {type(value).__name__}")
    if value >= 0:
        return
    raise ValueError(f"Parameter {param_name} must be non-negative, got {value}")

def validate_string_not_empty(value: str, param_name: str) -> None:
    """Validate that a string is not empty."""
    if not isinstance(value, str):
        raise TypeError(f"Parameter {param_name} must be a string, got {type(value).__name__}")
    if value:
        return
    raise ValueError(f"Parameter {param_name} cannot be empty")

def validate_vector_size(vector: List, expected_size: int, param_name: str) -> None:
    """Validate that a vector has the expected size."""
    if not isinstance(vector, list):
        raise TypeError(f"Parameter {param_name} must be a list, got {type(vector).__name__}")
    if len(vector) == expected_size:
        return
    raise ValueError(f"Parameter {param_name} must have size {expected_size}, got {len(vector)}")

def get_example_code() -> Dict[str, str]:
    """Get example code snippets for common use cases."""
    return {
        "simple_simulation": """
import pynlm_enhanced as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
""",
        "agent_example": """
import pynlm_enhanced as nlm

# Create components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Agent simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
""",
        "error_handling_example": """
import pynlm_enhanced as nlm

try:
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    print("Brain created and initialized successfully")
except nlm.BrainError as e:
    print(f"Caught BrainError: {e}")
except nlm.ConfigurationError as e:
    print(f"Caught ConfigurationError: {e}")
except nlm.NLMError as e:
    print(f"Caught NLMError: {e}")
"""
    }
'''
    
    # Create the module directory
    module_dir = Path("src/pynlm")
    
    # Write files
    (module_dir / "__init__.py").write_text(init_content)
    (module_dir / "errors.py").write_text(errors_content)
    (module_dir / "utils.py").write_text(utils_content)
    
    print("Enhanced Python module files created successfully")

def main():
    """Main build function."""
    print("=== Enhanced NLM Python Bindings Builder ===")
    print("This script builds the enhanced Python bindings with error handling and validation.")
    print()
    
    check_python_version()
    check_required_tools()
    
    # Build the enhanced bindings
    build_enhanced_bindings()
    
    # Install the enhanced module
    install_enhanced_module()
    
    # Create Python module files
    create_enhanced_python_module()
    
    print()
    print("=== Build Complete ===")
    print("Enhanced NLM Python bindings have been successfully built and installed.")
    print()
    print("To use the enhanced bindings:")
    print("  1. Import the module: import pynlm_enhanced as nlm")
    print("  2. Use the enhanced features with better error handling and validation")
    print("  3. Refer to the enhanced documentation for examples")
    print()
    print("Available features:")
    print("  - Comprehensive error handling with descriptive messages")
    print("  - Parameter validation and bounds checking")
    print("  - Type safety for all Python-C++ interactions")
    print("  - Memory safety and resource management")
    print("  - Enhanced documentation and examples")

if __name__ == "__main__":
    main()
