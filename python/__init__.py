# NLM Python Package
# NLM (Neural Learning Machine) Python Bindings

__version__ = "0.1.0"
__author__ = "NLM Authors" 
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import the pybind11 compiled module - this is the actual C++ bindings module
# The module is named 'pynlm' and is compiled from python/bindings.cpp
import importlib
import sys

# Try to import the compiled module, with fallback for development
_pynlm_module = None
try:
    # First try to import the compiled module
    _pynlm_module = importlib.import_module('pynlm')
except ImportError:
    # If not available, try to compile or raise informative error
    try:
        # Try to build the extension
        import subprocess
        import os
        
        # Change to python directory
        python_dir = os.path.dirname(os.path.abspath(__file__))
        
        # Try to build using scikit-build
        build_dir = os.path.join(python_dir, 'build')
        if not os.path.exists(build_dir):
            os.makedirs(build_dir, exist_ok=True)
            
        # This assumes the user has scikit-build-core and pybind11 installed
        # In a full installation, this would be pre-compiled
        print("WARNING: pynlm module not found. This is expected in development.")
        print("The module should be compiled using scikit-build-core:")
        print(f"  cd {python_dir}")
        print("  pip install -e .")
        
    except Exception as e:
        print(f"Could not build pynlm module: {e}")

# Create a proxy module that provides easy access to the C++ bindings
class _NLMModuleProxy:
    """Proxy class to provide easy access to NLM C++ bindings."""
    
    def __getattr__(self, name):
        if _pynlm_module is None:
            raise ImportError(
                "pynlm module not available. Please install it using: "
                "pip install -e . from the project root directory."
            )
        return getattr(_pynlm_module, name)
    
    def __dir__(self):
        if _pynlm_module is None:
            return []
        return dir(_pynlm_module)

# Create proxy module
pynlm = _NLMModuleProxy()

# Import utilities module
try:
    from .nlm_utils import *
except ImportError:
    # Define minimal fallback if utils module is not available
    pass

# Factory functions (wrappers for C++ functions)

def create_default_config():
    """Create a default NLM configuration."""
    if _pynlm_module is not None:
        return _pynlm_module.createDefaultConfig()
    else:
        raise ImportError("pynlm module not available")

def create_brain(config=None):
    """Create a new NLM brain with the given configuration."""
    if _pynlm_module is not None:
        if config is None:
            config = create_default_config()
        return _pynlm_module.createBrain(config)
    else:
        raise ImportError("pynlm module not available")

def create_simple_world():
    """Create a new simple world for NLM simulation."""
    if _pynlm_module is not None:
        return _pynlm_module.createSimpleWorld()
    else:
        raise ImportError("pynlm module not available")

def create_agent_brain(brain):
    """Create an agent brain interface for a given brain."""
    if _pynlm_module is not None:
        return _pynlm_module.createAgentBrain(brain)
    else:
        raise ImportError("pynlm module not available")

# Convenience constants
PHASE_6_FINAL = True  # All systems integrated

# Expose all C++ bindings classes and functions at module level
__all__ = [
    # Import all attributes from the C++ module
]

# Set module docstring
__doc__ = """NLM (Neural Learning Machine) Python Bindings

A Python interface to the NLM C++ neural simulation framework.

The NLM brain is a closed-loop artificial neural system that:
- Simulates biological neurons with spiking dynamics
- Learns through experience using plasticity rules  
- Maintains memory systems for working, episodic, and associative memory
- Uses neuromodulation (dopamine, curiosity, novelty) to guide learning
- Plans actions and interacts with simulated environments

Quick usage example:

    import pynlm
    
    # Create a brain
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Brain created with {brain.get_total_neuron_count()} neurons")

For more information, see:
- docs/ARCHITECTURE.md: Technical architecture
- easy_usage.md: Beginner's guide
- docs/PHASE6_FINAL_AUDIT.md: Final integration results

Available factory functions:
- pynlm.create_default_config()
- pynlm.create_brain(config)
- pynlm.create_simple_world()
- pynlm.create_agent_brain(brain)

For utilities and helper classes, import from pynlm.nlm_utils.

For advanced usage and complete API documentation, consult the
pynlm module documentation or the C++ source headers.
"""

# Configure module imports
__import__('pynlm.nlm_utils', fromlist=['*'])

# Make common utilities available at top level
for name in ['SimulationRunner', 'ExperimentRunner', 'NLMConfig', 
             'benchmark_simulation', 'validate_configuration']:
    if hasattr(sys.modules[__name__ + '.nlm_utils'], name):
        globals()[name] = getattr(sys.modules[__name__ + '.nlm_utils'], name)
        __all__.append(name)

# Pybind11 module access for advanced users
_pynlm = _pynlm_module

