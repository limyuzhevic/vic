#!/usr/bin/env python3
"""
Basic tests for NLM Python bindings.

Tests the new functionality added during comprehensive improvements:
- Custom exception classes
- Advanced features for expert users
- Convenience functions and utilities
- Performance optimizations
- Error handling
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_imports():
    """Test that all expected classes and exceptions are available."""
    print("\n=== Testing Basic Imports ===")
    
    # Test basic classes
    brain_classes = [pynlm.Brain, pynlm.Config, pynlm.AgentBrain, pynlm.SimpleWorld]
    print(f"✓ Found {len(brain_classes)} basic classes")
    
    # Test identifier classes
    id_classes = [pynlm.NeuronId, pynlm.SynapseId, pynlm.RegionId, pynlm.PopulationId]
    print(f"✓ Found {len(id_classes)} identifier classes")
    
    # Test enum classes
    enums = [pynlm.NeuronType, pynlm.SynapseType, pynlm.DevelopmentalStage,
             pynlm.FiringState, pynlm.ActionType, pynlm.MotorCommand,
             pynlm.WorldObjectType]
    print(f"✓ Found {len(enums)} enum classes")
    
    # Test exception classes
    exceptions = [pynlm.BrainError, pynlm.ConfigError, pynlm.WorldError,
                  pynlm.MemoryError, pynlm.ConnectivityError]
    print(f"✓ Found {len(exceptions)} custom exception classes")
    
    # Test convenience functions
    convenience_funcs = [pynlm.createDefaultConfig, pynlm.createMinimalConfig,
                        pynlm.createDevelopmentConfig, pynlm.runSimulation,
                        pynlm.getBrainStatistics, pynlm.checkpointBrain]
    print(f"✓ Found {len(convenience_funcs)} convenience functions")
    
    print("✓ All basic imports test passed")

def test_config_creation():
    """Test configuration creation functions."""
    print("\n=== Testing Config Creation ===")
    
    # Test different config types
    default_config = pynlm.createDefaultConfig()
    print(f"✓ Created default config: {type(default_config).__name__}")
    
    minimal_config = pynlm.createMinimalConfig()
    print(f"✓ Created minimal config")
    
    development_config = pynlm.createDevelopmentConfig()
    print(f"✓ Created development config")
    
    # Test config utility functions
    template = pynlm.getDefaultConfigurationTemplate()
    print(f"✓ Got configuration template: {type(template).__name__}")
    
    validated = pynlm.validateConfig(default_config)
    print(f"✓ Validated config: {type(validated).__name__}")
    
    print("✓ Config creation test passed")

def test_advanced_features():
    """Test advanced features for expert users."""
    print("\n=== Testing Advanced Features ===")
    
    # Create a brain for testing
    config = pynlm.createMinimalConfig()
    brain = pynlm.Brain(config)
    brain.initialize()
    print(f"✓ Created and initialized brain")
    
    # Test system compatibility report
    compatibility = pynlm.getSystemCompatibilityReport(brain)
    print(f"✓ Generated system compatibility report")
    
    # Test brain description
    description = pynlm.getBrainDescription(brain)
    print(f"✓ Generated brain description")
    
    # Test memory system operations
    pynlm.clearMemorySystem(brain)
    print(f"✓ Cleared memory system")
    
    # Test system info
    system_info = pynlm.getSystemInfo()
    print(f"✓ Retrieved system info")
    
    print("✓ Advanced features test passed")

def test_simulation_utilities():
    """Test simulation utilities and batch operations."""
    print("\n=== Testing Simulation Utilities ===")
    
    # Create brains for testing
    config1 = pynlm.createMinimalConfig()
    config2 = pynlm.createDevelopmentConfig()
    
    brain1 = pynlm.Brain(config1)
    brain2 = pynlm.Brain(config2)
    
    brain1.initialize()
    brain2.initialize()
    
    # Test batch simulation
    brains = pynlm.runMultipleSimulations([config1, config2], 10)
    print(f"✓ Ran multiple simulations: {len(brains)} brains")
    
    # Test simulation comparison
    comparison = pynlm.compareSimulations(brains, "total_neurons")
    print(f"✓ Compared simulations: {type(comparison).__name__}")
    
    # Test adaptive simulation
    def simple_condition(brain):
        return brain.getTotalNeuronCount() > 1000
    
    completed = pynlm.runAdaptiveSimulation(brain1, 50, simple_condition)
    print(f"✓ Ran adaptive simulation: {completed} steps")
    
    # Test benchmark
    benchmark = pynlm.benchmarkBrainPerformance(brain1, 20)
    print(f"✓ Benchmarked brain performance")
    
    print("✓ Simulation utilities test passed")

def test_error_handling():
    """Test error handling and exception classes."""
    print("\n=== Testing Error Handling ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Test safe operations
    result = pynlm.safeBrainOperation(brain, lambda b: b.getTotalNeuronCount())
    print(f"✓ Safe operation result: {result}")
    
    # Test safe simulation step
    safe_step = pynlm.safeSimulationStep(brain, 0)
    print(f"✓ Safe simulation step: {safe_step}")
    
    # Test exception types
    test_exception = pynlm.BrainError("Test brain error")
    print(f"✓ Created BrainError: {type(test_exception).__name__}")
    
    test_config_error = pynlm.ConfigError("Test config error")
    print(f"✓ Created ConfigError: {type(test_config_error).__name__}")
    
    print("✓ Error handling test passed")

def test_world_and_agent_features():
    """Test world and agent features."""
    print("\n=== Testing World and Agent Features ===")
    
    # Create world
    world = pynlm.createEmptyWorld(20, 20)
    print(f"✓ Created empty world: {world.getWidth()}x{world.getHeight()}")
    
    # Create agent brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.Brain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    print(f"✓ Created agent brain interface")
    
    # Test agent features
    enabled_agent = pynlm.enableAgentFeatures(agent_brain, True, False, False)
    print(f"✓ Enabled agent features")
    
    # Test data export/import utilities
    data_types = ["network", "statistics", "connectivity"]
    exported = pynlm.exportBrainData(brain, "./test_export", data_types)
    print(f"✓ Exported brain data: {len(exported)} file types")
    
    print("✓ World and agent features test passed")

def test_performance_monitoring():
    """Test performance monitoring utilities."""
    print("\n=== Testing Performance Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Test resource monitoring
    resource_data = pynlm.monitorResourceUsage(brain, 5)
    print(f"✓ Monitored resources: {type(resource_data).__name__}")
    
    # Test performance profiling
    profile = pynlm.profileBrainStep(brain, 0)
    print(f"✓ Profiled brain step: {type(profile).__name__}")
    
    # Test memory usage
    memory_info = pynlm.getMemoryUsage()
    print(f"✓ Retrieved memory info: {type(memory_info).__name__}")
    
    # Test system requirements
    requirements = pynlm.checkSystemRequirements()
    print(f"✓ Checked system requirements: {type(requirements).__name__}")
    
    print("✓ Performance monitoring test passed")

def test_utility_functions():
    """Test miscellaneous utility functions."""
    print("\n=== Testing Utility Functions ===")
    
    # Test unique ID generation
    brain_id1 = pynlm.generateBrainID()
    brain_id2 = pynlm.generateBrainID()
    print(f"✓ Generated brain IDs: {brain_id1}, {brain_id2}")
    
    # Test config validation
    config = pynlm.createMinimalConfig()
    validation = pynlm.validateConfig(config)
    print(f"✓ Validated config: {type(validation).__name__}")
    
    # Test config merging
    base_config = pynlm.createDefaultConfig()
    overrides = {"neuron_count": 500}
    merged = pynlm.mergeConfigs(base_config, overrides)
    print(f"✓ Merged config: {type(merged).__name__}")
    
    # Test custom config creation
    custom_config_dict = {"neuron_count": 200, "region_count": 2}
    custom_config = pynlm.createCustomConfig(custom_config_dict)
    print(f"✓ Created custom config: {type(custom_config).__name__}")
    
    # Test statistics formatting
    config = pynlm.createDefaultConfig()
    stats_str = pynlm.formatBrainStatistics(config)
    print(f"✓ Formatted statistics: {type(stats_str).__name__}")
    
    print("✓ Utility functions test passed")

def main():
    """Run all tests."""
    print("NLM Python Bindings - Basic Test Suite")
    print("=" * 50)
    
    tests = [
        test_basic_imports,
        test_config_creation,
        test_advanced_features,
        test_simulation_utilities,
        test_error_handling,
        test_world_and_agent_features,
        test_performance_monitoring,
        test_utility_functions,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ Test failed: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print(f"❌ {failed} tests failed")
        sys.exit(1)
    else:
        print("✅ All tests passed!")
        sys.exit(0)

if __name__ == "__main__":
    main()
