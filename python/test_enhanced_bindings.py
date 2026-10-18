# NLM Python Test Suite
# Comprehensive tests for the enhanced NLM Python bindings

import pynlm
import sys
import os

def test_module_info():
    """Test module metadata attributes"""
    print("Testing module metadata...")
    
    # Check version attribute
    assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
    assert isinstance(pynlm.__version__, str), "__version__ should be a string"
    print(f"✓ Version: {pynlm.__version__}")
    
    # Check author attribute
    assert hasattr(pynlm, '__author__'), "Missing __author__ attribute"
    assert isinstance(pynlm.__author__, str), "__author__ should be a string"
    print(f"✓ Author: {pynlm.__author__}")
    
    # Check license attribute
    assert hasattr(pynlm, '__license__'), "Missing __license__ attribute"
    assert isinstance(pynlm.__license__, str), "__license__ should be a string"
    print(f"✓ License: {pynlm.__license__}")

def test_convenience_functions():
    """Test new convenience factory functions"""
    print("\nTesting convenience functions...")
    
    # Test createBrainWithRandomConfig
    brain = pynlm.createBrainWithRandomConfig()
    assert brain is not None, "createBrainWithRandomConfig returned None"
    brain.initialize()
    print("✓ createBrainWithRandomConfig works")
    
    # Test createDefaultBrain
    brain = pynlm.createDefaultBrain()
    assert brain is not None, "createDefaultBrain returned None"
    brain.initialize()
    print("✓ createDefaultBrain works")
    
    # Test createReadyToRunSimpleExperiment
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    assert brain is not None, "createReadyToRunSimpleExperiment returned None brain"
    assert world is not None, "createReadyToRunSimpleExperiment returned None world"
    assert agent is not None, "createReadyToRunSimpleExperiment returned None agent"
    print("✓ createReadyToRunSimpleExperiment works")

def test_utility_functions():
    """Test utility and helper functions"""
    print("\nTesting utility functions...")
    
    # Test to_vision
    vision = pynlm.to_vision(32, 32, 3)
    assert vision.getWidth() == 32, "Vision width mismatch"
    assert vision.getHeight() == 32, "Vision height mismatch"
    assert vision.getChannels() == 3, "Vision channels mismatch"
    print("✓ to_vision works")
    
    # Test to_audio
    audio = pynlm.to_audio(44100, 1024)
    assert audio.getSampleRate() == 44100, "Audio sample rate mismatch"
    assert audio.getNumSamples() == 1024, "Audio num samples mismatch"
    print("✓ to_audio works")
    
    # Test random_position
    x, y = pynlm.random_position()
    assert isinstance(x, float), "Random position x should be float"
    assert isinstance(y, float), "Random position y should be float"
    assert 0 <= x < 100, "Random position x out of range"
    assert 0 <= y < 100, "Random position y out of range"
    print(f"✓ random_position works: ({x:.2f}, {y:.2f})")
    
    # Test random_action
    action = pynlm.random_action()
    assert action is not None, "random_action returned None"
    assert action.getType() in [pynlm.ActionType.MoveForward, 
                               pynlm.ActionType.TurnLeft,
                               pynlm.ActionType.Interact], "Unexpected action type"
    print(f"✓ random_action works: {action.getName()}")

def test_error_handling():
    """Test error handling and validation"""
    print("\nTesting error handling...")
    
    # Test with None arguments
    try:
        pynlm.createBrain(None)
        assert False, "Expected RuntimeError for None config"
    except RuntimeError:
        print("✓ None config validation works")
    
    try:
        pynlm.createAgentBrain(None)
        assert False, "Expected RuntimeError for None brain"
    except RuntimeError:
        print("✓ None brain validation works")
    
    # Test None config in get_brain_stats
    try:
        pynlm.get_brain_stats(None)
        assert False, "Expected RuntimeError for None brain in stats"
    except RuntimeError:
        print("✓ None brain in stats validation works")
    
    # Test None config in get_config_summary
    try:
        pynlm.get_config_summary(None)
        assert False, "Expected RuntimeError for None config"
    except RuntimeError:
        print("✓ None config in summary validation works")

