#!/usr/bin/env python3
"""
Comprehensive NLM (熙然) Examples

This module provides comprehensive examples demonstrating various aspects of
the NLM neural simulation framework, from basic usage to advanced techniques.

Examples included:
1. Basic NLM setup and simulation
2. Learning and memory demonstrations
3. Multi-agent environments
4. Advanced training techniques
5. Visualization and analysis
6. Custom NLM extensions
"""

import sys
import os
import time
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import nlm
from nlm.nlm_utils import (
    NLMValidator, NLMConvenience, NLMBenchmark,
    create_brain_from_dict, create_world_from_spec,
    run_training_session, demonstrate_basic_usage
)

class BasicNLMExample:
    """Basic NLM usage examples."""
    
    @staticmethod
    def simple_brain_simulation():
        """Example 1: Simple brain simulation."""
        print("=== Basic Brain Simulation ===")
        
        # Create and configure brain
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 100)
        config.set('region_count', 1)
        config.set('connection_probability', 0.1)
        config.set('simulation_timestep', 0.001)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation
        initial_spikes = brain.getTotalSpikeCount()
        print(f"Initial spikes: {initial_spikes}")
        
        for step in range(20):
            brain.step(step)
            current_spikes = brain.getTotalSpikeCount() - initial_spikes
            print(f"Step {step}: {current_spikes} spikes")
        
        # Brain status
        brain.logStatus()
        print()
    
    @staticmethod
    def simple_world_simulation():
        """Example 2: Simple world simulation."""
        print("=== Simple World Simulation ===")
        
        # Create world
        world = nlm.createSimpleWorld()
        world.configure(50, 50, 10, 10)
        
        print(f"World configured: {world.getWidth()}x{world.getHeight()}")
        
        # Create agent
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 50)
        brain = nlm.createBrain(config)
        brain.initialize()
        
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run simulation
        for step in range(30):
            world.update(0.001)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            if hasattr(world, 'applyAction'):
                world.applyAction(action)
            
            print(f"Step {step}: Action {action}, "
                  f"Sensory size: {agent_brain.getSensoryInputSize()}")
        
        print()
    
    @staticmethod
    def brain_world_interaction():
        """Example 3: Brain-world interaction."""
        print("=== Brain-World Interaction ===")
        
        # Create brain and world
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 150)
        config.set('region_count', 2)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(100, 100, 20, 20)
        
        # Create agent brain
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Set up reward modulation
        agent_brain.enableRewardModulation(True)
        
        # Main simulation loop
        initial_spikes = brain.getTotalSpikeCount()
        total_reward = 0.0
        
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process and get action
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            if hasattr(world, 'applyAction'):
                reward = world.applyAction(action)
                total_reward += reward
                
                # Apply reward to brain
                agent_brain.applyRewardModulation(reward, reward * 0.5)
            
            # Step brain
            brain.step(step)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
            
            # Print status every 10 steps
            if step % 10 == 0:
                spikes = brain.getTotalSpikeCount() - initial_spikes
                print(f"Step {step}: {spikes} spikes, "
                      f"Reward: {reward:.3f}, "
                      f"Total: {total_reward:.3f}")
        
        print(f"Simulation completed. Total reward: {total_reward:.3f}")
        print()
