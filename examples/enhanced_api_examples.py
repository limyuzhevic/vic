#!/usr/bin/env python3
"""
Comprehensive example demonstrating the enhanced NLM Python API.

This script showcases all the new Pythonic features and improvements
to the NLM Python bindings, from basic usage to advanced patterns.
"""

import time
from typing import Dict, Any

def print_header(title: str):
    """Print a formatted header."""
    print(f"\n{'=' * 60}")
    print(f"{title}")
    print(f"{'=' * 60}")

def example_1_basic_usage():
    """Example 1: Basic usage with enhanced configuration."""
    print_header("Example 1: Basic Usage with Enhanced Configuration")
    
    try:
        import pynlm
        from pynlm.core import create_config_from_dict, ConfigWrapper
        
        # Create configuration using enhanced API
        config_dict = {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "world.width": 100.0,
            "world.height": 100.0,
            "brain.v_thresh": -50.0,
            "brain.v_rest": -70.0,
        }
        
        config = create_config_from_dict(config_dict)
        
        print(f"✓ Created configuration: {config}")
        print(f"✓ Configuration has {len(config.keys())} settings")
        
        # Demonstrate dictionary-style access
        print("\nConfiguration values:")
        for key in ["brain.neuron_count", "brain.synapse_density", "world.width"]:
            if key in config:
                print(f"  {key}: {config[key]}")
        
        # Demonstrate validation
        config.validate_required_keys(["brain.neuron_count", "world.width"])
        print("✓ Configuration validation passed")
        
        # Demonstrate type-safe access
        neuron_count = config.get_int("brain.neuron_count")
        print(f"✓ Retrieved neuron count: {neuron_count} (type: {type(neuron_count)})")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 1 failed: {e}")
        return False

