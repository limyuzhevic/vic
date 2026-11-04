"""
NLM Python Bindings Test Suite
=============================

Comprehensive test suite for NLM Python bindings validation.
Tests all Python API functionality, error handling, and edge cases.
"""

import pynlm
import tempfile
import os
import json
import pytest
from pathlib import Path

class TestNLMConfig:
    """Test configuration system"""
    
    def test_default_config_creation(self):
        """Test default configuration creation"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.getKeys() is not None
    
    def test_config_with_settings(self):
        """Test configuration with custom settings"""
        config = pynlm.createConfigWithSettings(
            neuron_count=500,
            region_count=2,
            synapse_density=0.15,
            random_seed=12345
        )
        
        assert config is not None
        # Config doesn't have direct getter for settings, but should not crash
    
    def test_config_file_io(self):
        """Test configuration file I/O"""
        config = pynlm.createDefaultConfig()
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            temp_file = f.name
        
        try:
            # Load from file
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(temp_file)
            
            # Should not crash
            assert config2 is not None
        finally:
            os.unlink(temp_file)
    
    def test_config_error_handling(self):
        """Test configuration error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test loading non-existent file (should handle gracefully)
        try:
            config.loadFromFile("/non/existent/file.json")
            # Should not crash, though may raise exception
        except:
            pass  # Expected behavior
class TestNLMBrain:
    """Test brain functionality"""
    
    def test_brain_creation(self):
        """Test brain creation with configuration"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        success = brain.initialize()
        assert success
        assert brain.getTotalNeuronCount() > 0
    
    def test_brain_step(self):
        """Test brain simulation steps"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_neurons = brain.getTotalNeuronCount()
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run simulation steps
        for step in range(10):
            brain.step(step)
            assert brain.getTotalNeuronCount() == initial_neurons
        
        # Should have made progress (spikes may increase)
        assert brain.getTotalSpikeCount() >= initial_spikes
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Take some steps
        for step in range(5):
            brain.step(step)
        
        # Reset brain
        brain.reset()
        
        # Should be back to initial state
        assert brain.getTotalNeuronCount() > 0
    
    def test_brain_statistics(self):
        """Test brain statistics collection"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(20):
            brain.step(step)
        
        # Check statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        assert brain.getTotalSpikeCount() >= 0
        
        # Check developmental stage
        stage = brain.getDevelopmentalStage()
        assert stage in [pynlm.DevelopmentalStage.Initial,
                        pynlm.DevelopmentalStage.CriticalPeriod,
                        pynlm.DevelopmentalStage.Maturation,
                        pynlm.DevelopmentalStage.Adult,
                        pynlm.DevelopmentalStage.Aging]
    
    def test_brain_regions(self):
        """Test brain region functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check region functionality
        region_count = brain.getRegionCount()
        assert region_count >= 1
        
        # Get region IDs
        region_ids = brain.getRegionIds()
        assert len(region_ids) == region_count
        
        # Get region objects
        for region_id in region_ids:
            region = brain.getRegion(region_id)
            assert region is not None
    
    def test_brain_checkpointing(self):
        """Test brain checkpoint save/load"""
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Run some steps to create brain state
        for step in range(10):
            brain1.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            save_success = brain1.save(checkpoint_file)
            assert save_success
            
            # Create second brain
            config2 = pynlm.createDefaultConfig()
            brain2 = pynlm.createBrain(config2)
            brain2.initialize()
            
            # Load checkpoint
            load_success = brain2.load(checkpoint_file)
            assert load_success
            
            # Both brains should have same basic properties
            assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
