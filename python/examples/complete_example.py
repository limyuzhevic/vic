#!/usr/bin/env python3
"""
Complete NLM Example - Full Episode Simulation with All Features

This example demonstrates a complete simulation incorporating all NLM features:
- Neural brain with spiking dynamics
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages and plasticity
- Prediction and learning
- Episode management
- Serialization and checkpointing
- Performance profiling
- Error handling and validation
- External system integration

The simulation creates an agent that explores a world, learns from experience,
and develops over time through interaction with its environment.
"""
import nlm
import time
import json
import numpy as np
from pathlib import Path
from typing import List, Dict, Any

class CompleteNLMSimulation:
    """
    Complete NLM simulation class demonstrating all features.
    
    This class encapsulates a full agent simulation that:
    1. Initializes all NLM systems
    2. Runs learning episodes
    3. Collects and analyzes metrics
    4. Manages checkpoints and serialization
    5. Integrates with external systems
    """
    
    def __init__(self, config_file: str = None):
        """
        Initialize complete NLM simulation.
        
        Args:
            config_file: Optional path to configuration file
        """
        print("Initializing Complete NLM Simulation...")
        
        # Create configuration
        self.config = nlm.createDefaultConfig()
        
        # Load custom configuration if provided
        if config_file and Path(config_file).exists():
            try:
                self.config.loadFromFile(config_file)
                print(f"✓ Loaded configuration from {config_file}")
            except Exception as e:
                print(f"⚠ Could not load configuration: {e}")
        
        # Create core NLM systems
        self.brain = nlm.createBrain(self.config)
        self.world = nlm.createSimpleWorld()
        self.agent = nlm.createAgentBrain(self.brain)
        
        # Simulation state
        self.episode_history = []
        self.performance_metrics = []
        self.external_data = []
        
        # Configure world
        self.world.configure(width=100, height=100, visionWidth=32, visionHeight=32)
        self.world.setAgentStart(50.0, 50.0)  # Center start position
        self.world.setRandomSeed(42)
        
        # Enable all systems
        self._enable_systems()
        
        print("✓ Complete NLM simulation initialized")
        print(f"  - Brain neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Brain regions: {self.brain.getRegionCount()}")
        print(f"  - Agent input size: {self.agent.getSensoryInputSize()}")
        print(f"  - Agent output size: {agent.getMotorOutputSize()}")
    
    def _enable_systems(self):
        """Enable all NLM systems with optimal settings."""
        # Learning systems
        self.agent.enableLearning(True)
        self.agent.setLearningRate(0.01)
        self.agent.setExplorationRate(0.15)
        self.agent.setRewardShaping(0.8)
        
        # Neuromodulation systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.setCuriosityExploration(0.3)
        
        # Development system
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Performance optimization
        self.agent.enableDebugMode(False)
    
    def create_dynamic_percept(self, step: int, time_factor: float) -> nlm.SensoryPercept:
        """
        Create dynamic sensory percept based on simulation step.
        
        Args:
            step: Current simulation step
            time_factor: Time-based factor for dynamic changes
            
        Returns:
            SensoryPercept with dynamic sensory data
        """
        percept = nlm.SensoryPercept()
        
        # Vision: Dynamic patterns based on step and time
        vision_data = []
        for i in range(256):  # 16x16 vision grid
            x = (i % 16) / 15.0
            y = (i // 16) / 15.0
            
            # Create moving patterns
            pattern_x = 0.5 + 0.3 * np.sin(time_factor + x * 4.0)
            pattern_y = 0.5 + 0.3 * np.cos(time_factor + y * 3.0)
            pattern_static = 0.2 + 0.1 * np.sin(step * 0.1 + x * y * 5.0)
            
            # Combine patterns
            intensity = (pattern_x + pattern_y + pattern_static) / 3.0
            noise = 0.1 * np.sin(step * 0.05 + i)
            
            vision_data.append(max(0.0, min(1.0, intensity + noise)))
        
        percept.setVision(vision_data)
        
        # Touch: Limb interaction data
        # Create limb positions based on agent's movement history
        limb_base_x = self.world.getAgentBody().x
        limb_base_y = self.world.getAgentBody().y
        
        # Simulated limb sensors
        touch_data = [
            0.1 + 0.05 * np.sin(time_factor),  # Front touch
            0.1 + 0.05 * np.cos(time_factor),  # Back touch
            abs(limb_base_x - 50.0) / 50.0,   # Left-right distance
            abs(limb_base_y - 50.0) / 50.0,   # Up-down distance
            0.5 * (time_factor % 1.0),        # Time-based sensor
            0.3 * (step % 10) / 10.0,         # Step-based sensor
        ]
        
        percept.setTouch(touch_data)
        
        # Internal signals: Learning and neuromodulation state
        internal_signals = [
            self.agent.getCuriosityLevel(),
            self.agent.getNoveltyLevel(),
            self.agent.getPredictionError(),
            self.brain.getAverageFiringRate(),
            self.agent.getNeuromodulationLevel(),
            self.agent.getEpisodeScore(),
            time_factor % 1.0,  # Normalized time
        ]
        
        percept.setInternal(internal_signals)
        
        # Proprioception: Agent's body state
        body = self.world.getAgentBody()
        proprioception_data = [
            body.x,                    # X position
            body.y,                    # Y position
            body.orientation,          # Orientation
            body.velocityX,            # X velocity
            body.velocityY,            # Y velocity
            body.angularVelocity,      # Angular velocity
            body.energy,               # Energy level
            body.health,               # Health level
            body.age,                  # Age
            body.isMoving,             # Moving flag
            body.isTurning,            # Turning flag
        ]
        
        percept.setProprioception(proprioception_data)
        
        # Set timestamp
        percept.setTimestamp(time.time())
        
        return percept
    
    def integrate_external_data(self, step: int) -> Dict[str, Any]:
        """
        Integrate with external systems (simulated).
        
        Args:
            step: Current simulation step
            
        Returns:
            Dictionary with integrated external data
        """
        external_data = {
            'timestamp': time.time(),
            'step': step,
            'environmental_factors': {
                'temperature': 20.0 + 5.0 * np.sin(time.time() / 100.0),
                'humidity': 0.6 + 0.2 * np.random.random(),
                'light_intensity': max(0.0, 0.5 * np.sin(time.time() / 200.0)),
            },
            'resource_availability': {
                'energy_resources': 0.7 + 0.3 * np.random.random(),
                'food_resources': 0.5 + 0.5 * ((step % 20) / 20.0),
                'water_resources': 0.6 + 0.4 * np.cos(time.time() / 150.0),
            },
            'social_factors': {
                'population_density': min(1.0, step / 100.0),
                'competition_level': 0.3 + 0.7 * (step % 30) / 30.0,
            },
            'learning_opportunities': {
                'novelty_score': self.agent.getNoveltyLevel(),
                'reward_prediction_error': self.agent.getPredictionError(),
                'exploration_potential': self.agent.getCuriosityLevel(),
            }
        }
        
        self.external_data.append(external_data)
        return external_data
    
    def run_episode(self, episode_id: int, max_steps: int = 100) -> Dict[str, Any]:
        """
        Run a complete learning episode.
        
        Args:
            episode_id: Unique identifier for the episode
            max_steps: Maximum number of steps per episode
            
        Returns:
            Dictionary with episode results and metrics
        """
        print(f"\n{'='*60}")
        print(f"RUNNING EPISODE {episode_id}")
        print(f"{'='*60}")
        
        # Initialize episode
        episode_start_time = time.time()
        agent.initialize(self.world)
        self.agent.startEpisode(episode_id, f"simulation_{episode_id}")
        
        # Initialize episode tracking
        episode_data = {
            'episode_id': episode_id,
            'steps': [],
            'total_reward': 0.0,
            'max_reward': -float('inf'),
            'min_reward': float('inf'),
            'actions_taken': [],
            'novelty_scores': [],
            'curiosity_levels': [],
            'prediction_errors': [],
            'environmental_data': [],
            'start_time': episode_start_time,
        }
        
        # Run episode steps
        for step in range(max_steps):
            step_start_time = time.time()
            
            # Get current time factor
            time_factor = step * 0.1
            
            # Create sensory percept
            percept = self.create_dynamic_percept(step, time_factor)
            
            # Integrate with external systems
            external_data = self.integrate_external_data(step)
            episode_data['environmental_data'].append(external_data)
            
            # Process sensory input with novelty computation
            if step % 3 == 0:  # Compute novelty periodically
                self.agent.processSensoryInput(percept, True)
            else:
                self.agent.processSensoryInput(percept, False)
            
            # Get action from brain
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            action_result = self.world.applyMotorCommand(action, time.time())
            
            # Record episode data
            episode_step_data = {
                'step': step,
                'action': action,
                'action_result': {
                    'reward': action_result.reward,
                    'success': action_result.success,
                    'message': action_result.message,
                },
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'episode_score': self.agent.getEpisodeScore(),
                'step_time': time.time() - step_start_time,
                'brain_neurons_active': self.brain.getActiveNeuronCount(),
                'brain_neurons_firing': self.brain.getFiringNeuronCount(),
            }
            
            # Apply reward and modulation
            reward = action_result.reward
            predicted_reward = max(0.0, reward * 0.5)  # Simplified prediction
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update episode data
            episode_data['steps'].append(episode_step_data)
            episode_data['actions_taken'].append(action)
            episode_data['novelty_scores'].append(self.agent.getNoveltyLevel())
            episode_data['curiosity_levels'].append(self.agent.getCuriosityLevel())
            episode_data['prediction_errors'].append(self.agent.getPredictionError())
            episode_data['total_reward'] += reward
            episode_data['max_reward'] = max(episode_data['max_reward'], reward)
            episode_data['min_reward'] = min(episode_data['min_reward'], reward)
            
            # Update development periodically
            if step % 5 == 0:
                self.agent.updateDevelopment(1.0)
            
            # End episode if conditions met
            if self.agent.isEpisodeActive():
                # End if maximum score reached
                if self.agent.getEpisodeScore() >= 100.0:
                    self.agent.endEpisode(reward)
                    episode_data['end_reason'] = 'score_threshold'
                    break
                
                # End if episode gets too long
                if step >= max_steps - 1:
                    self.agent.endEpisode(reward)
                    episode_data['end_reason'] = 'max_steps'
                    break
                
                # End based on world conditions (low energy)
                body = self.world.getAgentBody()
                if body.energy < 0.1:
                    self.agent.endEpisode(reward)
                    episode_data['end_reason'] = 'low_energy'
                    break
        
        # Finalize episode
        episode_end_time = time.time()
        episode_duration = episode_end_time - episode_start_time
        
        self.agent.collectMetrics()
        
        # Calculate episode statistics
        episode_data.update({
            'end_time': episode_end_time,
            'duration': episode_duration,
            'steps_taken': len(episode_data['steps']),
            'average_step_time': np.mean([s['step_time'] for s in episode_data['steps']]),
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'final_prediction_error': self.agent.getPredictionError(),
            'final_neuromodulation': self.agent.getNeuromodulationLevel(),
            'final_developmental_stage': self.brain.getDevelopmentalStage(),
            'final_brain_state': {
                'neurons_active': self.brain.getActiveNeuronCount(),
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'excitatory_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            }
        })
        
        # Store episode in history
        self.episode_history.append(episode_data)
        
        # Print episode summary
        self._print_episode_summary(episode_data)
        
        return episode_data
    
    def _print_episode_summary(self, episode_data: Dict[str, Any]):
        """Print formatted episode summary."""
        print(f"\n--- Episode {episode_data['episode_id']} Summary ---")
        print(f"  Duration: {episode_data['duration']:.2f}s")
        print(f"  Steps taken: {episode_data['steps_taken']}")
        print(f"  Total reward: {episode_data['total_reward']:.2f}")
        print(f"  Max reward: {episode_data['max_reward']:.2f}")
        print(f"  Min reward: {episode_data['min_reward']:.2f}")
        print(f"  Average reward per step: {episode_data['total_reward']/max(1, episode_data['steps_taken']):.3f}")
        print(f"  End reason: {episode_data.get('end_reason', 'unknown')}")
        print(f"  Final curiosity: {episode_data['final_curiosity']:.3f}")
        print(f"  Final novelty: {episode_data['final_novelty']:.3f}")
        print(f"  Final prediction error: {episode_data['final_prediction_error']:.3f}")
        print(f"  Final developmental stage: {episode_data['final_developmental_stage']}")
    
    def run_learning_session(self, episode_count: int = 10, max_steps_per_episode: int = 100):
        """
        Run a complete learning session with multiple episodes.
        
        Args:
            episode_count: Number of episodes to run
            max_steps_per_episode: Maximum steps per episode
            
        Returns:
            Dictionary with session statistics
        """
        print(f"\n{'='*60}")
        print(f"STARTING LEARNING SESSION ({episode_count} episodes)")
        print(f"{'='*60}")
        
        session_start_time = time.time()
        
        for episode_id in range(episode_count):
            # Run single episode
            episode_data = self.run_episode(episode_id, max_steps_per_episode)
            
            # Store performance metrics
            metrics = self.agent.profilePerformance()
            self.performance_metrics.append({
                'episode': episode_id,
                'processing_efficiency': metrics.processingEfficiency,
                'steps_per_second': metrics.stepsPerSecond,
                'memory_usage_mb': metrics.memoryUsageBytes / (1024 * 1024),
                'average_step_time_ms': metrics.averageStepTime * 1000,
            })
        
        # Calculate session statistics
        session_end_time = time.time()
        session_duration = session_end_time - session_start_time
        
        total_steps = sum(ep['steps_taken'] for ep in self.episode_history)
        total_rewards = sum(ep['total_reward'] for ep in self.episode_history)
        
        session_stats = {
            'episode_count': episode_count,
            'total_steps': total_steps,
            'total_duration': session_duration,
            'total_rewards': total_rewards,
            'average_reward_per_episode': total_rewards / episode_count,
            'average_steps_per_episode': total_steps / episode_count,
            'episodes': self.episode_history,
            'performance_metrics': self.performance_metrics,
            'external_data_samples': len(self.external_data),
        }
        
        print(f"\n{'='*60}")
        print(f"LEARNING SESSION COMPLETE")
        print(f"{'='*60}")
        print(f"Total episodes: {episode_count}")
        print(f"Total steps: {total_steps}")
        print(f"Total duration: {session_duration:.2f}s")
        print(f"Total rewards: {total_rewards:.2f}")
        print(f"Average reward/episode: {session_stats['average_reward_per_episode']:.2f}")
        print(f"Average steps/episode: {session_stats['average_steps_per_episode']:.1f}")
        
        return session_stats
    
    def save_checkpoint(self, checkpoint_name: str, filepath: str = "checkpoint.json"):
        """
        Save current simulation state as checkpoint.
        
        Args:
            checkpoint_name: Name for the checkpoint
            filepath: File path to save checkpoint
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Set checkpoint threshold
            self.agent.setCheckpoint(0.5)
            
            # Save state
            if self.agent.saveState(filepath):
                print(f"✓ Checkpoint '{checkpoint_name}' saved to {filepath}")
                
                # Create metadata
                metadata = {
                    'checkpoint_name': checkpoint_name,
                    'timestamp': time.time(),
                    'episode_count': len(self.episode_history),
                    'total_steps': sum(ep['steps_taken'] for ep in self.episode_history),
                    'final_reward': self.episode_history[-1]['total_reward'] if self.episode_history else 0,
                    'final_curiosity': self.agent.getCuriosityLevel(),
                    'final_novelty': self.agent.getNoveltyLevel(),
                    'developmental_stage': self.brain.getDevelopmentalStage(),
                }
                
                # Save metadata
                metadata_file = filepath.replace('.json', '_metadata.json')
                with open(metadata_file, 'w') as f:
                    json.dump(metadata, f, indent=2)
                
                print(f"✓ Metadata saved to {metadata_file}")
                return True
            else:
                print("✗ Failed to save checkpoint")
                return False
                
        except Exception as e:
            print(f"✗ Checkpoint save failed: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """
        Load simulation state from checkpoint.
        
        Args:
            filepath: Path to checkpoint file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if Path(filepath).exists():
                if self.agent.loadState(filepath):
                    print(f"✓ Checkpoint loaded from {filepath}")
                    return True
                else:
                    print("✗ Failed to load checkpoint")
                    return False
            else:
                print(f"✗ Checkpoint file not found: {filepath}")
                return False
                
        except Exception as e:
            print(f"✗ Checkpoint load failed: {e}")
            return False
    
    def export_episode_data(self, filepath: str, format_type: str = "json"):
        """
        Export episode data for analysis.
        
        Args:
            filepath: Output file path
            format_type: Export format (json, csv, numpy)
        """
        try:
            if format_type.lower() == "json":
                # Convert numpy arrays to lists for JSON serialization
                export_data = {
                    'episodes': [],
                    'metadata': {
                        'total_episodes': len(self.episode_history),
                        'total_steps': sum(ep['steps_taken'] for ep in self.episode_history),
                        'exported_at': time.time(),
                    }
                }
                
                for episode in self.episode_history:
                    episode_copy = episode.copy()
                    
                    # Convert lists to lists (no numpy arrays)
                    for step in episode_copy['steps']:
                        step_copy = step.copy()
                        for key, value in step_copy.items():
                            if isinstance(value, np.ndarray):
                                step_copy[key] = value.tolist()
                        
                        episode_copy['steps'] = [step_copy]
                    
                    export_data['episodes'].append(episode_copy)
                
                with open(filepath, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                
                print(f"✓ Episode data exported to {filepath} (JSON)")
                
            elif format_type.lower() == "csv":
                import pandas as pd
                
                # Create DataFrame from episode data
                all_steps = []
                for episode in self.episode_history:
                    for step in episode['steps']:
                        step_data = {
                            'episode_id': episode['episode_id'],
                            'step': step['step'],
                            'action': step['action'],
                            'reward': step['action_result']['reward'],
                            'neuromodulation_level': step['neuromodulation_level'],
                            'curiosity_level': step['curiosity_level'],
                            'novelty_level': step['novelty_level'],
                            'prediction_error': step['prediction_error'],
                            'episode_score': step['episode_score'],
                            'step_time': step['step_time'],
                            'neurons_active': step['brain_neurons_active'],
                            'neurons_firing': step['brain_neurons_firing'],
                        }
                        all_steps.append(step_data)
                
                df = pd.DataFrame(all_steps)
                df.to_csv(filepath, index=False)
                print(f"✓ Episode data exported to {filepath} (CSV)")
                
            else:
                print(f"✗ Unsupported export format: {format_type}")
                
        except Exception as e:
            print(f"✗ Export failed: {e}")
    
    def get_session_summary(self) -> Dict[str, Any]:
        """
        Get comprehensive session summary and statistics.
        
        Returns:
            Dictionary with session summary
        """
        if not self.episode_history:
            return {'error': 'No episodes run yet'}
        
        # Calculate statistics
        total_steps = sum(ep['steps_taken'] for ep in self.episode_history)
        total_rewards = sum(ep['total_reward'] for ep in self.episode_history)
        
        # Calculate learning trends
        episode_rewards = [ep['total_reward'] for ep in self.episode_history]
        curiosity_trend = [ep['final_curiosity'] for ep in self.episode_history]
        novelty_trend = [ep['final_novelty'] for ep in self.episode_history]
        
        # Get performance metrics
        latest_metrics = self.agent.profilePerformance() if self.performance_metrics else None
        
        return {
            'session_info': {
                'total_episodes': len(self.episode_history),
                'total_steps': total_steps,
                'total_rewards': total_rewards,
                'average_reward_per_episode': total_rewards / len(self.episode_history),
                'average_steps_per_episode': total_steps / len(self.episode_history),
            },
            'learning_trends': {
                'reward_trend': episode_rewards,
                'curiosity_trend': curiosity_trend,
                'novelty_trend': novelty_trend,
                'reward_improvement': self._calculate_improvement(episode_rewards),
            },
            'final_state': {
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'developmental_stage': self.brain.getDevelopmentalStage(),
                'brain_efficiency': latest_metrics.processingEfficiency if latest_metrics else 0,
            },
            'environmental_integration': {
                'external_data_samples': len(self.external_data),
                'integration_successful': len(self.external_data) > 0,
            }
        }
    
    def _calculate_improvement(self, values: List[float]) -> Dict[str, float]:
        """Calculate improvement trends from a list of values."""
        if len(values) < 2:
            return {'improvement': 0.0, 'trend': 'insufficient_data'}
        
        first_half = values[:len(values)//2]
        second_half = values[len(values)//2:]
        
        avg_first = sum(first_half) / len(first_half)
        avg_second = sum(second_half) / len(second_half)
        
        improvement = avg_second - avg_first
        trend = 'improving' if improvement > 0.1 else 'declining' if improvement < -0.1 else 'stable'
        
        return {
            'improvement': improvement,
            'trend': trend,
            'first_half_avg': avg_first,
            'second_half_avg': avg_second,
        }

def main():
    """Main function demonstrating complete NLM simulation."""
    print("Complete NLM Simulation - Full Feature Demonstration")
    print("=" * 60)
    
    # Initialize simulation
    sim = CompleteNLMSimulation()
    
    # Run learning session
    print("\nRunning comprehensive learning session...")
    session_stats = sim.run_learning_session(
        episode_count=5,
        max_steps_per_episode=50
    )
    
    # Save checkpoint
    print("\nSaving simulation checkpoint...")
    sim.save_checkpoint("complete_simulation_checkpoint")
    
    # Export episode data for analysis
    print("\nExporting episode data for analysis...")
    sim.export_episode_data("complete_simulation_data.json")
    
    # Display session summary
    print("\nGenerating session summary...")
    summary = sim.get_session_summary()
    
    print("\n" + "=" * 60)
    print("SIMULATION COMPLETE - SESSION SUMMARY")
    print("=" * 60)
    
    print(f"\nSession Performance:")
    print(f"  - Episodes completed: {summary['session_info']['total_episodes']}")
    print(f"  - Total steps: {summary['session_info']['total_steps']}")
    print(f"  - Total rewards: {summary['session_info']['total_rewards']:.2f}")
    print(f"  - Average reward/episode: {summary['session_info']['average_reward_per_episode']:.2f}")
    
    print(f"\nLearning Trends:")
    print(f"  - Reward trend: {summary['learning_trends']['reward_improvement']['trend']}")
    print(f"  - Reward improvement: {summary['learning_trends']['reward_improvement']['improvement']:.3f}")
    print(f"  - Final curiosity: {summary['final_state']['curiosity_level']:.3f}")
    print(f"  - Final novelty: {summary['final_state']['novelty_level']:.3f}")
    print(f"  - Final prediction error: {summary['final_state']['prediction_error']:.3f}")
    print(f"  - Final developmental stage: {summary['final_state']['developmental_stage']}")
    
    print(f"\nSystem Integration:")
    print(f"  - External data samples: {summary['environmental_integration']['external_data_samples']}")
    print(f"  - Integration successful: {summary['environmental_integration']['integration_successful']}")
    
    print(f"\nBrain Performance:")
    print(f"  - Processing efficiency: {summary['final_state']['brain_efficiency']:.3f}")
    
    # Save summary to file
    with open("simulation_summary.json", "w") as f:
        json.dump(summary, f, indent=2, default=str)
    
    print(f"\n✓ Session summary saved to simulation_summary.json")
    print("✓ Complete NLM simulation finished successfully!")

if __name__ == "__main__":
    main()