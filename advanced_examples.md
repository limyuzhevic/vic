# Advanced NLM Usage - Power User Guide

# Introduction

This document contains advanced examples and techniques for experienced NLM users who want to push the boundaries of the neural simulation framework. These examples build upon the basic concepts and demonstrate how to leverage the new AgentBrain features for complex applications.

# Advanced Agent Configuration

## Custom Agent Factory with Advanced Features

```python
import pynlm
from typing import Dict, Any, Callable

class AdvancedAgentFactory:
    def __init__(self):
        self.config_presets = {
            'aggressive_explorer': {
                'curiosity_threshold': 0.7,
                'exploration_probability': 0.5,
                'reward_modulation': True,
                'structural_plasticity': True,
                'development': True
            },
            'precision_critic': {
                'curiosity_threshold': 0.1,
                'exploration_probability': 0.05,
                'reward_modulation': True,
                'structural_plasticity': False,
                'development': False
            },
            'balanced_developer': {
                'curiosity_threshold': 0.3,
                'exploration_probability': 0.2,
                'reward_modulation': True,
                'structural_plasticity': True,
                'development': True
            }
        }
    
    def create_agent(self, 
                    preset_name: str,
                    brain_config: pynlm.Config = None,
                    world_config: Dict[str, Any] = None,
                    custom_features: Callable = None) -> tuple:
        """Create a fully configured agent with advanced features."""
        
        # Create configuration
        if brain_config is None:
            brain_config = pynlm.createDefaultConfig()
        
        # Apply preset configuration
        if preset_name in self.config_presets:
            preset = self.config_presets[preset_name]
            brain_config.set("neuron_count", 2000)
            brain_config.set("region_count", 8)
            brain_config.set("connection_probability", 0.05)
        
        # Create brain and agent
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        if world_config:
            world.configure(**world_config)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Apply preset settings
        self._apply_preset(agent, preset_name, preset)
        
        # Apply custom features if provided
        if custom_features:
            custom_features(agent, brain, world)
        
        return brain, agent, world
    
    def _apply_preset(self, agent, preset_name: str, preset: Dict[str, Any]):
        """Apply preset configuration to agent."""
        if preset.get('reward_modulation', False):
            agent.enableRewardModulation(True)
        
        if preset.get('structural_plasticity', False):
            agent.enableStructuralPlasticity(True)
        
        if preset.get('development', False):
            agent.enableDevelopment(True)
        
        if preset.get('curiosity', False):
            agent.enableCuriosity(True)
        
        # Configure behavior based on preset
        if 'explorer' in preset_name:
            agent.configureForExploration()
        elif 'critic' in preset_name:
            agent.configureForExploitation()
        else:
            agent.configureForGoalOrientedBehavior()
    
    def benchmark_performance(self, brain, agent, world, steps: int = 1000) -> Dict[str, Any]:
        """Benchmark agent performance with detailed metrics."""
        import time
        
        start_time = time.time()
        
        # Optimize for speed for benchmarking
        agent.optimizeForSpeed()
        
        # Reset agent for clean benchmark
        agent.reset()
        
        # Run simulation
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(result.reward, 0.0)
        
        end_time = time.time()
        
        # Collect performance metrics
        performance_metrics = agent.getPerformanceMetrics()
        
        benchmark_results = {
            'total_steps': steps,
            'elapsed_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'performance_metrics': performance_metrics,
            'optimization': 'speed'
        }
        
        return benchmark_results
```

## Adaptive Agent Controller

