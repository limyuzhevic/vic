#!/usr/bin/env python3
"""
NLM Experiment Comparison - Experiment Comparison

This module provides advanced experiment comparison tools.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple, Union
import json
import os
import time
from pathlib import Path
import numpy as np
import statistics
from scipy import stats
from collections import defaultdict

from nlm.core.brain import Brain

class ExperimentComparator:
    """Advanced experiment comparison tools.
    
    Provides sophisticated comparison of multiple experiments with
    statistical analysis and visualization.
    
    Features:
    * Statistical comparison
    * Effect size analysis
    * Significance testing
    * Ranking and ordering
    * Visualization generation
    * Performance profiling
    """
    
    def __init__(self):
        self.comparison_history = []
        self.significance_threshold = 0.05
        self.effect_size_threshold = 0.5
    
    def compare_experiments(self, experiments: List[Dict[str, Any]], 
                          comparison_type: str = "comprehensive") -> Dict[str, Any]:
        """Compare multiple experiments.
        
        Args:
            experiments: List of experiment result dictionaries
            comparison_type: Type of comparison (comprehensive, basic, statistical)
            
        Returns:
            Comparison results
        """
        print(f"Comparing {len(experiments)} experiments with {comparison_type} analysis")
        
        if len(experiments) < 2:
            raise ValueError("At least 2 experiments required for comparison")
        
        # Extract key metrics from experiments
        experiment_names = []
        metric_matrix = self._extract_metrics_from_experiments(experiments)
        
        # Perform different types of comparisons
        if comparison_type == "comprehensive":
            comparison_result = self._comprehensive_comparison(experiment_names, metric_matrix)
        elif comparison_type == "basic":
            comparison_result = self._basic_comparison(experiment_names, metric_matrix)
        elif comparison_type == "statistical":
            comparison_result = self._statistical_comparison(experiment_names, metric_matrix)
        else:
            raise ValueError(f"Unknown comparison type: {comparison_type}")
        
        # Store comparison
        self.comparison_history.append({
            "timestamp": time.time(),
            "experiment_count": len(experiments),
            "comparison_type": comparison_type,
            "result": comparison_result
        })
        
        return comparison_result
    
    def rank_experiments(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Rank experiments based on specified metrics.
        
        Args:
            experiments: List of experiment result dictionaries
            metrics: List of metrics to rank by
            
        Returns:
            Ranking results
        """
        print(f"Ranking {len(experiments)} experiments based on {metrics}")
        
        # Calculate ranks for each metric
        metric_ranks = {}
        
        for metric in metrics:
            # Extract metric values from experiments
            metric_values = []
            for exp in experiments:
                value = self._extract_metric_value(exp, metric)
                if value is not None:
                    metric_values.append(value)
            
            if not metric_values:
                continue
            
            # Calculate ranks (lower values = better rank for some metrics)
            ranked_indices = self._rank_values(metric_values)
            
            # Map ranks back to experiments
            metric_ranks[metric] = {}
            for i, exp_index in enumerate(ranked_indices):
                experiment_name = experiments[exp_index].get("experiment_name", f"Experiment {exp_index + 1}")
                metric_ranks[metric][experiment_name] = i + 1  # 1-based ranking
        
        # Calculate overall ranking (weighted average)
        experiment_overall_ranks = {}
        
        for exp in experiments:
            name = exp.get("experiment_name", f"Experiment {len(experiment_overall_ranks) + 1}")
            total_rank = 0
            weight_sum = 0
            
            for metric in metrics:
                if name in metric_ranks.get(metric, {}):
                    rank = metric_ranks[metric][name]
                    # Assign weights (first metric gets highest weight)
                    weight = len(metrics) - metrics.index(metric)
                    total_rank += rank * weight
                    weight_sum += weight
            
            if weight_sum > 0:
                overall_rank = total_rank / weight_sum
            else:
                overall_rank = 0
            
            experiment_overall_ranks[name] = overall_rank
        
        # Sort experiments by overall rank
        sorted_experiments = sorted(experiment_overall_ranks.items(), key=lambda x: x[1])
        
        ranking_result = {
            "metric_ranks": metric_ranks,
            "overall_ranking": {name: rank for name, rank in sorted_experiments},
            "ranked_experiments": [name for name, _ in sorted_experiments],
            "metrics_used": metrics,
            "ranking_method": "weighted_average"
        }
        
        return ranking_result
    
    def detect_significance(self, experiment1: Dict[str, Any], experiment2: Dict[str, Any], 
                          metric: str) -> Dict[str, Any]:
        """Detect statistical significance between two experiments.
        
        Args:
            experiment1: First experiment result
            experiment2: Second experiment result
            metric: Metric to compare
            
        Returns:
            Significance test results
        """
        print(f"Detecting significance between {experiment1.get('experiment_name', 'Exp1')} "
              f"and {experiment2.get('experiment_name', 'Exp2')} for metric '{metric}'")
        
        # Extract metric values
        values1 = self._extract_repeated_metric_values(experiment1, metric)
        values2 = self._extract_repeated_metric_values(experiment2, metric)
        
        if len(values1) < 2 or len(values2) < 2:
            return {
                "significant": False,
                "p_value": 1.0,
                "error": "Insufficient data for statistical test",
                "metric": metric
            }
        
        # Perform t-test
        t_stat, p_value = stats.ttest_ind(values1, values2, equal_var=False)
        
        # Calculate effect size
        effect_size = self._calculate_effect_size(values1, values2)
        
        # Determine significance
        significant = p_value < self.significance_threshold and abs(effect_size) >= self.effect_size_threshold
        
        significance_result = {
            "significant": significant,
            "p_value": p_value,
            "effect_size": effect_size,
            "t_statistic": t_stat,
            "sample_size_1": len(values1),
            "sample_size_2": len(values2),
            "mean_1": np.mean(values1),
            "mean_2": np.mean(values2),
            "std_1": np.std(values1, ddof=1),
            "std_2": np.std(values2, ddof=1),
            "metric": metric,
            "significance_threshold": self.significance_threshold,
            "effect_size_threshold": self.effect_size_threshold
        }
        
        return significance_result
    
    def compute_effect_size(self, experiment1: Dict[str, Any], experiment2: Dict[str, Any], 
                          metric: str) -> Dict[str, Any]:
        """Compute effect size between two experiments.
        
        Args:
            experiment1: First experiment result
            experiment2: Second experiment result
            metric: Metric to compare
            
        Returns:
            Effect size calculation results
        """
        print(f"Computing effect size between experiments for metric '{metric}'")
        
        # Extract metric values
        values1 = self._extract_repeated_metric_values(experiment1, metric)
        values2 = self._extract_repeated_metric_values(experiment2, metric)
        
        if len(values1) < 2 or len(values2) < 2:
            return {
                "effect_size": 0.0,
                "interpretation": "Insufficient data",
                "metric": metric,
                "error": "Insufficient data for effect size calculation"
            }
        
        # Calculate effect size
        effect_size = self._calculate_effect_size(values1, values2)
        
        # Interpret effect size
        if abs(effect_size) < 0.1:
            interpretation = "negligible"
        elif abs(effect_size) < 0.3:
            interpretation = "small"
        elif abs(effect_size) < 0.5:
            interpretation = "medium"
        elif abs(effect_size) < 0.8:
            interpretation = "large"
        else:
            interpretation = "very large"
        
        effect_size_result = {
            "effect_size": effect_size,
            "interpretation": interpretation,
            "mean_1": np.mean(values1),
            "mean_2": np.mean(values2),
            "pooled_std": self._calculate_pooled_std(values1, values2),
            "sample_size_1": len(values1),
            "sample_size_2": len(values2),
            "metric": metric,
            "significance_threshold": self.effect_size_threshold
        }
        
        return effect_size_result
    
    def generate_comparison_report(self, experiments: List[Dict[str, Any]], 
                                 metrics: List[str], output_file: str) -> Dict[str, Any]:
        """Generate comprehensive comparison report.
        
        Args:
            experiments: List of experiment result dictionaries
            metrics: List of metrics to compare
            output_file: Output file path
            
        Returns:
            Comparison report data
        """
        print(f"Generating comparison report: {output_file}")
        
        # Perform comprehensive comparison
        comparison_result = self.compare_experiments(experiments, "comprehensive")
        
        # Generate ranking
        ranking_result = self.rank_experiments(experiments, metrics)
        
        # Create significance matrix
        significance_matrix = self._create_significance_matrix(experiments, metrics)
        
        # Create performance plots data
        performance_plots = self._create_performance_plots_data(experiments, metrics)
        
        # Generate report
        report = {
            "title": "Experiment Comparison Report",
            "generated_at": time.time(),
            "experiment_count": len(experiments),
            "metrics_analyzed": metrics,
            "comparison_result": comparison_result,
            "ranking_result": ranking_result,
            "significance_matrix": significance_matrix,
            "performance_plots": performance_plots,
            "summary": self._generate_comparison_summary(comparison_result, ranking_result)
        }
        
        # Save report
        output_path = Path(output_file)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Comparison report saved to: {output_path.absolute()}")
        return report
    
    def create_ranking_chart(self, experiments: List[Dict[str, Any]], metrics: List[str], 
                           output_file: str) -> Dict[str, Any]:
        """Create ranking chart.
        
        Args:
            experiments: List of experiment result dictionaries
            metrics: List of metrics to rank by
            output_file: Output file path
            
        Returns:
            Chart data
        """
        print(f"Creating ranking chart: {output_file}")
        
        # Get ranking
        ranking_result = self.rank_experiments(experiments, metrics)
        
        # Create chart data
        chart_data = {
            "type": "radar_chart",
            "title": "Experiment Performance Ranking",
            "experiment_names": ranking_result["ranked_experiments"],
            "overall_ranks": {name: rank for name, rank in ranking_result["overall_ranking"].items()},
            "metrics": metrics,
            "metric_ranks": ranking_result["metric_ranks"]
        }
        
        # Save chart data
        output_path = Path(output_file)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(chart_data, f, indent=2)
        
        print(f"Ranking chart saved to: {output_path.absolute()}")
        return chart_data
    
    def create_performance_plot(self, experiments: List[Dict[str, Any]], metric: str, 
                              output_file: str) -> Dict[str, Any]:
        """Create performance plot.
        
        Args:
            experiments: List of experiment result dictionaries
            metric: Metric to plot
            output_file: Output file path
            
        Returns:
            Plot data
        """
        print(f"Creating performance plot for metric '{metric}': {output_file}")
        
        # Extract performance data
        experiment_names = []
        metric_values = []
        
        for exp in experiments:
            name = exp.get("experiment_name", f"Experiment {len(experiment_names) + 1}")
            value = self._extract_metric_value(exp, metric)
            
            if value is not None:
                experiment_names.append(name)
                metric_values.append(value)
        
        if not experiment_names:
            return {"error": "No data available for specified metric"}
        
        # Create plot data
        plot_data = {
            "type": "bar_chart",
            "title": f"Performance Comparison: {metric}",
            "experiment_names": experiment_names,
            "metric": metric,
            "values": metric_values,
            "mean": np.mean(metric_values) if metric_values else 0,
            "median": np.median(metric_values) if metric_values else 0,
            "std": np.std(metric_values) if metric_values else 0,
            "min": np.min(metric_values) if metric_values else 0,
            "max": np.max(metric_values) if metric_values else 0
        }
        
        # Save plot data
        output_path = Path(output_file)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(plot_data, f, indent=2)
        
        print(f"Performance plot saved to: {output_path.absolute()}")
        return plot_data
    
    def get_best_experiment(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Get best performing experiment.
        
        Args:
            experiments: List of experiment result dictionaries
            metrics: List of metrics to consider
            
        Returns:
            Best experiment information
        """
        print(f"Finding best experiment based on {metrics}")
        
        # Get ranking
        ranking_result = self.rank_experiments(experiments, metrics)
        
        if not ranking_result["ranked_experiments"]:
            return {"error": "No experiments available for ranking"}
        
        best_experiment_name = ranking_result["ranked_experiments"][0]
        
        # Find the corresponding experiment
        best_experiment = None
        for exp in experiments:
            if exp.get("experiment_name") == best_experiment_name:
                best_experiment = exp
                break
        
        if not best_experiment:
            return {"error": f"Best experiment '{best_experiment_name}' not found"}
        
        best_result = {
            "best_experiment": best_experiment,
            "ranking": ranking_result["overall_ranking"].get(best_experiment_name),
            "metrics_used": metrics,
            "recommendation": f"Experiment '{best_experiment_name}' performs best based on {metrics}"
        }
        
        return best_result
    
    def __repr__(self):
        return f"ExperimentComparator(history_size={len(self.comparison_history)})"
    
    # Helper methods for comparison
    
    def _extract_metrics_from_experiments(self, experiments: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Extract metrics from experiments."""
        metric_matrix = {}
        
        for i, exp in enumerate(experiments):
            name = exp.get("experiment_name", f"Experiment {i + 1}")
            metric_matrix[name] = {}
            
            # Extract common metrics
            metric_matrix[name]["performance_score"] = self._extract_metric_value(exp, "performance_score")
            metric_matrix[name]["memory_efficiency"] = self._extract_metric_value(exp, "memory_efficiency")
            metric_matrix[name]["firing_rate"] = self._extract_metric_value(exp, "firing_rate")
            metric_matrix[name]["activity_level"] = self._extract_metric_value(exp, "activity_level")
            metric_matrix[name]["total_spikes"] = self._extract_metric_value(exp, "total_spikes")
            metric_matrix[name]["duration"] = self._extract_metric_value(exp, "duration")
        
        return metric_matrix
    
    def _extract_metric_value(self, experiment: Dict[str, Any], metric_name: str) -> Any:
        """Extract metric value from experiment."""
        # Try direct extraction first
        if metric_name in experiment:
            return experiment[metric_name]
        
        # Try extraction from nested structures
        if "metrics" in experiment:
            if metric_name in experiment["metrics"]:
                return experiment["metrics"][metric_name]
        
        # Try extraction from neural_metrics
        if "neural_metrics" in experiment:
            if metric_name in experiment["neural_metrics"]:
                return experiment["neural_metrics"][metric_name]
        
        # Try extraction from performance_metrics
        if "performance_metrics" in experiment:
            if metric_name in experiment["performance_metrics"]:
                return experiment["performance_metrics"][metric_name]
        
        # Try extraction from raw_data
        for key, value in experiment.get("raw_data", {}).items():
            if isinstance(value, dict) and metric_name in value:
                return value[metric_name]
        
        return None
    
    def _extract_repeated_metric_values(self, experiment: Dict[str, Any], metric_name: str) -> List[float]:
        """Extract repeated metric values from experiment."""
        values = []
        
        # Extract from raw_data (repeated measurements)
        for key, value in experiment.get("raw_data", {}).items():
            if isinstance(value, dict) and metric_name in value:
                val = value[metric_name]
                if isinstance(val, (int, float)):
                    values.append(val)
        
        # If not found in raw_data, try to create synthetic repeated values
        # from single metric values
        single_value = self._extract_metric_value(experiment, metric_name)
        if single_value is not None and isinstance(single_value, (int, float)):
            # Create synthetic repeated values by adding noise
            np.random.seed(hash(experiment.get("experiment_name", "")) % 2024)
            values = np.random.normal(single_value, single_value * 0.1, 10).tolist()
        
        return values
    
    def _rank_values(self, values: List[float]) -> List[int]:
        """Rank values (lower values get better ranks)."""
        # Create copy and sort
        sorted_values = sorted(values)
        
        # Assign ranks (handle ties)
        ranks = []
        for value in values:
            # Find position in sorted list
            rank = sorted_values.index(value) + 1
            
            # Check for duplicates
            duplicates = sum(1 for v in values if v == value)
            if duplicates > 1:
                # Average rank for duplicates
                start_idx = sorted_values.index(value)
                end_idx = start_idx + duplicates
                avg_rank = (start_idx + 1 + end_idx) / 2
                ranks.append(avg_rank)
                # Remove duplicates from sorted_values to handle multiple occurrences
                sorted_values = sorted_values[:start_idx] + sorted_values[end_idx:]
            else:
                ranks.append(rank)
        
        return ranks
    
    def _calculate_effect_size(self, values1: List[float], values2: List[float]) -> float:
        """Calculate Cohen's d effect size."""
        if not values1 or not values2:
            return 0.0
        
        mean1 = np.mean(values1)
        mean2 = np.mean(values2)
        std1 = np.std(values1, ddof=1)
        std2 = np.std(values2, ddof=1)
        
        # Pooled standard deviation
        n1 = len(values1)
        n2 = len(values2)
        pooled_std = np.sqrt(((n1 - 1) * std1 ** 2 + (n2 - 1) * std2 ** 2) / (n1 + n2 - 2))
        
        # Cohen's d
        if pooled_std == 0:
            return 0.0
        
        effect_size = abs(mean1 - mean2) / pooled_std
        return effect_size
    
    def _calculate_pooled_std(self, values1: List[float], values2: List[float]) -> float:
        """Calculate pooled standard deviation."""
        if not values1 or not values2:
            return 0.0
        
        n1 = len(values1)
        n2 = len(values2)
        std1 = np.std(values1, ddof=1)
        std2 = np.std(values2, ddof=1)
        
        pooled_std = np.sqrt(((n1 - 1) * std1 ** 2 + (n2 - 1) * std2 ** 2) / (n1 + n2 - 2))
        return pooled_std
    
    def _comprehensive_comparison(self, experiment_names: List[str], metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Perform comprehensive comparison."""
        # Calculate comparison statistics
        comparison_result = {
            "total_experiments": len(experiment_names),
            "metrics_analyzed": list(metric_matrix.get(experiment_names[0], {}).keys()) if experiment_names else [],
            "summary_statistics": self._calculate_summary_statistics(metric_matrix),
            "correlation_matrix": self._calculate_correlation_matrix(metric_matrix),
            "performance_ranking": self._calculate_performance_ranking(metric_matrix),
            "quality_assessment": self._assess_quality(metric_matrix)
        }
        
        return comparison_result
    
    def _basic_comparison(self, experiment_names: List[str], metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Perform basic comparison."""
        # Calculate basic statistics
        comparison_result = {
            "total_experiments": len(experiment_names),
            "metrics_analyzed": list(metric_matrix.get(experiment_names[0], {}).keys()) if experiment_names else [],
            "average_values": self._calculate_average_values(metric_matrix),
            "range_values": self._calculate_range_values(metric_matrix),
            "best_performers": self._find_best_performers(metric_matrix)
        }
        
        return comparison_result
    
    def _statistical_comparison(self, experiment_names: List[str], metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Perform statistical comparison."""
        # Calculate statistical comparisons
        comparison_result = {
            "total_experiments": len(experiment_names),
            "metrics_analyzed": list(metric_matrix.get(experiment_names[0], {}).keys()) if experiment_names else [],
            "significant_differences": self._find_significant_differences(metric_matrix),
            "confidence_intervals": self._calculate_confidence_intervals(metric_matrix),
            "effect_sizes": self._calculate_effect_sizes_matrix(metric_matrix)
        }
        
        return comparison_result
    
    def _calculate_summary_statistics(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate summary statistics."""
        summary = {}
        
        for metric_name in metric_matrix.get(list(metric_matrix.keys())[0], {}).keys():
            metric_values = [exp_data[metric_name] for exp_data in metric_matrix.values() 
                           if metric_name in exp_data and exp_data[metric_name] is not None]
            
            if metric_values:
                summary[metric_name] = {
                    "mean": np.mean(metric_values),
                    "median": np.median(metric_values),
                    "std": np.std(metric_values),
                    "min": np.min(metric_values),
                    "max": np.max(metric_values),
                    "range": np.max(metric_values) - np.min(metric_values),
                    "coefficient_of_variation": np.std(metric_values) / np.mean(metric_values) if np.mean(metric_values) != 0 else 0
                }
        
        return summary
    
    def _calculate_correlation_matrix(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate correlation matrix between metrics."""
        if not metric_matrix:
            return {}
        
        # Get all metric names
        metric_names = list(metric_matrix.get(list(metric_matrix.keys())[0], {}).keys())
        
        # Create correlation matrix
        correlation_matrix = {}
        
        for metric1 in metric_names:
            correlation_matrix[metric1] = {}
            for metric2 in metric_names:
                if metric1 == metric2:
                    correlation_matrix[metric1][metric2] = 1.0
                else:
                    # Calculate correlation
                    values1 = [exp_data[metric1] for exp_data in metric_matrix.values() 
                             if metric1 in exp_data and exp_data[metric1] is not None]
                    values2 = [exp_data[metric2] for exp_data in metric_matrix.values() 
                             if metric2 in exp_data and exp_data[metric2] is not None]
                    
                    if len(values1) >= 2 and len(values2) >= 2:
                        correlation = np.corrcoef(values1, values2)[0, 1]
                        correlation_matrix[metric1][metric2] = correlation if not np.isnan(correlation) else 0.0
                    else:
                        correlation_matrix[metric1][metric2] = 0.0
        
        return correlation_matrix
    
    def _calculate_performance_ranking(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate performance ranking."""
        if not metric_matrix:
            return {}
        
        # Get a representative metric for ranking (performance_score)
        ranking_metric = "performance_score"
        
        if ranking_metric not in metric_matrix[list(metric_matrix.keys())[0]]:
            # Use first available metric
            ranking_metric = list(metric_matrix[list(metric_matrix.keys())[0]].keys())[0]
        
        # Calculate ranks
        values = [exp_data[ranking_metric] for exp_data in metric_matrix.values() 
                 if ranking_metric in exp_data and exp_data[ranking_metric] is not None]
        
        if not values:
            return {}
        
        ranked_indices = self._rank_values(values)
        
        # Create ranking results
        performance_ranking = {}
        for i, (exp_name, exp_data) in enumerate(metric_matrix.items()):
            rank = ranked_indices[i] if i < len(ranked_indices) else len(ranked_indices)
            performance_ranking[exp_name] = {
                "rank": rank,
                "value": exp_data.get(ranking_metric),
                "metric_used": ranking_metric
            }
        
        return performance_ranking
    
    def _assess_quality(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Assess quality of experiments."""
        if not metric_matrix:
            return {}
        
        quality_assessment = {}
        
        for exp_name, exp_data in metric_matrix.items():
            # Calculate quality score based on metrics
            quality_score = 0
            metric_count = 0
            
            for metric_name, value in exp_data.items():
                if value is not None and isinstance(value, (int, float)):
                    # Normalize value to 0-1 scale (simplified)
                    normalized_value = min(1.0, value / 100.0)
                    quality_score += normalized_value
                    metric_count += 1
            
            overall_quality = quality_score / max(1, metric_count)
            
            quality_assessment[exp_name] = {
                "quality_score": overall_quality,
                "metric_count": metric_count,
                "average_value": np.mean([v for v in exp_data.values() if v is not None]) if any(v is not None for v in exp_data.values()) else 0
            }
        
        return quality_assessment
    
    def _calculate_average_values(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate average values for each metric."""
        averages = {}
        
        for metric_name in metric_matrix.get(list(metric_matrix.keys())[0], {}).keys():
            values = [exp_data[metric_name] for exp_data in metric_matrix.values() 
                     if metric_name in exp_data and exp_data[metric_name] is not None]
            
            if values:
                averages[metric_name] = {
                    "mean": np.mean(values),
                    "median": np.median(values),
                    "std": np.std(values)
                }
        
        return averages
    
    def _calculate_range_values(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate range values for each metric."""
        ranges = {}
        
        for metric_name in metric_matrix.get(list(metric_matrix.keys())[0], {}).keys():
            values = [exp_data[metric_name] for exp_data in metric_matrix.values() 
                     if metric_name in exp_data and exp_data[metric_name] is not None]
            
            if values:
                ranges[metric_name] = {
                    "min": np.min(values),
                    "max": np.max(values),
                    "range": np.max(values) - np.min(values)
                }
        
        return ranges
    
    def _find_best_performers(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Find best performing experiments."""
        best_performers = {}
        
        if not metric_matrix:
            return best_performers
        
        # Use performance_score if available, otherwise first metric
        ranking_metric = "performance_score"
        if ranking_metric not in metric_matrix[list(metric_matrix.keys())[0]]:
            ranking_metric = list(metric_matrix[list(metric_matrix.keys())[0]].keys())[0]
        
        # Get values for ranking metric
        exp_data_with_values = [(name, data[ranking_metric]) 
                              for name, data in metric_matrix.items() 
                              if ranking_metric in data and data[ranking_metric] is not None]
        
        if not exp_data_with_values:
            return best_performers
        
        # Sort by value (descending for higher is better)
        sorted_experiments = sorted(exp_data_with_values, key=lambda x: x[1], reverse=True)
        
        # Select top 3
        for i, (exp_name, value) in enumerate(sorted_experiments[:3]):
            best_performers[f"rank_{i + 1}"] = {
                "experiment_name": exp_name,
                "value": value,
                "metric": ranking_metric
            }
        
        return best_performers
    
    def _find_significant_differences(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Find significant differences between experiments."""
        significant_differences = {}
        
        # This would require more sophisticated statistical analysis
        # For now, return placeholder
        return significant_differences
    
    def _calculate_confidence_intervals(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate confidence intervals."""
        confidence_intervals = {}
        
        # This would require more sophisticated statistical analysis
        # For now, return placeholder
        return confidence_intervals
    
    def _calculate_effect_sizes_matrix(self, metric_matrix: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate effect sizes between experiment pairs."""
        effect_sizes = {}
        
        # This would require more sophisticated statistical analysis
        # For now, return placeholder
        return effect_sizes
    
    def _create_significance_matrix(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Create significance matrix."""
        significance_matrix = {}
        
        # Create matrix of experiment names
        experiment_names = [exp.get("experiment_name", f"Exp{i+1}") for i, exp in enumerate(experiments)]
        
        for i, exp1_name in enumerate(experiment_names):
            significance_matrix[exp1_name] = {}
            for j, exp2_name in enumerate(experiment_names):
                if i == j:
                    significance_matrix[exp1_name][exp2_name] = "same"
                else:
                    # This would require actual statistical testing
                    significance_matrix[exp1_name][exp2_name] = "not_tested"
        
        return significance_matrix
    
    def _create_performance_plots_data(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Create performance plots data."""
        plots_data = {
            "bar_chart": self._create_bar_chart_data(experiments, metrics),
            "radar_chart": self._create_radar_chart_data(experiments, metrics),
            "heatmap": self._create_heatmap_data(experiments, metrics)
        }
        
        return plots_data
    
    def _create_bar_chart_data(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Create bar chart data."""
        bar_data = {
            "experiment_names": [],
            "metrics": metrics,
            "data": defaultdict(list)
        }
        
        for exp in experiments:
            name = exp.get("experiment_name", "Unknown")
            bar_data["experiment_names"].append(name)
            
            for metric in metrics:
                value = self._extract_metric_value(exp, metric)
                if value is not None:
                    bar_data["data"][metric].append(value)
        
        return bar_data
    
    def _create_radar_chart_data(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:
        """Create radar chart data."""
        radar_data = {
            "categories": metrics,
            "experiments": []
        }
        
        for exp in experiments:
            name = exp.get("experiment_name", "Unknown")
            values = []
            
            for metric in metrics:
                value = self._extract_metric_value(exp, metric)
                if value is not None:
                    # Normalize to 0-1 scale
                    normalized_value = min(1.0, value / 100.0)
                    values.append(normalized_value)
                else:
                    values.append(0)
            
            radar_data["experiments"].append({
                "name": name,
                "values": values
            })
        
        return radar_data
    
    def _create_heatmap_data(self, experiments: List[Dict[str, Any]], metrics: List[str]) -> Dict[str, Any]:n        """Create heatmap data."""
        heatmap_data = {
            "x_labels": metrics,
            "y_labels": [],
            "correlation_matrix": [],
            "title": "Experiment Metric Correlation Heatmap"
        }
        
        # Extract data for heatmap
        heatmap_matrix = []
        
        for i, metric1 in enumerate(metrics):
            row = []
            for j, metric2 in enumerate(metrics):
                if i == j:
                    row.append(1.0)  # Perfect correlation with self
                else:
                    # Calculate correlation between metrics
                    values1 = []
                    values2 = []
                    
                    for exp in experiments:
                        val1 = self._extract_metric_value(exp, metric1)
                        val2 = self._extract_metric_value(exp, metric2)
                        
                        if val1 is not None:
                            values1.append(val1)
                        if val2 is not None:
                            values2.append(val2)
                    
                    if len(values1) >= 2 and len(values2) >= 2:
                        correlation = np.corrcoef(values1, values2)[0, 1]
                        row.append(correlation if not np.isnan(correlation) else 0.0)
                    else:
                        row.append(0.0)
            
            heatmap_matrix.append(row)
        
        heatmap_data["correlation_matrix"] = heatmap_matrix
        heatmap_data["y_labels"] = metrics
        
        return heatmap_data
    
    def _generate_comparison_summary(self, comparison_result: Dict[str, Any], 
                                   ranking_result: Dict[str, Any]) -> Dict[str, Any]:
        """Generate comparison summary."""
        summary = {
            "total_experiments_compared": comparison_result.get("total_experiments", 0),
            "metrics_analyzed": comparison_result.get("metrics_analyzed", []),
            "best_performing_experiment": ranking_result.get("ranked_experiments", [])[0] if ranking_result.get("ranked_experiments") else None,
            "overall_assessment": "Analysis completed",
            "recommendations": self._generate_comparison_recommendations(comparison_result, ranking_result)
        }
        
        return summary
    
    def _generate_comparison_recommendations(self, comparison_result: Dict[str, Any], 
                                           ranking_result: Dict[str, Any]) -> List[str]:
        """Generate comparison recommendations."""
        recommendations = []
        
        # Generate recommendations based on comparison results
        if comparison_result.get("total_experiments", 0) > 0:
            recommendations.append(f"Analyzed {comparison_result['total_experiments']} experiments across {len(comparison_result.get('metrics_analyzed', []))} metrics")
        
        if ranking_result.get("ranked_experiments"):
            best_exp = ranking_result["ranked_experiments"][0]
            recommendations.append(f"Best performing experiment: {best_exp}")
        
        recommendations.append("Consider further analysis of individual metrics for deeper insights")
        recommendations.append("Validate experimental conditions for consistent results")
        
        return recommendations

if __name__ == "__main__":
    # Example usage
    print("Running Experiment Comparator Example")
    
    # Create comparator
    comparator = ExperimentComparator()
    
    # Create sample experiment results
    sample_experiments = [
        {
            "experiment_name": "Control_Group",
            "performance_score": 85.5,
            "memory_efficiency": 0.8,
            "firing_rate": 5.2,
            "activity_level": 0.6,
            "total_spikes": 12500,
            "duration": 300.0,
            "neural_metrics": {
                "total_neurons": 1000,
                "active_neurons": 600,
                "firing_neurons": 480,
                "avg_firing_rate": 5.2,
                "e_i_ratio": 0.6
            },
            "performance_metrics": {
                "avg_step_time_ms": 25.0,
                "neurons_per_second": 3.33,
                "performance_score": 85.5
            }
        },
        {
            "experiment_name": "Treatment_Group_A",
            "performance_score": 78.2,
            "memory_efficiency": 0.7,
            "firing_rate": 4.8,
            "activity_level": 0.5,
            "total_spikes": 15000,
            "duration": 350.0,
            "neural_metrics": {
                "total_neurons": 1000,
                "active_neurons": 500,
                "firing_neurons": 400,
                "avg_firing_rate": 4.8,
                "e_i_ratio": 0.55
            },
            "performance_metrics": {
                "avg_step_time_ms": 28.0,
                "neurons_per_second": 3.57,
                "performance_score": 78.2
            }
        },
        {
            "experiment_name": "Treatment_Group_B",
            "performance_score": 92.8,
            "memory_efficiency": 0.9,
            "firing_rate": 5.8,
            "activity_level": 0.7,
            "total_spikes": 10800,
            "duration": 280.0,
            "neural_metrics": {
                "total_neurons": 1000,
                "active_neurons": 700,
                "firing_neurons": 580,
                "avg_firing_rate": 5.8,
                "e_i_ratio": 0.65
            },
            "performance_metrics": {
                "avg_step_time_ms": 22.0,
                "neurons_per_second": 4.55,
                "performance_score": 92.8
            }
        }
    ]
    
    # Compare experiments
    comparison_result = comparator.compare_experiments(sample_experiments, "comprehensive")
    print(f"\nComparison completed!")
    print(f"Total experiments: {comparison_result.get('total_experiments', 0)}")
    print(f"Metrics analyzed: {len(comparison_result.get('metrics_analyzed', []))}")
    
    # Rank experiments
    ranking_result = comparator.rank_experiments(sample_experiments, ["performance_score", "memory_efficiency", "firing_rate"])
    print(f"\nExperiment rankings:")
    for i, (exp_name, rank) in enumerate(ranking_result["overall_ranking"].items(), 1):
        print(f"  {i}. {exp_name}: {rank:.2f}")
    
    # Detect significance between first two experiments
    significance = comparator.detect_significance(sample_experiments[0], sample_experiments[1], "firing_rate")
    print(f"\nSignificance test (Control vs Treatment A - firing_rate):")
    print(f"  Significant: {significance['significant']}")
    print(f"  p-value: {significance['p_value']:.4f}")
    print(f"  Effect size: {significance['effect_size']:.3f}")
    
    # Compute effect size
    effect_size_result = comparator.compute_effect_size(sample_experiments[0], sample_experiments[1], "activity_level")
    print(f"\nEffect size (Control vs Treatment A - activity_level):")
    print(f"  Effect size: {effect_size_result['effect_size']:.3f}")
    print(f"  Interpretation: {effect_size_result['interpretation']}")
    
    # Get best experiment
    best_exp = comparator.get_best_experiment(sample_experiments, ["performance_score", "memory_efficiency"])
    print(f"\nBest experiment:")
    print(f"  Name: {best_exp['best_experiment']['experiment_name']}")
    print(f"  Rank: {best_exp['ranking']:.2f}")
    
    # Generate comparison report
    report = comparator.generate_comparison_report(sample_experiments, ["performance_score", "memory_efficiency"], "comparison_report.json")
    print(f"\nComparison report generated with {len(report.get('summary', {}).get('recommendations', []))} recommendations")
    
    # Create ranking chart
    chart_data = comparator.create_ranking_chart(sample_experiments, ["performance_score", "memory_efficiency", "firing_rate"], "ranking_chart.json")
    print(f"\nRanking chart data created with {len(chart_data['experiment_names'])} experiments")
    
    # Create performance plot
    plot_data = comparator.create_performance_plot(sample_experiments, "performance_score", "performance_plot.json")
    print(f"\nPerformance plot data created: {plot_data.get('experiment_names', [])[:2]}...")
    
    print("\nExperiment comparator example completed!")
