#!/usr/bin/env python3
"""
Comprehensive integration tests for NLM Python bindings and agent systems.
Tests all new API methods added for config, brain, agent brain, and world integration.
"""

import pytest
import sys
import os
import json
from unittest.mock import Mock, patch

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: pynlm module not available: {e}")
    PYNLM_AVAILABLE = False

# Test data constants
TEST_CONFIG_PATH = "test_config.ini"
DEFAULT_BRAIN_CONFIG = {"neuron_count": 10, "region_count": 2, "connection_probability": 0.1}

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestConfigIntegration:
    """Integration tests for new Config methods."""
    
    def test_getConfigString(self):
        """Test Config.getConfigString() method."""
        config = nlm.Config()
        config.set("test_key", "test_value")
        config.set("test_int", 42)
        config.set("test_double", 3.14)
        config.set("test_bool", True)
        
        config_str = config.getConfigString()
        assert isinstance(config_str, str)
        assert "test_key" in config_str
        assert "test_value" in config_str
        
        # Verify JSON-like structure
        assert config_str.startswith("{")
        assert config_str.endswith("}\n")
        
    def test_validateConfig_valid(self):
        """Test Config.validateConfig() with valid configuration."""
        config = nlm.Config()
        config.set("string_key", "valid_value")
        config.set("int_key", 100)
        config.set("double_key", 50.0)
        config.set("bool_key", True)
        
        # Valid config should return True
        assert config.validateConfig() == True
        
    def test_validateConfig_invalid(self):
        """Test Config.validateConfig() with invalid configuration."""
        config = nlm.Config()
        config.set("empty_string", "")
        config.set("invalid_double", float('nan'))
        
        # Invalid config should return False
        assert config.validateConfig() == False
        
    def test_mergeConfig(self):
        """Test Config.mergeConfig() method."""
        config1 = nlm.Config()
        config1.set("key1", "value1")
        config1.set("key2", "value2")
        
        config2 = nlm.Config()
        config2.set("key2", "new_value2")  # Override
        config2.set("key3", "value3")  # New
        
        config1.mergeConfig(config2)
        
        # key1 should still exist
        assert config1.has("key1") == True
        
        # key2 should be overridden
        assert config1.has("key2") == True
        # Note: Can't easily test value without exposing get method
        
        # key3 should be added
        assert config1.has("key3") == True
        
    def test_error_handling_invalid_key(self):
        """Test error handling for invalid configuration scenarios."""
        config = nlm.Config()
        
        # Test that has() returns False for non-existent keys
        assert config.has("nonexistent") == False
        
        # Test setting various types
        config.set("int_val", 42)
        config.set("double_val", 3.14)
        config.set("bool_val", False)
        config.set("string_val", "hello")
        
        # Empty string validation
        config.set("empty_str", "")
        assert config.validateConfig() == False
        
    def test_backward_compatibility(self):
        """Test backward compatibility with existing API."""
        config = nlm.Config()
        
        # Test existing methods still work
        config.loadFromFile(TEST_CONFIG_PATH)  # Should work even if file doesn't exist
        config.has("some_key")
        config.getKeys()
        config.clear()
        config.summary()
        
        # Test new methods added
        config.getConfigString()
        config.validateConfig()
        config.mergeConfig(config)

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestBrainIntegration:
    """Integration tests for new Brain methods."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = nlm.Config()
        
        # Set up minimal config for brain creation
        self.config.set("neuron_count", 10)
        self.config.set("region_count", 1)
        self.config.set("connection_probability", 0.1)
        
        # Create brain using the factory function
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_getAllNeuronIds(self):
        """Test Brain.getAllNeuronIds() method."""
        neuron_ids = self.brain.getAllNeuronIds()
        
        assert isinstance(neuron_ids, list)
        # Should have some neurons based on config
        assert len(neuron_ids) > 0
        
        # Each neuron ID should have a value attribute
        for neuron_id in neuron_ids:
            assert hasattr(neuron_id, 'value')
            assert neuron_id.value >= 0
            
    def test_getConnectionMatrix(self):
        """Test Brain.getConnectionMatrix() method."""
        connection_matrix = self.brain.getConnectionMatrix()
        
        assert isinstance(connection_matrix, dict)
        # Matrix should map pairs of neuron IDs to weights
        for (source, target), weight in connection_matrix.items():
            assert isinstance(source, nlm.NeuronId)
            assert isinstance(target, nlm.NeuronId)
            assert isinstance(weight, float)
            assert -1.0 <= weight <= 1.0  # Reasonable weight range
            
    def test_setExternalInput(self):
        """Test Brain.setExternalInput() method."""
        # Get all neuron IDs to test with
        neuron_ids = self.brain.getAllNeuronIds()
        if neuron_ids:
            test_neuron = neuron_ids[0]
            
            # Test setting external input
            self.brain.setExternalInput(test_neuron, 5.0, 1.0)
            
            # Verify no exception was raised
            assert True
            
    def test_neural_activity_tracing(self):
        """Test neural activity tracing and statistics collection."""
        # Get neural trace
        neural_trace = self.brain.getNeuralTrace()
        assert isinstance(neural_trace, list)
        
        for trace_point in neural_trace:
            timestamp, activity = trace_point
            assert isinstance(timestamp, float)
            assert isinstance(activity, float)
            
    def test_external_signal_injection_timing(self):
        """Test external signal injection with timing."""
        neuron_ids = self.brain.getAllNeuronIds()
        if neuron_ids:
            test_neuron = neuron_ids[0]
            
            # Test injection at different times
            times = [0.0, 1.0, 2.0, 5.0]
            currents = [1.0, 2.0, 0.5, 3.0]
            
            for time_val, current_val in zip(times, currents):
                self.brain.setExternalInput(test_neuron, current_val, time_val)
                
    def test_connection_matrix_small_network(self):
        """Test connection matrix generation for small networks."""
        # Test with a very small network
        config = nlm.Config()
        config.set("neuron_count", 5)
        config.set("region_count", 1)
        config.set("connection_probability", 0.5)  # High probability for small network
        
        small_brain = nlm.createBrain(config)
        small_brain.initialize()
        
        matrix = small_brain.getConnectionMatrix()
        
        # Even with small network, should have some connections
        # (could be 0 if no connections were made)
        assert isinstance(matrix, dict)
        
    def test_error_handling_invalid_neuron(self):
        """Test error handling for invalid neuron IDs."""
        # Create an invalid neuron ID
        invalid_neuron = nlm.NeuronId()
        invalid_neuron.value = -1  # Invalid value
        
        # Should handle invalid neuron gracefully
        # Note: actual behavior depends on implementation
        try:
            self.brain.setExternalInput(invalid_neuron, 1.0, 0.0)
            # No exception expected - should log warning
        except Exception:
            pass  # Exception handling is acceptable

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestAgentBrainIntegration:
    """Integration tests for new AgentBrain methods."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = nlm.Config()
        self.config.set("neuron_count", 20)
        self.config.set("region_count", 2)
        self.config.set("connection_probability", 0.2)
        
        # Create components
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = nlm.createSimpleWorld()
        self.world.configure(100.0, 100.0, 16, 16)
        self.world.reset()
        
        # Create agent brain
        self.agent_brain = nlm.createAgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
    
    def test_setExplorationMode(self):
        """Test AgentBrain.setExplorationMode() method."""
        exploration_modes = ["random", "epsilon_greedy", "ucb", "thompson"]
        
        for mode in exploration_modes:
            # Should not raise exception
            self.agent_brain.setExplorationMode(mode)
            assert True
            
        # Test invalid mode (should handle gracefully)
        self.agent_brain.setExplorationMode("invalid_mode")
        
    def test_getActionSelectionStats(self):
        """Test AgentBrain.getActionSelectionStats() method."""
        # Get initial stats
        stats = self.agent_brain.getActionSelectionStats()
        assert isinstance(stats, list)
        
        # Stats should be pairs of (action, count)
        for action, count in stats:
            assert isinstance(action, nlm.MotorCommand)
            assert isinstance(count, int)
            assert count >= 0
            
    def test_curiosity_strategy_settings(self):
        """Test curiosity strategy settings."""
        strategies = ["noveltys_based", "pred_error_based", "combined", "intrinsic"]
        
        for strategy in strategies:
            self.agent_brain.setCuriosityStrategy(strategy)
            assert True
            
        # Test invalid strategy
        self.agent_brain.setCuriosityStrategy("invalid_strategy")
        
    def test_behavior_history_tracking(self):
        """Test behavior history tracking."""
        # Get initial behavior history
        history = self.agent_brain.getBehaviorHistory()
        assert isinstance(history, list)
        
        for action in history:
            assert isinstance(action, nlm.MotorCommand)
            
    def test_learning_mode_controls(self):
        """Test learning mode controls."""
        # Test enabling learning
        self.agent_brain.enableLearningMode(True)
        assert True
        
        # Test disabling learning
        self.agent_brain.enableLearningMode(False)
        assert True
        
    def test_integration_with_config(self):
        """Test AgentBrain integration with configuration system."""
        # Test that exploration mode affects curiosity calculations
        self.agent_brain.setExplorationMode("random")
        self.agent_brain.setCuriosityStrategy("noveltys_based")
        
        # Get stats to verify integration
        stats = self.agent_brain.getActionSelectionStats()
        assert isinstance(stats, list)
        
        # Test learning mode integration
        self.agent_brain.enableLearningMode(True)
        assert True
        
        self.agent_brain.enableLearningMode(False)
        assert True

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestSimpleWorldIntegration:
    """Integration tests for new SimpleWorld methods."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.world = nlm.createSimpleWorld()
        self.world.configure(100.0, 100.0, 16, 16)
        self.world.reset()
    
    def test_getPhysicsParameters(self):
        """Test SimpleWorld.getPhysicsParameters() method."""
        physics_params = self.world.getPhysicsParameters()
        
        assert isinstance(physics_params, dict)
        
        # Should contain standard physics parameters
        expected_keys = ["gravity", "friction", "elasticity", "damping", "maxSpeed", "acceleration"]
        for key in expected_keys:
            assert key in physics_params
            assert isinstance(physics_params[key], float)
            
    def test_setPhysicsParameters(self):
        """Test SimpleWorld.setPhysicsParameters() method."""
        new_params = {
            "gravity": 9.8,
            "friction": 0.5,
            "elasticity": 0.8,
            "damping": 0.95,
            "maxSpeed": 10.0,
            "acceleration": 3.0
        }
        
        # Should not raise exception
        self.world.setPhysicsParameters(new_params)
        assert True
        
    def test_object_filtering_by_type(self):
        """Test object filtering by type."""
        # Get all objects of different types
        all_objects = []
        for obj_type in [
            nlm.WorldObjectType.Empty,
            nlm.WorldObjectType.Resource,
            nlm.WorldObjectType.Hazard,
            nlm.WorldObjectType.Wall,
            nlm.WorldObjectType.Marker
        ]:
            filtered = self.world.getObjectsByType(obj_type)
            assert isinstance(filtered, list)
            
            # Each filtered list should contain objects of the specified type
            for obj in filtered:
                assert obj.type == obj_type
                
    def test_environmental_statistics(self):
        """Test environmental statistics."""
        stats = self.world.getEnvironmentStats()
        
        assert isinstance(stats, dict)
        
        # Should contain basic environment statistics
        expected_stats = ["width", "height", "agent_x", "agent_y", "agent_energy", 
                         "agent_health", "simulation_time", "object_count", "density"]
        for stat in expected_stats:
            assert stat in stats
            assert isinstance(stats[stat], float)
            
    def test_weather_simulation_integration(self):
        """Test weather simulation integration."""
        # Enable weather simulation
        self.world.enableWeatherSimulation(True)
        
        # Set weather parameters
        self.world.setWeatherParameters(5.0, 0.3)  # windSpeed, precipitation
        
        # Get weather values
        assert self.world.getWindSpeed() == 5.0
        assert self.world.getPrecipitation() == 0.3
        
        # Test disabling weather
        self.world.enableWeatherSimulation(False)
        
    def test_integration_with_agent(self):
        """Test SimpleWorld integration with agent system."""
        # Get agent body state
        agent_body = self.world.getAgentBody()
        assert agent_body.x >= 0.0
        assert agent_body.y >= 0.0
        assert agent_body.energy >= 0.0
        assert 0.0 <= agent_body.orientation <= 2 * 3.14159  # Normalized orientation
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        # Note: specific sensory structure depends on implementation

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestSystemIntegration:
    """System integration tests."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = nlm.Config()
        self.config.set("neuron_count", 50)
        self.config.set("region_count", 2)
        self.config.set("connection_probability", 0.15)
        
        # Create all system components
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = nlm.createSimpleWorld()
        self.world.configure(200.0, 200.0, 32, 32)
        self.world.reset()
        
        self.agent_brain = nlm.createAgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
    
    def test_full_agent_world_integration(self):
        """Test full agent-world integration with new methods."""
        # Use new AgentBrain methods
        self.agent_brain.setExplorationMode("epsilon_greedy")
        self.agent_brain.setCuriosityStrategy("combined")
        self.agent_brain.enableLearningMode(True)
        
        # Process some sensory input
        percept = self.world.getSensoryPercept()
        self.agent_brain.processSensoryInput(percept)
        
        # Get action selection stats
        stats = self.agent_brain.getActionSelectionStats()
        assert isinstance(stats, list)
        
        # Decode motor command
        action = self.agent_brain.decodeMotorCommand()
        assert isinstance(action, nlm.MotorCommand)
        
    def test_neuromodulation_with_exploration_modes(self):
        """Test neuromodulation with new exploration modes."""
        # Test different exploration modes
        for mode in ["random", "epsilon_greedy", "ucb"]:
            self.agent_brain.setExplorationMode(mode)
            
            # Process input to trigger neuromodulation
            percept = self.world.getSensoryPercept()
            self.agent_brain.processSensoryInput(percept)
            
            # Apply reward modulation
            self.agent_brain.applyRewardModulation(1.0, 0.5)
            
        # Check neuromodulation levels
        neuromodulation_level = self.agent_brain.getNeuromodulationLevel()
        curiosity_level = self.agent_brain.getCuriosityLevel()
        
        assert isinstance(neuromodulation_level, float)
        assert isinstance(curiosity_level, float)
        
    def test_development_stages_with_learning_controls(self):
        """Test development stages with new learning controls."""
        # Test learning mode controls during development
        self.agent_brain.enableLearningMode(True)
        
        # Update development
        for timestep in [0.0, 10.0, 50.0, 100.0]:
            self.agent_brain.updateDevelopment(timestep)
            
            # Check developmental stage
            stage = self.agent_brain.getDevelopmentalStage()
            assert isinstance(stage, nlm.DevelopmentalStage)
            
        self.agent_brain.enableLearningMode(False)
        
    def test_end_to_end_simulation(self):
        """Test end-to-end simulation scenarios."""
        # Run a simple simulation loop
        simulation_steps = 20
        
        for step in range(simulation_steps):
            # Update world
            timestep = 0.1
            self.world.update(timestep)
            
            # Get current percept
            percept = self.world.getSensoryPercept()
            
            # Process with agent brain
            self.agent_brain.processSensoryInput(percept)
            
            # Decode action
            action = self.agent_brain.decodeMotorCommand()
            
            # Apply action to world
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = action_result.reward
            self.agent_brain.applyRewardModulation(reward, reward * 0.8)
            
            # Update development
            self.agent_brain.updateDevelopment(timestep)
            
        # Verify final state
        assert self.world.getAgentBody().energy >= 0.0
        assert self.world.getAgentBody().health >= 0.0
        
        # Verify stats are reasonable
        final_stats = self.agent_brain.getActionSelectionStats()
        assert isinstance(final_stats, list)
        
    def test_backward_compatibility(self):
        """Verify backward compatibility with existing API."""
        # Test that existing API still works alongside new methods
        
        # Config compatibility
        config = nlm.Config()
        config.set("test", "value")
        config.has("test")
        config.getKeys()
        config.summary()
        
        # Brain compatibility
        brain = nlm.createBrain(self.config)
        brain.initialize()
        brain.getTotalNeuronCount()
        brain.getTotalSynapseCount()
        brain.getActiveNeuronCount()
        
        # World compatibility
        world = nlm.createSimpleWorld()
        world.configure(100.0, 100.0, 16, 16)
        world.reset()
        
        # AgentBrain compatibility
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        agent_brain.getSensoryInputSize()
        agent_brain.getMotorOutputSize()
        agent_brain.getNeuromodulationLevel()
        
        # All new methods should also be available
        brain.getAllNeuronIds()
        brain.getConnectionMatrix()
        agent_brain.setExplorationMode("random")
        agent_brain.getActionSelectionStats()
        world.getPhysicsParameters()
        world.getObjectsByType(nlm.WorldObjectType.Resource)

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
def test_import_availability():
    """Test that all expected classes are available from pynlm."""
    expected_classes = [
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage',
        'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType'
    ]
    
    for class_name in expected_classes:
        assert hasattr(nlm, class_name), f"Class {class_name} not available"
        
    # Test factory functions
    assert hasattr(nlm, 'createDefaultConfig')
    assert hasattr(nlm, 'createBrain')
    assert hasattr(nlm, 'createSimpleWorld')
    assert hasattr(nlm, 'createAgentBrain')
    
    # Test constants
    assert hasattr(nlm, 'INVALID_NEURON_ID')
    assert hasattr(nlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(nlm, 'INVALID_REGION_ID')
    assert hasattr(nlm, 'INVALID_POPULATION_ID')

if __name__ == "__main__":
    print("Running NLM Python Bindings Integration Tests")
    print("=" * 50)
    
    # Run tests
    if not PYNLM_AVAILABLE:
        print("ERROR: pynlm module not available. Skipping tests.")
        sys.exit(1)
        
    # Use pytest to run tests
    pytest.main([__file__, "-v", "--tb=short"])
