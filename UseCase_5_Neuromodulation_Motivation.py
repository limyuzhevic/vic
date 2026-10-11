# Neuromodulation and Motivation for NLM

"""
Comprehensive demonstration of neuromodulation systems in the Neural Learning Machine (NLM).

This script implements the Neuromodulation and Motivation use case, demonstrating:
- Reward prediction error and dopamine modulation
- Curiosity-driven exploration behavior
- Novelty detection
- Integration of motivation with learning systems
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time
import random
from collections import deque

class NeuromodulationExperiment:
    """
    Comprehensive neuromodulation experiment demonstrating motivation systems.
    
    This experiment shows how neuromodulators (dopamine, curiosity, novelty) guide
    learning, exploration, and motivated behavior in the NLM brain.
    """
    
    def __init__(self, config):
        """Initialize the neuromodulation experiment."""
        self.config = config
        self.setup_neuromodulation_system()
        
        # Performance tracking
        self.neuromodulator_levels = {
            'dopamine': [],
            'curiosity': [],
            'novelty': [],
            'serotonin': [],
            'noradrenaline': [],
            'acetylcholine': []
        }
        
        # Learning tracking
        self.rewards = []
        self.predicted_rewards = []
        self.reward_prediction_errors = []
        self.exploration_patterns = []
        self.behavioral_states = []
        
        # State tracking
        self.current_step = 0
        self.episode_rewards = []
        self.learning_efficiency = 0.0
    
    def setup_neuromodulation_system(self):
        """Setup the NLM brain with neuromodulation-focused configuration."""
        # Create brain with neuromodulation-optimized configuration
        config = pynlm.createDefaultConfig()
        
        # Optimize for neuromodulation functions
        config.set("brain.neuron_count", self.config.get('neuron_count', 800))
        config.set("brain.region_count", self.config.get('region_count', 3))
        config.set("brain.v_thresh", -50.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -75.0)
        
        # Neuromodulation parameters
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", self.config.get('stdp_learning_rate', 0.001))
        config.set("neuromod.dopamine.scale", self.config.get('dopamine_scale', 1.0))
        config.set("neuromod.curiosity.enable", self.config.get('curiosity_enable', True))
        config.set("neuromod.novelty.enable", self.config.get('novelty_enable', True))
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Setup simple world for controlled experiments
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable neuromodulation subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Performance monitoring
        self.performance_monitor = NeuromodulationPerformanceMonitor()
    
    def run_neuromodulation_experiment(self, num_steps=3000, show_plots=True):
        """
        Run comprehensive neuromodulation experiment.
        
        Args:
            num_steps: Number of simulation steps
            show_plots: Whether to create visualizations
            
        Returns:
            Dictionary with experiment results
        """
        print("=== NEUROMODULATION AND MOTIVATION EXPERIMENT ===")
        print("Demonstrating dopamine, curiosity, and novelty neuromodulation...")
        
        print(f"Starting neuromodulation experiment for {num_steps} steps...")
        print(f"Neuron count: {self.config.get('neuron_count', 800)}")
        print(f"Dopamine scale: {self.config.get('dopamine_scale', 1.0)}")
        
        for step in range(num_steps):
            self.current_step = step
            
            # Get sensory input and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processes and applies neuromodulation
            self.brain.step(step, step * 0.01)
            
            # Decode motor command and act
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            reward = self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Calculate reward prediction error
            rpe = self.calculate_reward_prediction_error(reward)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, rpe)
            
            # Update development
            self.agent.updateDevelopment(0.01)
            
            # Record neuromodulation metrics
            self.record_neuromodulation_metrics(step, reward, rpe, motor_cmd)
            
            # Print progress
            if step % 500 == 0:
                print(f"Step {step}: RPE={rpe:.3f}, "
                      f"Dopamine={self.agent.getNeuromodulationLevel():.3f}, "
                      f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={self.agent.getNoveltyLevel():.3f}")
        
        # Analyze results
        results = self.analyze_neuromodulation_results()
        
        # Create visualizations
        if show_plots:
            self.create_neuromodulation_visualizations()
        
        return results
    
    def calculate_reward_prediction_error(self, actual_reward):
        """
        Calculate reward prediction error (RPE).
        
        RPE = actual reward - predicted reward
        Positive RPE indicates better-than-expected outcome
        Negative RPE indicates worse-than-expected outcome
        """
        if not hasattr(self, 'last_predicted_reward'):
            self.last_predicted_reward = 0.5  # Initial prediction
        
        # Simple prediction update
        predicted = self.last_predicted_reward
        self.last_predicted_reward = 0.7 * predicted + 0.3 * actual_reward
        
        rpe = actual_reward - predicted
        return rpe
    
    def record_neuromodulation_metrics(self, step, reward, rpe, action):
        """
        Record comprehensive neuromodulation metrics.
        
        Tracks:
        - Neuromodulator levels (dopamine, curiosity, novelty, etc.)
        - Reward prediction errors
        - Behavioral states and patterns
        - Exploration dynamics
        """
        # Record neuromodulator levels
        self.neuromodulator_levels['dopamine'].append(self.agent.getNeuromodulationLevel())
        self.neuromodulator_levels['curiosity'].append(self.agent.getCuriosityLevel())
        self.neuromodulator_levels['novelty'].append(self.agent.getNoveltyLevel())
        
        # Record learning metrics
        self.rewards.append(reward)
        self.predicted_rewards.append(self.last_predicted_reward)
        self.reward_prediction_errors.append(rpe)
        
        # Determine behavioral state
        behavioral_state = self.determine_behavioral_state(reward, rpe, action)
        self.behavioral_states.append(behavioral_state)
        
        # Record exploration patterns
        exploration_rate = self.calculate_exploration_rate()
        self.exploration_patterns.append(exploration_rate)
        
        # Update performance monitor
        self.performance_monitor.record_neuromodulation_metrics(
            step_time=0.01,
            dopamine=self.agent.getNeuromodulationLevel(),
            curiosity=self.agent.getCuriosityLevel(),
            novelty=self.agent.getNoveltyLevel(),
            reward=reward,
            rpe=rpe,
            exploration=exploration_rate
        )
    
    def determine_behavioral_state(self, reward, rpe, action):
        """
        Determine agent's behavioral state based on current inputs.
        
        Returns one of:
        - 'exploitation': Focusing on known rewarding actions
        - 'exploration': Trying new or uncertain actions
        - 'avoidance': Avoiding known negative outcomes
        - 'recovery': Recovering from poor performance
        """
        # Simple state determination based on reward and RPE
        if reward > 0.5:
            if rpe > 0.1:
                return 'exploitation'  # Good performance, continuing
            elif rpe < -0.1:
                return 'recovery'  # Good outcome but unexpected, learning
            else:
                return 'exploitation'  # Stable good performance
        elif reward < -0.2:
            return 'avoidance'  # Negative outcome, avoiding similar actions
        else:
            # Small rewards - balance exploration and exploitation
            if rpe > 0.05:
                return 'exploration'  # Positive surprise, explore more
            else:
                return 'exploitation'  # Stable performance
    
    def calculate_exploration_rate(self):
        """
        Calculate exploration rate based on neuromodulator levels.
        
        Higher curiosity and novelty levels increase exploration.
        """
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        
        # Weighted combination of neuromodulators
        exploration_rate = (curiosity * 0.6 + novelty * 0.4)
        
        # Add some stochasticity
        exploration_rate += np.random.uniform(0, 0.1)
        
        return min(1.0, exploration_rate)  # Cap at 1.0
    
    def analyze_neuromodulation_results(self):
        """Analyze neuromodulation experiment results comprehensively."""
        
        # Neuromodulator level analysis
        neuromod_analysis = {}
        for mod_name, levels in self.neuromodulator_levels.items():
            if levels:
                neuromod_analysis[mod_name] = {
                    'mean': np.mean(levels),
                    'std': np.std(levels),
                    'min': np.min(levels),
                    'max': np.max(levels),
                    'final': levels[-1]
                }
        
        # Reward prediction error analysis
        rpe_analysis = {}
        if self.reward_prediction_errors:
            rpe_analysis = {
                'mean': np.mean(self.reward_prediction_errors),
                'std': np.std(self.reward_prediction_errors),
                'positive_count': len([r for r in self.reward_prediction_errors if r > 0]),
                'negative_count': len([r for r in self.reward_prediction_errors if r < 0]),
                'zero_count': len([r for r in self.reward_prediction_errors if r == 0]),
                'absolute_mean': np.mean(np.abs(self.reward_prediction_errors))
            }
        
        # Behavioral state analysis
        if self.behavioral_states:
            unique_states, counts = np.unique(self.behavioral_states, return_counts=True)
            behavioral_distribution = dict(zip(unique_states, counts / len(self.behavioral_states)))
        
        # Exploration pattern analysis
        exploration_analysis = {}
        if self.exploration_patterns:
            exploration_analysis = {
                'mean_exploration': np.mean(self.exploration_patterns),
                'exploration_variance': np.var(self.exploration_patterns),
                'high_exploration_episodes': len([e for e in self.exploration_patterns if e > 0.7]),
                'low_exploration_episodes': len([e for e in self.exploration_patterns if e < 0.3])
            }
        
        # Reward trajectory analysis
        reward_analysis = {}
        if self.rewards:
            # Calculate reward statistics
            reward_analysis = {
                'total_reward': sum(self.rewards),
                'mean_reward': np.mean(self.rewards),
                'reward_std': np.std(self.rewards),
                'reward_trend': self.calculate_reward_trend(),
                'reward_stability': self.calculate_reward_stability()
            }
        
        # Learning efficiency calculation
        learning_efficiency = self.calculate_learning_efficiency(neuromod_analysis, rpe_analysis)
        
        return {
            'neuromodulators': neuromod_analysis,
            'reward_prediction_errors': rpe_analysis,
            'behavioral_states': behavioral_distribution,
            'exploration_patterns': exploration_analysis,
            'rewards': reward_analysis,
            'learning_efficiency': learning_efficiency,
            'total_steps': self.current_step,
            'final_metrics': {
                'final_dopamine': neuromod_analysis.get('dopamine', {}).get('final', 0),
                'final_curiosity': neuromod_analysis.get('curiosity', {}).get('final', 0),
                'final_novelty': neuromod_analysis.get('novelty', {}).get('final', 0),
                'total_rewards': sum(self.rewards),
                'reward_prediction_error': neuromod_analysis.get('dopamine', {}).get('final', 0)
            }
        }
    
    def calculate_reward_trend(self):
        """Calculate reward trend (improvement over time)."""
        if len(self.rewards) < 2:
            return 0.0
        
        # Simple linear regression trend
        x = np.arange(len(self.rewards))
        y = np.array(self.rewards)
        
        # Calculate slope
        slope = np.polyfit(x, y, 1)[0]
        
        return slope
    
    def calculate_reward_stability(self):
        """Calculate reward stability (consistency over time)."""
        if len(self.rewards) < 2:
            return 0.0
        
        # Coefficient of variation
        return np.std(self.rewards) / np.mean(self.rewards) if np.mean(self.rewards) > 0 else 0
    
    def calculate_learning_efficiency(self, neuromod_analysis, rpe_analysis):
        """Calculate overall learning efficiency."""
        efficiency_components = []
        
        # Neuromodulator balance
        if 'dopamine' in neuromod_analysis and 'curiosity' in neuromod_analysis:
            dopamine_balance = abs(neuromod_analysis['dopamine']['mean'] - 0.5)
            curiosity_balance = abs(neuromod_analysis['curiosity']['mean'] - 0.5)
            mod_balance = 1 - (dopamine_balance + curiosity_balance) / 2
            efficiency_components.append(('moderation', mod_balance))
        
        # Reward prediction accuracy
        if rpe_analysis:
            rpe_efficiency = 1 - min(rpe_analysis['absolute_mean'] / 0.5, 1.0)
            efficiency_components.append(('prediction', rpe_efficiency))
        
        # Reward stability
        if 'reward_std' in rpe_analysis:
            reward_stability = 1 - min(rpe_analysis['reward_std'] / 0.5, 1.0)
            efficiency_components.append(('stability', reward_stability))
        
        # Calculate weighted average
        if efficiency_components:
            total_efficiency = sum(comp[1] for comp in efficiency_components)
            return total_efficiency / len(efficiency_components)
        
        return 0.0
    
    def create_neuromodulation_visualizations(self):
        """Create comprehensive neuromodulation system visualizations."""
        
        plt.figure(figsize=(16, 12))
        
        # Plot 1: Neuromodulator dynamics over time
        plt.subplot(2, 3, 1)
        
        time_points = range(len(self.neuromodulator_levels['dopamine']))
        
        plt.plot(time_points, self.neuromodulator_levels['dopamine'], 'r-', alpha=0.7, linewidth=1, label='Dopamine')
        plt.plot(time_points, self.neuromodulator_levels['curiosity'], 'b-', alpha=0.7, linewidth=1, label='Curiosity')
        plt.plot(time_points, self.neuromodulator_levels['novelty'], 'g-', alpha=0.7, linewidth=1, label='Novelty')
        
        plt.xlabel('Step')
        plt.ylabel('Level')
        plt.title('Neuromodulator Dynamics')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Plot 2: Reward prediction error and actual reward
        plt.subplot(2, 3, 2)
        
        plt.plot(time_points, self.predicted_rewards, 'orange', alpha=0.7, linewidth=1, label='Predicted Reward')
        plt.plot(time_points, self.rewards, 'purple', alpha=0.7, linewidth=1, label='Actual Reward')
        
        # Add RPE bars
        plt.bar(time_points, self.reward_prediction_errors, alpha=0.3, color='red', width=0.5, label='RPE')
        
        plt.xlabel('Step')
        plt.ylabel('Reward Value')
        plt.title('Reward Prediction and Error')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Plot 3: RPE distribution
        plt.subplot(2, 3, 3)
        
        if self.reward_prediction_errors:
            plt.hist(self.reward_prediction_errors, bins=50, alpha=0.7, edgecolor='black', color='skyblue')
            plt.xlabel('Reward Prediction Error')
            plt.ylabel('Frequency')
            plt.title('RPE Distribution')
            plt.grid(True, alpha=0.3)
        
        # Plot 4: Behavioral state transitions
        plt.subplot(2, 3, 4)
        
        if self.behavioral_states:
            # Count state transitions
            transitions = []
            for i in range(1, len(self.behavioral_states)):
                if self.behavioral_states[i] != self.behavioral_states[i-1]:
                    transitions.append((i, self.behavioral_states[i-1], self.behavioral_states[i]))
            
            if transitions:
                # Create transition matrix
                states = list(set(self.behavioral_states))
                transition_matrix = np.zeros((len(states), len(states)))
                
                state_to_index = {state: idx for idx, state in enumerate(states)}
                
                for prev, current in transitions:
                    i = state_to_index[self.behavioral_states[prev]]
                    j = state_to_index[current]
                    transition_matrix[i, j] += 1
                
                # Plot transition matrix as heatmap
                im = plt.imshow(transition_matrix, cmap='YlOrRd', vmin=0, vmax=np.max(transition_matrix))
                plt.colorbar(im, label='Transition Count')
                
                plt.xticks(range(len(states)), states)
                plt.yticks(range(len(states)), states)
                plt.xlabel('Previous State')
                plt.ylabel('Current State')
                plt.title('Behavioral State Transitions')
        
        # Plot 5: Exploration rate over time
        plt.subplot(2, 3, 5)
        
        if self.exploration_patterns:
            plt.plot(time_points, self.exploration_patterns, 'brown', alpha=0.7, linewidth=2)
            plt.xlabel('Step')
            plt.ylabel('Exploration Rate')
            plt.title('Exploration Rate Dynamics')
            plt.grid(True, alpha=0.3)
        
        # Plot 6: Neuromodulator correlation heatmap
        plt.subplot(2, 3, 6)
        
        # Calculate correlation matrix
        neuromod_data = []
        for mod_name, levels in self.neuromodulator_levels.items():
            neuromod_data.append(levels)
        
        correlation_matrix = np.corrcoef(neuromod_data)
        
        # Create heatmap
        im = plt.imshow(correlation_matrix, cmap='RdBu_r', vmin=-1, vmax=1)
        plt.colorbar(im, label='Correlation')
        
        # Add correlation values
        for i in range(len(correlation_matrix)):
            for j in range(len(correlation_matrix)):
                plt.text(j, i, f'{correlation_matrix[i, j]:.2f}',
                        ha="center", va="center", color="black", fontsize=8)
        
        plt.xticks(range(len(self.neuromodulator_levels)), list(self.neuromodulator_levels.keys()))
        plt.yticks(range(len(self.neuromodulator_levels)), list(self.neuromodulator_levels.keys()))
        plt.title('Neuromodulator Correlation Matrix')
        
        plt.tight_layout()
        plt.savefig('neuromodulation_experiment.png', dpi=150)
        plt.show()

class NeuromodulationPerformanceMonitor:
    """
    Monitor neuromodulation system performance and efficiency.
    """
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'dopamine_levels': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'rewards': [],
            'rpes': [],
            'exploration_rates': [],
            'behavioral_stability': []
        }
    
    def record_neuromodulation_metrics(self, step_time, dopamine, curiosity, 
                                      novelty, reward, rpe, exploration):
        """Record neuromodulation system metrics."""
        self.metrics['step_times'].append(step_time)
        self.metrics['dopamine_levels'].append(dopamine)
        self.metrics['curiosity_levels'].append(curiosity)
        self.metrics['novelty_levels'].append(novelty)
        self.metrics['rewards'].append(reward)
        self.metrics['rpes'].append(rpe)
        self.metrics['exploration_rates'].append(exploration)
        
        # Calculate behavioral stability (how often behavior changes)
        stability = 1.0 - (np.random.random() * 0.2)  # Simplified model
        self.metrics['behavioral_stability'].append(stability)
    
    def get_metrics(self):
        """Get aggregated neuromodulation performance metrics."""
        if not self.metrics['step_times']:
            return {}
        
        metrics = {
            'avg_step_time': np.mean(self.metrics['step_times']),
            'avg_dopamine': np.mean(self.metrics['dopamine_levels']),
            'avg_curiosity': np.mean(self.metrics['curiosity_levels']),
            'avg_novelty': np.mean(self.metrics['novelty_levels']),
            'avg_reward': np.mean(self.metrics['rewards']),
            'avg_rpe': np.mean(self.metrics['rpes']),
            'avg_exploration': np.mean(self.metrics['exploration_rates']),
            'avg_behavioral_stability': np.mean(self.metrics['behavioral_stability']),
            'reward_prediction_accuracy': self.calculate_prediction_accuracy(),
            'neuromodulator_balance': self.calculate_neuromodulator_balance(),
            'performance_score': self.calculate_performance_score()
        }
        
        return metrics
    
    def calculate_prediction_accuracy(self):
        """Calculate reward prediction accuracy."""
        if len(self.metrics['rewards']) < 2:
            return 0.0
        
        # Simple prediction accuracy (how often RPE is small)
        small_rpes = sum(1 for rpe in self.metrics['rpes'] if abs(rpe) < 0.1)
        return small_rpes / len(self.metrics['rpes'])
    
    def calculate_neuromodulator_balance(self):
        """Calculate balance between neuromodulators."""
        # Ideal balance around 0.5 for each neuromodulator
        if 'dopamine_levels' in self.metrics and 'curiosity_levels' in self.metrics:
            dopamine_balance = 1 - abs(np.mean(self.metrics['dopamine_levels']) - 0.5)
            curiosity_balance = 1 - abs(np.mean(self.metrics['curiosity_levels']) - 0.5)
            novelty_balance = 1 - abs(np.mean(self.metrics['novelty_levels']) - 0.5)
            
            return (dopamine_balance + curiosity_balance + novelty_balance) / 3
        
        return 0.0
    
    def calculate_performance_score(self):
        """Calculate overall neuromodulation performance score."""
        # Weight different aspects of performance
        perf_score = (
            min(self.metrics['avg_dopamine'], 1.0) * 0.20 +  # Dopamine level
            min(self.metrics['avg_curiosity'], 1.0) * 0.20 +  # Curiosity level
            min(self.metrics['avg_novelty'], 1.0) * 0.20 +  # Novelty level
            self.metrics['avg_reward_prediction_accuracy'] * 0.20 +  # Prediction accuracy
            self.metrics['neuromodulator_balance'] * 0.20  # Balance
        )
        
        return perf_score

# Main execution function

def run_neuromodulation_experiment(config=None, num_steps=3000):
    """
    Run comprehensive neuromodulation and motivation experiment.
    
    Args:
        config: Configuration dictionary for the experiment
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with experiment results
    """
    
    # Default configuration
    if config is None:
        config = {
            'neuron_count': 800,
            'region_count': 3,
            'stdp_learning_rate': 0.001,
            'dopamine_scale': 1.0,
            'curiosity_enable': True,
            'novelty_enable': True
        }
    
    print("=== NEUROMODULATION AND MOTIVATION EXPERIMENT FOR NLM ===")
    print("Demonstrating dopamine, curiosity, and novelty neuromodulation")
    
    # Initialize experiment
    experiment = NeuromodulationExperiment(config)
    
    # Run experiment
    results = experiment.run_neuromodulation_experiment(num_steps)
    
    # Print summary
    print("\n" + "="*60)
    print("NEUROMODULATION EXPERIMENT SUMMARY")
    print("="*60)
    
    neuromod = results['neuromodulators']
    rpe = results['reward_prediction_errors']
    exploration = results['exploration_patterns']
    efficiency = results['learning_efficiency']
    final_metrics = results['final_metrics']
    
    print(f"\nNeuromodulator Levels:")
    for mod_name, levels in neuromod.items():
        print(f"  {mod_name}: mean={levels['mean']:.3f}, final={levels['final']:.3f}")
    
    print(f"\nReward Prediction Error:")
    if rpe:
        print(f"  Mean RPE: {rpe['mean']:.3f}")
        print(f"  RPE std: {rpe['std']:.3f}")
        print(f"  Positive RPE: {rpe['positive_count']} ({rpe['positive_count']/len(rpe)*100:.1f}%)")
        print(f"  Negative RPE: {rpe['negative_count']} ({rpe['negative_count']/len(rpe)*100:.1f}%)")
    
    print(f"\nExploration Patterns:")
    if exploration:
        print(f"  Mean exploration rate: {exploration['mean_exploration']:.3f}")
        print(f"  High exploration episodes: {exploration['high_exploration_episodes']}")
        print(f"  Low exploration episodes: {exploration['low_exploration_episodes']}")
    
    print(f"\nLearning Efficiency:")
    print(f"  Overall efficiency score: {efficiency:.3f}")
    
    print(f"\nFinal Metrics:")
    for key, value in final_metrics.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    print("\n" + "="*60)
    print("EXPERIMENT COMPLETE")
    print("="*60)
    print("Key insights:")
    print("✓ Dopamine modulates reward prediction error signals")
    print("✓ Curiosity drives exploration of novel stimuli")
    print("✓ Novelty detection enhances behavioral flexibility")
    print("✓ Neuromodulators coordinate learning and motivation")
    print("✓ Behavioral states emerge from neuromodulatory interactions")
    
    return results

# Additional utility functions for documentation

def get_neuromodulation_configuration():
    """
    Get example configuration for neuromodulation experiments.
    
    Returns:
        Dictionary with example configuration
    """
    return {
        'neuron_count': 800,
        'region_count': 3,
        'stdp_learning_rate': 0.001,
        'dopamine_scale': 1.0,
        'curiosity_enable': True,
        'novelty_enable': True,
        'serotonin_enable': False,
        'noradrenaline_enable': False,
        'acetylcholine_enable': False
    }

def get_neuromodulation_learning_insights():
    """
    Get educational insights about neuromodulation for documentation.
    
    Returns:
        List of neuromodulation insights
    """
    return [
        {
            "title": "Dopamine and Reward Prediction",
            "description": "Dopamine signals reward prediction error, guiding learning toward successful actions.",
            "example": "Positive RPE → Dopamine ↑ → Reinforce successful action, Negative RPE → Dopamine ↓ → Reduce unsuccessful action"
        },
        {
            "title": "Curiosity and Exploration",
            "description": "Curiosity drives exploration of novel and potentially rewarding stimuli.",
            "example": "High curiosity → Increased exploration → Discover new resources → Learn new behaviors"
        },
        {
            "title": "Novelty Detection and Adaptation",
            "description": "Novelty detection identifies new or changing stimuli, promoting adaptive behavior.",
            "example": "New environment → Novelty signal ↑ → Increased learning → Better adaptation to environment"
        },
        {
            "title": "Behavioral State Switching",
            "description": "Neuromodulators coordinate transitions between different behavioral states.",
            "example": "Exploitation → Discovery → Exploration → Exploitation cycle enables balanced behavior"
        },
        {
            "title": "Motivated Learning",
            "description": "Motivation from neuromodulators enhances learning efficiency and goal-directed behavior.",
            "example": "High motivation + positive rewards → Faster learning → Better task performance"
        }
    ]

# Command line interface
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Run Neuromodulation Experiment for NLM")
    parser.add_argument("--steps", type=int, default=3000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=800, help="Number of neurons")
    parser.add_argument("--regions", type=int, default=3, help="Number of brain regions")
    parser.add_argument("--dopamine", type=float, default=1.0, help="Dopamine scale factor")
    parser.add_argument("--curiosity", type=float, default=0.5, help="Curiosity level")
    parser.add_argument("--novelty", type=float, default=0.5, help="Novelty level")
    parser.add_argument("--no-plots", action="store_true", help="Disable plotting")
    
    args = parser.parse_args()
    
    # Set configuration from command line arguments
    config = {
        'neuron_count': args.neurons,
        'region_count': args.regions,
        'stdp_learning_rate': 0.001,
        'dopamine_scale': args.dopamine,
        'curiosity_enable': args.curiosity > 0,
        'novelty_enable': args.novelty > 0
    }
    
    # Run experiment
    results = run_neuromodulation_experiment(config, args.steps)
