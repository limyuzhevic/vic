#!/usr/bin/env python3
"""
NLM Quick Start Example Script

This script demonstrates the key features of the NLM (Neural Learning Machine)
framework. It shows how to create a brain, run a simple experiment, and explore
advanced features like memory systems, neuromodulation, and planning.

Run this script to see NLM in action:

python3 example_script.py

Features demonstrated:
1. Basic brain initialization and simulation
2. Working memory and episodic memory
3. Neuromodulation (dopamine, curiosity, novelty)
4. Action planning and selection
5. Learning through experience
6. Memory replay
7. Self-model development
"""

import pynlm
import time
import sys

def print_header(text):
    """Print formatted header"""
    print("\n" + "="*60)
    print(text)
    print("="*60)

def print_section(text):
    """Print formatted section header"""
    print("\n" + text)
    print("-" * len(text))

def example_1_basic_simulation():
    """Demonstrates basic brain simulation"""
    print_section("Example 1: Basic Brain Simulation")
    
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    if not brain.initialize():
        print("ERROR: Failed to initialize brain")
        return False
    
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ {brain.getRegionCount()} regions created")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: Spikes={brain.getTotalSpikeCount()}, Active={brain.getActiveNeuronCount()}")
    
    print(f"✓ Simulation complete: {brain.getTotalSpikeCount()} total spikes")
    return True

def example_2_memory_systems():
    """Demonstrates memory system usage"""
    print_section("Example 2: Memory Systems")
    
    # Create brain with memory support
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    concept_formation = brain.getConceptFormation()
    
    if working_memory:
        print(f"✓ Working Memory: {working_memory.getActiveTraces()} active traces")
        
        # Store some memory
        test_neuron = pynlm.NeuronId(100)
        working_memory.storeToNeuron(test_neuron, 0.8)
        
        # Retrieve memory
        activation = working_memory.getNeuronActivation(test_neuron)
        print(f"  Stored activation: {activation:.3f}")
    
    if episodic_memory:
        print(f"✓ Episodic Memory: {episodic_memory.getEpisodeCount()} episodes")
        
        # Check if we can replay
        episodes = episodic_memory.getEpisodesForReplay(1)
        print(f"  Episodes available for replay: {len(episodes)}")
    
    if concept_formation:
        print(f"✓ Concept Formation: {concept_formation.getConceptCount()} concepts")
    
    return True

def example_3_neuromodulation():
    """Demonstrates neuromodulation systems"""
    print_section("Example 3: Neuromodulation Systems")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Simulate some experience to trigger neuromodulation
    print("  Simulating experience...")
    for step in range(20):
        # Randomly get reward or punishment
        reward = 10.0 if step % 7 == 0 else -5.0
        
        # Apply reward modulation (this updates neuromodulators)
        agent.applyRewardModulation(reward, 0.0)
        
        # Log neuromodulation levels periodically
        if step % 5 == 0:
            print(f"    Step {step}: Reward={reward:+6.1f}")
            print(f"      Dopamine: {agent.getNeuromodulationLevel():+6.3f}")
            print(f"      Curiosity: {agent.getCuriosityLevel():+6.3f}")
            print(f"      Novelty: {agent.getNoveltyLevel():+6.3f}")
            print(f"      Prediction Error: {agent.getPredictionError():+6.3f}")
    
    print("✓ Neuromodulation systems demonstrated")
    return True

def example_4_planning_and_action():
    """Demonstrates planning and action selection"""
    print_section("Example 4: Planning and Action Selection")
    
    # Create brain with planning enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Get planning information
    planner = brain.getPlanner()
    if planner:
        print(f"✓ Neural Planner: Depth={planner.getPlanningDepth()}, Confidence={planner.getPlanningConfidence():.3f}")
        print(f"  Recent plan successful: {planner.wasRecentPlanSuccessful()}")
    
    # Create simple environment for action testing
    world = pynlm.createSimpleWorld()
    world.configure(8, 8, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Add a resource to interact with
    resource = pynlm.WorldObject(4, 4, pynlm.WorldObjectType.Resource, 1.0, 0.5)
    world.addObject(resource)
    
    # Run action selection episode
    print("  Running action selection episode...")
    for step in range(15):
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, step * 0.001)
        
        agent.applyRewardModulation(result.reward, 0.0)
        
        print(f"    Step {step}: Action={action}, Reward={result.reward:+6.1f}")
    
    print("✓ Action planning and selection demonstrated")
    return True

