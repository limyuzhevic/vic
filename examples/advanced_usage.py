"""
Advanced usage examples for NLM (Neural Learning Machine) Python bindings

This file contains comprehensive examples demonstrating advanced usage patterns,
performance optimization techniques, and real-world application scenarios.
"""

import pynlm
import time
import tempfile
import json

def example_basic_brain_simulation():
    """
    Example 1: Basic brain simulation with step-by-step execution
    """
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial state: {brain.getFiringNeuronCount()} neurons firing")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
        if step % 3 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    print()

def example_complete_agent_episode():
    """
    Example 2: Complete agent episode with world interaction
    """
    print("=== Example 2: Complete Agent Episode ===")
    
    # Setup complete agent
    config = pynlm.createDefaultConfig()
    brain, world, agent = pynlm.createCompleteAgent(config)
    
    print(f"Agent created in {world.getWidth()}x{world.getHeight()} world")
    
    # Run episode
    results = pynlm.runAgentEpisode(brain, world, agent, 50, 0.1)
    
    print(f"Episode completed in {len(results)} steps")
    print(f"Final metrics: {results}")
    print()

def example_configuration_management():
    """
    Example 3: Advanced configuration management
    """
    print("=== Example 3: Configuration Management ===")
    
    # Create experimental config
    config = pynlm.createExperimentalConfig()
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("neuromod.curiosity.enable", True)
    
    # Validate configuration
    errors = pynlm.validate_brain_config(config)
    if errors:
        print(f"Configuration errors: {errors}")
    else:
        print("Configuration is valid")
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created custom brain with {brain.getTotalNeuronCount()} neurons")
    
    # Save configuration to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config.saveToFile(f.name)
        print(f"Configuration saved to: {f.name}")
        
        # Load configuration from file
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(f.name)
        print(f"Loaded configuration: {config2.get('brain.neuron_count')} neurons")
    
    print()

def example_performance_benchmarking():
    """
    Example 4: Performance benchmarking
    """
    print("=== Example 4: Performance Benchmarking ===")
    
    # Benchmark default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    steps_completed = 0
    
    for step in range(100):
        brain.step(step)
        steps_completed += 1
    
    end_time = time.time()
    elapsed_time = end_time - start_time
    steps_per_second = steps_completed / elapsed_time
    
    print(f"Completed {steps_completed} steps in {elapsed_time:.2f} seconds")
    print(f"Performance: {steps_per_second:.1f} steps/second")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    
    # Benchmark with high-performance config
    print("\n--- High-Performance Configuration ---")
    hp_config = pynlm.createHighPerformanceConfig()
    hp_brain = pynlm.createBrain(hp_config)
    hp_brain.initialize()
    
    start_time = time.time()
    for step in range(100):
        hp_brain.step(step)
    end_time = time.time()
    
    hp_elapsed = end_time - start_time
    hp_steps_per_second = 100 / hp_elapsed
    
    print(f"High-performance brain: {hp_steps_per_second:.1f} steps/second")
    print()

