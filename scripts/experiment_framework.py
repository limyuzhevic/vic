#!/usr/bin/env python3
"""
Learning Experiment Framework

Structured experiment design and management for neural learning systems.

This module provides comprehensive tools for designing, executing, and analyzing
experiments for Neural Learning Machines (NLM) with support for structured
experimentation, results tracking, and statistical analysis.

Features:
- Structured experiment design
- Experiment tracking and results management
- Statistical analysis tools
- Experiment comparison tools
- Experiment visualization

Usage:
    python scripts/experiment_framework.py --design --name exploration_study --type longitudinal
    python scripts/experiment_framework.py --run --experiment my_experiment --config exp_config.json
    python scripts/experiment_framework.py --analyze --experiment my_experiment --metrics performance
"""

import sys
from pathlib import Path
import json
import copy
from typing import Dict, List, Any, Optional, Union, Tuple, Set, Callable
import numpy as np
import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt
from datetime import datetime, timedelta
from enum import Enum
import warnings
import os
import itertools
from dataclasses import dataclass, field

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm.models import BaseNeuralModel
from nlm.memory import WorkingMemory
from nlm.neuromodulation import NeuromodulationSystem


class ExperimentType(Enum):
    """Types of experiments."""
    LONGITUDINAL = "longitudinal"
    CROSS_SECTIONAL = "cross_sectional"
    INTERVENTION = "intervention"
    COMPARATIVE = "comparative"
    EXPLORATORY = "exploratory"
    REPLICATION = "replication"
    META_ANALYSIS = "meta_analysis"
    SIMULATION = "simulation"


class ExperimentStatus(Enum):
    """Status of an experiment."""
    DRAFTING = "drafting"
    PLANNING = "planning"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    CANCELLED = "cancelled"
    FAILED = "failed"


class MetricType(Enum):
    """Types of metrics."""
    PERFORMANCE = "performance"
    LEARNING_SPEED = "learning_speed"
    MEMORY_USAGE = "memory_usage"
    RESPONSE_TIME = "response_time"
    ACCURACY = "accuracy"
    ADAPTATION_RATE = "adaptation_rate"
    EXPLORATION_EFFICIENCY = "exploration_efficiency"
    EXPLORATION_BREADTH = "exploration_breadth"
    EXPLOITATION_DEPTH = "exploitation_depth"
    SURVIVAL_RATE = "survival_rate"
    COOPERATION = "cooperation"
    COMPETITION = "competition"


@dataclass
class ExperimentalCondition:
    """Experimental condition (treatment group)."""
    id: str
    name: str
    description: str
    parameters: Dict[str, Any] = field(default_factory=dict)
    config_overrides: Dict[str, Any] = field(default_factory=dict)
    participants: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ExperimentalParameter:
    """Experimental parameter (factor level)."""
    name: str
    values: List[Any]
    type: str = "continuous"  # continuous, discrete, categorical
    unit: str = ""
    constraints: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ExperimentalResult:
    """Result of a single experimental run."""
    id: str
    timestamp: datetime
    condition_id: str
    participant_id: str
    metrics: Dict[str, Any]
    raw_data: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)
    quality_score: float = 1.0
    anomalies: List[str] = field(default_factory=list)


@dataclass
class ExperimentalDesign:
    """Experimental design specification."""
    name: str
    description: str
    type: ExperimentType
    status: ExperimentStatus = ExperimentStatus.DRAFTING
    conditions: List[ExperimentalCondition] = field(default_factory=list)
    parameters: List[ExperimentalParameter] = field(default_factory=list)
    sample_size: int = 0
    duration: int = 0  # in time steps
    hypothesis: str = ""
    variables: Dict[str, Any] = field(default_factory=dict)
    controls: Dict[str, Any] = field(default_factory=dict)
    randomization: Dict[str, Any] = field(default_factory=dict)
    blocking: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


