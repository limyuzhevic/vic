"""
NLM Advanced Python Examples
=============================

This module provides advanced examples and demonstrations of the NLM (Neural Learning Machine)
Python API for experienced users and developers.

Examples include:
- Advanced agent learning and behavior
- Memory consolidation and replay
- Neuromodulation and reward-based learning
- Developmental systems and plastic evolution
- Complex world interactions and decision making
- Performance optimization techniques
"""

import pynlm
import json
import time
import numpy as np

__all__ = ['AdvancedAgentExample', 'MemoryConsolidationExample', 
           'NeuromodulationExample', 'DevelopmentExample',
           'ComplexWorldExample', 'PerformanceExample', 'LearningBenchmarks']

class AdvancedAgentExample:
    """
    Advanced example demonstrating sophisticated agent learning behaviors.
    
    This example showcases:
    - Curiosity-driven exploration
    - Episodic memory for experience replay
    - Predictive learning and error correction
    - Developmental plasticity
    """
    
    def __init__(self, config=None):
        """Initialize advanced agent with enhanced configuration."""
        self.config = config or pynlm.createDefaultConfig()
        
        # Enhanced configuration for advanced behavior
        self.config.set("brain.neuron_count", 2000)
        self.config.set("brain.synapse_density", 0.15)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.stdp.learning_rate", 0.001)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("plasticity.structural.enable", True)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.dopamine.scale", 2.0)
        self.config.set("development.stage", "CriticalPeriod")
        self.config.set("replay_interval", 50)
        self.config.set("consolidation_interval", 500)
        
        # Create brain with enhanced capabilities
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world and agent
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.world.setAgentStart(15.0, 15.0)
        self.world.setRandomSeed(42)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Performance tracking
        self.episode_rewards = []
        self.observations = []
        self.memory_traces = []
        
    def run_advanced_episode(self, num_steps=1000, explore=False):
        """
        Run an advanced learning episode with comprehensive monitoring.
        
        Args:
            num_steps: Number of simulation steps
            explore: Whether to enable exploratory behavior
            
        Returns:
            Dictionary with episode statistics
        """
        episode_reward = 0.0
        curiosity_peak = 0.0
        novelty_score = 0.0
        prediction_error = 0.0
        
        for step in range(num_steps):
            # World update with time-based simulation
            world_time = step * 0.1
            self.world.update(world_time)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Store observation for memory
            self.observations.append(percept)
            
            # Process sensory input through agent
            self.agent.processSensoryInput(percept)
            
            # Apply curiosity-driven exploration if enabled
            if explore and step % 10 == 0:
                curiosity_level = self.agent.getCuriosityLevel()
                curiosity_peak = max(curiosity_peak, curiosity_level)
            
            # Brain processing with developmental stage updates
            self.brain.step(step, world_time)
            
            # Advanced action decoding with context
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, world_time)
            
            # Get reward with prediction error
            reward = self._calculate_enhanced_reward(percept, action, step)
            predicted_reward = self._predict_reward(action, percept)
            
            # Apply reward modulation with prediction error
            self.agent.applyRewardModulation(reward, predicted_reward)
            prediction_error = abs(predicted_reward - reward)
            
            episode_reward += reward
            
            # Update development system
            self.agent.updateDevelopment(0.1)
            
            # Store memory traces periodically
            if step % 50 == 0:
                self._store_memory_trace(step, percept, action, reward)
            
            # Log episode statistics
            if step % 200 == 0:
                self._log_episode_stats(step, reward, prediction_error)
        
        # Finalize episode metrics
        novelty_score = self.agent.getNoveltyLevel()
        self.episode_rewards.append(episode_reward)
        
        stats = {
            'total_reward': episode_reward,
            'curiosity_peak': curiosity_peak,
            'novelty_score': novelty_score,
            'prediction_error': prediction_error,
            'final_development_stage': self.brain.getDevelopmentalStage(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'neurons_active': self.brain.getFiringNeuronCount(),
            'memory_episodes': len(self.memory_traces)
        }
        
        return stats
    
    def _calculate_enhanced_reward(self, percept, action, step):
        """Calculate enhanced reward with multiple factors."""
        base_reward = 0.0
        
        # Reward for successful actions based on action type
        if action.getType() == pynlm.ActionType.MoveForward:
            base_reward += 1.0
        elif action.getType() == pynlm.ActionType.Interact:
            base_reward += 2.0
        elif action.getType() == pynlm.ActionType.Eat:
            base_reward += 3.0
        
        # Novelty bonus
        if self.agent.getNoveltyLevel() > 0.5:
            base_reward += 0.5
        
        # Development stage bonus (better in later stages)
        stage = self.brain.getDevelopmentalStage()
        if stage.name == 'Adult':
            base_reward += 0.2
        
        # Time-dependent reward (encourage exploration early)
        if step < 100:
            base_reward += 0.1
            
        return base_reward
    
    def _predict_reward(self, action, percept):
        """Predict expected reward using neural predictions."""
        # Simple prediction based on action history
        predicted = 1.0  # Default prediction
        
        # Adjust based on action type
        if action.getType() == pynlm.ActionType.MoveForward:
            predicted = 0.8
        elif action.getType() == pynlm.ActionType.Interact:
            predicted = 1.5
        elif action.getType() == pynlm.ActionType.Eat:
            predicted = 2.0
            
        # Add noise based on novelty
        predicted += (self.agent.getNoveltyLevel() - 0.5) * 0.3
        
        return max(0.0, predicted)
    
    def _store_memory_trace(self, step, percept, action, reward):
        """Store detailed memory trace for later replay."""
        trace = {
            'step': step,
            'vision': percept.getVision(),
            'action': action.getType(),
            'reward': reward,
            'novelty': self.agent.getNoveltyLevel(),
            'curiosity': self.agent.getCuriosityLevel(),
            'prediction_error': self.agent.getPredictionError()
        }
        self.memory_traces.append(trace)
    
    def _log_episode_stats(self, step, reward, prediction_error):
        """Log detailed episode statistics."""
        print(f"Step {step}: Reward={reward:.2f}, "
              f"Novelty={self.agent.getNoveltyLevel():.3f}, "
              f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
              f"Prediction Error={prediction_error:.3f}, "
              f"Dev Stage={self.brain.getDevelopmentalStage().name}")

class MemoryConsolidationExample:
    """
    Example demonstrating advanced memory consolidation and replay mechanisms.
    
    This example showcases:
    - Episodic memory formation and consolidation
    - Systematic memory replay for learning
    - Working memory capacity management
    - Memory integration with neural plasticity
    """
    
    def __init__(self):
        """Initialize memory consolidation system."""
        self.config = pynlm.createDefaultConfig()
        
        # Configuration optimized for memory-intensive tasks
        self.config.set("brain.neuron_count", 3000)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("neuromod.curiosity.enable", False)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("working_memory.capacity", 500)
        self.config.set("episodic_memory.max_episodes", 5000)
        self.config.set("replay_interval", 100)
        self.config.set("consolidation_interval", 1000)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(False)  # Focus on memory learning
        
        self.memory_stats = {
            'working_memory_entries': [],
            'episodic_episodes': [],
            'consolidation_events': 0,
            'replay_events': 0,
            'memory_efficiency': 0.0
        }
        
    def run_memory_learning_session(self, duration_steps=5000):
        """
        Run a dedicated memory learning session.
        
        Args:
            duration_steps: Total simulation steps for learning
            
        Returns:
            Dictionary with memory system statistics
        """
        print("Starting memory learning session...")
        
        for step in range(duration_steps):
            # Simple deterministic world for controlled memory learning
            self.brain.step(step)
            
            # Store neural activity in working memory
            firing_count = self.brain.getFiringNeuronCount()
            if firing_count > 0:
                # Working memory stores active neuron patterns
                self.memory_stats['working_memory_entries'].append({
                    'step': step,
                    'active_neurons': firing_count,
                    'firing_rate': self.brain.getAverageFiringRate()
                })
            
            # Create episodic memory from neural patterns
            if step % 10 == 0:
                self._create_episodic_memory(step)
            
            # Periodic consolidation and replay
            if step % 100 == 0:
                self._consolidate_memory(step)
            
            if step % 50 == 0:
                self._replay_memory(step)
            
            # Log progress
            if step % 1000 == 0:
                self._log_memory_stats(step)
        
        return self._get_final_memory_stats()
    
    def _create_episodic_memory(self, step):
        """Create new episodic memory from current brain state."""
        # Capture neural activity pattern
        active_neurons = []
        neuron_activations = []
        
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                # Get population data
                for population in region.getPopulations():
                    for neuron in population.getNeurons():
                        state = neuron.getState()
                        if state.membranePotential > -55:  # Active threshold
                            active_neurons.append(neuron.getId().index())
                            activation = abs(state.membranePotential - state.restingPotential) / 20.0
                            neuron_activations.append(activation)
        
        episode = {
            'step': step,
            'active_neurons': active_neurons,
            'neuron_activations': neuron_activations,
            'timestamp': time.time()
        }
        
        self.memory_stats['episodic_episodes'].append(episode)
    
    def _consolidate_memory(self, step):
        """Consolidate and integrate memory traces."""
        self.memory_stats['consolidation_events'] += 1
        
        # Calculate memory consolidation metrics
        total_episodes = len(self.memory_stats['episodic_episodes'])
        if total_episodes > 0:
            # Calculate memory efficiency (episode retention)
            recent_episodes = self.memory_stats['episodic_episodes'][-100:]
            avg_neurons_per_episode = sum(len(ep['active_neurons']) for ep in recent_episodes) / len(recent_episodes) if recent_episodes else 0
            self.memory_stats['memory_efficiency'] = avg_neurons_per_episode / 1000.0  # Normalize
    
    def _replay_memory(self, step):
        """Replay important memories for reinforcement."""
        self.memory_stats['replay_events'] += 1
        
        # Find high-importance episodes for replay
        if len(self.memory_stats['episodic_episodes']) > 100:
            # Select episodes with high activation patterns
            episodes = self.memory_stats['episodic_episodes']
            high_importance = episodes[-50:]  # Recent episodes
            
            # Replay would integrate with neural plasticity here
            # In real implementation, would reactivate neural patterns
    
    def _log_memory_stats(self, step):
        """Log memory system statistics."""
        wm_size = len(self.memory_stats['working_memory_entries'])
        ep_size = len(self.memory_stats['episodic_episodes'])
        
        print(f"Memory System Progress - Step {step}:")
        print(f"  Working memory traces: {wm_size}")
        print(f"  Episodic episodes: {ep_size}")
        print(f"  Consolidation events: {self.memory_stats['consolidation_events']}")
        print(f"  Replay events: {self.memory_stats['replay_events']}")
        print(f"  Memory efficiency: {self.memory_stats['memory_efficiency']:.3f}")
    
    def _get_final_memory_stats(self):
        """Get final memory system statistics."""
        return {
            'total_working_memory_traces': len(self.memory_stats['working_memory_entries']),
            'total_episodic_episodes': len(self.memory_stats['episodic_episodes']),
            'consolidation_events': self.memory_stats['consolidation_events'],
            'replay_events': self.memory_stats['replay_events'],
            'final_memory_efficiency': self.memory_stats['memory_efficiency'],
            'avg_working_memory_per_step': len(self.memory_stats['working_memory_entries']) / 5000,
            'memory_density': len(self.memory_stats['episodic_episodes']) / 5000
        }

class NeuromodulationExample:
    """
    Advanced example demonstrating neuromodulation and reward-based learning.
    
    This example showcases:
    - Dopamine and serotonin systems
    - Curiosity and novelty detection
    - Reward prediction error optimization
    - Decision making under uncertainty
    """
    
    def __init__(self):
        """Initialize neuromodulation system."""
        self.config = pynlm.createDefaultConfig()
        
        # Configuration optimized for neuromodulation
        self.config.set("brain.neuron_count", 1500)
        self.config.set("neuromod.dopamine.scale", 3.0)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.stdp.learning_rate", 0.002)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        self.neuromod_stats = {
            'dopamine_levels': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_errors': [],
            'reward_predictions': [],
            'learning_rates': []
        }
        
    def run_neuromodulation_study(self, duration_steps=3000):
        """
        Run comprehensive neuromodulation study.
        
        Args:
            duration_steps: Number of simulation steps
            
        Returns:
            Dictionary with neuromodulation analysis
        """
        print("Starting neuromodulation study...")
        
        for step in range(duration_steps):
            # Progress through development stages
            if step % 500 == 0:
                stage = self.brain.getDevelopmentalStage()
                if stage.name == 'Initial':
                    self.brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
                elif stage.name == 'CriticalPeriod':
                    self.brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
                elif stage.name == 'Maturation':
                    self.brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
            
            # Simulate different environmental conditions
            environment_difficulty = self._get_environment_difficulty(step)
            reward_schedule = self._get_reward_schedule(step)
            
            # Process sensory input
            percept = self._generate_sensory_input(step, environment_difficulty)
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            
            # Decision making with neuromodulation
            action = self._make_neuromodulated_decision(step, percept)
            
            # Calculate reward with environmental factors
            reward = self._calculate_environment_reward(reward_schedule, environment_difficulty)
            predicted_reward = self._predict_environment_reward(action, environment_difficulty)
            
            # Apply neuromodulation
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development based on learning progress
            if step % 100 == 0:
                self.agent.updateDevelopment(0.1)
            
            # Record neuromodulation metrics
            self._record_neuromod_metrics(step, reward, predicted_reward, environment_difficulty)
            
            # Log progress
            if step % 1000 == 0:
                self._log_neuromod_stats(step)
        
        return self._analyze_neuromodulation_results()
    
    def _get_environment_difficulty(self, step):
        """Generate dynamic environment difficulty."""
        difficulty = 1.0  # Base difficulty
        
        # Periodically increase difficulty
        if step % 1000 == 0:
            difficulty += 0.5
        
        # Add random variation
        difficulty += (np.random.random() - 0.5) * 0.2
        
        return max(0.5, min(2.0, difficulty))
    
    def _get_reward_schedule(self, step):
        """Generate reward schedule with temporal patterns."""
        # Pulsed reward schedule
        if step % 500 == 0:
            return 5.0  # Large reward pulse
        elif step % 200 == 0:
            return 2.0  # Medium reward
        else:
            return 0.1  # Small continuous reward
    
    def _generate_sensory_input(self, step, difficulty):
        """Generate sensory input with environmental complexity."""
        # Create vision-like sensory input
        vision_data = []
        for _ in range(64):  # 8x8 vision grid
            # Base value with noise based on difficulty
            base_value = np.random.random() * difficulty
            # Add temporal pattern
            temporal_factor = 0.5 + 0.5 * np.sin(step * 0.01)
            vision_data.append(base_value * temporal_factor)
        
        # Create vision sensory input (simplified)
        vision = pynlm.Vision(8, 8, 1)
        vision.setData(vision_data)
        
        return vision
    
    def _make_neuromodulated_decision(self, step, percept):
        """Make decisions influenced by neuromodulators."""
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = self.agent.getPredictionError()
        
        # Decision bias based on neuromodulators
        move_probability = 0.5  # Base movement probability
        
        # Increase exploration with curiosity
        if curiosity > 0.7:
            move_probability += 0.3
        
        # Increase novelty seeking with novelty
        if novelty > 0.6:
            move_probability += 0.2
        
        # Reduce movement with high prediction error
        if prediction_error > 1.0:
            move_probability -= 0.2
        
        # Ensure valid probability range
        move_probability = max(0.1, min(0.9, move_probability))
        
        # Make decision
        if np.random.random() < move_probability:
            action = pynlm.Action(pynlm.ActionType.MoveForward)
        else:
            action = pynlm.Action(pynlm.ActionType.Wait)
        
        return action
    
    def _calculate_environment_reward(self, reward_schedule, difficulty):
        """Calculate environment-dependent reward."""
        # Base reward plus difficulty modulation
        base_reward = reward_schedule
        difficulty_factor = 1.0 / difficulty  # Harder environments give more reward
        
        return base_reward * difficulty_factor
    
    def _predict_environment_reward(self, action, difficulty):
        """Predict expected reward based on action and environment."""
        base_prediction = 1.0
        
        # Action-based prediction
        if action.getType() == pynlm.ActionType.MoveForward:
            base_prediction = 0.8
        elif action.getType() == pynlm.ActionType.Wait:
            base_prediction = 0.1
        
        # Difficulty-based prediction
        prediction = base_prediction * (2.0 - difficulty)
        
        return max(0.0, prediction)
    
    def _record_neuromod_metrics(self, step, reward, predicted_reward, difficulty):
        """Record neuromodulation system metrics."""
        self.neuromod_stats['dopamine_levels'].append(self.agent.getNeuromodulationLevel())
        self.neuromod_stats['curiosity_levels'].append(self.agent.getCuriosityLevel())
        self.neuromod_stats['novelty_levels'].append(self.agent.getNoveltyLevel())
        self.neuromod_stats['prediction_errors'].append(self.agent.getPredictionError())
        self.neuromod_stats['reward_predictions'].append(predicted_reward)
        self.neuromod_stats['learning_rates'].append(
            self.brain.getSTDP().getLTPWeight() if hasattr(self.brain, 'getSTDP') else 0.001
        )
    
    def _log_neuromod_stats(self, step):
        """Log neuromodulation system statistics."""
        avg_dopamine = np.mean(self.neuromod_stats['dopamine_levels']) if self.neuromod_stats['dopamine_levels'] else 0
        avg_curiosity = np.mean(self.neuromod_stats['curiosity_levels']) if self.neuromod_stats['curiosity_levels'] else 0
        avg_novelty = np.mean(self.neuromod_stats['novelty_levels']) if self.neuromod_stats['novelty_levels'] else 0
        avg_prediction_error = np.mean(self.neuromod_stats['prediction_errors']) if self.neuromod_stats['prediction_errors'] else 0
        
        print(f"Neuromodulation Stats - Step {step}:")
        print(f"  Average dopamine: {avg_dopamine:.3f}")
        print(f"  Average curiosity: {avg_curiosity:.3f}")
        print(f"  Average novelty: {avg_novelty:.3f}")
        print(f"  Average prediction error: {avg_prediction_error:.3f}")
        print(f"  Learning rate: {np.mean(self.neuromod_stats['learning_rates']) if self.neuromod_stats['learning_rates'] else 0:.6f}")
    
    def _analyze_neuromodulation_results(self):
        """Analyze neuromodulation system performance."""
        return {
            'final_dopamine_level': np.mean(self.neuromod_stats['dopamine_levels']) if self.neuromod_stats['dopamine_levels'] else 0,
            'final_curiosity_level': np.mean(self.neuromod_stats['curiosity_levels']) if self.neuromod_stats['curiosity_levels'] else 0,
            'final_novelty_level': np.mean(self.neuromod_stats['novelty_levels']) if self.neuromod_stats['novelty_levels'] else 0,
            'average_prediction_error': np.mean(self.neuromod_stats['prediction_errors']) if self.neuromod_stats['prediction_errors'] else 0,
            'reward_prediction_accuracy': self._calculate_prediction_accuracy(),
            'learning_rate_stability': self._calculate_learning_stability(),
            'exploration_exploitation_balance': self._calculate_balance_metric()
        }
    
    def _calculate_prediction_accuracy(self):
        """Calculate reward prediction accuracy."""
        if len(self.neuromod_stats['reward_predictions']) < 2:
            return 0.0
        
        # This would require tracking actual vs predicted rewards
        # For now, return a placeholder based on prediction error variance
        pred_errors = np.array(self.neuromod_stats['prediction_errors'])
        return 1.0 / (1.0 + np.std(pred_errors))
    
    def _calculate_learning_stability(self):
        """Calculate learning rate stability."""
        if len(self.neuromod_stats['learning_rates']) < 2:
            return 0.0
        
        learning_rates = np.array(self.neuromod_stats['learning_rates'])
        return 1.0 / (1.0 + np.std(learning_rates))
    
    def _calculate_balance_metric(self):
        """Calculate exploration-exploitation balance."""
        avg_curiosity = np.mean(self.neuromod_stats['curiosity_levels']) if self.neuromod_stats['curiosity_levels'] else 0
        avg_prediction_error = np.mean(self.neuromod_stats['prediction_errors']) if self.neuromod_stats['prediction_errors'] else 0
        
        return (avg_curiosity + (1.0 - min(avg_prediction_error, 1.0))) / 2.0
