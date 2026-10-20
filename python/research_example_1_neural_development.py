#!/usr/bin/env python3
"""
NLM Research Examples - Neural Development Studies

This module demonstrates research on neural development stages, synaptogenesis, and pruning
in the Neural Learning Machine (NLM) framework.
"""

import time
import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple, Any
from dataclasses import dataclass
from abc import ABC, abstractmethod

from nlm_visualization_tools import (
    VisualizationInterface,
    DebugInterface,
    VisualizationMode,
    DebugMode,
    NeuronActivityData,
    TimeSeriesDataPoint,
    DebugInfo,
    MonitoringHook
)

@dataclass
class DevelopmentMetrics:
    """Metrics for neural development studies"""
    stage: str
    total_neurons: int
    total_synapses: int
    synaptic_density: float
    average_connection_strength: float
    pruning_events: int
    synaptogenesis_events: int
    connectivity_over_time: List[float]
    development_time: float

class NeuralDevelopmentResearcher:
    """Researcher conducting neural development studies"""
    
    def __init__(self, neuron_count: int = 1000, synapse_count: int = 5000):
        self.neuron_count = neuron_count
        self.synapse_count = synapse_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Development tracking
        self.development_stages = []
        self.synapse_creation_rate = 0.0
        self.pruning_rate = 0.0
        self.connectivity_history = []
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for development research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.ANIMATED)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_synaptogenesis(self, duration: float, connection_probability: float = 0.1) -> DevelopmentMetrics:
        """Simulate neural development with synaptogenesis (formation of new synapses)"""
        print(f"\n=== SYNAPTOGENESIS STUDY ===")
        print(f"Simulating synapse formation over {duration} seconds...")
        print(f"Initial neurons: {self.neuron_count}, Initial synapses: {self.synapse_count}")
        print(f"Connection probability: {connection_probability * 100}%")
        
        start_time = time.time()
        steps = int(duration * 30)  # Assuming 30 Hz update rate
        current_synapses = self.synapse_count
        peak_synapses = current_synapses
        
        # Track development metrics
        connectivity_over_time = []
        synaptogenesis_events = 0
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate synaptogenesis events
            if np.random.random() < connection_probability * (1.0 / 30.0):
                current_synapses += 1
                synaptogenesis_events += 1
            
            # Update visualization
            self.visualization.add_time_series_metric("total_neurons", self.neuron_count)
            self.visualization.add_time_series_metric("total_synapses", current_synapses)
            self.visualization.add_time_series_metric("synaptogenesis_events", synaptogenesis_events)
            
            # Track connectivity
            connectivity = current_synapses / max(1, self.neuron_count)
            connectivity_over_time.append(connectivity)
            peak_synapses = max(peak_synapses, current_synapses)
            
            if step % 50 == 0:
                self.visualization.add_debug_info(DebugInfo(
                    component_name="Synaptogenesis",
                    current_state="Active",
                    warnings=[f"Synapses created: {synaptogenesis_events}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate development metrics
        development_time = time.time() - start_time
        final_connectivity = current_synapses / max(1, self.neuron_count)
        
        metrics = DevelopmentMetrics(
            stage="Synaptogenesis",
            total_neurons=self.neuron_count,
            total_synapses=current_synapses,
            synaptic_density=current_synapses / max(1, self.neuron_count),
            average_connection_strength=0.5 + np.random.random() * 0.5,
            pruning_events=0,
            synaptogenesis_events=synaptogenesis_events,
            connectivity_over_time=connectivity_over_time,
            development_time=development_time
        )
        
        self.development_stages.append(metrics)
        print(f"✓ Synaptogenesis complete: {synaptogenesis_events} new synapses formed")
        print(f"  Final synaptic density: {final_connectivity:.4f}")
        print(f"  Peak synapses reached: {peak_synapses}")
        
        return metrics
    
    def simulate_pruning(self, duration: float, pruning_rate: float = 0.05) -> DevelopmentMetrics:
        """Simulate neural development with pruning (elimination of weak synapses)"""
        print(f"\n=== PRUNING STUDY ===")
        print(f"Simulating synapse pruning over {duration} seconds...")
        print(f"Pruning rate: {pruning_rate * 100}%")
        
        start_time = time.time()
        steps = int(duration * 30)
        current_synapses = self.synapse_count
        total_pruned = 0
        
        # Track pruning metrics
        connectivity_over_time = []
        pruning_events = 0
        strength_threshold = 0.3
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate pruning events
            if np.random.random() < pruning_rate * (1.0 / 30.0):
                if current_synapses > 0 and np.random.random() < strength_threshold:
                    current_synapses -= 1
                    total_pruned += 1
                    pruning_events += 1
            
            # Update visualization
            self.visualization.add_time_series_metric("total_neurons", self.neuron_count)
            self.visualization.add_time_series_metric("total_synapses", current_synapses)
            self.visualization.add_time_series_metric("pruning_events", pruning_events)
            
            # Track connectivity
            connectivity = current_synapses / max(1, self.neuron_count)
            connectivity_over_time.append(connectivity)
            
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="Pruning",
                    current_state="Active",
                    warnings=[f"Synapses pruned: {pruning_events}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate development metrics
        development_time = time.time() - start_time
        final_connectivity = current_synapses / max(1, self.neuron_count)
        
        metrics = DevelopmentMetrics(
            stage="Pruning",
            total_neurons=self.neuron_count,
            total_synapses=current_synapses,
            synaptic_density=current_synapses / max(1, self.neuron_count),
            average_connection_strength=0.4 + np.random.random() * 0.4,
            pruning_events=pruning_events,
            synaptogenesis_events=0,
            connectivity_over_time=connectivity_over_time,
            development_time=development_time
        )
        
        self.development_stages.append(metrics)
        print(f"✓ Pruning complete: {pruning_events} synapses pruned")
        print(f"  Final synaptic density: {final_connectivity:.4f}")
        print(f"  Total pruned: {total_pruned}")
        
        return metrics
    
    def simulate_maturation(self, duration: float, maturation_rate: float = 0.02) -> DevelopmentMetrics:
        """Simulate neural maturation and stabilization after development"""
        print(f"\n=== MATURATION STUDY ===")
        print(f"Simulating neural maturation over {duration} seconds...")
        print(f"Maturation rate: {maturation_rate * 100}%")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Track maturation metrics
        connectivity_over_time = []
        stabilization_events = 0
        current_connectivity = 0.5  # Initial post-synaptogenesis connectivity
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate maturation events
            if np.random.random() < maturation_rate * (1.0 / 30.0):
                # Connectivity changes during maturation (small fluctuations)
                current_connectivity += np.random.normal(0, 0.01)
                current_connectivity = np.clip(current_connectivity, 0.1, 2.0)
                stabilization_events += 1
            
            # Update visualization
            current_synapses = int(self.neuron_count * current_connectivity)
            self.visualization.add_time_series_metric("total_neurons", self.neuron_count)
            self.visualization.add_time_series_metric("total_synapses", current_synapses)
            self.visualization.add_time_series_metric("maturation_events", stabilization_events)
            
            # Track connectivity
            connectivity_over_time.append(current_connectivity)
            
            time.sleep(1.0 / 30.0)
        
        # Calculate development metrics
        development_time = time.time() - start_time
        final_connectivity = connectivity_over_time[-1] if connectivity_over_time else current_connectivity
        
        metrics = DevelopmentMetrics(
            stage="Maturation",
            total_neurons=self.neuron_count,
            total_synapses=int(self.neuron_count * final_connectivity),
            synaptic_density=final_connectivity,
            average_connection_strength=0.6 + np.random.random() * 0.3,
            pruning_events=0,
            synaptogenesis_events=0,
            connectivity_over_time=connectivity_over_time,
            development_time=development_time
        )
        
        self.development_stages.append(metrics)
        print(f"✓ Maturation complete: {stabilization_events} stabilization events")
        print(f"  Final connectivity: {final_connectivity:.4f}")
        
        return metrics
    
    def analyze_development_data(self) -> Dict[str, Any]:
        """Analyze the collected development data"""
        print(f"\n=== DEVELOPMENT ANALYSIS ===")
        
        if not self.development_stages:
            print("No development data collected!")
            return {}
        
        analysis = {
            "total_stages": len(self.development_stages),
            "total_neuron_count": sum(stage.total_neurons for stage in self.development_stages),
            "max_synapses": max(stage.total_synapses for stage in self.development_stages),
            "min_synapses": min(stage.total_synapses for stage in self.development_stages),
            "average_connectivity": np.mean([stage.synaptic_density for stage in self.development_stages]),
            "connectivity_variance": np.var([stage.synaptic_density for stage in self.development_stages]),
            "total_synaptogenesis_events": sum(stage.synaptogenesis_events for stage in self.development_stages),
            "total_pruning_events": sum(stage.pruning_events for stage in self.development_stages),
        }
        
        # Calculate connectivity dynamics
        all_connectivity = []
        for stage in self.development_stages:
            all_connectivity.extend(stage.connectivity_over_time)
        
        if all_connectivity:
            analysis.update({
                "connectivity_range": max(all_connectivity) - min(all_connectivity),
                "connectivity_stability": 1.0 - (np.std(all_connectivity) / max(1, np.mean(all_connectivity))),
                "peak_connectivity_time": np.argmax(all_connectivity) / 30.0,  # Convert to seconds
            })
        
        print(f"✓ Development analysis complete")
        print(f"  Stages analyzed: {analysis['total_stages']}")
        print(f"  Max synapses: {analysis['max_synapses']}")
        print(f"  Min synapses: {analysis['min_synapses']}")
        print(f"  Average connectivity: {analysis['average_connectivity']:.4f}")
        print(f"  Synaptogenesis events: {analysis['total_synaptogenesis_events']}")
        print(f"  Pruning events: {analysis['total_pruning_events']}")
        
        return analysis
    
    def export_development_results(self, filename: str):
        """Export development research results"""
        print(f"\n=== EXPORTING DEVELOPMENT RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Neural Development Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
                "initial_synapse_count": self.synapse_count,
            },
            "development_stages": [
                {
                    "stage": stage.stage,
                    "total_neurons": stage.total_neurons,
                    "total_synapses": stage.total_synapses,
                    "synaptic_density": stage.synaptic_density,
                    "average_connection_strength": stage.average_connection_strength,
                    "pruning_events": stage.pruning_events,
                    "synaptogenesis_events": stage.synaptogenesis_events,
                    "development_time": stage.development_time,
                }
                for stage in self.development_stages
            ],
            "analysis": self.analyze_development_data(),
            "visualization_data": {
                "time_series": [
                    {
                        "metric_name": point.metric_name,
                        "value": point.value,
                        "timestamp": point.timestamp,
                    }
                    for point in self.visualization._time_series_data
                ]
            }
        }
        
        # Save to file
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"✓ Development results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_neural_development_research(duration_per_stage: float = 15.0):
    """Run a complete neural development research study"""
    print("=== NLM NEURAL DEVELOPMENT RESEARCH ===")
    
    # Initialize researcher
    researcher = NeuralDevelopmentResearcher(
        neuron_count=1000,
        synapse_count=5000
    )
    
    try:
        # Run development stages
        print("\n" + "="*60)
        print("STARTING NEURAL DEVELOPMENT RESEARCH")
        print("="*60)
        
        # Synaptogenesis stage
        syn_metrics = researcher.simulate_synaptogenesis(
            duration=duration_per_stage,
            connection_probability=0.1
        )
        
        # Pruning stage  
        prune_metrics = researcher.simulate_pruning(
            duration=duration_per_stage,
            pruning_rate=0.05
        )
        
        # Maturation stage
        matur_metrics = researcher.simulate_maturation(
            duration=duration_per_stage,
            maturation_rate=0.02
        )
        
        # Analyze and export results
        analysis = researcher.analyze_development_data()
        
        print(f"\n" + "="*60)
        print("NEURAL DEVELOPMENT RESEARCH COMPLETE")
        print("="*60)
        print(f"Total development time: {sum(stage.development_time for stage in researcher.development_stages):.2f}s")
        print(f"Final synapse count: {syn_metrics.total_synapses}")
        print(f"Net synapse change: {syn_metrics.total_synapses - researcher.synapse_count}")
        
        # Export results
        researcher.export_development_results("neural_development_research.json")
        
        return researcher.development_stages, analysis
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    stages, analysis = run_neural_development_research()
    print(f"\nResearch completed successfully with {len(stages)} development stages recorded.")
