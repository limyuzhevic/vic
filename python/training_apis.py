# Advanced Training and Evaluation APIs for NLM

## Overview

This module provides advanced APIs for training, evaluating, and optimizing NLM agents with enhanced capabilities for research, development, and experimentation.

## Core Training APIs

### 1. Advanced Agent Training Class

```python
from nlm.training import AdvancedAgentTrainer, TrainingConfig, EvaluationMetrics
import pynlm
import numpy as np

class AdvancedAgentTrainer:
    """
    Advanced agent training system with curriculum learning, hyperparameter optimization,
    and comprehensive performance tracking.
    """
    
    def __init__(self, base_config=None, world_template=None):
        """
        Initialize the advanced trainer.
        
        Args:
            base_config: Base configuration for all training sessions
            world_template: Template world configuration
        """
        self.base_config = base_config or pynlm.createDefaultConfig()
        self.world_template = world_template
        self.training_history = []
        self.checkpoint_manager = CheckpointManager()
        self.performance_analyzer = PerformanceAnalyzer()
        
    def create_training_config(self, **kwargs):
        """
        Create a comprehensive training configuration.
        
        Args:
            **kwargs: Training configuration parameters
            
        Returns:
            TrainingConfig object
        """
        config = TrainingConfig()
        
        # Basic training parameters
        config.max_episodes = kwargs.get('max_episodes', 1000)
        config.max_steps_per_episode = kwargs.get('max_steps_per_episode', 500)
        config.training_timestep = kwargs.get('training_timestep', 0.1)
        config.evaluation_interval = kwargs.get('evaluation_interval', 50)
        config.checkpoint_interval = kwargs.get('checkpoint_interval', 100)
        
        # Learning system configuration
        config.enable_reward_modulation = kwargs.get('enable_reward_modulation', True)
        config.enable_curiosity = kwargs.get('enable_curiosity', True)
        config.enable_structural_plasticity = kwargs.get('enable_structural_plasticity', True)
        config.enable_development = kwargs.get('enable_development', True)
        
        # Neuromodulation configuration
        config.dopamine_scale = kwargs.get('dopamine_scale', 1.0)
        config.curiosity_threshold = kwargs.get('curiosity_threshold', 0.5)
        config.novelty_threshold = kwargs.get('novelty_threshold', 0.3)
        
        # Exploration-exploitation configuration
        config.epsilon_start = kwargs.get('epsilon_start', 1.0)
        config.epsilon_end = kwargs.get('epsilon_end', 0.1)
        config.epsilon_decay = kwargs.get('epsilon_decay', 'linear')
        config.epsilon_decay_rate = kwargs.get('epsilon_decay_rate', 0.001)
        
        # Curriculum learning configuration
        config.use_curriculum = kwargs.get('use_curriculum', True)
        config.curriculum_stages = kwargs.get('curriculum_stages', [
            {'episode_start': 0, 'world_size': 10, 'vision_range': 3},
            {'episode_start': 100, 'world_size': 15, 'vision_range': 4},
            {'episode_start': 300, 'world_size': 20, 'vision_range': 5},
            {'episode_start': 600, 'world_size': 25, 'vision_range': 6},
        ])
        
        # Performance and evaluation
        config.use_success_criteria = kwargs.get('use_success_criteria', True)
        config.success_threshold = kwargs.get('success_threshold', 0.8)
        config.patience = kwargs.get('patience', 50)
        config.min_delta = kwargs.get('min_delta', 0.01)
        
        # Recording and analysis
        config.record_metrics = kwargs.get('record_metrics', True)
        config.metrics_to_record = kwargs.get('metrics_to_record', [
            'reward', 'curiosity', 'novelty', 'firing_rate', 
            'survival_time', 'exploration_rate', 'exploitation_rate'
        ])
        
        # Advanced features
        config.use_meta_learning = kwargs.get('use_meta_learning', False)
        config.use_reinforcement_learning = kwargs.get('use_reinforcement_learning', True)
        config.use_adaptive_controls = kwargs.get('use_adaptive_controls', True)
        
        return config
    
    def train_agent(self, config, world, agent=None, callback=None):
        """
        Train an agent with advanced training capabilities.
        
        Args:
            config: Training configuration
            world: Training environment
            agent: Agent to train (will be created if None)
            callback: Callback function for training progress
            
        Returns:
            dict: Training results and metrics
        """
        print(f"Starting advanced training with {config.max_episodes} episodes...")
        
        # Initialize training
        training_session = {
            'config': config,
            'world': world,
            'start_time': time.time(),
            'episode_rewards': [],
            'episode_curiosity': [],
            'episode_novelty': [],
            'episode_firing_rates': [],
            'episode_survival_times': [],
            'best_performance': -float('inf'),
            'best_episode': -1,
            'no_improvement_count': 0,
            'learning_curve': [],
        }
        
        # Setup agent if not provided
        if agent is None:
            brain = pynlm.createBrain(config.to_brain_config())
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
        
        # Setup world if needed
        if self.world_template:
            world.configure(**self.world_template)
        
        # Enable learning systems
        self._setup_learning_systems(agent, config)
        
        # Training loop
        for episode in range(config.max_episodes):
            episode_result = self._run_training_episode(
                agent, world, episode, config
            )
            
            # Record results
            training_session['episode_rewards'].append(episode_result['final_reward'])
            training_session['episode_curiosity'].append(episode_result['avg_curiosity'])
            training_session['episode_novelty'].append(episode_result['avg_novelty'])
            training_session['episode_firing_rates'].append(episode_result['avg_firing_rate'])
            training_session['episode_survival_times'].append(episode_result['survival_time'])
            
            # Update best performance
            if episode_result['final_reward'] > training_session['best_performance']:
                training_session['best_performance'] = episode_result['final_reward']
                training_session['best_episode'] = episode
                training_session['no_improvement_count'] = 0
                
                # Save best model
                self.checkpoint_manager.save_best_model(
                    agent, world, f"best_episode_{episode}"
                )
            else:
                training_session['no_improvement_count'] += 1
            
            # Record learning curve
            training_session['learning_curve'].append({
                'episode': episode,
                'reward': episode_result['final_reward'],
                'curiosity': episode_result['avg_curiosity'],
                'novelty': episode_result['avg_novelty'],
                'firing_rate': episode_result['avg_firing_rate'],
                'survival_time': episode_result['survival_time'],
            })
            
            # Call callback if provided
            if callback:
                callback(training_session, episode_result)
            
            # Save checkpoint
            if (episode + 1) % config.checkpoint_interval == 0:
                self.checkpoint_manager.save_checkpoint(
                    agent, world, f"checkpoint_episode_{episode}"
                )
            
            # Evaluate performance
            if (episode + 1) % config.evaluation_interval == 0:
                evaluation_results = self._evaluate_agent(agent, world, config)
                training_session['evaluation_results'] = evaluation_results
                
                # Early stopping if no improvement
                if training_session['no_improvement_count'] >= config.patience:
                    print(f"Early stopping triggered after {episode + 1} episodes")
                    break
            
            # Curriculum learning
            if config.use_curriculum:
                self._update_curriculum(world, episode, config)
            
            # Adaptive controls
            if config.use_adaptive_controls:
                self._apply_adaptive_controls(agent, episode, config)
        
        training_session['end_time'] = time.time()
        training_session['total_training_time'] = training_session['end_time'] - training_session['start_time']
        
        # Final evaluation
        final_evaluation = self._evaluate_agent(agent, world, config)
        training_session['final_evaluation'] = final_evaluation
        
        # Analyze training results
        training_session['analysis'] = self.performance_analyzer.analyze_training(
            training_session
        )
        
        # Save training session
        self.training_history.append(training_session)
        
        return training_session
    
    def _setup_learning_systems(self, agent, config):
        """Setup and configure learning systems."""
        # Configure neuromodulation
        if config.enable_reward_modulation:
            agent.enableRewardModulation(True)
            
        if config.enable_curiosity:
            agent.enableCuriosity(True)
            
        if config.enable_structural_plasticity:
            agent.enableStructuralPlasticity(True)
            
        if config.enable_development:
            agent.enableDevelopment(True)
        
        # Set neuromodulation levels
        # This would require additional API calls to configure neuromodulators
    
    def _run_training_episode(self, agent, world, episode, config):
        """Run a single training episode."""
        # Reset world and agent
        world.reset()
        agent.reset()
        
        # Apply curriculum world settings if needed
        if config.use_curriculum:
            self._apply_curriculum_settings(world, episode, config)
        
        # Episode tracking
        episode_stats = {
            'steps': 0,
            'total_reward': 0.0,
            'curiosity_values': [],
            'novelty_values': [],
            'firing_rates': [],
            'actions_taken': [],
            'rewards_received': [],
        }
        
        for step in range(config.max_steps_per_episode):
            # Get current time for curriculum
            current_time = world.getSimulationTime()
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Get brain step
            brain = agent.getBrain()
            brain.step(step, current_time)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, current_time)
            
            # Get reward and apply neuromodulation
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            predicted = agent.getPredictionError()
            
            agent.applyRewardModulation(reward, predicted)
            
            # Update development
            agent.updateDevelopment(config.training_timestep)
            
            # Record statistics
            episode_stats['steps'] += 1
            episode_stats['total_reward'] += reward
            episode_stats['curiosity_values'].append(agent.getCuriosityLevel())
            episode_stats['novelty_values'].append(agent.getNoveltyLevel())
            episode_stats['firing_rates'].append(brain.getAverageFiringRate())
            episode_stats['actions_taken'].append(action.getType())
            episode_stats['rewards_received'].append(reward)
            
            # Check if episode ended
            agent_body = world.getAgentBody()
            if agent_body.health <= 0 or step >= config.max_steps_per_episode - 1:
                break
        
        # Calculate episode statistics
        episode_result = {
            'episode': episode,
            'final_reward': episode_stats['total_reward'],
            'avg_curiosity': np.mean(episode_stats['curiosity_values']) if episode_stats['curiosity_values'] else 0.0,
            'avg_novelty': np.mean(episode_stats['novelty_values']) if episode_stats['novelty_values'] else 0.0,
            'avg_firing_rate': np.mean(episode_stats['firing_rates']) if episode_stats['firing_rates'] else 0.0,
            'survival_time': episode_stats['steps'] * config.training_timestep,
            'actions_taken': episode_stats['actions_taken'],
            'rewards_received': episode_stats['rewards_received'],
            'total_steps': episode_stats['steps'],
        }
        
        return episode_result
    
    def _evaluate_agent(self, agent, world, config):
        """Evaluate agent performance."""
        print(f"Evaluating agent performance...")
        
        evaluation_results = {
            'reward_statistics': {},
            'curiosity_statistics': {},
            'novelty_statistics': {},
            'neural_activity_statistics': {},
            'behavioral_patterns': {},
            'success_criteria': {},
        }
        
        # Run multiple evaluation episodes
        num_eval_episodes = min(10, config.max_episodes // 10)
        
        all_rewards = []
        all_curiosity = []
        all_novelty = []
        all_firing_rates = []
        
        for episode in range(num_eval_episodes):
            # Run evaluation episode
            eval_result = self._run_training_episode(agent, world, episode + config.max_episodes, config)
            
            all_rewards.append(eval_result['final_reward'])
            all_curiosity.append(eval_result['avg_curiosity'])
            all_novelty.append(eval_result['avg_novelty'])
            all_firing_rates.append(eval_result['avg_firing_rate'])
            
            # Reset for next evaluation episode
            world.reset()
            agent.reset()
        
        # Calculate statistics
        evaluation_results['reward_statistics'] = {
            'mean': np.mean(all_rewards),
            'std': np.std(all_rewards),
            'min': np.min(all_rewards),
            'max': np.max(all_rewards),
            'median': np.median(all_rewards),
            'percentile_95': np.percentile(all_rewards, 95),
            'success_rate': len([r for r in all_rewards if r > config.success_threshold]) / len(all_rewards),
        }
        
        evaluation_results['curiosity_statistics'] = {
            'mean': np.mean(all_curiosity),
            'std': np.std(all_curiosity),
            'coefficient_of_variation': np.std(all_curiosity) / (np.mean(all_curiosity) + 1e-6),
        }
        
        evaluation_results['novelty_statistics'] = {
            'mean': np.mean(all_novelty),
            'std': np.std(all_novelty),
            'exploration_index': np.mean(all_novelty),  # Simplified exploration measure
        }
        
        evaluation_results['neural_activity_statistics'] = {
            'mean_firing_rate': np.mean(all_firing_rates),
            'firing_rate_variance': np.var(all_firing_rates),
            'stability': 1.0 / (1.0 + np.var(all_firing_rates)),
        }
        
        # Analyze behavioral patterns
        evaluation_results['behavioral_patterns'] = self._analyze_behavioral_patterns(
            all_rewards, all_curiosity, all_novelty
        )
        
        # Check success criteria
        evaluation_results['success_criteria'] = self._check_success_criteria(
            evaluation_results, config
        )
        
        return evaluation_results
    
    def _analyze_behavioral_patterns(self, rewards, curiosity, novelty):
        """Analyze behavioral patterns in agent behavior."""
        patterns = {}
        
        # Reward pattern analysis
        if len(rewards) > 1:
            reward_changes = np.diff(rewards)
            patterns['reward_volatility'] = np.std(reward_changes)
            patterns['reward_trend'] = np.polyfit(range(len(rewards)), rewards, 1)[0]
            patterns['reward_autocorrelation'] = np.corrcoef(rewards[:-1], rewards[1:])[0, 1]
        
        # Curiosity pattern analysis
        if len(curiosity) > 1:
            curiosity_changes = np.diff(curiosity)
            patterns['curiosity_volatility'] = np.std(curiosity_changes)
            patterns['curiosity_steadiness'] = 1.0 - np.std(curiosity) / (np.mean(curiosity) + 1e-6)
        
        # Novelty pattern analysis
        if len(novelty) > 1:
            novelty_trend = np.polyfit(range(len(novelty)), novelty, 1)[0]
            patterns['novelty_trend'] = novelty_trend
            patterns['novelty_consistency'] = 1.0 / (1.0 + np.std(novelty))
        
        # Exploration-exploitation balance
        if len(rewards) > 5 and len(curiosity) > 5:
            # Simple heuristic: higher curiosity relative to reward suggests more exploration
            exploration_ratio = np.mean(curiosity) / (np.mean(rewards) + 1e-6)
            patterns['exploration_exploitation_ratio'] = exploration_ratio
            
            if exploration_ratio < 0.5:
                patterns['strategy'] = 'exploitation-dominant'
            elif exploration_ratio < 1.5:
                patterns['strategy'] = 'balanced'
            else:
                patterns['strategy'] = 'exploration-dominant'
        
        return patterns
    
    def _check_success_criteria(self, evaluation_results, config):
        """Check if training meets success criteria."""
        criteria = {}
        
        # Reward-based criteria
        avg_reward = evaluation_results['reward_statistics']['mean']
        reward_std = evaluation_results['reward_statistics']['std']
        
        criteria['meets_reward_threshold'] = avg_reward > config.success_threshold
        criteria['reward_consistency'] = avg_reward > config.success_threshold * 0.8
        criteria['reward_stability'] = reward_std < avg_reward * 0.5  # Less than 50% CV
        
        # Learning criteria
        curiosity_mean = evaluation_results['curiosity_statistics']['mean']
        novelty_mean = evaluation_results['novelty_statistics']['mean']
        
        criteria['sufficient_exploration'] = curiosity_mean > 0.1
        criteria['adequate_novelty_detection'] = novelty_mean > 0.05
        
        # Neural activity criteria
        firing_rate = evaluation_results['neural_activity_statistics']['mean_firing_rate']
        stability = evaluation_results['neural_activity_statistics']['stability']
        
        criteria['healthy_neural_activity'] = firing_rate > 0.1 and stability > 0.5
        criteria['stable_dynamics'] = stability > 0.7
        
        # Overall success
        success_count = sum(criteria.values())
        total_criteria = len(criteria)
        criteria['overall_success'] = success_count >= total_criteria * 0.6  # At least 60% success
        
        return criteria
    
    def _update_curriculum(self, world, episode, config):
        """Update curriculum based on training progress."""
        if not config.use_curriculum:
            return
        
        for stage in config.curriculum_stages:
            if episode >= stage['episode_start']:
                world.configure(
                    width=stage['world_size'],
                    height=stage['world_size'],
                    visionWidth=stage['vision_range'],
                    visionHeight=stage['vision_range']
                )
                break
    
    def _apply_curriculum_settings(self, world, episode, config):
        """Apply curriculum settings for current episode."""
        for stage in config.curriculum_stages:
            if episode >= stage['episode_start']:
                world.configure(
                    width=stage['world_size'],
                    height=stage['world_size'],
                    visionWidth=stage['vision_range'],
                    visionHeight=stage['vision_range']
                )
                break
    
    def _apply_adaptive_controls(self, agent, episode, config):
        """Apply adaptive controls during training."""
        if not config.use_adaptive_controls:
            return
        
        # Simple adaptive control logic
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        # Adjust neuromodulation levels based on performance
        if curiosity > 0.8:
            # Reduce curiosity if too high (risk of overfitting to exploration)
            agent.enableCuriosity(False)
        elif curiosity < 0.2:
            # Increase curiosity if too low (need more exploration)
            agent.enableCuriosity(True)
        
        if novelty > 0.7:
            # Reduce novelty detection if too high
            agent.enableNovelty(False)
        elif novelty < 0.3:
            # Increase novelty detection if too low
            agent.enableNovelty(True)

class TrainingConfig:
    """
    Comprehensive training configuration for NLM agents.
    """
    
    def __init__(self):
        self.max_episodes = 1000
        self.max_steps_per_episode = 500
        self.training_timestep = 0.1
        self.evaluation_interval = 50
        self.checkpoint_interval = 100
        
        # Learning system flags
        self.enable_reward_modulation = True
        self.enable_curiosity = True
        self.enable_structural_plasticity = True
        self.enable_development = True
        
        # Neuromodulation settings
        self.dopamine_scale = 1.0
        self.curiosity_threshold = 0.5
        self.novelty_threshold = 0.3
        
        # Exploration-exploitation
        self.epsilon_start = 1.0
        self.epsilon_end = 0.1
        self.epsilon_decay = 'linear'
        self.epsilon_decay_rate = 0.001
        
        # Curriculum learning
        self.use_curriculum = True
        self.curriculum_stages = []
        
        # Performance
        self.use_success_criteria = True
        self.success_threshold = 0.8
        self.patience = 50
        self.min_delta = 0.01
        
        # Recording
        self.record_metrics = True
        self.metrics_to_record = []
        
        # Advanced features
        self.use_meta_learning = False
        self.use_reinforcement_learning = True
        self.use_adaptive_controls = True
    
    def to_brain_config(self):
        """Convert to brain configuration."""
        import json
        from pathlib import Path
        
        # Create brain configuration dictionary
        brain_config_dict = {
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'v_thresh': -50.0,
                'v_rest': -70.0,
                'v_reset': -75.0,
                'tau_mem': 20.0,
                'tau_ref': 2.0,
            },
            'plasticity': {
                'stdp': {
                    'enable': True,
                    'learning_rate': 0.001,
                    'tau_plus': 20.0,
                    'tau_minus': 20.0,
                },
                'hebbian': {
                    'enable': True,
                },
                'structural': {
                    'enable': self.enable_structural_plasticity,
                },
            },
            'neuromod': {
                'dopamine': {
                    'scale': self.dopamine_scale,
                },
                'curiosity': {
                    'enable': self.enable_curiosity,
                },
                'novelty': {
                    'enable': True,
                },
            },
            'development': {
                'enable': self.enable_development,
            },
        }
        
        # Save to file
        config_dir = Path('configs')
        config_dir.mkdir(exist_ok=True)
        
        config_file = config_dir / f'training_config_{int(time.time())}.json'
        with open(config_file, 'w') as f:
            json.dump(brain_config_dict, f, indent=2)
        
        return brain_config_dict

class EvaluationMetrics:
    """
    Comprehensive evaluation metrics for NLM agents.
    """
    
    @staticmethod
    def calculate_comprehensive_metrics(agent, world, num_episodes=10):
        """
        Calculate comprehensive evaluation metrics.
        
        Args:
            agent: Trained agent
            world: Evaluation environment
            num_episodes: Number of evaluation episodes
            
        Returns:
            dict: Comprehensive evaluation metrics
        """
        metrics = {
            'reward_metrics': {},
            'curiosity_metrics': {},
            'novelty_metrics': {},
            'neural_metrics': {},
            'behavioral_metrics': {},
            'performance_rating': {},
        }
        
        # Collect evaluation data
        all_rewards = []
        all_curiosity = []
        all_novelty = []
        all_firing_rates = []
        all_survival_times = []
        
        for episode in range(num_episodes):
            # Run evaluation episode
            eval_result = AdvancedAgentTrainer._run_single_episode(agent, world)
            
            all_rewards.append(eval_result['final_reward'])
            all_curiosity.append(eval_result['avg_curiosity'])
            all_novelty.append(eval_result['avg_novelty'])
            all_firing_rates.append(eval_result['avg_firing_rate'])
            all_survival_times.append(eval_result['survival_time'])
            
            # Reset for next episode
            world.reset()
            agent.reset()
        
        # Reward metrics
        metrics['reward_metrics'] = {
            'mean': np.mean(all_rewards),
            'std': np.std(all_rewards),
            'median': np.median(all_rewards),
            'min': np.min(all_rewards),
            'max': np.max(all_rewards),
            'percentile_25': np.percentile(all_rewards, 25),
            'percentile_50': np.percentile(all_rewards, 50),
            'percentile_75': np.percentile(all_rewards, 75),
            'percentile_95': np.percentile(all_rewards, 95),
            'coefficient_of_variation': np.std(all_rewards) / (np.mean(all_rewards) + 1e-6),
            'skewness': stats.skew(all_rewards) if len(all_rewards) > 2 else 0,
            'kurtosis': stats.kurtosis(all_rewards) if len(all_rewards) > 3 else 0,
        }
        
        # Curiosity metrics
        metrics['curiosity_metrics'] = {
            'mean': np.mean(all_curiosity),
            'std': np.std(all_curiosity),
            'coefficient_of_variation': np.std(all_curiosity) / (np.mean(all_curiosity) + 1e-6),
            'exploration_index': np.mean(all_curiosity),
            'exploration_consistency': 1.0 - np.std(all_curiosity) / (np.mean(all_curiosity) + 1e-6),
        }
        
        # Novelty metrics
        metrics['novelty_metrics'] = {
            'mean': np.mean(all_novelty),
            'std': np.std(all_novelty),
            'novelty_detection_rate': np.mean([1 if n > 0.5 else 0 for n in all_novelty]),
            'novelty_sensitivity': np.mean(all_novelty),
        }
        
        # Neural metrics
        metrics['neural_metrics'] = {
            'mean_firing_rate': np.mean(all_firing_rates),
            'firing_rate_variance': np.var(all_firing_rates),
            'firing_rate_stability': 1.0 / (1.0 + np.var(all_firing_rates)),
            'neural_coherence': np.corrcoef(all_firing_rates[:-1], all_firing_rates[1:])[0, 1] if len(all_firing_rates) > 1 else 0,
        }
        
        # Behavioral metrics
        metrics['behavioral_metrics'] = {
            'survival_efficiency': np.mean(all_survival_times) / num_episodes,
            'action_diversity': len(set([tuple(a) for a in eval_result['actions_taken']])) / len(eval_result['actions_taken']),
            'reward_consistency': 1.0 - metrics['reward_metrics']['coefficient_of_variation'],
            'adaptation_rate': 'calculated_from_episode_data',  # Would need episode history
        }
        
        # Performance rating
        metrics['performance_rating'] = AdvancedAgentTrainer._rate_performance(metrics)
        
        return metrics
    
    @staticmethod
    def _run_single_episode(agent, world):
        """Run a single evaluation episode."""
        # Reset
        world.reset()
        agent.reset()
        
        # Episode tracking
        episode_stats = {
            'total_reward': 0.0,
            'curiosity_values': [],
            'novelty_values': [],
            'firing_rates': [],
            'actions_taken': [],
            'survival_time': 0.0,
        }
        
        for step in range(100):  # Fixed evaluation episode length
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Get brain step
            brain = agent.getBrain()
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record statistics
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            
            episode_stats['total_reward'] += reward
            episode_stats['curiosity_values'].append(agent.getCuriosityLevel())
            episode_stats['novelty_values'].append(agent.getNoveltyLevel())
            episode_stats['firing_rates'].append(brain.getAverageFiringRate())
            episode_stats['actions_taken'].append(action.getType())
            episode_stats['survival_time'] = step * 0.1
            
            # Check if episode ended
            agent_body = world.getAgentBody()
            if agent_body.health <= 0:
                break
        
        # Calculate episode statistics
        episode_result = {
            'final_reward': episode_stats['total_reward'],
            'avg_curiosity': np.mean(episode_stats['curiosity_values']) if episode_stats['curiosity_values'] else 0.0,
            'avg_novelty': np.mean(episode_stats['novelty_values']) if episode_stats['novelty_values'] else 0.0,
            'avg_firing_rate': np.mean(episode_stats['firing_rates']) if episode_stats['firing_rates'] else 0.0,
            'actions_taken': episode_stats['actions_taken'],
            'survival_time': episode_stats['survival_time'],
        }
        
        return episode_result
    
    @staticmethod
    def _rate_performance(metrics):
        """Rate overall performance based on metrics."""
        rating = {
            'overall_score': 0.0,
            'reward_performance': 0.0,
            'exploration_performance': 0.0,
            'neural_performance': 0.0,
            'behavioral_performance': 0.0,
            'grade': 'F',
        }
        
        # Calculate component scores (normalized to 0-1)
        reward_score = min(metrics['reward_metrics']['mean'] / 2.0, 1.0)  # Assume max reward ~2.0
        curiosity_score = min(metrics['curiosity_metrics']['exploration_index'], 1.0)
        firing_rate_score = min(metrics['neural_metrics']['mean_firing_rate'] / 5.0, 1.0)  # Assume max firing rate ~5Hz
        
        # Calculate overall score
        rating['reward_performance'] = reward_score
        rating['exploration_performance'] = curiosity_score
        rating['neural_performance'] = firing_rate_score
        rating['behavioral_performance'] = (reward_score + curiosity_score + firing_rate_score) / 3.0
        
        rating['overall_score'] = (
            rating['reward_performance'] * 0.4 +
            rating['exploration_performance'] * 0.3 +
            rating['neural_performance'] * 0.2 +
            rating['behavioral_performance'] * 0.1
        )
        
        # Assign grade
        if rating['overall_score'] >= 0.9:
            rating['grade'] = 'A+'
        elif rating['overall_score'] >= 0.8:
            rating['grade'] = 'A'
        elif rating['overall_score'] >= 0.7:
            rating['grade'] = 'B'
        elif rating['overall_score'] >= 0.6:
            rating['grade'] = 'C'
        elif rating['overall_score'] >= 0.5:
            rating['grade'] = 'D'
        else:
            rating['grade'] = 'F'
        
        return rating

class PerformanceAnalyzer:
    """
    Advanced performance analysis for NLM training.
    """
    
    def __init__(self):
        self.analysis_cache = {}
    
    def analyze_training(self, training_session):
        """
        Analyze training session results.
        
        Args:
            training_session: Training session data
            
        Returns:
            dict: Analysis results
        """
        analysis = {
            'training_dynamics': {},
            'performance_trajectory': {},
            'learning_insights': {},
            'optimization_recommendations': {},
        }
        
        rewards = training_session['episode_rewards']
        curiosity = training_session['episode_curiosity']
        novelty = training_session['episode_novelty']
        firing_rates = training_session['episode_firing_rates']
        
        # Analyze training dynamics
        analysis['training_dynamics'] = {
            'convergence_rate': self._calculate_convergence_rate(rewards),
            'stability_score': self._calculate_stability_score(rewards),
            'exploration_efficiency': self._calculate_exploration_efficiency(
                rewards, curiosity, novelty
            ),
            'neural_stability': self._calculate_neural_stability(firing_rates),
        }
        
        # Analyze performance trajectory
        analysis['performance_trajectory'] = {
            'reward_trend': self._analyze_reward_trend(rewards),
            'curiosity_trend': self._analyze_trend(curiosity),
            'novelty_trend': self._analyze_trend(novelty),
            'firing_rate_trend': self._analyze_trend(firing_rates),
            'episode_performance_variability': self._calculate_variability(rewards),
        }
        
        # Generate learning insights
        analysis['learning_insights'] = self._generate_learning_insights(
            analysis['training_dynamics'],
            analysis['performance_trajectory']
        )
        
        # Generate optimization recommendations
        analysis['optimization_recommendations'] = self._generate_optimization_recommendations(
            analysis['learning_insights']
        )
        
        return analysis
    
    def _calculate_convergence_rate(self, rewards):
        """Calculate how quickly the agent converges to optimal performance."""
        if len(rewards) < 10:
            return 0.0
        
        # Calculate moving average
        window_size = min(20, len(rewards) // 4)
        if window_size < 2:
            return 0.0
        
        moving_avg = np.convolve(rewards, np.ones(window_size)/window_size, mode='valid')
        
        # Calculate convergence rate (slope of final portion)
        final_portion = moving_avg[-min(10, len(moving_avg)):]
        if len(final_portion) < 2:
            return 0.0
        
        x = np.arange(len(final_portion))
        slope, _, _, _, _ = stats.linregress(x, final_portion)
        
        return max(0.0, min(slope, 1.0))  # Normalize to 0-1
    
    def _calculate_stability_score(self, rewards):
        """Calculate stability of performance."""
        if len(rewards) < 5:
            return 0.0
        
        # Coefficient of variation
        cv = np.std(rewards) / (np.mean(rewards) + 1e-6)
        
        # Stability score (inverse of CV, capped at 1.0)
        stability = max(0.0, min(1.0 - cv, 1.0))
        
        return stability
    
    def _calculate_exploration_efficiency(self, rewards, curiosity, novelty):
        """Calculate efficiency of exploration relative to reward."""
        if len(rewards) == 0 or len(curiosity) == 0:
            return 0.0
        
        # Simple metric: average curiosity per unit reward
        if np.mean(rewards) > 0:
            exploration_efficiency = np.mean(curiosity) / np.mean(rewards)
        else:
            exploration_efficiency = 0.0
        
        return min(exploration_efficiency, 1.0)
    
    def _calculate_neural_stability(self, firing_rates):
        """Calculate stability of neural activity."""
        if len(firing_rates) < 5:
            return 0.0
        
        # Calculate standard deviation of firing rates
        cv = np.std(firing_rates) / (np.mean(firing_rates) + 1e-6)
        
        # Stability score (inverse of CV)
        stability = max(0.0, min(1.0 - cv, 1.0))
        
        return stability
    
    def _analyze_reward_trend(self, rewards):
        """Analyze reward trend over time."""
        if len(rewards) < 5:
            return {'trend': 'insufficient_data', 'rate': 0.0}
        
        x = np.arange(len(rewards))
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, rewards)
        
        if p_value > 0.05:
            trend = 'no_significant_trend'
        elif slope > 0:
            trend = 'increasing'
        else:
            trend = 'decreasing'
        
        return {
            'trend': trend,
            'rate': slope,
            'r_squared': r_value ** 2,
            'significance': p_value < 0.05,
        }
    
    def _analyze_trend(self, data):
        """Analyze trend in time series data."""
        if len(data) < 3:
            return {'trend': 'insufficient_data', 'rate': 0.0}
        
        x = np.arange(len(data))
        slope, _, _, _, _ = stats.linregress(x, data)
        
        if slope > 0.01:
            trend = 'increasing'
        elif slope < -0.01:
            trend = 'decreasing'
        else:
            trend = 'stable'
        
        return {
            'trend': trend,
            'rate': slope,
        }
    
    def _calculate_variability(self, data):
        """Calculate variability in the data."""
        if len(data) < 2:
            return 0.0
        
        cv = np.std(data) / (np.mean(data) + 1e-6)
        return cv
    
    def _generate_learning_insights(self, dynamics, trajectory):
        """Generate insights about learning process."""
        insights = []
        
        # Check for learning convergence
        if dynamics['convergence_rate'] > 0.7:
            insights.append("Agent shows rapid convergence to stable performance")
        elif dynamics['convergence_rate'] > 0.3:
            insights.append("Agent shows moderate convergence")
        else:
            insights.append("Agent shows slow convergence - consider longer training")
        
        # Check for stability issues
        if dynamics['stability_score'] < 0.5:
            insights.append("Agent performance is unstable - consider regularization")
        else:
            insights.append("Agent performance is stable")
        
        # Check for exploration efficiency
        if dynamics['exploration_efficiency'] > 0.5:
            insights.append("Exploration is efficient - good balance between exploration and exploitation")
        elif dynamics['exploration_efficiency'] > 0.2:
            insights.append("Exploration could be improved - consider adjusting curiosity controls")
        else:
            insights.append("Exploration is insufficient - consider increasing curiosity")
        
        # Check for neural stability
        if dynamics['neural_stability'] > 0.7:
            insights.append("Neural dynamics are stable - good for reliable performance")
        elif dynamics['neural_stability'] > 0.4:
            insights.append("Neural dynamics could be more stable")
        else:
            insights.append("Neural dynamics are unstable - consider neural regularization")
        
        # Check performance trends
        reward_trend = trajectory['reward_trend']
        if reward_trend['trend'] == 'increasing':
            insights.append(f"Rewards are showing increasing trend at rate {reward_trend['rate']:.4f}")
        elif reward_trend['trend'] == 'decreasing':
            insights.append(f"Rewards are decreasing at rate {abs(reward_trend['rate']):.4f}")
        elif reward_trend['trend'] == 'stable':
            insights.append("Rewards are stable")
        
        return insights
    
    def _generate_optimization_recommendations(self, insights):
        """Generate recommendations for optimization."""
        recommendations = []
        
        # Analyze insights and generate recommendations
        for insight in insights:
            if "could be improved" in insight:
                recommendations.append(f"Action: {insight.replace('could be improved', 'optimize parameters')}")
            elif "could be more stable" in insight:
                recommendations.append(f"Action: {insight.replace('could be more stable', 'implement stabilization mechanisms')}")
            elif "could be improved" in insight and "exploration" in insight:
                recommendations.append(f"Action: {insight.replace('exploration', 'increase exploration parameters')}")
            elif "could be more stable" in insight and "neural" in insight:
                recommendations.append(f"Action: {insight.replace('neural', 'add neural regularization')}")
        
        if not recommendations:
            recommendations.append("Recommendation: Current performance is satisfactory - consider increasing training duration for further improvement")
        
        return recommendations

class CheckpointManager:
    """
    Advanced checkpoint management for NLM training.
    """
    
    def __init__(self, checkpoint_dir="checkpoints"):
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(exist_ok=True)
        
        # Create subdirectories
        (self.checkpoint_dir / "models").mkdir(exist_ok=True)
        (self.checkpoint_dir / "states").mkdir(exist_ok=True)
        (self.checkpoint_dir / "configs").mkdir(exist_ok=True)
        
    def save_checkpoint(self, agent, world, episode_name):
        """
        Save training checkpoint.
        
        Args:
            agent: Agent to save
            world: World to save
            episode_name: Name for the checkpoint
        """
        checkpoint_path = self.checkpoint_dir / "states" / f"{episode_name}.npy"
        
        # Create checkpoint data
        checkpoint_data = {
            'agent_brain': agent.getBrain().save(),
            'world_state': self._save_world_state(world),
            'timestamp': time.time(),
            'episode_name': episode_name,
        }
        
        # Save checkpoint
        np.save(checkpoint_path, checkpoint_data)
        
        print(f"Checkpoint saved: {checkpoint_path}")
        
        # Also save model separately
        model_path = self.checkpoint_dir / "models" / f"{episode_name}_model.h5"
        self._save_model(agent, model_path)
        
        return checkpoint_path
    
    def load_checkpoint(self, checkpoint_path):
        """
        Load training checkpoint.
        
        Args:
            checkpoint_path: Path to checkpoint file
            
        Returns:
            tuple: (agent, world) loaded from checkpoint
        """
        checkpoint_path = Path(checkpoint_path)
        
        if not checkpoint_path.exists():
            raise FileNotFoundError(f"Checkpoint not found: {checkpoint_path}")
        
        # Load checkpoint data
        checkpoint_data = np.load(checkpoint_path, allow_pickle=True).item()
        
        # Load agent brain
        agent_brain = self._load_agent_brain(checkpoint_data['agent_brain'])
        
        # Load world state
        world = self._load_world_state(checkpoint_data['world_state'])
        
        # Create agent
        agent = pynlm.createAgentBrain(agent_brain)
        
        print(f"Checkpoint loaded: {checkpoint_path}")
        
        return agent, world
    
    def save_best_model(self, agent, world, model_name):
        """
        Save best performing model.
        
        Args:
            agent: Agent to save
            world: World to save
            model_name: Name for the model
        """
        model_path = self.checkpoint_dir / "models" / f"{model_name}_best.h5"
        self._save_model(agent, model_path)
        
        print(f"Best model saved: {model_path}")
        
    def load_best_model(self, model_path):
        """
        Load best model.
        
        Args:
            model_path: Path to model file
            
        Returns:
            pynlm.AgentBrain: Loaded agent
        """
        if not Path(model_path).exists():
            raise FileNotFoundError(f"Model not found: {model_path}")
        
        # Load model
        model_data = np.load(model_path, allow_pickle=True).item()
        
        # Load agent brain
        agent_brain = self._load_agent_brain(model_data['agent_brain'])
        
        # Create agent
        agent = pynlm.createAgentBrain(agent_brain)
        
        print(f"Best model loaded: {model_path}")
        
        return agent
    
    def _save_world_state(self, world):
        """Save world state."""
        return {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'agent_start': (world.getAgentBody().x, world.getAgentBody().y) if world.getAgentBody() else (0, 0),
        }
    
    def _load_world_state(self, state_data):
        """Load world state."""
        world = pynlm.createSimpleWorld()
        world.configure(
            width=state_data['width'],
            height=state_data['height'],
            visionWidth=8,
            visionHeight=8
        )
        world.reset()
        world.setAgentStart(*state_data['agent_start'])
        return world
    
    def _save_model(self, agent, model_path):
        """Save model to file."""
        model_data = {
            'agent_brain': agent.getBrain().save(),
            'config': agent.getBrain().getConfig(),
        }
        
        np.save(model_path, model_data)
    
    def _load_agent_brain(self, brain_data):
        """Load agent brain from data."""
        # This is a simplified implementation
        # In practice, you'd need to handle deserialization properly
        import pickle
        
        with open(brain_data['file_path'], 'rb') as f:
            brain = pickle.load(f)
        
        return brain

# Example usage functions

def example_advanced_training():
    """Example of advanced training usage."""
    print("=== Advanced NLM Training Example ===")
    
    # Initialize trainer
    trainer = AdvancedAgentTrainer()
    
    # Create training configuration
    config = trainer.create_training_config(
        max_episodes=500,
        max_steps_per_episode=300,
        evaluation_interval=50,
        checkpoint_interval=100,
        use_curriculum=True,
        use_adaptive_controls=True,
        enable_reward_modulation=True,
        enable_curiosity=True,
        enable_structural_plasticity=True,
    )
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Define callback for training progress
    def training_callback(session, episode_result):
        if (session['episode'] + 1) % 50 == 0:
            print(f"Episode {session['episode'] + 1}: "
                  f"Reward: {episode_result['final_reward']:.2f}, "
                  f"Curiosity: {episode_result['avg_curiosity']:.3f}")
    
    # Train agent
    results = trainer.train_agent(
        config, world, callback=training_callback
    )
    
    # Evaluate agent
    print("\n=== Evaluating Trained Agent ===")
    evaluation_metrics = EvaluationMetrics.calculate_comprehensive_metrics(
        results['agent'], world, num_episodes=10
    )
    
    print("Evaluation Results:")
    for category, metrics in evaluation_metrics.items():
        print(f"\n{category.replace('_', ' ').title()}:")
        if isinstance(metrics, dict):
            for key, value in metrics.items():
                if isinstance(value, dict):
                    print(f"  {key}:")
                    for subkey, subvalue in value.items():
                        print(f"    {subkey}: {subvalue:.4f}")
                else:
                    print(f"  {key}: {value:.4f}")
    
    # Show performance rating
    performance_rating = evaluation_metrics['performance_rating']
    print(f"\nOverall Performance: {performance_rating['grade']} "
          f"(Score: {performance_rating['overall_score']:.2f}/1.0)")
    
    return results, evaluation_metrics

def example_hyperparameter_optimization():
    """Example of hyperparameter optimization."""
    print("=== Hyperparameter Optimization Example ===")
    
    # Define hyperparameter search space
    hyperparameter_space = {
        'neuron_count': [500, 1000, 2000],
        'curiosity_threshold': [0.3, 0.5, 0.7],
        'reward_scale': [0.5, 1.0, 2.0],
        'exploration_rate': [0.1, 0.5, 0.9],
    }
    
    # Simple grid search (in practice, use more sophisticated optimization)
    best_score = -float('inf')
    best_params = {}
    
    param_combinations = []
    for neuron_count in hyperparameter_space['neuron_count']:
        for curiosity_threshold in hyperparameter_space['curiosity_threshold']:
            for reward_scale in hyperparameter_space['reward_scale']:
                for exploration_rate in hyperparameter_space['exploration_rate']:
                    param_combinations.append({
                        'neuron_count': neuron_count,
                        'curiosity_threshold': curiosity_threshold,
                        'reward_scale': reward_scale,
                        'exploration_rate': exploration_rate,
                    })
    
    print(f"Testing {len(param_combinations)} hyperparameter combinations...")
    
    for i, params in enumerate(param_combinations):
        print(f"  Testing combination {i + 1}/{len(param_combinations)}: {params}")
        
        # Create configuration with hyperparameters
        config = TrainingConfig()
        config.neuron_count = params['neuron_count']
        config.curiosity_threshold = params['curiosity_threshold']
        config.dopamine_scale = params['reward_scale']
        config.epsilon_start = params['exploration_rate']
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Train agent with current hyperparameters
        trainer = AdvancedAgentTrainer()
        results = trainer.train_agent(config, world, max_episodes=100)
        
        # Evaluate performance
        eval_metrics = EvaluationMetrics.calculate_comprehensive_metrics(
            results['agent'], world, num_episodes=5
        )
        
        # Use reward performance as objective
        current_score = eval_metrics['reward_metrics']['mean']
        
        # Update best parameters
        if current_score > best_score:
            best_score = current_score
            best_params = params.copy()
        
        print(f"    Score: {current_score:.4f}, Best: {best_score:.4f}")
    
    print(f"\n=== Hyperparameter Optimization Results ===")
    print(f"Best parameters: {best_params}")
    print(f"Best score: {best_score:.4f}")
    
    return best_params, best_score

# Run examples if script is executed directly
if __name__ == "__main__":
    print("Running NLM Advanced Training and Evaluation Examples")
    print("=" * 60)
    
    # Example 1: Advanced training
    results1, metrics1 = example_advanced_training()
    
    print("\n" + "=" * 60)
    
    # Example 2: Hyperparameter optimization
    best_params, best_score = example_hyperparameter_optimization()
    
    print("\n" + "=" * 60)
    print("Examples completed successfully!")
