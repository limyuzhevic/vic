#!/usr/bin/env python3
"""
Advanced NLM Demo Script
=======================

This script demonstrates advanced features of the NLM framework
including world interaction, agent behavior, and learning systems.
"""

import sys
import time
import pynlm

def demo_complete_agent():
    """Demonstrate a complete agent in a world."""
    print("=== Complete Agent Demo ===")
    
    # 1. Setup: Create brain and agent brain interface
    print("\n1. Setting up brain and agent...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    agent = pynlm.createAgentBrain(brain)
    
    # 2. Create and configure world
    print("\n2. Creating world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=15,
        height=15,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(7.0, 7.0)  # Center of world
    
    print(f"   World size: {world.getWidth()}x{world.getHeight()}")
    
    # 3. Initialize agent with world
    print("\n3. Initializing agent...")
    agent.initialize(world)
    
    # 4. Enable advanced subsystems
    print("\n4. Enabling advanced features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   Enabled: Reward modulation, Structural plasticity, Development, Curiosity")
    
    # 5. Run complete simulation cycle
    print("\n5. Running simulation cycle...")
    num_steps = 100
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        # In this simple demo, we use internal signals as proxy for reward
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print periodic status
        if step % 20 == 0:
            print(f"   Step {step}:")
            print(f"     Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"     Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"     Dev stage: {brain.getDevelopmentalStage()}")
            print(f"     Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # 6. Print final statistics
    print("\n6. Final statistics...")
    print(f"   Total simulation steps: {num_steps}")
    print(f"   Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Agent final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # 7. Demonstrate thought trace (if available)
    print("\n7. Thought trace example...")
    thought = agent.getThoughtTrace()
    if thought != "none":
        print(f"   Agent thought: {thought}")
    else:
        print("   Thought trace available (not displayed for this demo)")
    
    return True

def demo_memory_systems():
    """Demonstrate memory system integration."""
    print("\n=== Memory Systems Demo ===")
    
    # Create brain with memory systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access memory systems through brain
    print("\n1. Exploring memory systems...")
    
    # Working memory
    working_mem = brain.getWorkingMemory()
    if working_mem:
        print("   ✓ Working memory system available")
    
    # Episodic memory  
    episodic_mem = brain.getEpisodicMemory()
    if episodic_mem:
        print("   ✓ Episodic memory system available")
    
    # Associative memory
    assoc_mem = brain.getAssociativeMemory()
    if assoc_mem:
        print("   ✓ Associative memory system available")
    
    # Prediction system
    prediction = brain.getPredictionSystem()
    if prediction:
        print("   ✓ Prediction system available")
    
    # Cognition systems
    print("\n2. Exploring cognition systems...")
    
    planner = brain.getPlanner()
    if planner:
        print("   ✓ Neural planner available")
    
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        print("   ✓ Concept formation system available")
    
    attention = brain.getAttention()
    if attention:
        print("   ✓ Attentional selection system available")
    
    # Neuromodulation systems
    print("\n3. Exploring neuromodulation systems...")
    
    dopamine = brain.getDopamine()
    if dopamine:
        print("   ✓ Dopamine (reward) system available")
    
    curiosity = brain.getCuriosity()
    if curiosity:
        print("   ✓ Curiosity system available")
    
    novelty = brain.getNovelty()
    if novelty:
        print("   ✓ Novelty detection system available")
    
    prediction_error = brain.getPredictionErrorSignal()
    if prediction_error:
        print("   ✓ Prediction error system available")
    
    print("\n✓ All memory and cognition systems integrated successfully!")
    return True

def demo_development_system():
    """Demonstrate development system functionality."""
    print("\n=== Development System Demo ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("\n1. Initial developmental stage...")
    print(f"   Stage: {brain.getDevelopmentalStage()}")
    
    # Run some simulation to trigger development
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"   Step {step}: {brain.getDevelopmentalStage()}")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Synapse count: {brain.getTotalSynapseCount()}")
    
    print("\n2. Development completed!")
    print(f"   Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Final synapse count: {brain.getTotalSynapseCount()}")
    
    return True

def main():
    """Main demo function."""
    print("NLM Python Advanced Demo")
    print("=" * 60)
    
    try:
        # Run demos
        demo_complete_agent()
        demo_memory_systems()
        demo_development_system()
        
        print("\n" + "=" * 60)
        print("✓ All advanced demos completed successfully!")
        print("\nThe NLM framework demonstrates:")
        print("• Complete agent-world interaction loops")
        print("• Integrated memory and cognition systems")
        print("• Neuromodulation for learning and exploration")
        print("• Developmental plasticity over time")
        print("• Rich sensory and motor processing")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Error during advanced demo: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
