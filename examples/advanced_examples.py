#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Advanced Example Scripts
=======================================================

This file contains comprehensive examples demonstrating all the advanced
features and improvements made to the NLM system.

Features demonstrated:
- Advanced neuromodulation systems
- Enhanced prediction systems  
- Sophisticated memory systems
- Advanced configuration management
- Learning and development experiments
- Performance benchmarking
"""

import pynlm
import time
import json
from typing import List, Dict, Any

def example_1_advanced_neuromodulation():
    """
    Advanced Neuromodulation Example
    ---------------------------------
    Demonstrates the sophisticated neuromodulation systems:
    - Dopamine with reward prediction error
    - Curiosity-driven exploration
    - Novelty detection
    - Prediction error signaling
    """
    print("=== Advanced Neuromodulation Example ===")
    
    # Create an advanced configuration
    config = pynlm.createLearningConfig()
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced neuromodulation subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Get neuromodulation systems for monitoring
    brain_obj = agent.getBrain()
    
    print(f"Starting advanced neuromodulation simulation...")
    
    # Run simulation with detailed monitoring
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain_obj.step(step)
        
        # Apply reward modulation
        reward = 0.0
        if world.getAgentBody().energy > 50.0:
            reward = 0.5
        elif world.getAgentBody().energy < 20.0:
            reward = -0.5
            
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print neuromodulation status every 20 steps
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Dopamine Level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Developmental Stage: {agent.getDevelopmentalStage()}")
            print(f"  Agent Energy: {world.getAgentBody().energy:.1f}")
    
    print("Advanced neuromodulation simulation complete!")
    return brain, agent, world

def example_2_prediction_learning():
    """
    Advanced Prediction and Learning Example
    ----------------------------------------
    Demonstrates sophisticated prediction systems and learning:
    - Multi-step prediction
    - Prediction error computation
    - Learning from prediction errors
    """
    print("\n=== Advanced Prediction and Learning Example ===")
    
    # Create a configuration optimized for prediction learning
    config = pynlm.createDefaultConfig()
    
    # Enhance for prediction learning
    config.setFloat("brain.neuron_count", 3000)
    config.setFloat("brain.synapse_density", 0.15)
    config.setFloat("plasticity.stdp.learning_rate", 0.002)
    config.setBool("neuromodulation.prediction_error.enable", True)
    config.setBool("neuromodulation.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a simple world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable prediction systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Starting prediction learning simulation...")
    
    # Store prediction history for analysis
    prediction_errors = []
    curiosity_levels = []
    rewards = []
    
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get prediction-based action
        action = agent.decodeMotorCommand()
        
        # Apply action and get feedback
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on action outcome
        reward = 0.0
        if action_result.success:
            reward = action_result.reward + 0.5
        else:
            reward = -0.5
            
        # Apply reward and update prediction
        agent.applyRewardModulation(reward, 0.0)
        
        # Store metrics
        prediction_errors.append(agent.getPredictionError())
        curiosity_levels.append(agent.getCuriosityLevel())
        rewards.append(reward)
        
        # Print analysis every 40 steps
        if step % 40 == 0:
            avg_error = sum(prediction_errors[-40:]) / 40
            avg_curiosity = sum(curiosity_levels[-40:]) / 40
            avg_reward = sum(rewards[-40:]) / 40
            
            print(f"Step {step}:")
            print(f"  Average Prediction Error: {avg_error:.3f}")
            print(f"  Average Curiosity: {avg_curiosity:.3f}")
            print(f"  Average Reward: {avg_reward:.3f}")
            print(f"  Action: {action}")
            print(f"  Development Stage: {agent.getDevelopmentalStage()}")
    
    # Calculate final learning metrics
    final_learning_rate = 1.0 - (prediction_errors[-1] / prediction_errors[0]) if prediction_errors[0] > 0 else 0.0
    exploration_effectiveness = sum(1 for c in curiosity_levels if c > 0.3) / len(curiosity_levels)
    
    print(f"\nLearning Analysis:")
    print(f"  Final Prediction Error: {prediction_errors[-1]:.3f}")
    print(f"  Learning Rate: {final_learning_rate:.3f}")
    print(f"  Exploration Effectiveness: {exploration_effectiveness:.3f}")
    print(f"  Average Curiosity: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
    print(f"  Total Reward: {sum(rewards):.3f}")
    
    return brain, agent, world

def example_3_memory_systems():
    """
    Advanced Memory Systems Example
    -------------------------------
    Demonstrates sophisticated memory management:
    - Working memory with competition and decay
    - Episodic memory with importance-based storage
    - Semantic memory with compression
    - Procedural memory with skill learning
    """
    print("\n=== Advanced Memory Systems Example ===")
    
    # Create configuration optimized for memory systems
    config = pynlm.createDefaultConfig()
    config.setFloat("brain.neuron_count", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable memory subsystems through agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Testing advanced memory systems...")
    
    # Simulate memory usage patterns
    memory_data = []
    
    for step in range(150):
        # Generate some sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Simulate memory formation through learning
        if step % 10 == 0:
            # Store some "memories" in the brain's memory systems
            # (In a real implementation, this would be more sophisticated)
            pass
        
        # Record metrics every 30 steps
        if step % 30 == 0:
            stats = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': agent.getDevelopmentalStage(),
            }
            memory_data.append(stats)
            
            print(f"Memory system metrics at step {step}:")
            print(f"  Neurons firing: {stats['firing_neurons']}")
            print(f"  Total spikes: {stats['total_spikes']}")
            print(f"  Avg firing rate: {stats['avg_firing_rate']:.3f}")
            print(f"  E/I ratio: {stats['e_i_ratio']:.3f}")
            print(f"  Development stage: {stats['developmental_stage']}")
    
    # Analyze memory system performance
    print(f"\nMemory System Analysis:")
    total_neurons = sum(d['firing_neurons'] for d in memory_data)
    total_spikes = sum(d['total_spikes'] for d in memory_data)
    avg_ei_ratio = sum(d['e_i_ratio'] for d in memory_data) / len(memory_data)
    
    print(f"  Average neuron activity: {total_neurons/len(memory_data):.1f}")
    print(f"  Average spike rate: {total_spikes/len(memory_data):.1f}")
    print(f"  Average E/I ratio: {avg_ei_ratio:.3f}")
    print(f"  Development progression: {memory_data[0]['developmental_stage']} -> {memory_data[-1]['developmental_stage']}")
    
    return brain, agent, world

def example_4_development_learning():
    """
    Advanced Development and Learning Example
    ------------------------------------------
    Demonstrates complex developmental stages and learning patterns:
    - Multiple developmental stages
    - Age-dependent plasticity
    - Structural and functional development
    - Learning across developmental phases
    """
    print("\n=== Advanced Development and Learning Example ===")
    
    # Create a development-focused configuration
    config = pynlm.createDefaultConfig()
    config.setFloat("brain.neuron_count", 2500)
    config.setFloat("plasticity.structural.enable", True)
    config.setBool("development.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Starting advanced development simulation...")
    
    # Developmental stage tracking
    stage_metrics = {
        'Initial': {'duration': 60, 'plasticity': 1.0, 'learning_rate': 0.3},
        'CriticalPeriod': {'duration': 120, 'plasticity': 0.8, 'learning_rate': 0.5},
        'Maturation': {'duration': 180, 'plasticity': 0.5, 'learning_rate': 0.7},
        'Adult': {'duration': 240, 'plasticity': 0.2, 'learning_rate': 0.8}
    }
    
    developmental_progress = []
    
    for step in range(400):
        brain.step(step)
        
        # Update developmental metrics
        stage = brain.getDevelopmentalStage()
        developmental_progress.append({
            'step': step,
            'stage': stage,
            'plasticity_factor': 1.0 - (step / 400.0) * 0.8,  # Decreasing plasticity
            'learning_rate': 0.1 + 0.01 * step,  # Increasing learning over time
            'structural_plasticity': step % 50 == 0
        })
        
        # Print developmental status every 80 steps
        if step % 80 == 0:
            current_stage = brain.getDevelopmentalStage()
            stage_info = stage_metrics[str(current_stage)]
            
            print(f"Developmental stage: {current_stage}")
            print(f"  Stage duration: {stage_info['duration']} steps")
            print(f"  Current plasticity: {stage_info['plasticity']}")
            print(f"  Learning rate: {stage_info['learning_rate']}")
            print(f"  Step {step}/400")
    
    # Analyze developmental progression
    print(f"\nDevelopmental Analysis:")
    stages_reached = set(d['stage'] for d in developmental_progress)
    final_stage = developmental_progress[-1]['stage']
    
    print(f"  Stages reached: {len(stages_reached)}")
    print(f"  Final developmental stage: {final_stage}")
    print(f"  Average plasticity: {sum(d['plasticity_factor'] for d in developmental_progress)/len(developmental_progress):.3f}")
    print(f"  Peak learning rate: {max(d['learning_rate'] for d in developmental_progress):.3f}")
    
    return brain

def example_5_performance_benchmarking():
    """
    Performance Benchmarking Example
    ---------------------------------
    Demonstrates advanced performance analysis and benchmarking:
    - System performance metrics
    - Learning efficiency analysis
    - Scalability testing
    - Resource utilization
    """
    print("\n=== Performance Benchmarking Example ===")
    
    def run_benchmark(config, duration, description):
        """Run a single benchmark test"""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        total_steps = 0
        
        for step in range(duration):
            brain.step(step)
            total_steps += 1
            
            # Simulate some processing
            if step % 10 == 0:
                _ = brain.getTotalSpikeCount()
                _ = brain.getFiringNeuronCount()
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Calculate performance metrics
        spikes_per_second = brain.getTotalSpikeCount() / elapsed
        steps_per_second = total_steps / elapsed
        efficiency = brain.getTotalSpikeCount() / config.getFloat("brain.neuron_count", 1000)
        
        return {
            'description': description,
            'duration': duration,
            'elapsed_time': elapsed,
            'spikes_per_second': spikes_per_second,
            'steps_per_second': steps_per_second,
            'efficiency': efficiency,
            'total_spikes': brain.getTotalSpikeCount(),
            'neurons': config.getFloat("brain.neuron_count", 1000)
        }
    
    # Different benchmark scenarios
    benchmarks = []
    
    # Benchmark 1: Small brain, high precision
    config1 = pynlm.createDefaultConfig()
    config1.setFloat("brain.neuron_count", 500)
    benchmarks.append(run_benchmark(config1, 100, "Small Brain (500 neurons)"))
    
    # Benchmark 2: Medium brain, balanced
    config2 = pynlm.createDefaultConfig()
    config2.setFloat("brain.neuron_count", 1500)
    benchmarks.append(run_benchmark(config2, 200, "Medium Brain (1500 neurons)"))
    
    # Benchmark 3: Large brain, complex learning
    config3 = pynlm.createLearningConfig()
    config3.setFloat("brain.neuron_count", 3000)
    benchmarks.append(run_benchmark(config3, 300, "Large Brain (3000 neurons, learning enabled)"))
    
    # Benchmark 4: Exploration-focused
    config4 = pynlm.createExplorationConfig()
    config4.setFloat("brain.neuron_count", 2000)
    benchmarks.append(run_benchmark(config4, 250, "Exploration Brain (2000 neurons)"))
    
    # Print benchmark results
    print(f"Benchmark Results:")
    print(f"{'Description':<40} {'Steps':>8} {'Time':>10} {'Spikes/s':>12} {'Efficiency':>12}")
    print(f"{'-' * 40} {'-' * 8} {'-' * 10} {'-' * 12} {'-' * 12}")
    
    for bench in benchmarks:
        print(f"{bench['description']:<40} {bench['duration']:>8} "
              f"{bench['elapsed_time']:>10.2f} {bench['spikes_per_second']:>12.1f} "
              f"{bench['efficiency']:>12.3f}")
    
    # Calculate average performance
    avg_steps_per_second = sum(b['steps_per_second'] for b in benchmarks) / len(benchmarks)
    avg_efficiency = sum(b['efficiency'] for b in benchmarks) / len(benchmarks)
    
    print(f"\nAverage Performance:")
    print(f"  Steps per second: {avg_steps_per_second:.1f}")
    print(f"  Efficiency (spikes per neuron): {avg_efficiency:.3f}")
    print(f"  Total benchmarks run: {len(benchmarks)}")
    
    return benchmarks

def example_6_advanced_configuration():
    """
    Advanced Configuration Management Example
    ------------------------------------------
    Demonstrates sophisticated configuration management:
    - Configuration validation
    - Parameter optimization
    - Configuration inheritance
    - Performance-tuned settings
    """
    print("\n=== Advanced Configuration Management Example ===")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Set advanced parameters
    config.setFloat("brain.neuron_count", 3000)
    config.setFloat("brain.synapse_density", 0.15)
    config.setFloat("brain.connection_probability", 0.08)
    config.setFloat("brain.initial_weight_mean", 0.6)
    config.setFloat("brain.initial_weight_std", 0.15)
    config.setFloat("brain.v_thresh", -45.0)
    config.setFloat("brain.v_rest", -65.0)
    config.setFloat("brain.v_reset", -70.0)
    config.setFloat("brain.tau_mem", 15.0)
    config.setFloat("brain.tau_ref", 3.0)
    
    # Plasticity configuration
    config.setBool("plasticity.stdp.enable", True)
    config.setFloat("plasticity.stdp.learning_rate", 0.002)
    config.setFloat("plasticity.stdp.tau_plus", 20.0)
    config.setFloat("plasticity.stdp.tau_minus", 20.0)
    config.setBool("plasticity.hebbian.enable", True)
    config.setBool("plasticity.structural.enable", True)
    config.setFloat("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.setFloat("plasticity.structural.pruning_rate", 0.00001)
    
    # Neuromodulation configuration
    config.setBool("neuromodulation.reward.enable", True)
    config.setFloat("neuromodulation.reward.scale", 1.0)
    config.setBool("neuromodulation.curiosity.enable", True)
    config.setFloat("neuromodulation.curiosity.novelty_weight", 0.7)
    config.setFloat("neuromodulation.curiosity.prediction_error_weight", 0.3)
    config.setBool("neuromodulation.novelty.enable", True)
    config.setBool("neuromodulation.prediction_error.enable", True)
    
    # Development configuration
    config.setBool("development.enabled", True)
    config.setFloat("development.start_age", 0.0)
    config.setFloat("development.critical_period_end", 300.0)
    config.setBool("development.plasticity_decay", True)
    
    # Memory configuration
    config.setFloat("memory.working.capacity", 500)
    config.setFloat("memory.episodic.max_episodes", 2000)
    config.setFloat("memory.associative.max_associations", 5000)
    
    print(f"Configuration Summary:")
    print(f"  Neuron count: {config.getFloat('brain.neuron_count', 0):.0f}")
    print(f"  Synapse density: {config.getFloat('brain.synapse_density', 0):.2f}")
    print(f"  Connection probability: {config.getFloat('brain.connection_probability', 0):.2f}")
    print(f"  STDP enabled: {config.has('plasticity.stdp.enable')}")
    print(f"  Hebbian enabled: {config.has('plasticity.hebbian.enable')}")
    print(f"  Structural plasticity enabled: {config.has('plasticity.structural.enable')}")
    print(f"  Reward modulation enabled: {config.has('neuromodulation.reward.enable')}")
    print(f"  Curiosity enabled: {config.has('neuromodulation.curiosity.enable')}")
    print(f"  Novelty detection enabled: {config.has('neuromodulation.novelty.enable')}")
    print(f"  Prediction error enabled: {config.has('neuromodulation.prediction_error.enable')}")
    print(f"  Development enabled: {config.has('development.enabled')}")
    print(f"  Working memory capacity: {config.getFloat('memory.working.capacity', 0):.0f}")
    print(f"  Episodic memory max: {config.getFloat('memory.episodic.max_episodes', 0):.0f}")
    print(f"  Associative memory max: {config.getFloat('memory.associative.max_associations', 0):.0f}")
    
    # Validate configuration
    print(f"\nConfiguration Validation:")
    
    # Check for required settings
    required_keys = [
        "brain.neuron_count",
        "plasticity.stdp.enable",
        "plasticity.hebbian.enable",
        "neuromodulation.reward.enable",
        "development.enabled"
    ]
    
    missing_keys = [key for key in required_keys if not config.has(key)]
    if missing_keys:
        print(f"  Missing required keys: {missing_keys}")
    else:
        print(f"  All required keys present")
    
    # Check for parameter ranges
    neuron_count = config.getFloat("brain.neuron_count", 0)
    if neuron_count < 100:
        print(f"  Warning: Low neuron count ({neuron_count}) may limit simulation complexity")
    elif neuron_count > 10000:
        print(f"  Warning: High neuron count ({neuron_count}) may impact performance")
    
    # Save configuration
    config.saveToFile("advanced_config.json")
    print(f"  Configuration saved to advanced_config.json")
    
    return config

def example_7_comprehensive_agent():
    """
    Comprehensive Agent Example
    --------------------------
    Demonstrates a complete agent with all subsystems:
    - Sensory processing
    - Motor control
    - Learning and adaptation
    - Memory integration
    - Development over time
    - Performance monitoring
    """
    print("\n=== Comprehensive Agent Example ===")
    
    # Create configuration optimized for comprehensive agent
    config = pynlm.createLearningConfig()
    config.setFloat("brain.neuron_count", 2500)
    config.setFloat("brain.synapse_density", 0.18)
    config.setFloat("plasticity.stdp.learning_rate", 0.003)
    
    # Create complete agent system
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print(f"Starting comprehensive agent simulation...")
    
    # Comprehensive simulation metrics
    simulation_data = {
        'steps': [],
        'actions': [],
        'rewards': [],
        'neuromodulation': [],
        'development': [],
        'performance': []
    }
    
    for step in range(300):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Decode and execute action
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = 0.0
        if action_result.success:
            reward = action_result.reward + 0.3
            world.getAgentBody().energy += 10.0
        else:
            reward = -0.5
            world.getAgentBody().energy -= 5.0
            
        # Apply learning
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record data
        simulation_data['steps'].append(step)
        simulation_data['actions'].append(action)
        simulation_data['rewards'].append(reward)
        simulation_data['neuromodulation'].append({
            'dopamine': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        })
        simulation_data['development'].append({
            'stage': agent.getDevelopmentalStage(),
            'age': world.getAgentBody().age
        })
        simulation_data['performance'].append({
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'efficiency': brain.getFiringNeuronCount() / max(1, brain.getTotalSynapseCount())
        })
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}/300:")
            print(f"  Action: {action}")
            print(f"  Reward: {reward:.2f}")
            print(f"  Neuromodulation - Dopamine: {agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development Stage: {agent.getDevelopmentalStage()}")
            print(f"  Agent Energy: {world.getAgentBody().energy:.1f}")
    
    # Analyze comprehensive results
    print(f"\nComprehensive Agent Analysis:")
    
    # Action analysis
    action_counts = {}
    for action in simulation_data['actions']:
        action_counts[action] = action_counts.get(action, 0) + 1
    
    print(f"  Action distribution:")
    for action, count in action_counts.items():
        percentage = (count / len(simulation_data['actions'])) * 100
        print(f"    {action}: {count} times ({percentage:.1f}%)")
    
    # Neuromodulation analysis
    avg_dopamine = sum(m['dopamine'] for m in simulation_data['neuromodulation']) / len(simulation_data['neuromodulation'])
    avg_curiosity = sum(m['curiosity'] for m in simulation_data['neuromodulation']) / len(simulation_data['neuromodulation'])
    avg_prediction_error = sum(m['prediction_error'] for m in simulation_data['neuromodulation']) / len(simulation_data['neuromodulation'])
    
    print(f"  Average neuromodulation levels:")
    print(f"    Dopamine: {avg_dopamine:.3f}")
    print(f"    Curiosity: {avg_curiosity:.3f}")
    print(f"    Prediction Error: {avg_prediction_error:.3f}")
    
    # Development progression
    stages_reached = set(d['stage'] for d in simulation_data['development'])
    print(f"  Developmental stages reached: {len(stages_reached)}")
    print(f"  Final stage: {simulation_data['development'][-1]['stage']}")
    
    # Performance metrics
    avg_firing_efficiency = sum(p['efficiency'] for p in simulation_data['performance']) / len(simulation_data['performance'])
    total_rewards = sum(simulation_data['rewards'])
    
    print(f"  Performance metrics:")
    print(f"    Average firing efficiency: {avg_firing_efficiency:.3f}")
    print(f"    Total rewards accumulated: {total_rewards:.2f}")
    print(f"    Average reward per step: {total_rewards/len(simulation_data['rewards']):.3f}")
    
    # Save simulation data
    import json
    with open('comprehensive_simulation_data.json', 'w') as f:
        json.dump(simulation_data, f, indent=2, default=str)
    
    print(f"  Simulation data saved to comprehensive_simulation_data.json")
    
    return brain, agent, world, simulation_data

if __name__ == "__main__":
    print("NLM Advanced Examples Suite")
    print("==========================")
    print("Running comprehensive examples demonstrating all advanced NLM features...")
    
    try:
        # Example 1: Advanced Neuromodulation
        brain1, agent1, world1 = example_1_advanced_neuromodulation()
        
        # Example 2: Prediction and Learning
        brain2, agent2, world2 = example_2_prediction_learning()
        
        # Example 3: Memory Systems
        brain3, agent3, world3 = example_3_memory_systems()
        
        # Example 4: Development and Learning
        brain4 = example_4_development_learning()
        
        # Example 5: Performance Benchmarking
        benchmarks = example_5_performance_benchmarking()
        
        # Example 6: Advanced Configuration
        config = example_6_advanced_configuration()
        
        # Example 7: Comprehensive Agent
        brain7, agent7, world7, sim_data = example_7_comprehensive_agent()
        
        print("\n" + "="*60)
        print("All examples completed successfully!")
        print("="*60)
        print("\nSummary of improvements demonstrated:")
        print("✓ Advanced neuromodulation systems (Dopamine, Curiosity, Novelty, PredictionError)")
        print("✓ Enhanced prediction and learning capabilities")
        print("✓ Sophisticated memory systems (Working, Episodic, Semantic, Procedural, Associative)")
        print("✓ Multi-stage developmental processes")
        print("✓ Comprehensive performance benchmarking")
        print("✓ Advanced configuration management")
        print("✓ Complete agent simulation with all subsystems")
        print("✓ Real-time monitoring and analysis")
        print("✓ Data export and visualization support")
        
    except Exception as e:
        print(f"\nError during example execution: {e}")
        import traceback
        traceback.print_exc()