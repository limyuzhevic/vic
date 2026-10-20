#!/usr/bin/env python3
"""
NLM Research Examples - Curiosity-Driven Exploration

This module demonstrates research on curiosity-driven exploration and exploration-exploitation trade-offs
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

class CuriosityType(Enum):
    """Types of curiosity mechanisms"""
    INTRINSIC_MOTIVATION = "intrinsic_motivation"
    INFORMATION_GAP = "information_gap"
    NOVELTY_SEEKING = "novelty_seeking"

@dataclass
class CuriosityMetrics:
    """Metrics for curiosity-driven exploration studies"""
    curiosity_type: CuriosityType
    exploration_rate: float
    exploitation_rate: float
    information_gain: float
    novelty_score: float
    learning_efficiency: float
    reward_prediction_error: float
    intrinsic_motivation: float
    uncertainty_reduction: float
    exploration_exploitation_balance: float

class CuriosityDrivenExplorationResearcher:
    """Researcher conducting curiosity-driven exploration studies"""
    
    def __init__(self, neuron_count: int = 350):
        self.neuron_count = neuron_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Curiosity-driven exploration tracking
        self.curiosity_strategies = []
        self.uncertainty_estimates = []
        self.novelty_memories = {}
        self.reward_estimates = {}
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for curiosity research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.HEAT_MAP)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_intrinsic_motivation(self, duration: float, motivation_strength: float = 0.8) -> CuriosityMetrics:
        """Simulate intrinsic motivation-driven exploration"""
        print(f"\n=== INTRINSIC MOTIVATION STUDY ===")
        print(f"Simulating intrinsic motivation with strength: {motivation_strength}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Intrinsic motivation setup
        current_motivation = motivation_strength
        exploration_count = 0
        exploitation_count = 0
        information_gain_total = 0.0
        
        # Track curiosity dynamics
        uncertainty_history = []
        novelty_history = []
        reward_history = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Information gap (uncertainty)
            current_uncertainty = np.random.beta(0.5, 2.0)
            uncertainty_history.append(current_uncertainty)
            
            # Novelty (newness of state)
            novelty = np.random.beta(0.3, 3.0)
            novelty_history.append(novelty)
            
            # Reward estimate
            reward_estimate = np.random.beta(1.0, 2.0)
            reward_history.append(reward_estimate)
            
            # Exploration vs exploitation decision
            # Driven by uncertainty and novelty
            exploration_drive = current_motivation * current_uncertainty
            novelty_drive = current_motivation * novelty
            
            if exploration_drive > 0.5 or novelty > 0.7:
                # Exploration
                exploration_count += 1
                # Information gain from exploration
                information_gain = current_uncertainty * novelty * motivation_strength
                information_gain_total += information_gain
            else:
                # Exploitation
                exploitation_count += 1
                # Information gain from exploitation
                information_gain = reward_estimate * 0.5
                information_gain_total += information_gain
            
            # Update motivation based on outcomes
            if exploration_count > 0:
                # Increase motivation if exploring leads to new information
                outcome_quality = np.random.beta(0.8, 1.5)
                motivation_update = (current_uncertainty * outcome_quality - 0.1) * motivation_strength * 0.01
                current_motivation += motivation_update
                current_motivation = np.clip(current_motivation, 0.1, 2.0)
            
            # Update visualization
            self.visualization.add_time_series_metric("motivation_level", current_motivation)
            self.visualization.add_time_series_metric("exploration_count", exploration_count)
            self.visualization.add_time_series_metric("exploitation_count", exploitation_count)
            self.visualization.add_time_series_metric("information_gain", information_gain)
            self.visualization.add_time_series_metric("uncertainty", current_uncertainty)
            self.visualization.add_time_series_metric("novelty", novelty)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="IntrinsicMotivation",
                    current_state="Active",
                    warnings=[f"Motivation: {current_motivation:.3f}, Exploration: {exploration_count}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate curiosity metrics
        total_actions = exploration_count + exploitation_count
        actual_exploration_rate = exploration_count / max(1, total_actions)
        actual_exploitation_rate = exploitation_count / max(1, total_actions)
        
        average_information_gain = information_gain_total / max(1, total_actions)
        average_novelty = np.mean(novelty_history)
        
        # Learning efficiency
        learning_efficiency = average_information_gain / max(1, duration)
        
        # Reward prediction error
        reward_prediction_error = np.std(reward_history) / max(1, np.mean(reward_history))
        
        # Uncertainty reduction
        uncertainty_reduction = 1.0 - (np.std(uncertainty_history) / max(1, np.mean(uncertainty_history)))
        
        # Exploration-exploitation balance
        exploration_exploitation_balance = actual_exploration_rate
        
        metrics = CuriosityMetrics(
            curiosity_type=CuriosityType.INTRINSIC_MOTIVATION,
            exploration_rate=actual_exploration_rate,
            exploitation_rate=actual_exploitation_rate,
            information_gain=average_information_gain,
            novelty_score=average_novelty,
            learning_efficiency=learning_efficiency,
            reward_prediction_error=reward_prediction_error,
            intrinsic_motivation=current_motivation,
            uncertainty_reduction=uncertainty_reduction,
            exploration_exploitation_balance=exploration_exploitation_balance
        )
        
        self.curiosity_strategies.append(metrics)
        
        print(f"✓ Intrinsic motivation complete: motivation {current_motivation:.3f}")
        print(f"  Exploration rate: {actual_exploration_rate:.3f}")
        print(f"  Information gain: {average_information_gain:.3f}")
        print(f"  Learning efficiency: {learning_efficiency:.3f}")
        
        return metrics
    
    def simulate_information_gap_driven(self, duration: float, gap_sensitivity: float = 1.2) -> CuriosityMetrics:
        """Simulate information-gap driven exploration (predictive coding)"""
        print(f"\n=== INFORMATION GAP STUDY ===")
        print(f"Simulating information-gap driven exploration with sensitivity: {gap_sensitivity}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Information-gap setup
        current_uncertainty = np.random.beta(0.5, 2.0)
        exploration_actions = 0
        exploitation_actions = 0
        total_information_gain = 0.0
        
        # Track information dynamics
        prediction_errors = []
        surprise_levels = []
        prediction_accuracy = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Generate prediction
            predicted_outcome = np.random.beta(0.5, 2.0)
            
            # Actual outcome
            actual_outcome = predicted_outcome + np.random.normal(0, gap_sensitivity * 0.5)
            actual_outcome = np.clip(actual_outcome, 0.0, 1.0)
            
            # Prediction error
            prediction_error = abs(predicted_outcome - actual_outcome)
            prediction_errors.append(prediction_error)
            
            # Surprise level (absolute prediction error)
            surprise = prediction_error * np.random.beta(1.5, 2.0)
            surprise_levels.append(surprise)
            
            # Prediction accuracy
            accuracy = 1.0 - prediction_error
            prediction_accuracy.append(accuracy)
            
            # Information gap (prediction error)
            information_gap = prediction_error
            current_uncertainty = (current_uncertainty + information_gap) / 2.0
            
            # Exploration-exploitation decision
            if information_gap > 0.3:  # Large prediction error -> explore
                exploration_actions += 1
                # Information gain from reducing uncertainty
                information_gain = information_gap * surprise * gap_sensitivity
                total_information_gain += information_gain
            else:  # Small prediction error -> exploit
                exploitation_actions += 1
                # Information gain from confirming predictions
                information_gain = accuracy * 0.3
                total_information_gain += information_gain
            
            # Update uncertainty based on action outcome
            if exploration_actions > 0:
                # Exploration reduces uncertainty
                uncertainty_update = -information_gap * 0.1
                current_uncertainty += uncertainty_update
            else:
                # Exploitation may increase or decrease uncertainty
                uncertainty_update = np.random.normal(0, 0.05)
                current_uncertainty += uncertainty_update
            
            current_uncertainty = np.clip(current_uncertainty, 0.1, 1.0)
            
            # Update visualization
            self.visualization.add_time_series_metric("uncertainty_level", current_uncertainty)
            self.visualization.add_time_series_metric("exploration_actions", exploration_actions)
            self.visualization.add_time_series_metric("exploitation_actions", exploitation_actions)
            self.visualization.add_time_series_metric("information_gain", information_gain)
            self.visualization.add_time_series_metric("prediction_error", prediction_error)
            self.visualization.add_time_series_metric("surprise_level", surprise)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="InformationGapDriven",
                    current_state="Active",
                    warnings=[f"Uncertainty: {current_uncertainty:.3f}, Gap: {information_gap:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate curiosity metrics
        total_actions = exploration_actions + exploitation_actions
        exploration_rate = exploration_actions / max(1, total_actions)
        exploitation_rate = exploitation_actions / max(1, total_actions)
        
        average_information_gain = total_information_gain / max(1, total_actions)
        average_surprise = np.mean(surprise_levels)
        
        # Learning efficiency
        learning_efficiency = average_information_gain / max(1, duration)
        
        # Reward prediction error
        reward_prediction_error = np.mean(prediction_errors)
        
        # Uncertainty reduction
        uncertainty_reduction = 1.0 - (np.std([current_uncertainty] * len(prediction_errors)) / max(1, np.mean([current_uncertainty] * len(prediction_errors))))
        
        # Exploration-exploitation balance
        exploration_exploitation_balance = exploration_rate
        
        metrics = CuriosityMetrics(
            curiosity_type=CuriosityType.INFORMATION_GAP,
            exploration_rate=exploration_rate,
            exploitation_rate=exploitation_rate,
            information_gain=average_information_gain,
            novelty_score=average_surprise,
            learning_efficiency=learning_efficiency,
            reward_prediction_error=reward_prediction_error,
            intrinsic_motivation=current_uncertainty,
            uncertainty_reduction=uncertainty_reduction,
            exploration_exploitation_balance=exploration_exploitation_balance
        )
        
        self.curiosity_strategies.append(metrics)
        
        print(f"✓ Information-gap driven complete: uncertainty {current_uncertainty:.3f}")
        print(f"  Exploration rate: {exploration_rate:.3f}")
        print(f"  Information gain: {average_information_gain:.3f}")
        print(f"  Average surprise: {average_surprise:.3f}")
        
        return metrics
    
    def simulate_novelty_seeking(self, duration: float, novelty_threshold: float = 0.6) -> CuriosityMetrics:
        """Simulate novelty-seeking driven exploration"""
        print(f"\n=== NOVELTY-SEEKING STUDY ===")
        print(f"Simulating novelty-seeking with threshold: {novelty_threshold}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Novelty-seeking setup
        curiosity_drive = np.random.beta(0.7, 1.5)
        exploration_count = 0
        exploitation_count = 0
        novelty_gain_total = 0.0
        
        # Track novelty dynamics
        novelty_history = []
        reward_responses = []
        learning_curves = []
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Novelty assessment
            current_novelty = np.random.beta(0.4, 3.0)
            novelty_history.append(current_novelty)
            
            # Reward response
            reward_response = np.random.beta(1.0, 2.0)
            reward_responses.append(reward_response)
            
            # Novelty-seeking decision
            if current_novelty > novelty_threshold:
                # Novelty-seeking exploration
                exploration_count += 1
                # Novelty gain (surprise and discovery)
                novelty_gain = current_novelty * curiosity_drive * reward_response
                novelty_gain_total += novelty_gain
                
                # Update curiosity
                curiosity_update = (current_novelty - novelty_threshold) * curiosity_drive * 0.01
                curiosity_drive += curiosity_update
                curiosity_drive = np.clip(curiosity_drive, 0.1, 2.0)
            else:
                # Exploitation of familiar patterns
                exploitation_count += 1
                # Novelty gain from refinement
                novelty_gain = reward_response * 0.4
                novelty_gain_total += novelty_gain
            
            # Learning curve (performance improvement)
            learning_efficiency = curiosity_drive * (1.0 - current_novelty * 0.3)
            learning_curves.append(learning_efficiency)
            
            # Update visualization
            self.visualization.add_time_series_metric("curiosity_drive", curiosity_drive)
            self.visualization.add_time_series_metric("novelty_count", exploration_count)
            self.visualization.add_time_series_metric("exploitation_count", exploitation_count)
            self.visualization.add_time_series_metric("novelty_gain", novelty_gain)
            self.visualization.add_time_series_metric("novelty_assessment", current_novelty)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="NoveltySeeking",
                    current_state="Active",
                    warnings=[f"Curiosity: {curiosity_drive:.3f}, Novelty: {current_novelty:.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate curiosity metrics
        total_actions = exploration_count + exploitation_count
        exploration_rate = exploration_count / max(1, total_actions)
        exploitation_rate = exploitation_count / max(1, total_actions)
        
        average_novelty_gain = novelty_gain_total / max(1, total_actions)
        average_novelty = np.mean(novelty_history)
        
        # Learning efficiency
        learning_efficiency = np.mean(learning_curves)
        
        # Reward prediction error
        reward_prediction_error = np.std(reward_responses) / max(1, np.mean(reward_responses))
        
        # Uncertainty reduction (decrease with novel exploration)
        uncertainty_reduction = 1.0 - (np.std(novelty_history) / max(1, np.mean(novelty_history)))
        
        # Exploration-exploitation balance
        exploration_exploitation_balance = exploration_rate
        
        metrics = CuriosityMetrics(
            curiosity_type=CuriosityType.NOVELTY_SEEKING,
            exploration_rate=exploration_rate,
            exploitation_rate=exploitation_rate,
            information_gain=average_novelty_gain,
            novelty_score=average_novelty,
            learning_efficiency=learning_efficiency,
            reward_prediction_error=reward_prediction_error,
            intrinsic_motivation=curiosity_drive,
            uncertainty_reduction=uncertainty_reduction,
            exploration_exploitation_balance=exploration_exploitation_balance
        )
        
        self.curiosity_strategies.append(metrics)
        
        print(f"✓ Novelty-seeking complete: curiosity {curiosity_drive:.3f}")
        print(f"  Exploration rate: {exploration_rate:.3f}")
        print(f"  Novelty gain: {average_novelty_gain:.3f}")
        print(f"  Learning efficiency: {learning_efficiency:.3f}")
        
        return metrics
    
    def compare_curiosity_driven_strategies(self) -> Dict[str, Any]:
        """Compare results across different curiosity-driven strategies"""
        print(f"\n=== CURIOUSY-DRIVEN STRATEGIES COMPARISON ===")
        
        if not self.curiosity_strategies:
            print("No curiosity-driven exploration data collected!")
            return {}
        
        comparison = {
            "best_strategy": None,
            "worst_strategy": None,
            "total_strategies_tested": len(self.curiosity_strategies),
            "strategy_metrics": {}
        }
        
        # Score each strategy (higher is better)
        strategy_scores = {}
        for strategy in self.curiosity_strategies:
            # Combined score considering exploration efficiency and learning
            score = (
                strategy.information_gain * 0.2 +  # Information gain
                strategy.learning_efficiency * 0.3 +  # Learning efficiency
                (1.0 - strategy.reward_prediction_error) * 0.2 +  # Reward prediction accuracy
                strategy.exploration_exploitation_balance * 0.3  # Balanced exploration
            )
            strategy_scores[strategy.curiosity_type.value] = score
        
        # Find best and worst
        comparison["best_strategy"] = max(strategy_scores, key=strategy_scores.get)
        comparison["worst_strategy"] = min(strategy_scores, key=strategy_scores.get)
        
        # Detailed metrics
        for strategy in self.curiosity_strategies:
            comparison["strategy_metrics"][strategy.curiosity_type.value] = {
                "exploration_rate": strategy.exploration_rate,
                "exploitation_rate": strategy.exploitation_rate,
                "information_gain": strategy.information_gain,
                "novelty_score": strategy.novelty_score,
                "learning_efficiency": strategy.learning_efficiency,
                "reward_prediction_error": strategy.reward_prediction_error,
                "intrinsic_motivation": strategy.intrinsic_motivation,
                "uncertainty_reduction": strategy.uncertainty_reduction,
                "exploration_exploitation_balance": strategy.exploration_exploitation_balance,
                "score": strategy_scores[strategy.curiosity_type.value],
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best strategy: {comparison['best_strategy']} (score: {strategy_scores[comparison['best_strategy']]:.4f})")
        print(f"  Worst strategy: {comparison['worst_strategy']} (score: {strategy_scores[comparison['worst_strategy']]:.4f})")
        print(f"  Total strategies tested: {len(self.curiosity_strategies)}")
        
        return comparison
    
    def export_curiosity_results(self, filename: str):
        """Export curiosity-driven exploration research results"""
        print(f"\n=== EXPORTING CURIOUSY RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Curiosity-Driven Exploration Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
            },
            "curiosity_strategies": [
                {
                    "curiosity_type": strategy.curiosity_type.value,
                    "exploration_rate": strategy.exploration_rate,
                    "exploitation_rate": strategy.exploitation_rate,
                    "information_gain": strategy.information_gain,
                    "novelty_score": strategy.novelty_score,
                    "learning_efficiency": strategy.learning_efficiency,
                    "reward_prediction_error": strategy.reward_prediction_error,
                    "intrinsic_motivation": strategy.intrinsic_motivation,
                    "uncertainty_reduction": strategy.uncertainty_reduction,
                    "exploration_exploitation_balance": strategy.exploration_exploitation_balance,
                }
                for strategy in self.curiosity_strategies
            ],
            "comparison": self.compare_curiosity_driven_strategies(),
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
        
        print(f"✓ Curiosity-driven exploration results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_curiosity_driven_exploration_research(duration_per_strategy: float = 12.0):
    """Run a complete curiosity-driven exploration research study"""
    print("=== NLM CURIOUSY-DRIVEN EXPLORATION RESEARCH ===")
    
    # Initialize researcher
    researcher = CuriosityDrivenExplorationResearcher(
        neuron_count=350
    )
    
    try:
        # Run curiosity-driven exploration strategy studies
        print("\n" + "="*60)
        print("STARTING CURIOUSY-DRIVEN EXPLORATION RESEARCH")
        print("="*60)
        
        # Intrinsic motivation study
        intrinsic_metrics = researcher.simulate_intrinsic_motivation(
            duration=duration_per_strategy,
            motivation_strength=0.8
        )
        
        # Information-gap driven study
        information_gap_metrics = researcher.simulate_information_gap_driven(
            duration=duration_per_strategy,
            gap_sensitivity=1.2
        )
        
        # Novelty-seeking study
        novelty_metrics = researcher.simulate_novelty_seeking(
            duration=duration_per_strategy,
            novelty_threshold=0.6
        )
        
        # Compare results
        comparison = researcher.compare_curiosity_driven_strategies()
        
        print(f"\n" + "="*60)
        print("CURIOUSY-DRIVEN EXPLORATION RESEARCH COMPLETE")
        print("="*60)
        print(f"Total strategies tested: {len(researcher.curiosity_strategies)}")
        print(f"Best performing strategy: {comparison['best_strategy']}")
        
        # Export results
        researcher.export_curiosity_results("curiosity_driven_exploration_research.json")
        
        return researcher.curiosity_strategies, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    strategies, comparison = run_curiosity_driven_exploration_research()
    print(f"\nResearch completed successfully with {len(strategies)} curiosity-driven exploration strategies tested.")
