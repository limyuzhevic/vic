#!/usr/bin/env python3
"""
Basic test framework for NLM (熙然) Python bindings.

This module provides basic functionality testing for the NLM Python bindings,
ensuring that the core functionality works correctly and providing a foundation
for more comprehensive testing.
"""

import sys
import os
import pytest
from unittest.mock import Mock, patch, MagicMock

# Add the current directory to Python path for module discovery
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import the NLM module
try:
    import nlm
    NLM_AVAILABLE = True
except ImportError as e:
    NLM_AVAILABLE = False
    print(f"Warning: NLM module not available: {e}")

class TestModuleImport:
    """Test that the NLM module can be imported and has expected attributes."""
    
    def test_module_imports(self):
        """Test that the main module imports successfully."""
        assert NLM_AVAILABLE, "NLM module should be available"
    
    def test_version_attribute(self):
        """Test that nlm has a __version__ attribute."""
        if NLM_AVAILABLE:
            assert hasattr(nlm, '__version__'), "nlm should have __version__ attribute"
            assert isinstance(nlm.__version__, str), "__version__ should be a string"
    
    def test_exception_classes(self):
        """Test that expected exception classes are available."""
        if NLM_AVAILABLE:
            assert hasattr(nlm, 'RuntimeError'), "nlm should have RuntimeError class"
            assert issubclass(nlm.RuntimeError, Exception), "nlm.RuntimeError should subclass Exception"
class TestConfig:
    """Test Config class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        if not NLM_AVAILABLE:
            pytest.skip("NLM module not available")
        
        self.config = nlm.createDefaultConfig()
    
    def test_config_creation(self):
        """Test that Config can be created."""
        assert self.config is not None
    
    def test_config_has_keys_method(self):
        """Test that Config has the expected keys method."""
        assert hasattr(self.config, 'getKeys'), "Config should have getKeys method"
        keys = self.config.getKeys()
        assert isinstance(keys, list), "getKeys should return a list"
    
    def test_config_save_load(self):
        """Test basic save/load functionality."""
        import tempfile
        import os
        
        # Set some values
        self.config.set('test_int', 42)
        self.config.set('test_float', 3.14)
        self.config.set('test_string', 'hello world')
        self.config.set('test_bool', True)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            filepath = f.name
        
        try:
            # Save config
            success = self.config.saveToFile(filepath)
            assert success, "Config.saveToFile should return True"
            assert os.path.exists(filepath), "Config file should be created"
            
            # Load config
            new_config = nlm.createDefaultConfig()
            success = new_config.loadFromFile(filepath)
            assert success, "Config.loadFromFile should return True"
            
            # Verify values
            assert new_config.getOr('test_int', 0) == 42
            assert abs(new_config.getOr('test_float', 0.0) - 3.14) < 0.01
            assert new_config.getOr('test_string', '') == 'hello world'
            assert new_config.getOr('test_bool', False) is True
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_config_has_summary(self):
        """Test that Config has summary method."""
        assert hasattr(self.config, 'summary'), "Config should have summary method"
        summary = self.config.summary()
        assert isinstance(summary, str), "summary should return a string"
        assert len(summary) > 0, "summary should not be empty"
class TestBrain:
    """Test Brain class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        if not NLM_AVAILABLE:
            pytest.skip("NLM module not available")
        
        # Create a minimal config
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 10)
        config.set('region_count', 1)
        
        self.brain = nlm.createBrain(config)
        
        # Initialize the brain
        assert self.brain.initialize(), "Brain initialization should succeed"
    
    def test_brain_creation(self):
        """Test that Brain can be created and initialized."""
        assert self.brain is not None
    
    def test_brain_step(self):
        """Test that brain stepping works."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Step the brain
        self.brain.step(0)
        
        # Check that step completed without error
        # (spike count might not change, but that's OK)
        assert isinstance(self.brain.getTotalSpikeCount(), int)
    
    def test_brain_getters(self):
        """Test various brain getter methods."""
        assert hasattr(self.brain, 'getTotalNeuronCount')
        assert hasattr(self.brain, 'getTotalSynapseCount')
        assert hasattr(self.brain, 'getActiveNeuronCount')
        assert hasattr(self.brain, 'getFiringNeuronCount')
        assert hasattr(self.brain, 'getAverageFiringRate')
        
        # Call the methods to ensure they work
        neuron_count = self.brain.getTotalNeuronCount()
        assert isinstance(neuron_count, int)
        assert neuron_count >= 0
        
        spike_count = self.brain.getTotalSpikeCount()
        assert isinstance(spike_count, int)
        assert spike_count >= 0
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        assert hasattr(self.brain, 'reset')
        
        # Get initial state
        initial_neurons = self.brain.getTotalNeuronCount()
        
        # Reset the brain
        self.brain.reset()
        
        # Re-initialize
        assert self.brain.initialize(), "Brain should re-initialize after reset"
        
        # Check state is restored
        final_neurons = self.brain.getTotalNeuronCount()
        assert final_neurons == initial_neurons, "Brain should restore initial state after reset"
class TestSimpleWorld:
    """Test SimpleWorld class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        if not NLM_AVAILABLE:
            pytest.skip("NLM module not available")
        
        self.world = nlm.createSimpleWorld()
        
        # Configure world with reasonable dimensions
        self.world.configure(100, 100, 20, 20)
    
    def test_world_creation(self):
        """Test that SimpleWorld can be created."""
        assert self.world is not None
    
    def test_world_configure(self):
        """Test world configuration."""
        assert hasattr(self.world, 'configure')
        
        # Test configuration
        self.world.configure(200, 200, 50, 50)
        
        # Check dimensions
        assert self.world.getWidth() == 200
        assert self.world.getHeight() == 200
    
    def test_world_getters(self):
        """Test world getter methods."""
        assert hasattr(self.world, 'getWidth')
        assert hasattr(self.world, 'getHeight')
        assert hasattr(self.world, 'getMaxEnergy')
        assert hasattr(self.world, 'getEnergyDecayRate')
        assert hasattr(self.world, 'getSimulationTime')
        
        # Call methods to ensure they work
        width = self.world.getWidth()
        assert isinstance(width, int)
        assert width > 0
        
        height = self.world.getHeight()
        assert isinstance(height, int)
        assert height > 0
    
    def test_world_reset(self):
        """Test world reset functionality."""
        assert hasattr(self.world, 'reset')
        
        # Reset world
        self.world.reset()
        
        # Should still be functional after reset
        assert self.world.getWidth() == 100
        assert self.world.getHeight() == 100
