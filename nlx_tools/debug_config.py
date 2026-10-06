#!/usr/bin/env python3
"""
NLM - Configuration Debugging Tool

Configuration validation and visualization with sophisticated capabilities for researchers.

Features:
- Configuration validation
- Configuration difference visualization
- Configuration dependency visualization
- Configuration impact analysis
- Configuration optimization visualization
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from typing import Dict, List, Optional, Tuple, Any
import time
import threading
from dataclasses import dataclass
from enum import Enum
import json
from datetime import datetime
import os
from copy import deepcopy

@dataclass
class ConfigDebugConfig:
    """Configuration for configuration debugging."""
    validate_dependencies: bool = True
    show_impact_analysis: bool = True
    suggest_optimizations: bool = True
    visualize_differences: bool = True
    analyze_performance: bool = True
    check_compatibility: bool = True
    update_rate: float = 5.0  # Hz
    config_history_size: int = 100
    optimization_threshold: float = 0.8
    window_size: Tuple[int, int] = (1400, 900)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 20

class ConfigDebugState(Enum):
    """Configuration debugging state enumeration."""
    IDLE = "idle"
    VALIDATING = "validating"
    ANALYZING = "analyzing"
    OPTIMIZING = "optimizing"
    EXPORTING = "exporting"

class NLConfigValidator:
    """
    Configuration validation and visualization tool.
    
    Provides sophisticated configuration validation capabilities including
    configuration validation, difference visualization, dependency visualization,
    impact analysis, and optimization visualization for neural network configurations.
    """
    
    def __init__(self, config: Optional[ConfigDebugConfig] = None):
        self.config = config or ConfigDebugConfig()
        self.state = ConfigDebugState.IDLE
        self.config_history: List[Dict[str, Any]] = []
        self.dependency_graph = {}
        self.impact_analysis = {}
        self.optimization_suggestions = []
        self.current_time = 0.0
        self.animation_obj = None
        self.fig = None
        self.axes = None
        self.validation_plots = []
        self.dependency_plots = []
        self.impact_plots = []
        self.optimization_plots = []
        
        # Setup visualization
        self._setup_visualization()
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        plt.style.use('seaborn-v0_8')
        self.fig, self.axes = plt.subplots(
            3, 1, figsize=self.config.window_size, 
            gridspec_kw={'height_ratios': [1, 1, 1], 'hspace': 0.4}
        )
        
        # Setup subplots
        self.validation_plots = [self.axes[0]]
        self.dependency_plots = [self.axes[1]]
        self.impact_plots = [self.axes[2]]
        self.optimization_plots = []
        
        for ax in self.axes:
            ax.set_facecolor('#f8f9fa')
            ax.grid(True, alpha=0.3)
            ax.set_xlabel('Time (ms)', fontsize=10)
        
        self.validation_plots[0].set_title('Configuration Validation', fontsize=12, fontweight='bold')
        self.dependency_plots[0].set_title('Configuration Dependencies', fontsize=12, fontweight='bold')
        self.impact_plots[0].set_title('Configuration Impact Analysis', fontsize=12, fontweight='bold')
        
        plt.tight_layout()
    
    def validate_config(self, config_dict: Dict[str, Any]):
        """
        Validate configuration with comprehensive analysis.
        
        Args:
            config_dict: Configuration dictionary to validate
        """
        self.state = ConfigDebugState.VALIDATING
        
        # Clear any existing animation
        if self.animation_obj:
            self.animation_obj.event_source.stop()
        
        # Initialize validation
        self._initialize_validation(config_dict)
        
        # Start validation thread
        self.validation_thread = threading.Thread(target=self._validation_loop)
        self.validation_thread.daemon = True
        self.validation_thread.start()
        
        print(f"Starting configuration validation...")
        print(f"Dependency validation: {self.config.validate_dependencies}")
        print(f"Impact analysis: {self.config.show_impact_analysis}")
        print(f"Optimization suggestions: {self.config.suggest_optimizations}")
        print(f"Difference visualization: {self.config.visualize_differences}")
        print(f"State: {self.state.value}")
    
    def _initialize_validation(self, config_dict: Dict[str, Any]):
        """Initialize configuration validation."""
        self.config_history = []
        self.dependency_graph = {}
        self.impact_analysis = {}
        self.optimization_suggestions = []
        
        # Analyze configuration structure
        self._analyze_config_structure(config_dict)
        
        # Build dependency graph
        if self.config.validate_dependencies:
            self._build_dependency_graph(config_dict)
        
        # Perform impact analysis
        if self.config.show_impact_analysis:
            self._perform_impact_analysis(config_dict)
        
        # Generate optimization suggestions
        if self.config.suggest_optimizations:
            self._generate_optimization_suggestions(config_dict)
        
        # Validate configuration
        validation_results = self._validate_configuration(config_dict)
        
        # Record validation
        self.config_history.append({
            'time': self.current_time,
            'config': deepcopy(config_dict),
            'validation_results': validation_results,
            'dependencies': self.dependency_graph,
            'impacts': self.impact_analysis,
            'optimizations': self.optimization_suggestions
        })
    
    def _analyze_config_structure(self, config_dict: Dict[str, Any]):
        """Analyze configuration structure for insights."""
        # Analyze configuration depth
        max_depth = self._calculate_config_depth(config_dict)
        
        # Analyze parameter types
        param_types = {}
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                param_types.setdefault('numeric', 0)
                param_types['numeric'] += 1
            elif isinstance(value, str):
                param_types.setdefault('string', 0)
                param_types['string'] += 1
            elif isinstance(value, list):
                param_types.setdefault('list', 0)
                param_types['list'] += 1
            elif isinstance(value, dict):
                param_types.setdefault('dict', 0)
                param_types['dict'] += 1
        
        # Analyze parameter ranges
        param_ranges = {}
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                param_ranges[key] = {'min': value, 'max': value, 'type': type(value).__name__}
            elif isinstance(value, list) and value:
                param_ranges[key] = {'min': min(value), 'max': max(value), 'length': len(value)}
        
        self.config_structure = {
            'depth': max_depth,
            'param_types': param_types,
            'param_ranges': param_ranges,
            'total_params': len(config_dict)
        }
    
    def _calculate_config_depth(self, config_dict: Dict[str, Any], depth=0) -> int:
        """Calculate maximum depth of configuration structure."""
        max_child_depth = 0
        for value in config_dict.values():
            if isinstance(value, dict):
                child_depth = self._calculate_config_depth(value, depth + 1)
                max_child_depth = max(max_child_depth, child_depth)
        return max(depth, max_child_depth)
    
    def _build_dependency_graph(self, config_dict: Dict[str, Any]):
        """Build dependency graph for configuration analysis."""
        self.dependency_graph = {
            'nodes': [],
            'edges': [],
            'dependencies': {}
        }
        
        # Add configuration parameters as nodes
        for key in config_dict.keys():
            self.dependency_graph['nodes'].append({
                'id': key,
                'type': self._infer_node_type(key),
                'value': config_dict[key]
            })
        
        # Build dependency relationships based on parameter names
        for i, key1 in enumerate(config_dict.keys()):
            for j, key2 in enumerate(config_dict.keys()):
                if i < j:
                    # Check if parameters are related
                    if self._are_parameters_related(key1, key2):
                        self.dependency_graph['edges'].append({
                            'source': key1,
                            'target': key2,
                            'strength': self._calculate_dependency_strength(key1, key2, config_dict)
                        })
    
    def _infer_node_type(self, param_name: str) -> str:
        """Infer type of configuration parameter."""
        param_name_lower = param_name.lower()
        if 'learning' in param_name_lower or 'rate' in param_name_lower:
            return 'learning_parameter'
        elif 'neuron' in param_name_lower or 'layer' in param_name_lower:
            return 'architecture'
        elif 'threshold' in param_name_lower or 'bias' in param_name_lower:
            return 'threshold'
        elif 'weight' in param_name_lower or 'scale' in param_name_lower:
            return 'scaling'
        elif 'regularization' in param_name_lower or 'penalty' in param_name_lower:
            return 'regularization'
        else:
            return 'general'
    
    def _are_parameters_related(self, key1: str, key2: str) -> bool:
        """Check if two parameters are related."""
        key1_lower = key1.lower()
        key2_lower = key2.lower()
        
        # Common related parameter pairs
        related_pairs = [
            ('learning_rate', 'decay_rate'),
            ('batch_size', 'learning_rate'),
            ('hidden_layers', 'output_layers'),
            ('weight_init', 'bias_init'),
            ('regularization', 'dropout'),
            ('optimizer', 'learning_rate'),
            ('epochs', 'batch_size'),
            ('momentum', 'learning_rate')
        ]
        
        # Check if either parameter name contains keywords from the other
        keywords1 = set(key1_lower.split('_'))
        keywords2 = set(key2_lower.split('_'))
        
        return len(keywords1.intersection(keywords2)) > 0 or \
               any((key1_lower, key2_lower) in pair or (pair[0], pair[1]) in (key1_lower, key2_lower) for pair in related_pairs)
    
    def _calculate_dependency_strength(self, key1: str, key2: str, config_dict: Dict[str, Any]) -> float:
        """Calculate dependency strength between parameters."""
        # Base strength from related keywords
        strength = 0.5
        
        # Adjust based on parameter types
        if self._infer_node_type(key1) == self._infer_node_type(key2):
            strength += 0.3
        
        # Adjust based on parameter values (simplified)
        value1 = config_dict[key1] if isinstance(config_dict.get(key1), (int, float)) else 0
        value2 = config_dict[key2] if isinstance(config_dict.get(key2), (int, float)) else 0
        
        if isinstance(value1, (int, float)) and isinstance(value2, (int, float)):
            strength += min(abs(value1 - value2) / max(abs(value1), abs(value2)), 1.0) * 0.2
        
        return min(strength, 1.0)
    
    def _perform_impact_analysis(self, config_dict: Dict[str, Any]):
        """Perform configuration impact analysis."""
        self.impact_analysis = {
            'performance_impact': {},
            'stability_impact': {},
            'accuracy_impact': {},
            'overfitting_risk': {}
        }
        
        # Analyze impact of each parameter
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                impact = self._calculate_parameter_impact(key, value, config_dict)
                self.impact_analysis['performance_impact'][key] = impact['performance']
                self.impact_analysis['stability_impact'][key] = impact['stability']
                self.impact_analysis['accuracy_impact'][key] = impact['accuracy']
                self.impact_analysis['overfitting_risk'][key] = impact['overfitting_risk']
    
    def _calculate_parameter_impact(self, param_name: str, param_value: float, config_dict: Dict[str, Any]) -> Dict[str, float]:
        """Calculate impact of a single parameter."""
        impact = {
            'performance': 0.0,
            'stability': 0.0,
            'accuracy': 0.0,
            'overfitting_risk': 0.0
        }
        
        # Simplified impact calculations based on parameter type
        param_name_lower = param_name.lower()
        
        # Learning rate impact
        if 'learning_rate' in param_name_lower:
            if param_value < 0.01:
                impact['performance'] = -0.5
                impact['accuracy'] = -0.3
                impact['stability'] = 0.8
            elif param_value < 0.1:
                impact['performance'] = 0.5
                impact['accuracy'] = 0.6
                impact['stability'] = 0.6
            else:
                impact['performance'] = -0.3
                impact['accuracy'] = -0.5
                impact['stability'] = 0.4
        
        # Neuron count impact
        elif 'neuron' in param_name_lower or 'hidden' in param_name_lower:
            if param_value < 100:
                impact['performance'] = -0.4
                impact['accuracy'] = -0.5
                impact['stability'] = 0.7
            elif param_value < 1000:
                impact['performance'] = 0.8
                impact['accuracy'] = 0.9
                impact['stability'] = 0.5
            else:
                impact['performance'] = 0.3
                impact['accuracy'] = 0.4
                impact['stability'] = 0.3
        
        # Regularization impact
        elif 'regularization' in param_name_lower or 'dropout' in param_name_lower:
            if param_value < 0.1:
                impact['performance'] = -0.6
                impact['accuracy'] = -0.7
                impact['overfitting_risk'] = 0.9
            elif param_value < 0.5:
                impact['performance'] = 0.6
                impact['accuracy'] = 0.7
                impact['overfitting_risk'] = 0.3
            else:
                impact['performance'] = -0.2
                impact['accuracy'] = -0.3
                impact['overfitting_risk'] = 0.8
        
        # Default impact
        else:
            impact['performance'] = 0.3
            impact['accuracy'] = 0.4
            impact['stability'] = 0.7
            impact['overfitting_risk'] = 0.5
        
        return impact
    
    def _generate_optimization_suggestions(self, config_dict: Dict[str, Any]):
        """Generate configuration optimization suggestions."""
        self.optimization_suggestions = []
        
        # Analyze current configuration for optimization opportunities
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                suggestion = self._generate_optimization_suggestion(key, value, config_dict)
                if suggestion:
                    self.optimization_suggestions.append(suggestion)
    
    def _generate_optimization_suggestion(self, param_name: str, param_value: float, config_dict: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """Generate optimization suggestion for a parameter."""
        suggestion = None
        
        param_name_lower = param_name.lower()
        
        # Learning rate optimization
        if 'learning_rate' in param_name_lower:
            if param_value < 0.01:
                suggestion = {
                    'parameter': param_name,
                    'current_value': param_value,
                    'recommended_value': 0.05,
                    'reason': 'Current learning rate is too low for efficient convergence',
                    'expected_improvement': 'Significant increase in training speed'
                }
            elif param_value > 0.1:
                suggestion = {
                    'parameter': param_name,
                    'current_value': param_value,
                    'recommended_value': 0.01,
                    'reason': 'Current learning rate is too high for stable convergence',
                    'expected_improvement': 'Improved stability and accuracy'
                }
        
        # Neuron count optimization
        elif 'neuron' in param_name_lower or 'hidden' in param_name_lower:
            if param_value > 1000:
                suggestion = {
                    'parameter': param_name,
                    'current_value': param_value,
                    'recommended_value': 500,
                    'reason': 'Current neuron count may cause overfitting and slow training',
                    'expected_improvement': 'Better generalization with reduced computational cost'
                }
            elif param_value < 100:
                suggestion = {
                    'parameter': param_name,
                    'current_value': param_value,
                    'recommended_value': 200,
                    'reason': 'Current neuron count may be insufficient for complex tasks',
                    'expected_improvement': 'Improved model capacity and performance'
                }
        
        # Regularization optimization
        elif 'regularization' in param_name_lower or 'dropout' in param_name_lower:
            if param_value < 0.1:
                suggestion = {
                    'parameter': param_name,
                    'current_value': param_value,
                    'recommended_value': 0.2,
                    'reason': 'Regularization is too weak, risking overfitting',
                    'expected_improvement': 'Better generalization and reduced overfitting'
                }
            elif param_value > 0.5:
                suggestion = {
                    'parameter': param_name,
                    'current_value': param_value,
                    'recommended_value': 0.3,
                    'reason': 'Regularization is too strong, limiting model capacity',
                    'expected_improvement': 'Better balance between regularization and model capacity'
                }
        
        return suggestion
    
    def _validate_configuration(self, config_dict: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration and return results."""
        validation_results = {
            'is_valid': True,
            'errors': [],
            'warnings': [],
            'score': 0.0
        }
        
        # Check for invalid parameter values
        for key, value in config_dict.items():
            if isinstance(value, (int, float)):
                if value < 0:
                    validation_results['errors'].append(f"Parameter {key} has negative value: {value}")
                    validation_results['is_valid'] = False
                elif key.endswith('_rate') and value > 1.0:
                    validation_results['warnings'].append(f"Parameter {key} value {value} > 1.0 may cause instability")
            
            # Check for inconsistent parameter combinations
            if 'learning_rate' in config_dict and 'batch_size' in config_dict:
                lr = config_dict['learning_rate']
                bs = config_dict['batch_size']
                if lr < 0.01 and bs > 100:
                    validation_results['warnings'].append("Low learning rate with large batch size may slow convergence")
        
        # Calculate validation score
        if validation_results['errors']:
            validation_results['score'] = 0.0
        elif validation_results['warnings']:
            validation_results['score'] = max(0.0, 1.0 - len(validation_results['warnings']) * 0.2)
        else:
            validation_results['score'] = 1.0
        
        return validation_results
    
    def _validation_loop(self):
        """Main validation loop for configuration debugging."""
        while self.state == ConfigDebugState.VALIDATING or self.state == ConfigDebugState.ANALYZING:
            # Update configuration visualization
            self._render_plots()
            
            # Save if auto-save is enabled
            if self.config.auto_save and len(self.config_history) % self.config.save_interval == 0:
                self._save_current_frame()
            
            # Control validation speed
            time.sleep(1.0 / self.config.update_rate)
    
    def _render_plots(self):
        """Render the current visualization plots."""
        if not self.fig:
            return
        
        # Clear axes
        for ax in self.axes:
            ax.clear()
        
        # Plot validation results if enabled
        if self.config_history:
            self._plot_validation_results()
        
        # Plot dependency graph if available
        if self.dependency_graph and self.dependency_graph['edges']:
            self._plot_dependency_graph()
        
        # Plot impact analysis if available
        if self.impact_analysis['performance_impact']:
            self._plot_impact_analysis()
        
        # Update plot labels
        self._update_plot_labels()
        
        # Refresh the figure
        self.fig.canvas.draw()
    
    def _plot_validation_results(self):
        """Plot configuration validation results."""
        if len(self.config_history) < 5:
            return
        
        # Get recent validation results
        recent_history = self.config_history[-5:]
        
        # Plot validation scores
        times = [record['time'] for record in recent_history]
        scores = [record.get('validation_results', {}).get('score', 0) for record in recent_history]
        errors = [len(record.get('validation_results', {}).get('errors', [])) for record in recent_history]
        warnings = [len(record.get('validation_results', {}).get('warnings', [])) for record in recent_history]
        
        # Plot validation score
        self.validation_plots[0].plot(times, scores, 'g-', linewidth=2, label='Validation Score')
        
        # Plot errors as scatter points
        self.validation_plots[0].scatter(times, errors, c='red', s=50, alpha=0.7, label='Errors')
        
        # Plot warnings as scatter points
        self.validation_plots[0].scatter(times, warnings, c='orange', s=50, alpha=0.7, label='Warnings')
        
        self.validation_plots[0].set_ylim(-0.1, 1.1)
        self.validation_plots[0].set_ylabel('Score/Count')
        self.validation_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_dependency_graph(self):
        """Plot dependency graph visualization."""
        if not self.dependency_graph['edges']:
            return
        
        # Create simple representation of dependency graph
        times = list(range(len(self.config_history)))
        
        # Calculate dependency complexity over time
        complexity_values = []
        for i, record in enumerate(self.config_history):
            edge_count = len(record.get('dependencies', {}).get('edges', []))
            complexity_values.append(edge_count)
        
        self.dependency_plots[0].plot(times, complexity_values, 'b-', linewidth=2, label='Dependency Complexity')
        self.dependency_plots[0].set_ylabel('Number of Dependencies')
        self.dependency_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_impact_analysis(self):
        """Plot configuration impact analysis."""
        if not self.impact_analysis['performance_impact']:
            return
        
        # Get current impact values
        param_names = list(self.impact_analysis['performance_impact'].keys())
        performance_impacts = list(self.impact_analysis['performance_impact'].values())
        
        # Create horizontal bar chart
        y_pos = np.arange(len(param_names))
        
        # Normalize impacts for visualization
        max_impact = max(max(abs(v) for v in performance_impacts), 1)
        normalized_impacts = [v / max_impact for v in performance_impacts]
        
        bars = self.impact_plots[0].barh(y_pos, normalized_impacts, alpha=0.7, color='steelblue')
        
        self.impact_plots[0].set_yticks(y_pos)
        self.impact_plots[0].set_yticklabels(param_names)
        self.impact_plots[0].set_xlabel('Impact (Normalized)')
        
        # Add value labels on bars
        for i, bar in enumerate(bars):
            width = bar.get_width()
            self.impact_plots[0].text(
                width + 0.01, i, f'{performance_impacts[i]:.2f}',
                va='center', fontsize=8
            )
    
    def _update_plot_labels(self):
        """Update plot labels with current configuration statistics."""
        current_time_ms = self.current_time
        
        # Update titles with current statistics
        if self.config_history:
            latest_record = self.config_history[-1]
            
            # Update validation title
            validation_results = latest_record.get('validation_results', {})
            if validation_results:
                score = validation_results.get('score', 0)
                error_count = len(validation_results.get('errors', []))
                self.validation_plots[0].set_title(
                    f'Configuration Validation (Score: {score:.2f}, Errors: {error_count})',
                    fontsize=12, fontweight='bold'
                )
            
            # Update dependency title
            if self.dependency_graph:
                node_count = len(self.dependency_graph.get('nodes', []))
                edge_count = len(self.dependency_graph.get('edges', []))
                self.dependency_plots[0].set_title(
                    f'Configuration Dependencies (Nodes: {node_count}, Edges: {edge_count})',
                    fontsize=12, fontweight='bold'
                )
        
        # Update time in title
        time_str = f'Time: {current_time_ms:.0f} ms'
        self.fig.suptitle(time_str, fontsize=14, fontweight='bold')
    
    def _save_current_frame(self):
        """Save current visualization frame."""
        if self.config.save_path:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{self.config.save_path}/config_debug_frame_{timestamp}.png"
            
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            try:
                self.fig.savefig(filename, dpi=150, bbox_inches='tight')
                print(f"Saved configuration debug frame: {filename}")
            except Exception as e:
                print(f"Warning: Could not save frame: {e}")
    
    def analyze_config(self):
        """Start configuration analysis."""
        if self.state != ConfigDebugState.VALIDATING:
            print("Configuration must be validated first. Call validate_config() first.")
            return
        
        self.state = ConfigDebugState.ANALYZING
        print("Starting configuration analysis...")
    
    def optimize_config(self):
        """Start configuration optimization."""
        if self.state != ConfigDebugState.ANALYZING:
            print("Configuration must be analyzed first. Call analyze_config() first.")
            return
        
        self.state = ConfigDebugState.OPTIMIZING
        print("Starting configuration optimization...")
    
    def stop(self):
        """Stop the configuration debugging."""
        self.state = ConfigDebugState.IDLE
        
        # Stop validation thread
        if hasattr(self, 'validation_thread') and self.validation_thread.is_alive():
            self.validation_thread.join(timeout=5.0)
        
        print("Configuration debugging stopped")
    
    def export_data(self, output_path: str, format: str = "json"):
        """
        Export configuration debugging data to file.
        
        Args:
            output_path: Path to output file
            format: Export format (json, csv, npy)
        """
        self.state = ConfigDebugState.EXPORTING
        
        try:
            if format.lower() == "json":
                export_data = {
                    'config': self.config.__dict__,
                    'config_history': self.config_history,
                    'dependency_graph': self.dependency_graph,
                    'impact_analysis': self.impact_analysis,
                    'optimization_suggestions': self.optimization_suggestions,
                    'config_structure': getattr(self, 'config_structure', {}),
                    'timestamp': datetime.now().isoformat()
                }
                
                with open(output_path, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                    
            elif format.lower() == "csv":
                import pandas as pd
                
                # Convert config history to DataFrame
                df_data = []
                for record in self.config_history:
                    df_data.append({
                        'time': record['time'],
                        'validation_score': record.get('validation_results', {}).get('score', 0),
                        'error_count': len(record.get('validation_results', {}).get('errors', [])),
                        'warning_count': len(record.get('validation_results', {}).get('warnings', [])),
                        'config_depth': record.get('config_structure', {}).get('depth', 0),
                        'total_params': record.get('config_structure', {}).get('total_params', 0)
                    })
                
                df = pd.DataFrame(df_data)
                df.to_csv(output_path, index=False)
                
            elif format.lower() == "npy":
                np.savez(output_path, 
                        config_history=np.array(self.config_history, dtype=object),
                        dependency_graph=np.array(self.dependency_graph, dtype=object),
                        config_structure=getattr(self, 'config_structure', {}),
                        timestamp=datetime.now().isoformat())
            
            print(f"Configuration debugging data exported to: {output_path}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
        
        finally:
            self.state = ConfigDebugState.IDLE

def main():
    """Main entry point for nlx-debug-config tool."""
    print("NLM - Neural Configuration Debugging Tool")
    print("=" * 50)
    print("Configuration validation and visualization with sophisticated capabilities")
    print()
    
    # Example usage
    config = ConfigDebugConfig(
        validate_dependencies=True,
        show_impact_analysis=True,
        suggest_optimizations=True,
        visualize_differences=True,
        analyze_performance=True,
        check_compatibility=True,
        update_rate=5.0,
        optimization_threshold=0.8,
        window_size=(1400, 900)
    )
    
    # Create configuration validator
    validator = NLConfigValidator(config)
    
    # Example configuration
    example_config = {
        "learning_rate": 0.01,
        "batch_size": 64,
        "hidden_layers": [128, 64],
        "dropout_rate": 0.2,
        "optimizer": "adam",
        "epochs": 100,
        "weight_decay": 0.001,
        "momentum": 0.9
    }
    
    print("Example configuration created successfully!")
    print(f"Dependency validation: {config.validate_dependencies}")
    print(f"Impact analysis: {config.show_impact_analysis}")
    print(f"Optimization suggestions: {config.suggest_optimizations}")
    print(f"Update rate: {config.update_rate} Hz")
    print(f"Window size: {config.window_size}")
    print()
    print("To use the validator:")
    print("  validator = NLConfigValidator(config)")
    print("  validator.validate_config(example_config)")
    print("  validator.analyze_config()")
    print("  validator.optimize_config()")
    print("  validator.stop()   # to stop")
    print("  validator.export_data('output.json', 'json')  # to export")

if __name__ == "__main__":
    main()
