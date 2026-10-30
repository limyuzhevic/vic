#!/usr/bin/env python3
"""
Advanced NLM Python Examples and Usage Guide

This file contains advanced examples and comprehensive documentation for the NLM Python bindings.
It demonstrates complex usage patterns, performance optimization, and integration scenarios.
"""

import pynlm
import json
import time
from pathlib import Path

def basic_advanced_example():
    """
    Advanced example showing complex brain-world interaction with learning features.
    This demonstrates how to create a sophisticated agent that learns from experience.
    """
    print("=== Advanced Brain-World Example ===")
    
    # 1. Configure a brain with learning capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500, pynlm.ConfigSource::Runtime)
    config.set("brain.synapse_density", 0.15, pynlm.ConfigSource::Runtime)
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource::Runtime)
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource::Runtime)
    config.set("plasticity.structural.enable", True, pynlm.ConfigSource::Runtime)
    
    # 2. Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Configure world with complex environment
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, height=30,
        visionWidth=12, visionHeight=12
    )
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    # 4. Create and configure agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Add environmental complexity
    _add_complex_environment(world)
    
    # 7. Run advanced simulation
    print("Running advanced simulation with learning...")
    learning_results = run_advanced_learning_simulation(brain, agent, world)
    
    # 8. Analyze and display results
    analyze_learning_results(learning_results)
    
    return learning_results

def _add_complex_environment(world):
    """Add complex environmental elements for realistic learning."""
    # Add resources with varying values
    for i in range(10):
        world.addObject(pynlm.WorldObject(
            x=float(i * 3 + 5),
            y=float(i * 2 + 8),
            type=pynlm.WorldObjectType::Resource,
            value=float(10 + i * 5)
        ))
    
    # Add hazards for risk assessment
    for i in range(5):
        world.addObject(pynlm.WorldObject(
            x=float((i + 1) * 6 - 3),
            y=float((i + 1) * 4 + 2),
            type=pynlm.WorldObjectType::Hazard,
            radius=0.8,
            value=-20.0
        ))
    
    # Add walls for navigation challenges
    for x in range(0, 31, 5):
        world.addObject(pynlm.WorldObject(
            x=float(x),
            y=2.5,
            type=pynlm.WorldObjectType::Wall,
            radius=1.0,
            value=0.0
        ))
        world.addObject(pynlm.WorldObject(
            x=float(x),
            y=27.5,
            type=pynlm.WorldObjectType::Wall,
            radius=1.0,
            value=0.0
        ))

def run_advanced_learning_simulation(brain, agent, world, steps=1000):
    """Run advanced learning simulation with detailed metrics collection."""
    metrics = {
        'steps_completed': 0,
        'total_spikes': 0,
        'learning_events': 0,
        'novelty_events': 0,
        'reward_events': 0,
        'curiosity_driven_actions': 0,
        'structural_changes': 0,
        'development_stages': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': []
    }
    
    print(f"Starting advanced learning simulation for {steps} steps...")
    
    for step in range(steps):
        # Update world state
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input through brain
        agent.processSensoryInput(percept)
        
        # Brain computation step
        brain.step(step)
        
        # Agent decision making
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation based on world state
        reward = _calculate_reward(percept, action)
        predicted_reward = agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0.0
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics['steps_completed'] = step + 1
        metrics['total_spikes'] = brain.getTotalSpikeCount()
        
        if hasattr(agent, 'getLearningEvents'):
            metrics['learning_events'] = agent.getLearningEvents()
        
        if hasattr(agent, 'getCuriosityLevel'):
            metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        
        if hasattr(agent, 'getNoveltyLevel'):
            metrics['novelty_levels'].append(agent.getNoveltyLevel())
        
        if hasattr(agent, 'getPredictionError'):
            metrics['prediction_errors'].append(agent.getPredictionError())
        
        if hasattr(agent, 'getDevelopmentalStage'):
            metrics['development_stages'].append(agent.getDevelopmentalStage())
        
        if step % 100 == 0:
            print(f"  Step {step:4d}: Spikes={brain.getTotalSpikeCount():6d}, "
                  f"Curiosity={metrics['curiosity_levels'][-1]:.3f}, "
                  f"Novelty={metrics['novelty_levels'][-1]:.3f}")
    
    return metrics

