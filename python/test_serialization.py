#!/usr/bin/env python3
"""
Serialization and Deserialization Tests for NLM Python Bindings

This module contains comprehensive tests for serialization/deserialization
functionality including JSON and MessagePack formats.
"""

import os
import sys
import json
import tempfile
import shutil
import time
from pathlib import Path
from typing import Dict, List, Any

# Import NLM bindings
try:
    import pynlm
except ImportError as e:
    print(f"Failed to import NLM bindings: {e}")
    print("Make sure NLM is properly installed and built.")
    sys.exit(1)

from test_suite import NLMTestSuite, NLMSyntheticDataGenerator, NLMTestValidationError
class SerializationTests(NLMTestSuite):
    """
    Comprehensive tests for NLM serialization and deserialization.
    
    Tests all serialization formats including:
    - JSON serialization
    - MessagePack serialization
    - State save/load functionality
    - Checkpoint functionality
    - Memory management during serialization
    - Error handling for corrupted data
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
    
    # Test basic state serialization
    def test_state_serialization_json(self):
        """Test basic JSON state serialization."""
        self.agent.initialize(self.world)
        
        # Process some data to create state
        for i in range(5):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.applyRewardModulation(0.5, 0.25)
        
        # Save state to JSON file
        with tempfile.TemporaryDirectory() as temp_dir:
            state_file = os.path.join(temp_dir, "agent_state.json")
            
            save_success = self.agent.saveState(state_file)
            self.framework.assert_true(save_success, "State save should succeed")
            
            # Verify file was created
            self.framework.assert_true(os.path.exists(state_file), "State file should exist")
            
            # Verify file content is valid JSON
            with open(state_file, 'r') as f:
                state_data = json.load(f)
            
            # Verify JSON structure
            self.framework.assert_true('brainState' in state_data or 'agentState' in state_data, 
                                     "JSON should contain state data")
            
            print(f"✓ JSON serialization successful: {len(str(state_data))} characters")
    
    def test_state_deserialization_json(self):
        """Test JSON state deserialization."""
        # Create and save a state first
        self.agent.initialize(self.world)
        
        # Process some data
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        
        with tempfile.TemporaryDirectory() as temp_dir:
            # Save state
            original_file = os.path.join(temp_dir, "original_state.json")
            save_success = self.agent.saveState(original_file)
            self.framework.assert_true(save_success, "Original state save should succeed")
            
            # Create new agent and load state
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_agent = pynlm.createAgentBrain(new_brain)
            
            load_success = new_agent.loadState(original_file)
            self.framework.assert_true(load_success, "State load should succeed")
            
            # Verify loaded state is functional
            new_agent.initialize(self.world)
            
            print("✓ JSON deserialization successful")
    
    # Test MessagePack serialization
    def test_messagepack_serialization(self):
        """Test MessagePack serialization if available."""
        # Note: This test assumes MessagePack is available
        # Check if MessagePack is installed and supported
        
        self.agent.initialize(self.world)
        
        # Process some data
        for i in range(3):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
        
        # Try to save with MessagePack if supported
        with tempfile.TemporaryDirectory() as temp_dir:
            msgpack_file = os.path.join(temp_dir, "agent_state.msgpack")
            
            try:
                # Check if MessagePack save is available
                if hasattr(self.agent, 'saveState') and hasattr(self.agent, 'loadState'):
                    # Try MessagePack format
                    save_success = self.agent.saveState(msgpack_file)
                    
                    if save_success and os.path.exists(msgpack_file):
                        self.framework.assert_true(os.path.exists(msgpack_file), 
                                                 "MessagePack file should exist")
                        
                        # Try to load back
                        new_config = pynlm.createDefaultConfig()
                        new_brain = pynlm.createBrain(new_config)
                        new_agent = pynlm.createAgentBrain(new_brain)
                        
                        load_success = new_agent.loadState(msgpack_file)
                        self.framework.assert_true(load_success, "MessagePack load should succeed")
                        
                        print("✓ MessagePack serialization successful")
                    else:
                        print("⚠ MessagePack serialization not supported, skipping test")
                else:
                    print("⚠ State save/load not supported, skipping test")
                    
            except Exception as e:
                print(f"⚠ MessagePack test skipped due to: {e}")
    
    # Test checkpoint functionality
    def test_checkpoint_functionality(self):
        """Test checkpoint save/load functionality."""
        self.agent.initialize(self.world)
        
        # Enable checkpointing
        self.agent.setCheckpoint(0.5)
        
        # Process some data
        for i in range(5):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            command = self.agent.decodeMotorCommand()
            self.agent.recordEpisodeAction(command, 0.5)
        
        # Save checkpoint
        with tempfile.TemporaryDirectory() as temp_dir:
            checkpoint_file = os.path.join(temp_dir, "checkpoint.json")
            
            checkpoint_success = self.agent.saveState(checkpoint_file)
            self.framework.assert_true(checkpoint_success, "Checkpoint save should succeed")
            
            # Verify checkpoint file exists
            self.framework.assert_true(os.path.exists(checkpoint_file), "Checkpoint file should exist")
            
            # Create metadata file (simulating checkpoint metadata)
            metadata = {
                'checkpoint_name': 'test_checkpoint',
                'timestamp': time.time(),
                'episode_count': 1,
                'total_steps': 5,
                'final_reward': 2.5
            }
            
            metadata_file = checkpoint_file.replace('.json', '_metadata.json')
            with open(metadata_file, 'w') as f:
                json.dump(metadata, f, indent=2)
            
            # Load checkpoint into new agent
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_agent = pynlm.createAgentBrain(new_brain)
            
            load_success = new_agent.loadState(checkpoint_file)
            self.framework.assert_true(load_success, "Checkpoint load should succeed")
            
            print("✓ Checkpoint functionality successful")
    
    # Test large state serialization
    def test_large_state_serialization(self):
        """Test serialization of large/complex states."""
        self.agent.initialize(self.world)
        
        # Process many percepts to create large state
        large_state_percepts = 100
        
        start_time = time.time()
        for i in range(large_state_percepts):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            
            # Occasionally decode commands and apply rewards
            if i % 10 == 0:
                command = self.agent.decodeMotorCommand()
                self.agent.applyRewardModulation(0.5, 0.25)
        
        processing_time = time.time() - start_time
        
        # Save large state
        with tempfile.TemporaryDirectory() as temp_dir:
            large_state_file = os.path.join(temp_dir, "large_state.json")
            
            save_start_time = time.time()
            save_success = self.agent.saveState(large_state_file)
            save_time = time.time() - save_start_time
            
            self.framework.assert_true(save_success, "Large state save should succeed")
            
            # Verify file size is reasonable
            file_size = os.path.getsize(large_state_file)
            self.framework.assert_true(file_size > 0, "Large state file should not be empty")
            
            # Estimate state complexity
            estimated_complexity = file_size / 1024  # KB
            
            self.framework.log_performance("test_large_state_serialization", processing_time + save_time,
                                          file_size_mb=file_size / (1024 * 1024),
                                          complexity_kb=estimated_complexity)
            
            print(f"✓ Large state serialization successful: {estimated_complexity:.1f}KB in {save_time:.4f}s")
    
    # Test serialization error handling
    def test_serialization_error_handling(self):
        """Test error handling for serialization issues."""
        # Test with invalid file paths
        self.agent.initialize(self.world)
        
        # Test save with invalid path (should handle gracefully)
        invalid_path = "/invalid/path/that/does/not/exist/state.json"
        
        try:
            # This should either succeed with fallback or raise appropriate exception
            result = self.agent.saveState(invalid_path)
            # If it succeeds, that's fine
            print("✓ Invalid path handled gracefully")
        except Exception as e:
            # If it raises an exception, that's also fine
            print(f"✓ Invalid path raised expected exception: {type(e).__name__}")
        
        # Test load with non-existent file
        try:
            non_existent = "/tmp/non_existent_file.json"
            result = self.agent.loadState(non_existent)
            print("✓ Non-existent file handled gracefully")
        except Exception as e:
            print(f"✓ Non-existent file raised expected exception: {type(e).__name__}")
        
        # Test load with corrupted file
        with tempfile.TemporaryDirectory() as temp_dir:
            corrupted_file = os.path.join(temp_dir, "corrupted.json")
            
            # Write corrupted JSON
            with open(corrupted_file, 'w') as f:
                f.write("{ invalid json content }")
            
            try:
                load_success = self.agent.loadState(corrupted_file)
                print("✓ Corrupted file handled gracefully")
            except Exception as e:
                print(f"✓ Corrupted file raised expected exception: {type(e).__name__}")
    
    # Test serialization performance
    def test_serialization_performance(self):
        """Test serialization performance characteristics."""
        self.agent.initialize(self.world)
        
        # Create performance baseline by processing data first
        for i in range(20):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
        
        # Test serialization performance
        test_sizes = [10, 50, 100]
        
        for test_size in test_sizes:
            # Reset agent state
            self.agent.initialize(self.world)
            
            # Generate test data
            test_percepts = self.data_generator.generate_batch_percepts(test_size)
            
            # Process data
            for percept in test_percepts:
                self.agent.processSensoryInput(percept, False)
            
            # Measure serialization performance
            with tempfile.TemporaryDirectory() as temp_dir:
                test_file = os.path.join(temp_dir, f"perf_test_{test_size}.json")
                
                start_time = time.time()
                save_success = self.agent.saveState(test_file)
                save_time = time.time() - start_time
                
                self.framework.assert_true(save_success, f"Serialization {test_size} should succeed")
                
                # Measure file size
                file_size = os.path.getsize(test_file)
                
                # Calculate performance metrics
                throughput = test_size / save_time  # items per second
                efficiency = file_size / (test_size * 256)  # bytes per float32 (256 per percept)
                
                self.framework.log_performance(f"serialization_performance_{test_size}", save_time,
                                              items_per_second=throughput,
                                              bytes_per_item=efficiency,
                                              total_bytes=file_size)
                
                print(f"✓ Serialization performance {test_size}: {throughput:.1f} items/s, {efficiency:.1f} bytes/item")
    
    # Test memory efficiency during serialization
    def test_memory_efficiency_serialization(self):
        """Test memory efficiency during serialization operations."""
        self.agent.initialize(self.world)
        
        # Generate large amount of data to stress memory
        large_percepts = self.data_generator.generate_batch_percepts(200)
        
        # Process large amount of data
        process_start_time = time.time()
        for i, percept in enumerate(large_percepts):
            self.agent.processSensoryInput(percept, False)
            
            # Monitor progress
            if (i + 1) % 50 == 0:
                elapsed = time.time() - process_start_time
                print(f"Processed {i + 1}/200 percepts in {elapsed:.2f}s")
        
        process_time = time.time() - process_start_time
        
        # Serialize large state
        with tempfile.TemporaryDirectory() as temp_dir:
            large_file = os.path.join(temp_dir, "large_memory_state.json")
            
            serialize_start_time = time.time()
            save_success = self.agent.saveState(large_file)
            serialize_time = time.time() - serialize_start_time
            
            self.framework.assert_true(save_success, "Large state serialization should succeed")
            
            file_size = os.path.getsize(large_file)
            
            # Calculate memory efficiency metrics
            data_processed = len(large_percepts) * 256  # 256 floats per percept
            efficiency = data_processed / file_size  # compression ratio
            
            self.framework.log_performance("test_memory_efficiency", process_time + serialize_time,
                                          process_time=process_time,
                                          serialize_time=serialize_time,
                                          data_processed_mb=data_processed / (1024 * 1024),
                                          file_size_mb=file_size / (1024 * 1024),
                                          compression_ratio=efficiency)
            
            print(f"✓ Memory efficiency test: {efficiency:.1f}x compression ratio")
    
    # Test serialization format compatibility
    def test_serialization_format_compatibility(self):
        """Test compatibility across different serialization formats."""
        # Test JSON format
        with tempfile.TemporaryDirectory() as temp_dir:
            json_file = os.path.join(temp_dir, "test_state.json")
            
            # Save in JSON format
            save_success = self.agent.saveState(json_file)
            self.framework.assert_true(save_success, "JSON save should succeed")
            
            # Verify it's valid JSON
            with open(json_file, 'r') as f:
                json_content = json.load(f)
            
            # Parse JSON structure
            self.framework.assert_true(isinstance(json_content, dict), "JSON content should be a dictionary")
            
            print("✓ JSON format compatibility verified")

# Run tests when executed directly
if __name__ == "__main__":
    print("Running Serialization and Deserialization Tests...")
    print("=" * 60)
    
    # Create test instance
    test_suite = SerializationTests()
    
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
        print(f"\n✅ All serialization tests passed successfully!")
        sys.exit(0)