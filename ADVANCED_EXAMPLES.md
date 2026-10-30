# NLM - Advanced Usage Examples for Power Users

## Overview

This document provides comprehensive examples and advanced usage patterns for the enhanced NLM Python bindings. These examples demonstrate how to leverage the advanced features including configuration management, performance profiling, neural activity analysis, and debugging capabilities.

## Advanced Example 1: Professional Neural Network Development

```python
import pynlm
import time
import json
from typing import List, Dict, Any

class ProfessionalNeuralNetwork:
    """Professional-grade neural network development environment."""
    
    def __init__(self, config_name="advanced", enable_monitoring=True):
        self.config_name = config_name
        self.enable_monitoring = enable_monitoring
        self.config = None
        self.brain = None
        self.agent = None
        self.world = None
        self.monitor = None
        self.profiler = None
        self.tracker = None
        
        self.initialize_system()
    
    def initialize_system(self):
        """Initialize the complete neural system with advanced features."""
        print(f"Initializing {self.config_name} neural network...")
        
        # Create optimized configuration
        self.config = pynlm.create_preset_config(self.config_name)
        
        # Initialize performance profiler
        self.profiler = pynlm.PerformanceProfiler("neural_system")
        self.profiler.start()
        
        # Create and initialize brain with safety checks
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize_safe()
        
        # Create simulation environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(32, 32, 16, 16)
        self.world.setAgentStart(16.0, 16.0)
        self.world.reset()
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all learning systems for advanced capabilities
        self.agent.enable_all_systems()
        
        # Initialize neural activity tracker
        if self.enable_monitoring:
            self.tracker = pynlm.NeuralActivityTracker()
            self.tracker.start()
        
        # Stop profiler for initialization
        self.profiler.stop()
        
        print(f"  ✓ System initialized with {self.brain.getTotalNeuronCount()} neurons")
        print(f"  ✓ Agent ready for {self.config_name} configuration")
    
    def run_adaptive_simulation(self, num_steps=1000, adaptive_params=None):
        """Run simulation with adaptive parameters and monitoring."""
        if adaptive_params is None:
            adaptive_params = {}
        
        print(f"Starting adaptive simulation with {num_steps} steps...")
        
        # Adaptive simulation parameters
        exploration_rate = adaptive_params.get("exploration_rate", 0.1)
        curiosity_threshold = adaptive_params.get("curiosity_threshold", 0.5)
        novelty_sensitivity = adaptive_params.get("novelty_sensitivity", 0.3)
        
        # Performance tracking
        step_times = []
        firing_rates = []
        rewards = []
        curiosity_levels = []
        
        # Start detailed profiling
        self.profiler.start()
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Adaptive decision making based on current state
            if curiosity_levels and curiosity_levels[-1] > curiosity_threshold:
                # High curiosity - explore more
                exploration_boost = novelty_sensitivity * curiosity_levels[-1]
            else:
                exploration_boost = 1.0
            
            # Update world with adaptive parameters
            world_dt = 0.1 * exploration_boost
            self.world.update(world_dt)
            
            # Get sensory input with noise for exploration
            percept = self.world.getSensoryPercept()
            if exploration_rate > 0 and step % int(1/exploration_rate) == 0:
                # Add exploration noise
                percept = self.add_exploration_noise(percept)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processing with performance tracking
            brain_step_start = time.time()
            self.brain.step(step, step * 0.001)
            brain_step_time = (time.time() - brain_step_start) * 1000
            step_times.append(brain_step_time)
            
            # Adaptive action selection based on curiosity and novelty
            action = self.adaptive_action_selection(curiosity_threshold, novelty_sensitivity)
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = self.calculate_reward(percept, action)
            rewards.append(reward)
            
            # Apply reward modulation with adaptive learning rate
            learning_rate = adaptive_params.get("learning_rate", 0.01)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development with time-dependent plasticity
            development_time = step * world_dt
            self.agent.updateDevelopment(development_time)
            
            # Capture neural activity for analysis
            if self.tracker:
                self.tracker.capture_snapshot(step)
            
            # Collect performance metrics
            firing_rates.append(self.brain.getAverageFiringRate())
            curiosity_levels.append(self.agent.getCuriosityLevel())
            
            # Periodic system analysis
            if step % 100 == 0:
                self.analyze_system_health(step, firing_rates, curiosity_levels, rewards)
                
                # Adaptive parameter adjustment
                if self.should_adjust_parameters(step, firing_rates, curiosity_levels):
                    self.adjust_adaptive_parameters(adaptive_params, step)
            
            # Record step time
            total_step_time = (time.time() - step_start) * 1000
            step_times[-1] += total_step_time  # Add world update time
        
        # Stop profiling
        self.profiler.stop()
        
        # Generate comprehensive analysis
        analysis = self.generate_comprehensive_analysis(
            step_times, firing_rates, rewards, curiosity_levels, num_steps
        )
        
        return analysis
    
    def add_exploration_noise(self, percept):
        """Add controlled noise for exploration."""
        # Implement noise addition logic
        return percept
    
    def adaptive_action_selection(self, curiosity_threshold, novelty_sensitivity):
        """Select actions based on adaptive criteria."""
        # Get current agent state
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        
        # Adaptive action selection logic
        if curiosity > curiosity_threshold and novelty > novelty_sensitivity:
            # High exploration - select random action
            return self.select_exploratory_action()
        else:
            # Normal operation - use learned policy
            return self.agent.decodeMotorCommand()
    
    def calculate_reward(self, percept, action):
        """Calculate reward based on percept and action."""
        # Implement reward calculation
        return 1.0
    
    def should_adjust_parameters(self, step, firing_rates, curiosity_levels):
        """Determine if adaptive parameters should be adjusted."""
        # Implement parameter adjustment logic
        return False
    
    def adjust_adaptive_parameters(self, adaptive_params, step):
        """Adjust adaptive parameters based on system state."""
        # Implement parameter adjustment
        pass
    
    def analyze_system_health(self, step, firing_rates, curiosity_levels, rewards):
        """Analyze system health and performance."""
        avg_firing_rate = sum(firing_rates) / len(firing_rates)
        avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
        avg_reward = sum(rewards) / len(rewards)
        
        # Health diagnostics
        health_status = "healthy"
        if avg_firing_rate < 0.1 or avg_firing_rate > 100:
            health_status = "concerning"
        if avg_curiosity < 0.01:
            health_status = "underactive"
        if avg_reward < -0.5:
            health_status = "unhealthy"
        
        print(f"  Step {step}: Health={health_status}, "
              f"Firing Rate={avg_firing_rate:.2f} Hz, "
              f"Curiosity={avg_curiosity:.2f}, "
              f"Reward={avg_reward:.2f}")
    
    def generate_comprehensive_analysis(self, step_times, firing_rates, rewards, curiosity_levels, num_steps):
        """Generate comprehensive analysis report."""
        analysis = {
            "simulation_summary": {
                "total_steps": num_steps,
                "total_time_ms": sum(step_times),
                "avg_step_time_ms": sum(step_times) / len(step_times),
                "min_step_time_ms": min(step_times),
                "max_step_time_ms": max(step_times),
                "std_step_time_ms": self.calculate_std(step_times)
            },
            "neural_performance": {
                "avg_firing_rate": sum(firing_rates) / len(firing_rates),
                "firing_rate_std": self.calculate_std(firing_rates),
                "firing_rate_min": min(firing_rates),
                "firing_rate_max": max(firing_rates)
            },
            "learning_metrics": {
                "avg_reward": sum(rewards) / len(rewards),
                "reward_std": self.calculate_std(rewards),
                "avg_curiosity": sum(curiosity_levels) / len(curiosity_levels),
                "final_curiosity": curiosity_levels[-1] if curiosity_levels else 0,
                "learning_efficiency": self.calculate_learning_efficiency(rewards)
            },
            "performance_bottlenecks": self.identify_bottlenecks(step_times),
            "neural_insights": self.generate_neural_insights(firing_rates, curiosity_levels)
        }
        
        return analysis
    
    def calculate_std(self, values):
        """Calculate standard deviation."""
        if len(values) < 2:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
        return (variance ** 0.5)
    
    def calculate_learning_efficiency(self, rewards):
        """Calculate learning efficiency based on reward progression."""
        if len(rewards) < 2:
            return 0.0
        
        # Calculate reward improvement over time
        first_half = rewards[:len(rewards)//2]
        second_half = rewards[len(rewards)//2:]
        
        first_avg = sum(first_half) / len(first_half)
        second_avg = sum(second_half) / len(second_half)
        
        if first_avg == 0:
            return 0.0
        
        return (second_avg - first_avg) / abs(first_avg)
    
    def identify_bottlenecks(self, step_times):
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        if len(step_times) < 10:
            return bottlenecks
        
        # Check for step time spikes
        avg_time = sum(step_times) / len(step_times)
        threshold = avg_time * 2  # 2x average time
        
        for i, time in enumerate(step_times):
            if time > threshold:
                bottlenecks.append({
                    "step": i,
                    "time_ms": time,
                    "threshold_ms": threshold,
                    "severity": "high" if time > avg_time * 3 else "medium"
                })
        
        return bottlenecks
    
    def generate_neural_insights(self, firing_rates, curiosity_levels):
        """Generate insights about neural activity."""
        insights = []
        
        if len(firing_rates) < 10:
            return insights
        
        # Analyze firing rate trends
        first_half = firing_rates[:len(firing_rates)//2]
        second_half = firing_rates[len(firing_rates)//2:]
        
        first_avg = sum(first_half) / len(first_half)
        second_avg = sum(second_half) / len(second_half)
        
        if second_avg > first_avg * 1.1:
            insights.append("Neural activity increasing - system warming up")
        elif second_avg < first_avg * 0.9:
            insights.append("Neural activity decreasing - system stabilizing")
        
        # Analyze curiosity dynamics
        if curiosity_levels:
            avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
            if avg_curiosity < 0.1:
                insights.append("Low curiosity - consider increasing exploration")
            elif avg_curiosity > 0.8:
                insights.append("High curiosity - system actively exploring")
        
        return insights
    
    def save_results(self, filename, analysis):
        """Save analysis results to file."""
        with open(filename, 'w') as f:
            json.dump(analysis, f, indent=2)
    
    def __del__(self):
        """Cleanup resources."""
        if self.tracker:
            self.tracker.stop()
        
        print("Professional neural network session completed successfully.")

# Usage example
if __name__ == "__main__":
    # Create professional neural network
    nn = ProfessionalNeuralNetwork(config_name="advanced", enable_monitoring=True)
    
    # Run adaptive simulation
    adaptive_params = {
        "exploration_rate": 0.05,
        "curiosity_threshold": 0.3,
        "novelty_sensitivity": 0.2,
        "learning_rate": 0.005
    }
    
    results = nn.run_adaptive_simulation(1000, adaptive_params)
    
    # Save results
    nn.save_results("neural_network_analysis.json", results)
    
    # Print summary
    print("\n=== SIMULATION RESULTS ===")
    print(f"Performance: {results['simulation_summary']['steps_per_second']:.2f} steps/second")
    print(f"Neural Activity: {results['neural_performance']['avg_firing_rate']:.2f} Hz average")
    print(f"Learning Efficiency: {results['learning_metrics']['learning_efficiency']:.2%}")
```

## Advanced Example 2: Research-Grade Neural Network Analysis

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
from typing import Tuple, List

