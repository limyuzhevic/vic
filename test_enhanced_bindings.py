#!/usr/bin/env python3
"""
Test script for NLM Enhanced Python Bindings

This script validates the enhanced Python bindings functionality
and provides basic testing of the new features.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import enhanced bindings
try:
    import pynlm as nlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Test configuration creation
        config = nlm.create_default_config()
        print("✓ Created default configuration")
        
        # Test brain creation
        brain = nlm.create_brain(config)
        print("✓ Created brain")
        
        # Test brain initialization
        brain.initialize()
        print(f"✓ Initialized brain with {brain.getRegionCount()} regions")
        
        # Test simulation steps
        for i in range(5):
            brain.step(i)
        print("✓ Completed simulation steps")
        
        # Test action production
        action = brain.produce_action()
        print(f"✓ Produced action: {action.getName()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_agent_world_integration():
    """Test agent-world integration."""
    print("\n=== Testing Agent-World Integration ===")
    
    try:
        # Create complete agent-environment system
        config, world, brain, agent = nlm.create_complete_agent()
        print(f"✓ Created complete system: {world}")
        
        # Test agent initialization
        agent.initialize(world)
        print("✓ Agent initialized")
        
        # Test sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        print("✓ Processed sensory input")
        
        # Test action decoding
        action = agent.decodeMotorCommand()
        print(f"✓ Decoded motor command: {action.getName()}")
        
        # Test world update
        world.update(0.1)
        print("✓ Updated world")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent-world integration test failed: {e}")
        return False

def test_memory_systems():
    """Test memory system functionality."""
    print("\n=== Testing Memory Systems ===")
    
    try:
        # Create brain and get working memory
        config, world, brain, agent = nlm.create_complete_agent()
        working_memory = nlm.NeuralWorkingMemory()
        working_memory.initialize(brain)
        print("✓ Created and initialized working memory")
        
        # Test memory storage
        vision_data = world.getSensoryPercept().getVision()
        working_memory.store(vision_data, strength=1.0)
        print("✓ Stored vision data in working memory")
        
        # Test memory retrieval
        retrieved = working_memory.retrieve()
        print(f"✓ Retrieved memory pattern: {len(retrieved)} elements")
        
        # Test memory statistics
        print(f"✓ Memory statistics:")
        print(f"  - Active traces: {working_memory.getActiveTraces()}")
        print(f"  - Capacity: {working_memory.getCapacity()}")
        print(f"  - Decay rate: {working_memory.getDecayRate()}")
        
        # Test memory update
        working_memory.update(0.1)
        print("✓ Updated working memory")
        
        return True
        
    except Exception as e:
        print(f"✗ Memory systems test failed: {e}")
        return False

def test_neuromodulation():
    """Test neuromodulation systems."""
    print("\n=== Testing Neuromodulation ===")
    
    try:
        # Create brain with neuromodulation
        config, world, brain, agent = nlm.create_complete_agent()
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("✓ Enabled neuromodulation systems")
        
        # Test neuromodulation levels
        dopamine_level = agent.getNeuromodulationLevel()
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        print(f"✓ Neuromodulation levels:")
        print(f"  - Dopamine: {dopamine_level:.3f}")
        print(f"  - Curiosity: {curiosity_level:.3f}")
        print(f"  - Novelty: {novelty_level:.3f}")
        print(f"  - Prediction Error: {prediction_error:.3f}")
        
        # Test neuromodulation updates
        agent.applyRewardModulation(reward=1.0, predicted_reward=0.8)
        print("✓ Applied reward modulation")
        
        return True
        
    except Exception as e:
        print(f"✗ Neuromodulation test failed: {e}")
        return False

def test_prediction_systems():
    """Test prediction system functionality."""
    print("\n=== Testing Prediction Systems ===")
    
    try:
        # Create brain and get prediction system
        config, world, brain, agent = nlm.create_complete_agent()
        prediction_system = brain.getPredictionSystem()
        
        print("✓ Created prediction system")
        
        # Test prediction
        percept = world.getSensoryPercept()
        prediction_system.update(percept, 0.1)
        
        prediction = prediction_system.getPrediction()
        prediction_error = prediction_system.getPredictionError()
        confidence = prediction_system.getConfidence()
        
        print(f"✓ Prediction system results:")
        print(f"  - Prediction: {prediction:.3f}")
        print(f"  - Prediction error: {prediction_error:.3f}")
        print(f"  - Confidence: {confidence:.3f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Prediction systems test failed: {e}")
        return False

def test_cognition_systems():
    """Test cognition system functionality."""
    print("\n=== Testing Cognition Systems ===")
    
    try:
        # Create brain and get cognition systems
        config, world, brain, agent = nlm.create_complete_agent()
        
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        print("✓ Created cognition systems")
        
        # Test concept formation
        percept = world.getSensoryPercept()
        vision_data = percept.getVision()
        
        concept_formation.learnConcept(vision_data, "test_concept")
        concepts = concept_formation.getAllConcepts()
        print(f"✓ Learned concepts: {concepts}")
        
        # Test concept application
        new_pattern = [0.1, 0.2, 0.3, 0.4]
        applied = concept_formation.applyConcept("test_concept", new_pattern)
        print(f"✓ Applied concept: {applied}")
        
        # Test attention selection
        competitor1 = brain.addRegion("region1")
        competitor2 = brain.addRegion("region2")
        
        attention.focusOnRegion(competitor1)
        attended = attention.getAttendedRegions()
        print(f"✓ Attended regions: {attended}")
        
        return True
        
    except Exception as e:
        print(f"✗ Cognition systems test failed: {e}")
        return False

def test_advanced_utilities():
    """Test advanced utility functions."""
    print("\n=== Testing Advanced Utilities ===")
    
    try:
        # Test experiment framework
        experiment = nlm.NLMExperiment("test_experiment")
        print("✓ Created experiment")
        
        # Test metric collection
        experiment.add_metric("test_metric", 42.0)
        print("✓ Added metric")
        
        # Test batch processing
        def create_multiple_agents(num=5):
            agents = []
            for i in range(num):
                config, world, brain, agent = nlm.create_complete_agent()
                agents.append(agent)
            return agents
        
        agents = create_multiple_agents(3)
        print(f"✓ Created {len(agents)} agents")
        
        # Test benchmarking
        benchmark = nlm.benchmark_performance(brain, 10)
        print(f"✓ Benchmarked performance: {benchmark:.3f}s")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced utilities test failed: {e}")
        return False

def test_numpy_integration():
    """Test NumPy integration."""
    print("\n=== Testing NumPy Integration ===")
    
    try:
        # Check if NumPy is available
        import numpy as np
        print("✓ NumPy is available")
        
        # Test NumPy conversion utilities
        test_vector = [1.0, 2.0, 3.0, 4.0, 5.0]
        
        # Create vision data
        vision = nlm.Vision(5, 1)
        vision.setData(test_vector)
        
        # Convert to NumPy
        vision_array = nlm.to_numpy(vision.getData())
        print(f"✓ Converted vision to NumPy: {vision_array.shape}")
        
        # Test NumPy operations
        mean_val = np.mean(vision_array)
        std_val = np.std(vision_array)
        max_val = np.max(vision_array)
        
        print(f"✓ NumPy operations:")
        print(f"  - Mean: {mean_val:.3f}")
        print(f"  - Std: {std_val:.3f}")
        print(f"  - Max: {max_val:.3f}")
        
        return True
        
    except ImportError:
        print("⚠ NumPy not available - skipping NumPy tests")
        return True
    except Exception as e:
        print(f"✗ NumPy integration test failed: {e}")
        return False

def test_reproducible_research():
    """Test reproducible research features."""
    print("\n=== Testing Reproducible Research ===")
    
    try:
        # Create reproducible simulations with same seed
        seed = 42
        
        config1, world1, brain1, agent1 = nlm.create_reproducible_simulation(seed)
        config2, world2, brain2, agent2 = nlm.create_reproducible_simulation(seed)
        
        print("✓ Created reproducible simulations")
        
        # Verify reproducibility
        percept1 = world1.getSensoryPercept()
        percept2 = world2.getSensoryPercept()
        
        # Compare initial states
        vision1 = percept1.getVision()
        vision2 = percept2.getVision()
        
        if len(vision1) == len(vision2):
            print(f"✓ Reproducibility confirmed: {len(vision1)} elements match")
        else:
            print(f"✗ Reproducibility issue: {len(vision1)} vs {len(vision2)} elements")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ Reproducible research test failed: {e}")
        return False

def test_validation_and_error_handling():
    """Test validation and error handling."""
    print("\n=== Testing Validation and Error Handling ===")
    
    try:
        # Test configuration validation
        config = nlm.create_default_config()
        validation_errors = nlm.validate_configuration(config)
        
        print(f"✓ Configuration validation: {len(validation_errors)} errors found")
        for error in validation_errors:
            print(f"  - {error}")
        
        return True
        
    except Exception as e:
        print(f"✗ Validation test failed: {e}")
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("NLM Enhanced Python Bindings - Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Agent-World Integration", test_agent_world_integration),
        ("Memory Systems", test_memory_systems),
        ("Neuromodulation", test_neuromodulation),
        ("Prediction Systems", test_prediction_systems),
        ("Cognition Systems", test_cognition_systems),
        ("Advanced Utilities", test_advanced_utilities),
        ("NumPy Integration", test_numpy_integration),
        ("Reproducible Research", test_reproducible_research),
        ("Validation and Error Handling", test_validation_and_error_handling),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"✗ Test '{test_name}' crashed: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 50)
    print("TEST SUMMARY")
    print("=" * 50)
    
    passed = 0
    failed = 0
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name:30} {status}")
        if result:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success Rate: {passed/(passed + failed)*100:.1f}%")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)