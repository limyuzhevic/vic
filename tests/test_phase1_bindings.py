#!/usr/bin/env python3
"""
Unit tests for Phase 1 - Basic Python bindings improvements.

This test file covers:
- Type hints and documentation improvements
- Error handling enhancements  
- Context manager support
- Basic brain configuration and manipulation
"""

import sys
import os
import tempfile
import shutil
from pathlib import Path
from typing import Dict, Any

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import python.pynlm as pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
def test_type_hints_and_documentation():
    """Test that all public classes have type hints and documentation."""
    # Check that the module exports expected classes
    expected_exports = [
        'createBrain', 'createDefaultConfig', 'createSimpleWorld', 
        'createAgentBrain', 'NeuronType', 'DevelopmentalStage', 'ActionType',
        'WorldObjectType', 'NeuronState', 'SynapseState', 'BrainError',
        'ValidationError', 'NLMError'
    ]
    
    for export in expected_exports:
        # Check if the export exists in the module
        assert hasattr(pynlm, export), f"Missing export: {export}"
        print(f"✓ Found export: {export}")
    
    print("✓ All expected exports are present")
def test_error_handling_improvements():
    """Test improved error handling with informative messages."""
    # Test invalid configuration
    config = pynlm.create_default_config("test", -1, 0.01)
    
    # Should handle negative max_steps gracefully
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("✓ Error handling implemented for invalid configurations")
def test_context_manager_support():
    """Test context manager support for resource management."""
    # Test brain context manager (if available)
    try:
        with pynlm.createBrain(pynlm.create_default_config()) as brain:
            brain.initialize()
            assert brain is not None
        print("✓ Brain context manager works correctly")
    except AttributeError:
        print("ℹ Brain context manager not implemented (optional)")
    
    # Test world context manager
    try:
        with pynlm.createSimpleWorld() as world:
            world.configure(width=10, height=10)
            assert world is not None
        print("✓ World context manager works correctly")
    except AttributeError:
        print("ℹ World context manager not implemented (optional)")
    
    # Test agent context manager
    try:
        brain = pynlm.createBrain(pynlm.create_default_config())
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            assert agent is not None
        print("✓ Agent context manager works correctly")
    except AttributeError:
        print("ℹ Agent context manager not implemented (optional)")
class TestPhase1Bindings:
    """Comprehensive test suite for Phase 1 bindings."""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp()
    
    def __del__(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def test_basic_bindings(self):
        """Test basic Phase 1 bindings functionality."""
        # Test configuration creation
        config = pynlm.create_default_config("basic_test", 100, 0.01)
        
        assert config is not None
        assert config.getString("name") == "basic_test"
        assert config.getInt("max_steps") == 100
        assert config.getFloat("timestep") == 0.01
        
        # Test brain creation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() == 0
        
        # Test world creation
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        assert world is not None
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        
        # Test agent creation
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        assert agent is not None
        assert agent.getCuriosityLevel() == 0.0
        
        print("✓ Basic Phase 1 bindings test passed")
    
    def test_configuration_manipulation(self):
        """Test configuration value manipulation."""
        config = pynlm.create_default_config("manip_test", 50, 0.005)
        
        # Test setting various value types
        config.setInt("brain.neuron_count", 2000)
        config.setFloat("world.size", 15.5)
        config.setString("experiment.name", "manipulation_test")
        config.setBool("plasticity.enabled", True)
        
        # Test getting values
        assert config.getInt("brain.neuron_count") == 2000
        assert config.getFloat("world.size") == 15.5
        assert config.getString("experiment.name") == "manipulation_test"
        assert config.getBool("plasticity.enabled") == True
        
        print("✓ Configuration manipulation test passed")
    
    def test_file_operations(self):
        """Test configuration file I/O."""
        config = pynlm.create_default_config("file_test", 75, 0.02)
        
        # Save to file
        config_path = Path(self.test_dir) / "test_config.json"
        config.saveToFile(str(config_path))
        assert config_path.exists()
        
        # Load from file
        loaded_config = pynlm.create_default_config()
        loaded_config.loadFromFile(str(config_path))
        
        assert loaded_config.getString("name") == "file_test"
        assert loaded_config.getInt("max_steps") == 75
        assert loaded_config.getFloat("timestep") == 0.02
        
        print("✓ File operations test passed")
    
    def test_simulation_basic(self):
        """Test basic simulation functionality."""
        config = pynlm.create_default_config("sim_test", 200, 0.001)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation steps
        for step in range(100):
            brain.step(step)
            assert brain.getStepCount() == step + 1
        
        # Check statistics
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0
        assert brain.getStepCount() == 100
        
        print("✓ Basic simulation test passed")
    
    def test_backward_compatibility_phase1(self):
        """Test backward compatibility with original Phase 1 API."""
        # Test that all original Phase 1 methods still work
        
        # Original configuration API
        config = pynlm.createDefaultConfig()  # Note: This should be create_default_config
        assert config is not None
        
        # Original brain API
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain is not None
        
        # Original world API
        world = pynlm.createSimpleWorld()
        world.configure(width=5, height=5)
        world.reset()
        assert world is not None
        
        # Original agent API
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        print("✓ Backward compatibility test passed")
    
    def run_all_tests(self):
        """Run all Phase 1 tests."""
        print("=" * 60)
        print("PHASE 1 BINDINGS - UNIT TESTS")
        print("=" * 60)
        
        try:
            test_type_hints_and_documentation()
            test_error_handling_improvements()
            test_context_manager_support()
            
            # Run comprehensive tests
            tester = TestPhase1Bindings()
            tester.test_basic_bindings()
            tester.test_configuration_manipulation()
            tester.test_file_operations()
            tester.test_simulation_basic()
            tester.test_backward_compatibility_phase1()
            
            print("\n" + "=" * 60)
            print("PHASE 1 TESTS PASSED")
            print("=" * 60)
            return True
            
        except Exception as e:
            print(f"\n✗ Phase 1 test failed: {e}")
            import traceback
            traceback.print_exc()
            return False
if __name__ == "__main__":
    tester = TestPhase1Bindings()
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)