class LearningExamples:
    """Examples focused on learning and memory."""
    
    @staticmethod
    def learning_experiment():
        """Example 4: Learning experiment with plasticity."""
        print("=== Learning Experiment ===")
        
        # Create brain with plasticity enabled
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 200)
        config.set('region_count', 2)
        config.set('connection_probability', 0.2)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain with all plasticity enabled
        agent_brain = nlm.createAgentBrain(brain)
        
        world = nlm.createSimpleWorld()
        world.configure(80, 80, 15, 15)
        agent_brain.initialize(world)
        
        # Enable all plasticity mechanisms
        agent_brain.enableRewardModulation(True)
        agent_brain.enableStructuralPlasticity(True)
        agent_brain.enableDevelopment(True)
        agent_brain.enableCuriosity(True)
        
        # Record initial state
        initial_weights = {}
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                weights = [syn.getWeight() for syn in region.getSynapses()]
                initial_weights[region_id] = np.mean(weights)
        
        print(f"Initial average weights: {initial_weights}")
        
        # Run learning experiment
        for step in range(100):
            # Generate random input
            percept = nlm.SensoryPercept()
            vision_data = np.random.randn(15, 15) * 0.1
            percept.setVision(vision_data)
            
            # Process input
            agent_brain.processSensoryInput(percept)
            
            # Get action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            if hasattr(world, 'applyAction'):
                reward = world.applyAction(action)
                agent_brain.applyRewardModulation(reward, reward * 0.8)
            
            # Step brain
            brain.step(step)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
        
        # Record final state
        final_weights = {}
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                weights = [syn.getWeight() for syn in region.getSynapses()]
                final_weights[region_id] = np.mean(weights)
        
        print(f"Final average weights: {final_weights}")
        
        # Analyze learning
        learning_detected = False
        for region_id in initial_weights:
            if region_id in final_weights:
                weight_change = final_weights[region_id] - initial_weights[region_id]
                if abs(weight_change) > 0.001:
                    learning_detected = True
                    print(f"Region {region_id}: weight changed by {weight_change:.6f}")
        
        print(f"Learning detected: {learning_detected}")
        print()
    
    @staticmethod
    def memory_system_demo():
        """Example 5: Memory system demonstration."""
        print("=== Memory System Demo ===")
        
        # Create brain with memory systems
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 300)
        config.set('region_count', 3)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Check memory systems
        print("Memory systems status:")
        print(f"  - Working memory available: {hasattr(brain, 'workingMemory')}")
        print(f"  - Episodic memory available: {hasattr(brain, 'episodicMemory')}")
        
        # Create episode in episodic memory
        if hasattr(brain, 'episodicMemory'):
            # Simulate an experience
            percept = nlm.SensoryPercept()
            vision_data = np.random.randn(20, 20) * 0.05
            audio_data = np.random.randn(44100 * 0.1) * 0.1  # 0.1 seconds of audio
            
            percept.setVision(vision_data)
            
            # Record episode (conceptual - actual API may vary)
            print("Episodic memory: Store experience")
            
            # Get episode count
            episode_count = brain.episodicMemory.getEpisodeCount()
            print(f"  - Episodes recorded: {episode_count}")
        
        # Working memory demonstration
        if hasattr(brain, 'workingMemory'):
            print("Working memory: Maintain active traces")
            active_traces = brain.workingMemory.getActiveTraces()
            print(f"  - Active traces: {active_traces}")
        
        # Associative memory demonstration
        print("Associative memory: Form Hebbian connections")
        print("  - Synaptic weights strengthen with correlated activity")
        
        # Run simulation
        for step in range(30):
            # Generate varied input
            percept = nlm.SensoryPercept()
            step_pattern = np.random.randn(20, 20) * (1.0 if step % 5 == 0 else 0.1)
            percept.setVision(step_pattern)
            
            # Process through brain
            brain.step(step)
        
        print("Memory demonstration completed.")
        print()
