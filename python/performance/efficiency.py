"""
Efficiency analysis module for NLM neural simulation framework.

This module provides efficiency analysis and optimization recommendations
for NLM simulations based on performance monitoring data.
"""

import time
from typing import Dict, List, Any, Optional
import numpy as np

from .enhanced_api import NLMError

class EfficiencyAnalyzer:
    """
    Comprehensive efficiency analysis for NLM simulations.
    
    Analyzes simulation performance to identify efficiency opportunities
    and provide actionable optimization recommendations.
    """
    
    def __init__(self, brain=None, performance_data: Optional[List[Dict[str, Any]]] = None):
        """Initialize efficiency analyzer with optional brain reference and performance data"""
        self.brain = brain
        self.performance_data = performance_data or []
        self.efficiency_metrics = {}
        self.optimization_recommendations = []
    
    def analyze_efficiency(self, brain=None, performance_data: Optional[List[Dict[str, Any]]] = None) -> Dict[str, Any]:
        """
        Perform comprehensive efficiency analysis.
        
        Args:
            brain: Brain instance for system configuration
            performance_data: Performance data to analyze (if None, uses brain metrics)
        
        Returns:
            Dictionary containing efficiency analysis results
        """
        if brain is not None:
            self.brain = brain
        
        if performance_data is not None:
            self.performance_data = performance_data
        
        if not self.performance_data and not self.brain:
            return {'error': 'No performance data or brain available for analysis'}
        
        # Collect performance data
        performance_stats = self._collect_performance_stats()
        
        # Analyze different efficiency dimensions
        neural_efficiency = self._analyze_neural_efficiency()
        simulation_efficiency = self._analyze_simulation_efficiency()
        resource_efficiency = self._analyze_resource_efficiency()
        
        # Calculate overall efficiency score
        overall_efficiency = self._calculate_overall_efficiency(
            neural_efficiency, simulation_efficiency, resource_efficiency
        )
        
        # Generate optimization recommendations
        self.optimization_recommendations = self._generate_optimization_recommendations(
            performance_stats, neural_efficiency, simulation_efficiency, resource_efficiency
        )
        
        return {
            'summary': {
                'overall_efficiency_score': overall_efficiency,
                'efficiency_rating': self._get_efficiency_rating(overall_efficiency),
                'optimization_potential': self._calculate_optimization_potential(performance_stats),
                'recommendations_count': len(self.optimization_recommendations),
            },
            'neural_efficiency': neural_efficiency,
            'simulation_efficiency': simulation_efficiency,
            'resource_efficiency': resource_efficiency,
            'optimization_recommendations': self.optimization_recommendations,
        }
    
    def _collect_performance_stats(self) -> Dict[str, Any]:
        """Collect performance statistics from data or brain"""
        if self.performance_data:
            # Calculate statistics from provided data
            steps = [d.get('step', i) for i, d in enumerate(self.performance_data)]
            times = [d.get('time', 0.0) for d in self.performance_data]
            spikes = [d.get('spikes', 0) for d in self.performance_data]
            neurons = [d.get('neurons', 0) for d in self.performance_data]
            synapses = [d.get('synapses', 0) for d in self.performance_data]
            memory = [d.get('memory_mb', 0.0) for d in self.performance_data]
            cpu = [d.get('cpu_percent', 0.0) for d in self.performance_data]
            
            return {
                'steps': steps,
                'times': times,
                'spikes': spikes,
                'neurons': neurons,
                'synapses': synapses,
                'memory_mb': memory,
                'cpu_percent': cpu,
                'avg_time_per_step': np.mean(times) if times else 0.0,
                'avg_spikes_per_step': np.mean(spikes) if spikes else 0.0,
                'avg_memory_per_step': np.mean(memory) if memory else 0.0,
                'avg_cpu_per_step': np.mean(cpu) if cpu else 0.0,
            }
        elif self.brain:
            # Use brain metrics
            return {
                'steps': [i for i in range(100)],  # Example data
                'times': [0.1] * 100,  # Example data
                'spikes': [self.brain.getFiringNeuronCount()] * 100,
                'neurons': [self.brain.getTotalNeuronCount()] * 100,
                'synapses': [self.brain.getTotalSynapseCount()] * 100,
                'memory_mb': [0.0] * 100,  # Would need psutil to get real data
                'cpu_percent': [0.0] * 100,  # Would need psutil to get real data
            }
        else:
            return {}
    
    def _analyze_neural_efficiency(self) -> Dict[str, Any]:
        """Analyze neural computation efficiency"""
        stats = self._collect_performance_stats()
        
        if not stats:
            return {}
        
        spikes = stats.get('spikes', [])
        neurons = stats.get('neurons', [])
        synapses = stats.get('synapses', [])
        
        # Calculate neural efficiency metrics
        avg_spikes_per_neuron = np.mean([s/n if n > 0 else 0 for s, n in zip(spikes, neurons)])
        avg_spikes_per_synapse = np.mean([s/syn if syn > 0 else 0 for s, syn in zip(spikes, synapses)])
        spike_efficiency = avg_spikes_per_neuron
        
        # Calculate neural dynamics efficiency
        spike_variance = np.var(spikes) if len(spikes) > 1 else 0.0
        spike_coefficient_of_variation = np.sqrt(spike_variance) / (avg_spikes_per_neuron + 1e-6)
        
        return {
            'spike_efficiency': spike_efficiency,
            'avg_spikes_per_neuron': avg_spikes_per_neuron,
            'avg_spikes_per_synapse': avg_spikes_per_synapse,
            'spike_coefficient_of_variation': spike_coefficient_of_variation,
            'efficiency_rating': self._get_neural_efficiency_rating(spike_efficiency),
        }
    
    def _analyze_simulation_efficiency(self) -> Dict[str, Any]:
        """Analyze simulation performance efficiency"""
        stats = self._collect_performance_stats()
        
        if not stats:
            return {}
        
        times = stats.get('times', [])
        
        # Calculate simulation efficiency metrics
        avg_time_per_step = stats.get('avg_time_per_step', 0.0)
        steps_per_second = 1.0 / avg_time_per_step if avg_time_per_step > 0 else 0.0
        
        # Calculate time series efficiency
        time_std = np.std(times) if len(times) > 1 else 0.0
        time_coefficient_of_variation = time_std / (avg_time_per_step + 1e-6)
        
        # Calculate computational efficiency
        computational_efficiency = 1.0 / (avg_time_per_step * 1000)  # steps per millisecond
        
        return {
            'steps_per_second': steps_per_second,
            'avg_time_per_step_ms': avg_time_per_step * 1000,
            'time_coefficient_of_variation': time_coefficient_of_variation,
            'computational_efficiency': computational_efficiency,
            'efficiency_rating': self._get_simulation_efficiency_rating(steps_per_second),
        }
    
    def _analyze_resource_efficiency(self) -> Dict[str, Any]:
        """Analyze resource efficiency"""
        stats = self._collect_performance_stats()
        
        if not stats:
            return {}
        
        memory = stats.get('memory_mb', [])
        cpu = stats.get('cpu_percent', [])
        
        # Calculate resource efficiency metrics
        avg_memory_per_step = stats.get('avg_memory_per_step', 0.0)
        avg_cpu_per_step = stats.get('avg_cpu_per_step', 0.0)
        
        # Calculate resource efficiency scores
        memory_efficiency = 1.0 / (avg_memory_per_step + 1e-6)
        cpu_efficiency = 1.0 / (avg_cpu_per_step + 1e-6)
        
        # Calculate overall resource efficiency
        overall_resource_efficiency = (memory_efficiency + cpu_efficiency) / 2
        
        # Calculate resource utilization efficiency
        avg_memory_usage = np.mean(memory) if memory else 0.0
        avg_cpu_usage = np.mean(cpu) if cpu else 0.0
        resource_utilization_efficiency = 1.0 - ((avg_memory_usage + avg_cpu_usage) / 2) / 100.0
        
        return {
            'memory_efficiency': memory_efficiency,
            'cpu_efficiency': cpu_efficiency,
            'overall_resource_efficiency': overall_resource_efficiency,
            'resource_utilization_efficiency': resource_utilization_efficiency,
            'avg_memory_per_step_mb': avg_memory_per_step,
            'avg_cpu_per_step_percent': avg_cpu_per_step,
            'efficiency_rating': self._get_resource_efficiency_rating(overall_resource_efficiency),
        }
    
    def _calculate_overall_efficiency(self, neural: Dict[str, Any], simulation: Dict[str, Any], resource: Dict[str, Any]) -> float:
        """Calculate overall efficiency score from component efficiencies"""
        neural_score = self._efficiency_to_score(neural.get('efficiency_rating', 'Low'))
        simulation_score = self._efficiency_to_score(simulation.get('efficiency_rating', 'Low'))
        resource_score = self._efficiency_to_score(resource.get('efficiency_rating', 'Low'))
        
        # Weighted average (neural efficiency is most important)
        overall_score = (neural_score * 0.4 + simulation_score * 0.3 + resource_score * 0.3)
        
        return overall_score
    
    def _efficiency_to_score(self, rating: str) -> float:
        """Convert efficiency rating to numerical score"""
        rating_scores = {
            'Excellent': 4.0,
            'High': 3.0,
            'Good': 2.5,
            'Moderate': 2.0,
            'Acceptable': 1.5,
            'Low': 1.0,
            'Needs Optimization': 0.5,
        }
        return rating_scores.get(rating, 1.0)
    
    def _get_efficiency_rating(self, score: float) -> str:
        """Get efficiency rating from score"""
        if score >= 3.5:
            return "Excellent"
        elif score >= 2.5:
            return "High"
        elif score >= 1.5:
            return "Good"
        elif score >= 1.0:
            return "Moderate"
        elif score >= 0.5:
            return "Acceptable"
        else:
            return "Needs Optimization"
    
    def _get_neural_efficiency_rating(self, efficiency: float) -> str:
        """Get neural efficiency rating"""
        if efficiency > 10:
            return "Excellent"
        elif efficiency > 5:
            return "High"
        elif efficiency > 2:
            return "Good"
        elif efficiency > 1:
            return "Moderate"
        else:
            return "Low"
    
    def _get_simulation_efficiency_rating(self, steps_per_second: float) -> str:
        """Get simulation efficiency rating"""
        if steps_per_second > 1000:
            return "Excellent"
        elif steps_per_second > 500:
            return "High"
        elif steps_per_second > 100:
            return "Good"
        elif steps_per_second > 10:
            return "Moderate"
        else:
            return "Low"
    
    def _get_resource_efficiency_rating(self, efficiency: float) -> str:
        """Get resource efficiency rating"""
        if efficiency > 0.8:
            return "Excellent"
        elif efficiency > 0.6:
            return "High"
        elif efficiency > 0.4:
            return "Good"
        elif efficiency > 0.2:
            return "Moderate"
        else:
            return "Low"
    
    def _calculate_optimization_potential(self, stats: Dict[str, Any]) -> Dict[str, float]:
        """Calculate optimization potential for different areas"""
        optimization_potential = {}
        
        if stats:
            steps_per_second = 1.0 / stats.get('avg_time_per_step', 1e-6)
            
            # Simulation optimization potential
            if steps_per_second < 100:
                optimization_potential['simulation_speed'] = (100 - steps_per_second) / 100
            else:
                optimization_potential['simulation_speed'] = 0.0
            
            # Memory optimization potential
            avg_memory = stats.get('avg_memory_per_step', 0.0)
            if avg_memory > 10:  # More than 10MB per step
                optimization_potential['memory_usage'] = min(avg_memory / 100, 1.0)
            else:
                optimization_potential['memory_usage'] = 0.0
            
            # CPU optimization potential
            avg_cpu = stats.get('avg_cpu_per_step', 0.0)
            if avg_cpu > 50:  # More than 50% CPU usage
                optimization_potential['cpu_utilization'] = (avg_cpu - 50) / 100
            else:
                optimization_potential['cpu_utilization'] = 0.0
        
        return optimization_potential
    
    def _generate_optimization_recommendations(self, stats: Dict[str, Any], neural: Dict[str, Any], 
                                               simulation: Dict[str, Any], resource: Dict[str, Any]) -> List[str]:
        """Generate optimization recommendations based on efficiency analysis"""
        recommendations = []
        
        # Neural optimization recommendations
        spike_efficiency = neural.get('spike_efficiency', 0.0)
        if spike_efficiency < 2:
            recommendations.append(
                f"Low spike efficiency ({spike_efficiency:.2f} spikes/neuron). "
                "Consider adjusting neuron parameters or synaptic connections."
            )
        
        # Simulation optimization recommendations
        steps_per_second = simulation.get('steps_per_second', 0.0)
        if steps_per_second < 100:
            recommendations.append(
                f"Low simulation speed ({steps_per_second:.2f} SPS). "
                "Consider optimizing neural computations or enabling parallel processing."
            )
        
        # Resource optimization recommendations
        memory_efficiency = resource.get('memory_efficiency', 0.0)
        if memory_efficiency < 0.5:
            recommendations.append(
                f"Low memory efficiency ({memory_efficiency:.2f}). "
                "Consider implementing memory pools or reducing object overhead."
            )
        
        cpu_efficiency = resource.get('cpu_efficiency', 0.0)
        if cpu_efficiency < 0.5:
            recommendations.append(
                f"Low CPU efficiency ({cpu_efficiency:.2f}). "
                "Consider optimizing algorithms or reducing computational complexity."
            )
        
        # Overall efficiency recommendations
        overall_efficiency = self._calculate_overall_efficiency(neural, simulation, resource)
        if overall_efficiency < 1.5:
            recommendations.append(
                "Overall efficiency is low. Consider comprehensive optimization of neural dynamics, "
                "simulation speed, and resource utilization."
            )
        
        if not recommendations:
            recommendations.append("Efficiency is good. Consider fine-tuning for specific workloads or minor optimizations.")
        
        return recommendations
    
    def export_efficiency_analysis(self, filepath: str):
        """Export efficiency analysis to file"""
        try:
            analysis = self.analyze_efficiency()
            
            with open(filepath, 'w') as f:
                json.dump(analysis, f, indent=2, default=str)
            
            print(f"Efficiency analysis exported to {filepath}")
            
        except Exception as e:
            raise NLMError(f"Error exporting efficiency analysis: {e}")
