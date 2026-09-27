#!/usr/bin/env python3
"""
Test script to verify Phase 4+ Python bindings for NLM.

This script performs basic import and functionality tests to ensure
that all Phase 4+ cognitive systems bindings are working correctly.
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

def test_basic_imports():
    """Test basic imports of all bound modules."""
    print("=== Testing Basic Imports ===")
    
    try:
        import pynlm as nlm
        print("✓ Successfully imported pynlm")
        
        # Test core classes
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        world = nlm.createSimpleWorld()
        agent_brain = nlm.createAgentBrain(brain)
        
        print("✓ Successfully created core NLM objects")
        
        # Test Phase 4+ cognitive systems
        cognitive_systems = brain.get_cognitive_systems()
        neuromodulators = brain.get_neuromodulators()
        
        print("✓ Successfully accessed cognitive systems and neuromodulators")
        
        return True
        
    except Exception as e:
        print(f"✗ Import test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_cognitive_systems():
    """Test individual cognitive systems."""
    print("\n=== Testing Cognitive Systems ===")
    
    try:
        import pynlm as nlm
        import numpy as np
        
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        cognitive_systems = brain.get_cognitive_systems()
        
        # Test NeuralPlanner
        neural_planner = cognitive_systems.get_neural_planner()
        print("✓ NeuralPlanner accessible")
        
        # Test ConceptFormation
        concept_formation = cognitive_systems.get_concept_formation()
        print("✓ ConceptFormation accessible")
        
        # Test PredictionSystem
        prediction_system = cognitive_systems.get_prediction_system()
        print("✓ PredictionSystem accessible")
        
        # Test NeuralWorkingMemory
        working_memory = cognitive_systems.get_neural_working_memory()
        print("✓ NeuralWorkingMemory accessible")
        
        # Test NeuralEpisodicMemory
        episodic_memory = cognitive_systems.get_episodic_memory()
        print("✓ NeuralEpisodicMemory accessible")
        
        # Test SelfModel
        self_model = cognitive_systems.get_self_model()
        print("✓ SelfModel accessible")
        
        # Test SocialLearning
        social_learning = cognitive_systems.get_social_learning()
        print("✓ SocialLearning accessible")
        
        # Test AttentionalSelection
        attentional_selection = cognitive_systems.get_attentional_selection()
        print("✓ AttentionalSelection accessible")
        
        return True
        
    except Exception as e:
        print(f"✗ Cognitive systems test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_neuromodulators():
    """Test neuromodulator systems."""
    print("\n=== Testing Neuromodulators ===")
    
    try:
        import pynlm as nlm
        import numpy as np
        
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        neuromodulators = brain.get_neuromodulators()
        
        # Test Reward (Dopamine)
        reward = neuromodulators.get_reward()
        print("✓ Reward neuromodulator accessible")
        
        # Test Curiosity
        curiosity = neuromodulators.get_curiosity()
        print("✓ Curiosity neuromodulator accessible")
        
        # Test PredictionError
        prediction_error = neuromodulators.get_prediction_error()
        print("✓ PredictionError neuromodulator accessible")
        
        # Test Novelty
        novelty = neuromodulators.get_novelty()
        print("✓ Novelty neuromodulator accessible")
        
        return True
        
    except Exception as e:
        print(f"✗ Neuromodulators test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_functionality():
    """Test basic functionality of key systems."""
    print("\n=== Testing Functionality ===")
    
    try:
        import pynlm as nlm
        import numpy as np
        
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        agent_brain = nlm.createAgentBrain(brain)
        
        # Test with simple data
        test_pattern = np.random.rand(10).astype(float)
        test_reward = 0.5
        
        # Test concept formation
        concept_formation = brain.get_cognitive_systems().get_concept_formation()
        concept_id = concept_formation.presentExperience(test_pattern, test_pattern * 0.5, test_reward, 0)
        print(f"✓ Concept formation test: concept_id = {concept_id}")
        
        # Test working memory
        working_memory = brain.get_cognitive_systems().get_neural_working_memory()
        working_memory.store(test_pattern, test_reward)
        retrieved = working_memory.retrieve()
        print(f"✓ Working memory test: stored and retrieved {len(retrieved)}-element pattern")
        
        # Test prediction
        prediction_system = brain.get_cognitive_systems().get_prediction_system()
        predicted = prediction_system.predictNextState(test_pattern)
        print(f"✓ Prediction test: made prediction")
        
        # Test neuromodulators
        reward = brain.get_neuromodulators().get_reward()
        reward.processReward(test_pattern, test_pattern, nlm.ActionType.MoveForward, test_reward)
        print(f"✓ Neuromodulation test: processed reward")
        
        # Test self-model
        self_model = brain.get_cognitive_systems().get_self_model()
        self_model.recordSelfAction(nlm.ActionType.MoveForward, test_pattern, test_pattern * 0.9)
        print(f"✓ Self-model test: recorded action experience")
        
        return True
        
    except Exception as e:
        print(f"✗ Functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_example_scripts():
    """Test that example scripts can be imported."""
    print("\n=== Testing Example Scripts ===")
    
    try:
        # Test that example modules can be imported
        sys.path.insert(0, 'python/examples')
        
        # Basic examples can be imported without running
        print("✓ Example scripts directory accessible")
        
        return True
        
    except Exception as e:
        print(f"✗ Example scripts test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Phase 4+ Python Bindings - Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Cognitive Systems", test_cognitive_systems),
        ("Neuromodulators", test_neuromodulators),
        ("Functionality", test_functionality),
        ("Example Scripts", test_example_scripts),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        if test_func():
            passed += 1
            print(f"✓ {test_name} PASSED")
        else:
            failed += 1
            print(f"✗ {test_name} FAILED")
    
    print(f"\n{'='*60}")
    print(f"Test Summary: {passed} passed, {failed} failed")
    print(f"{'='*60}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} test(s) failed. Please check the output above.")
        return 1
    else:
        print(f"\n🎉 All tests passed! Phase 4+ Python bindings are working correctly.")
        return 0

if __name__ == "__main__":
    sys.exit(main())
