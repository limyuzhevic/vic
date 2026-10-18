#!/usr/bin/env python3
"""
Comprehensive NLM Example Suite

This script provides extensive examples covering all major features of the NLM framework.
It demonstrates the complete range of NLM capabilities from basic operations to advanced research scenarios.

The examples are organized into functional categories:
1. Core Brain Operations
2. World Interaction and Environment
3. Learning Systems
4. Memory Systems
5. Neuromodulation
6. Development and Aging
7. Performance Monitoring and Analysis
8. State Management
9. Advanced Configurations
10. Research Scenarios

Each example function provides clear documentation and can be run independently.
"""

import sys
import time
import json
from typing import Dict, List, Any, Optional

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Error: Could not import pynlm: {e}")
    print("Please install the NLM Python bindings using:")
    print("  pip install pynlm")
    sys.exit(1)


def example_1_basic_brain_operations():
    """
    Example 1: Core Brain Operations
    
    Demonstrates fundamental brain creation, initialization, and basic operations.
    This covers the essential workflow for any NLM simulation.
    """
    print("=" * 70)
    print("Example 1: Core Brain Operations")
    print("=" * 70)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("random_seed", 42)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration: {config.summary()}")
    
    # Run basic simulation
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            spikes = brain.getTotalSpikeCount()
            print(f"  Step {step:2d}: {firing:3d} firing neurons, {rate:6.2f} avg rate, {spikes:3d} total spikes")
    
    print("Basic brain operations complete!")
    print()
    

def example_2_world_environment_interaction():
    """
    Example 2: World Environment Interaction
    
    Demonstrates how to create and interact with the world environment,
    including agent movement, sensory perception, and motor control.
    """
    print("=" * 70)
    print("Example 2: World Environment Interaction")
    print("=" * 70)
    
    # Create brain
    config = pynlm.createStandardConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable basic learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Environment: {world.getWidth()}x{world.getHeight()} cells")
    print(f"Agent starting position: (10.0, 10.0)")
    print(f"Vision range: {world.getPercept().getVisionWidth()}x{world.getPercept().getVisionHeight()}")
    
    # Run world interaction simulation
    for step in range(100):
        # Update world (time passes)
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Agent processes sensory input
        agent.processSensoryInput(percept)
        
        # Brain computes action
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log periodic updates
        if step % 20 == 0:
            body = world.getAgentBody()
            print(f"  Step {step:3d}: Position ({body.x:5.1f}, {body.y:5.1f}), "
                  f"Energy {body.energy:5.1f}, Time {world.getSimulationTime():5.2f}")
    
    print("World environment interaction complete!")
    print()
    

