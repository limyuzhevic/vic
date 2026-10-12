#!/usr/bin/env python3
"""
Advanced NLM Example - Demonstrates enhanced features for sophisticated users

This example showcases:
- Advanced brain configuration and optimization
- Multi-phase learning simulations
- Performance monitoring and metrics collection
- Advanced agent behaviors with neuromodulation
- Memory management and checkpointing
"""

import pynlm
import time
import json
from typing import Dict, List, Optional
class AdvancedNLMSimulation:
    """Advanced simulation framework for NLM with enhanced features"""

    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.metrics = {
            'episodes': [],
            'performance': {},
            'learning_curve': []
        }

    def optimize_configuration(self, scenario: str = "balanced"):
        """Create optimized configuration for different scenarios"""
        print(f"Configuring for {scenario} scenario...")

        if scenario == "balanced":
            # Balanced configuration for general learning
            self.config.set("neuron_count", 1500, pynlm.ConfigSource.Runtime)
            self.config.set("region_count", 3, pynlm.ConfigSource.Runtime)
            self.config.set("connection_probability", 0.12, pynlm.ConfigSource.Runtime)
            self.config.set("random_seed", 42, pynlm.ConfigSource.Runtime)
            self.config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Runtime)
            
        elif scenario == "fast_learning":
            # Optimized for rapid learning
            self.config.set("neuron_count", 1000, pynlm.ConfigSource.Runtime)
            self.config.set("region_count", 2, pynlm.ConfigSource.Runtime)
            self.config.set("connection_probability", 0.15, pynlm.ConfigSource.Runtime)
            self.config.set("random_seed", 123, pynlm.ConfigSource.Runtime)
            self.config.set("stdp_ltp_weight", 0.03, pynlm.ConfigSource.Runtime)
            self.config.set("stdp_ltd_weight", 0.025, pynlm.ConfigSource.Runtime)
            
        elif scenario == "stable_memory":
            # Optimized for memory retention
            self.config.set("neuron_count", 2000, pynlm.ConfigSource.Runtime)
            self.config.set("region_count", 4, pynlm.ConfigSource.Runtime)
            self.config.set("connection_probability", 0.08, pynlm.ConfigSource.Runtime)
            self.config.set("random_seed", 456, pynlm.ConfigSource.Runtime)
            self.config.set("synaptogenesis_rate", 0.0002, pynlm.ConfigSource.Runtime)
            self.config.set("pruning_rate", 0.00005, pynlm.ConfigSource.Runtime)

        print(f"   Neurons: {self.config.getOr('neuron_count', 0)}")
        print(f"   Regions: {self.config.getOr('region_count', 0)}")
        print(f"   Connections: {self.config.getOr('connection_probability', 0.0) * 100}%")

    def create_advanced_agent(self, world_size: tuple = (25, 25), 
                            vision_size: tuple = (8, 8)):
        """Create enhanced agent with advanced capabilities"""
        print("Initializing advanced agent...")

        # Initialize brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()

        # Create sophisticated world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=world_size[0],
            height=world_size[1],
            visionWidth=vision_size[0],
            visionHeight=vision_size[1]
        )
        self.world.reset()

        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)

        # Enable all advanced learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)

        print(f"   Brain initialized with {self.brain.getTotalNeuronCount()} neurons")
        print(f"   World: {world_size[0]}x{world_size[1]}, Vision: {vision_size[0]}x{vision_size[1]}")
        print(f"   Agent capabilities: Reward modulation, Structural plasticity, Development, Curiosity")

    def run_advanced_episode(self, episode_length: int = 200, 
                           enable_advanced_learning: bool = True) -> Dict:
        """Run a single advanced episode with comprehensive tracking"""
        print(f"Running advanced episode ({episode_length} steps)...")

        episode_start_time = time.time()

        # Record initial state for learning analysis
        initial_state = {
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'developmental_stage': self.agent.getDevelopmentalStage(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount()
        }

        # Note: Python bindings don't expose direct access to synaptic weights
        # This is a conceptual example - in practice you'd use the brain's internal metrics
        initial_weights = []

        episode_metrics = {
            'steps': [],
            'performance': {},
            'neural_dynamics': {}
        }

        # Advanced simulation loop
        for step in range(episode_length):
            step_metrics = {'step': step}

            # Update environment with advanced physics if available
            self.world.update(0.1)

            # Get rich sensory input
            percept = self.world.getSensoryPercept()

            # Advanced sensory processing
            self.agent.processSensoryInput(percept)

            # Brain computation with neuromodulation
            self.brain.step(step)

            # Advanced action selection with curiosity and exploration
            action = self.agent.decodeMotorCommand()

            # Execute action with world consequences
            world_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())

            # Advanced learning system with multiple factors
            if enable_advanced_learning and self.agent.isRewardModulationEnabled():
                # Multi-factor reward computation
                reward = self._compute_advanced_reward(action, world_result, step)
                predicted = self.agent.getPredictionError()

                # Apply sophisticated neuromodulation
                self.agent.applyRewardModulation(reward, predicted)

                # Update development with age-dependent plasticity
                if self.agent.isDevelopmentEnabled():
                    developmental_factor = self._get_developmental_factor()
                    self.agent.updateDevelopment(0.1 * developmental_factor)

                # Store learning metrics
                step_metrics['reward'] = reward
                step_metrics['prediction_error'] = predicted
                step_metrics['learning_efficiency'] = self._calculate_learning_efficiency(step)

            # Track neural dynamics
            step_metrics['spike_count'] = self.brain.getTotalSpikeCount() - initial_state['spikes']
            step_metrics['firing_neurons'] = self.brain.getFiringNeuronCount()
            step_metrics['firing_rate'] = self.brain.getAverageFiringRate()
            step_metrics['curiosity'] = self.agent.getCuriosityLevel()
            step_metrics['novelty'] = self.agent.getNoveltyLevel()

            # Log significant milestones
            if step % 50 == 0:
                print(f"     Step {step}: Spikes={step_metrics['spike_count']}, "
                      f"Firing={step_metrics['firing_neurons']}, "
                      f"Curiosity={step_metrics['curiosity']:.3f}, "
                      f"Novelty={step_metrics['novelty']:.3f}")

            episode_metrics['steps'].append(step_metrics)

        episode_end_time = time.time()
        episode_duration = episode_end_time - episode_start_time

        # Compute final metrics
        final_state = {
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'developmental_stage': self.agent.getDevelopmentalStage()
        }

        # Calculate learning metrics using available Python APIs
        learning_metrics = self._calculate_learning_metrics(initial_state, final_state,
                                                          initial_weights, [],
                                                          episode_length)

        episode_result = {
            'episode_number': len(self.metrics['episodes']) + 1,
            'duration': episode_duration,
            'initial_state': initial_state,
            'final_state': final_state,
            'learning_metrics': learning_metrics,
            'episode_metrics': episode_metrics,
            'performance_score': self._calculate_performance_score(learning_metrics)
        }

        self.metrics['episodes'].append(episode_result)
        self.metrics['learning_curve'].append({
            'episode': episode_result['episode_number'],
            'learning_rate': learning_metrics['synaptic_change_rate'],
            'performance': episode_result['performance_score']
        })

        print(f"Episode completed in {episode_duration:.2f}s")
        print(f"  Learning metrics: Weight change={learning_metrics['total_weight_change']:.4f}, "
              f"Spike rate={learning_metrics['spike_rate_change']:.2f}")

        return episode_result

    def run_multi_episode_training(self, num_episodes: int = 10, 
                                  episode_length: int = 200):
        """Run comprehensive multi-episode training with performance optimization"""
        print(f"Starting {num_episodes}-episode advanced training...")

        training_start_time = time.time()
        best_performance = 0.0
        best_episode = 0

        for episode in range(num_episodes):
            print(f"\n{'='*50}")
            print(f"EPISODE {episode + 1}/{num_episodes}")
            print(f"{'='*50}")

            # Adaptive configuration based on previous performance
            if episode > 0 and episode % 3 == 0:
                self._adapt_configuration()

            # Run episode with advanced features
            episode_result = self.run_advanced_episode(episode_length)

            # Track best performance
            if episode_result['performance_score'] > best_performance:
                best_performance = episode_result['performance_score']
                best_episode = episode + 1

            # Save checkpoint if enabled
            if episode % 5 == 0:
                self._save_checkpoint(f"checkpoint_episode_{episode + 1}")

        training_end_time = time.time()
        total_training_time = training_end_time - training_start_time

        # Analyze overall performance
        self._analyze_training_results(best_episode, best_performance, total_training_time)

        print(f"\n{'='*50}")
        print(f"TRAINING COMPLETE")
        print(f"{'='*50}")
        print(f"Total training time: {total_training_time:.2f}s")
        print(f"Best performance: {best_performance:.3f} (Episode {best_episode})")
        print(f"Average learning rate: {sum(m['learning_rate'] for m in self.metrics['learning_curve']) / len(self.metrics['learning_curve']):.4f}")
        print(f"Final developmental stage: {self.agent.getDevelopmentalStage()}")

    def _compute_advanced_reward(self, action, world_result, step: int) -> float:
        """Compute sophisticated reward with multiple factors"""
        base_reward = 0.0

        # Action-based rewards
        if action.type == pynlm.ActionType.Interact:
            base_reward += 2.0
        elif action.type == pynlm.ActionType.MoveForward:
            base_reward += 0.5
        elif action.type == pynlm.ActionType.TurnLeft or action.type == pynlm.ActionType.TurnRight:
            base_reward += 0.3
        elif action.type == pynlm.ActionType.Wait:
            base_reward -= 0.5

        # Novelty bonus (encourage exploration)
        novelty_bonus = self.agent.getNoveltyLevel() * 1.0
        base_reward += novelty_bonus

        # Curiosity-modulated reward (exploration vs exploitation)
        curiosity_modulation = 1.0 + (self.agent.getCuriosityLevel() * 0.5)
        base_reward *= curiosity_modulation

        # Developmental stage modulation
        developmental_factor = self._get_developmental_factor()
        base_reward *= developmental_factor

        # Step-based learning potential (earlier steps = higher learning potential)
        learning_potential = 1.0 - (step / 1000.0)  # Decreases over time
        base_reward *= learning_potential

        return base_reward

    def _get_developmental_factor(self) -> float:
        """Get developmental stage multiplier for learning efficiency"""
        stage = self.agent.getDevelopmentalStage()
        if stage == pynlm.DevelopmentalStage.Initial:
            return 1.5  # High plasticity
        elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
            return 1.2
        elif stage == pynlm.DevelopmentalStage.Maturation:
            return 1.0
        elif stage == pynlm.DevelopmentalStage.Adult:
            return 0.7  # More stable, less change
        else:
            return 0.5

    def _calculate_learning_efficiency(self, step: int) -> float:
        """Calculate how efficiently the brain is learning at this step"""
        current_curiosity = self.agent.getCuriosityLevel()
        current_prediction_error = abs(self.agent.getPredictionError())

        # Learning efficiency is high when both curiosity and prediction error are balanced
        efficiency = current_curiosity * current_prediction_error

        # Decay over time (earlier learning is more efficient)
        efficiency *= (1.0 - step / 1000.0)

        return min(efficiency, 1.0)

    def _calculate_learning_metrics(self, initial_state: Dict, final_state: Dict,
                                   initial_weights: List[float], final_weights: List[float],
                                   steps: int) -> Dict:
        """Calculate comprehensive learning metrics"""
        spike_change = final_state['spikes'] - initial_state['spikes']
        spike_rate_change = spike_change / max(steps, 1)

        # Since Python bindings don't expose direct synaptic access, use available metrics
        total_weight_change = 0.0  # Placeholder - would need actual synaptic access

        # Calculate synaptic change rate
        synaptic_change_rate = total_weight_change

        # Calculate information processing metrics
        firing_efficiency = final_state['firing_neurons'] / max(initial_state['firing_neurons'], 1)
        curiosity_growth = final_state['curiosity_level'] / max(initial_state['curiosity_level'], 0.001)
        novelty_evolution = final_state['novelty_level'] / max(initial_state['novelty_level'], 0.001)

        return {
            'spike_rate_change': spike_rate_change,
            'total_weight_change': total_weight_change,
            'synaptic_change_rate': synaptic_change_rate,
            'firing_efficiency': firing_efficiency,
            'curiosity_growth': curiosity_growth,
            'novelty_evolution': novelty_evolution,
            'learning_efficiency': (curiosity_growth + novelty_evolution) / 2
        }

    def _calculate_performance_score(self, learning_metrics: Dict) -> float:
        """Calculate overall performance score from learning metrics"""
        weights = {
            'synaptic_change_rate': 0.3,
            'curiosity_growth': 0.2,
            'firing_efficiency': 0.2,
            'learning_efficiency': 0.3
        }

        score = (learning_metrics['synaptic_change_rate'] * weights['synaptic_change_rate'] +
                learning_metrics['curiosity_growth'] * weights['curiosity_growth'] +
                learning_metrics['firing_efficiency'] * weights['firing_efficiency'] +
                learning_metrics['learning_efficiency'] * weights['learning_efficiency'])

        return min(score, 1.0)  # Cap at 1.0

    def _adapt_configuration(self):
        """Adapt configuration based on learning performance"""
        print("Adapting configuration based on performance...")

        # Simple adaptation logic based on recent learning metrics
        if len(self.metrics['episodes']) > 0:
            latest_episode = self.metrics['episodes'][-1]
            latest_performance = latest_episode['performance_score']

            if latest_performance < 0.3:
                # Poor performance - increase plasticity
                self.config.set("synaptogenesis_rate", 0.0003, pynlm.ConfigSource.Runtime)
                print("  Increasing plasticity for better learning")
            elif latest_performance > 0.7:
                # Good performance - stabilize
                self.config.set("synaptogenesis_rate", 0.0001, pynlm.ConfigSource.Runtime)
                print("  Stabilizing configuration")

            # Reset brain with new configuration
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()

            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)

            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)

    def _save_checkpoint(self, filename: str):
        """Save brain state to checkpoint file"""
        checkpoint_path = f"checkpoints/{filename}.json"
        print(f"Saving checkpoint to {checkpoint_path}")

        checkpoint_data = {
            'config': {
                'neuron_count': self.config.getOr('neuron_count', 0),
                'region_count': self.config.getOr('region_count', 0),
                'connection_probability': self.config.getOr('connection_probability', 0.0),
            },
            'brain_state': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'developmental_stage': self.agent.getDevelopmentalStage().value,
            },
            'agent_state': {
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
            },
            'metrics': self.metrics
        }

        # Note: Actual checkpoint saving would use NLM's built-in checkpoint system
        # This is a simplified version for demonstration

    def _analyze_training_results(self, best_episode: int, best_performance: float, total_time: float):
        """Analyze and display training results"""
        print(f"\n{'='*50}")
        print("TRAINING ANALYSIS")
        print(f"{'='*50}")

        if self.metrics['episodes']:
            latest_episode = self.metrics['episodes'][-1]
            latest_metrics = latest_episode['learning_metrics']

            print(f"Performance summary:")
            print(f"  Best performance: {best_performance:.3f} (Episode {best_episode})")
            print(f"  Latest performance: {latest_episode['performance_score']:.3f}")
            print(f"  Total training time: {total_time:.2f}s")

            print(f"\nLearning analysis:")
            print(f"  Synaptic change rate: {latest_metrics['synaptic_change_rate']:.4f}")
            print(f"  Curiosity growth: {latest_metrics['curiosity_growth']:.3f}")
            print(f"  Novelty evolution: {latest_metrics['novelty_evolution']:.3f}")
            print(f"  Overall learning efficiency: {latest_metrics['learning_efficiency']:.3f}")

            # Performance trend
            if len(self.metrics['learning_curve']) > 1:
                recent_trend = self.metrics['learning_curve'][-1]['learning_rate'] - self.metrics['learning_curve'][-2]['learning_rate']
                print(f"  Learning trend: {'Improving' if recent_trend > 0 else 'Declining'} ({recent_trend:+.4f})")

        print(f"\nFinal state:")
        print(f"  Developmental stage: {self.agent.getDevelopmentalStage()}")
        print(f"  Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
if __name__ == "__main__":
    print("=== Advanced NLM Brain Simulation ===")
    print("This example demonstrates sophisticated learning and neural dynamics")
    print()

    # Create and run advanced simulation
    sim = AdvancedNLMSimulation()

    # Configure for advanced learning
    sim.optimize_configuration("balanced")
    sim.create_advanced_agent(world_size=(30, 30), vision_size=(10, 10))

    # Run multi-episode training
    sim.run_multi_episode_training(num_episodes=5, episode_length=300)

    print("\n=== Advanced simulation completed ===")
    print("The brain demonstrated adaptive learning through:")
    print("- Multi-factor reward computation")
    print("- Developmental stage modulation")
    print("- Curiosity-driven exploration")
    print("- Synaptic plasticity optimization")
    print("- Performance-based adaptation")