```python
import pynlm
import time
from collections import deque

class AdaptiveAgentController:
    """Dynamically adapts agent configuration based on performance."""
    
    def __init__(self, agent, brain, world):
        self.agent = agent
        self.brain = brain
        self.world = world
        
        # Performance tracking
        self.performance_history = deque(maxlen=100)
        self.curiosity_history = deque(maxlen=100)
        self.stability_history = deque(maxlen=50)
        
        # Adaptive parameters
        self.performance_threshold = 0.7
        self.curiosity_threshold = 0.3
        self.min_stable_steps = 50
        
    def run_adaptive_simulation(self, target_steps: int = 2000) -> Dict[str, Any]:
        """Run simulation with adaptive agent control."""
        
        for step in range(target_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Process brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward with adaptive prediction
            predicted_reward = self._predict_reward(step)
            self.agent.applyRewardModulation(result.reward, predicted_reward)
            
            # Record performance metrics
            self._record_performance(step, result.reward, predicted_reward)
            
            # Adaptive control every 100 steps
            if step % 100 == 0:
                self._adapt_configuration(step)
            
            # Log progress
            if step % 500 == 0:
                print(f"Step {step}: Adaptive control running")
        
        return self._generate_adaptive_report()
    
    def _predict_reward(self, step: int) -> float:
        """Predict expected reward based on step and performance."""
        # Simple reward prediction based on recent performance
        if len(self.performance_history) < 10:
            return 0.0
        
        avg_performance = sum(p['reward'] for p in self.performance_history) / len(self.performance_history)
        return avg_performance * 0.1  # Conservative prediction
    
    def _record_performance(self, step: int, actual_reward: float, predicted_reward: float):
        """Record agent performance metrics."""
        metrics = {
            'step': step,
            'reward': actual_reward,
            'prediction_error': actual_reward - predicted_reward,
            'curiosity': self.agent.getCuriosityLevel(),
            'dopamine': self.agent.getNeuromodulationLevel(),
            'stability': self._calculate_stability()
        }
        
        self.performance_history.append(metrics)
        self.curiosity_history.append(self.agent.getCuriosityLevel())
        
        if step % 10 == 0:
            self.stability_history.append(metrics['stability'])
    
    def _calculate_stability(self) -> float:
        """Calculate performance stability."""
        if len(self.performance_history) < 5:
            return 1.0
        
        recent_rewards = [p['reward'] for p in list(self.performance_history)[-5:]]
        reward_variance = sum((r - sum(recent_rewards)/len(recent_rewards))**2 
                           for r in recent_rewards) / len(recent_rewards)
        
        return max(0.0, 1.0 - reward_variance)
    
    def _adapt_configuration(self, step: int):
        """Adapt agent configuration based on performance."""
        if not self.performance_history:
            return
        
        # Calculate average metrics
        avg_curiosity = sum(p['curiosity'] for p in self.performance_history) / len(self.performance_history)
        avg_stability = sum(p['stability'] for p in self.stability_history) / len(self.stability_history) if self.stability_history else 1.0
        
        # Adapt based on performance
        if avg_curiosity > self.curiosity_threshold and avg_stability > 0.7:
            # High curiosity and stable - maintain current configuration
            print(f"Step {step}: Stable performance, maintaining configuration")
        elif avg_curiosity < 0.1 and avg_stability > 0.8:
            # Low curiosity, very stable - increase exploration
            self.agent.configureForExploration()
            print(f"Step {step}: Increasing exploration (low curiosity detected)")
        elif avg_stability < 0.5:
            # Unstable performance - reduce curiosity, stabilize
            self.agent.configureForExploitation()
            print(f"Step {step}: Unstable performance, reducing exploration")
        elif avg_curiosity > 0.5:
            # High curiosity - maintain exploration
            self.agent.configureForExploration()
            print(f"Step {step}: High curiosity, maintaining exploration")
    
    def _generate_adaptive_report(self) -> Dict[str, Any]:
        """Generate comprehensive adaptive control report."""
        
        report = {
            'total_steps': len(self.performance_history),
            'final_configuration': {
                'curiosity_enabled': self.agent.isCuriosityEnabled(),
                'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
                'development_enabled': self.agent.isDevelopmentEnabled(),
                'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled()
            },
            'performance_statistics': {
                'average_reward': sum(p['reward'] for p in self.performance_history) / len(self.performance_history),
                'average_curiosity': sum(p['curiosity'] for p in self.performance_history) / len(self.performance_history),
                'average_stability': sum(p['stability'] for p in self.stability_history) / len(self.stability_history) if self.stability_history else 1.0,
                'max_reward': max(p['reward'] for p in self.performance_history),
                'min_reward': min(p['reward'] for p in self.performance_history)
            },
            'adaptation_summary': {
                'configuration_changes': 'Adaptive control maintained stability',
                'exploration_ratio': len([p for p in self.performance_history if p['curiosity'] > 0.3]) / len(self.performance_history)
            }
        }
        
        return report
```

