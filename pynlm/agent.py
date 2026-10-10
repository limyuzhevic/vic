"""
Agent brain interface and advanced agent functionality.

This module contains the agent brain interface for connecting NLM brains to environments,
as well as advanced agent features for expert users.
"""

from .core import *

class AdvancedAgent:
    """
    Advanced agent interface for expert users.
    
    Provides advanced agent control, learning algorithms, and simulation management
    for complex agent-environment interactions.
    """
    
    def __init__(self, agent_brain, world):
        self.agent = agent_brain
        self.world = world
        self._simulation_history = []
        self._learning_rate = 0.01
        self._exploration_rate = 0.1
        self._max_memory_size = 1000
        self._memory = []
        
    def run_episode(self, max_steps=1000, verbose=False):
        """
        Run a complete agent episode with advanced control.
        
        Args:
            max_steps: Maximum number of steps per episode
            verbose: Enable verbose output
            
        Returns:
            dict: Episode results and statistics
        """
        episode_result = {
            'steps': [],
            'total_reward': 0.0,
            'num_steps': 0,
            'final_state': None,
            'success': False,
            'performance': {}
        }
        
        # Reset agent and world
        self.agent.reset()
        if hasattr(self.world, 'reset'):
            self.world.reset()
        
        if verbose:
            print(f"Starting agent episode with max {max_steps} steps")
        
        for step in range(max_steps):
            step_result = {
                'step': step,
                'timestamp': step * 0.1,
                'action': None,
                'reward': 0.0,
                'novelty': 0.0,
                'curiosity': 0.0,
                'prediction_error': 0.0,
                'brain_state': None,
                'success': False
            }
            
            try:
                # Get sensory input from world
                percept = self.world.getSensoryPercept()
                
                # Process sensory input through agent
                self.agent.processSensoryInput(percept)
                
                # Get action from agent brain
                action = self.agent.decodeMotorCommand()
                step_result['action'] = action
                
                # Apply action to world
                if hasattr(self.world, 'applyMotorCommand'):
                    self.world.applyMotorCommand(action, step * 0.1)
                
                # Update development system
                if hasattr(self.agent, 'updateDevelopment'):
                    self.agent.updateDevelopment(0.1)
                
                # Apply reward modulation
                reward = 0.0
                if hasattr(self.world, 'getSensoryPercept'):
                    internal = self.world.getSensoryPercept().getInternal()
                    if internal and len(internal) > 0:
                        reward = internal[0]
                
                predicted_reward = 0.0
                if hasattr(self.agent, 'getPredictionError'):
                    predicted_reward = self.agent.getPredictionError()
                
                self.agent.applyRewardModulation(reward, predicted_reward)
                step_result['reward'] = reward
                step_result['prediction_error'] = predicted_reward
                
                # Get neuromodulation levels
                if hasattr(self.agent, 'getCuriosityLevel'):
                    step_result['curiosity'] = self.agent.getCuriosityLevel()
                if hasattr(self.agent, 'getNoveltyLevel'):
                    step_result['novelty'] = self.agent.getNoveltyLevel()
                
                # Store step result
                episode_result['steps'].append(step_result)
                episode_result['total_reward'] += reward
                episode_result['num_steps'] += 1
                
                # Store experience for learning
                experience = {
                    'state': percept,
                    'action': action,
                    'reward': reward,
                    'novelty': step_result['novelty'],
                    'curiosity': step_result['curiosity'],
                    'prediction_error': step_result['prediction_error']
                }
                self._store_experience(experience)
                
                # Learning from experience
                if self._should_learn(step):
                    self._learn_from_experience()
                
                if verbose and step % 100 == 0:
                    print(f"Step {step}: action={action}, reward={reward:.2f}, "
                          f"curiosity={step_result['curiosity']:.3f}")
                
            except Exception as e:
                print(f"[ERROR] Step {step} failed: {e}")
                step_result['error'] = str(e)
                episode_result['steps'].append(step_result)
                break
        
        # Calculate episode performance
        episode_result['performance'] = self._calculate_performance(episode_result['steps'])
        episode_result['final_state'] = self._get_final_state()
        episode_result['success'] = self._evaluate_success(episode_result)
        
        if verbose:
            print(f"Episode completed: {episode_result['num_steps']} steps, "
                  f"total reward: {episode_result['total_reward']:.2f}")
            print(f"Performance: {episode_result['performance']}")
        
        return episode_result
        
    def run_multiple_episodes(self, num_episodes, max_steps_per_episode=1000, verbose=False):
        """
        Run multiple agent episodes for comprehensive testing.
        
        Args:
            num_episodes: Number of episodes to run
            max_steps_per_episode: Maximum steps per episode
            verbose: Enable verbose output
            
        Returns:
            list: Results for each episode
        """
        all_episodes = []
        
        for episode in range(num_episodes):
            if verbose:
                print(f"Running episode {episode + 1}/{num_episodes}")
            
            episode_result = self.run_episode(max_steps_per_episode, verbose)
            all_episodes.append(episode_result)
            
            # Small delay between episodes if needed
            if episode < num_episodes - 1:
                import time
                time.sleep(0.1)
        
        # Calculate aggregate statistics
        aggregate_stats = self._calculate_aggregate_statistics(all_episodes)
        
        if verbose:
            print(f"All episodes completed. "
                  f"Average reward: {aggregate_stats['average_reward']:.2f}")
        
        return all_episodes, aggregate_stats
        
    def run_learning_simulation(self, total_steps, exploration_steps=0.2, verbose=False):
        """
        Run a learning simulation with exploration-exploitation tradeoff.
        
        Args:
            total_steps: Total number of steps to simulate
            exploration_steps: Fraction of steps for exploration
            verbose: Enable verbose output
            
        Returns:
            dict: Learning simulation results
        """
        exploration_steps = int(total_steps * exploration_steps)
        
        learning_result = {
            'steps': [],
            'exploration_steps': exploration_steps,
            'exploitation_steps': total_steps - exploration_steps,
            'learning_curve': [],
            'reward_history': [],
            'performance_metrics': {}
        }
        
        for step in range(total_steps):
            step_data = {'step': step}
            
            # Determine if this is exploration or exploitation
            if step < exploration_steps:
                step_data['mode'] = 'exploration'
                # Use random action for exploration
                action = self._get_random_action()
            else:
                step_data['mode'] = 'exploitation'
                # Use learned action for exploitation
                action = self._get_optimal_action()
            
            # Execute step (similar to run_episode but simplified)
            self._execute_step(step, action, step_data)
            
            learning_result['steps'].append(step_data)
            learning_result['reward_history'].append(step_data.get('reward', 0.0))
            
            if verbose and step % 100 == 0:
                print(f"Learning step {step}: mode={step_data['mode']}, "
                      f"reward={step_data.get('reward', 0.0):.2f}")
        
        # Calculate learning curve and performance metrics
        learning_result['performance_metrics'] = self._analyze_learning_curve(
            learning_result['reward_history']
        )
        
        return learning_result
        
    def _store_experience(self, experience):
        """Store experience in memory for learning."""
        self._memory.append(experience)
        
        # Limit memory size
        if len(self._memory) > self._max_memory_size:
            self._memory.pop(0)
            
    def _should_learn(self, step):
        """Determine if the agent should learn from current experience."""
        return len(self._memory) >= 10 and step % 10 == 0
        
    def _learn_from_experience(self):
        """Learn from stored experiences."""
        # Simplified learning algorithm
        if len(self._memory) >= 5:
            # Sample random experiences for learning
            import random
            experiences = random.sample(self._memory, min(5, len(self._memory)))
            
            # Calculate average reward
            avg_reward = sum(exp['reward'] for exp in experiences) / len(experiences)
            
            # Update learning rate based on performance
            if avg_reward > 1.0:
                self._learning_rate *= 1.05  # Increase learning rate
            elif avg_reward < 0.1:
                self._learning_rate *= 0.95  # Decrease learning rate
            
            # Update exploration rate
            if avg_reward > 0.5:
                self._exploration_rate *= 0.95  # Decrease exploration
            elif avg_reward < 0.2:
                self._exploration_rate *= 1.05  # Increase exploration
                
    def _get_random_action(self):
        """Get a random action for exploration."""
        from .core import ActionType
        import random
        return Action(ActionType(random.choice(list(ActionType))))
        
    def _get_optimal_action(self):
        """Get the optimal action based on learned experience."""
        if not self._memory:
            from .core import ActionType
            return Action(ActionType.WAIT)
            
        # Simple strategy: choose action with highest average reward
        action_rewards = {}
        for exp in self._memory:
            action = exp['action']
            reward = exp['reward']
            if action not in action_rewards:
                action_rewards[action] = []
            action_rewards[action].append(reward)
        
        if not action_rewards:
            from .core import ActionType
            return Action(ActionType.WAIT)
        
        # Find action with highest average reward
        best_action = None
        best_avg_reward = float('-inf')
        
        for action, rewards in action_rewards.items():
            avg_reward = sum(rewards) / len(rewards)
            if avg_reward > best_avg_reward:
                best_avg_reward = avg_reward
                best_action = action
        
        return best_action if best_action else Action(ActionType.WAIT)
        
    def _execute_step(self, step, action, step_data):
        """Execute a single step (simplified version)."""
        try:
            # Simple step execution
            if hasattr(self.agent, 'decodeMotorCommand'):
                # Override action for this step
                pass
                
            step_data['action'] = action
            step_data['reward'] = 0.0  # Simplified reward calculation
            
        except Exception as e:
            print(f"[ERROR] Step {step} execution failed: {e}")
            step_data['error'] = str(e)
            
    def _calculate_performance(self, steps):
        """Calculate performance metrics from episode steps."""
        if not steps:
            return {}
        
        rewards = [step.get('reward', 0.0) for step in steps]
        curiosities = [step.get('curiosity', 0.0) for step in steps]
        novelties = [step.get('novelty', 0.0) for step in steps]
        
        return {
            'total_reward': sum(rewards),
            'average_reward': sum(rewards) / len(rewards),
            'max_reward': max(rewards),
            'min_reward': min(rewards),
            'reward_std': (sum((r - sum(rewards)/len(rewards))**2 for r in rewards) / len(rewards))**0.5,
            'average_curiosity': sum(curiosities) / len(curiosities),
            'average_novelty': sum(novelties) / len(novelties),
            'num_steps': len(steps)
        }
        
    def _get_final_state(self):
        """Get the final state of the agent."""
        return {
            'memory_size': len(self._memory),
            'learning_rate': self._learning_rate,
            'exploration_rate': self._exploration_rate,
            'last_reward': self._memory[-1]['reward'] if self._memory else 0.0
        }
        
    def _evaluate_success(self, episode_result):
        """Evaluate if the episode was successful."""
        performance = episode_result['performance']
        return (performance.get('average_reward', 0.0) > 0.5 and
                performance.get('num_steps', 0) >= 100)
        
    def _calculate_aggregate_statistics(self, all_episodes):
        """Calculate aggregate statistics across multiple episodes."""
        if not all_episodes:
            return {}
        
        all_rewards = []
        all_steps = []
        all_successes = []
        
        for episode in all_episodes:
            all_rewards.append(episode['total_reward'])
            all_steps.append(episode['num_steps'])
            all_successes.append(episode['success'])
        
        return {
            'num_episodes': len(all_episodes),
            'average_reward': sum(all_rewards) / len(all_rewards),
            'average_steps': sum(all_steps) / len(all_steps),
            'success_rate': sum(all_successes) / len(all_successes),
            'total_reward': sum(all_rewards),
            'max_reward': max(all_rewards),
            'min_reward': min(all_rewards)
        }
        
    def _analyze_learning_curve(self, reward_history):
        """Analyze learning curve from reward history."""
        if len(reward_history) < 2:
            return {}
        
        # Calculate moving average
        window_size = min(100, len(reward_history) // 10)
        if window_size > 1:
            moving_avg = []
            for i in range(len(reward_history) - window_size + 1):
                avg = sum(reward_history[i:i + window_size]) / window_size
                moving_avg.append(avg)
            
            # Calculate trend
            if len(moving_avg) >= 2:
                x = list(range(len(moving_avg)))
                y = moving_avg
                slope = (y[-1] - y[0]) / (x[-1] - x[0]) if x[-1] != x[0] else 0
                
                return {
                    'moving_average': moving_avg,
                    'trend': 'increasing' if slope > 0.01 else 'decreasing' if slope < -0.01 else 'stable',
                    'slope': slope,
                    'final_reward': reward_history[-1],
                    'max_reward': max(reward_history),
                    'reward_variance': sum((r - sum(reward_history)/len(reward_history))**2 for r in reward_history) / len(reward_history)
                }
        
        return {
            'final_reward': reward_history[-1] if reward_history else 0,
            'max_reward': max(reward_history) if reward_history else 0,
            'reward_variance': 0
        }
        
    def set_learning_parameters(self, learning_rate=None, exploration_rate=None, max_memory_size=None):
        """
        Set learning parameters for the agent.
        
        Args:
            learning_rate: Learning rate (0.0 to 1.0)
            exploration_rate: Exploration rate (0.0 to 1.0)
            max_memory_size: Maximum memory size for experience replay
        """
        if learning_rate is not None:
            if 0.0 <= learning_rate <= 1.0:
                self._learning_rate = learning_rate
            else:
                print(f"[ERROR] Invalid learning rate: {learning_rate}. Must be between 0.0 and 1.0")
                
        if exploration_rate is not None:
            if 0.0 <= exploration_rate <= 1.0:
                self._exploration_rate = exploration_rate
            else:
                print(f"[ERROR] Invalid exploration rate: {exploration_rate}. Must be between 0.0 and 1.0")
                
        if max_memory_size is not None:
            if max_memory_size > 0:
                self._max_memory_size = max_memory_size
                # Trim memory if needed
                if len(self._memory) > self._max_memory_size:
                    self._memory = self._memory[-self._max_memory_size:]
            else:
                print(f"[ERROR] Invalid max memory size: {max_memory_size}. Must be positive")
        
    def get_learning_parameters(self):
        """
        Get current learning parameters.
        
        Returns:
            dict: Current learning parameters
        """
        return {
            'learning_rate': self._learning_rate,
            'exploration_rate': self._exploration_rate,
            'max_memory_size': self._max_memory_size,
            'memory_usage': len(self._memory)
        }
        
    def clear_memory(self):
        """Clear agent memory."""
        self._memory.clear()
        print("[INFO] Agent memory cleared")
        
    def save_agent_state(self, filepath):
        """
        Save agent state to file.
        
        Args:
            filepath: Path to save file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            state = {
                'memory': self._memory,
                'learning_rate': self._learning_rate,
                'exploration_rate': self._exploration_rate,
                'max_memory_size': self._max_memory_size
            }
            
            import json
            with open(filepath, 'w') as f:
                json.dump(state, f, default=str)
                
            print(f"[INFO] Agent state saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to save agent state: {e}")
            return False
            
    def load_agent_state(self, filepath):
        """
        Load agent state from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            with open(filepath, 'r') as f:
                state = json.load(f)
                
            self._memory = state.get('memory', [])
            self._learning_rate = state.get('learning_rate', 0.01)
            self._exploration_rate = state.get('exploration_rate', 0.1)
            self._max_memory_size = state.get('max_memory_size', 1000)
            
            print(f"[INFO] Agent state loaded from {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to load agent state: {e}")
            return False
            
    def __str__(self):
        """String representation of advanced agent."""
        return f"AdvancedAgent(learning_rate={self._learning_rate}, " \
               f"exploration_rate={self._exploration_rate}, " \
               f"memory_size={len(self._memory)})"
        
    def __repr__(self):
        """Detailed representation of advanced agent."""
        return f"AdvancedAgent(agent={self.agent}, world={self.world}, " \
               f"memory_size={len(self._memory)})"
# Export advanced agent classes and functions
from .agent import AdvancedAgent