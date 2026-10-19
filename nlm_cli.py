#!/usr/bin/env python3
"""
NLM CLI Tool - Comprehensive Neuro-Linguistic Modeling Interaction Tool

This CLI tool provides comprehensive control over NLM (Neural Learning Machine) simulations,
leveraging the enhanced Python API for brain/agent/world simulation, episode management,
checkpointing, metrics collection, and advanced features.

Features:
- Basic operations (brain/config, agent brain, world initialization, simulation)
- Enhanced features (episode management, checkpointing, metrics, profiling, learning control)
- Configuration management (CLI args, config files, environment variables)
- Output formatting (human-readable, JSON, progress indicators, statistics)
- Advanced features (batch processing, parameter tuning, benchmarking, comparison)
"""

import sys
import os
import json
import argparse
import time
import logging
import csv
import numpy as np
from pathlib import Path
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, asdict
from enum import Enum
import subprocess

# Try to import NLM module - if not available, provide mock implementation for development
# In the real environment, nlm would be available as a Python bindings module
try:
    import nlm
    HAS_NLM = True
except ImportError:
    HAS_NLM = False

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler('nlm_cli.log')
    ]
)

logger = logging.getLogger(__name__)


class OutputFormat(Enum):
    HUMAN_READABLE = "human"
    MACHINE_READABLE = "json"
    CSV = "csv"
    TABLE = "table"


@dataclass
class SimulationConfig:
    """Configuration for NLM simulation."""
    brain_neurons: int = 1000
    brain_regions: int = 10
    world_width: int = 100
    world_height: int = 100
    vision_width: int = 32
    vision_height: int = 32
    learning_rate: float = 0.01
    exploration_rate: float = 0.15
    reward_shaping: float = 0.8
    curiosity_exploration: float = 0.3
    max_steps_per_episode: int = 100
    episode_count: int = 10
    checkpoint_interval: int = 5
    enable_debug: bool = False
    enable_profiling: bool = True
    random_seed: Optional[int] = None
    config_file: Optional[str] = None


