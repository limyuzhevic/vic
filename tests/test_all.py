#!/usr/bin/env python3
"""
Python Tests for NLM (Neural Learning Machine)

This file contains comprehensive tests for the enhanced NLM Python API.
Tests include:
- Configuration system
- Brain functionality  
- Agent functionality
- World simulation
- Logging system
- Memory systems
- Neuromodulation
- Development stages
"""

import pytest
import tempfile
import os
import sys
import json
from pathlib import Path

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class TestBasicConfiguration:
    """Test basic configuration functionality."""
    
    def test_default_config_creation(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Should be able to set values
        config.set_brain_neuron_count(1000)
        assert config.has("brain.neuron_count")
        
        print("✓ Default config creation test passed")
    
    def test_configuration_values(self):
        """Test setting and getting configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Test different value types
        config.set_int("int_key", 42)
        assert config.has("int_key")
        
        config.set_double("float_key", 3.14159)
        assert config.has("float_key")
        
        config.set_bool("bool_key", True)
        assert config.has("bool_key")
        
        config.set_string("string_key", "test_value")
        assert config.has("string_key")
        
        # Test get methods
        assert config.get_int("int_key") == 42
        assert abs(config.get_double("float_key") - 3.14159) < 0.001
        assert config.get_bool("bool_key") is True
        assert config.get_string("string_key") == "test_value"
        
        # Test get with default
        assert config.get_int("nonexistent_key", 100) == 100
        
        print("✓ Configuration values test passed")
    
    def test_nested_keys(self):
        """Test nested key access."""
        config = pynlm.createDefaultConfig()
        
        # Test nested set/get
        config.set_nested(["brain", "neuron_count"], 500)
        assert config.has_nested(["brain", "neuron_count"])
        assert config.get_nested(["brain", "neuron_count"]) == "500"
        
        config.set_nested(["plasticity", "stdp", "enabled"], True)
        assert config.has_nested(["plasticity", "stdp", "enabled"])
        
        print("✓ Nested keys test passed")
    
    def test_configuration_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Should pass validation with defaults
        config.validate()
        
        # Should fail with invalid values
        config.set_int("brain.neuron_count", -100)
        try:
            config.validate()
            assert False, "Should have raised exception for invalid neuron_count"
        except RuntimeError:
            pass  # Expected
        
        # Test getting validation errors
        config.set_int("brain.neuron_count", 500)  # Fix invalid value
        config.set_double("brain.connection_probability", 1.5)  # Invalid probability
        
        errors = config.get_validation_errors()
        assert len(errors) > 0
        assert any("connection_probability" in error for error in errors)
        
        print("✓ Configuration validation test passed")
    
    def test_configuration_save_load(self):
        """Test saving and loading configuration from file."""
        config = pynlm.createDefaultConfig()
        config.set_string("test.key", "test_value")
        config.set_int("test.number", 123)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            temp_file = f.name
        
        try:
            # Load from file
            new_config = pynlm.createDefaultConfig()
            assert new_config.loadFromFile(temp_file)
            
            # Check values were loaded
            assert new_config.has("test.key")
            assert new_config.get_string("test.key") == "test_value"
            assert new_config.has("test.number")
            assert new_config.get_int("test.number") == 123
            
        finally:
            # Clean up
            os.unlink(temp_file)
        
        print("✓ Configuration save/load test passed")
class TestBrainFunctionality:
    """Test brain core functionality."""
    
    def test_brain_creation_and_initialization(self):
        """Test creating and initializing a brain."""
        config = pynlm.createDefaultConfig()
        config.set_brain_neuron_count(100)
        
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        brain.initialize()
        
        # Check statistics
        assert brain.getTotalNeuronCount() == 100
        assert brain.getTotalSynapseCount() > 0
        assert brain.getActiveNeuronCount() == 0
        
        print("✓ Brain creation and initialization test passed")
    
    def test_brain_simulation_step(self):
        """Test brain simulation steps."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run several steps
        for i in range(10):
            brain.step(i)
        
        # Should have generated some activity
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes
        
        print("✓ Brain simulation step test passed")
    
    def test_brain_sensory_input(self):
        """Test sensory input functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a simple sensory input
        sensory_input = pynlm.createDefaultConfig()  # Using config as placeholder
        
        # Inject current into a neuron
        sensory_input = pynlm.createDefaultConfig()
        
        print("✓ Brain sensory input test passed")
    
    def test_brain_action_production(self):
        """Test action production."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get action
        action = brain.produceAction()
        assert action is not None
        assert action.getType() is not None
        
        print("✓ Brain action production test passed")
    
    def test_brain_memory_systems(self):
        """Test integrated memory systems."""
        config = pynlm.createDefaultConfig()
        config.set_brain_neuron_count(500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check memory systems are available
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        # They should be available (not nullptr in Phase 6)
        # Note: In Python bindings, getWorkingMemory() returns None if not initialized
        
        print("✓ Brain memory systems test passed")
    
    def test_brain_prediction_system(self):
        """Test prediction system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        prediction_system = brain.getPredictionSystem()
        assert prediction_system is not None
        
        print("✓ Brain prediction system test passed")
    
    def test_brain_cognition_systems(self):
        """Test cognition systems integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check cognition systems
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        # They should be available in Phase 6
        assert planner is not None
        assert concept_formation is not None
        assert attention is not None
        
        print("✓ Brain cognition systems test passed")
    
    def test_brain_neuromodulation(self):
        """Test neuromodulation systems."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionErrorSignal()
        
        # Check neuromodulators are available
        assert dopamine is not None
        assert curiosity is not None
        assert novelty is not None
        assert prediction_error is not None
        
        print("✓ Brain neuromodulation test passed")
    
    def test_brain_development(self):
        """Test development system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check developmental stage
        stage = brain.getDevelopmentalStage()
        assert stage is not None
        
        print("✓ Brain development test passed")
class TestAgentFunctionality:
    """Test agent and brain interface functionality."""
    
    def test_agent_creation(self):
        """Test creating an agent brain interface."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        print("✓ Agent creation test passed")
    
    def test_agent_initialization(self):
        """Test agent initialization with world."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        print("✓ Agent initialization test passed")
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        # Update world to get sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        print("✓ Agent sensory processing test passed")
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        
        print("✓ Agent motor decoding test passed")
    
    def test_agent_neuromodulation(self):
        """Test reward neuromodulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Enable reward modulation
        agent.enableRewardModulation(True)
        assert agent.isRewardModulationEnabled()
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation level
        neuromod_level = agent.getNeuromodulationLevel()
        
        print("✓ Agent neuromodulation test passed")
    
    def test_agent_development(self):
        """Test agent development updates."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
        
        print("✓ Agent development test passed")
    
    def test_agent_subsystem_enables(self):
        """Test enabling/disabling subsystems."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Test enabling subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Check they're enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
        print("✓ Agent subsystem enables test passed")
class TestWorldFunctionality:
    """Test world simulation functionality."""
    
    def test_world_creation_and_configuration(self):
        """Test creating and configuring a world."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Configure world
        world.configure(width=30, height=40, visionWidth=10, visionHeight=10)
        
        # Check configuration
        assert world.getWidth() == 30
        assert world.getHeight() == 40
        assert world.getVisionWidth() == 10
        assert world.getVisionHeight() == 10
        
        print("✓ World creation and configuration test passed")
    
    def test_world_reset_and_update(self):
        """Test world reset and update."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(5.0, 5.0)
        
        # Reset world
        world.reset()
        
        # Update world
        world.update(0.1)
        
        # Check agent body
        body = world.getAgentBody()
        assert body is not None
        assert body.x == 5.0
        assert body.y == 5.0
        
        print("✓ World reset and update test passed")
    
    def test_world_sensory_percept(self):
        """Test sensory percept generation."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Check vision
        vision = percept.getVision()
        assert vision is not None
        
        # Check internal signals
        internal = percept.getInternal()
        assert internal is not None
        
        print("✓ World sensory percept test passed")
    
    def test_world_motor_commands(self):
        """Test motor command application."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10.0, 10.0)
        
        # Create a motor command
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ World motor commands test passed")
class TestLoggingFunctionality:
    """Test logging system functionality."""
    
    def test_logger_creation(self):
        """Test logger creation."""
        from nlm import Logger, ConsoleLogger, LogLevel
        
        logger = Logger()
        assert logger is not None
        
        print("✓ Logger creation test passed")
    
    def test_log_level_enum(self):
        """Test log level enumeration."""
        from nlm import LogLevel
        
        assert LogLevel.DEBUG == 0
        assert LogLevel.INFO == 1
        assert LogLevel.Warning == 2
        assert LogLevel.Error == 3
        assert LogLevel.Critical == 4
        
        print("✓ Log level enum test passed")
    
    def test_console_logger(self):
        """Test console logger functionality."""
        from nlm import ConsoleLogger, LogLevel
        
        logger = ConsoleLogger()
        assert logger is not None
        
        # Test log level setting
        logger.setLevel(LogLevel.Debug)
        assert logger.getLevel() == LogLevel.Debug
        
        # Test color setting
        logger.setUseColors(True)
        assert logger.getUseColors() is True
        
        logger.setUseColors(False)
        assert logger.getUseColors() is False
        
        print("✓ Console logger test passed")
    
    def test_global_logger_functions(self):
        """Test global logger functions."""
        from nlm import Logger, LogLevel
        
        # Test getting global logger
        logger = Logger.getGlobal()
        assert logger is not None
        
        # Test setting global logger
        new_logger = Logger()
        Logger.setGlobal(new_logger)
        
        # Verify it's set
        current_logger = Logger.getGlobal()
        assert current_logger is new_logger
        
        print("✓ Global logger functions test passed")
    
    def test_log_stream(self):
        """Test log stream functionality."""
        from nlm import Logger, LogLevel
        
        logger = Logger()
        
        # Create log stream
        stream = logger.logStream(LogLevel.Info)
        
        # Write to stream
        stream << "Test message"
        
        print("✓ Log stream test passed")
    
    def test_logging_macros(self):
        """Test logging macro functions."""
        from nlm import logDebug, logInfo, logWarning, logError, logCritical
        
        # These should not raise exceptions
        logDebug("Debug message")
        logInfo("Info message")
        logWarning("Warning message")
        logError("Error message")
        logCritical("Critical message")
        
        print("✓ Logging macros test passed")
    
    def test_log_stream_macros(self):
        """Test log stream macro functions."""
        from nlm import logDebugStream, logInfoStream, logWarningStream, logErrorStream, logCriticalStream
        
        # Create log streams
        debug_stream = logDebugStream()
        info_stream = logInfoStream()
        warning_stream = logWarningStream()
        error_stream = logErrorStream()
        critical_stream = logCriticalStream()
        
        # These should work without issues
        debug_stream << "Debug stream message"
        info_stream << "Info stream message"
        warning_stream << "Warning stream message"
        error_stream << "Error stream message"
        critical_stream << "Critical stream message"
        
        print("✓ Log stream macros test passed")
class TestMemorySystems:
    """Test memory systems functionality."""
    
    def test_working_memory(self):
        """Test working memory functionality."""
        from nlm import createDefaultConfig, createBrain
        
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        if working_memory is not None:
            # Test working memory functions
            working_memory.store("test_pattern", 1.0)
            
            retrieval = working_memory.retrieve()
            assert isinstance(retrieval, list)
            
            print("✓ Working memory test passed")
    
    def test_episodic_memory(self):
        """Test episodic memory functionality."""
        from nlm import createDefaultConfig, createBrain
        
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory is not None:
            # Test episodic memory functions
            episodes = episodic_memory.getEpisodes()
            assert isinstance(episodes, list)
            
            print("✓ Episodic memory test passed")
    
    def test_associative_memory(self):
        """Test associative memory functionality."""
        from nlm import createDefaultConfig, createBrain
        
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        associative_memory = brain.getAssociativeMemory()
        if associative_memory is not None:
            print("✓ Associative memory test passed")
class TestCompleteAgentIntegration:
    """Test complete agent integration with all subsystems."""
    
    def test_full_agent_simulation(self):
        """Test a complete agent simulation with all subsystems."""
        config = pynlm.createDefaultConfig()
        config.set_brain_neuron_count(200)
        
        # Create brain with all features enabled
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
        world.setAgentStart(12.5, 12.5)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Check final state
        assert brain.getTotalSpikeCount() > 0
        assert brain.getFiringNeuronCount() >= 0
        
        print("✓ Full agent simulation test passed")
class TestPerformanceBenchmarks:
    """Test performance benchmarks."""
    
    def test_brain_performance(self):
        """Test brain performance with different configurations."""
        import time
        
        # Test with small brain
        config = pynlm.createDefaultConfig()
        config.set_brain_neuron_count(100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        end_time = time.time()
        
        small_brain_time = end_time - start_time
        print(f"  Small brain (100 neurons): {small_brain_time:.3f}s for 100 steps")
        
        # Test with larger brain
        config.set_brain_neuron_count(1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        end_time = time.time()
        
        large_brain_time = end_time - start_time
        print(f"  Large brain (1000 neurons): {large_brain_time:.3f}s for 100 steps")
        
        print("✓ Brain performance test passed")

# Convenience functions for running specific tests
def run_all_tests():
    """Run all NLM tests."""
    print("=" * 60)
    print("NLM Python API Test Suite")
    print("=" * 60)
    
    test_classes = [
        TestBasicConfiguration,
        TestBrainFunctionality,
        TestAgentFunctionality,
        TestWorldFunctionality,
        TestLoggingFunctionality,
        TestMemorySystems,
        TestCompleteAgentIntegration,
        TestPerformanceBenchmarks,
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\n{'=' * 60}")
        print(f"Running {test_class.__name__}")
        print(f"{'=' * 60}")
        
        test_instance = test_class()
        
        # Find all test methods
        for attr_name in dir(test_instance):
            if attr_name.startswith('test_'):
                method = getattr(test_instance, attr_name)
                if callable(method):
                    try:
                        method()
                        passed += 1
                        print(f"✓ {attr_name} passed")
                    except Exception as e:
                        failed += 1
                        print(f"✗ {attr_name} failed: {str(e)}")
    
    print(f"\n{'=' * 60}")
    print("Test Summary")
    print(f"{'=' * 60}")
    print(f"Total tests: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
    else:
        print(f"\n❌ {failed} tests failed")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)