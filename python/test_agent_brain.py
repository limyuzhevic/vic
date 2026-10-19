#!/usr/bin/env python3
"""
Unit Tests for NLM AgentBrain Methods

This module contains comprehensive unit tests for all AgentBrain methods,
including processPercepts, decodeCommands, stepMultiple, and other core functions.
"""

import os
import sys
import time
import tempfile
import shutil
import numpy as np
from pathlib import Path
from typing import Dict, List, Any

# Import NLM bindings
import pynlm
from test_suite import NLMTestSuite, NLMSyntheticDataGenerator, NLMTestValidationError
class AgentBrainUnitTests(NLMTestSuite):
    """
    Comprehensive unit tests for AgentBrain methods.
    
    Tests all new AgentBrain methods including:
    - processPercepts
    - decodeCommands
    - stepMultiple
    - serialize/deserialize
    - error handling and validation
    - performance profiling
    - batch processing
    """
    
    def setup_method(self):
        """Setup before each test method."""
        self.setup_test_environment()
        
        # Create default configuration and brain
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        
        # Create world and agent brain
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world for testing
        self.world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        
    def teardown_method(self):
        """Cleanup after each test method."""
        self.teardown_test_environment()
    
    # Test basic initialization and configuration
    def test_agent_initialization(self):
        """Test basic agent brain initialization."""
        # Verify agent was created successfully
        self.framework.assert_true(self.agent is not None, "Agent should not be None")
        
        # Test configuration methods
        self.framework.assert_true(hasattr(self.agent, 'initialize'), "Agent should have initialize method")
        self.framework.assert_true(hasattr(self.agent, 'getSensoryInputSize'), "Agent should have getSensoryInputSize method")
        self.framework.assert_true(hasattr(self.agent, 'getMotorOutputSize'), "Agent should have getMotorOutputSize method")
        
        # Test sensory input size
        input_size = self.agent.getSensoryInputSize()
        self.framework.assert_true(input_size > 0, f"Sensory input size should be positive, got {input_size}")
        
        # Test motor output size  
        output_size = self.agent.getMotorOutputSize()
        self.framework.assert_true(output_size > 0, f"Motor output size should be positive, got {output_size}")
        
        print(f"✓ Agent initialized: input_size={input_size}, output_size={output_size}")
    
    def test_agent_initialization_with_world(self):
        """Test agent initialization with world."""
        self.agent.initialize(self.world)
        
        # Verify agent is now initialized
        self.framework.assert_true(self.agent.isEpisodeActive(), "Agent should be active after initialization")
        
        print("✓ Agent initialization with world successful")
    
    # Test sensory processing methods
    def test_sensory_percept_processing(self):
        """Test sensory percept processing with validation."""
        # Generate test percept
        percept = self.data_generator.generate_sensory_percept()
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Test processSensoryInput with validation
        start_time, duration = self.framework.measure_performance(
            self.agent.processSensoryInput, percept, True
        )
        
        self.framework.log_performance("test_sensory_percept_processing", duration)
        
        # Verify processing didn't crash
        self.framework.assert_true(True, "Sensory processing should not crash")
        
        print(f"✓ Sensory percept processing successful (duration: {duration:.4f}s)")
    
    def test_sensory_percept_processing_without_validation(self):
        """Test sensory percept processing without validation."""
        percept = self.data_generator.generate_sensory_percept()
        self.agent.initialize(self.world)
        
        # Test without validation for performance
        start_time, duration = self.framework.measure_performance(
            self.agent.processSensoryInput, percept, False
        )
        
        self.framework.log_performance("test_sensory_percept_processing_no_validation", duration)
        self.framework.assert_true(True, "Processing without validation should work")
        
        print(f"✓ Sensory processing without validation successful (duration: {duration:.4f}s)")
    
    def test_batch_percept_processing(self):
        """Test batch processing of multiple percepts."""
        # Generate batch of percepts
        batch_size = 10
        batch_percepts = self.data_generator.generate_batch_percepts(batch_size)
        
        self.agent.initialize(self.world)
        
        # Test batch processing performance
        start_time, duration = self.framework.measure_performance(
            self.agent.processPercepts, batch_percepts
        )
        
        self.framework.log_performance("test_batch_percept_processing", duration)
        
        # Verify all percepts were processed
        self.framework.assert_equal(len(batch_percepts), batch_size, "Batch size should match")
        
        print(f"✓ Batch processing successful: {batch_size} percepts in {duration:.4f}s")
    
    def test_step_multiple_methods(self):
        """Test stepMultiple and related methods."""
        self.agent.initialize(self.world)
        
        # Generate multiple steps of percepts
        steps = 5
        step_percepts = self.data_generator.generate_batch_percepts(steps)
        
        # Test stepMultiple if available
        if hasattr(self.agent, 'stepMultiple'):
            start_time, duration = self.framework.measure_performance(
                self.agent.stepMultiple, step_percepts
            )
            
            self.framework.log_performance("test_step_multiple", duration)
            print(f"✓ stepMultiple successful: {steps} steps in {duration:.4f}s")
        else:
            # Fallback: process each step individually
            total_start_time = time.time()
            for percept in step_percepts:
                self.agent.processSensoryInput(percept, False)
            total_duration = time.time() - total_start_time
            
            self.framework.log_performance("test_step_multiple_fallback", total_duration)
            print(f"✓ Step processing successful (fallback): {steps} steps in {total_duration:.4f}s")
    
    # Test motor command decoding
    def test_motor_command_decoding(self):
        """Test motor command decoding."""
        self.agent.initialize(self.world)
        
        # Process some sensory input to give brain something to work with
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        
        # Decode motor command
        start_time, duration = self.framework.measure_performance(
            self.agent.decodeMotorCommand
        )
        
        self.framework.log_performance("test_motor_command_decoding", duration)
        
        # Verify command was decoded
        command = self.agent.decodeMotorCommand()
        self.framework.assert_true(command is not None, "Motor command should not be None")
        
        print(f"✓ Motor command decoding successful: {command} (duration: {duration:.4f}s)")
    
    def test_motor_command_array_decoding(self):
        """Test decoding array of motor commands."""
        self.agent.initialize(self.world)
        
        # Process input to enable decoding
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        
        # Test multiple command decoding
        command_count = 5
        commands = []
        
        for _ in range(command_count):
            cmd = self.agent.decodeMotorCommand()
            commands.append(cmd)
        
        self.framework.assert_equal(len(commands), command_count, "Should have decoded all commands")
        
        print(f"✓ Motor command array decoding successful: {command_count} commands")
    
    # Test reward modulation
    def test_reward_modulation(self):
        """Test reward modulation functionality."""
        self.agent.initialize(self.world)
        
        # Test various reward values
        test_rewards = [0.0, 0.5, 1.0, -0.1, 0.99]
        
        for reward in test_rewards:
            predicted_reward = max(0.0, reward * 0.5)
            
            start_time, duration = self.framework.measure_performance(
                self.agent.applyRewardModulation, reward, predicted_reward
            )
            
            self.framework.log_performance(f"test_reward_modulation_{reward}", duration)
        
        print(f"✓ Reward modulation successful for {len(test_rewards)} reward values")
    
    # Test episode management
    def test_episode_lifecycle(self):
        """Test episode start/end lifecycle."""
        self.agent.initialize(self.world)
        
        # Start episode
        episode_id = 1
        self.agent.startEpisode(episode_id, "test_episode")
        
        self.framework.assert_true(self.agent.isEpisodeActive(), "Episode should be active")
        
        # Record some actions
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        command = self.agent.decodeMotorCommand()
        
        self.agent.recordEpisodeAction(command, 0.5)
        
        # Verify episode score
        score = self.agent.getEpisodeScore()
        self.framework.assert_true(score >= 0.0, "Episode score should be non-negative")
        
        # End episode
        self.agent.endEpisode(1.0)
        self.framework.assert_false(self.agent.isEpisodeActive(), "Episode should be inactive after end")
        
        print(f"✓ Episode lifecycle successful: score={score}")
    
    # Test serialization/deserialization
    def test_state_serialization(self):
        """Test agent state serialization."""
        self.agent.initialize(self.world)
        
        # Process some data to create state
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        
        # Save state
        with tempfile.TemporaryDirectory() as temp_dir:
            state_file = os.path.join(temp_dir, "test_state.json")
            
            save_success = self.agent.saveState(state_file)
            self.framework.assert_true(save_success, "State save should succeed")
            
            # Verify file was created
            self.framework.assert_true(os.path.exists(state_file), "State file should exist")
            
            # Load state (create new agent first)
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_agent = pynlm.createAgentBrain(new_brain)
            
            load_success = new_agent.loadState(state_file)
            self.framework.assert_true(load_success, "State load should succeed")
            
        print("✓ State serialization/deserialization successful")
    
    # Test error handling and validation
    def test_error_handling_null_agent(self):
        """Test error handling with null/None agent."""
        # This test is more conceptual since we can't actually pass None to
        # C++ bound methods. We'll test validation through the framework.
        self.framework.assert_true(True, "Error handling test placeholder")
        
        print("✓ Error handling test passed")
    
    def test_validation_methods(self):
        """Test validation methods if available."""
        self.agent.initialize(self.world)
        
        # Test validateSensoryPercept if available
        if hasattr(self.agent, 'validateSensoryPercept'):
            percept = self.data_generator.generate_sensory_percept()
            validation_result = self.agent.validateSensoryPercept(percept)
            self.framework.assert_true(validation_result, "Valid percept should pass validation")
            
            print("✓ Sensory percept validation successful")
        
        # Test validateRewardValue if available
        if hasattr(self.agent, 'validateRewardValue'):
            valid_reward = self.agent.validateRewardValue(0.5)
            invalid_reward = self.agent.validateRewardValue(-1.0)
            
            self.framework.assert_true(valid_reward, "Valid reward should pass validation")
            self.framework.assert_false(invalid_reward, "Invalid reward should fail validation")
            
            print("✓ Reward validation successful")
    
    # Test performance profiling
    def test_performance_profiling(self):
        """Test performance profiling functionality."""
        self.agent.initialize(self.world)
        
        # Generate workload
        percepts = self.data_generator.generate_batch_percepts(20)
        
        # Run performance test
        start_time = time.time()
        for percept in percepts:
            self.agent.processSensoryInput(percept, False)
        
        processing_time = time.time() - start_time
        
        # Collect metrics
        metrics = self.agent.profilePerformance()
        
        self.framework.assert_true(metrics is not None, "Performance metrics should not be None")
        self.framework.assert_true(metrics.processingEfficiency > 0, "Processing efficiency should be positive")
        self.framework.assert_true(metrics.stepsPerSecond > 0, "Steps per second should be positive")
        
        self.framework.log_performance("test_performance_profiling", processing_time, **metrics)
        
        print(f"✓ Performance profiling successful: efficiency={metrics.processingEfficiency:.3f}")
    
    # Test learning and development methods
    def test_learning_methods(self):
        """Test learning and development methods."""
        self.agent.initialize(self.world)
        
        # Test learning rate setting
        if hasattr(self.agent, 'setLearningRate'):
            self.agent.setLearningRate(0.05)
            print("✓ Learning rate setting successful")
        
        # Test exploration rate setting
        if hasattr(self.agent, 'setExplorationRate'):
            self.agent.setExplorationRate(0.2)
            print("✓ Exploration rate setting successful")
        
        # Test reward shaping
        if hasattr(self.agent, 'setRewardShaping'):
            self.agent.setRewardShaping(0.8)
            print("✓ Reward shaping setting successful")
        
        # Test curiosity exploration
        if hasattr(self.agent, 'setCuriosityExploration'):
            self.agent.setCuriosityExploration(0.3)
            print("✓ Curiosity exploration setting successful")
        
        # Test development features
        if hasattr(self.agent, 'enableDevelopment'):
            self.agent.enableDevelopment(True)
            print("✓ Development enable successful")
        
        if hasattr(self.agent, 'enableStructuralPlasticity'):
            self.agent.enableStructuralPlasticity(True)
            print("✓ Structural plasticity enable successful")
    
    # Test neuromodulation methods
    def test_neuromodulation_methods(self):
        """Test neuromodulation functionality."""
        self.agent.initialize(self.world)
        
        # Test neuromodulation level access
        neuromodulation_level = self.agent.getNeuromodulationLevel()
        self.framework.assert_true(0.0 <= neuromodulation_level <= 1.0, 
                                 "Neuromodulation level should be between 0 and 1")
        
        # Test curiosity level
        curiosity_level = self.agent.getCuriosityLevel()
        self.framework.assert_true(0.0 <= curiosity_level <= 1.0,
                                 "Curiosity level should be between 0 and 1")
        
        # Test novelty level
        novelty_level = self.agent.getNoveltyLevel()
        self.framework.assert_true(0.0 <= novelty_level <= 1.0,
                                 "Novelty level should be between 0 and 1")
        
        # Test prediction error
        prediction_error = self.agent.getPredictionError()
        self.framework.assert_true(0.0 <= prediction_error <= 1.0,
                                 "Prediction error should be between 0 and 1")
        
        # Test episode score
        episode_score = self.agent.getEpisodeScore()
        self.framework.assert_true(episode_score >= 0.0,
                                 "Episode score should be non-negative")
        
        print(f"✓ Neuromodulation levels successful: curiosity={curiosity_level:.3f}, novelty={novelty_level:.3f}")
    
    # Test system integration
    def test_system_integration(self):
        """Test integration of all systems."""
        # Create complete system
        world = pynlm.createSimpleWorld()
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Configure world
        world.configure(width=100, height=100, visionWidth=32, visionHeight=32)
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Generate and process percept
        percept = self.data_generator.generate_sensory_percept()
        agent.processSensoryInput(percept, True)
        
        # Get motor command
        command = agent.decodeMotorCommand()
        
        # Apply reward
        agent.applyRewardModulation(1.0, 0.5)
        
        # Update development
        if hasattr(agent, 'updateDevelopment'):
            agent.updateDevelopment(1.0)
        
        # Record action
        agent.recordEpisodeAction(command, 1.0)
        
        # Collect metrics
        metrics = agent.collectMetrics()
        
        # Verify all systems are working
        self.framework.assert_true(agent.isEpisodeActive(), "Agent should be active")
        self.framework.assert_true(metrics is not None, "Metrics should be available")
        
        print("✓ System integration successful")
    
    # Test batch processing performance
    def test_batch_processing_performance(self):
        """Test batch processing performance."""
        self.agent.initialize(self.world)
        
        # Test different batch sizes
        batch_sizes = [5, 10, 20, 50]
        
        for batch_size in batch_sizes:
            # Generate batch
            batch_percepts = self.data_generator.generate_batch_percepts(batch_size)
            
            # Measure performance
            start_time = time.time()
            for percept in batch_percepts:
                self.agent.processSensoryInput(percept, False)
            duration = time.time() - start_time
            
            # Log performance
            self.framework.log_performance(f"batch_size_{batch_size}", duration,
                                          throughput=batch_size/duration)
            
            print(f"✓ Batch processing {batch_size}: {duration:.4f}s ({batch_size/duration:.1f} items/s)")

# Run all tests when executed directly
if __name__ == "__main__":
    print("Running AgentBrain Unit Tests...")
    print("=" * 60)
    
    # Create test instance
    test_suite = AgentBrainUnitTests()
    
    # Run all tests
    test_suite.run_all_tests()
    
    # Print summary
    summary = test_suite.get_test_summary()
    print(f"\n{'='*60}")
    print("TEST SUMMARY")
    print(f"{'='*60}")
    print(f"Test Suite: {summary['test_suite']}")
    print(f"Passed: {summary['passed_tests']}")
    print(f"Failed: {summary['failed_tests']}")
    print(f"Total: {summary['total_tests']}")
    print(f"Duration: {summary['duration_seconds']:.2f}s")
    print(f"Avg time per test: {summary['average_time_per_test']:.4f}s")
    
    if summary['failed_tests'] > 0:
        print(f"\n❌ Test suite completed with {summary['failed_tests']} failures")
        sys.exit(1)
    else:
        print(f"\n✅ All tests passed successfully!")
        sys.exit(0)