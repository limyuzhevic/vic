# This is a minimal __init__.py that will be replaced by the actual Python module
# during the build process

__version__ = "0.1.0"
__author__ = "NLM Project Team"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Import the actual C++ bindings
# This file will be dynamically generated during the build process
from . import pynlm_core

# Re-export all public APIs
__all__ = [
    'NeuronId',
    'Config',
    'EnhancedConfig',
    'Brain',
    'EnhancedAgentBrain',
    'SimpleWorld',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'ConfigSource',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryPercept',
    'Action',
    'AgentBody',
    'ActionResult',
    'WorldObject',
    'Benchmark',
    'batchStep',
    'parallelBatchStep',
    'createBrainContext',
    'NLMError',
    'ConfigurationError',
    'BrainError',
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    'run_simple_simulation',
    'run_agent_world_simulation',
    'benchmark_simulation',
    'legacy_example',
    'enhanced_example',
]

# Pythonic helper functions
def __getattr__(name):
    """Lazy loading for enhanced compatibility"""
    if name in __all__:
        # This will be properly resolved during build
        try:
            from . import pynlm_core
            return getattr(pynlm_core, name)
        except ImportError:
            pass
    
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")

# Module metadata and validation
if __name__ == "__main__":
    print(f"NLM Python Bindings v{__version__}")
    print(__description__)
    print("Author:", __author__)
    print("License:", __license__)
    print("\nTo use NLM with Python, import the module:")
    print("  import pynlm")
    print("\nFor help, see documentation and examples in the repository.")