def test_statistics():
    """Test statistics and analysis functions"""
    print("\nTesting statistics functions...")
    
    # Create test setup
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    
    # Test get_brain_stats
    stats = pynlm.get_brain_stats(brain)
    assert isinstance(stats, dict), "Brain stats should return dict"
    
    # Check expected keys
    expected_keys = ['total_neurons', 'total_synapses', 'active_neurons',
                     'firing_neurons', 'total_spikes', 'average_firing_rate',
                     'excitation_inhibition_ratio']
    for key in expected_keys:
        assert key in stats, f"Missing key in brain stats: {key}"
    
    print(f"✓ get_brain_stats works: {stats['total_neurons']} neurons, {stats['total_spikes']} spikes")

def test_config_helpers():
    """Test configuration helper functions"""
    print("\nTesting configuration helpers...")
    
    # Test create_config
    config = pynlm.create_config()
    assert config is not None, "create_config returned None"
    print("✓ create_config works")
    
    # Test create_config_with_seed
    config = pynlm.create_config_with_seed(12345)
    assert config is not None, "create_config_with_seed returned None"
    print("✓ create_config_with_seed works")
    
    # Test get_config_summary
    summary = pynlm.get_config_summary(config)
    assert isinstance(summary, str), "Config summary should be string"
    assert len(summary) > 0, "Config summary should not be empty"
    print(f"✓ get_config_summary works: {summary}")

def test_advanced_simulation():
    """Test advanced simulation features"""
    print("\nTesting advanced simulation features...")
    
    # Create setup
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    
    # Test quickTestRun
    spike_count, x_pos, y_pos = pynlm.quickTestRun(brain, world, agent, 50)
    assert isinstance(spike_count, int), "Spike count should be int"
    assert isinstance(x_pos, float), "X position should be float"
    assert isinstance(y_pos, float), "Y position should be float"
    print(f"✓ quickTestRun works: {spike_count} spikes, position ({x_pos:.1f}, {y_pos:.1f})")
    
    # Test run_learning_simulation
    total_reward, success_count, total_spikes, stage = pynlm.run_learning_simulation(
        brain, world, agent, 30
    )
    assert isinstance(total_reward, float), "Total reward should be float"
    assert isinstance(success_count, int), "Success count should be int"
    assert isinstance(total_spikes, int), "Total spikes should be int"
    assert stage in [pynlm.DevelopmentalStage.Initial,
                     pynlm.DevelopmentalStage.CriticalPeriod,
                     pynlm.DevelopmentalStage.Maturation,
                     pynlm.DevelopmentalStage.Adult,
                     pynlm.DevelopmentalStage.Aging], "Invalid developmental stage"
    print(f"✓ run_learning_simulation works: {total_reward:.2f} reward, {success_count} successes")

def test_batch_processing():
    """Test batch processing functionality"""
    print("\nTesting batch processing...")
    
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    
    # Test batch_process_steps
    call_count = [0]
    
    def custom_processor(brain, step, time):
        call_count[0] += 1
    
    pynlm.batch_process_steps(brain, custom_processor, 0, 10)
    assert call_count[0] == 11, f"Batch processor called {call_count[0]} times, expected 11"
    print(f"✓ batch_process_steps works: {call_count[0]} calls")

def run_all_tests():
    """Run all tests and report results"""
    print("=" * 60)
    print("NLM Python Test Suite - Enhanced Bindings")
    print("=" * 60)
    
    tests = [
        test_module_info,
        test_convenience_functions,
        test_utility_functions,
        test_error_handling,
        test_statistics,
        test_config_helpers,
        test_advanced_simulation,
        test_batch_processing,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("Some tests failed!")
        return False
    else:
        print("All tests passed! 🎉")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)