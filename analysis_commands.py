#!/usr/bin/env python3
"""
Analysis Commands for NLM CLI

Handles analysis of simulation data, metrics, visualization, and pattern detection.
"""

import json
import time
import statistics
import numpy as np
from pathlib import Path
from typing import Dict, Any, Optional, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn

from nlm_cli import NLMCLI

console = Console()

class AnalysisCommands:
    """Handle analysis commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle analysis commands."""
        command = args.get('command')
        
        handlers = {
            'analyze_data': self.analyze_simulation_data,
            'generate_report': self.generate_performance_report,
            'visualize': self.visualize_results,
            'find_patterns': self.find_patterns_and_anomalies,
            'compare_results': self.compare_analysis_results,
            'export_analysis': self.export_analysis_results,
            'import_analysis': self.import_analysis_results,
            'system_stats': self.get_system_stats,
            'resource_monitor': self.monitor_resources,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown analysis command: {command}")
    
    def analyze_simulation_data(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze simulation data."""
        simulation_name = args.get('simulation_name')
        data_source = args.get('data_source')
        analysis_type = args.get('analysis_type', 'comprehensive')
        
        # Load data from various sources
        data = self._load_analysis_data(data_source, simulation_name)
        
        if not data:
            return {
                'success': False,
                'message': f"No data found for simulation '{simulation_name}'.",
                'error': 'Data source not found'
            }
        
        # Perform analysis based on type
        if analysis_type == 'comprehensive':
            results = self._comprehensive_analysis(data)
        elif analysis_type == 'performance':
            results = self._performance_analysis(data)
        elif analysis_type == 'neural':
            results = self._neural_analysis(data)
        elif analysis_type == 'temporal':
            results = self._temporal_analysis(data)
        else:
            results = self._comprehensive_analysis(data)
        
        return {
            'success': True,
            'message': f"Analysis of simulation '{simulation_name}' completed.",
            'simulation_name': simulation_name,
            'analysis_type': analysis_type,
            'results': results,
            'data_points': len(data)
        }
    
    def generate_performance_report(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Generate performance report."""
        report_name = args.get('name', 'performance_report')
        simulation_name = args.get('simulation_name')
        metrics = args.get('metrics')
        format = args.get('format', 'json')
        
        # Load simulation data if not provided
        if simulation_name:
            data = self._load_analysis_data(simulation_name, simulation_name)
        else:
            data = args.get('data', [])
        
        if not data:
            return {
                'success': False,
                'message': "No data available for performance report.",
                'error': 'Data not found'
            }
        
        # Generate report
        report = self._generate_performance_report(data, metrics or [])
        
        # Save report
        output_path = args.get('output')
        if output_path:
            output = Path(output_path)
            output.parent.mkdir(parents=True, exist_ok=True)
            
            if format == 'json':
                with open(output, 'w') as f:
                    json.dump(report, f, indent=2, default=str)
            elif format == 'csv':
                self._export_csv_report(report, output)
            elif format == 'text':
                self._export_text_report(report, output)
        
        return {
            'success': True,
            'message': f"Performance report '{report_name}' generated.",
            'report_name': report_name,
            'simulation_name': simulation_name,
            'report': report,
            'output_path': str(output) if output_path else None
        }
    
    def visualize_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Visualize simulation results."""
        simulation_name = args.get('simulation_name')
        plot_type = args.get('plot_type', 'comprehensive')
        output_path = args.get('output')
        
        # Load data
        data = self._load_analysis_data(simulation_name, simulation_name)
        
        if not data:
            return {
                'success': False,
                'message': f"No data found for simulation '{simulation_name}'.",
                'error': 'Data source not found'
            }
        
        try:
            # Create visualizations based on plot type
            if plot_type == 'comprehensive':
                visualizations = self._create_comprehensive_plots(data)
            elif plot_type == 'activity':
                visualizations = self._create_activity_plots(data)
            elif plot_type == 'network':
                visualizations = self._create_network_plots(data)
            elif plot_type == 'temporal':
                visualizations = self._create_temporal_plots(data)
            else:
                visualizations = self._create_comprehensive_plots(data)
            
            # Save visualizations
            if output_path:
                output_dir = Path(output_path)
                output_dir.mkdir(parents=True, exist_ok=True)
                
                for plot_name, plot_data in visualizations.items():
                    plot_file = output_dir / f"{plot_name}.json"
                    with open(plot_file, 'w') as f:
                        json.dump(plot_data, f, indent=2, default=str)
            
            return {
                'success': True,
                'message': f"Visualizations generated for simulation '{simulation_name}'.",
                'simulation_name': simulation_name,
                'plot_type': plot_type,
                'visualizations': visualizations,
                'output_path': str(output_dir) if output_path else None
            }
        except ImportError as e:
            return {
                'success': False,
                'message': f"Visualization failed due to missing dependencies: {e}",
                'error': str(e)
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Visualization failed: {e}",
                'error': str(e)
            }
    
    def find_patterns_and_anomalies(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Find patterns and anomalies in simulation data."""
        simulation_name = args.get('simulation_name')
        pattern_type = args.get('pattern_type', 'all')
        
        # Load data
        data = self._load_analysis_data(simulation_name, simulation_name)
        
        if not data:
            return {
                'success': False,
                'message': f"No data found for simulation '{simulation_name}'.",
                'error': 'Data source not found'
            }
        
        # Find patterns and anomalies
        patterns = {}
        anomalies = {}
        
        if pattern_type in ['all', 'temporal']:
            patterns['temporal'] = self._detect_temporal_patterns(data)
            anomalies['temporal'] = self._detect_temporal_anomalies(data)
        
        if pattern_type in ['all', 'neural']:
            patterns['neural'] = self._detect_neural_patterns(data)
            anomalies['neural'] = self._detect_neural_anomalies(data)
        
        if pattern_type in ['all', 'activity']:
            patterns['activity'] = self._detect_activity_patterns(data)
            anomalies['activity'] = self._detect_activity_anomalies(data)
        
        return {
            'success': True,
            'message': f"Pattern detection completed for simulation '{simulation_name}'.",
            'simulation_name': simulation_name,
            'patterns': patterns,
            'anomalies': anomalies,
            'data_points': len(data)
        }
    
    def compare_analysis_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Compare analysis results between simulations."""
        simulation1 = args.get('simulation1')
        simulation2 = args.get('simulation2')
        
        data1 = self._load_analysis_data(simulation1, simulation1)
        data2 = self._load_analysis_data(simulation2, simulation2)
        
        if not data1:
            return {
                'success': False,
                'message': f"No data found for simulation '{simulation1}'.",
                'error': 'Data source not found'
            }
        
        if not data2:
            return {
                'success': False,
                'message': f"No data found for simulation '{simulation2}'.",
                'error': 'Data source not found'
            }
        
        # Compare results
        comparison = {
            'simulation1': simulation1,
            'simulation2': simulation2,
            'data_points1': len(data1),
            'data_points2': len(data2),
            'statistical_comparison': self._compare_statistical_data(data1, data2),
            'performance_comparison': self._compare_performance_metrics(data1, data2),
            'neural_comparison': self._compare_neural_metrics(data1, data2),
            'temporal_comparison': self._compare_temporal_patterns(data1, data2)
        }
        
        return {
            'success': True,
            'message': f"Comparison completed between '{simulation1}' and '{simulation2}'.",
            'comparison': comparison
        }
    
    def export_analysis_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Export analysis results to file."""
        name = args.get('name')
        output_path = args.get('output')
        format = args.get('format', 'json')
        
        analysis_data = args.get('analysis_data', {})
        
        if not output_path:
            output_path = f"{name}_analysis.{format}"
        
        output = Path(output_path)
        output.parent.mkdir(parents=True, exist_ok=True)
        
        if format == 'json':
            with open(output, 'w') as f:
                json.dump(analysis_data, f, indent=2, default=str)
        elif format == 'csv':
            self._export_csv_analysis(analysis_data, output)
        elif format == 'text':
            self._export_text_analysis(analysis_data, output)
        
        return {
            'success': True,
            'message': f"Analysis results '{name}' exported to {output}.",
            'analysis_name': name,
            'output_path': str(output),
            'format': format
        }
    
    def import_analysis_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Import analysis results from file."""
        input_path = args.get('input')
        name = args.get('name')
        force = args.get('force', False)
        
        if not input_path:
            raise click.ClickException("Input file is required for importing analysis results.")
        
        if not Path(input_path).exists():
            raise click.ClickException(f"Input file not found: {input_path}")
        
        with open(input_path, 'r') as f:
            import_data = json.load(f)
        
        # Extract analysis from import data
        if 'analysis' in import_data:
            analysis_data = import_data['analysis']
        elif 'results' in import_data:
            analysis_data = import_data['results']
        else:
            analysis_data = import_data
        
        analysis_name = name or Path(input_path).stem
        
        if 'analysis_results' not in self.nlm_cli.context:
            self.nlm_cli.context['analysis_results'] = {}
        
        if analysis_name in self.nlm_cli.context['analysis_results'] and not force:
            if not click.confirm(f"Analysis '{analysis_name}' already exists. Overwrite?", default=False):
                return {
                    'success': False,
                    'message': "Operation cancelled."
                }
        
        analysis_data['last_modified'] = datetime.now().isoformat()
        self.nlm_cli.context['analysis_results'][analysis_name] = analysis_data
        
        return {
            'success': True,
            'message': f"Analysis results imported from {input_path}.",
            'analysis_name': analysis_name,
            'analysis_data': analysis_data
        }
    
    def get_system_stats(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get system statistics."""
        import psutil
        import os
        
        stats = {}
        
        # CPU statistics
        stats['cpu'] = {
            'usage_percent': psutil.cpu_percent(interval=1),
            'usage_per_core': psutil.cpu_percent(interval=1, percpu=True),
            'cores': psutil.cpu_count(),
            'logical_cores': psutil.cpu_count(logical=True)
        }
        
        # Memory statistics
        memory = psutil.virtual_memory()
        stats['memory'] = {
            'total_gb': memory.total / (1024**3),
            'available_gb': memory.available / (1024**3),
            'used_percent': memory.percent,
            'used_gb': memory.used / (1024**3),
            'free_gb': memory.free / (1024**3)
        }
        
        # Disk statistics
        disk = psutil.disk_usage('/')
        stats['disk'] = {
            'total_gb': disk.total / (1024**3),
            'used_gb': disk.used / (1024**3),
            'free_gb': disk.free / (1024**3),
            'used_percent': disk.percent
        }
        
        # Network statistics
        net_io = psutil.net_io_counters()
        stats['network'] = {
            'bytes_sent_gb': net_io.bytes_sent / (1024**3),
            'bytes_recv_gb': net_io.bytes_recv / (1024**3),
            'packets_sent': net_io.packets_sent,
            'packets_recv': net_io.packets_recv
        }
        
        # Process statistics
        process = psutil.Process(os.getpid())
        stats['process'] = {
            'pid': process.pid,
            'name': process.name(),
            'cpu_percent': process.cpu_percent(interval=1),
            'memory_mb': process.memory_info().rss / (1024**3),
            'threads': process.num_threads(),
            'create_time': datetime.fromtimestamp(process.create_time()).isoformat()
        }
        
        return {
            'success': True,
            'message': "System statistics retrieved.",
            'system_stats': stats,
            'timestamp': datetime.now().isoformat()
        }
    
    def monitor_resources(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Monitor system resources."""
        duration = args.get('duration', 60)  # seconds
        interval = args.get('interval', 1)   # seconds
        
        monitoring_data = {
            'start_time': datetime.now().isoformat(),
            'duration': duration,
            'interval': interval,
            'samples': []
        }
        
        end_time = time.time() + duration
        
        with Progress(
            SpinnerColumn(),
            TextColumn("[progress.description]{task.description}"),
            BarColumn(),
            TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
            console=console
        ) as progress:
            task = progress.add_task("Monitoring resources...", total=duration // interval)
            
            while time.time() < end_time:
                sample = self._collect_resource_sample()
                monitoring_data['samples'].append(sample)
                
                progress.update(task, advance=1)
                time.sleep(interval)
        
        monitoring_data['end_time'] = datetime.now().isoformat()
        monitoring_data['total_samples'] = len(monitoring_data['samples'])
        
        return {
            'success': True,
            'message': f"Resource monitoring completed for {duration} seconds.",
            'monitoring_data': monitoring_data
        }
    
    # Helper methods
    
    def _load_analysis_data(self, simulation_name: str, fallback_name: str) -> List[Dict[str, Any]]:
        """Load analysis data from various sources."""
        data = []
        
        # Try to load from experiment results
        if 'experiments' in self.nlm_cli.context:
            for exp_name, exp in self.nlm_cli.context['experiments'].items():
                if exp_name == simulation_name and 'results' in exp:
                    data.extend(exp['results'].get('stats_over_time', []))
        
        # Try to load from analysis results
        if not data and 'analysis_results' in self.nlm_cli.context:
            for analysis_name, analysis in self.nlm_cli.context['analysis_results'].items():
                if analysis_name == simulation_name and 'data' in analysis:
                    data.extend(analysis['data'])
        
        # Try to load from file
        if not data:
            data_file = Path.home() / '.nlm' / f"{simulation_name}_data.json"
            if data_file.exists():
                try:
                    with open(data_file, 'r') as f:
                        data = json.load(f)
                except Exception:
                    data = []
        
        # Fallback to empty data if not found
        if not data:
            # Try to load from brain context
            brain = self.nlm_cli.context.get('brain')
            if brain:
                from pynlm.enhanced_api import BrainStats
                brain_stats = BrainStats(brain)
                data = [{
                    'step': 0,
                    'timestamp': datetime.now().isoformat(),
                    'stats': brain_stats.get_summary()
                }]
        
        return data
    
    def _comprehensive_analysis(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform comprehensive analysis on data."""
        if not data:
            return {}
        
        # Extract metrics
        total_neurons = []
        total_synapses = []
        firing_rates = []
        spike_counts = []
        
        for point in data:
            stats = point.get('stats', {})
            total_neurons.append(stats.get('total_neurons', 0))
            total_synapses.append(stats.get('total_synapses', 0))
            firing_rates.append(stats.get('average_firing_rate', 0))
            spike_counts.append(stats.get('total_spikes', 0))
        
        # Calculate statistics
        analysis = {
            'data_points': len(data),
            'total_neurons': {
                'mean': statistics.mean(total_neurons) if total_neurons else 0,
                'min': min(total_neurons) if total_neurons else 0,
                'max': max(total_neurons) if total_neurons else 0,
                'std': statistics.stdev(total_neurons) if len(total_neurons) > 1 else 0
            },
            'total_synapses': {
                'mean': statistics.mean(total_synapses) if total_synapses else 0,
                'min': min(total_synapses) if total_synapses else 0,
                'max': max(total_synapses) if total_synapses else 0,
                'std': statistics.stdev(total_synapses) if len(total_synapses) > 1 else 0
            },
            'firing_rates': {
                'mean': statistics.mean(firing_rates) if firing_rates else 0,
                'min': min(firing_rates) if firing_rates else 0,
                'max': max(firing_rates) if firing_rates else 0,
                'std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0
            },
            'spike_counts': {
                'mean': statistics.mean(spike_counts) if spike_counts else 0,
                'min': min(spike_counts) if spike_counts else 0,
                'max': max(spike_counts) if spike_counts else 0,
                'std': statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0
            },
            'time_range': {
                'start': data[0]['timestamp'] if data else None,
                'end': data[-1]['timestamp'] if data else None,
                'duration_seconds': (datetime.fromisoformat(data[-1]['timestamp'].replace('Z', '+00:00')) - 
                                   datetime.fromisoformat(data[0]['timestamp'].replace('Z', '+00:00'))).total_seconds() if len(data) > 1 else 0
            }
        }
        
        return analysis
    
    def _performance_analysis(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform performance analysis."""
        # Simplified performance analysis
        analysis = {
            'performance_score': 0,
            'efficiency_metrics': {},
            'resource_usage': {}
        }
        
        if data:
            # Calculate simple performance score
            final_stats = data[-1].get('stats', {})
            neurons = final_stats.get('total_neurons', 0)
            synapses = final_stats.get('total_synapses', 0)
            firing_rate = final_stats.get('average_firing_rate', 0)
            spike_count = final_stats.get('total_spikes', 0)
            
            # Simple score calculation
            performance_score = min(100, (neurons / 1000) * 10 + (synapses / 10000) * 10 + 
                                  min(50, firing_rate) + min(40, spike_count / 100))
            
            analysis['performance_score'] = round(performance_score, 2)
        
        return analysis
    
    def _neural_analysis(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform neural analysis."""
        analysis = {
            'connectivity_metrics': {},
            'activation_patterns': {},
            'development_stage': None
        }
        
        if data:
            final_stats = data[-1].get('stats', {})
            analysis['development_stage'] = final_stats.get('development_stage')
        
        return analysis
    
    def _temporal_analysis(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform temporal analysis."""
        analysis = {
            'stability': 0,
            'trend': 'stable',
            'variability': {}
        }
        
        if len(data) > 1:
            # Calculate temporal stability
            firing_rates = []
            for point in data:
                stats = point.get('stats', {})
                firing_rates.append(stats.get('average_firing_rate', 0))
            
            if firing_rates:
                mean_rate = statistics.mean(firing_rates)
                std_rate = statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0
                
                coefficient_of_variation = std_rate / mean_rate if mean_rate > 0 else 0
                analysis['variability']['firing_rate_coefficient_of_variation'] = coefficient_of_variation
                
                if coefficient_of_variation < 0.1:
                    analysis['stability'] = 100 - coefficient_of_variation * 1000
                    analysis['trend'] = 'very_stable'
                elif coefficient_of_variation < 0.3:
                    analysis['stability'] = 100 - coefficient_of_variation * 300
                    analysis['trend'] = 'stable'
                elif coefficient_of_variation < 0.5:
                    analysis['stability'] = 100 - coefficient_of_variation * 100
                    analysis['trend'] = 'unstable'
                else:
                    analysis['stability'] = max(0, 100 - coefficient_of_variation * 50)
                    analysis['trend'] = 'very_unstable'
        
        return analysis
    
    def _generate_performance_report(self, data: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Generate performance report."""
        report = {
            'report_metadata': {
                'generated_at': datetime.now().isoformat(),
                'data_points': len(data),
                'metrics_requested': metrics
            },
            'executive_summary': {},
            'detailed_metrics': {},
            'recommendations': []
        }
        
        if not data:
            return report
        
        # Generate executive summary
        final_stats = data[-1].get('stats', {})
        report['executive_summary'] = {
            'total_neurons': final_stats.get('total_neurons', 0),
            'total_synapses': final_stats.get('total_synapses', 0),
            'average_firing_rate': final_stats.get('average_firing_rate', 0),
            'total_spikes': final_stats.get('total_spikes', 0),
            'development_stage': final_stats.get('development_stage'),
            'recommendation': 'Performance is normal.'
        }
        
        # Generate recommendations
        firing_rate = final_stats.get('average_firing_rate', 0)
        if firing_rate < 1.0:
            report['recommendations'].append("Low firing rate detected. Consider adjusting neural parameters.")
        elif firing_rate > 50:
            report['recommendations'].append("High firing rate detected. Consider reducing excitation.")
        
        neuron_count = final_stats.get('total_neurons', 0)
        if neuron_count > 5000:
            report['recommendations'].append("Large neuron count. Consider memory optimization.")
        
        return report
    
    def _create_comprehensive_plots(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Create comprehensive plots (JSON representation)."""
        plots = {
            'activity_over_time': [],
            'neuron_growth': [],
            'synapse_growth': [],
            'firing_rate_trend': []
        }
        
        if not data:
            return plots
        
        for point in data:
            stats = point.get('stats', {})
            
            plots['activity_over_time'].append({
                'step': point.get('step', 0),
                'total_spikes': stats.get('total_spikes', 0),
                'active_neurons': stats.get('active_neurons', 0),
                'firing_neurons': stats.get('firing_neurons', 0)
            })
            
            plots['neuron_growth'].append({
                'step': point.get('step', 0),
                'total_neurons': stats.get('total_neurons', 0)
            })
            
            plots['synapse_growth'].append({
                'step': point.get('step', 0),
                'total_synapses': stats.get('total_synapses', 0)
            })
            
            plots['firing_rate_trend'].append({
                'step': point.get('step', 0),
                'firing_rate': stats.get('average_firing_rate', 0)
            })
        
        return plots
    
    def _create_activity_plots(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Create activity plots (JSON representation)."""
        plots = {
            'firing_activity': [],
            'spike_count_trend': [],
            'activation_heatmap': []
        }
        
        if not data:
            return plots
        
        for point in data:
            stats = point.get('stats', {})
            
            plots['firing_activity'].append({
                'step': point.get('step', 0),
                'firing_rate': stats.get('average_firing_rate', 0),
                'excitation_inhibition_ratio': stats.get('excitation_inhibition_ratio', 0)
            })
            
            plots['spike_count_trend'].append({
                'step': point.get('step', 0),
                'total_spikes': stats.get('total_spikes', 0),
                'spike_rate': stats.get('total_spikes', 0) / max(1, point.get('step', 1))
            })
        
        return plots
    
    def _create_network_plots(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Create network plots (JSON representation)."""
        plots = {
            'connectivity_matrix': [],
            'region_activity': []
        }
        
        if not data:
            return plots
        
        for point in data:
            stats = point.get('stats', {})
            
            plots['region_activity'].append({
                'step': point.get('step', 0),
                'region_count': stats.get('region_count', 0),
                'development_stage': stats.get('development_stage', 0)
            })
        
        return plots
    
    def _create_temporal_plots(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Create temporal plots (JSON representation)."""
        plots = {
            'time_series': [],
            'rolling_average': [],
            'periodicity_analysis': []
        }
        
        if not data:
            return plots
        
        for point in data:
            stats = point.get('stats', {})
            
            plots['time_series'].append({
                'step': point.get('step', 0),
                'firing_rate': stats.get('average_firing_rate', 0),
                'active_neurons': stats.get('active_neurons', 0),
                'spike_count': stats.get('total_spikes', 0)
            })
        
        return plots
    
    def _detect_temporal_patterns(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect temporal patterns."""
        patterns = {
            'periodic_patterns': [],
            'trends': [],
            'cycles': []
        }
        
        if not data:
            return patterns
        
        # Simple pattern detection
        firing_rates = []
        for point in data:
            stats = point.get('stats', {})
            firing_rates.append(stats.get('average_firing_rate', 0))
        
        if len(firing_rates) > 10:
            # Detect linear trend
            x = list(range(len(firing_rates)))
            y = firing_rates
            
            if len(x) > 1:
                correlation = sum((xi - statistics.mean(x)) * (yi - statistics.mean(y)) 
                                 for xi, yi in zip(x, y)) / (len(x) * statistics.stdev(x) * statistics.stdev(y))
                
                if abs(correlation) > 0.7:
                    trend = 'increasing' if correlation > 0 else 'decreasing'
                    patterns['trends'].append({
                        'type': 'linear',
                        'correlation': correlation,
                        'direction': trend
                    })
        
        return patterns
    
    def _detect_neural_patterns(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect neural patterns."""
        patterns = {
            'connectivity_patterns': [],
            'activity_patterns': []
        }
        
        if not data:
            return patterns
        
        # Analyze connectivity and activity patterns
        for i, point in enumerate(data):
            stats = point.get('stats', {})
            
            # Activity pattern detection
            firing_rate = stats.get('average_firing_rate', 0)
            spike_count = stats.get('total_spikes', 0)
            
            if firing_rate < 1.0 and spike_count < 10:
                patterns['activity_patterns'].append({
                    'step': point.get('step', 0),
                    'type': 'quiescent',
                    'firing_rate': firing_rate,
                    'spike_count': spike_count
                })
            elif firing_rate > 20:
                patterns['activity_patterns'].append({
                    'step': point.get('step', 0),
                    'type': 'hyperactive',
                    'firing_rate': firing_rate,
                    'spike_count': spike_count
                })
        
        return patterns
    
    def _detect_activity_patterns(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect activity patterns."""
        patterns = {
            'burst_patterns': [],
            'oscillations': []
        }
        
        return patterns
    
    def _detect_temporal_anomalies(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect temporal anomalies."""
        anomalies = {
            'outliers': [],
            'jumps': []
        }
        
        if not data:
            return anomalies
        
        firing_rates = []
        for point in data:
            stats = point.get('stats', {})
            firing_rates.append(stats.get('average_firing_rate', 0))
        
        # Simple anomaly detection using z-score
        if len(firing_rates) > 3:
            mean_rate = statistics.mean(firing_rates)
            std_rate = statistics.stdev(firing_rates) if len(firing_rates) > 1 else 1
            
            for i, rate in enumerate(firing_rates):
                z_score = abs(rate - mean_rate) / std_rate if std_rate > 0 else 0
                if z_score > 2:  # More than 2 standard deviations
                    anomalies['outliers'].append({
                        'step': i,
                        'firing_rate': rate,
                        'z_score': z_score
                    })
        
        return anomalies
    
    def _detect_neural_anomalies(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect neural anomalies."""
        anomalies = {
            'abnormal_connectivity': [],
            'unusual_activity': []
        }
        
        return anomalies
    
    def _detect_activity_anomalies(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect activity anomalies."""
        anomalies = {
            'unexpected_events': []
        }
        
        return anomalies
    
    def _compare_statistical_data(self, data1: List[Dict[str, Any]], data2: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare statistical data between two datasets."""
        comparison = {
            'mean_comparison': {},
            'distribution_comparison': {},
            'similarity_score': 0
        }
        
        # Extract key metrics from both datasets
        metrics1 = self._extract_metrics_from_data(data1)
        metrics2 = self._extract_metrics_from_data(data2)
        
        # Compare means
        for metric_name in set(metrics1.keys()) | set(metrics2.keys()):
            mean1 = metrics1.get(metric_name, {}).get('mean', 0)
            mean2 = metrics2.get(metric_name, {}).get('mean', 0)
            
            comparison['mean_comparison'][metric_name] = {
                'dataset1': mean1,
                'dataset2': mean2,
                'difference': mean2 - mean1,
                'relative_difference': (mean2 - mean1) / mean1 if mean1 != 0 else float('inf')
            }
        
        # Calculate similarity score
        if metrics1 and metrics2:
            all_metrics = set(metrics1.keys()) | set(metrics2.keys())
            similarity_scores = []
            
            for metric in all_metrics:
                metric1 = metrics1.get(metric, {})
                metric2 = metrics2.get(metric, {})
                
                mean1 = metric1.get('mean', 0)
                mean2 = metric2.get('mean', 0)
                
                if mean1 == 0 and mean2 == 0:
                    similarity_scores.append(1.0)
                elif mean1 == 0 or mean2 == 0:
                    similarity_scores.append(0.0)
                else:
                    similarity = 1 - min(abs(mean1 - mean2) / max(mean1, mean2), 1.0)
                    similarity_scores.append(similarity)
            
            comparison['similarity_score'] = statistics.mean(similarity_scores) if similarity_scores else 0
        
        return comparison
    
    def _extract_metrics_from_data(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Extract metrics from data."""
        metrics = {}
        
        if not data:
            return metrics
        
        # Extract key metrics
        neurons = []
        synapses = []
        firing_rates = []
        spike_counts = []
        
        for point in data:
            stats = point.get('stats', {})
            neurons.append(stats.get('total_neurons', 0))
            synapses.append(stats.get('total_synapses', 0))
            firing_rates.append(stats.get('average_firing_rate', 0))
            spike_counts.append(stats.get('total_spikes', 0))
        
        # Calculate statistics
        metrics['total_neurons'] = {
            'mean': statistics.mean(neurons) if neurons else 0,
            'min': min(neurons) if neurons else 0,
            'max': max(neurons) if neurons else 0,
            'std': statistics.stdev(neurons) if len(neurons) > 1 else 0
        }
        
        metrics['total_synapses'] = {
            'mean': statistics.mean(synapses) if synapses else 0,
            'min': min(synapses) if synapses else 0,
            'max': max(synapses) if synapses else 0,
            'std': statistics.stdev(synapses) if len(synapses) > 1 else 0
        }
        
        metrics['average_firing_rate'] = {
            'mean': statistics.mean(firing_rates) if firing_rates else 0,
            'min': min(firing_rates) if firing_rates else 0,
            'max': max(firing_rates) if firing_rates else 0,
            'std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0
        }
        
        metrics['total_spikes'] = {
            'mean': statistics.mean(spike_counts) if spike_counts else 0,
            'min': min(spike_counts) if spike_counts else 0,
            'max': max(spike_counts) if spike_counts else 0,
            'std': statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0
        }
        
        return metrics
    
    def _compare_performance_metrics(self, data1: List[Dict[str, Any]], data2: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare performance metrics between datasets."""
        perf_comparison = {
            'performance_score': 0,
            'efficiency': {},
            'improvement': {}
        }
        
        # Calculate performance scores for both datasets
        perf1 = self._calculate_performance_score(data1)
        perf2 = self._calculate_performance_score(data2)
        
        perf_comparison['performance_score'] = {
            'dataset1': perf1,
            'dataset2': perf2,
            'difference': perf2 - perf1,
            'improvement_percentage': ((perf2 - perf1) / perf1 * 100) if perf1 != 0 else float('inf')
        }
        
        return perf_comparison
    
    def _compare_neural_metrics(self, data1: List[Dict[str, Any]], data2: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare neural metrics between datasets."""
        neural_comparison = {
            'neural_activity': {},
            'connectivity': {},
            'development': {}
        }
        
        # Compare neural activity patterns
        neural1 = self._extract_neural_metrics(data1)
        neural2 = self._extract_neural_metrics(data2)
        
        for metric_name in set(neural1.keys()) | set(neural2.keys()):
            metric1 = neural1.get(metric_name, {})
            metric2 = neural2.get(metric_name, {})
            
            neural_comparison['neural_activity'][metric_name] = {
                'dataset1': metric1,
                'dataset2': metric2,
                'difference': metric2 - metric1 if isinstance(metric1, (int, float)) else {}
            }
        
        return neural_comparison
    
    def _compare_temporal_patterns(self, data1: List[Dict[str, Any]], data2: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare temporal patterns between datasets."""
        temporal_comparison = {
            'stability': {},
            'trends': {},
            'variability': {}
        }
        
        # Compare temporal patterns
        temporal1 = self._extract_temporal_metrics(data1)
        temporal2 = self._extract_temporal_metrics(data2)
        
        temporal_comparison['stability'] = {
            'dataset1': temporal1.get('stability', 0),
            'dataset2': temporal2.get('stability', 0),
            'difference': temporal2.get('stability', 0) - temporal1.get('stability', 0)
        }
        
        return temporal_comparison
    
    def _extract_neural_metrics(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Extract neural metrics from data."""
        metrics = {}
        
        if not data:
            return metrics
        
        final_stats = data[-1].get('stats', {})
        
        metrics['total_neurons'] = final_stats.get('total_neurons', 0)
        metrics['total_synapses'] = final_stats.get('total_synapses', 0)
        metrics['average_firing_rate'] = final_stats.get('average_firing_rate', 0)
        metrics['excitation_inhibition_ratio'] = final_stats.get('excitation_inhibition_ratio', 0)
        metrics['development_stage'] = final_stats.get('development_stage', 0)
        
        return metrics
    
    def _extract_temporal_metrics(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Extract temporal metrics from data."""
        metrics = {}
        
        if not data or len(data) < 2:
            return metrics
        
        # Calculate temporal stability
        firing_rates = []
        for point in data:
            stats = point.get('stats', {})
            firing_rates.append(stats.get('average_firing_rate', 0))
        
        if firing_rates:
            mean_rate = statistics.mean(firing_rates)
            std_rate = statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0
            
            coefficient_of_variation = std_rate / mean_rate if mean_rate > 0 else 0
            metrics['stability'] = 100 - coefficient_of_variation * 1000
            metrics['variability'] = coefficient_of_variation
        
        return metrics
    
    def _calculate_performance_score(self, data: List[Dict[str, Any]]) -> float:
        """Calculate performance score for a dataset."""
        if not data:
            return 0
        
        final_stats = data[-1].get('stats', {})
        neurons = final_stats.get('total_neurons', 0)
        synapses = final_stats.get('total_synapses', 0)
        firing_rate = final_stats.get('average_firing_rate', 0)
        spike_count = final_stats.get('total_spikes', 0)
        
        # Simple performance score calculation
        performance_score = min(100, (neurons / 1000) * 10 + (synapses / 10000) * 10 + 
                              min(50, firing_rate) + min(40, spike_count / 100))
        
        return performance_score
    
    def _export_csv_report(self, report: Dict[str, Any], output_path: Path):
        """Export report as CSV."""
        import csv
        
        with open(output_path, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Write header
            writer.writerow(['Metric', 'Value'])
            
            # Write report data
            def write_dict_recursive(d, prefix=''):
                for key, value in d.items():
                    if isinstance(value, dict):
                        write_dict_recursive(value, f"{prefix}{key}.")
                    else:
                        writer.writerow([f"{prefix}{key}", value])
            
            write_dict_recursive(report)
    
    def _export_text_report(self, report: Dict[str, Any], output_path: Path):
        """Export report as text."""
        with open(output_path, 'w') as f:
            f.write("Performance Report\n")
            f.write("=" * 50 + "\n\n")
            
            # Write report in readable format
            self._write_dict_to_text(report, f)
    
    def _write_dict_to_text(self, d: Dict[str, Any], f, indent: int = 0):
        """Write dictionary to text file."""
        for key, value in d.items():
            if isinstance(value, dict):
                f.write(f"{' ' * indent}{key}:\n")
                self._write_dict_to_text(value, f, indent + 2)
            elif isinstance(value, list):
                f.write(f"{' ' * indent}{key}:\n")
                for item in value:
                    if isinstance(item, dict):
                        self._write_dict_to_text(item, f, indent + 4)
                    else:
                        f.write(f"{' ' * (indent + 2)}- {item}\n")
            else:
                f.write(f"{' ' * indent}{key}: {value}\n")
    
    def _export_csv_analysis(self, analysis_data: Dict[str, Any], output_path: Path):
        """Export analysis data as CSV."""
        import csv
        
        with open(output_path, 'w', newline='') as f:
            # Flatten the analysis data
            rows = self._flatten_dict_for_csv(analysis_data)
            
            writer = csv.writer(f)
            writer.writerow(['Category', 'Metric', 'Value'])
            
            for category, metrics in rows.items():
                for metric, value in metrics.items():
                    writer.writerow([category, metric, value])
    
    def _flatten_dict_for_csv(self, d: Dict[str, Any]) -> Dict[str, Dict[str, Any]]:
        """Flatten dictionary for CSV export."""
        flattened = {}
        
        def flatten(d, prefix=''):
            for key, value in d.items():
                if isinstance(value, dict):
                    flatten(value, f"{prefix}{key}_")
                elif isinstance(value, list):
                    for i, item in enumerate(value):
                        if isinstance(item, dict):
                            flatten(item, f"{prefix}{key}_{i}_")
                        else:
                            flattened.setdefault(f"{prefix}{key}", {})[f"item_{i}"] = item
                else:
                    flattened.setdefault(prefix, {})[key] = value
        
        flatten(d)
        return flattened
    
    def _export_text_analysis(self, analysis_data: Dict[str, Any], output_path: Path):
        """Export analysis data as text."""
        with open(output_path, 'w') as f:
            f.write("Analysis Results\n")
            f.write("=" * 50 + "\n\n")
            
            self._write_dict_to_text(analysis_data, f)
    
    def _collect_resource_sample(self) -> Dict[str, Any]:
        """Collect a sample of system resources."""
        import psutil
        import os
        
        sample = {}
        
        # CPU usage
        sample['cpu_percent'] = psutil.cpu_percent(interval=0.1)
        sample['cpu_cores'] = psutil.cpu_count()
        
        # Memory usage
        memory = psutil.virtual_memory()
        sample['memory_percent'] = memory.percent
        sample['memory_used_gb'] = memory.used / (1024**3)
        sample['memory_available_gb'] = memory.available / (1024**3)
        
        # Disk usage
        disk = psutil.disk_usage('/')
        sample['disk_percent'] = disk.percent
        sample['disk_used_gb'] = disk.used / (1024**3)
        
        # Network
        net_io = psutil.net_io_counters()
        sample['network_sent_bytes'] = net_io.bytes_sent
        sample['network_recv_bytes'] = net_io.bytes_recv
        
        # Process
        process = psutil.Process(os.getpid())
        sample['process_cpu_percent'] = process.cpu_percent(interval=0.1)
        sample['process_memory_mb'] = process.memory_info().rss / (1024**2)
        
        sample['timestamp'] = time.time()
        
        return sample