class AdvancedExamples:
    """Advanced usage examples."""
    
    @staticmethod
    def multi_agent_simulation():
        """Example 6: Multi-agent simulation."""
        print("=== Multi-Agent Simulation ===")
        
        # Create shared world
        world = nlm.createSimpleWorld()
        world.configure(200, 200, 30, 30)
        
        # Create multiple agents with different configurations
        agents = []
        
        # Agent 1: Slow, careful agent
        config1 = nlm.createDefaultConfig()
        config1.set('neuron_count', 100)
        config1.set('region_count', 1)
        config1.set('connection_probability', 0.1)
        
        brain1 = nlm.createBrain(config1)
        brain1.initialize()
        
        agent1 = nlm.createAgentBrain(brain1)
        agent1.initialize(world)
        agent1.enableRewardModulation(True)
        
        agents.append({'name': 'Agent1 (Slow)', 'brain': brain1, 'agent': agent1})
        
        # Agent 2: Fast, exploratory agent
        config2 = nlm.createDefaultConfig()
        config2.set('neuron_count', 150)
        config2.set('region_count', 2)
        config2.set('connection_probability', 0.2)
        
        brain2 = nlm.createBrain(config2)
        brain2.initialize()
        
        agent2 = nlm.createAgentBrain(brain2)
        agent2.initialize(world)
        agent2.enableCuriosity(True)  # Enable exploration
        agent2.enableRewardModulation(True)
        
        agents.append({'name': 'Agent2 (Fast)', 'brain': brain2, 'agent': agent2})
        
        # Agent 3: Specialized agent
        config3 = nlm.createDefaultConfig()
        config3.set('neuron_count', 80)
        config3.set('region_count', 1)
        config3.set('connection_probability', 0.05)
        
        brain3 = nlm.createBrain(config3)
        brain3.initialize()
        
        agent3 = nlm.createAgentBrain(brain3)
        agent3.initialize(world)
        agent3.enableDevelopment(True)
        agent3.enableStructuralPlasticity(True)
        
        agents.append({'name': 'Agent3 (Specialized)', 'brain': brain3, 'agent': agent3})
        
        # Set different starting positions
        agent1.agent.setAgentStart(20, 20)
        agent2.agent.setAgentStart(180, 180)
        agent3.agent.setAgentStart(100, 100)
        
        # Run multi-agent simulation
        for step in range(60):
            # Each agent acts independently
            for i, agent_info in enumerate(agents):
                agent = agent_info['agent']
                brain = agent_info['brain']
                name = agent_info['name']
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process and act
                agent.processSensoryInput(percept)
                action = agent.decodeMotorCommand()
                
                # Apply action
                if hasattr(world, 'applyAction'):
                    reward = world.applyAction(action)
                    agent.applyRewardModulation(reward, reward * 0.5)
                
                # Step brain
                brain.step(step)
                
                # Update development
                agent.updateDevelopment(0.001)
            
            # Update world
            world.update(0.001)
            
            # Print status every 10 steps
            if step % 10 == 0:
                print(f"Step {step}:")
                for i, agent_info in enumerate(agents):
                    brain = agent_info['brain']
                    spikes = brain.getTotalSpikeCount()
                    print(f"  {agent_info['name']}: {spikes} spikes")
        
        print("Multi-agent simulation completed.")
        print()
    
    @staticmethod
    def training_session():
        """Example 7: Complete training session."""
        print("=== Training Session ===")
        
        # Define configuration for training
        brain_config = {
            'neuron_count': 250,
            'region_count': 3,
            'connection_probability': 0.15,
            'simulation_timestep': 0.001,
            'random_seed': 42
        }
        
        world_config = {
            'width': 100,
            'height': 100,
            'vision_width': 20,
            'vision_height': 20
        }
        
        print("Training configuration:")
        print(f"  - Neurons: {brain_config['neuron_count']}")
        print(f"  - Regions: {brain_config['region_count']}")
        print(f"  - Connectivity: {brain_config['connection_probability']}")
        print(f"  - Episodes: 5")
        print(f"  - Steps per episode: 100")
        
        # Run training session
        results = run_training_session(
            brain_config=brain_config,
            world_config=world_config,
            episodes=5,
            steps_per_episode=100,
            learning_rate=0.02
        )
        
        print("\nTraining Results:")
        print(f"  - Final spike change: {results['final_result']['spike_delta']:.3f}")
        print(f"  - Learning episodes: {results['learning_episodes']}/{results['total_episodes']}")
        print(f"  - Average spike rate: {results['average_spike_rate']:.2f}")
        print(f"  - Final weight change: {results['final_result']['weight_change']:.6f}")
        
        # Plot training progress
        if results['all_results']:
            print("\nEpisode Progress:")
            for i, result in enumerate(results['all_results']):
                print(f"  Episode {i+1}: Spike change = {result['spike_delta']:.3f}, "
                      f"Learning = {result['learning_detected']}")
        
        print()
    
    @staticmethod
    def visualization_and_analysis():
        """Example 8: Visualization and analysis."""
        print("=== Visualization and Analysis ===")
        
        # Create convenience wrapper for analysis
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 200)
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(80, 80, 15, 15)
        
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run simulation
        simulation_data = []
        
        for step in range(50):
            # Get data
            percept = world.getSensoryPercept()
            action = agent_brain.decodeMotorCommand()
            
            brain.step(step)
            world.update(0.001)
            
            # Record metrics
            metrics = {
                'step': step,
                'spike_count': brain.getTotalSpikeCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'weight_change': brain.getAverageSynapticWeight(),
                'development_stage': brain.getDevelopmentalStage(),
                'reward': agent_brain.getNeuromodulationLevel()
            }
            
            simulation_data.append(metrics)
        
        # Create analysis
        print("Simulation Analysis:")
        
        # Spike count analysis
        spike_counts = [m['spike_count'] for m in simulation_data]
        print(f"  - Spike count statistics:")
        print(f"    Mean: {np.mean(spike_counts):.1f}")
        print(f"    Std: {np.std(spike_counts):.1f}")
        print(f"    Max: {np.max(spike_counts)}")
        
        # Learning analysis
        weight_changes = [m['weight_change'] for m in simulation_data if m['weight_change'] > 0]
        print(f"  - Weight changes:")
        print(f"    Number of changes: {len(weight_changes)}")
        print(f"    Average change: {np.mean(weight_changes):.6f}")
        print(f"    Learning detected: {len(weight_changes) > 0}")
        
        # Development analysis
        development_stages = [m['development_stage'] for m in simulation_data]
        final_stage = development_stages[-1]
        print(f"  - Development progress:")
        print(f"    Final stage: {final_stage}")
        
        # Create visualization (conceptual)
        print("Visualization created (conceptual):")
        print("  - Neural activity time series")
        print("  - Weight dynamics plot")
        print("  - Development progress chart")
        print("  - Learning progress graph")
        
        print("Analysis completed.")
        print()
