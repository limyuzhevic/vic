"""
Convenience Function Examples

This file demonstrates the new convenience functions and enhanced features
available in NLM Python bindings version 0.1.0.
"""

import pynlm
import tempfile
import os

def example_1_quick_brain():
    """Example 1: Using quick_brain convenience function."""
    print("Example 1: Quick Brain Creation")
    print("-" * 35)
    
    # Quick brain creation with preset config
    brain = pynlm.quick_brain(150, "configs/default.cfg")
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain initialized with {brain.getTotalSynapseCount()} synapses")
    print()

def example_2_preset_brains():
    """Example 2: Using preset brain configurations."""
    print("Example 2: Preset Brain Configurations")
    print("-" * 45)
    
    # Create different brain types with presets
    simple_brain = pynlm.create_brain_with_preset("simple", 100)
    complex_brain = pynlm.create_brain_with_preset("complex", 500)
    optimized_brain = pynlm.create_brain_with_preset("optimized", 200)
    
    brains = [simple_brain, complex_brain, optimized_brain]
    names = ["Simple", "Complex", "Optimized"]
    
    for brain, name in zip(brains, names):
        brain.initialize()
        for i in range(10):
            brain.step(i)
        print(f"{name} brain: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSpikeCount()} spikes, "
              f"{brain.getAverageFiringRate():.2f} avg firing rate")
    
    print()

def example_3_agent_presets():
    """Example 3: Using agent brain presets."""
    print("Example 3: Agent Brain Presets")
    print("-" * 30)
    
    # Setup base brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Create agent with different presets
    explorer_agent = pynlm.create_agent_with_preset("explorer", brain)
    explorer_agent.initialize(world)
    
    learner_agent = pynlm.create_agent_with_preset("learner", brain)
    learner_agent.initialize(world)
    
    methodical_agent = pynlm.create_agent_with_preset("methodical", brain)
    methodical_agent.initialize(world)
    
    agents = [explorer_agent, learner_agent, methodical_agent]
    names = ["Explorer", "Learner", "Methodical"]
    
    for agent, name in zip(agents, names):
        # Enable appropriate features based on preset
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"{name} agent - Curiosity: {agent.getCuriosityLevel():.2f}, "
              f"Learning: {agent.isRewardModulationEnabled()}, "
              f"Development: {agent.isDevelopmentEnabled()}")
    
    print()

def example_4_agent_simulation():
    """Example 4: Complete agent simulation with statistics."""
    print("Example 4: Agent Simulation with Statistics")
    print("-" * 45)
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(25, 25, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running complete agent simulation with statistics collection...")
    
    # Run simulation
    stats, final_stats = pynlm.run_agent_simulation(agent, world, 100, enable_learning=True)
    
    print(f"Simulation completed with {final_stats['total_steps']} steps")
    print(f"Final spike count: {final_stats['final_spike_count']}")
    print(f"Final firing neurons: {final_stats['final_firing_neurons']}")
    print(f"Average firing rate: {final_stats['average_firing_rate']:.2f}")
    print(f"Sample statistics collected: {len(stats)} time points")
    
    if stats:
        print("Sample statistics (first 5 time points):")
        for i in range(min(5, len(stats))):
            stat = stats[i]
            print(f"  Step {stat['step']}: {stat['neurons_firing']} firing, "
                  f"{stat['total_spikes']} total spikes, "
                  f"rate {stat['average_firing_rate']:.2f}")
    
    print()

def example_5_configuration_management():
    """Example 5: Enhanced configuration management."""
    print("Example 5: Configuration Management")
    print("-" * 35)
    
    # Create and work with configurations
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 200)
    config1.set("learning.rate", 0.01)
    
    # Validate configuration
    print("Validating configuration...")
    try:
        if pynlm.validate_config(config1):
            print("✓ Configuration is valid!")
    except pynlm.ConfigurationError as e:
        print(f"✗ Configuration error: {e}")
    
    # Save configuration safely
    print("Saving configuration to file...")
    try:
        pynlm.save_config_to_file(config1, "my_config.json")
        print("✓ Configuration saved successfully!")
        
        # Load configuration
        config2 = pynlm.load_config_from_file("my_config.json")
        print("✓ Configuration loaded successfully!")
        
        # Compare configurations
        differences = config1.compare(config2)
        if differences:
            print(f"⚠ Found {len(differences)} differences between configs")
        else:
            print("✓ Configurations are identical")
            
    except pynlm.ConfigurationError as e:
        print(f"✗ File I/O error: {e}")
    
    # Clean up
    if os.path.exists("my_config.json"):
        os.unlink("my_config.json")
        print("✓ Cleaned up temporary file")
    
    print()

def example_6_quick_experiments():
    """Example 6: Quick experiment functions."""
    print("Example 6: Quick Experiment Functions")
    print("-" * 40)
    
    # Run different types of experiments
    experiments = [
        ("memory", 100),
        ("learning", 150),
        ("plasticity", 200)
    ]
    
    for exp_type, steps in experiments:
        print(f"Running {exp_type} experiment ({steps} steps)...")
        results = pynlm.quick_experiment(exp_type, steps)
        
        print(f"  Type: {results['experiment_type']}")
        print(f"  Total spikes: {results['total_spikes']}")
        print(f"  Total steps: {results['total_steps']}")
        print(f"  Average spikes per step: {results['total_spikes'] / steps:.2f}")
        print()
    
    print()

def example_7_error_handling():
    """Example 7: Comprehensive error handling."""
    print("Example 7: Error Handling Best Practices")
    print("-" * 45)
    
    # Demonstrate different types of error handling
    
    # 1. Configuration validation
    print("1. Configuration validation:")
    try:
        config = pynlm.createDefaultConfig()
        # This will fail validation because neuron_count < 10
        config.set("neuron_count", 5)
        pynlm.validate_config(config)
        print("   ✓ Unexpected: Validation passed (should have failed)")
    except pynlm.ConfigurationError as e:
        print(f"   ✓ Caught configuration error: {str(e)[:50]}...")
    
    # 2. Agent operations
    print("2. Agent operations:")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # This should work fine
        print("   ✓ Agent operations completed successfully")
        
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # 3. File operations
    print("3. File operations:")
    try:
        # Create a temporary config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"neuron_count": 100}')
            temp_file = f.name
        
        config = pynlm.load_config_from_file(temp_file)
        print("   ✓ File loading successful")
        
        # Clean up
        os.unlink(temp_file)
        print("   ✓ Cleaned up temporary file")
        
    except pynlm.ConfigurationError as e:
        print(f"   ✗ Configuration error: {e}")
    except Exception as e:
        print(f"   ✗ File error: {e}")
    
    print()

if __name__ == "__main__":
    print("Convenience Function Examples")
    print("=" * 60)
    print("Demonstrating new convenience functions in NLM 0.1.0")
    print()
    
    example_1_quick_brain()
    example_2_preset_brains()
    example_3_agent_presets()
    example_4_agent_simulation()
    example_5_configuration_management()
    example_6_quick_experiments()
    example_7_error_handling()
    
    print("All convenience examples completed successfully!")
    print()
    print("Key features demonstrated:")
    print("• quick_brain() - simplified brain creation")
    print("• create_brain_with_preset() - predefined brain configurations")
    print("• create_agent_with_preset() - agent behavior presets")
    print("• run_agent_simulation() - complete simulations with statistics")
    print("• save_config_to_file()/load_config_from_file() - safe file I/O")
    print("• validate_config() - configuration validation")
    print("• quick_experiment() - predefined experiment types")
    print("• Comprehensive error handling with custom exceptions")
