#!/usr/bin/env python3
"""
NLM Research Examples - Cross-Modal Integration

This module demonstrates research on multisensory integration and cross-modal binding
in the Neural Learning Machine (NLM) framework.
"""

import time
import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple, Any
from dataclasses import dataclass
from enum import Enum

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

class ModalityType(Enum):
    """Types of sensory modalities"""
    VISUAL = "visual"
    AUDITORY = "auditory"
    VESTIBULAR = "vestibular"
    TACTILE = "tactile"
    PROPRIOCEPTIVE = "proprioceptive"

@dataclass
class CrossModalMetrics:
    """Metrics for cross-modal integration studies"""
    modality_pair: str
    binding_strength: float
    integration_efficiency: float
    cross_modal_cognition: float
    sensory_conflict_level: float
    perceptual_learning_rate: float
    multisensory_enhancement: float
    temporal_binding_window: float
    cross_modal_attention: float
    binding_stability: float

class CrossModalIntegrationResearcher:
    """Researcher conducting cross-modal integration studies"""
    
    def __init__(self, neuron_count: int = 450):
        self.neuron_count = neuron_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Cross-modal integration tracking
        self.integration_tests = []
        self.modality_responses = {}
        self.cross_modal_bindings = []
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for cross-modal research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.HEAT_MAP)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_visual_auditory_integration(self, duration: float, visual_auditory_synchrony: float = 0.8) -> CrossModalMetrics:
        """Simulate visual-auditory cross-modal integration"""
        print(f"\n=== VISUAL-AUDITORY INTEGRATION STUDY ===")
        print(f"Simulating visual-auditory integration with synchrony: {visual_auditory_synchrony}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Visual-auditory integration setup
        visual_input_strength = np.random.beta(0.6, 2.0)
        auditory_input_strength = np.random.beta(0.6, 2.0)
        
        # Integration metrics
        binding_strength = 0.0
        total_integration_trials = 0
        successful_bindings = 0
        integration_efficiency = 0.0
        
        # Track multimodal dynamics
        cross_modal_responses = []
        sensory_conflict_scores = []
        perceptual_learning_curves = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Generate multimodal inputs
            visual_input = visual_input_strength * (1.0 + np.random.normal(0, 0.1))
            auditory_input = auditory_input_strength * (1.0 + np.random.normal(0, 0.1))
            
            # Temporal synchrony
            temporal_synchrony = visual_auditory_synchrony + np.random.normal(0, 0.1)
            temporal_synchrony = np.clip(temporal_synchrony, 0.0, 1.0)
            
            # Cross-modal binding
            if np.random.random() < 0.15:  # 15% chance of binding attempt
                total_integration_trials += 1
                
                # Binding strength calculation
                binding_strength_current = (
                    visual_input * auditory_input * temporal_synchrony +
                    np.random.beta(0.2, 5.0) * 0.3
                )
                binding_strength = (binding_strength + binding_strength_current) / max(1, total_integration_trials)
                
                # Successful binding condition
                if binding_strength_current > 0.5:
                    successful_bindings += 1
                    integration_efficiency += binding_strength_current
                
                # Cross-modal response
                cross_modal_response = binding_strength_current * temporal_synchrony
                cross_modal_responses.append(cross_modal_response)
                
                # Sensory conflict score
                sensory_conflict = abs(visual_input - auditory_input) / max(1, visual_input + auditory_input)
                sensory_conflict_scores.append(sensory_conflict)
                
                # Perceptual learning
                perceptual_learning = binding_strength_current * (1.0 - sensory_conflict)
                perceptual_learning_curves.append(perceptual_learning)
                
                # Update visualization
                self.visualization.add_time_series_metric("visual_input", visual_input)
                self.visualization.add_time_series_metric("auditory_input", auditory_input)
                self.visualization.add_time_series_metric("binding_strength", binding_strength)
                self.visualization.add_time_series_metric("cross_modal_response", cross_modal_response)
                self.visualization.add_time_series_metric("sensory_conflict", sensory_conflict)
                self.visualization.add_time_series_metric("perceptual_learning", perceptual_learning)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="VisualAuditoryIntegration",
                    current_state="Active",
                    warnings=[f"Visual: {visual_input:.3f}, Auditory: {auditory_input:.3f}, Binding: {binding_strength:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate cross-modal integration metrics
        final_binding_strength = binding_strength
        integration_success_rate = successful_bindings / max(1, total_integration_trials)
        average_integration_efficiency = integration_efficiency / max(1, successful_bindings)
        
        # Cross-modal cognition
        cross_modal_cognition = integration_success_rate * (1.0 - np.mean(sensory_conflict_scores) if sensory_conflict_scores else 0)
        
        # Perceptual learning rate
        average_perceptual_learning = np.mean(perceptual_learning_curves) if perceptual_learning_curves else 0.0
        
        # Multisensory enhancement
        multisensory_enhancement = (visual_input + auditory_input) / 2.0 * temporal_synchrony
        
        # Temporal binding window (how long temporal asynchrony affects binding)
        temporal_binding_window = 1.0 / max(1, visual_auditory_synchrony)
        
        # Cross-modal attention
        cross_modal_attention = integration_success_rate * temporal_synchrony
        
        # Binding stability (consistency of binding over time)
        if cross_modal_responses:
            binding_stability = 1.0 - (np.std(cross_modal_responses) / max(1, np.mean(cross_modal_responses)))
        else:
            binding_stability = 0.0
        
        metrics = CrossModalMetrics(
            modality_pair="visual-auditory",
            binding_strength=final_binding_strength,
            integration_efficiency=average_integration_efficiency,
            cross_modal_cognition=cross_modal_cognition,
            sensory_conflict_level=np.mean(sensory_conflict_scores) if sensory_conflict_scores else 0.0,
            perceptual_learning_rate=average_perceptual_learning,
            multisensory_enhancement=multisensory_enhancement,
            temporal_binding_window=temporal_binding_window,
            cross_modal_attention=cross_modal_attention,
            binding_stability=binding_stability
        )
        
        self.integration_tests.append(metrics)
        self.modality_responses["visual-auditory"] = {
            "responses": cross_modal_responses,
            "conflicts": sensory_conflict_scores,
            "learning": perceptual_learning_curves
        }
        
        print(f"✓ Visual-auditory integration complete: binding {final_binding_strength:.3f}")
        print(f"  Integration success rate: {integration_success_rate:.3f}")
        print(f"  Average efficiency: {average_integration_efficiency:.3f}")
        print(f"  Cross-modal cognition: {cross_modal_cognition:.3f}")
        
        return metrics
    
    def simulate_visual_tactile_integration(self, duration: float, cross_modal_prediction: float = 0.7) -> CrossModalMetrics:
        """Simulate visual-tactile cross-modal integration"""
        print(f"\n=== VISUAL-TACTILE INTEGRATION STUDY ===")
        print(f"Simulating visual-tactile integration with prediction: {cross_modal_prediction}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Visual-tactile integration setup
        visual_sensitivity = np.random.beta(0.7, 1.5)
        tactile_sensitivity = np.random.beta(0.7, 1.5)
        
        # Integration metrics
        binding_strength = 0.0
        total_integration_trials = 0
        successful_bindings = 0
        integration_efficiency = 0.0
        
        # Track multimodal dynamics
        cross_modal_responses = []
        sensory_conflict_scores = []
        temporal_binding_patterns = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Generate multimodal inputs
            visual_input = visual_sensitivity * (1.0 + np.random.normal(0, 0.1))
            tactile_input = tactile_sensitivity * (1.0 + np.random.normal(0, 0.1))
            
            # Cross-modal prediction
            predicted_tactile = visual_input * cross_modal_prediction
            actual_tactile = tactile_input + np.random.normal(0, 0.1 * cross_modal_prediction)
            
            # Prediction error
            prediction_error = abs(predicted_tactile - actual_tactile)
            
            # Cross-modal binding based on prediction error
            if np.random.random() < 0.15:  # 15% chance of binding attempt
                total_integration_trials += 1
                
                # Binding strength calculation
                binding_strength_current = (
                    visual_input * tactile_input +
                    cross_modal_prediction * (1.0 - prediction_error) +
                    np.random.beta(0.2, 5.0) * 0.2
                )
                binding_strength = (binding_strength + binding_strength_current) / max(1, total_integration_trials)
                
                # Successful binding condition
                if binding_strength_current > 0.5:
                    successful_bindings += 1
                    integration_efficiency += binding_strength_current
                
                # Cross-modal response
                cross_modal_response = binding_strength_current * (1.0 - prediction_error)
                cross_modal_responses.append(cross_modal_response)
                
                # Sensory conflict score
                sensory_conflict = prediction_error
                sensory_conflict_scores.append(sensory_conflict)
                
                # Temporal binding patterns (how timing affects integration)
                temporal_binding = np.random.beta(0.6, 2.0) * (1.0 - prediction_error)
                temporal_binding_patterns.append(temporal_binding)
                
                # Update visualization
                self.visualization.add_time_series_metric("visual_input", visual_input)
                self.visualization.add_time_series_metric("tactile_input", tactile_input)
                self.visualization.add_time_series_metric("binding_strength", binding_strength)
                self.visualization.add_time_series_metric("cross_modal_response", cross_modal_response)
                self.visualization.add_time_series_metric("sensory_conflict", sensory_conflict)
                self.visualization.add_time_series_metric("temporal_binding", temporal_binding)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="VisualTactileIntegration",
                    current_state="Active",
                    warnings=[f"Visual: {visual_input:.3f}, Tactile: {tactile_input:.3f}, Binding: {binding_strength:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate cross-modal integration metrics
        final_binding_strength = binding_strength
        integration_success_rate = successful_bindings / max(1, total_integration_trials)
        average_integration_efficiency = integration_efficiency / max(1, successful_bindings)
        
        # Cross-modal cognition
        cross_modal_cognition = integration_success_rate * (1.0 - np.mean(sensory_conflict_scores) if sensory_conflict_scores else 0)
        
        # Perceptual learning rate
        average_perceptual_learning = np.mean(temporal_binding_patterns) if temporal_binding_patterns else 0.0
        
        # Multisensory enhancement
        multisensory_enhancement = (visual_input + tactile_input) / 2.0 * cross_modal_prediction
        
        # Temporal binding window (how long temporal asynchrony affects binding)
        temporal_binding_window = 1.0 / max(1, cross_modal_prediction)
        
        # Cross-modal attention
        cross_modal_attention = integration_success_rate * cross_modal_prediction
        
        # Binding stability (consistency of binding over time)
        if cross_modal_responses:
            binding_stability = 1.0 - (np.std(cross_modal_responses) / max(1, np.mean(cross_modal_responses)))
        else:
            binding_stability = 0.0
        
        metrics = CrossModalMetrics(
            modality_pair="visual-tactile",
            binding_strength=final_binding_strength,
            integration_efficiency=average_integration_efficiency,
            cross_modal_cognition=cross_modal_cognition,
            sensory_conflict_level=np.mean(sensory_conflict_scores) if sensory_conflict_scores else 0.0,
            perceptual_learning_rate=average_perceptual_learning,
            multisensory_enhancement=multisensory_enhancement,
            temporal_binding_window=temporal_binding_window,
            cross_modal_attention=cross_modal_attention,
            binding_stability=binding_stability
        )
        
        self.integration_tests.append(metrics)
        self.modality_responses["visual-tactile"] = {
            "responses": cross_modal_responses,
            "conflicts": sensory_conflict_scores,
            "temporal_bindings": temporal_binding_patterns
        }
        
        print(f"✓ Visual-tactile integration complete: binding {final_binding_strength:.3f}")
        print(f"  Integration success rate: {integration_success_rate:.3f}")
        print(f"  Average efficiency: {average_integration_efficiency:.3f}")
        print(f"  Cross-modal cognition: {cross_modal_cognition:.3f}")
        
        return metrics
    
    def simulate_auditory_proximal_integration(self, duration: float, multisensory_gain: float = 1.3) -> CrossModalMetrics:
        """Simulate auditory-proximal cross-modal integration"""
        print(f"\n=== AUDITORY-PROXIMAL INTEGRATION STUDY ===")
        print(f"Simulating auditory-proximal integration with gain: {multisensory_gain}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Auditory-proximal integration setup
        auditory_sensitivity = np.random.beta(0.8, 1.5)
        proprioceptive_sensitivity = np.random.beta(0.8, 1.5)
        
        # Integration metrics
        binding_strength = 0.0
        total_integration_trials = 0
        successful_bindings = 0
        integration_efficiency = 0.0
        
        # Track multimodal dynamics
        cross_modal_responses = []
        sensorimotor_prediction_errors = []
        cross_modal_attention_patterns = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Generate multimodal inputs
            auditory_input = auditory_sensitivity * (1.0 + np.random.normal(0, 0.1))
            proprioceptive_input = proprioceptive_sensitivity * (1.0 + np.random.normal(0, 0.1))
            
            # Cross-modal prediction (auditory predicts proprioceptive)
            predicted_proprioceptive = auditory_input * multisensory_gain
            actual_proprioceptive = proprioceptive_input + np.random.normal(0, 0.15 * multisensory_gain)
            
            # Sensorimotor prediction error
            prediction_error = abs(predicted_proprioceptive - actual_proprioceptive)
            sensorimotor_prediction_errors.append(prediction_error)
            
            # Cross-modal binding based on prediction error
            if np.random.random() < 0.15:  # 15% chance of binding attempt
                total_integration_trials += 1
                
                # Binding strength calculation
                binding_strength_current = (
                    auditory_input * proprioceptive_input * multisensory_gain +
                    (1.0 - prediction_error) * 0.5 +
                    np.random.beta(0.2, 5.0) * 0.3
                )
                binding_strength = (binding_strength + binding_strength_current) / max(1, total_integration_trials)
                
                # Successful binding condition
                if binding_strength_current > 0.5:
                    successful_bindings += 1
                    integration_efficiency += binding_strength_current
                
                # Cross-modal response
                cross_modal_response = binding_strength_current * (1.0 - prediction_error)
                cross_modal_responses.append(cross_modal_response)
                
                # Cross-modal attention pattern
                attention_pattern = np.random.beta(0.7, 1.5) * (1.0 - prediction_error)
                cross_modal_attention_patterns.append(attention_pattern)
                
                # Update visualization
                self.visualization.add_time_series_metric("auditory_input", auditory_input)
                self.visualization.add_time_series_metric("proprioceptive_input", proprioceptive_input)
                self.visualization.add_time_series_metric("binding_strength", binding_strength)
                self.visualization.add_time_series_metric("cross_modal_response", cross_modal_response)
                self.visualization.add_time_series_metric("prediction_error", prediction_error)
                self.visualization.add_time_series_metric("attention_pattern", attention_pattern)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="AuditoryProximalIntegration",
                    current_state="Active",
                    warnings=[f"Auditory: {auditory_input:.3f}, Proprioceptive: {proprioceptive_input:.3f}, Binding: {binding_strength:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate cross-modal integration metrics
        final_binding_strength = binding_strength
        integration_success_rate = successful_bindings / max(1, total_integration_trials)
        average_integration_efficiency = integration_efficiency / max(1, successful_bindings)
        
        # Cross-modal cognition
        cross_modal_cognition = integration_success_rate * (1.0 - np.mean(sensorimotor_prediction_errors) if sensorimotor_prediction_errors else 0)
        
        # Perceptual learning rate
        average_perceptual_learning = np.mean(cross_modal_attention_patterns) if cross_modal_attention_patterns else 0.0
        
        # Multisensory enhancement
        multisensory_enhancement = (auditory_input + proprioceptive_input) / 2.0 * multisensory_gain
        
        # Temporal binding window (how long temporal asynchrony affects binding)
        temporal_binding_window = 1.0 / max(1, multisensory_gain)
        
        # Cross-modal attention
        cross_modal_attention = integration_success_rate * multisensory_gain
        
        # Binding stability (consistency of binding over time)
        if cross_modal_responses:
            binding_stability = 1.0 - (np.std(cross_modal_responses) / max(1, np.mean(cross_modal_responses)))
        else:
            binding_stability = 0.0
        
        metrics = CrossModalMetrics(
            modality_pair="auditory-proximal",
            binding_strength=final_binding_strength,
            integration_efficiency=average_integration_efficiency,
            cross_modal_cognition=cross_modal_cognition,
            sensory_conflict_level=np.mean(sensorimotor_prediction_errors) if sensorimotor_prediction_errors else 0.0,
            perceptual_learning_rate=average_perceptual_learning,
            multisensory_enhancement=multisensory_enhancement,
            temporal_binding_window=temporal_binding_window,
            cross_modal_attention=cross_modal_attention,
            binding_stability=binding_stability
        )
        
        self.integration_tests.append(metrics)
        self.modality_responses["auditory-proximal"] = {
            "responses": cross_modal_responses,
            "prediction_errors": sensorimotor_prediction_errors,
            "attention_patterns": cross_modal_attention_patterns
        }
        
        print(f"✓ Auditory-proximal integration complete: binding {final_binding_strength:.3f}")
        print(f"  Integration success rate: {integration_success_rate:.3f}")
        print(f"  Average efficiency: {average_integration_efficiency:.3f}")
        print(f"  Cross-modal cognition: {cross_modal_cognition:.3f}")
        
        return metrics
    
    def compare_cross_modal_integration_strategies(self) -> Dict[str, Any]:
        """Compare results across different cross-modal integration strategies"""
        print(f"\n=== CROSS-MODAL INTEGRATION STRATEGIES COMPARISON ===")
        
        if not self.integration_tests:
            print("No cross-modal integration data collected!")
            return {}
        
        comparison = {
            "best_strategy": None,
            "worst_strategy": None,
            "total_strategies_tested": len(self.integration_tests),
            "strategy_metrics": {}
        }
        
        # Score each strategy (higher is better)
        strategy_scores = {}
        for test in self.integration_tests:
            # Combined score considering integration strength and efficiency
            score = (
                test.binding_strength * 0.2 +  # Binding strength
                test.integration_efficiency * 0.2 +  # Integration efficiency
                test.cross_modal_cognition * 0.2 +  # Cross-modal cognition
                test.multisensory_enhancement * 0.2 +  # Multisensory enhancement
                test.cross_modal_attention * 0.2  # Cross-modal attention
            )
            strategy_scores[test.modality_pair] = score
        
        # Find best and worst
        comparison["best_strategy"] = max(strategy_scores, key=strategy_scores.get)
        comparison["worst_strategy"] = min(strategy_scores, key=strategy_scores.get)
        
        # Detailed metrics
        for test in self.integration_tests:
            comparison["strategy_metrics"][test.modality_pair] = {
                "binding_strength": test.binding_strength,
                "integration_efficiency": test.integration_efficiency,
                "cross_modal_cognition": test.cross_modal_cognition,
                "sensory_conflict_level": test.sensory_conflict_level,
                "perceptual_learning_rate": test.perceptual_learning_rate,
                "multisensory_enhancement": test.multisensory_enhancement,
                "temporal_binding_window": test.temporal_binding_window,
                "cross_modal_attention": test.cross_modal_attention,
                "binding_stability": test.binding_stability,
                "score": strategy_scores[test.modality_pair],
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best strategy: {comparison['best_strategy']} (score: {strategy_scores[comparison['best_strategy']]:.4f})")
        print(f"  Worst strategy: {comparison['worst_strategy']} (score: {strategy_scores[comparison['worst_strategy']]:.4f})")
        print(f"  Total strategies tested: {len(self.integration_tests)}")
        
        return comparison
    
    def export_cross_modal_results(self, filename: str):
        """Export cross-modal integration research results"""
        print(f"\n=== EXPORTING CROSS-MODAL RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Cross-Modal Integration Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
            },
            "integration_tests": [
                {
                    "modality_pair": test.modality_pair,
                    "binding_strength": test.binding_strength,
                    "integration_efficiency": test.integration_efficiency,
                    "cross_modal_cognition": test.cross_modal_cognition,
                    "sensory_conflict_level": test.sensory_conflict_level,
                    "perceptual_learning_rate": test.perceptual_learning_rate,
                    "multisensory_enhancement": test.multisensory_enhancement,
                    "temporal_binding_window": test.temporal_binding_window,
                    "cross_modal_attention": test.cross_modal_attention,
                    "binding_stability": test.binding_stability,
                }
                for test in self.integration_tests
            ],
            "modality_responses": self.modality_responses,
            "comparison": self.compare_cross_modal_integration_strategies(),
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
        
        print(f"✓ Cross-modal integration results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_cross_modal_integration_research(duration_per_strategy: float = 12.0):
    """Run a complete cross-modal integration research study"""
    print("=== NLM CROSS-MODAL INTEGRATION RESEARCH ===")
    
    # Initialize researcher
    researcher = CrossModalIntegrationResearcher(
        neuron_count=450
    )
    
    try:
        # Run cross-modal integration strategy studies
        print("\n" + "="*60)
        print("STARTING CROSS-MODAL INTEGRATION RESEARCH")
        print("="*60)
        
        # Visual-auditory integration study
        visual_auditory_metrics = researcher.simulate_visual_auditory_integration(
            duration=duration_per_strategy,
            visual_auditory_synchrony=0.8
        )
        
        # Visual-tactile integration study
        visual_tactile_metrics = researcher.simulate_visual_tactile_integration(
            duration=duration_per_strategy,
            cross_modal_prediction=0.7
        )
        
        # Auditory-proximal integration study
        auditory_proximal_metrics = researcher.simulate_auditory_proximal_integration(
            duration=duration_per_strategy,
            multisensory_gain=1.3
        )
        
        # Compare results
        comparison = researcher.compare_cross_modal_integration_strategies()
        
        print(f"\n" + "="*60)
        print("CROSS-MODAL INTEGRATION RESEARCH COMPLETE")
        print("="*60)
        print(f"Total strategies tested: {len(researcher.integration_tests)}")
        print(f"Best performing strategy: {comparison['best_strategy']}")
        
        # Export results
        researcher.export_cross_modal_results("cross_modal_integration_research.json")
        
        return researcher.integration_tests, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    tests, comparison = run_cross_modal_integration_research()
    print(f"\nResearch completed successfully with {len(tests)} cross-modal integration strategies tested.")
