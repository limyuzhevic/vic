#!/usr/bin/env python3
"""
NLM Python Extension Utilities

Utility functions and helper classes for advanced NLM (Neural Learning Machine)
Python bindings usage. These utilities extend the core functionality with
additional tools for simulation management, data analysis, and performance optimization.
"""

import time
import json
import math
import statistics
import threading
from typing import Dict, List, Any, Optional, Callable, Union
from datetime import datetime, timedelta
import numpy as np
import pynlm

class NLMUtilityError(Exception):
    """Exception raised for NLM utility errors."""
    pass

class BrainSessionManager:
    """
    Manages NLM brain sessions with checkpointing, rollback, and session tracking.
    
    Features:
    - Automatic checkpointing at regular intervals
    - Session rollback to previous checkpoints
    - Session statistics and metrics collection
    - Thread-safe session operations
    """
    
    def __init__(self, checkpoint_interval: int = 100):
        self.checkpoint_interval = checkpoint_interval
        self.sessions: Dict[str, Dict] = {}
        self.checkpoints: Dict[str, List[Dict]] = {}
        self.lock = threading.Lock()
    
    def create_session(self, name: str, config: Optional[Dict] = None) -> str:
        """
        Create a new brain session.
        
        Args:
            name: Unique session name
            config: Optional configuration dictionary
        
        Returns:
            Session ID
        """
        with self.lock:
            session_id = f"{name}_{int(time.time())}"
            
            session_data = {
                'id': session_id,
                'name': name,
                'created_at': datetime.now().isoformat(),
                'config': config or {},
                'brain': None,
                'steps': 0,
                'total_reward': 0.0,
                'start_time': time.time(),
                'last_checkpoint': None,
                'metrics': {
                    'spikes': [],
                    'firing_rates': [],
                    'rewards': [],
                    'neuron_counts': [],
                    'development_stages': []
                }
            }
            
            self.sessions[session_id] = session_data
            self.checkpoints[session_id] = [session_data.copy()]
            
            return session_id
    
    def load_session(self, session_id: str) -> Optional[Dict]:
        """Load session data by ID."""
        return self.sessions.get(session_id)
    
    def get_session_brain(self, session_id: str) -> Optional[Any]:
        """Get brain object from session."""
        session = self.sessions.get(session_id)
        return session['brain'] if session else None
    
    def update_session(self, session_id: str, brain: Any, step: int, 
                      reward: float = 0.0) -> bool:
        """
        Update session with current brain state.
        
        Args:
            session_id: Session identifier
            brain: NLM Brain object
            step: Current simulation step
            reward: Current reward value
        
        Returns:
            True if successful
        """
        with self.lock:
            session = self.sessions.get(session_id)
            if not session:
                return False
            
            session['brain'] = brain
            session['steps'] = step
            session['total_reward'] += reward
            
            # Collect metrics
            metrics = session['metrics']
            metrics['spikes'].append(brain.getTotalSpikeCount())
            metrics['firing_rates'].append(brain.getAverageFiringRate())
            metrics['rewards'].append(reward)
            metrics['neuron_counts'].append(brain.getFiringNeuronCount())
            metrics['development_stages'].append(brain.getDevelopmentalStage())
            
            # Create checkpoint if interval reached
            if step % self.checkpoint_interval == 0:
                self._create_checkpoint(session_id)
            
            return True
    
    def _create_checkpoint(self, session_id: str):
        """Create checkpoint for session."""
        session = self.sessions[session_id]
        checkpoint = {
            'step': session['steps'],
            'timestamp': datetime.now().isoformat(),
            'brain_state': session.copy(),
            'metrics_snapshot': {k: v[:] for k, v in session['metrics'].items()}
        }
        
        self.checkpoints[session_id].append(checkpoint)
        session['last_checkpoint'] = checkpoint
    
    def rollback_to_checkpoint(self, session_id: str, checkpoint_index: int) -> bool:
        """
        Rollback session to specific checkpoint.
        
        Args:
            session_id: Session identifier
            checkpoint_index: Index of checkpoint to restore
        
        Returns:
            True if successful
        """
        with self.lock:
            if session_id not in self.checkpoints:
                return False
            
            checkpoints = self.checkpoints[session_id]
            if checkpoint_index >= len(checkpoints):
                return False
            
            checkpoint = checkpoints[checkpoint_index]
            session = self.sessions[session_id]
            
            # Restore from checkpoint
            session.update(checkpoint['brain_state'])
            
            # Keep metrics from checkpoint
            session['metrics'] = checkpoint['metrics_snapshot']
            
            return True
    
    def get_session_stats(self, session_id: str) -> Dict:
        """Get comprehensive session statistics."""
        session = self.sessions.get(session_id)
        if not session:
            return {}
        
        metrics = session['metrics']
        if not metrics['spikes']:
            return {}
        
        stats = {
            'session_id': session_id,
            'name': session['name'],
            'total_steps': session['steps'],
            'total_reward': session['total_reward'],
            'duration': time.time() - session['start_time'],
            'avg_firing_rate': statistics.mean(metrics['firing_rates']) if metrics['firing_rates'] else 0,
            'max_firing_rate': max(metrics['firing_rates']) if metrics['firing_rates'] else 0,
            'min_firing_rate': min(metrics['firing_rates']) if metrics['firing_rates'] else 0,
            'avg_spike_count': statistics.mean(metrics['spikes']) if metrics['spikes'] else 0,
            'avg_reward': statistics.mean(metrics['rewards']) if metrics['rewards'] else 0,
            'checkpoint_count': len(self.checkpoints.get(session_id, [])),
            'last_checkpoint_step': session['last_checkpoint']['step'] if session['last_checkpoint'] else 0
        }
        
        return stats
    
    def list_sessions(self) -> List[str]:
        """List all active session IDs."""
        return list(self.sessions.keys())
