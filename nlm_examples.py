#!/usr/bin/env python3
"""
NLM Advanced Usage Examples

This file provides comprehensive examples and advanced usage patterns
for the NLM (Neural Learning Machine) Python library.

These examples demonstrate:
1. Advanced brain creation with different configurations
2. Experiment design and running
3. Agent-environment interaction
4. Performance optimization
5. Advanced configuration techniques
6. Debugging and monitoring techniques
"""

import pynlm
import time
import numpy as np
from typing import List, Tuple, Optional
import json
import sys


def example_1_basic_usage():
    """Example 1: Basic brain creation and simulation."""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Create a simple brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Run a simple simulation
    for i in range(100):
        brain.step(i)
        if i % 25 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()


def example_2_advanced_configuration():
    """Example 2: Advanced brain configuration."""
    print("=== Example 2: Advanced Configuration ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set multiple configuration options
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.1)
    config.set("brain.v_thresh", -52.0)
    config.set("brain.v_rest", -65.0)
    config.set("brain.v_reset", -65.0)
    config.set("brain.tau_mem", 20.0)
    config.set("brain.tau_ref", 3.0)
    
    # Configure plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    
    config.set("plasticity.structural.enable", True)
    config.set("synaptogenesis_rate", 0.00005f)
    config.set("pruning_rate", 0.000005f)
    
    # Configure neuromodulation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_strength", 0.8)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.6)
    
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.reward_scale", 1.0)
    
    # Configure development
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.maturity_age", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created advanced brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Configuration summary: {config.summary()}")
    
    # Run learning simulation
    for i in range(200):
        brain.step(i)
        if i % 50 == 0:
            curiosity = brain.getCuriosityLevel()
            novelty = brain.getNoveltyLevel()
            print(f"Step {i}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final dopamine level: {brain.getDopamineLevel():.3f}")
    print()


