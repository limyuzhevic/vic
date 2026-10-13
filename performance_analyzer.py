#!/usr/bin/env bash
# Performance Analyzer for NLM
# Advanced neural activity analysis and visualization tools

import os
import sys
import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, Any, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
import time
import math
from pathlib import Path

class AnalysisType(Enum):
    SPATIAL = "spatial"
    TEMPORAL = "temporal"
    FUNCTIONAL = "functional"
    DEVELOPMENTAL = "developmental"
    CONNECTIVITY = "connectivity"
    PLACILITY = "plasticity"
    MEMORY = "memory"

@dataclass
class NeuralActivityData:
    neuron_id: int
    region_id: int
    population_id: int
    neuron_type: str
    membrane_potential: float
    firing_state: str
    last_spike_time: float
    activation_history: List[float]
    synaptic_weights: Dict[int, float]
    plasticity_metrics: Dict[str, float]
    memory_contributions: Dict[str, float]

@dataclass
class AnalysisResult:
    analysis_type: AnalysisType
    timestamp: float
    duration: float
    neurons_analyzed: int
    metrics: Dict[str, Any]
    patterns: List[Dict[str, Any]]
    anomalies: List[Dict[str, Any]]
    recommendations: List[str]

class NeuralAnalyzer:
    def __init__(self):
        self.activity_data: List[NeuralActivityData] = []
        self.analysis_cache: Dict[str, Any] = {}
        self.performance_thresholds: Dict[str, float] = {
            'normal_firing_rate': 0.1,
            'high_firing_rate': 5.0,
            'critical_memory_load': 0.8,
            'optimal_plasticity': 0.5
        }
        self.visualization_cache: Dict[str, Any] = {}
    
    def load_brain_state(self, brain_file: str) -> bool:
        """Load neural activity data from brain state file"""
        try:
            if brain_file.endswith('.npy'):
                return self._load_numpy_format(brain_file)
            elif brain_file.endswith('.json'):
                return self._load_json_format(brain_file)
            elif brain_file.endswith('.bin'):
                return self._load_binary_format(brain_file)
            else:
                print(f"Unsupported brain file format: {brain_file}")
                return False
        except Exception as e:
            print(f"Error loading brain state: {e}")
            return False
    
    def _load_numpy_format(self, filepath: str) -> bool:
        """Load brain state from numpy format"""
        try:
            data = np.load(filepath, allow_pickle=True).item()
            
            # Convert to NeuralActivityData objects
            for neuron_data in data.get('neurons', []):
                activity = NeuralActivityData(
                    neuron_id=neuron_data.get('id', 0),
                    region_id=neuron_data.get('region_id', 0),
                    population_id=neuron_data.get('population_id', 0),
                    neuron_type=neuron_data.get('type', 'internal'),
                    membrane_potential=neuron_data.get('membrane_potential', 0.0),
                    firing_state=neuron_data.get('firing_state', 'resting'),
                    last_spike_time=neuron_data.get('last_spike_time', 0.0),
                    activation_history=neuron_data.get('activation_history', []),
                    synaptic_weights=neuron_data.get('synaptic_weights', {}),
                    plasticity_metrics=neuron_data.get('plasticity_metrics', {}),
                    memory_contributions=neuron_data.get('memory_contributions', {})
                )
                self.activity_data.append(activity)
            
            print(f"Loaded {len(self.activity_data)} neurons from {filepath}")
            return True
        except Exception as e:
            print(f"Error loading numpy format: {e}")
            return False
    
    def _load_json_format(self, filepath: str) -> bool:
        """Load brain state from JSON format"""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            for neuron_data in data.get('neurons', []):
                activity = NeuralActivityData(
                    neuron_id=neuron_data.get('id', 0),
                    region_id=neuron_data.get('region_id', 0),
                    population_id=neuron_data.get('population_id', 0),
                    neuron_type=neuron_data.get('type', 'internal'),
                    membrane_potential=neuron_data.get('membrane_potential', 0.0),
                    firing_state=neuron_data.get('firing_state', 'resting'),
                    last_spike_time=neuron_data.get('last_spike_time', 0.0),
                    activation_history=neuron_data.get('activation_history', []),
                    synaptic_weights=neuron_data.get('synaptic_weights', {}),
                    plasticity_metrics=neuron_data.get('plasticity_metrics', {}),
                    memory_contributions=neuron_data.get('memory_contributions', {})
                )
                self.activity_data.append(activity)
            
            print(f"Loaded {len(self.activity_data)} neurons from {filepath}")
            return True
        except Exception as e:
            print(f"Error loading JSON format: {e}")
            return False
    
    def _load_binary_format(self, filepath: str) -> bool:
        """Load brain state from binary format"""
        try:
            with open(filepath, 'rb') as f:
                # Simplified binary format for demonstration
                header_size = 64
                header = f.read(header_size)
                neuron_count = int.from_bytes(header[32:36], byteorder='little')
                
                # Read neuron data
                for i in range(min(neuron_count, 10000)):  # Limit for performance
                    neuron_data = f.read(128)  # Simplified neuron record size
                    if len(neuron_data) < 128:
                        break
                    
                    activity = NeuralActivityData(
                        neuron_id=i,
                        region_id=1,
                        population_id=1,
                        neuron_type='internal',
                        membrane_potential=0.5,
                        firing_state='resting',
                        last_spike_time=0.0,
                        activation_history=[0.5] * 100,
                        synaptic_weights={},
                        plasticity_metrics={},
                        memory_contributions={}
                    )
                    self.activity_data.append(activity)
            
            print(f"Loaded {len(self.activity_data)} neurons from {filepath}")
            return True
        except Exception as e:
            print(f"Error loading binary format: {e}")
            return False
    
    def analyze_spatial_distribution(self) -> AnalysisResult:
        """Analyze spatial distribution of neural activity"""
        start_time = time.time()
        
        if not self.activity_data:
            return AnalysisResult(
                analysis_type=AnalysisType.SPATIAL,
                timestamp=start_time,
                duration=0.0,
                neurons_analyzed=0,
                metrics={},
                patterns=[],
                anomalies=[],
                recommendations=[]
            )
        
        # Analyze by region
        regions = {}
        neuron_types = {}
        firing_states = {}
        
        for neuron in self.activity_data:
            # Region analysis
            if neuron.region_id not in regions:
                regions[neuron.region_id] = {
                    'neuron_count': 0,
                    'total_activity': 0.0,
                    'average_firing_rate': 0.0
                }
            
            regions[neuron.region_id]['neuron_count'] += 1
            regions[neuron.region_id]['total_activity'] += neuron.membrane_potential
            
            # Type analysis
            if neuron.neuron_type not in neuron_types:
                neuron_types[neuron.neuron_type] = {
                    'neuron_count': 0,
                    'activity_distribution': []
                }
            
            neuron_types[neuron.neuron_type]['neuron_count'] += 1
            neuron_types[neuron.neuron_type]['activity_distribution'].append(neuron.membrane_potential)
            
            # State analysis
            if neuron.firing_state not in firing_states:
                firing_states[neuron.firing_state] = 0
            firing_states[neuron.firing_state] += 1
        
        # Calculate averages
        for region_id, region_data in regions.items():
            if region_data['neuron_count'] > 0:
                region_data['average_firing_rate'] = region_data['total_activity'] / region_data['neuron_count']
        
        for neuron_type, type_data in neuron_types.items():
            if type_data['neuron_count'] > 0:
                type_data['average_activity'] = sum(type_data['activity_distribution']) / len(type_data['activity_distribution'])
                type_data['activity_std'] = np.std(type_data['activity_distribution']) if len(type_data['activity_distribution']) > 1 else 0.0
        
        # Identify patterns
        patterns = []
        
        # Find regions with highest activity
        if regions:
            max_region = max(regions.items(), key=lambda x: x[1]['average_firing_rate'])
            patterns.append({
                'type': 'high_activity_region',
                'region_id': max_region[0],
                'average_activity': max_region[1]['average_firing_rate'],
                'neuron_count': max_region[1]['neuron_count']
            })
        
        # Find neuron types with highest activity
        if neuron_types:
            max_type = max(neuron_types.items(), key=lambda x: x[1]['average_activity'])
            patterns.append({
                'type': 'high_activity_neuron_type',
                'neuron_type': max_type[0],
                'average_activity': max_type[1]['average_activity'],
                'neuron_count': max_type[1]['neuron_count']
            })
        
        # Find firing state distribution
        total_neurons = len(self.activity_data)
        if total_neurons > 0:
            firing_distribution = {state: count/total_neurons * 100 for state, count in firing_states.items()}
            patterns.append({
                'type': 'firing_state_distribution',
                'distribution': firing_distribution
            })
        
        duration = time.time() - start_time
        
        return AnalysisResult(
            analysis_type=AnalysisType.SPATIAL,
            timestamp=start_time,
            duration=duration,
            neurons_analyzed=total_neurons,
            metrics={
                'region_count': len(regions),
                'neuron_type_count': len(neuron_types),
                'firing_state_count': len(firing_states),
                'average_activity_per_region': sum(r['average_firing_rate'] for r in regions.values()) / len(regions) if regions else 0.0
            },
            patterns=patterns,
            anomalies=self._identify_spatial_anomalies(regions, neuron_types),
            recommendations=self._generate_spatial_recommendations(regions, neuron_types)
        )
    
    def analyze_temporal_dynamics(self) -> AnalysisResult:
        """Analyze temporal dynamics of neural activity"""
        start_time = time.time()
        
        if not self.activity_data:
            return AnalysisResult(
                analysis_type=AnalysisType.TEMPORAL,
                timestamp=start_time,
                duration=0.0,
                neurons_analyzed=0,
                metrics={},
                patterns=[],
                anomalies=[],
                recommendations=[]
            )
        
        # Calculate temporal metrics
        activation_histories = {}
        spike_times = []
        
        for neuron in self.activity_data:
            # Find neurons with spike history
            if neuron.firing_state == 'active' and neuron.last_spike_time > 0:
                spike_times.append(neuron.last_spike_time)
            
            # Aggregate activation histories
            if neuron.activation_history:
                activation_histories[neuron.neuron_id] = neuron.activation_history
        
        # Analyze temporal patterns
        metrics = {}
        patterns = []
        
        if spike_times:
            # Calculate spike frequency statistics
            if len(spike_times) > 1:
                spike_intervals = np.diff(sorted(spike_times))
                metrics['average_spike_interval'] = np.mean(spike_intervals)
                metrics['spike_interval_std'] = np.std(spike_intervals)
                metrics['spike_frequency'] = 1.0 / metrics['average_spike_interval'] if metrics['average_spike_interval'] > 0 else 0.0
            
            # Identify burst patterns
            if len(spike_times) >= 3:
                intervals = np.diff(sorted(spike_times))
                short_intervals = intervals[intervals < 0.01]  # Bursts
                if len(short_intervals) > 0:
                    metrics['burst_count'] = len(short_intervals)
                    metrics['burst_percentage'] = len(short_intervals) / len(intervals) * 100
        
        # Analyze activation history trends
        if activation_histories:
            history_lengths = [len(hist) for hist in activation_histories.values()]
            metrics['average_history_length'] = np.mean(history_lengths)
            metrics['max_history_length'] = max(history_lengths)
            
            # Calculate trend analysis
            trends = []
            for neuron_id, history in activation_histories.items():
                if len(history) > 5:
                    x = np.arange(len(history))
                    y = np.array(history)
                    slope = np.polyfit(x, y, 1)[0]
                    trends.append(slope)
            
            if trends:
                metrics['average_trend_slope'] = np.mean(trends)
                metrics['trend_direction'] = 'increasing' if np.mean(trends) > 0 else 'decreasing'
        
        duration = time.time() - start_time
        
        return AnalysisResult(
            analysis_type=AnalysisType.TEMPORAL,
            timestamp=start_time,
            duration=duration,
            neurons_analyzed=len(self.activity_data),
            metrics=metrics,
            patterns=patterns,
            anomalies=self._identify_temporal_anomalies(metrics),
            recommendations=self._generate_temporal_recommendations(metrics)
        )
    
    def analyze_functional_connectivity(self) -> AnalysisResult:
        """Analyze functional connectivity between neurons"""
        start_time = time.time()
        
        if not self.activity_data:
            return AnalysisResult(
                analysis_type=AnalysisType.FUNCTIONAL,
                timestamp=start_time,
                duration=0.0,
                neurons_analyzed=0,
                metrics={},
                patterns=[],
                anomalies=[],
                recommendations=[]
            )
        
        # Build connectivity matrix (simplified)
        neuron_count = len(self.activity_data)
        connectivity_matrix = np.zeros((neuron_count, neuron_count))
        
        # Calculate functional connectivity based on correlation
        # For simplicity, we'll use a placeholder implementation
        for i, neuron_i in enumerate(self.activity_data):
            for j, neuron_j in enumerate(self.activity_data):
                if i != j:
                    # Simplified correlation based on activity similarity
                    correlation = 0.0
                    if neuron_i.activation_history and neuron_j.activation_history:
                        min_len = min(len(neuron_i.activation_history), len(neuron_j.activation_history))
                        if min_len > 0:
                            hist_i = neuron_i.activation_history[-min_len:]
                            hist_j = neuron_j.activation_history[-min_len:]
                            correlation = np.corrcoef(hist_i, hist_j)[0, 1] if len(hist_i) > 1 else 0.0
                    
                    connectivity_matrix[i, j] = (correlation + 1) / 2  # Normalize to [0, 1]
        
        # Analyze connectivity patterns
        metrics = {}
        patterns = []
        
        # Calculate graph metrics
        threshold = 0.5
        adjacency_matrix = (connectivity_matrix > threshold).astype(int)
        
        # Count connections
        total_connections = np.sum(adjacency_matrix)
        avg_connections_per_neuron = total_connections / neuron_count if neuron_count > 0 else 0
        
        metrics['total_possible_connections'] = neuron_count * (neuron_count - 1)
        metrics['actual_connections'] = int(total_connections)
        metrics['connectivity_density'] = total_connections / (neuron_count * (neuron_count - 1)) if neuron_count > 1 else 0
        metrics['average_connections_per_neuron'] = avg_connections_per_neuron
        
        # Identify hubs (neurons with highest degree)
        if neuron_count > 0:
            degrees = np.sum(adjacency_matrix, axis=1)
            top_hubs = np.argsort(degrees)[::-1][:5]  # Top 5 hubs
            hub_indices = top_hubs.tolist()
            
            patterns.append({
                'type': 'hub_identification',
                'hub_neuron_ids': hub_indices,
                'hub_degrees': degrees[hub_indices].tolist(),
                'hub_degree_distribution': np.histogram(degrees, bins=10).tolist()
            })
        
        # Analyze clustering coefficient (simplified)
        if neuron_count > 2:
            clustering_coefficients = []
            for i in range(neuron_count):
                neighbors = np.where(adjacency_matrix[i] == 1)[0]
                if len(neighbors) >= 2:
                    # Count connections between neighbors
                    neighbor_connections = 0
                    for j in range(len(neighbors)):
                        for k in range(j + 1, len(neighbors)):
                            if adjacency_matrix[neighbors[j], neighbors[k]] == 1:
                                neighbor_connections += 1
                    
                    possible_neighbor_connections = len(neighbors) * (len(neighbors) - 1) / 2
                    if possible_neighbor_connections > 0:
                        clustering = neighbor_connections / possible_neighbor_connections
                        clustering_coefficients.append(clustering)
            
            if clustering_coefficients:
                metrics['average_clustering_coefficient'] = np.mean(clustering_coefficients)
                metrics['clustering_coefficient_std'] = np.std(clustering_coefficients)
        
        duration = time.time() - start_time
        
        return AnalysisResult(
            analysis_type=AnalysisType.FUNCTIONAL,
            timestamp=start_time,
            duration=duration,
            neurons_analyzed=neuron_count,
            metrics=metrics,
            patterns=patterns,
            anomalies=self._identify_functional_anomalies(metrics),
            recommendations=self._generate_functional_recommendations(metrics)
        )
    
    def _identify_spatial_anomalies(self, regions: Dict, neuron_types: Dict) -> List[Dict[str, Any]]:
        """Identify spatial anomalies"""
        anomalies = []
        
        # Check for regions with unusually low activity
        for region_id, region_data in regions.items():
            if region_data['average_firing_rate'] < 0.01:
                anomalies.append({
                    'type': 'low_activity_region',
                    'region_id': region_id,
                    'average_activity': region_data['average_firing_rate'],
                    'severity': 'high' if region_data['average_firing_rate'] < 0.001 else 'medium'
                })
        
        # Check for neuron types with unusual distribution
        for neuron_type, type_data in neuron_types.items():
            if type_data['neuron_count'] == 0:
                anomalies.append({
                    'type': 'missing_neuron_type',
                    'neuron_type': neuron_type,
                    'severity': 'medium'
                })
        
        return anomalies
    
    def _generate_spatial_recommendations(self, regions: Dict, neuron_types: Dict) -> List[str]:n\n        """Generate spatial recommendations"""
        recommendations = []
        
        # Recommend adjusting regions with low activity
        for region_id, region_data in regions.items():
            if region_data['average_firing_rate'] < 0.01:
                recommendations.append(
                    f"Region {region_id} has low average activity ({region_data['average_firing_rate']:.4f}). "
                    f"Consider increasing input or adjusting parameters."
                )
        
        # Recommend balancing neuron type distribution
        type_counts = {neuron_type: data['neuron_count'] for neuron_type, data in neuron_types.items()}
        if len(type_counts) > 0:
            max_count = max(type_counts.values())
            min_count = min(type_counts.values())
            ratio = max_count / min_count if min_count > 0 else 1
            
            if ratio > 10:
                recommendations.append(
                    f"Neuron type distribution is imbalanced (ratio: {ratio:.1f}). "
                    f"Consider adjusting population allocation."
                )
        
        return recommendations
    
    def _identify_temporal_anomalies(self, metrics: Dict) -> List[Dict[str, Any]]:
        """Identify temporal anomalies"""
        anomalies = []
        
        if 'average_spike_frequency' in metrics:
            spike_freq = metrics['average_spike_frequency']
            if spike_freq > self.performance_thresholds['high_firing_rate']:
                anomalies.append({
                    'type': 'high_spike_frequency',
                    'value': spike_freq,
                    'threshold': self.performance_thresholds['high_firing_rate'],
                    'severity': 'high'
                })
        
        return anomalies
    
    def _generate_temporal_recommendations(self, metrics: Dict) -> List[str]:
        """Generate temporal recommendations"""
        recommendations = []
        
        if 'spike_frequency' in metrics and metrics['spike_frequency'] > 10:
            recommendations.append(
                f"High spike frequency detected ({metrics['spike_frequency']:.2f} Hz). "
                f"Consider reducing input strength or increasing refractory period."
            )
        
        if 'average_trend_slope' in metrics and metrics['average_trend_slope'] > 0.1:
            recommendations.append(
                f"Activity trend is increasing ({metrics['average_trend_slope']:.4f}). "
                f"Monitor for runaway excitation."
            )
        
        return recommendations
    
    def _identify_functional_anomalies(self, metrics: Dict) -> List[Dict[str, Any]]:
        """Identify functional anomalies"""
        anomalies = []
        
        if 'connectivity_density' in metrics:
            density = metrics['connectivity_density']
            if density < 0.01:
                anomalies.append({
                    'type': 'low_connectivity',
                    'value': density,
                    'threshold': 0.01,
                    'severity': 'medium'
                })
        
        if 'average_clustering_coefficient' in metrics:
            clustering = metrics['average_clustering_coefficient']
            if clustering < 0.01:
                anomalies.append({
                    'type': 'low_clustering',
                    'value': clustering,
                    'threshold': 0.01,
                    'severity': 'low'
                })
        
        return anomalies
    
    def _generate_functional_recommendations(self, metrics: Dict) -> List[str]:
        """Generate functional recommendations"n"""
        recommendations = []
        
        if 'connectivity_density' in metrics and metrics['connectivity_density'] < 0.05:
            recommendations.append(
                f"Low functional connectivity density ({metrics['connectivity_density']:.4f}). "
                f"Consider adjusting synaptic connection probabilities."
            )
        
        if 'average_clustering_coefficient' in metrics and metrics['average_clustering_coefficient'] < 0.1:
            recommendations.append(
                f"Low clustering coefficient ({metrics['average_clustering_coefficient']:.4f}). "
                f"Network may lack local processing capabilities."
            )
        
        return recommendations
    
    def run_comprehensive_analysis(self) -> Dict[str, AnalysisResult]:
        """Run all analyses and return results"""
        analyses = {}
        
        print("Running comprehensive neural analysis...")
        
        # Run different types of analyses
        analyses['spatial'] = self.analyze_spatial_distribution()
        print("✓ Spatial distribution analysis completed")
        
        analyses['temporal'] = self.analyze_temporal_dynamics()
        print("✓ Temporal dynamics analysis completed")
        
        analyses['functional'] = self.analyze_functional_connectivity()
        print("✓ Functional connectivity analysis completed")
        
        # Additional analyses can be added here
        # analyses['plasticity'] = self.analyze_plasticity_dynamics()
        # analyses['memory'] = self.analyze_memory_dynamics()
        # analyses['developmental'] = self.analyze_developmental_dynamics()
        
        return analyses
    
    def generate_visualization_data(self, analysis_results: Dict[str, AnalysisResult]) -> Dict[str, Any]:
        """Generate visualization-ready data from analysis results"""
        visualization_data = {
            'charts': [],
            'graphs': [],
            'maps': [],
            'summary': {}
        }
        
        # Generate summary statistics
        total_neurons = len(self.activity_data)
        if total_neurons > 0:
            visualization_data['summary'] = {
                'total_neurons': total_neurons,
                'average_activity': np.mean([n.membrane_potential for n in self.activity_data]),
                'activity_std': np.std([n.membrane_potential for n in self.activity_data]),
                'firing_states': {state: sum(1 for n in self.activity_data if n.firing_state == state) 
                                 for state in set(n.firing_state for n in self.activity_data)}
            }
        
        # Generate chart data for spatial analysis
        if 'spatial' in analysis_results:
            spatial_data = analysis_results['spatial']
            chart_data = {
                'type': 'bar',
                'title': 'Activity Distribution by Region',
                'data': []
            }
            
            # Add data for regions
            for pattern in spatial_data.patterns:
                if pattern['type'] == 'high_activity_region':
                    chart_data['data'].append({
                        'name': f"Region {pattern['region_id']}",
                        'value': pattern['average_activity']
                    })
            
            visualization_data['charts'].append(chart_data)
        
        # Generate graph data for functional connectivity
        if 'functional' in analysis_results:
            functional_data = analysis_results['functional']
            graph_data = {
                'type': 'network',
                'title': 'Functional Connectivity Graph',
                'nodes': [],
                'edges': []
            }
            
            # Add nodes (neurons)
            for i, neuron in enumerate(self.activity_data[:100]):  # Limit for performance
                graph_data['nodes'].append({
                    'id': i,
                    'label': f"N{i}",
                    'value': neuron.membrane_potential,
                    'size': neuron.neuron_type.count('_') + 1
                })
            
            visualization_data['graphs'].append(graph_data)
        
        # Generate map data for temporal patterns
        if 'temporal' in analysis_results:
            temporal_data = analysis_results['temporal']
            map_data = {
                'type': 'timeline',
                'title': 'Temporal Activity Patterns',
                'data': []
            }
            
            if 'average_trend_slope' in temporal_data.metrics:
                map_data['data'].append({
                    'x': 'Trend Analysis',
                    'y': temporal_data.metrics['average_trend_slope'],
                    'label': f"Trend: {temporal_data.metrics.get('trend_direction', 'unknown')}"
                })
            
            visualization_data['maps'].append(map_data)
        
        return visualization_data