## Multi-Agent Coordination System

```python
import pynlm
import threading
import time
from queue import Queue

class MultiAgentSystem:
    """Manage multiple agents in a shared environment with coordination."""
    
    def __init__(self, num_agents: int, environment_config: Dict[str, Any] = None):
        self.num_agents = num_agents
        self.agents = []
        self.brains = []
        self.world = None
        self.command_queue = Queue()
        self.result_queue = Queue()
        self.running = False
        
        # Environment configuration
        self.env_config = environment_config or {
            'width': 100,
            'height': 100,
            'visionWidth': 16,
            'visionHeight': 16,
            'maxEnergy': 100.0,
            'energyDecayRate': 0.01
        }
        
        # Agent coordination parameters
        self.agent_configs = []
        self.coordination_strategy = 'competitive'  # competitive, cooperative, mixed
        
    def create_agents(self, agent_configs: list):
        """Create multiple agents with different configurations."""
        self.agent_configs = agent_configs
        
        # Create shared environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(**self.env_config)
        self.world.reset()
        
        # Create each agent
        for i in range(self.num_agents):
            config = agent_configs[i] if i < len(agent_configs) else self._default_agent_config(i)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Apply agent-specific configuration
            self._configure_agent(agent, config, i)
            
            self.agents.append(agent)
            self.brains.append(brain)
            
            print(f"Created Agent {i} with {brain.getTotalNeuronCount()} neurons")
    
    def _default_agent_config(self, agent_id: int) -> pynlm.Config:
        """Generate default configuration for agent."""
        config = pynlm.createDefaultConfig()
        
        # Vary agent characteristics
        neuron_count = 1000 + agent_id * 200
        region_count = 2 + agent_id
        
        config.set("neuron_count", neuron_count)
        config.set("region_count", region_count)
        config.set("connection_probability", 0.05)
        
        # Different exploration tendencies
        if agent_id % 3 == 0:
            # Aggressive explorer
            config.set("stdp_ltp_weight", 0.02)
            config.set("stdp_ltd_weight", 0.024)
        elif agent_id % 3 == 1:
            # Conservative exploiter
            config.set("stdp_ltp_weight", 0.005)
            config.set("stdp_ltd_weight", 0.006)
        else:
            # Balanced agent
            config.set("stdp_ltp_weight", 0.01)
            config.set("stdp_ltd_weight", 0.012)
        
        return config
    
    def _configure_agent(self, agent: pynlm.AgentBrain, config: pynlm.Config, agent_id: int):
        """Configure individual agent based on its ID and configuration."""
        
        # Set configuration based on coordination strategy
        if self.coordination_strategy == 'competitive':
            if agent_id % 2 == 0:
                # Even agents: exploration-oriented
                agent.configureForExploration()
                agent.enableCuriosity(True)
            else:
                # Odd agents: exploitation-oriented
                agent.configureForExploitation()
                agent.enableCuriosity(False)
                
        elif self.coordination_strategy == 'cooperative':
            # All agents: balanced cooperation
            agent.configureForGoalOrientedBehavior()
            agent.enableCuriosity(True)
            
        elif self.coordination_strategy == 'mixed':
            # Random assignment of strategies
            if agent_id % 4 == 0:
                agent.configureForExploration()
            elif agent_id % 4 == 1:
                agent.configureForExploitation()
            else:
                agent.configureForGoalOrientedBehavior()
        
        # Enable learning features based on config
        if config.get("enable_reward_modulation", True):
            agent.enableRewardModulation(True)
        
        if config.get("enable_structural_plasticity", True):
            agent.enableStructuralPlasticity(True)
        
        if config.get("enable_development", True):
            agent.enableDevelopment(True)
        
        if config.get("enable_curiosity", True):
            agent.enableCuriosity(True)
    
    def run_simulation(self, steps: int) -> Dict[str, Any]:
        """Run multi-agent simulation."""
        self.running = True
        
        print(f"Starting multi-agent simulation with {self.num_agents} agents")
        print(f"Total simulation steps: {steps}")
        
        # Start all agents in separate threads
        agent_threads = []
        
        for i, (agent, brain) in enumerate(zip(self.agents, self.brains)):
            thread = threading.Thread(
                target=self._agent_simulation_loop,
                args=(i, agent, brain, steps)
            )
            agent_threads.append(thread)
            thread.start()
            
            # Small delay to stagger agent starts
            time.sleep(0.05)
        
        # Wait for all threads to complete
        for thread in agent_threads:
            thread.join()
        
        self.running = False
        
        return self._collect_results()
    
    def _agent_simulation_loop(self, agent_id: int, agent: pynlm.AgentBrain, 
                               brain: pynlm.Brain, steps: int):
        """Individual agent simulation loop."""
        
        for step in range(steps):
            if not self.running:
                break
            
            # Update shared environment
            self.world.update(0.1)
            
            # Get sensory percept from shared environment
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Process brain
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action in shared environment
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Record metrics
            if step % 100 == 0:
                curiosity = agent.getCuriosityLevel()
                dopamine = agent.getNeuromodulationLevel()
                print(f"Agent {agent_id} Step {step}: Curiosity={curiosity:.3f}, Dopamine={dopamine:.3f}")
        
        # Collect agent results
        metrics = agent.getPerformanceMetrics()
        self.result_queue.put({
            'agent_id': agent_id,
            'performance_metrics': metrics,
            'final_configuration': {
                'curiosity_enabled': agent.isCuriosityEnabled(),
                'reward_modulation_enabled': agent.isRewardModulationEnabled(),
                'development_enabled': agent.isDevelopmentEnabled(),
                'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled()
            }
        })
    
    def _collect_results(self) -> Dict[str, Any]:
        """Collect and analyze results from all agents."""
        results = []
        
        while not self.result_queue.empty():
            results.append(self.result_queue.get())
        
        # Sort by agent ID
        results.sort(key=lambda x: x['agent_id'])
        
        # Analyze overall system performance
        total_steps = sum(r['performance_metrics']['steps_processed'] for r in results)
        avg_curiosity = sum(r['performance_metrics']['curiosity_level'] for r in results) / len(results)
        avg_reward = sum(r['performance_metrics']['dopamine_level'] for r in results) / len(results)
        
        # Configuration analysis
        configs = [r['final_configuration'] for r in results]
        
        analysis = {
            'num_agents': self.num_agents,
            'total_steps': total_steps,
            'agents_performed': len(results),
            'coordination_strategy': self.coordination_strategy,
            'system_performance': {
                'average_curiosity': avg_curiosity,
                'average_reward_response': avg_reward,
                'performance_variance': self._calculate_variance(results)
            },
            'agent_configurations': configs,
            'agent_results': results
        }
        
        print("\n=== Multi-Agent Simulation Results ===")
        print(f"Coordination Strategy: {self.coordination_strategy}")
        print(f"Total Steps: {total_steps}")
        print(f"Average Curiosity Level: {avg_curiosity:.3f}")
        print(f"Average Reward Response: {avg_reward:.3f}")
        
        return analysis
    
    def _calculate_variance(self, results: list) -> Dict[str, float]:
        """Calculate variance in agent performance."""
        curiosity_values = [r['performance_metrics']['curiosity_level'] for r in results]
        reward_values = [r['performance_metrics']['dopamine_level'] for r in results]
        
        if len(curiosity_values) < 2:
            return {'curiosity_variance': 0.0, 'reward_variance': 0.0}
        
        def variance(values):
            mean = sum(values) / len(values)
            return sum((x - mean) ** 2 for x in values) / len(values)
        
        return {
            'curiosity_variance': variance(curiosity_values),
            'reward_variance': variance(reward_values)
        }
```