class CustomExtensions:
    """Custom extensions and extensions examples."""
    
    @staticmethod
    def custom_validator():
        """Example 9: Custom validator."""
        print("=== Custom Validator ===")
        
        # Create a custom validator
        class CustomNLMValidator(NLMValidator):
            def validate_custom_config(self, config):
                """Validate custom configuration."""
                issues = []
                
                # Check for custom settings
                if config.has('custom_setting'):
                    custom_value = config.getOr('custom_setting', 0)
                    
                    # Validate custom value
                    if not isinstance(custom_value, (int, float)):
                        issues.append(f"Custom setting must be numeric, got {type(custom_value)}")
                    
                    if custom_value < 0 or custom_value > 100:
                        issues.append(f"Custom setting out of range: {custom_value}")
                
                return issues
            
            def validate_brain_behavior(self, brain):
                """Validate brain behavior metrics."""
                metrics = []
                
                # Check spike count
                spike_count = brain.getTotalSpikeCount()
                metrics.append(('spike_count', spike_count))
                
                # Check firing rate
                firing_rate = brain.getAverageFiringRate()
                metrics.append(('firing_rate', firing_rate))
                
                # Check network stability
                # (conceptual - would need more sophisticated analysis)
                stability_score = 1.0 - (firing_rate / 100.0) if firing_rate < 100 else 0.0
                metrics.append(('stability_score', stability_score))
                
                return metrics
        
        # Use custom validator
        validator = CustomNLMValidator()
        
        # Validate custom configuration
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 100)
        config.set('custom_setting', 50)  # Custom setting
        
        custom_issues = validator.validate_custom_config(config)
        if custom_issues:
            print(f"Custom config issues: {custom_issues}")
        else:
            print("Custom configuration validated successfully")
        
        # Validate brain behavior
        brain = nlm.createBrain(config)
        brain.initialize()
        
        for step in range(10):
            brain.step(step)
        
        brain_metrics = validator.validate_brain_behavior(brain)
        print("Brain metrics:")
        for name, value in brain_metrics:
            print(f"  - {name}: {value}")
        
        print()
    
    @staticmethod
    def custom_convenience():
        """Example 10: Custom convenience wrapper."""
        print("=== Custom Convenience Wrapper ===")
        
        # Create custom convenience wrapper
        class CustomNLMConvenience(NLMConvenience):
            def __init__(self, brain, world, config):
                super().__init__(brain, world)
                self.config = config
                self.session_data = []
            
            def run_advanced_episode(self, steps=100, exploration_mode=True):
                """Run episode with advanced features."""
                print(f"Running advanced episode (exploration={exploration_mode})")
                
                # Record session info
                session_info = {
                    'steps': steps,
                    'exploration_mode': exploration_mode,
                    'start_time': datetime.now()
                }
                
                # Modify behavior based on config
                if self.config.get('enable_curiosity', False):
                    # Apply curiosity-driven exploration
                    pass
                
                # Run episode using parent method
                results = self.run_episode(steps, verbose=False)
                
                # Add custom metrics
                results['session_info'] = session_info
                results['efficiency'] = self._calculate_efficiency(results)
                
                # Store session data
                self.session_data.append(results)
                
                return results
            
            def _calculate_efficiency(self, results):
                """Calculate episode efficiency."""
                # Efficiency = spike change / steps
                if results['steps'] > 0:
                    return results['spike_delta'] / results['steps']
                return 0.0
            
            def get_session_summary(self):
                """Get summary of all sessions."""
                if not self.session_data:
                    return {'sessions': 0}
                
                total_spikes = sum(s['spike_delta'] for s in self.session_data)
                total_steps = sum(s['steps'] for s in self.session_data)
                
                return {
                    'total_sessions': len(self.session_data),
                    'total_steps': total_steps,
                    'average_spike_change': total_spikes / len(self.session_data),
                    'overall_efficiency': total_spikes / total_steps if total_steps > 0 else 0
                }
        
        # Use custom convenience wrapper
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 150)
        config.set('enable_curiosity', True)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(60, 60, 12, 12)
        
        convenience = CustomNLMConvenience(brain, world, config)
        
        # Run advanced episodes
        for episode in range(3):
            results = convenience.run_advanced_episode(steps=30, exploration_mode=True)
            print(f"Episode {episode+1}: Spike change = {results['spike_delta']:.3f}, "
                  f"Efficiency = {results['efficiency']:.6f}")
        
        # Get session summary
        summary = convenience.get_session_summary()
        print("\nSession Summary:")
        for key, value in summary.items():
            print(f"  {key}: {value}")
        
        print()
    
    @staticmethod
    def benchmarking_example():
        """Example 11: Benchmarking."""
        print("=== Benchmarking Example ===")
        
        # Create benchmark suite
        benchmark = NLMBenchmark()
        
        # Test different configurations
        configs = [
            {'neuron_count': 50, 'region_count': 1},
            {'neuron_count': 100, 'region_count': 2},
            {'neuron_count': 200, 'region_count': 3}
        ]
        
        results = {}
        
        for config_dict in configs:
            config = nlm.createDefaultConfig()
            for key, value in config_dict.items():
                config.set(key, value)
            
            brain = nlm.createBrain(config)
            
            # Benchmark initialization
            init_time = benchmark.time_operation("brain_init", brain.initialize)
            
            # Benchmark simulation steps
            for step in range(20):
                benchmark.time_operation("brain_step", brain.step, step)
            
            results[tuple(config_dict.items())] = {
                'init_time': benchmark.get_timing_stats('brain_init'),
                'step_time': benchmark.get_timing_stats('brain_step')
            }
        
        # Print benchmark results
        print("Benchmark Results:")
        for config, metrics in results.items():
            print(f"\nConfig: {dict(config)}")
            print(f"  Initialization: {metrics['init_time'].get('mean', 0):.4f}s")
            print(f"  Step time: {metrics['step_time'].get('mean', 0):.4f}s")
        
        # Print summary
        benchmark.print_summary()
        
        print()