class ExperimentalDataManager:
    """Manages experimental data collection and storage."""

    def __init__(self, data_dir: str = "data/experiments"):
        """Initialize the data manager.
        
        Args:
            data_dir: Directory for storing experimental data
        """
        self.data_dir = Path(data_dir)
        self.data_dir.mkdir(parents=True, exist_ok=True)
        
        self.experiments = {}
        self.results = {}
        self.metadata = {}
        
        # Create data subdirs
        (self.data_dir / "raw").mkdir(exist_ok=True)
        (self.data_dir / "processed").mkdir(exist_ok=True)
        (self.data_dir / "analyses").mkdir(exist_ok=True)
        (self.data_dir / "visualizations").mkdir(exist_ok=True)

    def create_experiment(self, design: ExperimentalDesign) -> str:
        """Create a new experiment.
        
        Args:
            design: Experimental design
            
        Returns:
            Experiment ID
        """
        experiment_id = f"exp_{len(self.experiments) + 1:06d}"
        
        # Initialize experiment data
        self.experiments[experiment_id] = {
            "design": design,
            "created_at": datetime.now(),
            "status": design.status,
            "conditions": {cond.id: [] for cond in design.conditions},
            "participants": {},
            "results": [],
            "metadata": {}
        }
        
        return experiment_id

    def add_condition_result(self, experiment_id: str, condition_id: str, result: ExperimentalResult) -> None:
        """Add a condition result to an experiment.
        
        Args:
            experiment_id: Experiment ID
            condition_id: Condition ID
            result: Experimental result
        """
        if experiment_id not in self.experiments:
            raise ValueError(f"Experiment not found: {experiment_id}")
        
        if condition_id not in self.experiments[experiment_id]["conditions"]:
            raise ValueError(f"Condition not found: {condition_id}")
        
        # Add result
        result_id = f"res_{len(self.experiments[experiment_id]['results']) + 1:06d}"
        result.id = result_id
        
        self.experiments[experiment_id]["conditions"][condition_id].append(result)
        self.experiments[experiment_id]["results"].append(result)
        
        # Save raw result
        self._save_raw_result(experiment_id, result)

    def get_experiment_results(self, experiment_id: str) -> List[ExperimentalResult]:
        """Get all results for an experiment.
        
        Args:
            experiment_id: Experiment ID
            
        Returns:
            List of experimental results
        """
        if experiment_id not in self.experiments:
            raise ValueError(f"Experiment not found: {experiment_id}")
        
        return self.experiments[experiment_id]["results"]

    def get_condition_results(self, experiment_id: str, condition_id: str) -> List[ExperimentalResult]:
        """Get results for a specific condition.
        
        Args:
            experiment_id: Experiment ID
            condition_id: Condition ID
            
        Returns:
            List of experimental results for the condition
        """
        if experiment_id not in self.experiments:
            raise ValueError(f"Experiment not found: {experiment_id}")
        
        return self.experiments[experiment_id]["conditions"].get(condition_id, [])

    def analyze_experiment(self, experiment_id: str, metrics: Optional[List[str]] = None) -> Dict[str, Any]:
        """Analyze experiment results.
        
        Args:
            experiment_id: Experiment ID
            metrics: List of metrics to analyze
            
        Returns:
            Analysis results
        """
        if experiment_id not in self.experiments:
            raise ValueError(f"Experiment not found: {experiment_id}")
        
        results = self.get_experiment_results(experiment_id)
        if not results:
            return {"error": "No results found"}
        
        # Organize results by condition
        results_by_condition = {}
        for result in results:
            if result.condition_id not in results_by_condition:
                results_by_condition[result.condition_id] = []
            results_by_condition[result.condition_id].append(result)
        
        analysis = {
            "experiment_id": experiment_id,
            "total_runs": len(results),
            "conditions": list(results_by_condition.keys()),
            "condition_counts": {cond_id: len(cond_results) for cond_id, cond_results in results_by_condition.items()},
            "metrics_analysis": {},
            "comparisons": {},
            "quality_metrics": {}
        }
        
        # Analyze each metric
        for condition_id, condition_results in results_by_condition.items():
            analysis["metrics_analysis"][condition_id] = {}
            
            # Collect all metric values
            metric_values = {}
            for result in condition_results:
                for metric_name, metric_value in result.metrics.items():
                    if metric_name not in metric_values:
                        metric_values[metric_name] = []
                    metric_values[metric_name].append(metric_value)
            
            # Calculate statistics for each metric
            for metric_name, values in metric_values.items():
                if values:
                    # Basic statistics
                    analysis["metrics_analysis"][condition_id][metric_name] = {
                        "mean": np.mean(values),
                        "median": np.median(values),
                        "std": np.std(values),
                        "min": np.min(values),
                        "max": np.max(values),
                        "count": len(values)
                    }
        
        # Compare conditions if more than one
        if len(results_by_condition) > 1:
            analysis["comparisons"] = self._compare_conditions(results_by_condition)
        
        # Calculate quality metrics
        analysis["quality_metrics"] = self._calculate_quality_metrics(results)
        
        return analysis

    def _compare_conditions(self, results_by_condition: Dict[str, List[ExperimentalResult]]) -> Dict[str, Any]:
        """Compare results across conditions.
        
        Args:
            results_by_condition: Results organized by condition
            
        Returns:
            Comparison results
        """
        comparisons = {}
        condition_ids = list(results_by_condition.keys())
        
        if len(condition_ids) < 2:
            return comparisons
        
        # Compare each metric across conditions
        for metric_name in self._get_all_metric_names(results_by_condition):
            comparisons[metric_name] = {}
            
            # Collect values for each condition
            condition_values = {}
            for condition_id, condition_results in results_by_condition.items():
                values = []
                for result in condition_results:
                    if metric_name in result.metrics:
                        values.append(result.metrics[metric_name])
                condition_values[condition_id] = values
            
            # Perform ANOVA if possible
            if len(condition_values) >= 2:
                # Prepare data for ANOVA
                anova_data = []
                groups = []
                for condition_id, values in condition_values.items():
                    if values:
                        anova_data.extend(values)
                        groups.extend([condition_id] * len(values))
                
                if len(np.unique(groups)) >= 2 and len(anova_data) >= 4:
                    try:
                        f_stat, p_value = stats.f_oneway(*[
                            condition_values[cond_id] for cond_id in condition_ids
                            if condition_values[cond_id]
                        ])
                        comparisons[metric_name]["anova"] = {
                            "f_statistic": f_stat,
                            "p_value": p_value,
                            "significant": p_value < 0.05
                        }
                    except Exception as e:
                        comparisons[metric_name]["anova"] = {"error": str(e)}
                
                # Post-hoc tests (Tukey's HSD)
                try:
                    # Prepare data for Tukey test
                    tukey_data = pd.DataFrame({
                        "value": np.concatenate([condition_values[cond_id] for cond_id in condition_ids if condition_values[cond_id]]),
                        "group": np.concatenate([[cond_id] * len(condition_values[cond_id]) for cond_id in condition_ids if condition_values[cond_id]])
                    })
                    
                    if len(tukey_data["group"].unique()) >= 2 and len(tukey_data) >= 6:
                        tukey_result = stats.ttest_ind(
                            tukey_data[tukey_data["group"] == condition_ids[0]]["value"],
                            tukey_data[tukey_data["group"] == condition_ids[1]]["value"]
                        )
                        comparisons[metric_name]["tukey"] = {
                            "t_statistic": tukey_result.statistic,
                            "p_value": tukey_result.pvalue,
                            "significant": tukey_result.pvalue < 0.05
                        }
                except Exception as e:
                    comparisons[metric_name]["tukey"] = {"error": str(e)}
        
        return comparisons

    def _get_all_metric_names(self, results_by_condition: Dict[str, List[ExperimentalResult]]) -> List[str]:
        """Get all unique metric names across all results.
        
        Args:
            results_by_condition: Results organized by condition
            
        Returns:
            List of unique metric names
        """
        metric_names = set()
        for condition_results in results_by_condition.values():
            for result in condition_results:
                metric_names.update(result.metrics.keys())
        return list(metric_names)

    def _calculate_quality_metrics(self, results: List[ExperimentalResult]) -> Dict[str, Any]:
        """Calculate quality metrics for results.
        
        Args:
            results: List of experimental results
            
        Returns:
            Quality metrics
        """
        metrics = {
            "completeness": 0.0,
            "consistency": 0.0,
            "validity": 0.0,
            "reliability": 0.0,
            "overall_quality": 0.0
        }
        
        if not results:
            return metrics
        
        # Completeness: check if all expected metrics are present
        expected_metrics = set()
        for result in results:
            expected_metrics.update(result.metrics.keys())
        
        total_possible_metrics = len(expected_metrics)
        actual_metrics = sum(1 for result in results if result.metrics)
        metrics["completeness"] = actual_metrics / max(1, total_possible_metrics) * 100
        
        # Consistency: check for similar results across replicates
        if len(results) >= 2:
            # For each metric, calculate coefficient of variation
            cv_values = []
            for metric_name in self._get_all_metric_names({}):
                values = []
                for result in results:
                    if metric_name in result.metrics:
                        values.append(result.metrics[metric_name])
                
                if len(values) >= 2 and np.mean(values) != 0:
                    cv = np.std(values) / abs(np.mean(values))
                    cv_values.append(cv)
            
            metrics["consistency"] = max(0, 100 - (np.mean(cv_values) * 100) if cv_values else 0)
        
        # Validity: check for anomalies
        anomaly_count = sum(1 for result in results if result.anomalies)
        metrics["validity"] = max(0, 100 - (anomaly_count / len(results) * 100))
        
        # Reliability: check for reproducibility
        if len(results) >= 3:
            # Group by condition and calculate variance
            results_by_condition = {}
            for result in results:
                if result.condition_id not in results_by_condition:
                    results_by_condition[result.condition_id] = []
                results_by_condition[result.condition_id].append(result)
            
            # Calculate intra-condition variance
            variances = []
            for condition_results in results_by_condition.values():
                if len(condition_results) >= 2:
                    for metric_name in self._get_all_metric_names({}):
                        values = []
                        for result in condition_results:
                            if metric_name in result.metrics:
                                values.append(result.metrics[metric_name])
                        
                        if len(values) >= 2:
                            variance = np.var(values)
                            variances.append(variance)
            
            metrics["reliability"] = max(0, 100 - (np.mean(variances) * 100) if variances else 0)
        
        # Overall quality
        metrics["overall_quality"] = np.mean(list(metrics.values())[:-1])
        
        return metrics

    def _save_raw_result(self, experiment_id: str, result: ExperimentalResult) -> None:
        """Save raw experimental result.
        
        Args:
            experiment_id: Experiment ID
            result: Experimental result
        """
        raw_data_dir = self.data_dir / "raw" / experiment_id
        raw_data_dir.mkdir(parents=True, exist_ok=True)
        
        result_file = raw_data_dir / f"{result.id}.json"
        
        with open(result_file, 'w') as f:
            json.dump({
                "id": result.id,
                "timestamp": result.timestamp.isoformat(),
                "condition_id": result.condition_id,
                "participant_id": result.participant_id,
                "metrics": result.metrics,
                "raw_data": result.raw_data,
                "metadata": result.metadata,
                "quality_score": result.quality_score,
                "anomalies": result.anomalies
            }, f, indent=2)


