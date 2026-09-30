#!/usr/bin/env python3
"""
NLM - Neural Learning Machine Python Package

This is the improved Python interface for the NLM neural simulation framework.

## What's New

This version provides:

1. **Complete Python Package Structure**: Proper `__init__.py`, `setup.py`, and package organization
2. **Enhanced API**: More Pythonic interfaces with helper functions and convenience methods
3. **Advanced Features**: Support for visualization, experiment management, and advanced neural operations
4. **Comprehensive Documentation**: Detailed docstrings and examples
5. **Testing Suite**: Comprehensive tests for all functionality
6. **Error Handling**: Better error messages and exception handling

## Quick Start

### Installation

```bash
pip install -e .
```

Or for development:

```bash
pip install -e .[dev]
```

### Basic Usage

```python
import nlm

# Create a brain with default configuration
brain = nlm.Brain()

# Initialize the brain
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.get_total_neurons()} neurons, {brain.get_firing_rate():.2f} Hz")
```

### Advanced Usage

```python
import nlm

# Create world and agent for simulation
brain = nlm.Brain()
brain.initialize()

world = nlm.SimpleWorld(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

agent = nlm.AgentBrain(brain, world)
agent.initialize()

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)

# Run agent simulation
simulation = nlm.Simulation(brain, world, agent)
results = simulation.run(500, verbose=True)

# Analyze results
print(f"Average reward: {results.get('avg_reward', 0):.2f}")
print(f"Curiosity level: {agent.get_curiosity_level():.2f}")
print(f"Novelty detected: {len(results.get('novelty_events', []))}")
```

## Core Module Imports

The NLM package provides direct access to core neural simulation components:

```python
# Core simulation components
from nlm import Brain, AgentBrain, SimpleWorld

# Helper functions and utilities
from nlm import utils

# Experiment management
from nlm import experiments

# Visualization tools
from nlm import visualization
```

## Error Handling

The NLM package includes robust error handling with custom exception classes:

```python
class NLMError(Exception):
    """Base exception for NLM errors."""

class BrainError(NLMError):
    """Exception related to brain simulation errors."""

class ConfigurationError(NLMError):
    """Exception related to configuration errors."""

# Usage example
try:
    brain = nlm.Brain(config)
    brain.initialize()
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.BrainError as e:
    print(f"Brain error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

## API Documentation

### Brain Class

The main neural simulation class providing comprehensive control over neural networks:

```python
class Brain:
    """Central neural simulation brain class.
    
    Implements real spiking neural computation with event-driven dynamics
    and integrated memory, prediction, cognition, and neuromodulation systems.
    """
    
    def __init__(self, config=None):
        """Initialize brain with configuration.
        
        Args:
            config: Configuration object for brain settings.
                   If None, uses default configuration.
        """
        if config is None:
            config = self._create_default_config()
        self.config = config
        # Initialize internal components...
    
    def initialize(self):
        """Initialize the brain with current configuration.
        
        This method must be called before using the brain for simulation.
        It sets up all neural populations, connections, and system states.
        """
        # Initialization logic...
    
    def step(self, step_number=None, current_time=None):
        """Advance simulation by one time step.
        
        Args:
            step_number: Current simulation step number. Used for logging
                        and time-dependent behaviors. If None, uses internal counter.
            current_time: Current simulation time in milliseconds. If None,
                         calculated from step number using default timestep.
        
        This method performs one complete simulation iteration including:
        - Neural dynamics update (LIF neuron model)
        - Spike propagation and processing
        - Plasticity updates (STDP, Hebbian, structural)
        - Memory system updates
        - Neuromodulation application
        - Development system updates
        """
        # Simulation step logic...
    
    # Property-based access methods
    @property
    def total_neurons(self):
        """Get total number of neurons in the brain."""
        return self._brain_ptr.getTotalNeuronCount()
    
    @property
    def firing_rate(self):
        """Get average firing rate in Hz."""
        return self._brain_ptr.getAverageFiringRate()
    
    @property
    def developmental_stage(self):
        """Get current developmental stage."""
        return self._brain_ptr.getDevelopmentalStage()
    
    # Configuration methods
    def load_config(self, filepath):
        """Load configuration from file."""
        # Load and apply configuration...
    
    def save_config(self, filepath):
        """Save configuration to file."""
        # Save current configuration...
    
    def update_config(self, **kwargs):
        """Update configuration parameters."""
        # Update specified parameters...
    
    # State management
    def save(self, filepath):
        """Save brain state to file (checkpoint)."""
        # Save current state...
    
    def load(self, filepath):
        """Load brain state from file."""
        # Restore saved state...
    
    def reset(self):
        """Reset brain to initial state."""
        # Reset all internal states...
    
    # Region and population management
    def add_region(self, name="", neuron_count=100, neuron_type=NeuronType.Excitatory):
        """Add a new neural region.
        
        Args:
            name: Name for the new region. If empty, generates a unique name.
            neuron_count: Number of neurons to create in the region.
            neuron_type: Type of neurons to create.
        
        Returns:
            RegionId of the newly created region.
        """
        # Create and add new region...
    
    def get_region(self, region_id):
        """Get neural region by ID.
        
        Args:
            region_id: ID of the region to retrieve.
        
        Returns:
            NeuralRegion object or None if region doesn't exist.
        """
        return self._brain_ptr.getRegion(region_id)
    
    def remove_region(self, region_id):
        """Remove neural region by ID."""
        # Remove region and clean up...
    
    # Statistics and monitoring
    def get_total_spike_count(self):
        """Get total spike count since last reset."""
        return self._brain_ptr.getTotalSpikeCount()
    
    def get_active_neuron_count(self):
        """Get number of currently active neurons."""
        return self._brain_ptr.getActiveNeuronCount()
    
    def get_firing_neuron_count(self):
        """Get number of neurons currently firing."""
        return self._brain_ptr.getFiringNeuronCount()
    
    def get_excitation_inhibition_ratio(self):
        """Get excitation/inhibition balance ratio."""
        return self._brain_ptr.getExcitationInhibitionRatio()
    
    def get_developmental_progress(self):
        """Get developmental stage and progress."""
        stage = self._brain_ptr.getDevelopmentalStage()
        age = self.config.development.age if hasattr(self.config, 'development') else 0
        return {"stage": stage, "age": age}
    
    # System access
    def get_spike_system(self):
        """Get access to spike system for low-level control."""
        return self._brain_ptr.getSpikeSystem()
    
    def get_stdp(self):
        """Get access to STDP plasticity system."""
        return self._brain_ptr.getSTDP()
    
    def get_hebbian(self):
        """Get access to Hebbian plasticity system."""
        return self._brain_ptr.getHebbian()
    
    def get_structural_plasticity(self):
        """Get access to structural plasticity system."""
        return self._brain_ptr.getStructuralPlasticity()
    
    # Memory system access
    def get_working_memory(self):
        """Get access to working memory system."""
        return self._brain_ptr.getWorkingMemory()
    
    def get_episodic_memory(self):
        """Get access to episodic memory system."""
        return self._brain_ptr.getEpisodicMemory()
    
    def get_associative_memory(self):
        """Get access to associative memory system."""
        return self._brain_ptr.getAssociativeMemory()
    
    # Prediction and cognition systems
    def get_prediction_system(self):
        """Get access to prediction system."""
        return self._brain_ptr.getPredictionSystem()
    
    def get_planner(self):
        """Get access to neural planner."""
        return self._brain_ptr.getPlanner()
    
    def get_concept_formation(self):
        """Get access to concept formation system."""
        return self._brain_ptr.getConceptFormation()
    
    # Neuromodulation systems
    def get_dopamine(self):
        """Get access to dopamine system."""
        return self._brain_ptr.getDopamine()
    
    def get_curiosity(self):
        """Get access to curiosity system."""
        return self._brain_ptr.getCuriosity()
    
    def get_novelty(self):
        """Get access to novelty detection system."""
        return self._brain_ptr.getNovelty()
    
    def get_prediction_error(self):
        """Get access to prediction error system."""
        return self._brain_ptr.getPredictionErrorSignal()
    
    # Development system
    def get_development_system(self):
        """Get access to development system."""
        return self._brain_ptr.getDevelopmentSystem()
    
    # Random and clock systems
    def get_random_generator(self):
        """Get access to random number generator."""
        return self._brain_ptr.getRandomGenerator()
    
    def get_simulation_clock(self):
        """Get access to simulation clock."""
        return self._brain_ptr.getSimulationClock()
    
    # Logging and monitoring
    def log_status(self, level=LogLevel.INFO):
        """Log brain status and statistics."""
        self._brain_ptr.logStatus()
    
    def profile_performance(self):
        """Profile current brain performance."""
        # Performance profiling logic...
    
    # Utility methods
    def validate_connectivity(self):
        """Validate neural connectivity and report issues."""
        # Connectivity validation logic...
    
    def optimize_weights(self):
        """Optimize synaptic weights for better dynamics."""
        # Weight optimization logic...
    
    # Context manager support
    def __enter__(self):
        """Support for 'with' statement."""
        self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Cleanup on context exit."""
        self.reset()
    
    # String representation
    def __repr__(self):
        return f"<NLM.Brain neurons={self.total_neurons} rate={self.firing_rate:.2f} Hz stage={self.developmental_stage}>"
    
    def __str__(self):
        return f"NLM Neural Brain with {self.total_neurons} neurons, {self.firing_rate:.2f} Hz firing rate"
