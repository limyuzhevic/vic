#!/usr/bin/env python3
"""
NLM Advanced Examples and Mini-Projects

This file contains advanced examples demonstrating sophisticated NLM usage patterns.
Users can copy and modify these examples for their own projects.
"""

import pynlm
import time
import json
from typing import Dict, List, Optional, Callable
import math

def project_silent_brain():
    """
    Project 1: Silent Brain (Just Neurons)
    
    Create a brain and observe its neural activity without interaction.
    """
    print("=== Project 1: Silent Brain ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run for a moderate number of steps
    for i in range(50):
        brain.step(i)
        if i % 10 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print("Silent brain test complete!")
    print()


def project_brain_watching_world():
    """
    Project 2: Brain Watching a World
    
    Create a brain and world, let the brain observe but not interact.
    """
    print("=== Project 2: Brain Watching a World ===")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
        if i % 10 == 0:
            print(f"Step {i}: Firing rate={brain.getAverageFiringRate():.2f} Hz, "
                  f"Firing neurons={brain.getFiringNeuronCount()}")
    
    print("Watched world for 30 steps")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()


def project_complete_agent():
    """
    Project 3: Complete Agent
    
    Full-featured agent with all subsystems enabled.
    """
    print("=== Project 3: Complete Agent ===")
    
    # Setup with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
    print(f"Agent enabled: Reward={agent.isRewardModulationEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}")
    
    # Run agent for 100 steps
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    print()


def project_development_tracking():
    """
    Project 4: Development Tracking
    
    Track and visualize brain development over time.
    """
    print("=== Project 4: Development Tracking ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    # Track development metrics
    development_stages = []
    firing_rates = []
    curiosity_levels = []
    novelty_levels = []
    
    for step in range(200):
        agent.updateDevelopment(0.1)
        
        # Record metrics
        development_stages.append(brain.getDevelopmentalStage())
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        # Continue world simulation
        if step % 2 == 0:  # Update every other step to see development effects
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    # Analyze development
    unique_stages = set(development_stages)
    print(f"Development stages observed: {len(unique_stages)}")
    print(f"Time spent in each stage:")
    
    for stage in pynlm.DevelopmentalStage:
        count = development_stages.count(stage)
        if count > 0:
            print(f"  {stage.name}: {count} steps ({count/len(development_stages)*100:.1f}%)")
    
    print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.2f} Hz")
    print(f"Max curiosity level: {max(curiosity_levels):.3f}")
    print(f"Max novelty level: {max(novelty_levels):.3f}")
    print()


def project_prediction_learning():
    """
    Project 5: Prediction and Learning
    
    Focus on prediction systems and error-based learning.
    """
    print("=== Project 5: Prediction and Learning ===")
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(6.0, 6.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    
    # Track prediction metrics
    prediction_errors = []
    reward_history = []
    confidence_levels = []
    
    for step in range(150):
        world.update(0.1)
        
        # Get prediction error
        pred_error = agent.getPredictionError()
        prediction_errors.append(pred_error)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward (simplified)
        reward = 1.0 if action.getType() in [pynlm.MotorCommand.MoveForward, 
                                             pynlm.MotorCommand.TurnLeft, 
                                             pynlm.MotorCommand.TurnRight] else 0.0
        reward_history.append(reward)
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 30 == 0:
            avg_error = sum(prediction_errors[-30:]) / len(prediction_errors[-30:])
            print(f"Step {step}: Avg prediction error: {avg_error:.3f}, "
                  f"Reward: {reward}, Curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Calculate prediction learning metrics
    if prediction_errors:
        avg_error = sum(prediction_errors) / len(prediction_errors)
        error_variance = sum((x - avg_error) ** 2 for x in prediction_errors) / len(prediction_errors)
        
        print(f"\nPrediction Learning Summary:")
        print(f"Average prediction error: {avg_error:.3f}")
        print(f"Error variance: {error_variance:.3f}")
        print(f"Error trend: {'Improving' if avg_error < 0.5 else 'Needs improvement'}")
    
    print()


def project_multiscale_brain():
    """
    Project 6: Multi-Scale Brain
    
    Create and compare brains of different sizes and configurations.
    """
    print("=== Project 6: Multi-Scale Brain ===")
    
    scales = [
        {"neurons": 100, "density": 0.05, "name": "Small"},
        {"neurons": 500, "density": 0.1, "name": "Medium"},
        {"neurons": 1000, "density": 0.05, "name": "Large"}
    ]
    
    results = []
    
    for scale in scales:
        print(f"\nTesting {scale['name']} brain ({scale['neurons']} neurons, "
              f"density {scale['density']})")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", scale['neurons'])
        config.set("brain.synapse_density", scale['density'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Quick performance test
        start_time = time.time()
        
        for step in range(50):
            brain.step(step)
        
        end_time = time.time()
        
        avg_firing = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        step_time = (end_time - start_time) / 50
        
        results.append({
            'scale': scale['name'],
            'neurons': scale['neurons'],
            'avg_firing_rate': avg_firing,
            'total_spikes': total_spikes,
            'step_time_ms': step_time * 1000,
            'spikes_per_neuron': total_spikes / scale['neurons'] if scale['neurons'] > 0 else 0
        })
        
        print(f"  Avg firing rate: {avg_firing:.2f} Hz")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Average step time: {step_time*1000:.2f} ms")
        print(f"  Spikes per neuron: {total_spikes / scale['neurons']:.2f}")
    
    # Find best performing brain
    if results:
        best_by_efficiency = max(results, key=lambda x: x['spikes_per_neuron'] / x['step_time_ms'])
        print(f"\nBest performing brain: {best_by_efficiency['scale']} "
              f"(efficiency: {best_by_efficiency['spikes_per_neuron']/best_by_efficiency['step_time_ms']:.3f})")
    
    print()


def project_reproduction_experiment():
    """
    Project 7: Reproduction Experiment
    
    Implement a scientifically-inspired reproduction experiment.
    """
    print("=== Project 7: Reproduction Experiment ===")
    
    # Experiment configuration based on biological principles
    experiment_config = {
        "brain.neuron_count": 800,
        "brain.synapse_density": 0.08,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.initial_plasticity": 1.0,
        "development.critical_period_duration": 50
    }
    
    config = pynlm.createDefaultConfig()
    for key, value in experiment_config.items():
        if isinstance(value, (int, float, bool)):
            config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all experimental conditions
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run experiment with data collection
    experimental_data = {
        'steps': [],
        'rewards': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'development_stages': []
    }
    
    for step in range(200):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect experimental data
        reward = world.getSensoryPercept().getInternal()[0] or 0.0
        
        experimental_data['steps'].append(step)
        experimental_data['rewards'].append(reward)
        experimental_data['firing_rates'].append(brain.getAverageFiringRate())
        experimental_data['curiosity_levels'].append(agent.getCuriosityLevel())
        experimental_data['novelty_levels'].append(agent.getNoveltyLevel())
        experimental_data['prediction_errors'].append(agent.getPredictionError())
        experimental_data['development_stages'].append(brain.getDevelopmentalStage())
        
        # Apply learning
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 50 == 0:
            print(f"Step {step}: Reward={reward:.2f}, "
                  f"Firing={brain.getAverageFiringRate():.2f} Hz, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    # Analyze experimental results
    print("\n=== Experimental Analysis ===")
    print(f"Total simulation steps: {len(experimental_data['steps'])}")
    print(f"Average reward per step: {sum(experimental_data['rewards'])/len(experimental_data['rewards']):.3f}")
    print(f"Final development stage: {experimental_data['development_stages'][-1]}")
    
    # Calculate learning curves
    if len(experimental_data['rewards']) > 20:
        recent_rewards = experimental_data['rewards'][-20:]
        avg_recent_reward = sum(recent_rewards) / len(recent_rewards)
        print(f"Average reward (last 20 steps): {avg_recent_reward:.3f}")
        print(f"Learning status: {'Improving' if avg_recent_reward > 0.3 else 'Stable'}")
    
    # Save experimental data
    with open('experiment_data.json', 'w') as f:
        json.dump(experimental_data, f, indent=2)
    
    print("Experimental data saved to 'experiment_data.json'")
    print()


def project_checkpoint_resilience():
    """
    Project 8: Checkpoint and Resilience Testing
    
    Test brain state saving and loading functionality.
    """
    print("=== Project 8: Checkpoint and Resilience Testing ===")
    
    config = pynlm.createDefaultConfig()
    
    # Create and train first brain
    print("Creating and training initial brain...")
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    for step in range(100):
        brain1.step(step)
        if step % 25 == 0:
            print(f"  Training step {step}: {brain1.getFiringNeuronCount()} firing neurons")
    
    # Save checkpoint
    checkpoint_file = "brain_checkpoint.bin"
    if brain1.save(checkpoint_file):
        print(f"✓ Checkpoint saved to {checkpoint_file}")
    else:
        print(f"✗ Failed to save checkpoint")
        return
    
    # Create second brain and load checkpoint
    print("Creating second brain and loading checkpoint...")
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load(checkpoint_file):
        print(f"✓ Checkpoint loaded successfully")
    else:
        print(f"✗ Failed to load checkpoint")
        return
    
    # Verify brains have same state
    neurons1 = brain1.getTotalNeuronCount()
    neurons2 = brain2.getTotalNeuronCount()
    
    print(f"Original brain neurons: {neurons1}")
    print(f"Loaded brain neurons: {neurons2}")
    
    if neurons1 == neurons2:
        print("✓ Brain states match!")
    else:
        print("✗ Brain states differ!")
    
    # Continue training from checkpoint
    print("Continuing training from checkpoint...")
    for step in range(100, 150):
        brain2.step(step)
    
    print(f"Final state: {brain2.getFiringNeuronCount()} firing neurons")
    print()


def project_parameter_sweep():
    """
    Project 9: Parameter Sweep
    
    Systematically test different parameter combinations.
    """
    print("=== Project 9: Parameter Sweep ===")
    
    # Define parameter space to test
    parameters = {
        "brain.neuron_count": [100, 500, 1000],
        "brain.synapse_density": [0.05, 0.1, 0.15],
        "plasticity.stdp.enable": [True, False]
    }
    
    results = []
    
    # Generate all combinations (limited for demonstration)
    test_configs = [
        {"neurons": 100, "density": 0.05, "stdp": True},
        {"neurons": 100, "density": 0.05, "stdp": False},
        {"neurons": 500, "density": 0.1, "stdp": True},
        {"neurons": 500, "density": 0.1, "stdp": False},
        {"neurons": 1000, "density": 0.15, "stdp": True},
    ]
    
    print(f"Testing {len(test_configs)} configurations...")
    
    for i, params in enumerate(test_configs):
        print(f"\nConfiguration {i+1}/{len(test_configs)}: "
              f"{params['neurons']} neurons, density {params['density']}, "
              f"STDP={'enabled' if params['stdp'] else 'disabled'}")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params['neurons'])
        config.set("brain.synapse_density", params['density'])
        config.set("plasticity.stdp.enable", params['stdp'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
        
        # Record metrics
        result = {
            'neurons': params['neurons'],
            'density': params['density'],
            'stdp_enabled': params['stdp'],
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getFiringNeuronCount()
        }
        
        results.append(result)
        
        print(f"  Final firing rate: {result['final_firing_rate']:.2f} Hz")
        print(f"  Total spikes: {result['total_spikes']}")
        print(f"  Active neurons: {result['active_neurons']}")
    
    # Analyze results
    print("\n=== Parameter Sweep Analysis ===")
    
    if results:
        # Find best performing configuration
        best_by_firing = max(results, key=lambda x: x['final_firing_rate'])
        print(f"Best firing rate: {best_by_firing['neurons']} neurons, "
              f"density {best_by_firing['density']}, STDP {best_by_firing['stdp_enabled']} "
              f"({best_by_firing['final_firing_rate']:.2f} Hz)")
        
        # Group by STDP
        with_stdp = [r for r in results if r['stdp_enabled']]
        without_stdp = [r for r in results if not r['stdp_enabled']]
        
        if with_stdp and without_stdp:
            avg_with_stdp = sum(r['final_firing_rate'] for r in with_stdp) / len(with_stdp)
            avg_without_stdp = sum(r['final_firing_rate'] for r in without_stdp) / len(without_stdp)
            
            print(f"Average firing rate with STDP: {avg_with_stdp:.2f} Hz")
            print(f"Average firing rate without STDP: {avg_without_stdp:.2f} Hz")
            print(f"STDP improvement: {((avg_with_stdp - avg_without_stdp) / avg_without_stdp * 100):.1f}%")
    
    print()


def project_benchmark_suite():
    """
    Project 10: Benchmark Suite
    
    Comprehensive benchmark of NLM performance across different tasks.
    """
    print("=== Project 10: Benchmark Suite ===")
    
    benchmarks = {
        "silent_brain": project_silent_brain,
        "brain_world": project_brain_watching_world,
        "complete_agent": project_complete_agent,
        "development": project_development_tracking,
        "prediction": project_prediction_learning,
        "multiscale": project_multiscale_brain,
        "reproduction": project_reproduction_experiment,
        "checkpoint": project_checkpoint_resilience,
        "sweep": project_parameter_sweep,
    }
    
    print("Running NLM Benchmark Suite...")
    print("Note: This runs all projects as benchmarks.")
    print()
    
    results = {}
    
    for name, benchmark_func in benchmarks.items():
        print(f"--- Running {name} benchmark ---")
        start_time = time.time()
        
        try:
            benchmark_func()
            end_time = time.time()
            duration = end_time - start_time
            results[name] = {
                'status': 'completed',
                'duration_seconds': duration
            }
            print(f"✓ {name} completed in {duration:.2f} seconds")
        except Exception as e:
            end_time = time.time()
            duration = end_time - start_time
            results[name] = {
                'status': 'failed',
                'error': str(e),
                'duration_seconds': duration
            }
            print(f"✗ {name} failed after {duration:.2f} seconds: {e}")
        
        print()
    
    # Summary
    print("=== Benchmark Summary ===")
    for name, result in results.items():
        status_icon = "✓" if result['status'] == 'completed' else "✗"
        print(f"{status_icon} {name:20s}: {result['status']:8s} "
              f"(duration: {result['duration_seconds']:.2f}s)")
    
    # Save benchmark results
    with open('benchmark_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\nBenchmark results saved to 'benchmark_results.json'")
    print()


if __name__ == "__main__":
    print("NLM Advanced Examples and Mini-Projects")
    print("=" * 50)
    print()
    
    # Run all projects as a comprehensive demonstration
    project_silent_brain()
    project_brain_watching_world()
    project_complete_agent()
    project_development_tracking()
    project_prediction_learning()
    project_multiscale_brain()
    project_reproduction_experiment()
    project_checkpoint_resilience()
    project_parameter_sweep()
    project_benchmark_suite()
    
    print("All projects completed!")
    print("\nEach project demonstrates different NLM capabilities:")
    print("- Silent brain: Basic neural activity")
    print("- Brain watching world: Observation without interaction")
    print("- Complete agent: Full-featured learning agent")
    print("- Development tracking: Brain maturation monitoring")
    print("- Prediction learning: Error-based learning")
    print("- Multi-scale brain: Performance across sizes")
    print("- Reproduction experiment: Scientific experimental setup")
    print("- Checkpoint resilience: State persistence")
    print("- Parameter sweep: Systematic testing")
    print("- Benchmark suite: Comprehensive evaluation")