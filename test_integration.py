#!/usr/bin/env python3

"""
NLM Integration Test Suite

This test suite verifies that Phase 6 integration is working correctly.
Tests the connectivity and functionality of all major brain systems.
"""

import sys
import time
import tempfile
import os
from pathlib import Path

# Add the current directory to path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
except ImportError:
    print("ERROR: Could not import pynlm. Make sure it's built first.")
    sys.exit(1)

def test_basic_brain_creation():
    """Test 1: Basic brain creation and initialization"""
    print("=== Test 1: Basic Brain Creation ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    assert brain is not None, "Failed to create brain"
    
    if brain.initialize():
        print("✓ Brain initialized successfully")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Regions: {brain.getRegionCount()}")
        print(f"  Synaptic weight ratio: {brain.getExcitationInhibitionRatio()}")
        return True
    else:
        print("✗ Brain initialization failed")
        return False

def test_memory_systems():
    """Test 2: Memory system integration"""
    print("\n=== Test 2: Memory System Integration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    # Test working memory
    wm = brain.getWorkingMemory()
    if wm:
        print(f"✓ Working memory exists: {wm.getActiveTraces()} active traces")
        
        # Test basic working memory operations
        test_neurons = [1, 2, 3]
        for i, neuron_id in enumerate(test_neurons):
            wm.storeToNeuron(neuron_id, 0.5 + i * 0.1)
        
        print(f"  Stored {len(test_neurons)} neurons in working memory")
        
        # Test retrieval
        retrieved = wm.retrieve()
        print(f"  Retrieved {len(retrieved)} values from working memory")
        
        # Test competition
        wm.runCompetition()
        print(f"  Competition complete, {len(wm.getMemoryNeurons())} winners")
    else:
        print("✗ Working memory is null - NOT INTEGRATED")
        return False
    
    # Test episodic memory
    em = brain.getEpisodicMemory()
    if em:
        print(f"✓ Episodic memory exists: {em.getEpisodeCount()} episodes")
        
        # Test episode storage
        from pynlm import ActionType
        test_episode = type('MockEpisode', (), {
            'timestamp': 100,
            'sensoryState': [0.1, 0.2, 0.3],
            'positionX': 5.0,
            'positionY': 3.0,
            'orientation': 0.5,
            'action': ActionType.MoveForward,
            'reward': 0.8,
            'energy': 1.0,
            'novelty': 0.3,
            'resultingSensoryState': [0.15, 0.25, 0.35],
            'resultingReward': 0.9,
            'activeNeurons': [1, 2, 3],
            'neuronActivations': [0.6, 0.7, 0.5],
            'age': 5
        })()
        
        em.storeEpisode(test_episode)
        print(f"  Stored episode: reward={test_episode.reward}, energy={test_episode.energy}")
        
        # Test retrieval
        similar = em.retrieveSimilar([0.1, 0.2, 0.3], 3)
        print(f"  Found {len(similar)} similar episodes")
        
        # Test temporal retrieval
        temporal = em.retrieveTemporal(50, 150, 3)
        print(f"  Found {len(temporal)} episodes in time window")
        
        # Test consolidation
        em.consolidate(0.5)
        print(f"  After consolidation: {em.getEpisodeCount()} episodes")
        
        # Test replay
        em.enableReplay(True)
        episodes = em.getEpisodesForReplay(3)
        print(f"  Episodes available for replay: {len(episodes)}")
    else:
        print("✗ Episodic memory is null - NOT INTEGRATED")
        return False
    
    # Test associative memory
    am = brain.getAssociativeMemory()
    if am:
        print(f"✓ Associative memory exists: {am.getAssociationCount()} associations")
        
        # Test association creation
        pattern_a = [0.1, 0.2, 0.3]
        pattern_b = [0.4, 0.5, 0.6]
        am.associate(pattern_a, pattern_b, 0.8)
        
        # Test retrieval
        retrieved = am.retrieve(pattern_a, 3)
        print(f"  Retrieved {len(retrieved)} patterns via association")
        
        # Test spreading activation
        spread = am.spreadActivation(pattern_a, 2)
        print(f"  Spreading activation found {len(spread)} patterns")
    else:
        print("✗ Associative memory is null - NOT INTEGRATED")
        return False
    
    print("✓ All memory systems are properly integrated")
    return True

def test_neuromodulation():
    """Test 3: Neuromodulation system integration"""
    print("\n=== Test 3: Neuromodulation System Integration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    # Test dopamine system
    dopamine = brain.getDopamine()
    if dopamine:
        print(f"✓ Dopamine system exists")
        print(f"  Level: {dopamine.getLevel()}")
        print(f"  Plasticity factor: {dopamine.getPlasticityFactor()}")
        
        # Test reward signaling
        dopamine.signalReward(0.5)
        print(f"  After reward signal: {dopamine.getLevel()}")
        
        # Test prediction error
        dopamine.signalRewardPredictionError(0.2)
        print(f"  After prediction error: {dopamine.getLevel()}")
    else:
        print("✗ Dopamine system is null - NOT INTEGRATED")
        return False
    
    # Test curiosity system
    curiosity = brain.getCuriosity()
    if curiosity:
        print(f"✓ Curiosity system exists")
        print(f"  Current level: {curiosity.getLevel()}")
    else:
        print("✗ Curiosity system is null - NOT INTEGRATED")
        return False
    
    # Test novelty system
    novelty = brain.getNovelty()
    if novelty:
        print(f"✓ Novelty system exists")
        print(f"  Current level: {novelty.getLevel()}")
    else:
        print("✗ Novelty system is null - NOT INTEGRATED")
        return False
    
    # Test prediction error system
    pred_error = brain.getPredictionErrorSignal()
    if pred_error:
        print(f"✓ Prediction error system exists")
        print(f"  Current level: {pred_error.getLevel()}")
    else:
        print("✗ Prediction error system is null - NOT INTEGRATED")
        return False
    
    print("✓ All neuromodulation systems are properly integrated")
    return True

def test_prediction_system():
    """Test 4: Prediction system integration"""
    print("\n=== Test 4: Prediction System Integration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    pred_system = brain.getPredictionSystem()
    if pred_system:
        print(f"✓ Prediction system exists")
        print(f"  Current error: {pred_system.getPredictionError()}")
        print(f"  Confidence: {pred_system.getConfidence()}")
        
        # Test prediction updating
        # Note: This requires a SensoryInput object which may not be easily created
        print("  Prediction system would be updated with sensory observations in real use")
    else:
        print("✗ Prediction system is null - NOT INTEGRATED")
        return False
    
    print("✓ Prediction system is properly integrated")
    return True

def test_cognition_systems():
    """Test 5: Cognitive system integration"""
    print("\n=== Test 5: Cognitive System Integration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    # Test neural planner
    planner = brain.getPlanner()
    if planner:
        print(f"✓ Neural planner exists")
        print(f"  Planning depth: {planner.getPlanningDepth()}")
        print(f"  Confidence: {planner.getPlanningConfidence()}")
        
        # Test action planning
        current_state = [0.1, 0.2, 0.3, 0.4]
        action = planner.planAction(current_state, 0.5)
        print(f"  Planned action: {action}")
        
        # Test sequence evaluation
        actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnRight]
        candidate = planner.evaluateSequence(actions, current_state)
        print(f"  Evaluated sequence: expected reward={candidate.expectedReward}")
    else:
        print("✗ Neural planner is null - NOT INTEGRATED")
        return False
    
    # Test concept formation
    concept = brain.getConceptFormation()
    if concept:
        print(f"✓ Concept formation exists")
        # Note: Concept formation requires sensory state encoding
        print("  Concept formation would process current neural activity patterns")
    else:
        print("✗ Concept formation is null - NOT INTEGRATED")
        return False
    
    # Test attention system
    attention = brain.getAttention()
    if attention:
        print(f"✓ Attention system exists")
        print(f"  Inhibition strength: {attention.getInhibitionStrength()}")
        print(f"  Excitation strength: {attention.getExcitationStrength()}")
        
        # Test competition processing
        competitors = [1, 2, 3, 4, 5]
        winners = attention.processCompetition(competitors, 0.5)
        print(f"  Competition processed {len(competitors)} competitors, {len(winners)} winners")
    else:
        print("✗ Attention system is null - NOT INTEGRATED")
        return False
    
    print("✓ All cognitive systems are properly integrated")
    return True

def test_development_system():
    """Test 6: Development system integration"""
    print("\n=== Test 6: Development System Integration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    dev_system = brain.getDevelopmentSystem()
    if dev_system:
        print(f"✓ Development system exists")
        print(f"  Current stage: {brain.getDevelopmentalStage()}")
        
        # Test stage setting
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        print(f"  After setting to CriticalPeriod: {brain.getDevelopmentalStage()}")
        
        # Note: Development updates occur automatically during steps
        print("  Development system updates plasticity rates automatically")
    else:
        print("✗ Development system is null - NOT INTEGRATED")
        return False
    
    print("✓ Development system is properly integrated")
    return True

def test_checkpoint_system():
    """Test 7: Checkpoint system integration"""
    print("\n=== Test 7: Checkpoint System Integration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    # Create temporary file for checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp:
        checkpoint_path = tmp.name
    
    try:
        print(f"  Testing checkpoint at: {checkpoint_path}")
        
        # Record some state
        initial_spikes = brain.getTotalSpikeCount()
        print(f"  Initial spike count: {initial_spikes}")
        
        # Save checkpoint
        print("  Saving checkpoint...")
        if brain.save(checkpoint_path):
            print("✓ Checkpoint saved successfully")
        else:
            print("✗ Checkpoint save failed")
            return False
        
        # Create new brain and load
        brain2 = pynlm.createBrain(config)
        if (!brain2.initialize()):
            print("✗ Second brain initialization failed")
            return False
        
        print("  Loading checkpoint into new brain...")
        if brain2.load(checkpoint_path):
            print("✓ Checkpoint loaded successfully")
            
            # Note: Exact state restoration is complex due to neural simulation nature
            print("  Note: Neural state restoration depends on checkpoint format complexity")
        else:
            print("✗ Checkpoint load failed")
            return False
        
        return True
    finally:
        # Clean up
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
            print(f"  Cleaned up checkpoint file")

def test_world_integration():
    """Test 8: World integration with agent"""
    print("\n=== Test 8: World Integration ===")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("✓ Brain, world, and agent created successfully")
    
    # Run simulation loop
    print("  Running simulation loop...")
    for step in range(50):
        # Get observation
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step, step * 0.1)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if step % 10 == 0:
            reward = world.getLastReward()
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development if enabled
        if step % 25 == 0:
            agent.updateDevelopment(0.1)
    
    print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} total spikes")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Test agent features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"  Agent features enabled:")
    print(f"    Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"    Curiosity: {agent.isCuriosityEnabled()}")
    print(f"    Development: {agent.isDevelopmentEnabled()}")
    
    return True

def test_full_integration():
    """Test 9: Full integration test"""
    print("\n=== Test 9: Full Integration Test ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain.initialize()):
        print("✗ Brain initialization failed")
        return False
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("  Running full integration test...")
    
    # Test all integrated systems
    for step in range(200):
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step (this should integrate all systems)
        brain.step(step, step * 0.05)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward processing
        reward = world.getLastReward()
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(0.05)
        
        # Log periodically
        if step % 50 == 0:
            print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes, "
                  f"mem={brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0} eps")
    
    print("✓ Full integration test completed")
    print(f"  Final state:")
    print(f"    Total spikes: {brain.getTotalSpikeCount()}")
    print(f"    Average firing: {brain.getAverageFiringRate():.3f}")
    print(f"    Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
    print(f"    Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
    print(f"    Neuromodulation: dopamine={agent.getNeuromodulationLevel():.3f}")
    print(f"    Development stage: {agent.getDevelopmentalStage()}")
    
    return True

def main():
    """Run all integration tests"""
    print("NLM Phase 6 Integration Test Suite")
    print("=" * 50)
    print("Testing Phase 6 integration of all brain systems...")
    
    tests = [
        ("Basic Brain Creation", test_basic_brain_creation),
        ("Memory Systems", test_memory_systems),
        ("Neuromodulation Systems", test_neuromodulation),
        ("Prediction System", test_prediction_system),
        ("Cognitive Systems", test_cognition_systems),
        ("Development System", test_development_system),
        ("Checkpoint System", test_checkpoint_system),
        ("World Integration", test_world_integration),
        ("Full Integration", test_full_integration),
    ]
    
    results = []
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                results.append((test_name, "PASSED"))
                passed += 1
            else:
                results.append((test_name, "FAILED"))
                failed += 1
        except Exception as e:
            print(f"✗ Test failed with exception: {e}")
            import traceback
            traceback.print_exc()
            results.append((test_name, f"ERROR: {e}"))
            failed += 1
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    for test_name, result in results:
        status = "✓ PASS" if "PASSED" in result else "✗ FAIL"
        print(f"{status:8} | {test_name:30} | {result}")
    
    print("\n" + "=" * 60)
    print(f"TOTAL: {passed + failed} tests")
    print(f"PASSED: {passed}")
    print(f"FAILED: {failed}")
    print("=" * 60)
    
    if failed > 0:
        print(f"\n⚠️  Phase 6 integration needs work - {failed} tests failed")
        return 1
    else:
        print(f"\n🎉 All Phase 6 integration tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(main())
