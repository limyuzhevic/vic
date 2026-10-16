#!/usr/bin/env python3
"""
NLM Advanced Usage Examples
===========================

This file contains advanced examples demonstrating NLM (Neural Learning Machine)
capabilities including complex simulations, learning patterns, and real-world scenarios.

Examples covered:
1. Complex Brain Architecture
2. Learning Pattern Recognition
3. Multi-Environment Adaptation
4. Memory and Learning Integration
5. Performance Optimization
"""

import pynlm
import time
import sys
from typing import List, Tuple

def create_complex_brain(neuron_count: int = 2000, region_count: int = 4) -> pynlm.Brain:
    """
    Create a complex brain architecture with multiple regions
    """
    config = pynlm.createDefaultConfig()
    
    # Configure complex architecture
    config.set("brain.neuron_count", neuron_count, pynlm.ConfigSource.CommandLine)
    config.set("brain.region_count", region_count, pynlm.ConfigSource.CommandLine)
    config.set("brain.synapse_density", 0.15, pynlm.ConfigSource.CommandLine)
    config.set("brain.initial_weight_mean", 0.3, pynlm.ConfigSource.CommandLine)
    config.set("brain.initial_weight_std", 0.1, pynlm.ConfigSource.CommandLine)
    
    # Enable all plasticity types
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.stdp.ltp_weight", 0.015, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.stdp.ltd_weight", 0.018, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.stdp.tau", 25.0, pynlm.ConfigSource.CommandLine)
    
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.hebbian.learning_rate", 0.005, pynlm.ConfigSource.CommandLine)
    
    config.set("plasticity.structural.enable", True, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.structural.synaptogenesis_rate", 0.00005, pynlm.ConfigSource.CommandLine)
    config.set("plasticity.structural.pruning_rate", 0.00005, pynlm.ConfigSource.CommandLine)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.5, pynlm.ConfigSource.CommandLine)
    config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.CommandLine)
    config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.CommandLine)
    config.set("neuromod.prediction_error.enable", True, pynlm.ConfigSource.CommandLine)
    
    # Development settings
    config.set("development.synaptogenesis_rate", 0.001, pynlm.ConfigSource.CommandLine)
    config.set("development.pruning_rate", 0.0001, pynlm.ConfigSource.CommandLine)
    
    # Memory settings
    config.set("memory.working_capacity", neuron_count // 10, pynlm.ConfigSource.CommandLine)
    config.set("memory.episodic.max_episodes", 2000, pynlm.ConfigSource.CommandLine)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created complex brain with {neuron_count} neurons in {region_count} regions")
    print(f"  STDP enabled: {brain.getSTDP() is not None}")
    print(f"  Hebbian enabled: {brain.getHebbian() is not None}")
    print(f"  Structural plasticity enabled: {brain.getStructuralPlasticity() is not None}")
    
    return brain

def simulate_learning_pattern(brain: pynlm.Brain, steps: int = 1000) -> dict:
    """
    Simulate learning through repeated exposure to patterns
    """
    print(f"\n=== Learning Pattern Simulation ({steps} steps) ===")
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neuron_count = brain.getTotalNeuronCount()
    
    # Pattern injection: periodic sensory input to stimulate learning
    print("Injecting learning patterns...")
    
    for step in range(steps):
        # Create learning pattern: different sensory inputs based on step
        pattern = step % 10
        
        # Inject current based on pattern
        if pattern == 0:
            # Strong excitatory input to all sensory neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0)
        elif pattern == 1:
            # Random balanced input
            import random
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 
                                         random.uniform(20.0, 60.0))
        elif pattern == 2:
            # Inhibitory input to some neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, -20.0)
        
        # Run brain step
        brain.step(step)
        
        # Log progress every 200 steps
        if (step + 1) % 200 == 0:
            spikes = brain.getTotalSpikeCount() - initial_spikes
            rate = (float)spikes / (step + 1)
            print(f"  Step {step + 1}: {spikes} new spikes, {rate:.2f} Hz")
    
    # Record final state
    final_spikes = brain.getTotalSpikeCount()
    total_spikes = final_spikes - initial_spikes
    avg_rate = (float)total_spikes / steps
    
    # Get plasticity statistics
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    
    print(f"\nLearning Results:")
    print(f"  Total spikes generated: {total_spikes}")
    print(f"  Average firing rate: {avg_rate:.2f} Hz")
    print(f"  Final E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Get synaptic weight statistics
    total_synapses = brain.getTotalSynapseCount()
    print(f"  Total synapses: {total_synapses}")
    
    # Sample some synapse weights to show learning
    sample_weights = []
    for i in range(min(10, total_synapses)):
        # Note: Direct access to synapse weights would require more API support
        pass
    
    return {
        'initial_spikes': initial_spikes,
        'final_spikes': final_spikes,
        'total_spikes': total_spikes,
        'average_rate': avg_rate,
        'steps': steps
    }

def multi_environment_adaptation():
    """
    Demonstrate brain adaptation across different environments
    """
    print("\n=== Multi-Environment Adaptation ===")
    
    # Create brain with agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create different environments
    environment_configs = [
        {"name": "SimpleGrid", "width": 10, "height": 10},
        {"name": "LargeArena", "width": 20, "height": 20},
        {"name": "ComplexMaze", "width": 15, "height": 15, "walls": True}
    ]
    
    environments = []
    for env_config in environment_configs:
        world = pynlm.createSimpleWorld()
        world.configure(width=env_config["width"], height=env_config["height"])
        
        if env_config.get("walls"):
            # Add walls to maze
            for x in range(5, 10):
                world.addObject(pynlm.WorldObject(x, 5, pynlm.WorldObjectType.Wall))
        
        world.reset()
        world.setAgentStart(env_config["width"] // 2, env_config["height"] // 2)
        
        agent.initialize(world)
        
        # Enable different learning strategies per environment
        if env_config["name"] == "SimpleGrid":
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
        elif env_config["name"] == "LargeArena":
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        elif env_config["name"] == "ComplexMaze":
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
        
        environments.append((env_config["name"], world, agent))
        print(f"Created environment: {env_config['name']} ({env_config['width']}x{env_config['height']})")
    
    # Run through each environment
    for name, world, env_agent in environments:
        print(f"\nAdapting to {name}...")
        
        # Reset and reconfigure agent for new environment
        env_agent.reset()
        env_agent.initialize(world)
        
        # Run simulation
        steps_per_env = 100
        for step in range(steps_per_env):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            env_agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = env_agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning based on environment
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            env_agent.applyRewardModulation(reward, 0.0)
        
        # Record adaptation metrics
        curiosity = env_agent.getCuriosityLevel()
        novelty = env_agent.getNoveltyLevel()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"  After adaptation to {name}:")
        print(f"    Curiosity level: {curiosity:.2f}")
        print(f"    Novelty level: {novelty:.2f}")
        print(f"    Firing rate: {firing_rate:.2f} Hz")
    
    print(f"\nMulti-environment adaptation completed!")

def memory_and_learning_integration(brain: pynlm.Brain, steps: int = 500):
    """
    Demonstrate integration of memory systems with learning
    """
    print(f"\n=== Memory and Learning Integration ({steps} steps) ===")
    
    # Create agent with memory capabilities
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(6.0, 6.0)
    
    agent.initialize(world)
    
    # Enable all memory and learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Memory and learning features enabled:")
    print("  Reward Modulation: Learning from outcomes")
    print("  Structural Plasticity: Growing new connections")
    print("  Development: Brain maturation over time")
    print("  Curiosity: Exploration and novelty detection")
    
    # Track memory metrics
    memory_metrics = {
        'working_memory_size': 0,
        'episodic_episodes': 0,
        'spike_patterns': [],
        'learning_curves': []
    }
    
    # Run simulation with focus on memory integration
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input (stores in working memory)
        agent.processSensoryInput(percept)
        
        # Brain processes (forms episodic memories)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (reinforces memory)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (affects memory consolidation)
        agent.updateDevelopment(0.1)
        
        # Record metrics periodically
        if step % 100 == 0:
            working_memory = brain.getWorkingMemory()
            if working_memory:
                memory_metrics['working_memory_size'] = working_memory.getActiveTraces()
            
            episodic_memory = brain.getEpisodicMemory()
            if episodic_memory:
                memory_metrics['episodic_episodes'] = episodic_memory.getEpisodeCount()
            
            firing_rate = brain.getAverageFiringRate()
            memory_metrics['spike_patterns'].append(firing_rate)
            memory_metrics['learning_curves'].append(agent.getCuriosityLevel())
            
            print(f"  Step {step}: WM={memory_metrics['working_memory_size']} traces, "
                  f"Episodes={memory_metrics['episodic_episodes']}, "
                  f"Firing={firing_rate:.2f} Hz, Curiosity={agent.getCuriosityLevel():.2f}")
    
    # Analyze memory integration results
    print(f"\nMemory Integration Analysis:")
    print(f"  Final working memory size: {memory_metrics['working_memory_size']}")
    print(f"  Final episodic memory episodes: {memory_metrics['episodic_episodes']}")
    
    if memory_metrics['spike_patterns']:
        avg_firing_rate = sum(memory_metrics['spike_patterns']) / len(memory_metrics['spike_patterns'])
        avg_curiosity = sum(memory_metrics['learning_curves']) / len(memory_metrics['learning_curves'])
        
        print(f"  Average firing rate during observation: {avg_firing_rate:.2f} Hz")
        print(f"  Average curiosity level: {avg_curiosity:.2f}")
        
        # Check for learning trends
        if len(memory_metrics['learning_curves']) >= 3:
            initial_curiosity = memory_metrics['learning_curves'][0]
            final_curiosity = memory_metrics['learning_curves'][-1]
            curiosity_change = final_curiosity - initial_curiosity
            
            if abs(curiosity_change) > 0.1:
                print(f"  Learning detected: Curiosity changed by {curiosity_change:.2f}")
            else:
                print(f"  Curiosity stabilized: Change of {curiosity_change:.2f}")
    
    return memory_metrics

def performance_optimization(brain: pynlm.Brain, target_steps: int = 1000):
    """
    Demonstrate performance optimization techniques
    """
    print(f"\n=== Performance Optimization ({target_steps} steps) ===")
    
    # Get baseline performance
    print("Running baseline simulation...")
    start_time = time.time()
    
    baseline_spikes = brain.getTotalSpikeCount()
    
    for step in range(target_steps):
        brain.step(step)
    
    baseline_time = time.time() - start_time
    baseline_rate = target_steps / baseline_time
    
    print(f"Baseline performance:")
    print(f"  Time: {baseline_time:.2f} seconds")
    print(f"  Steps/sec: {baseline_rate:.1f}")
    print(f"  Spikes generated: {brain.getTotalSpikeCount() - baseline_spikes}")
    
    # Reset for comparison
    brain.reset()
    brain.initialize()
    
    # Simulate with optimization suggestions
    print("\nSimulating with optimization practices...")
    start_time = time.time()
    
    optimization_spikes = brain.getTotalSpikeCount()
    
    # Use optimized stepping patterns
    for step in range(target_steps):
        # Group similar operations
        if step % 100 == 0:
            # Batch processing for periodic operations
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
        
        # Main computation
        brain.step(step)
        
        # Periodic cleanup and statistics
        if step % 250 == 0:
            # Optional: checkpoint management
            pass
    
    optimization_time = time.time() - start_time
    optimization_rate = target_steps / optimization_time
    
    print(f"Optimized performance:")
    print(f"  Time: {optimization_time:.2f} seconds")
    print(f"  Steps/sec: {optimization_rate:.1f}")
    print(f"  Spikes generated: {brain.getTotalSpikeCount() - optimization_spikes}")
    
    # Performance analysis
    speedup = baseline_rate / optimization_rate
    time_improvement = (baseline_time - optimization_time) / baseline_time * 100
    
    print(f"\nPerformance Analysis:")
    print(f"  Speedup factor: {speedup:.2f}x")
    print(f"  Time improvement: {time_improvement:.1f}%")
    print(f"  Efficiency: {optimization_rate / baseline_rate * 100:.1f}% of baseline")
    
    return {
        'baseline_time': baseline_time,
        'optimized_time': optimization_time,
        'speedup': speedup,
        'improvement_percent': time_improvement
    }

def advanced_api_usage():
    """
    Demonstrate advanced API usage patterns
    """
    print("\n=== Advanced API Usage Patterns ===")
    
    # Create sophisticated configuration
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration patterns
    config.set("brain.neuron_count", 3000, pynlm.ConfigSource.CommandLine)
    config.set("brain.synapse_density", 0.12, pynlm.ConfigSource.CommandLine)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced features
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=18, height=18, visionWidth=9, visionHeight=9)
    world.reset()
    world.setAgentStart(9.0, 9.0)
    
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced features enabled:")
    print("  Reward Prediction Error (RPE) learning")
    print("  Structural plasticity (synaptogenesis)")
    print("  Developmental stages")
    print("  Curiosity-driven exploration")
    print("  Memory consolidation")
    
    # Advanced simulation with custom learning rules
    print("\nRunning advanced simulation...")
    
    # Track various metrics
    metrics = {
        'time_series': [],
        'curiosity_trends': [],
        'novelty_patterns': [],
        'development_stages': []
    }
    
    for step in range(300):
        # Advanced sensory processing
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Custom learning schedule
        if step % 3 == 0:
            # Schedule periodic learning events
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            # Predict reward for curiosity-based exploration
            predicted_reward = agent.getCuriosityLevel() * 0.5
            agent.applyRewardModulation(reward, predicted_reward)
        
        # Brain processing
        brain.step(step)
        
        # Action with curiosity modulation
        action = agent.decodeMotorCommand()
        
        # Apply action with developmental timing
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Development updates at specific times
        if step % 50 == 0:
            agent.updateDevelopment(0.1)
            dev_stage = brain.getDevelopmentalStage()
            metrics['development_stages'].append(str(dev_stage))
        
        # Record metrics
        metrics['curiosity_trends'].append(agent.getCuriosityLevel())
        metrics['novelty_patterns'].append(agent.getNoveltyLevel())
        
        # Print progress
        if step % 60 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}")
    
    # Analyze advanced metrics
    print(f"\nAdvanced Usage Analysis:")
    print(f"  Total simulation time: {len(metrics['time_series'])} steps")
    
    if metrics['curiosity_trends']:
        avg_curiosity = sum(metrics['curiosity_trends']) / len(metrics['curiosity_trends'])
        curiosity_variance = sum((x - avg_curiosity) ** 2 for x in metrics['curiosity_trends']) / len(metrics['curiosity_trends'])
        
        print(f"  Average curiosity: {avg_curiosity:.2f}")
        print(f"  Curiosity variance: {curiosity_variance:.3f}")
        
        # Detect patterns in curiosity
        if len(metrics['curiosity_trends']) >= 10:
            recent_curiosity = metrics['curiosity_trends'][-10:]
            if all(x < recent_curiosity[0] * 0.5 for x in recent_curiosity[1:]):
                print(f"  Curiosity pattern: Declining exploration (stable environment)")
            elif all(x > recent_curiosity[0] * 1.5 for x in recent_curiosity[1:]):
                print(f"  Curiosity pattern: Increasing exploration (novel environment)")
            else:
                print(f"  Curiosity pattern: Dynamic exploration")
    
    print(f"  Development stages experienced: {len(set(metrics['development_stages']))}")
    
    return metrics

