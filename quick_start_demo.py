#!/usr/bin/env python3
"""
Enhanced NLM Python API - Quick Start Demonstration

This script provides a quick demonstration of the enhanced NLM Python API,
showing the key improvements and new capabilities.

Author: NLM Development Team
Version: 2.0.0
"""

import pynlm
import time

def main():
    print("="*70)
    print("Enhanced NLM Python API - Quick Start Demonstration")
    print("="*70)
    print("\nThis demonstration shows the enhanced NLM Python API capabilities.")
    print("\nKey improvements:")
    print("  • Complete neuroscience class bindings (Neuron, Synapse, etc.)")
    print("  • Advanced memory systems (Working Memory, Episodic Memory)")
    print("  • Neuromodulation systems (Dopamine, Curiosity, Novelty)")
    print("  • Enhanced simulation controls and data export")
    print("  • Comprehensive documentation and examples")
    print("  • Robust error handling and validation")
    
    print("\n" + "="*70)
    print("1. Basic Enhanced Simulation")
    print("="*70)
    
    # Create enhanced configuration
    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 500,
        'brain.synapse_density': 0.15,
        'plasticity.stdp.enable': True,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"and {brain.getTotalSynapseCount()} synapses")
    
    # Run basic simulation
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("\n" + "="*70)
    print("2. Memory System Demonstration")
    print("="*70)
    
    # Create memory-enabled brain
    config2 = pynlm.createDefaultConfig()
    config2.batch_set({
        'brain.neuron_count': 200,
        'neuromod.curiosity.enable': True
    })
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    working_memory = brain2.getWorkingMemory()
    episodic_memory = brain2.getEpisodicMemory()
    
    print("Working memory demonstration:")
    working_memory.store_to_neuron(1, 0.8)
    working_memory.update(0.1)
    print(f"  - Active traces: {working_memory.getActiveTraces()}")
    print(f"  - Neuron 1 activation: {working_memory.getActivation(1):.3f}")
    
    print("\nEpisodic memory demonstration:")
    episode = pynlm.EpisodicMemoryItem()
    episode.set_position(3.0, 4.0)
    episode.set_action(pynlm.ActionType.MoveForward)
    episode.set_reward(0.8)
    episodic_memory.store_episode(episode)
    print(f"  - Episode stored at ({episode.get_position()[0]}, {episode.get_position()[1]})")
    print(f"  - Action: {episode.get_action().name}")
    print(f"  - Reward: {episode.get_reward():.3f}")
    
    print("\n" + "="*70)
    print("3. Neuromodulation Demonstration")
    print("="*70)
    
    config3 = pynlm.createDefaultConfig()
    config3.batch_set({
        'brain.neuron_count': 300,
        'neuromod.dopamine.scale': 1.5,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True
    })
    
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    dopamine = brain3.getDopamine()
    curiosity = brain3.getCuriosity()
    novelty = brain3.getNoveltyDetector()
    
    print("Neuromodulation levels:")
    for step in range(20):
        brain3.step(step)
        novelty.update([0.5, 0.3, 0.7, 0.2, 0.8], 0.1)
        curiosity.update(novelty.getLevel(), 0.0, 0.1)
        dopamine.update(0.1)
        
        if step % 5 == 0:
            print(f"  Step {step}: curiosity={curiosity.getLevel():.3f}, "
                  f"novelty={novelty.getLevel():.3f}, dopamine={dopamine.getLevel():.3f}")
    
    print(f"\nHigh curiosity: {curiosity.getLevel() > 0.5}")
    print(f"High novelty: {novelty.getLevel() > 0.7}")
    print(f"Positive prediction error: {dopamine.getLevel() > 0.0}")
    
    print("\n" + "="*70)
    print("4. Agent-World Simulation")
    print("="*70)
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    config4 = pynlm.createDefaultConfig()
    config4.batch_set({
        'brain.neuron_count': 400,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'neuromod.dopamine.scale': 1.2
    })
    
    brain4 = pynlm.createBrain(config4)
    agent = pynlm.createAgentBrain(brain4)
    agent.initialize(world)
    
    # Enhanced simulation
    sim = pynlm.createSimulationController(brain4, world, agent)
    print("Running curiosity-driven simulation...")
    
    start_time = time.time()
    sim.run_curiosity_driven(40.0, curiosity_threshold=0.3)
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Agent final state:")
    print(f"  - Developmental stage: {agent.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print("\n" + "="*70)
    print("5. Data Export Demonstration")
    print("="*70)
    
    # Create brain for data export
    config5 = pynlm.createDefaultConfig()
    config5.batch_set({
        'brain.neuron_count': 100,
        'simulation_timestep': 0.01
    })
    
    brain5 = pynlm.createBrain(config5)
    brain5.initialize()
    
    print("Running simulation and exporting data...")
    for step in range(30):
        brain5.step(step)
    
    # Export data
    pynlm.export_neural_data(brain5, "demo_neural_activity.csv", window=30)
    pynlm.export_synapse_weights(brain5, "demo_synaptic_connections.csv")
    pynlm.plot_firing_rates(brain5, "demo_firing_rates.png")
    
    print("Data exported to:")
    print("  - demo_neural_activity.csv")
    print("  - demo_synaptic_connections.csv")
    print("  - demo_firing_rates.png")
    
    print("\n" + "="*70)
    print("Enhanced NLM Python API Demonstration Complete!")
    print("="*70)
    print("\nKey features demonstrated:")
    print("  ✓ Enhanced brain configuration and initialization")
    print("  ✓ Working memory and episodic memory systems")
    print("  ✓ Neuromodulation (Dopamine, Curiosity, Novelty)")
    print("  ✓ Agent-world interaction with curiosity-driven behavior")
    print("  ✓ Data export and visualization capabilities")
    
    print("\nFor more information and examples, see:")
    print("  - enhanced_python_api_documentation.md")
    print("  - FINAL_IMPLEMENTATION_SUMMARY.md")
    print("  - NLM_PYTHON_IMPROVEMENT_PLAN.md")
    
    print(f"\nTotal demonstration time: {end_time - start_time:.2f} seconds")
    print("\nThe enhanced NLM Python API is ready for use!")

if __name__ == "__main__":
    main()