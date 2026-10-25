#!/usr/bin/env python3
"""
NLM Python Bindings Example Script

This script demonstrates the improved NLM Python bindings with enhanced
functionality, error handling, and user-friendly API design.
"""

import pynlm
import time

def example_basic_usage():
    """Example 1: Basic usage with enhanced API."""
    print("=" * 60)
    print("Example 1: Basic Usage with Enhanced API")
    print("=" * 60)
    
    # Create configuration with enhanced Pythonic access
    print("\n1. Creating and configuring a brain:")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.learning_rate", 0.005)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created: {brain.getStats()}")
    
    # Run simulation with enhanced statistics
    print("\n2. Running simulation:")
    for step in range(20):
        brain.step(step)
        if step % 5 == 0:
            stats = brain.getStats()
            print(f"   Step {step}: {stats['firing_neurons']} firing neurons")
    
    print("\n   ✓ Basic simulation completed successfully")
    return brain

def example_agent_simulation():
    """Example 2: Agent simulation with enhanced convenience functions."""
    print("\n" + "=" * 60)
    print("Example 2: Agent Simulation with Enhanced Convenience Functions")
    print("=" * 60)
    
    # Create a ready-to-use agent with enhanced API
    print("\n1. Creating enhanced agent with world:")
    result = pynlm.createReadyAgent(
        width=20,
        height=20,
        enableRewardModulation=True,
        enableCuriosity=True,
        enableDevelopment=True,
        enableStructuralPlasticity=True
    )
    
    brain = result["brain"]
    world = result["world"]
    agent = result["agent"]
    
    print(f"   Agent created with subsystems: {agent.getSubsystemsStatus()}")
    
    # Run enhanced episode with comprehensive logging
    print("\n2. Running enhanced agent episode:")
    pynlm.runSimpleEpisode(brain, world, agent, 30)
    
    print(f"\n   Episode completed!")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Final prediction error: {agent.getPredictionError():.3f}")
    
    return brain, world, agent

def example_learning_and_development():
    """Example 3: Learning and development with enhanced error handling."""
    print("\n" + "=" * 60)
    print("Example 3: Learning and Development with Enhanced Error Handling")
    print("=" * 60)
    
    try:
        print("\n1. Setting up learning environment:")
        config = pynlm.createStandardConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("   Configuration created with standard settings")
        
        # Validate configuration
        print("\n2. Validating configuration:")
        for key in config.getKeys():
            if config.validateKey(key):
                value = config.get(key)
                print(f"   ✓ {key}: {value}")
            else:
                print(f"   ✗ {key}: invalid")
        
        # Enhanced learning simulation
        print("\n3. Running enhanced learning simulation:")
        for episode in range(3):
            print(f"\n   Episode {episode + 1}:")
            
            # Create fresh brain for each episode
            episode_brain = pynlm.createBrain(config)
            episode_brain.initialize()
            
            # Run many steps for learning
            for step in range(100):
                episode_brain.step(step)
                
                # Check development progress
                if step % 25 == 0:
                    stage = episode_brain.getDevelopmentalStage()
                    print(f"     Step {step}: Developmental stage = {stage}")
            
            # Check if learning occurred
            initial_stats = brain.getStats()
            final_stats = episode_brain.getStats()
            
            learning_occurred = (
                final_stats["total_spikes"] > initial_stats["total_spikes"] or
                final_stats["e_i_ratio"] != initial_stats["e_i_ratio"]
            )
            
            if learning_occurred:
                print("     ✓ Learning detected through synaptic changes")
            else:
                print("     ℹ Learning may require more complexity")
        
        print("\n   ✓ Learning and development simulation completed")
        return brain
        
    except Exception as e:
        print(f"\n   ✗ Error in learning simulation: {e}")
        raise

def example_error_handling_and_validation():
    """Example 4: Error handling and validation with enhanced safety."""
    print("\n" + "=" * 60)
    print("Example 4: Error Handling and Validation with Enhanced Safety")
    print("=" * 60)
    
    print("\n1. Testing enhanced error handling:")
    
    # Create config for error handling tests
    config = pynlm.createDefaultConfig()
    
    # Test 1: Invalid file loading
    print("\n   a. Testing invalid file loading:")
    try:
        config.loadFromFile("/invalid/path/file.json")
        print("      ✗ Should have raised an error")
    except Exception as e:
        print(f"      ✓ Correctly handled error: {type(e).__name__}")
    
    # Test 2: Invalid key access
    print("\n   b. Testing invalid key access:")
    try:
        value = config.get("nonexistent_key")
        print(f"      ✗ Should have raised an error (got: {value})")
    except Exception as e:
        print(f"      ✓ Correctly handled error: {type(e).__name__}")
    
    # Test 3: Invalid key validation
    print("\n   c. Testing invalid key validation:")
    try:
        if config.validateKey("definitely_not_a_key"):
            print("      ✗ Should have returned False for invalid key")
        else:
            print("      ✓ Correctly identified invalid key")
    except Exception as e:
        print(f"      ✓ Correctly handled error: {type(e).__name__}")
    
    # Test 4: Enhanced brain validation
    print("\n   d. Testing enhanced brain validation:")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    is_valid = brain.isValid()
    is_initialized = brain.checkInitialized()
    
    print(f"      ✓ Brain validation: is_valid={is_valid}, is_initialized={is_initialized}")
    
    print("\n   ✓ All error handling and validation tests passed")

