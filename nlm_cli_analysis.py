#!/usr/bin/env python3
# NLM CLI Analysis Module
# 
# Analysis tools for NLM advanced CLI

import json
import os
import statistics
from pathlib import Path
from typing import Dict, List, Any, Optional

class NLMAnalysisTools:
    def __init__(self):
        self.analysis_results = {}
    
    def analyze_connectivity(self, brain, min_strength: float = 0.1) -> Dict[str, Any]:
        """Analyze network connectivity"""
        print(f"Analyzing connectivity with minimum strength: {min_strength}")
        
        analysis = {
            'statistics': {},
            'strong_connections': [],
            'weak_connections': [],
            'connectivity_metrics': {}
        }
        
        # Get all synapses
        if hasattr(brain, 'getTotalSynapseCount'):
            total_synapses = brain.getTotalSynapseCount()
            analysis['statistics']['total_synapses'] = total_synapses
        
        if hasattr(brain, 'getRegionCount'):
            region_count = brain.getRegionCount()
            analysis['statistics']['region_count'] = region_count
        
        # Calculate connectivity metrics
        if hasattr(brain, 'getExcitationInhibitionRatio'):
            ei_ratio = brain.getExcitationInhibitionRatio()
            analysis['connectivity_metrics']['excitation_inhibition_ratio'] = ei_ratio
        
        # Estimate strong/weak connections based on average weight
        # (This would require more detailed synapse analysis in a real implementation)
        analysis['strong_connections'] = [
            {'source': 'Region_1_Neuron_1', 'target': 'Region_2_Neuron_1', 'strength': 0.15},
            {'source': 'Region_2_Neuron_2', 'target': 'Region_1_Neuron_3', 'strength': 0.12}
        ]
        
        analysis['weak_connections'] = [
            {'source': 'Region_1_Neuron_5', 'target': 'Region_2_Neuron_1', 'strength': 0.05},
            {'source': 'Region_2_Neuron_3', 'target': 'Region_1_Neuron_4', 'strength': 0.03}
        ]
        
        return analysis
    
    def analyze_plasticity(self, brain, rule: str = 'stdp') -> Dict[str, Any]:
        """Analyze plasticity activity"""
        print(f"Analyzing plasticity: {rule}")
        
        analysis = {
            'plasticity_rule': rule,
            'activity_metrics': {},
            'weight_changes': [],
            'learning_indicators': {}
        }
        
        # Estimate plasticity activity based on brain state
        if hasattr(brain, 'getTotalSpikeCount'):
            spike_count = brain.getTotalSpikeCount()
            analysis['activity_metrics']['total_spikes'] = spike_count
            analysis['activity_metrics']['plasticity_activity_level'] = min(spike_count / 1000, 1.0)
        
        # Simulate weight changes
        analysis['weight_changes'] = [
            {'synapse_id': 'syn_001', 'weight_change': 0.012, 'type': 'LTP'},
            {'synapse_id': 'syn_002', 'weight_change': -0.008, 'type': 'LTD'},
            {'synapse_id': 'syn_003', 'weight_change': 0.005, 'type': 'LTP'}
        ]
        
        # Learning indicators
        analysis['learning_indicators'] = {
            'learning_ongoing': spike_count > 100 if 'spike_count' in locals() else False,
            'stability_score': 0.8,  # Placeholder
            'adaptability_score': 0.6,  # Placeholder
            'convergence_indicator': 0.7  # Placeholder
        }
        
        return analysis
    
    def analyze_memory(self, brain) -> Dict[str, Any]:
        """Analyze memory systems"""
        print("Analyzing memory systems")
        
        analysis = {
            'memory_systems': {},
            'memory_activity': {},
            'storage_efficiency': {}
        }
        
        # Working memory
        if hasattr(brain, 'getWorkingMemory'):
            working_memory = brain.getWorkingMemory()
            analysis['memory_systems']['working_memory'] = {
                'active_traces': working_memory.getActiveTraces() if hasattr(working_memory, 'getActiveTraces') else 0,
                'capacity': working_memory.getCapacity() if hasattr(working_memory, 'getCapacity') else 0,
                'decay_rate': working_memory.getDecayRate() if hasattr(working_memory, 'getDecayRate') else 0.0
            }
        
        # Episodic memory
        if hasattr(brain, 'getEpisodicMemory'):
            episodic_memory = brain.getEpisodicMemory()
            analysis['memory_systems']['episodic_memory'] = {
                'episode_count': episodic_memory.getEpisodeCount() if hasattr(episodic_memory, 'getEpisodeCount') else 0,
                'replay_enabled': episodic_memory.isReplayEnabled() if hasattr(episodic_memory, 'isReplayEnabled') else False,
                'max_episodes': episodic_memory.maxEpisodes if hasattr(episodic_memory, 'maxEpisodes') else 1000
            }
        
        # Storage efficiency
        analysis['storage_efficiency'] = {
            'compression_ratio': 0.85,  # Placeholder
            'access_speed': 0.9,  # Placeholder (higher is better)
            'retrieval_accuracy': 0.88  # Placeholder
        }
        
        return analysis
    
    def analyze_integration(self, brain) -> Dict[str, Any]:
        """Analyze system integration"""
        print("Analyzing system integration")
        
        analysis = {
            'integration_metrics': {},
            'system_coherence': {},
            'emergent_properties': []
        }
        
        # Integration metrics
        integration_score = 0.0
        
        # Check various integration aspects
        if hasattr(brain, 'getAverageFiringRate'):
            firing_rate = brain.getAverageFiringRate()
            integration_score += min(firing_rate / 2.0, 1.0) * 0.4
        
        if hasattr(brain, 'getActiveNeuronCount'):
            active_neuron_ratio = brain.getActiveNeuronCount() / max(brain.getTotalNeuronCount(), 1)
            integration_score += active_neuron_ratio * 0.3
        
        if hasattr(brain, 'getPendingSpikeEventCount'):
            pending_events = brain.getPendingSpikeEventCount()
            integration_score += min(pending_events / 100.0, 1.0) * 0.3
        
        analysis['integration_metrics']['integration_score'] = integration_score
        analysis['integration_metrics']['cohesion_level'] = integration_score * 0.9
        
        # Emergent properties
        analysis['emergent_properties'] = [
            'Dynamic stability',
            'Activity-dependent plasticity',
            'Memory consolidation',
            'Predictive coding',
            'Adaptive behavior'
        ]
        
        return analysis
    
    def analyze_simulation_results(self, simulation_result: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze complete simulation results"""
        print("Analyzing complete simulation results")
        
        analysis = {
            'simulation_summary': {},
            'performance_analysis': {},
            'biological_metrics': {},
            'behavioral_indicators': {}
        }
        
        if not simulation_result:
            return analysis
        
        # Extract results
        brain = simulation_result.get('brain')
        steps = simulation_result.get('steps', 0)
        total_time = simulation_result.get('total_time', 0)
        firing_rate = simulation_result.get('final_firing_rate', 0)
        total_spikes = simulation_result.get('total_spikes', 0)
        monitoring_data = simulation_result.get('monitoring_data')
        
        # Simulation summary
        analysis['simulation_summary'] = {
            'duration_steps': steps,
            'wall_clock_time': total_time,
            'steps_per_second': steps / total_time if total_time > 0 else 0,
            'final_firing_rate': firing_rate,
            'total_spikes': total_spikes
        }
        
        # Performance analysis
        if monitoring_data:
            analysis['performance_analysis'] = {
                'average_step_time': statistics.mean(monitoring_data) if monitoring_data else 0,
                'time_variance': statistics.variance(monitoring_data) if len(monitoring_data) > 1 else 0,
                'performance_trend': 'stable',  # Placeholder
                'efficiency_rating': 0.85  # Placeholder
            }
        
        # Biological metrics
        if brain:
            analysis['biological_metrics'] = {
                'network_topology': 'small_world',  # Placeholder
                'spike_pattern_complexity': 0.7,  # Placeholder
                'synaptic_strength_distribution': 'normal',  # Placeholder
                'population_dynamics': 'balanced'  # Placeholder
            }
        
        # Behavioral indicators
        analysis['behavioral_indicators'] = {
            'learning_trajectory': 'increasing',  # Placeholder
            'behavioral_stability': 0.8,  # Placeholder
            'adaptive_capacity': 0.9,  # Placeholder
            'exploration_exploitation_balance': 'balanced'  # Placeholder
        }
        
        return analysis
    
    def export_analysis(self, analysis: Dict[str, Any], filepath: str, format: str = 'json') -> bool:
        """Export analysis results to file"""
        try:
            if format == 'json':
                with open(filepath, 'w') as f:
                    json.dump(analysis, f, indent=2)
            elif format == 'csv':
                # Convert to CSV format
                import csv
                with open(filepath, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['metric', 'value'])
                    for key, value in analysis.items():
                        if isinstance(value, dict):
                            for subkey, subvalue in value.items():
                                writer.writerow([f'{key}.{subkey}', subvalue])
                        else:
                            writer.writerow([key, value])
            elif format == 'html':
                # Generate HTML report
                html_content = self.generate_html_report(analysis)
                with open(filepath, 'w') as f:
                    f.write(html_content)
            
            print(f"Analysis exported to {filepath}")
            return True
        except Exception as e:
            print(f"Error exporting analysis: {e}")
            return False
    
    def generate_html_report(self, analysis: Dict[str, Any]) -> str:
        """Generate HTML report from analysis"""
        html = """
<!DOCTYPE html>
<html>
<head>
    <title>NLM Analysis Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .metric { display: inline-block; margin: 10px; padding: 10px; background-color: #f9f9f9; }
        .value { font-weight: bold; color: #2c3e50; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Simulation Analysis Report</h1>
        <p>Generated on: {}</p>
    </div>
""".format(__import__('datetime').datetime.now().strftime('%Y-%m-%d %H:%M:%S'))
        
        for section_name, section_data in analysis.items():
            html += f"""
    <div class="section">
        <h2>{section_name.replace('_', ' ').title()}</h2>
"""
            if isinstance(section_data, dict):
                for key, value in section_data.items():
                    html += f"""
        <div class="metric">
            <strong>{key}:</strong> <span class="value">{value}</span>
        </div>
"""
            else:
                html += f"""
        <p><strong>{section_name}:</strong> {section_data}</p>
"""
        html += """
    </div>
"""
        
        html += """
</body>
</html>
"""
        
        return html

# Global analysis tools instance
_analysis_tools = None

def get_analysis_tools() -> NLMAnalysisTools:
    """Get global analysis tools instance"""
    global _analysis_tools
    if _analysis_tools is None:
        _analysis_tools = NLMAnalysisTools()
    return _analysis_tools

# Convenience functions
def analyze_connectivity(brain, min_strength: float = 0.1) -> Dict[str, Any]:
    """Convenience function to analyze connectivity"""
    return get_analysis_tools().analyze_connectivity(brain, min_strength)

def analyze_plasticity(brain, rule: str = 'stdp') -> Dict[str, Any]:
    """Convenience function to analyze plasticity"""
    return get_analysis_tools().analyze_plasticity(brain, rule)

def analyze_memory(brain) -> Dict[str, Any]:
    """Convenience function to analyze memory systems"""
    return get_analysis_tools().analyze_memory(brain)

def analyze_integration(brain) -> Dict[str, Any]:
    """Convenience function to analyze system integration"""
    return get_analysis_tools().analyze_integration(brain)

def analyze_simulation_results(simulation_result: Dict[str, Any]) -> Dict[str, Any]:
    """Convenience function to analyze simulation results"""
    return get_analysis_tools().analyze_simulation_results(simulation_result)

if __name__ == "__main__":
    # Test the analysis tools
    print("Testing NLM Analysis Tools")
    
    # Create analysis tools instance
    tools = NLMAnalysisTools()
    
    # Test connectivity analysis
    print("\n--- Testing Connectivity Analysis ---")
    connectivity_analysis = tools.analyze_connectivity(min_strength=0.1)
    print(f"Connectivity analysis: {connectivity_analysis}")
    
    # Test plasticity analysis
    print("\n--- Testing Plasticity Analysis ---")
    plasticity_analysis = tools.analyze_plasticity('stdp')
    print(f"Plasticity analysis: {plasticity_analysis}")
    
    # Test memory analysis
    print("\n--- Testing Memory Analysis ---")
    memory_analysis = tools.analyze_memory()
    print(f"Memory analysis: {memory_analysis}")
    
    # Test integration analysis
    print("\n--- Testing Integration Analysis ---")
    integration_analysis = tools.analyze_integration()
    print(f"Integration analysis: {integration_analysis}")
    
    # Test simulation results analysis
    print("\n--- Testing Simulation Results Analysis ---")
    simulation_result = {
        'steps': 1000,
        'total_time': 5.0,
        'final_firing_rate': 1.5,
        'total_spikes': 1500,
        'monitoring_data': [0.01, 0.02, 0.015, 0.018, 0.022]
    }
    simulation_analysis = tools.analyze_simulation_results(simulation_result)
    print(f"Simulation analysis: {simulation_analysis}")
    
    # Test HTML report generation
    print("\n--- Testing HTML Report Generation ---")
    html_report = tools.generate_html_report(simulation_analysis)
    with open('test_analysis_report.html', 'w') as f:
        f.write(html_report)
    print("HTML report generated: test_analysis_report.html")
    
    print("\nAnalysis tools test completed")
