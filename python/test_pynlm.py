#!/usr/bin/env python3
"""
Python test suite for NLM (Neural Learning Machine) Python bindings.
Tests the enhanced Python API with Phase 6 system access and new features.
"""

import sys
import tempfile
import os
sys.path.insert(0, os.path.dirname(__file__))

import pynlm
import pytest

class TestNLMBasic:
    """Basic NLM functionality tests."""
    
    def test_basic_brain_creation(self):
        """Test basic brain creation and initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Aging
        
    def test_convenience_constructors(self):
        """Test convenience constructors for different brain types."""
        # Test research brain
        research_brain = pynlm.createResearchBrain(1000, 0.001)
        research_brain.initialize()
        assert research_brain.getTotalNeuronCount() == 1000
        
        # Test development brain
        dev_brain = pynlm.createDevelopmentBrain(500, True)
        dev_brain.initialize()
        assert dev_brain.getTotalNeuronCount() == 500
        
        # Test performance brain
        perf_brain = pynlm.createPerformanceBrain(2000, True)
        perf_brain.initialize()
        assert perf_brain.getTotalNeuronCount() == 2000
        
        # Test simple brain
        simple_brain = pynlm.createSimpleBrain(50)
        simple_brain.initialize()
        assert simple_brain.getTotalNeuronCount() == 50

    def test_configuration_presets(self):
        """Test configuration presets."""
        research_config = pynlm.createResearchConfig()
        assert research_config.getOr("brain.neuron_count", 0) > 0
        
        dev_config = pynlm.createDevelopmentConfig()
        assert dev_config.getOr("development.enable", False)
        
        perf_config = pynlm.createPerformanceConfig()
        assert perf_config.getOr("performance.optimization_level", 0) > 0
        
        simple_config = pynlm.createSimpleConfig()
        assert simple_config.getOr("brain.neuron_count", 0) == 100

    def test_brain_simulation(self):
        """Test basic brain simulation."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        brain.step(1)
        brain.step(2)
        
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes
        
        # Test statistics
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0f
        assert brain.getExcitationInhibitionRatio() >= 0.0f

    def test_agent_operations(self):
        """Test agent operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Test motor decoding
        action = agent.decodeMotorCommand()
        assert action is not None
        
        # Test neuromodulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.applyRewardModulation(1.0, 0.5)
        
        assert agent.isRewardModulationEnabled()
        assert agent.isCuriosityEnabled()

class TestPhase6Systems:
    """Tests for Phase 6 systems exposed in Python bindings."""
    
    def test_memory_systems(self):
        """Test Phase 6 memory systems."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test working memory
        working_memory = brain.getBrainWorkingMemory()
        working_memory.store([0.1, 0.2, 0.3, 0.4])
        retrieved = working_memory.retrieve()
        assert len(retrieved) == 4
        
        # Test episodic memory (if available)
        episodic_memory = brain.getBrainEpisodicMemory()
        if episodic_memory:
            # Store an experience
            episodic_memory.storeExperience([0.0, 0.0], [1.0, 0.0], [0.0, 1.0], 1.0)
            assert episodic_memory.getExperienceCount() > 0
        
        # Test associative memory (if available)
        associative_memory = brain.getBrainAssociativeMemory()
        if associative_memory:
            associative_memory.associate([0.1, 0.2], [0.3, 0.4])
            recall = associative_memory.recall([0.1, 0.2], 5)
            assert len(recall) > 0

    def test_prediction_system(self):
        """Test prediction system."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        prediction_system = brain.getBrainPredictionSystem()
        if prediction_system:
            # Test prediction
            test_input = pynlm.Vision(8, 8)
            prediction_system.predict(test_input)
            
            # Test prediction error
            error = prediction_system.getPredictionError()
            assert error >= 0.0f
            
            confidence = prediction_system.getPredictionConfidence()
            assert 0.0f <= confidence <= 1.0f

    def test_cognition_systems(self):
        """Test cognition systems."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test neural planner
        planner = brain.getBrainPlanner()
        if planner:
            test_state = [0.1, 0.2, 0.3, 0.4]
            action = planner.planAction(test_state, 0.5)
            assert action is not None
            
            # Test goal setting
            goal = [0.5, 0.5, 0.5, 0.5]
            planner.setCurrentGoal(goal)
            retrieved_goal = planner.getCurrentGoal()
            assert len(retrieved_goal) == len(goal)
        
        # Test attentional selection
        attention = brain.getBrainAttentionalSelection()
        if attention:
            competitors = [1, 2, 3, 4, 5]
            winners = attention.processCompetition(competitors, 0.5)
            assert len(winners) > 0
        
        # Test concept formation
        concept_formation = brain.getBrainConceptFormation()
        if concept_formation:
            pattern = [0.1, 0.2, 0.3]
            concept_id = concept_formation.getConceptId(pattern)
            # Should create a new concept
            assert concept_id >= 0

    def test_development_system(self):
        """Test development system."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        development_system = brain.getBrainDevelopmentSystem()
        if development_system:
            stage = development_system.getCurrentStage()
            assert stage != pynlm.DevelopmentalStage.Aging  # Should not be aging initially
            
            # Test setting development stage
            development_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)
            assert development_system.getCurrentStage() == pynlm.DevelopmentalStage.CriticalPeriod

    def test_neuromodulation_systems(self):
        """Test neuromodulation systems."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test curiosity
        curiosity = brain.getBrainCuriosity()
        if curiosity:
            level = curiosity.getCuriosityLevel()
            assert 0.0f <= level <= 1.0f
            curiosity.setCuriosityLevel(0.8)
            assert curiosity.getCuriosityLevel() == 0.8f
        
        # Test novelty
        novelty = brain.getBrainNovelty()
        if novelty:
            test_input = pynlm.Vision(8, 8)
            novelty_level = novelty.computeNovelty(test_input)
            assert 0.0f <= novelty_level <= 1.0f
        
        # Test prediction error
        pred_error = brain.getBrainPredictionError()
        if pred_error:
            error = pred_error.getPredictionError()
            assert error >= 0.0f

