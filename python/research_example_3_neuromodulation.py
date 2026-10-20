#!/usr/bin/env python3
"""
NLM Research Examples - Neuromodulation Studies

This module demonstrates research on neuromodulation (dopamine, curiosity, reward-based learning)
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

class NeuromodulatorType(Enum):
    """Types of neuromodulators"""
    DOPAMINE = "dopamine"
    CURIOUSY = "curiosity"
    NOVELTY = "novelty"
    PREDICTION_ERROR = "prediction_error"
    REWARD = "reward"

@dataclass
class NeuromodulationMetrics:
    """Metrics for neuromodulation studies"""
    modulator_type: NeuromodulatorType
    baseline_level: float
    peak_level: float
    learning_rate: float
    reward_acquisition_rate: float
    exploration_rate: float
    prediction_error_reduction: float
    temporal_difference_error: float
    dopamine_signal_strength: float
    curiosity_drive: float
    reward_prediction_error: float

class NeuromodulationResearcher:
    """Researcher conducting neuromodulation studies"""
    
    def __init__(self, neuron_count: int = 300):
        self.neuron_count = neuron_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Neuromodulation tracking
        self.modulators = []
        self.rewards_learned = []
        self.curiosity_signals = []
        self.prediction_errors = []
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for neuromodulation research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.HEAT_MAP)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_dopamine_modulation(self, duration: float, initial_dopamine: float = 0.5) -> NeuromodulationMetrics:
        """Simulate dopamine modulation in reward learning"""
        print(f"\n=== DOPAMINE NEUROMODULATION STUDY ===")
        print(f"Simulating dopamine modulation with initial level: {initial_dopamine}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Dopamine dynamics
        current_dopamine = initial_dopamine
        reward_history = []
        prediction_error_history = []
        learning_curve = []
        
        # Track neuromodulatory signals
        baseline_levels = []
        peak_levels = []
        td_errors = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Simulate reward prediction error
            predicted_reward = np.random.beta(0.5, 2.0) * current_dopamine
            actual_reward = np.random.beta(1.0, 2.0) * (current_dopamine + 0.1)
            
            prediction_error = actual_reward - predicted_reward
            reward_history.append(actual_reward)
            prediction_error_history.append(prediction_error)
            
            # Update dopamine levels based on prediction error
            # phasic dopamine response
            phasic_response = 0.1 * max(0, prediction_error)
            current_dopamine += phasic_response * 0.01
            
            # tonic dopamine baseline
            tonic_adjustment = -0.001 * current_dopamine
            current_dopamine += tonic_adjustment
            
            # Keep dopamine within bounds
            current_dopamine = np.clip(current_dopamine, 0.0, 2.0)
            
            # Calculate learning rate based on dopamine
            learning_rate = 0.01 * (0.5 + current_dopamine / 2.0)
            
            # Update behavior based on dopamine-modulated learning
            learning_curve.append(learning_rate * prediction_error)
            
            # Track neuromodulatory metrics
            baseline_levels.append(current_dopamine)
            peak_levels.append(current_dopamine + 0.2)
            td_errors.append(abs(prediction_error))
            
            # Update visualization
            self.visualization.add_time_series_metric("dopamine_level", current_dopamine)
            self.visualization.add_time_series_metric("prediction_error", prediction_error)
            self.visualization.add_time_series_metric("reward_prediction", predicted_reward)
            self.visualization.add_time_series_metric("learning_curve", learning_curve[-1])
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="DopamineModulation",
                    current_state="Active",
                    warnings=[f"Dopamine: {current_dopamine:.3f}, Learning rate: {learning_rate:.4f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate neuromodulation metrics
        final_dopamine = current_dopamine
        average_dopamine = np.mean(baseline_levels)
        peak_dopamine = max(peak_levels)
        
        # Calculate reward acquisition rate
        reward_acquisition_rate = np.mean([1 if r > 0.5 else 0 for r in reward_history])
        
        # Calculate prediction error reduction
        td_error_reduction = 1.0 - (np.std(td_errors) / max(1, np.mean(td_errors)))
        
        # Calculate curiosity drive
        curiosity_drive = 0.5 * (1.0 - np.exp(-final_dopamine))
        
        metrics = NeuromodulationMetrics(
            modulator_type=NeuromodulatorType.DOPAMINE,
            baseline_level=average_dopamine,
            peak_level=peak_dopamine,
            learning_rate=np.mean(learning_curve),
            reward_acquisition_rate=reward_acquisition_rate,
            exploration_rate=np.random.random() * 0.4 + 0.3,
            prediction_error_reduction=td_error_reduction,
            temporal_difference_error=np.mean(td_errors),
            dopamine_signal_strength=final_dopamine,
            curiosity_drive=curiosity_drive,
            reward_prediction_error=np.mean(np.abs(prediction_error_history))
        )
        
        self.modulators.append(metrics)
        self.rewards_learned.append(np.sum(reward_history))
        self.prediction_errors.append(np.mean(td_errors))
        
        print(f"✓ Dopamine modulation complete: final level {final_dopamine:.3f}")
        print(f"  Average prediction error: {metrics.reward_prediction_error:.4f}")
        print(f"  Reward acquisition rate: {reward_acquisition_rate:.4f}")
        print(f"  Learning rate: {metrics.learning_rate:.4f}")
        print(f"  Curiosity drive: {curiosity_drive:.4f}")
        
        return metrics
    
    def simulate_curiosity_driven_learning(self, duration: float, curiosity_param: float = 0.8) -> NeuromodulationMetrics:
        """Simulate curiosity-driven exploration and learning"""
        print(f"\n=== CURIOUSY-DRIVEN LEARNING STUDY ===")
        print(f"Simulating curiosity-driven learning with curiosity param: {curiosity_param}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Curiosity-driven learning dynamics
        current_curiosity = curiosity_param
        exploration_rewards = []
        intrinsic_motivation = []
        extrinsic_rewards = []
        prediction_errors = []
        
        # Track neuromodulatory signals
        curiosity_drives = []
        reward_prediction_errors = []
        learning_rates = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Intrinsic curiosity: explore novel states
            novel_state = np.random.random() < 0.2  # 20% chance of novel state
            intrinsic_motivation_value = current_curiosity * novel_state
            intrinsic_motivation.append(intrinsic_motivation_value)
            
            # Extrinsic reward
            extrinsic_reward = np.random.beta(2.0, 2.0)
            extrinsic_rewards.append(extrinsic_reward)
            
            # Combined reward signal
            combined_reward = intrinsic_motivation_value + extrinsic_reward
            
            # Prediction error
            predicted_reward = np.random.beta(1.0, 2.0)
            prediction_error = combined_reward - predicted_reward
            prediction_errors.append(prediction_error)
            
            # Update curiosity based on prediction error
            curiosity_update = 0.005 * (abs(prediction_error) - 0.1 * current_curiosity)
            current_curiosity += curiosity_update
            
            # Keep curiosity within bounds
            current_curiosity = np.clip(current_curiosity, 0.1, 2.0)
            
            # Learning rate modulated by curiosity
            learning_rate = 0.01 * current_curiosity
            learning_rates.append(learning_rate)
            
            # Exploration-exploitation trade-off
            exploration_rate = 0.7 * current_curiosity / 2.0
            
            # Track neuromodulatory metrics
            curiosity_drives.append(current_curiosity)
            reward_prediction_errors.append(abs(prediction_error))
            
            # Update visualization
            self.visualization.add_time_series_metric("curiosity_level", current_curiosity)
            self.visualization.add_time_series_metric("intrinsic_motivation", intrinsic_motivation_value)
            self.visualization.add_time_series_metric("extrinsic_reward", extrinsic_reward)
            self.visualization.add_time_series_metric("exploration_rate", exploration_rate)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="CuriosityDrivenLearning",
                    current_state="Active",
                    warnings=[f"Curiosity: {current_curiosity:.3f}, Exploration: {exploration_rate:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate neuromodulation metrics
        final_curiosity = current_curiosity
        average_curiosity = np.mean(curiosity_drives)
        
        # Calculate reward acquisition
        curiosity_reward_acquisition = np.mean([1 if r > 0.6 else 0 for r in extrinsic_rewards])
        
        # Calculate prediction error reduction
        td_error_reduction = 1.0 - (np.std(reward_prediction_errors) / max(1, np.mean(reward_prediction_errors)))
        
        metrics = NeuromodulationMetrics(
            modulator_type=NeuromodulatorType.CURIOUSY,
            baseline_level=average_curiosity,
            peak_level=max(curiosity_drives),
            learning_rate=np.mean(learning_rates),
            reward_acquisition_rate=curiosity_reward_acquisition,
            exploration_rate=np.mean([0.7 * c / 2.0 for c in curiosity_drives]),
            prediction_error_reduction=td_error_reduction,
            temporal_difference_error=np.mean(reward_prediction_errors),
            dopamine_signal_strength=final_curiosity * 0.7,  # Convert to dopamine equivalent
            curiosity_drive=final_curiosity,
            reward_prediction_error=np.mean(np.abs(prediction_errors))
        )
        
        self.modulators.append(metrics)
        self.curiosity_signals.append(np.mean(curiosity_drives))
        self.prediction_errors.append(np.mean(prediction_errors))
        
        print(f"✓ Curiosity-driven learning complete: final curiosity {final_curiosity:.3f}")
        print(f"  Average curiosity: {average_curiosity:.4f}")
        print(f"  Reward acquisition: {curiosity_reward_acquisition:.4f}")
        print(f"  Learning rate: {metrics.learning_rate:.4f}")
        print(f"  Exploration rate: {metrics.exploration_rate:.4f}")
        
        return metrics
    
    def simulate_reward_based_learning(self, duration: float, reward_strength: float = 1.0) -> NeuromodulationMetrics:
        """Simulate reward-based learning with reinforcement"""
        print(f"\n=== REWARD-BASED LEARNING STUDY ===")
        print(f"Simulating reward-based learning with strength: {reward_strength}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Reward-based learning dynamics
        policy_value = np.random.uniform(0.5, 0.8)  # Initial policy value
        value_estimates = []
        reward_history = []
        policy_updates = []
        
        # Track neuromodulatory signals
        reward_prediction_errors = []
        learning_curves = []
        temporal_differences = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Choose action based on policy
            action_value = policy_value + np.random.normal(0, 0.1)
            action_taken = np.argmax(action_value) if isinstance(action_value, np.ndarray) else 1
            
            # Receive reward
            expected_reward = policy_value * reward_strength
            actual_reward = expected_reward + np.random.normal(0, 0.2 * reward_strength)
            actual_reward = max(0.0, actual_reward)  # Ensure non-negative
            
            # Calculate reward prediction error (RPE)
            rpe = actual_reward - expected_reward
            reward_prediction_errors.append(abs(rpe))
            
            # Update value estimate
            value_update = reward_strength * rpe
            policy_value += value_update * 0.1
            policy_value = np.clip(policy_value, 0.0, 1.0)
            
            # Update policy
            policy_update = policy_value - value_estimates[-1] if value_estimates else policy_value
            policy_updates.append(policy_update)
            
            value_estimates.append(policy_value)
            reward_history.append(actual_reward)
            temporal_differences.append(abs(rpe))
            
            # Learning rate modulated by reward
            learning_rate = reward_strength * 0.05
            learning_curves.append(learning_rate)
            
            # Update visualization
            self.visualization.add_time_series_metric("policy_value", policy_value)
            self.visualization.add_time_series_metric("expected_reward", expected_reward)
            self.visualization.add_time_series_metric("actual_reward", actual_reward)
            self.visualization.add_time_series_metric("reward_prediction_error", reward_prediction_errors[-1])
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="RewardBasedLearning",
                    current_state="Active",
                    warnings=[f"Policy: {policy_value:.3f}, RPE: {rpe:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate neuromodulation metrics
        final_policy = policy_value
        average_rpe = np.mean(reward_prediction_errors)
        
        # Calculate reward acquisition rate
        reward_acquisition_rate = np.mean([1 if r > 0.5 else 0 for r in reward_history])
        
        # Calculate prediction error reduction
        td_error_reduction = 1.0 - (np.std(temporal_differences) / max(1, np.mean(temporal_differences)))
        
        metrics = NeuromodulationMetrics(
            modulator_type=NeuromodulatorType.REWARD,
            baseline_level=final_policy,
            peak_level=max(value_estimates),
            learning_rate=np.mean(learning_curves),
            reward_acquisition_rate=reward_acquisition_rate,
            exploration_rate=0.5 - policy_value * 0.3,  # Less exploration with higher policy value
            prediction_error_reduction=td_error_reduction,
            temporal_difference_error=average_rpe,
            dopamine_signal_strength=final_policy * 1.2,  # Convert to dopamine equivalent
            curiosity_drive=0.6 - policy_value * 0.2,  # Reduced curiosity with stronger policy
            reward_prediction_error=average_rpe
        )
        
        self.modulators.append(metrics)
        self.rewards_learned.append(np.sum(reward_history))
        self.prediction_errors.append(average_rpe)
        
        print(f"✓ Reward-based learning complete: final policy {final_policy:.3f}")
        print(f"  Average reward prediction error: {average_rpe:.4f}")
        print(f"  Reward acquisition rate: {reward_acquisition_rate:.4f}")
        print(f"  Learning rate: {metrics.learning_rate:.4f}")
        print(f"  Exploration rate: {metrics.exploration_rate:.4f}")
        
        return metrics
    
    def compare_neuromodulation_strategies(self) -> Dict[str, Any]:
        """Compare results across different neuromodulation strategies"""
        print(f"\n=== NEUROMODULATION STRATEGIES COMPARISON ===")
        
        if not self.modulators:
            print("No neuromodulation data collected!")
            return {}
        
        comparison = {
            "best_strategy": None,
            "worst_strategy": None,
            "total_strategies_tested": len(self.modulators),
            "strategy_metrics": {}
        }
        
        # Score each strategy (higher is better)
        strategy_scores = {}
        for modulator in self.modulators:
            # Combined score considering learning, stability, and exploration
            score = (
                modulator.reward_acquisition_rate * 0.3 +  # Reward learning
                modulator.prediction_error_reduction * 0.3 +  # Learning accuracy
                modulator.exploration_rate * 0.2 +  # Exploration balance
                (1.0 - modulator.reward_prediction_error / max(1, modulator.baseline_level)) * 0.2  # RPE reduction
            )
            strategy_scores[modulator.modulator_type.value] = score
        
        # Find best and worst
        comparison["best_strategy"] = max(strategy_scores, key=strategy_scores.get)
        comparison["worst_strategy"] = min(strategy_scores, key=strategy_scores.get)
        
        # Detailed metrics
        for modulator in self.modulators:
            comparison["strategy_metrics"][modulator.modulator_type.value] = {
                "baseline_level": modulator.baseline_level,
                "peak_level": modulator.peak_level,
                "learning_rate": modulator.learning_rate,
                "reward_acquisition_rate": modulator.reward_acquisition_rate,
                "exploration_rate": modulator.exploration_rate,
                "prediction_error_reduction": modulator.prediction_error_reduction,
                "temporal_difference_error": modulator.temporal_difference_error,
                "dopamine_signal_strength": modulator.dopamine_signal_strength,
                "curiosity_drive": modulator.curiosity_drive,
                "reward_prediction_error": modulator.reward_prediction_error,
                "score": strategy_scores[modulator.modulator_type.value],
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best strategy: {comparison['best_strategy']} (score: {strategy_scores[comparison['best_strategy']]:.4f})")
        print(f"  Worst strategy: {comparison['worst_strategy']} (score: {strategy_scores[comparison['worst_strategy']]:.4f})")
        print(f"  Total strategies tested: {len(self.modulators)}")
        
        return comparison
    
    def export_neuromodulation_results(self, filename: str):
        """Export neuromodulation research results"""
        print(f"\n=== EXPORTING NEUROMODULATION RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Neuromodulation Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
            },
            "neuromodulators": [
                {
                    "modulator_type": modulator.modulator_type.value,
                    "baseline_level": modulator.baseline_level,
                    "peak_level": modulator.peak_level,
                    "learning_rate": modulator.learning_rate,
                    "reward_acquisition_rate": modulator.reward_acquisition_rate,
                    "exploration_rate": modulator.exploration_rate,
                    "prediction_error_reduction": modulator.prediction_error_reduction,
                    "temporal_difference_error": modulator.temporal_difference_error,
                    "dopamine_signal_strength": modulator.dopamine_signal_strength,
                    "curiosity_drive": modulator.curiosity_drive,
                    "reward_prediction_error": modulator.reward_prediction_error,
                }
                for modulator in self.modulators
            ],
            "comparison": self.compare_neuromodulation_strategies(),
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
        
        print(f"✓ Neuromodulation results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_neuromodulation_research(duration_per_strategy: float = 12.0):
    """Run a complete neuromodulation research study"""
    print("=== NLM NEUROMODULATION RESEARCH ===")
    
    # Initialize researcher
    researcher = NeuromodulationResearcher(
        neuron_count=300
    )
    
    try:
        # Run neuromodulation strategy studies
        print("\n" + "="*60)
        print("STARTING NEUROMODULATION RESEARCH")
        print("="*60)
        
        # Dopamine modulation study
        dopamine_metrics = researcher.simulate_dopamine_modulation(
            duration=duration_per_strategy,
            initial_dopamine=0.5
        )
        
        # Curiosity-driven learning study
        curiosity_metrics = researcher.simulate_curiosity_driven_learning(
            duration=duration_per_strategy,
            curiosity_param=0.8
        )
        
        # Reward-based learning study
        reward_metrics = researcher.simulate_reward_based_learning(
            duration=duration_per_strategy,
            reward_strength=1.0
        )
        
        # Compare results
        comparison = researcher.compare_neuromodulation_strategies()
        
        print(f"\n" + "="*60)
        print("NEUROMODULATION RESEARCH COMPLETE")
        print("="*60)
        print(f"Total strategies tested: {len(researcher.modulators)}")
        print(f"Best performing strategy: {comparison['best_strategy']}")
        
        # Export results
        researcher.export_neuromodulation_results("neuromodulation_research.json")
        
        return researcher.modulators, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    modulators, comparison = run_neuromodulation_research()
    print(f"\nResearch completed successfully with {len(modulators)} neuromodulation strategies tested.")
