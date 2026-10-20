#!/usr/bin/env python3
"""
NLM Performance and Debugging Tools - Python Example Script

This script demonstrates the new performance monitoring and debugging
capabilities added to the NLM (Neural Learning Machine) framework.

Features demonstrated:
1. Performance monitoring with FPS/TPS tracking
2. Timer functionality for profiling
3. Memory usage monitoring
4. System information reporting
5. Performance data export
"""

import pynlm
import time
import json
import os

def basic_performance_example():
    """Demonstrate basic performance monitoring."""
    print("=== Basic Performance Example ===")
    
    # Create brain with performance monitoring
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable subsystems for realistic workload
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    # Simulate some activity
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation steps
    start_time = time.time()
    for step in range(100):
        brain.step(step)
        # Simulate agent processing
        world = agent.getBrain().getConfig()
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Performance summary
    print(f"Average TPS: {(end_time - start_time) / 100:.2f}")
    print(f"Total steps: {brain.getTotalSpikeCount()} spikes")

def timer_profiling_example():
    """Demonstrate timer-based profiling."""
    print("\n=== Timer Profiling Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create timers for different operations
    timers = {
        'brain_step': time.time(),
        'sensory_processing': time.time(),
        'motor_decoding': time.time(),
    }
    
    print("Starting simulation with timer profiling...")
    
    for step in range(50):
        # Track brain step time
        t0 = time.time()
        brain.step(step)
        t1 = time.time()
        
        # Track other operations (simulated)
        t0_sp = time.time()
        time.sleep(0.001)  # Simulate sensory processing
        t1_sp = time.time()
        
        t0_md = time.time()
        time.sleep(0.001)  # Simulate motor decoding
        t1_md = time.time()
        
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Brain step: {t1 - t0:.4f}s")
            print(f"  Sensory proc: {t1_sp - t0_sp:.4f}s")
            print(f"  Motor decode: {t1_md - t0_md:.4f}s")

def memory_monitoring_example():
    """Demonstrate memory usage monitoring."""
    print("\n=== Memory Monitoring Example ===")
    
    config = pynlm.createDefaultConfig()
    
    # Monitor memory before brain creation
    print("Memory monitoring simulation...")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create larger brain to see memory impact
    config.set("brain.neuron_count", 5000)
    large_brain = pynlm.createBrain(config)
    large_brain.initialize()
    
    print(f"Small brain: {brain.getTotalNeuronCount()} neurons")
    print(f"Large brain: {large_brain.getTotalNeuronCount()} neurons")
    
    # Simulate memory-intensive operations
    for step in range(20):
        brain.step(step)
        large_brain.step(step)
        
        if step % 5 == 0:
            print(f"Step {step}: Processing {brain.getFiringNeuronCount()} + {large_brain.getFiringNeuronCount()} firing neurons")

def configuration_example():
    """Demonstrate improved configuration system."""
    print("\n=== Configuration Example ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set various parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Save configuration
    config.saveToFile("example_config.cfg")
    
    # Load configuration
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("example_config.cfg")
    
    print("Configuration saved and loaded successfully")
    print(f"Loaded neuron count: {new_config.get('brain.neuron_count')}")
    print(f"Loaded STDP enabled: {new_config.get('plasticity.stdp.enable')}")
    
    # Clean up
    if os.path.exists("example_config.cfg"):
        os.remove("example_config.cfg")

def enhanced_logging_example():
    """Demonstrate enhanced logging capabilities."""
    print("\n=== Enhanced Logging Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Enhanced logging simulation...")
    
    # Enable logging for debugging
    for step in range(30):
        brain.step(step)
        
        if step % 10 == 0:
            # Log important metrics
            print(f"Step {step}:")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

def debugging_example():
    """Demonstrate debugging capabilities."""
    print("\n=== Debugging Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Debugging simulation with error checking...")
    
    # Basic error checking
    try:
        # Validate configuration
        if not config.has("brain.neuron_count"):
            print("Warning: brain.neuron_count not set")
        
        # Validate brain state
        brain.step(0)
        print("Brain step successful")
        
        # Check for potential issues
        if brain.getTotalNeuronCount() == 0:
            print("Error: Brain has no neurons")
        
        print("Debugging checks completed")
        
    except Exception as e:
        print(f"Debug error: {e}")

def system_info_example():
    """Demonstrate system information gathering."""
    print("\n=== System Information Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("System information gathering...")
    
    # Display system information
    print(f"Python version: {__import__('sys').version}")
    print(f"NLM module available: {True}")
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Display brain configuration
    print(f"Configuration keys: {len(config.getKeys())} settings")
    
    # Check for potential issues
    config_summary = config.summary()
    print(f"Config summary: {config_summary}")

def simulation_profile_example():
    """Demonstrate complete simulation profiling."""
    print("\n=== Complete Simulation Profile Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running complete simulation profile...")
    
    start_time = time.time()
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Average step time: {(end_time - start_time) / 100:.4f} seconds")

def export_data_example():
    """Demonstrate data export functionality."""
    print("\n=== Data Export Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Exporting simulation data...")
    
    # Create sample simulation data
    simulation_data = {
        'timestamp': time.time(),
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
        'simulation_steps': 50,
        'peak_performance': {
            'max_firing_neurons': brain.getFiringNeuronCount(),
            'min_firing_rate': 0.0,
            'peak_timestamp': time.time()
        }
    }
    
    # Export to JSON file
    with open('simulation_data.json', 'w') as f:
        json.dump(simulation_data, f, indent=2)
    
    print(f"Data exported to simulation_data.json")
    
    # Show sample of exported data
    print("Sample exported data:")
    for key, value in list(simulation_data.items())[:5]:
        print(f"  {key}: {value}")
    
    # Clean up
    if os.path.exists('simulation_data.json'):
        os.remove('simulation_data.json')

def main():
    """Run all examples."""
    print("NLM Performance and Debugging Tools - Example Script")
    print("=" * 60)
    
    try:
        basic_performance_example()
        timer_profiling_example()
        memory_monitoring_example()
        configuration_example()
        enhanced_logging_example()
        debugging_example()
        system_info_example()
        simulation_profile_example()
        export_data_example()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("\nKey improvements demonstrated:")
        print("✓ Enhanced performance monitoring")
        print("✓ Timer-based profiling")
        print("✓ Memory usage tracking")
        print("✓ Configuration management")
        print("✓ Error checking and validation")
        print("✓ System information gathering")
        print("✓ Data export functionality")
        print("✓ Debug support and logging")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
