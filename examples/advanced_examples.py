#!/usr/bin/env python3

"""
Advanced NLM Phase 6 Examples

This module provides comprehensive examples of using NLM Phase 6
for advanced users who want to explore the full capabilities of
the integrated artificial brain system.
"""

import pynlm
import json
import os
import time
from typing import List, Dict, Optional

def save_experiment_results(results: Dict, filename: str):
    """Save experiment results to JSON file."""
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"Results saved to {filename}")

def load_experiment_results(filename: str) -> Optional[Dict]:
    """Load experiment results from JSON file."""
    if os.path.exists(filename):
        with open(filename, 'r') as f:
            return json.load(f)
    return None

def example_advanced_agent():
    """
    Example: Advanced agent with all learning mechanisms enabled.
    
    This demonstrates how to create an agent that uses all Phase 6 systems:
    - Reward modulation with prediction errors
    - Structural plasticity for learning new connections
    - Development stages affecting plasticity
    - Curiosity-driven exploration
    - Working memory integration
    - Episodic memory storage
    """
    print("=== Advanced Agent Example ===")
    
    # Configure for advanced learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize systems
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    if dopamine:
        dopamine.setLevel(0.5)
    if curiosity:
        curiosity.setExplorationDrive(0.7)
    if novelty:
        novelty.setSensitivity(0.8)
    
    print("Created advanced agent with full Phase 6 integration")
    
    # Run extended simulation
    simulation_results = {
        "episodes": [],
        "memory_stats": [],
        "learning_metrics": []
    }
    
    for episode in range(5):
        episode_start = time.time()
        episode_data = {
            "episode": episode + 1,
            "steps": [],
            "final_state": {},
            "rewards": []
        }
        
        # Reset environment
        world.reset()
        agent.reset()
        
        # Run episode
        for step in range(200):
            # World update
            world.update(0.1)
            
            # Get observations and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get action and execute
            motor_cmd = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(
                action_result.reward,
                agent.getPredictionError()
            )
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record step data
            step_data = {
                "step": step,
                "time": world.getSimulationTime(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "spike_count": brain.getTotalSpikeCount(),
                "development_stage": agent.getDevelopmentalStage(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError(),
                "reward": action_result.reward,
                "success": action_result.success
            }
            episode_data["steps"].append(step_data)
            episode_data["rewards"].append(action_result.reward)
        
        # Record episode final state
        episode_data["final_state"] = {
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "firing": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "development": agent.getDevelopmentalStage(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError()
        }
        
        simulation_results["episodes"].append(episode_data)
        
        episode_duration = time.time() - episode_start
        print(f"Episode {episode + 1} completed in {episode_duration:.2f}s")
        print(f"  Total spikes: {episode_data['final_state']['total_spikes']}")
        print(f"  Development stage: {episode_data['final_state']['development']}")
        print(f"  Final curiosity: {episode_data['final_state']['curiosity']:.3f}")
        
        if episode % 2 == 0:
            # Check memory system status
            working_memory = brain.getWorkingMemory()
            episodic_memory = brain.getEpisodicMemory()
            prediction = brain.getPredictionSystem()
            
            memory_stats = {
                "episode": episode + 1,
                "working_memory_active_traces": working_memory.getActiveTraces() if working_memory else 0,
                "episodic_memory_count": episodic_memory.getEpisodeCount() if episodic_memory else 0,
                "prediction_system_active": prediction is not None,
                "average_firing_rate": brain.getAverageFiringRate()
            }
            simulation_results["memory_stats"].append(memory_stats)
    
    # Calculate learning metrics
    all_rewards = []
    for episode in simulation_results["episodes"]:
        all_rewards.extend(episode["rewards"])
    
    learning_metrics = {
        "average_reward": sum(all_rewards) / len(all_rewards) if all_rewards else 0,
        "reward_variance": sum((r - (sum(all_rewards) / len(all_rewards))) ** 2 for r in all_rewards) / len(all_rewards) if all_rewards else 0,
        "total_spikes": brain.getTotalSpikeCount(),
        "final_development_stage": agent.getDevelopmentalStage(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
        "final_prediction_error": agent.getPredictionError()
    }
    
    simulation_results["learning_metrics"] = learning_metrics
    
    # Save results
    save_experiment_results(simulation_results, "advanced_agent_results.json")
    
    print(f"\nAdvanced agent simulation complete!")
    print(f"Total simulation time: {len(all_rewards)} steps")
    print(f"Average reward per step: {learning_metrics['average_reward']:.4f}")
    print(f"Final development stage: {learning_metrics['final_development_stage']}")
    
    return brain, world

def example_memory_integration():
    """
    Example: Demonstrating memory system integration with neural activity.
    
    This example shows how working memory, episodic memory, and
    episodic memory work together to create a coherent memory system.
    """
    print("\n=== Memory Integration Example ===")
    
    # Create brain with memory systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check memory system availability
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    prediction = brain.getPredictionSystem()
    
    print(f"Memory systems available:")
    print(f"  Working memory: {'Yes' if working_memory else 'No'}")
    print(f"  Episodic memory: {'Yes' if episodic_memory else 'No'}")
    print(f"  Associative memory: {'Yes' if associative_memory else 'No'}")
    print(f"  Prediction system: {'Yes' if prediction else 'No'}")
    
    # Run neural activity simulation
    memory_integration_results = {
        "activity_patterns": [],
        "memory_formation": [],
        "pattern_association": []
    }
    
    for pattern in range(5):
        # Inject pattern into sensory neurons
        for step in range(50):
            # Create a simple pattern (sine wave simulation)
            pattern_value = 0.5 + 0.5 * ((step % 10) / 10.0)
            
            # Inject into sensory neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, pattern_value)
            
            # Run brain step
            brain.step(step)
            
            # Store in working memory
            if working_memory:
                # Store neural activity pattern
                firing_neurons = [n.getId().index() for n in brain.getRegions()[0].getPopulations()[0].getNeurons() 
                                 if n.isFiring()]
                if firing_neurons:
                    # Normalize pattern
                    pattern_vector = [float(nid) / 1000.0 for nid in firing_neurons]
                    # Store first neuron's activity
                    working_memory.storeToNeuron(brain.getRegions()[0].getPopulations()[0].getNeurons()[0]->getId(), 
                                               pattern_vector[0] if pattern_vector else 0.0)
        
        # Store episodic memory
        if episodic_memory and pattern % 2 == 0:
            from nlm import EpisodicMemoryItem
            item = EpisodicMemoryItem()
            item.timestamp = pattern * 50
            item.metadata = f"Pattern {pattern} activation"
            episodic_memory.storeEpisode(item)
            
            memory_formation = {
                "pattern": pattern,
                "episode_count": episodic_memory.getEpisodeCount(),
                "timestamp": item.timestamp
            }
            memory_integration_results["memory_formation"].append(memory_formation)
            print(f"Stored episodic memory for pattern {pattern}")
        
        # Create associations
        if associative_memory and pattern > 0:
            # Associate this pattern with previous ones
            for prev_pattern in range(pattern):
                strength = 0.1 * (pattern - prev_pattern) / (pattern + 1)
                associative_memory.associate(
                    brain.getRegions()[0].getPopulations()[0].getNeurons()[0]->getId(),
                    brain.getRegions()[0].getPopulations()[0].getNeurons()[1]->getId(),
                    strength
                )
                
                pattern_association = {
                    "source_pattern": prev_pattern,
                    "target_pattern": pattern,
                    "strength": strength
                }
                memory_integration_results["pattern_association"].append(pattern_association)
    
    print(f"\nMemory integration complete!")
    print(f"Total episodic memories stored: {memory_integration_results['memory_formation'][-1]['episode_count'] if memory_integration_results['memory_formation'] else 0}")
    print(f"Pattern associations created: {len(memory_integration_results['pattern_association'])}")
    
    return brain

def example_development_dynamics():
    """
    Example: Exploring developmental dynamics and plasticity changes.
    
    This example shows how the brain develops over time, with changing
    plasticity rates, neuromodulation effects, and learning capabilities.
    """
    print("\n=== Development Dynamics Example ===")
    
    # Create brain with development
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all development systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Developmental stage progression:")
    print(f"{'Time (s)':<10} {'Stage':<15} {'Plasticity':<12} {'Curiosity':<10} {'Novelty':<10}")
    print("-" * 60)
    
    developmental_results = {
        "stages": [],
        "plasticity_changes": [],
        "neuromodulation_effects": []
    }
    
    for timestep in range(0, 1200, 100):
        # Update development
        agent.updateDevelopment(timestep)
        
        # Get system status
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        
        stage_info = {
            "time": timestep,
            "stage": agent.getDevelopmentalStage(),
            "plasticity_modifier": agent.getBrain().getDevelopmentSystem().getPlasticityModifier(),
            "curiosity": curiosity.getExplorationDrive() if curiosity else 0,
            "novelty": novelty.getNoveltyLevel() if novelty else 0,
            "dopamine": dopamine.getLevel() if dopamine else 0
        }
        
        developmental_results["stages"].append(stage_info)
        
        # Check plasticity effects
        if timestep % 200 == 0:
            # Measure learning rate
            learning_rate = 0.0
            if dopamine:
                learning_rate = dopamine.getLevel() * 0.1
            
            plasticity_change = {
                "time": timestep,
                "learning_rate": learning_rate,
                "stage": agent.getDevelopmentalStage()
            }
            developmental_results["plasticity_changes"].append(plasticity_change)
        
        # Record neuromodulation effects
        neuromod_effect = {
            "time": timestep,
            "dopamine_level": dopamine.getLevel() if dopamine else 0,
            "curiosity_level": curiosity.getExplorationDrive() if curiosity else 0,
            "novelty_level": novelty.getNoveltyLevel() if novelty else 0
        }
        developmental_results["neuromodulation_effects"].append(neuromod_effect)
        
        print(f"{timestep:<10} {agent.getDevelopmentalStage():<15} "
              f"{agent.getBrain().getDevelopmentSystem().getPlasticityModifier():<12.3f} "
              f"{curiosity.getExplorationDrive() if curiosity else 0:<10.3f} "
              f"{novelty.getNoveltyLevel() if novelty else 0:<10.3f}")
    
    print(f"\nDevelopment dynamics complete!")
    print(f"Total developmental stages recorded: {len(developmental_results['stages'])}")
    print(f"Plasticity change events: {len(developmental_results['plasticity_changes'])}")
    
    return brain, agent

def example_checkpoint_comprehensive():
    """
    Example: Comprehensive checkpointing and brain persistence.
    
    This example demonstrates the complete checkpoint system,
    including saving, loading, and version management.
    """
    print("\n=== Comprehensive Checkpointing Example ===")
    
    # Create and train brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    print("Training initial brain...")
    for step in range(100):
        brain1.step(step)
        if step % 25 == 0:
            print(f"  Step {step}: {brain1.getTotalSpikeCount()} total spikes")
    
    # Save checkpoint
    checkpoint1 = "brain_checkpoint_1.bin"
    save1 = brain1.save(checkpoint1)
    print(f"✓ Initial brain saved to {checkpoint1}")
    
    # Continue training
    print("\nContinuing training...")
    for step in range(100, 200):
        brain1.step(step)
    
    # Save second checkpoint
    checkpoint2 = "brain_checkpoint_2.bin"
    save2 = brain1.save(checkpoint2)
    print(f"✓ Intermediate brain saved to {checkpoint2}")
    
    # Create new brain and load first checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    load1 = brain2.load(checkpoint1)
    print(f"✓ Loaded first checkpoint: {'Success' if load1 else 'Failed'}")
    
    # Continue training loaded brain
    print("\nTraining loaded brain...")
    for step in range(200, 250):
        brain2.step(step)
    
    # Save loaded brain state
    checkpoint3 = "brain_checkpoint_3.bin"
    save3 = brain2.save(checkpoint3)
    print(f"✓ Loaded brain saved to {checkpoint3}")
    
    # Compare brain states
    print(f"\nBrain state comparison:")
    print(f"Brain 1 - Neurons: {brain1.getTotalNeuronCount()}, Synapses: {brain1.getTotalSynapseCount()}, Spikes: {brain1.getTotalSpikeCount()}")
    print(f"Brain 2 - Neurons: {brain2.getTotalNeuronCount()}, Synapses: {brain2.getTotalSynapseCount()}, Spikes: {brain2.getTotalSpikeCount()}")
    
    # Clean up checkpoints
    for checkpoint in [checkpoint1, checkpoint2, checkpoint3]:
        if os.path.exists(checkpoint):
            os.remove(checkpoint)
            print(f"Cleaned up {checkpoint}")
    
    print(f"\nComprehensive checkpointing complete!")
    
    return brain1, brain2

def example_advanced_configuration():
    """
    Example: Advanced configuration management and parameter tuning.
    
    This example shows how to create custom configurations for
    specific experimental conditions and parameter optimization.
    """
    print("\n=== Advanced Configuration Example ===")
    
    # Define different experimental conditions
    experimental_conditions = {
        "learning_rate": {"stdp_ltp_weight": 0.02, "stdp_ltd_weight": 0.015, "plasticity_factor": 1.0},
        "fast_learning": {"stdp_ltp_weight": 0.05, "stdp_ltd_weight": 0.04, "plasticity_factor": 1.5},
        "slow_learning": {"stdp_ltp_weight": 0.01, "stdp_ltd_weight": 0.008, "plasticity_factor": 0.5},
        "balanced": {"stdp_ltp_weight": 0.02, "stdp_ltd_weight": 0.02, "plasticity_factor": 1.0}
    }
    
    results = {
        "conditions": [],
        "performance_comparison": []
    }
    
    for condition_name, params in experimental_conditions.items():
        print(f"\nTesting condition: {condition_name}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 400)
        config.set("brain.synapse_density", 0.12)
        config.set("stdp_ltp_weight", params["stdp_ltp_weight"])
        config.set("stdp_ltd_weight", params["stdp_ltd_weight"])
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Enable features based on condition
        if "fast_learning" in condition_name:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
        elif "slow_learning" in condition_name:
            agent.enableDevelopment(True)
        
        # Run simulation
        condition_results = {
            "name": condition_name,
            "parameters": params,
            "episodes": [],
            "final_metrics": {}
        }
        
        for episode in range(3):
            # Simple simulation loop
            for step in range(50):
                brain.step(step)
            
            episode_result = {
                "episode": episode + 1,
                "total_spikes": brain.getTotalSpikeCount(),
                "learning_rate": params["plasticity_factor"],
                "development_stage": agent.getDevelopmentalStage()
            }
            condition_results["episodes"].append(episode_result)
        
        # Calculate condition metrics
        total_spikes = sum(e["total_spikes"] for e in condition_results["episodes"])
        avg_learning_rate = sum(e["learning_rate"] for e in condition_results["episodes"]) / len(condition_results["episodes"])
        
        condition_results["final_metrics"] = {
            "total_spikes": total_spikes,
            "average_learning_rate": avg_learning_rate,
            "development_stage": agent.getDevelopmentalStage(),
            "curiosity_level": agent.getCuriosityLevel()
        }
        
        results["conditions"].append(condition_results)
        results["performance_comparison"].append({
            "condition": condition_name,
            "total_spikes": total_spikes,
            "learning_rate": avg_learning_rate,
            "development_progress": agent.getDevelopmentalStage()
        })
        
        print(f"  Total spikes: {total_spikes}")
        print(f"  Average learning rate: {avg_learning_rate:.3f}")
        print(f"  Final development stage: {agent.getDevelopmentalStage()}")
    
    # Save comparison results
    save_experiment_results(results, "advanced_configuration_results.json")
    
    print(f"\nAdvanced configuration testing complete!")
    print(f"Tested {len(results['conditions'])} experimental conditions")
    
    return results

def main_advanced_examples():
    """Run all advanced examples."""
    print("NLM Phase 6 Advanced Examples")
    print("=" * 50)
    
    # Example 1: Advanced agent
    brain1, world1 = example_advanced_agent()
    
    # Example 2: Memory integration
    brain2 = example_memory_integration()
    
    # Example 3: Development dynamics
    brain3, agent3 = example_development_dynamics()
    
    # Example 4: Comprehensive checkpointing
    brain4, brain5 = example_checkpoint_comprehensive()
    
    # Example 5: Advanced configuration
    config_results = example_advanced_configuration()
    
    print("\n" + "=" * 50)
    print("All advanced examples completed successfully!")
    print("=" * 50)
    
    return {
        "advanced_agent": brain1,
        "memory_integration": brain2,
        "development_dynamics": brain3,
        "checkpointing": (brain4, brain5),
        "configuration": config_results
    }

if __name__ == "__main__":
    main_advanced_examples()
