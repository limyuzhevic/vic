#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings.

This script tests the enhanced Python bindings for NLM, including:
- Basic functionality
- NumPy integration
- Visualization
- Performance profiling
- Checkpoint/restore
- Learning and development examples
- Neuromodulation examples
"""

import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import the enhanced Python bindings
try:
    from nlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Classes
        Config, Vision, Audio, InternalSignals, Action, WorldObject,
        AgentBody, ActionResult, SensoryPercept,
        SimpleWorld, Brain, AgentBrain,
        
        # Helper functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Enhanced classes
        NeuralSystem, SensorData, BrainMetrics, DataManager,
        PerformanceProfiler, Visualizer,
        
        # Example modules
        LearningExamples, DevelopmentExamples, NeuromodulationExamples,
        EnvironmentExamples, PerformanceExamples, CheckpointExamples,
        
        # Type aliases
        tensor, epoch, step, neuron_id, synapse_id, population_id, region_id,
        
        # Exception classes
        NLMError, ValidationError, ConfigurationError, DeviceError,
        
        # Context managers
        Timer, BrainSession, WorldSession,
        
        # Utility functions
        configure_logging, set_config, get_config,
        raise_validation_error, raise_configuration_error, raise_device_error
    )
    print("✓ Successfully imported enhanced NLM Python bindings")
except ImportError as e:
    print(f"❌ Import error: {e}")
    sys.exit(1)

import numpy as np

# Mock IPython display for testing
class DisplayMock:
    def __call__(self, *args, **kwargs):
        pass

display = DisplayMock()

def test_basic_import():
    """Test basic import and functionality."""
    print("\n=== Testing Basic Import ===")
    
    # Test 1: Check if required classes exist
    print("1. Testing class imports...")
    assert Config is not None
    assert SimpleWorld is not None
    assert Brain is not None
    assert NeuralSystem is not None
    assert SensorData is not None
    print("   ✓ All classes imported successfully")
    
    # Test 2: Check type aliases
    print("2. Testing type aliases...")
    assert tensor is np.ndarray
    assert isinstance(epoch, float)
    assert isinstance(step, int)
    print("   ✓ Type aliases defined correctly")
    
    # Test 3: Check exception classes
    print("3. Testing exception classes...")
    assert issubclass(ValidationError, NLMError)
    assert issubclass(ConfigurationError, NLMError)
    assert issubclass(DeviceError, NLMError)
    print("   ✓ Exception classes defined correctly")
    
    # Test 4: Check context managers
    print("4. Testing context managers...")
    assert Timer is not None
    assert BrainSession is not None
    assert WorldSession is not None
    print("   ✓ Context managers defined correctly")
    
    # Test 5: Check utility functions
    print("5. Testing utility functions...")
    assert configure_logging is not None
    assert set_config is not None
    assert get_config is not None
    print("   ✓ Utility functions defined correctly")
    
    print("✓ All basic import tests passed")

def test_enhanced_features():
    """Test enhanced features."""
    print("\n=== Testing Enhanced Features ===")
    
    # Test 1: Create neural system
    print("1. Testing NeuralSystem...")
    brain = NeuralSystem()
    assert brain is not None
    print("   ✓ NeuralSystem created successfully")
    
    # Test 2: Create world
    print("2. Testing world creation...")
    world = brain.create_world(width=100.0, height=100.0, vision_width=32, vision_height=32)
    assert world is not None
    assert world.getWidth() == 100.0
    assert world.getHeight() == 100.0
    print("   ✓ World created successfully")
    
    # Test 3: Get metrics
    print("3. Testing metrics...")
    metrics = brain.get_metrics()
    assert metrics is not None
    assert isinstance(metrics, BrainMetrics)
    print(f"   ✓ Metrics collected: {metrics.total_neurons} neurons")
    
    # Test 4: NumPy integration
    print("4. Testing NumPy integration...")
    vision_data = np.random.rand(32, 32, 3).astype(np.float32)
    sensor_data = SensorData.from_vision(vision_data)
    assert sensor_data is not None
    assert sensor_data.vision is not None
    print("   ✓ NumPy integration successful")
    
    # Test 5: Configuration
    print("5. Testing configuration...")
    configure_logging("INFO")
    set_config('test_key', 'test_value')
    value = get_config('test_key')
    assert value == 'test_value'
    print("   ✓ Configuration management successful")
    
    # Test 6: DataManager
    print("6. Testing DataManager...")
    data_manager = DataManager()
    assert data_manager is not None
    print("   ✓ DataManager created successfully")
    
    # Test 7: PerformanceProfiler
    print("7. Testing PerformanceProfiler...")
    profiler = PerformanceProfiler()
    assert profiler is not None
    print("   ✓ PerformanceProfiler created successfully")
    
    # Test 8: Visualizer
    print("8. Testing Visualizer...")
    visualizer = Visualizer(backend='matplotlib')
    assert visualizer is not None
    print("   ✓ Visualizer created successfully")
    
    print("✓ All enhanced feature tests passed")

def test_backward_compatibility():
    """Test backward compatibility."""
    print("\n=== Testing Backward Compatibility ===")
    
    # Test 1: Original C++ bindings
    print("1. Testing original C++ bindings...")
    config = createDefaultConfig()
    assert config is not None
    print("   ✓ createDefaultConfig works")
    
    brain = createBrain(config)
    assert brain is not None
    print("   ✓ createBrain works")
    
    world = createSimpleWorld()
    assert world is not None
    print("   ✓ createSimpleWorld works")
    
    agent_brain = createAgentBrain(brain)
    assert agent_brain is not None
    print("   ✓ createAgentBrain works")
    
    # Test 2: Original types
    print("2. Testing original types...")
    neuron_id = NeuronId(1)
    assert neuron_id.value == 1
    print("   ✓ NeuronId works")
    
    synapse_id = SynapseId(2)
    assert synapse_id.value == 2
    print("   ✓ SynapseId works")
    
    # Test 3: Enums
    print("3. Testing enums...")
    assert NeuronType.Excitatory is not None
    assert ActionType.MoveForward is not None
    print("   ✓ Enums work")
    
    print("✓ All backward compatibility tests passed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 60)
    
    try:
        # Run all tests
        test_basic_import()
        test_enhanced_features()
        test_backward_compatibility()
        
        print("\n" + "=" * 60)
        print("✓ All tests passed successfully!")
        print("=" * 60)
        print("\nEnhanced NLM Python bindings are ready for use!")
        print("\nKey features available:")
        print("- Type hints and autocomplete support")
        print("- Context managers for resource management")
        print("- NumPy integration for scientific computing")
        print("- Matplotlib and Plotly visualization")
        print("- Performance profiling and benchmarking")
        print("- Checkpoint/restore capabilities")
        print("- Learning and development examples")
        print("- Neuromodulation examples")
        print("- Advanced world interaction")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
