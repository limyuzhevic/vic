#!/usr/bin/env python3
"""
NLM Complete Agent Example
=========================

This example demonstrates a complete agent simulation with:
- Neural processing
- Sensory perception  
- Motor control
- Reward-based learning
- Curiosity-driven exploration
- Development over time

The agent lives in a world and learns to survive through:
- Energy collection
- Hazard avoidance
- Novelty exploration
- Adaptive behavior
"""

import pynlm
import random

def complete_agent_example():
    print("=== NLM Complete Agent Example ===")
    print("Running a complete agent simulation...")
    
    # 1. Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setAgentStart(15.0, 15.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Setup complete:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  Agent: {agent.getSensoryInputSize()} sensory inputs, {agent.getMotorOutputSize()} motor outputs")
    print()
    
    # 6. Run simulation
    num_steps = 500
    print(f"Running {num_steps} simulation steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(action_result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Health: {world.getAgentBody().health:.1f}")
            print(f"  Age: {world.getAgentBody().age:.1f}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print()
    
    # 7. Final statistics
    print("=== Simulation Complete ===")
    print(f"Final brain statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print(f"\nFinal agent statistics:")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    print(f"\nWorld statistics:")
    print(f"  Agent position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
    print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    print(f"  Agent health: {world.getAgentBody().health:.1f}")
    print(f"  Agent age: {world.getAgentBody().age:.1f}")
    
    return brain, agent, world

def run_reproduction_experiment():
    print("\n" + "="*60)
    print("REPRODUCTION EXPERIMENT")
    print("="*60)
    print("Running the same simulation multiple times to check consistency...")
    
    results = []
    for run in range(3):
        print(f"\n--- Run {run + 1} ---")
        brain, agent, world = complete_agent_example()
        results.append({
            'run': run + 1,
            'final_energy': world.getAgentBody().energy,
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': agent.getDevelopmentalStage(),
            'dopamine_level': agent.getNeuromodulationLevel()
        })
    
    print("\n=== REPRODUCTION RESULTS ===")
    for result in results:
        print(f"Run {result['run']}: Energy={result['final_energy']:.1f}, "
              f"Spikes={result['total_spikes']}, "
              f"Stage={result['development_stage']}, "
              f"Dopamine={result['dopamine_level']:.3f}")
    
    return results

if __name__ == "__main__":
    complete_agent_example()
    
    # Optionally run reproduction experiment
    if "REPRODUCTION" in open(__file__).read():
        run_reproduction_experiment()