def example_2_enhanced_brain():
    """Example 2: Enhanced brain with context manager and better API."""
    print_header("Example 2: Enhanced Brain with Context Manager")
    
    try:
        from pynlm.core import create_brain_with_config, brain_context, BrainWrapper
        
        # Create brain using enhanced factory
        brain_config = {
            "brain.neuron_count": 500,
            "brain.synapse_density": 0.05,
            "development.enabled": True,
        }
        
        brain_wrapper = create_brain_with_config(brain_config)
        
        print(f"✓ Created brain wrapper: {brain_wrapper}")
        
        # Show brain statistics
        stats = brain_wrapper.get_stats()
        print(f"\nBrain statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
        
        # Demonstrate context manager (conceptual)
        print("\nNote: Brain supports context manager for resource management")
        print("  with brain_context(brain_wrapper, auto_initialize=True) as brain:")
        
        # Demonstrate step execution
        for step in range(5):
            brain_wrapper.step(step)
            print(f"  ✓ Step {step} completed")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 2 failed: {e}")
        return False

def example_3_enhanced_world():
    """Example 3: Enhanced world with convenience methods."""
    print_header("Example 3: Enhanced World with Convenience Methods")
    
    try:
        from pynlm.core import NLMFactory
        
        # Create world using factory
        world_wrapper = NLMFactory.create_world_with_agents(
            width=200,
            height=200,
            vision_width=30,
            vision_height=30,
            agent_start=(100.0, 100.0),
            random_seed=42
        )
        
        print(f"✓ Created world wrapper: {world_wrapper.get_width()}x{world_wrapper.get_height()}")
        
        # Show world statistics
        stats = world_wrapper.get_stats()
        print(f"\nWorld statistics:")
        for key, value in stats.items():
            if isinstance(value, tuple):
                print(f"  {key}: ({value[0]:.1f}, {value[1]:.1f})")
            else:
                print(f"  {key}: {value}")
        
        # Demonstrate convenience methods
        print(f"\nConvenience methods:")
        
        # Distance calculation
        distance = world_wrapper.distance_to_target(150.0, 150.0)
        print(f"  Distance to target (150,150): {distance:.2f}")
        
        # Move agent
        moved = world_wrapper.move_agent(10.0, 5.0)
        print(f"  Agent moved: {moved}")
        
        # Check if agent is moving
        is_moving = world_wrapper.is_agent_moving()
        print(f"  Agent is moving: {is_moving}")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 3 failed: {e}")
        return False

def example_4_sensor_helper():
    """Example 4: Sensor helper and data processing."""
    print_header("Example 4: Sensor Helper and Data Processing")
    
    try:
        from pynlm.core import SensorHelper
        
        helper = SensorHelper()
        
        print("✓ Created SensorHelper instance")
        
        # Show available helper methods
        print(f"\nAvailable helper methods:")
        methods = [
            'normalize_vision', 'normalize_audio', 'normalize_internal_signals',
            'normalize_sensory_percept', 'convert_to_numpy', 'get_sensory_stats',
            'extract_features', 'detect_edges', 'apply_filter'
        ]
        
        for method in methods:
            if hasattr(helper, method):
                print(f"  ✓ {method}")
        
        # Demonstrate helper usage (conceptual)
        print("\nNote: SensorHelper provides:")
        print("  - Data normalization for different sensory modalities")
        print("  - Conversion to numpy arrays for analysis")
        print("  - Feature extraction from sensory data")
        print("  - Edge detection and filtering")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 4 failed: {e}")
        return False

def example_5_factory_methods():
    """Example 5: Factory methods for common use cases."""
    print_header("Example 5: Factory Methods for Common Use Cases")
    
    try:
        from pynlm.core import NLMFactory
        
        print("Available factory methods:")
        
        # Brain factories
        print("\n  Brain factories:")
        brain1 = NLMFactory.create_brain_with_validation(1000, 0.1)
        print("    ✓ create_brain_with_validation(1000, 0.1)")
        
        brain2 = NLMFactory.create_brain_from_config({"brain.neuron_count": 500})
        print("    ✓ create_brain_from_config({'brain.neuron_count': 500})")
        
        # World factories
        print("\n  World factories:")
        world1 = NLMFactory.create_world_with_agents(100, 100, 20, 20, (50, 50))
        print("    ✓ create_world_with_agents(100, 100, 20, 20, (50, 50))")
        
        world2 = NLMFactory.create_world_from_config({"width": 200, "height": 200})
        print("    ✓ create_world_from_config({'width': 200, 'height': 200})")
        
        # Simulation factories
        print("\n  Simulation factories:")
        brain_wrapper, world_wrapper = NLMFactory.create_agent_simulation()
        print("    ✓ create_agent_simulation()")
        
        # Experiment factories
        print("\n  Experiment factories:")
        experiment_config = NLMFactory.create_experiment_config(
            "brain_development_test",
            brain_config={"brain.neuron_count": 1000},
            world_config={"width": 100, "height": 100},
            sensor_types=["vision", "internal"]
        )
        print(f"    ✓ create_experiment_config: {experiment_config['experiment_name']}")
        
        # Training factories
        print("\n  Training factories:")
        training_config = NLMFactory.create_training_config(
            learning_rate=0.001,
            batch_size=32,
            epochs=10,
            validation_split=0.2
        )
        print(f"    ✓ create_training_config with {training_config['epochs']} epochs")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 5 failed: {e}")
        return False

def example_6_error_handling():
    """Example 6: Enhanced error handling with custom exceptions."""
    print_header("Example 6: Enhanced Error Handling")
    
    try:
        from pynlm.core import (
            NLMError, ConfigError, BrainError, WorldError,
            SensorError, AgentError, ValidationError
        )
        
        print("✓ Imported custom exception classes")
        
        # Demonstrate exception hierarchy
        print("\nException hierarchy:")
        print("  NLMError (base class)")
        print("    ├── ConfigError")
        print("    ├── BrainError")
        print("    ├── WorldError")
        print("    ├── SensorError")
        print("    ├── AgentError")
        print("    ├── ValidationError")
        print("    ├── MemoryError")
        print("    ├── SimulationError")
        print("    └── DevelopmentalError")
        
        # Demonstrate exception usage
        print("\nException usage examples:")
        
        try:
            raise ConfigError("Invalid configuration key")
        except ConfigError as e:
            print(f"  ✓ Caught ConfigError: {e}")
        
        try:
            raise ValidationError("Validation failed")
        except NLMError as e:  # Catch base class
            print(f"  ✓ Caught NLMError (base class): {e}")
        
        try:
            raise BrainError("Brain initialization failed")
        except (ConfigError, BrainError) as e:  # Multiple exception types
            print(f"  ✓ Caught specific exception type: {e}")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 6 failed: {e}")
        return False

def example_7_comprehensive_simulation():
    """Example 7: Complete simulation with all enhancements."""
    print_header("Example 7: Complete Simulation with All Enhancements")
    
    try:
        from pynlm.core import NLMFactory, run_full_episode
        
        print("Creating complete simulation...")
        
        # Create brain and world using factory
        brain_wrapper, world_wrapper = NLMFactory.create_agent_simulation(
            brain_config={
                "brain.neuron_count": 800,
                "brain.synapse_density": 0.08,
                "development.enabled": True,
                "neuromodulation.reward.enable": True,
                "neuromodulation.curiosity.enable": True,
            },
            world_config={
                "width": 150,
                "height": 150,
                "vision_width": 25,
                "vision_height": 25,
            }
        )
        
        print(f"✓ Created brain: {brain_wrapper.get_stats()['neuron_count']} neurons")
        print(f"✓ Created world: {world_wrapper.get_stats()['width']}x{world_wrapper.get_stats()['height']}")
        
        # Run simulation episode
        print("\nRunning simulation episode...")
        results = run_full_episode(brain_wrapper, world_wrapper, steps=20, timestep=0.1)
        
        print(f"✓ Simulation completed: {results['steps_completed']} steps")
        print(f"✓ Final brain stats: {results['final_brain_stats']['neuron_count']} neurons")
        print(f"✓ Final world stats: {results['final_world_stats']['width']}x{results['final_world_stats']['height']}")
        
        # Demonstrate additional functionality
        print("\nAdditional functionality available:")
        print("  • Advanced error handling with specific exception types")
        print("  • Context managers for resource management")
        print("  • Sensor data processing helpers")
        print("  • Factory methods for common scenarios")
        print("  • Configuration validation and type safety")
        print("  • Statistics and monitoring tools")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 7 failed: {e}")
        return False

def example_8_backward_compatibility():
    """Example 8: Backward compatibility with original API."""
    print_header("Example 8: Backward Compatibility")
    
    try:
        # Test that original pynlm API still works
        import pynlm
        
        print("✓ Original pynlm module still available")
        
        # Test original factory functions
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        print("✓ Original factory functions still work:")
        print(f"  - pynlm.createDefaultConfig(): {type(config).__name__}")
        print(f"  - pynlm.createBrain(): {type(brain).__name__}")
        print(f"  - pynlm.createSimpleWorld(): {type(world).__name__}")
        print(f"  - pynlm.createAgentBrain(): {type(agent).__name__}")
        
        # Show that new enhanced API is available
        print("\nEnhanced API also available:")
        from pynlm.core import (
            create_brain_with_config, create_config_from_dict,
            BrainWrapper, WorldWrapper, ConfigWrapper, SensorHelper
        )
        
        print("  ✓ pynlm.core.BrainWrapper")
        print("  ✓ pynlm.core.WorldWrapper")
        print("  ✓ pynlm.core.ConfigWrapper")
        print("  ✓ pynlm.core.SensorHelper")
        
        return True
        
    except Exception as e:
        print(f"✗ Example 8 failed: {e}")
        return False

def main():
    """Run all examples."""
    print("NLM Enhanced Python API Examples")
    print("=" * 60)
    
    examples = [
        ("Basic Usage with Enhanced Configuration", example_1_basic_usage),
        ("Enhanced Brain with Context Manager", example_2_enhanced_brain),
        ("Enhanced World with Convenience Methods", example_3_enhanced_world),
        ("Sensor Helper and Data Processing", example_4_sensor_helper),
        ("Factory Methods for Common Use Cases", example_5_factory_methods),
        ("Enhanced Error Handling", example_6_error_handling),
        ("Complete Simulation with All Enhancements", example_7_comprehensive_simulation),
        ("Backward Compatibility", example_8_backward_compatibility),
    ]
    
    passed = 0
    failed = 0
    
    for title, example in examples:
        try:
            if example():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ {title} failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Summary: {passed} examples passed, {failed} examples failed")
    print("=" * 60)
    
    if failed == 0:
        print("\n🎉 All examples completed successfully!")
        print("\nThe enhanced NLM Python package provides:")
        print("  • Pythonic API with context managers and type safety")
        print("  • Custom exceptions for better error handling")
        print("  • Factory methods for common use cases")
        print("  • Sensor data processing helpers")
        print("  • Configuration validation and convenience methods")
        print("  • Full backward compatibility with original API")
        print("  • Comprehensive documentation and examples")
    else:
        print(f"\n⚠️  {failed} example(s) failed. Please check the output above.")
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    import sys
    sys.exit(main())