class ExperimentDesigner:
    """Designer for structured experiments."""

    def __init__(self):
        """Initialize the experiment designer."""
        self.data_manager = ExperimentalDataManager()

    def design_longitudinal_experiment(
        self,
        name: str,
        description: str,
        duration: int,
        conditions: List[ExperimentalCondition],
        measurement_intervals: int = 1,
        hypothesis: str = ""
    ) -> ExperimentalDesign:
        """Design a longitudinal experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            duration: Experiment duration in time steps
            conditions: Experimental conditions
            measurement_intervals: How often to measure (every N steps)
            hypothesis: Experimental hypothesis
            
        Returns:
            Experimental design
        """
        design = ExperimentalDesign(
            name=name,
            description=description,
            type=ExperimentType.LONGITUDINAL,
            conditions=conditions,
            duration=duration,
            hypothesis=hypothesis
        )
        
        # Calculate sample size based on conditions
        total_participants = 0
        for condition in conditions:
            total_participants += len(condition.participants)
        
        design.sample_size = total_participants
        
        return design

    def design_cross_sectional_experiment(
        self,
        name: str,
        description: str,
        conditions: List[ExperimentalCondition],
        hypothesis: str = ""
    ) -> ExperimentalDesign:
        """Design a cross-sectional experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            conditions: Experimental conditions
            hypothesis: Experimental hypothesis
            
        Returns:
            Experimental design
        """
        design = ExperimentalDesign(
            name=name,
            description=description,
            type=ExperimentType.CROSS_SECTIONAL,
            conditions=conditions,
            hypothesis=hypothesis
        )
        
        # Calculate sample size based on conditions
        total_participants = 0
        for condition in conditions:
            total_participants += len(condition.participants)
        
        design.sample_size = total_participants
        
        return design

    def design_intervention_experiment(
        self,
        name: str,
        description: str,
        control_condition: ExperimentalCondition,
        treatment_conditions: List[ExperimentalCondition],
        duration: int,
        hypothesis: str = ""
    ) -> ExperimentalDesign:
        """Design an intervention experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            control_condition: Control condition
            treatment_conditions: Treatment conditions
            duration: Experiment duration in time steps
            hypothesis: Experimental hypothesis
            
        Returns:
            Experimental design
        """
        conditions = [control_condition] + treatment_conditions
        
        design = ExperimentalDesign(
            name=name,
            description=description,
            type=ExperimentType.INTERVENTION,
            conditions=conditions,
            duration=duration,
            hypothesis=hypothesis
        )
        
        # Calculate sample size
        total_participants = sum(len(condition.participants) for condition in conditions)
        design.sample_size = total_participants
        
        return design

    def randomize_participants(
        self,
        design: ExperimentalDesign,
        participants: List[str],
        random_seed: Optional[int] = None
    ) -> ExperimentalDesign:
        """Randomize participants across conditions.
        
        Args:
            design: Experimental design
            participants: List of participant IDs
            random_seed: Random seed for reproducibility
            
        Returns:
            Updated experimental design
        """
        if random_seed is not None:
            np.random.seed(random_seed)
        
        # Clear existing participants
        for condition in design.conditions:
            condition.participants.clear()
        
        # Randomize participants across conditions
        np.random.shuffle(participants)
        
        # Distribute participants
        participants_per_condition = len(participants) // len(design.conditions)
        remainder = len(participants) % len(design.conditions)
        
        start_idx = 0
        for i, condition in enumerate(design.conditions):
            # Calculate number of participants for this condition
            num_participants = participants_per_condition + (1 if i < remainder else 0)
            
            condition.participants = participants[start_idx:start_idx + num_participants]
            start_idx += num_participants
        
        design.sample_size = len(participants)
        
        return design

    def create_parameterized_experiment(
        self,
        name: str,
        description: str,
        parameters: List[ExperimentalParameter],
        base_config: Dict[str, Any],
        hypothesis: str = ""
    ) -> ExperimentalDesign:
        """Create a parameterized experiment.
        
        str
        name: Experiment name
        description: Experiment description
        parameters: Experimental parameters (factors)
        base_config: Base configuration to modify
        hypothesis: Experimental hypothesis
        
        Returns:
            Experimental design
        """
        # Generate all parameter combinations
        parameter_combinations = list(itertools.product(*[param.values for param in parameters]))
        
        conditions = []
        for i, combination in enumerate(parameter_combinations):
            # Create condition name
            condition_name = f"{name}_cond_{i}"
            
            # Create condition parameters
            condition_params = {}
            for param, value in zip(parameters, combination):
                condition_params[param.name] = value
            
            # Create config overrides
            config_overrides = copy.deepcopy(base_config)
            for param_name, value in condition_params.items():
                # Find parameter path in config
                keys = param_name.split('.')
                config = config_overrides
                for key in keys[:-1]:
                    if key not in config:
                        config[key] = {}
                    config = config[key]
                config[keys[-1]] = value
            
            # Create condition
            condition = ExperimentalCondition(
                id=f"cond_{i}",
                name=condition_name,
                description=f"Condition with parameters: {condition_params}",
                parameters=condition_params,
                config_overrides=config_overrides
            )
            
            conditions.append(condition)
        
        design = ExperimentalDesign(
            name=name,
            description=description,
            type=ExperimentType.COMPARATIVE,
            conditions=conditions,
            hypothesis=hypothesis
        )
        
        design.sample_size = len(parameter_combinations)
        
        return design


