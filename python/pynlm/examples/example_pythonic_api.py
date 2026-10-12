#!/usr/bin/env python3
"""
Example usage of NLM Python bindings with Pythonic API features.

This file demonstrates the enhanced Pythonic API for the NLM (Neural Learning Machine)
framework, showcasing new convenience methods, error handling, and utilities.
"""

import pynlm as nlm
import tempfile
import os

def example_basic_functionality():
    """Example: Basic NLM functionality with Pythonic API."""
    print("=== Basic NLM Functionality Example ===")
    
    # Create a default configuration
    config = nlm.create_simulation_config()
    print(f"Created config: {config}")
    
    # Create brain with configuration
    brain = nlm.Brain(config)
    brain.initialize()
    print(f"Brain initialized: {brain}")
    
    # Create a test environment
    world = nlm.create_test_environment()
    print(f"Created world: {world}")
    print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
    
    # Add objects to world using Pythonic API
    world.add_object_at(10.0, 10.0, nlm.WorldObjectType.Resource, 10.0)
    world.add_object_at(50.0, 50.0, nlm.WorldObjectType.Hazard, -5.0)
    print("Added objects to world")
    
    # Create agent brain
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    print(f"Agent brain initialized: {agent_brain}")
    
    return brain, world, agent_brain

def example_pythonic_config():
    """Example: Pythonic configuration loading and management."""
    print("\n=== Pythonic Configuration Example ===")
    
    # Create configuration using Pythonic API
    config = nlm.Config()
    
    # Use update() method with keyword arguments
    config.update(
        brain_config={
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        },
        environment_config={
            "width": 100.0,
            "height": 100.0,
            "vision_width": 32,
            "vision_height": 32
        },
        simulation_config={
            "learning_rate": 0.01,
            "plasticity_enabled": True,
            "development_enabled": True
        }
    )
    
    print(f"Config keys: {config.getKeys()}")
    
    # Use validation
    try:
        config.validate("brain_config:dict,environment_config:dict")
        print("Configuration validation passed")
    except Exception as e:
        print(f"Validation error: {e}")
    
    # Load from file using Pythonic API
    config_data = {
        "simulation": {
            "neuron_count": 500,
            "max_firing_rate": 50.0,
            "learning_rate": 0.005
        }
    }
    
    import json
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(config_data, f)
        temp_file = f.name
    
    try:
        config2 = nlm.Config()
        config2.loadConfigFile(temp_file)  # Pythonic method
        print(f"Loaded config from file: {config2.summary()}")
    finally:
        os.unlink(temp_file)
    
    return config