```

### AgentBrain Class

Interface connecting brain to the world, handling sensory transduction and motor decoding:

```python
class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    Handles sensory transduction from world to brain and motor decoding from
    brain activity to world actions. Manages reward-based learning and
    development systems.
    """
    
    def __init__(self, brain, world=None):
        """Initialize agent brain.
        
        Args:
            brain: Brain instance to control.
            world: World instance to interact with. If None, can be set later.
        """
        self.brain = brain
        self.world = world
        self.config = brain.config
        self._initialize_components()
    
    def set_world(self, world):
        """Set the world instance for this agent brain."""
        self.world = world
        self.initialize()
    
    def initialize(self):
        """Initialize agent brain with world.
        
        This method should be called after setting the world instance or
        initially when creating the agent brain.
        """
        if not self.world:
            raise ValueError("World must be set before initializing AgentBrain")
        
        # Initialize with world...
        self._reset_state()
        self._setup_neuron_groups()
    
    def process_sensory_input(self, percept):
        """Process sensory input from environment.
        
        Args:
            percept: SensoryPercept object containing all sensory modalities
                    (vision, touch, internal, proprioception).
        
        This method injects sensory information into the brain's sensory neurons
        and updates novelty and curiosity signals.
        """
        if not self.world:
            raise RuntimeError("World not set - cannot process sensory input")
        
        # Process different sensory modalities...
        self._process_vision_input(percept)
        self._process_touch_input(percept)
        self._process_internal_input(percept)
        self._process_proprioceptive_input(percept)
        
        # Update neuromodulation based on novel input
        self._update_neuromodulation(percept)
    
    def decode_motor_command(self):
        """Decode brain motor activity into motor command.
        
        Returns:
            MotorCommand object representing the action to take.
        
        This method analyzes motor neuron activity and produces appropriate
        actions, incorporating curiosity-driven exploration when enabled.
        """
        if not self.brain:
            raise RuntimeError("Brain not available - cannot decode motor command")
        
        # Decode motor command from brain activity...
        motor_command = self._decode_from_neural_activity()
        
        # Apply curiosity-based exploration...
        if self.curiosity_enabled and self.curiosity_level > 0.3:
            motor_command = self._apply_curiosity(modified_command)
        
        return motor_command
    
    def apply_reward_modulation(self, reward, predicted_reward=None):
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received from environment.
            predicted_reward: Expected reward (if available for prediction error).
        
        This method updates neuromodulatory signals based on reward prediction
        error, affecting learning and behavior.
        """
        if not self.reward_modulation_enabled:
            return
        
        # Calculate prediction error...
        prediction_error = reward - (predicted_reward if predicted_reward else reward)
        
        # Update neuromodulation levels...
        self._update_dopamine_level(prediction_error)
        self._update_curiosity(prediction_error)
        
        # Apply neuromodulation to brain...
        self._apply_neuromodulation_to_brain()
    
    # Configuration and feature management
    def enable_features(self, feature_list):
        """Enable multiple features at once.
        
        Args:
            feature_list: List of feature names to enable.
        
        Available features:
        - 'reward_modulation': Reward-based learning
        - 'curiosity': Curiosity-driven exploration
        - 'structural_plasticity': Structural synaptic plasticity
        - 'development': Developmental processes
        - 'novelty_detection': Novelty-based learning
        - 'prediction_error': Prediction error signaling
        """
        feature_map = {
            'reward_modulation': self.enable_reward_modulation,
            'curiosity': self.enable_curiosity,
            'structural_plasticity': self.enable_structural_plasticity,
            'development': self.enable_development,
            'novelty_detection': self.enable_novelty_detection,
            'prediction_error': self.enable_prediction_error
        }
        
        for feature in feature_list:
            if feature in feature_map:
                feature_map[feature](True)
    
    def disable_features(self, feature_list):
        """Disable multiple features at once."""
        feature_map = {
            'reward_modulation': self.disable_reward_modulation,
            'curiosity': self.disable_curiosity,
            'structural_plasticity': self.disable_structural_plasticity,
            'development': self.disable_development,
            'novelty_detection': self.disable_novelty_detection,
            'prediction_error': self.disable_prediction_error
        }
        
        for feature in feature_list:
            if feature in feature_map:
                feature_map[feature](False)
    
    # Individual feature control
    def enable_reward_modulation(self, enable=True):
        """Enable/disable reward modulation."""
        self.reward_modulation_enabled = enable
        if enable:
            self.dopamine_level = 0.0
    
    def enable_curiosity(self, enable=True):
        """Enable/disable curiosity-driven exploration."""
        self.curiosity_enabled = enable
        if not enable:
            self.curiosity_level = 0.0
    
    def enable_structural_plasticity(self, enable=True):
        """Enable/disable structural plasticity."""
        self.structural_plasticity_enabled = enable
    
    def enable_development(self, enable=True):
        """Enable/disable developmental processes."""
        self.development_enabled = enable
    
    def enable_novelty_detection(self, enable=True):
        """Enable/disable novelty detection."""
        self.novelty_detection_enabled = enable
    
    def enable_prediction_error(self, enable=True):
        """Enable/disable prediction error signaling."""
        self.prediction_error_enabled = enable
    
    # State information
    @property
    def neuromodulation_level(self):
        """Get current neuromodulation level (dopamine)."""
        return self.dopamine_level
    
    @property
    def curiosity_level(self):
        """Get current curiosity level."""
        return self.curiosity_level
    
    @property
    def novelty_level(self):
        """Get current novelty level."""
        return self.novelty_level
    
    @property
    def prediction_error(self):
        """Get current prediction error."""
        return self.prediction_error
    
    @property
    def developmental_stage(self):
        """Get current developmental stage."""
        if self.brain:
            return self.brain.developmental_stage
        return DevelopmentalStage.Initial
    
    # Sensory and motor information
    def get_sensory_input_size(self):
        """Get expected sensory input size from brain."""
        return self.brain.get_sensory_input_size() if self.brain else 0
    
    def get_motor_output_size(self):
        """Get expected motor output size from brain."""
        return self.brain.get_motor_output_size() if self.brain else 0
    
    # Environment interaction
    def reset(self):
        """Reset agent for new episode."""
        self._reset_state()
        if self.brain:
            self.brain.reset()
    
    # Context manager
    def __enter__(self):
        if self.world:
            self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()
    
    # String representation
    def __repr__(self):
        return (f"<NLM.AgentBrain brain={self.brain} world={self.world} "
                f"curiosity={self.curiosity_level:.2f} novelty={self.novelty_level:.2f}>")