def example_system_monitoring():
    """
    Example 5: System monitoring and health checks
    """
    print("=== Example 5: System Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for step in range(20):
        brain.step(step)
    
    # Get health summary
    health_summary = brain.getHealthSummary()
    print("Brain Health Summary:")
    for key, value in health_summary.items():
        print(f"  {key}: {value}")
    
    # Check stability
    is_stable = brain.isStable()
    print(f"Brain is stable: {is_stable}")
    
    # Get metrics
    metrics = brain.getMetrics()
    print("\nDetailed Metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value}")
    
    print()

def example_advanced_agent_control():
    """
    Example 6: Advanced agent control and system management
    """
    print("=== Example 6: Advanced Agent Control ===")
    
    config = pynlm.createDefaultConfig()
    brain, world, agent = pynlm.createCompleteAgent(config)
    
    # Get current system status
    system_status = agent.getSystemStatus()
    print("Initial System Status:")
    for key, value in system_status.items():
        print(f"  {key}: {value}")
    
    # Control systems
    print("\nEnabling all systems...")
    agent.enableAllSystems()
    
    print("Disabling specific systems...")
    agent.enableRewardModulation(False)
    agent.enableCuriosity(False)
    
    # Get updated status
    updated_status = agent.getSystemStatus()
    print("\nUpdated System Status:")
    for key, value in updated_status.items():
        print(f"  {key}: {value}")
    
    # Run agent with controlled systems
    print("\nRunning agent with limited systems...")
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
    
    print(f"Agent completed {step + 1} steps")
    print()

def example_neuron_injection_control():
    """
    Example 7: Precise neuron control and stimulation
    """
    print("=== Example 7: Neuron Injection Control ===")
    
    config = pynlm.createMinimalConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get neuron IDs
    region_ids = brain.getRegionIds()
    if region_ids:
        region = brain.getRegion(region_ids[0])
        # In a real implementation, you would get actual neuron IDs from the region
        print(f"Brain has {region_ids.size()} regions")
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        
        # Demonstrate neuron control (simplified)
        print("Neuron control features available:")
        print("  - Direct current injection to specific neurons")
        print("  - Current injection by neuron type")
        print("  - Targeted stimulation for experiments")
    
    # Run simulation with injection
    for step in range(15):
        brain.step(step)
        
        # Example: Inject current into excitatory neurons at specific times
        if step % 5 == 0:
            print(f"Step {step}: Brain firing rate {brain.getAverageFiringRate():.3f}")
    
    print()

def example_configuration_comparison():
    """
    Example 8: Configuration comparison and optimization
    """
    print("=== Example 8: Configuration Comparison ===")
    
    configs = {
        "Default": pynlm.createDefaultConfig(),
        "Minimal": pynlm.createMinimalConfig(),
        "High-Performance": pynlm.createHighPerformanceConfig(),
    }
    
    print("Configuration Comparison:")
    print("Name" + " " * 40 + "Neurons" + " " * 15 + "Plasticity" + " " * 15 + "Notes")
    print("-" * 90)
    
    for name, config in configs.items():
        neuron_count = config.getInt("brain.neuron_count")
        stdp_enabled = config.getBool("plasticity.stdp.enable")
        hebbian_enabled = config.getBool("plasticity.hebbian.enable")
        struct_enabled = config.getBool("plasticity.structural.enable")
        
        plasticity_text = f"STDP:{stdp_enabled}, Hebb:{hebbian_enabled}, Struct:{struct_enabled}"
        
        notes = ""
        if name == "High-Performance":
            notes = "Optimized for speed"
        elif name == "Minimal":
            notes = "Small scale testing"
        else:
            notes = "Balanced approach"
        
        print(f"{name:<10} {neuron_count:<15} {plasticity_text:<15} {notes}")
    
    print()

def example_real_world_scenario():
    """
    Example 9: Real-world scenario - Agent learning in environment
    """
    print("=== Example 9: Real-World Learning Scenario ===")
    
    # Setup learning environment
    config = pynlm.createExperimentalConfig()
    brain, world, agent = pynlm.createCompleteAgent(config)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Learning environment setup:")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    
    # Simulate learning episode
    print("\nRunning learning episode...")
    learning_metrics = {
        "steps_completed": 0,
        "total_spikes": 0,
        "final_curiosity": 0.0,
        "final_novelty": 0.0,
        "development_stage": 0,
    }
    
    for step in range(100):
        # Environment interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Agent processing
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Action and learning
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward-based learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted = 0.0
        agent.applyRewardModulation(reward, predicted)
        
        # Development updates
        agent.updateDevelopment(0.1)
        
        # Track metrics
        learning_metrics["steps_completed"] = step + 1
        learning_metrics["total_spikes"] = brain.getTotalSpikeCount()
        learning_metrics["final_curiosity"] = agent.getCuriosityLevel()
        learning_metrics["final_novelty"] = agent.getNoveltyLevel()
        learning_metrics["development_stage"] = agent.getDevelopmentalStage()
        
        if step % 25 == 0:
            print(f"  Step {step:3d}: Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Spikes={brain.getTotalSpikeCount()}")
    
    print("\nLearning Episode Summary:")
    for key, value in learning_metrics.items():
        print(f"  {key}: {value}")
    
    print()

def example_batch_processing():
    """
    Example 10: Batch processing for parameter exploration
    """
    print("=== Example 10: Batch Processing ===")
    
    # Parameter space exploration
    neuron_counts = [100, 500, 1000, 2000]
    plasticity_settings = [True, False]
    
    results = []
    
    for neurons in neuron_counts:
        for plasticity in plasticity_settings:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neurons)
            config.set("plasticity.stdp.enable", plasticity)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run batch simulation
            for step in range(20):
                brain.step(step)
            
            result = {
                "neurons": neurons,
                "plasticity": plasticity,
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
            }
            
            results.append(result)
            print(f"Config: {neurons} neurons, Plasticity={plasticity} -> "
                  f"Rate={result['firing_rate']:.3f}, Spikes={result['total_spikes']}")
    
    # Summary
    print(f"\nBatch processing completed {len(results)} configurations")
    
    # Find optimal configuration
    best_config = max(results, key=lambda x: x['firing_rate'])
    print(f"Best firing rate: {best_config['firing_rate']:.3f} "
          f"(with {best_config['neurons']} neurons, "
          f"plasticity={best_config['plasticity']})")
    
    print()

def main():
    """
    Run all examples
    """
    print("NLM Python Bindings - Advanced Usage Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_basic_brain_simulation,
        example_complete_agent_episode,
        example_configuration_management,
        example_performance_benchmarking,
        example_system_monitoring,
        example_advanced_agent_control,
        example_neuron_injection_control,
        example_configuration_comparison,
        example_real_world_scenario,
        example_batch_processing,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            print()
    
    print("All examples completed!")
    print("\nNext Steps:")
    print("  1. Read the documentation for deeper understanding")
    print("  2. Experiment with different configurations")
    print("  3. Explore the C++ API for advanced features")
    print("  4. Contribute to the project!")

if __name__ == "__main__":
    main()
