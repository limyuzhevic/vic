"""
NLM API Utilities

Utility functions and helpers for NLM usage.
"""

from typing import Any, Callable, Dict, List, Optional, Tuple
from .exceptions import NLMBrainError

# Example scripts for different use cases
EXAMPLE_SCRIPTS = {
    'basic_brain': '''
import pynlm

# Create and initialize brain
brain = pynlm.createSimpleBrain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
''',
    
    'agent_world': '''
import pynlm

# Create complete agent setup
brain, world, agent = pynlm.create_explorer()

# Run episode
pynlm.run_reproduction_experiment(brain, world, agent, steps=500)
''',
    
    'memory_experiment': '''
import pynlm

# Create brain with memory systems
brain = pynlm.createBrain(pynlm.createConfig({
    'neuromodulation': {'enable_memory': True},
    'brain': {'neuron_count': 2000}
}))

brain.enableWorkingMemory(True)
brain.enableEpisodicMemory(True)

# Run experiment
brain.initialize()
for step in range(200):
    brain.step(step)

print(f"Memory experiment: {brain.getEpisodicMemoryCount()} episodes recorded")
''',
    
    'development_study': '''
import pynlm

# Create brain with development
brain = pynlm.createBrain(pynlm.createConfig({
    'development': {'enable_development': True},
    'brain': {'neuron_count': 3000}
}))

brain.initialize()

# Track development
stages = []
for step in range(300):
    brain.step(step)
    stage = brain.getDevelopmentStage()
    if stage not in stages:
        stages.append(stage)
    
print(f"Development progression: {' -> '.join(stages)}")
''',
    
    'prediction_test': '''
import pynlm

# Create brain with prediction
brain = pynlm.createBrain(pynlm.createConfig({
    'neuromodulation': {'enable_prediction': True},
    'brain': {'neuron_count': 2000}
}))

brain.initialize()
brain.enablePredictionSystem(True)

# Test prediction
state = [0.1] * 50
prediction = brain.predictNextAction(state)
prediction_error = brain.getPredictionError()

print(f"Prediction test: Error = {prediction_error:.4f}")
''',
}

# Simulation presets
SIMULATION_PRESETS = {
    'fast_test': {
        'steps': 100,
        'timestep': 0.1,
        'description': 'Quick test run (100 steps)',
    },
    'standard_test': {
        'steps': 1000,
        'timestep': 0.01,
        'description': 'Standard test run (1000 steps)',
    },
    'long_experiment': {
        'steps': 10000,
        'timestep': 0.001,
        'description': 'Long-term experiment (10000 steps)',
    },
    'memory_focus': {
        'steps': 2000,
        'timestep': 0.01,
        'description': 'Memory system focus test',
    },
    'development_focus': {
        'steps': 3000,
        'timestep': 0.02,
        'description': 'Development system focus test',
    },
}

# Action presets
ACTION_PRESETS = {
    'safe': {
        'max_energy': 100.0,
        'energy_decay_rate': 0.05,
        'reward_for_resource': 1.0,
        'penalty_for_hazard': -1.0,
    },
    'adventure': {
        'max_energy': 200.0,
        'energy_decay_rate': 0.02,
        'reward_for_resource': 2.0,
        'penalty_for_hazard': -3.0,
    },
    'survival': {
        'max_energy': 50.0,
        'energy_decay_rate': 0.1,
        'reward_for_resource': 0.5,
        'penalty_for_hazard': -5.0,
    },
}

def get_example_script(name: str) -> str:
    """
    Get an example script by name.
    
    Args:
        name: Name of the example script
        
    Returns:
        Example script code
        
    Raises:
        KeyError: If example script not found
    """
    if name not in EXAMPLE_SCRIPTS:
        raise KeyError(f"Example script '{name}' not found. Available: {list(EXAMPLE_SCRIPTS.keys())}")
    return EXAMPLE_SCRIPTS[name]


def get_simulation_preset(name: str) -> Dict[str, Any]:
    """
    Get a simulation preset.
    
    Args:
        name: Preset name
        
    Returns:
        Simulation preset configuration
        
    Raises:
        KeyError: If preset not found
    """
    if name not in SIMULATION_PRESETS:
        raise KeyError(f"Simulation preset '{name}' not found. Available: {list(SIMULATION_PRESETS.keys())}")
    return SIMULATION_PRESETS[name].copy()


