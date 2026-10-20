"""
Enhanced NLM Python Examples
=============================

This module provides comprehensive examples for using the enhanced NLM Python API.
It demonstrates performance monitoring, learning experiments, advanced agent capabilities,
and more intuitive interfaces.

The examples cover:
1. Basic enhanced brain setup and usage
2. Performance monitoring and profiling
3. Advanced agent with curiosity-driven exploration
4. Learning experiments with automatic analysis
5. Configuration management
6. Real-world simulation scenarios
"""

import pynlm_enhanced as nlm
import time
import json
import os
from typing import Dict, List, Optional

def example_basic_enhanced_brain():
    """
    Example 1: Basic Enhanced Brain Setup
    =====================================
    
    This example demonstrates the enhanced brain API with performance monitoring
    and intuitive configuration management.
    """
    print("=== Example 1: Basic Enhanced Brain Setup ===\n")
    
    # Create enhanced configuration with preset modes
    config = nlm.createEnhancedConfig()
    config.setupDefaultBrain(neuronCount=500, regionCount=2)
    config.setupLearningMode()
    
    print("Configuration loaded:")
    config.printConfig()
    
    # Create enhanced brain
    brain = nlm.createEnhancedBrain(config)
    
    # Initialize with monitoring
    if brain.initialize():
        print("\n✓ Brain initialized successfully")
        print(f"  Total neurons: {brain.getNeuronCount()}")
        print(f"  Total synapses: {brain.getSynapseCount()}")
    else:
        print("\n✗ Brain initialization failed")
        return
    
    # Run simulation with performance monitoring
    print("\nRunning simulation with performance monitoring...")
    start_time = time.time()
    
    brain.batchProcess(100, timestep=0.001)
    
    elapsed = time.time() - start_time
    print(f"\nSimulation completed in {elapsed:.2f} seconds")
    
    # Get performance report
    print("\n=== Performance Report ===")
    print(brain.getPerformanceReport())
    
    return brain

def example_advanced_agent():
    """
    Example 2: Advanced Agent with Curiosity-Driven Exploration
    ============================================================
    
    This example demonstrates the enhanced agent API with curiosity, learning,
    and exploration capabilities.
    """
    print("\n\n=== Example 2: Advanced Agent with Curiosity ===\n")
    
    # Create brain and enhanced agent
    config = nlm.createEnhancedConfig()
    config.setupDefaultBrain(neuronCount=1000, regionCount=3)
    config.setupAgentMode()
    config.setupLearningMode()
    
    brain = nlm.createEnhancedBrain(config)
    agent = nlm.createEnhancedAgent(brain)
    
    # Initialize agent with all features enabled
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world, enableExploration=True, enableLearning=True, enableDevelopment=True)
    
    print("Agent initialized with all features enabled")
    
    # Show initial agent status
    print("\n=== Initial Agent Status ===")
    print(agent.getStatus())
    
    # Run enhanced simulation with curiosity and exploration
    print("\n=== Running Enhanced Simulation ===")
    for step in range(50):
        # Execute action with curiosity
        action = agent.executeAction(world, reward=0.5, predictedReward=0.3)
        
        if step % 10 == 0:
            print(f"\nStep {step}:")
            print(f"  Action: {action}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Brain status: {brain.getFiringNeuronCount()} firing neurons")
    
    print("\n=== Final Agent Status ===")
    print(agent.getStatus())
    
    return brain, agent

def example_learning_experiment():
    """
    Example 3: Learning Experiment with Automatic Analysis
    ======================================================
    
    This example demonstrates the learning experiment utility with automatic
    recording, analysis, and reporting.
    """
    print("\n\n=== Example 3: Learning Experiment ===\n")
    
    # Create brain for experiment
    config = nlm.createEnhancedConfig()
    config.setupDefaultBrain(neuronCount=800, regionCount=2)
    config.setupLearningMode()
    
    brain = nlm.createEnhancedBrain(config)
    experiment = nlm.createLearningExperiment(brain)
    
    print("Running learning experiment...")
    print("The experiment will:")
    print("  - Apply learning stimuli to the brain")
    print("  - Record initial and final synaptic weights")
    print("  - Analyze weight changes")
    print("  - Report on learning effectiveness")
    
    # Run learning experiment
    experiment.runLearningExperiment(duration=5.0, verbose=True)
    
    return brain

