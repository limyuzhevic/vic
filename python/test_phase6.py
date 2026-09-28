"""
Phase 6 Integration Test Runner

This module provides a comprehensive test suite for Phase 6 integration features.
"""

import sys
import os
from typing import Dict, Any

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from nlm import (
    NLMBrain, NLMWorld, NLMEnvironment,
    create_simple_brain, ConfigurationError, 
    SimulationError, MemoryError,
    NeuronType
)

def test_memory_systems():
    """Test that all memory systems are integrated."""
    try:
        brain = create_simple_brain(100, 1)
        
        # Test working memory
        working_memory = brain._brain.getWorkingMemory()
        if working_memory is None:
            return False, "Working memory system is not integrated"
            
        # Test episodic memory
        episodic_memory = brain._brain.getEpisodicMemory()
        if episodic_memory is None:
            return False, "Episodic memory system is not integrated"
            
        # Test associative memory
        associative_memory = brain._brain.getAssociativeMemory()
        if associative_memory is None:
            return False, "Associative memory system is not integrated"
            
        # Run simulation to generate memory activity
        for step in range(50):
            brain.step(step, step * 0.001)
            
        return True, f"All memory systems integrated. Working traces: {working_memory.getActiveTraces()}, Episodes: {episodic_memory.getEpisodeCount()}"
        
    except Exception as e:
        return False, f"Exception during memory systems test: {str(e)}"

def test_neuromodulation_systems():
    """Test that neuromodulation systems are integrated."""
    try:
        brain = create_simple_brain(100, 1)
        
        # Test all neuromodulation systems
        dopamine = brain._brain.getDopamine()
        curiosity = brain._brain.getCuriosity()
        novelty = brain._brain.getNovelty()
        prediction_error = brain._brain.getPredictionErrorSignal()
        
        if not all([dopamine, curiosity, novelty, prediction_error]):
            return False, "Some neuromodulation systems are not integrated"
            
        # Run simulation with sensory input to stimulate neuromodulation
        for step in range(30):
            brain._brain.injectCurrentToNeurons(NeuronType.Sensory, 5.0f)
            brain.step(step, step * 0.001)
            
        return True, f"All neuromodulation systems integrated. Dopamine: {dopamine.getLevel():.3f}, Curiosity: {curiosity.getLevel():.3f}"
        
    except Exception as e:
        return False, f"Exception during neuromodulation test: {str(e)}"

def test_prediction_system():
    """Test that prediction system is integrated."""
    try:
        brain = create_simple_brain(100, 1)
        
        prediction_system = brain._brain.getPredictionSystem()
        if prediction_system is None:
            return False, "Prediction system is not integrated"
            
        return True, "Prediction system is integrated and functional"
        
    except Exception as e:
        return False, f"Exception during prediction system test: {str(e)}"

def test_cognition_systems():
    """Test that cognition systems are integrated."""
    try:
        brain = create_simple_brain(100, 1)
        
        # Test all cognition systems
        planner = brain._brain.getPlanner()
        concept_formation = brain._brain.getConceptFormation()
        attention = brain._brain.getAttention()
        
        if not all([planner, concept_formation, attention]):
            return False, "Some cognition systems are not integrated"
            
        return True, "All cognition systems are integrated and functional"
        
    except Exception as e:
        return False, f"Exception during cognition systems test: {str(e)}"

def test_development_system():
    """Test that development system is integrated."""
    try:
        brain = create_simple_brain(100, 1)
        
        development_system = brain._brain.getDevelopmentSystem()
        if development_system is None:
            return False, "Development system is not integrated"
            
        return True, "Development system is integrated"
        
    except Exception as e:
        return False, f"Exception during development system test: {str(e)}"

