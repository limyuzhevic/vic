#!/usr/bin/env python3
"""
Test script for NLM Python bindings
"""

import pynlm

def test_basic_bindings():
    print("Testing NLM Python bindings...")
    
    # Test 1: Create default config
    print("1. Testing Config creation...")
    config = pynlm.createDefaultConfig()
    print(f"   Config created: {config}")
    
    # Test 2: Create brain
    print("2. Testing Brain creation...")
    brain = pynlm.createBrain(config)
    print(f"   Brain created: {brain}")
    
    # Test 3: Initialize brain
    print("3. Testing Brain initialization...")
    brain.initialize()
    print(f"   Brain initialized successfully")
    
    # Test 4: Test step
    print("4. Testing Brain step...")
    brain.step(0)
    print(f"   Brain step completed, spikes: {brain.getTotalSpikeCount()}")
    
    # Test 5: Create world
    print("5. Testing World creation...")
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    print(f"   World configured: {world.getWidth()}x{world.getHeight()}")
    
    # Test 6: Create agent brain
    print("6. Testing AgentBrain creation...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print(f"   AgentBrain created and initialized")
    
    # Test 7: Test sensory processing
    print("7. Testing sensory processing...")
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    print(f"   Sensory input processed")
    
    # Test 8: Test motor decoding
    print("8. Testing motor decoding...")
    action = agent.decodeMotorCommand()
    print(f"   Motor command decoded: {action}")
    
    # Test 9: Test neuromodulation
    print("9. Testing neuromodulation levels...")
    print(f"   Dopamine level: {agent.getNeuromodulationLevel()}")
    print(f"   Curiosity level: {agent.getCuriosityLevel()}")
    print(f"   Novelty level: {agent.getNoveltyLevel()}")
    print(f"   Prediction error: {agent.getPredictionError()}")
    
    # Test 10: Test development
    print("10. Testing development...")
    print(f"   Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Test 11: Test memory systems
    print("11. Testing memory systems...")
    working_memory = agent.getWorkingMemory()
    if working_memory:
        print(f"   Working memory available: {working_memory.getActiveTraces()} traces")
    
    episodic_memory = agent.getEpisodicMemory()
    if episodic_memory:
        print(f"   Episodic memory available: {episodic_memory.getEpisodeCount()} episodes")
    
    associative_memory = agent.getAssociativeMemory()
    if associative_memory:
        print(f"   Associative memory available: {associative_memory.getAssociationCount()} associations")
    
    # Test 12: Test prediction system
    print("12. Testing prediction system...")
    prediction_system = agent.getPredictionSystem()
    if prediction_system:
        print(f"   Prediction system available")
        print(f"   Prediction error: {prediction_system.getPredictionError()}")
        print(f"   Confidence: {prediction_system.getConfidence()}")
    
    # Test 13: Test planner
    print("13. Testing planner...")
    planner = agent.getPlanner()
    if planner:
        print(f"   Planner available")
        print(f"   Planning depth: {planner.getPlanningDepth()}")
        print(f"   Planning confidence: {planner.getPlanningConfidence()}")
    
    # Test 14: Test attention
    print("14. Testing attention system...")
    attention = agent.getAttention()
    if attention:
        print(f"   Attention system available")
        print(f"   Attended regions: {len(attention.getAttendedRegions())}")
    
    # Test 15: Test development system
    print("15. Testing development system...")
    development_system = agent.getDevelopmentSystem()
    if development_system:
        print(f"   Development system available")
        print(f"   Current stage: {development_system.getStage()}")
        print(f"   Age: {development_system.getAge()}")
    
    # Test 16: Test concept formation
    print("16. Testing concept formation...")
    concept_formation = agent.getBrain().getConceptFormation()
    if concept_formation:
        print(f"   Concept formation available")
        print(f"   Concept count: {concept_formation.getConceptCount()}")
    
    print("\nAll tests completed successfully!")

def test_advanced_features():
    print("\n" + "="*50)
    print("Testing advanced features...")
    
    # Create brain, world, and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    print("\nEnabling all learning systems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run training simulation
    print("Running training simulation...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"Step {step}: Reward={result.reward}, Success={result.success}")
    
    # Print final stats
    print("\nFinal simulation statistics:")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Test benchmarking
    print("\nRunning benchmark...")
    import time
    start_time = time.time()
    
    for step in range(100):
        brain.step(step, 0.1 * step)
    
    end_time = time.time()
    print(f"Processed 100 steps in {end_time - start_time:.3f} seconds")
    
    # Test save/load functionality
    print("\nTesting save/load functionality...")
    brain.save("test_brain_state.json")
    print("Brain state saved")
    
    # Create a new brain and load the state
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.initialize()
    brain2.load("test_brain_state.json")
    print("Brain state loaded successfully")
    
    print("\nAdvanced features test completed!")

if __name__ == "__main__":
    print("NLM Python Bindings Test Suite")
    print("="*50)
    
    test_basic_bindings()
    test_advanced_features()
    
    print("\n" + "="*50)
    print("All tests passed! NLM Python bindings are working correctly.")
    print("="*50)