# Main command-line interface
def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='NLM Performance Analyzer')
    parser.add_argument('--brain', '-b', required=True, help='Brain state file to analyze')
    parser.add_argument('--output', '-o', help='Output file for analysis results')
    parser.add_argument('--format', choices=['json', 'html', 'png'], default='json',
                       help='Output format for visualization data')
    parser.add_argument('--analyses', nargs='+', default=['spatial', 'temporal', 'functional'],
                       help='Types of analyses to run')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    # Initialize analyzer
    analyzer = NeuralAnalyzer()
    
    # Load brain state
    print(f"Loading brain state from: {args.brain}")
    if not analyzer.load_brain_state(args.brain):
        print("Error: Failed to load brain state")
        return 1
    
    # Run analyses
    analysis_mapping = {
        'spatial': analyzer.analyze_spatial_distribution,
        'temporal': analyzer.analyze_temporal_dynamics,
        'functional': analyzer.analyze_functional_connectivity
    }
    
    selected_analyses = []
    for analysis_name in args.analyses:
        if analysis_name in analysis_mapping:
            selected_analyses.append((analysis_name, analysis_mapping[analysis_name]()))
        else:
            print(f"Warning: Unknown analysis type '{analysis_name}'")
    
    # Generate results
    print("\n" + "=" * 60)
    print("ANALYSIS RESULTS")
    print("=" * 60)
    
    # Create analysis results dict
    analysis_results = {}
    for analysis_name, result in selected_analyses:
        print(f"\n{analysis_name.upper()} ANALYSIS:")
        print(f"  Duration: {result.duration:.3f}s")
        print(f"  Neurons analyzed: {result.neurons_analyzed}")
        print(f"  Key metrics: {result.metrics}")
        
        if result.patterns:
            print(f"  Identified patterns: {len(result.patterns)}")
        
        if result.anomalies:
            print(f"  Anomalies detected: {len(result.anomalies)}")
            for anomaly in result.anomalies:
                print(f"    - {anomaly['type']}: {anomaly.get('message', 'N/A')}")
        
        analysis_results[analysis_name] = result
    
    # Generate visualization data
    print("\nGenerating visualization data...")
    viz_data = analyzer.generate_visualization_data(analysis_results)
    
    # Save results
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(viz_data, f, indent=2, default=str)
        print(f"\nAnalysis results saved to: {args.output}")
    
    # Generate additional visualization formats if requested
    if args.format == 'html':
        html_file = args.output.replace('.json', '.html') if args.output else 'analysis_results.html'
        _generate_html_report(viz_data, html_file)
        print(f"HTML report saved to: {html_file}")
    
    print("\nAnalysis completed successfully!")
    return 0