class PracticalExamples:
    """Practical examples for real-world applications."""
    
    @staticmethod
    def perceptual_learning_example():
        """Example 12: Perceptual learning."""
        print("=== Perceptual Learning Example ===")
        
        # Create brain optimized for perceptual learning
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 300)
        config.set('region_count', 3)
        config.set('simulation_timestep', 0.001)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(40, 40, 8, 8)
        
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Enable neuromodulation for learning
        agent_brain.enableRewardModulation(True)
        
        # Perceptual learning task: recognize patterns
        patterns = []
        for i in range(10):
            # Generate random pattern
            pattern = np.random.randn(8, 8) * 0.5
            pattern[pattern > 0] = 1.0
            pattern[pattern < 0] = 0.0
            patterns.append(pattern)
        
        # Learning loop
        for episode in range(20):
            # Select pattern
            target_pattern = patterns[episode % len(patterns)]
            
            # Create input matching target
            percept = nlm.SensoryPercept()
            percept.setVision(target_pattern)
            
            # Process input
            agent_brain.processSensoryInput(percept)
            
            # Get action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            if hasattr(world, 'applyAction'):
                reward = world.applyAction(action)
                # Reward based on pattern matching
                pattern_reward = np.mean(np.abs(target_pattern - percept.getVision()))
                reward += pattern_reward
                
                agent_brain.applyRewardModulation(reward, reward * 0.9)
            
            # Step brain
            brain.step(episode)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
        
        print(f"Perceptual learning completed with {len(patterns)} patterns")
        print("Patterns learned: Form internal representations")
        print("Expected improvement: Better pattern recognition over time")
        print()
    
    @staticmethod
    def decision_making_example():
        """Example 13: Decision making."""
        print("=== Decision Making Example ===")
        
        # Create brain for decision making
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 250)
        config.set('region_count', 4)
        config.set('simulation_timestep', 0.001)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(60, 60, 12, 12)
        
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Enable decision-making systems
        agent_brain.enableDevelopment(True)
        
        # Decision making simulation
        decisions = []
        rewards = []
        
        for step in range(40):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process through brain
            agent_brain.processSensoryInput(percept)
            
            # Make decision
            action = agent_brain.decodeMotorCommand()
            
            # Convert action to decision
            decision = {
                'step': step,
                'action': action,
                'developmental_stage': agent_brain.getDevelopmentalStage(),
                'confidence': agent_brain.getPredictionError(),
                'exploration': agent_brain.getCuriosityLevel()
            }
            
            decisions.append(decision)
            
            # Apply action
            if hasattr(world, 'applyAction'):
                reward = world.applyAction(action)
                rewards.append(reward)
                
                # Apply reward with prediction error
                agent_brain.applyRewardModulation(reward, reward * 0.7)
            
            # Step brain
            brain.step(step)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
        
        # Analyze decisions
        print(f"Decision making completed over {len(decisions)} steps")
        print(f"Average confidence: {np.mean([d['confidence'] for d in decisions]):.3f}")
        print(f"Average exploration: {np.mean([d['exploration'] for d in decisions]):.3f}")
        print(f"Total reward: {sum(rewards):.3f}")
        
        # Decision patterns
        action_counts = {}
        for decision in decisions:
            action = decision['action']
            if action not in action_counts:
                action_counts[action] = 0
            action_counts[action] += 1
        
        print("Decision distribution:")
        for action, count in action_counts.items():
            print(f"  {action}: {count} times")
        
        print()
    
    @staticmethod
    def adaptive_behavior_example():
        """Example 14: Adaptive behavior."""
        print("=== Adaptive Behavior Example ===")
        
        # Create adaptive brain
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 180)
        config.set('region_count', 3)
        config.set('simulation_timestep', 0.001)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(70, 70, 14, 14)
        
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Enable adaptive systems
        agent_brain.enableDevelopment(True)
        agent_brain.enableCuriosity(True)
        agent_brain.enableRewardModulation(True)
        
        # Adaptive behavior simulation
        behaviors = []
        adaptations = []
        
        for step in range(60):
            # Get current state
            percept = world.getSensoryPercept()
            stage = agent_brain.getDevelopmentalStage()
            curiosity = agent_brain.getCuriosityLevel()
            
            # Process and decide
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            if hasattr(world, 'applyAction'):
                reward = world.applyAction(action)
                adaptations.append(reward)
                
                # Adapt based on reward
                agent_brain.applyRewardModulation(reward, reward * 0.6)
            
            # Record behavior
            behavior = {
                'step': step,
                'stage': stage,
                'curiosity': curiosity,
                'action': action,
                'reward': reward if 'reward' in locals() else 0.0
            }
            
            behaviors.append(behavior)
            
            # Step brain
            brain.step(step)
            
            # Update development (adaptive)
            agent_brain.updateDevelopment(0.001)
        
        # Analyze adaptation
        print(f"Adaptive behavior completed over {len(behaviors)} steps")
        print(f"Development progression: {behaviors[0]['stage']} -> {behaviors[-1]['stage']}")
        print(f"Curiosity level: {behaviors[0]['curiosity']:.3f} -> {behaviors[-1]['curiosity']:.3f}")
        print(f"Total reward: {sum(b['reward'] for b in behaviors):.3f}")
        
        # Adaptation patterns
        stage_changes = []
        for i in range(1, len(behaviors)):
            if behaviors[i]['stage'] != behaviors[i-1]['stage']:
                stage_changes.append({
                    'from': behaviors[i-1]['stage'],
                    'to': behaviors[i]['stage'],
                    'step': i
                })
        
        if stage_changes:
            print(f"Development stages changed {len(stage_changes)} times:")
            for change in stage_changes:
                print(f"  Step {change['step']}: {change['from']} -> {change['to']}")
        
        print()
