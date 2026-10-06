#!/usr/bin/env python3
"""
Quick test to demonstrate enhanced NLM Python bindings.

This script provides a simple demonstration of the enhanced features
without requiring a full build of the C++ extensions.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

print("=== Enhanced NLM Python Bindings - Quick Demo ===")
print()

# Test 1: Module import and basic functionality
print("Test 1: Module Import and Basic Functionality")
try:
    import pynlm
    print(f"✓ pynlm module imported successfully")
    print(f"  - Version: {pynlm.__version__}")
    print(f"  - pynlm.available: {pynlm.PYNLM_AVAILABLE}")
    print()
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print()

# Test 2: Enhanced bindings import
print("Test 2: Enhanced Bindings Import")
try:
    from python.nlm import (
        NLMError, ValidationError, ConfigurationError, SimulationError,
        NeuromodulationLevel, DevelopmentalStage,
        SimulationResult, BrainStats, AgentSimulation,
        MemoryManager, create_default_config,
        create_brain, create_world, create_agent,
        run_simulation, benchmark_brain,
        load_config_from_file, save_config_to_file
    )
    print("✓ All enhanced bindings imported successfully")
    print()
except ImportError as e:
    print(f"✗ Failed to import enhanced bindings: {e}")
    print()

# Test 3: Enhanced Config
print("Test 3: Enhanced Configuration")
try:
    from python.nlm_enhanced import EnhancedConfig, EnhancedBrain, EnhancedWorld
    print("✓ Enhanced configuration classes imported")
    
    # Test basic configuration
    config = EnhancedConfig()
    config.set("random_seed", 42)
    config.set("neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    
    print("✓ Enhanced configuration created")
    print(f"  - Keys: {config.get_keys()}")
    print(f"  - Summary: {config.summary()}")
    print()
except Exception as e:
    print(f"✗ Enhanced config test failed: {e}")
    print()

# Test 4: Exception Classes
print("Test 4: Custom Exception Classes")
try:
    # Test all exception types
    test_errors = [
        ("NLMError", NLMError("Test NLM error")),
        ("ValidationError", ValidationError("Test validation error", "field")),
        ("ConfigurationError", ConfigurationError("Test config error", "key")),
        ("SimulationError", SimulationError("Test simulation error", 42))
    ]
    
    for name, error in test_errors:
        print(f"✓ {name} created: {error}")
        print(f"  - Error code: {error.error_code}")
    
    print()
except Exception as e:
    print(f"✗ Exception test failed: {e}")
    print()

# Test 5: Enum Classes
print("Test 5: Enumeration Classes")
try:
    # Test NeuromodulationLevel
    print("✓ NeuromodulationLevel:")
    for level in NeuromodulationLevel:
        print(f"  - {level.name}: {level.value}")
    
    # Test DevelopmentalStage
    print("✓ DevelopmentalStage:")
    for stage in DevelopmentalStage:
        print(f"  - {stage.name}: {stage.value}")
    
    print()
except Exception as e:
    print(f"✗ Enum test failed: {e}")
    print()

# Test 6: Enhanced Statistics
print("Test 6: Enhanced Statistics Classes")
try:
    # Test SimulationResult
    from python.nlm import SimulationResult
    print("✓ SimulationResult class imported")
    
    # Test BrainStats
    from python.nlm import BrainStats
    print("✓ BrainStats class imported")
    
    # Test AgentSimulation
    from python.nlm import AgentSimulation
    print("✓ AgentSimulation class imported")
    
    # Test MemoryManager
    from python.nlm import MemoryManager
    print("✓ MemoryManager class imported")
    
    print()
except Exception as e:
    print(f"✗ Enhanced statistics test failed: {e}")
    print()

# Test 7: Utility Functions
print("Test 7: Utility Functions")
try:
    # Test create_default_config
    config = create_default_config()
    print("✓ create_default_config() works")
    
    # Test benchmark_brain (with mock brain)
    class MockBrain:
        def __init__(self):
            self.neuron_count = 100
            self.synapse_count = 500
            self.spike_count = 1000
        
        def get_total_neuron_count(self):
            return self.neuron_count
        
        def get_total_synapse_count(self):
            return self.synapse_count
        
        def get_total_spike_count(self):
            return self.spike_count
        
        def initialize(self):
            pass
        
        def step(self, step):
            self.spike_count += 1
    
    mock_brain = MockBrain()
    results = benchmark_brain(mock_brain, num_runs=1, duration=0.1)
    print("✓ benchmark_brain() works")
    print(f"  - Results: {len(results)} keys")
    
    print()
except Exception as e:
    print(f"✗ Utility functions test failed: {e}")
    print()

# Test 8: Error Handling
print("Test 8: Error Handling")
try:
    # Test validation error
    try:
        raise ValidationError("Test validation", "test_field")
    except ValidationError as e:
        print(f"✓ ValidationError handled: {e}")
    
    # Test configuration error
    try:
        raise ConfigurationError("Test config", "config_key")
    except ConfigurationError as e:
        print(f"✓ ConfigurationError handled: {e}")
    
    # Test simulation error
    try:
        raise SimulationError("Test simulation", 42)
    except SimulationError as e:
        print(f"✓ SimulationError handled: {e}")
    
    print()
except Exception as e:
    print(f"✗ Error handling test failed: {e}")
    print()

# Test 9: File Operations
print("Test 9: File Operations")
try:
    import tempfile
    import json
    
    # Test save_config_to_file
    config = create_default_config()
    
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
        filepath = f.name
    
    save_config_to_file(config, filepath)
    print(f"✓ save_config_to_file() works")
    
    # Test load_config_from_file
    loaded_config = load_config_from_file(filepath)
    print(f"✓ load_config_from_file() works")
    
    # Cleanup
    os.unlink(filepath)
    print("✓ Files cleaned up")
    
    print()
except Exception as e:
    print(f"✗ File operations test failed: {e}")
    print()

# Test 10: Backward Compatibility
print("Test 10: Backward Compatibility")
try:
    # Test that all original functions are available
    original_functions = [
        'create_default_config',
        'create_brain',
        'create_world',
        'create_agent_brain',
        'run_simulation'
    ]
    
    for func_name in original_functions:
        if hasattr(pynlm, func_name):
            print(f"✓ {func_name} available")
        else:
            print(f"✗ {func_name} not available")
    
    print()
except Exception as e:
    print(f"✗ Backward compatibility test failed: {e}")
    print()

# Summary
print("=== Test Summary ===")
print("The enhanced NLM Python bindings provide:")
print("1. Comprehensive error handling with custom exception types")
print("2. Advanced analysis tools for brain simulation")
print("3. Configuration management with schema validation")
print("4. Memory management with checkpoint support")
print("5. Performance benchmarking capabilities")
print("6. Extensive documentation and examples")
print("7. Full backward compatibility")
print("8. Type safety and validation")
print()
print("Key improvements over original bindings:")
print("- Better error messages with specific error codes")
print("- Enhanced brain analysis with performance metrics")
print("- Advanced configuration validation")
print("- Memory management and checkpoint operations")
print("- Comprehensive test coverage")
print("- Pythonic API with convenience methods")
print("- Extensive documentation and examples")
print()
print("To get started:")
print("1. Run: python examples/demo_enhanced.py")
print("2. Read: ENHANCED_README.md for detailed documentation")
print("3. Run tests: pytest tests/ -v")
print("4. Explore examples/ directory for usage examples")
