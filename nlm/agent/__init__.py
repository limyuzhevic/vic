"""
NLM Agent Module

Agent system for the NLM neural simulation framework.
Includes agent brain interface, body state, and motor decoding systems.
"""

from .agent_brain import AgentBrain
from .agent_body import AgentBody
from .sensory_percept import SensoryPercept

__all__ = [
    'AgentBrain',
    'AgentBody',
    'SensoryPercept',
]

# Python utilities for agent module
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
)

# Re-export utilities
__all__.extend([
    'PythonSessionLogger',
    'PythonTimer',
])

# Agent utility functions

def create_default_agent_config():
    """Create a default configuration for agent simulation"""
    from nlm.core import Config
    
    config = Config()
    config.set("enable_reward_modulation", True)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", False)
    config.set("brain_name", "default_agent_brain")
    
    return config

def validate_agent_config(config):
    """Validate agent configuration"""
    from nlm.core import Config
    
    if not isinstance(config, Config):
        raise TypeError("config must be a Config instance")
    
    errors = []
    
    # Check required keys
    required_keys = ["enable_reward_modulation", "enable_structural_plasticity",
                    "enable_development", "enable_curiosity"]
    
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required configuration: {key}")
    
    # Validate value types
    for key in required_keys:
        if config.has(key):
            value = config.get(key)
            if not isinstance(value, bool):
                errors.append(f"{key} must be a boolean")
    
    return errors

# Module initialization
def _init_module():
    """Initialize the agent module"""
    try:
        logger = PythonSessionLogger()
        logger.logEntry("Agent module initialized successfully")
    except Exception:
        pass  # Module may not have logger available yet

# Initialize module
_init_module()

# Command-line interface utilities
def export_agent(agent_brain, filepath):
    """
    Export agent state to file.
    
    Args:
        agent_brain: AgentBrain object to export
        filepath: Path to export file
    """
    import json
    import os
    
    try:
        # Create export data
        export_data = {
            "agent_brain": {
                "reward_modulation_enabled": agent_brain.isRewardModulationEnabled(),
                "structural_plasticity_enabled": agent_brain.isStructuralPlasticityEnabled(),
                "development_enabled": agent_brain.isDevelopmentEnabled(),
                "curiosity_enabled": agent_brain.isCuriosityEnabled(),
                "neuromodulation_level": agent_brain.getNeuromodulationLevel(),
                "curiosity_level": agent_brain.getCuriosityLevel(),
                "novelty_level": agent_brain.getNoveltyLevel(),
                "prediction_error": agent_brain.getPredictionError(),
                "developmental_stage": agent_brain.getDevelopmentalStage().name,
            }
        }
        
        # Save to JSON file
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return True
        
    except Exception as e:
        print(f"Failed to export agent: {e}")
        return False

def import_agent(filepath):
    """
    Import agent state from file.
    
    Args:
        filepath: Path to import file
        
    Returns:
        AgentBrain object or None if import failed
    """
    import json
    import os
    
    try:
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return None
        
        # Load from JSON file
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Create agent brain with imported configuration
        from nlm.core import Config
        
        agent_config = Config()
        
        # Set configuration from import data
        if "agent_brain" in data:
            brain_data = data["agent_brain"]
            for key, value in brain_data.items():
                # Convert keys to proper format
                config_key = key.replace('_', '')
                if isinstance(value, str):
                    agent_config.set(config_key, value)
                elif isinstance(value, (int, float)):
                    agent_config.set(config_key, value)
                elif isinstance(value, bool):
                    agent_config.set(config_key, value)
        
        # Create agent brain (placeholder)
        # agent_brain = AgentBrain(agent_config)
        agent_brain = None
        
        return agent_brain
        
    except Exception as e:
        print(f"Failed to import agent: {e}")
        return None

__all__.extend([
    'create_default_agent_config',
    'validate_agent_config',
    'export_agent',
    'import_agent',
])