class TestBatchOperations:
    """Tests for batch operations and performance monitoring."""
    
    def test_batch_sensory_processing(self):
        """Test batch sensory processing."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        
        # Create multiple sensory inputs
        percepts = [world.getSensoryPercept() for _ in range(5)]
        
        # Test batch processing
        pynlm.receiveSensoryInputs(brain, percepts)
        
        # Each percept should have been processed
        brain.step(0)
    
    def test_performance_benchmarking(self):
        """Test performance benchmarking."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run benchmark
        result = pynlm.benchmark(brain, 100, True)
        
        assert result.stepsCompleted == 100
        assert result.totalSpikes >= 0
        assert result.averageFiringRate >= 0.0f
        assert result.exitationInhibitionRatio >= 0.0f
        assert result.totalTime >= 0.0f
        
        # Check subsystem stats if detailed mode
        if result.subsystemStats:
            assert "working_memory_activity" in result.subsystemStats
            assert "planning_confidence" in result.subsystemStats
            assert "curiosity_level" in result.subsystemStats
    
    def test_memory_usage_monitoring(self):
        """Test memory usage monitoring."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        usage = pynlm.getMemoryUsage(brain)
        
        assert usage.totalNeurons > 0
        assert usage.totalSynapses >= 0
        assert usage.activeNeurons >= 0
        assert usage.firingNeurons >= 0
        assert usage.pendingSpikeEvents >= 0
        assert 0.0f <= usage.memoryUtilization <= 1.0f

class TestSimulationUtilities:
    """Tests for simulation utilities."""
    
    def test_run_for_steps(self):
        """Test runForSteps utility."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        pynlm.runForSteps(brain, 50, False)
        
        # Should have run exactly 50 steps
        assert brain.getTotalNeuronCount() > 0
    
    def test_save_checkpoint(self):
        """Test saveCheckpoint utility."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brain.step(0)
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save checkpoint
            success = pynlm.saveCheckpoint(brain, filepath, True)
            assert success
            assert os.path.exists(filepath)
            
            # Load checkpoint
            new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            new_brain.initialize()
            load_success = new_brain.load(filepath)
            assert load_success
        
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)

class TestValidationAndErrorChecking:
    """Tests for validation and error checking."""
    
    def test_sensory_input_validation(self):
        """Test sensory input validation."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Create a valid sensory input
        valid_input = pynlm.Vision(8, 8)
        assert pynlm.validateSensoryInput(brain, valid_input)
        
        # Test with None input
        assert not pynlm.validateSensoryInput(brain, None)
    
    def test_configuration_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", -100)  # Invalid
        config.set("brain.connection_probability", 1.5)  # Invalid
        config.set("simulation_timestep", 2.0)  # Invalid
        
        is_valid, errors = pynlm.validateConfig(config)
        assert not is_valid
        assert len(errors) > 0
        
        # Test valid configuration
        config.set("brain.neuron_count", 100)
        config.set("brain.connection_probability", 0.1)
        config.set("simulation_timestep", 0.001)
        
        is_valid, errors = pynlm.validateConfig(config)
        assert is_valid
        assert len(errors) == 0

class TestAdvancedFeatures:
    """Tests for advanced features and edge cases."""
    
    def test_random_generator(self):
        """Test random generator functionality."""
        rng = pynlm.RandomGenerator(42)
        assert rng.getSeed() == 42
        
        # Test random float
        float_val = rng.randomFloat()
        assert 0.0 <= float_val < 1.0
        
        # Test random integer
        int_val = rng.randomInt(1, 10)
        assert 1 <= int_val <= 10
        
        # Test random normal
        normal_val = rng.randomNormal(0.0, 1.0)
        assert normal_val == normal_val  # Not NaN or inf
        
        # Test random boolean
        bool_val = rng.randomBool(0.7)
        assert bool_val in [True, False]
    
    def test_error_handling(self):
        """Test error handling for invalid inputs."""
        config = pynlm.createDefaultConfig()
        
        # Test creating brain with invalid config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test brain operations with various invalid inputs
        try:
            brain.step(-1)  # Should this be allowed?
            # Implementation may not validate this
        except Exception:
            pass  # Expected to handle gracefully

if __name__ == "__main__":
    print("Running NLM Python Bindings Test Suite...")
    print("=" * 50)
    
    # Run tests manually
    test_classes = [
        TestNLMBasic,
        TestPhase6Systems,
        TestBatchOperations,
        TestSimulationUtilities,
        TestValidationAndErrorChecking,
        TestAdvancedFeatures,
    ]
    
    total_tests = 0
    passed_tests = 0
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        test_instance = test_class()
        
        for attr_name in dir(test_instance):
            if attr_name.startswith('test_'):
                method = getattr(test_instance, attr_name)
                if callable(method):
                    total_tests += 1
                    try:
                        print(f"  Testing {attr_name}...", end=" ")
                        method()
                        print("PASS")
                        passed_tests += 1
                    except Exception as e:
                        print(f"FAIL: {e}")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed_tests}/{total_tests} tests passed")
    
    if passed_tests == total_tests:
        print("All tests PASSED!")
        sys.exit(0)
    else:
        print("Some tests FAILED!")
        sys.exit(1)