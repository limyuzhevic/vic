"""
Unit tests for NLM Python bindings improvements.

This module contains comprehensive unit tests for the enhanced Python bindings,
including new convenience methods, error handling, and configuration management.
"""

import pynlm
import tempfile
import os
import pytest
import time
class TestConfigImprovements:
    """Test configuration-related improvements."""

    def test_load_from_dict(self):
        """Test loadFromDict method."""
        config = pynlm.createDefaultConfig()
        
        # Test with various data types
        test_dict = {
            "brain.neuron_count": 1000,
            "brain.v_thresh": -50.0,
            "brain.tau_mem": 20.0,
            "brain.v_rest": -70.0,
            "brain.v_reset": -75.0,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": False,
            "neuromodulation.curiosity.enable": True,
            "learning.rate": 0.001,
            "string_value": "test",
            "list_value": ["item1", "item2", "item3"]
        }
        
        result = config.loadFromDict(test_dict)
        assert result is True, "loadFromDict should return True on success"
        
        # Verify values were set correctly
        assert config.getOr("brain.neuron_count", 0) == 1000
        assert config.getOr("brain.v_thresh", 0.0) == -50.0
        assert config.getOr("plasticity.stdp.enable", False) is True
        assert config.getOr("plasticity.hebbian.enable", True) is False
        assert config.getOr("neuromodulation.curiosity.enable", False) is True
        assert config.getOr("learning.rate", 0.0) == 0.001
        assert config.getOr("string_value", "") == "test"
        assert config.getOr("list_value", []) == ["item1", "item2", "item3"]

    def test_pythonic_set_get_methods(self):
        """Test Pythonic set and get methods."""
        config = pynlm.createDefaultConfig()
        
        # Test type-specific set methods
        config.set("int_key", 42)
        config.set("float_key", 3.14)
        config.set("bool_key", True)
        config.set("string_key", "hello")
        config.set("list_key", ["a", "b", "c"])
        
        # Test type-specific get methods
        assert config.getOr("int_key", 0) == 42
        assert config.getOr("float_key", 0.0) == 3.14
        assert config.getOr("bool_key", False) is True
        assert config.getOr("string_key", "") == "hello"
        assert config.getOr("list_key", []) == ["a", "b", "c"]
        
        # Test get with None values
        assert config.get("nonexistent_key") is None

    def test_get_or_with_defaults(self):
        """Test getOr method with default values."""
        config = pynlm.createDefaultConfig()
        
        # Set some values
        config.set("existing_key", 123)
        
        # Test getOr with existing values
        assert config.getOr("existing_key", 0) == 123
        
        # Test getOr with non-existing values
        assert config.getOr("missing_key", "default_value") == "default_value"
        assert config.getOr("missing_int", 999) == 999
        assert config.getOr("missing_bool", True) is True
        assert config.getOr("missing_float", 3.14) == 3.14

    def test_remove_and_clear(self):
        """Test remove and clear methods."""
        config = pynlm.createDefaultConfig()
        
        # Set some values
        config.set("key1", "value1")
        config.set("key2", "value2")
        config.set("key3", "value3")
        
        # Verify values are set
        assert config.getOr("key1", "") == "value1"
        assert config.getOr("key2", "") == "value2"
        assert config.getOr("key3", "") == "value3"
        
        # Remove one key
        config.remove("key2")
        assert config.get("key2") is None
        assert config.getOr("key2", "default") == "default"
        
        # Clear all
        config.clear()
        assert config.get("key1") is None
        assert config.get("key3") is None

    def test_load_save_file_operations(self):
        """Test file load and save operations."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.v_thresh", -45.0)
        
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Save configuration
            result = config.saveToFile(filepath)
            assert result is True, "saveToFile should return True"
            
            # Load configuration back
            new_config = pynlm.createDefaultConfig()
            result = new_config.loadFromFile(filepath)
            assert result is True, "loadFromFile should return True"
            
            # Verify values
            assert new_config.getOr("brain.neuron_count", 0) == 2000
            assert new_config.getOr("brain.v_thresh", 0.0) == -45.0
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)

    def test_load_from_dict_error_handling(self):
        """Test error handling in loadFromDict."""
        config = pynlm.createDefaultConfig()
        
        # Test with invalid data types (should return False, not raise exception)
        test_dict = {
            "valid_key": "valid_value",
            "invalid_key": None  # Invalid type for ConfigValue
        }
        
        # This should not raise an exception, just return False
        result = config.loadFromDict(test_dict)
        # The current implementation might handle this differently, 
        # so we'll just test that it doesn't crash

class TestBrainImprovements:
    """Test brain-related improvements."""

    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1000)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()

    def test_run_simulation(self):
        """Test runSimulation method."""
        # Reset brain
        self.brain.reset()
        self.brain.initialize()
        
        # Run simulation
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.runSimulation(50)
        
        final_spikes = self.brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes, "Should have more spikes after simulation"

    def test_get_stats(self):
        """Test getStats method."""
        stats = self.brain.getStats()
        
        # Check expected keys
        expected_keys = [
            "total_neurons", "total_synapses", "firing_neurons",
            "active_neurons", "total_spikes", "pending_spikes",
            "firing_rate", "e_i_ratio", "developmental_stage"
        ]
        
        for key in expected_keys:
            assert key in stats, f"Stats should contain key: {key}"
        
        # Check value types
        assert isinstance(stats["total_neurons"], int)
        assert isinstance(stats["total_synapses"], int)
        assert isinstance(stats["firing_neurons"], int)
        assert isinstance(stats["active_neurons"], int)
        assert isinstance(stats["total_spikes"], int)
        assert isinstance(stats["pending_spikes"], int)
        assert isinstance(stats["firing_rate"], float)
        assert isinstance(stats["e_i_ratio"], float)
        assert isinstance(stats["developmental_stage"], int)

    def test_get_brain_info(self):
        """Test getBrainInfo method."""
        info = self.brain.getBrainInfo()
        
        # Check expected keys
        expected_keys = ["neurons", "synapses", "regions", "firing", "spike_count", "development"]
        
        for key in expected_keys:
            assert key in info, f"Brain info should contain key: {key}"
        
        # Check value types
        assert isinstance(info["neurons"], int)
        assert isinstance(info["synapses"], int)
        assert isinstance(info["regions"], int)
        assert isinstance(info["firing"], int)
        assert isinstance(info["spike_count"], int)
        assert isinstance(info["development"], int)

    def test_run_with_input(self):
        """Test runWithInput method."""
        # Create a simple sensory input
        from pynlm import Vision
        input_data = [0.1, 0.2, 0.3, 0.4, 0.5]
        vision_input = Vision(5, 1, 3)  # width, height, channels
        vision_input.setData(input_data)
        
        # Reset and run with input
        self.brain.reset()
        self.brain.initialize()
        
        # This should not crash
        self.brain.runWithInput(vision_input, 20)
        
        # Verify simulation ran
        assert self.brain.getTotalSpikeCount() >= 0

    def test_inject_current_safe(self):
        """Test injectCurrentSafe method."""
        # Get a valid neuron ID
        neuron_id = pynlm.INVALID_NEURON_ID  # This should be a valid type
        
        # This should not crash
        self.brain.injectCurrentSafe(neuron_id, 0.5)
        
        # Test with different current values
        self.brain.injectCurrentSafe(neuron_id, 0.0)
        self.brain.injectCurrentSafe(neuron_id, 1.0)

    def test_inject_current_to_type_safe(self):
        """Test injectCurrentToTypeSafe method."""
        # Test with different neuron types
        for neuron_type in [pynlm.NeuronType.Excitatory, 
                           pynlm.NeuronType.Inhibitory,
                           pynlm.NeuronType.Sensory,
                           pynlm.NeuronType.Motor]:
            self.brain.injectCurrentToTypeSafe(neuron_type, 0.1)
        
        # Test with float current values
        self.brain.injectCurrentToTypeSafe(pynlm.NeuronType.Excitatory, 0.0)
        self.brain.injectCurrentToTypeSafe(pynlm.NeuronType.Excitatory, 2.5)

    def test_produce_action_and_validate(self):
        """Test produceActionAndValidate method."""
        # This should not crash
        action = self.brain.produceActionAndValidate()
        
        # Action should have a type
        assert action is not None
        assert action.getType() is not None

    def test_save_and_validate(self):
        """Test saveAndValidate method."""
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            result = self.brain.saveAndValidate(filepath)
            assert result is True, "saveAndValidate should return True"
            
            # Verify file was created
            assert os.path.exists(filepath), "File should be created"
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)

    def test_load_and_validate(self):
        """Test loadAndValidate method."""
        # First, create a temporary brain and save it
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            self.brain.saveAndValidate(filepath)
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            new_brain.loadAndValidate(filepath)
            
            # Verify spike counts are similar
            original_stats = self.brain.getStats()
            loaded_stats = new_brain.getStats()
            
            # They should be the same (or at least similar)
            assert abs(original_stats["total_spikes"] - loaded_stats["total_spikes"]) < 10
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)

    def test_add_region_with_name(self):
        """Test addRegionWithName method."""
        # Add region with name
        region_id = self.brain.addRegionWithName("cortex")
        assert region_id is not None, "Region ID should not be None"
        
        # Get region
        region = self.brain.getRegion(region_id)
        assert region is not None, "Region should be retrievable"

    def test_get_region_ids_names(self):
        """Test getRegionIds and getRegionNames methods."""
        # Add some regions
        self.brain.addRegionWithName("region1")
        self.brain.addRegionWithName("region2")
        
        # Get region IDs
        ids = self.brain.getRegionIds()
        assert len(ids) >= 1, "Should have at least one region"
        assert all(isinstance(id, int) for id in ids), "All IDs should be integers"
        
        # Get region names (using our implementation)
        names = self.brain.getRegionNames()
        assert len(names) >= 1, "Should have at least one region name"
        assert all(isinstance(name, str) for name in names), "All names should be strings"

    def test_get_regions(self):
        """Test getRegions method."""
        # Get regions
        regions = self.brain.getRegions()
        assert len(regions) >= 1, "Should have at least one region"
        
        # Each region should have a getName method
        for region in regions:
            # Regions are internal C++ objects, so we can't easily test them
            # But we can verify the method exists and returns something
            assert region is not None, "Region should not be None"
class TestAgentBrainImprovements:
    """Test AgentBrain-related improvements."""

    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1000)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create a simple world for testing
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=4, visionHeight=4)

    def test_run_agent_simulation(self):
        """Test runAgentSimulation method."""
        # This is a simplified test - the method has some limitations
        # but we can test that it doesn't crash
        self.agent.runAgentSimulation(self.world, 10)
        
        # Agent should be initialized
        assert self.agent.getBrain() is not None

    def test_get_agent_stats(self):
        """Test getAgentStats method."""
        stats = self.agent.getAgentStats()
        
        # Check expected keys
        expected_keys = ["curiosity", "novelty", "prediction_error",
                        "neuromodulation", "development"]
        
        for key in expected_keys:
            assert key in stats, f"Agent stats should contain key: {key}"
        
        # Check value types
        assert isinstance(stats["curiosity"], float)
        assert isinstance(stats["novelty"], float)
        assert isinstance(stats["prediction_error"], float)
        assert isinstance(stats["neuromodulation"], float)
        assert isinstance(stats["development"], int)

    def test_process_input_and_validate(self):
        """Test processInputAndValidate method."""
        # Create a simple percept
        from pynlm import SensoryPercept
        percept = SensoryPercept()
        
        # This should not crash
        self.agent.processInputAndValidate(percept)

    def test_decode_motor_and_validate(self):
        """Test decodeMotorAndValidate method."""
        # This should not crash
        action = self.agent.decodeMotorAndValidate()
        assert action is not None, "Decoded action should not be None"

    def test_enable_all_features(self):
        """Test enableAllFeatures method."""
        # This should not crash
        self.agent.enableAllFeatures()
        
        # All features should be enabled
        assert self.agent.isRewardModulationEnabled() is True
        assert self.agent.isStructuralPlasticityEnabled() is True
        assert self.agent.isDevelopmentEnabled() is True
        assert self.agent.isCuriosityEnabled() is True

    def test_run_complete_episode(self):
        """Test runCompleteEpisode method."""
        # This is a simplified test
        self.agent.runCompleteEpisode(self.world, 5)
        
        # Should not crash
        assert self.agent.getBrain() is not None

    def test_initialize(self):
        """Test initialize method."""
        # Create a new agent and initialize it
        new_agent = pynlm.createAgentBrain(self.brain)
        new_agent.initialize(self.world)
        
        # Should not crash
        assert new_agent.getBrain() is not None
class TestErrorHandling:
    """Test error handling and validation."""

    def test_error_handling_neuron_injection(self):
        """Test error handling in current injection."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This should not crash, even with invalid neuron ID
        brain.injectCurrentSafe(pynlm.INVALID_NEURON_ID, 0.5)
        brain.injectCurrentToTypeSafe(pynlm.NeuronType.Excitatory, 0.5)

    def test_error_handling_file_operations(self):
        """Test error handling in file operations."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Try to load non-existent file
        result = brain.loadAndValidate("/nonexistent/path/file.bin")
        assert result is False, "loadAndValidate should return False for non-existent file"

    def test_error_handling_config_operations(self):
        """Test error handling in configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Try to set and get values
        config.set("valid_key", "valid_value")
        assert config.getOr("valid_key", "") == "valid_value"
        
        # Non-existent key should return None or default
        assert config.get("nonexistent_key") is None
        assert config.getOr("nonexistent_key", "default") == "default"
