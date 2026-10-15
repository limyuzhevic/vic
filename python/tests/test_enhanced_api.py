#!/usr/bin/env python3
"""
Test suite for NLM Python enhanced API

This file contains comprehensive tests for the enhanced NLM Python API.
It tests all new functionality, Pythonic features, and advanced utilities.
"""

import sys
import os
import tempfile
import numpy as np
from typing import Dict, Any, List

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm import PythonicBrain, BrainSession, createOptimizedConfig, validateConfiguration
from pynlm.brain import BrainSession as SessionClass
from pynlm.world import SensoryData
from pynlm.agent import ActionInterface, AgentInterface

class TestEnhancedPythonAPI:
    """Test class for enhanced Python API functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = createOptimizedConfig()
        self.brain = PythonicBrain(pynlm.createBrain(self.config))
        self.brain.enable_all_systems()
        self.brain._brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, vision_width=16, vision_height=16)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain._brain)
        self.agent.initialize(self.world)
        
    def test_pythonic_brain_properties(self):
        """Test PythonicBrain properties."""
        # Test properties exist
        assert hasattr(self.brain, 'total_neuron_count')
        assert hasattr(self.brain, 'total_synapse_count')
        assert hasattr(self.brain, 'active_neuron_count')
        assert hasattr(self.brain, 'firing_neuron_count')
        assert hasattr(self.brain, 'total_spike_count')
        assert hasattr(self.brain, 'average_firing_rate')
        assert hasattr(self.brain, 'excitation_inhibition_ratio')
        assert hasattr(self.brain, 'development_stage')
        
        # Test property values are accessible
        neurons = self.brain.total_neuron_count
        assert isinstance(neurons, int)
        assert neurons >= 0
        
        firing_rate = self.brain.average_firing_rate
        assert isinstance(firing_rate, float)
        
    def test_pythonic_brain_summary(self):
        """Test PythonicBrain summary methods."""
        summary = self.brain.get_summary()
        assert isinstance(summary, str)
        assert "PythonicBrain Summary" in summary
        
        detailed_stats = self.brain.get_detailed_stats()
        assert isinstance(detailed_stats, dict)
        assert 'neuron_count' in detailed_stats
        assert 'synapse_count' in detailed_stats
        assert 'average_firing_rate' in detailed_stats
        
    def test_brain_session_context_manager(self):
        """Test BrainSession context manager."""
        # Test context manager usage
        with pynlm.createBrainSession(self.brain._brain, self.world, self.agent) as session:
            assert isinstance(session, SessionClass)
            session.run_simulation(10, 0.1)
            assert isinstance(session.get_stats(), dict)
        
    def test_sensory_data(self):
        """Test SensoryData class."""
        # Test basic sensory data creation
        sensory = SensoryData(
            vision_data=np.array([1.0, 2.0, 3.0]),
            audio_data=np.array([0.5, 0.6]),
            internal_data=np.array([0.1, 0.2, 0.3]),
            timestamp=1.5
        )
        
        assert sensory.has_vision
        assert sensory.has_audio
        assert sensory.has_internal
        assert sensory.get_vision_shape() == (3,)
        assert sensory.get_audio_shape() == (2,)
        assert sensory.get_internal_shape() == (3,)
        assert sensory.timestamp == 1.5
        
        # Test dictionary conversion
        data_dict = sensory.to_dict()
        assert 'vision' in data_dict
        assert 'audio' in data_dict
        assert 'internal' in data_dict
        assert 'timestamp' in data_dict
        
    def test_configuration_validation(self):
        """Test configuration validation."""
        # Test with valid configuration
        errors, warnings = validateConfiguration(self.config)
        assert isinstance(errors, list)
        assert isinstance(warnings, list)
        
        # Test custom settings
        self.config.set("custom.test", "value")
        errors, warnings = validateConfiguration(self.config)
        assert "custom.test" not in " ".join(errors + warnings)  # Should not be in validation errors
        
    def test_configuration_export_import(self):
        """Test configuration export and import."""
        # Export to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Export configuration
            export_result = pynlm.exportConfiguration(self.config, filepath)
            assert export_result is True
            
            # Import configuration
            imported_config = pynlm.importConfiguration(filepath)
            assert imported_config is not None
            
            # Verify imported config has keys
            keys = imported_config.getKeys()
            assert isinstance(keys, list)
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
        
    def test_action_interface(self):
        """Test ActionInterface class."""
        action = ActionInterface(pynlm.ActionType.MoveForward, {"speed": 1.0, "direction": 0.5})
        
        assert action.type == pynlm.ActionType.MoveForward
        assert action.params["speed"] == 1.0
        assert action.params["direction"] == 0.5
        assert action.get_name() == "MoveForward"
        
        # Test parameter manipulation
        action.set_parameter("new_param", "test_value")
        assert action.get_parameter("new_param") == "test_value"
        assert action.get_parameter("nonexistent", "default") == "default"
        
    def test_agent_interface(self):
        """Test AgentInterface class."""
        agent = AgentInterface(self.brain._brain)
        agent.initialize(self.brain._brain)
        
        assert agent.is_initialized is True
        assert agent.get_sensory_input_size() > 0
        assert agent.get_motor_output_size() > 0
        
        # Test system enable/disable
        agent.enable_system("reward_modulation", True)
        assert agent.is_system_enabled("reward_modulation") is True
        
        agent.enable_system("development", False)
        assert agent.is_system_enabled("development") is False
        
    def test_session_run_episode(self):
        """Test session run_episode method."""
        stats = self.agent.run_episode(
            self.brain._brain, 
            self.world, 
            steps=20, 
            timestep=0.1
        )
        
        assert isinstance(stats, dict)
        assert 'agent_sensory_size' in stats
        assert 'agent_motor_size' in stats
        assert 'agent_development_stage' in stats
        
    def test_optimized_config(self):
        """Test optimized configuration."""
        config = createOptimizedConfig()
        
        # Check that optimized settings are applied
        assert config.has("performance.optimization_level")
        assert config.getOr("performance.optimization_level", "") == "high"
        
        assert config.has("memory.pooling")
        assert config.getOr("memory.pooling", False) is True
        
    def test_advanced_features(self):
        """Test advanced features."""
        # Test that advanced features are available
        assert hasattr(pynlm, 'PythonicBrain')
        assert hasattr(pynlm, 'BrainSession')
        assert hasattr(pynlm, 'createOptimizedConfig')
        assert hasattr(pynlm, 'validateConfiguration')
        assert hasattr(pynlm, 'exportConfiguration')
        assert hasattr(pynlm, 'importConfiguration')
        
    def test_all_enums_and_types(self):
        """Test that all enumerations and types are accessible."""
        # Neuron types
        neuron_types = [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory, 
                       pynlm.NeuronType.Modulatory, pynlm.NeuronType.Sensory,
                       pynlm.NeuronType.Motor, pynlm.NeuronType.Internal]
        assert len(neuron_types) == 6
        
        # Synapse types
        synapse_types = [pynlm.SynapseType.Excitatory, pynlm.SynapseType.Inhibitory,
                        pynlm.SynapseType.Modulatory, pynlm.SynapseType.Electrical,
                        pynlm.SynapseType.GapJunction]
        assert len(synapse_types) == 5
        
        # Action types
        action_types = [pynlm.ActionType.MoveForward, pynlm.ActionType.MoveBackward,
                       pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight]
        assert len(action_types) == 4
        
    def test_error_handling(self):
        """Test error handling."""
        # Test that errors are properly raised
        try:
            # Create invalid configuration (should be handled gracefully)
            config = createOptimizedConfig()
            # This should not raise an exception
            config.set("test.invalid", "value")
            
        except Exception as e:
            assert False, f"Unexpected error: {e}"
        
    def test_simulation_consistency(self):
        """Test simulation consistency."""
        # Run simulation multiple times and check consistency
        initial_spikes = self.brain.total_spike_count
        
        for i in range(5):
            self.brain._brain.step(i)
        
        # Should have increased spike count
        assert self.brain.total_spike_count >= initial_spikes
        
    def test_memory_efficiency(self):
        """Test memory efficiency of PythonicBrain."""
        import sys
        
        # Check that PythonicBrain doesn't create excessive memory overhead
        brain_memory = sys.getsizeof(self.brain)
        # This is just a basic check - actual memory usage would require more sophisticated testing
        assert brain_memory < 10000  # Should be reasonable
        
    def test_thread_safety(self):
        """Test basic thread safety (placeholder)."""
        # This would require more sophisticated testing with actual threads
        # For now, just verify that the interface doesn't have obvious thread-safety issues
        assert self.brain.is_initialized
        assert self.agent.is_initialized
        
    def test_backward_compatibility(self):
        """Test backward compatibility with original API."""
        # Test that original API still works
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Original methods should still work
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        
        # New API should also work
        pythonic_brain = PythonicBrain(brain)
        assert pythonic_brain.total_neuron_count > 0

class TestIntegration:
    """Integration tests for NLM Python API."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = createOptimizedConfig()
        self.brain = PythonicBrain(pynlm.createBrain(self.config))
        self.brain.enable_all_systems()
        self.brain._brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=100, height=100, vision_width=32, vision_height=32)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain._brain)
        self.agent.initialize(self.world)
        
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow."""
        # Step 1: Create and initialize all components
        assert self.brain.total_neuron_count > 0
        assert self.world.width > 0
        assert self.agent.is_initialized
        
        # Step 2: Run complete simulation episode
        stats = self.agent.run_episode(self.brain._brain, self.world, steps=50, timestep=0.1)
        
        # Step 3: Verify results
        assert stats['agent_sensory_size'] > 0
        assert stats['agent_motor_size'] > 0
        assert 'firing_neurons' in stats
        
        # Step 4: Test session management
        with pynlm.createBrainSession(self.brain._brain, self.world, self.agent) as session:
            session.run_simulation(30, 0.1)
            session_stats = session.get_stats()
            assert 'neurons_firing' in session_stats
        
    def test_configuration_workflow(self):
        """Test complete configuration workflow."""
        # Step 1: Create optimized config
        config = createOptimizedConfig()
        
        # Step 2: Add custom settings
        config.set("research.custom_setting", "value")
        config.set("performance.custom_steps", 1000)
        
        # Step 3: Validate configuration
        errors, warnings = validateConfiguration(config)
        
        # Step 4: Export configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            pynlm.exportConfiguration(config, filepath)
            
            # Step 5: Import configuration
            imported_config = pynlm.importConfiguration(filepath)
            
            # Step 6: Verify imported config
            assert imported_config.has("performance.optimization_level")
            assert imported_config.getOr("performance.optimization_level", "") == "high"
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
        
    def test_advanced_agent_workflow(self):
        """Test advanced agent workflow."""
        # Step 1: Create agent with all systems enabled
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        self.agent.enable_development(True)
        
        # Step 2: Run multiple episodes
        for episode in range(3):
            stats = self.agent.run_episode(
                self.brain._brain, 
                self.world, 
                steps=20, 
                timestep=0.1,
                systems={
                    'reward_modulation': True,
                    'development': True,
                }
            )
            
            # Step 3: Verify episode results
            assert stats['agent_developer_stage'] == 0  # Initial stage
            assert stats['agent_neuromodulation'] >= 0.0
        
    def test_error_recovery_workflow(self):
        """Test error recovery workflow."""
        # Step 1: Create components
        config = createOptimizedConfig()
        brain = PythonicBrain(pynlm.createBrain(config))
        brain.enable_all_systems()
        brain._brain.initialize()
        
        # Step 2: Simulate error handling
        try:
            # This should work normally
            assert brain.total_neuron_count > 0
            
            # Test with invalid parameters (should be handled gracefully)
            config.set("invalid.parameter", "value")
            # Should not crash
            
        except Exception as e:
            # If there's an error, it should be handled
            print(f"Handled error gracefully: {e}")
        
        # Step 3: Continue with valid operations
        assert brain.total_neuron_count > 0

def run_tests():
    """Run all tests."""
    test_instance = TestEnhancedPythonAPI()
    integration_test_instance = TestIntegration()
    
    tests = [
        test_instance.test_pythonic_brain_properties,
        test_instance.test_pythonic_brain_summary,
        test_instance.test_brain_session_context_manager,
        test_instance.test_sensory_data,
        test_instance.test_configuration_validation,
        test_instance.test_configuration_export_import,
        test_instance.test_action_interface,
        test_instance.test_agent_interface,
        test_instance.test_session_run_episode,
        test_instance.test_optimized_config,
        test_instance.test_advanced_features,
        test_instance.test_all_enums_and_types,
        test_instance.test_error_handling,
        test_instance.test_simulation_consistency,
        test_instance.test_memory_efficiency,
        test_instance.test_thread_safety,
        test_instance.test_backward_compatibility,
        integration_test_instance.test_complete_simulation_workflow,
        integration_test_instance.test_configuration_workflow,
        integration_test_instance.test_advanced_agent_workflow,
        integration_test_instance.test_error_recovery_workflow,
    ]
    
    print("Running NLM Python Enhanced API Tests")
    print("=" * 50)
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            # Setup
            test_instance.setup_method()
            
            # Run test
            test()
            
            print(f"✓ {test.__name__}")
            passed += 1
            
        except Exception as e:
            print(f"✗ {test.__name__}: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print(f"\n{failed} tests failed. Please check the output above for details.")
        return False
    else:
        print("\nAll tests passed! Enhanced NLM Python API is working correctly.")
        return True

if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)