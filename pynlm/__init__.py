import os
from .pynlm import *
from .brain_examples import *
from .agent_examples import *
from .learning_examples import *

# Re-export key symbols
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python interface for NLM brain simulation"

# Make the module more accessible
brain = pynlm
agent = pynlm
world = pynlm

__all__ = [
    # Core classes
    'Config',
    'create_brain',
    'create_simple_world',
    'create_agent',
    'load_config_template',
    'save_config_to_file',
    'load_config_from_file',
    'create_default_config',
    'create_test_agent_simulation',
    'benchmark_performance',
    'get_available_templates',
    'get_template_info',
    
    # Configuration templates
    'DEFAULT_CONFIG',
    'CONFIG_TEMPLATES',
    
    # Examples modules
    'brain_examples',
    'agent_examples',
    'learning_examples',
    
    # Version info
    '__version__',
    '__author__',
    '__description__'
]