def _generate_html_report(data: Dict[str, Any], output_file: str):
    """Generate HTML report from analysis data"""
    html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Analysis Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric {{ display: inline-block; margin: 10px; padding: 10px; background-color: #f9f9f9; border-radius: 3px; }}
        .pattern {{ background-color: #e8f4fd; }}
        .anomaly {{ background-color: #ffe8e8; }}
        .recommendation {{ background-color: #e8f5e8; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Neural Analysis Report</h1>
        <p>Generated on {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>
    
    <div class="section">
        <h2>Summary Statistics</h2>
        <div class="metric">
            <h3>Total Neurons</h3>
            <p>{data.get('summary', {}).get('total_neurons', 'N/A')}</p>
        </div>
        <div class="metric">
            <h3>Average Activity</h3>
            <p>{data.get('summary', {}).get('average_activity', 'N/A'):.4f}</p>
        </div>
        <div class="metric">
            <h3>Activity Standard Deviation</h3>
            <p>{data.get('summary', {}).get('activity_std', 'N/A'):.4f}</p>
        </div>
    </div>
    
    <div class="section">
        <h2>Analysis Charts</h2>
        <!-- Charts would be rendered here in a real implementation -->
        <p>Chart data available in JSON format</p>
    </div>
    
    <div class="section">
        <h2>Identified Patterns</h2>
        {''.join(f'<div class="pattern"><h3>{pattern.get("type", "Unknown")}</h3><pre>{json.dumps(pattern, indent=2)}</pre></div>' for pattern in data.get('charts', []))}
    </div>
    
    <div class="section">
        <h2>Anomalies Detected</h2>
        {''.join(f'<div class="anomaly"><h3>{anomaly.get("type", "Unknown")}</h3><pre>{json.dumps(anomaly, indent=2)}</pre></div>' for anomaly in data.get('anomalies', []))}
    </div>
    
    <div class="section">
        <h2>Recommendations</h2>
        {''.join(f'<div class="recommendation"><pre>{rec}</pre></div>' for rec in data.get('recommendations', []))}
    </div>
</body>
</html>
"""
    
    with open(output_file, 'w') as f:
        f.write(html_content)

if __name__ == '__main__':
    exit(main())