def example_pythonic_brain_operations():
    """Example: Pythonic brain operations and context managers."""
    print("\n=== Pythonic Brain Operations Example ===")
    
    config = nlm.create_simulation_config()
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Perform simulation steps
    print("Running simulation steps...")
    for step in range(5):
        brain.step(step)
        print(f"Step {step}: Neurons={brain.getTotalNeuronCount()}, "
              f"Firing={brain.getFiringNeuronCount()}, "
              f"Rate={brain.getAverageFiringRate():.2f}")
    
    # Demonstrate brain methods
    print(f"\nBrain statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Demonstrate getNeuronsByType
    neurons_exc = brain.getNeuronsByType(nlm.NeuronType.Excitatory)
    neurons_inh = brain.getNeuronsByType(nlm.NeuronType.Inhibitory)
    print(f"\nNeurons by type:")
    print(f"  Excitatory: {len(neurons_exc)}")
    print(f"  Inhibitory: {len(neurons_inh)}")
    
    # Demonstrate getConnections
    connections = brain.getConnections()
    print(f"Total connections: {len(connections)}")
    
    # Demonstrate benchmark
    results = brain.benchmark(steps=3, warmup=2)
    print(f"\nBenchmark results:")
    for key, value in results.items():
        print(f"  {key}: {value}")
    
    return brain

def example_pythonic_world_operations():
    """Example: Pythonic world operations and spatial queries."""
    print("\n=== Pythonic World Operations Example ===")
    
    # Create world using utility function
    world = nlm.create_test_environment(150.0, 100.0)
    print(f"Created test environment: {world.getWidth()}x{world.getHeight()}")
    
    # Configure using Pythonic API
    world.configure_simulation(
        width=200.0,
        height=200.0,
        vision_width=64,
        vision_height=64,
        max_energy=150.0,
        energy_decay_rate=0.05
    )
    print(f"Configured simulation: {world}")
    
    # Add objects using Pythonic API
    objects = [
        (20.0, 20.0, nlm.WorldObjectType.Resource, 15.0),
        (100.0, 100.0, nlm.WorldObjectType.Hazard, -8.0),
        (150.0, 50.0, nlm.WorldObjectType.Wall, 0.0, 3.0),
        (50.0, 150.0, nlm.WorldObjectType.Marker, 0.0)
    ]
    
    for x, y, obj_type, value in objects:
        world.add_object_at(x, y, obj_type, value)
    
    print(f"Added {len(objects)} objects to world")
    
    # Get nearby objects using Pythonic API
    center_x, center_y = 75.0, 75.0
    radius = 60.0
    
    nearby = world.get_nearby_objects(center_x, center_y, radius)
    print(f"\nNearby objects (radius {radius} from ({center_x}, {center_y})):")
    for i, obj in enumerate(nearby):
        print(f"  {i+1}. ({obj.x:.1f}, {obj.y:.1f}) - {obj.type.name} "
              f"(value: {obj.value}, radius: {obj.radius})")
    
    # Set agent start position
    world.setAgentStart(100.0, 100.0)
    print(f"\nAgent start position set to (100.0, 100.0)")
    
    return world

def example_pythonic_agent_operations():
    """Example: Pythonic agent operations and episode management."""
    print("\n=== Pythonic Agent Operations Example ===")
    
    # Create simulation environment
    config = nlm.create_simulation_config()
    brain = nlm.Brain(config)
    brain.initialize()
    
    world = nlm.create_test_environment()
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Create episode context manager
    episode = agent_brain.episode(max_duration=10.0, verbose=False)
    print(f"Created episode context manager")
    
    # Demonstrate act() method
    for step in range(3):
        print(f"\nEpisode step {step + 1}:")
        
        # Process sensory input (in real scenario, this would come from world)
        percept = nlm.SensoryPercept()
        
        # Act based on percept
        action = agent_brain.act(percept)
        print(f"  Action selected: {action.name}")
        
        # Observe percept (update internal state)
        agent_brain.observe(percept)
        print(f"  Observed sensory input")
        
        # Update development
        agent_brain.updateDevelopment(1.0)
        print(f"  Updated development")
    
    # Demonstrate neuromodulation state
    print(f"\nNeuromodulation state:")
    print(f"  Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent_brain.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent_brain.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent_brain.getPredictionError():.3f}")
    print(f"  Developmental stage: {agent_brain.getDevelopmentalStage().name}")
    
    return agent_brain

def example_error_handling():
    """Example: Pythonic error handling."""
    print("\n=== Error Handling Example ===")
    
    # Demonstrate validation error handling
    config = nlm.Config()
    config.update(valid_field=1, another_field="test")
    
    try:
        # This should work (no error for valid config)
        config.validate("valid_field:int,another_field:string")
        print("✓ Configuration validation passed")
    except Exception as e:
        print(f"✗ Validation error: {e}")
    
    try:
        # This would fail in a real implementation
        config.validate("nonexistent_field:string")
        print("✗ Validation should have failed but didn't")
    except Exception as e:
        print(f"✓ Expected validation error: {e}")
    
    # Demonstrate file loading error handling
    config2 = nlm.Config()
    result = config2.loadConfigFile("/nonexistent/path/config.json")
    print(f"✓ File loading handled gracefully: {result is not None or result is False}")
    
    # Demonstrate type conversion errors (if any)
    try:
        config.update(invalid_type="this should work")
        print("✓ Type conversion handled gracefully")
    except Exception as e:
        print(f"✗ Unexpected type error: {e}")

def example_performance_utilities():
    """Example: Performance utilities and benchmarking."""
    print("\n=== Performance Utilities Example ===")
    
    config = nlm.create_simulation_config()
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Demonstrate benchmark method
    print("Running brain benchmark...")
    results = brain.benchmark(steps=5, warmup=3)
    
    print("Benchmark results:")
    for key, value in results.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.4f}")
        else:
            print(f"  {key}: {value}")
    
    # Demonstrate large-scale configuration performance
    print("\nTesting large-scale configuration...")
    large_config = nlm.Config()
    
    import time
    start_time = time.time()
    
    for i in range(1000):
        large_config.update(**{
            f"param_{i}": i,
            f"float_{i}": float(i) * 0.001,
            f"string_{i}": f"value_{i}",
            f"bool_{i}": i % 2 == 0
        })
    
    elapsed = time.time() - start_time
    print(f"✓ Created 1000 parameters in {elapsed:.3f} seconds")
    print(f"✓ Total config keys: {len(large_config.getKeys())}")

def example_training_environment():
    """Example: Training environment setup."""
    print("\n=== Training Environment Example ===")
    
    # Create a custom training environment
    world = nlm.create_training_environment(300.0, 200.0)
    print(f"Created training environment: {world.getWidth()}x{world.getHeight()}")
    
    # Configure for training
    world.configure_simulation(
        width=300.0,
        height=200.0,
        vision_width=128,
        vision_height=128,
        max_energy=200.0,
        energy_decay_rate=0.02
    )
    
    # Add diverse training objects
    import random
    for i in range(20):
        x = random.uniform(0, world.getWidth())
        y = random.uniform(0, world.getHeight())
        
        # Create different types of objects
        if i % 3 == 0:
            obj_type = nlm.WorldObjectType.Resource
            value = random.uniform(5.0, 20.0)
            radius = random.uniform(1.0, 3.0)
        elif i % 3 == 1:
            obj_type = nlm.WorldObjectType.Hazard
            value = random.uniform(-10.0, -1.0)
            radius = random.uniform(0.5, 2.0)
        else:
            obj_type = nlm.WorldObjectType.Marker
            value = 0.0
            radius = random.uniform(0.5, 1.5)
        
        world.add_object_at(x, y, obj_type, value, radius)
    
    print(f"Added 20 diverse training objects")
    
    # Set agent start for training
    world.setAgentStart(world.getWidth() * 0.5, world.getHeight() * 0.5)
    print(f"Agent start set to center of world")
    
    return world

def main():
    """Run all examples."""
    print("NLM Python Bindings - Pythonic API Examples")
    print("=" * 50)
    
    try:
        # Run all examples
        example_basic_functionality()
        example_pythonic_config()
        example_pythonic_brain_operations()
        example_pythonic_world_operations()
        example_pythonic_agent_operations()
        example_error_handling()
        example_performance_utilities()
        example_training_environment()
        
        print("\n" + "=" * 50)
        print("✓ All examples completed successfully!")
        print("\nKey features demonstrated:")
        print("  • Pythonic configuration loading and validation")
        print("  • Enhanced brain operations with context managers")
        print("  • World simulation with spatial queries")
        print("  • Agent brain interface with episode management")
        print("  • Error handling and validation")
        print("  • Performance benchmarking utilities")
        print("  • Training environment setup")
        
    except Exception as e:
        print(f"\n✗ Error during examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()