def example_performance_comparison():
    """
    Example 4: Performance Comparison
    ==================================
    
    This example demonstrates performance monitoring and comparison between
    different configurations and settings.
    """
    print("\n\n=== Example 4: Performance Comparison ===\n")
    
    # Test different configurations
    configs = [
        ("Basic", lambda c: c.setupDefaultBrain(neuronCount=500)),
        ("High Performance", lambda c: c.setupHighPerformance(threads=4)),
        ("Development Mode", lambda c: (c.setupDefaultBrain(), c.setupDevelopmentMode())),
        ("Learning Mode", lambda c: (c.setupDefaultBrain(), c.setupLearningMode())),
    ]
    
    results = {}
    
    for name, config_func in configs:
        print(f"\nTesting {name} configuration...")
        
        config = nlm.createEnhancedConfig()
        config_func(config)
        
        brain = nlm.createEnhancedBrain(config)
        
        if brain.initialize():
            brain.batchProcess(50, timestep=0.001)
            metrics = brain.getMetrics()
            results[name] = {
                'spikes': metrics.totalSpikeCount,
                'active_neurons': metrics.activeNeuronCount,
                'performance_score': brain.getPerformanceScore()
            }
            print(f"  Performance score: {results[name]['performance_score']:.6f}")
            print(f"  Total spikes: {results[name]['spikes']}")
        else:
            results[name] = {'error': 'Initialization failed'}
    
    # Compare results
    print("\n=== Performance Comparison ===")
    for name, result in results.items():
        if 'error' in result:
            print(f"  {name}: {result['error']}")
        else:
            print(f"  {name}:")
            print(f"    Performance: {result['performance_score']:.6f}")
            print(f"    Spikes: {result['spikes']}")
            print(f"    Active neurons: {result['active_neurons']}")
    
    return results

def example_configuration_management():
    """
    Example 5: Configuration Management
    ====================================
    
    This example demonstrates the enhanced configuration management with
    template configurations, validation, and file I/O.
    """
    print("\n\n=== Example 5: Configuration Management ===\n")
    
    # Get example configurations
    examples = nlm.getExampleConfigs()
    
    print("Available configuration templates:")
    for name, config_template in examples.items():
        print(f"\n{name.upper()} Template:")
        print(config_template[:200] + "..." if len(config_template) > 200 else config_template)
    
    # Create configuration from template
    config = nlm.createEnhancedConfig()
    
    # You can load from file, or create programmatically
    print("\n=== Configuration Management Demo ===")
    
    # Get all available keys
    all_keys = config.getAllKeys()
    print(f"Default config has {len(all_keys)} keys:")
    for key in all_keys[:10]:  # Show first 10 keys
        print(f"  - {key}")
    if len(all_keys) > 10:
        print(f"  ... and {len(all_keys) - 10} more")
    
    # Demonstrate getting values with defaults
    print("\nValue retrieval with defaults:")
    print(f"  Neuron count: {config.getValueOr('neuron_count', 1000)}")
    print(f"  Simulation timestep: {config.getValueOr('simulation_timestep', 0.001)}")
    print(f"  Non-existent key: {config.getValueOr('nonexistent_key', 'default_value')}")
    
    # Print configuration summary
    print("\nConfiguration summary:")
    config.printConfig()
    
    return config

