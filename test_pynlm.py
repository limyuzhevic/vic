"""
Test script to verify NLM Python package structure and functionality.

This script tests the basic functionality of the NLM Python bindings
to ensure the package is properly structured and functional.
"""

import sys
import os

def test_python_package_structure():
    """Test that the Python package structure is correct"""
    print("Testing Python package structure...")
    
    # Check that essential files exist
    essential_files = [
        "__init__.py",
        "core.py",
        "enums.py", 
        "types.py",
        "constants.py",
        "visualization.py",
        "checkpoint.py",
        "bindings.py",
    ]
    
    for filename in essential_files:
        filepath = os.path.join("python", filename)
        if os.path.exists(filepath):
            print(f"✓ {filename} exists")
        else:
            print(f"✗ {filename} missing")
            return False
    
    return True

def test_imports():
    """Test that all modules can be imported"""
    print("\nTesting imports...")
    
    try:
        # Test main package import
        import pynlm
        print(f"✓ pynlm imported successfully (version {pynlm.__version__})")
        
        # Test submodule imports
        from pynlm import enums, types, constants, visualization, checkpoint
        print("✓ Submodules imported successfully")
        
        # Test core imports
        from pynlm.core import createDefaultConfig, createBrain, createSimpleWorld
        print("✓ Core functions imported successfully")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False

def test_basic_functionality():
    """Test basic Python functionality"""
    print("\nTesting basic functionality...")
    
    try:
        import pynlm
        
        # Test factory functions
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        # Test configuration operations
        config.set("brain.neuron_count", 1000)
        print("✓ config.set() works")
        
        if config.has("brain.neuron_count"):
            print("✓ config.has() works")
        
        # Test enumeration access
        from pynlm.enums import NeuronType, ActionType, DevelopmentalStage
        print(f"✓ NeuronType: {NeuronType.Excitatory}")
        print(f"✓ ActionType: {ActionType.MoveForward}")
        print(f"✓ DevelopmentalStage: {DevelopmentalStage.Adult}")
        
        # Test type checking
        validated_neuron = pynlm.enums.validate_neuron_type("excitatory")
        print(f"✓ Type validation works: {validated_neuron}")
        
        # Test constants
        from pynlm.constants import CompressionLevel, CheckpointSection
        print(f"✓ CompressionLevel: {CompressionLevel.BALANCED}")
        print(f"✓ CheckpointSection: {CheckpointSection.HEADER}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_enumerations():
    """Test enumeration functionality"""
    print("\nTesting enumerations...")
    
    try:
        from pynlm.enums import (
            NeuronType, SynapseType, DevelopmentalStage, FiringState,
            ActionType, MotorCommand, WorldObjectType
        )
        
        # Test enumeration methods
        print(f"✓ NeuronType.get_names(): {NeuronType.get_names()}")
        print(f"✓ ActionType.get_movement_actions(): {ActionType.get_movement_actions()}")
        print(f"✓ DevelopmentalStage.get_early_stages(): {DevelopmentalStage.get_early_stages()}")
        
        # Test string conversion
        print(f"✓ String conversion: {pynlm.enums.get_neuron_type_string(NeuronType.Excitatory)}")
        
        # Test type validation
        print(f"✓ Type validation: {pynlm.enums.validate_neuron_type('inhibitory')}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enumeration test failed: {e}")
        return False

def test_constants():
    """Test constants functionality"""
    print("\nTesting constants...")
    
    try:
        from pynlm.constants import (
            SimulationConstants, ErrorConstants, ConfigConstants, SystemInfo
        )
        
        # Test simulation constants
        print(f"✓ Default neuron count: {SimulationConstants.get_default('neuron_count')}")
        
        # Test error constants
        print(f"✓ Error message: {ErrorConstants.get_error_message(1)}")
        
        # Test config constants
        print(f"✓ Brain prefix: {ConfigConstants.get_config_prefix('brain')}")
        
        # Test system info
        print(f"✓ Python version: {SystemInfo.get_python_version()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Constants test failed: {e}")
        return False

def test_visualization():
    """Test visualization functionality"""
    print("\nTesting visualization...")
    
    try:
        from pynlm.visualization import (
            NLMVisualizer, create_visualizer, visualize_brain
        )
        
        # Test visualizer creation
        visualizer = create_visualizer("stub")
        print("✓ Visualizer created successfully")
        
        # Test backend switching
        available = visualizer.get_available_backends()
        print(f"✓ Available backends: {available}")
        
        # Test visualizer methods
        print(f"✓ Visualizer active: {visualizer.is_active()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Visualization test failed: {e}")
        return False

def test_checkpoint():
    """Test checkpoint functionality"""
    print("\nTesting checkpoint...")
    
    try:
        from pynlm.checkpoint import (
            CheckpointManager, CheckpointInfo, CheckpointStatus
        )
        
        # Test checkpoint info
        import tempfile
        with tempfile.NamedTemporaryFile(delete=False) as tmp:
            tmp_path = tmp.name
        
        checkpoint_info = CheckpointInfo(tmp_path, "test_checkpoint")
        print(f"✓ Checkpoint info created: {checkpoint_info.name}")
        print(f"✓ Checkpoint status: {checkpoint_info.status}")
        
        # Test checkpoint manager
        manager = CheckpointManager("/tmp/test_checkpoints")
        print("✓ Checkpoint manager created")
        
        stats = manager.get_stats()
        print(f"✓ Checkpoint stats: {stats}")
        
        return True
        
    except Exception as e:
        print(f"✗ Checkpoint test failed: {e}")
        return False

def test_error_handling():
    """Test error handling functionality"""
    print("\nTesting error handling...")
    
    try:
        from pynlm import NLMError, ConfigurationError, SimulationError
        
        # Test exception creation
        error = NLMError("Test error", 1)
        print(f"✓ NLMError created: {error}")
        
        config_error = ConfigurationError("Config error", 2)
        print(f"✓ ConfigurationError created: {config_error}")
        
        simulation_error = SimulationError("Simulation error", 4)
        print(f"✓ SimulationError created: {simulation_error}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Package Test Suite")
    print("=" * 60)
    
    tests = [
        ("Package Structure", test_python_package_structure),
        ("Imports", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Enumerations", test_enumerations),
        ("Constants", test_constants),
        ("Visualization", test_visualization),
        ("Checkpoint", test_checkpoint),
        ("Error Handling", test_error_handling),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} ERROR: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())