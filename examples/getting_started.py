#!/usr/bin/env python3
"""
NLM Python API - Getting Started Tutorial

This tutorial guides you through the basics of using the NLM Python API.
Each section builds on the previous one.

Author: NLM Development Team
Version: 0.1.0
"""

import pynlm

def section_1_basic_brain():
    """Section 1: Create and run a basic neural brain"""
    print("=" * 60)
    print("SECTION 1: BASIC BRAIN CREATION")
    print("=" * 60)
    
    # Create brain
    print("\n1. Creating a brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    print("2. Initializing brain...")
    brain.initialize()
    
    print(f"3. Brain created with {brain.getTotalNeuronCount()} neurons!")
    
    # Run brain
    print("\n4. Running brain for 50 steps...")
    for i in range(50):
        brain.step(i)
    
    # Results
    print(f"\nResults:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

def section_2_world_simulation():
    """Section 2: Brain in a simulated world"""
    print("\n" + "=" * 60)
    print("SECTION 2: WORLD SIMULATION")
    print("=" * 60)
    
    # Setup
    print("\n1. Creating brain and world...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"   World size: {world.getWidth()} x {world.getHeight()}")
    print(f"   Vision range: {world.getWidth()/world.getVisionWidth():.1f}m per pixel")
    
    # Run simulation
    print("\n2. Running simulation for 30 steps...")
    for step in range(30):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Results
    print("\nResults:")
    pos = world.getAgentBody()
    print(f"   Agent final position: ({pos.x:.1f}, {pos.y:.1f})")
    print(f"   Brain activity: {brain.getFiringNeuronCount()} neurons firing")
    print(f"   Total steps completed: 30")
    
    return brain, world, agent

def section_3_learning_systems():
    """Section 3: Enable learning systems"""
    print("\n" + "=" * 60)
    print("SECTION 3: ENABLING LEARNING SYSTEMS")
    print("=" * 60)
    
    # Setup with learning
    print("\n1. Creating brain with learning subsystems...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    print("2. Enabling learning systems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("   ✓ Reward modulation (reinforcement learning)")
    print("   ✓ Curiosity (exploration drives)")
    print("   ✓ Development (structural changes over time)")
    
    # Run learning simulation
    print("\n3. Running learning simulation...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            curiosity = agent.getCuriosityLevel()
            dev_stage = brain.getDevelopmentalStage()
            print(f"   Step {step}: Curiosity={curiosity:.2f}, Stage={dev_stage}")
    
    # Results
    print("\nLearning Results:")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final development stage: {brain.getDevelopmentalStage()}")
    print(f"   Total learning steps: 50")
    
    return brain, world, agent

def section_4_advanced_usage():
    """Section 4: Advanced usage patterns"""
    print("\n" + "=" * 60)
    print("SECTION 4: ADVANCED USAGE PATTERNS")
    print("=" * 60)
    
    print("\n1. Using custom configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Created brain with {config.getOr('brain.neuron_count', 500)} neurons")
    print(f"   STDP enabled: {config.getOr('plasticity.stdp.enable', True)}")
    print(f"   Hebbian enabled: {config.getOr('plasticity.hebbian.enable', True)}")
    
    print("\n2. Checkpointing example...")
    import tempfile
    import os
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_file = f.name
    
    try:
        # Run some steps
        for i in range(20):
            brain.step(i)
        
        brain.save(checkpoint_file)
        print(f"   ✓ Saved brain to {checkpoint_file}")
        
        # Reset and reload
        brain.reset()
        brain.initialize()
        brain.load(checkpoint_file)
        print(f"   ✓ Reloaded brain state")
        
    finally:
        if os.path.exists(checkpoint_file):
            os.unlink(checkpoint_file)
    
    print("\n3. Multi-region brain example...")
    # Create a multi-region brain
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    # Add regions
    region1_id = brain2.addRegion("cortex")
    region2_id = brain2.addRegion("memory")
    
    print(f"   Created regions: {brain2.getRegionCount()}")
    print(f"   Region 1: {brain2.getRegion(region1_id)}")
    print(f"   Region 2: {brain2.getRegion(region2_id)}")
    
    return brain, brain2

def main():
    """Run the complete tutorial"""
    print("\n" + "=" * 70)
    print("NLM Python API - Getting Started Tutorial")
    print("=" * 70)
    print("\nThis tutorial will guide you through:")
    print("  • Basic brain creation and operation")
    print("  • World interaction and sensory processing")
    print("  • Enabling learning systems")
    print("  • Advanced usage patterns")
    
    # Run sections
    brain1 = section_1_basic_brain()
    brain2, world2, agent2 = section_2_world_simulation()
    brain3, world3, agent3 = section_3_learning_systems()
    brain4, brain5 = section_4_advanced_usage()
    
    # Summary
    print("\n" + "=" * 70)
    print("TUTORIAL COMPLETE!")
    print("=" * 70)
    print("\nYou now know how to:")
    print("1. Create and run basic neural brains")
    print("2. Connect brains to simulated worlds")
    print("3. Enable and use learning systems")
    print("4. Use advanced features like checkpointing")
    print("\nNext steps:")
    print("- Run the comprehensive examples: examples/comprehensive_examples.py")
    print("- Check the documentation in docs/")
    print("- Experiment with different configurations")

if __name__ == "__main__":
    main()