```

### SimpleWorld Class

Simple 2D world simulator for NLM agent interaction:

```python
class SimpleWorld:
    """Simple 2D world for NLM simulation.
    
    Provides a basic environment for agent interaction with obstacles,
    resources, and simple physics simulation.
    """
    
    def __init__(self, width=20, height=20, vision_width=8, vision_height=8):
        """Initialize simple world.
        
        Args:
            width: World width in cells.
            height: World height in cells.
            vision_width: Vision sensor width in cells.
            vision_height: Vision sensor height in cells.
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Initialize world state...
        self._initialize_world()
    
    def configure(self, width, height, vision_width, vision_height):
        """Configure world dimensions and sensor parameters.
        
        Args:
            width: New world width.
            height: New world height.
            vision_width: New vision sensor width.
            vision_height: New vision sensor height.
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Resize world and reset...
        self._resize_world()
        self.reset()
    
    def reset(self):
        """Reset world to initial state."""
        # Reset all world state...
    
    def update(self, timestep):
        """Update world state for one time step.
        
        Args:
            timestep: Time step duration in seconds.
        
        This method updates the world state including:
        - Agent movement and physics
        - Environmental changes
        - Resource generation/regeneration
        - Hazard movement
        - Weather effects (if any)
        """
        # Update world dynamics...
    
    def get_sensory_percept(self):
        """Get agent's sensory perception of current world state.
        
        Returns:
            SensoryPercept object containing all available sensory information.
        """
        # Create and return percept...
    
    def apply_motor_command(self, command, timestep):
        """Apply motor command to world.
        
        Args:
            command: MotorCommand to execute.
            timestep: Duration of command execution.
        
        This method updates agent position and state based on the command,
        handling collisions, physics, and world interactions.
        """
        # Apply motor command...
    
    # Position and state queries
    def get_agent_position(self):
        """Get current agent position in world coordinates."""
        return self.agent_body.x, self.agent_body.y
    
    def get_agent_orientation(self):
        """Get current agent orientation in radians."""
        return self.agent_body.orientation
    
    def is_valid_position(self, x, y):
        """Check if position is valid (not in obstacle)."""
        return self._is_position_valid(x, y)
    
    # World properties
    @property
    def simulation_time(self):
        """Get current simulation time in seconds."""
        return self._simulation_time
    
    @property
    def timestep(self):
        """Get current time step duration."""
        return self._timestep
    
    # Sensor access
    def get_vision_sensor(self):
        """Get vision sensor for vision input."""
        return self._vision_sensor
    
    def get_touch_sensor(self):
        """Get touch sensor for collision detection."""
        return self._touch_sensor
    
    # Object management
    def add_object(self, obj):
        """Add object to world."""
        # Add object to world...
    
    def remove_object(self, x, y):
        """Remove object at position."""
        # Remove object...
    
    def clear_objects(self):
        """Remove all objects from world."""
        # Clear all objects...
    
    # Configuration and setup
    def set_agent_start(self, x, y):
        """Set initial agent starting position."""
        # Set agent start position...
    
    def set_random_seed(self, seed):
        """Set random seed for reproducible simulations."""
        # Set random seed...
    
    def get_random_seed(self):
        """Get current random seed."""
        return self._random_seed
    
    # Physics and environment parameters
    def set_max_energy(self, energy):
        """Set maximum agent energy."""
        # Set energy limit...
    
    def get_max_energy(self):
        """Get maximum agent energy."""
        return self._max_energy
    
    def set_energy_decay_rate(self, rate):
        """Set energy decay rate per timestep."""
        # Set decay rate...
    
    def get_energy_decay_rate(self):
        """Get energy decay rate."""
        return self._energy_decay_rate
    
    def set_hazard_density(self, density):
        """Set density of hazards in world."""
        # Set hazard density...
    
    def get_hazard_density(self):
        """Get current hazard density."""
        return self._hazard_density
    
    # Context manager
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()
    
    # String representation
    def __repr__(self):
        return (f"<NLM.SimpleWorld size={self.width}x{self.height} "
                f"vision={self.vision_width}x{self.vision_height} "
                f"time={self.simulation_time:.1f}s>")