def example_5_learning_experiment():
    """Demonstrates learning through experience"""
    print_section("Example 5: Learning Through Experience")
    
    # Configure for learning
    config = pynlm.createDefaultConfig()
    config.set("enable_curiosity", True)
    config.set("enable_planning", True)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    if not brain.initialize():
        print("ERROR: Failed to initialize brain")
        return False
    
    # Get initial synaptic state
    region = brain.getRegion(pynlm.RegionId(1))
    initial_weights = [syn.getWeight() for syn in region.getSynapses()]
    initial_mean = sum(initial_weights) / len(initial_weights)
    
    print(f"  Initial synaptic mean weight: {initial_mean:.4f}")
    
    # Create environment and agent
    world = pynlm.createSimpleWorld()
    world.configure(8, 8, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run learning episode
    print("  Running learning episode...")
    for step in range(50):
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, step * 0.001)
        
        agent.applyRewardModulation(result.reward, 0.0)
        
        if step % 10 == 0:
            print(f"    Step {step}: Reward={result.reward:+6.1f}")
    
    # Check learning results
    final_weights = [syn.getWeight() for syn in region.getSynapses()]
    final_mean = sum(final_weights) / len(final_weights)
    
    learning_change = final_mean - initial_mean
    learning_occurred = learning_change > 0.001
    
    print(f"  Final synaptic mean weight: {final_mean:.4f}")
    print(f"  Learning change: {learning_change:+.4f}")
    print(f"  Learning occurred: {learning_occurred}")
    print(f"  Total spikes generated: {brain.getTotalSpikeCount()}")
    
    return learning_occurred

def example_6_self_model():
    """Demonstrates self-model development"""
    print_section("Example 6: Self-Model Development")
    
    # Create brain with self-model support
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create environment
    world = pynlm.createSimpleWorld()
    world.configure(6, 6, 6, 6)
    world.reset()
    agent.initialize(world)
    
    # Add a resource
    resource = pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 1.0, 0.5)
    world.addObject(resource)
    
    print("  Developing self-model through interaction...")
    
    # Record initial self-model confidence
    initial_confidence = agent.getSelfModelConfidence()
    print(f"  Initial self-model confidence: {initial_confidence:.3f}")
    
    # Run episodes to develop self-model
    for episode in range(5):
        for step in range(20):
            world.update(0.001)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, step * 0.001)
            
            agent.applyRewardModulation(result.reward, 0.0)
        
        # Check self-model confidence after each episode
        confidence = agent.getSelfModelConfidence()
        print(f"    Episode {episode}: Self-model confidence = {confidence:.3f}")
    
    print("✓ Self-model development demonstrated")
    return True

def main():
    """Main function that runs all examples"""
    print_header("NLM Quick Start Example")
    
    print("Welcome to NLM (Neural Learning Machine)!")
    print("This script demonstrates key features of the Phase 6 complete AI brain.")
    print()
    
    examples = [
        ("Basic Brain Simulation", example_1_basic_simulation),
        ("Memory Systems", example_2_memory_systems),
        ("Neuromodulation Systems", example_3_neuromodulation),
        ("Planning and Action Selection", example_4_planning_and_action),
        ("Learning Through Experience", example_5_learning_experiment),
        ("Self-Model Development", example_6_self_model),
    ]
    
    results = []
    
    # Run each example
    for name, example_func in examples:
        try:
            print(f"\nRunning: {name}")
            success = example_func()
            results.append((name, success, None))
            status = "✓ SUCCESS" if success else "✗ FAILED"
            print(f"Result: {status}")
        except Exception as e:
            print(f"✗ ERROR: {str(e)}")
            import traceback
            traceback.print_exc()
            results.append((name, False, str(e)))
    
    # Summary
    print_header("Example Summary")
    
    successful = sum(1 for _, success, _ in results if success)
    total = len(results)
    
    print(f"Examples run: {total}")
    print(f"Successful: {successful}")
    print(f"Failed: {total - successful}")
    
    print("\nExamples completed:")
    for name, success, error in results:
        status = "✓" if success else "✗"
        print(f"  {status} {name}")
        if error:
            print(f"    Error: {error}")
    
    print("\n" + "="*60)
    print("NLM Quick Start Complete!")
    print("="*60)
    
    if successful == total:
        print("\nAll examples ran successfully!")
        print("\nNext steps:")
        print("  • Explore the NLM documentation for more details")
        print("  • Try running the command-line demos:")
        print("    - ./nlm (basic neural computation)")
        print("    - ./nlm_phase6_demo (comprehensive integration)")
        print("  • Customize experiments with different configurations")
    else:
        print(f"\nNote: {total - successful} examples had issues.")
        print("Check for error messages above and adjust your environment if needed.")
    
    print("\nHappy exploring NLM's brain-inspired AI capabilities!")
    return 0

if __name__ == "__main__":
    sys.exit(main())