def main():
    """Example usage of ExperimentDesigner."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Learning Experiment Framework")
    parser.add_argument("--design", action="store_true", help="Design an experiment")
    parser.add_argument("--name", help="Experiment name")
    parser.add_argument("--type", dest="exp_type", choices=[t.value for t in ExperimentType],
                       help="Experiment type")
    parser.add_argument("--create", action="store_true", help="Create experiment in data manager")
    parser.add_argument("--randomize", action="store_true", help="Randomize participants")
    parser.add_argument("--participants", nargs="*", help="List of participant IDs")
    parser.add_argument("--config", help="Experiment configuration file")
    parser.add_argument("--export", help="Export experiment design to file")
    parser.add_argument("--analyze", help="Analyze experiment (experiment ID)")
    
    args = parser.parse_args()
    
    designer = ExperimentDesigner()
    
    if args.config:
        # Load experiment configuration
        with open(args.config, 'r') as f:
            config = json.load(f)
        
        # Create design from config
        if "conditions" in config:
            conditions = []
            for cond_config in config["conditions"]:
                condition = ExperimentalCondition(
                    id=cond_config["id"],
                    name=cond_config["name"],
                    description=cond_config.get("description", ""),
                    parameters=cond_config.get("parameters", {}),
                    config_overrides=cond_config.get("config_overrides", {})
                )
                
                if "participants" in cond_config:
                    condition.participants = cond_config["participants"]
                
                conditions.append(condition)
            
            design = ExperimentalDesign(
                name=config.get("name", "Experiment"),
                description=config.get("description", ""),
                type=ExperimentType(config.get("type", "comparative")),
                conditions=conditions,
                sample_size=config.get("sample_size", 0),
                duration=config.get("duration", 0),
                hypothesis=config.get("hypothesis", "")
            )
        else:
            print("Error: Configuration must contain 'conditions'")
            return
        
        # Create experiment
        experiment_id = designer.data_manager.create_experiment(design)
        print(f"✓ Experiment created with ID: {experiment_id}")
        
        # Randomize if requested
        if args.randomize and args.participants:
            design = designer.randomize_participants(design, args.participants)
            print(f"✓ Participants randomized across conditions")
            
            # Update experiment
            designer.data_manager.experiments[experiment_id]["design"] = design
        
        # Export if requested
        if args.export:
            with open(args.export, 'w') as f:
                json.dump({
                    "name": design.name,
                    "description": design.description,
                    "type": design.type.value,
                    "conditions": [{
                        "id": cond.id,
                        "name": cond.name,
                        "description": cond.description,
                        "parameters": cond.parameters,
                        "config_overrides": cond.config_overrides,
                        "participants": cond.participants
                    } for cond in design.conditions],
                    "sample_size": design.sample_size,
                    "duration": design.duration,
                    "hypothesis": design.hypothesis
                }, f, indent=2)
            
            print(f"\n✓ Experiment exported to {args.export}")
    
    elif args.design and args.name and args.exp_type:
        print(f"Designing {args.exp_type} experiment: {args.name}")
        
        # Create a simple example experiment
        condition1 = ExperimentalCondition(
            id="cond_1",
            name="Control",
            description="Control condition with default parameters",
            parameters={"learning_rate": 0.01, "curiosity_weight": 0.5},
            config_overrides={"learning_rate": 0.01, "curiosity_weight": 0.5}
        )
        
        condition2 = ExperimentalCondition(
            id="cond_2",
            name="Treatment",
            description="Treatment condition with modified parameters",
            parameters={"learning_rate": 0.05, "curiosity_weight": 0.8},
            config_overrides={"learning_rate": 0.05, "curiosity_weight": 0.8}
        )
        
        design = designer.design_intervention_experiment(
            name=args.name,
            description=f"Example {args.exp_type} experiment",
            control_condition=condition1,
            treatment_conditions=[condition2],
            duration=100,
            hypothesis="Higher learning rate and curiosity weight will improve exploration efficiency"
        )
        
        # Create experiment in data manager
        experiment_id = designer.data_manager.create_experiment(design)
        print(f"✓ Experiment designed and created with ID: {experiment_id}")
        print(f"  Type: {design.type.value}")
        print(f"  Conditions: {len(design.conditions)}")
        print(f"  Sample size: {design.sample_size}")
        print(f"  Duration: {design.duration} time steps")
        
        # Export if requested
        if args.export:
            with open(args.export, 'w') as f:
                json.dump({
                    "name": design.name,
                    "description": design.description,
                    "type": design.type.value,
                    "conditions": [{
                        "id": cond.id,
                        "name": cond.name,
                        "description": cond.description,
                        "parameters": cond.parameters,
                        "config_overrides": cond.config_overrides
                    } for cond in design.conditions],
                    "sample_size": design.sample_size,
                    "duration": design.duration,
                    "hypothesis": design.hypothesis
                }, f, indent=2)
            
            print(f"\n✓ Experiment exported to {args.export}")
    
    elif args.analyze:
        experiment_id = args.analyze
        print(f"Analyzing experiment: {experiment_id}")
        
        try:
            analysis = designer.data_manager.analyze_experiment(experiment_id)
            
            print(f"\nAnalysis results:")
            print(f"  Experiment ID: {analysis['experiment_id']}")
            print(f"  Total runs: {analysis['total_runs']}")
            print(f"  Conditions: {analysis['conditions']}")
            print(f"  Condition counts: {analysis['condition_counts']}")
            
            # Print quality metrics
            quality = analysis['quality_metrics']
            print(f"\nQuality metrics:")
            print(f"  Completeness: {quality['completeness']:.1f}%")
            print(f"  Consistency: {quality['consistency']:.1f}%")
            print(f"  Validity: {quality['validity']:.1f}%")
            print(f"  Reliability: {quality['reliability']:.1f}%")
            print(f"  Overall quality: {quality['overall_quality']:.1f}%")
            
            # Print metric statistics
            print(f"\nMetric statistics by condition:")
            for condition_id, metrics in analysis['metrics_analysis'].items():
                print(f"  Condition {condition_id}:")
                for metric_name, stats in metrics.items():
                    print(f"    {metric_name}: mean={stats['mean']:.3f}, std={stats['std']:.3f}, count={stats['count']}")
            
            # Print comparisons
            if analysis['comparisons']:
                print(f"\nStatistical comparisons:")
                for metric_name, comparison in analysis['comparisons'].items():
                    print(f"  {metric_name}:")
                    if 'anova' in comparison:
                        anova = comparison['anova']
                        print(f"    ANOVA: p={anova['p_value']:.4f}, significant={anova['significant']}")
                    if 'tukey' in comparison:
                        tukey = comparison['tukey']
                        print(f"    Tukey: p={tukey['p_value']:.4f}, significant={tukey['significant']}")
        
        except Exception as e:
            print(f"Error analyzing experiment: {e}")


if __name__ == "__main__":
    main()
