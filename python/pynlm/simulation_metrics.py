"""
Simulation metrics collection and analysis utilities.
"""

from typing import Dict, Any, List, Optional, Tuple
import numpy as np

from .nlm_exception import ValidationError


class SimulationMetrics:
    """
    Helper class for collecting and analyzing simulation metrics.
    
    Provides comprehensive metrics collection for NLM simulations including:
    - Basic brain statistics (neuron count, firing rates, etc.)
    - Agent and world statistics (position, energy, etc.)
    - Development and neuromodulation metrics
    - Performance analysis and comparison
    - Metric formatting and reporting
    """
    
    @staticmethod
    def collect(brain: 'Brain', world: Optional['SimpleWorld'] = None, 
                agent: Optional['AgentBrain'] = None) -> Dict[str, Any]:
        """
        Collect comprehensive simulation metrics from brain and optional world/agent.
        
        Args:
            brain: Brain instance to collect metrics from
            world: World instance (optional)
            agent: Agent instance (optional)
            
        Returns:
            Dictionary containing all collected metrics
            
        Raises:
            ValidationError: If brain is invalid or metrics collection fails
        """
        if not brain:
            raise ValidationError("Brain instance cannot be None")
        
        # Basic brain metrics
        metrics = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'pending_spike_event_count': brain.getPendingSpikeEventCount(),
        }
        
        # World metrics
        if world:
            body = world.getAgentBody()
            metrics.update({
                'world_time': world.getSimulationTime(),
                'world_width': world.getWidth(),
                'world_height': world.getHeight(),
                'agent_x': body.x,
                'agent_y': body.y,
                'agent_orientation': body.orientation,
                'agent_velocity_x': body.velocityX,
                'agent_velocity_y': body.velocityY,
                'agent_angular_velocity': body.angularVelocity,
                'agent_energy': body.energy,
                'agent_health': body.health,
                'agent_age': body.age,
                'agent_is_moving': body.isMoving,
                'agent_is_turning': body.isTurning,
                'agent_last_action_time': body.lastActionTime,
                'world_max_energy': world.getMaxEnergy(),
                'world_energy_decay_rate': world.getEnergyDecayRate(),
                'world_random_seed': world.getRandomSeed(),
            })
        
        # Agent metrics
        if agent:
            metrics.update({
                'agent_curiosity_level': agent.getCuriosityLevel(),
                'agent_novelty_level': agent.getNoveltyLevel(),
                'agent_prediction_error': agent.getPredictionError(),
                'agent_neuromodulation_level': agent.getNeuromodulationLevel(),
                'agent_reward_modulation_enabled': agent.isRewardModulationEnabled(),
                'agent_structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
                'agent_development_enabled': agent.isDevelopmentEnabled(),
                'agent_curiosity_enabled': agent.isCuriosityEnabled(),
                'agent_sensory_input_size': agent.getSensoryInputSize(),
                'agent_motor_output_size': agent.getMotorOutputSize(),
            })
        
        return metrics
    
    @staticmethod
    def format(metrics: Dict[str, Any], precision: int = 3, 
               include_world: bool = True, include_agent: bool = True) -> str:
        """
        Format metrics dictionary as a human-readable string.
        
        Args:
            metrics: Metrics dictionary to format
            precision: Number of decimal places for floating-point values
            include_world: Whether to include world metrics in output
            include_agent: Whether to include agent metrics in output
            
        Returns:
            Formatted metrics string
        """
        lines = ["Simulation Metrics:", "=" * 50]
        
        # Basic brain metrics (always included)
        brain_metrics = {
            'neuron_count': 'Neurons',
            'synapse_count': 'Synapses',
            'firing_neuron_count': 'Firing Neurons',
            'total_spike_count': 'Total Spikes',
            'average_firing_rate': 'Average Firing Rate',
            'excitation_inhibition_ratio': 'E/I Ratio',
            'developmental_stage': 'Development Stage',
            'active_neuron_count': 'Active Neurons',
            'pending_spike_event_count': 'Pending Spike Events',
        }
        
        lines.append("\nBrain Metrics:")
        lines.append("-" * 30)
        for key, label in brain_metrics.items():
            if key not in metrics:
                continue
            
            value = metrics[key]
            formatted = SimulationMetrics._format_value(value, precision)
            lines.append(f"{label:25}: {formatted}")
        
        # World metrics
        if include_world and 'agent_x' in metrics:
            lines.append("\nWorld Metrics:")
            lines.append("-" * 30)
            
            world_metrics = [
                ('agent_x', 'Agent X Position'),
                ('agent_y', 'Agent Y Position'),
                ('agent_energy', 'Agent Energy'),
                ('agent_health', 'Agent Health'),
                ('agent_age', 'Agent Age'),
                ('world_time', 'World Time'),
                ('world_max_energy', 'Max World Energy'),
                ('world_energy_decay_rate', 'Energy Decay Rate'),
            ]
            
            for key, label in world_metrics:
                if key not in metrics:
                    continue
                
                value = metrics[key]
                formatted = SimulationMetrics._format_value(value, precision)
                lines.append(f"{label:25}: {formatted}")
        
        # Agent metrics
        if include_agent and 'agent_curiosity_level' in metrics:
            lines.append("\nAgent Metrics:")
            lines.append("-" * 30)
            
            agent_metrics = [
                ('agent_curiosity_level', 'Curiosity Level'),
                ('agent_novelty_level', 'Novelty Level'),
                ('agent_prediction_error', 'Prediction Error'),
                ('agent_neuromodulation_level', 'Neuromodulation Level'),
                ('agent_reward_modulation_enabled', 'Learning Enabled'),
                ('agent_structural_plasticity_enabled', 'Structural Plasticity'),
                ('agent_development_enabled', 'Development Enabled'),
                ('agent_curiosity_enabled', 'Curiosity Enabled'),
                ('agent_sensory_input_size', 'Sensory Input Size'),
                ('agent_motor_output_size', 'Motor Output Size'),
            ]
            
            for key, label in agent_metrics:
                if key not in metrics:
                    continue
                
                value = metrics[key]
                formatted = SimulationMetrics._format_value(value, precision)
                lines.append(f"{label:25}: {formatted}")
        
        # Add summary statistics
        lines.append("\nSummary:")
        lines.append("-" * 30)
        
        if 'average_firing_rate' in metrics:
            firing_rate = metrics['average_firing_rate']
            lines.append(f"Firing Rate: {firing_rate:.{precision}f} Hz")
        
        if 'excitation_inhibition_ratio' in metrics:
            e_i_ratio = metrics['excitation_inhibition_ratio']
            balance = "Balanced" if abs(e_i_ratio - 1.0) < 0.1 else "Unbalanced"
            lines.append(f"Excitatory/Inhibitory Balance: {e_i_ratio:.{precision}f} ({balance})")
        
        if 'agent_energy' in metrics:
            energy = metrics['agent_energy']
            energy_percent = (energy / metrics.get('world_max_energy', 100)) * 100
            lines.append(f"Agent Energy: {energy:.{precision}f} ({energy_percent:.{precision}f}% of max)")
        
        return "\n".join(lines)
    
    @staticmethod
    def _format_value(value: Any, precision: int) -> str:
        """Format a value for display."""
        if value is None:
            return "N/A"
        
        if isinstance(value, (int, np.integer)):
            return str(value)
        elif isinstance(value, (float, np.floating)):
            return f"{value:.{precision}f}"
        elif isinstance(value, tuple):
            formatted = ", ".join([SimulationMetrics._format_value(v, precision) for v in value])
            return f"({formatted})"
        else:
            return str(value)
    
    @staticmethod
    def calculate_derivatives(metrics_list: List[Dict[str, Any]]) -> Dict[str, List[float]]:
        """
        Calculate derivatives (rates of change) from a list of metrics dictionaries.
        
        Args:
            metrics_list: List of metrics dictionaries (usually time-series data)
            
        Returns:
            Dictionary of derivative values for each metric
        """
        if len(metrics_list) < 2:
            return {}
        
        derivatives = {}
        
        for key in metrics_list[0].keys():
            values = [m[key] for m in metrics_list if key in m]
            if len(values) >= 2:
                # Calculate simple forward difference
                deriv = [values[i+1] - values[i] for i in range(len(values)-1)]
                derivatives[key] = deriv
        
        return derivatives
    
    @staticmethod
    def analyze_trends(metrics_list: List[Dict[str, Any]], 
                      window: int = 5) -> Dict[str, str]:
        """
        Analyze trends in metrics over time.
        
        Args:
            metrics_list: List of metrics dictionaries (time-series data)
            window: Window size for moving average calculation
            
        Returns:
            Dictionary of trend descriptions for each metric
        """
        trends = {}
        
        if len(metrics_list) < window:
            return {"error": f"Need at least {window} data points for trend analysis"}
        
        for key in metrics_list[0].keys():
            values = [m[key] for m in metrics_list if key in m]
            if len(values) < window:
                continue
            
            # Calculate moving average
            moving_averages = []
            for i in range(len(values) - window + 1):
                avg = sum(values[i:i+window]) / window
                moving_averages.append(avg)
            
            # Determine trend
            if len(moving_averages) >= 2:
                first_half = moving_averages[:len(moving_averages)//2]
                second_half = moving_averages[len(moving_averages)//2:]
                
                if len(first_half) > 0 and len(second_half) > 0:
                    first_avg = sum(first_half) / len(first_half)
                    second_avg = sum(second_half) / len(second_half)
                    
                    if abs(second_avg - first_avg) < 0.001 * abs(first_avg):
                        trend = "Stable"
                    elif second_avg > first_avg:
                        trend = "Increasing"
                    else:
                        trend = "Decreasing"
                    
                    trends[key] = trend
        
        return trends
    
    @staticmethod
    def compare_metrics(baseline_metrics: Dict[str, Any], 
                        test_metrics: Dict[str, Any]) -> Dict[str, Any]:
        """
        Compare two sets of metrics and highlight differences.
        
        Args:
            baseline_metrics: Baseline metrics
            test_metrics: Test metrics to compare
            
        Returns:
            Dictionary of comparison results
        """
        comparison = {
            'differences': {},
            'similarities': {},
            'summary': "",
        }
        
        all_keys = set(baseline_metrics.keys()) | set(test_metrics.keys())
        
        for key in all_keys:
            if key not in baseline_metrics or key not in test_metrics:
                continue
            
            baseline_val = baseline_metrics[key]
            test_val = test_metrics[key]
            
            if isinstance(baseline_val, (int, float)) and isinstance(test_val, (int, float)):
                diff = test_val - baseline_val
                rel_diff = (diff / baseline_val * 100) if baseline_val != 0 else float('inf')
                
                comparison['differences'][key] = {
                    'baseline': baseline_val,
                    'test': test_val,
                    'absolute_difference': diff,
                    'relative_difference_percent': rel_diff,
                }
            else:
                if baseline_val == test_val:
                    comparison['similarities'][key] = baseline_val
                else:
                    comparison['differences'][key] = {
                        'baseline': baseline_val,
                        'test': test_val,
                        'note': 'Different (non-numeric)',
                    }
        
        # Generate summary
        num_diff = len(comparison['differences'])
        num_sim = len(comparison['similarities'])
        
        if num_diff == 0:
            comparison['summary'] = "Metrics are identical"
        elif num_sim == 0:
            comparison['summary'] = f"All {num_diff} metrics differ"
        else:
            comparison['summary'] = f"{num_sim} metrics similar, {num_diff} metrics differ"
        
        return comparison
    
    @staticmethod
    def export_metrics(metrics_list: List[Dict[str, Any]], 
                      filename: str, format: str = 'csv') -> bool:
        """
        Export metrics to a file.
        
        Args:
            metrics_list: List of metrics dictionaries to export
            filename: Output filename
            format: Export format ('csv', 'json', 'tsv')
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if format == 'csv':
                import csv
                if not metrics_list:
                    return False
                
                with open(filename, 'w', newline='') as csvfile:
                    writer = csv.writer(csvfile)
                    
                    # Write header
                    writer.writerow(metrics_list[0].keys())
                    
                    # Write data
                    for metrics in metrics_list:
                        writer.writerow([metrics.get(key, '') for key in metrics_list[0].keys()])
            
            elif format == 'json':
                import json
                with open(filename, 'w') as jsonfile:
                    json.dump(metrics_list, jsonfile, indent=2, default=str)
            
            elif format == 'tsv':
                import csv
                if not metrics_list:
                    return False
                
                with open(filename, 'w', newline='') as tsvfile:
                    writer = csv.writer(tsvfile, delimiter='\t')
                    
                    # Write header
                    writer.writerow(metrics_list[0].keys())
                    
                    # Write data
                    for metrics in metrics_list:
                        writer.writerow([metrics.get(key, '') for key in metrics_list[0].keys()])
            
            else:
                raise ValidationError(f"Unsupported format: {format}")
            
            return True
        
        except Exception as e:
            raise ValidationError(f"Failed to export metrics: {e}")
    
    @staticmethod
    def import_metrics(filename: str, format: str = 'csv') -> List[Dict[str, Any]]:
        """
        Import metrics from a file.
        
        Args:
            filename: Input filename
            format: Import format ('csv', 'json', 'tsv')
            
        Returns:
            List of metrics dictionaries
        """
        try:
            if format == 'csv':
                import csv
                with open(filename, 'r') as csvfile:
                    reader = csv.DictReader(csvfile)
                    return list(reader)
            
            elif format == 'json':
                import json
                with open(filename, 'r') as jsonfile:
                    return json.load(jsonfile)
            
            elif format == 'tsv':
                import csv
                with open(filename, 'r') as tsvfile:
                    reader = csv.DictReader(tsvfile, delimiter='\t')
                    return list(reader)
            
            else:
                raise ValidationError(f"Unsupported format: {format}")
        
        except Exception as e:
            raise ValidationError(f"Failed to import metrics: {e}")


__all__ = ['SimulationMetrics']