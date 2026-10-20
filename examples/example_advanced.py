#!/usr/bin/env python3
"""
Additional NLM Python Examples
===============================

More specialized examples for advanced usage of NLM Python bindings.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from typing import List, Tuple

def example_memory_systems():
    """Example demonstrating memory system integration."""
    print("=== Memory Systems Example ===")
    
    # Create brain with memory-focused configuration
    config = pynlm.createDefaultConfig()
    config.set("working_memory_capacity", 200)
    config.set("episodic_max_episodes", 2000)
    config.set("connection_probability", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify memory systems are integrated
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"Memory system integration check:")
    print(f"  Working memory: {'✓' if working_memory else '✗'}")
    if working_memory:
        print(f"    Capacity: {working_memory.getCapacity()}")
        print(f"    Initial traces: {working_memory.getActiveTraces()}")
    
    print(f"  Episodic memory: {'✓' if episodic_memory else '✗'}")
    if episodic_memory:
        print(f"    Max episodes: {episodic_memory.getMaxEpisodes()}")
        print(f"    Initial episodes: {episodic_memory.getEpisodeCount()}")
    
    print(f"  Associative memory: {'✓' if associative_memory else '✗'}")
    
    # Run simulation with memory formation
    print("\nRunning memory formation simulation...")
    
    for step in range(300):
        # Inject sensory input to drive memory formation
        if step % 30 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 3.0)
        
        brain.step(step)
        
        # Record memory statistics
        if working_memory and episodic_memory:
            traces = working_memory.getActiveTraces()
            episodes = episodic_memory.getEpisodeCount()
            
            if step % 50 == 0:
                print(f"  Step {step}: {traces} memory traces, {episodes} episodes")
    
    print(f"\nFinal memory statistics:")
    if working_memory:
        print(f"  Working memory: {working_memory.getActiveTraces()} active traces")
    if episodic_memory:
        print(f"  Episodic memory: {episodic_memory.getEpisodeCount()} episodes")
        print(f"  Memory utilization: {episodic_memory.getEpisodeCount() / episodic_memory.getMaxEpisodes() * 100:.1f}%")
    
    print()

def example_neuromodulation():
    """Example demonstrating neuromodulation effects."""
    print("=== Neuromodulation Example ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    print(f"Neuromodulation system integration check:")
    print(f"  Dopamine: {'✓' if dopamine else '✗'}")
    if dopamine:
        print(f"    Current level: {dopamine.getLevel():.3f}")
        print(f"    Plasticity factor: {dopamine.getPlasticityFactor():.3f}")
    
    print(f"  Curiosity: {'✓' if curiosity else '✗'}")
    if curiosity:
        print(f"    Curiosity level: {curiosity.getLevel():.3f}")
    
    print(f"  Novelty: {'✓' if novelty else '✗'}")
    if novelty:
        print(f"    Novelty level: {novelty.getLevel():.3f}")
    
    # Run neuromodulation simulation
    print("\nRunning neuromodulation simulation...")
    
    for step in range(200):
        # Create conditions that activate neuromodulators
        if step % 40 == 0:
            # Sensory input triggers novelty detection
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 2.0)
        
        if step % 80 == 20:
            # Reward prediction error affects dopamine
            pass  # Would apply reward modulation
        
        brain.step(step)
        
        # Log neuromodulation state
        if step % 50 == 0 and dopamine and curiosity and novelty:
            print(f"  Step {step}:")
            print(f"    Dopamine: {dopamine.getLevel():.3f}")
            print(f"    Curiosity: {curiosity.getLevel():.3f}")
            print(f"    Novelty: {novelty.getLevel():.3f}")
    
    print()

def example_development():
    """Example demonstrating developmental processes."""
    print("=== Development Example ===")
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    config.set("development_enabled", True)
    config.set("development_age_rate", 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Run development simulation
    print("\nRunning developmental simulation...")
    
    stages = []
    for step in range(500):
        brain.step(step)
        
        current_stage = brain.getDevelopmentalStage()
        
        # Log development progress
        if step % 100 == 0:
            print(f"  Step {step}: Developmental stage {current_stage}")
            stages.append((step, current_stage))
    
    print(f"\nDevelopment progression:")
    for step, stage in stages:
        print(f"  Step {step}: {stage}")
    
    print(f"\nFinal developmental stage: {brain.getDevelopmentalStage()}")
    print()

def example_planning():
    """Example demonstrating cognitive planning."""
    print("=== Planning Example ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify planning system
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print(f"Cognition system integration check:")
    print(f"  Neural planner: {'✓' if planner else '✗'}")
    if planner:
        print(f"    Planning depth: {planner.getPlanningDepth()}")
    
    print(f"  Concept formation: {'✓' if concept_formation else '✗'}")
    
    print(f"  Attention system: {'✓' if attention else '✗'}")
    if attention:
        print(f"    Inhibition strength: {attention.getInhibitionStrength():.3f}")
        print(f"    Excitation strength: {attention.getExcitationStrength():.3f}")
    
    # Run planning simulation
    print("\nRunning planning simulation...")
    
    for step in range(400):
        # Inject input to activate cognition systems
        if step % 60 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 1.0)
        
        brain.step(step)
        
        # Log cognition state
        if step % 100 == 0:
            print(f"  Step {step}:")
            if planner:
                print(f"    Planner depth: {planner.getPlanningDepth()}")
            if attention:
                print(f"    Attention weights: {attention.getInhibitionStrength():.3f} / {attention.getExcitationStrength():.3f}")
    
    print()

def example_prediction():
    """Example demonstrating prediction systems."""
    print("=== Prediction Example ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify prediction system
    prediction_system = brain.getPredictionSystem()
    
    print(f"Prediction system integration check:")
    print(f"  Prediction system: {'✓' if prediction_system else '✗'}")
    
    # Run prediction simulation
    print("\nRunning prediction simulation...")
    
    for step in range(300):
        # Inject sensory input (triggers prediction)
        if step % 40 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 1.0)
        
        brain.step(step)
        
        # Log prediction state
        if step % 75 == 0:
            print(f"  Step {step}: Prediction system tracking")
    
    print()

def example_agent_integration():
    """Example demonstrating complete agent integration."""
    print("=== Complete Agent Integration Example ===")
    
    # Create complete agent system
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    # Add interesting world elements
    # Resource at center
    center_resource = pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Resource, value=20.0)
    world.addObject(center_resource)
    
    # Hazard in upper right
    hazard = pynlm.WorldObject(35.0, 35.0, pynlm.WorldObjectType.Hazard, value=-10.0)
    world.addObject(hazard)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems for complete behavior
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Created complete agent system:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  World: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  Agent: Active with all subsystems enabled")
    print(f"  Starting position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Run complete agent simulation
    print("\nRunning complete agent simulation...")
    
    exploration_metrics = []
    
    for step in range(600):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode and apply motor command
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Log metrics periodically
        if step % 100 == 0:
            metrics = {
                'step': step,
                'position': (world.getAgentBody().x, world.getAgentBody().y),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dopamine': agent.getNeuromodulationLevel(),
                'firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage(),
                'working_memory': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
            }
            exploration_metrics.append(metrics)
            
            print(f"  Step {step}:")
            print(f"    Position: ({metrics['position'][0]:.1f}, {metrics['position'][1]:.1f})")
            print(f"    Curiosity: {metrics['curiosity']:.3f}")
            print(f"    Novelty: {metrics['novelty']:.3f}")
            print(f"    Dopamine: {metrics['dopamine']:.3f}")
            print(f"    Firing rate: {metrics['firing_rate']:.3f} Hz")
    
    print(f"\nExploration complete!")
    print(f"Final metrics:")
    final = exploration_metrics[-1]
    print(f"  Final position: ({final['position'][0]:.1f}, {final['position'][1]:.1f})")
    print(f"  Final curiosity: {final['curiosity']:.3f}")
    print(f"  Final novelty: {final['novelty']:.3f}")
    print(f"  Final dopamine: {final['dopamine']:.3f}")
    print(f"  Final development stage: {final['development_stage']}")
    print(f"  Final working memory traces: {final['working_memory']}")
    
    print()

def example_batch_simulation():
    """Example demonstrating batch processing capabilities."""
    print("=== Batch Simulation Example ===")
    
    # Create batch configuration
    configs = []
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000 - i * 200)
        config.set("random_seed", 42 + i)
        configs.append(config)
    
    print(f"Created batch of {len(configs)} brain configurations:")
    
    brains = []
    for i, config in enumerate(configs):
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
        print(f"  Brain {i+1}: {brain.getTotalNeuronCount()} neurons")
    
    # Run synchronized batch simulation
    print("\nRunning synchronized batch simulation...")
    
    for step in range(100):
        for i, brain in enumerate(brains):
            brain.step(step)
        
        if step % 25 == 0:
            print(f"  Step {step}:")
            for i, brain in enumerate(brains):
                print(f"    Brain {i+1}: {brain.getTotalSpikeCount()} spikes, "
                      f"{brain.getAverageFiringRate():.3f} Hz")
    
    # Compare results
    print("\nBatch comparison results:")
    for i, brain in enumerate(brains):
        stats = {
            'spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'neurons': brain.getTotalNeuronCount(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
        print(f"  Brain {i+1}:")
        print(f"    Total spikes: {stats['spikes']}")
        print(f"    Average firing rate: {stats['firing_rate']:.3f} Hz")
        print(f"    Neuron count: {stats['neurons']}")
        print(f"    E/I ratio: {stats['e_i_ratio']:.2f}")
    
    print()

if __name__ == "__main__":
    print("Additional NLM Python Examples")
    print("=" * 40)
    print()
    
    try:
        example_memory_systems()
        example_neuromodulation()
        example_development()
        example_planning()
        example_prediction()
        example_agent_integration()
        example_batch_simulation()
        
        print("=" * 40)
        print("All additional examples completed successfully!")
        print()
        print("These examples demonstrate:")
        print("  - Memory system integration and formation")
        print("  - Neuromodulation (dopamine, curiosity, novelty)")
        print("  - Developmental processes")
        print("  - Cognitive planning and attention")
        print("  - Prediction systems")
        print("  - Complete agent-environment interaction")
        print("  - Batch processing capabilities")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)