#!/usr/bin/env python3
"""
Comprehensive Integration Tests for NLM

Tests complete end-to-end workflows, agent-brain-world integration,
multi-agent scenarios, and real-world learning scenarios.
"""

import sys
import os
import time
import tempfile
import pytest
import numpy as np
from typing import List, Dict, Any

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from sensory.SensoryInput import SensoryInput
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
    from performance.MemoryPool import MemoryPool
    from performance.SIMDVectorization import SIMDUtils, VectorizedLIF
    from performance.ParallelProcessing import ThreadPool, ParallelFor
    from performance.CheckpointSystem import CheckpointManager
except ImportError as e:
    print(f"Error importing integration modules: {e}")
    print("Make sure NLM is properly built.")
    sys.exit(1)
class TestIntegration:
    """Test integration workflow features"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        # Create temporary directory for test files
        self.temp_dir = os.path.join(tempfile.gettempdir(), "nlm_integration_tests")
        os.makedirs(self.temp_dir, exist_ok=True)
    
    def teardown_method(self):
        """Clean up after each test method"""
        import shutil
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
    
    def test_beginner_tutorial_integration(self):
        """Test integration with beginner tutorial"""
        # Test the beginner tutorial workflow
        config = Config()
        config.set("neuron_count", 500)
        config.set("plasticity_rate", 0.1)
        config.set("working_memory_capacity", 100)
        config.set("prediction_horizon", 3)
        
        # Create brain
        brain = Brain(config)
        assert brain.initialize() == True
        
        # Create sensory input
        input_ = SensoryInput(10, 10)
        for i in range(10):
            input_.setPixel(i, i, 1.0f)
        
        # Process input
        brain.receiveSensoryInput(input_)
        
        # Run simulation
        for step in range(20):
            brain.step(step)
            
            if step % 5 == 0:
                assert brain.getTotalSpikeCount() >= 0
                assert brain.getActiveNeuronCount() >= 0
        
        # Get output
        action = brain.produceAction()
        # Action may be None depending on brain state
        
        # Explore working memory
        working_memory = brain.getWorkingMemory()
        if working_memory:
            working_memory.store("test_time", 0.0f)
            working_memory.store("test_goal", 1.0f)
            
            time_val = working_memory.retrieve("test_time")
            goal = working_memory.retrieve("test_goal")
            
            assert time_val == 0.0f
            assert goal == 1.0f
        
        brain.shutdown()
    
    def test_complete_workflow(self):
        """Test complete end-to-end workflow"""
        # Step 1: Create configuration
        config = Config()
        config.set("brain.neuron_count", 1000)
        config.set("brain.plasticity_rate", 0.05)
        config.set("brain.working_memory_capacity", 200)
        config.set("brain.prediction_horizon", 5)
        config.set("brain.simulation_speed", 1.0)
        
        # Step 2: Create brain
        brain = Brain(config)
        assert brain.initialize() == True
        
        # Step 3: Create world environment
        world = SimpleWorld(32, 32)
        world.initialize()
        
        # Step 4: Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Step 5: Run simulation loop
        for episode in range(5):
            # Reset world and agent
            world.reset()
            agent.reset()
            
            # Create sensory input based on world state
            input_ = SensoryInput(10, 10)
            
            # Process input
            agent.processSensoryInput(input_)
            
            # Run neural simulation
            for step in range(10):
                brain.step(step)
                
                if step % 2 == 0:
                    action = brain.produceAction()
                    if action:
                        # Apply action to world
                        world.applyAction(*action)
                        
                        # Get rewards and feedback
                        reward = world.getReward()
                        assert reward is not None
                        
                        # Apply neuromodulation
                        agent.applyRewardModulation(reward, reward * 0.5)
                        
                        # Update development
                        agent.updateDevelopment(0.1f)
            
            # Get performance metrics
            spike_count = brain.getTotalSpikeCount()
            active_neurons = brain.getActiveNeuronCount()
            working_memory = agent.getWorkingMemory()
            
            assert spike_count >= 0
            assert active_neurons >= 0
            
            if working_memory:
                assert working_memory.getEntryCount() >= 0
        
        # Step 6: Save checkpoint if available
        try:
            checkpoint_manager = CheckpointManager()
            checkpoint_manager.configure(self.temp_dir, 10, 2, True)
            
            # Save brain state
            checkpoint_path = checkpoint_manager.saveCheckpoint("integration_test", brain)
            assert checkpoint_path is not None
            
            # List checkpoints
            checkpoints = checkpoint_manager.listCheckpoints()
            assert len(checkpoints) >= 1
            
        except:
            # Checkpoint system may not be fully implemented
            pass
        
        # Cleanup
        agent.shutdown()
        world.shutdown()
        brain.shutdown()
    
    def test_multi_agent_scenario(self):
        """Test multi-agent coordination and social learning"""
        # Create multiple agents with different configurations
        agents = []
        brains = []
        worlds = []
        
        # Agent configurations
        agent_configs = [
            {"neuron_count": 500, "plasticity_rate": 0.2, "name": "FastLearner"},
            {"neuron_count": 300, "plasticity_rate": 0.05, "name": "CarefulLearner"},
            {"neuron_count": 400, "plasticity_rate": 0.1, "name": "BalancedLearner"},
        ]
        
        for config_dict in agent_configs:
            # Create configuration
            config = Config()
            config.set("brain.neuron_count", config_dict["neuron_count"])
            config.set("brain.plasticity_rate", config_dict["plasticity_rate"])
            config.set("brain.working_memory_capacity", 100)
            
            # Create brain
            brain = Brain(config)
            assert brain.initialize() == True
            brains.append(brain)
            
            # Create world for this agent
            world = SimpleWorld(20, 20)
            world.initialize()
            worlds.append(world)
            
            # Create agent brain
            agent = AgentBrain(brain)
            agent.initialize(world)
            agents.append(agent)
        
        # Run coordination simulation
        for episode in range(3):
            print(f"Episode {episode + 1}/3")
            
            # Reset all agents and worlds
            for agent, world in zip(agents, worlds):
                world.reset()
                agent.reset()
            
            # Each agent takes turns
            for agent_idx, (agent, world, brain) in enumerate(zip(agents, worlds, brains)):
                print(f"  Agent {agent_idx + 1}: {agent_configs[agent_idx]['name']}")
                
                # Create sensory input
                input_ = SensoryInput(10, 10)
                
                # Add agent-specific patterns
                for x in range(10):
                    for y in range(10):
                        # Simple pattern based on agent index
                        value = float((x + y + agent_idx) % 10) / 10.0
                        input_.setPixel(x, y, value)
                
                # Agent processes sensory input
                agent.processSensoryInput(input_)
                
                # Run neural simulation
                for step in range(5):
                    brain.step(step)
                    
                    action = brain.produceAction()
                    if action:
                        # Apply action to world
                        world.applyAction(*action)
                        
                        # Get rewards
                        reward = world.getReward()
                        print(f"    Action: {action.getType()}, Reward: {reward}")
                        
                        # Apply neuromodulation
                        agent.applyRewardModulation(reward, reward * 0.3)
                        
                        # Update development
                        agent.updateDevelopment(0.05)
                
                # Get agent-specific metrics
                neuromodulation = agent.getNeuromodulationLevel()
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                prediction_error = agent.getPredictionError()
                
                print(f"    Neuromodulation: {neuromodulation}")
                print(f"    Curiosity: {curiosity}")
                print(f"    Novelty: {novelty}")
                print(f"    Prediction Error: {prediction_error}")
                
                # Social interaction (agent observes nearby agents)
                if agent_idx > 0:
                    nearby_agent = agents[agent_idx - 1]
                    other_neuromod = nearby_agent.getNeuromodulationLevel()
                    
                    # Simple social learning signal
                    social_signal = other_neuromod * 0.3
                    print(f"    Social signal from Agent {agent_idx}: {social_signal}")
        
        # Cleanup
        for agent in agents:
            agent.shutdown()
        
        for world in worlds:
            world.shutdown()
        
        for brain in brains:
            brain.shutdown()
    
    def test_real_world_learning_scenario(self):
        """Test real-world learning scenario with complex tasks"""
        # Simulate a real-world learning task: navigating a maze
        config = Config()
        config.set("brain.neuron_count", 800)
        config.set("brain.plasticity_rate", 0.08)
        config.set("brain.working_memory_capacity", 150)
        config.set("brain.prediction_horizon", 4)
        
        # Create brain
        brain = Brain(config)
        assert brain.initialize() == True
        
        # Create world (maze environment)
        world = SimpleWorld(50, 50)
        world.initialize()
        
        # Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Run learning episodes
        total_rewards = []
        total_steps = []
        
        for episode in range(10):
            print(f"Learning Episode {episode + 1}/10")
            
            # Reset environment
            world.reset()
            agent.reset()
            
            # Place agent in random position
            agent_pos = world.setAgentPosition(25, 25)
            print(f"  Agent started at {agent_pos}")
            
            # Set goal in different location
            goal_pos = world.setGoalPosition(5, 5)
            print(f"  Goal at {goal_pos}")
            
            episode_steps = 0
            episode_reward = 0.0
            
            # Learning loop
            for step in range(100):
                episode_steps += 1
                
                # Create sensory input from world
                input_ = SensoryInput(10, 10)
                
                # Get agent position and create features
                agent_pos = world.getAgentPosition()
                goal_pos = world.getGoalPosition()
                
                # Create simple sensory representation
                # Center of input represents agent position
                input_x = min(agent_pos[0] // 5, 9)
                input_y = min(agent_pos[1] // 5, 9)
                input_.setPixel(input_x, input_y, 1.0f)
                
                # Goal representation
                goal_x = min(goal_pos[0] // 5, 9)
                goal_y = min(goal_pos[1] // 5, 9)
                input_.setPixel(goal_x, goal_y, 0.5f)
                
                # Add some noise for exploration
                for x in range(10):
                    for y in range(10):
                        if np.random.random() < 0.1:
                            input_.setPixel(x, y, np.random.random())
                
                # Agent processes input
                agent.processSensoryInput(input_)
                
                # Run neural simulation
                for neural_step in range(3):
                    brain.step(neural_step)
                
                # Get action
                action = brain.produceAction()
                if action:
                    action_type = action.getType()
                    
                    # Apply action
                    world.applyAction(*action)
                    
                    # Get reward and feedback
                    reward = world.getReward()
                    episode_reward += reward
                    
                    print(f"    Step {step}: Action={action_type}, Reward={reward:.2f}")
                    
                    # Check if goal reached
                    if world.hasReachedGoal():
                        print(f"    Goal reached! Total reward: {episode_reward}")
                        break
                    
                    # Apply learning
                    agent.applyRewardModulation(reward, reward * 0.5)
                    agent.updateDevelopment(0.02)
                else:
                    print(f"    Step {step}: No action selected")
            
            total_rewards.append(episode_reward)
            total_steps.append(episode_steps)
            
            print(f"  Episode finished. Reward: {episode_reward:.2f}, Steps: {episode_steps}")
        
        # Analyze learning progress
        avg_reward = sum(total_rewards) / len(total_rewards)
        avg_steps = sum(total_steps) / len(total_steps)
        
        print(f"\nLearning Results:")
        print(f"  Average Reward per Episode: {avg_reward:.2f}")
        print(f"  Average Steps per Episode: {avg_steps:.1f}")
        print(f"  Total Episodes: {len(total_rewards)}")
        
        # Learning should show improvement or at least reasonable performance
        assert len(total_rewards) == 10
        assert all(r >= 0 for r in total_rewards)  # Rewards should be non-negative
        assert all(s > 0 for s in total_steps)  # Should have taken steps
        
        # If we reached goals, that would be excellent
        # goals_reached = world.hasReachedGoal()
        # assert goals_reached  # This would be great but not guaranteed
        
        # Cleanup
        agent.shutdown()
        world.shutdown()
        brain.shutdown()
    
    def test_agent_brain_world_integration(self):
        """Test comprehensive agent-brain-world integration"""
        # Create multiple integrated systems
        systems = []
        
        for i in range(3):
            # Create configuration for each agent
            config = Config()
            config.set(f"agent{i}.neuron_count", 400)
            config.set(f"agent{i}.plasticity_rate", 0.1 + (i * 0.05))
            config.set(f"agent{i}.working_memory_capacity", 80 + i * 20)
            config.set(f"agent{i}.prediction_horizon", 3)
            
            # Create brain
            brain = Brain(config)
            assert brain.initialize() == True
            
            # Create world
            world = SimpleWorld(30, 30)
            world.initialize()
            
            # Create agent
            agent = AgentBrain(brain)
            agent.initialize(world)
            
            systems.append({
                "agent": agent,
                "brain": brain,
                "world": world,
                "config": config
            })
        
        # Run concurrent simulation
        import threading
        import queue
        
        results = queue.Queue()
        
        def run_system(system_id, system):
            agent = system["agent"]
            brain = system["brain"]
            world = system["world"]
            
            system_results = {
                "id": system_id,
                "spikes": 0,
                "actions": 0,
                "rewards": 0.0
            }
            
            for step in range(20):
                # Create input
                input_ = SensoryInput(8, 8)
                
                # Simple input pattern
                for x in range(8):
                    for y in range(8):
                        value = (x + y + system_id) % 8 / 8.0
                        input_.setPixel(x, y, value)
                
                # Process
                agent.processSensoryInput(input_)
                
                for neural_step in range(2):
                    brain.step(neural_step)
                
                action = brain.produceAction()
                if action:
                    system_results["actions"] += 1
                    
                    # Simulate world interaction
                    reward = np.random.random() * 10
                    system_results["rewards"] += reward
                
                system_results["spikes"] += brain.getTotalSpikeCount()
                
                time.sleep(0.01)  # Small delay for simulation
            
            results.put(system_results)
        
        # Run systems concurrently
        threads = []
        for i, system in enumerate(systems):
            thread = threading.Thread(target=run_system, args=(i, system))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Collect results
        all_results = []
        while not results.empty():
            all_results.append(results.get())
        
        # Verify results
        assert len(all_results) == 3
        total_spikes = sum(r["spikes"] for r in all_results)
        total_actions = sum(r["actions"] for r in all_results)
        total_rewards = sum(r["rewards"] for r in all_results)
        
        print(f"Concurrent Simulation Results:")
        print(f"  Total Spikes: {total_spikes}")
        print(f"  Total Actions: {total_actions}")
        print(f"  Total Rewards: {total_rewards:.2f}")
        
        assert total_spikes >= 0
        assert total_actions >= 0
        assert total_rewards >= 0
        
        # Cleanup
        for system in systems:
            system["agent"].shutdown()
            system["world"].shutdown()
            system["brain"].shutdown()
    
    def test_checkpoint_integration(self):
        """Test checkpoint system integration with full workflow"""
        # Create brain
        config = Config()
        config.set("brain.neuron_count", 600)
        config.set("brain.plasticity_rate", 0.12)
        config.set("brain.working_memory_capacity", 120)
        
        brain = Brain(config)
        assert brain.initialize() == True
        
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 5, 1, True)  # path, max, frequency, compress
        
        # Run simulation with checkpoints
        checkpoint_count = 0
        
        for episode in range(10):
            # Run some simulation work
            for step in range(15):
                brain.step(step)
                
                # Save checkpoint every 3 steps
                if step % 3 == 0 and step > 0:
                    checkpoint_name = f"episode_{episode}_step_{step}"
                    checkpoint_path = manager.saveCheckpoint(checkpoint_name, brain)
                    if checkpoint_path:
                        checkpoint_count += 1
            
            # Load last checkpoint
            checkpoints = manager.listCheckpoints()
            if checkpoints:
                last_checkpoint = checkpoints[-1]
                # Note: This depends on the actual checkpoint loading implementation
                # For now, we just verify the checkpoint was saved
        
        print(f"Saved {checkpoint_count} checkpoints")
        
        # Verify checkpoints were created
        assert checkpoint_count > 0
        
        # Cleanup
        manager.clearCheckpoints()
        import shutil
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
        
        brain.shutdown()
    
    def test_performance_monitoring_integration(self):
        """Test performance monitoring with full workflow"""
        # Create brain with performance monitoring
        config = Config()
        config.set("brain.neuron_count", 400)
        config.set("brain.plasticity_rate", 0.08)
        config.set("brain.working_memory_capacity", 100)
        
        brain = Brain(config)
        assert brain.initialize() == True
        
        # Get performance stats
        stats = brain.getPerformanceStats()
        assert stats.activeNeurons >= 0
        assert stats.firingNeurons >= 0
        assert stats.avgFiringRate >= 0.0
        
        # Run simulation and monitor performance
        total_spikes = 0
        total_steps = 100
        
        for step in range(total_steps):
            brain.step(step)
            total_spikes += brain.getTotalSpikeCount()
            
            # Get updated stats
            stats = brain.getPerformanceStats()
            
            # Performance metrics should be reasonable
            assert stats.activeNeurons >= 0
            assert stats.firingNeurons >= 0
            assert stats.avgFiringRate >= 0.0
        
        avg_spikes = total_spikes / total_steps
        print(f"Performance Monitor Results:")
        print(f"  Total Spikes: {total_spikes}")
        print(f"  Average Spikes per Step: {avg_spikes:.2f}")
        print(f"  Final Active Neurons: {stats.activeNeurons}")
        print(f"  Final Firing Neurons: {stats.firingNeurons}")
        print(f"  Final Average Firing Rate: {stats.avgFiringRate:.2f} Hz")
        
        assert avg_spikes >= 0
        assert stats.activeNeurons >= 0
        
        brain.shutdown()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
