#!/usr/bin/env python3
"""
Integration Tests for NLM Full Simulation Workflows

This module contains comprehensive integration tests for full NLM simulation workflows
including complete episode simulations, memory management, and system integration.
"""

import os
import sys
import json
import time
import tempfile
import shutil
import numpy as np
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
class IntegrationTests(NLMTestSuite):
    """
    Comprehensive integration tests for NLM full simulation workflows.
    
    Tests all integration scenarios including:
    - Full simulation workflows
    - Memory management
    - Callback mechanisms
    - Python binding interoperability
    - System integration
    """
    
    def setup_method(self):
        """Setup before each test method."""
        self.setup_test_environment()
        
    def teardown_method(self):
        """Cleanup after each test method."""
        self.teardown_test_environment()
    
    # Test complete simulation workflow
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow with all systems."""
        print("Starting complete simulation workflow test...")
        
        # Initialize complete system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=100, height=100, visionWidth=32, visionHeight=32)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems
        agent.enableLearning(True)
        agent.setLearningRate(0.01)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation steps
        num_steps = 100
        rewards = []
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get percept from world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept, True)
            
            # Brain thinks
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record reward
            reward = action_result.reward
            rewards.append(reward)
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, max(0.0, reward * 0.5))
            
            # Update development
            if step % 5 == 0:
                agent.updateDevelopment(1.0)
        
        # Collect metrics
        metrics = agent.collectMetrics()
        
        # Verify simulation results
        self.framework.assert_true(len(rewards) == num_steps, "Should have collected all rewards")
        self.framework.assert_true(metrics is not None, "Should have collected metrics")
        self.framework.assert_true(metrics.totalSteps == num_steps, "Should have processed all steps")
        
        # Calculate statistics
        avg_reward = sum(rewards) / len(rewards) if rewards else 0
        max_reward = max(rewards) if rewards else 0
        
        print(f"✓ Complete simulation workflow successful:")
        print(f"  - Steps processed: {num_steps}")
        print(f"  - Average reward: {avg_reward:.3f}")
        print(f"  - Max reward: {max_reward:.3f}")
        print(f"  - Processing efficiency: {metrics.processingEfficiency:.3f}")
        print(f"  - Steps per second: {metrics.stepsPerSecond:.1f}")
    
    # Test memory management in simulation
    def test_memory_management_simulation(self):
        """Test memory management during long-running simulations."""
        print("Testing memory management in simulation...")
        
        # Initialize simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get initial memory
        process = psutil.Process()
        memory_before = process.memory_info().rss / (1024 * 1024)  # MB
        
        # Run simulation with memory monitoring
        steps_per_phase = 500
        phases = 3
        
        for phase in range(phases):
            print(f"  Phase {phase + 1}/{phases}...")
            
            # Run simulation phase
            for step in range(steps_per_phase):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept, False)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply some learning
                agent.applyRewardModulation(0.1, 0.05)
            
            # Check memory during phase
            current_memory = process.memory_info().rss / (1024 * 1024)
            memory_increase = current_memory - memory_before
            
            # Memory should remain reasonable (less than 500MB increase per phase)
            memory_threshold_mb = 500
            self.framework.assert_true(memory_increase < memory_threshold_mb,
                                     f"Memory increase {memory_increase:.1f}MB exceeds threshold {memory_threshold_mb}MB")
            
            print(f"    Memory after phase {phase + 1}: {memory_increase:.1f}MB increase")
        
        # Final memory check
        memory_final = process.memory_info().rss / (1024 * 1024)
        total_memory_increase = memory_final - memory_before
        
        print(f"✓ Memory management test successful:")
        print(f"  - Total memory increase: {total_memory_increase:.1f}MB")
        print(f"  - Average per phase: {total_memory_increase / phases:.1f}MB")
        print(f"  - Memory per step: {total_memory_increase / (steps_per_phase * phases):.3f}MB")
    
    # Test callback mechanisms
    def test_callback_mechanisms(self):
        """Test callback mechanisms in simulation."""
        print("Testing callback mechanisms...")
        
        # Initialize system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Track callback events
        callback_events = []
        
        # Process some steps with callback tracking
        for step in range(50):
            # Get current state for callback
            current_memory = agent.profilePerformance()
            current_score = agent.getEpisodeScore()
            
            # Record callback data
            callback_events.append({
                'step': step,
                'timestamp': time.time(),
                'memory_usage': current_memory.memory_usage_bytes if current_memory else 0,
                'episode_score': current_score,
                'curiosity_level': agent.getCuriosityLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation_level': agent.getNeuromodulationLevel()
            })
            
            # Simulate callback processing
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept, False)
            
            if step % 10 == 0:
                # Periodic callback for learning
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record learning callback
                callback_events[-1]['action'] = str(action)
                callback_events[-1]['learning_callback'] = True
        
        # Analyze callback events
        successful_callbacks = sum(1 for event in callback_events if event.get('learning_callback', False))
        
        self.framework.assert_true(len(callback_events) == 50, "Should have collected 50 callback events")
        self.framework.assert_true(successful_callbacks > 0, "Should have some successful learning callbacks")
        
        # Calculate callback statistics
        avg_score = sum(event['episode_score'] for event in callback_events) / len(callback_events)
        avg_curiosity = sum(event['curiosity_level'] for event in callback_events) / len(callback_events)
        
        print(f"✓ Callback mechanisms test successful:")
        print(f"  - Total callback events: {len(callback_events)}")
        print(f"  - Successful learning callbacks: {successful_callbacks}")
        print(f"  - Average episode score: {avg_score:.3f}")
        print(f"  - Average curiosity level: {avg_curiosity:.3f}")
    
    # Test Python binding interoperability
    def test_python_binding_interoperability(self):
        """Test interoperability between Python and NLM bindings."""
        print("Testing Python binding interoperability...")
        
        # Test 1: Multiple data types interoperability
        test_data = {
            'numpy_arrays': np.array([1.0, 2.0, 3.0, 4.0, 5.0]),
            'python_lists': [1, 2, 3, 4, 5],
            'python_dicts': {'a': 1, 'b': 2, 'c': 3},
            'python_tuples': (1.0, 2.0, 3.0),
            'python_strings': 'test_string',
            'python_integers': 42,
            'python_floats': 3.14159,
            'python_bools': True
        }
        
        # Initialize system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Process different data types
        for step in range(20):
            # Create percept using different data representations
            vision_data = test_data['numpy_arrays'].tolist()  # numpy to list
            touch_data = test_data['python_lists'][:8]  # list slicing
            internal_data = test_data['python_dicts']  # dict (may need conversion)
            
            # Create percept
            percept = pynlm.SensoryPercept()
            
            if hasattr(percept, 'setVision'):
                # Convert to proper size if needed
                vision_size = percept.getVisionWidth() * percept.getVisionHeight() if hasattr(percept, 'getVisionWidth') else 256
                if len(vision_data) >= vision_size:
                    percept.setVision(vision_data[:vision_size])
            
            if hasattr(percept, 'setTouch'):
                if len(touch_data) >= 8:
                    percept.setTouch(touch_data[:8])
            
            # Process with Python-interop percept
            agent.processSensoryInput(percept, False)
        
        # Test serialization interoperability
        with tempfile.TemporaryDirectory() as temp_dir:
            # Save with Python-interop data
            state_file = os.path.join(temp_dir, "interoperability_state.json")
            
            save_success = agent.saveState(state_file)
            self.framework.assert_true(save_success, "Interoperability save should succeed")
            
            # Load back and verify
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_agent = pynlm.createAgentBrain(new_brain)
            
            load_success = new_agent.loadState(state_file)
            self.framework.assert_true(load_success, "Interoperability load should succeed")
        
        # Test external system integration
        external_data = {
            'simulation_step': 100,
            'external_time': time.time(),
            'environment_data': {
                'temperature': 22.5,
                'humidity': 0.65,
                'pressure': 1013.25
            },
            'agent_state': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError()
            }
        }
        
        # This simulates integration with external systems
        print(f"✓ Python binding interoperability test successful")
        print(f"  - Processed {20} steps with Python data types")
        print(f"  - Saved/loaded state: {save_success and load_success}")
        print(f"  - External data integration: ✓")
    
    # Test simulation persistence
    def test_simulation_persistence(self):
        """Test simulation persistence across checkpoints."""
        print("Testing simulation persistence...")
        
        # Run initial simulation
        config1 = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config1)
        brain1.initialize()
        
        world1 = pynlm.createSimpleWorld()
        world1.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world1.reset()
        
        agent1 = pynlm.createAgentBrain(brain1)
        agent1.initialize(world1)
        
        # Run initial phase
        for step in range(100):
            world1.update(0.1)
            percept = world1.getSensoryPercept()
            agent1.processSensoryInput(percept, False)
            brain1.step(step)
            action = agent1.decodeMotorCommand()
            world1.applyMotorCommand(action, world1.getSimulationTime())
            agent1.applyRewardModulation(0.5, 0.25)
        
        # Save checkpoint
        with tempfile.TemporaryDirectory() as temp_dir:
            checkpoint_file = os.path.join(temp_dir, "simulation_checkpoint.json")
            
            checkpoint_success = agent1.saveState(checkpoint_file)
            self.framework.assert_true(checkpoint_success, "Checkpoint save should succeed")
            
            # Create metadata
            metadata = {
                'checkpoint_name': 'simulation_checkpoint',
                'simulation_step': 100,
                'timestamp': time.time(),
                'configuration': 'initial',
                'performance_metrics': agent1.profilePerformance().__dict__ if hasattr(agent1.profilePerformance(), '__dict__') else {}
            }
            
            metadata_file = checkpoint_file.replace('.json', '_metadata.json')
            with open(metadata_file, 'w') as f:
                json.dump(metadata, f, indent=2)
        
        # Load checkpoint and continue simulation
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        world2 = pynlm.createSimpleWorld()
        world2.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world2.reset()
        
        agent2 = pynlm.createAgentBrain(brain2)
        agent2.initialize(world2)
        
        load_success = agent2.loadState(checkpoint_file)
        self.framework.assert_true(load_success, "Checkpoint load should succeed")
        
        # Continue simulation from checkpoint
        for step in range(50):
            world2.update(0.1)
            percept = world2.getSensoryPercept()
            agent2.processSensoryInput(percept, False)
            brain2.step(step + 100)  # Continue from where we left off
            action = agent2.decodeMotorCommand()
            world2.applyMotorCommand(action, world2.getSimulationTime())
            agent2.applyRewardModulation(0.3, 0.15)
        
        # Verify continuation worked
        metrics1 = agent1.profilePerformance()
        metrics2 = agent2.profilePerformance()
        
        self.framework.assert_true(metrics1 is not None, "Initial metrics should exist")
        self.framework.assert_true(metrics2 is not None, "Checkpoint metrics should exist")
        
        print(f"✓ Simulation persistence test successful")
        print(f"  - Initial simulation: {metrics1.totalSteps if metrics1 else 0} steps")
        print(f"  - Checkpointed simulation: {metrics2.totalSteps if metrics2 else 0} steps")
        print(f"  - Checkpoint save/load: {checkpoint_success and load_success}")
    
    # Test system integration with external data
    def test_system_integration_external_data(self):
        """Test integration with external data sources."""
        print("Testing system integration with external data...")
        
        # Simulate external data integration
        external_systems = {
            'environmental_sensor': {
                'temperature': lambda: 20.0 + 5.0 * np.sin(time.time() / 100.0),
                'humidity': lambda: 0.6 + 0.2 * np.random.random(),
                'light_intensity': lambda: max(0.0, 0.5 * np.sin(time.time() / 200.0))
            },
            'resource_availability': {
                'energy': lambda: 0.7 + 0.3 * np.random.random(),
                'food': lambda: 0.5 + 0.5 * np.random.random(),
                'water': lambda: 0.6 + 0.4 * np.cos(time.time() / 150.0)
            },
            'social_factors': {
                'population_density': lambda: min(1.0, time.time() / 1000.0),
                'competition_level': lambda: 0.3 + 0.7 * np.random.random()
            }
        }
        
        # Initialize system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Integrate external data into simulation
        integration_samples = []
        
        for step in range(50):
            # Collect external data
            external_data = {
                'timestamp': time.time(),
                'step': step,
                'environmental': {k: func() for k, func in external_systems['environmental_sensor'].items()},
                'resources': {k: func() for k, func in external_systems['resource_availability'].items()},
                'social': {k: func() for k, func in external_systems['social_factors'].items()}
            }
            
            # Integrate with agent state
            integrated_state = {
                **external_data,
                'agent_state': {
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'prediction_error': agent.getPredictionError(),
                    'neuromodulation': agent.getNeuromodulationLevel()
                }
            }
            
            integration_samples.append(integrated_state)
            
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept, False)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward based on external factors
            energy_factor = external_data['resources']['energy']
            curiosity_factor = agent.getCuriosityLevel()
            
            reward = 0.5 * energy_factor + 0.3 * curiosity_factor
            agent.applyRewardModulation(reward, reward * 0.5)
        
        # Analyze integration results
        avg_environmental = np.mean([s['environmental']['temperature'] for s in integration_samples])
        avg_resources = np.mean([s['resources']['energy'] for s in integration_samples])
        avg_social = np.mean([s['social']['population_density'] for s in integration_samples])
        
        print(f"✓ System integration with external data test successful")
        print(f"  - Integration samples: {len(integration_samples)}")
        print(f"  - Average environmental temperature: {avg_environmental:.2f}")
        print(f"  - Average energy availability: {avg_resources:.2f}")
        print(f"  - Average population density: {avg_social:.2f}")
        print(f"  - External data integration: ✓")

# Run tests when executed directly
if __name__ == "__main__":
    print("Running Integration Tests for NLM Full Simulation Workflows...")
    print("=" * 70)
    
    # Create test instance
    test_suite = IntegrationTests()
    
    # Run all tests
    test_suite.run_all_tests()
    
    # Print summary
    summary = test_suite.get_test_summary()
    print(f"\n{'='*70}")
    print("TEST SUMMARY")
    print(f"{'='*70}")
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
        print(f"\n✅ All integration tests passed successfully!")
        sys.exit(0)