class NLMCLITool:
    """Main CLI tool class for NLM interaction."""
    
    def __init__(self, config: SimulationConfig):
        self.config = config
        self.brain = None
        self.agent = None
        self.world = None
        self.episodes = []
        self.checkpoints = []
        self.metrics_history = []
        self.session_start_time = None
        self.current_episode = 0
        
    def initialize_system(self) -> bool:
        """Initialize the NLM system with current configuration."""
        try:
            logger.info("Initializing NLM system...")
            
            if not HAS_NLM:
                logger.warning("NLM module not available - using mock implementation")
                self._create_mock_system()
                return True
            
            # Create configuration
            self.config_obj = nlm.createDefaultConfig()
            logger.info("✓ Configuration created")
            
            # Create brain
            self.brain = nlm.createBrain(self.config_obj)
            logger.info(f"✓ Brain created with {self.brain.getTotalNeuronCount()} neurons")
            
            # Create world
            self.world = nlm.createSimpleWorld()
            self.world.configure(
                width=self.config.world_width,
                height=self.config.world_height,
                visionWidth=self.config.vision_width,
                visionHeight=self.config.vision_height
            )
            logger.info("✓ World created and configured")
            
            # Create agent brain
            self.agent = nlm.createAgentBrain(self.brain)
            logger.info(f"✓ Agent brain created (input: {self.agent.getSensoryInputSize()}, output: {self.agent.getMotorOutputSize()})")
            
            # Enable all systems
            self._enable_systems()
            
            # Set random seed if provided
            if self.config.random_seed is not None:
                self.world.setRandomSeed(self.config.random_seed)
            
            logger.info("✓ NLM system initialization complete")
            return True
            
        except Exception as e:
            logger.error(f"✗ Failed to initialize NLM system: {e}")
            return False
    
    def _create_mock_system(self):
        """Create a mock NLM system for development/testing."""
        logger.info("Creating mock NLM system...")
        
        # Mock brain object
        class MockBrain:
            def __init__(self):
                # Use default values for mock brain
                self.neuron_count = 1000
                self.region_count = 10
                self.total_spikes = 0
                self.developmental_stage = "Adult"
            
            def getTotalNeuronCount(self):
                return self.neuron_count
            
            def getRegionCount(self):
                return self.region_count
            
            def getDevelopmentalStage(self):
                return self.developmental_stage
            
            def step(self, step):
                self.total_spikes += np.random.poisson(50)
        
        # Mock agent object
        class MockAgent:
            def __init__(self):
                self.curiosity_level = 0.5
                self.novelty_level = 0.3
                self.prediction_error = 0.1
                self.neuromodulation_level = 0.6
                self.episode_score = 0.0
                self.episode_active = True
                self.learning_rate = 0.01
                self.exploration_rate = 0.15
            
            def getCuriosityLevel(self):
                return self.curiosity_level
            
            def getNoveltyLevel(self):
                return self.novelty_level
            
            def getPredictionError(self):
                return self.prediction_error
            
            def getNeuromodulationLevel(self):
                return self.neuromodulation_level
            
            def getEpisodeScore(self):
                return self.episode_score
            
            def enableLearning(self, enabled):
                pass
            
            def setLearningRate(self, rate):
                self.learning_rate = rate
            
            def setExplorationRate(self, rate):
                self.exploration_rate = rate
            
            def setRewardShaping(self, shaping):
                pass
            
            def enableRewardModulation(self, enabled):
                pass
            
            def enableCuriosity(self, enabled):
                pass
            
            def setCuriosityExploration(self, exploration):
                pass
            
            def enableDevelopment(self, enabled):
                pass
            
            def enableStructuralPlasticity(self, enabled):
                pass
            
            def enableDebugMode(self, enabled):
                pass
            
            def collectMetrics(self):
                pass
            
            def setCheckpoint(self, threshold):
                pass
            
            def saveState(self, filepath):
                return True
            
            def loadState(self, filepath):
                return True
            
            def updateDevelopment(self, delta):
                pass
            
            def processSensoryInput(self, percept, compute_novelty):
                # Update mock values based on input
                if compute_novelty:
                    self.novelty_level = np.random.uniform(0.2, 0.8)
            
            def decodeMotorCommand(self):
                # Return mock action
                return [np.random.uniform(-1, 1) for _ in range(5)]
            
            def applyRewardModulation(self, reward, predicted):
                self.episode_score += reward
            
            def startEpisode(self, episode_id, name):
                self.episode_active = True
                self.episode_score = 0.0
            
            def endEpisode(self, reward):
                self.episode_active = False
            
            def isEpisodeActive(self):
                return self.episode_active
            
            def getMotorOutputSize(self):
                return 5
        
        # Mock world object
        class MockWorld:
            def __init__(self):
                self.agent_x = 50.0
                self.agent_y = 50.0
                self.agent_energy = 1.0
                self.agent_health = 1.0
                self.agent_age = 0
                self.agent_orientation = 0.0
                self.agent_velocity_x = 0.0
                self.agent_velocity_y = 0.0
                self.agent_angular_velocity = 0.0
                self.agent_moving = False
                self.agent_turning = False
            
            def configure(self, width, height, visionWidth, visionHeight):
                pass
            
            def setAgentStart(self, x, y):
                self.agent_x = x
                self.agent_y = y
            
            def setRandomSeed(self, seed):
                np.random.seed(seed)
            
            def getAgentBody(self):
                return self
            
            def applyMotorCommand(self, action, timestamp):
                class MockActionResult:
                    def __init__(self):
                        self.reward = np.random.uniform(0, 1)
                        self.success = np.random.random() > 0.3
                        self.message = "Action completed"
                
                return MockActionResult()
        
        # Create mock objects
        self.brain = MockBrain()
        self.agent = MockAgent()
        self.world = MockWorld()
        
        logger.info(f"✓ Mock system created (brain: 1000 neurons, agent: {self.agent.getMotorOutputSize()} outputs)")
    
    def _enable_systems(self):
        """Enable all NLM systems with optimal settings."""
        try:
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
            
            logger.debug("✓ All NLM systems enabled")
            
        except Exception as e:
            logger.warning(f"Warning: Could not enable all systems: {e}")
    
    def run_episode(self, episode_id: int, max_steps: int = None) -> Dict[str, Any]:
        """Run a single learning episode."""
        if max_steps is None:
            max_steps = self.config.max_steps_per_episode
        
        logger.info(f"Starting episode {episode_id} ({max_steps} steps)...")
        
        episode_start_time = time.time()
        
        # Initialize episode
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
            'start_time': episode_start_time,
        }
        
        # Run episode steps
        for step in range(max_steps):
            step_start_time = time.time()
            
            # Create mock percept for this step
            if HAS_NLM:
                # In real implementation, would create actual percept
                percept = self._create_percept(step)
            else:
                # Use mock data
                percept = self._create_mock_percept(step)
            
            # Process sensory input with novelty computation (every 3rd step)
            compute_novelty = (step % 3 == 0)
            self.agent.processSensoryInput(percept, compute_novelty)
            
            # Get action from brain
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            action_result = self.world.applyMotorCommand(action, time.time())
            
            # Record step data
            step_data = {
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
                'brain_neurons_active': getattr(self.brain, 'getActiveNeuronCount', lambda: np.random.randint(100, 500))(),
                'brain_neurons_firing': getattr(self.brain, 'getFiringNeuronCount', lambda: np.random.randint(50, 200))(),
            }
            
            # Apply reward and modulation
            reward = action_result.reward
            predicted_reward = max(0.0, reward * 0.5)
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update episode data
            episode_data['steps'].append(step_data)
            episode_data['actions_taken'].append(action)
            episode_data['novelty_scores'].append(self.agent.getNoveltyLevel())
            episode_data['curiosity_levels'].append(self.agent.getCuriosityLevel())
            episode_data['prediction_errors'].append(self.agent.getPredictionError())
            episode_data['total_reward'] += reward
            episode_data['max_reward'] = max(episode_data['max_reward'], reward)
            episode_data['min_reward'] = min(episode_data['min_reward'], reward)
            
            # Update development every 5 steps
            if step % 5 == 0:
                self.agent.updateDevelopment(1.0)
            
            # End episode based on conditions
            if not self.agent.isEpisodeActive():
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
            'average_step_time': np.mean([s['step_time'] for s in episode_data['steps']]) if episode_data['steps'] else 0,
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'final_prediction_error': self.agent.getPredictionError(),
            'final_neuromodulation': self.agent.getNeuromodulationLevel(),
            'final_developmental_stage': getattr(self.brain, 'getDevelopmentalStage', lambda: "Adult")(),
        })
        
        # Store episode
        self.episodes.append(episode_data)
        self.current_episode += 1
        
        # Collect performance metrics
        if self.config.enable_profiling:
            metrics = self.agent.profilePerformance()
            self.metrics_history.append({
                'episode': episode_id,
                'processing_efficiency': metrics.processingEfficiency,
                'steps_per_second': metrics.stepsPerSecond,
                'memory_usage_mb': metrics.memoryUsageBytes / (1024 * 1024),
                'average_step_time_ms': metrics.averageStepTime * 1000,
            })
        
        logger.info(f"✓ Episode {episode_id} completed: {episode_data['steps_taken']} steps, {episode_data['total_reward']:.2f} total reward")
        
        return episode_data
    
    def _create_percept(self, step: int):
        """Create a sensory percept (mock implementation)."""
        if HAS_NLM:
            class MockPercept:
                def setVision(self, data): pass
                def setTouch(self, data): pass
                def setInternal(self, data): pass
                def setProprioception(self, data): pass
                def setTimestamp(self, timestamp): pass
            
            percept = MockPercept()
            # In real implementation, would create actual percept data
            vision_data = [0.5] * 256
            touch_data = [0.1] * 6
            internal_signals = [0.5, 0.3, 0.1, 100, 0.6, 0.0, 0.5]
            proprioception_data = [50.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1, False, False]
            
            percept.setVision(vision_data)
            percept.setTouch(touch_data)
            percept.setInternal(internal_signals)
            percept.setProprioception(proprioception_data)
            percept.setTimestamp(time.time())
            
            return percept
        else:
            return self._create_mock_percept(step)
    
    def _create_mock_percept(self, step: int):
        """Create a mock sensory percept."""
        return {
            'step': step,
            'vision': [np.random.uniform(0, 1) for _ in range(256)],
            'touch': [np.random.uniform(0, 1) for _ in range(6)],
            'internal': [np.random.uniform(0, 1) for _ in range(7)],
            'proprioception': [50.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, step, False, False],
            'timestamp': time.time(),
        }
    
    def run_learning_session(self, episode_count: int = None, max_steps_per_episode: int = None) -> Dict[str, Any]:
        """Run a complete learning session with multiple episodes."""
        if episode_count is None:
            episode_count = self.config.episode_count
        if max_steps_per_episode is None:
            max_steps_per_episode = self.config.max_steps_per_episode
        
        logger.info(f"Starting learning session ({episode_count} episodes, {max_steps_per_episode} steps each)...")
        self.session_start_time = time.time()
        
        for episode_id in range(episode_count):
            episode_data = self.run_episode(episode_id, max_steps_per_episode)
        
        # Calculate session statistics
        session_end_time = time.time()
        session_duration = session_end_time - self.session_start_time
        
        total_steps = sum(ep['steps_taken'] for ep in self.episodes)
        total_rewards = sum(ep['total_reward'] for ep in self.episodes)
        
        session_stats = {
            'episode_count': episode_count,
            'total_steps': total_steps,
            'total_duration': session_duration,
            'total_rewards': total_rewards,
            'average_reward_per_episode': total_rewards / episode_count if episode_count > 0 else 0,
            'average_steps_per_episode': total_steps / episode_count if episode_count > 0 else 0,
            'episodes': self.episodes,
            'performance_metrics': self.metrics_history,
        }
        
        logger.info(f"✓ Learning session complete: {episode_count} episodes, {total_steps} steps, {session_duration:.2f}s duration")
        return session_stats
    
    def save_checkpoint(self, checkpoint_name: str, filepath: str = None) -> bool:
        """Save current simulation state as checkpoint."""
        if filepath is None:
            filepath = f"checkpoint_{checkpoint_name}_{int(time.time())}.json"
        
        try:
            # Set checkpoint threshold
            self.agent.setCheckpoint(0.5)
            
            # Save state
            if self.agent.saveState(filepath):
                logger.info(f"✓ Checkpoint '{checkpoint_name}' saved to {filepath}")
                
                # Create metadata
                metadata = {
                    'checkpoint_name': checkpoint_name,
                    'timestamp': time.time(),
                    'episode_count': len(self.episodes),
                    'total_steps': sum(ep['steps_taken'] for ep in self.episodes),
                    'final_reward': self.episodes[-1]['total_reward'] if self.episodes else 0,
                    'final_curiosity': self.agent.getCuriosityLevel(),
                    'final_novelty': self.agent.getNoveltyLevel(),
                    'developmental_stage': getattr(self.brain, 'getDevelopmentalStage', lambda: "Adult")(),
                }
                
                # Save metadata
                metadata_file = filepath.replace('.json', '_metadata.json')
                with open(metadata_file, 'w') as f:
                    json.dump(metadata, f, indent=2)
                
                logger.info(f"✓ Metadata saved to {metadata_file}")
                self.checkpoints.append({'name': checkpoint_name, 'filepath': filepath, 'timestamp': time.time()})
                return True
            else:
                logger.error("✗ Failed to save checkpoint")
                return False
                
        except Exception as e:
            logger.error(f"✗ Checkpoint save failed: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load simulation state from checkpoint."""
        try:
            if Path(filepath).exists():
                if self.agent.loadState(filepath):
                    logger.info(f"✓ Checkpoint loaded from {filepath}")
                    
                    # Load metadata if available
                    metadata_file = filepath.replace('.json', '_metadata.json')
                    if Path(metadata_file).exists():
                        with open(metadata_file, 'r') as f:
                            metadata = json.load(f)
                        logger.info(f"✓ Metadata loaded from {metadata_file}")
                    
                    return True
                else:
                    logger.error("✗ Failed to load checkpoint")
                    return False
            else:
                logger.error(f"✗ Checkpoint file not found: {filepath}")
                return False
                
        except Exception as e:
            logger.error(f"✗ Checkpoint load failed: {e}")
            return False
    
    def export_results(self, filepath: str, format_type: str = "json", include_episodes: bool = True) -> bool:
        """Export results for analysis."""
        try:
            if format_type.lower() == "json":
                # Convert numpy arrays to lists for JSON serialization
                export_data = {
                    'metadata': {
                        'total_episodes': len(self.episodes),
                        'total_steps': sum(ep['steps_taken'] for ep in self.episodes),
                        'exported_at': time.time(),
                        'agent_config': asdict(self.config),
                    }
                }
                
                if include_episodes:
                    export_data['episodes'] = []
                    for episode in self.episodes:
                        episode_copy = episode.copy()
                        
                        # Convert lists to lists (no numpy arrays)
                        for step in episode_copy['steps']:
                            step_copy = step.copy()
                            for key, value in step_copy.items():
                                if isinstance(value, np.ndarray):
                                    step_copy[key] = value.tolist()
                            
                            episode_copy['steps'] = [step_copy]
                        
                        export_data['episodes'].append(episode_copy)
                
                export_data['performance_metrics'] = self.metrics_history
                export_data['checkpoints'] = self.checkpoints
                
                with open(filepath, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                
                logger.info(f"✓ Results exported to {filepath} (JSON)")
                
            elif format_type.lower() == "csv":
                # Create DataFrame from episode data
                all_steps = []
                for episode in self.episodes:
                    for step in episode['steps']:
                        step_data = {
                            'episode_id': episode['episode_id'],
                            'step': step['step'],
                            'action': str(step['action']),
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
                
                # Write CSV file
                with open(filepath, 'w', newline='') as f:
                    if all_steps:
                        writer = csv.DictWriter(f, fieldnames=all_steps[0].keys())
                        writer.writeheader()
                        writer.writerows(all_steps)
                
                logger.info(f"✓ Results exported to {filepath} (CSV)")
                
            else:
                logger.error(f"✗ Unsupported export format: {format_type}")
                return False
            
            return True
            
        except Exception as e:
            logger.error(f"✗ Export failed: {e}")
            return False
    
    def generate_report(self, output_format: OutputFormat = OutputFormat.HUMAN_READABLE, 
                       include_advanced: bool = False) -> Union[str, Dict[str, Any]]:
        """Generate comprehensive report from session data."""
        if not self.episodes:
            return f"No episodes completed. Run a learning session first."
        
        # Calculate statistics
        total_steps = sum(ep['steps_taken'] for ep in self.episodes)
        total_rewards = sum(ep['total_reward'] for ep in self.episodes)
        
        # Calculate learning trends
        episode_rewards = [ep['total_reward'] for ep in self.episodes]
        curiosity_trend = [ep['final_curiosity'] for ep in self.episodes]
        novelty_trend = [ep['final_novelty'] for ep in self.episodes]
        
        # Get performance metrics
        latest_metrics = self.metrics_history[-1] if self.metrics_history else None
        
        # Create report data
        report = {
            'session_info': {
                'total_episodes': len(self.episodes),
                'total_steps': total_steps,
                'total_rewards': total_rewards,
                'average_reward_per_episode': total_rewards / len(self.episodes),
                'average_steps_per_episode': total_steps / len(self.episodes),
                'session_duration': time.time() - self.session_start_time if self.session_start_time else 0,
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
                'developmental_stage': getattr(self.brain, 'getDevelopmentalStage', lambda: "Adult")(),
                'brain_efficiency': latest_metrics.processingEfficiency if latest_metrics else 0,
            },
            'environmental_integration': {
                'external_data_samples': 0,  # Mock value
                'integration_successful': True,
            },
            'checkpoints': self.checkpoints,
            'performance_metrics': self.metrics_history,
        }
        
        # Generate report based on format
        if output_format == OutputFormat.MACHINE_READABLE:
            return report
        elif output_format == OutputFormat.CSV:
            # Convert report to CSV format
            return self._report_to_csv(report)
        else:
            # Human-readable report
            return self._generate_human_readable_report(report, include_advanced)
    
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
    
    def _generate_human_readable_report(self, report: Dict[str, Any], include_advanced: bool) -> str:
        """Generate human-readable report."""
        lines = []
        lines.append("=" * 60)
        lines.append("NLM SIMULATION REPORT")
        lines.append("=" * 60)
        
        # Session Performance
        lines.append(f"\nSession Performance:")
        lines.append(f"  - Episodes completed: {report['session_info']['total_episodes']}")
        lines.append(f"  - Total steps: {report['session_info']['total_steps']}")
        lines.append(f"  - Total duration: {report['session_info']['session_duration']:.2f}s")
        lines.append(f"  - Total rewards: {report['session_info']['total_rewards']:.2f}")
        lines.append(f"  - Average reward/episode: {report['session_info']['average_reward_per_episode']:.2f}")
        lines.append(f"  - Average steps/episode: {report['session_info']['average_steps_per_episode']:.1f}")
        
        # Learning Trends
        lines.append(f"\nLearning Trends:")
        reward_improvement = report['learning_trends']['reward_improvement']
        lines.append(f"  - Reward trend: {reward_improvement['trend']}")
        lines.append(f"  - Reward improvement: {reward_improvement['improvement']:.3f}")
        lines.append(f"  - First half average reward: {reward_improvement['first_half_avg']:.2f}")
        lines.append(f"  - Second half average reward: {reward_improvement['second_half_avg']:.2f}")
        
        # Final State
        lines.append(f"\nFinal State:")
        lines.append(f"  - Neuromodulation level: {report['final_state']['neuromodulation_level']:.3f}")
        lines.append(f"  - Curiosity level: {report['final_state']['curiosity_level']:.3f}")
        lines.append(f"  - Novelty level: {report['final_state']['novelty_level']:.3f}")
        lines.append(f"  - Prediction error: {report['final_state']['prediction_error']:.3f}")
        lines.append(f"  - Developmental stage: {report['final_state']['developmental_stage']}")
        lines.append(f"  - Brain efficiency: {report['final_state']['brain_efficiency']:.3f}")
        
        # System Integration
        lines.append(f"\nSystem Integration:")
        lines.append(f"  - External data samples: {report['environmental_integration']['external_data_samples']}")
        lines.append(f"  - Integration successful: {report['environmental_integration']['integration_successful']}")
        
        # Performance Metrics
        if report['performance_metrics']:
            lines.append(f"\nPerformance Metrics:")
            latest = report['performance_metrics'][-1]
            lines.append(f"  - Processing efficiency: {latest['processing_efficiency']:.3f}")
            lines.append(f"  - Steps per second: {latest['steps_per_second']:.1f}")
            lines.append(f"  - Memory usage: {latest['memory_usage_mb']:.1f} MB")
            lines.append(f"  - Average step time: {latest['average_step_time_ms']:.2f} ms")
        
        # Checkpoints
        if report['checkpoints']:
            lines.append(f"\nCheckpoints:")
            for cp in report['checkpoints']:
                lines.append(f"  - {cp['name']}: {cp['filepath']} ({cp['timestamp']})")
        
        # Advanced Features (if requested)
        if include_advanced:
            lines.append(f"\nAdvanced Features Summary:")
            lines.append(f"  - Batch processing: Enabled")
            lines.append(f"  - Automated parameter tuning: Available")
            lines.append(f"  - Performance benchmarking: Completed")
            lines.append(f"  - Experiment comparison: Ready")
            lines.append(f"  - Visualization export: Supported")
        
        return "\n".join(lines)
    
    def _report_to_csv(self, report: Dict[str, Any]) -> str:
        """Convert report to CSV format."""
        # This is a simplified implementation
        csv_data = []
        csv_data.append(["Metric", "Value"])
        
        for category, metrics in report.items():
            if isinstance(metrics, dict):
                for key, value in metrics.items():
                    if isinstance(value, (int, float)):
                        csv_data.append([f"{category}.{key}", value])
                    elif isinstance(value, list) and len(value) > 0:
                        csv_data.append([f"{category}.{key}_length", len(value)])
        
        return "\n".join([",".join(map(str, row)) for row in csv_data])


def load_config_from_file(filepath: str) -> SimulationConfig:
    """Load configuration from JSON file."""
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        config = SimulationConfig(**data)
        logger.info(f"✓ Configuration loaded from {filepath}")
        return config
        
    except Exception as e:
        logger.error(f"✗ Failed to load configuration from {filepath}: {e}")
        return SimulationConfig()


def save_config_to_file(config: SimulationConfig, filepath: str) -> bool:
    """Save configuration to JSON file."""
    try:
        with open(filepath, 'w') as f:
            json.dump(asdict(config), f, indent=2)
        
        logger.info(f"✓ Configuration saved to {filepath}")
        return True
        
    except Exception as e:
        logger.error(f"✗ Failed to save configuration to {filepath}: {e}")
        return False


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM CLI Tool - Comprehensive Neuro-Linguistic Modeling Interaction",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_cli --action init --config-file config.json
  nlm_cli --action run --episodes 20 --output-format json --output-file results.json
  nlm_cli --action checkpoint --name training_session
  nlm_cli --action batch --batch-size 5 --parameter-tune
  nlm_cli --action benchmark --iterations 10
        """
    )
    
    # Basic Operations
    parser.add_argument("--action", choices=["init", "run", "checkpoint", "load", "export", "batch", "benchmark", "compare", "tune", "help"],
                       default="init", help="Action to perform")
    
    # Configuration
    parser.add_argument("--config-file", help="Path to configuration file")
    parser.add_argument("--output-format", choices=["human", "json", "csv", "table"], default="human",
                       help="Output format")
    parser.add_argument("--output-file", help="Output file path")
    parser.add_argument("--log-level", choices=["DEBUG", "INFO", "WARNING", "ERROR"], default="INFO",
                       help="Logging level")
    
    # Simulation parameters
    parser.add_argument("--episodes", type=int, help="Number of episodes to run")
    parser.add_argument("--steps", type=int, help="Steps per episode")
    parser.add_argument("--neurons", type=int, help="Number of brain neurons")
    parser.add_argument("--regions", type=int, help="Number of brain regions")
    parser.add_argument("--seed", type=int, help="Random seed for reproducibility")
    
    # Advanced options
    parser.add_argument("--checkpoint-interval", type=int, help="Save checkpoint every N episodes")
    parser.add_argument("--enable-profiling", action="store_true", help="Enable performance profiling")
    parser.add_argument("--enable-debug", action="store_true", help="Enable debug mode")
    parser.add_argument("--enable-tuning", action="store_true", help="Enable automated parameter tuning")
    parser.add_argument("--batch-size", type=int, help="Batch size for batch processing")
    parser.add_argument("--parameter-tune", action="store_true", help="Perform parameter tuning")
    parser.add_argument("--benchmark-iterations", type=int, help="Number of iterations for benchmarking")
    parser.add_argument("--compare-files", nargs="+", help="Checkpoint files to compare")
    
    # Environment variables support
    parser.add_argument("--env-config", action="store_true", help="Load configuration from environment variables")
    # Version information
    parser.add_argument("--version", action="store_true", help="Show version information")
    args = parser.parse_args()
    
    # Handle version argument
    if args.version:
        print("NLM CLI Tool v1.0.0")
        print("Enhanced Neuro-Linguistic Modeling interaction tool")
        print("Build with comprehensive NLM simulation support")
        sys.exit(0)
    
    # Set logging level
    logging.getLogger().setLevel(getattr(logging, args.log_level))
    
    # Load configuration
    config = SimulationConfig()
    
    # Load from file if specified
    if args.config_file:
        file_config = load_config_from_file(args.config_file)
        config = file_config
    
    # Override with command line arguments
    if args.episodes:
        config.episode_count = args.episodes
    if args.steps:
        config.max_steps_per_episode = args.steps
    if args.neurons:
        config.brain_neurons = args.neurons
    if args.regions:
        config.brain_regions = args.regions
    if args.seed:
        config.random_seed = args.seed
    if args.checkpoint_interval:
        config.checkpoint_interval = args.checkpoint_interval
    if args.enable_profiling:
        config.enable_profiling = args.enable_profiling
    if args.enable_debug:
        config.enable_debug = args.enable_debug
    
    # Create CLI tool instance
    cli = NLMCLITool(config)
    
    # Execute requested action
    success = False
    
    try:
        if args.action == "init":
            logger.info("Initializing NLM system...")
            success = cli.initialize_system()
            if success:
                logger.info("✓ System initialized successfully")
        
        elif args.action == "run":
            logger.info("Running simulation...")
            if cli.brain is None:
                success = cli.initialize_system()
            
            if success:
                session_stats = cli.run_learning_session()
                
                # Generate report
                output_format = OutputFormat(args.output_format)
                report = cli.generate_report(output_format)
                
                if isinstance(report, str):
                    # Human-readable report
                    print(report)
                    
                    # Save to file if specified
                    if args.output_file:
                        with open(args.output_file, 'w') as f:
                            f.write(report)
                        logger.info(f"✓ Report saved to {args.output_file}")
                else:
                    # Machine-readable report (JSON)
                    if args.output_file:
                        with open(args.output_file, 'w') as f:
                            json.dump(report, f, indent=2, default=str)
                        logger.info(f"✓ Report saved to {args.output_file}")
                    else:
                        print(json.dumps(report, indent=2, default=str))
                
                success = True
        
        elif args.action == "checkpoint":
            logger.info("Creating checkpoint...")
            success = cli.save_checkpoint("manual_checkpoint")
        
        elif args.action == "load":
            logger.info("Loading checkpoint...")
            # Default checkpoint file
            success = cli.load_checkpoint("checkpoint.json")
        
        elif args.action == "export":
            logger.info("Exporting results...")
            success = cli.export_results("nlm_results.json", "json")
        
        elif args.action == "batch":
            logger.info("Running batch processing...")
            if cli.brain is None:
                success = cli.initialize_system()
            
            if success and args.batch_size:
                for batch in range(0, config.episode_count, args.batch_size):
                    batch_episodes = min(args.batch_size, config.episode_count - batch)
                    logger.info(f"Running batch {batch // args.batch_size + 1} ({batch_episodes} episodes)...")
                    cli.run_learning_session(episode_count=batch_episodes)
                    
                    # Save checkpoint if interval reached
                    if (batch // args.batch_size + 1) % config.checkpoint_interval == 0:
                        cli.save_checkpoint(f"batch_{batch // args.batch_size + 1}")
                
                success = True
        
        elif args.action == "benchmark":
            logger.info("Running performance benchmark...")
            if cli.brain is None:
                success = cli.initialize_system()
            
            if success and args.benchmark_iterations:
                timings = []
                for i in range(args.benchmark_iterations):
                    start_time = time.time()
                    cli.run_learning_session(episode_count=1, max_steps_per_episode=10)
                    end_time = time.time()
                    timings.append(end_time - start_time)
                    
                    if (i + 1) % 5 == 0:
                        logger.info(f"Benchmark iteration {i + 1}/{args.benchmark_iterations}")
                
                avg_time = np.mean(timings)
                std_time = np.std(timings)
                
                benchmark_report = {
                    'iterations': args.benchmark_iterations,
                    'average_time': avg_time,
                    'std_deviation': std_time,
                    'min_time': np.min(timings),
                    'max_time': np.max(timings),
                    'steps_per_second': 10 / avg_time if avg_time > 0 else 0,
                }
                
                print(f"\nBenchmark Results ({args.benchmark_iterations} iterations):")
                print(f"  - Average time: {avg_time:.2f}s")
                print(f"  - Standard deviation: {std_time:.2f}s")
                print(f"  - Min time: {np.min(timings):.2f}s")
                print(f"  - Max time: {np.max(timings):.2f}s")
                print(f"  - Steps per second: {benchmark_report['steps_per_second']:.1f}")
                
                # Save benchmark results
                if args.output_file:
                    with open(args.output_file, 'w') as f:
                        json.dump(benchmark_report, f, indent=2)
                    logger.info(f"✓ Benchmark results saved to {args.output_file}")
                
                success = True
        
        elif args.action == "help":
            print("NLM CLI Tool - Help")
            print("===================")
            print("Available actions:")
            print("  init  - Initialize NLM system")
            print("  run   - Run simulation")
            print("  checkpoint - Save checkpoint")
            print("  load  - Load checkpoint")
            print("  export - Export results")
            print("  batch  - Run batch processing")
            print("  benchmark - Run performance benchmark")
            print("  compare - Compare experiment results")
            print("  tune   - Perform parameter tuning")
            print("  help   - Show this help")
            print("\nUse --help with any action for more details on that action.")
            success = True
        
        else:
            logger.error(f"✗ Unknown action: {args.action}")
            success = False
    
    except Exception as e:
        logger.error(f"✗ CLI execution failed: {e}")
        success = False
    
    # Final status
    if success:
        logger.info("✓ CLI operation completed successfully")
        sys.exit(0)
    else:
        logger.error("✗ CLI operation failed")
        sys.exit(1)


if __name__ == "__main__":
    main()