class NeuralActivityAnalyzer:
    """
    Advanced analysis of neural activity patterns and characteristics.
    
    Provides tools for:
    - Pattern detection in neural activity
    - Burst analysis and characteristics
    - Synchronization analysis
    - Learning curve analysis
    - Quality metrics calculation
    """
    
    def __init__(self, sampling_interval: int = 10):
        self.sampling_interval = sampling_interval
        self.activity_history: List[Dict] = []
        self.burst_detector = self._create_burst_detector()
    
    def _create_burst_detector(self):
        """Create burst detection parameters."""
        return {
            'threshold': 3.0,  # Standard deviations above mean
            'min_duration': 5,  # Minimum burst duration in steps
            'min_activity': 10  # Minimum activity for burst
        }
    
    def analyze_activity_pattern(self, brain: Any, step: int) -> Dict:
        """
        Analyze current brain activity pattern.
        
        Args:
            brain: NLM Brain object
            step: Current simulation step
        
        Returns:
            Dictionary with activity analysis
        """
        # Collect current metrics
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        active_neurons = brain.getFiringNeuronCount()
        ei_ratio = brain.getExcitationInhibitionRatio()
        development_stage = brain.getDevelopmentalStage()
        
        # Calculate derived metrics
        activity_level = active_neurons / max(brain.getTotalNeuronCount(), 1)
        burst_likelihood = self._calculate_burst_likelihood(spike_count, firing_rate)
        pattern_complexity = self._calculate_pattern_complexity(
            active_neurons, firing_rate, ei_ratio)
        
        # Store in history
        activity_record = {
            'step': step,
            'timestamp': datetime.now().isoformat(),
            'spike_count': spike_count,
            'firing_rate': firing_rate,
            'active_neurons': active_neurons,
            'activity_level': activity_level,
            'burst_likelihood': burst_likelihood,
            'pattern_complexity': pattern_complexity,
            'excitation_inhibition_ratio': ei_ratio,
            'development_stage': development_stage.value
        }
        
        self.activity_history.append(activity_record)
        
        # Perform burst detection if sampling interval reached
        if step % self.sampling_interval == 0:
            self._detect_bursts()
        
        return activity_record
    
    def _calculate_burst_likelihood(self, spike_count: int, firing_rate: float) -> float:
        """Calculate likelihood of burst activity."""
        # Simple heuristic: high spike count with moderate firing rate suggests bursts
        if firing_rate < 0.1:
            return 0.0
        
        # Normalize spike count (assuming typical range)
        normalized_spikes = min(spike_count / 1000.0, 1.0)
        
        # Burst likelihood increases with spike count and decreases with firing rate
        burst_likelihood = (normalized_spikes * 0.7) + (0.3 * (1.0 - firing_rate))
        
        return min(burst_likelihood, 1.0)
    
    def _calculate_pattern_complexity(self, active_neurons: int, firing_rate: float, 
                                    ei_ratio: float) -> float:
        """Calculate pattern complexity score."""
        # Complexity increases with balanced E/I and moderate activity
        ei_balance = 1.0 - abs(ei_ratio - 1.0) / max(ei_ratio, 1.0)
        activity_modulation = min(firing_rate / 10.0, 1.0)
        
        complexity = (ei_balance * 0.5) + (activity_modulation * 0.5)
        return complexity
    
    def _detect_bursts(self):
        """Detect burst patterns in activity history."""
        if len(self.activity_history) < self.burst_detector['min_duration']:
            return
        
        # Simple burst detection based on spike count spikes
        spike_counts = [record['spike_count'] for record in self.activity_history]
        
        for i in range(len(spike_counts) - self.burst_detector['min_duration']):
            # Check if this window contains a burst
            window_spikes = spike_counts[i:i + self.burst_detector['min_duration']]
            
            # Calculate window statistics
            mean_spikes = statistics.mean(window_spikes)
            std_spikes = statistics.stdev(window_spikes) if len(window_spikes) > 1 else 0
            
            # Check if window represents a burst
            if (std_spikes > self.burst_detector['threshold'] * mean_spikes and
                mean_spikes > self.burst_detector['min_activity']):
                # Found a burst
                burst_record = {
                    'step_start': self.activity_history[i]['step'],
                    'step_end': self.activity_history[i + self.burst_detector['min_duration'] - 1]['step'],
                    'mean_spikes': mean_spikes,
                    'std_spikes': std_spikes,
                    'burst_score': std_spikes / mean_spikes if mean_spikes > 0 else 0,
                    'detected_at': datetime.now().isoformat()
                }
                
                # Store burst detection (would integrate with a burst database in real implementation)
                if 'bursts_detected' not in self.__dict__:
                    self.bursts_detected = []
                self.bursts_detected.append(burst_record)
    
    def analyze_learning_dynamics(self) -> Dict:
        """Analyze learning dynamics over time."""
        if not self.activity_history:
            return {}
        
        spikes = [record['spike_count'] for record in self.activity_history]
        firing_rates = [record['firing_rate'] for record in self.activity_history]
        development_stages = [record['development_stage'] for record in self.activity_history]
        
        # Calculate learning metrics
        early_spikes = statistics.mean(spikes[:len(spikes)//3]) if len(spikes) >= 3 else 0
        late_spikes = statistics.mean(spikes[-len(spikes)//3:]) if len(spikes) >= 3 else 0
        spike_change = late_spikes - early_spikes
        
        early_firing = statistics.mean(firing_rates[:len(firing_rates)//3]) if len(firing_rates) >= 3 else 0
        late_firing = statistics.mean(firing_rates[-len(firing_rates)//3:]) if len(firing_rates) >= 3 else 0
        firing_change = late_firing - early_firing
        
        # Determine if learning is occurring
        learning_score = 0.0
        if spike_change > 0:
            learning_score += 0.5  # Spike increase suggests learning
        if firing_change > 0.1:
            learning_score += 0.5  # Firing rate increase suggests learning
        
        return {
            'learning_score': learning_score,
            'spike_change': spike_change,
            'firing_change': firing_change,
            'early_spike_average': early_spikes,
            'late_spike_average': late_spikes,
            'activity_improvement': spike_change > 0 and firing_change > 0,
            'stability_score': 1.0 / (1.0 + statistics.stdev(spikes) if len(spikes) > 1 else 0)
        }
    
    def get_pattern_summary(self) -> Dict:
        """Get summary of detected patterns."""
        if not self.activity_history:
            return {}
        
        spikes = [record['spike_count'] for record in self.activity_history]
        firing_rates = [record['firing_rate'] for record in self.activity_history]
        
        return {
            'total_samples': len(self.activity_history),
            'spike_statistics': {
                'mean': statistics.mean(spikes) if spikes else 0,
                'median': statistics.median(spikes) if spikes else 0,
                'std': statistics.stdev(spikes) if len(spikes) > 1 else 0,
                'min': min(spikes) if spikes else 0,
                'max': max(spikes) if spikes else 0
            },
            'firing_rate_statistics': {
                'mean': statistics.mean(firing_rates) if firing_rates else 0,
                'median': statistics.median(firing_rates) if firing_rates else 0,
                'std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
                'min': min(firing_rates) if firing_rates else 0,
                'max': max(firing_rates) if firing_rates else 0
            },
            'pattern_trends': {
                'spike_trend': 'increasing' if spike_change > 0 else 'decreasing' if spike_change < 0 else 'stable',
                'firing_trend': 'increasing' if firing_change > 0 else 'decreasing' if firing_change < 0 else 'stable'
            }
        }
    
    def export_activity_data(self, filename: str):
        """Export activity data to JSON file."""
        data = {
            'metadata': {
                'export_time': datetime.now().isoformat(),
                'total_samples': len(self.activity_history),
                'sampling_interval': self.sampling_interval
            },
            'activity_history': self.activity_history,
            'bursts_detected': getattr(self, 'bursts_detected', [])
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2, default=str)
class PerformanceProfiler:
    """
    Performance profiling and optimization for NLM simulations.
    
    Features:
    - Simulation performance timing
    - Memory usage tracking
    - Resource utilization monitoring
    - Optimization recommendations
    - Benchmarking capabilities
    """
    
    def __init__(self):
        self.profile_data = {
            'sessions': {},
            'system_metrics': {},
            'performance_trends': {},
            'optimization_scores': {}
        }
        self.start_time = time.time()
    
    def start_session(self, session_name: str) -> str:
        """
        Start performance profiling for a session.
        
        Args:
            session_name: Name of the profiling session
        
        Returns:
            Session ID for this profiling session
        """
        session_id = f"perf_{session_name}_{int(self.start_time)}"
        
        self.profile_data['sessions'][session_id] = {
            'name': session_name,
            'start_time': time.time(),
            'end_time': None,
            'brain_configs': [],
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'performance_metrics': {}
        }
        
        return session_id
    
    def record_brain_config(self, session_id: str, config: Dict):
        """Record brain configuration for profiling."""
        if session_id in self.profile_data['sessions']:
            self.profile_data['sessions'][session_id]['brain_configs'].append({
                'config': config,
                'recorded_at': time.time()
            })
    
    def record_step_performance(self, session_id: str, step: int, 
                               step_time: float, memory_usage: float,
                               cpu_usage: float):
        """Record performance metrics for a simulation step."""
        if session_id in self.profile_data['sessions']:
            session = self.profile_data['sessions'][session_id]
            
            session['step_times'].append({
                'step': step,
                'time': step_time,
                'memory_mb': memory_usage,
                'cpu_percent': cpu_usage
            })
            
            session['memory_usage'].append(memory_usage)
            session['cpu_usage'].append(cpu_usage)
    
    def end_session(self, session_id: str):
        """End performance profiling session."""
        if session_id in self.profile_data['sessions']:
            session = self.profile_data['sessions'][session_id]
            session['end_time'] = time.time()
            
            # Calculate performance metrics
            if session['step_times']:
                step_times = [s['time'] for s in session['step_times']]
                session['performance_metrics']['avg_step_time'] = statistics.mean(step_times)
                session['performance_metrics']['min_step_time'] = min(step_times)
                session['performance_metrics']['max_step_time'] = max(step_times)
                session['performance_metrics']['std_step_time'] = statistics.stdev(step_times) if len(step_times) > 1 else 0
                
                session['performance_metrics']['steps_per_second'] = (
                    len(session['step_times']) / sum(step_times) if sum(step_times) > 0 else 0)
                
                session['performance_metrics']['memory_efficiency'] = (
                    session['performance_metrics']['steps_per_second'] /
                    (statistics.mean(session['memory_usage']) + 0.001))
    
    def calculate_optimization_score(self, session_id: str) -> float:
        """Calculate optimization score for a session."""
        if session_id not in self.profile_data['sessions']:
            return 0.0
        
        session = self.profile_data['sessions'][session_id]
        metrics = session.get('performance_metrics', {})
        
        # Calculate composite optimization score
        scores = []
        
        # Step efficiency
        if 'steps_per_second' in metrics:
            scores.append(min(metrics['steps_per_second'] / 100.0, 1.0))
        
        # Time consistency (lower variance is better)
        if 'std_step_time' in metrics and 'avg_step_time' in metrics:
            consistency = 1.0 / (1.0 + metrics['std_step_time'] / max(metrics['avg_step_time'], 0.001))
            scores.append(consistency)
        
        # Memory efficiency
        if 'memory_efficiency' in metrics:
            scores.append(min(metrics['memory_efficiency'] / 10.0, 1.0))
        
        # Overall optimization score
        optimization_score = statistics.mean(scores) if scores else 0.0
        
        self.profile_data['optimization_scores'][session_id] = optimization_score
        
        return optimization_score
    
    def generate_recommendations(self, session_id: str) -> List[str]:
        """Generate optimization recommendations for a session."""
        recommendations = []
        
        if session_id not in self.profile_data['sessions']:
            return recommendations
        
        session = self.profile_data['sessions'][session_id]
        metrics = session.get('performance_metrics', {})
        
        # Analyze step times
        if 'std_step_time' in metrics and metrics['std_step_time'] > metrics.get('avg_step_time', 0) * 0.5:
            recommendations.append(
                "Step times show high variance - consider optimizing brain initialization"
            )
        
        # Analyze memory efficiency
        if 'memory_efficiency' in metrics and metrics['memory_efficiency'] < 5.0:
            recommendations.append(
                "Memory efficiency could be improved - consider memory pool optimization"
            )
        
        # Analyze step rate
        if 'steps_per_second' in metrics and metrics['steps_per_second'] < 10.0:
            recommendations.append(
                "Simulation step rate is low - consider performance optimization"
            )
        
        if not recommendations:
            recommendations.append(
                "Performance is good - consider micro-optimizations for further gains"
            )
        
        return recommendations
    
    def export_profile(self, session_id: str, filename: str):
        """Export profiling data to JSON file."""
        profile_data = {
            'session_id': session_id,
            'export_time': datetime.now().isoformat(),
            'profile_data': self.profile_data['sessions'].get(session_id, {}),
            'optimization_score': self.profile_data['optimization_scores'].get(session_id, 0.0),
            'recommendations': self.generate_recommendations(session_id)
        }
        
        with open(filename, 'w') as f:
            json.dump(profile_data, f, indent=2, default=str)
class NLMSimulationOrchestrator:
    """
    High-level orchestrator for NLM simulations with advanced features.
    
    Provides:
    - Simulation workflow management
    - Multi-agent coordination
    - Environment simulation
    - Experiment management
    - Result aggregation and analysis
    """
    
    def __init__(self):
        self.simulation_manager = BrainSessionManager()
        self.activity_analyzer = NeuralActivityAnalyzer()
        self.performance_profiler = PerformanceProfiler()
        self.active_simulations: Dict[str, Any] = {}
    
    def create_complex_simulation(self, name: str, config: Dict) -> str:
        """
        Create a complex NLM simulation with multiple components.
        
        Args:
            name: Simulation name
            config: Configuration dictionary
        
        Returns:
            Simulation session ID
        """
        # Create brain session
        session_id = self.simulation_manager.create_session(name, config)
        
        # Create brain with advanced configuration
        brain_config = pynlm.createDefaultConfig()
        
        # Apply advanced configuration
        advanced_config = {
            'neuron_count': config.get('neuron_count', 500),
            'region_count': config.get('region_count', 2),
            'connection_probability': config.get('connection_probability', 0.15),
            'stdp_ltp_weight': config.get('stdp_ltp_weight', 0.02),
            'stdp_ltd_weight': config.get('stdp_ltd_weight', 0.015),
            'synaptogenesis_rate': config.get('synaptogenesis_rate', 0.0001),
            'pruning_rate': config.get('pruning_rate', 0.00001),
            'development_enabled': config.get('development_enabled', True),
            'curiosity_enabled': config.get('curiosity_enabled', True),
            'reward_learning': config.get('reward_learning', True)
        }
        
        for key, value in advanced_config.items():
            brain_config.set(key, value)
        
        # Create brain
        brain = pynlm.createBrain(brain_config)
        
        # Create agent if needed
        agent = None
        if config.get('include_agent', True):
            agent = pynlm.createAgentBrain(brain)
            
            # Create world if needed
            world = None
            if config.get('include_world', True):
                world = pynlm.createSimpleWorld()
                world.configure(
                    width=config.get('world_width', 20),
                    height=config.get('world_height', 20),
                    vision_width=config.get('vision_width', 8),
                    vision_height=config.get('vision_height', 8)
                )
                world.reset()
                world.setAgentStart(10.0f, 10.0f)
                
                agent.initialize(world)
                
                # Enable learning features
                agent.enableRewardModulation(config.get('reward_modulation', True))
                agent.enableStructuralPlasticity(config.get('structural_plasticity', True))
                agent.enableDevelopment(config.get('development', True))
                agent.enableCuriosity(config.get('curiosity', True))
        
        # Store in session
        self.simulation_manager.update_session(session_id, brain)
        self.active_simulations[session_id] = {
            'brain': brain,
            'agent': agent,
            'world': world,
            'config': config,
            'session_manager': self.simulation_manager,
            'activity_analyzer': self.activity_analyzer
        }
        
        # Start performance profiling
        perf_session_id = self.performance_profiler.start_session(f"{name}_performance")
        self.performance_profiler.record_brain_config(perf_session_id, advanced_config)
        
        return session_id
    
    def run_simulation_step(self, session_id: str, step: int, reward: float = 0.0) -> Dict:
        """
        Run a single simulation step.
        
        Args:
            session_id: Simulation session ID
            step: Current step number
            reward: Current reward value
        
        Returns:
            Dictionary with step results
        """
        simulation = self.active_simulations.get(session_id)
        if not simulation:
            raise NLMUtilityError(f"Simulation {session_id} not found")
        
        brain = simulation['brain']
        agent = simulation.get('agent')
        world = simulation.get('world')
        
        # Get current performance metrics
        memory_usage = self._get_memory_usage()
        cpu_usage = self._get_cpu_usage()
        
        # Record performance
        perf_session_id = f"perf_{session_id}"
        self.performance_profiler.record_step_performance(
            perf_session_id, step, 0.0, memory_usage, cpu_usage
        )
        
        # Run brain step
        brain.step(step)
        
        # Process with agent if available
        if agent and world:
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Update session
        self.simulation_manager.update_session(session_id, brain, step, reward)
        
        # Analyze activity
        activity_record = self.activity_analyzer.analyze_activity_pattern(brain, step)
        
        # Get current performance
        performance_metrics = simulation['brain'].getPerformanceMetrics()
        
        return {
            'step': step,
            'success': True,
            'brain_metrics': {
                'spike_count': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getFiringNeuronCount(),
                'development_stage': brain.getDevelopmentalStage().value
            },
            'agent_metrics': {
                'neuromodulation': agent.getNeuromodulationLevel() if agent else 0.0,
                'curiosity': agent.getCuriosityLevel() if agent else 0.0,
                'novelty': agent.getNoveltyLevel() if agent else 0.0,
                'prediction_error': agent.getPredictionError() if agent else 0.0
            } if agent else {},
            'performance': {
                'step_time': 0.0,
                'memory_usage': memory_usage,
                'cpu_usage': cpu_usage
            },
            'activity_analysis': activity_record
        }
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        import psutil
        return psutil.Process().memory_info().rss / 1024 / 1024
    
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage percentage."""
        import psutil
        return psutil.cpu_percent()
    
    def run_complete_simulation(self, session_id: str, max_steps: int) -> Dict:
        """
        Run complete simulation until completion.
        
        Args:
            session_id: Simulation session ID
            max_steps: Maximum number of steps
        
        Returns:
            Dictionary with final results
        """
        print(f"Running simulation {session_id} for {max_steps} steps...")
        
        simulation = self.active_simulations.get(session_id)
        if not simulation:
            raise NLMUtilityError(f"Simulation {session_id} not found")
        
        start_time = time.time()
        results = []
        
        for step in range(max_steps):
            step_start = time.time()
            
            # Generate random reward for simulation
            reward = 0.0
            if step % 50 == 0:
                reward = 1.0  # Positive reward
            elif step % 75 == 0:
                reward = -0.5  # Negative reward
            
            # Run step
            step_result = self.run_simulation_step(session_id, step, reward)
            
            # Add step timing
            step_result['performance']['step_time'] = time.time() - step_start
            
            results.append(step_result)
            
            # Print progress
            if step % 100 == 0:
                print(f"  Step {step}/{max_steps}: "
                      f"Firing rate: {step_result['brain_metrics']['firing_rate']:.2f} Hz, "
                      f"Reward: {reward:.1f}")
        
        # Get final session stats
        session_stats = self.simulation_manager.get_session_stats(session_id)
        
        # Analyze activity patterns
        learning_dynamics = self.activity_analyzer.analyze_learning_dynamics()
        pattern_summary = self.activity_analyzer.get_pattern_summary()
        
        # Calculate performance
        optimization_score = self.performance_profiler.calculate_optimization_score(session_id)
        recommendations = self.performance_profiler.generate_recommendations(session_id)
        
        # End performance profiling
        perf_session_id = f"perf_{session_id}"
        self.performance_profiler.end_session(perf_session_id)
        
        total_time = time.time() - start_time
        
        return {
            'session_id': session_id,
            'total_steps': max_steps,
            'total_time': total_time,
            'steps_per_second': max_steps / total_time if total_time > 0 else 0,
            'final_metrics': session_stats,
            'learning_dynamics': learning_dynamics,
            'pattern_summary': pattern_summary,
            'optimization_score': optimization_score,
            'recommendations': recommendations,
            'step_results': results,
            'simulation_completed': True
        }
    
    def get_simulation_status(self, session_id: str) -> Dict:
        """Get comprehensive simulation status."""
        simulation = self.active_simulations.get(session_id)
        if not simulation:
            return {}
        
        brain = simulation['brain']
        
        return {
            'session_id': session_id,
            'brain_status': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage().value
            },
            'simulation_status': {
                'steps_completed': simulation.get('steps_completed', 0),
                'target_steps': simulation.get('target_steps', 0),
                'progress_percentage': (
                    simulation.get('steps_completed', 0) / 
                    max(session.get('target_steps', 1), 1) * 100
                )
            },
            'activity_analyzer_status': {
                'total_samples': len(self.activity_analyzer.activity_history),
                'bursts_detected': len(getattr(self.activity_analyzer, 'bursts_detected', []))
            },
            'performance_profiler_status': {
                'sessions_monitored': len(self.performance_profiler.profile_data['sessions']),
                'optimization_scores': len(self.performance_profiler.profile_data['optimization_scores'])
            }
        }
    
    def export_simulation_results(self, session_id: str, filename: str):
        """Export simulation results to file."""
        simulation = self.active_simulations.get(session_id)
        if not simulation:
            raise NLMUtilityError(f"Simulation {session_id} not found")
        
        # Get session results
        session_stats = self.simulation_manager.get_session_stats(session_id)
        learning_dynamics = self.activity_analyzer.analyze_learning_dynamics()
        pattern_summary = self.activity_analyzer.get_pattern_summary()
        
        # Create comprehensive results
        export_data = {
            'simulation_id': session_id,
            'export_time': datetime.now().isoformat(),
            'configuration': simulation['config'],
            'session_stats': session_stats,
            'learning_dynamics': learning_dynamics,
            'pattern_summary': pattern_summary,
            'activity_history': self.activity_analyzer.activity_history,
            'performance_profile': self.performance_profiler.profile_data['sessions'].get(f"perf_{session_id}", {}),
            'recommendations': self.performance_profiler.generate_recommendations(session_id)
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"Simulation results exported to: {filename}")
class UtilityManager:
    """
    Central manager for all NLM utilities and services.
    
    Provides unified access to:
    - Session management
    - Activity analysis
    - Performance profiling
    - Simulation orchestration
    """
    
    def __init__(self):
        self.simulation_orchestrator = NLMSimulationOrchestrator()
        self.session_manager = BrainSessionManager()
        self.activity_analyzer = NeuralActivityAnalyzer()
        self.performance_profiler = PerformanceProfiler()
    
    def create_advanced_simulation(self, name: str, config: Dict) -> str:
        """Create an advanced simulation using all utilities."""
        return self.simulation_orchestrator.create_complex_simulation(name, config)
    
    def run_simulation_complete(self, session_id: str, max_steps: int) -> Dict:
        """Run complete simulation with all utilities."""
        return self.simulation_orchestrator.run_complete_simulation(session_id, max_steps)
    
    def get_comprehensive_status(self) -> Dict:
        """Get comprehensive status of all utilities."""
        status = {
            'session_manager': {
                'active_sessions': len(self.session_manager.list_sessions()),
                'services': {
                    'brain_session_management': 'active',
                    'checkpointing': 'active',
                    'rollback': 'active'
                }
            },
            'activity_analyzer': {
                'services': {
                    'pattern_detection': 'active',
                    'burst_detection': 'active',
                    'learning_dynamics': 'active',
                    'complexity_analysis': 'active'
                },
                'history_samples': len(self.activity_analyzer.activity_history)
            },
            'performance_profiler': {
                'services': {
                    'performance_timing': 'active',
                    'memory_tracking': 'active',
                    'cpu_monitoring': 'active',
                    'optimization_scoring': 'active'
                },
                'sessions_monitored': len(self.performance_profiler.profile_data['sessions'])
            },
            'simulation_orchestrator': {
                'services': {
                    'complex_simulation_creation': 'active',
                    'multi_agent_coordination': 'active',
                    'experiment_management': 'active',
                    'result_aggregation': 'active'
                },
                'active_simulations': len(self.simulation_orchestrator.active_simulations)
            }
        }
        
        return status

# Global utility manager instance
utility_manager = UtilityManager()

# Convenience functions for quick access
def create_brain_session(name: str, config: Optional[Dict] = None) -> str:
    """Quick function to create a brain session."""
    return utility_manager.session_manager.create_session(name, config)

def analyze_brain_activity(brain: Any, step: int) -> Dict:
    """Quick function to analyze brain activity."""
    return utility_manager.activity_analyzer.analyze_activity_pattern(brain, step)

def profile_simulation(session_id: str, step: int, step_time: float,
                      memory_usage: float, cpu_usage: float):
    """Quick function to profile simulation performance."""
    perf_session_id = f"perf_{session_id}"
    utility_manager.performance_profiler.record_step_performance(
        perf_session_id, step, step_time, memory_usage, cpu_usage
    )

def get_utility_status() -> Dict:
    """Get comprehensive utility status."""
    return utility_manager.get_comprehensive_status()

# Export all key classes and functions
__all__ = [
    # Main classes
    'BrainSessionManager',
    'NeuralActivityAnalyzer',
    'PerformanceProfiler',
    'NLMSimulationOrchestrator',
    'UtilityManager',
    
    # Exception classes
    'NLMUtilityError',
    
    # Convenience functions
    'create_brain_session',
    'analyze_brain_activity',
    'profile_simulation',
    'get_utility_status',
    
    # Global instances
    'utility_manager'
]

# Module metadata
__version__ = "1.0.0"
__author__ = "NLM Project"
__description__ = "Advanced utilities and services for NLM Python bindings"

# Example usage function
def example_usage():
    """Show example usage of NLM utilities."""
    print("=== NLM Utilities Example Usage ===")
    print("1. Create a brain session:")
    print("   session_id = create_brain_session('my_simulation', {'neuron_count': 1000})")
    print()
    print("2. Analyze brain activity:")
    print("   activity = analyze_brain_activity(brain, step)")
    print()
    print("3. Profile performance:")
    print("   profile_simulation(session_id, step, step_time, memory, cpu)")
    print()
    print("4. Get comprehensive status:")
    print("   status = get_utility_status()")
    print()
    print("5. Create advanced simulation:")
    print("   sim_id = utility_manager.create_advanced_simulation('demo', config)")
    print()
    print("6. Run complete simulation:")
    print("   results = utility_manager.run_simulation_complete(sim_id, 1000)")
    print()
    print("✓ See individual class documentation for more details!")