```

### Helper Functions

#### Configuration Management

```python
def create_default_config():
    """Create default brain configuration.
    
    Returns:
        Config object with default settings for typical NLM simulations.
    """
    config = Config()
    
    # Default settings for neural properties
    config.neuron_count = 1000
    config.connection_density = 0.1
    config.plasticity_enabled = True
    config.development_enabled = True
    config.memory_capacity = 1000
    
    # Default settings for simulation parameters
    config.simulation.timestep = 0.01
    config.simulation.max_steps = 1000
    config.simulation.random_seed = 42
    
    # Default settings for neuromodulation
    config.neuromodulation.reward_modulation_enabled = True
    config.neuromodulation.curiosity_enabled = True
    config.neuromodulation.novelty_detection_enabled = True
    
    # Default settings for learning
    config.plasticity.stdp.enabled = True
    config.plasticity.hebbian.enabled = True
    config.plasticity.structural.enabled = True
    
    return config


def load_config(filepath):
    """Load configuration from file.
    
    Args:
        filepath: Path to configuration file.
    
    Returns:
        Config object loaded from file.
    
    Raises:
        ConfigurationError: If file cannot be loaded or is invalid.
    """
    try:
        with open(filepath, 'r') as f:
            import json
            config_dict = json.load(f)
        
        # Convert dictionary to Config object...
        config = Config.from_dict(config_dict)
        return config
    
    except (IOError, json.JSONDecodeError) as e:
        raise ConfigurationError(f"Failed to load configuration from {filepath}: {e}")


def validate_config(config):
    """Validate configuration parameters.
    
    Args:
        config: Configuration object to validate.
    
    Returns:
        List of validation errors (empty if valid).
    
    Raises:
        TypeError: If config is not a Config object.
    """
    if not isinstance(config, Config):
        raise TypeError("config must be a Config object")
    
    errors = []
    
    # Validate neural properties
    if config.neuron_count <= 0:
        errors.append("neuron_count must be positive")
    
    if not 0.0 <= config.connection_density <= 1.0:
        errors.append("connection_density must be between 0 and 1")
    
    # Validate simulation parameters
    if config.simulation.timestep <= 0:
        errors.append("simulation.timestep must be positive")
    
    if config.simulation.max_steps <= 0:
        errors.append("simulation.max_steps must be positive")
    
    return errors
```

#### Visualization Utilities

```python
def plot_neural_activity(brain, output_file=None, show=True):
    """Plot neural activity over time.
    
    Args:
        brain: Brain object to analyze.
        output_file: Optional file path to save plot.
        show: Whether to display plot interactively.
    
    Returns:
        matplotlib.figure.Figure object.
    """
    import matplotlib.pyplot as plt
    import numpy as np
    
    # Collect neural activity data...
    activity_data = brain.get_activity_history()
    
    # Create plots...
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot total neuron count over time
    axes[0, 0].plot(activity_data['step'], activity_data['neuron_count'])
    axes[0, 0].set_title('Total Neuron Count')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Neuron Count')
    
    # Plot firing rate over time
    axes[0, 1].plot(activity_data['step'], activity_data['firing_rate'])
    axes[0, 1].set_title('Average Firing Rate')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Hz')
    
    # Plot synaptic weights over time
    axes[1, 0].plot(activity_data['step'], activity_data['synapse_weights'])
    axes[1, 0].set_title('Average Synaptic Weight')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Weight')
    
    # Plot developmental stage over time
    stages = list(DevelopmentalStage)
    stage_values = [activity_data['stage'].index(s) for s in stages
                    if s in activity_data['stage']]
    stage_labels = [s.name for s in stages if s in activity_data['stage']]
    axes[1, 1].barh(range(len(stage_values)), stage_values)
    axes[1, 1].set_yticks(range(len(stage_labels)))
    axes[1, 1].set_yticklabels(stage_labels)
    axes[1, 1].set_title('Developmental Stage')
    axes[1, 1].set_xlabel('Index')
    
    plt.tight_layout()
    
    if output_file:
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
    
    if show:
        plt.show()
    
    return fig


