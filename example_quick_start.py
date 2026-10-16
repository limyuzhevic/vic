#!/usr/bin/env python3
"""
Quick Start Example for NLM Python Bindings

This script demonstrates the basic usage of the NLM Python bindings
to get you started with neural simulation and cognitive modeling.
"""

import pynlm
import time

def main():
    print("=== NLM Python Bindings Quick Start ===\n")
    
    # Step 1: Create configuration
    print("Step 1: Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    print("✓ Configuration created")
    
    # Step 2: Create brain
    print("\nStep 2: Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain initialized")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    
    # Step 3: Access memory systems
    print("\nStep 3: Exploring memory systems...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    semantic_memory = brain.getSemanticMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    print(f"Semantic memory facts: {len(semantic_memory.getAllFacts())}")
    
    # Step 4: Demonstrate working memory
    print("\nStep 4: Testing working memory...")
    pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
    working_memory.store(pattern, strength=1.0)
    print(f"Stored pattern: {pattern}")
    retrieved = working_memory.retrieve()
    print(f"Retrieved pattern: {retrieved}")
    
    # Step 5: Run simulation
    print("\nStep 5: Running simulation...")
    for step in range(5):
        brain.step(step, step * 0.001)
        print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons")
    
    # Step 6: Access neuromodulation
    print("\nStep 6: Exploring neuromodulation...")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    
    dopamine.signalReward(1.0)
    curiosity.setExplorationLevel(0.5)
    
    print(f"Dopamine level: {dopamine.getLevel():.3f}")
    print(f"Curiosity exploration: {curiosity.getExplorationLevel():.3f}")
    
    # Step 7: Access prediction system
    print("\nStep 7: Testing prediction system...")
    prediction_system = brain.getPredictionSystem()
    print(f"Prediction system initialized")
    
    # Step 8: Access cognitive systems
    print("\nStep 8: Exploring cognitive systems...")
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    development = brain.getDevelopmentSystem()
    
    print(f"Planner depth: {planner.getPlanningDepth()}")
    print(f"Development stage: {development.getCurrentStage()}")
    
    # Step 9: Create agent brain and world
    print("\nStep 9: Creating agent interface...")
    world = pynlm.createSimpleWorld()
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    print(f"Agent brain ready: {agent_brain.getSensoryInputSize()} inputs, "
          f"{agent_brain.getMotorOutputSize()} outputs")
    
    # Step 10: Save state
    print("\nStep 10: Saving brain state...")
    brain.save("quick_start_brain.nlm")
    print("✓ Brain state saved to 'quick_start_brain.nlm'")
    
    print("\n=== Quick Start Complete ===")
    print("This example demonstrated:")
    print("  ✓ Basic brain initialization and simulation")
    print("  ✓ Memory systems (working, episodic, semantic)")
    print("  ✓ Neuromodulation (dopamine, curiosity)")
    print("  ✓ Prediction and cognitive systems")
    print("  ✓ Agent brain integration")
    print("  ✓ State saving/loading")
    print("\nFor more advanced examples, see the documentation and test files.")

if __name__ == "__main__":
    main()