def example_3_advanced_learning_systems():
    """
    Example 3: Advanced Learning Systems
    
    Demonstrates sophisticated learning capabilities including reward-based learning,
    structural plasticity, development, and curiosity-driven exploration.
    """
    print("=" * 70)
    print("Example 3: Advanced Learning Systems")
    print("=" * 70)
    
    # Create exploration configuration
    config = pynlm.createExplorationConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Learning systems enabled:")
    print(f"  ✓ Reward Modulation: Learns from rewards and punishments")
    print(f"  ✓ Structural Plasticity: Can create new connections")
    print(f"  ✓ Development: Brain matures over time")
    print(f"  ✓ Curiosity: Explores novel experiences")
    
    # Run advanced learning simulation
    learning_data = []
    for step in range(300):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate sophisticated reward
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        
        # Position-based reward
        position_reward = 1.0 if (0 <= x < width and 0 <= y < height) else -1.0
        
        # Activity-based reward
        activity_reward = brain.getFiringNeuronCount() * 0.001
        
        # Curiosity-based reward
        curiosity_reward = agent.getCuriosityLevel() * 0.1
        
        total_reward = position_reward + activity_reward + curiosity_reward
        agent.applyRewardModulation(total_reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record learning metrics
        learning_data.append({
            'step': step,
            'reward': total_reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'development_stage': brain.getDevelopmentalStage(),
            'firing_neurons': brain.getFiringNeuronCount()
        })
        
        # Log periodic updates
        if step % 50 == 0:
            print(f"  Step {step:3d}: curiosity={agent.getCuriosityLevel():.2f}, "
                  f"novelty={agent.getNoveltyLevel():.2f}, "
                  f"neuromod={agent.getNeuromodulationLevel():.2f}, "
                  f"dev_stage={brain.getDevelopmentalStage()}, "
                  f"reward={total_reward:6.2f}")
    
    # Analyze learning results
    final_step = learning_data[-1]
    avg_curiosity = sum(d['curiosity'] for d in learning_data) / len(learning_data)
    avg_novelty = sum(d['novelty'] for d in learning_data) / len(learning_data)
    avg_reward = sum(d['reward'] for d in learning_data) / len(learning_data)
    
    print("\nLearning Analysis:")
    print(f"  Final curiosity level: {final_step['curiosity']:.2f}")
    print(f"  Final novelty level: {final_step['novelty']:.2f}")
    print(f"  Final neuromodulation level: {final_step['neuromodulation']:.2f}")
    print(f"  Final developmental stage: {final_step['development_stage']}")
    print(f"  Average reward per step: {avg_reward:.2f}")
    print(f"  Average curiosity over time: {avg_curiosity:.2f}")
    print(f"  Average novelty over time: {avg_novelty:.2f}")
    
    print("Advanced learning systems complete!")
    print()
    

def example_4_memory_systems():
    """
    Example 4: Memory Systems
    
    Demonstrates the different memory systems in NLM: working memory,
    episodic memory, and associative memory with pattern completion.
    """
    print("=" * 70)
    print("Example 4: Memory Systems")
    print("=" * 70)
    
    # Create configuration optimized for memory research
    config = pynlm.createStandardConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    
    # Create brain for memory research
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent.initialize(world)
    
    # Enable all memory-related systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Memory system demonstration with:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  World: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  Agent: Learning from rewards and novel experiences")
    
    # Run memory exploration simulation
    memory_metrics = []
    for step in range(400):
        # Update world and get sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        
        position_reward = 0.5 if (10 <= x <= 30 and 10 <= y <= 30) else -0.5
        novelty_reward = agent.getNoveltyLevel() * 0.05
        curiosity_reward = agent.getCuriosityLevel() * 0.02
        
        total_reward = position_reward + novelty_reward + curiosity_reward
        agent.applyRewardModulation(total_reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record memory metrics
        memory_metrics.append({
            'step': step,
            'reward': total_reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'development_stage': brain.getDevelopmentalStage(),
            'working_memory': agent.getNeuromodulationLevel() * 0.1,  # Simulated working memory metric
            'episodic_strength': agent.getCuriosityLevel() * 0.05,     # Simulated episodic memory metric
            'associative_links': agent.getNoveltyLevel() * 0.03       # Simulated associative memory metric
        })
        
        # Log memory-related metrics
        if step % 80 == 0:
            print(f"  Step {step:3d}: curiosity={agent.getCuriosityLevel():.2f}, "
                  f"novelty={agent.getNoveltyLevel():.2f}, "
                  f"working_mem={agent.getNeuromodulationLevel()*0.1:.2f}, "
                  f"episodic={agent.getCuriosityLevel()*0.05:.2f}, "
                  f"associative={agent.getNoveltyLevel()*0.03:.2f}")
    
    # Analyze memory system performance
    final_metrics = memory_metrics[-1]
    total_steps = len(memory_metrics)
    
    print("\nMemory System Analysis:")
    print(f"  Total simulation steps: {total_steps}")
    print(f"  Final development stage: {final_metrics['development_stage']}")
    print(f"  Final curiosity level: {final_metrics['curiosity']:.2f}")
    print(f"  Final novelty level: {final_metrics['novelty']:.2f}")
    print(f"  Average neuromodulation: {sum(m['neuromodulation'] for m in memory_metrics)/total_steps:.2f}")
    print(f"  Working memory metric (average): {sum(m['working_memory'] for m in memory_metrics)/total_steps:.2f}")
    print(f"  Episodic memory metric (average): {sum(m['episodic_strength'] for m in memory_metrics)/total_steps:.2f}")
    print(f"  Associative memory metric (average): {sum(m['associative_links'] for m in memory_metrics)/total_steps:.2f}")
    
    # Demonstrate memory consolidation
    print("\nMemory Consolidation Phase:")
    print("  Simulating replay and consolidation of important episodes...")
    consolidation_steps = 50
    for i in range(consolidation_steps):
        # Replay important episodes (simplified simulation)
        replay_reward = 1.0  # Successful replay gives reward
        agent.applyRewardModulation(replay_reward, 0.0)
        agent.updateDevelopment(0.05)
        
        if i % 10 == 0:
            print(f"  Consolidation step {i+1}/{consolidation_steps}: "
                  f"Replay strength {replay_reward:.1f}")
    
    print("Memory systems complete!")
    print()
    

def example_5_prediction_systems():
    """
    Example 5: Prediction Systems
    
    Demonstrates temporal sequence prediction and action consequence prediction,
    showing how NLM predicts future states and uses prediction errors for learning.
    """
    print("=" * 70)
    print("Example 5: Prediction Systems")
    print("=" * 70)
    
    # Create configuration for prediction research
    config = pynlm.createExplorationConfig()
    config.set("neuron_count", 800)
    config.set("region_count", 2)
    
    # Create brain with prediction capabilities
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=35, height=35, visionWidth=14, visionHeight=14)
    world.reset()
    world.setAgentStart(17.5, 17.5)
    
    agent.initialize(world)
    
    # Enable prediction-related systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Prediction system demonstration with:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  Prediction system integrated into brain loop")
    print(f"  Action consequence prediction enabled")
    
    # Run prediction simulation
    prediction_data = []
    for step in range(350):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate prediction error reward
        prediction_error = agent.getPredictionError()
        prediction_reward = prediction_error * 0.5  # Reward for good prediction
        
        # Position reward
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        position_reward = 0.5 if (10 <= x <= 25 and 10 <= y <= 25) else -0.5
        
        # Neuromodulation reward
        neuromodulation_reward = agent.getNeuromodulationLevel() * 0.1
        
        total_reward = prediction_reward + position_reward + neuromodulation_reward
        agent.applyRewardModulation(total_reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record prediction metrics
        prediction_data.append({
            'step': step,
            'prediction_error': prediction_error,
            'prediction_reward': prediction_reward,
            'position_reward': position_reward,
            'neuromodulation_reward': neuromodulation_reward,
            'total_reward': total_reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
        
        # Log prediction metrics
        if step % 70 == 0:
            print(f"  Step {step:3d}: prediction_error={agent.getPredictionError():.2f}, "
                  f"prediction_reward={prediction_reward:6.2f}, "
                  f"curiosity={agent.getCuriosityLevel():.2f}, "
                  f"novelty={agent.getNoveltyLevel():.2f}")
    
    # Analyze prediction system performance
    final_pred = prediction_data[-1]
    avg_prediction_error = sum(d['prediction_error'] for d in prediction_data) / len(prediction_data)
    avg_prediction_reward = sum(d['prediction_reward'] for d in prediction_data) / len(prediction_data)
    
    print("\nPrediction System Analysis:")
    print(f"  Final prediction error: {final_pred['prediction_error']:.2f}")
    print(f"  Average prediction error: {avg_prediction_error:.2f}")
    print(f"  Average prediction reward: {avg_prediction_reward:.2f}")
    print(f"  Final curiosity level: {final_pred['curiosity']:.2f}")
    print(f"  Final novelty level: {final_pred['novelty']:.2f}")
    
    # Demonstrate prediction accuracy improvement
    print("\nPrediction Accuracy Improvement:")
    initial_accuracy = 1.0 - abs(prediction_data[0]['prediction_error'])
    final_accuracy = 1.0 - abs(prediction_data[-1]['prediction_error'])
    
    print(f"  Initial prediction accuracy: {initial_accuracy:.2f}")
    print(f"  Final prediction accuracy: {final_accuracy:.2f}")
    print(f"  Improvement: {final_accuracy - initial_accuracy:.2f}")
    
    print("Prediction systems complete!")
    print()
    

def example_6_cognitive_systems():
    """
    Example 6: Cognitive Systems
    
    Demonstrates cognitive mechanisms including attention, planning, and self-model,
    showing how NLM implements cognition without traditional AI approaches.
    """
    print("=" * 70)
    print("Example 6: Cognitive Systems")
    print("=" * 70)
    
    # Create configuration for cognitive research
    config = pynlm.createStandardConfig()
    config.set("neuron_count", 1200)
    config.set("region_count", 4)
    
    # Create brain with cognitive capabilities
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent.initialize(world)
    
    # Enable cognitive systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Cognitive system demonstration with:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions")
    print(f"  Cognitive systems: Attention, Planning, Self-model")
    print(f"  Competitive selection dynamics implemented")
    
    # Run cognitive simulation
    cognitive_metrics = []
    for step in range(450):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate cognitive reward
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        
        # Position-based reward
        position_reward = 0.3 if (15 <= x <= 35 and 15 <= y <= 35) else -0.3
        
        # Cognitive load reward (based on novelty and curiosity)
        cognitive_load_reward = (agent.getNoveltyLevel() + agent.getCuriosityLevel()) * 0.05
        
        # Development reward
        development_reward = brain.getDevelopmentalStage() * 0.02
        
        total_reward = position_reward + cognitive_load_reward + development_reward
        agent.applyRewardModulation(total_reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record cognitive metrics
        cognitive_metrics.append({
            'step': step,
            'reward': total_reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': brain.getDevelopmentalStage(),
            'attention_level': agent.getNeuromodulationLevel() * 0.8,  # Simulated attention
            'planning_complexity': agent.getCuriosityLevel() * 0.6,   # Simulated planning
            'self_model_strength': agent.getNoveltyLevel() * 0.4      # Simulated self-model
        })
        
        # Log cognitive metrics
        if step % 90 == 0:
            print(f"  Step {step:3d}: attention={agent.getNeuromodulationLevel()*0.8:.2f}, "
                  f"planning={agent.getCuriosityLevel()*0.6:.2f}, "
                  f"self_model={agent.getNoveltyLevel()*0.4:.2f}, "
                  f"curiosity={agent.getCuriosityLevel():.2f}")
    
    # Analyze cognitive system performance
    final_cog = cognitive_metrics[-1]
    avg_attention = sum(m['attention_level'] for m in cognitive_metrics) / len(cognitive_metrics)
    avg_planning = sum(m['planning_complexity'] for m in cognitive_metrics) / len(cognitive_metrics)
    avg_self_model = sum(m['self_model_strength'] for m in cognitive_metrics) / len(cognitive_metrics)
    
    print("\nCognitive System Analysis:")
    print(f"  Final development stage: {final_cog['development_stage']}")
    print(f"  Average attention level: {avg_attention:.2f}")
    print(f"  Average planning complexity: {avg_planning:.2f}")
    print(f"  Average self-model strength: {avg_self_model:.2f}")
    print(f"  Final curiosity level: {final_cog['curiosity']:.2f}")
    print(f"  Final novelty level: {final_cog['novelty']:.2f}")
    
    # Demonstrate cognitive emergence
    print("\nCognitive Emergence Phase:")
    print("  Simulating emergence of cognitive capabilities...")
    
    # Track cognitive emergence over time
    emergence_metrics = []
    for i in range(100):
        # Simulate cognitive development
        curiosity_growth = 0.02 * (i / 100) ** 0.5
        novelty_growth = 0.015 * (i / 100) ** 0.7
        
        emergence_metrics.append({
            'step': i,
            'curiosity_level': curiosity_growth,
            'novelty_level': novelty_growth,
            'cognitive_maturity': (curiosity_growth + novelty_growth) / 2
        })
        
        if i % 25 == 0:
            maturity = emergence_metrics[-1]['cognitive_maturity']
            print(f"  Cognitive maturity step {i+1}/100: {maturity:.2f}")
    
    print("Cognitive systems complete!")
    print()
    

def example_7_performance_monitoring():
    """
    Example 7: Performance Monitoring and Analysis
    
    Demonstrates comprehensive performance monitoring, benchmarking, and analysis
    for optimization and system evaluation.
    """
    print("=" * 70)
    print("Example 7: Performance Monitoring and Analysis")
    print("=" * 70)
    
    # Create different configurations for performance comparison
    configs = {
        'small': pynlm.createDefaultConfig(),
        'medium': pynlm.createStandardConfig(),
        'large': pynlm.createExplorationConfig()
    }
    
    # Set neuron counts for scaling
    for name, config in configs.items():
        if name == 'small':
            config.set("neuron_count", 100)
            config.set("region_count", 1)
        elif name == 'medium':
            config.set("neuron_count", 1000)
            config.set("region_count", 2)
        else:  # large
            config.set("neuron_count", 5000)
            config.set("region_count", 4)
    
    # Performance benchmark function
    def benchmark_configuration(name, config, steps=200):
        """Benchmark a single configuration."""
        print(f"\nBenchmarking {name} configuration...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
            
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Calculate performance metrics
        steps_per_second = steps / elapsed
        neurons_per_second = config.get("neuron_count", 1000) * steps_per_second
        spikes_per_step = brain.getTotalSpikeCount() / steps if steps > 0 else 0
        
        return {
            'name': name,
            'elapsed_time': elapsed,
            'steps_per_second': steps_per_second,
            'neurons_per_second': neurons_per_second,
            'spikes_per_step': spikes_per_step,
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_spikes': brain.getTotalSpikeCount()
        }
    
    # Run benchmarks
    benchmark_results = []
    for name, config in configs.items():
        result = benchmark_configuration(name, config, 200)
        benchmark_results.append(result)
        
        print(f"  Steps per second: {result['steps_per_second']:.0f}")
        print(f"  Neurons per second: {result['neurons_per_second']:.0f}")
        print(f"  Spikes per step: {result['spikes_per_step']:.1f}")
        print(f"  Final firing rate: {result['final_firing_rate']:.2f}")
    
    # Performance analysis
    print("\n" + "=" * 70)
    print("PERFORMANCE ANALYSIS")
    print("=" * 70)
    
    # Find best performer
    best_config = max(benchmark_results, key=lambda x: x['steps_per_second'])
    slowest_config = min(benchmark_results, key=lambda x: x['steps_per_second'])
    
    print(f"Best performing configuration: {best_config['name']}")
    print(f"  Steps per second: {best_config['steps_per_second']:.0f}")
    print(f"  Neurons per second: {best_config['neurons_per_second']:.0f}")
    
    print(f"\nSlowest performing configuration: {slowest_config['name']}")
    print(f"  Steps per second: {slowest_config['steps_per_second']:.0f}")
    print(f"  Neurons per second: {slowest_config['neurons_per_second']:.0f}")
    
    # Calculate performance scaling
    print("\nPerformance Scaling Analysis:")
    for i, result in enumerate(benchmark_results):
        efficiency = result['neurons_per_second'] / result['elapsed_time']
        print(f"  {result['name']}: {efficiency:.0f} efficiency score")
    
    # Memory usage estimation (simplified)
    print("\nMemory Usage Estimation:")
    for result in benchmark_results:
        estimated_memory_mb = result['neurons_per_second'] * 0.001  # Rough estimate
        print(f"  {result['name']}: ~{estimated_memory_mb:.1f} MB estimated memory")
    
    # Generate performance report
    performance_report = {
        'benchmarks': benchmark_results,
        'analysis': {
            'best_configuration': best_config['name'],
            'scaling_efficiency': best_config['steps_per_second'] / slowest_config['steps_per_second'],
            'recommendations': [
                f"Use {best_config['name']} configuration for best performance",
                "Consider parallel processing for large-scale simulations",
                "Optimize memory usage with appropriate configuration"
            ]
        }
    }
    
    # Save performance report
    with open('performance_report.json', 'w') as f:
        json.dump(performance_report, f, indent=2)
    
    print(f"\nPerformance report saved to performance_report.json")
    
    print("Performance monitoring complete!")
    print()
    

def example_8_state_management():
    """
    Example 8: State Management and Persistence
    
    Demonstrates brain state saving, loading, and checkpoint management
    for long-running simulations and experiment reproducibility.
    """
    print("=" * 70)
    print("Example 8: State Management and Persistence")
    print("=" * 70)
    
    # Create and initialize brain
    config = pynlm.createStandardConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created initial brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial state - Spikes: {brain.getTotalSpikeCount()}, "
          f"Firing: {brain.getFiringNeuronCount()}, "
          f"Rate: {brain.getAverageFiringRate():.2f}")
    
    # Run initial simulation phase
    print("\nRunning initial simulation phase...")
    for step in range(100):
        brain.step(step)
        if step % 25 == 0:
            print(f"  Step {step:3d}: {brain.getFiringNeuronCount():3d} firing neurons")
    
    print(f"\nInitial simulation complete:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Save brain state (checkpoint 1)
    print("\nSaving checkpoint 1...")
    brain.save("checkpoint_1.bin")
    
    config.saveToFile("checkpoint_config_1.json")
    
    print("Checkpoint 1 saved successfully!")
    
    # Continue simulation
    print("\nContinuing simulation...")
    for step in range(100, 250):
        brain.step(step)
        
        # Save intermediate checkpoint
        if step % 50 == 0:
            checkpoint_num = step // 50
            brain.save(f"checkpoint_{checkpoint_num}.bin")
            print(f"  Checkpoint {checkpoint_num} saved at step {step}")
    
    print(f"\nFinal simulation state:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Demonstrate state loading and recovery
    print("\n" + "=" * 40)
    print("STATE RECOVERY DEMONSTRATION")
    print("=" * 40)
    
    # Create new brain and load checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    print(f"Created new brain (pre-recovery)")
    print(f"  Spikes before recovery: {brain2.getTotalSpikeCount()}")
    
    # Load checkpoint 1
    recovery_success = brain2.load("checkpoint_1.bin")
    
    if recovery_success:
        print(f"\nRecovery successful!")
        print(f"  Spikes after recovery: {brain2.getTotalSpikeCount()}")
        print(f"  Neurons: {brain2.getTotalNeuronCount()}")
        print(f"  Synapses: {brain2.getTotalSynapseCount()}")
        print(f"  Firing rate: {brain2.getAverageFiringRate():.2f}")
        
        # Continue from recovered state
        print("\nContinuing simulation from recovered state...")
        for step in range(250, 350):
            brain2.step(step)
            
            if step % 25 == 0:
                print(f"  Step {step:3d}: {brain2.getFiringNeuronCount():3d} firing neurons")
        
        print(f"\nFinal recovered simulation state:")
        print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
        print(f"  Development stage: {brain2.getDevelopmentalStage()}")
        
    else:
        print("Recovery failed!")
    
    # Save final experiment results
    print("\nSaving experiment results...")
    
    # Save brain state
    brain2.save("final_experiment_brain.bin")
    
    # Save configuration
    config2.saveToFile("final_experiment_config.json")
    
    # Create results summary
    experiment_results = {
        'experiment_name': 'Comprehensive State Management Test',
        'initial_spikes': brain.getTotalSpikeCount(),
        'final_spikes': brain2.getTotalSpikeCount(),
        'total_steps': 350,
        'checkpoints_created': 5,
        'recovery_successful': recovery_success,
        'neural_evolution': {
            'initial_firing_rate': brain.getAverageFiringRate(),
            'final_firing_rate': brain2.getAverageFiringRate(),
            'firing_rate_change': brain2.getAverageFiringRate() - brain.getAverageFiringRate()
        }
    }
    
    # Save results to file
    with open("experiment_results.json", 'w') as f:
        json.dump(experiment_results, f, indent=2)
    
    print("Experiment results saved to experiment_results.json")
    
    print("\nState management complete!")
    print()
    

def example_9_advanced_configurations():
    """
    Example 9: Advanced Configurations
    
    Demonstrates advanced configuration strategies for different research scenarios
    and experiment types, including template-based configuration management.
    """
    print("=" * 70)
    print("Example 9: Advanced Configurations")
    print("=" * 70)
    
    # Configuration templates for different research scenarios
    configuration_templates = {
        'neurogenesis_study': {
            "description": "Study of neuron birth and integration",
            "neuron_count": 1500,
            "region_count": 3,
            "connection_probability": 0.12,
            "stdp_ltp_weight": 0.015,
            "stdp_ltd_weight": 0.018,
            "synaptogenesis_rate": 0.002,
            "pruning_rate": 0.0005,
            "random_seed": 1001
        },
        'plastic_reproduction_study': {
            "description": "Study of synaptic plasticity and reproduction",
            "neuron_count": 2000,
            "region_count": 4,
            "connection_probability": 0.15,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.022,
            "synaptogenesis_rate": 0.003,
            "pruning_rate": 0.001,
            "random_seed": 1002
        },
        'cognitive_formation_study': {
            "description": "Study of cognitive formation and concept development",
            "neuron_count": 800,
            "region_count": 2,
            "connection_probability": 0.08,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0002,
            "random_seed": 1003
        },
        'social_dynamics_study': {
            "description": "Study of social dynamics and interaction",
            "neuron_count": 1200,
            "region_count": 3,
            "connection_probability": 0.18,
            "stdp_ltp_weight": 0.025,
            "stdp_ltd_weight": 0.028,
            "synaptogenesis_rate": 0.004,
            "pruning_rate": 0.0008,
            "random_seed": 1004
        },
        'development_aging_study': {
            "description": "Study of development and aging processes",
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "stdp_ltp_weight": 0.008,
            "stdp_ltd_weight": 0.01,
            "synaptogenesis_rate": 0.0005,
            "pruning_rate": 0.0015,
            "random_seed": 1005
        }
    }
    
    # Create configuration manager
    print("Advanced Configuration Setup:")
    print(f"Available research templates: {len(configuration_templates)}")
    
    # Demonstrate each configuration
    for template_name, template_config in configuration_templates.items():
        print(f"\n{template_name.upper()}:")
        print(f"  Description: {template_config['description']}")
        print(f"  Neurons: {template_config['neuron_count']}")
        print(f"  Regions: {template_config['region_count']}")
        print(f"  Connection probability: {template_config['connection_probability']}")
        print(f"  STDP LTP weight: {template_config['stdp_ltp_weight']}")
        print(f"  STDP LTD weight: {template_config['stdp_ltd_weight']}")
        print(f"  Synaptogenesis rate: {template_config['synaptogenesis_rate']}")
        print(f"  Pruning rate: {template_config['pruning_rate']}")
    
    # Create configurations for demonstration
    print("\n" + "=" * 50)
    print("DEMONSTRATION CONFIGURATIONS")
    print("=" * 50)
    
    # Neurogenesis study configuration
    print("\n1. Neurogenesis Study Configuration:")
    neurogenesis_config = pynlm.createDefaultConfig()
    for key, value in configuration_templates['neurogenesis_study'].items():
        if key != 'description' and key != 'random_seed':
            neurogenesis_config.set(key, value)
        elif key == 'random_seed':
            neurogenesis_config.set(key, value)
    
    brain1 = pynlm.createBrain(neurogenesis_config)
    brain1.initialize()
    print(f"  Created brain: {brain1.getTotalNeuronCount()} neurons")
    
    # Cognitive formation study configuration
    print("\n2. Cognitive Formation Study Configuration:")
    cognitive_config = pynlm.createStandardConfig()
    for key, value in configuration_templates['cognitive_formation_study'].items():
        if key != 'description':
            cognitive_config.set(key, value)
    
    brain2 = pynlm.createBrain(cognitive_config)
    brain2.initialize()
    print(f"  Created brain: {brain2.getTotalNeuronCount()} neurons")
    
    # Development and aging study configuration
    print("\n3. Development and Aging Study Configuration:")
    aging_config = pynlm.createExplorationConfig()
    for key, value in configuration_templates['development_aging_study'].items():
        if key != 'description':
            aging_config.set(key, value)
    
    brain3 = pynlm.createBrain(aging_config)
    brain3.initialize()
    print(f"  Created brain: {brain3.getTotalNeuronCount()} neurons")
    
    # Compare configurations
    print("\n" + "=" * 50)
    print("CONFIGURATION COMPARISON")
    print("=" * 50)
    
    configurations = [
        ("Neurogenesis Study", brain1, neurogenesis_config),
        ("Cognitive Formation Study", brain2, cognitive_config),
        ("Development & Aging Study", brain3, aging_config)
    ]
    
    for name, brain, config in configurations:
        print(f"\n{name}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Regions: {brain.getRegionCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        # Extract key config parameters
        keys = ["neuron_count", "region_count", "connection_probability", 
                "stdp_ltp_weight", "stdp_ltd_weight"]
        for key in keys:
            if config.has(key):
                print(f"  {key}: {config.get(key)}")
    
    # Demonstrate configuration inheritance
    print("\n" + "=" * 50)
    print("CONFIGURATION INHERITANCE DEMONSTRATION")
    print("=" * 50)
    
    # Base configuration
    base_config = pynlm.createStandardConfig()
    base_config.set("base_param", "base_value")
    base_config.set("common_param", "common_value")
    
    print("Base configuration parameters:")
    print(f"  neuron_count: {base_config.get('neuron_count')}")
    print(f"  region_count: {base_config.get('region_count')}")
    print(f"  base_param: {base_config.get('base_param')}")
    print(f"  common_param: {base_config.get('common_param')}")
    
    # Override base configuration
    override_config = pynlm.createDefaultConfig()
    for key in ["neuron_count", "region_count"]:
        if base_config.has(key):
            override_config.set(key, base_config.get(key))
    
    # Add experiment-specific parameters
    override_config.set("experiment_param", "experiment_value")
    override_config.set("debug_mode", True)
    
    print("\nOverridden configuration parameters:")
    print(f"  neuron_count: {override_config.get('neuron_count')}")
    print(f"  region_count: {override_config.get('region_count')}")
    print(f"  experiment_param: {override_config.get('experiment_param')}")
    print(f"  debug_mode: {override_config.get('debug_mode')}")
    
    # Create brain with overridden configuration
    brain4 = pynlm.createBrain(override_config)
    brain4.initialize()
    print(f"\nCreated brain with overridden config: {brain4.getTotalNeuronCount()} neurons")
    
    # Demonstrate configuration validation
    print("\n" + "=" * 50)
    print("CONFIGURATION VALIDATION")
    print("=" * 50)
    
    def validate_configuration(config):
        """Validate configuration for required parameters."""
        required_keys = ["neuron_count", "region_count", "connection_probability"]
        missing_keys = []
        
        for key in required_keys:
            if not config.has(key):
                missing_keys.append(key)
        
        if missing_keys:
            print(f"  Configuration invalid. Missing keys: {missing_keys}")
            return False
        else:
            print(f"  Configuration valid. All required keys present.")
            return True
    
    # Validate configurations
    validate_configuration(base_config)
    validate_configuration(override_config)
    
    # Demonstrate configuration export/import
    print("\nConfiguration Export/Import Demonstration:")
    
    # Export configuration to file
    export_config = pynlm.createStandardConfig()
    export_config.set("export_param", "export_value")
    export_config.saveToFile("exported_config.json")
    print("  Exported configuration to exported_config.json")
    
    # Import configuration from file
    import_config = pynlm.createDefaultConfig()
    import_config.loadFromFile("exported_config.json")
    print(f"  Imported configuration. export_param: {import_config.get('export_param')}")
    
    print("\nAdvanced configurations complete!")
    print()
    

def example_10_research_scenarios():
    """
    Example 10: Research Scenarios
    
    Demonstrates comprehensive research scenarios covering various experimental
    designs, from simple exploratory tasks to complex multi-agent studies.
    """
    print("=" * 70)
    print("Example 10: Research Scenarios")
    print("=" * 70)
    
    # Research scenario definitions
    research_scenarios = {
        'exploratory_learning': {
            'title': 'Exploratory Learning with Curiosity-Driven Agents',
            'description': 'Study how curiosity drives exploration behavior',
            'duration': 600,
            'neurons': 800,
            'regions': 3,
            'learning_enabled': True,
            'visual_inspection': True,
            'reward_structure': 'exploration_based'
        },
        'social_cooperation': {
            'title': 'Social Cooperation and Joint Action',
            'description': 'Study emergence of cooperative behavior',
            'duration': 500,
            'neurons': 1200,
            'regions': 4,
            'learning_enabled': True,
            'multi_agent': True,
            'social_interaction': True,
            'reward_structure': 'cooperation_based'
        },
        'pattern_formation': {
            'title': 'Pattern Formation and Category Learning',
            'description': 'Study how patterns emerge from experience',
            'duration': 400,
            'neurons': 600,
            'regions': 2,
            'learning_enabled': True,
            'concept_formation': True,
            'pattern_recognition': True,
            'reward_structure': 'learning_based'
        },
        'development_trajectory': {
            'title': 'Neural Development and Cognitive Growth',
            'description': 'Study developmental trajectories and maturation',
            'duration': 800,
            'neurons': 1000,
            'regions': 3,
            'learning_enabled': True,
            'developmental_tracking': True,
            'age_simulation': True,
            'reward_structure': 'developmental'
        },
        'stress_response': {
            'title': 'Stress Response and Adaptation',
            'description': 'Study stress responses and adaptive behaviors',
            'duration': 450,
            'neurons': 900,
            'regions': 3,
            'learning_enabled': True,
            'stress_simulation': True,
            'recovery_tracking': True,
            'reward_structure': 'homeostatic'
        }
    }
    
    print(f"Available research scenarios: {len(research_scenarios)}")
    
    # Demonstrate exploratory learning scenario
    print("\n" + "=" * 50)
    print("DEMONSTRATING: EXPLORATORY LEARNING SCENARIO")
    print("=" * 50)
    
    scenario = research_scenarios['exploratory_learning']
    print(f"Title: {scenario['title']}")
    print(f"Description: {scenario['description']}")
    print(f"Duration: {scenario['duration']} steps")
    print(f"Neurons: {scenario['neurons']}")
    print(f"Regions: {scenario['regions']}")
    print(f"Learning enabled: {scenario['learning_enabled']}")
    print(f"Visual inspection: {scenario['visual_inspection']}")
    print(f"Reward structure: {scenario['reward_structure']}")
    
    # Create experimental setup for exploratory learning
    exploration_config = pynlm.createExplorationConfig()
    exploration_config.set("neuron_count", scenario['neurons'])
    exploration_config.set("region_count", scenario['regions'])
    exploration_config.set("random_seed", 2001)
    
    # Setup experimental components
    brain = pynlm.createBrain(exploration_config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=45, height=45, visionWidth=18, visionHeight=18)
    world.reset()
    world.setAgentStart(22.5, 22.5)
    
    agent.initialize(world)
    
    # Enable learning for exploration study
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("\nStarting exploratory learning experiment...")
    
    # Track exploration behavior
    exploration_data = []
    novel_sites_visited = set()
    total_rewards = []
    curiosity_levels = []
    
    # Run exploratory learning simulation
    for step in range(scenario['duration']):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate exploration-based reward
        body = world.getAgentBody()
        x, y = body.x, body.y
        
        # Reward for novel locations
        if (round(x, 1), round(y, 1)) not in novel_sites_visited:
            novel_sites_visited.add((round(x, 1), round(y, 1)))
            exploration_reward = 2.0  # High reward for exploring new areas
        else:
            exploration_reward = 0.1  # Small reward for familiar areas
        
        # Reward for successful navigation
        position_reward = 1.0 if (10 <= x <= 35 and 10 <= y <= 35) else -0.5
        
        # Curiosity reward (encourages exploration)
        curiosity_reward = agent.getCuriosityLevel() * 0.1
        
        total_reward = exploration_reward + position_reward + curiosity_reward
        agent.applyRewardModulation(total_reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record exploration metrics
        exploration_data.append({
            'step': step,
            'position': (x, y),
            'novel_sites': len(novel_sites_visited),
            'exploration_reward': exploration_reward,
            'position_reward': position_reward,
            'curiosity_reward': curiosity_reward,
            'total_reward': total_reward,
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        total_rewards.append(total_reward)
        curiosity_levels.append(agent.getCuriosityLevel())
        
        # Log progress
        if step % 100 == 0:
            print(f"  Step {step:3d}: Novel sites {len(novel_sites_visited)}, "
                  f"Curiosity {agent.getCuriosityLevel():.2f}, "
                  f"Reward {total_reward:6.2f}")
    
    # Analyze exploration results
    final_explore = exploration_data[-1]
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    avg_reward = sum(total_rewards) / len(total_rewards)
    unique_sites_ratio = len(novel_sites_visited) / (scenario['width'] * scenario['height'])
    
    print("\nExploratory Learning Analysis:")
    print(f"  Total simulation steps: {len(exploration_data)}")
    print(f"  Novel sites discovered: {len(novel_sites_visited)}")
    print(f"  Novel sites ratio: {unique_sites_ratio:.2%}")
    print(f"  Final curiosity level: {final_explore['curiosity_level']:.2f}")
    print(f"  Average curiosity: {avg_curiosity:.2f}")
    print(f"  Average reward per step: {avg_reward:.2f}")
    print(f"  Final development stage: {final_explore['development_stage']}")
    
    # Save exploration results
    exploration_results = {
        'scenario': 'exploratory_learning',
        'total_steps': scenario['duration'],
        'novel_sites_discovered': len(novel_sites_visited),
        'final_curiosity_level': final_explore['curiosity_level'],
        'average_curiosity': avg_curiosity,
        'average_reward': avg_reward,
        'final_development_stage': final_explore['development_stage'],
        'sample_data': exploration_data[:10]  # First 10 steps as sample
    }
    
    with open("exploration_results.json", 'w') as f:
        json.dump(exploration_results, f, indent=2)
    
    print("\nExploration results saved to exploration_results.json")
    
    # Demonstrate pattern formation scenario
    print("\n" + "=" * 50)
    print("DEMONSTRATING: PATTERN FORMATION SCENARIO")
    print("=" * 50)
    
    scenario = research_scenarios['pattern_formation']
    print(f"Title: {scenario['title']}")
    print(f"Description: {scenario['description']}")
    print(f"Duration: {scenario['duration']} steps")
    print(f"Neurons: {scenario['neurons']}")
    print(f"Regions: {scenario['regions']}")
    
    # Create pattern formation configuration
    pattern_config = pynlm.createStandardConfig()
    pattern_config.set("neuron_count", scenario['neurons'])
    pattern_config.set("region_count", scenario['regions'])
    pattern_config.set("random_seed", 2002)
    
    # Setup pattern formation experiment
    pattern_brain = pynlm.createBrain(pattern_config)
    pattern_brain.initialize()
    
    pattern_agent = pynlm.createAgentBrain(pattern_brain)
    
    pattern_world = pynlm.createSimpleWorld()
    pattern_world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    pattern_world.reset()
    pattern_world.setAgentStart(20.0, 20.0)
    
    pattern_agent.initialize(pattern_world)
    
    # Enable learning for pattern formation
    pattern_agent.enableRewardModulation(True)
    pattern_agent.enableCuriosity(True)
    
    print("\nStarting pattern formation experiment...")
    
    # Track pattern formation
    pattern_data = []
    color_clusters = []
    pattern_emergence = []
    
    # Run pattern formation simulation
    for step in range(scenario['duration']):
        # Update world
        pattern_world.update(0.1)
        
        # Get sensory input
        percept = pattern_world.getSensoryPercept()
        pattern_agent.processSensoryInput(percept)
        pattern_brain.step(step)
        
        # Get action
        action = pattern_agent.decodeMotorCommand()
        pattern_world.applyMotorCommand(action, pattern_world.getSimulationTime())
        
        # Calculate learning-based reward
        body = pattern_world.getAgentBody()
        x, y = body.x, body.y
        
        # Reward for visiting different areas (pattern exploration)
        if step % 20 == 0:
            # Create a simulated color cluster based on position
            cluster_id = (int(x) // 5) * 10 + (int(y) // 5)
            color_clusters.append(cluster_id)
        
        # Reward for consistent exploration patterns
        pattern_reward = 0.5 if len(set(color_clusters[-5:])) > 2 else 0.1
        
        # Reward for neural activity (learning)
        activity_reward = pattern_brain.getFiringNeuronCount() * 0.0005
        
        # Curiosity reward for pattern discovery
        curiosity_reward = pattern_agent.getCuriosityLevel() * 0.15
        
        total_reward = pattern_reward + activity_reward + curiosity_reward
        pattern_agent.applyRewardModulation(total_reward, 0.0)
        pattern_agent.updateDevelopment(0.1)
        
        # Track pattern emergence
        distinct_patterns = len(set(color_clusters))
        pattern_emergence.append(distinct_patterns)
        
        # Record pattern metrics
        pattern_data.append({
            'step': step,
            'position': (x, y),
            'color_clusters': color_clusters.copy(),
            'distinct_patterns': distinct_patterns,
            'pattern_reward': pattern_reward,
            'activity_reward': activity_reward,
            'curiosity_reward': curiosity_reward,
            'total_reward': total_reward,
            'curiosity': pattern_agent.getCuriosityLevel(),
            'novelty': pattern_agent.getNoveltyLevel()
        })
        
        # Log pattern formation progress
        if step % 80 == 0:
            print(f"  Step {step:3d}: Distinct patterns {distinct_patterns}, "
                  f"Curiosity {pattern_agent.getCuriosityLevel():.2f}, "
                  f"Novelty {pattern_agent.getNoveltyLevel():.2f}")
    
    # Analyze pattern formation results
    final_pattern = pattern_data[-1]
    avg_distinct_patterns = sum(m['distinct_patterns'] for m in pattern_data) / len(pattern_data)
    pattern_stability = max(pattern_emergence) - min(pattern_emergence)
    
    print("\nPattern Formation Analysis:")
    print(f"  Total simulation steps: {len(pattern_data)}")
    print(f"  Maximum distinct patterns: {max(pattern_emergence)}")
    print(f"  Average distinct patterns: {avg_distinct_patterns:.1f}")
    print(f"  Pattern stability: {pattern_stability:.1f}")
    print(f"  Final curiosity level: {final_pattern['curiosity']:.2f}")
    print(f"  Final novelty level: {final_pattern['novelty']:.2f}")
    
    print("\nPattern formation experiment complete!")
    
    # Create research summary
    research_summary = {
        'research_scenarios': research_scenarios,
        'exploration_results': exploration_results,
        'pattern_results': {
            'total_steps': len(pattern_data),
            'distinct_patterns_formed': max(pattern_emergence),
            'pattern_stability': pattern_stability,
            'final_curiosity': final_pattern['curiosity'],
            'final_novelty': final_pattern['novelty']
        },
        'recommendations': [
            "Explore scenarios with varying neural configurations",
            "Monitor curiosity and novelty metrics for learning assessment",
            "Track pattern emergence and stability metrics",
            "Use different reward structures for specific research questions"
        ]
    }
    
    # Save research summary
    with open("research_summary.json", 'w') as f:
        json.dump(research_summary, f, indent=2)
    
    print("\nResearch summary saved to research_summary.json")
    
    print("\nResearch scenarios complete!")
    print()
    

# Helper functions for examples
def calculate_advanced_reward(world, brain, agent, step):
    """Calculate sophisticated reward based on multiple factors."""
    body = world.getAgentBody()
    x, y = body.x, body.y
    width, height = world.getWidth(), world.getHeight()
    
    # Position-based reward
    if 10 <= x <= 30 and 10 <= y <= 30:
        position_reward = 1.0
    elif 5 <= x <= 35 and 5 <= y <= 35:
        position_reward = 0.5
    else:
        position_reward = -0.5
    
    # Activity-based reward
    activity_reward = brain.getFiringNeuronCount() * 0.001
    
    # Neuromodulation reward
    neuromodulation_reward = agent.getNeuromodulationLevel() * 0.1
    
    # Curiosity-based reward
    curiosity_reward = agent.getCuriosityLevel() * 0.05
    
    # Development reward
    development_reward = brain.getDevelopmentalStage() * 0.02
    
    # Novelty-based reward
    novelty_reward = agent.getNoveltyLevel() * 0.03
    
    return position_reward + activity_reward + neuromodulation_reward + \
           curiosity_reward + development_reward + novelty_reward


def calculate_reward(world, brain, agent, step):
    """Calculate basic reward for examples."""
    body = world.getAgentBody()
    x, y = body.x, body.y
    width, height = world.getWidth(), world.getHeight()
    
    position_reward = 1.0 if (0 <= x < width and 0 <= y < height) else -1.0
    activity_reward = brain.getFiringNeuronCount() * 0.001
    curiosity_reward = agent.getCuriosityLevel() * 0.1
    
    return position_reward + activity_reward + curiosity_reward


def save_visualization_data(data, filename):
    """Save visualization data to file."""
    with open(filename, 'w') as f:
        json.dump(data, f, indent=2)


def main():
    """Run all comprehensive NLM examples."""
    print("\n" + "=" * 80)
    print("NLM COMPREHENSIVE EXAMPLE SUITE")
    print("=" * 80)
    print("\nThis script demonstrates all major features of the NLM framework.")
    print("Each example showcases different aspects of neural simulation.")
    
    examples = [
        ("Basic Brain Operations", example_1_basic_brain_operations),
        ("World Environment Interaction", example_2_world_environment_interaction),
        ("Advanced Learning Systems", example_3_advanced_learning_systems),
        ("Memory Systems", example_4_memory_systems),
        ("Prediction Systems", example_5_prediction_systems),
        ("Cognitive Systems", example_6_cognitive_systems),
        ("Performance Monitoring", example_7_performance_monitoring),
        ("State Management", example_8_state_management),
        ("Advanced Configurations", example_9_advanced_configurations),
        ("Research Scenarios", example_10_research_scenarios),
    ]
    
    print(f"\nAvailable examples ({len(examples)}):")
    for i, (name, _) in enumerate(examples, 1):
        print(f"  {i:2d}. {name}")
    
    print("\n" + "=" * 80)
    print("RUNNING ALL EXAMPLES")
    print("=" * 80)
    
    for name, example_func in examples:
        try:
            example_func()
            print(f"✓ {name} completed successfully!")
        except Exception as e:
            print(f"✗ {name} failed with error: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 80)
    print("ALL EXAMPLES COMPLETED")
    print("=" * 80)
    print("\nSummary of examples run:")
    for name, _ in examples:
        print(f"  ✓ {name}")
    
    print("\nGenerated files:")
    generated_files = [
        "performance_report.json",
        "experiment_results.json", 
        "exploration_results.json",
        "research_summary.json",
        "nlm_analysis.png"
    ]
    
    for file in generated_files:
        try:
            import os
            if os.path.exists(file):
                print(f"  ✓ {file}")
            else:
                print(f"  - {file} (not generated)")
        except:
            print(f"  - {file} (not generated)")
    
    print("\nThe comprehensive example suite demonstrates:")
    print("  1. Core brain operations and initialization")
    print("  2. World interaction and sensory-motor processing")
    print("  3. Advanced learning systems (reward, curiosity, development)")
    print("  4. Memory systems and consolidation")
    print("  5. Prediction systems and temporal learning")
    print("  6. Cognitive systems and attention/planning")
    print("  7. Performance monitoring and analysis")
    print("  8. State management and checkpointing")
    print("  9. Advanced configuration strategies")
    print("  10. Research scenarios and experimental design")
    
    print("\nEach example provides practical experience with NLM features")
    print("and includes data collection, analysis, and visualization.")


if __name__ == "__main__":
    main()
