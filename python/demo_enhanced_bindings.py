#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings - Comprehensive Demonstration
=========================================================

This script demonstrates the enhanced Python API for the NLM (Neural Learning Machine)
framework. The enhanced bindings provide:

1. Pythonic API with context managers for resource management
2. Comprehensive error handling with custom Python exceptions
3. Statistical analysis utilities for neural data
4. Debugging and development tools
5. Serialization utilities for brain state management
6. Experiment management and benchmarking
7. Property accessors and improved method naming
8. Type safety and better documentation

To use these bindings, you would need to:
1. Build the enhanced bindings (requires CMake, pybind11, Python development headers)
2. Install the module via pip
3. Import and use the classes as shown in this demonstration
"""

import sys
import json
from datetime import datetime
from typing import List, Dict, Tuple, Optional, Any

# Note: This is a demonstration of the API. The actual bindings would be:
# from pynlm_enhanced import (
#     Config, Brain, SimpleWorld, AgentBrain, Neuron, NeuralRegion,
#     Vision, Audio, InternalSignals, Action, SensoryPercept,
#     WorldObject, AgentBody, ActionResult, STDP, Hebbian, StructuralPlasticity,
#     Neuromodulator, Reward, Curiosity, Novelty, PredictionError,
#     WorkingMemory, EpisodicMemory, PredictionSystem,
#     NeuralPlanner, ConceptFormation, AttentionalSelection,
#     DevelopmentSystem, Synaptogenesis, Pruning, Maturation,
#     SpikeSystem, NLMException, NLMConfigError, NLMSimulationError,
#     compute_statistics, normalize_vector, timestamp_to_string,
#     format_neuron_stats, format_world_stats,
#     print_brain_state, print_neuron_statistics,
#     write_debug_log, save_brain, load_brain,
#     save_config, load_config, run_simulation
# )

print("Enhanced NLM Python Bindings Demonstration")
print("=" * 50)
print()

# Mock classes to demonstrate API (not actual implementations)
class MockNeuron:
    """Mock Neuron class for demonstration"""
    def __init__(self, neuron_id):
        self.neuron_id = neuron_id
        self.type = "Excitatory"
        self.potential = -70.0
        self.threshold = -55.0
        self.is_firing = False
        
    def __repr__(self):
        return f"<MockNeuron id={self.neuron_id}, type={self.type}, potential={self.potential:.1f}>"

class MockBrain:
    """Mock Brain class for demonstration"""
    def __init__(self, config=None):
        self.config = config or {}
        self.neurons = []
        self.total_spike_count = 0
        self.development_stage = "Adult"
        self.average_firing_rate = 0.0
        
    def initialize(self):
        """Initialize the brain with configuration"""
        print("Brain initialized with config:", self.config)
        return self
        
    def add_neuron(self, neuron):
        """Add a neuron to the brain"""
        self.neurons.append(neuron)
        return self
        
    def step(self, current_step, current_time):
        """Perform a simulation step"""
        print(f"Brain step: {current_step}, time: {current_time:.3f}s")
        return self
        
    def inject_current(self, neuron_id, current):
        """Inject current into a specific neuron"""
        print(f"Injected current {current:.3f} into neuron {neuron_id}")
        return self
        
    def inject_current_to_neurons(self, neuron_type, current):
        """Inject current into all neurons of a specific type"""
        print(f"Injected current {current:.3f} to all {neuron_type} neurons")
        return self
        
    def get_total_spike_count(self):
        """Get total spike count"""
        return self.total_spike_count
        
    def get_developmental_stage(self):
        """Get current developmental stage"""
        return self.development_stage
        
    def set_developmental_stage(self, stage):
        """Set developmental stage"""
        self.development_stage = stage
        print(f"Developmental stage set to: {stage}")
        return self
        
    def get_average_firing_rate(self):
        """Get average firing rate"""
        return self.average_firing_rate
        
    def save(self, filepath):
        """Save brain state to file"""
        print(f"Saved brain state to: {filepath}")
        return True
        
    def load(self, filepath):
        """Load brain state from file"""
        print(f"Loaded brain state from: {filepath}")
        return True
        
    def reset(self):
        """Reset brain state"""
        print("Brain reset")
        return self

class MockWorld:
    """Mock World class for demonstration"""
    def __init__(self):
        self.width = 20.0
        self.height = 20.0
        self.agent = None
        self.objects = []
        self.simulation_time = 0.0
        
    def configure(self, width, height, vision_width, vision_height):
        """Configure world dimensions"""
        self.width = width
        self.height = height
        print(f"World configured: {width}x{height}, vision: {vision_width}x{vision_height}")
        return self
        
    def reset(self):
        """Reset world to initial state"""
        print("World reset")
        return self
        
    def set_agent_start(self, x, y):
        """Set agent starting position"""
        if not self.agent:
            self.agent = MockAgentBody()
        self.agent.x = x
        self.agent.y = y
        print(f"Agent start position set to: ({x}, {y})")
        return self
        
    def update(self, timestep):
        """Update world state"""
        self.simulation_time += timestep
        print(f"World update: timestep={timestep:.3f}s, total time={self.simulation_time:.3f}s")
        return self
        
    def get_sensory_percept(self):
        """Get current sensory percept"""
        return MockSensoryPercept()
        
    def get_agent_body(self):
        """Get agent body state"""
        return self.agent or MockAgentBody()
        
    def get_width(self):
        """Get world width"""
        return self.width
        
    def get_height(self):
        """Get world height"""
        return self.height
        
    def get_simulation_time(self):
        """Get simulation time"""
        return self.simulation_time
        
    def set_random_seed(self, seed):
        """Set random seed for reproducibility"""
        print(f"Random seed set to: {seed}")
        return self

class MockAgentBody:
    """Mock AgentBody for demonstration"""
    def __init__(self):
        self.x = 10.0
        self.y = 10.0
        self.orientation = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.is_moving = False
        
    @property
    def position(self):
        """Agent position (Python property)"""
        return (self.x, self.y)
        
    @property
    def velocity(self):
        """Agent velocity (Python property)"""
        return (0.0, 0.0)  # Mock velocity
        
    @property
    def speed(self):
        """Agent speed (calculated Python property)"""
        import math
        return math.sqrt(self.x**2 + self.y**2)
        
    def reset(self):
        """Reset agent"""
        self.x = 10.0
        self.y = 10.0
        self.energy = 100.0
        self.health = 100.0
        print("Agent reset")

class MockSensoryPercept:
    """Mock SensoryPercept for demonstration"""
    def __init__(self):
        self.vision = [0.1] * 256
        self.touch = [0.0] * 8
        self.internal = [0.0] * 4
        self.proprioception = [0.0] * 6
        self.timestamp = 0.0
        
    @property
    def vision_shape(self):
        """Vision shape (Python property)"""
        return [16, 16, 1]  # width, height, channels
        
    def get_all_signals(self):
        """Get all sensory signals concatenated"""
        all_signals = self.vision + self.touch + self.internal + self.proprioception
        return all_signals

class MockAction:
    """Mock Action for demonstration"""
    def __init__(self, action_type="MoveForward", parameters=None):
        self.type = action_type
        self.parameters = parameters or []
        
    @property
    def type(self):
        """Action type (Python property)"""
        return self._type
        
    @type.setter
    def type(self, value):
        self._type = value
        
    @property
    def parameters(self):
        """Action parameters (Python property)"""
        return self._parameters
        
    @parameters.setter
    def parameters(self, value):
        self._parameters = value
        
    def get_name(self):
        """Get human-readable action name"""
        return self.type.replace("_", " ").title()
        
    def clone(self):
        """Clone the action"""
        return MockAction(self.type, self.parameters.copy())

# Statistics and analysis functions

def compute_statistics(data):
    """Compute mean and standard deviation of data vector"""
    if not data:
        return (0.0, 0.0)
    
    mean = sum(data) / len(data)
    variance = sum((x - mean) ** 2 for x in data) / len(data)
    stddev = variance ** 0.5
    
    return mean, stddev


def normalize_vector(data, min_val=0.0, max_val=1.0):
    """Normalize vector to [0, 1] range"""
    if max_val == min_val:
        return [0.5] * len(data)
    
    return [(x - min_val) / (max_val - min_val) for x in data]


def timestamp_to_string(timestamp):
    """Convert timestamp to human-readable string"""
    seconds = int(timestamp)
    millis = int((timestamp - seconds) * 1000)
    return f"{seconds}.{millis:03d}s"


def format_neuron_stats(total, active, firing, rate):
    """Format neuron statistics as string"""
    result = f"Neuron Statistics:\n"
    result += f"  Total: {total}\n"
    if total > 0:
        active_percent = (active / total) * 100
        firing_percent = (firing / total) * 100
        result += f"  Active: {active} ({active_percent:.1f}%)\n"
        result += f"  Firing: {firing} ({firing_percent:.1f}%)\n"
    result += f"  Average Firing Rate: {rate} Hz\n"
    return result


def format_world_stats(world):
    """Format world statistics as string"""
    return f"World Statistics:\n" \
           f"  Dimensions: {world.get_width()} x {world.get_height()}\n" \
           f"  Simulation Time: {world.get_simulation_time()}s\n"

# Debug and analysis utilities

def print_brain_state(brain):
    """Print brain state to console"""
    print("=== BRAIN STATE ===")
    print(f"Total Neurons: {len(brain.neurons)}")
    print(f"Total Spike Count: {brain.get_total_spike_count()}")
    print(f"Developmental Stage: {brain.get_developmental_stage()}")
    print(f"Average Firing Rate: {brain.get_average_firing_rate()} Hz")


def print_neuron_statistics(brain):
    """Print detailed neuron statistics"""
    print("=== NEURON STATISTICS ===")
    for neuron in brain.neurons:
        print(f"  {neuron}")


def write_debug_log(filename, message):
    """Write debug message to log file"""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(filename, 'a') as f:
        f.write(f"[{timestamp}] {message}\n")

# Serialization utilities

def save_brain(brain, filepath):
    """Save brain state to file"""
    print(f"Saving brain state to: {filepath}")
    # In real implementation, would serialize brain data
    return True


def load_brain(brain, filepath):
    """Load brain state from file"""
    print(f"Loading brain state from: {filepath}")
    # In real implementation, would deserialize brain data
    return True


def save_config(config, filepath):
    """Save configuration to JSON file"""
    print(f"Saving config to: {filepath}")
    # In real implementation, would serialize config
    return True


def load_config(config, filepath):
    """Load configuration from JSON file"""
    print(f"Loading config from: {filepath}")
    # In real implementation, would deserialize config
    return True

# Experiment utilities

class BenchmarkResult:
    """Benchmark results container"""
    def __init__(self, name, time_seconds, steps_completed, memory_usage_mb):
        self.name = name
        self.time_seconds = time_seconds
        self.steps_completed = steps_completed
        self.memory_usage_mb = memory_usage_mb
        self.metrics = {}
        
    def add_metric(self, name, value):
        """Add a metric to the benchmark result"""
        self.metrics[name] = value
        
    def to_string(self):
        """Format benchmark results as string"""
        result = f"=== BENCHMARK: {self.name} ===\n"
        result += f"Time: {self.time_seconds} seconds\n"
        result += f"Steps: {self.steps_completed}\n"
        result += f"Memory: {self.memory_usage_mb} MB\n"
        for name, value in self.metrics.items():
            result += f"{name}: {value}\n"
        return result


def run_simulation(brain, world, duration_seconds, target_fps=60):
    """Run simulation for specified duration and return benchmark results"""
    print(f"Running simulation for {duration_seconds} seconds...")
    
    # Simulate steps
    steps_completed = int(duration_seconds * target_fps)
    timestep = 1.0 / target_fps
    
    for step in range(steps_completed):
        current_time = step * timestep
        brain.step(step, current_time)
        world.update(timestep)
        
    # Create benchmark result
    result = BenchmarkResult(
        name="Simulation Benchmark",
        time_seconds=duration_seconds,
        steps_completed=steps_completed,
        memory_usage_mb=50.0  # Mock value
    )
    result.add_metric("Average Firing Rate", brain.get_average_firing_rate())
    result.add_metric("Total Spike Count", brain.get_total_spike_count())
    
    print(result.to_string())
    return result

# Custom exception classes

class NLMException(Exception):
    """Base exception for NLM errors"""
    pass


class NLMConfigError(NLMException):
    """Configuration error"""
    pass


class NLMSimulationError(NLMException):
    """Simulation error"""
    pass


class NLMResourceError(NLMException):
    """Resource management error"""
    pass

# Context manager classes

class BrainManager:
    """Context manager for automatic brain resource cleanup"""
    def __init__(self, brain):
        self.brain = brain
        
    def __enter__(self):
        print(f"Entering brain context: {self.brain}")
        return self.brain
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        print(f"Exiting brain context: {self.brain}")
        self.brain.reset()
        return False  # Don't suppress exceptions


class WorldManager:
    """Context manager for automatic world resource cleanup"""
    def __init__(self, world):
        self.world = world
        
    def __enter__(self):
        print(f"Entering world context: {self.world}")
        return self.world
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        print(f"Exiting world context: {self.world}")
        self.world.reset()
        return False  # Don't suppress exceptions


class AgentBrainManager:
    """Context manager for automatic agent brain resource cleanup"""
    def __init__(self, agent_brain):
        self.agent_brain = agent_brain
        
    def __enter__(self):
        print(f"Entering agent brain context: {self.agent_brain}")
        return self.agent_brain
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        print(f"Exiting agent brain context: {self.agent_brain}")
        self.agent_brain.reset()
        return False  # Don't suppress exceptions

# Factory functions

def create_default_config():
    """Create a default configuration"""
    return {"default": "config"}


def create_brain(config=None):
    """Create a new brain with configuration"""
    return MockBrain(config)


def create_simple_world():
    """Create a new simple world"""
    return MockWorld()


def create_agent_brain(brain):
    """Create a new agent brain interface"""
    return brain  # In real implementation, would create AgentBrain

# Main demonstration function

def main():
    """Run the enhanced bindings demonstration"""
    print("Enhanced NLM Python Bindings Demonstration")
    print("=" * 60)
    print()
    
    # Create objects using factory functions
    print("1. Creating objects using factory functions:")
    config = create_default_config()
    brain = create_brain(config)
    world = create_simple_world()
    
    print(f"   Config: {config}")
    print(f"   Brain: {brain}")
    print(f"   World: {world}")
    print()
    
    # Configure world
    print("2. Configuring world:")
    world.configure(width=30.0, height=30.0, vision_width=32, vision_height=32)
    world.set_agent_start(15.0, 15.0)
    print()
    
    # Initialize brain
    print("3. Initializing brain:")
    brain.initialize()
    
    # Add some neurons for demonstration
    for i in range(5):
        neuron = MockNeuron(i)
        brain.add_neuron(neuron)
    
    print(f"   Added {len(brain.neurons)} neurons")
    print()
    
    # Demonstrate Pythonic properties
    print("4. Demonstrating Pythonic properties:")
    print(f"   Agent position: {world.get_agent_body().position}")
    print(f"   Agent speed: {world.get_agent_body().speed:.2f}")
    print(f"   Vision shape: {world.get_sensory_percept().vision_shape}")
    print()
    
    # Demonstrate context managers
    print("5. Demonstrating context managers:")
    with BrainManager(brain) as managed_brain:
        print(f"   Inside brain context: {managed_brain}")
        managed_brain.step(0, 0.0)
    
    with WorldManager(world) as managed_world:
        print(f"   Inside world context: {managed_world}")
        managed_world.update(0.1)
    print()
    
    # Demonstrate statistics and analysis
    print("6. Statistics and analysis:")
    vision_data = world.get_sensory_percept().vision
    mean, stddev = compute_statistics(vision_data)
    print(f"   Vision data statistics - Mean: {mean:.4f}, StdDev: {stddev:.4f}")
    
    normalized = normalize_vector(vision_data, min_val=0.0, max_val=1.0)
    print(f"   Normalized vision data range: [{min(normalized):.4f}, {max(normalized):.4f}]")
    
    neuron_stats = format_neuron_stats(len(brain.neurons), 3, 2, 15.5)
    print(f"   Formatted neuron stats:\n{neuron_stats}")
    
    world_stats = format_world_stats(world)
    print(f"   Formatted world stats:\n{world_stats}")
    print()
    
    # Demonstrate debugging utilities
    print("7. Debugging utilities:")
    print_brain_state(brain)
    print_neuron_statistics(brain)
    
    write_debug_log("demo_log.txt", "Brain initialized successfully")
    print("   Debug log written to demo_log.txt")
    print()
    
    # Demonstrate serialization
    print("8. Serialization utilities:")
    save_brain(brain, "brain_state.json")
    save_config(config, "config.json")
    
    load_brain(brain, "brain_state.json")
    load_config(config, "config.json")
    print()
    
    # Demonstrate experiment management
    print("9. Experiment management:")
    benchmark_result = run_simulation(brain, world, duration_seconds=2.0, target_fps=30)
    print(f"   Benchmark result:")
    print(benchmark_result.to_string())
    print()
    
    # Demonstrate error handling
    print("10. Error handling:")
    try:
        raise NLMConfigError("Invalid configuration format")
    except NLMConfigError as e:
        print(f"   Caught NLMConfigError: {e}")
    
    try:
        raise NLMSimulationError("Simulation failed")
    except NLMSimulationError as e:
        print(f"   Caught NLMSimulationError: {e}")
    
    try:
        raise NLMResourceError("Resource not available")
    except NLMResourceError as e:
        print(f"   Caught NLMResourceError: {e}")
    print()
    
    # Demonstrate advanced Pythonic features
    print("11. Advanced Pythonic features:")
    
    # List comprehension
    active_neurons = [n for n in brain.neurons if n.is_firing]
    print(f"   Active neurons found: {len(active_neurons)}")
    
    # Dictionary comprehension
    neuron_dict = {n.neuron_id: n for n in brain.neurons}
    print(f"   Neurons dictionary created with {len(neuron_dict)} entries")
    
    # Generator expression
    firing_rates = (n.potential * 0.1 for n in brain.neurons)
    avg_rate = sum(firing_rates) / len(brain.neurons) if brain.neurons else 0
    print(f"   Average neuron firing rate (simulated): {avg_rate:.2f}")
    
    # Enum usage
    class NeuronType:
        EXCITATORY = "excitatory"
        INHIBITORY = "inhibitory"
        MODULATORY = "modulatory"
        
    neuron_types = [NeuronType.EXCITATORY, NeuronType.INHIBITORY, NeuronType.MODULATORY]
    print(f"   Neuron types: {[nt.value for nt in neuron_types]}")
    print()
    
    print("Enhanced bindings demonstration completed!")
    print()
    print("Key features demonstrated:")
    print("  ✓ Pythonic API with properties and context managers")
    print("  ✓ Comprehensive error handling with custom exceptions")
    print("  ✓ Statistical analysis utilities")
    print("  ✓ Debugging and development tools")
    print("  ✓ Serialization utilities")
    print("  ✓ Experiment management and benchmarking")
    print("  ✓ Type safety and better documentation")
    print("  ✓ Pythonic features (comprehensions, generators, etc.)")

if __name__ == "__main__":
    main()