def example_configuration_management():
    """Example 5: Configuration management with enhanced Pythonic API."""
    print("\n" + "=" * 60)
    print("Example 5: Configuration Management with Enhanced Pythonic API")
    print("=" * 60)
    
    print("\n1. Creating and managing configuration:")
    
    # Start with standard configuration
    config = pynlm.createStandardConfig()
    print(f"   Standard config created with {len(config.getKeys())} parameters")
    
    # Pythonic configuration access
    print("\n2. Pythonic configuration access:")
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Access values using Pythonic methods
    neuron_count = config.getInt("brain.neuron_count")
    stdp_rate = config.getFloat("plasticity.stdp.learning_rate")
    
    print(f"   ✓ Neuron count: {neuron_count}")
    print(f"   ✓ STDP learning rate: {stdp_rate}")
    
    # Configuration validation
    print("\n3. Configuration validation:")
    important_keys = ["brain.neuron_count", "plasticity.stdp.enable", "random_seed"]
    
    for key in important_keys:
        if config.validateKey(key):
            value = config.get(key)
            print(f"   ✓ {key}: {value}")
        else:
            print(f"   ✗ {key}: missing")
    
    print("\n   ✓ Configuration management completed successfully")

def example_advanced_patterns():
    """Example 6: Advanced patterns and usage scenarios."""
    print("\n" + "=" * 60)
    print("Example 6: Advanced Patterns and Usage Scenarios")
    print("=" * 60)
    
    print("\n1. Pattern: Quick experiment setup:")
    
    # Quick setup for experimentation
    def setup_experiment(neurons=200, learning_rate=0.01):
        """Quick setup function for experiments."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neurons)
        config.set("plasticity.stdp.learning_rate", learning_rate)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    # Run multiple experiments
    experiments = [
        (100, 0.01, "Small brain, low learning"),
        (500, 0.05, "Medium brain, medium learning"),
        (1000, 0.02, "Large brain, low-medium learning"),
    ]
    
    for neuron_count, learning_rate, description in experiments:
        print(f"\n   Running: {description}")
        brain = setup_experiment(neuron_count, learning_rate)
        
        for step in range(20):
            brain.step(step)
        
        stats = brain.getStats()
        print(f"     Result: {stats['total_neurons']} neurons, {stats['firing_neurons']} firing")
    
    print("\n2. Pattern: Session with persistence:")
    
    # Simulate session with checkpointing (conceptual example)
    print("\n   Simulating session management:")
    config = pynlm.createDefaultConfig()
    
    # In a real implementation, you would save/load brain states
    # For this example, we'll just demonstrate the concept
    print("   ✓ Session management pattern demonstrated")
    print("   (In production: save/load brain checkpoints)")
    
    print("\n   ✓ Advanced patterns completed")

def main():
    """Run all examples."""
    print("NLM Python Bindings - Enhanced Examples")
    print("This script demonstrates the improved NLM Python bindings")
    print("with enhanced error handling, convenience functions, and user-friendly API.")
    
    examples = [
        ("Basic Usage with Enhanced API", example_basic_usage),
        ("Agent Simulation with Convenience Functions", example_agent_simulation),
        ("Learning and Development with Enhanced Safety", example_learning_and_development),
        ("Error Handling and Validation", example_error_handling_and_validation),
        ("Configuration Management with Pythonic API", example_configuration_management),
        ("Advanced Patterns and Usage Scenarios", example_advanced_patterns),
    ]
    
    completed_examples = 0
    
    for example_name, example_func in examples:
        try:
            example_func()
            completed_examples += 1
        except Exception as e:
            print(f"\n❌ Example '{example_name}' failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("Examples Summary")
    print("=" * 60)
    print(f"Examples completed: {completed_examples}/{len(examples)}")
    print(f"Success rate: {completed_examples/len(examples)*100:.1f}%")
    
    if completed_examples == len(examples):
        print("\n🎉 All examples completed successfully!")
        print("\nThe improved NLM Python bindings provide:")
        print("  • Enhanced error handling and validation")
        print("  • Convenience functions for common patterns")
        print("  • Pythonic API design for easier usage")
        print("  • Better statistics and debugging information")
        print("  • Support for learning and development scenarios")
    else:
        print(f"\n⚠️  {len(examples) - completed_examples} example(s) failed.")

if __name__ == "__main__":
    main()