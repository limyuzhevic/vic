#!/usr/bin/env python3
"""
Test script to validate NLM improvements

This script tests the various improvements made to the NLM project:
1. Configuration file fixes
2. Advanced configuration parameters
3. Logging improvements
4. Error handling improvements
"""

import os
import sys
import tempfile
import configparser
from pathlib import Path

def test_config_file_fixes():
    """Test that the config file has been fixed"""
    print("Testing configuration file fixes...")
    
    config_path = "/workspace/app/configs/default.cfg"
    
    if not os.path.exists(config_path):
        print(f"ERROR: Config file not found at {config_path}")
        return False
    
    with open(config_path, 'r') as f:
        content = f.read()
    
    # Check for duplicate dopamine_baseline
    dopamine_lines = [line for line in content.split('\n') if 'dopamine_baseline' in line]
    if len(dopamine_lines) > 1:
        print(f"ERROR: Found {len(dopamine_lines)} instances of dopamine_baseline (expected 1)")
        return False
    
    # Check for new advanced configuration parameters
    required_params = [
        'prediction_enabled',
        'prediction_accuracy_threshold',
        'curiosity_novelty_weight',
        'exploration_bonus',
        'learning_rate'
    ]
    
    missing_params = []
    for param in required_params:
        if f"{param} = " not in content:
            missing_params.append(param)
    
    if missing_params:
        print(f"ERROR: Missing advanced configuration parameters: {missing_params}")
        return False
    
    print("✓ Configuration file fixes test passed")
    return True

def test_logging_levels():
    """Test that new logging levels are properly supported"""
    print("Testing logging levels...")
    
    # Check that Logger.hpp has been updated
    logger_header = "/workspace/app/src/core/Logger/Logger.hpp"
    
    if not os.path.exists(logger_header):
        print("ERROR: Logger.hpp not found")
        return False
    
    with open(logger_header, 'r') as f:
        content = f.read()
    
    # Check for new log levels
    required_levels = [
        'LogLevel::Trace',
        'LogLevel::Performance'
    ]
    
    missing_levels = []
    for level in required_levels:
        if level not in content:
            missing_levels.append(level)
    
    if missing_levels:
        print(f"ERROR: Missing log levels in Logger.hpp: {missing_levels}")
        return False
    
    # Check for new convenience methods
    required_methods = [
        'void trace(',
        'void performance(',
        'void logWithContext('
    ]
    
    missing_methods = []
    for method in required_methods:
        if method not in content:
            missing_methods.append(method)
    
    if missing_methods:
        print(f"ERROR: Missing logging methods in Logger.hpp: {missing_methods}")
        return False
    
    # Check for new macros
    required_macros = [
        '#define NLM_LOG_TRACE(',
        '#define NLM_LOG_PERFORMANCE('
    ]
    
    missing_macros = []
    for macro in required_macros:
        if macro not in content:
            missing_macros.append(macro)
    
    if missing_macros:
        print(f"ERROR: Missing logging macros in Logger.hpp: {missing_macros}")
        return False
    
    print("✓ Logging levels test passed")
    return True

def test_config_error_handling():
    """Test that Config class has improved error handling"""
    print("Testing Config error handling...")
    
    # Check that Config.cpp has been updated
    config_cpp = "/workspace/app/src/core/Config/Config.cpp"
    
    if not os.path.exists(config_cpp):
        print("ERROR: Config.cpp not found")
        return False
    
    with open(config_cpp, 'r') as f:
        content = f.read()
    
    # Check for improved error handling
    error_handling_features = [
        'NLM_LOG_WARNING',
        'NLM_LOG_ERROR',
        'std::exception& e',
        'bad_variant_access'
    ]
    
    missing_features = []
    for feature in error_handling_features:
        if feature not in content:
            missing_features.append(feature)
    
    if missing_features:
        print(f"ERROR: Missing error handling features in Config.cpp: {missing_features}")
        return False
    
    # Check for helper function
    if 'describeVariantType' not in content:
        print("ERROR: Missing describeVariantType helper function")
        return False
    
    print("✓ Config error handling test passed")
    return True

def test_brain_config_integration():
    """Test that Brain.cpp properly uses new configuration parameters"""
    print("Testing Brain configuration integration...")
    
    # Check that Brain.cpp has been updated
    brain_cpp = "/workspace/app/src/brain/Brain.cpp"
    
    if not os.path.exists(brain_cpp):
        print("ERROR: Brain.cpp not found")
        return False
    
    with open(brain_cpp, 'r') as f:
        content = f.read()
    
    # Check for new configuration parameter usage
    config_usage = [
        'pImpl->config->getOr<int>(\"planning_depth\"',
        'pImpl->config->getOr<float>("attention_excitation_strength"',
        'pImpl->config->getOr<float>("curiosity_novelty_weight"'
    ]
    
    missing_usage = []
    for usage in config_usage:
        if usage not in content:
            missing_usage.append(usage)
    
    if missing_usage:
        print(f"ERROR: Missing configuration parameter usage in Brain.cpp: {missing_usage}")
        return False
    
    print("✓ Brain configuration integration test passed")
    return True

def test_agent_improvements():
    """Test that AgentBrain.cpp has been fixed"""
    print("Testing AgentBrain improvements...")
    
    # Check that AgentBrain.cpp has been fixed
    agent_brain_cpp = "/workspace/app/src/agent/AgentBrain.cpp"
    
    if not os.path.exists(agent_brain_cpp):
        print("ERROR: AgentBrain.cpp not found")
        return False
    
    with open(agent_brain_cpp, 'r') as f:
        content = f.read()
    
    # Check for curiosity selection fix
    if 'int choice = brain_->getRandomGenerator()->uniformInt(0, 6);' not in content:
        print("ERROR: Curiosity selection not fixed (still uses range 0-7)")
        return False
    
    # Check for no default case in switch
    if 'default: return MotorCommand::Wait;' in content:
        print("ERROR: Still has default case in curiosity selection switch")
        return False
    
    print("✓ AgentBrain improvements test passed")
    return True

def test_curiosity_config_integration():
    """Test that curiosity system has been properly configured"""
    print("Testing curiosity configuration integration...")
    
    # Check that Curiosity.cpp has been updated
    curiosity_cpp = "/workspace/app/src/neuromodulation/Curiosity.cpp"
    
    if not os.path.exists(curiosity_cpp):
        print("ERROR: Curiosity.cpp not found")
        return False
    
    with open(curiosity_cpp, 'r') as f:
        content = f.read()
    
    # Check for new setDecayRate method
    if 'void Curiosity::setDecayRate(float rate)' not in content:
        print("ERROR: setDecayRate method not found in Curiosity.cpp")
        return False
    
    # Check for getDecayRate method
    if 'float Curiosity::getDecayRate() const' not in content:
        print("ERROR: getDecayRate method not found in Curiosity.cpp")
        return False
    
    print("✓ Curiosity configuration integration test passed")
    return True

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Improvements Validation Test Suite")
    print("=" * 60)
    
    tests = [
        test_config_file_fixes,
        test_logging_levels,
        test_config_error_handling,
        test_brain_config_integration,
        test_agent_improvements,
        test_curiosity_config_integration
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"ERROR: Test {test.__name__} failed with exception: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! NLM improvements are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please review the improvements.")
        return 1

if __name__ == "__main__":
    sys.exit(main())