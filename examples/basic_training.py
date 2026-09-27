"""
NLM Basic Training Example
==========================

Demonstrates basic usage of the NLM Training Module with
easy setup, training loop execution, and performance monitoring.

This example shows:
1. Creating a simple training setup
2. Configuring training parameters
3. Running basic training with monitoring
4. Evaluating agent performance
5. Basic experiment tracking

Prerequisites:
- NLM C++ bindings must be built and installed
- Python 3.8+ with pynlm module
- Training module dependencies installed
"""

import pynlm as nlm
import time
from typing import Dict, List, Any

class BasicTrainingExample:
    """Example class demonstrating basic NLM training."""
    
    def __init__(self):
        self.experiment_name = "BasicTrainingExample"
        self.training_results = {}
        
    def create_training_setup(self, config: Dict[str, Any] = None) -> Dict[str, Any]:
        """Create a complete training setup."""
        print("1. Creating training setup...")
        
        # Create brain with configuration
        brain_config = config.get('brain_config', {
            'neuron_count': 500,
            'complexity': 0.6,
            'excitatory_neurons': 400,
            'inhibitory_neurons': 100
        })
        
        brain = nlm.NLMBrain(
            neuron_count=brain_config['neuron_count'],
            complexity=brain_config['complexity'],
            config={
                "brain": {
                    "excitatory_neurons": brain_config['excitatory_neurons'],
                    "inhibitory_neurons": brain_config['inhibitory_neurons'],
                    "v_thresh": -50.0,
                    "tau_mem": 20.0
                }
            }
        )
        
        # Create world
        world = nlm.createSimpleWorld()
        world_width = config.get('world_width', 15)
        world_height = config.get('world_height', 15)
        world.configure(
            width=world_width,
            height=world_height,
            visionWidth=6,
            visionHeight=6
        )
        world.reset()
        world.setAgentStart(world_width / 2, world_height / 2)
        
        # Create agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create training trainer
        trainer = nlm.createTrainingCore()
        
        # Enable learning features
        brain.enable_features(reward_modulation=True, curiosity=True)
        agent.enable_learning(True)
        
        print(f"   ✓ Brain created: {brain.get_total_neurons()} neurons")
        print(f"   ✓ World configured: {world_width}x{world_height}")
        print(f"   ✓ Agent initialized")
        print(f"   ✓ Training trainer ready")
        
        return {
            'brain': brain,
            'world': world,
            'agent': agent,
            'trainer': trainer
        }
    
    def configure_training(self, training_config: Dict[str, Any] = None) -> Dict[str, Any]:
        """Configure training parameters."""
        print("2. Configuring training parameters...")
        
        default_config = {
            'max_episodes': 100,
            'learning_rate': 0.001,
            'discount_factor': 0.99,
            'exploration_rate': 0.1,
            'reward_type': 'weighted',
            'min_performance': 0.8,
            'patience': 10,
            'early_stopping': True,
            'checkpoint_frequency': 20
        }
        
        if training_config:
            default_config.update(training_config)
        
        config = nlm.TrainingConfig()
        config.maxEpisodes = default_config['max_episodes']
        config.learningRate = default_config['learning_rate']
        config.discountFactor = default_config['discount_factor']
        config.explorationRate = default_config['exploration_rate']
        
        # Configure reward system
        config.rewardConfig.type = nlm.RewardConfig.Type.WEIGHTED
        config.rewardConfig.weighted.stepCompletion = 1.0
        config.rewardConfig.weighted.novelty = 0.5
        config.rewardConfig.weighted.survival = 0.8
        config.rewardConfig.weighted.efficiency = 1.0
        
        # Configure stopping criteria
        config.earlyStopping = default_config['early_stopping']
        config.patience = default_config['patience']
        config.minPerformance = default_config['min_performance']
        
        print(f"   ✓ Max episodes: {config.maxEpisodes}")
        print(f"   ✓ Learning rate: {config.learningRate}")
        print(f"   ✓ Discount factor: {config.discountFactor}")
        print(f"   ✓ Exploration rate: {config.explorationRate}")
        print(f"   ✓ Reward type: {config.rewardConfig.type}")
        print(f"   ✓ Early stopping: {config.earlyStopping}")
        print(f"   ✓ Patience: {config.patience}")
        
        return config
    
    def create_callback(self) -> nlm.TrainingCallback:
        """Create a simple callback for training monitoring."""
        print("3. Setting up training monitoring...")
        
        class BasicTrainingCallback(nlm.TrainingCallback):
            def __init__(self):
                self.episode_rewards = []
                self.step_losses = []
                self.episode_count = 0
                self.best_reward = float('-inf')
                
            def afterEpisode(self, episode, reward, success):
                self.episode_rewards.append(reward)
                self.episode_count = episode + 1
                
                if reward > self.best_reward:
                    self.best_reward = reward
                
                # Log progress
                if episode % 20 == 0:
                    avg_reward = sum(self.episode_rewards) / len(self.episode_rewards)
                    print(f"     Episode {episode}: Reward = {reward:.2f}, "
                          f"Success = {success}, Avg = {avg_reward:.2f}")
                
            def afterStep(self, episode, step, reward, loss):
                self.step_losses.append(loss)
                
            def getPerformanceSummary(self):
                """Get comprehensive performance summary."""
                if not self.episode_rewards:
                    return {
                        'episodes_completed': 0,
                        'status': 'no_data'
                    }
                
                avg_reward = sum(self.episode_rewards) / len(self.episode_rewards)
                avg_loss = sum(self.step_losses) / len(self.step_losses) if self.step_losses else 0
                
                return {
                    'episodes_completed': len(self.episode_rewards),
                    'total_reward': sum(self.episode_rewards),
                    'average_reward': avg_reward,
                    'best_reward': max(self.episode_rewards),
                    'worst_reward': min(self.episode_rewards),
                    'average_loss': avg_loss,
                    'episodes_successful': sum(1 for r in self.episode_rewards if r > 10.0),
                    'success_rate': sum(1 for r in self.episode_rewards if r > 10.0) / len(self.episode_rewards),
                    'learning_progress': {
                        'initial_performance': self.episode_rewards[0] if len(self.episode_rewards) > 0 else 0,
                        'final_performance': self.episode_rewards[-1] if len(self.episode_rewards) > 0 else 0,
                        'improvement': (self.episode_rewards[-1] - (self.episode_rewards[0] if len(self.episode_rewards) > 0 else 0)) / max(1, len(self.episode_rewards) - 1)
                    }
                }
        
        callback = BasicTrainingCallback()
        print("   ✓ Basic callback created with episode tracking")
        print("   ✓ Progress logging configured (every 20 episodes)")
        print("   ✓ Performance tracking enabled")
        
        return callback
    
    def simple_reward_function(self, world, brain, action, step):
        """Simple reward function for basic training."""
        reward = 1.0  # Base reward
        
        # Learning activity reward
        firing_rate = brain.get_average_firing_rate()
        if 5.0 < firing_rate < 30.0:
            reward += firing_rate * 0.02
        elif firing_rate < 2.0:
            reward -= 0.5
        
        # Novelty exploration reward
        novelty = agent.get_novelty_level() if hasattr(agent, 'get_novelty_level') else 0.0
        if novelty > 0.3:
            reward += novelty * 2.0
        
        return reward
    
    def run_training(self, setup: Dict[str, Any], config: Dict[str, Any], callback: nlm.TrainingCallback) -> Dict[str, Any]:
        """Run basic training with the given setup and configuration."""
        print("4. Running basic training...")
        
        brain = setup['brain']
        world = setup['world']
        agent = setup['agent']
        trainer = setup['trainer']
        
        # Start timer
        start_time = time.time()
        
        # Run training
        print("   Starting training loop...")
        training_complete = trainer.trainAgent(
            brain, world, agent, config,
            [callback]
        )
        
        # Calculate duration
        training_time = time.time() - start_time
        
        # Get performance summary
        performance = callback.getPerformanceSummary()
        
        print(f"   ✓ Training completed: {performance['episodes_completed']} episodes")
        print(f"   ✓ Average reward: {performance['average_reward']:.3f}")
        print(f"   ✓ Best reward: {performance['best_reward']:.3f}")
        print(f"   ✓ Success rate: {performance['success_rate']:.2%}")
        print(f"   ✓ Training time: {training_time:.2f} seconds")
        
        # Store results
        self.training_results = {
            'config': config.__dict__ if hasattr(config, '__dict__') else config,
            'performance': performance,
            'training_time': training_time,
            'complete': training_complete
        }
        
        return self.training_results
    
    def evaluate_agent(self, setup: Dict[str, Any], config: Dict[str, Any]) -> Dict[str, Any]:
        """Evaluate agent performance."""
        print("5. Evaluating agent performance...")
        
        brain = setup['brain']
        world = setup['world']
        agent = setup['agent']
        trainer = setup['trainer']
        
        # Create evaluation test cases
        test_cases = [
            {
                'type': 'connectivity',
                'steps': 100,
                'target_metrics': {'min_active': 10},
                'weight': 1.0
            },
            {
                'type': 'firing',
                'steps': 80,
                'target_metrics': {'min_firing_rate': 1.0},
                'weight': 1.0
            },
            {
                'type': 'learning',
                'steps': 150,
                'target_metrics': {'stage_change': True},
                'weight': 1.0
            }
        ]
        
        # Run evaluation
        evaluation_results = nlm.evaluate_performance(brain, test_cases, metrics=['speed', 'stability'])
        
        print(f"   ✓ Test cases evaluated: {evaluation_results['test_cases_evaluated']}")
        print(f"   ✓ Overall performance: {evaluation_results['overall_performance']:.3f}")
        print(f"   ✓ Performance score: {evaluation_results['performance_scores']}")
        
        if evaluation_results['recommendations']:
            print("   Recommendations:")
            for rec in evaluation_results['recommendations']:
                print(f"     - {rec}")
        
        return evaluation_results
    
    def run_complete_example(self) -> Dict[str, Any]:
        """Run the complete basic training example."""
        print("=" * 60)
        print(f"NLM Basic Training Example")
        print("=" * 60)
        
        # 1. Create training setup
        setup = self.create_training_setup({
            'world_width': 15,
            'world_height': 15,
            'brain_config': {
                'neuron_count': 600,
                'complexity': 0.7,
                'excitatory_neurons': 480,
                'inhibitory_neurons': 120
            }
        })
        
        # 2. Configure training
        config = self.configure_training({
            'max_episodes': 150,
            'learning_rate': 0.001,
            'exploration_rate': 0.15,
            'min_performance': 0.7,
            'patience': 15
        })
        
        # 3. Create callback
        callback = self.create_callback()
        
        # 4. Run training
        training_results = self.run_training(setup, config, callback)
        
        # 5. Evaluate agent
        evaluation_results = self.evaluate_agent(setup, config)
        
        # 6. Display summary
        self.display_summary(training_results, evaluation_results)
        
        return {
            'example': 'BasicTraining',
            'training_results': training_results,
            'evaluation_results': evaluation_results,
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S')
        }
    
    def display_summary(self, training_results: Dict[str, Any], evaluation_results: Dict[str, Any]):
        """Display training summary."""
        print("\n" + "=" * 60)
        print("TRAINING SUMMARY")
        print("=" * 60)
        
        performance = training_results.get('performance', {})
        
        print(f"Example: {self.experiment_name}")
        print(f"Training completed: {'Yes' if training_results.get('complete') else 'No'}")
        print(f"\nPerformance Metrics:")
        print(f"  Episodes completed: {performance.get('episodes_completed', 'N/A')}")
        print(f"  Average reward: {performance.get('average_reward', 0):.3f}")
        print(f"  Best reward: {performance.get('best_reward', 0):.3f}")
        print(f"  Success rate: {performance.get('success_rate', 0):.2%}")
        print(f"  Training time: {training_results.get('training_time', 0):.2f} seconds")
        
        print(f"\nEvaluation Results:")
        print(f"  Overall performance: {evaluation_results.get('overall_performance', 0):.3f}")
        print(f"  Test cases evaluated: {evaluation_results.get('test_cases_evaluated', 'N/A')}")
        
        print(f"\nKey Insights:")
        if performance.get('average_reward', 0) > 10.0:
            print(f"  ✓ Good learning performance (avg reward > 10)")
        else:
            print(f"  ⚠ Moderate learning performance (avg reward <= 10)")
            
        if performance.get('success_rate', 0) > 0.5:
            print(f"  ✓ High episode success rate (> 50%)")
        else:
            print(f"  ⚠ Low episode success rate (<= 50%)")
            
        if evaluation_results.get('overall_performance', 0) > 0.5:
            print(f"  ✓ Good overall system performance (> 0.5)")
        else:
            print(f"  ⚠ Need improvement (<= 0.5)")
        
        print(f"\nNext Steps:")
        print(f"  1. Experiment with different brain configurations")
        print(f"  2. Try different reward functions for specific tasks")
        print(f"  3. Use hyperparameter optimization for better parameters")
        print(f"  4. Set up advanced features (curiosity, curriculum learning)")
        print(f"  5. Implement experiment tracking for reproducibility")

def main():
    """Run the basic training example."""
    print("Starting NLM Basic Training Example...")
    print("This example demonstrates core training concepts with NLM.")
    
    try:
        example = BasicTrainingExample()
        results = example.run_complete_example()
        
        print(f"\n✓ Basic Training Example completed successfully!")
        print(f"Results saved: {json.dumps(results, indent=2)}")
        
        return results
        
    except Exception as e:
        print(f"\nError running basic training example: {e}")
        print("Please ensure NLM C++ bindings and training module are properly installed.")
        return {"error": str(e)}

if __name__ == '__main__':
    try:
        result = main()
        exit(0)
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        exit(1)
