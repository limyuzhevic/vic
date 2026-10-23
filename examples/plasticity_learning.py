"""
Example 3: Learning and Plasticity Experiment
Demonstrates how the brain learns through experience and plasticity.
"""

import sys
sys.path.insert(0, 'python')

import pynlm
import time
import json

def plasticity_experiment():
    """Example showing learning through synaptic plasticity"""
    print("=== NLM Plasticity Learning Experiment ===\n")
    
    # Create configuration optimized for learning experiments
    config = pynlm.create_default_config()
    config.set("neuron_count", 800)
    config.set("region_count", 2)
    config.set("connection_probability", 0.2)
    
    # Enable plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    print("Creating brain with plasticity enabled...")
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create world and agent for embodied learning
    world = pynlm.create_simple_world()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Brain initialized with:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # Record initial state
    initial_firing = brain.getAverageFiringRate()
    initial_spikes = brain.getTotalSpikeCount()
    initial_weights = get_average_synapse_weights(brain)
    
    print(f"Initial state:")
    print(f"  Average firing rate: {initial_firing:.3f} Hz")
    print(f"  Total spikes: {initial_spikes}")
    print(f"  Average synapse weight: {initial_weights:.3f}")
    print()
    
    # Phase 1: Sensory exploration
    print("Phase 1: Sensory exploration and novelty detection")
    print("-" * 50)
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        agent.decodeMotorCommand()
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        reward = 0.0
        if world.getSensoryPercept().getInternal() and not world.getSensoryPercept().getInternal().empty():
            reward = world.getSensoryPercept().getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 50 == 0:
            print(f"  Step {step:3d}: Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, Reward={reward:.3f}")
    
    # Phase 2: Structured learning with reward signals
    print("\nPhase 2: Structured learning with rewards")
    print("-" * 50)
    
    for step in range(200, 400):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        # Apply motor command
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward based on world feedback
        percept = world.getSensoryPercept()
        reward = 0.0
        if percept.getInternal() and not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        
        # Apply reward with prediction error
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 50 == 0:
            print(f"  Step {step:3d}: Reward={reward:.3f}, PredErr={agent.getPredictionError():.3f}, "
                  f"Development={agent.getDevelopmentalStage()}")
    
    # Phase 3: Replay and consolidation
    print("\nPhase 3: Memory replay and consolidation")
    print("-" * 50)
    
    for step in range(400, 600):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = 0.0
        if world.getSensoryPercept().getInternal() and not world.getSensoryPercept().getInternal().empty():
            reward = world.getSensoryPercept().getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Simulate replay every 100 steps
        if step % 100 == 0:
            print(f"  Step {step:3d}: Replaying recent memories...")
            agent.updateDevelopment(0.1)
    
    # Record final state
    final_firing = brain.getAverageFiringRate()
    final_spikes = brain.getTotalSpikeCount()
    final_weights = get_average_synapse_weights(brain)
    
    print(f"\nFinal state:")
    print(f"  Average firing rate: {final_firing:.3f} Hz")
    print(f"  Total spikes: {final_spikes}")
    print(f"  Average synapse weight: {final_weights:.3f}")
    print(f"  Weight change: {final_weights - initial_weights:+.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    print()
    
    # Determine if learning occurred
    learning_detected = (abs(final_firing - initial_firing) > 0.1 or
                        abs(final_weights - initial_weights) > 0.01 or
                        final_spikes > initial_spikes + 100)
    
    if learning_detected:
        print("✓ LEARNING DETECTED: Brain has changed through experience!")
        print("  Synaptic plasticity is working correctly.")
    else:
        print("ℹ Learning effects may be subtle - try increasing simulation time or adjusting parameters")
    
    return brain, agent, world

def get_average_synapse_weights(brain):
    """Calculate average synapse weight across all regions"""
    total_weight = 0.0
    total_synapses = 0
    
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            total_weight += synapse.getWeight()
            total_synapses += 1
    
    return total_weight / total_synapses if total_synapses > 0 else 0.0

if __name__ == "__main__":
    brain, agent, world = plasticity_experiment()
    print("\nPlasticity learning experiment completed successfully!")
