"""
StateInspector class for memory analysis and comparison.

Provides comprehensive state inspection and comparison for neural simulations,
including memory tracking, state comparison, and deep analysis of neural system behavior.
"""

import numpy as np
import psutil
import time
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field

@dataclass
class StateSnapshot:
    """Neural state snapshot for debugging."""
    timestamp: float
    step: int
    time: float
    neural_state: Dict[str, Any]
    memory_usage: Dict[str, float]
    performance_metrics: Dict[str, float]
    metadata: Dict[str, Any] = field(default_factory=dict)

class StateInspector:
    """
    Advanced state inspection and comparison for neural simulations.
    
    Provides comprehensive memory analysis, state comparison, and detailed inspection
    capabilities for neural simulation states. Supports memory tracking, state
    comparison, and deep analysis of neural system behavior.
    """
    
    def __init__(self, brain):
        self.brain = brain
        self.state_history = []
        self.memory_samples = []
        self.state_snapshots = {}
        self.inspection_rules = []
        self.comparison_cache = {}
        self.performance_profile = {
            'memory_usage': [],
            'active_neurons': [],
            'firing_rates': [],
            'e_i_ratios': [],
            'timestamps': []
        }
        self.baseline_state = None
        self.anomaly_detector = AnomalyDetector()
        self.change_detector = ChangeDetector()
        
        print(f"[INSPECTOR] State inspector initialized for brain with {brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 'unknown'} neurons")
    
    def capture_state(self, step: int, time: float, metadata: Optional[Dict] = None) -> StateSnapshot:
        """
        Capture current brain state for inspection.
        
        Args:
            step: Current simulation step
            time: Current simulation time
            metadata: Additional metadata for the state
            
        Returns:
            StateSnapshot object containing the captured state
        """
        snapshot = StateSnapshot(
            timestamp=time.time(),
            step=step,
            time=time,
            neural_state=self._capture_neural_state(),
            memory_usage=self._capture_memory_usage(),
            performance_metrics=self._capture_performance_metrics(),
            metadata=metadata or {}
        )
        
        self.state_history.append(snapshot)
        self.state_snapshots[f"step_{step}"] = snapshot
        
        # Update performance profile
        self._update_performance_profile(snapshot)
        
        # Detect anomalies
        self._detect_anomalies(snapshot)
        
        # Detect significant changes
        if len(self.state_history) > 1:
            self._detect_state_changes()
        
        return snapshot
    
    def set_baseline(self, step: int = 0):
        """Set baseline state for comparison."""
        self.baseline_state = self.get_state_at_step(step)
        print(f"[INSPECTOR] Baseline state set at step {step}")
    
    def compare_to_baseline(self, step: int = None) -> Dict[str, Any]:
        """
        Compare current state to baseline.
        
        Args:
            step: Step to compare (uses current step if None)
            
        Returns:
            Comparison results dictionary
        """
        if self.baseline_state is None:
            return {'error': 'No baseline state set. Call set_baseline() first.'}
        
        target_step = step if step is not None else self.state_history[-1].step
        current_state = self.get_state_at_step(target_step)
        
        if current_state is None:
            return {'error': f'State at step {target_step} not found'}
        
        comparison = {
            'baseline_step': self.baseline_state.step,
            'comparison_step': target_step,
            'differences': self._compare_states(self.baseline_state, current_state),
            'similarity_score': self._calculate_similarity(self.baseline_state, current_state)
        }
        
        return comparison
    
    def compare_states(self, step1: int, step2: int) -> Dict[str, Any]:
        """
        Compare two arbitrary states.
        
        Args:
            step1: First step to compare
            step2: Second step to compare
            
        Returns:
            Comparison results dictionary
        """
        state1 = self.get_state_at_step(step1)
        state2 = self.get_state_at_step(step2)
        
        if state1 is None:
            return {'error': f'State at step {step1} not found'}
        if state2 is None:
            return {'error': f'State at step {step2} not found'}
        
        comparison = {
            'step1': step1,
            'step2': step2,
            'differences': self._compare_states(state1, state2),
            'similarity_score': self._calculate_similarity(state1, state2)
        }
        
        return comparison
    
    def get_state_at_step(self, step: int) -> Optional[StateSnapshot]:
        """Get state snapshot at specified step."""
        for snapshot in self.state_history:
            if snapshot.step == step:
                return snapshot
        return None
    
    def get_state_history(self, limit: int = 100) -> List[StateSnapshot]:
        """Get recent state snapshots."""
        return self.state_history[-limit:] if limit > 0 else self.state_history
    
    def analyze_memory_usage(self, step: int = None) -> Dict[str, Any]:
        """
        Analyze memory usage patterns.
        
        Args:
            step: Step to analyze (uses current step if None)
            
        Returns:
            Memory analysis results
        """
        if step is None:
            step = self.state_history[-1].step if self.state_history else 0
        
        state = self.get_state_at_step(step)
        if state is None:
            return {'error': f'State at step {step} not found'}
        
        # Analyze memory trends
        recent_memory = [s.memory_usage for s in self.state_history[-10:]] if len(self.state_history) >= 10 else []
        
        analysis = {
            'current_memory': state.memory_usage,
            'memory_trends': self._analyze_memory_trends(),
            'peak_memory': self._get_peak_memory(),
            'memory_growth_rate': self._calculate_memory_growth_rate(),
            'memory_efficiency': self._calculate_memory_efficiency()
        }
        
        return analysis
    
    def analyze_neural_patterns(self, step: int = None) -> Dict[str, Any]:
        """
        Analyze neural activity patterns.
        
        Args:
            step: Step to analyze (uses current step if None)
            
        Returns:
            Pattern analysis results
        """
        if step is None:
            step = self.state_history[-1].step if self.state_history else 0
        
        state = self.get_state_at_step(step)
        if state is None:
            return {'error': f'State at step {step} not found'}
        
        # Pattern analysis
        analysis = {
            'current_activity': state.neural_state,
            'activity_patterns': self._extract_activity_patterns(),
            'regional_dynamics': self._analyze_regional_dynamics(),
            'temporal_patterns': self._analyze_temporal_patterns(),
            'correlation_analysis': self._analyze_correlations()
        }
        
        return analysis
    
    def detect_anomalies(self, step: int = None, sensitivity: float = 1.0) -> List[Dict[str, Any]]:
        """
        Detect anomalies in neural activity.
        
        Args:
            step: Step to analyze (uses current step if None)
            sensitivity: Sensitivity threshold for anomaly detection
            
        Returns:
            List of detected anomalies
        """
        if step is None:
            step = self.state_history[-1].step if self.state_history else 0
        
        state = self.get_state_at_step(step)
        if state is None:
            return []
        
        anomalies = self.anomaly_detector.detect_anomalies(state, sensitivity)
        
        # Log anomalies
        for anomaly in anomalies:
            self._log_warning(f"Anomaly detected: {anomaly['description']} at step {step}")
        
        return anomalies
    
    def generate_inspection_report(self, step: int = None) -> Dict[str, Any]:
        """
        Generate comprehensive inspection report.
        
        Args:
            step: Step to generate report for (uses current step if None)
            
        Returns:
            Inspection report
        """
        if step is None:
            step = self.state_history[-1].step if self.state_history else 0
        
        state = self.get_state_at_step(step)
        if state is None:
            return {'error': f'State at step {step} not found'}
        
        # Generate report
        report = {
            'inspection_info': {
                'step': step,
                'timestamp': state.timestamp,
                'total_states_captured': len(self.state_history),
                'baseline_set': self.baseline_state is not None
            },
            'memory_analysis': self.analyze_memory_usage(step),
            'neural_analysis': self.analyze_neural_patterns(step),
            'anomalies_detected': self.detect_anomalies(step),
            'performance_summary': self._get_performance_summary(),
            'change_summary': self._get_change_summary()
        }
        
        return report
    
    def _capture_neural_state(self) -> Dict[str, Any]:
        """Capture current neural state."""
        neural_state = {}
        
        if hasattr(self.brain, 'getRegions'):
            regions = self.brain.getRegions()
            neural_state['regions'] = {
                region_id.value: {
                    'active_neurons': region.getActiveNeuronCount(),
                    'total_neurons': region.getTotalNeuronCount(),
                    'firing_rate': region.getActiveNeuronCount() / max(region.getTotalNeuronCount(), 1),
                    'density': region.getActiveNeuronCount() / max(region.getTotalNeuronCount(), 1)
                }
                for region_id, region in regions.items()
            }
        
        # Capture brain-wide metrics
        if hasattr(self.brain, 'getTotalSpikeCount'):
            neural_state['total_spikes'] = self.brain.getTotalSpikeCount()
        if hasattr(self.brain, 'getActiveNeuronCount'):
            neural_state['active_neurons'] = self.brain.getActiveNeuronCount()
        if hasattr(self.brain, 'getAverageFiringRate'):
            neural_state['firing_rate'] = self.brain.getAverageFiringRate()
        if hasattr(self.brain, 'getExcitationInhibitionRatio'):
            neural_state['e_i_ratio'] = self.brain.getExcitationInhibitionRatio()
        
        return neural_state
    
    def _capture_memory_usage(self) -> Dict[str, float]:
        """Capture current memory usage."""
        try:
            process = psutil.Process()
            return {
                'rss_mb': process.memory_info().rss / 1024 / 1024,
                'vms_mb': process.memory_info().vms / 1024 / 1024,
                'memory_percent': process.memory_percent()
            }
        except Exception:
            return {'error': 'Could not capture memory usage'}
    
    def _capture_performance_metrics(self) -> Dict[str, float]:
        """Capture current performance metrics."""
        metrics = {}
        
        # Add performance metrics as they become available
        metrics['step_rate'] = 1.0  # Placeholder
        
        return metrics
    
    def _update_performance_profile(self, state: StateSnapshot):
        """Update performance profile with new state."""
        self.performance_profile['memory_usage'].append(state.memory_usage.get('rss_mb', 0))
        self.performance_profile['active_neurons'].append(state.neural_state.get('active_neurons', 0))
        self.performance_profile['firing_rates'].append(state.neural_state.get('firing_rate', 0))
        self.performance_profile['e_i_ratios'].append(state.neural_state.get('e_i_ratio', 0))
        self.performance_profile['timestamps'].append(state.timestamp)
        
        # Keep profile manageable
        max_length = 1000
        for key in self.performance_profile:
            if len(self.performance_profile[key]) > max_length:
                self.performance_profile[key] = self.performance_profile[key][-max_length:]
    
    def _detect_anomalies(self, state: StateSnapshot):
        """Detect anomalies in the given state."""
        anomalies = self.anomaly_detector.detect_anomalies(state)
        
        # Store detected anomalies
        for anomaly in anomalies:
            if 'step_anomalies' not in self.__dict__:
                self.step_anomalies = {}
            if state.step not in self.step_anomalies:
                self.step_anomalies[state.step] = []
            self.step_anomalies[state.step].append(anomaly)
    
    def _detect_state_changes(self):
        """Detect significant changes between recent states."""
        if len(self.state_history) < 2:
            return
        
        recent_states = self.state_history[-10:]
        for i in range(1, len(recent_states)):
            change = self.change_detector.detect_change(recent_states[i-1], recent_states[i])
            if change['significant']:
                self._log_info(f"Significant state change detected between step {recent_states[i-1].step} and {recent_states[i].step}: {change['description']}")
    
    def _analyze_memory_trends(self) -> Dict[str, Any]:
        """Analyze memory usage trends."""
        if not self.performance_profile['memory_usage']:
            return {}
        
        memory = self.performance_profile['memory_usage']
        if len(memory) < 2:
            return {'insufficient_data': True}
        
        # Calculate trends
        recent_trend = np.polyfit(range(len(memory)), memory, 1)[0] if len(memory) >= 2 else 0
        
        return {
            'current_memory': memory[-1],
            'average_memory': np.mean(memory),
            'memory_growth_rate': recent_trend,
            'memory_volatility': np.std(memory) if len(memory) > 1 else 0,
            'memory_efficiency': np.mean(memory) / max(memory[-1], 1) if memory else 0
        }
    
    def _get_peak_memory(self) -> Dict[str, float]:
        """Get peak memory usage."""
        if not self.performance_profile['memory_usage']:
            return {'current': 0, 'peak': 0}
        
        return {
            'current': self.performance_profile['memory_usage'][-1],
            'peak': max(self.performance_profile['memory_usage'])
        }
    
    def _calculate_memory_growth_rate(self) -> float:
        """Calculate memory growth rate."""
        if len(self.performance_profile['memory_usage']) < 2:
            return 0.0
        
        memory = self.performance_profile['memory_usage']
        if len(memory) < 2:
            return 0.0
        
        recent_trend = np.polyfit(range(len(memory)), memory, 1)[0]
        return recent_trend
    
    def _calculate_memory_efficiency(self) -> float:
        """Calculate memory efficiency."""
        if not self.performance_profile['memory_usage'] or not self.performance_profile['active_neurons']:
            return 0.0
        
        memory = self.performance_profile['memory_usage']
        neurons = self.performance_profile['active_neurons']
        
        if len(memory) != len(neurons):
            return 0.0
        
        # Calculate neurons per MB of memory
        efficiency = []
        for m, n in zip(memory, neurons):
            efficiency.append(n / max(m, 1))
        
        return np.mean(efficiency) if efficiency else 0.0
    
    def _extract_activity_patterns(self) -> Dict[str, Any]:
        """Extract activity patterns."""
        if not self.performance_profile['firing_rates']:
            return {}
        
        firing_rates = self.performance_profile['firing_rates']
        
        return {
            'mean_firing_rate': np.mean(firing_rates),
            'firing_rate_std': np.std(firing_rates),
            'firing_rate_trend': np.polyfit(range(len(firing_rates)), firing_rates, 1)[0] if len(firing_rates) >= 2 else 0,
            'activity_volatility': np.std(firing_rates) / max(np.mean(firing_rates), 1)
        }
    
    def _analyze_regional_dynamics(self) -> Dict[str, Any]:
        """Analyze regional dynamics."""
        # This would require access to regional state history
        return {'status': 'not_implemented'}
    
    def _analyze_temporal_patterns(self) -> Dict[str, Any]:
        """Analyze temporal patterns."""
        return {'status': 'not_implemented'}
    
    def _analyze_correlations(self) -> Dict[str, Any]:
        """Analyze correlations between metrics."""
        if len(self.performance_profile['memory_usage']) < 2:
            return {}
        
        memory = self.performance_profile['memory_usage']
        neurons = self.performance_profile['active_neurons']
        
        if len(memory) != len(neurons):
            return {}
        
        correlations = {
            'memory_vs_neurons': np.corrcoef(memory, neurons)[0, 1] if len(set(memory + neurons)) > 1 else 0,
            'memory_vs_firing': np.corrcoef(memory, self.performance_profile['firing_rates'])[0, 1] if len(set(memory + self.performance_profile['firing_rates'])) > 1 else 0,
            'neurons_vs_firing': np.corrcoef(neurons, self.performance_profile['firing_rates'])[0, 1] if len(set(neurons + self.performance_profile['firing_rates'])) > 1 else 0
        }
        
        return correlations
    
    def _get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary."""
        return {
            'total_steps': len(self.state_history),
            'current_step': self.state_history[-1].step if self.state_history else 0,
            'performance_efficiency': self._calculate_memory_efficiency(),
            'activity_stability': np.std(self.performance_profile['firing_rates']) if self.performance_profile['firing_rates'] else 0
        }
    
    def _get_change_summary(self) -> Dict[str, Any]:
        """Get change summary."""
        return {
            'significant_changes_detected': getattr(self, 'significant_changes', 0),
            'anomalies_detected': getattr(self, 'anomaly_count', 0),
            'baseline_set': self.baseline_state is not None
        }
    
    def _calculate_similarity(self, state1: StateSnapshot, state2: StateSnapshot) -> float:
        """Calculate similarity between two states."""
        # Simple similarity calculation based on neural state
        if 'regions' not in state1.neural_state or 'regions' not in state2.neural_state:
            return 0.0
        
        regions1 = state1.neural_state['regions']
        regions2 = state2.neural_state['regions']
        
        if not regions1 or not regions2:
            return 0.0
        
        similarities = []
        for region_id in regions1:
            if region_id in regions2:
                # Compare firing rates
                rate1 = regions1[region_id].get('firing_rate', 0)
                rate2 = regions2[region_id].get('firing_rate', 0)
                similarity = 1.0 - abs(rate1 - rate2) / max(abs(rate1) + abs(rate2), 1)
                similarities.append(similarity)
        
        return np.mean(similarities) if similarities else 0.0
    
    def _compare_states(self, state1: StateSnapshot, state2: StateSnapshot) -> Dict[str, Any]:
        """Compare two states and return differences."""
        differences = {}
        
        # Compare neural states
        if 'regions' in state1.neural_state and 'regions' in state2.neural_state:
            regions1 = state1.neural_state['regions']
            regions2 = state2.neural_state['regions']
            
            for region_id in regions1:
                if region_id in regions2:
                    diff = {}
                    for key in regions1[region_id]:
                        if key in regions2[region_id]:
                            val1 = regions1[region_id][key]
                            val2 = regions2[region_id][key]
                            if isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
                                if abs(val1 - val2) > 0.01:
                                    diff[key] = {'before': val1, 'after': val2, 'difference': val2 - val1}
                    
                    if diff:
                        differences[f'region_{region_id}'] = diff
        
        # Compare spikes
        if 'total_spikes' in state1.neural_state and 'total_spikes' in state2.neural_state:
            diff = state2.neural_state['total_spikes'] - state1.neural_state['total_spikes']
            if abs(diff) > 0:
                differences['spikes'] = {'before': state1.neural_state['total_spikes'], 'after': state2.neural_state['total_spikes'], 'difference': diff}
        
        return differences
    
    def _log_warning(self, message: str):
        """Log warning message."""
        print(f"[INSPECTOR WARNING] {message}")
    
    def _log_info(self, message: str):
        """Log info message."""
        print(f"[INSPECTOR INFO] {message}")