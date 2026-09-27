"""
NLM Advanced Training Example
============================

Advanced demonstration of NLM Training Module with
callback systems, advanced features, and comprehensive monitoring.

This example shows:
1. Advanced training configuration
2. Complex callback systems
3. Integration with curiosity-driven exploration
4. Performance monitoring and analysis
5. Experiment tracking with persistence

Prerequisites:
- NLM C++ bindings must be built and installed
- Python 3.8+ with pynlm module
- Training module dependencies installed
"""

import pynlm as nlm
import time
import json
import os
from typing import Dict, List, Any, Callable
from datetime import datetime

class AdvancedTrainingExample:
    """Advanced example demonstrating complex training scenarios."""
    
    def __init__(self):
        self.experiment_name = "AdvancedTrainingExample"
        self.session_data = {}
        self.performance_metrics = {}
        
    def setup_advanced_environment(self) -> Dict[str, Any]:
        """Setup advanced training environment with all features."""
        print("1. Setting up advanced training environment...")
        
        # Create complex brain configuration
        brain = nlm.NLMBrain(
            neuron_count=2000,
            complexity=0.85,
            config={
                "brain": {
                    "excitatory_neurons": 1700,
                    "inhibitory_neurons": 300,
                    "v_thresh": -40.0,
                    "tau_mem": 25.0,
                    "g_leak": 0.3
                },
                "plasticity": {
                    "stdp": {
                        "learning_rate": 0.005,
                        "tau_plus": 30.0,
                        "tau_minus": 30.0,
                        "A_plus": 0.01,
                        "A_minus": 0.01
                    },
                    "hebbian": {
                        "learning_rate": 0.002,
                        "enable": True
                    },
                    "structural": {
                        "enable": True,
                        "growth_rate": 0.001,
                        "pruning_rate": 0.0005
                    }
                }
            }
        )
        
        # Create advanced world
        world = nlm.createSimpleWorld()
        world.configure(
            width=30,
            height=30,
            visionWidth=12,
            visionHeight=12
        )
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Create advanced agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create advanced trainer
        trainer = nlm.createTrainingCore()
        
        # Enable ALL advanced features
        brain.enable_features(
            reward_modulation=True,
            structural_plasticity=True,
            development=True,
            curiosity=True
        )
        
        # Configure world for advanced simulation
        world.enable_features(
            exploration=True,
            reward=True,
            novelty=True,
            competition=True,
            social=True
        )
        
        agent.enable_learning(True)
        
        print(f"   ✓ Advanced brain created: {brain.get_total_neurons()} neurons")
        print(f"   ✓ Advanced world configured: 30x30 with full features")
        print(f"   ✓ Advanced agent initialized with all learning capabilities")
        print(f"   ✓ Training trainer with monitoring ready")
        
        return {
            'brain': brain,
            'world': world,
            'agent': agent,
            'trainer': trainer
        }
    
    def configure_advanced_training(self) -> Dict[str, Any]:
        """Configure advanced training with complex parameters."""
        print("2. Configuring advanced training parameters...")
        
        config = nlm.TrainingConfig()
        
        # Basic training parameters
        config.maxEpisodes = 1000
        config.maxStepsPerEpisode = 300
        config.timeStep = 0.05
        config.learningRate = 0.002
        config.discountFactor = 0.995
        config.explorationRate = 0.2
        
        # Advanced reward configuration
        config.rewardConfig.type = nlm.RewardConfig.Type.WEIGHTED
        config.rewardConfig.weighted.stepCompletion = 1.0
        config.rewardConfig.weighted.novelty = 0.8
        config.rewardConfig.weighted.survival = 1.2
        config.rewardConfig.weighted.efficiency = 1.5
        config.rewardConfig.weighted.curiosity = 1.0
        config.rewardConfig.weighted.predictionError = 0.7
        config.rewardConfig.weighted.social = 0.6
        config.rewardConfig.weighted.competition = 0.5
        
        # Advanced stopping criteria
        config.earlyStopping = True
        config.patience = 50
        config.minPerformance = 0.9
        config.improvementThreshold = 0.005
        
        # Performance monitoring
        config.checkpointFrequency = 100
        config.saveCheckpoints = True
        config.logFrequency = 10
        
        # Advanced feature configuration
        config.curiosityEnabled = True
        config.curriculumEnabled = True
        config.transferEnabled = True
        config.metaLearningEnabled = True
        
        print(f"   ✓ Max episodes: {config.maxEpisodes}")
        print(f"   ✓ Max steps per episode: {config.maxStepsPerEpisode}")
        print(f"   ✓ Learning rate: {config.learningRate}")
        print(f"   ✓ Discount factor: {config.discountFactor}")
        print(f"   ✓ Exploration rate: {config.explorationRate}")
        print(f"   ✓ Reward type: {config.rewardConfig.type}")
        print(f"   ✓ Early stopping: {config.earlyStopping}")
        print(f"   ✓ Patience: {config.patience}")
        print(f"   ✓ Min performance: {config.minPerformance}")
        print(f"   ✓ Advanced features enabled")
        
        return config
    
    def create_advanced_callbacks(self) -> List[nlm.TrainingCallback]:
        """Create sophisticated callback system for advanced monitoring."""
        print("3. Setting up advanced callback system...")
        
        callbacks = []
        
        # 1. Performance Tracking Callback
        class PerformanceTrackingCallback(nlm.TrainingCallback):
            def __init__(self):
                self.episode_data = []
                self.step_data = []
                self.feature_usage = {}
                self.performance_history = []
                
            def afterEpisode(self, episode, reward, success):
                self.episode_data.append({
                    'episode': episode,
                    'reward': reward,
                    'success': success,
                    'timestamp': time.time()
                })
                
                # Track feature usage
                if hasattr(agent, 'isCuriosityEnabled') and agent.isCuriosityEnabled():
                    curiosity = agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.0
                    self.feature_usage['curiosity'] = curiosity
                
                if hasattr(agent, 'isCurriculumEnabled') and agent.isCurriculumEnabled():
                    curriculum = agent.getDevelopmentalStage() if hasattr(agent, 'getDevelopmentalStage') else 'Unknown'
                    self.feature_usage['curriculum'] = curriculum
                
                # Calculate performance metrics
                avg_reward = sum(d['reward'] for d in self.episode_data) / len(self.episode_data)
                success_rate = sum(1 for d in self.episode_data if d['success']) / len(self.episode_data)
                
                self.performance_history.append({
                    'episode': episode,
                    'avg_reward': avg_reward,
                    'success_rate': success_rate,
                    'best_reward': max(d['reward'] for d in self.episode_data),
                    'worst_reward': min(d['reward'] for d in self.episode_data)
                })
                
                # Log progress
                if episode % 100 == 0:
                    print(f"     Advanced Episode {episode}: Reward = {reward:.2f}, "
                          f"Success = {success}, Avg Reward = {avg_reward:.2f}")
            
            def afterStep(self, episode, step, reward, loss):
                self.step_data.append({
                    'episode': episode,
                    'step': step,
                    'reward': reward,
                    'loss': loss,
                    'timestamp': time.time()
                })
                
            def getAdvancedPerformanceSummary(self):
                if not self.performance_history:
                    return {"episodes_completed": 0}
                
                latest = self.performance_history[-1]
                
                return {
                    'episodes_completed': len(self.episode_data),
                    'total_steps': len(self.step_data),
                    'current_performance': latest,
                    'overall_performance': {
                        'average_reward': sum(d['reward'] for d in self.episode_data) / len(self.episode_data),
                        'final_success_rate': sum(1 for d in self.episode_data if d['success']) / len(self.episode_data),
                        'learning_curve': [p['avg_reward'] for p in self.performance_history],
                        'feature_usage_summary': self.feature_usage
                    }
                }
        
        # 2. Advanced Features Monitoring Callback
        class AdvancedFeaturesCallback(nlm.TrainingCallback):
            def __init__(self):
                self.feature_stats = {}
                self.interaction_patterns = {}
                
            def afterEpisode(self, episode, reward, success):
                # Monitor and adapt advanced features
                if hasattr(agent, 'isCuriosityEnabled') and agent.isCuriosityEnabled():
                    curiosity = agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.0
                    
                    # Adjust curiosity based on performance
                    if curiosity > 0.8 and success:
                        print(f"   Curating curiosity reduction for episode {episode}")
                    elif curiosity < 0.3 and not success:
                        print(f"   Increasing curiosity for exploration (episode {episode})")
                
                # Track curriculum progression
                if hasattr(agent, 'isCurriculumEnabled') and agent.isCurriculumEnabled():
                    stage = agent.getDevelopmentalStage() if hasattr(agent, 'getDevelopmentalStage') else 'Unknown'
                    if stage not in self.feature_stats:
                        self.feature_stats[stage] = []
                    self.feature_stats[stage].append(episode)
                    
                    print(f"   Curriculum progression: Episode {episode}, Stage: {stage}")
                
                # Monitor transfer learning
                if hasattr(agent, 'isTransferEnabled') and agent.isTransferEnabled():
                    transfer_efficiency = agent.getTransferEfficiency() if hasattr(agent, 'getTransferEfficiency') else 0.0
                    self.interaction_patterns['transfer'] = transfer_efficiency
                    print(f"   Transfer learning efficiency: {transfer_efficiency:.3f}")
        
        # 3. Real-time Visualization Callback
        class VisualizationCallback(nlm.TrainingCallback):
            def __init__(self):
                self.visualization_data = []
                
            def afterStep(self, episode, step, reward, loss):
                # Collect data for visualization
                self.visualization_data.append({
                    'episode': episode,
                    'step': step,
                    'reward': reward,
                    'loss': loss,
                    'timestamp': time.time()
                })
                
                # Update visualization every 50 steps
                if step % 50 == 0:
                    print(f"     Visualization update: Episode {episode}, Step {step}")
        
        # 4. Experiment Tracking Callback
        class ExperimentTrackingCallback(nlm.TrainingCallback):
            def __init__(self, session_id: str):
                self.session_id = session_id
                self.session_start = time.time()
                self.checkpoints = []
                
            def afterEpisode(self, episode, reward, success):
                # Create experiment record
                experiment_record = {
                    'session_id': self.session_id,
                    'episode': episode,
                    'reward': reward,
                    'success': success,
                    'timestamp': time.time(),
                    'session_duration': time.time() - self.session_start,
                    'features_used': self.getCurrentFeatureUsage()
                }
                
                self.save_experiment_record(experiment_record)
                
                if episode % 200 == 0:
                    checkpoint_path = f"experiments/checkpoint_{self.session_id}_ep{episode}.json"
                    self.save_checkpoint(checkpoint_path, experiment_record)
                    self.checkpoints.append(checkpoint_path)
                    print(f"   ✓ Experiment checkpoint saved: {checkpoint_path}")
            
            def onTrainingEnd(self, trainer, results):
                print(f"   ✓ Training session completed: {self.session_id}")
                print(f"   ✓ {len(self.checkpoints)} checkpoints saved")
                
            def getCurrentFeatureUsage(self):
                # Determine which features were used
                features = []
                if hasattr(agent, 'isCuriosityEnabled') and agent.isCuriosityEnabled():
                    features.append('curiosity')
                if hasattr(agent, 'isCurriculumEnabled') and agent.isCurriculumEnabled():
                    features.append('curriculum')
                if hasattr(agent, 'isTransferEnabled') and agent.isTransferEnabled():
                    features.append('transfer')
                if hasattr(agent, 'isMetaLearningEnabled') and agent.isMetaLearningEnabled():
                    features.append('meta_learning')
                return features
            
            def save_experiment_record(self, record):
                # Save experiment record to file
                filename = f"experiments/experiment_{self.session_id}_{record['episode']}.json"
                with open(filename, 'w') as f:
                    json.dump(record, f, indent=2)
            
            def save_checkpoint(self, filepath, data):
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
        
        # Create and add callbacks
        print("   Creating performance tracking callback...")
        performance_callback = PerformanceTrackingCallback()
        callbacks.append(performance_callback)
        
        print("   Creating advanced features monitoring callback...")
        features_callback = AdvancedFeaturesCallback()
        callbacks.append(features_callback)
        
        print("   Creating visualization callback...")
        visualization_callback = VisualizationCallback()
        callbacks.append(visualization_callback)
        
        # Create session ID for experiment tracking
        session_id = f"adv_train_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        
        print("   Creating experiment tracking callback...")
        experiment_callback = ExperimentTrackingCallback(session_id)
        callbacks.append(experiment_callback)
        
        print(f"   ✓ Advanced callback system created: {len(callbacks)} callbacks")
        print(f"   ✓ Session ID: {session_id}")
        print(f"   ✓ Experiment tracking enabled")
        
        # Store callbacks for later use
        self.session_data['callbacks'] = {
            'performance': performance_callback,
            'features': features_callback,
            'visualization': visualization_callback,
            'experiment': experiment_callback
        }
        
        return callbacks
    
    def create_advanced_reward_function(self) -> Callable:
        """Create sophisticated reward function with multiple factors."""
        print("4. Creating advanced reward function...")
        
        def advanced_reward_function(world, brain, action, step):
            """Advanced reward function combining multiple factors."""
            reward = 1.0  # Base reward
            
            # 1. Primary navigation reward
            if hasattr(world, 'getAgentBody'):
                body = world.getAgentBody()
                center_x = world.getWidth() / 2
                center_y = world.getHeight() / 2
                distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
                normalized_distance = distance / min(world.getWidth(), world.getHeight())
                reward += 2.0 * max(0, 1.0 - normalized_distance)
            
            # 2. Learning activity reward
            firing_rate = brain.get_average_firing_rate()
            if 10.0 < firing_rate < 50.0:
                reward += firing_rate * 0.03
            elif firing_rate > 80.0:
                reward -= firing_rate * 0.02  # Penalize excessive firing
            elif firing_rate < 1.0:
                reward -= 1.0  # Penalize inactivity
            
            # 3. Curiosity-driven exploration reward
            if hasattr(agent, 'isCuriosityEnabled') and agent.isCuriosityEnabled():
                curiosity = agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.0
                reward += curiosity * 3.0
                
                # Novelty bonus
                novelty = agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.0
                if novelty > 0.5:
                    reward += novelty * 5.0
            
            # 4. Survival and efficiency reward
            energy = world.getMaxEnergy() if hasattr(world, 'getMaxEnergy') else 10.0
            if hasattr(world, 'getAgentEnergy'):
                agent_energy = world.getAgentEnergy()
                efficiency = agent_energy / energy
                reward += efficiency * 1.5
            
            # 5. Social and competition reward
            if hasattr(world, 'getCompetitionLevel'):
                competition = world.getCompetitionLevel()
                reward += competition * 0.8
            
            # 6. Development stage reward
            if hasattr(brain, 'get_developmental_stage'):
                stage = brain.get_developmental_stage()
                # Reward for reaching advanced developmental stages
                if stage == 'Adult':
                    reward += 5.0
                elif stage == 'Maturation':
                    reward += 2.0
            
            # 7. Prediction error reward (curiosity bonus)
            if hasattr(agent, 'get_prediction_error'):
                prediction_error = agent.get_prediction_error()
                reward += prediction_error * 2.0
            
            # 8. Curriculum completion reward
            if hasattr(agent, 'is_curriculum_enabled') and agent.is_curriculum_enabled():
                current_stage = agent.get_developmental_stage() if hasattr(agent, 'get_developmental_stage') else None
                if current_stage == 'Adult':
                    reward += 10.0  # Bonus for completing curriculum
            
            # 9. Meta-learning reward
            if hasattr(agent, 'is_meta_learning_enabled') and agent.is_meta_learning_enabled():
                meta_learning_progress = agent.get_meta_learning_progress() if hasattr(agent, 'get_meta_learning_progress') else 0.0
                reward += meta_learning_progress * 3.0
            
            return reward
        
        print("   ✓ Advanced reward function created with multiple factors")
        print("   ✓ Navigation, learning, curiosity, survival, social rewards")
        print("   ✓ Development, meta-learning, efficiency bonuses")
        
        return advanced_reward_function
    
    def run_advanced_training(self, setup: Dict[str, Any], config: Dict[str, Any], callbacks: List[nlm.TrainingCallback]) -> Dict[str, Any]:
        """Run advanced training with sophisticated monitoring."""
        print("5. Running advanced training with monitoring...")
        
        brain = setup['brain']
        world = setup['world']
        agent = setup['agent']
        trainer = setup['trainer']
        
        # Create advanced reward function
        reward_function = self.create_advanced_reward_function()
        
        # Start timer
        start_time = time.time()
        
        # Run advanced training
        print("   Starting advanced training loop...")
        print("   Monitoring: Performance, features, exploration, learning")
        
        training_complete = trainer.trainAgent(
            brain, world, agent, config,
            callbacks
        )
        
        # Calculate duration
        training_time = time.time() - start_time
        
        # Get performance summary from main callback
        performance_callback = self.session_data['callbacks']['performance']
        performance_summary = performance_callback.getAdvancedPerformanceSummary()
        
        # Save session data
        self.performance_metrics = {
            'training_time': training_time,
            'config': config.__dict__ if hasattr(config, '__dict__') else config,
            'performance_summary': performance_summary,
            'feature_usage': performance_callback.feature_usage,
            'session_start': self.session_data.get('start_time', time.time() - training_time)
        }
        
        print(f"   ✓ Advanced training completed")
        print(f"   ✓ Episodes completed: {performance_summary.get('episodes_completed', 0)}")
        print(f"   ✓ Training time: {training_time:.2f} seconds")
        print(f"   ✓ Performance achieved: {performance_summary.get('overall_performance', {}).get('average_reward', 0):.3f} avg reward")
        
        return {
            'training_complete': training_complete,
            'performance_summary': performance_summary,
            'metrics': self.performance_metrics,
            'feature_usage': performance_callback.feature_usage,
            'visualization_data': performance_callback.visualization_data
        }
    
    def save_advanced_results(self, results: Dict[str, Any]):
        """Save advanced training results with comprehensive analysis."""
        print("6. Saving advanced training results...")
        
        # Create results directory
        os.makedirs('advanced_results', exist_ok=True)
        
        # Generate timestamp
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        
        # Create comprehensive results summary
        results_summary = {
            'session_info': {
                'session_id': f"adv_train_{timestamp}",
                'timestamp': timestamp,
                'experiment_name': self.experiment_name,
                'python_version': '.'.join(map(str, nlm.__version__.split('.')[:3])),
                'nelm_version': 'Phase 6'
            },
            'training_configuration': self.performance_metrics.get('config', {}),
            'performance_analysis': self.performance_metrics.get('performance_summary', {}),
            'feature_usage': self.performance_metrics.get('feature_usage', {}),
            'advanced_metrics': {
                'total_training_time': self.performance_metrics.get('training_time', 0),
                'steps_per_second': self.performance_metrics.get('performance_summary', {}).get('total_steps', 0) / max(1, self.performance_metrics.get('training_time', 1)),
                'efficiency_score': self.calculate_efficiency_score(),
                'exploration_rate': self.calculate_exploration_rate()
            },
            'visualization_data': self.session_data.get('callbacks', {}).get('performance', {}).visualization_data if hasattr(self.session_data.get('callbacks', {}).get('performance', {}), 'visualization_data') else []
        }
        
        # Save results to JSON file
        results_file = f"advanced_results/advanced_training_results_{timestamp}.json"
        with open(results_file, 'w') as f:
            json.dump(results_summary, f, indent=2)
        
        # Save performance metrics to CSV
        performance_file = f"advanced_results/advanced_training_metrics_{timestamp}.csv"
        with open(performance_file, 'w') as f:
            f.write("episode,reward,success,avg_reward,success_rate\n")
            for episode_data in self.session_data.get('callbacks', {}).get('performance', {}).episode_data:
                f.write(f"{episode_data['episode']},{episode_data['reward']},{episode_data['success']},\n")
        
        print(f"   ✓ Advanced results saved to {results_file}")
        print(f"   ✓ Performance metrics saved to {performance_file}")
        
        # Create analysis report
        self.create_analysis_report(results_summary)
    
    def calculate_efficiency_score(self) -> float:
        """Calculate overall training efficiency score."""
        performance = self.performance_metrics.get('performance_summary', {})
        
        if not performance:
            return 0.0
        
        # Efficiency factors
        avg_reward = performance.get('overall_performance', {}).get('average_reward', 0)
        episodes_completed = performance.get('episodes_completed', 0)
        
        # Normalize efficiency (0 to 1 scale)
        reward_score = min(avg_reward / 100.0, 1.0)  # Scale based on typical rewards
        episode_score = min(episodes_completed / 100.0, 1.0)  # Scale based on typical episodes
        
        efficiency = (reward_score * 0.7 + episode_score * 0.3)
        return efficiency
    
    def calculate_exploration_rate(self) -> float:
        """Calculate exploration rate from training data."""
        if not hasattr(self.session_data.get('callbacks', {}).get('performance', {}), 'feature_usage'):
            return 0.0
        
        feature_usage = self.session_data.get('callbacks', {}).get('performance', {}).feature_usage
        
        # Calculate average curiosity level
        curiosity_values = []
        for episode_data in self.session_data.get('callbacks', {}).get('performance', {}).episode_data:
            curiosity = feature_usage.get('curiosity', 0.0)
            curiosity_values.append(curiosity)
        
        return sum(curiosity_values) / len(curiosity_values) if curiosity_values else 0.0
    
    def create_analysis_report(self, results_summary: Dict[str, Any]):
        """Create comprehensive analysis report."""
        print("7. Creating analysis report...")
        
        efficiency = self.calculate_efficiency_score()
        exploration = self.calculate_exploration_rate()
        
        # Generate insights
        insights = []
        
        if efficiency > 0.7:
            insights.append("✓ High training efficiency achieved")
        elif efficiency > 0.5:
            insights.append("⚠ Moderate training efficiency")
        else:
            insights.append("✗ Low training efficiency - consider optimizations")
        
        if exploration > 0.5:
            insights.append("✓ Good exploration behavior")
        elif exploration > 0.2:
            insights.append("⚠ Moderate exploration")
        else:
            insights.append("✗ Low exploration - may indicate over-exploitation")
        
        # Performance trends
        learning_curve = results_summary.get('performance_analysis', {}).get('overall_performance', {}).get('learning_curve', [])
        if learning_curve:
            if len(learning_curve) >= 2:
                trend = learning_curve[-1] - learning_curve[0]
                if trend > 0.1:
                    insights.append("✓ Strong positive learning trend")
                elif trend > 0.01:
                    insights.append("⚠ Mild positive learning trend")
                else:
                    insights.append("⚠ Flat or declining learning trend")
        
        # Create analysis report
        analysis_report = {
            'insights': insights,
            'efficiency_score': efficiency,
            'exploration_rate': exploration,
            'recommendations': self.generate_recommendations(efficiency, exploration),
            'next_steps': [
                'Fine-tune exploration parameters',
                'Implement curriculum learning for better progression',
                'Add meta-learning capabilities',
                'Set up automated hyperparameter optimization',
                'Implement multi-objective reward functions'
            ]
        }
        
        # Save analysis report
        report_file = f"advanced_results/analysis_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(report_file, 'w') as f:
            json.dump(analysis_report, f, indent=2)
        
        print(f"   ✓ Analysis report saved to {report_file}")
        print(f"   Insights: {', '.join(insights[:3])}")  # Show first 3 insights
        
        return analysis_report
    
    def generate_recommendations(self, efficiency: float, exploration: float) -> List[str]:
        """Generate recommendations based on performance metrics."""
        recommendations = []
        
        if efficiency < 0.5:
            recommendations.extend([
                "Reduce training episodes or increase reward scales",
                "Adjust learning rate for better convergence",
                "Implement early stopping with better criteria"
            ])
        
        if exploration < 0.2:
            recommendations.extend([
                "Increase exploration rate or curiosity bonus",
                "Add novelty-based rewards",
                "Implement intrinsic motivation mechanisms"
            ])
        elif exploration > 0.8:
            recommendations.extend([
                "Reduce exploration to focus on exploitation",
                "Implement curriculum to structure learning",
                "Add penalty for excessive exploration"
            ])
        
        if not recommendations:
            recommendations.extend([
                "Maintain current parameter settings",
                "Monitor performance for stability",
                "Consider adding advanced features (transfer learning)"
            ])
        
        return recommendations
    
    def run_complete_advanced_example(self) -> Dict[str, Any]:
        """Run the complete advanced training example."""
        print("=" * 80)
        print(f"NLM Advanced Training Example")
        print("=" * 80)
        print("This example demonstrates sophisticated training with:")
        print("• Complex callback systems")
        print("• Advanced feature monitoring")
        print("• Multi-factor reward functions")
        print("• Comprehensive performance analysis")
        print("• Experiment tracking and persistence")
        print("• Real-time visualization and reporting")
        print("=" * 80)
        
        # Record session start time
        self.session_data['start_time'] = time.time()
        
        # 1. Setup advanced environment
        setup = self.setup_advanced_environment()
        
        # 2. Configure advanced training
        config = self.configure_advanced_training()
        
        # 3. Create advanced callbacks
        callbacks = self.create_advanced_callbacks()
        
        # 4. Run advanced training
        results = self.run_advanced_training(setup, config, callbacks)
        
        # 5. Save advanced results
        self.save_advanced_results(results)
        
        # 6. Display final summary
        self.display_advanced_summary(results)
        
        return {
            'example': 'AdvancedTraining',
            'session_id': f"adv_train_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            'results': results,
            'timestamp': datetime.now().isoformat(),
            'success': True
        }
    
    def display_advanced_summary(self, results: Dict[str, Any]):
        """Display comprehensive advanced training summary."""
        print("\n" + "=" * 80)
        print("ADVANCED TRAINING SUMMARY")
        print("=" * 80)
        
        performance = results.get('performance_summary', {})
        metrics = self.performance_metrics
        
        print(f"Example: {self.experiment_name}")
        print(f"Session: {results.get('session_id', 'unknown')}")
        print(f"Training completed: {'Yes' if results.get('complete') else 'No'}")
        
        print(f"\nPerformance Metrics:")
        print(f"  Episodes completed: {performance.get('episodes_completed', 'N/A')}")
        print(f"  Average reward: {performance.get('overall_performance', {}).get('average_reward', 0):.3f}")
        print(f"  Best reward: {performance.get('overall_performance', {}).get('best_reward', 0):.3f}")
        print(f"  Final success rate: {performance.get('overall_performance', {}).get('success_rate', 0):.2%}")
        print(f"  Training time: {metrics.get('training_time', 0):.2f} seconds")
        
        efficiency = self.calculate_efficiency_score()
        exploration = self.calculate_exploration_rate()
        
        print(f"\nAdvanced Metrics:")
        print(f"  Efficiency score: {efficiency:.3f}")
        print(f"  Exploration rate: {exploration:.3f}")
        print(f"  Features used: {list(metrics.get('feature_usage', {}).keys())}")
        
        # Feature-specific insights
        if 'curiosity' in metrics.get('feature_usage', {}):
            curiosity_level = metrics['feature_usage']['curiosity']
            print(f"  Curvature level: {curiosity_level:.3f}")
        
        if 'curriculum' in metrics.get('feature_usage', {}):
            curriculum_stage = metrics['feature_usage']['curriculum']
            print(f"  Curriculum stage: {curriculum_stage}")
        
        print(f"\nLearning Analysis:")
        learning_curve = performance.get('overall_performance', {}).get('learning_curve', [])
        if learning_curve:
            initial = learning_curve[0] if len(learning_curve) > 0 else 0
            final = learning_curve[-1] if len(learning_curve) > 0 else 0
            progress = final - initial
            print(f"  Initial performance: {initial:.3f}")
            print(f"  Final performance: {final:.3f}")
            print(f"  Learning progress: {progress:.3f}")
            
            if progress > 0.5:
                print(f"  ✓ Excellent learning progression")
            elif progress > 0.1:
                print(f"  ✓ Good learning progression")
            elif progress > 0:
                print(f"  ⚠ Mild learning progression")
            else:
                print(f"  ✗ Declining performance - investigate issues")
        
        print(f"\nTechnical Capabilities Demonstrated:")
        print(f"  ✓ Advanced callback systems")
        print(f"  ✓ Feature monitoring and adaptation")
        print(f"  ✓ Multi-factor reward optimization")
        print(f"  ✓ Comprehensive performance analysis")
        print(f"  ✓ Experiment tracking and persistence")
        print(f"  ✓ Real-time visualization and reporting")
        print(f"  ✓ Efficiency and exploration metrics")
        
        print(f"\nFiles Generated:")
        print(f"  • Advanced results directory with detailed analysis")
        print(f"  • Performance metrics CSV for further analysis")
        print(f"  • Experiment checkpoints for reproducibility")
        print(f"  • Analysis report with recommendations")
        
        print(f"\nNext Steps:")
        print(f"  1. Analyze generated performance metrics")
        print(f"  2. Fine-tune advanced feature parameters")
        print(f"  3. Implement transfer learning from this session")
        print(f"  4. Set up automated hyperparameter optimization")
        print(f"  5. Combine multiple advanced training examples")

def main():
    """Run the advanced training example."""
    print("Starting NLM Advanced Training Example...")
    print("This example demonstrates state-of-the-art training capabilities.")
    print("Features include:")
    print("• Multi-factor reward systems")
    print("• Advanced feature monitoring")
    print("• Complex callback architectures")
    print("• Comprehensive experiment tracking")
    print("• Performance optimization and analysis")
    
    try:
        example = AdvancedTrainingExample()
        results = example.run_complete_advanced_example()
        
        print(f"\n✓ Advanced Training Example completed successfully!")
        print(f"Session ID: {results.get('session_id', 'unknown')}")
        print(f"Results saved to: advanced_results/")
        
        return results
        
    except Exception as e:
        print(f"\nError running advanced training example: {e}")
        print("Please ensure NLM C++ bindings and training module are properly installed.")
        print("Check error messages for specific installation issues.")
        return {"error": str(e)}

if __name__ == '__main__':
    try:
        result = main()
        exit(0)
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
