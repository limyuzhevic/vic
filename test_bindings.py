#!/usr/bin/env python3

"""
Test script to validate the enhanced Python bindings for NLM.
This script exercises the newly added memory systems, neuromodulation,
and convenience APIs.
"""

import sys
import time
import traceback

# Add the current directory to Python path to import pynlm
sys.path.insert(0, '/workspace/app/python')

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic functionality with new API"""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Create default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Check basic metrics
        metrics = pynlm.get_brain_metrics(brain)
        print(f"✓ Got brain metrics: {metrics}")
        
        # Run simulation
        spikes = pynlm.run_brain_dynamics(brain, 10)
        print(f"✓ Ran brain dynamics: {spikes} spikes")
        
        return True
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_memory_systems():
    """Test memory system API"""
    print("\n=== Testing Memory Systems ===")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        print(f"✓ Got working memory: {working_memory}")
        print(f"✓ Got episodic memory: {episodic_memory}")
        print(f"✓ Got associative memory: {associative_memory}")
        
        # Test working memory
        if working_memory:
            working_memory.setCapacity(100)
            working_memory.setDecayRate(0.1)
            print("✓ Configured working memory")
            
            # Store some information
            test_pattern = [0.1, 0.5, 0.3, 0.7, 0.2]
            working_memory.store(test_pattern, 1.0)
            print("✓ Stored pattern in working memory")
            
            # Retrieve
            retrieved = working_memory.retrieve()
            print(f"✓ Retrieved from working memory: {retrieved}")
        
        # Test episodic memory
        if episodic_memory:
            # Create a test episode
            episode = pynlm.EpisodicMemoryItem()
            episode.timestamp = 100
            episode.positionX = 5.0
            episode.positionY = 5.0
            episode.action = pynlm.ActionType.Eat
            episode.reward = 1.5
            episode.sensoryState = [0.1, 0.2, 0.3]
            
            episodic_memory.storeEpisode(episode)
            print("✓ Stored episode")
            
            # Check episode count
            count = episodic_memory.getEpisodeCount()
            print(f"✓ Episodic memory has {count} episodes")
        
        # Test associative memory
        if associative_memory:
            pattern_a = [0.1, 0.5, 0.3]
            pattern_b = [0.2, 0.6, 0.4]
            associative_memory.associate(pattern_a, pattern_b, 0.8)
            print("✓ Created association between patterns")
            
            strength = associative_memory.getAssociationStrength(pattern_a, pattern_b)
            print(f"✓ Association strength: {strength}")
        
        return True
    except Exception as e:
        print(f"✗ Memory systems test failed: {e}")
        traceback.print_exc()
        return False

def test_neuromodulation():
    """Test neuromodulation API"""
    print("\n=== Testing Neuromodulation ===")
    
    try:
        # Create brain with neuromodulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable neuromodulation
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        print("✓ Enabled neuromodulation systems")
        
        # Check neuromodulation levels
        dopamine_level = agent.getNeuromodulationLevel()
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        print(f"✓ Neuromodulation levels - Dopamine: {dopamine_level}")
        print(f"✓ Neuromodulation levels - Curiosity: {curiosity_level}")
        print(f"✓ Neuromodulation levels - Novelty: {novelty_level}")
        print(f"✓ Neuromodulation levels - Prediction Error: {prediction_error}")
        
        # Test reward system
        reward_system = brain.getDopamine()
        if reward_system:
            reward_system.signalReward(1.0)
            print("✓ Signal reward to dopamine system")
        
        return True
    except Exception as e:
        print(f"✗ Neuromodulation test failed: {e}")
        traceback.print_exc()
        return False

def test_development():
    """Test development system API"""
    print("\n=== Testing Development System ===")
    
    try:
        # Create brain with development enabled
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable development
        agent = pynlm.createAgentBrain(brain)
        agent.enableDevelopment(True)
        
        print("✓ Enabled development system")
        
        # Check developmental stage
        stage = brain.getDevelopmentalStage()
        print(f"✓ Current developmental stage: {stage}")
        
        # Test development update
        agent.updateDevelopment(0.1)
        print("✓ Updated development")
        
        return True
    except Exception as e:
        print(f"✗ Development test failed: {e}")
        traceback.print_exc()
        return False

def test_cognition():
    """Test cognition system API"""
    print("\n=== Testing Cognition Systems ===")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get cognition systems
        attention = brain.getAttention()
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        
        print(f"✓ Got attentional selection: {attention}")
        print(f"✓ Got neural planner: {planner}")
        print(f"✓ Got concept formation: {concept_formation}")
        
        # Test attentional selection
        if attention:
            attention.initialize(brain.getRegion(0))
            # Focus on a region
            attention.focusOnRegion(brain.getRegion(0).getId())
            print("✓ Applied attentional focus")
            
            # Update attention
            attention.update(0.1)
            print("✓ Updated attention")
        
        return True
    except Exception as e:
        print(f"✗ Cognition test failed: {e}")
        traceback.print_exc()
        return False

def test_agent_integration():
    """Test complete agent integration"""
    print("\n=== Testing Agent Integration ===")
    
    try:
        # Create complete agent setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        print("✓ Created complete agent setup")
        
        # Run agent simulation
        success = pynlm.run_agent_simulation(brain, world, agent, 20)
        print(f"✓ Agent simulation ran: {success}")
        
        # Get final metrics
        metrics = pynlm.get_brain_metrics(brain)
        print(f"✓ Final metrics: {metrics}")
        
        return True
    except Exception as e:
        print(f"✗ Agent integration test failed: {e}")
        traceback.print_exc()
        return False

def test_phase6_experiment():
    """Test Phase 6 experiment API"""
    print("\n=== Testing Phase 6 Experiment ===")
    
    try:
        # Create experiment
        experiment = pynlm.Phase6IntegratedExperiment()
        
        # Test integration verification
        integration_ok = experiment.verifyIntegration()
        print(f"✓ Integration verification: {integration_ok}")
        
        # Configure for test run
        config = pynlm.Phase6Config()
        config.neuronCount = 100
        config.maxSteps = 100
        config.enableCheckpointing = True
        config.enableReplay = True
        config.enableDevelopment = True
        
        # Run experiment (limited for testing)
        result = experiment.run(config)
        
        print(f"✓ Experiment completed")
        print(f"  Total reward: {result.totalReward}")
        print(f"  Average firing rate: {result.avgFiringRate}")
        print(f"  Episodes stored: {result.memoryEpisodesStored}")
        print(f"  Dopamine level: {result.dopamineLevel}")
        
        return True
    except Exception as e:
        print(f"✗ Phase 6 experiment test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Memory Systems", test_memory_systems),
        ("Neuromodulation", test_neuromodulation),
        ("Development System", test_development),
        ("Cognition Systems", test_cognition),
        ("Agent Integration", test_agent_integration),
        ("Phase 6 Experiment", test_phase6_experiment),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test_name} crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
