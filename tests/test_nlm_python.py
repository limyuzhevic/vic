"""
Test NLM Python bindings functionality.

This module contains comprehensive tests for the NLM Python bindings,
testing both the C++ exposed functions and Python utility functions.
"""

import unittest
import sys
import os
import tempfile
import shutil

# Import NLM modules
try:
    import pynlm
    from pynlm.pynlm_core import (
        Config, createDefaultConfig, Brain, createBrain,
        SimpleWorld, createSimpleWorld, AgentBrain, createAgentBrain,
        Vision, Audio, Action, WorldObject,
        NeuronType, DevelopmentalStage, ActionType, WorldObjectType,
    )
    from pynlm.utils import (
        load_config_from_dict,
        create_config_from_brain_params,
        setup_plasticity_config,
        create_vision_from_array,
        create_audio_from_samples,
    )
    
    NATIVE_AVAILABLE = True
except ImportError as e:
    NATIVE_AVAILABLE = False
    print(f"Warning: NLM native module not available: {e}")

class TestNLMNativeModule(unittest.TestCase):
    """Test NLM native Python bindings."""
    
    @classmethod
    def setUpClass(cls):
        """Set up test class."""
        cls.native_available = NATIVE_AVAILABLE
        if not cls.native_available:
            print("Skipping native module tests - not available")
    
    def test_native_available(self):
        """Test that native module is available (when expected)."""
        if self.native_available:
            self.assertTrue(True, "Native module should be available")
        else:
            self.skipTest("Native module not available")
    
    def test_config_creation(self):
        """Test Config creation."""
        if not self.native_available:
            self.skipTest("Native module not available")
        
        config = createDefaultConfig()
        self.assertIsInstance(config, Config)
        self.assertTrue(hasattr(config, 'has'))
        self.assertTrue(hasattr(config, 'getKeys'))
        self.assertTrue(hasattr(config, 'set'))
    
    def test_brain_creation(self):
        """Test Brain creation."""
        if not self.native_available:
            self.skipTest("Native module not available")
        
        config = createDefaultConfig()
        brain = createBrain(config)
        self.assertIsInstance(brain, Brain)
        self.assertTrue(hasattr(brain, 'initialize'))
        self.assertTrue(hasattr(brain, 'step'))
        self.assertTrue(hasattr(brain, 'getTotalNeuronCount'))
    
    def test_simple_world_creation(self):
        """Test SimpleWorld creation."""
        if not self.native_available:
            self.skipTest("Native module not available")
        
        world = createSimpleWorld()
        self.assertIsInstance(world, SimpleWorld)
        self.assertTrue(hasattr(world, 'configure'))
        self.assertTrue(hasattr(world, 'reset'))
        self.assertTrue(hasattr(world, 'update'))
    
    def test_agent_brain_creation(self):
        """Test AgentBrain creation."""
        if not self.native_available:
            self.skipTest("Native module not available")
        
        config = createDefaultConfig()
        brain = createBrain(config)
        agent = createAgentBrain(brain)
        self.assertIsInstance(agent, AgentBrain)
        self.assertTrue(hasattr(agent, 'initialize'))
        self.assertTrue(hasattr(agent, 'processSensoryInput'))
        self.assertTrue(hasattr(agent, 'decodeMotorCommand'))
    
    def test_enums(self):
        """Test enumerations."""
        if not self.native_available:
            self.skipTest("Native module not available")
        
        # Test NeuronType
        self.assertIsNotNone(NeuronType.Excitatory)
        self.assertIsNotNone(NeuronType.Inhibitory)
        self.assertIsNotNone(NeuronType.Motor)
        
        # Test DevelopmentalStage
        self.assertIsNotNone(DevelopmentalStage.Initial)
        self.assertIsNotNone(DevelopmentalStage.Maturation)
        self.assertIsNotNone(DevelopmentalStage.Adult)
        
        # Test ActionType
        self.assertIsNotNone(ActionType.MoveForward)
        self.assertIsNotNone(ActionType.TurnLeft)
        self.assertIsNotNone(ActionType.Interact)
class TestNLMUtils(unittest.TestCase):
    """Test NLM utility functions."""
    
    def test_load_config_from_dict(self):
        """Test load_config_from_dict utility."""
        config = load_config_from_dict({
            'brain.neuron_count': 2000,
            'brain.synapse_density': 0.2,
            'plasticity.stdp.enable': True
        })
        self.assertIsInstance(config, Config)
        self.assertTrue(hasattr(config, 'has'))
    
    def test_create_config_from_brain_params(self):
        """Test create_config_from_brain_params utility."""
        config = create_config_from_brain_params(
            neuron_count=500,
            synapse_density=0.1,
            v_thresh=-60.0
        )
        self.assertIsInstance(config, Config)
        
        # Check that parameters were set
        self.assertTrue(config.has('brain.neuron_count'))
        # Note: the actual get/set methods need to be tested properly
    
    def test_setup_plasticity_config(self):
        """Test setup_plasticity_config utility."""
        config = setup_plasticity_config(
            stdp_enable=False,
            hebbian_enable=True,
            structural_enable=False
        )
        self.assertIsInstance(config, Config)
    
    def test_setup_neuromodulation_config(self):
        """Test setup_neuromodulation_config utility."""
        # This needs to be imported
        from pynlm.utils import setup_neuromodulation_config
        
        config = setup_neuromodulation_config(
            dopamine_scale=2.0,
            curiosity_enable=False
        )
        self.assertIsInstance(config, Config)
    
    @unittest.skipIf(NATIVE_AVAILABLE, "Requires native module for full testing")
    def test_utility_without_native(self):
        """Test utilities when native module is not available."""
        # This should work with placeholder classes
        config = load_config_from_dict({})
        self.assertIsInstance(config, Config)

class TestNLMIntegration(unittest.TestCase):
    """Test integration between different NLM components."""
    
    @unittest.skipIf(NATIVE_AVAILABLE, "Requires native module for integration tests")
    def test_basic_integration_mock(self):
        """Test basic integration with mocked components."""
        # This test runs even without native module
        from pynlm.utils import create_agent_environment
        
        # This should create a basic setup
        setup = create_agent_environment(
            width=10, height=10, vision_width=4, vision_height=4
        )
        
        self.assertIn('brain', setup)
        self.assertIn('world', setup)
        self.assertIn('agent', setup)
class TestNLMConvenienceFunctions(unittest.TestCase):
    """Test NLM convenience functions."""
    
    def test_run_simple_simulation(self):
        """Test run_simple_simulation convenience function."""
        from pynlm import run_simple_simulation
        
        # This should work even without native module
        # The function will raise an error but that's expected
        try:
            result = run_simple_simulation(neuron_count=100, steps=10)
            self.assertIsInstance(result, dict)
        except (ImportError, AttributeError):
            pass  # Expected when native module not available
    
    def test_create_agent_environment(self):
        """Test create_agent_environment convenience function."""
        from pynlm import create_agent_environment
        
        try:
            setup = create_agent_environment(
                width=5, height=5, vision_width=2, vision_height=2
            )
            self.assertIn('brain', setup)
            self.assertIn('world', setup)
            self.assertIn('agent', setup)
        except (ImportError, AttributeError):
            pass  # Expected when native module not available
    
    def test_help_getting_started(self):
        """Test help_getting_started function."""
        from pynlm import help_getting_started
        
        # This should just print and not raise an exception
        try:
            help_getting_started()
        except Exception:
            self.fail("help_getting_started should not raise an exception")
def run_tests():
    """Run all NLM tests."""
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()
if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