def example_3_agent_environment_simulation():
    """Example 3: Complete agent-environment simulation."""
    print("=== Example 3: Agent-Environment Simulation ===")
    
    # Create agent and world
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print("Starting agent-world simulation...")
    
    # Simulation parameters
    total_steps = 500
    statistics_interval = 50
    
    for step in range(total_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log statistics
        if step % statistics_interval == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Action: {action.getType()}")
            print()
    
    print("=== SIMULATION COMPLETE ===")
    print(f"Total steps: {total_steps}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
    print()


def example_4_memory_systems():
    """Example 4: Memory systems demonstration."""
    print("=== Example 4: Memory Systems ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("memory.working.capacity", 500)
    config.set("memory.episodic.max_episodes", 50)
    config.set("replay_interval", 20)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate experience accumulation
    working_memory_trends = []
    episodic_memory_trends = []
    
    for episode in range(50):
        # Present novel stimulus
        for step in range(20):
            # Inject sensory input
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
            brain.step(step + episode * 20)
        
        # Store episode
        if brain.getEpisodicMemory():
            # Record working memory state
            wm_size = brain.getWorkingMemory().getActiveTraces()
            working_memory_trends.append(wm_size)
            
            # Record episodic memory
            em_count = brain.getEpisodicMemory().getEpisodeCount()
            episodic_memory_trends.append(em_count)
        
        # Periodic replay
        if episode % 10 == 0 and brain.getEpisodicMemory():
            episodes_to_replay = brain.getEpisodicMemory().getEpisodesForReplay(3)
            print(f"Episode {episode}: Replaying {len(episodes_to_replay)} episodes")
    
    print(f"Working memory evolution: {working_memory_trends[-10:]}")
    print(f"Episodic memory growth: {episodic_memory_trends[-10:]}")
    print(f"Final memory capacity: {brain.getWorkingMemory().getActiveTraces()}")
    print()


def example_5_development_process():
    """Example 5: Developmental process simulation."""
    print("=== Example 5: Development Process ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.maturity_age", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate developmental stages
    developmental_stages = []
    
    for age in range(0, 2500, 100):
        # Update development
        brain.develop()
        
        # Simulate neural activity
        for step in range(10):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
            brain.step(age + step)
        
        # Record developmental state
        stage = brain.getDevelopmentalStage()
        plasticity = brain.getStructuralPlasticity()
        synaptogenesis = plasticity.getSynaptogenesisRate() if plasticity else 0.0
        pruning = plasticity.getPruningRate() if plasticity else 0.0
        
        developmental_stages.append({
            'age': age,
            'stage': stage,
            'synaptogenesis_rate': synaptogenesis,
            'pruning_rate': pruning,
            'avg_firing_rate': brain.getAverageFiringRate()
        })
        
        if age % 500 == 0:
            print(f"Age {age}: {stage}, "
                  f"Synaptogenesis: {synaptogenesis:.6f}, "
                  f"Pruning: {pruning:.6f}, "
                  f"Firing rate: {brain.getAverageFiringRate():.3f}")
    
    print(f"\nDevelopment completed. Final stage: {brain.getDevelopmentalStage()}")
    print(f"Final plasticity parameters:")
    plasticity = brain.getStructuralPlasticity()
    if plasticity:
        print(f"  Synaptogenesis rate: {plasticity.getSynaptogenesisRate():.6f}")
        print(f"  Pruning rate: {plasticity.getPruningRate():.6f}")
    print()


def example_6_curiosity_driven_learning():
    """Example 6: Curiosity-driven learning."""
    print("=== Example 6: Curiosity-Driven Learning ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_strength", 0.7)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.5)
    config.set("neuromod.dopamine.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    curiosity_history = []
    novelty_history = []
    dopamine_history = []
    
    print("Running curiosity-driven exploration...")
    
    for step in range(1000):
        # Inject varied input to generate novelty
        if step % 10 == 0:
            # Present novel stimulus
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 25.0f)
        else:
            # Present familiar stimulus
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
        
        brain.step(step)
        
        # Record curiosity-driven metrics
        if brain.getCuriosity():
            curiosity_history.append(brain.getCuriosity().getLevel())
        if brain.getNovelty():
            novelty_history.append(brain.getNovelty().getLevel())
        if brain.getDopamine():
            dopamine_history.append(brain.getDopamine().getLevel())
        
        # Log progress
        if step % 200 == 0:
            curiosity = brain.getCuriosity().getLevel() if brain.getCuriosity() else 0.0
            novelty = brain.getNovelty().getLevel() if brain.getNovelty() else 0.0
            dopamine = brain.getDopamine().getLevel() if brain.getDopamine() else 0.0
            
            print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
                  f"Dopamine={dopamine:.3f}")
    
    print(f"\nCuriosity-driven learning completed.")
    print(f"Final curiosity level: {curiosity_history[-1]:.3f}")
    print(f"Average curiosity: {sum(curiosity_history)/len(curiosity_history):.3f}")
    print(f"Novelty exploration peak: {max(novelty_history):.3f}")
    print()


def example_7_prediction_learning():
    """Example 7: Prediction and prediction error learning."""
    print("=== Example 7: Prediction Learning ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("prediction.enable", True)
    config.set("prediction.horizon", 15)
    config.set("prediction.error_threshold", 0.1)
    config.set("neuromod.prediction_error.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    prediction_history = []
    error_history = []
    learning_events = []
    
    print("Running prediction learning...")
    
    for step in range(1500):
        # Simulate prediction-error cycle
        if step % 30 == 0:
            # Generate prediction error
            prediction = brain.getPredictionSystem()
            if prediction:
                # Simulate prediction
                predicted_value = 0.5 + (step % 100) * 0.01
                actual_value = predicted_value + (np.random.random() - 0.5) * 0.2
                
                error = abs(predicted_value - actual_value)
                prediction_history.append(predicted_value)
                error_history.append(error)
                
                # Apply prediction error learning
                if error > 0.1:
                    learning_events.append({
                        'step': step,
                        'error': error,
                        'action': 'prediction_error_learning'
                    })
                    brain.applyNeuromodulation(brain.getPredictionErrorSignal())
        
        # Normal brain processing
        brain.step(step)
        
        # Log progress
        if step % 300 == 0:
            avg_error = sum(error_history[-30:]) / min(30, len(error_history))
            print(f"Step {step}: Avg prediction error: {avg_error:.3f}")
    
    print(f"\nPrediction learning completed.")
    print(f"Total prediction events: {len(prediction_history)}")
    print(f"Average prediction error: {sum(error_history)/len(error_history):.3f}")
    print(f"Learning events triggered: {len(learning_events)}")
    print(f"Max prediction error: {max(error_history):.3f}")
    print()


def example_8_checkpointing():
    """Example 8: Checkpointing and saving/loading brain states."""
    print("=== Example 8: Checkpointing and Brain State Management ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run initial training
    print("Running initial training...")
    for step in range(200):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
        brain.step(step)
    
    # Save checkpoint
    checkpoint1 = "brain_checkpoint_1.bin"
    success = brain.save(checkpoint1)
    if success:
        print(f"Saved initial checkpoint to: {checkpoint1}")
    
    # Continue training
    print("Continuing training...")
    for step in range(200, 400):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 15.0f)
        brain.step(step)
    
    # Save another checkpoint
    checkpoint2 = "brain_checkpoint_2.bin"
    success = brain.save(checkpoint2)
    if success:
        print(f"Saved second checkpoint to: {checkpoint2}")
    
    # Get brain statistics for comparison
    print(f"\nCheckpoint statistics:")
    print(f"Checkpoint 1 total spikes: {brain.getTotalSpikeCount()} (would be from file)")
    print(f"Checkpoint 2 total spikes: {brain.getTotalSpikeCount()} (would be from file)")
    
    # Note: Loading from checkpoints would require creating new brain instances
    # and loading saved state - this would be done with brain.load()
    print("\nNote: Loading from checkpoints would use brain.load(filename)")
    print()


def example_9_performance_profiling():
    """Example 9: Performance profiling and analysis."""
    print("=== Example 9: Performance Profiling ===")
    
    # Test different brain sizes
    sizes = [500, 1000, 2000]
    results = []
    
    for size in sizes:
        print(f"\nTesting brain with {size} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initialization time
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Measure simulation performance
        start_time = time.time()
        for step in range(100):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
            brain.step(step)
        sim_time = time.time() - start_time
        
        results.append({
            'neurons': size,
            'init_time': init_time,
            'sim_time': sim_time,
            'steps_per_second': 100 / sim_time if sim_time > 0 else 0,
            'avg_firing_rate': brain.getAverageFiringRate()
        })
        
        print(f"  Initialization: {init_time:.3f}s")
        print(f"  Simulation: {sim_time:.3f}s (100 steps)")
        print(f"  Performance: {results[-1]['steps_per_second']:.1f} steps/sec")
    
    # Analyze results
    print(f"\nPerformance Analysis:")
    print(f"{'Neurons':<10} {'Init(s)':<10} {'Sim(s)':<10} {'Steps/sec':<12} {'Firing Rate':<12}")
    print("-" * 60)
    for result in results:
        print(f"{result['neurons']:<10} {result['init_time']:<10.3f} "
              f"{result['sim_time']:<10.3f} {result['steps_per_second']:<12.1f} "
              f"{result['avg_firing_rate']:<12.3f}")
    
    # Performance comparison
    if len(results) > 1:
        speedup = results[2]['steps_per_second'] / results[0]['steps_per_second']
        print(f"\nPerformance scaling (2000 neurons / 500 neurons): {speedup:.2f}x")
    print()


def example_10_advanced_techniques():
    """Example 10: Advanced techniques and patterns."""
    print("=== Example 10: Advanced Techniques ===")
    
    # Pattern 1: Multi-stage learning
    print("Pattern 1: Multi-stage learning")
    brain, config = create_advanced_brain(
        neuron_count=1200,
        enable_learning=True,
        enable_memory=True,
        enable_development=True
    )
    
    # Stage 1: Rapid initial learning
    print("  Stage 1: Rapid initial learning")
    for step in range(100):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step)
    
    # Stage 2: Consolidation
    print("  Stage 2: Consolidation phase")
    for step in range(100, 200):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 15.0f)
        brain.step(step)
    
    # Stage 3: Fine-tuning
    print("  Stage 3: Fine-tuning phase")
    for step in range(200, 300):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
        brain.step(step)
    
    print(f"  Final performance: {brain.getAverageFiringRate():.3f} firing rate")
    
    # Pattern 2: Meta-learning with curiosity
    print("\nPattern 2: Meta-learning with curiosity")
    brain2, config2 = create_curiosity_experiment(
        exploration_rate=0.4,
        novelty_sensitivity=0.6,
        reward_scaling=0.8
    )
    
    curiosity_scores = []
    for step in range(200):
        brain2.step(step)
        if brain2.getCuriosity():
            curiosity_scores.append(brain2.getCuriosity().getLevel())
    
    print(f"  Average curiosity: {sum(curiosity_scores)/len(curiosity_scores):.3f}")
    print(f"  Max curiosity: {max(curiosity_scores):.3f}")
    
    # Pattern 3: Ensemble methods (multiple brains)
    print("\nPattern 3: Ensemble methods")
    ensemble_brains = []
    ensemble_configs = []
    
    for i in range(5):
        brain_ens, config_ens = create_advanced_brain(
            neuron_count=800,
            connection_probability=0.12 + (i * 0.01)
        )
        ensemble_brains.append(brain_ens)
        ensemble_configs.append(config_ens)
    
    # Compare ensemble performance
    ensemble_performances = []
    for i, brain_ens in enumerate(ensemble_brains):
        for step in range(50):
            brain_ens.step(step)
        ensemble_performances.append(brain_ens.getAverageFiringRate())
    
    print(f"  Ensemble performances: {[f'{p:.3f}' for p in ensemble_performances]}")
    print(f"  Average ensemble: {sum(ensemble_performances)/len(ensemble_performances):.3f}")
    
    # Pattern 4: Transfer learning preparation
    print("\nPattern 4: Transfer learning preparation")
    base_brain, base_config = create_advanced_brain(
        neuron_count=1000,
        enable_memory=True,
        enable_planning=True
    )
    
    # Create task-specific modifications
    task_config = base_config.clone()  # Note: clone method doesn't exist, this would need implementation
    task_config.set("brain.neuron_count", 1200)  # Increase for specific task
    task_config.set("task_specific.parameter", "value")
    
    task_brain = pynlm.createBrain(task_config)
    task_brain.initialize()
    
    print(f"  Base brain: {base_brain.getTotalNeuronCount()} neurons")
    print(f"  Task-specific brain: {task_brain.getTotalNeuronCount()} neurons")
    print()


def main():
    """Run all examples."""
    print("NLM Advanced Usage Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_advanced_configuration,
        example_3_agent_environment_simulation,
        example_4_memory_systems,
        example_5_development_process,
        example_6_curiosity_driven_learning,
        example_7_prediction_learning,
        example_8_checkpointing,
        example_9_performance_profiling,
        example_10_advanced_techniques
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in example: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("All examples completed!")
    print("\nFor more information, see:")
    print("- nlm_cli.py: Command-line interface for running simulations")
    print("- pynlm_advanced.py: Advanced Python convenience functions")
    print("- Documentation files in the docs/ directory")


if __name__ == '__main__':
    main()