def _calculate_reward(percept, action):
    """Calculate reward based on percept and action."""
    reward = 0.0
    
    # Positive reward for resources
    if hasattr(percept, 'getVision'):
        vision_data = percept.getVision()
        for y in range(8):  # Simplified vision processing
            for x in range(8):
                pixel_value = vision_data[y * 8 + x]
                if pixel_value > 0.5:  # Bright areas likely represent resources
                    reward += pixel_value * 10.0
    
    # Negative reward for hazards
    if hasattr(percept, 'getInternal'):
        internal_signals = percept.getInternal()
        if internal_signals and internal_signals[0] > 0.5:  # Hazard indicator
            reward -= 50.0
    
    # Action-based reward
    if hasattr(action, 'getType'):
        action_type = action.getType()
        if action_type == pynlm.ActionType::Interact:
            reward += 5.0
        elif action_type == pynlm.ActionType::MoveForward:
            reward += 1.0
    
    return reward

def analyze_learning_results(metrics):
    """Analyze and display learning results."""
    print("\n=== Learning Analysis ===")
    print(f"Total steps completed: {metrics['steps_completed']}")
    print(f"Total spikes generated: {metrics['total_spikes']}")
    
    if metrics['curiosity_levels']:
        avg_curiosity = sum(metrics['curiosity_levels']) / len(metrics['curiosity_levels'])
        max_curiosity = max(metrics['curiosity_levels'])
        print(f"Average curiosity level: {avg_curiosity:.3f}")
        print(f"Peak curiosity level: {max_curiosity:.3f}")
    
    if metrics['novelty_levels']:
        avg_novelty = sum(metrics['novelty_levels']) / len(metrics['novelty_levels'])
        print(f"Average novelty detection: {avg_novelty:.3f}")
    
    if metrics['prediction_errors']:
        avg_prediction_error = sum(metrics['prediction_errors']) / len(metrics['prediction_errors'])
        print(f"Average prediction error: {avg_prediction_error:.3f}")
    
    # Developmental stages reached
    if metrics['development_stages']:
        unique_stages = set(metrics['development_stages'])
        print(f"Developmental stages reached: {len(unique_stages)}")
        for stage in unique_stages:
            print(f"  - {stage}")
    
    # Learning effectiveness
    learning_rate = metrics['learning_events'] / max(1, metrics['steps_completed'] / 100)
    print(f"Learning rate: {learning_rate:.2f} events per 100 steps")

def create_learning_experiment():
    """
    Create a structured learning experiment with controlled conditions.
    This demonstrates how to design reproducible scientific experiments.
    """
    print("\n=== Creating Learning Experiment ===")
    
    experiment_config = {
        'name': 'Complex Environment Navigation',
        'brain_config': {
            'neuron_count': 1000,
            'synapse_density': 0.1,
            'plasticity_enabled': True
        },
        'environment_config': {
            'width': 40,
            'height': 40,
            'vision_range': 8,
            'resource_density': 0.15,
            'hazard_density': 0.05
        },
        'learning_config': {
            'reward_modulation': True,
            'structural_plasticity': True,
            'development': True,
            'curiosity': True,
            'novelty_detection': True
        },
        'experiment_duration': 5000,
        'save_interval': 500
    }
    
    # Save experiment configuration
    with open('experiment_config.json', 'w') as f:
        json.dump(experiment_config, f, indent=2)
    
    print("Experiment configuration saved to 'experiment_config.json'")
    return experiment_config

