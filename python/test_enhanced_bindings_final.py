#!/usr/bin/env python3
"""
Quick test for enhanced NLM Python bindings.

This script provides a quick test to verify the enhanced Python bindings.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import the enhanced Python bindings
try:
    from nlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Classes
        Config, SimpleWorld, Brain, AgentBrain,
        
        # Helper functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Enhanced classes
        NeuralSystem, SensorData, BrainMetrics, DataManager,
        PerformanceProfiler, Visualizer,
        
        # Example modules
        LearningExamples, DevelopmentExamples, NeuromodulationExamples,
        EnvironmentExamples, PerformanceExamples, CheckpointExamples,
        
        # Exception classes
        NLMError, ValidationError, ConfigurationError, DeviceError,
        
        # Context managers
        Timer, BrainSession, WorldSession,
        
        # Utility functions
        configure_logging, set_config, get_config
    )
    print("✓ Successfully imported enhanced NLM Python bindings")
except ImportError as e:
    print(f"❌ Import error: {e}")
    sys.exit(1)

import numpy as np

def test_enhanced_bindings():
    """Test the enhanced Python bindings."""
    print("\n" + "=" * 60)
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 60)
    
    try:
        # Test 1: Basic import
        print("1. Testing basic imports...")
        assert Config is not None
        assert SimpleWorld is not None
        assert Brain is not None
        assert NeuralSystem is not None
        assert SensorData is not None
        assert BrainMetrics is not None
        print("   ✓ All imports successful")
        
        # Test 2: Create neural system
        print("2. Testing NeuralSystem...")
        brain = NeuralSystem()
        assert brain is not None
        print("   ✓ NeuralSystem created")
        
        # Test 3: Create world
        print("3. Testing world creation...")
        world = brain.create_world(width=100.0, height=100.0, vision_width=32, vision_height=32)
        assert world is not None
        assert world.getWidth() == 100.0
        assert world.getHeight() == 100.0
        print("   ✓ World created")
        
        # Test 4: Get metrics
        print("4. Testing metrics...")
        metrics = brain.get_metrics()
        assert metrics is not None
        assert isinstance(metrics, BrainMetrics)
        print(f"   ✓ Metrics collected: {metrics.total_neurons} neurons")
        
        # Test 5: NumPy integration
        print("5. Testing NumPy integration...")
        vision_data = np.random.rand(32, 32, 3).astype(np.float32)
        sensor_data = SensorData.from_vision(vision_data)
        assert sensor_data is not None
        assert sensor_data.vision is not None
        print("   ✓ NumPy integration successful")
        
        # Test 6: Configuration
        print("6. Testing configuration...")
        configure_logging("INFO")
        set_config('test_key', 'test_value')
        value = get_config('test_key')
        assert value == 'test_value'
        print("   ✓ Configuration management successful")
        
        # Test 7: DataManager
        print("7. Testing DataManager...")
        data_manager = DataManager()
        assert data_manager is not None
        print("   ✓ DataManager created")
        
        # Test 8: PerformanceProfiler
        print("8. Testing PerformanceProfiler...")
        profiler = PerformanceProfiler()
        assert profiler is not None
        print("   ✓ PerformanceProfiler created")
        
        # Test 9: Visualizer
        print("9. Testing Visualizer...")
        visualizer = Visualizer(backend='matplotlib')
        assert visualizer is not None
        print("   ✓ Visualizer created")
        
        # Test 10: Backward compatibility
        print("10. Testing backward compatibility...")
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
        
        # Test 11: Enums
        print("11. Testing enums...")
        assert NeuronType.Excitatory is not None
        assert ActionType.MoveForward is not None
        print("   ✓ Enums work")
        
        # Test 12: Context managers
        print("12. Testing context managers...")
        assert Timer is not None
        assert BrainSession is not None
        assert WorldSession is not None
        print("   ✓ Context managers available")
        
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
    sys.exit(test_enhanced_bindings())