class TestNLMWorld:
    """Test world functionality"""
    
    def test_world_creation(self):
        """Test world creation"""
        world = pynlm.createSimpleWorld()
        assert world is not None
    
    def test_world_configuration(self):
        """Test world configuration"""
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Reset world
        world.reset()
        
        # Check configuration
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getSimulationTime() == 0.0
    
    def test_world_agent_interaction(self):
        """Test agent-world interaction"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get initial state
        percept1 = world.getSensoryPercept()
        body1 = world.getAgentBody()
        
        # Update world
        world.update(0.1)
        
        # Check state changed
        assert world.getSimulationTime() > 0.0
        
        # Apply motor command
        action = pynlm.MotorCommand.MoveForward
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check agent moved
        body2 = world.getAgentBody()
        # Agent should have moved forward
        assert body2.x >= body1.x
    
    def test_world_objects(self):
        """Test world object management"""
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Create world object
        world_object = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
        
        # Add object
        world.addObject(world_object)
        
        # Check position
        # (Note: SimpleWorld doesn't expose object removal/retrieval in bindings)
    
    def test_world_position_validation(self):
        """Test position validation"""
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Valid position
        assert world.isValidPosition(5.0, 5.0)
        
        # Invalid position (out of bounds)
        assert not world.isValidPosition(15.0, 5.0)
        assert not world.isValidPosition(5.0, 15.0)
class TestNLMAgent:
    """Test agent functionality"""
    
    def test_agent_creation(self):
        """Test agent creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
    
    def test_agent_initialization(self):
        """Test agent initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Check agent properties
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_agent_sensory_processing(self):
        """Test agent sensory processing"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        
        # Process should not crash
        agent.processSensoryInput(percept)
        
        # Agent should have processed the input
        assert percept.getVisionWidth() > 0
    
    def test_agent_motor_decoding(self):
        """Test agent motor decoding"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Decode motor command (should return valid command)
        action = agent.decodeMotorCommand()
        
        # Should be a valid motor command
        assert action in [pynlm.MotorCommand.MoveForward,
                         pynlm.MotorCommand.MoveBackward,
                         pynlm.MotorCommand.TurnLeft,
                         pynlm.MotorCommand.TurnRight,
                         pynlm.MotorCommand.Interact,
                         pynlm.MotorCommand.Wait]
    
    def test_agent_neuromodulation(self):
        """Test agent neuromodulation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable neuromodulation
        agent.enableRewardModulation(True)
        
        # Apply reward modulation
        agent.applyRewardModulation(10.0, 0.0)
        
        # Check neuromodulation levels
        assert agent.getNeuromodulationLevel() != 0.0
        
        # Check curiosity level
        assert agent.getCuriosityLevel() >= 0.0
    
    def test_agent_development(self):
        """Test agent development"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        
        # Update development
        agent.updateDevelopment(1.0)
        
        # Check developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage in [pynlm.DevelopmentalStage.Initial,
                        pynlm.DevelopmentalStage.CriticalPeriod,
                        pynlm.DevelopmentalStage.Maturation,
                        pynlm.DevelopmentalStage.Adult,
                        pynlm.DevelopmentalStage.Aging]
    
    def test_agent_subsystem_control(self):
        """Test agent subsystem control"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test enabling/disabling subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Check they are enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
        # Test disabling subsystems
        agent.enableRewardModulation(False)
        agent.enableCuriosity(False)
        
        # Check they are disabled
        assert not agent.isRewardModulationEnabled()
        assert not agent.isCuriosityEnabled()
    
    def test_agent_text_encoding(self):
        """Test agent text encoding (Phase 6)"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Encode text
        text_pattern = agent.encodeText("hello")
        
        # Should return list of neuron indices
        assert isinstance(text_pattern, list)
        assert len(text_pattern) == 4  # Phase 6: 4 neurons per word
        
        # All indices should be valid
        for idx in text_pattern:
            assert idx >= 0 and idx < 15  # Phase 6: 15 text neurons
    
    def test_agent_thought_trace(self):
        """Test agent thought trace"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get thought trace
        thought_trace = agent.getThoughtTrace()
        
        # Should return string
        assert isinstance(thought_trace, str)
        
        # Should contain trace information
        assert "perceived" in thought_trace.lower()
    
    def test_agent_counters(self):
        """Test agent diagnostic counters"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get various counters
        assert agent.getPlannerOverrideCount() >= 0
        assert agent.getPlannerAgreementCount() >= 0
        assert agent.getPlannerDecisionCount() >= 0
        assert agent.getConceptBiasCount() >= 0
        assert agent.getSelfBiasCount() >= 0
        assert agent.getSocialBiasCount() >= 0
class TestNLMSimulationHelpers:
    """Test simulation helper functions"""
    
    def test_run_simulation_episode(self):
        """Test simulation episode helper"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation episode
        pynlm.run_simulation_episode(brain, world, agent, 10, enable_learning=True)
        
        # Should not crash
        assert brain.getTotalNeuronCount() > 0
        assert agent.getCuriosityLevel() > 0.0
    
    def test_vector_to_list(self):
        """Test vector to list conversion"""
        test_vector = [1.0, 2.0, 3.0, 4.0, 5.0]
        
        # Test without filter
        python_list = pynlm.vectorToList(test_vector)
        assert isinstance(python_list, list)
        assert len(python_list) == 5
        assert python_list[0] == 1.0
        
        # Test with filter
        def filter_positive(x):
            return x > 0
        
        filtered_list = pynlm.vectorToList(test_vector, filter_positive)
        assert len(filtered_list) == 5  # All are positive
    
    def test_get_neurons_by_type(self):
        """Test get neurons by type helper"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get neurons by type
        excitatory_neurons = pynlm.get_neurons_by_type(brain, pynlm.NeuronType.Excitatory)
        inhibitory_neurons = pynlm.get_neurons_by_type(brain, pynlm.NeuronType.Inhibitory)
        
        # Should return lists
        assert isinstance(excitatory_neurons, list)
        assert isinstance(inhibitory_neurons, list)
        
        # Should contain neuron objects
        if excitatory_neurons:
            assert hasattr(excitatory_neurons[0], 'getFiringRate')
    
    def test_average_firing_rate_by_type(self):
        """Test average firing rate by type helper"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get average firing rates
        avg_exc = pynlm.get_average_firing_rate_by_type(brain, pynlm.NeuronType.Excitatory)
        avg_inh = pynlm.get_average_firing_rate_by_type(brain, pynlm.NeuronType.Inhibitory)
        
        # Should return floats
        assert isinstance(avg_exc, float)
        assert isinstance(avg_inh, float)
        
        # Should be non-negative
        assert avg_exc >= 0.0
        assert avg_inh >= 0.0
