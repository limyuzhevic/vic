"""
NLM (Neural Learning Machine) Python Package
=============================================

A Python interface for the NLM neural simulation framework.

This package provides a user-friendly Python API to the NLM C++ neural
simulation system, making it accessible for experimentation and research
in computational neuroscience and artificial intelligence.
"""

import sys
import os

# Try to import the main module
try:
    from .pynlm import *
    from .nlm_examples import *
except ImportError:
    # Fallback to direct imports if not in package context
    pass

# Package metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"
__description__ = "Neural Learning Machine - Python Interface"

# Define package exports
__all__ = [
    # Core classes from pynlm.py
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    'SensoryPercept',
    'AgentBody',
    'ActionResult',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'WorldObjectType',
    'MotorCommand',
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'get_version',
    'check_cpp_module',
    # Builder classes
    'ConfigBuilder',
    'BrainBuilder',
    'AgentBuilder',
    'WorldBuilder',
    'SimulationRunner',
    # Configuration
    'DEFAULT_CONFIG',
    # Examples
    'run_example_simulation',
    'demo_neural_network',
    # Example functions
    'run_silent_brain_example',
    'run_agent_world_example',
    'run_learning_demonstration',
    'run_curiosity_example',
    'run_plasticity_demo',
    'create_visualization_data',
    'plot_simulation_results',
]

# Initialize module when imported
if __name__ != "__main__":
    try:
        # Check if C++ module is available
        from .pynlm import check_cpp_module
        if not check_cpp_module():
            print("Warning: C++ module not available. Some functionality may be limited.")
            print("Install NLM with: pip install -e .")
    except ImportError:
        pass

# Set up Python path
package_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, package_dir)

# Provide helpful error messages
def _check_environment():
    """Check if the environment is properly set up."""
    issues = []
    
    # Check for C++ module
    try:
        from .pynlm import check_cpp_module
        if not check_cpp_module():
            issues.append("C++ module not available. Install with: pip install -e .")
    except ImportError:
        issues.append("Could not import pynlm module")
    
    # Check for required dependencies
    try:
        import numpy
    except ImportError:
        issues.append("numpy not installed. Install with: pip install numpy")
    
    try:
        import matplotlib
    except ImportError:
        issues.append("matplotlib not installed. Install with: pip install matplotlib")
    
    if issues:
        print("Environment setup issues:")
        for issue in issues:
            print(f"  - {issue}")
        print("\nPlease fix these issues before running simulations.")
    else:
        print("Environment check passed. All dependencies available.")


if __name__ == "__main__":
    print("NLM Python Package")
    print("==================")
    print("\nThis package provides the NLM neural simulation framework Python interface.")
    print("\nFor usage information, see the documentation and examples.")
    print("\nTo run examples:")
    print("  python -m nlm.nlm_examples")
    print("\nTo check environment:")
    print("  python -c 'import nlm; nlm._check_environment()'")