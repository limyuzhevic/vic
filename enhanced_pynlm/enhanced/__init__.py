# enhanced_pynlm/enhanced/__init__.py
"""
Enhanced NLM Python API modules

This package provides enhanced versions of the NLM Python API with:
• Better error handling
• Context managers
• Performance monitoring
• Neural recording
• Configuration management
• Advanced agent simulation
• Analysis and visualization tools
"""

# Import base classes from enhanced.py
from .base import (
    EnhancedConfig,
    EnhancedBrain, 
    EnhancedWorld,
    EnhancedAgent,
    BrainMonitor,
    NeuralRecorder,
    ConfigManager,
    BrainAnalyzer,
    SimulationError,
    ConfigurationError,
    ResourceError
)

# Import enhanced modules
from . import (
    core,
    monitoring,
    recording,
    agent,
    analysis,
    management
)

# Re-export key enhanced classes
from .core import CoreEnhanced
from .monitoring import AdvancedMonitor
from .recording import AdvancedRecorder
from .agent import AdvancedAgent
from .analysis import AdvancedAnalyzer
from .management import ExperimentManager

# Package metadata
__version__ = "2.0.0"
__description__ = "Enhanced NLM Python API with advanced features"

# Re-export enhanced classes for convenience
__all__ = [
    # Core enhanced classes
    'EnhancedConfig',
    'EnhancedBrain',
    'EnhancedWorld', 
    'EnhancedAgent',
    'BrainMonitor',
    'NeuralRecorder',
    'ConfigManager',
    'BrainAnalyzer',
    
    # Enhanced modules
    'CoreEnhanced',
    'AdvancedMonitor',
    'AdvancedRecorder',
    'AdvancedAgent',
    'AdvancedAnalyzer',
    'ExperimentManager',
    
    # Error classes
    'SimulationError',
    'ConfigurationError', 
    'ResourceError'
]

# Utility functions for easy access
def create_enhanced_session(config=None, monitor=None):
    """Create an enhanced simulation session."""
    from .base import EnhancedBrain
    config = config or EnhancedConfig()
    
    with EnhancedBrain(config) as brain:
        if monitor:
            monitor.start_monitoring()
            yield brain, monitor
        else:
            yield brain, None

def run_enhanced_experiment(experiment_func, config=None, iterations=1):
    """Run an enhanced experiment."""
    from .base import EnhancedBrain
    config = config or EnhancedConfig()
    results = []
    
    for i in range(iterations):
        with EnhancedBrain(config) as brain:
            result = experiment_func(brain, i)
            results.append(result)
    
    return results

print(f"Enhanced NLM API v{__version__} loaded successfully!")
print(f"Available: {len(__all__)} enhanced modules and classes")
