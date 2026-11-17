#!/usr/bin/env python3
"""
NLM Python Interface Examples
=============================

This script demonstrates the Python interface to NLM (Neural Learning Machine),
demonstrating both basic and advanced usage patterns.

Note: This is a conceptual example. The actual Python bindings (pynlm)
would be implemented using pybind11 or similar binding technology.
"""

import json
import time
from typing import Dict, Any, Optional

# Mock NLM classes for demonstration
class MockBrain:
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.neuron_count = config.get('neuron_count', 1000)
        self.region_count = config.get('region_count', 2)
        self.spike_count = 0
        self.firing_neurons = 0
        print(f"✓ Brain initialized with {self.neuron_count} neurons")
    
    def initialize(self):
        print("✓ Brain initialized (setup neurons and synapses)")
        return True
    
    def step(self, step_num: int):
        # Simulate neural computation
        import random
        self.spike_count += random.randint(0, 10)
        self.firing_neurons = random.randint(0, self.neuron_count // 10)
        return self.spike_count, self.firing_neurons
    
    def getTotalSpikeCount(self):
        return self.spike_count
    
    def getFiringNeuronCount(self):
        return self.firing_neurons
    
    def logStatus(self):
        print(f"Brain Status: {self.spike_count} total spikes, {self.firing_neurons} firing neurons")

class MockWorld:
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.width = config.get('width', 10)
        self.height = config.get('height', 10)
        self.vision_width = config.get('vision_width', 8)
        self.vision_height = config.get('vision_height', 8)
        self.time = 0.0
        print(f"✓ World initialized ({self.width}x{self.height})")
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        print(f"✓ World configured: {width}x{height}, vision {vision_width}x{vision_height}")
    
    def reset(self):
        self.time = 0.0
        print("✓ World reset")
    
    def update(self, dt: float):
        self.time += dt
        return self.time
    
    def getSensoryPercept(self):
        # Return a mock sensory perception
        import random
        return {
            'vision': [[random.random() for _ in range(self.vision_width)] 
                      for _ in range(self.vision_height)],
            'position': [self.width // 2, self.height // 2],
            'time': self.time
        }
    
    def applyMotorCommand(self, action: Dict[str, Any], simulation_time: float):
        print(f"✓ Applied motor command: {action.get('type', 'unknown')} at time {simulation_time}")

class MockAgent:
    def __init__(self, brain: MockBrain):
        self.brain = brain
        self.reward_modulation = False
        self.curiosity = False
        self.plasticity = False
        print("✓ AgentBrain created")
    
    def initialize(self, world: MockWorld):
        self.world = world
        print("✓ Agent initialized with world")
    
    def processSensoryInput(self, percept: Dict[str, Any]):
        # Process sensory input through brain
        print(f"✓ Agent processed sensory input (vision: {len(percept.get('vision', []))}x{len(percept.get('vision', [[]])[0]) if percept.get('vision') else 0})")
    
    def decodeMotorCommand(self):
        import random
        action_types = ['move_forward', 'turn_left', 'turn_right', 'stay']
        action_type = random.choice(action_types)
        return {
            'type': action_type,
            'confidence': random.random(),
            'timestamp': self.world.time if hasattr(self, 'world') else time.time()
        }
    
    def enableRewardModulation(self, enable: bool):
        self.reward_modulation = enable
        print(f"✓ Reward modulation {'enabled' if enable else 'disabled'}")
    
    def enableCuriosity(self, enable: bool):
        self.curiosity = enable
        print(f"✓ Curiosity module {'enabled' if enable else 'disabled'}")
    
    def enableStructuralPlasticity(self, enable: bool):
        self.plasticity = enable
        print(f"✓ Structural plasticity {'enabled' if enable else 'disabled'}")

def createDefaultConfig():
    """Create a default NLM configuration."""
    return {
        'random_seed': 42,
        'simulation_timestep': 0.001,
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'log_level': 'INFO',
        'log_to_file': False,
        'enable_reward_modulation': False,
        'enable_curiosity': False,
        'enable_structural_plasticity': False
    }

def saveConfig(config: Dict[str, Any], filename: str):
    """Save configuration to file in JSON format."""
    with open(filename, 'w') as f:
        json.dump(config, f, indent=2)
    print(f"✓ Configuration saved to {filename}")

def loadConfig(filename: str) -> Dict[str, Any]:
    """Load configuration from file."""
    try:
        with open(filename, 'r') as f:
            config = json.load(f)
        print(f"✓ Configuration loaded from {filename}")
        return config
    except FileNotFoundError:
        print(f"⚠ Configuration file {filename} not found, using defaults")
        return createDefaultConfig()
    except json.JSONDecodeError as e:
        print(f"⚠ Invalid JSON in {filename}: {e}")
        return createDefaultConfig()

def runSimpleSimulation():
    """Run a simple simulation with just a brain."""
    print("\n" + "="*60)
    print("EXAMPLE 1: Simple Brain Simulation")
    print("="*60)
    
    # Create brain with default config
    config = createDefaultConfig()
    brain = MockBrain(config)
    
    # Initialize brain
    brain.initialize()
    
    # Run for 100 steps
    print("\nRunning simulation for 100 steps...")
    for i in range(100):
        spikes, firing = brain.step(i)
        if i % 20 == 0:
            print(f"  Step {i}: {spikes} total spikes, {firing} firing neurons")
    
    print(f"\nFinal Results:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Max firing rate: {brain.getFiringNeuronCount()}")
    print("✓ Simple simulation complete!")

def runCompleteAgentSimulation():
    """Run a complete agent simulation with world interaction."""
    print("\n" + "="*60)
    print("EXAMPLE 2: Complete Agent in a World")
    print("="*60)
    
    # Setup configuration
    config = createDefaultConfig()
    config.update({
        'width': 15,
        'height': 15,
        'vision_width': 8,
        'vision_height': 8,
        'enable_reward_modulation': True,
        'enable_curiosity': True,
        'enable_structural_plasticity': True
    })
    
    # Create all components
    brain = MockBrain(config)
    world = MockWorld(config)
    
    # Configure world
    world.configure(15, 15, 8, 8)
    
    # Create agent
    agent = MockAgent(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    print("\nRunning complete agent simulation for 50 steps...")
    brain.initialize()
    world.reset()
    
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get what agent sees
        percept = world.getSensoryPercept()
        
        # Tell brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides action
        action = agent.decodeMotorCommand()
        
        # Do the action in world
        world.applyMotorCommand(action, world.time)
        
        # Log progress
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes, {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"\nFinal Results:")
    print(f"  Simulation completed: {brain.getTotalSpikeCount()} total spikes")
    print(f"  Activity: {brain.getFiringNeuronCount()} firing neurons")
    print("✓ Complete agent simulation complete!")

def runAdvancedFeaturesExample():
    """Demonstrate advanced features."""
    print("\n" + "="*60)
    print("EXAMPLE 3: Advanced Features")
    print("="*60)
    
    # Create advanced config
    config = createDefaultConfig()
    config.update({
        'neuron_count': 5000,
        'region_count': 4,
        'connection_probability': 0.15,
        'enable_reward_modulation': True,
        'enable_curiosity': True,
        'enable_structural_plasticity': True,
        'enable_experimental_features': True
    })
    
    # Save advanced config
    saveConfig(config, "advanced_config.json")
    
    # Load config (simulating user loading from file)
    loaded_config = loadConfig("advanced_config.json")
    
    # Create components with advanced config
    brain = MockBrain(loaded_config)
    world = MockWorld(loaded_config)
    agent = MockAgent(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"\nAdvanced Configuration:")
    print(f"  Neuron count: {loaded_config['neuron_count']}")
    print(f"  Region count: {loaded_config['region_count']}")
    print(f"  Connection probability: {loaded_config['connection_probability']}")
    print(f"  Learning features: Reward={loaded_config['enable_reward_modulation']}, "
          f"Curiosity={loaded_config['enable_curiosity']}, "
          f"Plasticity={loaded_config['enable_structural_plasticity']}")
    
    # Run shorter simulation for advanced example
    print("\nRunning advanced simulation (20 steps)...")
    brain.initialize()
    world.reset()
    
    for step in range(20):
        world.update(0.05)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.time)
    
    print(f"\nAdvanced Features Results:")
    print(f"  Simulation with advanced features: {brain.getTotalSpikeCount()} spikes")
    print(f"  Activity level: {brain.getFiringNeuronCount()} firing neurons")
    print("✓ Advanced features example complete!")

def main():
    """Main function demonstrating all examples."""
    print("NLM Python Interface Examples")
    print("=============================")
    print("This script demonstrates the Python interface to NLM.")
    print("Note: This is a conceptual example using mock classes.")
    
    # Run all examples
    runSimpleSimulation()
    runCompleteAgentSimulation()
    runAdvancedFeaturesExample()
    
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print("This script demonstrates:")
    print("1. Simple brain simulation without a world")
    print("2. Complete agent with world interaction and learning")
    print("3. Advanced features with large networks and multiple learning systems")
    print("\nFor the real pynlm Python bindings, refer to the NLM documentation")
    print("or check the official Python interface implementation.")

if __name__ == "__main__":
    main()