def test_persistent_storage():
    """Test checkpoint save/load functionality."""
    try:
        brain1 = create_simple_brain(100, 1)
        
        for step in range(30):
            brain1.step(step, step * 0.001)
            
        import tempfile
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
            
        save_success = brain1.save(checkpoint_file)
        if not save_success:
            return False, "Failed to save checkpoint"
            
        brain2 = create_simple_brain(100, 1)
        load_success = brain2.load(checkpoint_file)
        
        if not load_success:
            return False, "Failed to load checkpoint"
            
        if os.path.exists(checkpoint_file):
            os.unlink(checkpoint_file)
            
        return True, "Persistent storage (checkpoint save/load) works correctly"
        
    except Exception as e:
        return False, f"Exception during persistent storage test: {str(e)}"

def test_brain_world_integration():
    """Test integration between brain and world systems."""
    try:
        brain = create_simple_brain(100, 1)
        world = NLMWorld()
        
        world.configure(20, 20, 6, 6)
        world.reset()
        world.set_agent_start(10.0, 10.0)
        
        percept = world.update(0.1)
        brain._brain.receiveSensoryInput(percept)
        
        for step in range(20):
            brain.step(step, step * 0.001)
            world.update(0.1)
            
        return True, "Brain and world systems integrate correctly"
        
    except Exception as e:
        return False, f"Exception during brain-world integration test: {str(e)}"

def test_complete_environment():
    """Test complete environment integration."""
    try:
        env = NLMEnvironment()
        env.setup(world_width=15, world_height=15,
                  vision_width=6, vision_height=6,
                  agent_start_x=7.5, agent_start_y=7.5)
        
        episode_stats = env.run_episode(max_steps=30, timestep=0.1)
        
        success = (
            episode_stats['steps_completed'] == 30 and
            episode_stats['total_reward'] >= 0.0
        )
        
        if success:
            return True, f"Agent-environment integration completed {episode_stats['steps_completed']} steps, total reward: {episode_stats['total_reward']:.2f}"
        else:
            return False, f"Agent-environment integration failed. Steps: {episode_stats['steps_completed']}, Reward: {episode_stats['total_reward']:.2f}"
        
    except Exception as e:
        return False, f"Exception during environment integration test: {str(e)}"

def run_phase6_integration_tests():
    """Run all Phase 6 integration tests and generate report."""
    print("=" * 80)
    print("NLM Phase 6 Integration Test Suite")
    print("=" * 80)
    
    tests = [
        ("Memory Systems Integration", test_memory_systems),
        ("Neuromodulation Integration", test_neuromodulation_systems),
        ("Prediction System Integration", test_prediction_system),
        ("Cognition Systems Integration", test_cognition_systems),
        ("Development System Integration", test_development_system),
        ("Persistent Storage", test_persistent_storage),
        ("Brain-World Integration", test_brain_world_integration),
        ("Complete Environment Integration", test_complete_environment),
    ]
    
    results = []
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"Running: {test_name}...", end=" ")
        try:
            success, message = test_func()
            results.append((test_name, success, message))
            
            if success:
                print("PASS")
                passed += 1
            else:
                print("FAIL")
                failed += 1
                print(f"  -> {message}")
                
        except Exception as e:
            print(f"ERROR: {e}")
            results.append((test_name, False, f"Exception: {e}"))
            failed += 1
    
    print("\n" + "=" * 80)
    print("TEST SUMMARY")
    print("=" * 80)
    print(f"Total Tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success Rate: {passed/len(tests)*100:.1f}%")
    
    print("\nDetailed Results:")
    for test_name, success, message in results:
        status = "PASS" if success else "FAIL"
        print(f"  {test_name}: {status}")
        if not success:
            print(f"    -> {message}")
    
    print("\n" + "=" * 80)
    if failed == 0:
        print("🎉 ALL TESTS PASSED! Phase 6 integration is successful.")
    else:
        print(f"⚠️  {failed} test(s) failed. Please review the failures above.")
    
    print("=" * 80)
    
    return {
        'total_tests': len(tests),
        'passed_tests': passed,
        'failed_tests': failed,
        'success_rate': passed / len(tests) * 100,
        'detailed_results': results
    }

if __name__ == '__main__':
    result = run_phase6_integration_tests()
    sys.exit(1 if result['failed_tests'] > 0 else 0)
