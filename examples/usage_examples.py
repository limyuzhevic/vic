#!/usr/bin/env python3
"""
NLM Python API - Usage Examples

This file contains practical examples of how to use the NLM Python API
for common use cases and workflows.

Usage:
    python -m examples.usage_examples

Author: NLM Development Team
Version: 0.1.0
"""

import pynlm
import time

def create_simple_brain():
    """Example 1: Create a simple brain for basic testing"""
    print("1. Creating a simple brain...")
    
    # Use default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    return brain

def simulate_basic_interaction():
    """Example 2: Basic brain-world interaction"""
    print("2. Basic brain-world interaction...")
    
    # Setup
    brain = create_simple_brain()
    world = pynlm.createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=3, visionHeight=3)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run basic simulation
    for step in range(20):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"   Simulation complete: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    return brain, world, agent

def enable_advanced_learning():
    """Example 3: Enable advanced learning capabilities"""
    print("3. Enabling advanced learning...")
    
    # Custom configuration for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run learning simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"   Step {step}: Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Dev={brain.getDevelopmentalStage()}")
    
    print(f"   Learning complete: Final curiosity={agent.getCuriosityLevel():.3f}")
    return brain, world, agent

def brain_checkpointing_example():
    """Example 4: Brain checkpointing and persistence"""
    print("4. Brain checkpointing example...")
    
    brain = create_simple_brain()
    
    # Save initial state
    initial_spikes = brain.getTotalSpikeCount()
    brain.save("test_checkpoint.bin")
    
    # Run some more steps
    for i in range(20, 40):
        brain.step(i)
    
    after_steps_spikes = brain.getTotalSpikeCount()
    print(f"   After running steps: {after_steps_spikes - initial_spikes} additional spikes")
    
    # Reset and reload
    brain.reset()
    brain.initialize()
    brain.load("test_checkpoint.bin")
    
    after_reload_spikes = brain.getTotalSpikeCount()
    print(f"   After reload: {after_reload_spikes} spikes (should equal {initial_spikes})")
    
    import os
    os.remove("test_checkpoint.bin")
    
    return brain

def multi_agent_example():
    """Example 5: Multiple agents with shared brain"""
    print("5. Multiple agents with shared brain...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create two different worlds
    world1 = pynlm.createSimpleWorld()
    world1.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world1.reset()
    
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world2.reset()
    
    # Create agents
    agent1 = pynlm.createAgentBrain(brain)
    agent2 = pynlm.createAgentBrain(brain)
    
    agent1.initialize(world1)
    agent2.initialize(world2)
    
    # Run both agents
    print("   Running agent 1 simulation...")
    for step in range(20):
        world1.update(0.1)
        agent1.processSensoryInput(world1.getSensoryPercept())
        brain.step(step)
        action = agent1.decodeMotorCommand()
        world1.applyMotorCommand(action, world1.getSimulationTime())
    
    print("   Running agent 2 simulation...")
    for step in range(20, 40):
        world2.update(0.1)
        agent2.processSensoryInput(world2.getSensoryPercept())
        brain.step(step)
        action = agent2.decodeMotorCommand()
        world2.applyMotorCommand(action, world2.getSimulationTime())
    
    print(f"   Agent 1 position: ({world1.getAgentBody().x:.1f}, {world1.getAgentBody().y:.1f})")
    print(f"   Agent 2 position: ({world2.getAgentBody().x:.1f}, {world2.getAgentBody().y:.1f})")
    
    return brain

def batch_processing_example():
    """Example 6: Batch processing multiple brains"""
    print("6. Batch processing multiple brains...")
    
    brains = []
    num_brains = 3
    
    for i in range(num_brains):
        print(f"   Creating brain {i+1}/{num_brains}...")
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500 + i * 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Process all brains
    print(f"   Processing {num_brains} brains simultaneously...")
    for brain in brains:
        for step in range(10):
            brain.step(step)
    
    total_neurons = sum(b.getTotalNeuronCount() for b in brains)
    total_spikes = sum(b.getTotalSpikeCount() for b in brains)
    
    print(f"   Total: {total_neurons} neurons, {total_spikes} spikes")
    
    return brains

def main():
    print("=" * 70)
    print("NLM Python API - Usage Examples")
    print("=" * 70)
    
    examples = [
        ("Simple Brain Creation", create_simple_brain),
        ("Basic Interaction", simulate_basic_interaction),
        ("Advanced Learning", enable_advanced_learning),
        ("Checkpointing", brain_checkpointing_example),
        ("Multiple Agents", multi_agent_example),
        ("Batch Processing", batch_processing_example),
    ]
    
    results = []
    for example_name, example_func in examples:
        print(f"\n--- {example_name} ---")
        try:
            result = example_func()
            results.append((example_name, True, None))
            print(f"✓ {example_name} completed successfully")
        except Exception as e:
            results.append((example_name, False, str(e)))
            print(f"✗ {example_name} failed: {e}")
    
    # Summary
    print("\n" + "=" * 70)
    print("USAGE EXAMPLES SUMMARY")
    print("=" * 70)
    
    passed = sum(1 for _, success, _ in results if success)
    print(f"Completed: {passed}/{len(results)} examples")
    
    if passed == len(results):
        print("\n🎉 All examples completed successfully!")
        print("\nThese examples demonstrate:")
        print("  • Basic brain creation and operation")
        print("  • Brain-world interaction")
        print("  • Advanced learning system usage")
        print("  • Brain checkpointing and persistence")
        print("  • Multiple agent scenarios")
        print("  • Batch processing of brains")
    else:
        print(f"\n⚠️  {len(results) - passed} examples had issues")
        print("Check the output above for error details")

if __name__ == "__main__":
    main()