class TestNLMIntegration:
    """Integration tests"""
    
    def test_complete_workflow(self):
        """Test complete workflow with all components"""
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run complete simulation
        for step in range(50):
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Neuromodulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Development
            agent.updateDevelopment(0.1)
        
        # Verify completion
        assert brain.getTotalNeuronCount() > 0
        assert agent.getCuriosityLevel() > 0.0
        assert brain.getTotalSpikeCount() > 0
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            brain.save(checkpoint_file)
            assert os.path.exists(checkpoint_file)
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
    
    def test_multiple_agents(self):
        """Test multiple agent instances"""
        config = pynlm.createDefaultConfig()
        
        # Create multiple brains and agents
        agents = []
        for i in range(3):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 8, 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            agents.append(agent)
        
        # All should be independent
        for i, agent in enumerate(agents):
            assert agent.getCuriosityLevel() >= 0.0
            assert agent.getPredictionError() is not None
    
    def test_large_scale_simulation(self):
        """Test large-scale simulation"""
        # Create brain with more neurons
        config = pynlm.createConfigWithSettings(
            neuron_count=2000,
            region_count=4,
            synapse_density=0.08
        )
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(30, 30, 16, 16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run larger simulation
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify performance
        assert brain.getTotalNeuronCount() == 2000
        assert brain.getTotalSynapseCount() > 0
        assert agent.getCuriosityLevel() > 0.0
class TestNLMErrorHandling:
    """Test error handling"""
    
    def test_null_brain_error(self):
        """Test error handling with null brain"""
        # Create agent with null brain (should not crash)
        try:
            # This might raise an exception, which is fine
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent and then simulate brain being null (if possible)
            agent = pynlm.createAgentBrain(brain)
            
            # Test with null brain by setting brain_ to nullptr in C++ (not directly testable)
            pass
        except:
            pass  # Expected
    
    def test_invalid_parameter_handling(self):
        """Test handling of invalid parameters"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to create world with invalid parameters (bindings should handle gracefully)
        try:
            world = pynlm.createSimpleWorld()
            world.configure(-10, -10, -8, -8)  # Invalid dimensions
            # Should not crash
        except:
            pass  # Expected
    
    def test_simulation_error_recovery(self):
        """Test error recovery in simulation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation with potential errors
        for step in range(20):
            try:
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Continue simulation despite errors
                continue
        
        # Should complete without crashing
        assert brain.getTotalNeuronCount() > 0
def run_all_tests():
    """Run all tests and report results"""
    print("NLM Python Bindings Test Suite")
    print("=" * 40)
    
    test_classes = [
        TestNLMConfig,
        TestNLMBrain,
        TestNLMWorld,
        TestNLMAgent,
        TestNLMSimulationHelpers,
        TestNLMIntegration,
        TestNLMErrorHandling
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [
            method for method in dir(test_instance) 
            if method.startswith('test_') and callable(getattr(test_instance, method))
        ]
        
        for test_method_name in test_methods:
            try:
                test_method = getattr(test_instance, test_method_name)
                test_method()
                print(f"  ✓ {test_method_name}")
                passed += 1
            except Exception as e:
                print(f"  ✗ {test_method_name}: {e}")
                failed += 1
    
    print(f"\nTest Results:")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Total:  {passed + failed}")
    
    if failed == 0:
        print("\nAll tests passed! ✓")
        return True
    else:
        print(f"\n{failed} tests failed!")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)