"""pynlm - Python interface to NLM brain simulation

This module provides a Pythonic interface to the high-performance C++ brain simulation
engine, enabling research and experimentation with artificial developmental systems.

Main modules:
- brain: Core brain simulation classes
- world: Environment and simulation world
- agent: Agent brain interface for perception-action loops
- config: Configuration system
- memory: Memory system interfaces
"""

from .brain import (
    Brain,
    createBrain,
    createDefaultConfig,
)
from .world import (
    SimpleWorld,
    createSimpleWorld,
    createAgentWorld,
)
from .agent import (
    AgentBrain,
    createAgentBrain,
    processSensoryPercept,
    decodeMotorCommand,
)
from .config import Config
from .exceptions import NLMError, ConfigurationError, SimulationError
from .utils import (
    get_version,
    get_build_info,
    validate_config,
    print_system_info,
)

__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "contact@nlm.org"
__license__ = "MIT"

__all__ = [
    # Core brain classes
    "Brain",
    "createBrain",
    "createDefaultConfig",
    
    # World and environment
    "SimpleWorld",
    "createSimpleWorld",
    "createAgentWorld",
    
    # Agent interface
    "AgentBrain",
    "createAgentBrain",
    "processSensoryPercept",
    "decodeMotorCommand",
    
    # Configuration
    "Config",
    
    # Utilities
    "get_version",
    "get_build_info",
    "validate_config",
    "print_system_info",
    
    # Exceptions
    "NLMError",
    "ConfigurationError",
    "SimulationError",
]

# Initialize module-level imports
try:
    from . import brain
    from . import world
    from . import agent
    from . import config
    from . import memory
    from . import plasticity
    from . import cognition
    from . import neuromodulation
    from . import development
except ImportError as e:
    import warnings
    warnings.warn(f"Optional module import failed: {e}")
