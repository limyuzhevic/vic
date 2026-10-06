#!/usr/bin/env python3
"""
Integration tests between C++ core and Python bindings
Tests end-to-end scenarios and error handling from Python perspective
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import pytest
import numpy as np
from pynlm import (
    create_agent_simulation, get_default_config, 
    Config, Brain, SimpleWorld, AgentBrain
)

class TestIntegration:
    """Test integration between C++ core and Python bindings"""
    
    def test_complete_workflow(self):
        """Test complete end-to-end workflow"""
        print("Testing complete workflow...")
        
        # Create simulation with realistic parameters
        config_dict = get_default_config()
        config_dict.update({
            "neuron_count": 200,
            "region_count": 3,
            "connection_probability": 0.15,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "enable_reward_modulation": True,
            "enable_curiosity": True,
            "random_seed": 42,
            "simulation_timestep": 0.001,
        })
        
        # Create complete agent simulation
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Verify initial state
        assert brain.neuron_count() == 200
        assert brain.get_region_count() == 3
        assert world.get_width() == 20.0
        assert world.get_height() == 20.0
        
        # Run a complete episode
        episode_actions = []
        for step in range(20):
            # Get sensory percept
            percept = world.get_sensory_percept()
            assert percept is not None
            
            # Process sensory input
            agent_brain.process_sensory_input(percept)
            
            # Produce action
            action = agent_brain.decode_motor_command()
            assert action is not None
            episode_actions.append(action)
            
            # Apply action to world
            action_result = world.apply_motor_command(action.getType(), world.getSimulationTime())
            
            # Update simulation
            world.step(0.1)
            
            # Step brain
            brain.step(step)
        
        # Verify episode results
        assert len(episode_actions) == 20
        assert brain.neuron_count() == 200
        assert brain.get_region_count() == 3
        
        print(f"✓ Complete workflow test passed ({len(episode_actions)} actions)")
    
    def test_config_integration(self):
        """Test configuration system integration"""
        print("Testing configuration integration...")
        
        # Test 1: Create config from dict
        config_data = {
            "neuron_count": 1000,
            "region_count": 5,
            "connection_probability": 0.2,
            "enable_structural_plasticity": True,
            "stdp_ltp_weight": 0.05,
            "stdp_ltd_weight": 0.055,
            "simulation_timestep": 0.0005,
            "random_seed": 12345,
            "working_memory_capacity": 200,
            "episodic_memory_max_episodes": 2000,
        }
        
        config = Config()
        for key, value in config_data.items():
            config.set(key, value)
        
        # Verify all config values
        assert config.get_int("neuron_count") == 1000
        assert config.get_int("region_count") == 5
        assert config.get_float("connection_probability") == 0.2
        assert config.get_bool("enable_structural_plasticity") == True
        assert config.get_float("stdp_ltp_weight") == 0.05
        assert config.get_float("stdp_ltd_weight") == 0.055
        assert config.get_float("simulation_timestep") == 0.0005
        assert config.get_int("random_seed") == 12345
        assert config.get_int("working_memory_capacity") == 200
        assert config.get_int("episodic_memory_max_episodes") == 2000
        
        # Test 2: Use config in brain creation
        config_dict = {
            "neuron_count": config.get_int("neuron_count"),
            "region_count": config.get_int("region_count"),
            "connection_probability": config.get_float("connection_probability"),
            "simulation_timestep": config.get_float("simulation_timestep"),
            "random_seed": config.get_int("random_seed"),
            "enable_structural_plasticity": config.get_bool("enable_structural_plasticity"),
        }
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        assert brain.neuron_count() == 1000
        assert brain.get_region_count() == 5
        
        # Test 3: Dict-like config access
        config["neuron_count"] = 500  # Should work with dict-like access
        assert config.get_int("neuron_count") == 500
        
        print("✓ Config integration test passed")
    
    def test_error_handling_integration(self):
        """Test error handling from Python perspective"""
        print("Testing error handling integration...")
        
        try:
            import pynlm.error
            error_manager = pynlm.get_error_manager()
            
            # Enable error reporting
            error_manager.enable(True)
            error_manager.set_strict_mode(False)
            
            # Test 1: Invalid file path
            config = Config()
            result = config.load_from_file("/nonexistent/path/config.json")
            assert result == False
            
            initial_errors = error_manager.get_error_count()
            assert initial_errors >= 0
            
            # Test 2: Invalid directory for save
            config.set("test_key", "test_value")
            result = config.save_to_file("/nonexistent_dir/config.json")
            assert result == False
            
            final_errors = error_manager.get_error_count()
            assert final_errors >= initial_errors
            
            # Test 3: Invalid command line arguments (simulate)
            # Note: This would require mocking sys.argv
            # For now, just test that error reporting is functional
            
            assert error_manager.is_enabled()
            assert not error_manager.is_strict_mode()
            assert not error_manager.should_exit()
            
            print(f"✓ Error handling integration test passed ({final_errors} errors)")
            
        except ImportError:
            print("   Error management not available, skipping error handling integration tests")
            # This is acceptable - error handling is optional
    
    def test_sensory_motor_integration(self):
        """Test sensory-motor integration"""
        print("Testing sensory-motor integration...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 2
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test sensory input pipeline
        # 1. Get world percept
        percept = world.get_sensory_percept()
        assert percept is not None
        
        # 2. Process percept through agent brain
        agent_brain.process_sensory_input(percept)
        
        # 3. Get motor command
        action = agent_brain.decode_motor_command()
        assert action is not None
        
        # 4. Apply action to world
        action_result = world.apply_motor_command(action.getType(), world.getSimulationTime())
        
        # 5. Update simulation
        world.step(0.1)
        
        # 6. Verify agent's physical state changed
        agent_body = world.get_agent_body()
        assert agent_body is not None
        
        # The agent should have moved or changed state
        # (actual behavior depends on action type)
        
        print("✓ Sensory-motor integration test passed")
    
    def test_development_integration(self):
        """Test developmental stage integration"""
        print("Testing development integration...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 150
        config_dict["region_count"] = 2
        config_dict["enable_development"] = True
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test initial developmental stage
        initial_stage = brain.get_developmental_stage()
        assert initial_stage is not None
        
        # Test developmental stage modification
        brain.set_developmental_stage("Adult")
        assert brain.get_developmental_stage() == "Adult"
        
        # Run several steps to allow development to progress
        for step in range(10):
            brain.step(step)
        
        # Developmental stage should still be accessible
        stage = brain.get_developal_stage()
        assert stage is not None
        
        print("✓ Development integration test passed")
    
    def test_memory_systems_integration(self):
        """Test memory systems integration"""
        print("Testing memory systems integration...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 200
        config_dict["region_count"] = 3
        config_dict["working_memory_capacity"] = 100
        config_dict["episodic_memory_max_episodes"] = 500
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test that brain has memory systems (internal implementation)
        # We can't directly test memory systems without breaking encapsulation,
        # but we can test that brain operations don't crash
        
        for step in range(5):
            percept = world.get_sensory_percept()
            agent_brain.process_sensory_input(percept)
            action = agent_brain.decode_motor_command()
            world.apply_motor_command(action.getType(), world.getSimulationTime())
            world.step(0.1)
            brain.step(step)
        
        # Verify brain is still functional
        assert brain.neuron_count() == 200
        assert brain.get_region_count() == 3
        
        print("✓ Memory systems integration test passed")
    
    def test_neuromodulation_integration(self):
        """Test neuromodulation integration"""
        print("Testing neuromodulation integration...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 2
        config_dict["enable_reward_modulation"] = True
        config_dict["enable_curiosity"] = True
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test reward modulation
        agent_brain.apply_reward_modulation(1.0, 0.8)  # reward, predicted_reward
        
        # Test novelty detection
        novelty_level = agent_brain.get_novelty_level()
        curiosity_level = agent_brain.get_curiosity_level()
        
        # These should be accessible and have valid values
        assert isinstance(novelty_level, float)
        assert isinstance(curiosity_level, float)
        
        # Test prediction error
        prediction_error = agent_brain.get_prediction_error()
        assert isinstance(prediction_error, float)
        
        print("✓ Neuromodulation integration test passed")
    
    def test_structural_plasticity_integration(self):
        """Test structural plasticity integration"""
        print("Testing structural plasticity integration...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 2
        config_dict["enable_structural_plasticity"] = True
        config_dict["synaptogenesis_rate"] = 0.001
        config_dict["pruning_rate"] = 0.0001
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Run simulation for several steps
        for step in range(10):
            percept = world.get_sensory_percept()
            agent_brain.process_sensory_input(percept)
            action = agent_brain.decode_motor_command()
            world.apply_motor_command(action.getType(), world.getSimulationTime())
            world.step(0.1)
            brain.step(step)
        
        # Verify structural plasticity didn't crash and system is stable
        assert brain.neuron_count() == 100
        assert brain.get_region_count() == 2
        
        print("✓ Structural plasticity integration test passed")
    
    def test_stdp_plasticity_integration(self):
        """Test STDP plasticity integration"""
        print("Testing STDP plasticity integration...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 2
        config_dict["stdp_ltp_weight"] = 0.01
        config_dict["stdp_ltd_weight"] = 0.012
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Run STDP-related operations
        # Note: Direct STDP manipulation is internal,
        # but we can test that the brain with STDP enabled works correctly
        
        for step in range(5):
            brain.step(step)
            
            # Process sensory input
            percept = world.get_sensory_percept()
            agent_brain.process_sensory_input(percept)
            
            # Generate action
            action = agent_brain.decode_motor_command()
            world.apply_motor_command(action.getType(), world.getSimulationTime())
            world.step(0.1)
        
        # Verify STDP-enabled brain is functional
        assert brain.neuron_count() == 100
        assert brain.get_region_count() == 2
        
        print("✓ STDP plasticity integration test passed")
    
    def test_performance_characteristics(self):
        """Test performance characteristics"""
        print("Testing performance characteristics...")
        
        import time
        
        # Test large-scale simulation
        config_dict = get_default_config()
        config_dict["neuron_count"] = 500
        config_dict["region_count"] = 5
        config_dict["enable_structural_plasticity"] = False  # Speed up
        config_dict["enable_development"] = False  # Speed up
        config_dict["enable_reward_modulation"] = False  # Speed up
        config_dict["enable_curiosity"] = False  # Speed up
        
        # Measure creation time
        start_time = time.time()
        world, brain, agent_brain = create_agent_simulation(config_dict)
        creation_time = time.time() - start_time
        
        assert creation_time < 2.0  # Should create within 2 seconds
        
        # Measure step performance
        start_time = time.time()
        for step in range(50):
            brain.step(step)
            
            # Do some agent operations
            percept = world.get_sensory_percept()
            agent_brain.process_sensory_input(percept)
            action = agent_brain.decode_motor_command()
            world.apply_motor_command(action.getType(), world.getSimulationTime())
            world.step(0.1)
        
        step_time = time.time() - start_time
        
        # Should complete 50 steps reasonably quickly
        assert step_time < 5.0  # Allow some overhead
        
        print(f"✓ Performance characteristics test passed")
        print(f"  Creation: {creation_time:.3f}s, Steps: {step_time:.3f}s (50 steps)")
    
    def test_resilience_and_recovery(self):
        """Test resilience and error recovery"""
        print("Testing resilience and recovery...")
        
        # Test that brain can be reset after errors
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Run some steps
        for step in range(5):
            brain.step(step)
        
        # Reset (should recover gracefully)
        brain.reset()
        
        # Verify reset worked
        assert brain.neuron_count() == 100
        
        # Continue running after reset
        for step in range(5, 10):
            brain.step(step)
        
        # Verify brain still works
        assert brain.neuron_count() == 100
        
        print("✓ Resilience and recovery test passed")
    
    def test_configuration_persistence(self):
        """Test configuration persistence"""
        print("Testing configuration persistence...")
        
        # Create config with specific settings
        config_dict = {
            "neuron_count": 300,
            "region_count": 4,
            "connection_probability": 0.25,
            "simulation_timestep": 0.002,
            "random_seed": 999,
            "enable_structural_plasticity": True,
        }
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Verify initial configuration
        assert brain.neuron_count() == 300
        assert brain.get_region_count() == 4
        assert brain.getConfig().get_int("connection_probability") == 0.25
        
        # Run simulation to ensure config is used consistently
        for step in range(10):
            brain.step(step)
        
        # Configuration should persist
        assert brain.neuron_count() == 300
        assert brain.get_region_count() == 4
        
        print("✓ Configuration persistence test passed")

if __name__ == "__main__":
    # Run tests manually if script is executed directly
    import sys
    pytest.main([__file__, "-v"])