def example_real_world_simulation():
    """
    Example 6: Real-World Simulation Scenario
    ============================================
    
    This example demonstrates a realistic simulation scenario with an agent
    learning to navigate a world, make decisions, and adapt based on experience.
    """
    print("\n\n=== Example 6: Real-World Simulation ===\n")
    
    # Setup environment
    print("Initializing real-world simulation...")
    config = nlm.createEnhancedConfig()
    config.setupDefaultBrain(neuronCount=1200, regionCount=3)
    config.setupAgentMode()
    config.setupLearningMode()
    
    brain = nlm.createEnhancedBrain(config)
    agent = nlm.createEnhancedAgent(brain)
    
    # Create realistic world
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.01)
    
    # Add some obstacles and resources
    for x in range(5, 25, 7):
        for y in range(5, 25, 7):
            world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType::Resource, 10.0))
    
    for x in range(10, 20, 5):
        for y in range(10, 20, 5):
            world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType::Hazard, -20.0))
    
    world.reset()
    
    # Initialize agent with enhanced features
    agent.initialize(world, enableExploration=True, enableLearning=True, enableDevelopment=True)
    
    print("Real-world simulation initialized")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision range: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run simulation episode
    episode_reward = 0.0
    episode_steps = 0
    max_steps = 100
    
    print("\n=== Running Simulation Episode ===")
    for step in range(max_steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Execute action with curiosity and learning
        action = agent.executeAction(world, reward=0.0, predictedReward=0.0)
        
        # Get reward from world
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        reward = action_result.reward
        episode_reward += reward
        
        episode_steps += 1
        
        if step % 20 == 0:
            print(f"\nStep {step}:")
            print(f"  Action: {action}")
            print(f"  Reward: {reward:.2f}")
            print(f"  Total episode reward: {episode_reward:.2f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Brain firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        # Check if agent should learn from this experience
        if abs(reward) > 0.1:
            agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Check for early termination
        agent_body = world.getAgentBody()
        if agent_body.health <= 0 or episode_steps >= max_steps:
            print(f"\nEpisode terminated at step {step}")
            break
    
    print(f"\n=== Episode Complete ===")
    print(f"Total steps: {episode_steps}")
    print(f"Total reward: {episode_reward:.2f}")
    print(f"Final agent status:")
    print(agent.getStatus())
    
    # Save brain state
    brain.save("saved_brain.nlm")
    print(f"\nBrain state saved to 'saved_brain.nlm'")
    
    return brain, agent, world

def example_advanced_features():
    """
    Example 7: Advanced Features and Customization
    ================================================
    
    This example demonstrates advanced features like custom configurations,
    performance optimization, and feature integration.
    """
    print("\n\n=== Example 7: Advanced Features ===\n")
    
    # Create custom configuration
    config = nlm.createEnhancedConfig()
    config.setupDefaultBrain(neuronCount=2000, regionCount=5)
    config.setupDevelopmentMode()
    config.setupLearningMode()
    config.setupHighPerformance(threads=8)
    
    print("Custom configuration created with:")
    print("  - 2000 neurons, 5 regions")
    print("  - Development mode enabled")
    print("  - Learning mode enabled")
    print("  - High performance optimization")
    
    # Create enhanced brain
    brain = nlm.createEnhancedBrain(config)
    
    # Enable advanced features
    brain.enableLogging(True, "advanced_simulation.log")
    
    print("\nInitializing advanced brain...")
    if brain.initialize():
        print("✓ Advanced brain initialized")
        
        # Run more intensive simulation
        print("\nRunning intensive simulation...")
        brain.batchProcess(200, timestep=0.001)
        
        # Get performance analysis
        print("\n=== Advanced Performance Analysis ===")
        print(brain.getPerformanceReport())
        
    else:
        print("✗ Failed to initialize advanced brain")
    
    return brain

def run_all_examples():
    """
    Run all examples with proper error handling and reporting.
    """
    print("=" * 80)
    print("Enhanced NLM Python Examples")
    print("Running comprehensive demonstration of NLM capabilities")
    print("=" * 80)
    
    examples = [
        ("Basic Enhanced Brain", example_basic_enhanced_brain),
        ("Advanced Agent with Curiosity", example_advanced_agent),
        ("Learning Experiment", example_learning_experiment),
        ("Performance Comparison", example_performance_comparison),
        ("Configuration Management", example_configuration_management),
        ("Real-World Simulation", example_real_world_simulation),
        ("Advanced Features", example_advanced_features),
    ]
    
    results = {}
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            print(f"EXECUTING: {name}")
            print(f"{'='*60}")
            
            result = example_func()
            results[name] = {"status": "success", "result": result}
            print(f"\n✓ {name} completed successfully")
            
        except Exception as e:
            print(f"\n✗ {name} failed with error: {e}")
            import traceback
            traceback.print_exc()
            results[name] = {"status": "error", "error": str(e)}
    
    print(f"\n{'='*60}")
    print("SUMMARY")
    print(f"{'='*60}")
    
    success_count = 0
    for name, result in results.items():
        status = "✓ PASS" if result["status"] == "success" else "✗ FAIL"
        print(f"{status} {name}")
        if result["status"] == "success":
            success_count += 1
    
    print(f"\nCompleted: {success_count}/{len(examples)} examples")
    return results

if __name__ == "__main__":
    # Run all examples
    all_results = run_all_examples()
    
    # Save results to file
    with open("example_results.json", "w") as f:
        import json
        # Convert results to JSON-serializable format
        serializable_results = {}
        for name, result in all_results.items():
            if result["status"] == "success":
                serializable_results[name] = {"status": "success"}
            else:
                serializable_results[name] = {"status": "error", "error": result["error"]}
        json.dump(serializable_results, f, indent=2)
    
    print(f"\nResults saved to 'example_results.json'")
    
    print("\n" + "="*80)
    print("Enhanced NLM Python Examples completed!")
    print("="*80)