def main():
    """Run all advanced examples"""
    print("NLM Advanced Usage Examples")
    print("=" * 50)
    print()
    
    # Check if pynlm is available
    try:
        version = getattr(pynlm, '__version__', 'unknown')
        print(f"NLM Python bindings version: {version}")
    except AttributeError:
        print("ERROR: pynlm module not properly loaded")
        sys.exit(1)
    
    print()
    print("Running advanced examples...")
    print()
    
    # Example 1: Complex Brain Architecture
    print("EXAMPLE 1: Complex Brain Architecture")
    print("-" * 40)
    complex_brain = create_complex_brain(neuron_count=2000, region_count=4)
    complex_brain_results = simulate_learning_pattern(complex_brain, steps=500)
    print()
    
    # Example 2: Multi-Environment Adaptation
    print("EXAMPLE 2: Multi-Environment Adaptation")
    print("-" * 40)
    multi_environment_adaptation()
    print()
    
    # Example 3: Memory and Learning Integration
    print("EXAMPLE 3: Memory and Learning Integration")
    print("-" * 40)
    memory_results = memory_and_learning_integration(complex_brain, steps=400)
    print()
    
    # Example 4: Performance Optimization
    print("EXAMPLE 4: Performance Optimization")
    print("-" * 40)
    perf_results = performance_optimization(complex_brain, target_steps=800)
    print()
    
    # Example 5: Advanced API Usage
    print("EXAMPLE 5: Advanced API Usage")
    print("-" * 40)
    advanced_results = advanced_api_usage()
    print()
    
    print("=" * 50)
    print("All advanced examples completed successfully!")
    print()
    print("Key takeaways:")
    print("1. Complex brains can be created with multiple regions and specialized connections")
    print("2. Learning patterns emerge from repeated exposure and neuromodulation")
    print("3. Brains adapt to different environments through plasticity and development")
    print("4. Memory systems integrate with learning for richer behavior")
    print("5. Performance optimization techniques can improve efficiency")
    print("6. Advanced API enables sophisticated control over brain dynamics")
    print()
    print("For more examples, see:")
    print("- Basic examples: examples/nlm_example_basic.py")
    print("- Command line interface: nlm_cli run --help")
    print("- Configuration management: nlm_cli config commands")
    
    return 0

if __name__ == "__main__":
    exit(main())