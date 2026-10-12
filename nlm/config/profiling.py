#!/usr/bin/env python3
"""
NLM Config Profiling - Configuration Profiling

This module provides configuration profiling and optimization.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple, Union
import json
import os
import time
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict, Counter
from copy import deepcopy

from nlm.core.config import Config

class ConfigProfiler:
    """Configuration profiling and optimization.
    
    Provides tools for analyzing and optimizing configuration performance
    and resource usage.
    
    Features:
    * Performance profiling
    * Resource usage analysis
    * Configuration optimization
    * Bottleneck detection
    * Recommendation generation
    """
    
    def __init__(self):
        self.profiling_data = {}
        self.optimization_results = {}
        self.bottleneck_analysis = {}
        self.recommendations = []
        self.benchmark_config = None
    
    def profile_config(self, config: Config) -> Dict[str, Any]:
        """Profile configuration performance.
        
        Args:
            config: Configuration to profile
            
        Returns:
            Performance metrics
        """
        print(f"Profiling configuration")
        
        # Extract configuration parameters
        config_dict = self._extract_config_dict(config)
        
        # Profile performance
        performance_metrics = self._calculate_performance_metrics(config_dict)
        
        # Profile resource usage
        resource_metrics = self._calculate_resource_usage(config_dict)
        
        # Analyze bottlenecks
        bottleneck_analysis = self._analyze_bottlenecks(config_dict, performance_metrics, resource_metrics)
        
        # Generate recommendations
        recommendations = self._generate_recommendations(config_dict, performance_metrics, resource_metrics)
        
        # Store profiling results
        profile_result = {
            "config_name": "profile_" + str(int(time.time())),
            "timestamp": time.time(),
            "parameters": config_dict,
            "performance_metrics": performance_metrics,
            "resource_metrics": resource_metrics,
            "bottleneck_analysis": bottleneck_analysis,
            "recommendations": recommendations,
            "overall_score": self._calculate_overall_score(performance_metrics, resource_metrics)
        }
        
        self.profiling_data[profile_result["config_name"]] = profile_result
        
        return profile_result
    
    def get_performance_metrics(self, config: Config) -> Dict[str, Any]:
        """Get performance metrics for configuration.
        
        Args:
            config: Configuration to analyze
            
        Returns:
            Performance metrics
        """
        config_dict = self._extract_config_dict(config)
        return self._calculate_performance_metrics(config_dict)
    
    def get_resource_usage(self, config: Config) -> Dict[str, Any]:
        """Get resource usage for configuration.
        
        Args:
            config: Configuration to analyze
            
        Returns:
            Resource usage metrics
        """
        config_dict = self._extract_config_dict(config)
        return self._calculate_resource_usage(config_dict)
    
    def optimize_config(self, config: Config, optimization_objective: str = "performance") -> Dict[str, Any]:
        """Optimize configuration for specified objective.
        
        Args:
            config: Configuration to optimize
            optimization_objective: Optimization objective (performance, memory, balanced)
            
        Returns:
            Optimized configuration
        """
        print(f"Optimizing configuration for {optimization_objective}")
        
        config_dict = self._extract_config_dict(config)
        
        # Profile current configuration
        current_profile = self._calculate_performance_metrics(config_dict)
        current_resources = self._calculate_resource_usage(config_dict)
        
        # Generate optimization recommendations
        recommendations = self._generate_optimization_recommendations(config_dict, optimization_objective)
        
        # Apply optimizations
        optimized_config_dict = self._apply_optimizations(config_dict, recommendations, optimization_objective)
        
        # Profile optimized configuration
        optimized_profile = self._calculate_performance_metrics(optimized_config_dict)
        optimized_resources = self._calculate_resource_usage(optimized_config_dict)
        
        # Compare before and after
        comparison = self._compare_configurations(config_dict, optimized_config_dict, optimization_objective)
        
        optimization_result = {
            "original_config": config_dict,
            "optimized_config": optimized_config_dict,
            "original_performance": current_profile,
            "optimized_performance": optimized_profile,
            "original_resources": current_resources,
            "optimized_resources": optimized_resources,
            "recommendations": recommendations,
            "improvement": comparison,
            "optimization_objective": optimization_objective
        }
        
        self.optimization_results[f"optimization_{int(time.time())}"] = optimization_result
        
        return optimized_config_dict
    
    def get_recommendations(self, config: Config) -> List[str]:
        """Get optimization recommendations for configuration.
        
        Args:
            config: Configuration to analyze
            
        Returns:
            List of recommendations
        """
        config_dict = self._extract_config_dict(config)
        
        # Profile configuration
        performance_metrics = self._calculate_performance_metrics(config_dict)
        resource_metrics = self._calculate_resource_usage(config_dict)
        
        # Generate recommendations
        recommendations = self._generate_recommendations(config_dict, performance_metrics, resource_metrics)
        
        return recommendations
    
    def detect_bottlenecks(self, config: Config) -> Dict[str, Any]:
        """Detect performance bottlenecks in configuration.
        
        Args:
            config: Configuration to analyze
            
        Returns:
            Bottleneck analysis
        """
        config_dict = self._extract_config_dict(config)
        
        # Analyze performance
        performance_metrics = self._calculate_performance_metrics(config_dict)
        resource_metrics = self._calculate_resource_usage(config_dict)
        
        # Detect bottlenecks
        bottleneck_analysis = self._analyze_bottlenecks(config_dict, performance_metrics, resource_metrics)
        
        return bottleneck_analysis
    
    def compare_configs(self, config1: Config, config2: Config, objective: str = "performance") -> Dict[str, Any]:
        """Compare two configurations.
        
        Args:
            config1: First configuration
            config2: Second configuration
            objective: Comparison objective
            
        Returns:
            Comparison results
        """
        dict1 = self._extract_config_dict(config1)
        dict2 = self._extract_config_dict(config2)
        
        return self._compare_configurations(dict1, dict2, objective)
    
    def get_optimization_history(self) -> List[Dict[str, Any]]:
        """Get optimization history.
        
        Returns:
            List of optimization results
        """
        return list(self.optimization_results.values())
    
    def clear_profile_cache(self):
        """Clear profiling cache."""
        print("Clearing profiling cache")
        self.profiling_data.clear()
        self.optimization_results.clear()
        self.bottleneck_analysis.clear()
        self.recommendations.clear()
    
    def __repr__(self):
        return f"ConfigProfiler(profiles={len(self.profiling_data)}, optimizations={len(self.optimization_results)})"
    
    # Helper methods for profiling
    
    def _extract_config_dict(self, config: Config) -> Dict[str, Any]:
        """Extract configuration dictionary."""
        config_dict = {}
        
        # Extract key configuration parameters
        if config.has("brain.neuron_count"):
            config_dict["neuron_count"] = config.get("brain.neuron_count")
        
        if config.has("brain.connection_probability"):
            config_dict["connection_probability"] = config.get("brain.connection_probability")
        
        if config.has("brain.v_thresh"):
            config_dict["v_thresh"] = config.get("brain.v_thresh")
        
        if config.has("brain.v_rest"):
            config_dict["v_rest"] = config.get("brain.v_rest")
        
        if config.has("brain.tau_mem"):
            config_dict["tau_mem"] = config.get("brain.tau_mem")
        
        if config.has("brain.dt"):
            config_dict["dt"] = config.get("brain.dt")
        
        if config.has("brain.simulation_steps"):
            config_dict["simulation_steps"] = config.get("brain.simulation_steps")
        
        # Add other parameters as needed
        other_keys = ["plasticity_rate", "learning_rate", "random_seed", "enable_activity_detection"]
        for key in other_keys:
            if config.has(key):
                config_dict[key] = config.get(key)
        
        return config_dict
    
    def _calculate_performance_metrics(self, config_dict: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate performance metrics."""
        # Estimate performance based on configuration parameters
        neuron_count = config_dict.get("neuron_count", 1000)
        connection_probability = config_dict.get("connection_probability", 0.1)
        v_thresh = config_dict.get("v_thresh", -50.0)
        v_rest = config_dict.get("v_rest", -70.0)
        tau_mem = config_dict.get("tau_mem", 20.0)
        dt = config_dict.get("dt", 0.1)
        simulation_steps = config_dict.get("simulation_steps", 1000)
        
        # Calculate estimated performance metrics
        # Based on computational complexity O(N^2) for dense connectivity
        estimated_time_per_step = (neuron_count ** 2) * connection_probability * dt
        total_estimated_time = estimated_time_per_step * simulation_steps
        
        # Normalize metrics (0-100 scale)
        performance_score = min(100, (1000000 / max(1, neuron_count)) * connection_probability * 10)
        memory_efficiency = min(100, (1000 / max(1, neuron_count)) * (1 - connection_probability))
        computational_efficiency = min(100, (1000 / max(1, total_estimated_time / 1000)))
        
        # Calculate stability based on neuron parameters
        stability_score = min(100, 50 + (abs(v_thresh - v_rest) / 30) * 50)
        
        return {
            "performance_score": performance_score,
            "memory_efficiency": memory_efficiency,
            "computational_efficiency": computational_efficiency,
            "stability_score": stability_score,
            "estimated_time_per_step_ms": estimated_time_per_step * 1000,
            "total_estimated_time_ms": total_estimated_time * 1000,
            "neuron_count_impact": min(100, neuron_count / 1000),
            "connectivity_impact": connection_probability * 100,
            "excitability_impact": min(100, (abs(v_thresh) / 100) * 50)
        }
    
    def _calculate_resource_usage(self, config_dict: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate resource usage."""
        neuron_count = config_dict.get("neuron_count", 1000)
        connection_probability = config_dict.get("connection_probability", 0.1)
        
        # Estimate memory usage
        # Each neuron: state variables (~50 bytes)
        # Each synapse: connection weight (~8 bytes)
        neuron_memory_mb = (neuron_count * 50) / (1024 * 1024)
        synapse_memory_mb = (neuron_count * neuron_count * connection_probability * 8) / (1024 * 1024)
        total_memory_mb = neuron_memory_mb + synapse_memory_mb
        
        # Estimate CPU usage
        cpu_usage_percent = min(100, (neuron_count / 10000) * connection_probability * 100)
        
        # Estimate I/O operations
        io_operations_per_step = neuron_count * connection_probability
        total_io_operations = io_operations_per_step * config_dict.get("simulation_steps", 1000)
        
        # Calculate efficiency scores
        memory_efficiency = min(100, 1000 / max(1, total_memory_mb))
        cpu_efficiency = 100 - cpu_usage_percent
        io_efficiency = min(100, 10000 / max(1, total_io_operations))
        
        return {
            "total_memory_mb": total_memory_mb,
            "neuron_memory_mb": neuron_memory_mb,
            "synapse_memory_mb": synapse_memory_mb,
            "cpu_usage_percent": cpu_usage_percent,
            "io_operations_per_step": io_operations_per_step,
            "total_io_operations": total_io_operations,
            "memory_efficiency": memory_efficiency,
            "cpu_efficiency": cpu_efficiency,
            "io_efficiency": io_efficiency,
            "resource_utilization_score": (memory_efficiency + cpu_efficiency + io_efficiency) / 3
        }
    
    def _analyze_bottlenecks(self, config_dict: Dict[str, Any], performance_metrics: Dict[str, Any], 
                           resource_metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze performance bottlenecks."""
        bottlenecks = []
        severity_scores = []
        
        # Check neuron count bottleneck
        neuron_count = config_dict.get("neuron_count", 1000)
        if neuron_count > 10000:
            bottlenecks.append("High neuron count may cause memory and performance issues")
            severity_scores.append(min(10, (neuron_count - 10000) / 1000))
        elif neuron_count > 50000:
            bottlenecks.append("Very high neuron count - consider scaling down")
            severity_scores.append(min(10, (neuron_count - 50000) / 5000))
        
        # Check connection probability bottleneck
        connection_prob = config_dict.get("connection_probability", 0.1)
        if connection_prob > 0.5:
            bottlenecks.append("High connection probability - may cause computational overload")
            severity_scores.append(min(10, (connection_prob - 0.5) * 20))
        
        # Check memory bottleneck
        if resource_metrics["total_memory_mb"] > 500:
            bottlenecks.append("High memory usage - consider memory optimization")
            severity_scores.append(min(10, (resource_metrics["total_memory_mb"] - 500) / 100))
        
        # Check stability bottleneck
        if performance_metrics["stability_score"] < 50:
            bottlenecks.append("Low stability - consider adjusting neuron parameters")
            severity_scores.append(min(10, 50 - performance_metrics["stability_score"]) / 10)
        
        # Check overall efficiency bottleneck
        overall_efficiency = (performance_metrics["performance_score"] + 
                            resource_metrics["resource_utilization_score"]) / 2
        if overall_efficiency < 50:
            bottlenecks.append("Low overall efficiency - significant optimization needed")
            severity_scores.append(min(10, 50 - overall_efficiency) / 10)
        
        return {
            "bottlenecks_found": len(bottlenecks),
            "bottleneck_list": bottlenecks,
            "severity_scores": severity_scores,
            "overall_bottleneck_score": sum(severity_scores) / max(1, len(severity_scores)),
            "recommendations": self._generate_bottleneck_recommendations(bottlenecks, severity_scores)
        }
    
    def _generate_recommendations(self, config_dict: Dict[str, Any], performance_metrics: Dict[str, Any], 
                                 resource_metrics: Dict[str, Any]) -> List[str]:
        """Generate optimization recommendations."""
        recommendations = []
        
        # Analyze performance
        if performance_metrics["performance_score"] < 50:
            recommendations.append("Consider reducing neuron count or connection probability for better performance")
        
        # Analyze memory
        if resource_metrics["total_memory_mb"] > 200:
            recommendations.append("Memory usage is high - consider using sparse connectivity or compression")
        
        # Analyze stability
        if performance_metrics["stability_score"] < 60:
            recommendations.append("Stability is low - adjust neuron threshold parameters (v_thresh, v_rest)")
        
        # Analyze efficiency
        overall_efficiency = (performance_metrics["performance_score"] + 
                            resource_metrics["resource_utilization_score"]) / 2
        if overall_efficiency < 60:
            recommendations.append("Overall efficiency is low - multiple optimizations needed")
        
        # Check for specific configurations
        neuron_count = config_dict.get("neuron_count", 1000)
        if neuron_count > 5000 and resource_metrics["total_memory_mb"] > 100:
            recommendations.append("Large network with high memory usage - consider distributed computing")
        
        if config_dict.get("connection_probability", 0.1) > 0.3 and neuron_count > 10000:
            recommendations.append("High connectivity with large network - computationally expensive, consider pruning")
        
        return recommendations
    
    def _generate_optimization_recommendations(self, config_dict: Dict[str, Any], objective: str) -> List[str]:
        """Generate optimization recommendations."""
        recommendations = []
        
        if objective == "performance":
            recommendations.append("Reduce neuron count for better performance")
            recommendations.append("Decrease connection probability")
            recommendations.append("Optimize simulation time step (dt)")
        elif objective == "memory":
            recommendations.append("Use sparse connectivity patterns")
            recommendations.append("Implement weight compression")
            recommendations.append("Consider distributed memory")
        elif objective == "balanced":
            recommendations.append("Find optimal balance between neurons and connections")
            recommendations.append("Implement adaptive connectivity")
            recommendations.append("Use efficient data structures")
        
        return recommendations
    
    def _apply_optimizations(self, config_dict: Dict[str, Any], recommendations: List[str], 
                           objective: str) -> Dict[str, Any]:
        """Apply optimizations to configuration."""
        optimized_config = deepcopy(config_dict)
        
        # Apply different optimizations based on objective
        if objective == "performance":
            # Reduce neuron count by 20%
            neuron_count = optimized_config.get("neuron_count", 1000)
            optimized_config["neuron_count"] = int(neuron_count * 0.8)
            
            # Reduce connection probability by 30%
            conn_prob = optimized_config.get("connection_probability", 0.1)
            optimized_config["connection_probability"] = max(0.01, conn_prob * 0.7)
            
        elif objective == "memory":
            # Use sparse connectivity
            optimized_config["connection_probability"] = max(0.01, optimized_config.get("connection_probability", 0.1) * 0.5)
            
        elif objective == "balanced":
            # Find optimal balance
            neuron_count = optimized_config.get("neuron_count", 1000)
            conn_prob = optimized_config.get("connection_probability", 0.1)
            
            # Adjust to balance memory and performance
            optimized_config["neuron_count"] = int(neuron_count * 0.9)
            optimized_config["connection_probability"] = max(0.05, conn_prob * 0.8)
        
        return optimized_config
    
    def _compare_configurations(self, config1: Dict[str, Any], config2: Dict[str, Any], objective: str) -> Dict[str, Any]:
        """Compare two configurations."""
        # Profile both configurations
        perf1 = self._calculate_performance_metrics(config1)
        perf2 = self._calculate_performance_metrics(config2)
        
        res1 = self._calculate_resource_usage(config1)
        res2 = self._calculate_resource_usage(config2)
        
        # Calculate improvements
        if objective == "performance":
            performance_improvement = perf2["performance_score"] - perf1["performance_score"]
            memory_improvement = res2["resource_utilization_score"] - res1["resource_utilization_score"]
            overall_improvement = (performance_improvement + memory_improvement) / 2
        elif objective == "memory":
            memory_improvement = res2["resource_utilization_score"] - res1["resource_utilization_score"]
            performance_improvement = perf2["performance_score"] - perf1["performance_score"]
            overall_improvement = (performance_improvement + memory_improvement) / 2
        else:  # balanced
            overall_improvement = (perf2["performance_score"] - perf1["performance_score"] + 
                                 res2["resource_utilization_score"] - res1["resource_utilization_score"]) / 2
        
        return {
            "performance_improvement": performance_improvement,
            "memory_improvement": memory_improvement,
            "overall_improvement": overall_improvement,
            "improvement_percentage": (overall_improvement / max(1, perf1.get("performance_score", 50))) * 100,
            "better_configuration": "config2" if overall_improvement > 0 else "config1",
            "significant_improvement": abs(overall_improvement) > 10
        }
    
    def _calculate_overall_score(self, performance_metrics: Dict[str, Any], resource_metrics: Dict[str, Any]) -> float:
        """Calculate overall score."""
        return (performance_metrics["performance_score"] + 
                resource_metrics["resource_utilization_score"]) / 2

if __name__ == "__main__":
    # Example usage
    print("Running Config Profiler Example")
    
    # Create config profiler
    profiler = ConfigProfiler()
    
    # Create test configurations
    from nlm.core.config import Config
    
    # Create small configuration
    small_config = Config()
    small_config.set("brain.neuron_count", 1000)
    small_config.set("brain.connection_probability", 0.1)
    small_config.set("brain.v_thresh", -50.0)
    small_config.set("brain.v_rest", -70.0)
    small_config.set("brain.tau_mem", 20.0)
    
    # Create medium configuration
    medium_config = Config()
    medium_config.set("brain.neuron_count", 10000)
    medium_config.set("brain.connection_probability", 0.05)
    medium_config.set("brain.v_thresh", -55.0)
    medium_config.set("brain.v_rest", -70.0)
    medium_config.set("brain.tau_mem", 25.0)
    
    # Create large configuration
    large_config = Config()
    large_config.set("brain.neuron_count", 100000)
    large_config.set("brain.connection_probability", 0.02)
    large_config.set("brain.v_thresh", -60.0)
    large_config.set("brain.v_rest", -75.0)
    large_config.set("brain.tau_mem", 30.0)
    
    # Profile configurations
    print("Profiling configurations...")
    small_profile = profiler.profile_config(small_config)
    medium_profile = profiler.profile_config(medium_config)
    large_profile = profiler.profile_config(large_config)
    
    print(f"Small config score: {small_profile['overall_score']:.1f}")
    print(f"Medium config score: {medium_profile['overall_score']:.1f}")
    print(f"Large config score: {large_profile['overall_score']:.1f}")
    
    # Get performance metrics
    print("\nPerformance metrics:")
    print(f"Small config performance: {small_profile['performance_metrics']['performance_score']:.1f}")
    print(f"Medium config performance: {medium_profile['performance_metrics']['performance_score']:.1f}")
    print(f"Large config performance: {large_profile['performance_metrics']['performance_score']:.1f}")
    
    # Get resource usage
    print("\nResource usage:")
    print(f"Small config memory: {small_profile['resource_metrics']['total_memory_mb']:.1f} MB")
    print(f"Medium config memory: {medium_profile['resource_metrics']['total_memory_mb']:.1f} MB")
    print(f"Large config memory: {large_profile['resource_metrics']['total_memory_mb']:.1f} MB")
    
    # Detect bottlenecks
    print("\nBottleneck analysis:")
    small_bottlenecks = profiler.detect_bottlenecks(small_config)
    medium_bottlenecks = profiler.detect_bottlenecks(medium_config)
    large_bottlenecks = profiler.detect_bottlenecks(large_config)
    
    print(f"Small config bottlenecks: {small_bottlenecks['bottlenecks_found']}")
    print(f"Medium config bottlenecks: {medium_bottlenecks['bottlenecks_found']}")
    print(f"Large config bottlenecks: {large_bottlenecks['bottlenecks_found']}")
    
    # Get recommendations
    print("\nRecommendations:")
    small_recs = profiler.get_recommendations(small_config)
    print(f"Small config recommendations: {len(small_recs)}")
    for rec in small_recs:
        print(f"  - {rec}")
    
    # Optimize configurations
    print("\nOptimizing configurations...")
    small_optimized = profiler.optimize_config(small_config, "balanced")
    print(f"Small config optimized: neuron_count={small_optimized.get('neuron_count')}, "
          f"conn_prob={small_optimized.get('connection_probability')}")
    
    # Compare configurations
    print("\nConfiguration comparisons:")
    small_vs_medium = profiler.compare_configs(small_config, medium_config, "balanced")
    print(f"Small vs Medium: {small_vs_medium['improvement_percentage']:.1f}% improvement for {small_vs_medium['better_configuration']}")
    
    # Show optimization history
    print(f"\nOptimization history: {len(profiler.get_optimization_history())} entries")
    
    print("Config profiler example completed!")
