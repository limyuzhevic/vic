"""Utility functions for NLM Python interface"""

import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Union
from pathlib import Path
from datetime import datetime
class ConfigurationValidator:
    """Validate and normalize NLM configurations."""
    
    def __init__(self):
        self.validation_rules = self._load_validation_rules()
    
    def _load_validation_rules(self) -> Dict[str, Any]:
        """Load validation rules for configuration parameters."""
        return {
            'brain': {
                'neuron_count': {'type': int, 'min': 1, 'max': 100000, 'default': 1000},
                'synapse_density': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.1},
                'connection_probability': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.05},
                'v_thresh': {'type': float, 'default': -50.0},
                'v_rest': {'type': float, 'default': -70.0},
                'v_reset': {'type': float, 'default': -75.0},
                'tau_mem': {'type': float, 'min': 1.0, 'max': 1000.0, 'default': 20.0},
                'tau_ref': {'type': float, 'min': 0.1, 'max': 100.0, 'default': 2.0}
            },
            'plasticity': {
                'stdp': {'type': dict, 'defaults': {'enable': True, 'learning_rate': 0.001}},
                'hebbian': {'type': dict, 'defaults': {'enable': True}},
                'structural': {'type': dict, 'defaults': {'enable': True}}
            },
            'neuromod': {
                'dopamine': {'type': dict, 'defaults': {'scale': 1.0}},
                'curiosity': {'type': dict, 'defaults': {'enable': True}},
                'novelty': {'type': dict, 'defaults': {'enable': True}}
            }
        }
    
    def validate_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration and return validation results."""
        errors = []
        warnings = []
        normalized_config = {}
        
        # Validate brain section
        if 'brain' in config:
            brain_config = self._validate_section('brain', config['brain'])
            normalized_config['brain'] = brain_config
            errors.extend(brain_config.get('errors', []))
            warnings.extend(brain_config.get('warnings', []))
        
        # Validate plasticity section
        if 'plasticity' in config:
            plasticity_config = self._validate_section('plasticity', config['plasticity'])
            normalized_config['plasticity'] = plasticity_config
            errors.extend(plasticity_config.get('errors', []))
            warnings.extend(plasticity_config.get('warnings', []))
        
        # Validate neuromod section
        if 'neuromod' in config:
            neuromod_config = self._validate_section('neuromod', config['neuromod'])
            normalized_config['neuromod'] = neuromod_config
            errors.extend(neuromod_config.get('errors', []))
            warnings.extend(neuromod_config.get('warnings', []))
        
        # Merge with defaults
        normalized_config = self._apply_defaults(normalized_config)
        
        return {
            'valid': len(errors) == 0,
            'errors': errors,
            'warnings': warnings,
            'normalized_config': normalized_config
        }
    
    def _validate_section(self, section_name: str, section_config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate a configuration section."""
        errors = []
        warnings = []
        validated_config = {}
        
        if section_name not in self.validation_rules:
            warnings.append(f"Unknown configuration section: {section_name}")
            return {'config': section_config, 'errors': errors, 'warnings': warnings}
        
        rules = self.validation_rules[section_name]
        
        for key, value in section_config.items():
            if key in rules:
                rule = rules[key]
                validated_value = self._validate_value(key, value, rule)
                validated_config[key] = validated_value
                
                if validated_value.get('invalid'):
                    errors.append(f"Invalid value for {section_name}.{key}: {value}")
            
            else:
                warnings.append(f"Unknown parameter {section_name}.{key}")
                validated_config[key] = value
        
        return {
            'config': validated_config,
            'errors': errors,
            'warnings': warnings
        }
    
    def _validate_value(self, key: str, value: Any, rule: Dict[str, Any]) -> Any:
        """Validate a single value against its rule."""
        if rule['type'] == type(value):
            # Type matches, validate range if specified
            if 'min' in rule and value < rule['min']:
                return {'value': value, 'invalid': True, 'reason': f"Below minimum {rule['min']}"}
            
            if 'max' in rule and value > rule['max']:
                return {'value': value, 'invalid': True, 'reason': f"Above maximum {rule['max']}"}
            
            return {'value': value, 'valid': True}
        
        else:
            # Try to convert to the expected type
            try:
                if rule['type'] == int:
                    converted = int(value)
                elif rule['type'] == float:
                    converted = float(value)
                elif rule['type'] == bool:
                    converted = bool(value)
                else:
                    return {'value': value, 'invalid': True, 'reason': f"Wrong type {type(value)}, expected {rule['type']}"}
                
                # Validate range after conversion
                if 'min' in rule and converted < rule['min']:
                    return {'value': converted, 'invalid': True, 'reason': f"Below minimum {rule['min']}"}
                
                if 'max' in rule and converted > rule['max']:
                    return {'value': converted, 'invalid': True, 'reason': f"Above maximum {rule['max']}"}
                
                return {'value': converted, 'valid': True, 'converted': True}
            
            except (ValueError, TypeError):
                return {'value': value, 'invalid': True, 'reason': f"Cannot convert to {rule['type']}"}
    
    def _apply_defaults(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Apply default values for missing parameters."""
        for section, rules in self.validation_rules.items():
            if section not in config:
                config[section] = {}
            
            for key, rule in rules.items():
                if key not in config[section] and 'default' in rule:
                    config[section][key] = rule['default']
                
                elif key in config[section] and isinstance(rule.get('defaults'), dict):
                    # Merge sub-dict defaults
                    if isinstance(config[section][key], dict):
                        sub_defaults = rule['defaults']
                        for sub_key, sub_default in sub_defaults.items():
                            if sub_key not in config[section][key]:
                                config[section][key][sub_key] = sub_default
        
        return config
    
    def create_test_config(self, complexity: str = 'simple') -> Dict[str, Any]:
        """Create a test configuration with specified complexity."""
        configs = {
            'simple': {
                'brain': {
                    'neuron_count': 100,
                    'synapse_density': 0.1,
                    'connection_probability': 0.05
                },
                'plasticity': {
                    'stdp': {'enable': True, 'learning_rate': 0.001},
                    'hebbian': {'enable': True}
                }
            },
            'moderate': {
                'brain': {
                    'neuron_count': 1000,
                    'synapse_density': 0.15,
                    'connection_probability': 0.08,
                    'v_thresh': -50.0,
                    'v_rest': -70.0,
                    'tau_mem': 20.0
                },
                'plasticity': {
                    'stdp': {'enable': True, 'learning_rate': 0.002, 'tau_plus': 20.0},
                    'hebbian': {'enable': True},
                    'structural': {'enable': True}
                },
                'neuromod': {
                    'dopamine': {'scale': 1.0},
                    'curiosity': {'enable': True, 'scale': 0.5},
                    'novelty': {'enable': True}
                }
            },
            'complex': {
                'brain': {
                    'neuron_count': 5000,
                    'synapse_density': 0.2,
                    'connection_probability': 0.1,
                    'v_thresh': -45.0,
                    'v_rest': -65.0,
                    'tau_mem': 25.0,
                    'tau_ref': 3.0
                },
                'plasticity': {
                    'stdp': {'enable': True, 'learning_rate': 0.003, 'tau_plus': 25.0, 'tau_minus': 25.0},
                    'hebbian': {'enable': True},
                    'structural': {'enable': True, 'synaptogenesis_rate': 0.0001}
                },
                'neuromod': {
                    'dopamine': {'scale': 1.0},
                    'curiosity': {'enable': True, 'scale': 0.8},
                    'novelty': {'enable': True, 'scale': 0.6},
                    'prediction_error': {'enable': True}
                }
            }
        }
        
        return configs.get(complexity, configs['simple'])
class OutputFormatter:
    """Format simulation output for different purposes."""
    
    @staticmethod
    def format_json(data: Dict[str, Any], indent: int = 2) -> str:
        """Format data as JSON."""
        return json.dumps(data, indent=indent, default=str)
    
    @staticmethod
    def format_csv(data: List[Dict[str, Any]], 
                  metrics: List[str] = None) -> str:
        """Format data as CSV."""
        if not data:
            return ""
        
        if metrics is None:
            # Use all available keys from first record
            metrics = list(data[0].keys())
        
        import io
        import csv
        
        output = io.StringIO()
        writer = csv.writer(output)
        
        # Write header
        writer.writerow(metrics)
        
        # Write data rows
        for record in data:
            row = []
            for metric in metrics:
                if '.' in metric:
                    # Handle nested metrics
                    value = OutputFormatter._get_nested_value(record, metric)
                    row.append(value)
                else:
                    row.append(record.get(metric))
            writer.writerow(row)
        
        return output.getvalue()
    
    @staticmethod
    def _get_nested_value(data: Dict[str, Any], key_path: str) -> Any:
        """Get nested value using dot notation."""
        keys = key_path.split('.')
        value = data
        
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                return None
        
        return value
    
    @staticmethod
    def format_report(data: Dict[str, Any], 
                     report_type: str = 'summary') -> str:
        """Format data as human-readable report."""
        if report_type == 'summary':
            return OutputFormatter._format_summary_report(data)
        elif report_type == 'detailed':
            return OutputFormatter._format_detailed_report(data)
        else:
            return OutputFormatter._format_json_report(data)
    
    @staticmethod
    def _format_summary_report(data: Dict[str, Any]) -> str:
        """Format summary report."""
        lines = ["NLM Simulation Report", "=" * 50]
        
        if 'brain' in data:
            brain = data['brain']
            lines.append(f"Brain Configuration:")
            lines.append(f"  Neurons: {brain.get('neuron_count', 'N/A')}")
            lines.append(f"  Synapses: {brain.get('synapse_count', 'N/A')}")
            lines.append(f"  E/I Ratio: {brain.get('e_i_ratio', 'N/A'):.3f}")
        
        if 'statistics' in data:
            stats = data['statistics']
            lines.append(f"\nSimulation Statistics:")
            lines.append(f"  Total Steps: {stats.get('total_steps', 'N/A')}")
            lines.append(f"  Total Spikes: {stats.get('total_spikes', 'N/A')}")
            lines.append(f"  Average Firing Rate: {stats.get('average_firing_rate', 'N/A'):.3f}")
        
        return "\n".join(lines)
    
    @staticmethod
    def _format_detailed_report(data: Dict[str, Any]) -> str:
        """Format detailed report."""
        lines = ["NLM Simulation Detailed Report", "=" * 60]
        
        for section, content in data.items():
            lines.append(f"\n{section.upper().replace('_', ' ')}:")
            
            if isinstance(content, dict):
                for key, value in content.items():
                    if isinstance(value, dict):
                        lines.append(f"  {key}:")
                        for sub_key, sub_value in value.items():
                            lines.append(f"    {sub_key}: {sub_value}")
                    else:
                        lines.append(f"  {key}: {value}")
            
            elif isinstance(content, list):
                lines.append(f"  Items: {len(content)}")
                if len(content) <= 10:
                    for item in content:
                        lines.append(f"    - {item}")
        
        return "\n".join(lines)
    
    @staticmethod
    def _format_json_report(data: Dict[str, Any]) -> str:
        """Format JSON report."""
        return json.dumps(data, indent=2, default=str)
class VisualizationHelper:
    """Helper functions for visualizing simulation results."""
    
    def __init__(self, style: str = 'default'):
        self.style = style
        self._setup_plot_style()
    
    def _setup_plot_style(self):
        """Setup matplotlib plot style."""
        if self.style == 'dark':
            plt.style.use('dark_background')
        elif self.style == 'scientific':
            plt.style.use('seaborn-v0_8-whitegrid')
        else:
            plt.style.use('default')
    
    def plot_neural_activity(self, data: List[Dict[str, Any]], 
                            output_path: str = None,
                            title: str = "Neural Activity"):
        """Plot neural activity time series."""
        if not data:
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle(title, fontsize=16)
        
        # Extract data
        steps = [d.get('step', i) for i, d in enumerate(data)]
        firing_rates = [d.get('average_firing_rate', 0) for d in data]
        active_neurons = [d.get('active_neurons', 0) for d in data]
        total_spikes = [d.get('total_spikes', 0) for d in data]
        e_i_ratios = [d.get('e_i_ratio', 0) for d in data]
        
        # Plot 1: Firing rate
        axes[0, 0].plot(steps, firing_rates, 'b-', linewidth=1, label='Firing Rate')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Rate (Hz)')
        axes[0, 0].set_title('Neural Firing Rate')
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Active neurons
        axes[0, 1].plot(steps, active_neurons, 'r-', linewidth=1, label='Active Neurons')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Number of Neurons')
        axes[0, 1].set_title('Active Neurons')
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Total spikes
        axes[1, 0].plot(steps, total_spikes, 'g-', linewidth=1, label='Total Spikes')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Total Spikes')
        axes[1, 0].set_title('Cumulative Spike Count')
        axes[1, 0].legend()
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: E/I ratio
        axes[1, 1].plot(steps, e_i_ratios, 'm-', linewidth=1, label='E/I Ratio')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('E/I Ratio')
        axes[1, 1].set_title('Excitatory/Inhibitory Balance')
        axes[1, 1].legend()
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=150, bbox_inches='tight')
        
        return fig
    
    def plot_neuron_distribution(self, brain_data: Dict[str, Any],
                                output_path: str = None,
                                title: str = "Neural Population Distribution"):
        """Plot neuron type distribution."""
        import matplotlib.pyplot as plt
        
        # This would require accessing region data from the brain
        # For now, return a placeholder
        fig, ax = plt.subplots(figsize=(10, 6))
        ax.text(0.5, 0.5, "Neuron Distribution Plot\n(Data would be extracted from brain regions)",
                ha='center', va='center', transform=ax.transAxes, fontsize=14)
        ax.set_title(title)
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        
        if output_path:
            plt.savefig(output_path, dpi=150, bbox_inches='tight')
        
        return fig
    
    def create_animation(self, data: List[Dict[str, Any]],
                         output_path: str = None,
                         fps: int = 10) -> bool:
        """Create animation of simulation progression."""
        try:
            import matplotlib.animation as animation
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle("NLM Simulation Animation", fontsize=16)
            
            # Initialize plot lines
            lines = []
            colors = ['blue', 'red', 'green', 'magenta']
            
            for i in range(4):
                line, = axes[i//2, i%2].plot([], [], color=colors[i], linewidth=2)
                lines.append(line)
                axes[i//2, i%2].set_xlim(0, 1000) if i == 0 else axes[i//2, i%2].set_xlim(0, 100)
                axes[i//2, i%2].set_ylim(0, 1)
                axes[i//2, i%2].grid(True, alpha=0.3)
            
            def init():
                for line in lines:
                    line.set_data([], [])
                return lines
            
            def animate(frame):
                # Update data for each frame
                max_frame = min(frame, len(data) - 1)
                current_data = data[:max_frame + 1]
                
                if current_data:
                    steps = [d.get('step', i) for i, d in enumerate(current_data)]
                    firing_rates = [d.get('average_firing_rate', 0) for d in current_data]
                    active_neurons = [d.get('active_neurons', 0) for d in current_data]
                    total_spikes = [d.get('total_spikes', 0) for d in current_data]
                    
                    # Update plot data (show last 100 points for performance)
                    show_last = min(100, len(steps))
                    line_data = {
                        'firing_rates': firing_rates[-show_last:],
                        'active_neurons': active_neurons[-show_last:],
                        'total_spikes': total_spikes[-show_last:]
                    }
                    
                    x_data = steps[-show_last:]
                    
                    lines[0].set_data(x_data, line_data['firing_rates'])
                    lines[1].set_data(x_data, line_data['active_neurons'])
                    lines[2].set_data(x_data, [s/max(total_spikes[-show_last:], [1]) for s in line_data['total_spikes']])
                    lines[3].set_data(x_data, [min(1, r/10) for r in line_data['firing_rates']])  # Normalized firing rate
                
                return lines
            
            anim = animation.FuncAnimation(fig, animate, init_func=init,
                                         frames=len(data), interval=1000/fps, blit=True)
            
            if output_path:
                anim.save(output_path, writer='ffmpeg', fps=fps)
            
            plt.close(fig)
            return True
        
        except ImportError:
            print("Animation requires ffmpeg. Install with: pip install ffmpeg-python")
            return False
    
    def plot_performance_metrics(self, performance_data: Dict[str, Any],
                                output_path: str = None,
                                title: str = "Performance Metrics"):
        """Plot performance metrics over time."""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle(title, fontsize=16)
        
        # Extract metrics
        steps = list(performance_data.keys())
        
        # Step time metrics
        if 'step_times' in performance_data:
            axes[0, 0].plot(steps, performance_data['step_times'], 'r-', alpha=0.7)
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Step Time (s)')
            axes[0, 0].set_title('Step Execution Time')
            axes[0, 0].grid(True, alpha=0.3)
        
        # Memory usage
        if 'memory_usage' in performance_data:
            axes[0, 1].plot(steps, performance_data['memory_usage'], 'g-', alpha=0.7)
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Memory (MB)')
            axes[0, 1].set_title('Memory Usage')
            axes[0, 1].grid(True, alpha=0.3)
        
        # CPU usage
        if 'cpu_usage' in performance_data:
            axes[1, 0].plot(steps, performance_data['cpu_usage'], 'b-', alpha=0.7)
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('CPU (%)')
            axes[1, 0].set_title('CPU Usage')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Performance summary
        if 'steps_per_second' in performance_data:
            axes[1, 1].bar(['Steps/s', 'Efficiency'], 
                          [performance_data['steps_per_second'], 
                           performance_data.get('efficiency', 0)], 
                          color=['skyblue', 'lightgreen'])
            axes[1, 1].set_ylabel('Value')
            axes[1, 1].set_title('Performance Summary')
            axes[1, 1].grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=150, bbox_inches='tight')
        
        return fig
class ResultAnalyzer:
    """Analyze simulation results and extract insights."""
    
    @staticmethod
    def analyze_simulation_results(data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze simulation results and extract insights."""
        if not data:
            return {'error': 'No data provided for analysis'}
        
        analysis = {
            'basic_statistics': ResultAnalyzer._calculate_basic_stats(data),
            'temporal_patterns': ResultAnalyzer._analyze_temporal_patterns(data),
            'spatial_patterns': ResultAnalyzer._analyze_spatial_patterns(data),
            'transitions': ResultAnalyzer._detect_transitions(data),
            'regime_changes': ResultAnalyzer._detect_regime_changes(data),
            'conclusions': ResultAnalyzer._draw_conclusions(data, analysis)
        }
        
        return analysis
    
    @staticmethod
    def _calculate_basic_stats(data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate basic statistics from simulation data."""
        if not data:
            return {}
        
        # Extract numeric metrics
        steps = len(data)
        firing_rates = [d.get('average_firing_rate', 0) for d in data]
        active_neurons = [d.get('active_neurons', 0) for d in data]
        total_spikes = [d.get('total_spikes', 0) for d in data]
        
        stats = {
            'duration': steps,
            'firing_rate': {
                'mean': statistics.mean(firing_rates) if firing_rates else 0,
                'std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
                'min': min(firing_rates) if firing_rates else 0,
                'max': max(firing_rates) if firing_rates else 0,
                'cv': statistics.stdev(firing_rates) / statistics.mean(firing_rates) 
                      if statistics.mean(firing_rates) > 0 else 0
            },
            'active_neurons': {
                'mean': statistics.mean(active_neurons) if active_neurons else 0,
                'std': statistics.stdev(active_neurons) if len(active_neurons) > 1 else 0,
                'min': min(active_neurons) if active_neurons else 0,
                'max': max(active_neurons) if active_neurons else 0
            },
            'total_spikes': {
                'mean': statistics.mean(total_spikes) if total_spikes else 0,
                'std': statistics.stdev(total_spikes) if len(total_spikes) > 1 else 0,
                'min': min(total_spikes) if total_spikes else 0,
                'max': max(total_spikes) if total_spikes else 0
            }
        }
        
        return stats
    
    @staticmethod
    def _analyze_temporal_patterns(data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze temporal patterns in the data."""
        if len(data) < 10:
            return {'patterns_detected': False}
        
        patterns = {'periodicities': [], 'trends': [], 'anomalies': []}
        
        # Check for trends
        firing_rates = [d.get('average_firing_rate', 0) for d in data]
        
        # Linear trend
        if len(firing_rates) > 1:
            x = list(range(len(firing_rates)))
            y = firing_rates
            
            # Simple linear regression
            n = len(x)
            if n > 1:
                sum_x = sum(x)
                sum_y = sum(y)
                sum_xy = sum(x[i] * y[i] for i in range(n))
                sum_x2 = sum(xi * xi for xi in x)
                
                slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
                
                if abs(slope) > 0.01:  # Significant trend
                    trend = 'increasing' if slope > 0 else 'decreasing'
                    patterns['trends'].append({
                        'type': 'linear_trend',
                        'slope': slope,
                        'direction': trend,
                        'significance': abs(slope) / (max(firing_rates) - min(firing_rates) + 0.001)
                    })
        
        # Detect anomalies (points significantly different from mean)
        mean_fr = statistics.mean(firing_rates) if firing_rates else 0
        std_fr = statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0
        
        for i, fr in enumerate(firing_rates):
            if std_fr > 0 and abs(fr - mean_fr) > 2 * std_fr:
                patterns['anomalies'].append({
                    'step': data[i].get('step', i),
                    'firing_rate': fr,
                    'deviation': (fr - mean_fr) / std_fr,
                    'type': 'outlier'
                })
        
        return patterns
    
    @staticmethod
    def _analyze_spatial_patterns(data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze spatial patterns (if region data available)."""
        # This would analyze region-specific data if available
        return {'regions_analyzed': 0, 'spatial_patterns': []}
    
    @staticmethod
    def _detect_transitions(data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect state transitions in the simulation."""
        if len(data) < 5:
            return {'transitions': []}
        
        transitions = []
        firing_rates = [d.get('average_firing_rate', 0) for d in data]
        
        # Detect regime changes based on firing rate thresholds
        for i in range(1, len(firing_rates)):
            # Check for significant change
            if firing_rates[i-1] > 0:
                change = abs(firing_rates[i] - firing_rates[i-1]) / firing_rates[i-1]
                
                if change > 0.5:  # 50% change threshold
                    transitions.append({
                        'step': data[i].get('step', i),
                        'from_value': firing_rates[i-1],
                        'to_value': firing_rates[i],
                        'change_magnitude': change,
                        'change_direction': 'increase' if firing_rates[i] > firing_rates[i-1] else 'decrease'
                    })
        
        return {'transitions': transitions}
    
    @staticmethod
    def _detect_regime_changes(data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect regime changes using clustering."""
        # Simplified regime detection
        return {'regimes_detected': [], 'regime_boundaries': []}
    
    @staticmethod
    def _draw_conclusions(data: List[Dict[str, Any]], 
                         analysis: Dict[str, Any]) -> List[str]:
        """Draw conclusions from the analysis."""
        conclusions = []
        
        stats = analysis.get('basic_statistics', {})
        temporal = analysis.get('temporal_patterns', {})
        transitions = analysis.get('transitions', {})
        
        # Check for convergence
        firing_rates = [d.get('average_firing_rate', 0) for d in data]
        if len(firing_rates) > 20:
            recent_rates = firing_rates[-20:]
            if max(recent_rates) - min(recent_rates) < 0.1:  # Less than 10% variance
                conclusions.append("Simulation reached stable firing regime")
        
        # Check for trends
        if temporal.get('trends'):
            trend = temporal['trends'][0]
            if trend['direction'] == 'increasing':
                conclusions.append("Simulation shows increasing activity trend")
            else:
                conclusions.append("Simulation shows decreasing activity trend")
        
        # Check for anomalies
        if temporal.get('anomalies'):
            num_anomalies = len(temporal['anomalies'])
            conclusions.append(f"Detected {num_anomalies} anomalous activity patterns")
        
        # Check for transitions
        if transitions.get('transitions'):
            num_transitions = len(transitions['transitions'])
            conclusions.append(f"Detected {num_transitions} state transitions")
        
        # Overall assessment
        if not conclusions:
            conclusions.append("Simulation completed without significant patterns detected")
        
        return conclusions
# Public API functions
def create_config(neuron_count: int = 1000, **kwargs) -> Dict[str, Any]:
    """Create a configuration dictionary with specified parameters."""
    config = ConfigurationValidator().create_test_config('simple')
    config['brain']['neuron_count'] = neuron_count
    
    # Override with user parameters
    if 'brain' in kwargs:
        config['brain'].update(kwargs['brain'])
    
    return config


def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
    """Validate configuration dictionary."""
    return ConfigurationValidator().validate_config(config)


def create_test_config(complexity: str = 'simple') -> Dict[str, Any]:
    """Create a test configuration with specified complexity."""
    return ConfigurationValidator().create_test_config(complexity)


def analyze_results(data: List[Dict[str, Any]], 
                   output_format: str = 'summary') -> Dict[str, Any]:
    """Analyze simulation results and format output."""
    analysis = ResultAnalyzer.analyze_simulation_results(data)
    
    if output_format == 'summary':
        # Create human-readable summary
        summary = {
            'status': 'success' if analysis.get('conclusions') else 'error',
            'duration': analysis.get('basic_statistics', {}).get('duration', 0),
            'key_findings': analysis.get('conclusions', []),
            'statistics': analysis.get('basic_statistics', {})
        }
        return summary
    
    return analysis


def export_results(data: List[Dict[str, Any]], 
                   filepath: str,
                   format: str = 'json',
                   include_analysis: bool = True) -> bool:
    """Export simulation results to file."""
    try:
        Path(filepath).parent.mkdir(parents=True, exist_ok=True)
        
        export_data = {
            'metadata': {
                'export_time': datetime.now().isoformat(),
                'data_points': len(data),
                'export_format': format
            },
            'simulation_data': data
        }
        
        if include_analysis:
            export_data['analysis'] = ResultAnalyzer.analyze_simulation_results(data)
        
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(export_data, f, indent=2, default=str)
        
        elif format.lower() == 'csv':
            formatter = OutputFormatter()
            csv_content = formatter.format_csv(data)
            with open(filepath, 'w') as f:
                f.write(csv_content)
        
        return True
    
    except Exception as e:
        print(f"Error exporting results: {e}")
        return False


def load_example_config(config_type: str = 'simple') -> Dict[str, Any]:
    """Load example configuration of specified type."""
    examples = {
        'simple': {
            'brain': {
                'neuron_count': 100,
                'synapse_density': 0.1,
                'connection_probability': 0.05
            },
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.001},
                'hebbian': {'enable': True}
            }
        },
        'moderate': {
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.15,
                'connection_probability': 0.08,
                'v_thresh': -50.0,
                'v_rest': -70.0
            },
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.002},
                'hebbian': {'enable': True},
                'structural': {'enable': True}
            }
        },
        'complex': {
            'brain': {
                'neuron_count': 5000,
                'synapse_density': 0.2,
                'connection_probability': 0.1,
                'v_thresh': -45.0,
                'v_rest': -65.0,
                'tau_mem': 25.0
            },
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.003, 'tau_plus': 25.0},
                'hebbian': {'enable': True},
                'structural': {'enable': True, 'synaptogenesis_rate': 0.0001}
            },
            'neuromod': {
                'dopamine': {'scale': 1.0},
                'curiosity': {'enable': True, 'scale': 0.8},
                'novelty': {'enable': True, 'scale': 0.6}
            }
        }
    }
    
    return examples.get(config_type, examples['simple']).copy()