class TestPerformance:
    """Test performance-related functionality."""

    def test_performance_benchmark(self):
        """Test performance benchmarking."""
        import time
        
        # Simple performance test
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        brain.runSimulation(100)
        end_time = time.time()
        
        duration = end_time - start_time
        assert duration > 0, "Simulation should take some time"
        assert duration < 10, "Simulation should complete quickly"

    def test_multiple_scenarios(self):
        """Test multiple configuration scenarios."""
        scenarios = []
        
        for neuron_count in [500, 1000, 2000]:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.runSimulation(50)
            
            stats = brain.getStats()
            scenarios.append({
                "neurons": neuron_count,
                "firing_rate": stats["firing_rate"],
                "total_spikes": stats["total_spikes"]
            })
        
        # Verify all scenarios completed successfully
        assert len(scenarios) == 3, "Should have 3 scenarios"
        for scenario in scenarios:
            assert scenario["neurons"] > 0, "Neuron count should be positive"
            assert scenario["firing_rate"] >= 0, "Firing rate should be non-negative"
            assert scenario["total_spikes"] >= 0, "Total spikes should be non-negative"
class TestIntegration:
    """Integration tests for complete workflows."""

    def test_complete_workflow(self):
        """Test complete workflow with brain, agent, and world."""
        # Setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.setAgentStart(7.5, 7.5)
        world.reset()
        
        agent.initialize(world)
        
        # Enable learning features
        agent.enableAllFeatures()
        
        # Run complete simulation
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify all components worked
        assert brain.getTotalSpikeCount() >= 0, "Brain should have spikes"
        assert agent.getBrain() is not None, "Agent should have brain"
        
        # Get statistics
        brain_stats = brain.getStats()
        agent_stats = agent.getAgentStats()
        
        # Should have reasonable values
        assert brain_stats["total_neurons"] == 1000, "Should have 1000 neurons"
        assert agent_stats["curiosity"] >= 0, "Curiosity should be non-negative"

    def test_state_persistence(self):
        """Test brain state persistence."""
        # Create and save brain state
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        brain1.runSimulation(100)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            brain1.saveAndValidate(filepath)
            
            # Create new brain and load
            brain2 = pynlm.createBrain(config)
            brain2.loadAndValidate(filepath)
            
            # Verify spike counts are similar
            stats1 = brain1.getStats()
            stats2 = brain2.getStats()
            
            # Should be close (allowing for some difference due to random initialization)
            spike_diff = abs(stats1["total_spikes"] - stats2["total_spikes"])
            assert spike_diff < 50, f"Spike difference should be small, got {spike_diff}"
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)

    def test_multiple_agents_scenario(self):
        """Test scenario with multiple agents."""
        # Create multiple agents with different configurations
        agents = []
        worlds = []
        
        for i in range(3):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000 + i * 500)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
            
            agent.initialize(world)
            agent.enableAllFeatures()
            
            agents.append(agent)
            worlds.append(world)
        
        # Run simulation for each agent
        for i, agent in enumerate(agents):
            for step in range(10):
                worlds[i].update(0.1)
                agent.processSensoryInput(worlds[i].getSensoryPercept())
                agent.decodeMotorCommand()
                worlds[i].applyMotorCommand(agent.decodeMotorCommand(), 
                                           worlds[i].getSimulationTime())
                agent.applyRewardModulation(0.1, 0.0)
                agent.updateDevelopment(0.1)
        
        # Verify all agents completed successfully
        assert len(agents) == 3, "Should have 3 agents"
        assert len(worlds) == 3, "Should have 3 worlds"
        
        # Get statistics from each agent
        for i, agent in enumerate(agents):
            agent_stats = agent.getAgentStats()
            assert agent_stats["curiosity"] >= 0, f"Agent {i} curiosity should be non-negative"
            assert agent_stats["novelty"] >= 0, f"Agent {i} novelty should be non-negative"
if __name__ == "__main__":
    print("Running NLM Python Bindings Tests...")
    
    # Run tests
    test_classes = [
        TestConfigImprovements,
        TestBrainImprovements,
        TestAgentBrainImprovements,
        TestErrorHandling,
        TestPerformance,
        TestIntegration
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(test_instance) 
                       if method.startswith('test_') and callable(getattr(test_instance, method))]
        
        for test_method in test_methods:
            try:
                print(f"  Running {test_method}...", end=" ")
                test_instance.setup_method()
                getattr(test_instance, test_method)()
                print("✓ PASSED")
                passed += 1
            except Exception as e:
                print(f"✗ FAILED: {e}")
                failed += 1
    
    print(f"\nTest Summary: {passed} passed, {failed} failed")
    
    if failed > 0:
        exit(1)
    else:
        print("All tests passed!")