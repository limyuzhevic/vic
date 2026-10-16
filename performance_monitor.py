#!/usr/bin/env python3
"""
NLM Performance Monitoring and Debugging Tools

Advanced monitoring and debugging tools for NLM Python simulations.
"""

import pynlm
import time
import threading
import statistics
from collections import defaultdict, deque
from typing import Dict, List, Any, Optional, Callable
import json
from enum import Enum
import numpy as np


class PerformanceMetrics:
    """Collect and analyze performance metrics for NLM simulations."""

    def __init__(self, history_size: int = 1000):
        self.history_size = history_size
        self.step_times = deque(maxlen=history_size)
        self.spike_counts = deque(maxlen=history_size)
        self.firing_rates = deque(maxlen=history_size)
        self.neuromodulation_levels = deque(maxlen=history_size)
        self.curiosity_levels = deque(maxlen=history_size)
        self.novelty_levels = deque(maxlen=history_size)
        self.prediction_errors = deque(maxlen=history_size)
        
        self.start_time = None
        self.end_time = None
        self.total_steps = 0
        self.metrics_enabled = False

    def start_monitoring(self):
        """Start performance monitoring."""
        self.start_time = time.time()
        self.metrics_enabled = True

    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.end_time = time.time()
        self.metrics_enabled = False

    def record_step(self, step_time: float, brain_stats: Dict[str, Any], 
                    agent_stats: Optional[Dict[str, Any]] = None):
        """Record metrics for a simulation step."""
        if not self.metrics_enabled:
            return

        self.step_times.append(step_time)
        self.spike_counts.append(brain_stats.get('total_spikes', 0))
        self.firing_rates.append(brain_stats.get('firing_rate', 0.0))

        if agent_stats:
            self.neuromodulation_levels.append(agent_stats.get('neuromodulation_level', 0.0))
            self.curiosity_levels.append(agent_stats.get('curiosity_level', 0.0))
            self.novelty_levels.append(agent_stats.get('novelty_level', 0.0))
            self.prediction_errors.append(agent_stats.get('prediction_error', 0.0))

        self.total_steps += 1

    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary."""
        if not self.step_times:
            return {"error": "No metrics recorded"}

        summary = {
            "total_steps": self.total_steps,
            "total_time": self.end_time - self.start_time if self.end_time and self.start_time else 0.0,
            "steps_per_second": self.total_steps / (self.end_time - self.start_time) 
                                if self.end_time and self.start_time and self.total_steps > 0 else 0.0,
            "avg_step_time": statistics.mean(self.step_times) if self.step_times else 0.0,
            "min_step_time": min(self.step_times) if self.step_times else 0.0,
            "max_step_time": max(self.step_times) if self.step_times else 0.0,
            "step_time_stddev": statistics.stdev(self.step_times) 
                                if len(self.step_times) > 1 else 0.0,
            "total_spikes": sum(self.spike_counts) if self.spike_counts else 0,
            "avg_spikes_per_step": statistics.mean(self.spike_counts) 
                                    if self.spike_counts else 0.0,
            "spike_count_stddev": statistics.stdev(self.spike_counts) 
                                  if len(self.spike_counts) > 1 else 0.0,
            "avg_firing_rate": statistics.mean(self.firing_rates) 
                               if self.firing_rates else 0.0,
        }

        # Add neuromodulation statistics if available
        if self.neuromodulation_levels:
            summary.update({
                "avg_neuromodulation": statistics.mean(self.neuromodulation_levels),
                "neuromodulation_stddev": statistics.stdev(self.neuromodulation_levels) 
                                          if len(self.neuromodulation_levels) > 1 else 0.0,
                "min_neuromodulation": min(self.neuromodulation_levels),
                "max_neuromodulation": max(self.neuromodulation_levels),
            })

        if self.curiosity_levels:
            summary.update({
                "avg_curiosity": statistics.mean(self.curiosity_levels),
                "curiosity_stddev": statistics.stdev(self.curiosity_levels) 
                                    if len(self.curiosity_levels) > 1 else 0.0,
                "min_curiosity": min(self.curiosity_levels),
                "max_curiosity": max(self.curiosity_levels),
            })

        if self.novelty_levels:
            summary.update({
                "avg_novelty": statistics.mean(self.novelty_levels),
                "novelty_stddev": statistics.stdev(self.novelty_levels) 
                                  if len(self.novelty_levels) > 1 else 0.0,
                "min_novelty": min(self.novelty_levels),
                "max_novelty": max(self.novelty_levels),
            })

        if self.prediction_errors:
            summary.update({
                "avg_prediction_error": statistics.mean(self.prediction_errors),
                "prediction_error_stddev": statistics.stdev(self.prediction_errors) 
                                          if len(self.prediction_errors) > 1 else 0.0,
                "min_prediction_error": min(self.prediction_errors),
                "max_prediction_error": max(self.prediction_errors),
            })

        return summary

    def get_time_series_data(self) -> Dict[str, List[float]]:
        """Get time series data for plotting."""
        return {
            "step_times": list(self.step_times),
            "spike_counts": list(self.spike_counts),
            "firing_rates": list(self.firing_rates),
            "neuromodulation": list(self.neuromodulation_levels),
            "curiosity": list(self.curiosity_levels),
            "novelty": list(self.novelty_levels),
            "prediction_errors": list(self.prediction_errors),
        }

    def save_metrics(self, filepath: str):
        """Save metrics to file."""
        data = {
            "summary": self.get_summary(),
            "time_series": self.get_time_series_data(),
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)

    def load_metrics(self, filepath: str):
        """Load metrics from file."""
        with open(filepath, 'r') as f:
            data = json.load(f)

        self.step_times = deque(data['time_series']['step_times'], maxlen=self.history_size)
        self.spike_counts = deque(data['time_series']['spike_counts'], maxlen=self.history_size)
        self.firing_rates = deque(data['time_series']['firing_rates'], maxlen=self.history_size)
        
        if 'neuromodulation' in data['time_series']:
            self.neuromodulation_levels = deque(data['time_series']['neuromodulation'], 
                                                maxlen=self.history_size)

        if 'curiosity' in data['time_series']:
            self.curiosity_levels = deque(data['time_series']['curiosity'], 
                                         maxlen=self.history_size)

        if 'novelty' in data['time_series']:
            self.novelty_levels = deque(data['time_series']['novelty'], 
                                       maxlen=self.history_size)

        if 'prediction_errors' in data['time_series']:
            self.prediction_errors = deque(data['time_series']['prediction_errors'], 
                                           maxlen=self.history_size)

        self.total_steps = len(self.step_times)
        self.metrics_enabled = False


class DebugLogger:
    """Enhanced debugging and logging for NLM simulations."""

    def __init__(self, log_file: Optional[str] = None, verbose: bool = True):
        self.log_file = log_file
        self.verbose = verbose
        self.log_entries = []
        self.debug_level = "INFO"
        self.filters = []
        self.handlers: List[Callable] = []

    def set_debug_level(self, level: str):
        """Set debug logging level."""
        valid_levels = ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]
        if level in valid_levels:
            self.debug_level = level

    def add_filter(self, filter_func: Callable[[str, str], bool]):
        """Add a logging filter."""
        self.filters.append(filter_func)

    def add_handler(self, handler: Callable[[str, str, Dict], None]):
        """Add a log handler."""
        self.handlers.append(handler)

    def debug(self, message: str, context: Optional[Dict] = None):
        """Log debug message."""
        self._log("DEBUG", message, context)

    def info(self, message: str, context: Optional[Dict] = None):
        """Log info message."""
        self._log("INFO", message, context)

    def warning(self, message: str, context: Optional[Dict] = None):
        """Log warning message."""
        self._log("WARNING", message, context)

    def error(self, message: str, context: Optional[Dict] = None):
        """Log error message."""
        self._log("ERROR", message, context)

    def critical(self, message: str, context: Optional[Dict] = None):
        """Log critical message."""
        self._log("CRITICAL", message, context)

    def _log(self, level: str, message: str, context: Optional[Dict] = None):
        """Internal logging method."""
        timestamp = time.time()
        log_entry = {
            "timestamp": timestamp,
            "level": level,
            "message": message,
            "context": context or {},
        }

        # Apply filters
        should_log = True
        for filter_func in self.filters:
            if not filter_func(level, message):
                should_log = False
                break

        if should_log and self._should_log_level(level):
            self.log_entries.append(log_entry)

            # Handle log entry
            for handler in self.handlers:
                handler(level, message, log_entry)

            # Console output
            if self.verbose:
                formatted_msg = f"[{level}] {timestamp:.2f}: {message}"
                if context:
                    formatted_msg += f" | Context: {context}"
                print(formatted_msg)

            # File output
            if self.log_file:
                with open(self.log_file, 'a') as f:
                    f.write(f"[{level}] {timestamp:.2f}: {message}\n")

    def _should_log_level(self, level: str) -> bool:
        """Check if level should be logged based on debug level."""
        levels = ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]
        try:
            log_index = levels.index(level)
            debug_index = levels.index(self.debug_level)
            return log_index >= debug_index
        except ValueError:
            return True

    def get_logs_by_level(self, level: str) -> List[Dict]:
        """Get all logs for a specific level."""
        return [entry for entry in self.log_entries if entry['level'] == level]

    def get_logs_by_keyword(self, keyword: str) -> List[Dict]:
        """Get all logs containing keyword."""
        return [entry for entry in self.log_entries 
                if keyword.lower() in entry['message'].lower()]

    def get_recent_logs(self, count: int) -> List[Dict]:
        """Get recent log entries."""
        return self.log_entries[-count:]

    def save_logs(self, filepath: str):
        """Save logs to file."""
        with open(filepath, 'w') as f:
            json.dump(self.log_entries, f, indent=2)

    def clear_logs(self):
        """Clear all log entries."""
        self.log_entries.clear()


class SimulationProfiler:
    """Profile and analyze simulation performance."""

    def __init__(self):
        self.function_profiles = {}
        self.step_profiles = []
        self.overall_start_time = None
        self.overall_end_time = None

    def start_overall(self):
        """Start overall simulation timing."""
        self.overall_start_time = time.time()

    def end_overall(self):
        """End overall simulation timing."""
        self.overall_end_time = time.time()

    def start_function(self, name: str):
        """Start profiling a function."""
        if name not in self.function_profiles:
            self.function_profiles[name] = []
        return time.time()

    def end_function(self, name: str, start_time: float):
        """End profiling a function."""
        if name in self.function_profiles:
            duration = time.time() - start_time
            self.function_profiles[name].append(duration)

    def record_step_profile(self, step: int, step_time: float,
                           brain_time: float, agent_time: float,
                           world_time: float):
        """Record step-level performance profile."""
        profile = {
            'step': step,
            'total_time': step_time,
            'brain_time': brain_time,
            'agent_time': agent_time,
            'world_time': world_time,
            'timestamp': time.time(),
        }
        self.step_profiles.append(profile)

    def get_summary(self) -> Dict[str, Any]:
        """Get profiling summary."""
        summary = {
            'overall_duration': (self.overall_end_time - self.overall_start_time) 
                               if self.overall_end_time and self.overall_start_time else 0.0,
            'total_steps': len(self.step_profiles),
            'avg_step_time': statistics.mean([p['total_time'] for p in self.step_profiles]) 
                           if self.step_profiles else 0.0,
            'step_time_stddev': statistics.stdev([p['total_time'] for p in self.step_profiles]) 
                               if len(self.step_profiles) > 1 else 0.0,
            'brain_time_percentage': (statistics.mean([p['brain_time'] for p in self.step_profiles]) 
                                    / statistics.mean([p['total_time'] for p in self.step_profiles]) 
                                    if self.step_profiles and statistics.mean([p['total_time'] for p in self.step_profiles]) > 0 else 0.0) * 100,
            'agent_time_percentage': (statistics.mean([p['agent_time'] for p in self.step_profiles]) 
                                    / statistics.mean([p['total_time'] for p in self.step_profiles]) 
                                    if self.step_profiles and statistics.mean([p['total_time'] for p in self.step_profiles]) > 0 else 0.0) * 100,
            'world_time_percentage': (statistics.mean([p['world_time'] for p in self.step_profiles]) 
                                    / statistics.mean([p['total_time'] for p in self.step_profiles]) 
                                    if self.step_profiles and statistics.mean([p['total_time'] for p in self.step_profiles]) > 0 else 0.0) * 100,
        }

        # Add function profiling
        for func_name, durations in self.function_profiles.items():
            if durations:
                summary[f'{func_name}_count'] = len(durations)
                summary[f'{func_name}_avg_time'] = statistics.mean(durations)
                summary[f'{func_name}_stddev'] = statistics.stdev(durations) 
                                              if len(durations) > 1 else 0.0

        return summary

    def save_profiles(self, filepath: str):
        """Save profiles to file."""
        data = {
            'step_profiles': self.step_profiles,
            'function_profiles': self.function_profiles,
            'overall_start_time': self.overall_start_time,
            'overall_end_time': self.overall_end_time,
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)


class AgentDebugger:
    """Debug agent behavior and decision-making."""

    def __init__(self):
        self.action_history = []
        self.sensory_history = []
        self.reward_history = []
        self.neuromodulation_history = []
        self.decision_patterns = []

    def record_step(self, step: int, action: pynlm.MotorCommand,
                   percept: pynlm.SensoryPercept, reward: float,
                   neuromodulation: Dict[str, float]):
        """Record agent behavior for analysis."""
        self.action_history.append({
            'step': step,
            'action': str(action),
            'action_type': action.value if hasattr(action, 'value') else str(action),
            'timestamp': time.time(),
        })

        self.sensory_history.append({
            'step': step,
            'vision': percept.getVision(),
            'touch': percept.getTouch(),
            'internal': percept.getInternal(),
            'proprioception': percept.getProprioception(),
            'audio': percept.getAudio(),
            'timestamp': time.time(),
        })

        self.reward_history.append({
            'step': step,
            'reward': reward,
            'timestamp': time.time(),
        })

        self.neuromodulation_history.append({
            'step': step,
            'curiosity': neuromodulation.get('curiosity', 0.0),
            'novelty': neuromodulation.get('novelty', 0.0),
            'prediction_error': neuromodulation.get('prediction_error', 0.0),
            'neuromodulation': neuromodulation.get('neuromodulation', 0.0),
            'timestamp': time.time(),
        })

    def analyze_action_patterns(self) -> Dict[str, Any]:
        """Analyze action patterns and decision-making."""
        if not self.action_history:
            return {'error': 'No action data available'}

        actions = [entry['action_type'] for entry in self.action_history]
        unique_actions = list(set(actions))

        analysis = {
            'total_actions': len(actions),
            'unique_actions': len(unique_actions),
            'action_frequency': {action: actions.count(action) / len(actions) 
                                for action in unique_actions},
            'most_common_action': max(set(actions), key=actions.count),
            'least_common_action': min(set(actions), key=actions.count),
            'action_entropy': self._calculate_entropy(actions),
        }

        # Add temporal patterns
        if len(self.action_history) > 10:
            consecutive_same = 0
            max_consecutive_same = 0
            
            for i in range(1, len(actions)):
                if actions[i] == actions[i-1]:
                    consecutive_same += 1
                    max_consecutive_same = max(max_consecutive_same, consecutive_same)
                else:
                    consecutive_same = 0

            analysis['max_consecutive_same_action'] = max_consecutive_same
            analysis['pattern_stability'] = 1.0 - (max_consecutive_same / len(actions))

        return analysis

    def analyze_sensory_patterns(self) -> Dict[str, Any]:
        """Analyze sensory input patterns."""
        if not self.sensory_history:
            return {'error': 'No sensory data available'}

        analysis = {
            'total_steps': len(self.sensory_history),
            'vision_stats': {
                'mean': np.mean([len(s['vision']) for s in self.sensory_history]),
                'std': np.std([len(s['vision']) for s in self.sensory_history]),
                'min': min(len(s['vision']) for s in self.sensory_history),
                'max': max(len(s['vision']) for s in self.sensory_history),
            },
            'touch_stats': {
                'mean': np.mean([len(s['touch']) for s in self.sensory_history]),
                'std': np.std([len(s['touch']) for s in self.sensory_history]),
                'min': min(len(s['touch']) for s in self.sensory_history),
                'max': max(len(s['touch']) for s in self.sensory_history),
            },
        }

        return analysis

    def analyze_reward_patterns(self) -> Dict[str, Any]:
        """Analyze reward patterns."""
        if not self.reward_history:
            return {'error': 'No reward data available'}

        rewards = [entry['reward'] for entry in self.reward_history]

        analysis = {
            'total_rewards': len(rewards),
            'mean_reward': statistics.mean(rewards),
            'median_reward': statistics.median(rewards),
            'min_reward': min(rewards),
            'max_reward': max(rewards),
            'reward_stddev': statistics.stdev(rewards) if len(rewards) > 1 else 0.0,
            'positive_rewards': sum(1 for r in rewards if r > 0),
            'negative_rewards': sum(1 for r in rewards if r < 0),
            'zero_rewards': sum(1 for r in rewards if r == 0),
            'reward_entropy': self._calculate_entropy(['positive' if r > 0 else 'negative' 
                                                      if r < 0 else 'zero' for r in rewards]),
        }

        # Calculate reward change patterns
        reward_changes = []
        for i in range(1, len(rewards)):
            reward_changes.append(rewards[i] - rewards[i-1])

        if reward_changes:
            analysis['reward_change_stats'] = {
                'mean_change': statistics.mean(reward_changes),
                'stddev_change': statistics.stdev(reward_changes) 
                               if len(reward_changes) > 1 else 0.0,
                'positive_changes': sum(1 for c in reward_changes if c > 0),
                'negative_changes': sum(1 for c in reward_changes if c < 0),
                'zero_changes': sum(1 for c in reward_changes if c == 0),
            }

        return analysis

    def _calculate_entropy(self, values: List) -> float:
        """Calculate Shannon entropy of a list of values."""
        from collections import Counter
        counts = Counter(values)
        total = len(values)
        entropy = 0.0
        
        for count in counts.values():
            probability = count / total
            entropy -= probability * np.log2(probability)
        
        return entropy

    def get_timeline_data(self) -> Dict[str, List]:
        """Get timeline data for visualization."""
        return {
            'steps': [entry['step'] for entry in self.action_history],
            'actions': [entry['action_type'] for entry in self.action_history],
            'rewards': [entry['reward'] for entry in self.reward_history],
            'curiosity': [entry['curiosity'] for entry in self.neuromodulation_history],
            'novelty': [entry['novelty'] for entry in self.neuromodulation_history],
        }


# Convenience functions for easy integration

def create_debugging_environment() -> tuple:
    """Create a complete debugging environment."""
    metrics = PerformanceMetrics()
    logger = DebugLogger()
    profiler = SimulationProfiler()
    agent_debugger = AgentDebugger()
    
    return metrics, logger, profiler, agent_debugger


def run_monitored_simulation(brain, world, agent, num_steps: int,
                            logger: Optional[DebugLogger] = None,
                            metrics: Optional[PerformanceMetrics] = None) -> Dict[str, Any]:
    """Run a simulation with monitoring and debugging."""
    if logger:
        logger.info("Starting monitored simulation", {'steps': num_steps})
    
    if metrics:
        metrics.start_monitoring()

    start_time = time.time()

    for step in range(num_steps):
        step_start = time.time()

        # Update world
        world.update(0.1)

        # Get sensory input
        percept = world.getSensoryPercept()

        # Process sensory input
        agent.processSensoryInput(percept)

        # Run brain step
        brain.step(step)

        # Get action
        action = agent.decodeMotorCommand()

        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())

        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)

        # Update development
        agent.updateDevelopment(0.1)

        step_end = time.time()
        step_time = step_end - step_start

        # Record metrics
        if metrics:
            brain_stats = pynlm.getBrainStats(brain)
            agent_stats = pynlm.getAgentStats(agent)
            metrics.record_step(step_time, brain_stats, agent_stats)

        # Log step
        if logger:
            logger.debug(f"Step {step} completed", {
                'step_time': step_time,
                'spikes': brain_stats.get('total_spikes', 0) if metrics else 0,
                'action': str(action),
                'reward': reward,
            })

    end_time = time.time()
    total_time = end_time - start_time

    if metrics:
        metrics.stop_monitoring()

    if logger:
        logger.info("Simulation completed", {
            'total_steps': num_steps,
            'total_time': total_time,
            'steps_per_second': num_steps / total_time if total_time > 0 else 0.0,
        })

    result = {
        'success': True,
        'total_steps': num_steps,
        'total_time': total_time,
        'steps_per_second': num_steps / total_time if total_time > 0 else 0.0,
    }

    if metrics:
        result['metrics_summary'] = metrics.get_summary()

    return result