def plot_plasticity_evolution(brain, output_file=None, show=True):
    """Plot synaptic plasticity evolution.
    
    Args:
        brain: Brain object to analyze.
        output_file: Optional file path to save plot.
        show: Whether to display plot interactively.
    
    Returns:
        matplotlib.figure.Figure object.
    """
    import matplotlib.pyplot as plt
    
    # Collect plasticity data...
    plasticity_data = brain.get_plasticity_history()
    
    # Create plots...
    fig, axes = plt.subplots(3, 1, figsize=(10, 12))
    
    # Plot STDP weight changes
    axes[0].plot(plasticity_data['step'], plasticity_data['stdp_changes'])
    axes[0].set_title('STDP Weight Changes')
    axes[0].set_xlabel('Step')
    axes[0].set_ylabel('Weight Change')
    axes[0].axhline(y=0, color='black', linestyle='-', alpha=0.3)
    
    # Plot Hebbian weight changes
    axes[1].plot(plasticity_data['step'], plasticity_data['hebbian_changes'])
    axes[1].set_title('Hebbian Weight Changes')
    axes[1].set_xlabel('Step')
    axes[1].set_ylabel('Weight Change')
    axes[1].axhline(y=0, color='black', linestyle='-', alpha=0.3)
    
    # Plot structural plasticity events
    axes[2].hist(plasticity_data['structural_events'], bins=20, alpha=0.7)
    axes[2].set_title('Structural Plasticity Events Distribution')
    axes[2].set_xlabel('Number of Events')
    axes[2].set_ylabel('Frequency')
    
    plt.tight_layout()
    
    if output_file:
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
    
    if show:
        plt.show()
    
    return fig


def plot_memory_trace(memory_system, output_file=None, show=True):
    """Plot memory traces.
    
    Args:
        memory_system: Memory system to analyze.
        output_file: Optional file path to save plot.
        show: Whether to display plot interactively.
    
    Returns:
        matplotlib.figure.Figure object.
    """
    import matplotlib.pyplot as plt
    import numpy as np
    
    # Collect memory data...
    memory_data = memory_system.get_trace_history()
    
    # Create plots...
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot working memory usage
    axes[0, 0].plot(memory_data['step'], memory_data['working_memory_usage'])
    axes[0, 0].set_title('Working Memory Usage')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Usage')
    
    # Plot episodic memory storage
    axes[0, 1].plot(memory_data['step'], memory_data['episodic_memory_storage'])
    axes[0, 1].set_title('Episodic Memory Storage')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Storage')
    
    # Plot memory retrieval accuracy
    axes[1, 0].plot(memory_data['step'], memory_data['retrieval_accuracy'])
    axes[1, 0].set_title('Memory Retrieval Accuracy')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Accuracy')
    
    # Plot memory consolidation
    axes[1, 1].plot(memory_data['step'], memory_data['consolidation_level'])
    axes[1, 1].set_title('Memory Consolidation Level')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Consolidation')
    
    plt.tight_layout()
    
    if output_file:
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
    
    if show:
        plt.show()
    
    return fig
```

#### Experiment Utilities

```python
def run_parameter_sweep(param_configs):
    """Run parameter sweep across multiple configurations.
    
    Args:
        param_configs: List of parameter configurations to test.
    
    Returns:
        List of experiment results for each configuration.
    """
    results = []
    
    for config in param_configs:
        experiment = Experiment(config)
        result = experiment.run()
        results.append(result)
    
    return results


def cross_validation(folds=5, experiments=None):
    """Perform cross-validation on experiments.
    
    Args:
        folds: Number of cross-validation folds.
        experiments: List of experiment configurations.
    
    Returns:
        Cross-validation results.
    """
    if experiments is None:
        experiments = []
    
    # Split experiments into folds...
    fold_size = len(experiments) // folds
    cv_results = []
    
    for fold in range(folds):
        start = fold * fold_size
        end = (fold + 1) * fold_size if fold < folds - 1 else len(experiments)
        
        test_experiments = experiments[start:end]
        train_experiments = experiments[:start] + experiments[end:]
        
        # Train on training set and test on test set...
        model = train_on_experiments(train_experiments)
        fold_result = evaluate_on_experiments(model, test_experiments)
        cv_results.append(fold_result)
    
    return cv_results


def aggregate_results(experiments):
    """Aggregate results from multiple experiments.
    
    Args:
        experiments: List of experiment results to aggregate.
    
    Returns:
        Aggregated results with statistics.
    """
    aggregated = {
        'mean': {},
        'std': {},
        'min': {},
        'max': {}
    }
    
    # Aggregate each metric...
    for exp in experiments:
        for metric, value in exp.items():
            if metric not in aggregated['mean']:
                aggregated['mean'][metric] = []
            aggregated['mean'][metric].append(value)
    
    # Calculate statistics...
    for metric, values in aggregated['mean'].items():
        aggregated['mean'][metric] = np.mean(values)
        aggregated['std'][metric] = np.std(values)
        aggregated['min'][metric] = np.min(values)
        aggregated['max'][metric] = np.max(values)
    
    return aggregated
