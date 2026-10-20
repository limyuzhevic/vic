#!/usr/bin/env python3
"""
Example usage of NeuralWorkingMemory bindings based on NLM Phase 6 integration

This demonstrates how NeuralWorkingMemory integrates with the broader NLM system
for Phase 6 (Final Integration) experiments.
"""

import pynlm

def phase6_working_memory_example():
    """Complete example of working memory in Phase 6 brain integration."""
    
    print("Phase 6 NeuralWorkingMemory Integration Example")
    print("=" * 70)
    
    # 1. Create the complete brain system as in Phase 6
    print("\n1. Creating complete brain system...")
    config = pynlm.createWorkingMemoryConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("   ✓ Brain system initialized")
    
    # 2. Create world and agent for closed-loop testing
    print("\n2. Setting up world and agent...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("   ✓ World and agent setup complete")
    
    # 3. Access and configure working memory
    print("\n3. Accessing and configuring working memory...")
    wm = pynlm.getWorkingMemoryFromBrain(brain)
    
    # Configure for Phase 6 integration
    wm.setCapacity(150)  # Higher capacity for complex tasks
    wm.setDecayRate(0.02)  # Slower decay for longer persistence
    print("   ✓ Working memory configured")
    print(f"     Capacity: {wm.getCapacity()}")
    print(f"     Decay rate: {wm.getDecayRate()}")
    
    # 4. Simulate working memory during closed-loop operation
    print("\n4. Simulating working memory during closed-loop operation...")
    
    # Simulate storing sensory patterns
    for step in range(10):
        # Get sensory input (what agent sees)
        percept = world.getSensoryPercept()
        
        # Process through agent (which accesses working memory)
        agent.processSensoryInput(percept)
        
        # Store a representation of the sensory pattern in working memory
        # This simulates Phase 6: Memory systems connected to neural processing
        if percept:
            # Convert percept to float array for working memory
            pattern = percept.getAllSignals()
            if pattern:
                wm.store(pattern, strength=1.0)
                
                # Get working memory activity
                activity = wm.getMemoryActivity()
                traces = wm.getActiveTraces()
                
                print(f"     Step {step}: Stored pattern, "
                      f"Activity: {activity:.3f}, "
                      f"Traces: {traces}")
        
        # Update the brain
        brain.step(step)
        
        # Agent makes action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update world
        world.update(0.1)
        
        # Reward-based modulation affects working memory
        reward = 0.0
        if action.getType() == pynlm.ActionType.Interact:
            reward = 1.0
        elif action.getType() == pynlm.ActionType.Eat:
            reward = 2.0
            
        agent.applyRewardModulation(reward, 0.5)
    
    # 5. Demonstrate working memory dynamics
    print("\n5. Working memory dynamics...")
    
    # Run competition for selective retention
    wm.runCompetition()
    print("   ✓ Competition run - winners selected")
    
    # Show memory activity level
    final_activity = wm.getMemoryActivity()
    print(f"   ✓ Final memory activity: {final_activity:.3f}")
    
    # Check how many neurons are in working memory
    memory_neurons = wm.getMemoryNeurons()
    print(f"   ✓ Neurons in working memory: {len(memory_neurons)}")
    
    # 6. Demonstrate reinforcement and decay
    print("\n6. Memory reinforcement and decay...")
    
    # Store a new pattern
    new_pattern = [0.8, 0.6, 0.4, 0.2, 0.1]
    wm.store(new_pattern)
    print(f"   ✓ Stored new pattern: {new_pattern}")
    
    # Strong reinforcement (rehearsal)
    wm.strengthenMemory(1.5)
    print("   ✓ Applied reinforcement factor: 1.5")
    
    # Update to allow decay
    wm.update(10.0)  # 10 timesteps
    print("   ✓ Updated with decay")
    
    # 7. Demonstrate memory clearing
    print("\n7. Memory clearing...")
    wm.clear()
    traces = wm.getActiveTraces()
    print(f"   ✓ Cleared working memory, remaining traces: {traces}")
    
    # 8. Complete Phase 6 summary
    print("\n8. Phase 6 Integration Summary...")
    print("   ✓ Working memory integrated with neural processing")
    print("   ✓ Memory dynamics linked to action selection")
    print("   ✓ Reward modulation affects memory strength")
    print("   ✓ Competition enables selective retention")
    print("   ✓ Capacity and decay configured for persistence")
    
    print("\n" + "=" * 70)
    print("Phase 6 NeuralWorkingMemory integration complete!")
    print("\nKey integration points:")
    print("1. Working memory connects to perception through agent")
    print("2. Memory patterns influence action selection")
    print("3. Rewards strengthen relevant memory traces")
    print("4. Competition selects most salient patterns")
    print("5. Decay prevents memory overload")
    print("6. Configuration supports complex task demands")

def simple_working_memory_only():
    """Simple example focusing only on working memory."""
    print("\n" + "=" * 70)
    print("Simple NeuralWorkingMemory Example")
    print("=" * 70)
    
    # Create minimal working memory system
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    wm = pynlm.getWorkingMemoryFromBrain(brain)
    
    print("\nDemonstrating basic working memory operations:")
    
    # Store multiple patterns
    patterns = [
        [1.0, 0.0, 0.0, 0.0, 0.0],
        [0.0, 1.0, 0.0, 0.0, 0.0],
        [0.0, 0.0, 1.0, 0.0, 0.0],
        [0.0, 0.0, 0.0, 1.0, 0.0],
        [0.0, 0.0, 0.0, 0.0, 1.0]
    ]
    
    for i, pattern in enumerate(patterns):
        wm.store(pattern, strength=1.0)
        retrieved = wm.retrieve()
        print(f"  Stored pattern {i}: {pattern}")
        print(f"    Retrieved: {retrieved}")
    
    # Test contains
    print("\nTesting contains:")
    print(f"  wm.contains(neuron_1) = {wm.contains(brain.getRegion(1))}")
    print(f"  wm.contains(neuron_2) = {wm.contains(brain.getRegion(2))}")
    
    # Test activity
    print(f"\nMemory activity: {wm.getMemoryActivity():.3f}")
    
    # Test capacity
    print(f"Current capacity: {wm.getCapacity()}")
    
    # Run competition
    wm.runCompetition()
    print("Competition completed")
    
    print("\nSimple working memory example complete!")

if __name__ == "__main__":
    phase6_working_memory_example()
    simple_working_memory_only()