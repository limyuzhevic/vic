#!/usr/bin/env python3
"""
Validation and Runtime Checking Test Script

Comprehensive test script for pynlm validation and runtime checking utilities
"""

import sys
import os
import tempfile
import traceback

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_config_validation():
    """Test configuration validation utilities"""
    print("Testing configuration validation...")
    
    # Create test configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100, pynlm.ConfigSource.Default)
    config.set("brain.synapse_density", 0.1, pynlm.ConfigSource.Default)
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.Default)
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.Default)
    
    # Test basic validation
    result = pynlm.validation.validateConfig(config)
    assert result.valid, "Configuration should be valid"
    assert len(result.errors) == 0, "Should have no errors"
    
    # Test validation with invalid configuration
    config.set("brain.neuron_count", -1, pynlm.ConfigSource.Default)
    invalid_result = pynlm.validation.validateConfig(config)
    assert not invalid_result.valid, "Configuration should be invalid"
    assert len(invalid_result.errors) > 0, "Should have errors"
    
    # Test brain configuration validation
    brain_result = pynlm.validation.validateBrainConfig(config)
    assert not brain_result.valid, "Brain config should be invalid"
    assert len(brain_result.errors) > 0, "Should have brain validation errors"
    
    # Test agent configuration validation
    agent_result = pynlm.validation.validateAgentConfig(config)
    assert agent_result.valid, "Agent config should be valid"
    
    # Test complete configuration validation
    config.set("development.stage.age_to_mature", -1, pynlm.ConfigSource.Default)
    complete_result = pynlm.validation.validateCompleteConfig(config)
    assert not complete_result.valid, "Complete config should be invalid"
    assert len(complete_result.errors) > 0, "Should have validation errors"
    
    print("✓ Configuration validation test passed")

def test_brain_validation():
    """Test brain validation functionality"""
    print("Testing brain validation...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain stability check
    assert pynlm.runtime.is_brain_stable(brain), "Brain should be stable"
    
    # Test simulation progression check
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    assert pynlm.runtime.is_simulation_progressing(brain, world), "Simulation should be progressing"
    
    # Test runtime statistics
    stats = pynlm.runtime.getRuntimeStats(brain, world)
    assert stats.simulation_time >= 0.0, "Should have simulation time"
    assert stats.spikes_processed >= 0, "Should have spike count"
    assert stats.neurons_active >= 0, "Should have active neuron count"
    
    # Test performance analysis
    analysis = pynlm.runtime.analyzePerformance(stats)
    assert isinstance(analysis, str), "Analysis should be string"
    assert len(analysis) > 0, "Analysis should not be empty"
    
    # Test issue checking
    issues = pynlm.runtime.checkForIssues(brain, world)
    assert isinstance(issues, list), "Issues should be a list"
    
    print("✓ Brain validation test passed")

def test_convenience_functions():
    """Test convenience functions"""
    print("Testing convenience functions...")
    
    # Test validate_configuration with exception
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100, pynlm.ConfigSource.Default)
    
    # Should not raise exception
    result = pynlm.validate_configuration(config)
    assert result.valid, "Configuration should be valid"
    
    # Test invalid configuration should raise exception
    config.set("brain.neuron_count", -1, pynlm.ConfigSource.Default)
    try:
        pynlm.validate_configuration(config)
        assert False, "Should have raised exception for invalid configuration"
    except Exception as e:
        assert "Configuration validation failed" in str(e), "Should have validation error message"
    
    # Test safe_brain_step
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Should work with valid brain
    success = pynlm.safe_brain_step(brain, 0)
    assert success, "Safe brain step should succeed"
    
    # Test check_simulation_health
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    health = pynlm.check_simulation_health(brain, world)
    assert "is_stable" in health, "Should have stability status"
    assert "is_progressing" in health, "Should have progression status"
    assert "issues" in health, "Should have issues list"
    assert "stable_and_progressing" in health, "Should have combined status"
    
    print("✓ Convenience functions test passed")

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    # Test with invalid configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1, pynlm.ConfigSource.Default)
    config.set("brain.synapse_density", 2.0, pynlm.ConfigSource.Default)  # Invalid: > 1.0
    
    # Should raise validation exception
    try:
        pynlm.validate_configuration(config)
        assert False, "Should have raised exception"
    except Exception as e:
        assert "Configuration validation failed" in str(e), "Should have validation error"
    
    # Test brain stability check
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Modify brain to make it unstable (by injecting unrealistic current)
    for neuron_id in brain.getRegionIds():
        region = brain.getRegion(neuron_id)
        for population in region.getPopulations():
            for neuron in population.getNeurons() {
                neuron->injectCurrent(1000.0);  // Very high current
            }
        }
    }
    
    # Should detect instability
    assert not pynlm.runtime.is_brain_stable(brain), "Brain should be unstable"
    
    print("✓ Error handling test passed")

def test_logging():
    """Test logging functionality"""
    print("Testing logging functionality...")
    
    # Test logger
    logger = pynlm.log
    assert logger is not None, "Should have logger"
    
    # Test log methods
    logger.info("Test info message")
    logger.warn("Test warning message")
    logger.error("Test error message")
    logger.debug("Test debug message")
    
    # Test logger interface
    assert hasattr(logger, 'info'), "Logger should have info method"
    assert hasattr(logger, 'warn'), "Logger should have warn method"
    assert hasattr(logger, 'error'), "Logger should have error method"
    assert hasattr(logger, 'debug'), "Logger should have debug method"
    
    print("✓ Logging test passed")

def run_all_tests():
    """Run all validation and runtime checking tests"""
    print("=" * 70)
    print("PYNLM VALIDATION AND RUNTIME CHECKING TEST SUITE")
    print("=" * 70)
    print()
    
    tests = [
        test_config_validation,
        test_brain_validation,
        test_convenience_functions,
        test_error_handling,
        test_logging,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            print(f"\nRunning: {test.__name__}")
            print("-" * 50)
            test()
            passed += 1
            print(f"✓ {test.__name__} PASSED")
        except Exception as e:
            print(f"✗ {test.__name__} FAILED: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 70)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed > 0:
        print("Some tests failed. Please check the output above.")
        return False
    else:
        print("All tests passed!")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)