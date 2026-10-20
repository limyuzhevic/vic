#!/usr/bin/env python3
"""
NLM Research Examples - Attention and Cognition

This module demonstrates research on attentional selection, planning, and self-modeling
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

class CognitiveTaskType(Enum):
    """Types of cognitive tasks"""
    ATTENTIONAL_SELECTION = "attentional_selection"
    PLANNING = "planning"
    SELF_MODELING = "self_modeling"

@dataclass
class CognitiveMetrics:
    """Metrics for cognitive function studies"""
    task_type: CognitiveTaskType
    task_difficulty: float
    success_rate: float
    reaction_time: float
    attentional_focus: float
    planning_efficiency: float
    working_memory_load: float
    cognitive_flexibility: float
    decision_confidence: float
    self_awareness_level: float

class CognitiveFunctionResearcher:
    """Researcher conducting cognitive function studies"""
    
    def __init__(self, neuron_count: int = 500):
        self.neuron_count = neuron_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Cognitive function tracking
        self.cognitive_tasks = []
        self.attention_weights = []
        self.planning_strategies = []
        self.self_models = {}
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for cognitive research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.HEAT_MAP)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_attentional_selection(self, duration: float, task_difficulty: float = 0.5) -> CognitiveMetrics:
        """Simulate attentional selection and focused attention"""
        print(f"\n=== ATTENTIONAL SELECTION STUDY ===")
        print(f"Simulating attentional selection with difficulty: {task_difficulty}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Attentional selection setup
        attentional_focus = np.random.beta(0.5, 2.0)  # Initial focus level
        task_success_rate = 0.0
        total_attempts = 0
        attentional_shifts = 0
        
        # Track attentional dynamics
        focus_volatility = 0.0
        distraction_events = 0
        sustained_attention_time = 0.0
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Generate task stimulus
            task_appeared = np.random.random() < 0.15  # 15% chance of new task
            if task_appeared:
                # Update attentional focus based on task difficulty
                attentional_focus = np.clip(
                    attentional_focus + (task_difficulty - attentional_focus) * 0.1 + np.random.normal(0, 0.05),
                    0.1, 1.0
                )
            
            # Sustained attention tracking
            if attentional_focus > 0.7:
                sustained_attention_time += 1.0 / 30.0
            
            # Distraction and attentional shifts
            if np.random.random() < 0.05:  # 5% chance of distraction
                distraction_events += 1
                # Attentional shift
                attentional_shift = np.random.beta(0.3, 2.0)
                attentional_focus *= (1.0 - attentional_shift)
                attentional_shifts += 1
            
            # Task performance based on attentional focus
            if np.random.random() < attentional_focus:
                task_success_rate += 1
            total_attempts += 1
            
            # Calculate attentional volatility
            focus_volatility = np.std([attentional_focus] * min(10, step + 1))
            
            # Update visualization
            self.visualization.add_time_series_metric("attentional_focus", attentional_focus)
            self.visualization.add_time_series_metric("success_rate", 
                                                     task_success_rate / max(1, total_attempts))
            self.visualization.add_time_series_metric("attentional_shifts", attentional_shifts)
            self.visualization.add_time_series_metric("focus_volatility", focus_volatility)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="AttentionalSelection",
                    current_state="Active",
                    warnings=[f"Focus: {attentional_focus:.3f}, Success: {task_success_rate/max(1,total_attempts):.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate cognitive metrics
        final_success_rate = task_success_rate / max(1, total_attempts)
        average_reaction_time = 1.0 / max(1, attentional_focus) * 100  # milliseconds
        
        # Attentional focus efficiency
        attentional_focus_efficiency = attentional_focus * (1.0 - focus_volatility)
        
        # Working memory load (inverse of attentional focus)
        working_memory_load = 1.0 - attentional_focus
        
        # Cognitive flexibility (ability to shift attention)
        cognitive_flexibility = min(1.0, attentional_shifts / max(1, duration * 10))
        
        # Decision confidence (related to attentional stability)
        decision_confidence = attentional_focus * (1.0 - focus_volatility)
        
        metrics = CognitiveMetrics(
            task_type=CognitiveTaskType.ATTENTIONAL_SELECTION,
            task_difficulty=task_difficulty,
            success_rate=final_success_rate,
            reaction_time=average_reaction_time,
            attentional_focus=attentional_focus,
            planning_efficiency=0.0,  # Not applicable for this task
            working_memory_load=working_memory_load,
            cognitive_flexibility=cognitive_flexibility,
            decision_confidence=decision_confidence,
            self_awareness_level=0.0  # Not applicable for this task
        )
        
        self.cognitive_tasks.append(metrics)
        
        print(f"✓ Attentional selection complete: focus {attentional_focus:.3f}")
        print(f"  Success rate: {final_success_rate:.3f}")
        print(f"  Sustained attention: {sustained_attention_time:.1f}s")
        print(f"  Attentional shifts: {attentional_shifts}")
        
        return metrics
    
    def simulate_planning(self, duration: float, planning_complexity: float = 3.0) -> CognitiveMetrics:
        """Simulate planning and sequential decision making"""
        print(f"\n=== PLANNING STUDY ===")
        print(f"Simulating planning with complexity: {planning_complexity}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Planning setup
        planning_efficiency = np.random.beta(0.5, 2.0)
        plan_formation_success = 0.0
        plan_execution_success = 0.0
        total_planning_attempts = 0
        planning_iterations = 0
        
        # Track planning dynamics
        plan_quality = []
        execution_success_rate = []
        replanning_events = 0
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Plan formation
            if np.random.random() < 0.2:  # 20% chance of planning attempt
                total_planning_attempts += 1
                planning_iterations += 1
                
                # Plan quality based on complexity and efficiency
                base_quality = np.random.beta(0.5, 2.0)
                quality = base_quality * planning_efficiency / planning_complexity
                plan_quality.append(quality)
                
                # Successful plan formation
                if quality > 0.4:
                    plan_formation_success += 1
            
            # Plan execution
            if np.random.random() < 0.15:  # 15% chance of execution attempt
                # Execution success based on plan quality
                current_plan_quality = np.random.choice(plan_quality) if plan_quality else 0.5
                execution_success = current_plan_quality * (1.0 - planning_complexity * 0.1)
                
                if np.random.random() < execution_success:
                    plan_execution_success += 1
                
                execution_success_rate.append(1.0 if execution_success else 0.0)
                
                # Replanning if execution fails
                if execution_success <= 0.3:
                    replanning_events += 1
                    # Update planning efficiency based on replanning
                    planning_efficiency = min(1.0, planning_efficiency + 0.05)
            
            # Update visualization
            self.visualization.add_time_series_metric("planning_efficiency", planning_efficiency)
            self.visualization.add_time_series_metric("plan_formation_success_rate",
                                                     plan_formation_success / max(1, total_planning_attempts))
            self.visualization.add_time_series_metric("plan_execution_success_rate",
                                                     plan_execution_success / max(1, (total_planning_attempts + step)))
            self.visualization.add_time_series_metric("replanning_events", replanning_events)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="Planning",
                    current_state="Active",
                    warnings=[f"Efficiency: {planning_efficiency:.3f}, Formation: {plan_formation_success/max(1,total_planning_attempts):.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate cognitive metrics
        average_success_rate = plan_formation_success / max(1, total_planning_attempts)
        
        # Planning efficiency (inverse of replanning events)
        planning_efficiency_metric = planning_efficiency * (1.0 - replanning_events / max(1, steps / 30.0))
        
        # Working memory load (planning requires memory)
        working_memory_load = min(1.0, planning_complexity * 0.1)
        
        # Cognitive flexibility (ability to adapt plans)
        cognitive_flexibility = 1.0 - (replanning_events / max(1, steps / 30.0))
        
        # Decision confidence (plan quality)
        decision_confidence = np.mean(plan_quality) if plan_quality else 0.5
        
        # Self-awareness level (not applicable for planning)
        self_awareness_level = 0.0
        
        metrics = CognitiveMetrics(
            task_type=CognitiveTaskType.PLANNING,
            task_difficulty=planning_complexity,
            success_rate=average_success_rate,
            reaction_time=100.0 + planning_complexity * 50,  # milliseconds
            attentional_focus=planning_efficiency_metric,
            planning_efficiency=planning_efficiency_metric,
            working_memory_load=working_memory_load,
            cognitive_flexibility=cognitive_flexibility,
            decision_confidence=decision_confidence,
            self_awareness_level=self_awareness_level
        )
        
        self.cognitive_tasks.append(metrics)
        
        print(f"✓ Planning complete: efficiency {planning_efficiency_metric:.3f}")
        print(f"  Success rate: {average_success_rate:.3f}")
        print(f"  Planning iterations: {planning_iterations}")
        print(f"  Replanning events: {replanning_events}")
        
        return metrics
    
    def simulate_self_modeling(self, duration: float, self_model_complexity: float = 0.7) -> CognitiveMetrics:
        """Simulate self-modeling and metacognition"""
        print(f"\n=== SELF-MODELING STUDY ===")
        print(f"Simulating self-modeling with complexity: {self_model_complexity}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Self-modeling setup
        self_awareness = np.random.beta(0.5, 2.0)
        metacognitive_accuracy = 0.0
        total_self_assessments = 0
        prediction_errors = []
        calibration_scores = []
        
        # Track self-modeling dynamics
        confidence_calibration = 0.0
        prediction_consistency = 0.0
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Self-assessment
            if np.random.random() < 0.1:  # 10% chance of self-assessment
                total_self_assessments += 1
                
                # Estimate performance
                estimated_performance = self_awareness + np.random.normal(0, 0.1)
                estimated_performance = np.clip(estimated_performance, 0.0, 1.0)
                
                # Actual performance
                actual_performance = self_model_complexity + np.random.normal(0, 0.15)
                actual_performance = np.clip(actual_performance, 0.0, 1.0)
                
                # Metacognitive accuracy
                metacognitive_accuracy += 1.0 if abs(estimated_performance - actual_performance) < 0.2 else 0.0
                
                # Calibration score
                calibration = 1.0 - abs(estimated_performance - actual_performance)
                calibration_scores.append(calibration)
                
                # Prediction error
                prediction_error = abs(estimated_performance - actual_performance)
                prediction_errors.append(prediction_error)
                
                # Update self-awareness based on metacognitive feedback
                self_awareness = (self_awareness + calibration) / 2.0
            
            # Update visualization
            self.visualization.add_time_series_metric("self_awareness", self_awareness)
            self.visualization.add_time_series_metric("metacognitive_accuracy",
                                                     metacognitive_accuracy / max(1, total_self_assessments))
            self.visualization.add_time_series_metric("calibration_score",
                                                     np.mean(calibration_scores) if calibration_scores else 0.0)
            self.visualization.add_time_series_metric("prediction_error",
                                                     np.mean(prediction_errors) if prediction_errors else 0.0)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="SelfModeling",
                    current_state="Active",
                    warnings=[f"Awareness: {self_awareness:.3f}, Accuracy: {metacognitive_accuracy/max(1,total_self_assessments):.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate cognitive metrics
        final_metacognitive_accuracy = metacognitive_accuracy / max(1, total_self_assessments)
        
        # Confidence calibration
        confidence_calibration = np.mean(calibration_scores) if calibration_scores else 0.0
        
        # Prediction consistency
        prediction_consistency = 1.0 - (np.std(prediction_errors) / max(1, np.mean(prediction_errors)))
        
        # Success rate (simplified)
        success_rate = (self_awareness + final_metacognitive_accuracy) / 2.0
        
        # Reaction time (increases with higher awareness)
        reaction_time = 100.0 + self_awareness * 100.0
        
        # Attentional focus (related to awareness)
        attentional_focus = self_awareness
        
        # Planning efficiency (self-awareness helps planning)
        planning_efficiency = self_awareness * 0.8
        
        # Working memory load
        working_memory_load = 1.0 - self_awareness * 0.5
        
        # Cognitive flexibility (awareness enables flexibility)
        cognitive_flexibility = self_awareness * 0.9
        
        # Decision confidence (related to awareness and calibration)
        decision_confidence = self_awareness * confidence_calibration
        
        # Self-awareness level (the main metric)
        self_awareness_level = self_awareness
        
        metrics = CognitiveMetrics(
            task_type=CognitiveTaskType.SELF_MODELING,
            task_difficulty=self_model_complexity,
            success_rate=success_rate,
            reaction_time=reaction_time,
            attentional_focus=attentional_focus,
            planning_efficiency=planning_efficiency,
            working_memory_load=working_memory_load,
            cognitive_flexibility=cognitive_flexibility,
            decision_confidence=decision_confidence,
            self_awareness_level=self_awareness_level
        )
        
        self.cognitive_tasks.append(metrics)
        
        print(f"✓ Self-modeling complete: awareness {self_awareness_level:.3f}")
        print(f"  Metacognitive accuracy: {final_metacognitive_accuracy:.3f}")
        print(f"  Calibration score: {confidence_calibration:.3f}")
        print(f"  Prediction consistency: {prediction_consistency:.3f}")
        
        return metrics
    
    def compare_cognitive_tasks(self) -> Dict[str, Any]:
        """Compare results across different cognitive tasks"""
        print(f"\n=== COGNITIVE TASKS COMPARISON ===")
        
        if not self.cognitive_tasks:
            print("No cognitive task data collected!")
            return {}
        
        comparison = {
            "best_task": None,
            "worst_task": None,
            "total_tasks_tested": len(self.cognitive_tasks),
            "task_metrics": {}
        }
        
        # Score each task (higher is better)
        task_scores = {}
        for task in self.cognitive_tasks:
            # Combined score considering performance and efficiency
            score = (
                task.success_rate * 0.2 +  # Task success
                (1.0 - task.reaction_time / 500.0) * 0.2 +  # Reaction speed
                task.attentional_focus * 0.2 +  # Attentional focus
                task.planning_efficiency * 0.2 +  # Planning efficiency
                task.cognitive_flexibility * 0.2  # Cognitive flexibility
            )
            task_scores[task.task_type.value] = score
        
        # Find best and worst
        comparison["best_task"] = max(task_scores, key=task_scores.get)
        comparison["worst_task"] = min(task_scores, key=task_scores.get)
        
        # Detailed metrics
        for task in self.cognitive_tasks:
            comparison["task_metrics"][task.task_type.value] = {
                "task_difficulty": task.task_difficulty,
                "success_rate": task.success_rate,
                "reaction_time": task.reaction_time,
                "attentional_focus": task.attentional_focus,
                "planning_efficiency": task.planning_efficiency,
                "working_memory_load": task.working_memory_load,
                "cognitive_flexibility": task.cognitive_flexibility,
                "decision_confidence": task.decision_confidence,
                "self_awareness_level": task.self_awareness_level,
                "score": task_scores[task.task_type.value],
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best task: {comparison['best_task']} (score: {task_scores[comparison['best_task']]:.4f})")
        print(f"  Worst task: {comparison['worst_task']} (score: {task_scores[comparison['worst_task']]:.4f})")
        print(f"  Total tasks tested: {len(self.cognitive_tasks)}")
        
        return comparison
    
    def export_cognitive_results(self, filename: str):
        """Export cognitive function research results"""
        print(f"\n=== EXPORTING COGNITIVE RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Cognitive Function Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
            },
            "cognitive_tasks": [
                {
                    "task_type": task.task_type.value,
                    "task_difficulty": task.task_difficulty,
                    "success_rate": task.success_rate,
                    "reaction_time": task.reaction_time,
                    "attentional_focus": task.attentional_focus,
                    "planning_efficiency": task.planning_efficiency,
                    "working_memory_load": task.working_memory_load,
                    "cognitive_flexibility": task.cognitive_flexibility,
                    "decision_confidence": task.decision_confidence,
                    "self_awareness_level": task.self_awareness_level,
                }
                for task in self.cognitive_tasks
            ],
            "comparison": self.compare_cognitive_tasks(),
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
        
        print(f"✓ Cognitive results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_cognitive_function_research(duration_per_task: float = 10.0):
    """Run a complete cognitive function research study"""
    print("=== NLM COGNITIVE FUNCTION RESEARCH ===")
    
    # Initialize researcher
    researcher = CognitiveFunctionResearcher(
        neuron_count=500
    )
    
    try:
        # Run cognitive task studies
        print("\n" + "="*60)
        print("STARTING COGNITIVE FUNCTION RESEARCH")
        print("="*60)
        
        # Attentional selection study
        attentional_metrics = researcher.simulate_attentional_selection(
            duration=duration_per_task,
            task_difficulty=0.5
        )
        
        # Planning study
        planning_metrics = researcher.simulate_planning(
            duration=duration_per_task,
            planning_complexity=3.0
        )
        
        # Self-modeling study
        selfmodel_metrics = researcher.simulate_self_modeling(
            duration=duration_per_task,
            self_model_complexity=0.7
        )
        
        # Compare results
        comparison = researcher.compare_cognitive_tasks()
        
        print(f"\n" + "="*60)
        print("COGNITIVE FUNCTION RESEARCH COMPLETE")
        print("="*60)
        print(f"Total tasks tested: {len(researcher.cognitive_tasks)}")
        print(f"Best performing task: {comparison['best_task']}")
        
        # Export results
        researcher.export_cognitive_results("cognitive_function_research.json")
        
        return researcher.cognitive_tasks, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    tasks, comparison = run_cognitive_function_research()
    print(f"\nResearch completed successfully with {len(tasks)} cognitive tasks tested.")
