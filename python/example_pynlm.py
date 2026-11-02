#!/usr/bin/env python3
"""
Comprehensive example demonstrating the improved pynlm Python bindings.
This script showcases the new modular structure, enhanced API, and better
user experience features.
"""

import sys
import os
import tempfile
import json
import numpy as np
from pathlib import Path

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm

def main():
    print("=" * 70)
    print("pynlm - Neural Learning Machine Python Bindings Example")
    print("=" * 70)
    print()
    
    print("1. Creating a basic NLM system using factory functions")
    print("-" * 50)
    
    # Use the enhanced factory function to create a complete system
    brain, world, agent_brain = pynlm.create_nlm_system()
    
    print(f"✓ Created brain: {brain}")
    print(f"✓ Created world: {world}")
    print(f"✓ Created agent brain: {agent_brain}")
    
    # Initialize the agent brain with the world
    agent_brain.initialize(world)
    print("✓ Initialized agent brain with world")
    
    print()
    print("2. Exploring the brain system")
    print("-" * 50)
    
    # Get system statistics
    stats = pynlm.get_system_stats(brain)
    print(f"System Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print()
    print("3. Demonstrating improved ID types")
    print("-" * 50)
    
    # Show enhanced ID type functionality
    neuron_id = pynlm.NeuronId(42)
    print(f"Created NeuronId: {neuron_id}")
    print(f"  value: {neuron_id.value}")
    print(f"  index: {neuron_id.index()}")
    print(f"  hash: {hash(neuron_id)}")
    print(f"  string: {str(neuron_id)}")
    print(f"  repr: {repr(neuron_id)}")
    
    print()
    print("4. Working with sensory input")
    print("-" * 50)
    
    # Create vision sensory input
    vision = pynlm.Vision(640, 480, 3)
    print(f"Created Vision: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Simulate some image data
    test_data = list(range(640 * 480 * 3))  # Simple gradient pattern
    vision.setData(test_data)
    
    # Access data
    retrieved_data = vision.getData()
    print(f"Set and retrieved {len(retrieved_data)} pixels")
    print(f"First pixel: {retrieved_data[0]}, Middle pixel: {retrieved_data[640*480*3//2]}, Last pixel: {retrieved_data[-1]}")
    
    print()
    print("5. Exploring action system")
    print("-" * 50)
    
    # Create various actions
    actions = [
        pynlm.Action(pynlm.ActionType.MoveForward),
        pynlm.Action(pynlm.ActionType.TurnLeft, [0.5]),
        pynlm.Action(pynlm.ActionType.TurnRight, [1.0]),
        pynlm.Action(pynlm.ActionType.Interact),
        pynlm.Action(pynlm.ActionType.Eat, [2.5, 1.0]),
    ]
    
    for i, action in enumerate(actions):
        print(f"Action {i+1}: {action}")
        print(f"  Type: {action.getName()}")
        print(f"  Parameters: {action.getParameters()}")
        
        # Clone action
        cloned = action.clone()
        assert cloned.getType() == action.getType()
        assert cloned.getParameters() == action.getParameters()
        print(f"  ✓ Successfully cloned")
    
    print()
    print("6. World simulation")
    print("-" * 50)
    
    # Configure world
    world.configure(100, 100, 32, 32)
    world.setAgentStart(50, 50)
    
    print(f"World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent start position: (50, 50)")
    
    # Get initial agent state
    initial_body = world.getAgentBody()
    print(f"Initial agent state:")
    print(f"  Position: ({initial_body.x}, {initial_body.y})")
    print(f"  Energy: {initial_body.energy}")
    print(f"  Is moving: {initial_body.isMoving}")
    
    # Update world state
    world.update(0.1)
    print(f"After update, simulation time: {world.getSimulationTime()}")
    
    # Move agent
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    world.applyMotorCommand(action, 0.1)
    print(f"Applied MoveForward action")
    print(f"New agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"Agent energy: {world.getAgentBody().energy}")
    
    print()
    print("7. Brain simulation with world integration")
    print("-" * 50)
    
    # Configure brain with parameters
    brain_config = pynlm.Config()
    brain_config.set("neuron_count", 100)
    brain_config.set("region_count", 2)
    brain_config.set("connection_probability", 0.1)
    
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    print("Brain initialized with config:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation for a few steps
    for step in range(3):
        # Get current state
        percept = world.getSensoryPercept()
        action = brain.produceAction()
        
        print(f"Step {step}: Action = {action.getName()}")
        
        # Apply action to world
        world.applyMotorCommand(action, step * 0.1)
        
        # Step brain with timestamp
        brain.step(step, step * 0.1)
        
        # Update development
        agent_brain.updateDevelopment(step)
        
        # Get current world state
        body = world.getAgentBody()
        stats = pynlm.get_system_stats(brain)
        
        print(f"  Agent position: ({body.x:.1f}, {body.y:.1f})")
        print(f"  Active neurons: {stats['active_neurons']}")
        print(f"  Firing neurons: {stats['firing_neurons']}")
        print(f"  Average firing rate: {stats['avg_firing_rate']:.2f}")
    
    print()
    print("8. Neuromodulation and development")
    print("-" * 50)
    
    # Enable various systems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableStructuralPlasticity(True)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    
    print("Enabled systems:")
    print(f"  Reward modulation: {agent_brain.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent_brain.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent_brain.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent_brain.isCuriosityEnabled()}")
    
    # Get current neuromodulation levels
    neuromod = agent_brain.getNeuromodulationLevel()
    curiosity = agent_brain.getCuriosityLevel()
    novelty = agent_brain.getNoveltyLevel()
    prediction_error = agent_brain.getPredictionError()
    
    print(f"Current neuromodulation levels:")
    print(f"  Neuromodulation: {neuromod}")
    print(f"  Curiosity: {curiosity}")
    print(f"  Novelty: {novelty}")
    print(f"  Prediction error: {prediction_error}")
    
    print()
    print("9. Advanced features - simulation utilities")
    print("-" * 50)
    
    # Run a complete simulation using the utility function
    print("Running complete simulation...")
    
    # Create fresh system
    brain, world, agent_brain = pynlm.create_nlm_system()
    agent_brain.initialize(world)
    
    # Run simulation
    body = pynlm.run_simulation(brain, world, 5, 0.1)
    
    print(f"Simulation completed!")
    print(f"Final agent position: ({body.x:.1f}, {body.y:.1f})")
    print(f"Final agent energy: {body.energy:.1f}")
    print(f"Final agent health: {body.health}")
    print(f"Final agent age: {body.age}")
    
    print()
    print("10. Error handling and safety")
    print("-" * 50)
    
    # Demonstrate safe step with error handling
    print("Testing safe step function...")
    try:
        success = pynlm.safe_step(brain, 0, 0.0)
        print(f"Safe step succeeded: {success}")
    except Exception as e:
        print(f"Safe step failed with error: {e}")
    
    print()
    print("11. Configuration file operations")
    print("-" * 50)
    
    # Save and load configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 3)
    config.set("connection_probability", 0.05)
    
    print("Original config:")
    print(f"  Neuron count: {config.get('neuron_count')}")
    print(f"  Region count: {config.get('region_count')}")
    print(f"  Connection probability: {config.get('connection_probability')}")
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        temp_file = f.name
    
    try:
        # Load from file
        config2 = pynlm.Config()
        config2.loadFromFile(temp_file)
        
        print("Loaded config from file:")
        print(f"  Neuron count: {config2.get('neuron_count')}")
        print(f"  Region count: {config2.get('region_count')}")
        print(f"  Connection probability: {config2.get('connection_probability')}")
        
        # Verify data integrity
        assert config2.get("neuron_count") == 200
        assert config2.get("region_count") == 3
        assert config2.get("connection_probability") == 0.05
        print("✓ Configuration data integrity verified!")
        
    finally:
        os.unlink(temp_file)
    
    print()
    print("=" * 70)
    print("Example completed successfully!")
    print("=" * 70)
    print()
    print("Key improvements demonstrated:")
    print("• Modular, maintainable code structure")
    print("• Enhanced factory functions for easy system creation")
    print("• Improved ID types with better string representations")
    print("• Comprehensive sensory input handling")
    print("• Action cloning and manipulation")
    print("• World and brain integration")
    print("• Neuromodulation and development support")
    print("• Simulation utilities and error handling")
    print("• Configuration file operations")
    print("• Comprehensive testing framework")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error: Could not import pynlm module.")
        print(f"Please build the Python bindings first:")
        print(f"  mkdir build && cd build && cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON && make")
        sys.exit(1)
    except Exception as e:
        print(f"Error during execution: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)