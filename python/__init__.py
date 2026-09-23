import os
import sys

# Add version information
__version__ = "0.1.0"

# Define package metadata
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import the main module
try:
    from .pynlm import *
    from .nlm_types import *
    from .nlm_brain import *
    from .nlm_world import *
    from .nlm_agent import *
    from .nlm_config import *
    
    # Expose key classes and functions at package level
    __all__ = [
        # Core types
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage',
        'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
        'ConfigSource',
        
        # Main classes
        'Brain', 'AgentBrain', 'SimpleWorld',
        'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept',
        
        # Factory functions
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',
        
        # Constants
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
        
        # Convenience functions
        'initialize_nlm',
        'version',
        'get_build_info',
    ]
except ImportError as e:
    # If the C++ extension is not available, provide helpful error message
    def __getattr__(name):
        if name in __all__:
            raise ImportError(
                f"Cannot import '{name}' from 'pynlm'. "
                f"The NLM Python bindings are not properly installed. "
                f"Please install the package using pip or build it from source. "
                f"Original error: {e}"
            ) from e
        raise

# Add helpful package-level functions
def initialize_nlm(config_file=None, **kwargs):
    """
    Initialize NLM with optional configuration.
    
    Args:
        config_file (str, optional): Path to configuration file
        **kwargs: Additional configuration parameters
        
    Returns:
        Brain: An initialized Brain instance
        
    Example:
        >>> from pynlm import initialize_nlm, createDefaultConfig
        >>> brain = initialize_nlm(neuron_count=1000, region_count=5)
    """
    from .nlm_config import createDefaultConfig
    
    config = createDefaultConfig()
    
    if config_file:
        config.loadFromFile(config_file)
    
    # Apply additional configuration parameters
    for key, value in kwargs.items():
        try:
            config.set(key, value)
        except (ValueError, TypeError):
            # Ignore invalid config values
            pass
    
    from .nlm_brain import createBrain
    return createBrain(config)

def get_build_info():
    """
    Get information about the NLM build.
    
    Returns:
        dict: Dictionary containing build information
        
    Example:
        >>> from pynlm import get_build_info
        >>> info = get_build_info()
        >>> print(f"Version: {info['version']}")
    """
    return {
        'version': __version__,
        'author': __author__,
        'description': __description__,
        'platform': sys.platform,
        'python_version': f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}",
        'built_with_cpp': True,
    }
def _setup_logging(level='INFO', verbose=False):
    """Internal function to setup logging for NLM."""
    import logging
    
    if verbose:
        logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    else:
        logging.basicConfig(level=getattr(logging, level.upper()), format='%(levelname)s: %(message)s')

# Add package metadata
__package_name__ = 'pynlm'
__license__ = 'MIT'

# For backward compatibility
def __dir__():
    """Return package contents for autocomplete."""
    return __all__