def performance_benchmark():
    """
    Performance benchmark for NLM Python bindings.
    Demonstrates how to measure and optimize performance.
    """
    print("\n=== Performance Benchmark ===")
    
    import timeit
    
    # Benchmark brain creation
    def create_brain_benchmark():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    # Benchmark world configuration
    def configure_world_benchmark():
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        return world
    
    # Benchmark simulation step
    def simulation_step_benchmark():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        for i in range(10):
            brain.step(i)
        return brain
    
    # Run benchmarks
    brain_time = timeit.timeit(create_brain_benchmark, number=5)
    world_time = timeit.timeit(configure_world_benchmark, number=10)
    step_time = timeit.timeit(simulation_step_benchmark, number=5)
    
    print(f"Brain creation time (5 runs): {brain_time:.3f}s")
    print(f"World configuration time (10 runs): {world_time:.3f}s")
    print(f"Simulation step time (5 runs): {step_time:.3f}s")
    
    return {
        'brain_creation': brain_time,
        'world_configuration': world_time,
        'simulation_step': step_time
    }

def compare_scenarios():
    """
    Compare different scenario configurations to find optimal settings.
    Demonstrates experimental design and parameter tuning.
    """
    print("\n=== Scenario Comparison ===")
    
    scenarios = {
        'simple': {
            'neuron_count': 100,
            'synapse_density': 0.05,
            'plasticity': False,
            'name': 'Simple Learning'
        },
        'moderate': {
            'neuron_count': 500,
            'synapse_density': 0.1,
            'plasticity': True,
            'name': 'Moderate Learning'
        },
        'complex': {
            'neuron_count': 1000,
            'synapse_density': 0.15,
            'plasticity': True,
            'name': 'Complex Learning'
        }
    }
    
    results = {}
    
    for scenario_name, config in scenarios.items():
        print(f"\nRunning {config['name']} scenario...")
        
        # Create configuration
        config_obj = pynlm.createDefaultConfig()
        config_obj.set("brain.neuron_count", config['neuron_count'])
        config_obj.set("brain.synapse_density", config['synapse_density'])
        config_obj.set("plasticity.stdp.enable", config['plasticity'])
        config_obj.set("plasticity.hebbian.enable", config['plasticity'])
        
        # Run scenario
        brain = pynlm.createBrain(config_obj)
        brain.initialize()
        
        # Run simulation
        for step in range(200):
            brain.step(step)
        
        # Collect results
        results[scenario_name] = {
            'final_spike_count': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'spike_count_per_step': brain.getTotalSpikeCount() / 200
        }
        
        print(f"  Final spike count: {results[scenario_name]['final_spike_count']}")
        print(f"  Average firing rate: {results[scenario_name]['average_firing_rate']:.3f}")
    
    # Compare results
    print("\n=== Scenario Comparison Results ===")
    best_scenario = None
    best_metric = 0
    
    for scenario_name, result in results.items():
        metric = result['spike_count_per_step']  # Use spike generation efficiency
        print(f"{scenario_name.capitalize():12} - Spikes/step: {result['spike_count_per_step']:6.2f}, "
              f"Final spikes: {result['final_spike_count']:6d}")
        
        if metric > best_metric:
            best_metric = metric
            best_scenario = scenario_name
    
    print(f"\nBest performing scenario: {best_scenario}")
    return results

def main():
    """Main function demonstrating all advanced examples."""
    print("NLM Advanced Python Examples")
    print("=" * 40)
    
    # Run basic advanced example
    try:
        results = basic_advanced_example()
    except Exception as e:
        print(f"Advanced example failed: {e}")
        results = None
    
    # Create learning experiment
    try:
        experiment = create_learning_experiment()
    except Exception as e:
        print(f"Experiment creation failed: {e}")
        experiment = None
    
    # Run performance benchmark
    try:
        benchmarks = performance_benchmark()
    except Exception as e:
        print(f"Performance benchmark failed: {e}")
        benchmarks = None
    
    # Compare scenarios
    try:
        scenario_results = compare_scenarios()
    except Exception as e:
        print(f"Scenario comparison failed: {e}")
        scenario_results = None
    
    print("\n=== Advanced Examples Complete ===")
    print("For more examples, see the documentation and sample scripts.")

if __name__ == "__main__":
    main()