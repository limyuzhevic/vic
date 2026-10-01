"""
NLM Enhanced Python Bindings
=============================

This module provides enhanced Python bindings for the NLM (Neural Learning Machine)
framework with comprehensive functionality for neural simulation and research.

Key Features:
- Complete access to all neural systems (memory, prediction, cognition, neuromodulation)
- High-level convenience functions for common tasks
- NumPy integration for scientific computing
- Extensive error handling and validation
- Comprehensive documentation with examples

Usage Examples:
---------------

Basic Simulation:
    >>> import pynlm as nlm
    >>> config = nlm.create_default_config()
    >>> brain = nlm.create_brain(config)
    >>> brain.initialize()
    >>> action = brain.produce_action()

Advanced Memory Systems:
    >>> import pynlm as nlm
    >>> working_memory = nlm.NeuralWorkingMemory()
    >>> working_memory.initialize(brain)
    >>> working_memory.store([0.1, 0.5, 0.9, 0.2])
    >>> pattern = working_memory.retrieve()

Neuromodulation:
    >>> import pynlm as nlm
    >>> dopamine = nlm.Dopamine()
    >>> dopamine.initialize(brain)
    >>> dopamine.update(reward=1.0, prediction_error=0.1, dt=0.1)

Complete Agent-Environment Integration:
    >>> config, world, brain, agent = nlm.create_demo_environment()
    >>> agent.initialize(world)
    >>> for step in range(100):
    >>>     percept = world.get_sensory_percept()
    >>>     agent.process_sensory_input(percept)
    >>>     world.update(0.1)
    >>> time = world.get_simulation_time()

NumPy Integration:
    >>> import pynlm as nlm
    >>> import numpy as np
    >>> vision_data = np.array([[0.1, 0.2], [0.3, 0.4]])
    >>> vision = nlm.Vision(2, 2)
    >>> vision.set_data(vision_data.flatten().tolist())
    >>> vision_array = nlm.to_numpy(vision.get_data())
"""

# Core imports
from pynlm_enhanced import (
    # Core types and enums
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID,
    DEFAULT_TIMESTEP, DEFAULT_SIMULATION_STEPS,
    INITIAL_DEVELOPMENT_STAGES, CRITICAL_PERIOD_MULTIPLIER,
    WORKING_MEMORY_CAPACITY, WORKING_MEMORY_DECAY_RATE,
    MAX_DOPAMINE_LEVEL, MAX_CURIOUSITY_LEVEL, MAX_NOVELTY_LEVEL,
    
    # Main classes
    Config, SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult,
    SensoryPercept, SimpleWorld, Brain, AgentBrain,
    
    # Memory systems
    NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory,
    
    # Prediction systems
    PredictionSystem, NeuralPrediction,
    
    # Cognition systems
    NeuralPlanner, ConceptFormation, AttentionalSelection,
    
    # Neuromodulation
    Neuromodulator, Dopamine, Curiosity, Novelty, PredictionError,
    
    # Development
    DevelopmentSystem, Synaptogenesis, Maturation, Pruning,
    
    # Plasticity
    STDP, Hebbian, StructuralPlasticity,
    
    # Environment and experiments
    Environment, Experiment, ExperimentRunner,
    
    # High-level convenience functions
    create_default_config, create_brain, create_simple_world, create_agent_brain,
    create_demo_environment, run_episode, benchmark_performance, validate_configuration,
)

# NumPy integration utilities
import numpy as np

def to_numpy(vector):
    """Convert a vector to NumPy array"""
    return np.array(vector)

def from_numpy(array):
    """Convert a NumPy array to vector"""
    return array.flatten().tolist()

# Utility functions for enhanced usability

def create_brain_from_config(config_dict=None):
    """
    Create a brain with optional configuration dictionary.
    
    Args:
        config_dict: Optional dictionary with configuration parameters
        
    Returns:
        Configured Brain object
    """
    config = create_default_config()
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    return create_brain(config)

def create_complete_agent(config_dict=None):
    """
    Create a complete agent (brain + agent brain + world) with optional configuration.
    
    Args:
        config_dict: Optional dictionary with configuration parameters
        
    Returns:
        Tuple of (config, world, brain, agent)
    """
    config = create_default_config()
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    
    world = create_simple_world()
    world.configure(100, 100, 20, 20)  # Standard environment size
    
    brain = create_brain(config)
    agent = create_agent_brain(brain)
    
    agent.initialize(world)
    
    return config, world, brain, agent

