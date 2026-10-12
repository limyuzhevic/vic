#!/usr/bin/env python3
"""
NLM Enhanced Python API - Basic Examples

This script demonstrates the new high-level Python API features
for the NLM (Neural Learning Machine) project.

Run with: python examples/basic_examples.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import time

def example_1_basic_usage():
    """Example 1: Basic usage with enhanced API"""
    print("\n=== Example 1: Basic Usage ===")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

def example_2_agent_simulation():
    """Example 2: Complete agent simulation with visualization"""
    print("\n=== Example 2: Agent Simulation ===")
    
    # Setup configuration
    config = pynlm.create_experimental_config()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    pynlm.enable_learning(agent, True)
    pynlm.set_exploration_rate(agent, 0.2)
    
    # Run simulation
    print("Running simulation...")
    start_time = time.time()
    
    simulation_results = pynlm.run_simulation(brain, world, agent, 200)
    
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Steps simulated: {len(simulation_results)}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"Dopamine level: {agent.getNeuromodulationLevel():.4f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.4f}")

def example_3_training():
    """Example 3: Training a brain"""
    print("\n=== Example 3: Training ===")
    
    # Create experimental configuration
    config = pynlm.create_experimental_config()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    pynlm.enable_learning(agent, True)
    pynlm.set_exploration_rate(agent, 0.3)
    
    # Train the agent
    print("Training agent...")
    training_results = pynlm.train_agent(
        brain, world, agent, 
        episodes=20, 
        steps_per_episode=100
    )
    
    print("Training completed!")
    print(f"Total reward: {training_results['total_reward']:.4f}")
    print(f"Average reward per episode: {training_results['avg_reward_per_episode']:.4f}")
    print(f"Max firing rate: {training_results['max_firing_rate']:.4f}")
    print(f"Average firing rate: {training_results['avg_firing_rate']:.4f}")

def example_4_model_persistence():
    """Example 4: Save and load models"""
    print("\n=== Example 4: Model Persistence ===")
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation to "train" it
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain1)
    agent.initialize(world)
    
    pynlm.run_simulation(brain1, world, agent, 50)
    
    # Save the model
    print("Saving brain model...")
    saved_path = "example_brain.nlm"
    pynlm.save_model(brain1, saved_path)
    print(f"Saved to: {saved_path}")
    
    # Create a new brain and load the model
    print("Loading brain model...")
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    pynlm.load_model(brain2, saved_path)
    brain2.initialize()
    
    print("Model loaded successfully!")
    print(f"Brain 1 neuron count: {brain1.getTotalNeuronCount()}")
    print(f"Brain 2 neuron count: {brain2.getTotalNeuronCount()}")

def example_5_analysis():
    """Example 5: Advanced analysis"""
    print("\n=== Example 5: Advanced Analysis ===")
    
    # Create a brain and run simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run longer simulation for better analysis
    for i in range(1000):
        brain.step(i)
    
    # Analyze patterns
    print("Analyzing neural patterns...")
    patterns = pynlm.analyze_patterns(brain)
    
    print("Pattern Analysis Results:")
    for key, value in patterns.items():
        print(f"  {key}: {value:.4f}")
    
    # Detect anomalies
    print("\nDetecting anomalies...")
    anomalies = pynlm.detect_anomalies(brain)
    
    detected = False
    for key, value in anomalies.items():
        if value > 0:
            print(f"  {key}: DETECTED (score: {value:.4f})")
            detected = True
    
    if not detected:
        print("  No anomalies detected - good!")
    
    # Get performance summary
    print("\nGetting performance summary...")
    summary = pynlm.summarize_performance(brain)
    
    print("Performance Summary:")
    for key, value in summary.items():
        print(f"  {key}: {value:.4f}")

def example_6_performance_monitoring():
    """Example 6: Performance monitoring"""
    print("\n=== Example 6: Performance Monitoring ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get performance stats
    print("Getting performance statistics...")
    stats = pynlm.get_performance_stats(brain)
    
    print("Performance Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value:.4f}")
    
    # Run benchmarks
    print("\nRunning benchmarks...")
    benchmarks = pynlm.benchmark(brain)
    
    print("Benchmark Results:")
    for key, value in benchmarks.items():
        print(f"  {key}: {value:.4f} ms")

def example_7_configuration_utilities():
    """Example 7: Configuration utilities"""
    print("\n=== Example 7: Configuration Utilities ===")
    
    # Create experimental config
    print("Creating experimental configuration...")
    experimental_config = pynlm.create_experimental_config()
    print(f"Experimental config keys: {experimental_config.getKeys()}")
    
    # Create production config
    print("\nCreating production configuration...")
    production_config = pynlm.create_production_config()
    print(f"Production config keys: {production_config.getKeys()}")
    
    # Validate configurations
    print("\nValidating configurations...")
    try:
        result = pynlm.validate_config(experimental_config)
        print(f"Experimental config validation: {result}")
    except Exception as e:
        print(f"Experimental config validation failed: {e}")
    
    try:
        result = pynlm.validate_config(production_config)
        print(f"Production config validation: {result}")
    except Exception as e:
        print(f"Production config validation failed: {e}")

def example_8_error_handling():
    """Example 8: Enhanced error handling"""
    print("\n=== Example 8: Error Handling ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test safe operation
    print("Testing safe operation...")
    
    def risky_operation():
        return 1 / 0  # This will cause an error
    
    def error_handler(error_msg):
        print(f"Error caught: {error_msg}")
        return None
    
    result = pynlm.safe_operation(risky_operation, error_handler)
    print(f"Safe operation result: {result}")
    
    # Test validation
    print("\nTesting config validation...")
    invalid_config = pynlm.createDefaultConfig()
    try:
        pynlm.validate_config(invalid_config)
        print("Config validation passed (unexpected)")
    except Exception as e:
        print(f"Config validation correctly failed: {e}")

def main():
    """Run all examples"""
    print("=" * 60)
    print("NLM Enhanced Python API - Basic Examples")
    print("=" * 60)
    
    examples = [
        example_1_basic_usage,
        example_2_agent_simulation,
        example_3_training,
        example_4_model_persistence,
        example_5_analysis,
        example_6_performance_monitoring,
        example_7_configuration_utilities,
        example_8_error_handling,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
        print("-" * 60)
    
    print("\nAll examples completed!")
    print("\nFor more information, see README_ENHANCED.md")

if __name__ == "__main__":
    main()