def get_action_preset(name: str) -> Dict[str, Any]:
    """
    Get an action preset.
    
    Args:
        name: Preset name
        
    Returns:
        Action preset configuration
        
    Raises:
        KeyError: If preset not found
    """
    if name not in ACTION_PRESETS:
        raise KeyError(f"Action preset '{name}' not found. Available: {list(ACTION_PRESETS.keys())}")
    return ACTION_PRESETS[name].copy()


def format_performance_metrics(metrics: Dict[str, Any]) -> str:
    """
    Format performance metrics for display.
    
    Args:
        metrics: Performance metrics dictionary
        
    Returns:
        Formatted metrics string
    """
    lines = [
        "=== Performance Metrics ===",
        f"Total Spikes: {metrics.get('totalSpikes', 0):,}",
        f"Firing Neurons: {metrics.get('firingNeurons', 0):,}",
        f"Average Firing Rate: {metrics.get('averageFiringRate', 0.0):.2f} Hz",
        f"Working Memory Traces: {metrics.get('workingMemoryTraces', 0):,}",
        f"Episodic Memory Episodes: {metrics.get('episodicMemoryEpisodes', 0):,}",
        f"Curiosity Level: {metrics.get('curiosityLevel', 0.0):.3f}",
        f"Novelty Level: {metrics.get('noveltyLevel', 0.0):.3f}",
        f"Prediction Error: {metrics.get('predictionError', 0.0):.4f}",
    ]
    
    if 'wallClockTime' in metrics:
        lines.append(f"Wall Clock Time: {metrics['wallClockTime']:.2f}s")
    
    return '\n'.join(lines)


def create_world_config(width: int = 20, height: int = 20, 
                       vision_width: int = 8, vision_height: int = 8,
                       preset: Optional[str] = None) -> Dict[str, Any]:
    """
    Create world configuration.
    
    Args:
        width: World width
        height: World height
        vision_width: Agent vision width
        vision_height: Agent vision height
        preset: Action preset name
        
    Returns:
        World configuration dictionary
    """
    config = {
        'width': width,
        'height': height,
        'visionWidth': vision_width,
        'visionHeight': vision_height,
        'maxEnergy': 100.0,
        'energyDecayRate': 0.05,
        'rewardForResource': 1.0,
        'penaltyForHazard': -1.0,
    }
    
    if preset:
        action_preset = get_action_preset(preset)
        config.update(action_preset)
    
    return config


def validate_agent_setup(brain, world, agent) -> bool:
    """
    Validate that an agent setup is properly configured.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: Agent instance
        
    Returns:
        True if setup is valid, False otherwise
    """
    try:
        # Check brain
        if not brain or brain.getTotalNeuronCount() == 0:
            return False
        
        # Check world
        if not world or world.getWidth() <= 0 or world.getHeight() <= 0:
            return False
        
        # Check agent
        if not agent:
            return False
        
        return True
    except Exception:
        return False


def print_welcome_message():
    """Print welcome message for new users."""
    print("=" * 60)
    print("NLM (Neural Learning Machine) - Enhanced Python API")
    print("=" * 60)
    print("\nWelcome to NLM!")
    print("\nGetting started:")
    print("  1. Import the library: import pynlm")
    print("  2. Create a simple brain: brain = pynlm.createSimpleBrain()")
    print("  3. Initialize: brain.initialize()")
    print("  4. Run simulation: brain.runSteps(100)")
    print("\nFor examples and documentation, see the examples directory.")
    print("=" * 60)


def quick_help() -> str:
    """Get quick help information."""
    return """
NLM Quick Help:

Basic Usage:
  import pynlm
  brain = pynlm.createSimpleBrain()
  brain.initialize()
  brain.runSteps(100)

Presets:
  brain = pynlm.create_explorer()  # Advanced learning
  brain = pynlm.create_developer() # Development focus
  brain = pynlm.create_researcher() # Research capabilities

Memory Systems:
  brain.enableWorkingMemory(True)
  brain.enableEpisodicMemory(True)

Prediction:
  brain.enablePredictionSystem(True)
  prediction = brain.predictNextAction(state)

Configuration:
  config = pynlm.createConfig({'brain': {'neuron_count': 5000}})
  config.load_preset('researcher')

Examples:
  See examples/ directory for complete working examples.
"""

# Common error messages
ERROR_MESSAGES = {
    'brain_not_initialized': 'Brain must be initialized before use',
    'invalid_step': 'Step number must be non-negative',
    'insufficient_neurons': 'Brain has insufficient neurons for operation',
    'memory_not_enabled': 'Memory system is not enabled',
    'prediction_not_enabled': 'Prediction system is not enabled',
    'invalid_preset': 'Unknown preset name',
    'configuration_error': 'Configuration error occurred',
}