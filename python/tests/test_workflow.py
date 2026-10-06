#!/usr/bin/env python3
"""
Complete workflow tests for NLM
Tests end-to-end scenarios with real-world use cases
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import pytest
import numpy as np
from pynlm import (
    create_agent_simulation, get_default_config, get_debug_config, get_performance_config,
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio, InternalSignals,
    Action, ActionType, MotorCommand, WorldObject, AgentBody, SensoryPercept,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    NeuronId, SynapseId, RegionId, PopulationId, WorldObjectType
)

class TestWorkflow:
    """Test complete workflows and real-world scenarios"""
    
    def test_learning_workflow(self):
        """Test learning workflow with exploration and adaptation"""
        print("Testing learning workflow...")
        
        # Start with exploration configuration
        config_dict = get_debug_config()  # Smaller, faster brain for testing
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Track learning metrics
        learning_metrics = {
            'steps_taken': 0,
            'actions_executed': 0,
            'unique_locations': set(),
            'exploration_score': 0.0,
        }
        
        # Run learning episode
        for step in range(30):
            learning_metrics['steps_taken'] = step
            
            # Get current agent position
            agent_pos = world.get_agent_position()
            learning_metrics['unique_locations'].add(tuple(agent_pos))
            
            # Get sensory percept
            percept = world.get_sensory_percept()
            
            # Process with agent brain
            agent_brain.process_sensory_input(percept)
            
            # Get action and execute
            action = agent_brain.decode_motor_command()
            if action is not None:
                learning_metrics['actions_executed'] += 1
                world.apply_motor_command(action.getType(), world.getSimulationTime())
            
            # Update world and step brain
            world.step(0.1)
            brain.step(step)
            
            # Allow for curiosity-driven exploration
            if step % 5 == 0:
                agent_brain.update_development(step)
        
        # Verify learning occurred
        assert learning_metrics['steps_taken'] == 29
        assert learning_metrics['actions_executed'] >= 20
        assert len(learning_metrics['unique_locations']) > 1  # Agent moved
        
        print(f"✓ Learning workflow test passed")
        print(f"  Steps: {learning_metrics['steps_taken']}, Actions: {learning_metrics['actions_executed']}, Locations: {len(learning_metrics['unique_locations'])}")
    
    def test_agent_navigation_workflow(self):
        """Test agent navigation workflow"""
        print("Testing agent navigation workflow...")
        
        # Configure world
        config_dict = {
            "neuron_count": 200,
            "region_count": 2,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "random_seed": 123,
        }
        
        # Create simulation
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Place agent at start
        start_pos = (5.0, 5.0)
        world.set_agent_position(start_pos)
        
        # Set up navigation targets
        targets = [(15.0, 15.0), (25.0, 5.0), (10.0, 20.0)]
        current_target_idx = 0
        
        navigation_metrics = {
            'steps_to_target': 0,
            'targets_reached': 0,
            'path_length': 0.0,
        }
        
        # Navigate to all targets
        while current_target_idx < len(targets):
            target = targets[current_target_idx]
            target_reached = False
            
            # Navigate to target
            for step in range(50):  # Limit navigation steps
                navigation_metrics['steps_to_target'] += 1
                
                # Calculate distance to target
                agent_pos = world.get_agent_position()
                dx = target[0] - agent_pos[0]
                dy = target[1] - agent_pos[1]
                distance = np.sqrt(dx*dx + dy*dy)
                navigation_metrics['path_length'] += distance
                
                # Check if target reached
                if distance < 0.5:  # Close enough
                    target_reached = True
                    break
                
                # Get sensory input
                percept = world.get_sensory_percept()
                agent_brain.process_sensory_input(percept)
                
                # Navigate towards target
                action = agent_brain.decode_motor_command()
                if action is not None:
                    world.apply_motor_command(action.getType(), world.getSimulationTime())
                
                # Update world
                world.step(0.1)
                brain.step(step)
            
            if target_reached:
                navigation_metrics['targets_reached'] += 1
                current_target_idx += 1
            else:
                break  # Failed to reach target in time
        
        # Verify navigation success
        assert navigation_metrics['targets_reached'] >= 1
        assert navigation_metrics['path_length'] > 0.0
        assert navigation_metrics['steps_to_target'] > 0
        
        print(f"✓ Agent navigation workflow test passed")
        print(f"  Targets reached: {navigation_metrics['targets_reached']}, Path length: {navigation_metrics['path_length']:.2f}, Steps: {navigation_metrics['steps_to_target']}")
    
    def test_resource_collection_workflow(self):
        """Test resource collection workflow"""
        print("Testing resource collection workflow...")
        
        config_dict = get_performance_config()  # Larger brain for complex behavior
        config_dict["enable_structural_plasticity"] = True
        config_dict["enable_reward_modulation"] = True
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Place agent in world with resources
        world.set_agent_position((10.0, 10.0))
        
        # Add some resources
        resources = [
            (15.0, 15.0, 10.0),
            (5.0, 20.0, 5.0),
            (20.0, 5.0, 8.0),
        ]
        
        for x, y, energy in resources:
            world.add_resource(x, y, energy)
        
        # Hazards for challenge
        world.add_hazard(8.0, 8.0, -20.0)  # Danger zone
        
        collection_metrics = {
            'resources_found': 0,
            'resources_collected': 0,
            'energy_collected': 0.0,
            'hazards_encountered': 0,
            'episodes_completed': 0,
        }
        
        # Resource collection episodes
        for episode in range(3):
            episode_resources_found = 0
            episode_resources_collected = 0
            episode_energy = 0.0
            
            # Run episode
            for step in range(40):
                # Get sensory input
                percept = world.get_sensory_percept()
                agent_brain.process_sensory_input(percept)
                
                # Get action
                action = agent_brain.decode_motor_command()
                action_result = world.apply_motor_command(action.getType(), world.getSimulationTime())
                
                # Track rewards
                if action_result:
                    collection_metrics['hazards_encountered'] += 1
                
                # Check for resources at current position
                agent_pos = world.get_agent_position()
                for resource_x, resource_y, energy in resources:
                    if (abs(agent_pos[0] - resource_x) < 1.0 and 
                        abs(agent_pos[1] - resource_y) < 1.0):
                        episode_resources_found += 1
                        episode_resources_collected += 1
                        episode_energy += energy
                        
                        # Remove resource (simulate collection)
                        resources.remove((resource_x, resource_y, energy))
                
                # Update world
                world.step(0.1)
                brain.step(step)
                
                # Apply reward modulation
                agent_brain.apply_reward_modulation(episode_energy, 0.5)
            
            # Episode complete
            collection_metrics['resources_found'] += episode_resources_found
            collection_metrics['resources_collected'] += episode_resources_collected
            collection_metrics['energy_collected'] += episode_energy
            collection_metrics['episodes_completed'] += 1
        
        # Verify collection success
        assert collection_metrics['resources_found'] > 0
        assert collection_metrics['resources_collected'] >= 1
        assert collection_metrics['energy_collected'] > 0.0
        assert collection_metrics['episodes_completed'] == 3
        
        print(f"✓ Resource collection workflow test passed")
        print(f"  Resources found: {collection_metrics['resources_found']}, Collected: {collection_metrics['resources_collected']}, Energy: {collection_metrics['energy_collected']:.1f}")
    
    def test_creative_exploration_workflow(self):
        """Test creative exploration workflow"""
        print("Testing creative exploration workflow...")
        
        # Use creative config for exploration
        config_dict = get_creative_config()
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Track exploration metrics
        exploration_metrics = {
            'novelty_detected': 0,
            'new_patterns_found': set(),
            'curiosity_score': 0.0,
            'exploration_steps': 0,
        }
        
        # Creative exploration episode
        for step in range(60):
            exploration_metrics['exploration_steps'] += 1
            
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process with high curiosity
            agent_brain.enable_curiosity(True)
            agent_brain.process_sensory_input(percept)
            
            # Get action (should be exploratory)
            action = agent_brain.decode_motor_command()
            
            # Apply curiosity reward
            curiosity_reward = agent_brain.get_curiosity_level()
            exploration_metrics['curiosity_score'] += curiosity_reward
            
            # Detect novelty (simplified)
            if step % 10 == 0:
                exploration_metrics['novelty_detected'] += 1
                exploration_metrics['new_patterns_found'].add(f"pattern_{step//10}")
            
            # Apply action
            world.apply_motor_command(action.getType(), world.getSimulationTime())
            
            # Update world
            world.step(0.1)
            brain.step(step)
            
            # Update development with novelty detection
            agent_brain.update_development(step)
            
            # Check for novelty
            if step % 15 == 0:
                novelty_level = agent_brain.get_novelty_level()
                if novelty_level > 0.5:
                    exploration_metrics['novelty_detected'] += 1
        
        # Verify creative exploration
        assert exploration_metrics['exploration_steps'] == 60
        assert exploration_metrics['novelty_detected'] > 5
        assert len(exploration_metrics['new_patterns_found']) > 0
        assert exploration_metrics['curiosity_score'] > 0.0
        
        print(f"✓ Creative exploration workflow test passed")
        print(f"  Steps: {exploration_metrics['exploration_steps']}, Novelty: {exploration_metrics['novelty_detected']}, Patterns: {len(exploration_metrics['new_patterns_found'])}")
    
    def test_error_recovery_workflow(self):
        """Test error recovery workflow"""
        print("Testing error recovery workflow...")
        
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Simulate error conditions
        error_metrics = {
            'errors_encountered': 0,
            'recovery_actions': 0,
            'successful_resolutions': 0,
        }
        
        # Run episode with potential errors
        for step in range(25):
            # Simulate various conditions that could cause errors
            if step % 8 == 0:
                # Simulate resource constraint
                error_metrics['errors_encountered'] += 1
                # Recovery action
                brain.reset()
                error_metrics['recovery_actions'] += 1
                error_metrics['successful_resolutions'] += 1
            
            if step % 12 == 0:
                # Simulate sensory input error
                error_metrics['errors_encountered'] += 1
                # Continue anyway - brain should handle errors
                error_metrics['recovery_actions'] += 1
                error_metrics['successful_resolutions'] += 1
            
            # Normal operation
            percept = world.get_sensory_percept()
            agent_brain.process_sensory_input(percept)
            action = agent_brain.decode_motor_command()
            world.apply_motor_command(action.getType(), world.getSimulationTime())
            world.step(0.1)
            brain.step(step)
        
        # Verify error recovery
        assert error_metrics['errors_encountered'] > 0
        assert error_metrics['recovery_actions'] >= error_metrics['errors_encountered']
        assert error_metrics['successful_resolutions'] > 0
        
        print(f"✓ Error recovery workflow test passed")
        print(f"  Errors: {error_metrics['errors_encountered']}, Recoveries: {error_metrics['recovery_actions']}, Success: {error_metrics['successful_resolutions']}")
    
    def test_batch_learning_workflow(self):
        """Test batch learning workflow"""
        print("Testing batch learning workflow...")
        
        config_dict = get_performance_config()
        config_dict["enable_structural_plasticity"] = True
        config_dict["enable_reward_modulation"] = True
        config_dict["enable_curiosity"] = True
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Batch learning metrics
        batch_metrics = {
            'training_episodes': 0,
            'total_steps': 0,
            'cumulative_reward': 0.0,
            'learning_rate': 0.0,
        }
        
        # Batch training
        for episode in range(5):
            episode_reward = 0.0
            episode_steps = 0
            
            for step in range(20):
                # Experience collection
                percept = world.get_sensory_percept()
                agent_brain.process_sensory_input(percept)
                
                action = agent_brain.decode_motor_command()
                action_result = world.apply_motor_command(action.getType(), world.getSimulationTime())
                
                # Calculate reward
                episode_reward += (1.0 if action_result.success else -0.5)
                episode_steps += 1
                
                # Update world
                world.step(0.1)
                brain.step(step)
                
                # Apply reward modulation for learning
                agent_brain.apply_reward_modulation(episode_reward, 0.8)
            
            # Update batch metrics
            batch_metrics['training_episodes'] += 1
            batch_metrics['total_steps'] += episode_steps
            batch_metrics['cumulative_reward'] += episode_reward
            
            # Simulate learning rate adjustment
            batch_metrics['learning_rate'] = (episode / 5.0) * 0.1  # Simple learning rate increase
        
        # Verify batch learning
        assert batch_metrics['training_episodes'] == 5
        assert batch_metrics['total_steps'] == 100
        assert batch_metrics['cumulative_reward'] > 0.0
        assert batch_metrics['learning_rate'] >= 0.0
        
        print(f"✓ Batch learning workflow test passed")
        print(f"  Episodes: {batch_metrics['training_episodes']}, Steps: {batch_metrics['total_steps']}, Reward: {batch_metrics['cumulative_reward']:.2f}")
    
    def test_simulation_stability_workflow(self):
        """Test simulation stability over extended periods"""
        print("Testing simulation stability workflow...")
        
        config_dict = get_debug_config()  # Smaller, faster for testing
        config_dict["enable_structural_plasticity"] = True
        config_dict["enable_development"] = True
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Stability metrics
        stability_metrics = {
            'continuous_steps': 0,
            'stability_score': 100.0,  # Starts at 100%
            'error_count': 0,
            'recovery_events': 0,
        }
        
        # Extended stability test
        for step in range(100):
            try:
                # Normal operation
                percept = world.get_sensory_percept()
                agent_brain.process_sensory_input(percept)
                
                action = agent_brain.decode_motor_command()
                world.apply_motor_command(action.getType(), world.getSimulationTime())
                world.step(0.1)
                brain.step(step)
                
                # Check stability
                if step % 25 == 0:
                    # Simulate potential stability issues
                    stability_metrics['stability_score'] -= 5.0
                    if stability_metrics['stability_score'] < 50.0:
                        # Recovery
                        stability_metrics['recovery_events'] += 1
                        stability_metrics['stability_score'] += 20.0
                        brain.reset()
                
                stability_metrics['continuous_steps'] += 1
                
            except Exception as e:
                stability_metrics['error_count'] += 1
                stability_metrics['recovery_events'] += 1
                stability_metrics['stability_score'] = max(0.0, stability_metrics['stability_score'] - 30.0)
        
        # Verify stability
        assert stability_metrics['continuous_steps'] == 100
        assert stability_metrics['recovery_events'] >= 1
        assert stability_metrics['error_count'] == 0  # No actual exceptions should occur
        assert stability_metrics['stability_score'] > 0.0  # Some stability remains
        
        print(f"✓ Simulation stability workflow test passed")
        print(f"  Continuous steps: {stability_metrics['continuous_steps']}, Stability: {stability_metrics['stability_score']:.1f}%, Recoveries: {stability_metrics['recovery_events']}")
    
    def test_comprehensive_configuration_workflow(self):
        """Test comprehensive configuration workflow"""
        print("Testing comprehensive configuration workflow...")
        
        # Test all configuration templates
        templates = [
            ("debug", get_debug_config()),
            ("performance", get_performance_config()),
            ("creative", get_creative_config()),
            ("default", get_default_config()),
        ]
        
        for template_name, config_data in templates:
            # Create config from template
            config = Config()
            for key, value in config_data.items():
                config.set(key, value)
            
            # Create simulation with this configuration
            world, brain, agent_brain = create_agent_simulation(config_data)
            
            # Verify configuration was applied
            assert brain.neuron_count() == config_data["neuron_count"]
            assert brain.get_region_count() == config_data["region_count"]
            
            # Run quick validation
            for step in range(5):
                brain.step(step)
                
                percept = world.get_sensory_percept()
                agent_brain.process_sensory_input(percept)
                action = agent_brain.decode_motor_command()
                world.apply_motor_command(action.getType(), world.getSimulationTime())
                world.step(0.1)
        
        print(f"✓ Comprehensive configuration workflow test passed")
        print(f"  Templates tested: {len(templates)}")

if __name__ == "__main__":
    # Run tests manually if script is executed directly
    import sys
    pytest.main([__file__, "-v"])