def run_learning_episode(agent, world, max_steps=100, verbose=False):
    """
    Run a complete learning episode with optional verbose output.
    
    Args:
        agent: AgentBrain object
        world: SimpleWorld object
        max_steps: Maximum number of simulation steps
        verbose: Whether to print progress information
        
    Returns:
        Dictionary with episode statistics
    """
    stats = {
        'total_reward': 0.0,
        'steps': 0,
        'time': 0.0,
        'action_count': {},
        'memory_activity': []
    }
    
    for step in range(max_steps):
        percept = world.getSensoryPercept()
        action = agent.decodeMotorCommand()
        
        # Count actions
        action_key = action.getType()
        stats['action_count'][action_key] = stats['action_count'].get(action_key, 0) + 1
        
        # Update memory and neuromodulation
        agent.processSensoryInput(percept)
        
        # Apply neuromodulation based on action
        if action.getType() == ActionType.Eat:
            agent.applyRewardModulation(1.0, 0.0)
            stats['total_reward'] += 1.0
        elif action.getType() == ActionType.Rest:
            agent.applyRewardModulation(0.5, 0.0)
            stats['total_reward'] += 0.5
        
        # Update development and memory
        agent.updateDevelopment(0.1)
        
        # World update
        world.update(0.1)
        
        stats['steps'] += 1
        stats['time'] = world.getSimulationTime()
        
        if verbose and step % 10 == 0:
            print(f"Step {step}: Time={stats['time']:.2f}, "
                  f"Reward={stats['total_reward']:.2f}, "
                  f"Actions={stats['action_count']}")
    
    return stats

def analyze_neural_activity(brain, steps=10):
    """
    Analyze neural activity patterns over several steps.
    
    Args:
        brain: Brain object
        steps: Number of steps to analyze
        
    Returns:
        Dictionary with neural activity statistics
    """
    analysis = {
        'region_activity': {},
        'firing_patterns': {},
        'excitatory_inhibitory_ratio': 0.0,
        'total_spikes': 0,
        'active_neurons': 0,
    }
    
    for step in range(steps):
        brain.step(step)
        
        # Get statistics
        analysis['region_activity'][step] = brain.getRegionCount()
        analysis['firing_patterns'][step] = brain.getFiringNeuronCount()
        analysis['total_spikes'] += brain.getTotalSpikeCount()
        analysis['active_neurons'] += brain.getActiveNeuronCount()
    
    analysis['average_active_neurons'] = analysis['active_neurons'] / steps
    analysis['average_firing_patterns'] = analysis['total_spikes'] / steps
    analysis['excitatory_inhibitory_ratio'] = brain.getExcitationInhibitionRatio()
    
    return analysis

def create_reproducible_simulation(seed=None):
    """
    Create a reproducible simulation environment with fixed random seed.
    
    Args:
        seed: Random seed (if None, generates random seed)
        
    Returns:
        Tuple of (config, world, brain, agent)
    """
    config, world, brain, agent = create_complete_agent()
    
    if seed is None:
        import random
        seed = random.randint(0, 2**31 - 1)
    
    world.setRandomSeed(seed)
    config.set("simulation.seed", seed)
    
    return config, world, brain, agent

# Advanced utility classes

class NLMExperiment:
    """
    High-level experiment framework for running and analyzing NLM experiments.
    """
    
    def __init__(self, name, config_dict=None):
        """
        Initialize experiment.
        
        Args:
            name: Experiment name
            config_dict: Optional configuration dictionary
        """
        self.name = name
        self.config, self.world, self.brain, self.agent = create_complete_agent(config_dict)
        self.results = {}
        self.metrics = {}
    
    def run(self, max_steps=1000):
        """Run experiment."""
        self.metrics = run_learning_episode(self.agent, self.world, max_steps, verbose=True)
        return self.metrics
    
    def get_results(self):
        """Get experiment results."""
        return self.results
    
    def add_metric(self, name, value):
        """Add a metric to the experiment."""
        self.metrics[name] = value
        self.results[name] = value
    
    def save_results(self, filepath):
        """Save experiment results to file."""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.metrics, f, indent=2)