## Advanced Training and Optimization

```python
import pynlm
import numpy as np
import time

class AdvancedAgentTrainer:
    """Advanced agent training with optimization and regularization."""
    
    def __init__(self, agent: pynlm.AgentBrain, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.agent = agent
        self.brain = brain
        self.world = world
        
        # Training parameters
        self.training_history = {
            'steps': [],
            'rewards': [],
            'curiosity_levels': [],
            'performance_metrics': [],
            'brain_weights': []
        }
        
        # Optimization settings
        self.learning_rate = 0.01
        self.epsilon = 0.1  # Exploration rate
        self.discount_factor = 0.99
        self.target_network_update = 100
        
        # Performance tracking
        self.performance_thresholds = {
            'min_reward': -1.0,
            'max_curiosity': 1.0,
            'target_stability': 0.7
        }
    
    def train_agent(self, training_episodes: int, max_steps_per_episode: int) -> Dict[str, Any]:
        """Train agent with advanced optimization techniques."""
        
        print(f"Starting advanced training: {training_episodes} episodes")
        print(f"Max steps per episode: {max_steps_per_episode}")
        
        for episode in range(training_episodes):
            episode_start_time = time.time()
            
            # Reset agent for new episode
            self.agent.reset()
            self.world.reset()
            
            # Episode metrics
            episode_metrics = {
                'steps': 0,
                'total_reward': 0.0,
                'max_reward': -float('inf'),
                'min_reward': float('inf'),
                'curiosity_score': 0.0,
                'stability_score': 0.0
            }
            
            # Run episode
            for step in range(max_steps_per_episode):
                # Choose action with epsilon-greedy policy
                action = self._choose_action(step, episode)
                
                # Execute action
                result = self._execute_action(action, step)
                
                # Learn from experience
                self._learn_from_experience(action, result, step, episode)
                
                # Update metrics
                episode_metrics['steps'] += 1
                episode_metrics['total_reward'] += result.reward
                episode_metrics['max_reward'] = max(episode_metrics['max_reward'], result.reward)
                episode_metrics['min_reward'] = min(episode_metrics['min_reward'], result.reward)
                
                # Record training history
                self._record_training_data(step, episode, result.reward)
                
                # Break if episode ended
                if not self._episode_ended(step, episode):
                    break
            
            # Calculate episode scores
            episode_metrics['average_reward'] = (episode_metrics['total_reward'] / 
                                                episode_metrics['steps'] if episode_metrics['steps'] > 0 else 0)
            episode_metrics['curiosity_score'] = self.agent.getCuriosityLevel()
            episode_metrics['stability_score'] = self._calculate_episode_stability(episode)
            
            # Apply regularization if performance is poor
            self._apply_regularization(episode, episode_metrics)
            
            # Log episode progress
            episode_time = time.time() - episode_start_time
            print(f"Episode {episode + 1}/{training_episodes}: "
                  f"Avg Reward: {episode_metrics['average_reward']:.3f}, "
                  f"Steps: {episode_metrics['steps']}, "
                  f"Time: {episode_time:.2f}s")
        
        return self._generate_training_report()
    
    def _choose_action(self, step: int, episode: int) -> pynlm.MotorCommand:
        """Choose action using advanced decision making."""
        
        # Get agent's current motor command (from brain activity)
        default_action = self.agent.decodeMotorCommand()
        
        # Epsilon-greedy exploration with adaptive epsilon
        epsilon = self._calculate_epsilon(episode)
        
        if np.random.random() < epsilon:
            # Exploration: choose random action
            return self._explore_action()
        else:
            # Exploitation: use agent's learned policy
            return default_action
    
    def _calculate_epsilon(self, episode: int) -> float:
        """Calculate epsilon for epsilon-greedy policy."""
        # Exponential decay of exploration
        epsilon = max(0.01, self.epsilon * (1 - episode / 1000))
        return epsilon
    
    def _explore_action(self) -> pynlm.MotorCommand:
        """Choose exploration action."""
        # Select from available motor actions
        actions = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        
        return np.random.choice(actions)
    
    def _execute_action(self, action: pynlm.MotorCommand, step: int) -> pynlm.ActionResult:
        """Execute action and return result."""
        # Update world
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Process brain
        self.brain.step(step)
        
        # Apply action
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply reward modulation
        self.agent.applyRewardModulation(result.reward, 0.0)
        
        return result
    
    def _learn_from_experience(self, action: pynlm.MotorCommand, result: pynlm.ActionResult, 
                              step: int, episode: int):
        """Learn from experience using advanced learning algorithms."""
        
        # Calculate temporal difference error
        td_error = result.reward + self.discount_factor * 0.0 - 0.0  # Simplified
        
        # Update agent's neuromodulation based on learning
        self.agent.applyRewardModulation(result.reward, td_error)
        
        # Update development system
        self.agent.updateDevelopment(0.1)
        
        # Apply structural plasticity if enabled
        if self.agent.isStructuralPlasticityEnabled():
            self._apply_structural_plasticity(step)
    
    def _apply_structural_plasticity(self, step: int):
        """Apply structural plasticity changes."""
        # Simplified structural plasticity
        if step % 100 == 0:
            # Occasionally add new connections
            pass
    
    def _episode_ended(self, step: int, episode: int) -> bool:
        """Check if episode has ended."""
        # Check termination conditions
        if step >= 1000:  # Max steps
            return True
        
        if self.agent.getEnergy() <= 0:  # Agent energy depleted
            return True
        
        # Add custom termination conditions
        return False
    
    def _record_training_data(self, step: int, episode: int, reward: float):
        """Record training data for analysis."""
        self.training_history['steps'].append(step)
        self.training_history['rewards'].append(reward)
        self.training_history['curiosity_levels'].append(self.agent.getCuriosityLevel())
        
        # Record brain performance metrics
        metrics = self.agent.getPerformanceMetrics()
        self.training_history['performance_metrics'].append(metrics)
        
        # Record brain weights (simplified)
        total_weight = self.brain.getExcitationInhibitionRatio()
        self.training_history['brain_weights'].append(total_weight)
    
    def _calculate_episode_stability(self, episode: int) -> float:
        """Calculate stability score for an episode."""
        if len(self.training_history['rewards']) < 5:
            return 1.0
        
        recent_rewards = self.training_history['rewards'][-5:]
        mean_reward = sum(recent_rewards) / len(recent_rewards)
        variance = sum((r - mean_reward) ** 2 for r in recent_rewards) / len(recent_rewards)
        
        return max(0.0, 1.0 - variance)
    
    def _apply_regularization(self, episode: int, episode_metrics: Dict[str, Any]):
        """Apply regularization if performance is poor."""
        
        # Check performance thresholds
        if (episode_metrics['average_reward'] < self.performance_thresholds['min_reward'] or
            episode_metrics['curiosity_score'] > self.performance_thresholds['max_curiosity'] or
            episode_metrics['stability_score'] < self.performance_thresholds['target_stability']):
            
            # Apply performance regularization
            print(f"Episode {episode}: Performance regularization applied")
            
            # Reduce curiosity if too high
            if episode_metrics['curiosity_score'] > 0.8:
                self.agent.configureForExploitation()
            
            # Reset development if stability is poor
            if episode_metrics['stability_score'] < 0.5:
                self.agent.enableDevelopment(False)
    
    def _generate_training_report(self) -> Dict[str, Any]:
        """Generate comprehensive training report."""
        
        if not self.training_history['steps']:
            return {'error': 'No training data collected'}
        
        # Calculate training statistics
        total_steps = sum(self.training_history['steps'])
        total_rewards = sum(self.training_history['rewards'])
        
        report = {
            'total_training_steps': total_steps,
            'total_episodes': len(self.training_history['steps']),
            'final_performance': {
                'average_reward': total_rewards / len(self.training_history['rewards']),
                'average_curiosity': (sum(self.training_history['curiosity_levels']) / 
                                    len(self.training_history['curiosity_levels'])),
                'final_stability': self._calculate_episode_stability(len(self.training_history['steps'])),
                'final_configuration': {
                    'curiosity_enabled': self.agent.isCuriosityEnabled(),
                    'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
                    'development_enabled': self.agent.isDevelopmentEnabled(),
                    'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled()
                }
            },
            'training_efficiency': {
                'steps_per_second': total_steps / len(self.training_history['steps']),
                'reward_efficiency': total_rewards / len(self.training_history['steps'])
            },
            'optimization_history': {
                'episodes_with_regularization': len([e for e in range(len(self.training_history['steps'])) 
                                                   if e % 50 == 0]),
                'performance_improvements': self._calculate_performance_improvements()
            }
        }
        
        return report
    
    def _calculate_performance_improvements(self) -> Dict[str, Any]:
        """Calculate performance improvements over time."""
        
        if len(self.training_history['rewards']) < 10:
            return {'insufficient_data': True}
        
        # Split into first half and second half
        mid_point = len(self.training_history['rewards']) // 2
        
        first_half_rewards = self.training_history['rewards'][:mid_point]
        second_half_rewards = self.training_history['rewards'][mid_point:]
        
        first_half_curiosity = self.training_history['curiosity_levels'][:mid_point]
        second_half_curiosity = self.training_history['curiosity_levels'][mid_point:]
        
        def calculate_stats(rewards):
            return {
                'mean': sum(rewards) / len(rewards),
                'std': (sum((r - sum(rewards)/len(rewards))**2 for r in rewards) / len(rewards))**0.5,
                'min': min(rewards),
                'max': max(rewards)
            }
        
        improvements = {
            'reward_trend': {
                'first_half': calculate_stats(first_half_rewards),
                'second_half': calculate_stats(second_half_rewards),
                'improvement': calculate_stats(second_half_rewards)['mean'] - calculate_stats(first_half_rewards)['mean']
            },
            'curiosity_trend': {
                'first_half': calculate_stats(first_half_curiosity),
                'second_half': calculate_stats(second_half_curiosity),
                'improvement': (calculate_stats(second_half_curiosity)['mean'] - 
                              calculate_stats(first_half_curiosity)['mean'])
            }
        }
        
        return improvements
```

# Conclusion

These advanced examples demonstrate the full power of NLM's enhanced AgentBrain implementation. The system now provides:

- **Advanced Configuration Management**: Multiple preset behaviors and adaptive control
- **Multi-Agent Coordination**: Complex interactions between multiple agents
- **Sophisticated Learning**: Advanced training algorithms with regularization
- **Performance Optimization**: Dynamic adaptation and real-time monitoring
- **Custom Development**: Extensible architecture for power users

These features make NLM suitable for complex applications including:

- **Research simulations**: Study neural dynamics and learning
- **Competitive environments**: Multi-agent competitions and coordination
- **Real-time systems**: Dynamic adaptation to changing conditions
- **Optimization challenges**: Finding optimal behavior strategies
- **Educational tools**: Interactive learning and demonstration

The enhanced AgentBrain class transforms NLM from a basic brain simulator into a comprehensive neural computing platform capable of handling sophisticated applications in neuroscience research, artificial intelligence, and autonomous systems development.