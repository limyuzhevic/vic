"""
pynlm - Neural Learning Machine Python Bindings

An experimental artificial developmental brain simulator that creates neural systems
beginning in primitive developmental states and acquiring complex abilities through environmental interaction.
"""

import sys
import os
from pathlib import Path

# Add the parent directory to path so we can import the C++ extensions
parent_dir = Path(__file__).parent.parent
sys.path.insert(0, str(parent_dir))

try:
    import pynlm_modern
    __version__ = getattr(pynlm_modern, '__version__', '1.0.0')
    
    # Import core classes
    from pynlm_modern import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage,
        FiringState, ActionType, MotorCommand, WorldObjectType,
        
        # Main classes
        Brain, Config, AgentBrain, SimpleWorld,
        
        # Sensory input types
        Vision, Audio, InternalSignals, SensoryInput, SensoryPercept,
        
        # Motor and action components
        Action, AgentBody, ActionResult,
        
        # Factory functions
        createBrainWithConfig, createWorldWithSettings, createAgentWithBrainWorld,
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    )
    
    # Modern utility functions
    from pynlm_modern import (
        validate_simulation,
        createSimulationFromDict,
        get_version,
    )
    
    # Exception classes
    from pynlm_modern import (
        NLMError, ConfigurationError, SimulationError, ValidationError,
        PyNLMError, PyConfigurationError, PySimulationError, PyValidationError,
    )
    
except ImportError as e:
    # If pynlm_modern is not available, we can still provide useful error messages
    __version__ = '1.0.0'
    pynlm_modern = None
    
    # Define placeholder classes for better error messages
    class NLMError(Exception):
        pass
    
    class ConfigurationError(NLMError):
        pass
    
    class SimulationError(NLMError):
        pass
    
    class ValidationError(NLMError):
        pass
    
    # Create placeholder classes with error messages
    class Brain:
        def __init__(self, config):
            raise ImportError(
                "Modern NLM bindings are not available. "
                "Please build the project with 'pip install -e .' or 'pip install .'\n"
                "For development, run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4\n"
                "The modern bindings provide enhanced API with better error handling, "
                "convenience methods, and improved documentation."
            )
    
    class Config:
        def __init__(self):
            raise ImportError(
                "Modern NLM bindings are not available. "
                "Please build the project with 'pip install -e .' or 'pip install .'\n"
                "The modern bindings provide enhanced configuration management "
                "with dictionary interfaces and type conversion."
            )
    
    class AgentBrain:
        def __init__(self, brain):
            raise ImportError(
                "Modern NLM bindings are not available. "
                "Please build the project with 'pip install -e .' or 'pip install .'\n"
                "The modern bindings provide enhanced agent interfaces "
                "with episode running and progress tracking."
            )
    
    class SimpleWorld:
        def __init__(self):
            raise ImportError(
                "Modern NLM bindings are not available. "
                "Please build the project with 'pip install -e .' or 'pip install .'\n"
                "The modern bindings provide enhanced world interfaces "
                "with dictionary I/O and physics updates."
            )
    
    # Placeholder functions
    def validate_simulation(brain, world, agent):
        raise ImportError("Modern NLM bindings are not available. Please build the project.")
    
    def createSimulationFromDict(config):
        raise ImportError("Modern NLM bindings are not available. Please build the project.")
    
    def get_version():
        return ("1.0.0", "NLM Research Team", "Neural Learning Machine Python Bindings (Modern)")
    
    def createBrainWithConfig(config):
        raise ImportError("Modern NLM bindings are not available. Please build the project.")
    
    def createWorldWithSettings(width, height, visionWidth, visionHeight):
        raise ImportError("Modern NLM bindings are not available. Please build the project.")
    
    def createAgentWithBrainWorld(brain, world):
        raise ImportError("Modern NLM bindings are not available. Please build the project.")

# Re-export classes for backward compatibility
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Main classes
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld',
    
    # Sensory input types
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'SensoryPercept',
    
    # Motor and action components
    'Action', 'AgentBody', 'ActionResult',
    
    # Factory functions
    'createBrainWithConfig', 'createWorldWithSettings', 'createAgentWithBrainWorld',
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Utility functions
    'validate_simulation', 'createSimulationFromDict', 'get_version',
    
    # Exception classes
    'NLMError', 'ConfigurationError', 'SimulationError', 'ValidationError',
    'PyNLMError', 'PyConfigurationError', 'PySimulationError', 'PyValidationError',
    
    # Version and metadata
    '__version__',
]

