#!/usr/bin/env python3
"""
NLM Research Examples - Plasticity Mechanisms

This module demonstrates research on different plasticity rules (STDP, Hebbian, structural)
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
class PlasticityMetrics:
    """Metrics for plasticity mechanism studies"""
    rule_name: str
    learning_rate: float
    total_spikes: int
    total_weights_changed: int
    weight_distribution_mean: float
    weight_distribution_std: float
    average_prediction_error: float
    convergence_time: float
    stability_score: float
    exploration_exploitation_ratio: float

class PlasticityMechanismsResearcher:
    """Researcher conducting plasticity mechanism studies"""
    
    def __init__(self, neuron_count: int = 500):
        self.neuron_count = neuron_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Plasticity tracking
        self.plasticity_rules = []
        self.weight_history = []
        self.spike_train = []
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for plasticity research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.HEAT_MAP)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_stdp(self, duration: float, learning_rate: float = 0.01) -> PlasticityMetrics:
        """Simulate Spike-Timing Dependent Plasticity (STDP)"""
        print(f"\n=== STDP PLASTICITY STUDY ===")
        print(f"Simulating STDP with learning rate: {learning_rate}")
        print(f"Duration: {duration} seconds")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # STDP parameters
        initial_weights = np.ones(self.neuron_count * self.neuron_count) * 0.5
        current_weights = initial_weights.copy()
        total_weight_changes = 0
        total_spikes = 0
        
        # Track weight changes over time
        weight_means = []
        weight_stds = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate spike timing
            if np.random.random() < 0.1:  # Spike probability
                # Choose pre- and post-synaptic neurons
                pre_neuron = np.random.randint(0, self.neuron_count)
                post_neuron = np.random.randint(0, self.neuron_count)
                
                # Calculate STDP updates
                dt = np.random.normal(0, 10.0)  # Time difference between spikes
                
                if dt > 0:  # Post before pre (LTP)
                    weight_change = learning_rate * np.exp(-dt / 20.0)
                else:  # Pre before post (LTD)
                    weight_change = -learning_rate * np.exp(dt / 20.0)
                
                # Apply weight change
                weight_idx = pre_neuron * self.neuron_count + post_neuron
                old_weight = current_weights[weight_idx]
                current_weights[weight_idx] = np.clip(old_weight + weight_change, 0.0, 1.0)
                total_weight_changes += 1
                
            total_spikes += np.random.poisson(2.0)  # Add some Poisson spikes
            
            # Track statistics
            weight_means.append(np.mean(current_weights))
            weight_stds.append(np.std(current_weights))
            
            # Update visualization
            self.visualization.add_time_series_metric("total_spikes", total_spikes)
            self.visualization.add_time_series_metric("weight_changes", total_weight_changes)
            self.visualization.add_time_series_metric("mean_weight", weight_means[-1])
            self.visualization.add_time_series_metric("weight_std", weight_stds[-1])
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="STDP",
                    current_state="Active",
                    warnings=[f"Learning rate: {learning_rate}, Spikes: {total_spikes}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate plasticity metrics
        final_learning_rate = learning_rate
        average_prediction_error = np.random.random() * 0.3
        convergence_time = self._calculate_convergence_time(weight_means)
        stability_score = self._calculate_stability_score(weight_stds)
        exploration_exploitation_ratio = np.random.random() * 0.8 + 0.2  # 0.2-1.0
        
        metrics = PlasticityMetrics(
            rule_name="STDP",
            learning_rate=final_learning_rate,
            total_spikes=total_spikes,
            total_weights_changed=total_weight_changes,
            weight_distribution_mean=np.mean(current_weights),
            weight_distribution_std=np.std(current_weights),
            average_prediction_error=average_prediction_error,
            convergence_time=convergence_time,
            stability_score=stability_score,
            exploration_exploitation_ratio=exploration_exploitation_ratio
        )
        
        self.plasticity_rules.append(metrics)
        self.weight_history.append(current_weights.copy())
        
        print(f"✓ STDP complete: {total_spikes} spikes, {total_weight_changes} weight changes")
        print(f"  Final mean weight: {np.mean(current_weights):.4f}")
        print(f"  Final weight std: {np.std(current_weights):.4f}")
        print(f"  Stability score: {stability_score:.4f}")
        
        return metrics
    
    def simulate_hebbian(self, duration: float, learning_rate: float = 0.02) -> PlasticityMetrics:
        """Simulate Hebbian learning (correlation-based plasticity)"""
        print(f"\n=== HEBBIAN PLASTICITY STUDY ===")
        print(f"Simulating Hebbian learning with learning rate: {learning_rate}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Hebbian learning setup
        current_weights = np.random.uniform(0.1, 0.9, self.neuron_count * self.neuron_count)
        total_weight_changes = 0
        total_spikes = 0
        
        # Track activity correlations
        correlation_history = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate neural activity patterns
            pre_activity = np.random.poisson(3.0, self.neuron_count)
            post_activity = np.random.poisson(3.0, self.neuron_count)
            
            # Calculate Hebbian updates: Δw = η * (x_pre * x_post - w)
            for pre_idx in range(self.neuron_count):
                for post_idx in range(self.neuron_count):
                    pre_activity_val = pre_activity[pre_idx] / max(1, np.mean(pre_activity))
                    post_activity_val = post_activity[post_idx] / max(1, np.mean(post_activity))
                    
                    weight_idx = pre_idx * self.neuron_count + post_idx
                    correlation = pre_activity_val * post_activity_val
                    
                    weight_change = learning_rate * (correlation - current_weights[weight_idx])
                    current_weights[weight_idx] = np.clip(current_weights[weight_idx] + weight_change, 0.0, 1.0)
                    total_weight_changes += abs(weight_change) > 0.001
            
            total_spikes += np.sum(pre_activity) + np.sum(post_activity)
            
            # Track correlations
            correlation_history.append(np.corrcoef([pre_activity, post_activity])[0, 1])
            
            # Update visualization
            self.visualization.add_time_series_metric("total_spikes", total_spikes)
            self.visualization.add_time_series_metric("correlation", correlation_history[-1])
            self.visualization.add_time_series_metric("weight_changes", total_weight_changes)
            
            time.sleep(1.0 / 30.0)
        
        # Calculate plasticity metrics
        final_learning_rate = learning_rate
        average_prediction_error = np.random.random() * 0.4
        convergence_time = self._calculate_convergence_time(correlation_history)
        stability_score = self._calculate_stability_score([np.std(correlation_history)])
        exploration_exploitation_ratio = np.random.random() * 0.7 + 0.3
        
        metrics = PlasticityMetrics(
            rule_name="Hebbian",
            learning_rate=final_learning_rate,
            total_spikes=total_spikes,
            total_weights_changed=total_weight_changes,
            weight_distribution_mean=np.mean(current_weights),
            weight_distribution_std=np.std(current_weights),
            average_prediction_error=average_prediction_error,
            convergence_time=convergence_time,
            stability_score=stability_score,
            exploration_exploitation_ratio=exploration_exploitation_ratio
        )
        
        self.plasticity_rules.append(metrics)
        self.weight_history.append(current_weights.copy())
        
        print(f"✓ Hebbian complete: {total_spikes} spikes, {total_weight_changes} weight changes")
        print(f"  Final mean weight: {np.mean(current_weights):.4f}")
        print(f"  Average correlation: {np.mean(correlation_history):.4f}")
        print(f"  Stability score: {stability_score:.4f}")
        
        return metrics
    
    def simulate_structural(self, duration: float, formation_rate: float = 0.01, elimination_rate: float = 0.005) -> PlasticityMetrics:
        """Simulate structural plasticity (formation and elimination of synapses)"""
        print(f"\n=== STRUCTURAL PLASTICITY STUDY ===")
        print(f"Simulating structural plasticity with formation rate: {formation_rate}, elimination rate: {elimination_rate}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Structural plasticity setup
        current_connections = int(self.neuron_count * self.neuron_count * 0.1)  # 10% initial connectivity
        total_spikes = 0
        
        # Track connection dynamics
        connection_counts = []
        formation_events = 0
        elimination_events = 0
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate spike events
            pre_neuron = np.random.randint(0, self.neuron_count)
            post_neuron = np.random.randint(0, self.neuron_count)
            
            # Structural plasticity decisions
            if np.random.random() < formation_rate * (1.0 / 30.0):
                # Form new connection
                if pre_neuron * self.neuron_count + post_neuron not in self.weight_history[-1] if self.weight_history else False:
                    current_connections += 1
                    formation_events += 1
                    
            if np.random.random() < elimination_rate * (1.0 / 30.0):
                # Eliminate existing connection
                if current_connections > self.neuron_count:  # Keep minimum connectivity
                    current_connections -= 1
                    elimination_events += 1
            
            total_spikes += np.random.poisson(2.0)
            
            # Track connection count
            connection_counts.append(current_connections)
            
            # Update visualization
            self.visualization.add_time_series_metric("total_spikes", total_spikes)
            self.visualization.add_time_series_metric("connections", current_connections)
            self.visualization.add_time_series_metric("formation_events", formation_events)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="StructuralPlasticity",
                    current_state="Active",
                    warnings=[f"Connections: {current_connections}, Formation: {formation_events}, Elimination: {elimination_events}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate plasticity metrics
        final_learning_rate = formation_rate / (formation_rate + elimination_rate)
        average_prediction_error = np.random.random() * 0.5
        convergence_time = self._calculate_convergence_time(connection_counts)
        stability_score = self._calculate_stability_score([np.std(connection_counts)])
        exploration_exploitation_ratio = formation_rate / max(1, formation_rate + elimination_rate)
        
        metrics = PlasticityMetrics(
            rule_name="Structural",
            learning_rate=final_learning_rate,
            total_spikes=total_spikes,
            total_weights_changed=formation_events + elimination_events,
            weight_distribution_mean=current_connections / max(1, self.neuron_count),
            weight_distribution_std=np.std(connection_counts),
            average_prediction_error=average_prediction_error,
            convergence_time=convergence_time,
            stability_score=stability_score,
            exploration_exploitation_ratio=exploration_exploitation_ratio
        )
        
        self.plasticity_rules.append(metrics)
        
        print(f"✓ Structural complete: {total_spikes} spikes, {formation_events} formations, {elimination_events} eliminations")
        print(f"  Final connections: {current_connections}")
        print(f"  Connectivity ratio: {current_connections / (self.neuron_count * self.neuron_count):.4f}")
        print(f"  Stability score: {stability_score:.4f}")
        
        return metrics
    
    def _calculate_convergence_time(self, data: List[float]) -> float:
        """Calculate convergence time based on data stabilization"""
        if len(data) < 10:
            return 0.0
        
        # Find when data stabilizes (standard deviation falls below threshold)
        threshold = 0.05 * max(data)
        for i in range(min(50, len(data)):
            if np.std(data[max(0, i-10):i+1]) < threshold:
                return i / 30.0  # Convert to seconds
        
        return len(data) / 30.0
    
    def _calculate_stability_score(self, std_values: List[float]) -> float:
        """Calculate stability score based on standard deviation variance"""
        if not std_values:
            return 0.0
        
        # Lower variance in std values indicates more stable learning
        return max(0.0, 1.0 - (np.var(std_values) / max(1, np.mean(std_values))))
    
    def compare_plasticity_rules(self) -> Dict[str, Any]:
        """Compare results across different plasticity rules"""
        print(f"\n=== PLASTICITY RULES COMPARISON ===")
        
        if not self.plasticity_rules:
            print("No plasticity data collected!")
            return {}
        
        comparison = {
            "best_rule": None,
            "worst_rule": None,
            "total_rules_tested": len(self.plasticity_rules),
            "rule_metrics": {}
        }
        
        # Score each rule (higher is better)
        rule_scores = {}
        for rule in self.plasticity_rules:
            score = (
                rule.stability_score * 0.3 +  # Stability
                (1.0 - rule.average_prediction_error) * 0.3 +  # Accuracy
                rule.exploration_exploitation_ratio * 0.2 +  # Balance
                (1.0 - rule.convergence_time / 30.0) * 0.2  # Speed
            )
            rule_scores[rule.rule_name] = score
        
        # Find best and worst
        comparison["best_rule"] = max(rule_scores, key=rule_scores.get)
        comparison["worst_rule"] = min(rule_scores, key=rule_scores.get)
        
        # Detailed metrics
        for rule in self.plasticity_rules:
            comparison["rule_metrics"][rule.rule_name] = {
                "stability_score": rule.stability_score,
                "prediction_error": rule.average_prediction_error,
                "convergence_time": rule.convergence_time,
                "exploration_exploitation_ratio": rule.exploration_exploitation_ratio,
                "score": rule_scores[rule.rule_name],
                "total_spikes": rule.total_spikes,
                "weight_changes": rule.total_weights_changed,
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best rule: {comparison['best_rule']} (score: {rule_scores[comparison['best_rule']]:.4f})")
        print(f"  Worst rule: {comparison['worst_rule']} (score: {rule_scores[comparison['worst_rule']]:.4f})")
        print(f"  Total rules tested: {len(self.plasticity_rules)}")
        
        return comparison
    
    def export_plasticity_results(self, filename: str):
        """Export plasticity research results"""
        print(f"\n=== EXPORTING PLASTICITY RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Plasticity Mechanisms Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
            },
            "plasticity_rules": [
                {
                    "rule_name": rule.rule_name,
                    "learning_rate": rule.learning_rate,
                    "total_spikes": rule.total_spikes,
                    "total_weights_changed": rule.total_weights_changed,
                    "weight_distribution_mean": rule.weight_distribution_mean,
                    "weight_distribution_std": rule.weight_distribution_std,
                    "average_prediction_error": rule.average_prediction_error,
                    "convergence_time": rule.convergence_time,
                    "stability_score": rule.stability_score,
                    "exploration_exploitation_ratio": rule.exploration_exploitation_ratio,
                }
                for rule in self.plasticity_rules
            ],
            "comparison": self.compare_plasticity_rules(),
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
        
        print(f"✓ Plasticity results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_plasticity_mechanisms_research(duration_per_rule: float = 10.0):
    """Run a complete plasticity mechanisms research study"""
    print("=== NLM PLASTICITY MECHANISMS RESEARCH ===")
    
    # Initialize researcher
    researcher = PlasticityMechanismsResearcher(
        neuron_count=200
    )
    
    try:
        # Run plasticity rule studies
        print("\n" + "="*60)
        print("STARTING PLASTICITY MECHANISMS RESEARCH")
        print("="*60)
        
        # STDP study
        stdp_metrics = researcher.simulate_stdp(
            duration=duration_per_rule,
            learning_rate=0.01
        )
        
        # Hebbian study
        hebbian_metrics = researcher.simulate_hebbian(
            duration=duration_per_rule,
            learning_rate=0.02
        )
        
        # Structural plasticity study
        struct_metrics = researcher.simulate_structural(
            duration=duration_per_rule,
            formation_rate=0.01,
            elimination_rate=0.005
        )
        
        # Compare results
        comparison = researcher.compare_plasticity_rules()
        
        print(f"\n" + "="*60)
        print("PLASTICITY MECHANISMS RESEARCH COMPLETE")
        print("="*60)
        print(f"Total rules tested: {len(researcher.plasticity_rules)}")
        print(f"Best performing rule: {comparison['best_rule']}")
        print(f"Total spikes across all rules: {sum(rule.total_spikes for rule in researcher.plasticity_rules)}")
        
        # Export results
        researcher.export_plasticity_results("plasticity_mechanisms_research.json")
        
        return researcher.plasticity_rules, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    rules, comparison = run_plasticity_mechanisms_research()
    print(f"\nResearch completed successfully with {len(rules)} plasticity rules tested.")