class NLMVisualizer:
    """
    Visualization utilities for NLM simulations.
    """
    
    @staticmethod
    def plot_vision(percept, title="Vision Input"):
        """
        Plot vision input from a sensory percept.
        
        Args:
            percept: SensoryPercept object
            title: Plot title
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            vision = percept.getVision()
            vision_width = percept.getVisionWidth()
            vision_height = percept.getVisionHeight()
            
            # Reshape vision data
            vision_matrix = np.array(vision).reshape(vision_height, vision_width)
            
            plt.figure(figsize=(8, 6))
            plt.imshow(vision_matrix, cmap='hot', interpolation='nearest')
            plt.colorbar(label='Intensity')
            plt.title(title)
            plt.xlabel('X')
            plt.ylabel('Y')
            plt.show()
            
        except ImportError:
            print("matplotlib not available. Install with: pip install matplotlib")
        except Exception as e:
            print(f"Error plotting vision: {e}")
    
    @staticmethod
    def plot_activity_over_time(brain, steps=100):
        """
        Plot neural activity over time.
        
        Args:
            brain: Brain object
            steps: Number of steps to analyze
        """
        try:
            import matplotlib.pyplot as plt
            
            analysis = analyze_neural_activity(brain, steps)
            
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
            
            # Active neurons over time
            ax1.plot(list(analysis['firing_patterns'].values()))
            ax1.set_ylabel('Active Neurons')
            ax1.set_title('Neural Activity Over Time')
            ax1.grid(True)
            
            # Action count distribution
            action_counts = analysis.get('action_count', {})
            if action_counts:
                ax2.bar(range(len(action_counts)), list(action_counts.values()))
                ax2.set_xlabel('Action Type')
                ax2.set_ylabel('Count')
                ax2.set_title('Action Distribution')
            
            plt.tight_layout()
            plt.show()
            
        except ImportError:
            print("matplotlib not available. Install with: pip install matplotlib")
        except Exception as e:
            print(f"Error plotting activity: {e}")
    
    @staticmethod
    def save_to_video(world, filepath, fps=10, duration=5):
        """
        Save world simulation to video file.
        
        Args:
            world: SimpleWorld object
            filepath: Output video file path
            fps: Frames per second
            duration: Video duration in seconds
        """
        try:
            import cv2
            import numpy as np
            
            frame_count = int(fps * duration)
            width = int(world.getWidth())
            height = int(world.getHeight())
            
            # Initialize video writer
            fourcc = cv2.VideoWriter_fourcc(*'mp4v')
            out = cv2.VideoWriter(filepath, fourcc, fps, (width, height))
            
            for frame in range(frame_count):
                # Get current percept
                percept = world.getSensoryPercept()
                vision = percept.getVision()
                
                # Convert to grayscale image
                vision_matrix = np.array(vision).reshape(int(percept.getVisionHeight()),
                                                        int(percept.getVisionWidth()))
                vision_matrix = (vision_matrix * 255).astype(np.uint8)
                
                # Draw agent
                agent = world.getAgentBody()
                cv2.circle(vision_matrix, 
                          (int(agent.x), int(agent.y)), 
                          5, (255, 0, 0), -1)
                
                out.write(vision_matrix)
                world.update(0.1)
            
            out.release()
            print(f"Video saved to {filepath}")
            
        except ImportError:
            print("cv2 (OpenCV) not available. Install with: pip install opencv-python")
        except Exception as e:
            print(f"Error saving video: {e}")

# Version and metadata
__version__ = "2.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Initialize numpy support
numpy_available = True
try:
    import numpy
except ImportError:
    numpy_available = False
    print("Warning: NumPy not available. Some features may be limited.")

# Export enhanced API
def __getattr__(name):
    """Lazy loading for optional dependencies."""
    if name == "VisualizationInterface":
        try:
            from .visualization import VisualizationInterface
            return VisualizationInterface
        except ImportError:
            raise ImportError("VisualizationInterface not available. Install additional dependencies.")
    
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")

# Validate installation
def validate_installation():
    """
    Validate that the enhanced NLM bindings are properly installed.
    
    Returns:
        Dictionary with installation status
    """
    status = {
        'pynlm_enhanced': True,
        'numpy': numpy_available,
        'matplotlib': False,
        'opencv': False,
        'brain_available': False,
    }
    
    try:
        import matplotlib
        status['matplotlib'] = True
    except ImportError:
        pass
    
    try:
        import cv2
        status['opencv'] = True
    except ImportError:
        pass
    
    # Test basic functionality
    try:
        config = create_default_config()
        status['brain_available'] = True
    except Exception:
        status['brain_available'] = False
    
    return status

# Create a simple test/demo
if __name__ == "__main__":
    print("NLM Enhanced Python Bindings")
    print("=" * 40)
    
    # Check installation
    status = validate_installation()
    print("Installation Status:")
    for key, value in status.items():
        print(f"  {key}: {'✓' if value else '✗'}")
    
    print("\nQuick Demo:")
    print("-" * 20)
    
    try:
        # Create demo environment
        config, world, brain, agent = create_demo_environment()
        print(f"Created demo environment: {world}")
        
        # Run short episode
        stats = run_learning_episode(agent, world, max_steps=20, verbose=True)
        print(f"Episode completed: {stats['steps']} steps, "
              f"reward={stats['total_reward']:.2f}")
        
        # Neural activity analysis
        analysis = analyze_neural_activity(brain, steps=5)
        print(f"Neural analysis: {analysis['average_active_neurons']:.1f} avg active neurons")
        
    except Exception as e:
        print(f"Demo failed: {e}")
    
    print("\nEnhanced NLM bindings ready for use!")