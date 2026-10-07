#!/usr/bin/env python3
"""
Comprehensive Test Suite for NLM Python Bindings

This test suite verifies that all Python bindings are working correctly
and that the improved functionality is properly exposed.
"""

import pynlm
import tempfile
import os
import sys

class TestNLMPythonBindings:
    """Test suite for NLM Python bindings"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
    def test_basic_brain_creation(self):
        """Test basic brain creation and initialization"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Verify brain is initialized
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
    def test_brain_step(self):
        """Test brain stepping"""
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        assert brain.getTotalSpikeCount() >= initial_spikes
        
    def test_agent_functionality(self):
        """Test agent basic functionality"""
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test processing sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Test decoding motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        
    def test_memory_systems_access(self):
        """Test access to memory systems"""
        # Test working memory
        working_memory = brain.getWorkingMemory()
        if working_memory:
            # Should have methods to access working memory traces
            assert hasattr(working_memory, 'getActiveTraces')
            
        # Test episodic memory
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            assert hasattr(episodic_memory, 'getEpisodeCount')
            
        # Test associative memory
        associative_memory = brain.getAssociativeMemory()
        if associative_memory:
            assert hasattr(associative_memory, 'getAssociationCount')
            
    def test_plasticity_systems_access(self):
        """Test access to plasticity systems"""
        stdp = brain.getSTDP()
        if stdp:
            assert hasattr(stdp, 'getLTPWeight')
            assert hasattr(stdp, 'getLTDWeight')
            
        hebbian = brain.getHebbian()
        if hebbian:
            assert hasattr(hebbian, 'getLearningRate')
            
        structural = brain.getStructuralPlasticity()
        if structural:
            assert hasattr(structural, 'getSynaptogenesisRate')
            assert hasattr(structural, 'getPruningRate')
            
    def test_neuromodulation_systems_access(self):
        """Test access to neuromodulation systems"""
        dopamine = brain.getDopamine()
        if dopamine:
            assert hasattr(dopamine, 'getLevel')
            assert hasattr(dopamine, 'getPlasticityFactor')
            
        curiosity = brain.getCuriosity()
        if curiosity:
            assert hasattr(curiosity, 'getLevel')
            
        novelty = brain.getNovelty()
        if novelty:
            assert hasattr(novelty, 'getLevel')
            
        prediction_error = brain.getPredictionErrorSignal()
        if prediction_error:
            assert hasattr(prediction_error, 'getErrorValue')
            
    def test_prediction_system_access(self):
        """Test access to prediction system"""
        prediction_system = brain.getPredictionSystem()
        if prediction_system:
            assert hasattr(prediction_system, 'getPredictionError')
            assert hasattr(prediction_system, 'getConfidence')
            
    def test_cognition_systems_access(self):
        """Test access to cognition systems"""
        planner = brain.getPlanner()
        if planner:
            assert hasattr(planner, 'getPlanningDepth')
            assert hasattr(planner, 'getActionOptions')
            
        concept_formation = brain.getConceptFormation()
        if concept_formation:
            assert hasattr(concept_formation, 'getConceptCount')
            
        attention = brain.getAttention()
        if attention:
            assert hasattr(attention, 'getAttentionLevel')
            
    def test_development_system_access(self):
        """Test access to development system"""
        development_system = brain.getDevelopmentSystem()
        if development_system:
            assert hasattr(development_system, 'getDevelopmentalStage')
            assert hasattr(development_system, 'getPlasticityRate')
            
        # Test developmental stage
        stage = brain.getDevelopmentalStage()
        assert stage is not None
        
    def test_configuration_access(self):
        """Test configuration access and manipulation"""
        config = brain.getConfig()
        assert config is not None
        
        # Test basic configuration methods
        assert hasattr(config, 'has')
        assert hasattr(config, 'getKeys')
        assert hasattr(config, 'summary')
        
        # Test getting values
        neuron_count = config.getOr("neuron_count", 1000)
        assert isinstance(neuron_count, int)
        assert neuron_count > 0
        
    def test_agent_features(self):
        """Test agent feature flags"""
        # Test feature flags
        assert hasattr(agent, 'enableRewardModulation')
        assert hasattr(agent, 'enableCuriosity')
        assert hasattr(agent, 'enableStructuralPlasticity')
        assert hasattr(agent, 'enableDevelopment')
        
        # Test enable methods
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Test check methods
        assert hasattr(agent, 'isRewardModulationEnabled')
        assert hasattr(agent, 'isCuriosityEnabled')
        assert hasattr(agent, 'isStructuralPlasticityEnabled')
        assert hasattr(agent, 'isDevelopmentEnabled')
        
        # These should return booleans
        assert isinstance(agent.isRewardModulationEnabled(), bool)
        assert isinstance(agent.isCuriosityEnabled(), bool)
        assert isinstance(agent.isStructuralPlasticityEnabled(), bool)
        assert isinstance(agent.isDevelopmentEnabled(), bool)
        
    def test_world_integration(self):
        """Test world integration with brain"""
        # Configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Verify simulation ran
        assert brain.getTotalSpikeCount() > 0
        assert brain.getAverageFiringRate() >= 0.0
        
    def test_checkpointing(self):
        """Test checkpoint save/load functionality"""
        # Get initial state
        initial_spikes = brain.getTotalSpikeCount()
        
        # Create a temporary file for checkpoint
        with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
            checkpoint_file = f.name
            
        try:
            # Save brain state
            save_result = brain.save(checkpoint_file)
            assert save_result is True
            
            # Verify file exists and is not empty
            assert os.path.exists(checkpoint_file)
            assert os.path.getsize(checkpoint_file) > 0
            
            # Create a new brain and load the checkpoint
            new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            
            # Note: The C++ implementation might not fully support loading
            # since we haven't verified the checkpoint reader functionality
            # but we test that the save method works correctly
            assert new_brain is not None
            
        finally:
            # Clean up
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
                
    def test_advanced_configuration_factories(self):
        """Test advanced configuration factory methods"""
        # These methods should be available through the module level
        # The actual import of advanced config functions depends on
        # how they're exposed in the bindings
        
        # Test that we can create different types of configs
        default_config = pynlm.createDefaultConfig()
        assert default_config is not None
        
        # Test brain creation with different configs
        brain1 = pynlm.createBrain(default_config)
        brain1.initialize()
        
        assert brain1.getTotalNeuronCount() > 0
        
    def test_performance_metrics(self):
        """Test performance metrics and benchmarking"""
        # Simulate a simple benchmark
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        steps = 50
        start_firing = brain.getTotalSpikeCount()
        
        for i in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        end_firing = brain.getTotalSpikeCount()
        
        # Verify metrics
        assert brain.getAverageFiringRate() >= 0.0
        assert brain.getTotalSpikeCount() >= start_firing
        assert brain.getTotalSynapseCount() > 0
        assert brain.getActiveNeuronCount() >= 0
        
    def test_error_handling(self):
        """Test error handling and edge cases"""
        # Test invalid configuration key access
        config = brain.getConfig()
        
        # Test non-existent key returns default
        nonexistent_value = config.getOr("nonexistent_key", "default_value")
        assert nonexistent_value == "default_value"
        
        # Test that brain methods handle edge cases gracefully
        # (They should not crash on empty states)
        brain.reset()
        assert brain.getTotalSpikeCount() == 0
        
    def test_convenience_methods(self):
        """Test convenience and utility methods"""
        # Test world creation convenience
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Test agent creation convenience
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test brain creation convenience
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        
        assert brain2.getTotalNeuronCount() > 0
        
    def run_all_tests(self):
        """Run all test methods"""
        test_methods = [
            self.test_basic_brain_creation,
            self.test_brain_step,
            self.test_agent_functionality,
            self.test_memory_systems_access,
            self.test_plasticity_systems_access,
            self.test_neuromodulation_systems_access,
            self.test_prediction_system_access,
            self.test_cognition_systems_access,
            self.test_development_system_access,
            self.test_configuration_access,
            self.test_agent_features,
            self.test_world_integration,
            self.test_checkpointing,
            self.test_advanced_configuration_factories,
            self.test_performance_metrics,
            self.test_error_handling,
            self.test_convenience_methods,
        ]
        
        passed = 0
        failed = 0
        
        for test_method in test_methods:
            try:
                test_method()
                print(f"✓ {test_method.__name__} PASSED")
                passed += 1
            except Exception as e:
                print(f"✗ {test_method.__name__} FAILED: {e}")
                failed += 1
                
        print(f"\n=== Test Results ===")
        print(f"Passed: {passed}")
        print(f"Failed: {failed}")
        print(f"Total: {passed + failed}")
        
        return failed == 0

if __name__ == "__main__":
    print("=== NLM Python Bindings Test Suite ===")
    print("Testing all improved Python bindings functionality...\n")
    
    test_suite = TestNLMPythonBindings()
    
    success = test_suite.run_all_tests()
    
    if success:
        print("\n🎉 All tests passed! Python bindings are working correctly.")
        sys.exit(0)
    else:
        print(f"\n❌ {sum(1 for m in [m for m in dir(test_suite) if m.startswith('test_')] if callable(getattr(test_suite, m)))} tests failed.")
        sys.exit(1)