# Enhanced convenience functions for common operations
def run_simulation_episode(brain, num_steps=1000, world=None, agent=None):
    """Run a simulation episode with progress tracking.
    
    Args:
        brain: The brain object to simulate
        num_steps: Number of steps to run
        world: Optional world object for agent-based simulation
        agent: Optional agent object for processing sensory input
        
    Returns:
        Dictionary with simulation statistics
    """
    try:
        from pynlm.utils import run_with_progress
        return run_with_progress(brain, world, agent, num_steps)
    except ImportError:
        # Fallback implementation
        print(f"Starting simulation for {num_steps} steps...")
        
        for step in range(num_steps):
            if world is not None and agent is not None:
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            else:
                brain.step(step)
            
            if step % 100 == 0 and step > 0:
                print(f"Progress: {(step/num_steps)*100:.1f}% (Step {step}/{num_steps})")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        
        print("Simulation complete!")
        
        return {
            'steps_completed': num_steps,
            'total_spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
        }


def save_simulation_checkpoint(brain, filepath, description="Simulation checkpoint"):
    """Save brain state to a checkpoint file.
    
    Args:
        brain: The brain object to save
        filepath: Path to save the checkpoint file
        description: Optional description for the checkpoint
        
    Returns:
        Dictionary with checkpoint metadata
    """
    try:
        from pynlm.utils import save_checkpoint
        return save_checkpoint(brain, filepath, description)
    except ImportError:
        # Fallback implementation
        brain.save(filepath)
        print(f"Checkpoint saved to {filepath}")
        return {
            'filepath': filepath,
            'timestamp': None,
            'description': description,
        }


def benchmark_performance(brain, world=None, agent=None, num_steps=1000, num_runs=5):
    """Benchmark simulation performance.
    
    Args:
        brain: The brain object to benchmark
        world: Optional world object for agent-based simulation
        agent: Optional agent object for processing sensory input
        num_steps: Number of steps per benchmark run
        num_runs: Number of benchmark runs to average over
        
    Returns:
        Dictionary with benchmark statistics
    """
    try:
        from pynlm.utils import benchmark_simulation
        return benchmark_simulation(brain, world, agent, num_steps, num_runs)
    except ImportError:
        # Fallback implementation
        import time
        print(f"Running benchmark: {num_runs} runs of {num_steps} steps each")
        
        times = []
        for run in range(num_runs):
            start_time = time.time()
            
            for step in range(num_steps):
                if world is not None and agent is not None:
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                else:
                    brain.step(step)
            
            end_time = time.time()
            run_time = end_time - start_time
            times.append(run_time)
            print(f"  Run {run + 1}/{num_runs}: {run_time:.3f}s")
        
        avg_time = sum(times) / len(times)
        std_time = (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5
        
        return {
            'num_runs': num_runs,
            'num_steps': num_steps,
            'times': times,
            'average_time': avg_time,
            'std_time': std_time,
            'steps_per_second': num_steps / avg_time,
        }


def get_simulation_summary(brain, world=None, agent=None):
    """Get comprehensive simulation statistics.
    
    Args:
        brain: The brain object
        world: Optional world object
        agent: Optional agent object
        
    Returns:
        Dictionary with comprehensive simulation statistics
    """
    try:
        from pynlm.utils import get_simulation_stats
        return get_simulation_stats(brain, world, agent)
    except ImportError:
        # Fallback implementation
        stats = {
            'brain_stats': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'development_stage': brain.getDevelopmentalStage(),
            }
        }
        
        if agent:
            stats['agent_stats'] = {
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
            }
        
        if world:
            stats['world_stats'] = {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
            }
        
        return stats

# Interactive help and examples
def get_examples():
    """Get example patterns for common NLM operations."""
    return {
        'simple_brain': '''
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
''',
        
        'agent_simulation': '''
import pynlm

# Setup complete agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
''',
        
        'performance_profiling': '''
import pynlm

# Setup simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Profile performance with statistics
stats = pynlm.run_with_progress(brain, world, agent, num_steps=1000)

print(f"Simulation complete!")
print(f"Total time: {stats['elapsed_time']:.2f} seconds")
print(f"Average firing rate: {stats['average_firing_rate']:.3f} Hz")
''',
        
        'checkpointing': '''
import pynlm

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 50 == 0:
        # Save checkpoint periodically
        pynlm.save_checkpoint(brain, f"checkpoint_step_{step}.bin", "Simulation at step {step}")
''',
    }

# Progress tracking callback
def create_progress_callback(brain=None):
    """Create a progress callback function for simulations."""
    def progress_callback(step, total_steps, stats):
        progress = (step / total_steps) * 100
        print(f"Progress: {progress:.1f}% (Step {step}/{total_steps})")
        if brain:
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {stats['total_spike_count']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.3f} Hz")
        print()
    
    return progress_callback