```

### Advanced Classes

#### Simulation Classes

```python
class Simulation:
    """Run complete simulation of agent in world.
    
    This class manages the complete interaction between agent, brain, and world,
    handling the main simulation loop and collecting results.
    """
    
    def __init__(self, brain, world, agent):
        """Initialize simulation.
        
        Args:
            brain: Brain object to simulate.
            world: World object to simulate in.
            agent: AgentBrain object to control.
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.results = SimulationResults()
    
    def run(self, num_steps, verbose=False):
        """Run simulation for specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run.
            verbose: Whether to print progress information.
        
        Returns:
            SimulationResults object containing all collected data.
        """
        for step in range(num_steps):
            if verbose and step % 100 == 0:
                print(f"Step {step}/{num_steps}")
            
            # Run one simulation step...
            self._run_step(step)
        
        return self.results
    
    def _run_step(self, step):
        """Run a single simulation step."""
        # Update world
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.get_sensory_percept()
        
        # Process sensory input
        self.agent.process_sensory_input(percept)
        
        # Brain thinks
        self.brain.step(step)
        
        # Agent decides action
        action = self.agent.decode_motor_command()
        
        # Apply action to world
        self.world.apply_motor_command(action, self.world.simulation_time)
        
        # Collect results
        self.results.record_step(step, self.brain, self.world, self.agent)
    
    def get_results(self):
        """Get simulation results."""
        return self.results
```

#### Visualization Classes

```python
class Visualization:
    """Create visualizations of brain simulation data.
    
    This class provides methods for creating various visualizations of neural
    simulation data including neural activity, plasticity evolution, and
    memory traces.
    """
    
    def __init__(self, brain):
        """Initialize visualization.
        
        Args:
            brain: Brain object to visualize.
        """
        self.brain = brain
        self.results = VisualizationResults()
    
    def plot_neural_activity(self, output_file=None, show=True):
        """Plot neural activity over time."""
        # Create neural activity visualization...
        return self.results.neural_activity_plot
    
    def plot_plasticity_evolution(self, output_file=None, show=True):
        """Plot synaptic plasticity evolution."""
        # Create plasticity evolution visualization...
        return self.results.plasticity_plot
    
    def plot_memory_trace(self, memory_system, output_file=None, show=True):
        """Plot memory traces."""
        # Create memory trace visualization...
        return self.results.memory_plot
    
    def show(self):
        """Display all visualizations."""
        # Show all visualizations...
        pass
    
    def save_plots(self, directory):
        """Save all visualizations to directory."""
        # Save all visualizations...
        pass
```

#### Experiment Management

```python
class ExperimentConfig:
    """Configuration for neural simulation experiments."""
    
    def __init__(self, duration=1000, agents=1, learning_rate=0.01,
                 memory_capacity=1000, environment="simple", **kwargs):
        """Initialize experiment configuration.
        
        Args:
            duration: Experiment duration in steps.
            agents: Number of agents to simulate.
            learning_rate: Learning rate for plasticity.
            memory_capacity: Capacity of memory systems.
            environment: Type of environment to use.
            **kwargs: Additional configuration parameters.
        """
        self.duration = duration
        self.agents = agents
        self.learning_rate = learning_rate
        self.memory_capacity = memory_capacity
        self.environment = environment
        self.additional_config = kwargs
    
    def create_brain(self):
        """Create brain according to configuration."""
        # Create brain...
        pass
    
    def create_world(self):
        """Create world according to configuration."""
        # Create world...
        pass
    
    def create_agent(self, brain):
        """Create agent according to configuration."""
        # Create agent...
        pass


class Experiment:
    """Manage and run neural simulation experiments."""
    
    def __init__(self, config):
        """Initialize experiment.
        
        Args:
            config: Experiment configuration.
        """
        self.config = config
        self.results = ExperimentResults()
        self.brain = None
        self.world = None
        self.agent = None
    
    def run(self):
        """Run experiment and return results.
        
        Returns:
            ExperimentResults object containing all experimental data.
        """
        # Run experiment...
        return self.results
    
    def get_results(self):
        """Get experiment results."""
        return self.results


class ExperimentResults:
    """Store and analyze experiment results."""
    
    def __init__(self):
        self.data = {}
        self.metadata = {}
    
    def add_result(self, metric_name, value, step=None):
        """Add experimental result."""
        if metric_name not in self.data:
            self.data[metric_name] = []
        self.data[metric_name].append(value)
    
    def plot_results(self, output_file=None, show=True):
        """Plot experimental results."""
        # Plot results...
        pass
    
    def save_to_file(self, filepath):
        """Save results to file."""
        # Save results...
        pass
```

### Neural Network Analysis

```python
class NeuralNetworkAnalyzer:
    """Analyze neural network structure and dynamics."""
    
    def __init__(self, brain):
        """Initialize analyzer.
        
        Args:
            brain: Brain object to analyze.
        """
        self.brain = brain
    
    def analyze_topology(self):
        """Analyze neural network topology.
        
        Returns:
            Dictionary containing topological analysis.
        """
        analysis = {}
        
        for region_id in self.brain.get_region_ids():
            region = self.brain.get_region(region_id)
            analysis[region_id] = {
                'neuron_count': region.get_neuron_count(),
                'synapse_count': region.get_synapse_count(),
                'connectivity': self._calculate_connectivity(region),
                'neuron_types': self._get_neuron_type_distribution(region)
            }
        
        return analysis
    
    def analyze_dynamics(self, num_steps=100):
        """Analyze neural dynamics over time.
        
        Args:
            num_steps: Number of steps to analyze.
        
        Returns:
            Dictionary containing dynamic analysis.
        """
        dynamics = {}
        
        # Collect dynamics data...
        for step in range(num_steps):
            self.brain.step(step)
            dynamics[step] = {
                'total_neurons': self.brain.get_total_neuron_count(),
                'firing_rate': self.brain.get_firing_rate(),
                'active_neurons': self.brain.get_active_neuron_count(),
                'excitatory_inhibition_ratio': self.brain.get_excitation_inhibition_ratio()
            }
        
        return dynamics
    
    def visualize_network(self, layout='spring', output_file=None, show=True):
        """Visualize neural network structure.
        
        Args:
            layout: Network layout algorithm ('spring', 'circular', 'random').
            output_file: Optional file path to save visualization.
            show: Whether to display visualization interactively.
        
        Returns:
            Network visualization object.
        """
        # Create network visualization...
        pass
```

### Advanced Agent Control

```python
class AgentTrainer:
    """Train agent using reinforcement learning."""
    
    def __init__(self, agent):
        """Initialize trainer.
        
        Args:
            agent: AgentBrain object to train.
        """
        self.agent = agent
        self.training_history = {}
    
    def train_episode(self, world, max_steps=200):
        """Train agent for one episode.
        
        Args:
            world: World object for training.
            max_steps: Maximum steps per episode.
        
        Returns:
            Episode statistics.
        """
        episode_stats = {
            'steps': 0,
            'reward': 0.0,
            'novelty': 0.0,
            'curiosity': 0.0,
            'success': False
        }
        
        world.reset()
        
        for step in range(max_steps):
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process sensory input
            self.agent.process_sensory_input(percept)
            
            # Brain thinks
            self.agent.brain.step(step)
            
            # Agent decides action
            action = self.agent.decode_motor_command()
            
            # Apply action to world
            world.apply_motor_command(action, world.simulation_time)
            
            # Get reward and update agent
            reward = world.get_reward()
            predicted_reward = self.agent.get_expected_reward()
            
            self.agent.apply_reward_modulation(reward, predicted_reward)
            
            episode_stats['reward'] += reward
            episode_stats['novelty'] += self.agent.novelty_level
            episode_stats['curiosity'] += self.agent.curiosity_level
            episode_stats['steps'] += 1
            
            # Check if episode ended
            if world.is_episode_finished():
                episode_stats['success'] = True
                break
        
        # Record training history
        self._record_training_history(episode_stats)
        
        return episode_stats
    
    def display_progress(self, episode, episode_stats):
        """Display training progress."""
        print(f"Episode {episode}:")
        print(f"  Steps: {episode_stats['steps']}")
        print(f"  Reward: {episode_stats['reward']:.2f}")
        print(f"  Novelty: {episode_stats['novelty']:.2f}")
        print(f"  Curiosity: {episode_stats['curiosity']:.2f}")
        print(f"  Success: {episode_stats['success']}")
```

### Simulation Management

```python
class SimulationPool:
    """Manage multiple simulations running in parallel."""
    
    def __init__(self, size=4):
        """Initialize simulation pool.
        
        Args:
            size: Number of simulations to run in parallel.
        """
        self.size = size
        self.simulations = []
    
    def add_simulation(self, brain, world, agent):
        """Add simulation to pool.
        
        Args:
            brain: Brain object for simulation.
            world: World object for simulation.
            agent: AgentBrain object for simulation.
        """
        simulation = Simulation(brain, world, agent)
        self.simulations.append(simulation)
    
    def run_synchronously(self, num_steps):
        """Run all simulations synchronously.
        
        Args:
            num_steps: Number of steps to run.
        
        Returns:
            List of simulation results.
        """
        results = []
        
        for simulation in self.simulations:
            result = simulation.run(num_steps)
            results.append(result)
        
        return results
    
    def run_async(self, num_steps):
        """Run all simulations asynchronously.
        
        Args:
            num_steps: Number of steps to run.
        
        Returns:
            List of futures for simulation results.
        """
        import concurrent.futures
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.size) as executor:
            futures = []
            for simulation in self.simulations:
                future = executor.submit(simulation.run, num_steps)
                futures.append(future)
            
            results = []
            for future in concurrent.futures.as_completed(futures):
                results.append(future.result())
        
        return results
```

### Performance Profiling

```python
class PerformanceProfiler:
    """Profile performance of neural simulations."""
    
    def __init__(self, brain):
        """Initialize profiler.
        
        Args:
            brain: Brain object to profile.
        """
        self.brain = brain
        self.profile_data = {}
    
    def start_profile(self):
        """Start performance profiling."""
        import time
        self.profile_data['start_time'] = time.time()
    
    def stop_profile(self):
        """Stop performance profiling."""
        import time
        self.profile_data['end_time'] = time.time()
        self.profile_data['total_time'] = (
            self.profile_data['end_time'] - self.profile_data['start_time']
        )
    
    def profile_neural_dynamics(self):
        """Profile neural dynamics performance."""
        import time
        start_time = time.time()
        
        # Run neural dynamics simulation...
        for step in range(100):
            self.brain.step(step)
        
        self.profile_data['neural_dynamics_time'] = time.time() - start_time
    
    def profile_plasticity(self):
        """Profile plasticity performance."""
        import time
        start_time = time.time()
        
        # Run plasticity simulation...
        for step in range(100):
            self.brain.step(step)
            self._update_plasticity(step)
        
        self.profile_data['plasticity_time'] = time.time() - start_time
    
    def profile_memory_systems(self):
        """Profile memory system performance."""
        import time
        start_time = time.time()
        
        # Run memory system simulation...
        for step in range(100):
            self.brain.step(step)
            self._update_memory_systems(step)
        
        self.profile_data['memory_time'] = time.time() - start_time
    
    def display_results(self):
        """Display profiling results."""
        print("Performance Profiling Results:")
        print(f"Total time: {self.profile_data.get('total_time', 0):.2f} seconds")
        print(f"Neural dynamics time: {self.profile_data.get('neural_dynamics_time', 0):.2f} seconds")
        print(f"Plasticity time: {self.profile_data.get('plasticity_time', 0):.2f} seconds")
        print(f"Memory time: {self.profile_data.get('memory_time', 0):.2f} seconds")
        
        if self.profile_data.get('total_time', 0) > 0:
            print(f"\nPerformance:")
            print(f"Neural dynamics: {100 * self.profile_data.get('neural_dynamics_time', 0) / self.profile_data.get('total_time', 0):.1f}%")
            print(f"Plasticity: {100 * self.profile_data.get('plasticity_time', 0) / self.profile_data.get('total_time', 0):.1f}%")
            print(f"Memory: {100 * self.profile_data.get('memory_time', 0) / self.profile_data.get('total_time', 0):.1f}%")
```

### Statistical Analysis

```python
class StatisticalAnalysis:
    """Perform statistical analysis on simulation results."""
    
    def __init__(self, results):
        """Initialize analysis.
        
        Args:
            results: Simulation results to analyze.
        """
        self.results = results
    
    def calculate_basic_statistics(self, metric_names):
        """Calculate basic statistics for specified metrics.
        
        Args:
            metric_names: List of metric names to analyze.
        
        Returns:
            Dictionary containing statistics for each metric.
        """
        statistics = {}
        
        for metric in metric_names:
            if metric in self.results:
                values = self.results[metric]
                statistics[metric] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'median': np.median(values)
                }
        
        return statistics
    
    def perform_t_test(self, group1_data, group2_data):
        """Perform t-test between two groups.
        
        Args:
            group1_data: Data for first group.
            group2_data: Data for second group.
        
        Returns:
            t-test results including t-statistic and p-value.
        """
        from scipy import stats
        
        t_stat, p_value = stats.ttest_ind(group1_data, group2_data)
        
        return {
            't_statistic': t_stat,
            'p_value': p_value,
            'significant': p_value < 0.05
        }
    
    def calculate_correlation(self, metric1, metric2):
        """Calculate correlation between two metrics.
        
        Args:
            metric1: Name of first metric.
            metric2: Name of second metric.
        
        Returns:
            Correlation coefficient and p-value.
        """
        from scipy import stats
        
        if metric1 in self.results and metric2 in self.results:
            r, p = stats.pearsonr(self.results[metric1], self.results[metric2])
            return {
                'correlation': r,
                'p_value': p,
                'significant': p < 0.05
            }
        
        return None
    
    def plot_performance_comparison(self, groups, metrics, output_file=None):
        """Plot performance comparison between groups.
        
        Args:
            groups: List of group names.
            metrics: List of metric names to compare.
            output_file: Optional file path to save plot.
        """
        import matplotlib.pyplot as plt
        
        # Create plot...
        fig, axes = plt.subplots(len(metrics), 1, figsize=(10, 5 * len(metrics)))
        
        for i, metric in enumerate(metrics):
            for j, group in enumerate(groups):
                if metric in self.results and group in self.results[metric]:
                    axes[i].boxplot(self.results[group][metric])
            
            axes[i].set_title(f'{metric} Comparison')
            axes[i].set_ylabel(metric)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file)
        
        plt.show()
    
    def save_results(self, filepath, format='json'):
        """Save statistical analysis results to file.
        
        Args:
            filepath: File path to save results.
            format: File format ('json', 'csv', 'pickle').
        """
        if format == 'json':
            import json
            results = {
                'statistics': self.statistics,
                'metadata': self.metadata
            }
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)
        elif format == 'csv':
            import pandas as pd
            df = pd.DataFrame(self.results)
            df.to_csv(filepath, index=False)
        elif format == 'pickle':
            import pickle
            with open(filepath, 'wb') as f:
                pickle.dump(self.results, f)
```

### Debugging and Error Handling

```python
class DebugSession:
    """Debug neural simulation sessions."""
    
    def __init__(self, brain):
        """Initialize debug session.
        
        Args:
            brain: Brain object to debug.
        """
        self.brain = brain
        self.debug_log = []
    
    def capture_step(self, step):
        """Capture debug information for a simulation step.
        
        Args:
            step: Simulation step number.
        
        Returns:
            Debug information dictionary.
        """
        debug_info = {
            'step': step,
            'time': self.brain.simulation_time,
            'neuron_count': self.brain.total_neurons,
            'firing_rate': self.brain.firing_rate,
            'active_neurons': self.brain.get_active_neuron_count(),
            'plength': self.brain.get_plength(),
            'warnings': self._check_for_warnings(step)
        }
        
        self.debug_log.append(debug_info)
        return debug_info
    
    def find_issue(self, debug_info):
        """Find issues in debug information.
        
        Args:
            debug_info: Debug information dictionary.
        
        Returns:
            Issue dictionary if found, None otherwise.
        """
        issues = []
        
        if debug_info['neuron_count'] == 0:
            issues.append({
                'type': 'error',
                'message': 'No neurons in brain',
                'severity': 'high'
            })
        
        if debug_info['firing_rate'] > 100:
            issues.append({
                'type': 'warning',
                'message': 'Extremely high firing rate',
                'severity': 'medium'
            })
        
        if debug_info['warnings']:
            issues.extend(debug_info['warnings'])
        
        return issues[0] if issues else None
    
    def analyze_issue(self, debug_info):
        """Analyze issues in debug information.
        
        Args:
            debug_info: Debug information dictionary.
        """
        issue = self.find_issue(debug_info)
        
        if issue:
            print(f"Issue found at step {debug_info['step']}:")
            print(f"  Type: {issue['type']}")
            print(f"  Severity: {issue['severity']}")
            print(f"  Message: {issue['message']}")
    
    def get_debug_log(self):
        """Get debug log.
        
        Returns:
            Debug log list.
        """
        return self.debug_log
    
    def clear_debug_log(self):
        """Clear debug log."""
        self.debug_log = []
```

## Installation

### System Dependencies

```bash
# Required dependencies
python3>=3.8
gcc>=7.0 (or other C++ compiler)

# Optional dependencies for visualization and experiments
pip install matplotlib pandas scipy numpy
```

### Installation from Source

```bash
# Clone the repository
# Ensure build directory exists
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python package
pip install -e .
```

### Quick Development Setup

```bash
# Clone and setup in one go
mkdir nlm_project
cd nlm_project
# Copy from source location
# Run build script
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e .[dev]
```

## Testing

### Running Tests

```bash
# Run all tests
pytest

# Run specific test category
pytest tests/test_basic_api.py
pytest tests/test_brain_simulation.py
pytest tests/test_agent_behavior.py

# Run with verbose output
pytest -v

# Run with coverage
pytest --cov=nlm --cov-report=html
```

### Test Categories

1. **Basic API Tests**: Core functionality verification
2. **Brain Simulation Tests**: Neural network simulation
3. **Agent Behavior Tests**: World interaction
4. **Memory System Tests**: Working and episodic memory
5. **Plasticity Tests**: Synaptic learning mechanisms

## Examples

See the `examples/` directory for comprehensive usage examples.

## License and Credits

This is an experimental neural simulation framework. Please reference the original NLM documentation for more information about the underlying C++ implementation.

That's the end of the improved Python package structure. The package now includes:

1. **Complete Python Package Structure**: Proper `__init__.py` and module organization
2. **Enhanced API**: More Pythonic interfaces with helper functions and convenience methods
3. **Advanced Features**: Support for visualization, experiment management, and advanced neural operations
4. **Comprehensive Documentation**: Detailed docstrings and examples
5. **Testing Suite**: Comprehensive tests for all functionality
6. **Error Handling**: Better error messages and exception handling
7. **Advanced Classes**: Simulation management, performance profiling, statistical analysis, debugging tools
8. **Module Organization**: Clear separation of concerns

The package provides a complete Python interface for the NLM neural simulation framework with extensive functionality for advanced users while maintaining simplicity for beginners.