class TestAgentBrain:
    """Test AgentBrain class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        if not NLM_AVAILABLE:
            pytest.skip("NLM module not available")
        
        # Create a minimal config and brain
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 50)
        config.set('region_count', 1)
        
        brain = nlm.createBrain(config)
        assert brain.initialize(), "Brain should initialize"
        
        # Create AgentBrain
        self.agent_brain = nlm.createAgentBrain(brain)
        
        # Create and configure world
        self.world = nlm.createSimpleWorld()
        self.world.configure(100, 100, 20, 20)
        
        # Initialize agent brain with world
        self.agent_brain.initialize(self.world)
    
    def test_agent_brain_creation(self):
        """Test that AgentBrain can be created."""
        assert self.agent_brain is not None
    
    def test_agent_brain_getters(self):
        """Test AgentBrain getter methods."""
        assert hasattr(self.agent_brain, 'getSensoryInputSize')
        assert hasattr(self.agent_brain, 'getMotorOutputSize')
        assert hasattr(self.agent_brain, 'getDevelopmentalStage')
        assert hasattr(self.agent_brain, 'getNeuromodulationLevel')
        assert hasattr(self.agent_brain, 'getCuriosityLevel')
        assert hasattr(self.agent_brain, 'getNoveltyLevel')
        assert hasattr(self.agent_brain, 'getPredictionError')
        
        # Call methods to ensure they work
        input_size = self.agent_brain.getSensoryInputSize()
        assert isinstance(input_size, int)
        assert input_size >= 0
        
        motor_size = self.agent_brain.getMotorOutputSize()
        assert isinstance(motor_size, int)
        assert motor_size >= 0
    
    def test_agent_brain_configuration_methods(self):
        """Test AgentBrain configuration methods."""
        assert hasattr(self.agent_brain, 'enableRewardModulation')
        assert hasattr(self.agent_brain, 'enableStructuralPlasticity')
        assert hasattr(self.agent_brain, 'enableDevelopment')
        assert hasattr(self.agent_brain, 'enableCuriosity')
        assert hasattr(self.agent_brain, 'isRewardModulationEnabled')
        assert hasattr(self.agent_brain, 'isStructuralPlasticityEnabled')
        assert hasattr(self.agent_brain, 'isDevelopmentEnabled')
        assert hasattr(self.agent_brain, 'isCuriosityEnabled')
        
        # Test toggling configuration
        self.agent_brain.enableRewardModulation(False)
        assert self.agent_brain.isRewardModulationEnabled() is False
        
        self.agent_brain.enableRewardModulation(True)
        assert self.agent_brain.isRewardModulationEnabled() is True
    
    def test_agent_brain_reset(self):
        """Test AgentBrain reset functionality."""
        assert hasattr(self.agent_brain, 'reset')
        
        # Reset agent
        self.agent_brain.reset()
        
        # Should still be functional after reset
        assert self.agent_brain.getSensoryInputSize() >= 0
def run_basic_tests():
    """Run basic tests and return results."""
    print("Running basic NLM Python binding tests...")
    
    test_results = {
        'total_tests': 0,
        'passed_tests': 0,
        'failed_tests': 0,
        'errors': []
    }
    
    # Get all test classes
    test_classes = [
        TestModuleImport,
        TestConfig,
        TestBrain,
        TestSimpleWorld,
        TestAgentBrain
    ]
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        
        # Create test instance
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [
            method for method in dir(test_instance)
            if method.startswith('test_') and callable(getattr(test_instance, method))
        ]
        
        test_results['total_tests'] += len(test_methods)
        
        for method_name in test_methods:
            try:
                method = getattr(test_instance, method_name)
                
                # Check if setup_method exists and call it
                if hasattr(test_instance, 'setup_method'):
                    test_instance.setup_method()
                
                print(f"  Running {method_name}...")
                method()
                
                test_results['passed_tests'] += 1
                print(f"    ✓ PASSED")
                
            except Exception as e:
                test_results['failed_tests'] += 1
                error_msg = f"{method_name}: {str(e)}"
                test_results['errors'].append(error_msg)
                print(f"    ✗ FAILED: {error_msg}")
    
    return test_results
if __name__ == '__main__':
    # Run tests
    results = run_basic_tests()
    
    # Print summary
    print("\n" + "="*60)
    print("TEST SUMMARY")
    print("="*60)
    print(f"Total tests: {results['total_tests']}")
    print(f"Passed: {results['passed_tests']}")
    print(f"Failed: {results['failed_tests']}")
    
    if results['errors']:
        print("\nErrors:")
        for error in results['errors']:
            print(f"  - {error}")
    
    # Exit with appropriate code
    if results['failed_tests'] > 0:
        sys.exit(1)
    else:
        print("\nAll tests passed!")
        sys.exit(0)