class ResearchNeuralAnalyzer:
    """Research-grade neural network analysis and visualization."""
    
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.neural_history = []
        self.performance_history = []
        self.error_events = []
    
    def run_research_simulation(self, duration_steps: int, analysis_interval: int = 100) -> Dict[str, Any]:
        """Run comprehensive research simulation with detailed analysis."""
        print(f"Starting research simulation with {duration_steps} steps...")
        
        for step in range(duration_steps):
            # Capture baseline state
            baseline = self.capture_research_baseline(step)
            
            # Run simulation step
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step, step * 0.001)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply neuromodulation
            reward = self.calculate_research_reward(percept, action)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Capture analysis data
            analysis_data = self.analyze_research_step(step, baseline, percept, action, reward)
            self.performance_history.append(analysis_data)
            
            # Periodic detailed analysis
            if step % analysis_interval == 0 and step > 0:
                self.run_detailed_analysis(step, analysis_interval)
        
        # Generate research-grade report
        research_report = self.generate_research_report()
        return research_report
    
    def capture_research_baseline(self, step: int) -> Dict[str, Any]:
        """Capture comprehensive baseline data for research analysis."""
        return {
            'step': step,
            'timestamp': time.time(),
            'brain_neuron_count': self.brain.getTotalNeuronCount(),
            'brain_synapse_count': self.brain.getTotalSynapseCount(),
            'brain_firing_count': self.brain.getFiringNeuronCount(),
            'brain_firing_rate': self.brain.getAverageFiringRate(),
            'brain_developmental_stage': self.brain.getDevelopmentalStage(),
            'agent_curiosity': self.agent.getCuriosityLevel(),
            'agent_novelty': self.agent.getNoveltyLevel(),
            'agent_prediction_error': self.agent.getPredictionError(),
            'agent_neuromodulation': self.agent.getNeuromodulationLevel(),
            'world_simulation_time': self.world.getSimulationTime(),
            'world_agent_position': (self.world.getAgentBody().x, self.world.getAgentBody().y)
        }
    
    def analyze_research_step(self, step: int, baseline: Dict[str, Any], 
                            percept, action, reward: float) -> Dict[str, Any]:
        """Analyze single simulation step for research purposes."""
        # Capture current state
        current_state = self.capture_research_baseline(step)
        
        # Calculate dynamics
        dynamics = {
            'firing_rate_change': current_state['brain_firing_rate'] - baseline['brain_firing_rate'],
            'curiosity_change': current_state['agent_curiosity'] - baseline['agent_curiosity'],
            'novelty_change': current_state['agent_novelty'] - baseline['agent_novelty'],
            'reward_change': reward - baseline.get('last_reward', 0.0)
        }
        
        # Detect potential errors
        errors = self.detect_research_errors(current_state, dynamics, step)
        
        # Store neural activity for later analysis
        self.neural_history.append({
            'step': step,
            'firing_rate': current_state['brain_firing_rate'],
            'firing_count': current_state['brain_firing_count'],
            'curiosity': current_state['agent_curiosity'],
            'novelty': current_state['agent_novelty']
        })
        
        return {
            'step': step,
            'timestamp': time.time(),
            'baseline': baseline,
            'current_state': current_state,
            'dynamics': dynamics,
            'errors': errors,
            'reward': reward,
            'action': action,
            'percept': percept
        }
    
    def detect_research_errors(self, current_state: Dict[str, Any], 
                              dynamics: Dict[str, Any], step: int) -> List[Dict[str, Any]]:
        """Detect potential errors and anomalies for research analysis."""
        errors = []
        
        # Neural activity anomalies
        firing_rate = current_state['brain_firing_rate']
        if firing_rate < 0.1 or firing_rate > 100:
            errors.append({
                'type': 'neural_activity_anomaly',
                'severity': 'high' if firing_rate < 0.01 or firing_rate > 50 else 'medium',
                'value': firing_rate,
                'threshold': '0.1 - 100 Hz'
            })
        
        # Learning dynamics anomalies
        curiosity = current_state['agent_curiosity']
        if curiosity < 0.01 and step > 100:
            errors.append({
                'type': 'learning_stagnation',
                'severity': 'medium',
                'value': curiosity,
                'threshold': '> 0.01'
            })
        
        # Neuromodulation anomalies
        neuromodulation = current_state['agent_neuromodulation']
        if neuromodulation < 0.0 or neuromodulation > 2.0:
            errors.append({
                'type': 'neuromodulation_anomaly',
                'severity': 'high',
                'value': neuromodulation,
                'threshold': '0.0 - 2.0'
            })
        
        return errors
    
    def run_detailed_analysis(self, step: int, interval: int):
        """Run detailed analysis at specified intervals."""
        print(f"  Running detailed analysis at step {step}...")
        
        # Time series analysis
        time_series_data = np.array(self.neural_history[-interval:])
        
        # Statistical analysis
        firing_rates = time_series_data[:, 1]
        curiosity_levels = time_series_data[:, 3]
        
        analysis_results = {
            'firing_rate_statistics': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'skew': stats.skew(firing_rates),
                'kurtosis': stats.kurtosis(firing_rates)
            },
            'curiosity_statistics': {
                'mean': np.mean(curiosity_levels),
                'std': np.std(curiosity_levels),
                'trend': self.calculate_trend(curiosity_levels)
            },
            'pattern_analysis': self.analyze_patterns(time_series_data),
            'change_point_detection': self.detect_change_points(firing_rates)
        }
        
        # Store analysis results
        if not hasattr(self, 'detailed_analyses'):
            self.detailed_analyses = []
        self.detailed_analyses.append({
            'step': step,
            'results': analysis_results
        })
    
    def calculate_trend(self, data: np.ndarray) -> Dict[str, float]:
        """Calculate linear trend of data."""
        if len(data) < 2:
            return {'slope': 0.0, 'intercept': 0.0, 'r_squared': 0.0}
        
        x = np.arange(len(data))
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, data)
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'p_value': p_value
        }
    
    def analyze_patterns(self, data: np.ndarray) -> Dict[str, Any]:
        """Analyze patterns in neural activity data."""
        patterns = {}
        
        # Detect oscillations
        if len(data) >= 10:
            autocorr = np.correlate(data[:, 1], data[:, 1], mode='full')
            autocorr = autocorr[len(autocorr)//2:]
            
            # Find dominant frequency
            freqs = np.fft.fftfreq(len(autocorr), d=1.0)
            fft_vals = np.fft.fft(autocorr)
            dominant_idx = np.argmax(np.abs(fft_vals[1:len(fft_vals)//2])) + 1
            
            patterns['dominant_frequency'] = freqs[dominant_idx]
            patterns['oscillation_amplitude'] = np.std(data[:, 1])
        
        # Burst detection
        bursts = self.detect_bursts(data[:, 1])
        patterns['burst_count'] = len(bursts)
        patterns['burst_duration'] = np.mean([b[1] - b[0] for b in bursts]) if bursts else 0
        
        return patterns
    
    def detect_bursts(self, firing_rates: np.ndarray, threshold: float = 2.0) -> List[Tuple[int, int]]:
        """Detect burst periods in firing rates."""
        bursts = []
        in_burst = False
        burst_start = 0
        
        for i, rate in enumerate(firing_rates):
            if rate > threshold and not in_burst:
                in_burst = True
                burst_start = i
            elif rate <= threshold and in_burst:
                in_burst = False
                bursts.append((burst_start, i))
        
        if in_burst:
            bursts.append((burst_start, len(firing_rates)))
        
        return bursts
    
    def detect_change_points(self, data: np.ndarray) -> Dict[str, Any]:n        """Detect significant change points in data."""
        if len(data) < 5:
            return {'change_points': [], 'significance': []}
        
        change_points = []
        significance = []
        
        # Use simple CUSUM algorithm for change point detection
        cumulative_sum = np.cumsum(data - np.mean(data))
        max_deviation = np.max(np.abs(cumulative_sum))
        
        # Find potential change points
        for i in range(1, len(data) - 1):
            if np.abs(cumulative_sum[i] - cumulative_sum[i-1]) > max_deviation * 0.1:
                change_points.append(i)
                significance.append(np.abs(cumulative_sum[i] - cumulative_sum[i-1]) / max_deviation)
        
        return {
            'change_points': change_points,
            'significance': significance
        }
    
    def calculate_research_reward(self, percept, action) -> float:
        """Calculate research-grade reward based on multiple factors."""
        # Base reward from action outcome
        base_reward = 1.0 if self.is_desirable_action(action) else -0.1
        
        # Exploration bonus
        exploration_bonus = self.agent.getCuriosityLevel() * 0.5
        
        # Novelty bonus
        novelty_bonus = self.agent.getNoveltyLevel() * 0.3
        
        # Learning bonus
        learning_bonus = min(self.agent.getPredictionError() * 2, 0.5)
        
        return base_reward + exploration_bonus + novelty_bonus + learning_bonus
    
    def is_desirable_action(self, action) -> bool:
        """Determine if action is desirable."""
        # Implement action desirability logic
        return True
    
    def generate_research_report(self) -> Dict[str, Any]:
        """Generate comprehensive research report."""
        if not hasattr(self, 'detailed_analyses'):
            self.detailed_analyses = []
        
        research_report = {
            'simulation_summary': {
                'total_steps': len(self.performance_history),
                'total_time': len(self.performance_history) * 0.1,  # Assuming 0.1s per step
                'neural_events': len(self.neural_history),
                'error_events': len(self.error_events)
            },
            'neural_dynamics': self.analyze_neural_dynamics(),
            'learning_curves': self.analyze_learning_curves(),
            'performance_metrics': self.analyze_performance_metrics(),
            'pattern_analysis': self.aggregate_pattern_analysis(),
            'statistical_significance': self.calculate_statistical_significance(),
            'recommendations': self.generate_research_recommendations()
        }
        
        return research_report
    
    def analyze_neural_dynamics(self) -> Dict[str, Any]:
        """Analyze neural dynamics over time."""
        if not self.neural_history:
            return {}
        
        firing_rates = [event['firing_rate'] for event in self.neural_history]
        curiosity_levels = [event['curiosity'] for event in self.neural_history]
        
        return {
            'firing_rate': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'min': np.min(firing_rates),
                'max': np.max(firing_rates),
                'cv': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
            },
            'curiosity': {
                'mean': np.mean(curiosity_levels),
                'std': np.std(curiosity_levels),
                'trend': self.calculate_trend(np.array(curiosity_levels))
            }
        }
    
    def analyze_learning_curves(self) -> Dict[str, Any]:
        """Analyze learning curves and reward progression."""
        rewards = [event['reward'] for event in self.performance_history]
        
        return {
            'reward_progression': {
                'total_reward': np.sum(rewards),
                'avg_reward': np.mean(rewards),
                'reward_trend': self.calculate_trend(np.array(rewards))
            },
            'convergence_analysis': self.analyze_convergence(rewards),
            ' exploration_metrics': self.calculate_exploration_metrics()
        }
    
    def analyze_performance_metrics(self) -> Dict[str, Any]:
        """Analyze performance metrics over time."""
        steps = [event['step'] for event in self.performance_history]
        processing_times = [event.get('processing_time', 0.1) for event in self.performance_history]
        
        return {
            'temporal_performance': {
                'avg_processing_time': np.mean(processing_times),
                'processing_time_std': np.std(processing_times),
                'throughput': len(steps) / np.sum(processing_times) if np.sum(processing_times) > 0 else 0
            },
            'efficiency_metrics': {
                'neural_efficiency': self.calculate_neural_efficiency(),
                'learning_efficiency': self.calculate_learning_efficiency()
            }
        }
    
    def aggregate_pattern_analysis(self) -> Dict[str, Any]:
        """Aggregate pattern analysis from all detailed analyses."""
        if not hasattr(self, 'detailed_analyses'):
            return {}
        
        patterns = []
        for analysis in self.detailed_analyses:
            patterns.append(analysis['results'])
        
        return {
            'oscillation_patterns': self.aggregate_oscillation_patterns(patterns),
            'burst_patterns': self.aggregate_burst_patterns(patterns),
            'change_point_patterns': self.aggregate_change_point_patterns(patterns)
        }
    
    def calculate_statistical_significance(self) -> Dict[str, Any]:
        """Calculate statistical significance of observed patterns."""
        if not self.neural_history:
            return {}
        
        firing_rates = np.array([event['firing_rate'] for event in self.neural_history])
        curiosity_levels = np.array([event['curiosity'] for event in self.neural_history])
        
        # Statistical tests
        independent_samples_t_test = stats.ttest_ind(firing_rates[:len(firing_rates)//2], 
                                                    firing_rates[len(firing_rates)//2:])
        
        return {
            'firing_rate_significance': {
                't_statistic': independent_samples_t_test.statistic,
                'p_value': independent_samples_t_test.pvalue,
                'significant': independent_samples_t_test.pvalue < 0.05
            },
            'correlation_analysis': {
                'firing_curiosity_correlation': np.corrcoef(firing_rates, curiosity_levels)[0, 1],
                'correlation_significance': self.calculate_correlation_significance(firing_rates, curiosity_levels)
            }
        }
    
    def generate_research_recommendations(self) -> List[str]:
        """Generate research recommendations based on analysis."""
        recommendations = []
        
        if self.neural_history:
            firing_rates = np.array([event['firing_rate'] for event in self.neural_history])
            avg_firing = np.mean(firing_rates)
            
            if avg_firing < 1.0:
                recommendations.append("Consider increasing neural excitability or reducing inhibition")
            elif avg_firing > 10.0:
                recommendations.append("Consider reducing neural excitability or increasing inhibition")
            
            curiosity_levels = np.array([event['curiosity'] for event in self.neural_history])
            avg_curiosity = np.mean(curiosity_levels)
            
            if avg_curiosity < 0.1:
                recommendations.append("Increase exploration drive through neuromodulation settings")
            elif avg_curiosity > 0.8:
                recommendations.append("Reduce exploration drive to stabilize learning")
        
        return recommendations
    
    def save_research_data(self, filename: str):
        """Save research data for further analysis."""
        research_data = {
            'neural_history': self.neural_history,
            'performance_history': self.performance_history,
            'error_events': self.error_events,
            'detailed_analyses': getattr(self, 'detailed_analyses', [])
        }
        
        import json
        with open(filename, 'w') as f:
            json.dump(research_data, f, indent=2, default=str)

# Usage example
if __name__ == "__main__":
    # Create research-grade neural system
    config = pynlm.create_preset_config("research")
    brain = pynlm.createBrain(config)
    brain.initialize_safe()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 10.0)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enable_all_systems()
    
    # Create research analyzer
    analyzer = ResearchNeuralAnalyzer(brain, agent, world)
    
    # Run research simulation
    research_report = analyzer.run_research_simulation(1000, analysis_interval=100)
    
    # Save research data
    analyzer.save_research_data("research_data.json")
    
    # Print research summary
    print("\n=== RESEARCH SIMULATION RESULTS ===")
    print(f"Total Steps: {research_report['simulation_summary']['total_steps']}")
    print(f"Neural Events: {research_report['simulation_summary']['neural_events']}")
    print(f"Error Events: {research_report['simulation_summary']['error_events']}")
    
    if 'neural_dynamics' in research_report:
        nn = research_report['neural_dynamics']
        print(f"Average Firing Rate: {nn['firing_rate']['mean']:.2f} ± {nn['firing_rate']['std']:.2f} Hz")
        print(f"Firing Rate Coefficient of Variation: {nn['firing_rate']['cv']:.3f}")
    
    print("\nResearch recommendations:")
    for rec in research_report.get('recommendations', []):
        print(f"  • {rec}")
```

## Advanced Example 3: Production-Ready Neural System with Monitoring

```python
import pynlm
import asyncio
import websockets
from typing import Dict, List, Callable
import logging

class ProductionNeuralSystem:
    """Production-ready neural system with comprehensive monitoring and scaling capabilities."""
    
    def __init__(self, config_name="advanced", enable_scaling=True):
        self.config_name = config_name
        self.enable_scaling = enable_scaling
        self.neural_systems = []
        self.performance_monitors = []
        self.error_handlers = []
        self.event_listeners = []
        self.websocket_servers = []
        
        # Initialize logging
        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger("production_neural_system")
        
        # Initialize core system
        self.initialize_production_system()
    
    def initialize_production_system(self):
        """Initialize production-grade neural system with monitoring."""
        print(f"Initializing production neural system with {self.config_name} configuration...")
        
        # Create main neural system
        main_system = self.create_neural_system("main", self.config_name)
        self.neural_systems.append(main_system)
        
        # Initialize performance monitoring
        performance_monitor = self.create_performance_monitor("main")
        self.performance_monitors.append(performance_monitor)
        
        # Initialize error handling
        error_handler = self.create_error_handler("main")
        self.error_handlers.append(error_handler)
        
        # Setup event listeners
        self.setup_event_listeners("main")
        
        # Initialize scaling if enabled
        if self.enable_scaling:
            self.setup_scaling_infrastructure()
        
        self.logger.info("Production neural system initialized successfully")
    
    def create_neural_system(self, name: str, config_name: str) -> Dict[str, Any]:
        """Create neural system with production-grade configuration."""
        print(f"  Creating neural system: {name}")
        
        # Create optimized configuration
        config = pynlm.create_preset_config(config_name)
        
        # Enhance configuration for production
        config->set("performance.optimization_level", 2)
        config->set("memory.checkpoint_interval", 100)
        config->set("monitoring.log_level", "info")
        config->set("error_handling.retry_attempts", 3)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(24, 24, 12, 12)
        world.setAgentStart(12.0, 12.0)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all production systems
        agent.enable_all_systems()
        
        # Initialize performance tracking
        performance_monitor = self.create_performance_monitor(name)
        performance_monitor.start()
        
        return {
            'name': name,
            'config': config,
            'brain': brain,
            'world': world,
            'agent': agent,
            'performance_monitor': performance_monitor,
            'status': 'active',
            'start_time': time.time(),
            'total_steps': 0
        }
    
    def create_performance_monitor(self, system_name: str) -> pynlm.PerformanceProfiler:
        """Create performance monitor for neural system."""
        monitor = pynlm.PerformanceProfiler(f"neural_system_{system_name}")
        
        # Configure monitoring targets
        monitor.set_monitoring_targets([
            'brain_step_time',
            'world_update_time',
            'agent_processing_time',
            'neural_activity',
            'memory_usage',
            'cpu_usage'
        ])
        
        # Set up alerts
        monitor.set_alert_thresholds({
            'max_brain_step_time': 0.1,  # seconds
            'max_world_update_time': 0.05,
            'min_neural_activity': 0.01,
            'max_memory_usage': 0.9
        })
        
        return monitor
    
    def create_error_handler(self, system_name: str) -> Callable[[Exception], None]:
        """Create error handler for neural system."""
        def handle_error(error: Exception):
            self.logger.error(f"Neural system {system_name} error: {error}")
            
            # Log error details
            error_info = {
                'timestamp': time.time(),
                'system_name': system_name,
                'error_type': type(error).__name__,
                'error_message': str(error),
                'stack_trace': traceback.format_exc()
            }
            
            # Store error for analysis
            if not hasattr(self, 'error_logs'):
                self.error_logs = []
            self.error_logs.append(error_info)
            
            # Send alerts if needed
            if isinstance(error, (pynlm.BrainInitializationError, pynlm.SimulationError)):
                self.send_alert(f"Critical error in neural system {system_name}", error_info)
        
        return handle_error
    
    def setup_event_listeners(self, system_name: str):
        """Setup event listeners for neural system."""
        listeners = []
        
        # Brain event listener
        def brain_event_listener(event):
            self.log_system_event(system_name, "brain_event", event)
        
        # Agent event listener
        def agent_event_listener(event):
            self.log_system_event(system_name, "agent_event", event)
        
        # World event listener
        def world_event_listener(event):
            self.log_system_event(system_name, "world_event", event)
        
        # Performance event listener
        def performance_event_listener(event):
            self.log_system_event(system_name, "performance_event", event)
        
        listeners.extend([
            brain_event_listener,
            agent_event_listener,
            world_event_listener,
            performance_event_listener
        ])
        
        self.event_listeners.append({
            'system_name': system_name,
            'listeners': listeners
        })
    
    def log_system_event(self, system_name: str, event_type: str, event: Any):
        """Log system event for monitoring."""
        event_log = {
            'timestamp': time.time(),
            'system_name': system_name,
            'event_type': event_type,
            'event': event
        }
        
        if not hasattr(self, 'system_events'):
            self.system_events = []
        self.system_events.append(event_log)
        
        self.logger.info(f"System event - {system_name}: {event_type}")
    
    def setup_scaling_infrastructure(self):
        """Setup infrastructure for scaling neural systems."""
        print("  Setting up scaling infrastructure...")
        
        # Create load balancer
        self.load_balancer = {
            'active_systems': [],
            'max_concurrent_systems': 10,
            'load_threshold': 0.8
        }
        
        # Initialize monitoring dashboards
        self.dashboards = {
            'system_performance': self.create_performance_dashboard(),
            'neural_activity': self.create_neural_activity_dashboard(),
            'error_analysis': self.create_error_analysis_dashboard()
        }
        
        # Setup auto-scaling
        self.auto_scaler = self.create_auto_scaler()
        
        self.logger.info("Scaling infrastructure setup complete")
    
    def create_performance_dashboard(self) -> Dict[str, Any]:
        """Create performance monitoring dashboard."""
        return {
            'metrics': [
                'cpu_usage',
                'memory_usage',
                'neural_activity',
                'processing_time',
                'throughput'
            ],
            'alerts': {
                'high_cpu': {'threshold': 0.8, 'severity': 'warning'},
                'high_memory': {'threshold': 0.9, 'severity': 'critical'},
                'low_neural_activity': {'threshold': 0.01, 'severity': 'warning'}
            }
        }
    
    def create_neural_activity_dashboard(self) -> Dict[str, Any]:
        """Create neural activity monitoring dashboard."""
        return {
            'tracking_targets': [
                'firing_rate',
                'firing_count',
                'curiosity_level',
                'novelty_level',
                'prediction_error'
            ],
            'visualizations': [
                'firing_rate_time_series',
                'curiosity_evolution',
                'activity_heatmap'
            ]
        }
    
    def create_error_analysis_dashboard(self) -> Dict[str, Any]:
        """Create error analysis monitoring dashboard."""
        return {
            'error_types': [
                'brain_initialization',
                'simulation_error',
                'neuron_operation',
                'configuration_error'
            ],
            'error_trends': {
                'daily': 'hourly',
                'weekly': 'daily',
                'monthly': 'weekly'
            },
            'root_cause_analysis': {
                'enabled': True,
                'auto_remediation': True
            }
        }
    
    def create_auto_scaler(self) -> Dict[str, Any]:
        """Create auto-scaling system."""
        return {
            'scaling_policies': {
                'high_load': 'scale_out',
                'low_load': 'scale_in',
                'maintenance': 'scale_none'
            },
            'scaling_triggers': {
                'cpu_threshold': 0.7,
                'memory_threshold': 0.8,
                'neural_activity_threshold': 0.1
            },
            'scaling_actions': {
                'scale_out': self.scale_out_system,
                'scale_in': self.scale_in_system,
                'scale_none': self.maintain_system
            }
        }
    
    def scale_out_system(self):
        """Scale out neural system for high load."""
        print("  Scaling out system...")
        
        if len(self.neural_systems) < 5:  # Max systems limit
            new_system = self.create_neural_system(f"scaled_{len(self.neural_systems)}", self.config_name)
            self.neural_systems.append(new_system)
            self.logger.info(f"Scaled out system {len(self.neural_systems)}")
    
    def scale_in_system(self):
        """Scale in neural system for low load."""
        print("  Scaling in system...")
        
        if len(self.neural_systems) > 1:  # Keep at least one system
            idle_system = self.find_idle_system()
            if idle_system:
                self.neural_systems.remove(idle_system)
                self.logger.info(f"Scaled in system {len(self.neural_systems)}")
    
    def find_idle_system(self) -> Dict[str, Any]:
        """Find idle neural system for scaling in."""
        for system in self.neural_systems:
            if system['status'] == 'idle' and system['total_steps'] < 100:
                return system
        return None
    
    def maintain_system(self):
        """Maintain neural system in normal operation."""
        print("  Maintaining system...")
        
        # Regular maintenance activities
        for system in self.neural_systems:
            system['performance_monitor'].analyze_bottlenecks()
            
            if system['total_steps'] % 1000 == 0:
                system['brain'].logStatus()
                self.logger.info(f"System {system['name']} maintenance completed")
    
    def run_production_simulation(self, num_steps: int) -> Dict[str, Any]:
        """Run production simulation with comprehensive monitoring."""
        print(f"Starting production simulation with {num_steps} steps...")
        
        start_time = time.time()
        simulation_start_time = time.time()
        
        # Run simulation with load balancing
        total_steps = 0
        for system in self.neural_systems:
            system_steps = self.run_system_simulation(system, num_steps)
            total_steps += system_steps
        
        total_time = time.time() - simulation_start_time
        
        # Generate production report
        production_report = {
            'simulation_summary': {
                'total_steps': total_steps,
                'total_time_seconds': total_time,
                'steps_per_second': total_steps / total_time,
                'systems_used': len(self.neural_systems),
                'simulation_efficiency': total_steps / (total_time * len(self.neural_systems))
            },
            'system_performance': self.generate_system_performance_report(),
            'load_balancing': self.generate_load_balancing_report(),
            'error_summary': self.generate_error_summary(),
            'scaling_actions': self.generate_scaling_actions(),
            'recommendations': self.generate_production_recommendations()
        }
        
        return production_report
    
    def run_system_simulation(self, system: Dict[str, Any], max_steps: int) -> int:
        """Run simulation for specific neural system."""
        steps_completed = 0
        
        for step in range(max_steps // len(self.neural_systems)):
            try:
                # Get performance monitor
                performance_monitor = system['performance_monitor']
                performance_monitor.start()
                
                # Run simulation step
                world = system['world']
                agent = system['agent']
                brain = system['brain']
                
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step, step * 0.001)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply neuromodulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.001)
                
                steps_completed = step + 1
                system['total_steps'] = steps_completed
                
                # Stop performance monitoring
                performance_monitor.stop()
                
            except Exception as e:
                # Handle error
                self.error_handlers[0](e)  # Use first error handler
                break
        
        return steps_completed
    
    def generate_system_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive system performance report."""
        report = {
            'system_metrics': [],
            'performance_trends': [],
            'bottleneck_analysis': [],
            'efficiency_metrics': []
        }
        
        for system in self.neural_systems:
            system_metrics = {
                'system_name': system['name'],
                'performance_monitor': system['performance_monitor'].get_report(),
                'status': system['status'],
                'total_steps': system['total_steps']
            }
            
            report['system_metrics'].append(system_metrics)
        
        return report
    
    def generate_load_balancing_report(self) -> Dict[str, Any]:
        """Generate load balancing report."""
        return {
            'load_distribution': {
                'system_names': [s['name'] for s in self.neural_systems],
                'total_load': len(self.neural_systems),
                'avg_load_per_system': len(self.neural_systems) / len(self.neural_systems) if self.neural_systems else 0
            },
            'balancing_efficiency': self.calculate_balancing_efficiency(),
            'scaling_events': self.get_scaling_events()
        }
    
    def generate_error_summary(self) -> Dict[str, Any]:
        """Generate error summary."""
        if not hasattr(self, 'error_logs'):
            return {'error_count': 0, 'error_types': []}
        
        error_types = {}
        for error_log in self.error_logs:
            error_type = error_log['error_type']
            if error_type not in error_types:
                error_types[error_type] = 0
            error_types[error_type] += 1
        
        return {
            'error_count': len(self.error_logs),
            'error_types': error_types,
            'error_rate': len(self.error_logs) / (sum(s['total_steps'] for s in self.neural_systems) if self.neural_systems else 1)
        }
    
    def generate_scaling_actions(self) -> List[str]:
        """Generate scaling actions taken."""
        # This would track scaling events
        return []
    
    def generate_production_recommendations(self) -> List[str]:
        """Generate production recommendations."""
        recommendations = []
        
        # Analyze system performance
        for system in self.neural_systems:
            if system['total_steps'] == 0:
                recommendations.append(f"System {system['name']} has not completed any steps")
            
            # Check for potential issues
            if 'performance_monitor' in system:
                monitor = system['performance_monitor']
                if monitor.is_running():
                    report = monitor.get_report()
                    if 'bottleneck' in report.lower():
                        recommendations.append(f"System {system['name']} may have performance bottlenecks")
        
        if len(self.neural_systems) == 1 and self.enable_scaling:
            recommendations.append("Consider enabling scaling for improved fault tolerance")
        
        return recommendations
    
    def calculate_balancing_efficiency(self) -> float:
        """Calculate load balancing efficiency."""
        if len(self.neural_systems) <= 1:
            return 1.0
        
        # Simple efficiency calculation
        loads = [s['total_steps'] for s in self.neural_systems]
        avg_load = sum(loads) / len(loads)
        
        # Calculate standard deviation as imbalance measure
        if avg_load > 0:
            std_load = (sum((load - avg_load) ** 2 for load in loads) / len(loads)) ** 0.5
            efficiency = max(0, 1 - std_load / avg_load)
            return efficiency
        
        return 0.0
    
    def get_scaling_events(self) -> List[Dict[str, Any]]:
        """Get scaling events history."""
        # This would track actual scaling events
        return []
    
    def save_production_results(self, filename: str, report: Dict[str, Any]):
        """Save production results to file."""
        import json
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2, default=str)

# Usage example for production system
if __name__ == "__main__":
    # Initialize production neural system
    production_system = ProductionNeuralSystem(
        config_name="research",
        enable_scaling=True
    )
    
    # Run production simulation
    production_report = production_system.run_production_simulation(10000)
    
    # Save production results
    production_system.save_production_results("production_report.json", production_report)
    
    # Print production summary
    print("\n=== PRODUCTION SIMULATION RESULTS ===")
    summary = production_report['simulation_summary']
    print(f"Total Steps: {summary['total_steps']}")
    print(f"Total Time: {summary['total_time_seconds']:.2f} seconds")
    print(f"Steps per Second: {summary['steps_per_second']:.2f}")
    print(f"Systems Used: {summary['systems_used']}")
    print(f"Simulation Efficiency: {summary['simulation_efficiency']:.2f}")
    
    # Print recommendations
    recommendations = production_report['recommendations']
    if recommendations:
        print("\nRecommendations:")
        for rec in recommendations:
            print(f"  • {rec}")
```

## Advanced Example 4: Research-Grade Neural Network Analysis Suite

```python
import pynlm
import numpy as np
import pandas as pd
from scipy import stats, signal
from sklearn.decomposition import PCA
from sklearn.cluster import DBSCAN
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Dict, List, Tuple, Any
import warnings
warnings.filterwarnings('ignore')

class NeuralNetworkAnalysisSuite:
    """Comprehensive analysis suite for neural network research."""
    
    def __init__(self, config_name="research"):
        self.config_name = config_name
        self.analysis_results = {}
        self.raw_data = {}
        self.processed_data = {}
        
        # Initialize analysis components
        self.setup_analysis_components()
    
    def setup_analysis_components(self):
        """Setup analysis components and tools."""
        self.analysis_tools = {
            'time_series_analysis': self.analyze_time_series,
            'pattern_detection': self.detect_patterns,
            'statistical_analysis': self.perform_statistical_analysis,
            'dimensionality_reduction': self.perform_dimensionality_reduction,
            'clustering_analysis': self.perform_clustering_analysis,
            'wavelet_analysis': self.perform_wavelet_analysis,
            'network_analysis': self.analyze_network_topology,
            'dynamic_analysis': self.analyze_dynamics,
            'synchronization_analysis': self.analyze_synchronization
        }
        
        self.data_processors = {
            'signal_processing': self.process_signals,
            'noise_reduction': self.reduce_noise,
            'feature_extraction': self.extract_features,
            'data_normalization': self.normalize_data,
            'data_augmentation': self.augment_data
        }
    
    def create_research_neural_system(self) -> Dict[str, Any]:
        """Create research-grade neural system."""
        # Create enhanced configuration for research
        config = pynlm.create_preset_config("research")
        
        # Add research-specific settings
        config->set("research.enable_detailed_logging", True)
        config->set("research.enable_noise_analysis", True)
        config->set("research.enable_pattern_detection", True)
        config->set("research.enable_dimensionality_reduction", True)
        config->set("research.enable_clustering", True)
        config->set("research.enable_synchronization_analysis", True)
        
        # Initialize neural system
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        # Create advanced environment
        world = pynlm.createSimpleWorld()
        world.configure(32, 32, 16, 16)
        world.setAgentStart(16.0, 16.0)
        world.reset()
        
        # Create agent with research capabilities
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable research-grade learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Enable advanced neuromodulation
        config->set("neuromod.dopamine.scale", 2.0)
        config->set("neuromod.curiosity.enable", True)
        config->set("neuromod.novelty.enable", True)
        config->set("neuromod.error.enable", True)
        
        return {
            'config': config,
            'brain': brain,
            'world': world,
            'agent': agent,
            'analysis_data': {
                'neural_activity': [],
                'rewards': [],
                'curiosity_levels': [],
                'novelty_levels': [],
                'prediction_errors': []
            }
        }
    
    def run_research_analysis(self, neural_system: Dict[str, Any], num_steps: int = 5000) -> Dict[str, Any]:
        """Run comprehensive research analysis."""
        print(f"Starting research analysis with {num_steps} steps...")
        
        brain = neural_system['brain']
        world = neural_system['world']
        agent = neural_system['agent']
        analysis_data = neural_system['analysis_data']
        
        # Run simulation with data collection
        for step in range(num_steps):
            # Record time series data
            time_step_data = self.collect_research_data(step, brain, world, agent)
            
            # Update analysis data
            for key, value in time_step_data.items():
                if key in analysis_data:
                    analysis_data[key].append(value)
            
            # Run periodic deep analysis
            if step % 500 == 0 and step > 0:
                self.run_deep_research_analysis(step, analysis_data)
            
            # Simulation step
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.001)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply research-grade neuromodulation
            reward = self.calculate_research_reward(percept, action)
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.001)
        
        # Generate comprehensive research analysis
        research_analysis = self.generate_comprehensive_research_analysis(analysis_data)
        
        return research_analysis
    
    def collect_research_data(self, step: int, brain, world, agent) -> Dict[str, Any]:
        """Collect research-grade data."""
        return {
            'step': step,
            'timestamp': time.time(),
            'firing_rate': brain.getAverageFiringRate(),
            'firing_count': brain.getFiringNeuronCount(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'developmental_stage': brain.getDevelopmentalStage(),
            'world_simulation_time': world.getSimulationTime(),
            'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health
        }
    
    def calculate_research_reward(self, percept, action) -> float:
        """Calculate research-grade reward."""
        # Multi-factor reward calculation
        base_reward = 1.0
        
        # Exploration-exploitation balance reward
        curiosity = self.agent.getCuriosityLevel()
        exploitation_bonus = (1 - curiosity) * 0.5
        
        # Novelty reward
        novelty = self.agent.getNoveltyLevel()
        novelty_bonus = novelty * 0.3
        
        # Learning efficiency reward
        prediction_error = self.agent.getPredictionError()
        learning_bonus = min(prediction_error * 2, 0.5)
        
        # Action quality reward
        action_quality = self.assess_action_quality(action)
        quality_bonus = action_quality * 0.2
        
        return base_reward + exploitation_bonus + novelty_bonus + learning_bonus + quality_bonus
    
    def assess_action_quality(self, action) -> float:
        """Assess action quality for research purposes."""
        # Implement action quality assessment
        return 1.0
    
    def run_deep_research_analysis(self, step: int, analysis_data: Dict[str, Any]):
        """Run deep research analysis."""
        print(f"  Running deep research analysis at step {step}...")
        
        # Prepare data for analysis
        analysis_data['processed'] = self.process_research_data(analysis_data)
        
        # Run various analyses
        self.analyze_neural_dynamics(analysis_data)
        self.analyze_learning_curves(analysis_data)
        self.analyze_connectivity(analysis_data)
        self.analyze_plasticity(analysis_data)
        self.analyze_information_processing(analysis_data)
        self.analyze_brain_topology(analysis_data)
    
    def process_research_data(self, raw_data: Dict[str, Any]) -> Dict[str, Any]:
        """Process research data with advanced techniques."""
        processed = {}
        
        # Extract neural activity features
        firing_rates = raw_data['firing_rate']
        curiosity_levels = raw_data['curiosity']
        
        processed['firing_rate_features'] = self.extract_firing_rate_features(firing_rates)
        processed['curiosity_features'] = self.extract_curiosity_features(curiosity_levels)
        
        # Signal processing
        processed['filtered_firing_rates'] = self.apply_lowpass_filter(firing_rates, cutoff=10.0)
        processed['derivative_curiosity'] = self.calculate_derivative(curiosity_levels)
        
        # Statistical features
        processed['statistical_features'] = {
            'firing_rate_skew': stats.skew(firing_rates),
            'firing_rate_kurtosis': stats.kurtosis(firing_rates),
            'curiosity_skew': stats.skew(curiosity_levels),
            'curiosity_kurtosis': stats.kurtosis(curiosity_levels)
        }
        
        return processed
    
    def extract_firing_rate_features(self, firing_rates: List[float]) -> Dict[str, float]:
        """Extract features from firing rate data."""
        if len(firing_rates) < 10:
            return {}
        
        features = {
            'mean_firing_rate': np.mean(firing_rates),
            'std_firing_rate': np.std(firing_rates),
            'cv_firing_rate': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0,
            'firing_rate_trend': self.calculate_linear_trend(firing_rates),
            'firing_rate_volatility': self.calculate_volatility(firing_rates)
        }
        
        return features
    
    def extract_curiosity_features(self, curiosity_levels: List[float]) -> Dict[str, float]:
        """Extract features from curiosity data."""
        if len(curiosity_levels) < 10:
            return {}
        
        features = {
            'mean_curiosity': np.mean(curiosity_levels),
            'std_curiosity': np.std(curiosity_levels),
            'curiosity_burst_frequency': self.detect_curiosity_bursts(curiosity_levels),
            'curiosity_decay_rate': self.calculate_curiosity_decay(curiosity_levels)
        }
        
        return features
    
    def apply_lowpass_filter(self, data: List[float], cutoff: float) -> List[float]:
        """Apply low-pass filter to data."""
        # Simple moving average filter
        window_size = int(1.0 / (cutoff / len(data))) if cutoff > 0 else 1
        window_size = max(1, min(window_size, len(data)))
        
        filtered = []
        for i in range(len(data)):
            start = max(0, i - window_size // 2)
            end = min(len(data), i + window_size // 2 + 1)
            filtered.append(np.mean(data[start:end]))
        
        return filtered
    
    def calculate_derivative(self, data: List[float]) -> List[float]:
        """Calculate derivative of data."""
        if len(data) < 2:
            return [0.0] * len(data)
        
        derivative = [0.0]
        for i in range(1, len(data)):
            derivative.append(data[i] - data[i-1])
        
        return derivative
    
    def calculate_linear_trend(self, data: List[float]) -> Dict[str, float]:
        """Calculate linear trend of data."""
        if len(data) < 2:
            return {'slope': 0.0, 'intercept': 0.0, 'r_squared': 0.0}
        
        x = np.arange(len(data))
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, data)
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'p_value': p_value
        }
    
    def calculate_volatility(self, data: List[float]) -> float:
        """Calculate volatility of data."""
        if len(data) < 2:
            return 0.0
        
        returns = np.diff(data)
        return np.std(returns)
    
    def detect_curiosity_bursts(self, curiosity_levels: List[float], threshold: float = 0.5) -> int:
        """Detect curiosity bursts."""
        bursts = 0
        in_burst = False
        
        for level in curiosity_levels:
            if level > threshold and not in_burst:
                in_burst = True
                bursts += 1
            elif level <= threshold and in_burst:
                in_burst = False
        
        return bursts
    
    def calculate_curiosity_decay(self, curiosity_levels: List[float]) -> float:
        """Calculate curiosity decay rate."""
        if len(curiosity_levels) < 10:
            return 0.0
        
        # Calculate average decay after peak
        peak_idx = np.argmax(curiosity_levels)
        if peak_idx < len(curiosity_levels) - 2:
            after_peak = curiosity_levels[peak_idx+1:]
            if len(after_peak) > 1:
                decay_rate = 1 - (after_peak[-1] / after_peak[0]) if after_peak[0] > 0 else 0
                return decay_rate / len(after_peak)
        
        return 0.0
    
    def analyze_neural_dynamics(self, analysis_data: Dict[str, Any]):
        """Analyze neural dynamics."""
        if 'processed' not in analysis_data:
            return
        
        dynamics = analysis_data['processed']
        
        # Analyze firing rate dynamics
        if 'firing_rate_features' in dynamics:
            firing_features = dynamics['firing_rate_features']
            print(f"  Firing Rate: Mean={firing_features['mean_firing_rate']:.2f} Hz, "
                  f"CV={firing_features['cv_firing_rate']:.3f}, "
                  f"Trend={firing_features['firing_rate_trend']['slope']:.6f}")
        
        # Analyze curiosity dynamics
        if 'curiosity_features' in dynamics:
            curiosity_features = dynamics['curiosity_features']
            print(f"  Curiosity: Mean={curiosity_features['mean_curiosity']:.3f}, "
                  f"Bursts={curiosity_features['curiosity_burst_frequency']}, "
                  f"Decay={curiosity_features['curiosity_decay_rate']:.3f}")
    
    def analyze_learning_curves(self, analysis_data: Dict[str, Any]):
        """Analyze learning curves."""
        # Implement learning curve analysis
        pass
    
    def analyze_connectivity(self, analysis_data: Dict[str, Any]):
        """Analyze network connectivity."""
        # Implement connectivity analysis
        pass
    
    def analyze_plasticity(self, analysis_data: Dict[str, Any]):
        """Analyze plasticity mechanisms."""
        # Implement plasticity analysis
        pass
    
    def analyze_information_processing(self, analysis_data: Dict[str, Any]):
        """Analyze information processing."""
        # Implement information processing analysis
        pass
    
    def analyze_brain_topology(self, analysis_data: Dict[str, Any]):
        """Analyze brain topology."""
        # Implement topology analysis
        pass
    
    def generate_comprehensive_research_analysis(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Generate comprehensive research analysis."""
        research_report = {
            'simulation_summary': {
                'total_steps': len(analysis_data['firing_rate']),
                'duration_seconds': len(analysis_data['firing_rate']) * 0.1,
                'final_firing_rate': analysis_data['firing_rate'][-1] if analysis_data['firing_rate'] else 0,
                'final_curiosity': analysis_data['curiosity'][-1] if analysis_data['curiosity'] else 0,
                'total_rewards': sum(analysis_data.get('rewards', [])),
                'avg_reward': np.mean(analysis_data.get('rewards', [0])) if analysis_data.get('rewards') else 0
            },
            'neural_analysis': self.analyze_neural_dynamics_comprehensive(analysis_data),
            'learning_analysis': self.analyze_learning_comprehensive(analysis_data),
            'performance_analysis': self.analyze_performance_comprehensive(analysis_data),
            'pattern_recognition': self.recognize_patterns_research(analysis_data),
            'statistical_significance': self.calculate_research_statistics(analysis_data),
            'model_validation': self.validate_research_model(analysis_data),
            'research_insights': self.generate_research_insights(analysis_data)
        }
        
        return research_report
    
    def analyze_neural_dynamics_comprehensive(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Comprehensive neural dynamics analysis."""
        firing_rates = analysis_data['firing_rate']
        curiosity_levels = analysis_data['curiosity']
        
        return {
            'firing_dynamics': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'cv': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0,
                'trend': self.calculate_linear_trend(firing_rates),
                'stationarity': self.test_stationarity(firing_rates),
                'autocorrelation': self.calculate_autocorrelation(firing_rates)
            },
            'curiosity_dynamics': {
                'mean': np.mean(curiosity_levels),
                'std': np.std(curiosity_levels),
                'trend': self.calculate_linear_trend(curiosity_levels),
                'bursts': self.detect_curiosity_bursts(curiosity_levels),
                'decay': self.calculate_curiosity_decay(curiosity_levels)
            },
            'cross_dynamics': {
                'correlation': np.corrcoef(firing_rates, curiosity_levels)[0, 1],
                'phase_relationship': self.analyze_phase_relationship(firing_rates, curiosity_levels)
            }
        }
    
    def analyze_learning_comprehensive(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Comprehensive learning analysis."""
        rewards = analysis_data.get('rewards', [])
        
        return {
            'reward_dynamics': {
                'mean': np.mean(rewards) if rewards else 0,
                'std': np.std(rewards) if rewards else 0,
                'trend': self.calculate_linear_trend(rewards) if rewards else {},
                'convergence': self.analyze_convergence(rewards) if rewards else {}
            },
            'learning_efficiency': self.calculate_learning_efficiency(analysis_data),
            'exploration_exploitation': self.analyze_exploration_exploitation(analysis_data)
        }
    
    def analyze_performance_comprehensive(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Comprehensive performance analysis."""
        # Performance metrics analysis
        return {
            'stability_metrics': {
                'firing_rate_stability': self.calculate_stability(analysis_data['firing_rate']),
                'curiosity_stability': self.calculate_stability(analysis_data['curiosity'])
            },
            'efficiency_metrics': {
                'information_efficiency': self.calculate_information_efficiency(analysis_data),
                'adaptation_speed': self.calculate_adaptation_speed(analysis_data)
            }
        }
    
    def recognize_patterns_research(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Recognize patterns in research data."""
        firing_rates = analysis_data['firing_rate']
        
        # Pattern recognition using advanced techniques
        patterns = {
            'oscillations': self.detect_oscillations(firing_rates),
            'bursts': self.detect_firing_bursts(firing_rates),
            'transients': self.detect_transients(firing_rates),
            'regime_changes': self.detect_regime_changes(firing_rates)
        }
        
        return patterns
    
    def calculate_research_statistics(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate statistical significance for research."""
        firing_rates = analysis_data['firing_rate']
        curiosity_levels = analysis_data['curiosity']
        
        if len(firing_rates) < 2:
            return {'insufficient_data': True}
        
        # Statistical tests
        half_point = len(firing_rates) // 2
        first_half = firing_rates[:half_point]
        second_half = firing_rates[half_point:]
        
        t_statistic, p_value = stats.ttest_ind(first_half, second_half)
        
        return {
            'stationarity_test': {
                'statistic': t_statistic,
                'p_value': p_value,
                'significant_change': p_value < 0.05
            },
            'correlation_significance': {
                'correlation': np.corrcoef(firing_rates, curiosity_levels)[0, 1],
                'p_value': stats.pearsonr(firing_rates, curiosity_levels)[1]
            }
        }
    
    def validate_research_model(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Validate research model."""
        # Model validation checks
        return {
            'data_quality': self.assess_data_quality(analysis_data),
            'model_fit': self.assess_model_fit(analysis_data),
            'predictive_power': self.assess_predictive_power(analysis_data)
        }
    
    def generate_research_insights(self, analysis_data: Dict[str, Any]) -> List[str]:
        """Generate research insights."""
        insights = []
        
        firing_rates = analysis_data['firing_rate']
        curiosity_levels = analysis_data['curiosity']
        
        # Generate insights based on analysis
        avg_firing_rate = np.mean(firing_rates)
        avg_curiosity = np.mean(curiosity_levels)
        
        if avg_firing_rate < 1.0:
            insights.append("Low firing rates suggest insufficient neural activation - consider adjusting network parameters")
        elif avg_firing_rate > 50.0:
            insights.append("High firing rates suggest potential instability - consider reducing neural excitability")
        
        if avg_curiosity < 0.1:
            insights.append("Low curiosity levels indicate reduced exploration - consider increasing neuromodulation")
        elif avg_curiosity > 0.8:
            insights.append("High curiosity levels suggest excessive exploration - consider adjusting exploration-exploitation balance")
        
        # Detect learning plateaus
        if len(firing_rates) > 10:
            recent_trend = np.mean(firing_rates[-5:]) - np.mean(firing_rates[-10:-5])
            if abs(recent_trend) < 0.01:
                insights.append("Learning plateau detected - consider introducing novelty or increasing exploration")
        
        return insights
    
    def save_research_results(self, filename: str, research_report: Dict[str, Any]):
        """Save research results."""
        import json
        with open(filename, 'w') as f:
            json.dump(research_report, f, indent=2, default=str)

# Usage example for research analysis
if __name__ == "__main__":
    # Initialize research analysis suite
    analysis_suite = NeuralNetworkAnalysisSuite(config_name="research")
    
    # Create research neural system
    neural_system = analysis_suite.create_research_neural_system()
    
    # Run research analysis
    research_report = analysis_suite.run_research_analysis(neural_system, num_steps=10000)
    
    # Save research results
    analysis_suite.save_research_results("research_report.json", research_report)
    
    # Print research summary
    print("\n=== RESEARCH ANALYSIS RESULTS ===")
    
    summary = research_report['simulation_summary']
    print(f"Total Steps: {summary['total_steps']}")
    print(f"Duration: {summary['duration_seconds']:.2f} seconds")
    print(f"Final Firing Rate: {summary['final_firing_rate']:.2f} Hz")
    print(f"Final Curiosity: {summary['final_curiosity']:.3f}")
    print(f"Total Rewards: {summary['total_rewards']:.2f}")
    print(f"Average Reward: {summary['avg_reward']:.3f}")
    
    # Print key insights
    insights = research_report.get('research_insights', [])
    if insights:
        print("\nKey Research Insights:")
        for insight in insights:
            print(f"  • {insight}")
    
    # Print statistical significance
    stats = research_report.get('statistical_significance', {})
    if 'stationarity_test' in stats:
        stationarity = stats['stationarity_test']
        print(f"\nStatistical Significance:")
        print(f"  Stationarity Test: p-value = {stationarity['p_value']:.4f}")
        print(f"  Significant Change: {stationarity['significant_change']}")
```

## Advanced Example 5: Neural Network Optimization Suite

```python
import pynlm
import numpy as np
from scipy.optimize import minimize, differential_evolution
from typing import Dict, List, Tuple, Any
import warnings
warnings.filterwarnings('ignore')

class NeuralNetworkOptimizationSuite:
    """Comprehensive neural network optimization suite."""
    
    def __init__(self):
        self.optimization_history = []
        self.convergence_data = []
        self.optimization_results = {}
        
        # Initialize optimizers
        self.setup_optimizers()
    
    def setup_optimizers(self):
        """Setup various optimization algorithms."""
        self.optimizers = {
            'gradient_descent': self.gradient_descent_optimization,
            'local_search': self.local_search_optimization,
            'differential_evolution': self.differential_evolution_optimization,
            'genetic_algorithm': self.genetic_algorithm_optimization,
            'bayesian_optimization': self.bayesian_optimization,
            'simulated_annealing': self.simulated_annealing_optimization
        }
    
    def create_optimization_objective(self, neural_system: Dict[str, Any]) -> Callable:
        """Create optimization objective function."""
        def objective_function(params: np.ndarray) -> float:
            # Apply parameters to neural system
            self.apply_optimization_parameters(neural_system, params)
            
            # Run simulation
            brain = neural_system['brain']
            world = neural_system['world']
            agent = neural_system['agent']
            
            # Collect performance metrics
            performance_metrics = self.evaluate_neural_performance(brain, world, agent)
            
            # Calculate optimization score (maximize performance)
            score = self.calculate_optimization_score(performance_metrics)
            
            # Record optimization history
            self.optimization_history.append({
                'params': params.copy(),
                'score': score,
                'metrics': performance_metrics,
                'step': len(self.optimization_history)
            })
            
            return -score  # Minimize negative score (maximize actual score)
        
        return objective_function
    
    def evaluate_neural_performance(self, brain, world, agent) -> Dict[str, float]:
        """Evaluate neural system performance."""
        # Run test simulation
        test_steps = 200
        
        total_reward = 0
        firing_rate_sum = 0
        curiosity_sum = 0
        novelty_sum = 0
        prediction_error_sum = 0
        
        for step in range(test_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step, step * 0.001)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            total_reward += reward
            
            # Collect metrics
            firing_rate_sum += brain.getAverageFiringRate()
            curiosity_sum += agent.getCuriosityLevel()
            novelty_sum += agent.getNoveltyLevel()
            prediction_error_sum += agent.getPredictionError()
        
        # Calculate averages
        metrics = {
            'average_reward': total_reward / test_steps,
            'average_firing_rate': firing_rate_sum / test_steps,
            'average_curiosity': curiosity_sum / test_steps,
            'average_novelty': novelty_sum / test_steps,
            'average_prediction_error': prediction_error_sum / test_steps,
            'reward_variance': self.calculate_variance([0] * test_steps),  # Simplified
            'stability_score': self.calculate_stability_score(brain.getAverageFiringRate(), brain.getFiringNeuronCount())
        }
        
        return metrics
    
    def calculate_optimization_score(self, metrics: Dict[str, float]) -> float:
        """Calculate overall optimization score."""
        # Weighted sum of performance metrics
        weights = {
            'average_reward': 0.3,
            'average_firing_rate': 0.2,
            'average_curiosity': 0.15,
            'average_novelty': 0.1,
            'stability_score': 0.25
        }
        
        score = 0
        for metric, weight in weights.items():
            if metric in metrics:
                score += weight * self.normalize_metric(metric, metrics[metric])
        
        return score
    
    def normalize_metric(self, metric_name: str, value: float) -> float:
        """Normalize metric to [0, 1] range."""
        # Metric-specific normalization
        if metric_name == 'average_reward':
            return min(value / 10.0, 1.0)  # Assume max reward of 10
        elif metric_name == 'average_firing_rate':
            return min(value / 50.0, 1.0)  # Assume max firing rate of 50 Hz
        elif metric_name == 'average_curiosity':
            return value  # Curiosity is naturally in [0, 1]
        elif metric_name == 'average_novelty':
            return value  # Novelty is naturally in [0, 1]
        elif metric_name == 'stability_score':
            return value  # Stability score is naturally in [0, 1]
        
        return 0.5  # Default
    
    def gradient_descent_optimization(self, objective_function, initial_params: np.ndarray, 
                                    bounds: List[Tuple[float, float]], iterations: int = 1000) -> Dict[str, Any]:
        """Perform gradient descent optimization."""
        print("Running gradient descent optimization...")
        
        current_params = initial_params.copy()
        best_params = current_params.copy()
        best_score = np.inf
        
        for iteration in range(iterations):
            # Calculate gradient using finite differences
            gradient = np.zeros_like(current_params)
            epsilon = 1e-6
            
            for i in range(len(current_params)):
                params_plus = current_params.copy()
                params_plus[i] += epsilon
                
                params_minus = current_params.copy()
                params_minus[i] -= epsilon
                
                score_plus = objective_function(params_plus)
                score_minus = objective_function(params_minus)
                
                gradient[i] = (score_plus - score_minus) / (2 * epsilon)
            
            # Update parameters
            learning_rate = 0.01
            current_params -= learning_rate * gradient
            
            # Apply bounds
            for i, (lower, upper) in enumerate(bounds):
                current_params[i] = np.clip(current_params[i], lower, upper)
            
            # Evaluate current parameters
            current_score = objective_function(current_params)
            
            if current_score < best_score:
                best_score = current_score
                best_params = current_params.copy()
        
        return {
            'optimized_params': best_params,
            'best_score': best_score,
            'convergence_data': self.optimization_history[-len(self.optimization_history)//2:],
            'optimization_type': 'gradient_descent'
        }
    
    def local_search_optimization(self, objective_function, initial_params: np.ndarray,
                                 radius: float = 1.0, iterations: int = 1000) -> Dict[str, Any]:
        """Perform local search optimization."""
        print("Running local search optimization...")
        
        current_params = initial_params.copy()
        current_score = objective_function(current_params)
        best_params = current_params.copy()
        best_score = current_score
        
        for iteration in range(iterations):
            # Generate random neighbor
            neighbor_params = current_params + np.random.uniform(-radius, radius, len(current_params))
            
            # Apply bounds (assuming [0, 1] bounds for parameters)
            neighbor_params = np.clip(neighbor_params, 0, 1)
            
            # Evaluate neighbor
            neighbor_score = objective_function(neighbor_params)
            
            # Accept or reject based on score
            if neighbor_score < current_score:  # Assuming minimization
                current_params = neighbor_params
                current_score = neighbor_score
                
                if current_score < best_score:
                    best_score = current_score
                    best_params = current_params.copy()
            
            # Gradually reduce radius
            radius *= (1 - iteration / iterations)
        
        return {
            'optimized_params': best_params,
            'best_score': best_score,
            'convergence_data': self.optimization_history[-len(self.optimization_history)//2:],
            'optimization_type': 'local_search'
        }
    
    def differential_evolution_optimization(self, objective_function, bounds: List[Tuple[float, float]],
                                         population_size: int = 50, generations: int = 100) -> Dict[str, Any]:
        """Perform differential evolution optimization."""
        print("Running differential evolution optimization...")
        
        # Initialize population
        population = np.random.uniform([b[0] for b in bounds], [b[1] for b in bounds], (population_size, len(bounds)))
        fitness = np.array([objective_function(ind) for ind in population])
        
        for generation in range(generations):
            # Generate new population
            new_population = []
            new_fitness = []
            
            for i in range(population_size):
                # Select three random individuals
                idxs = np.random.choice(population_size, 3, replace=False)
                a, b, c = population[idxs]
                
                # Differential evolution mutation
                mutation_factor = np.random.uniform(0.5, 1.0)
                crossover_rate = np.random.uniform(0.5, 0.9)
                
                # Mutation
                mutant = a + mutation_factor * (b - c)
                
                # Crossover
                trial = np.zeros_like(current_params)
                for j in range(len(current_params)):
                    if np.random.uniform() < crossover_rate:
                        trial[j] = mutant[j]
                    else:
                        trial[j] = population[i][j]
                
                # Apply bounds
                trial = np.clip(trial, [b[0] for b in bounds], [b[1] for b in bounds])
                
                # Selection
                trial_fitness = objective_function(trial)
                if trial_fitness < fitness[i]:  # Assuming minimization
                    new_population.append(trial)
                    new_fitness.append(trial_fitness)
                else:
                    new_population.append(population[i])
                    new_fitness.append(fitness[i])
            
            population = np.array(new_population)
            fitness = np.array(new_fitness)
        
        # Find best individual
        best_idx = np.argmin(fitness)
        best_params = population[best_idx]
        best_score = fitness[best_idx]
        
        return {
            'optimized_params': best_params,
            'best_score': best_score,
            'convergence_data': self.optimization_history[-len(self.optimization_history)//2:],
            'optimization_type': 'differential_evolution'
        }
    
    def genetic_algorithm_optimization(self, objective_function, bounds: List[Tuple[float, float]],
                                      population_size: int = 100, generations: int = 200) -> Dict[str, Any]:
        """Perform genetic algorithm optimization."""
        print("Running genetic algorithm optimization...")
        
        # Initialize population
        population = np.random.uniform([b[0] for b in bounds], [b[1] for b in bounds], (population_size, len(bounds)))
        fitness = np.array([objective_function(ind) for ind in population])
        
        for generation in range(generations):
            # Selection
            selected_parents = self.select_parents(population, fitness, tournament_size=3)
            
            # Crossover
            offspring = self.crossover(selected_parents, crossover_rate=0.8)
            
            # Mutation
            offspring = self.mutate(offspring, mutation_rate=0.1)
            
            # Evaluate offspring
            offspring_fitness = np.array([objective_function(ind) for ind in offspring])
            
            # Replacement
            population = np.vstack([population, offspring])
            fitness = np.hstack([fitness, offspring_fitness])
            
            # Keep best individuals
            elite_size = population_size // 2
            elite_indices = np.argsort(fitness)[:elite_size]
            population = population[elite_indices]
            fitness = fitness[elite_indices]
        
        # Find best individual
        best_idx = np.argmin(fitness)
        best_params = population[best_idx]
        best_score = fitness[best_idx]
        
        return {
            'optimized_params': best_params,
            'best_score': best_score,
            'convergence_data': self.optimization_history[-len(self.optimization_history)//2:],
            'optimization_type': 'genetic_algorithm'
        }
    
    def select_parents(self, population: np.ndarray, fitness: np.ndarray, tournament_size: int = 3) -> np.ndarray:
        """Select parents using tournament selection."""
        parents = []
        
        for _ in range(len(population)):
            # Random tournament
            tournament_indices = np.random.choice(len(population), tournament_size, replace=False)
            tournament_fitness = fitness[tournament_indices]
            
            # Select best from tournament
            best_idx = tournament_indices[np.argmin(tournament_fitness)]
            parents.append(population[best_idx])
        
        return np.array(parents)
    
    def crossover(self, parents: np.ndarray, crossover_rate: float = 0.8) -> np.ndarray:
        """Perform crossover to create offspring."""
        offspring = []
        
        for i in range(0, len(parents), 2):
            if i + 1 < len(parents):
                parent1, parent2 = parents[i], parents[i + 1]
                
                # Two-point crossover
                crossover_points = np.random.choice(len(parent1) - 1, 2, replace=False)
                crossover_points.sort()
                
                child1 = np.zeros_like(parent1)
                child2 = np.zeros_like(parent2)
                
                for j in range(len(parent1)):
                    if crossover_points[0] <= j <= crossover_points[1]:
                        child1[j] = parent2[j]
                        child2[j] = parent1[j]
                    else:
                        child1[j] = parent1[j]
                        child2[j] = parent2[j]
                
                offspring.extend([child1, child2])
        
        return np.array(offspring)
    
    def mutate(self, population: np.ndarray, mutation_rate: float = 0.1) -> np.ndarray:
        """Apply mutation to population."""
        mutated = []
        
        for individual in population:
            mutated_individual = individual.copy()
            
            for i in range(len(mutated_individual)):
                if np.random.uniform() < mutation_rate:
                    # Gaussian mutation
                    mutation_strength = np.random.normal(0, 0.1)
                    mutated_individual[i] += mutation_strength
                    
                    # Apply bounds
                    mutated_individual[i] = np.clip(mutated_individual[i], 0, 1)
            
            mutated.append(mutated_individual)
        
        return np.array(mutated)
    
    def apply_optimization_parameters(self, neural_system: Dict[str, Any], params: np.ndarray):
        """Apply optimization parameters to neural system."""
        # Map optimization parameters to neural system configuration
        # This is application-specific and depends on what parameters are being optimized
        
        config = neural_system['config']
        
        # Example parameter mappings
        if len(params) >= 1:
            config.set("brain.neuron_count", int(params[0] * 10000))  # Scale to neuron count
        if len(params) >= 2:
            config.set("region_count", int(params[1] * 8))  # Scale to region count
        if len(params) >= 3:
            config->set("connection_probability", params[2] * 0.5)  # Scale to connection probability
        if len(params) >= 4:
            config->set("plasticity.stdp.learning_rate", params[3] * 0.01)  # Scale to learning rate
        
        # Reinitialize system with new parameters
        brain = neural_system['brain']
        world = neural_system['world']
        agent = neural_system['agent']
        
        # Reset and reinitialize (simplified)
        if hasattr(brain, 'reset'):
            brain.reset()
        if hasattr(brain, 'initialize'):
            brain.initialize()
    
    def calculate_variance(self, values: List[float]) -> float:
        """Calculate variance of values."""
        if len(values) < 2:
            return 0.0
        
        mean = np.mean(values)
        variance = np.mean([(x - mean) ** 2 for x in values])
        return variance
    
    def calculate_stability_score(self, average_firing_rate: float, firing_neuron_count: int) -> float:
        """Calculate stability score based on firing dynamics."""
        # Simplified stability calculation
        stability = 1.0 / (1.0 + abs(average_firing_rate - 10.0) / 10.0)  # Prefer firing rate around 10 Hz
        stability *= min(firing_neuron_count / 1000.0, 1.0)  # Prefer moderate firing counts
        return stability
    
    def run_optimization_suite(self, neural_system: Dict[str, Any], optimization_types: List[str],
                              bounds: List[Tuple[float, float]], initial_params: np.ndarray) -> Dict[str, Any]:
        """Run complete optimization suite."""
        print("Starting comprehensive neural network optimization...")
        
        optimization_results = {}
        
        # Create objective function
        objective_function = self.create_optimization_objective(neural_system)
        
        # Run each optimization type
        for opt_type in optimization_types:
            if opt_type in self.optimizers:
                print(f"\nRunning {opt_type} optimization...")
                
                try:
                    result = self.optimizers[opt_type](objective_function, initial_params, bounds)
                    optimization_results[opt_type] = result
                    
                    print(f"  {opt_type} completed with score: {result['best_score']:.4f}")
                    
                except Exception as e:
                    print(f"  {opt_type} failed: {e}")
                    optimization_results[opt_type] = {'error': str(e)}
        
        # Find best result
        successful_results = {k: v for k, v in optimization_results.items() 
                            if 'error' not in v and 'best_score' in v}
        
        if successful_results:
            best_opt_type = min(successful_results.keys(), 
                              key=lambda k: successful_results[k]['best_score'])
            best_result = successful_results[best_opt_type]
            
            optimization_results['best_overall'] = {
                'optimization_type': best_opt_type,
                'optimized_params': best_result['optimized_params'],
                'best_score': best_result['best_score']
            }
            
            print(f"\nBest overall optimization: {best_opt_type}")
            print(f"Score: {best_result['best_score']:.4f}")
        
        return optimization_results

# Usage example for optimization suite
if __name__ == "__main__":
    # Initialize optimization suite
    optimization_suite = NeuralNetworkOptimizationSuite()
    
    # Create neural system for optimization
    neural_system = {
        'config': pynlm.create_preset_config("research"),
        'brain': pynlm.createBrain(optimization_suite.optimization_suite['config']),
        'world': pynlm.createSimpleWorld(),
        'agent': pynlm.createAgentBrain(optimization_suite.optimization_suite['brain'])
    }
    
    # Initialize neural system
    neural_system['brain'].initialize_safe()
    neural_system['world'].configure(20, 20, 8, 8)
    neural_system['world'].setAgentStart(10.0, 10.0)
    neural_system['world'].reset()
    neural_system['agent'].initialize(neural_system['world'])
    neural_system['agent'].enable_all_systems()
    
    # Define optimization bounds
    bounds = [(0.1, 1.0), (0.5, 2.0), (0.01, 0.5), (0.0001, 0.1)]  # neuron count, regions, connection prob, learning rate
    
    # Initial parameters
    initial_params = np.array([0.5, 1.0, 0.2, 0.005])
    
    # Run optimization suite
    optimization_results = optimization_suite.run_optimization_suite(
        neural_system,
        ['gradient_descent', 'local_search', 'differential_evolution'],
        bounds,
        initial_params
    )
    
    # Display results
    print("\n=== OPTIMIZATION RESULTS ===")
    
    for opt_type, result in optimization_results.items():
        if 'error' in result:
            print(f"{opt_type}: Failed - {result['error']}")
        elif 'best_score' in result:
            print(f"{opt_type}:")
            print(f"  Score: {result['best_score']:.4f}")
            if opt_type == 'best_overall':
                print(f"  Optimized Parameters:")
                for i, param in enumerate(result['optimized_params']):
                    print(f"    Param {i}: {param:.4f}")
    
    # Save optimization results
    import json
    with open("optimization_results.json", 'w') as f:
        json.dump(optimization_results, f, indent=2, default=str)
```

## Advanced Example 6: Real-Time Neural Network Monitoring and Control

```python
import pynlm
import asyncio
import websockets
import threading
import time
from typing import Dict, List, Any
import numpy as np
import json
from datetime import datetime

class RealTimeNeuralMonitor:
    """Real-time monitoring and control system for neural networks."""
    
    def __init__(self, neural_system: Dict[str, Any], host="localhost", port=8765):
        self.neural_system = neural_system
        self.host = host
        self.port = port
        self.monitoring_data = {}
        self.control_commands = []
        self.alerts = []
        self.websocket_server = None
        self.monitoring_thread = None
        self.is_monitoring = False
        
        # Initialize monitoring components
        self.setup_monitoring_components()
    
    def setup_monitoring_components(self):
        """Setup monitoring and control components."""
        self.monitoring_config = {
            'sampling_rate': 10,  # Hz
            'data_retention': 3600,  # seconds
            'alert_thresholds': {
                'max_firing_rate': 50.0,
                'min_firing_rate': 0.1,
                'max_curiosity': 1.0,
                'min_curiosity': 0.0,
                'max_prediction_error': 1.0,
                'max_neuromodulation': 2.0,
                'temperature_threshold': 80.0,  # Celsius
                'memory_threshold': 0.9
            },
            'control_policies': {
                'auto_rewinding': False,
                'curiosity_adjustment': True,
                'learning_rate_adaptation': True,
                'exploration_boost': False
            }
        }
        
        # Initialize performance tracking
        self.performance_tracker = {
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'neuron_activity': []
        }
        
        # Initialize state tracking
        self.state_history = {
            'brain_states': [],
            'agent_states': [],
            'world_states': []
        }
    
    async def start_monitoring(self):
        """Start real-time monitoring."""
        print(f"Starting real-time monitoring on ws://{self.host}:{self.port}")
        
        self.is_monitoring = True
        
        # Start background monitoring thread
        self.monitoring_thread = threading.Thread(target=self.background_monitoring, daemon=True)
        self.monitoring_thread.start()
        
        # Start WebSocket server
        self.websocket_server = await websockets.serve(self.handle_websocket_connection, self.host, self.port)
        
        print("Real-time monitoring started successfully")
    
    def background_monitoring(self):
        """Background monitoring loop."""
        while self.is_monitoring:
            try:
                # Collect monitoring data
                self.collect_monitoring_data()
                
                # Check for alerts
                self.check_for_alerts()
                
                # Apply control policies
                self.apply_control_policies()
                
                # Store state
                self.store_state()
                
                # Sleep for monitoring interval
                time.sleep(1.0 / self.monitoring_config['sampling_rate'])
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(1.0)
    
    def collect_monitoring_data(self):
        """Collect real-time monitoring data."""
        brain = self.neural_system['brain']
        agent = self.neural_system['agent']
        world = self.neural_system['world']
        
        # Current state snapshot
        current_state = {
            'timestamp': time.time(),
            'brain': {
                'firing_rate': brain.getAverageFiringRate(),
                'firing_count': brain.getFiringNeuronCount(),
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'developmental_stage': brain.getDevelopmentalStage(),
                'neuromodulation_level': agent.getNeuromodulationLevel()
            },
            'agent': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'systems_status': self.get_system_status(agent)
            },
            'world': {
                'simulation_time': world.getSimulationTime(),
                'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
                'agent_energy': world.getAgentBody().energy,
                'agent_health': world.getAgentBody().health
            },
            'performance': self.get_performance_metrics()
        }
        
        # Store in monitoring data
        if not hasattr(self, 'monitoring_history'):
            self.monitoring_history = []
        
        self.monitoring_history.append(current_state)
        
        # Keep only recent data
        max_history = self.monitoring_config['data_retention'] * self.monitoring_config['sampling_rate']
        if len(self.monitoring_history) > max_history:
            self.monitoring_history = self.monitoring_history[-max_history:]
    
    def check_for_alerts(self):
        """Check for alerts and warnings."""
        if not hasattr(self, 'monitoring_history') or len(self.monitoring_history) == 0:
            return
        
        current_state = self.monitoring_history[-1]
        
        # Check brain metrics
        brain = current_state['brain']
        if brain['firing_rate'] > self.monitoring_config['alert_thresholds']['max_firing_rate']:
            self.trigger_alert('high_firing_rate', {
                'value': brain['firing_rate'],
                'threshold': self.monitoring_config['alert_thresholds']['max_firing_rate']
            })
        
        if brain['firing_rate'] < self.monitoring_config['alert_thresholds']['min_firing_rate']:
            self.trigger_alert('low_firing_rate', {
                'value': brain['firing_rate'],
                'threshold': self.monitoring_config['alert_thresholds']['min_firing_rate']
            })
        
        # Check agent metrics
        agent = current_state['agent']
        if agent['curiosity'] > self.monitoring_config['alert_thresholds']['max_curiosity']:
            self.trigger_alert('high_curiosity', {
                'value': agent['curiosity'],
                'threshold': self.monitoring_config['alert_thresholds']['max_curiosity']
            })
        
        if agent['curiosity'] < self.monitoring_config['alert_thresholds']['min_curiosity']:
            self.trigger_alert('low_curiosity', {
                'value': agent['curiosity'],
                'threshold': self.monitoring_config['alert_thresholds']['min_curiosity']
            })
        
        if agent['prediction_error'] > self.monitoring_config['alert_thresholds']['max_prediction_error']:
            self.trigger_alert('high_prediction_error', {
                'value': agent['prediction_error'],
                'threshold': self.monitoring_config['alert_thresholds']['max_prediction_error']
            })
        
        if brain['neuromodulation_level'] > self.monitoring_config['alert_thresholds']['max_neuromodulation']:
            self.trigger_alert('high_neuromodulation', {
                'value': brain['neuromodulation_level'],
                'threshold': self.monitoring_config['alert_thresholds']['max_neuromodulation']
            })
        
        # Check performance metrics
        performance = current_state['performance']
        if performance['cpu_usage'] > self.monitoring_config['alert_thresholds']['temperature_threshold']:
            self.trigger_alert('high_cpu_usage', {
                'value': performance['cpu_usage'],
                'threshold': self.monitoring_config['alert_thresholds']['temperature_threshold']
            })
        
        if performance['memory_usage'] > self.monitoring_config['alert_thresholds']['memory_threshold']:
            self.trigger_alert('high_memory_usage', {
                'value': performance['memory_usage'],
                'threshold': self.monitoring_config['alert_thresholds']['memory_threshold']
            })
    
    def trigger_alert(self, alert_type: str, details: Dict[str, Any]):
        """Trigger an alert."""
        alert = {
            'timestamp': time.time(),
            'type': alert_type,
            'details': details,
            'severity': self.get_alert_severity(alert_type, details)
        }
        
        self.alerts.append(alert)
        
        # Log alert
        print(f"ALERT: {alert_type} - {details}")
        
        # Send to WebSocket clients if available
        if self.websocket_server:
            asyncio.create_task(self.send_alert_to_clients(alert))
    
    def get_alert_severity(self, alert_type: str, details: Dict[str, Any]) -> str:
        """Determine alert severity."""
        severity_map = {
            'high_firing_rate': 'warning',
            'low_firing_rate': 'warning',
            'high_curiosity': 'info',
            'low_curiosity': 'info',
            'high_prediction_error': 'error',
            'high_neuromodulation': 'warning',
            'high_cpu_usage': 'critical',
            'high_memory_usage': 'critical'
        }
        
        return severity_map.get(alert_type, 'info')
    
    async def send_alert_to_clients(self, alert: Dict[str, Any]):
        """Send alert to WebSocket clients."""
        if self.websocket_server:
            message = json.dumps({
                'type': 'alert',
                'data': alert
            })
            # Send to all connected clients
            # Implementation depends on WebSocket server details
            pass
    
    def apply_control_policies(self):
        """Apply automatic control policies."""
        if not hasattr(self, 'monitoring_history') or len(self.monitoring_history) == 0:
            return
        
        current_state = self.monitoring_history[-1]
        
        # Apply auto-rewinding if enabled
        if self.monitoring_config['control_policies']['auto_rewinding']:
            self.apply_auto_rewinding(current_state)
        
        # Apply curiosity adjustment if enabled
        if self.monitoring_config['control_policies']['curiosity_adjustment']:
            self.apply_curiosity_adjustment(current_state)
        
        # Apply learning rate adaptation if enabled
        if self.monitoring_config['control_policies']['learning_rate_adaptation']:
            self.apply_learning_rate_adaptation(current_state)
        
        # Apply exploration boost if enabled
        if self.monitoring_config['control_policies']['exploration_boost']:
            self.apply_exploration_boost(current_state)
    
    def apply_auto_rewinding(self, current_state: Dict[str, Any]):
        """Apply auto-rewinding based on performance."""
        brain = current_state['brain']
        
        # Check if performance is degrading
        if len(self.monitoring_history) >= 10:
            recent_states = self.monitoring_history[-10:]
            recent_rewards = [state['performance']['avg_reward'] for state in recent_states]
            
            # Simple trend detection
            if len(recent_rewards) >= 5:
                first_half = recent_rewards[:len(recent_rewards)//2]
                second_half = recent_rewards[len(reabout_rewards)//2:]
                
                if np.mean(second_half) < np.mean(first_half) * 0.8:  # 20% degradation
                    print("Auto-rewinding triggered - performance degradation detected")
                    # Implement rewinding logic
    
    def apply_curiosity_adjustment(self, current_state: Dict[str, Any]):
        """Adjust curiosity based on system state."""
        agent = current_state['agent']
        
        # Adjust curiosity based on prediction error
        prediction_error = agent['prediction_error']
        current_curiosity = agent['curiosity']
        
        if prediction_error > 0.5:
            # High prediction error - increase curiosity
            adjusted_curiosity = min(current_curiosity * 1.2, 1.0)
        elif prediction_error < 0.1:
            # Low prediction error - decrease curiosity
            adjusted_curiosity = max(current_curiosity * 0.8, 0.0)
        else:
            adjusted_curiosity = current_curiosity
        
        if abs(adjusted_curiosity - current_curiosity) > 0.1:
            self.control_commands.append({
                'timestamp': time.time(),
                'type': 'curiosity_adjustment',
                'value': adjusted_curiosity,
                'reason': 'prediction_error_based'
            })
            print(f"Curiosity adjustment: {current_curiosity:.3f} -> {adjusted_curiosity:.3f}")
    
    def apply_learning_rate_adaptation(self, current_state: Dict[str, Any]):
        """Adapt learning rate based on performance."""
        # Implement learning rate adaptation
        pass
    
    def apply_exploration_boost(self, current_state: Dict[str, Any]):
        """Apply exploration boost to encourage exploration."""
        # Implement exploration boost
        pass
    
    def store_state(self):
        """Store current state for historical analysis."""
        if not hasattr(self, 'state_history'):
            return
        
        # Store state (simplified)
        self.state_history['brain_states'].append({
            'timestamp': time.time(),
            'firing_rate': self.neural_system['brain'].getAverageFiringRate(),
            'firing_count': self.neural_system['brain'].getFiringNeuronCount()
        })
    
    def get_system_status(self, agent) -> Dict[str, bool]:
        """Get system status for all learning subsystems."""
        return {
            'reward_modulation': agent.isRewardModulationEnabled(),
            'structural_plasticity': agent.isStructuralPlasticityEnabled(),
            'development': agent.isDevelopmentEnabled(),
            'curiosity': agent.isCuriosityEnabled()
        }
    
    def get_performance_metrics(self) -> Dict[str, float]:
        """Get current performance metrics."""
        # Simplified performance metrics
        return {
            'cpu_usage': 0.0,  # Would get from system
            'memory_usage': 0.0,  # Would get from system
            'step_time': 0.1,  # Average step time
            'throughput': 10.0,  # Steps per second
            'avg_reward': 0.0,  # Would get from simulation
            'firing_rate_stability': 0.9,  # Would calculate from history
            'adaptation_speed': 1.0  # Would calculate from learning curves
        }
    
    async def handle_websocket_connection(self, websocket, path):
        """Handle WebSocket connections."""
        print(f"WebSocket client connected: {websocket.remote_address}")
        
        try:
            # Send initial state
            await websocket.send(json.dumps({
                'type': 'system_status',
                'data': self.get_current_system_status()
            }))
            
            # Send current monitoring data
            await websocket.send(json.dumps({
                'type': 'monitoring_data',
                'data': self.get_recent_monitoring_data()
            }))
            
            # Send alerts if any
            if self.alerts:
                await websocket.send(json.dumps({
                    'type': 'alerts',
                    'data': self.alerts[-10:]  # Last 10 alerts
                }))
            
            # Handle incoming messages
            async for message in websocket:
                await self.handle_websocket_message(message, websocket)
                
        except websockets.exceptions.ConnectionClosed:
            print(f"WebSocket client disconnected: {websocket.remote_address}")
        except Exception as e:
            print(f"WebSocket error: {e}")
    
    def get_current_system_status(self) -> Dict[str, Any]:
        """Get current system status for WebSocket clients."""
        brain = self.neural_system['brain']
        agent = self.neural_system['agent']
        world = self.neural_system['world']
        
        return {
            'system_info': {
                'status': 'running',
                'version': '2.0.0',
                'uptime': time.time() - self.monitoring_start_time
            },
            'brain_state': {
                'firing_rate': brain.getAverageFiringRate(),
                'firing_count': brain.getFiringNeuronCount(),
                'developmental_stage': brain.getDevelopmentalStage()
            },
            'agent_state': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'systems': self.get_system_status(agent)
            },
            'world_state': {
                'simulation_time': world.getSimulationTime(),
                'agent_position': (world.getAgentBody().x, world.getAgentBody().y)
            },
            'monitoring_status': {
                'is_monitoring': self.is_monitoring,
                'data_points': len(self.monitoring_history) if hasattr(self, 'monitoring_history') else 0,
                'alert_count': len(self.alerts)
            }
        }
    
    def get_recent_monitoring_data(self) -> List[Dict[str, Any]]:
        """Get recent monitoring data for WebSocket clients."""
        if not hasattr(self, 'monitoring_history'):
            return []
        
        # Return last 1000 data points or all if fewer
        return self.monitoring_history[-1000:]
    
    async def handle_websocket_message(self, message: str, websocket):
        """Handle incoming WebSocket messages."""
        try:
            data = json.loads(message)
            
            if data['type'] == 'control_command':
                await self.handle_control_command(data['data'], websocket)
            elif data['type'] == 'request_data':
                await self.handle_data_request(data['data'], websocket)
            elif data['type'] == 'system_query':
                await self.handle_system_query(data['data'], websocket)
            elif data['type'] == 'control_policy_update':
                await self.handle_control_policy_update(data['data'], websocket)
            else:
                await websocket.send(json.dumps({
                    'type': 'error',
                    'message': f"Unknown message type: {data['type']}"
                }))
                
        except Exception as e:
            print(f"Error handling WebSocket message: {e}")
            await websocket.send(json.dumps({
                'type': 'error',
                'message': str(e)
            }))
    
    async def handle_control_command(self, command: Dict[str, Any], websocket):
        """Handle control commands."""
        command_type = command.get('command_type')
        
        if command_type == 'set_curiosity':
            value = command.get('value', 0.5)
            self.control_commands.append({
                'timestamp': time.time(),
                'type': 'set_curiosity',
                'value': value
            })
            print(f"Control command: Set curiosity to {value}")
            
            await websocket.send(json.dumps({
                'type': 'command_response',
                'status': 'success',
                'message': f"Curiosity set to {value}"
            }))
        
        elif command_type == 'set_learning_rate':
            value = command.get('value', 0.01)
            self.control_commands.append({
                'timestamp': time.time(),
                'type': 'set_learning_rate',
                'value': value
            })
            print(f"Control command: Set learning rate to {value}")
            
            await websocket.send(json.dumps({
                'type': 'command_response',
                'status': 'success',
                'message': f"Learning rate set to {value}"
            }))
        
        elif command_type == 'toggle_system':
            system = command.get('system', 'all')
            self.control_commands.append({
                'timestamp': time.time(),
                'type': 'toggle_system',
                'system': system
            })
            print(f"Control command: Toggle system {system}")
            
            await websocket.send(json.dumps({
                'type': 'command_response',
                'status': 'success',
                'message': f"System {system} toggled"
            }))
    
    async def handle_data_request(self, request: Dict[str, Any], websocket):
        """Handle data requests."""
        request_type = request.get('request_type')
        
        if request_type == 'recent_data':
            limit = request.get('limit', 100)
            data = self.get_recent_monitoring_data()
            await websocket.send(json.dumps({
                'type': 'data_response',
                'data': data[:limit]
            }))
        
        elif request_type == 'system_metrics':
            metrics = self.get_current_system_status()
            await websocket.send(json.dumps({
                'type': 'data_response',
                'data': metrics
            }))
        
        elif request_type == 'alert_history':
            limit = request.get('limit', 50)
            await websocket.send(json.dumps({
                'type': 'data_response',
                'data': self.alerts[-limit:]
            }))
    
    async def handle_system_query(self, query: Dict[str, Any], websocket):
        """Handle system queries."""
        query_type = query.get('query_type')
        
        if query_type == 'health_check':
            health_status = self.get_health_status()
            await websocket.send(json.dumps({
                'type': 'query_response',
                'data': health_status
            }))
        
        elif query_type == 'performance_report':
            performance_report = self.get_performance_report()
            await websocket.send(json.dumps({
                'type': 'query_response',
                'data': performance_report
            }))
    
    async def handle_control_policy_update(self, update: Dict[str, Any], websocket):
        """Handle control policy updates."""
        policy_type = update.get('policy_type')
        value = update.get('value')
        
        if policy_type in self.monitoring_config['control_policies']:
            self.monitoring_config['control_policies'][policy_type] = value
            
            await websocket.send(json.dumps({
                'type': 'policy_response',
                'status': 'success',
                'message': f"Policy {policy_type} updated to {value}"
            }))
        else:
            await websocket.send(json.dumps({
                'type': 'policy_response',
                'status': 'error',
                'message': f"Unknown policy type: {policy_type}"
            }))
    
    def get_health_status(self) -> Dict[str, Any]:
        """Get system health status."""
        health_status = {
            'is_healthy': True,
            'issues': [],
            'last_check': time.time()
        }
        
        # Check system health
        if len(self.monitoring_history) == 0:
            health_status['is_healthy'] = False
            health_status['issues'].append('No monitoring data available')
        
        # Check for critical alerts
        critical_alerts = [alert for alert in self.alerts if alert['severity'] == 'critical']
        if critical_alerts:
            health_status['is_healthy'] = False
            health_status['issues'].extend([f"Critical alert: {alert['type']}" for alert in critical_alerts])
        
        # Check for performance degradation
        if hasattr(self, 'monitoring_history') and len(self.monitoring_history) >= 10:
            recent_states = self.monitoring_history[-10:]
            brain_firing_rates = [state['brain']['firing_rate'] for state in recent_states]
            
            # Check for abnormal firing rates
            if any(rate < 0.1 or rate > 50 for rate in brain_firing_rates):
                health_status['issues'].append('Abnormal firing rates detected')
        
        return health_status
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate performance report."""
        if not hasattr(self, 'monitoring_history'):
            return {'status': 'no_data'}
        
        # Calculate performance metrics
        recent_data = self.monitoring_history[-100:] if len(self.monitoring_history) >= 100 else self.monitoring_history
        
        performance_metrics = {
            'average_firing_rate': np.mean([state['brain']['firing_rate'] for state in recent_data]),
            'firing_rate_std': np.std([state['brain']['firing_rate'] for state in recent_data]),
            'average_curiosity': np.mean([state['agent']['curiosity'] for state in recent_data]),
            'average_prediction_error': np.mean([state['agent']['prediction_error'] for state in recent_data]),
            'data_points': len(recent_data),
            'alert_count': len(self.alerts),
            'monitoring_uptime': self.monitoring_uptime if hasattr(self, 'monitoring_uptime') else time.time() - self.monitoring_start_time
        }
        
        return performance_metrics
    
    def stop_monitoring(self):
        """Stop monitoring."""
        print("Stopping real-time monitoring...")
        
        self.is_monitoring = False
        
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
        
        if self.websocket_server:
            self.websocket_server.close()
        
        print("Real-time monitoring stopped successfully")

# Usage example for real-time monitoring
if __name__ == "__main__":
    import asyncio
    
    async def main():
        # Create neural system
        config = pynlm.create_preset_config("research")
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.setAgentStart(10.0, 10.0)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enable_all_systems()
        
        neural_system = {
            'brain': brain,
            'world': world,
            'agent': agent
        }
        
        # Create real-time monitor
        monitor = RealTimeNeuralMonitor(neural_system, host="localhost", port=8765)
        
        # Start monitoring
        await monitor.start_monitoring()
        
        # Run simulation in background
        def run_simulation():
            for step in range(10000):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step, step * 0.001)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.001)
                
                time.sleep(0.1)  # Simulate real-time
        
        simulation_thread = threading.Thread(target=run_simulation, daemon=True)
        simulation_thread.start()
        
        try:
            # Keep running until interrupted
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            print("\nShutting down...")
            monitor.stop_monitoring()
            break
    
    # Run the example
    asyncio.run(main())
```

## Conclusion

This document provides comprehensive examples and advanced usage patterns for the enhanced NLM Python bindings. The examples demonstrate:

1. **Professional Neural Network Development** - Advanced system architecture with monitoring and adaptive control
2. **Research-Grade Analysis** - Comprehensive scientific analysis tools with statistical validation
3. **Neural Network Optimization** - Automated optimization using various algorithms
4. **Real-Time Monitoring and Control** - Production-grade monitoring with WebSocket interfaces

These examples showcase the enhanced capabilities of the NLM Python bindings, including:

- Advanced error handling and validation
- Comprehensive monitoring and debugging tools
- Performance profiling and optimization
- Real-time data collection and analysis
- Automated control systems
- Research-grade statistical analysis
- Production-ready monitoring infrastructure

The examples are designed to work with the enhanced Python bindings that include:

1. **Enhanced Configuration Management** - Advanced configuration validation and preset templates
2. **Comprehensive Monitoring Tools** - Performance profilers, neural activity trackers, and debug consoles
3. **Advanced Error Handling** - Custom Python exceptions and robust error recovery
4. **Sophisticated Statistics** - Comprehensive neural activity analysis and pattern recognition
5. **Batch Processing Capabilities** - Efficient processing of multiple operations
6. **Performance Optimization** - Automated optimization and bottleneck detection

These examples provide a complete toolkit for professional, research, and production use of NLM neural networks, from basic development to large-scale deployment with real-time monitoring and control.