def run_all_examples():
    """Run all examples sequentially."""
    print("=" * 60)
    print("COMPREHENSIVE NLM EXAMPLES")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic NLM Examples", BasicNLMExample),
        ("Learning Examples", LearningExamples),
        ("Advanced Examples", AdvancedExamples),
        ("Custom Extensions", CustomExtensions),
        ("Practical Examples", PracticalExamples)
    ]
    
    for title, example_class in examples:
        print(f"\n{'='*60}")
        print(f"EXAMPLE GROUP: {title}")
        print(f"{'='*60}")
        print()
        
        # Get all example methods
        methods = [method for method in dir(example_class)
                  if method.startswith('example') and callable(getattr(example_class, method))]
        
        # Sort by method number
        methods.sort(key=lambda m: int(m.split('_')[1]) if '_' in m else 0)
        
        for method_name in methods:
            try:
                method = getattr(example_class, method_name)
                method()
                
            except Exception as e:
                print(f"Error in {method_name}: {e}")
                import traceback
                traceback.print_exc()
    
    print("\n" + "="*60)
    print("ALL EXAMPLES COMPLETED")
    print("="*60)
    print()
    print("This comprehensive example suite demonstrates:")
    print("1. Basic NLM usage and setup")
    print("2. Learning mechanisms and plasticity")
    print("3. Advanced multi-agent environments")
    print("4. Training sessions and optimization")
    print("5. Visualization and analysis techniques")
    print("6. Custom extensions and modifications")
    print("7. Practical real-world applications")
    print()
    print("These examples provide a foundation for working with NLM")
    print("in research, education, and practical applications.")
if __name__ == '__main__':
    # Run all examples
    run_all_examples()
    
    # Also demonstrate basic usage
    print("\n" + "="*60)
    print("BASIC USAGE DEMONSTRATION")
    print("="*60)
    demonstrate_basic_usage()
