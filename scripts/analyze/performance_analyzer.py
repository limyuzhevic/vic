#!/usr/bin/env python3
"""
Performance Analyzer for NLM Brain Simulation

Comprehensive performance analyzer for identifying performance bottlenecks,
optimization opportunities, and comparing performance across different hardware.
Generates detailed performance optimization recommendations and analysis reports.

Features:
- Performance bottleneck identification
- Optimization opportunity analysis
- Cross-hardware performance comparison
- Performance trend analysis
- Resource utilization analysis
- Bottleneck categorization and prioritization
- Automated optimization recommendations
- Integration with profiling tools

Usage:
    # Analyze performance bottlenecks
    python performance_analyzer.py --profile-results performance_data.json
    
    # Compare across different hardware
    python performance_analyzer.py --hardware-analysis --data-dir hardware_data/
    
    # Generate optimization recommendations
    python performance_analyzer.py --optimize --target cpu memory
    
    # Analyze performance trends
    python performance_analyzer.py --trend-analysis --historical-data historical/
    
    # Generate comprehensive performance report
    python performance_analyzer.py --report --input profile1.json profile2.json profile3.json

Examples:
    >>> from performance_analyzer import PerformanceAnalyzer
    >>> analyzer = PerformanceAnalyzer()
    >>> bottlenecks = analyzer.analyze_performance("profile_results.json")
    >>> recommendations = analyzer.generate_optimization_recommendations(bottlenecks)
    >>> analyzer.generate_report(recommendations, "performance_optimization.html")

See Also:
    performance_profiler.py - For detailed performance profiling
    memory_profiler.py - For memory analysis
    benchmark_suite.py - For performance comparison
    load_tester.py - For performance under load

API Reference:
    PerformanceAnalyzer - Main performance analysis class
    analyze_bottlenecks() - Identify performance bottlenecks
    analyze_optimization_opportunities() - Find optimization opportunities
    compare_hardware_performance() - Compare across different hardware
    generate_recommendations() - Generate optimization recommendations

Author: NLM Team
Version: 1.0.0
License: MIT

For support and documentation, visit: https://github.com/your-repo/nlm-tools
"""

import json
import os
import sys
import argparse
from typing import Dict, List, Any, Optional, Tuple, Union
from pathlib import Path
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import seaborn as sns
from collections import defaultdict, Counter
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
import warnings
warnings.filterwarnings('ignore')

# Set style for matplotlib
plt.style.use('seaborn-v0_8')


class BottleneckDetector:
    """Detect performance bottlenecks from profiling data."""
    
    def __init__(self, sensitivity: float = 1.0):
        self.sensitivity = sensitivity
        self.thresholds = {
            'cpu_usage': 80.0,  # 80% CPU utilization
            'memory_usage': 85.0,  # 85% memory utilization
            'response_time': 1000.0,  # 1 second response time
            'throughput': 10.0,  # 10 operations/sec
            'gc_overhead': 50.0,  # 50% GC overhead
            'context_switch': 1000.0,  # 1000 context switches/sec
            'lock_contention': 100.0,  # 100ms lock contention
            'cache_miss_rate': 20.0,  # 20% cache miss rate
            'io_wait_time': 500.0  # 500ms I/O wait time
        }
        
    def analyze_profile(self, profile_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze profiling data to identify bottlenecks."""
        bottlenecks = {
            'detected_bottlenecks': [],
            'severity_scores': {},
            'recommendations': [],
            'bottleneck_categories': defaultdict(list)
        }
        
        # Extract key metrics
        metrics = self._extract_metrics(profile_data)
        
        # Analyze CPU bottlenecks
        cpu_bottlenecks = self._analyze_cpu_bottlenecks(metrics)
        bottlenecks['detected_bottlenecks'].extend(cpu_bottlenecks)
        
        # Analyze memory bottlenecks
        memory_bottlenecks = self._analyze_memory_bottlenecks(metrics)
        bottlenecks['detected_bottlenecks'].extend(memory_bottlenecks)
        
        # Analyze I/O bottlenecks
        io_bottlenecks = self._analyze_io_bottlenecks(metrics)
        bottlenecks['detected_bottlenecks'].extend(io_bottlenecks)
        
        # Analyze algorithmic bottlenecks
        alg_bottlenecks = self._analyze_algorithmic_bottlenecks(metrics)
        bottlenecks['detected_bottlenecks'].extend(alg_bottlenecks)
        
        # Calculate severity scores
        bottlenecks['severity_scores'] = self._calculate_severity_scores(bottlenecks['detected_bottlenecks'])
        
        # Categorize bottlenecks
        bottlenecks['bottleneck_categories'] = self._categorize_bottlenecks(bottlenecks['detected_bottlenecks'])
        
        # Generate recommendations
        bottlenecks['recommendations'] = self._generate_recommendations(bottlenecks)
        
        return bottlenecks
        
    def _extract_metrics(self, profile_data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract key performance metrics from profile data."""
        metrics = {}
        
        # Extract from different profile sources
        if 'performance_profiler' in profile_data:
            metrics.update(profile_data['performance_profiler'])
        if 'memory_profiler' in profile_data:
            metrics.update(profile_data['memory_profiler'])
        if 'benchmark_suite' in profile_data:
            metrics.update(profile_data['benchmark_suite'])
        if 'load_tester' in profile_data:
            metrics.update(profile_data['load_tester'])
            
        # Standardize metric names
        standardized = {}
        for key, value in metrics.items():
            # Standardize metric keys
            standardized_key = key.lower().replace(' ', '_').replace('-', '_')
            standardized[standardized_key] = value
            
        return standardized
        
    def _analyze_cpu_bottlenecks(self, metrics: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Analyze CPU-related bottlenecks."""
        bottlenecks = []
        
        # Check CPU usage
        cpu_usage = metrics.get('cpu_percent', 0)
        if cpu_usage > self.thresholds['cpu_usage']:
            bottlenecks.append({
                'type': 'cpu_bound',
                'severity': self._calculate_severity(cpu_usage, self.thresholds['cpu_usage']),
                'description': f"High CPU utilization: {cpu_usage:.1f}%",
                'location': 'system-wide',
                'potential_impact': 'Reduced throughput and increased latency',
                'recommendation': 'Optimize CPU-intensive algorithms or scale horizontally'
            })
            
        # Check context switches
        context_switches = metrics.get('context_switches_per_sec', 0)
        if context_switches > self.thresholds['context_switch']:
            bottlenecks.append({
                'type': 'thread_saturation',
                'severity': self._calculate_severity(context_switches, self.thresholds['context_switch']),
                'description': f"High context switch rate: {context_switches:.0f}/sec",
                'location': 'thread management',
                'potential_impact': 'Increased overhead and reduced performance',
                'recommendation': 'Reduce thread count, use async patterns, or optimize locking'
            })
            
        # Check CPU efficiency
        cpu_efficiency = metrics.get('cpu_efficiency', 0)
        if cpu_efficiency < 0.7:  # Less than 70% efficiency
            bottlenecks.append({
                'type': 'cpu_inefficiency',
                'severity': self._calculate_severity(0.7 - cpu_efficiency, 0.7),
                'description': f"Low CPU efficiency: {cpu_efficiency:.1%}",
                'location': 'algorithm implementation',
                'potential_impact': 'Underutilized computing resources',
                'recommendation': 'Optimize algorithms, reduce idle time, or improve cache locality'
            })
            
        return bottlenecks
        
    def _analyze_memory_bottlenecks(self, metrics: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Analyze memory-related bottlenecks."""
        bottlenecks = []
        
        # Check memory usage
        memory_usage = metrics.get('memory_usage_percent', 0)
        if memory_usage > self.thresholds['memory_usage']:
            bottlenecks.append({
                'type': 'memory_bound',
                'severity': self._calculate_severity(memory_usage, self.thresholds['memory_usage']),
                'description': f"High memory usage: {memory_usage:.1f}%",
                'location': 'memory management',
                'potential_impact': 'Increased GC pressure and potential OOM',
                'recommendation': 'Optimize memory usage, implement object pooling, or increase heap size'
            })
            
        # Check GC overhead
        gc_overhead = metrics.get('gc_overhead_percent', 0)
        if gc_overhead > self.thresholds['gc_overhead']:
            bottlenecks.append({
                'type': 'gc_pressure',
                'severity': self._calculate_severity(gc_overhead, self.thresholds['gc_overhead']),
                'description': f"High garbage collection overhead: {gc_overhead:.1f}%",
                'location': 'memory allocation',
                'potential_impact': 'Pauses and reduced throughput',
                'recommendation': 'Reduce object creation, use object pooling, or optimize GC settings'
            })
            
        # Check memory leak indicators
        memory_growth = metrics.get('memory_growth_rate_mb_per_sec', 0)
        if memory_growth > 1.0:  # Growing more than 1MB/sec
            bottlenecks.append({
                'type': 'memory_leak',
                'severity': 'high',
                'description': f"Memory growth rate: {memory_growth:.1f} MB/sec",
                'location': 'application code',
                'potential_impact': 'Gradual resource exhaustion',
                'recommendation': 'Memory leak detection and fix, review resource cleanup'
            })
            
        # Check cache performance
        cache_hit_rate = metrics.get('cache_hit_rate_percent', 0)
        if cache_hit_rate < (100 - self.thresholds['cache_miss_rate']):
            bottlenecks.append({
                'type': 'cache_inefficiency',
                'severity': self._calculate_severity(100 - cache_hit_rate, self.thresholds['cache_miss_rate']),
                'description': f"Low cache hit rate: {cache_hit_rate:.1f}%",
                'location': 'data access patterns',
                'potential_impact': 'Increased memory access time and reduced throughput',
                'recommendation': 'Optimize data access patterns, use appropriate caching strategies'
            })
            
        return bottlenecks
        
    def _analyze_io_bottlenecks(self, metrics: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Analyze I/O-related bottlenecks."""
        bottlenecks = []
        
        # Check I/O wait time
        io_wait = metrics.get('io_wait_time_ms', 0)
        if io_wait > self.thresholds['io_wait_time']:
            bottlenecks.append({
                'type': 'io_bound',
                'severity': self._calculate_severity(io_wait, self.thresholds['io_wait_time']),
                'description': f"High I/O wait time: {io_wait:.1f} ms",
                'location': 'I/O operations',
                'potential_impact': 'Reduced throughput and increased latency',
                'recommendation': 'Optimize I/O operations, use asynchronous I/O, or increase I/O bandwidth'
            })
            
        # Check disk I/O
        disk_read = metrics.get('disk_read_mb_per_sec', 0)
        disk_write = metrics.get('disk_write_mb_per_sec', 0)
        
        if disk_read > 500 or disk_write > 200:  # High I/O rates
            bottlenecks.append({
                'type': 'disk_saturation',
                'severity': 'medium',
                'description': f"High disk I/O: {disk_read:.0f} MB/sec read, {disk_write:.0f} MB/sec write",
                'location': 'storage subsystem',
                'potential_impact': 'I/O bottlenecks and reduced throughput',
                'recommendation': 'Use faster storage, implement SSD, or optimize I/O patterns'
            })
            
        return bottlenecks
        
    def _analyze_algorithmic_bottlenecks(self, metrics: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Analyze algorithmic bottlenecks."""
        bottlenecks = []
        
        # Check response time
        response_time = metrics.get('avg_response_time_ms', 0)
        if response_time > self.thresholds['response_time']:
            bottlenecks.append({
                'type': 'algorithmic_inefficiency',
                'severity': self._calculate_severity(response_time, self.thresholds['response_time']),
                'description': f"High response time: {response_time:.1f} ms",
                'location': 'algorithm implementation',
                'potential_impact': 'Poor user experience and reduced throughput',
                'recommendation': 'Optimize algorithms, use more efficient data structures, or implement caching'
            })
            
        # Check throughput
        throughput = metrics.get('avg_throughput_per_sec', 0)
        if throughput > 0 and throughput < self.thresholds['throughput']:
            bottlenecks.append({
                'type': 'throughput_limit',
                'severity': self._calculate_severity(self.thresholds['throughput'] - throughput, self.thresholds['throughput']),
                'description': f"Low throughput: {throughput:.1f} operations/sec",
                'location': 'system capacity',
                'potential_impact': 'Reduced system performance',
                'recommendation': 'Increase system resources, optimize algorithms, or scale horizontally'
            })
            
        # Check lock contention
        lock_contention = metrics.get('lock_contention_time_ms', 0)
        if lock_contention > self.thresholds['lock_contention']:
            bottlenecks.append({
                'type': 'lock_contention',
                'severity': self._calculate_severity(lock_contention, self.thresholds['lock_contention']),
                'description': f"High lock contention: {lock_contention:.1f} ms",
                'location': 'synchronization primitives',
                'potential_impact': 'Reduced parallelism and increased latency',
                'recommendation': 'Reduce lock scope, use non-blocking algorithms, or optimize concurrency patterns'
            })
            
        return bottlenecks
        
    def _calculate_severity(self, value: float, threshold: float) -> str:
        """Calculate severity based on value and threshold."""
        ratio = value / threshold if threshold > 0 else 0
        
        if ratio >= 2.0:
            return 'critical'
        elif ratio >= 1.5:
            return 'high'
        elif ratio >= 1.0:
            return 'medium'
        elif ratio >= 0.5:
            return 'low'
        else:
            return 'minimal'
            
    def _calculate_severity_scores(self, bottlenecks: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate severity scores for each bottleneck."""
        severity_scores = {
            'critical': 0,
            'high': 0,
            'medium': 0,
            'low': 0,
            'minimal': 0
        }
        
        for bottleneck in bottlenecks:
            severity = bottleneck.get('severity', 'minimal')
            severity_scores[severity] += 1
            
        total = len(bottlenecks) if bottlenecks else 1
        severity_scores['percentages'] = {
            'critical': (severity_scores['critical'] / total) * 100,
            'high': (severity_scores['high'] / total) * 100,
            'medium': (severity_scores['medium'] / total) * 100,
            'low': (severity_scores['low'] / total) * 100,
            'minimal': (severity_scores['minimal'] / total) * 100
        }
        
        return severity_scores
        
    def _categorize_bottlenecks(self, bottlenecks: List[Dict[str, Any]]) -> Dict[str, List[str]]:
        """Categorize bottlenecks by type."""
        categories = defaultdict(list)
        
        for bottleneck in bottlenecks:
            bottleneck_type = bottleneck.get('type', 'unknown')
            categories[bottleneck_type].append(bottleneck.get('description', ''))
            
        return dict(categories)
        
    def _generate_recommendations(self, bottlenecks: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Generate optimization recommendations based on bottlenecks."""
        recommendations = []
        
        # Analyze by severity
        severity_counts = bottlenecks.get('severity_scores', {}).get('percentages', {})
        
        # Critical recommendations
        if severity_counts.get('critical', 0) > 0:
            recommendations.append({
                'priority': 'critical',
                'title': 'Immediate Action Required',
                'description': 'Critical bottlenecks detected that require immediate attention',
                'actions': [
                    'Address memory leaks immediately',
                    'Optimize CPU-intensive algorithms',
                    'Increase system resources if needed'
                ]
            })
            
        # High priority recommendations
        if severity_counts.get('high', 0) > 0:
            recommendations.append({
                'priority': 'high',
                'title': 'High Priority Optimizations',
                'description': 'High-severity bottlenecks that should be addressed soon',
                'actions': [
                    'Optimize memory usage patterns',
                    'Reduce I/O wait times',
                    'Improve cache efficiency'
                ]
            })
            
        # Medium priority recommendations
        if severity_counts.get('medium', 0) > 0:
            recommendations.append({
                'priority': 'medium',
                'title': 'Performance Improvements',
                'description': 'Medium-severity bottlenecks for incremental improvements',
                'actions': [
                    'Optimize algorithmic complexity',
                    'Improve resource utilization',
                    'Enhance concurrency patterns'
                ]
            })
            
        # Add specific recommendations based on bottleneck types
        for bottleneck_type in bottlenecks.get('bottleneck_categories', {}).keys():
            type_recommendations = self._get_type_recommendations(bottleneck_type)
            if type_recommendations:
                recommendations.append(type_recommendations)
                
        return recommendations
        
    def _get_type_recommendations(self, bottleneck_type: str) -> Optional[Dict[str, Any]]:
        """Get specific recommendations for bottleneck type."""
        type_recommendations = {
            'cpu_bound': {
                'priority': 'high',
                'title': 'CPU Optimization',
                'description': 'Optimize CPU-intensive operations',
                'actions': [
                    'Profile CPU usage to identify hotspots',
                    'Optimize algorithms and data structures',
                    'Consider parallel processing for CPU-bound tasks',
                    'Use more efficient programming patterns'
                ]
            },
            'memory_bound': {
                'priority': 'high',
                'title': 'Memory Optimization',
                'description': 'Optimize memory usage and allocation',
                'actions': [
                    'Implement object pooling',
                    'Optimize data structures',
                    'Use memory-efficient algorithms',
                    'Implement proper resource cleanup'
                ]
            },
            'gc_pressure': {
                'priority': 'critical',
                'title': 'Garbage Collection Optimization',
                'description': 'Reduce garbage collection overhead',
                'actions': [
                    'Implement object pooling',
                    'Reduce temporary object creation',
                    'Use more efficient memory allocation',
                    'Configure GC settings appropriately'
                ]
            },
            'io_bound': {
                'priority': 'medium',
                'title': 'I/O Optimization',
                'description': 'Optimize I/O operations',
                'actions': [
                    'Use asynchronous I/O',
                    'Implement caching strategies',
                    'Optimize database queries',
                    'Use faster storage solutions'
                ]
            },
            'algorithmic_inefficiency': {
                'priority': 'medium',
                'title': 'Algorithm Optimization',
                'description': 'Optimize algorithmic complexity',
                'actions': [
                    'Profile to identify algorithmic hotspots',
                    'Use more efficient algorithms',
                    'Implement caching for frequently computed results',
                    'Optimize data structures'
                ]
            },
            'lock_contention': {
                'priority': 'medium',
                'title': 'Concurrency Optimization',
                'description': 'Reduce lock contention and improve parallelism',
                'actions': [
                    'Reduce lock scope',
                    'Use non-blocking data structures',
                    'Implement fine-grained locking',
                    'Use read-write locks where appropriate'
                ]
            }
        }
        
        return type_recommendations.get(bottleneck_type)


class HardwarePerformanceComparator:
    """Compare performance across different hardware configurations."""
    
    def __init__(self):
        self.hardware_profiles = {}
        
    def add_hardware_profile(self, hardware_name: str, profile_data: Dict[str, Any]):
        """Add a hardware profile for comparison."""
        self.hardware_profiles[hardware_name] = profile_data
        
    def compare_hardware(self, metrics: List[str] = None) -> Dict[str, Any]:
        """Compare performance across all hardware profiles."""
        if not self.hardware_profiles:
            return {'status': 'no_hardware_profiles'}
            
        if metrics is None:
            metrics = ['cpu_performance', 'memory_bandwidth', 'throughput', 'latency', 'energy_efficiency']
            
        comparison = {
            'hardware_names': list(self.hardware_profiles.keys()),
            'metric_comparison': {},
            'ranking': {}
        }
        
        # Compare each metric
        for metric in metrics:
            metric_values = {}
            for hardware_name, profile in self.hardware_profiles.items():
                if metric in profile:
                    metric_values[hardware_name] = profile[metric]
                    
            if metric_values:
                comparison['metric_comparison'][metric] = metric_values
                
        # Calculate rankings
                comparison['ranking'] = self._calculate_rankings(comparison['metric_comparison'])
                
        return comparison
        
    def _calculate_rankings(self, metric_comparison: Dict[str, Dict[str, float]]) -> Dict[str, Dict[str, int]]:
        """Calculate rankings for each metric."""
        rankings = {}
        
        for metric, values in metric_comparison.items():
            # Sort by value (descending for performance metrics)
            sorted_items = sorted(values.items(), key=lambda x: x[1], reverse=True)
            
            # Assign ranks (1 = best)
            rank_dict = {}
            for rank, (hardware, value) in enumerate(sorted_items, 1):
                rank_dict[hardware] = rank
                
            rankings[metric] = rank_dict
            
        return rankings
        
    def analyze_hardware_trends(self, profile_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze hardware performance trends."""
        trends = {
            'cpu_performance_trend': 'stable',
            'memory_efficiency_trend': 'stable',
            'throughput_trend': 'stable',
            'efficiency_improvements': []
        }
        
        # Analyze CPU performance trends
        cpu_perf = profile_data.get('cpu_performance', 0)
        if cpu_perf < 50:  # Low CPU performance
            trends['cpu_performance_trend'] = 'degrading'
            trends['efficiency_improvements'].append('Consider upgrading CPU or optimizing CPU-bound operations')
        elif cpu_perf > 150:  # High CPU performance
            trends['cpu_performance_trend'] = 'optimal'
            
        # Analyze memory efficiency trends
        memory_efficiency = profile_data.get('memory_efficiency', 0)
        if memory_efficiency < 50:
            trends['memory_efficiency_trend'] = 'degrading'
            trends['efficiency_improvements'].append('Optimize memory usage patterns and data structures')
        elif memory_efficiency > 150:
            trends['memory_efficiency_trend'] = 'optimal'
            
        return trends


class PerformanceAnalyzer:
    """Comprehensive performance analyzer for NLM brain simulations."""
    
    def __init__(self):
        self.bottleneck_detector = BottleneckDetector()
        self.hardware_comparator = HardwarePerformanceComparator()
        
    def analyze_performance(self, profile_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze performance data and identify bottlenecks.
        
        Args:
            profile_data: Performance profile data to analyze
            
        Returns:
            Analysis results with bottlenecks and recommendations
        """
        # Detect bottlenecks
        bottlenecks = self.bottleneck_detector.analyze_profile(profile_data)
        
        # Analyze optimization opportunities
        optimization_opportunities = self._analyze_optimization_opportunities(profile_data)
        
        # Generate recommendations
        recommendations = self.bottleneck_detector._generate_recommendations(bottlenecks)
        
        # Analyze hardware performance if available
        hardware_analysis = {}
        if 'hardware_info' in profile_data:
            hardware_analysis = self.hardware_comparator.analyze_hardware_trends(
                profile_data['hardware_info']
            )
            
        analysis_result = {
            'timestamp': datetime.now().isoformat(),
            'bottlenecks': bottlenecks,
            'optimization_opportunities': optimization_opportunities,
            'recommendations': recommendations,
            'hardware_analysis': hardware_analysis,
            'summary': {
                'total_bottlenecks': len(bottlenecks.get('detected_bottlenecks', [])),
                'critical_issues': len([b for b in bottlenecks.get('detected_bottlenecks', []) 
                                      if b.get('severity') == 'critical']),
                'high_priority_issues': len([b for b in bottlenecks.get('detected_bottlenecks', []) 
                                           if b.get('severity') == 'high']),
                'optimization_score': self._calculate_optimization_score(bottlenecks)
            }
        }
        
        return analysis_result
        
    def analyze_optimization_opportunities(self, profile_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze optimization opportunities from performance data."""
        opportunities = {
            'low_hanging_fruit': [],
            'medium_impact': [],
            'high_impact': [],
            'strategic_improvements': []
        }
        
        # Analyze based on profile data
        metrics = self._extract_metrics_from_profile(profile_data)
        
        # Low hanging fruit (easy wins)
        if metrics.get('cpu_efficiency', 0) < 0.5:
            opportunities['low_hanging_fruit'].append({
                'title': 'Improve CPU Efficiency',
                'impact': 'medium',
                'description': 'CPU utilization is below optimal levels',
                'effort': 'low',
                'recommendation': 'Profile CPU usage and optimize hotspots'
            })
            
        if metrics.get('memory_usage_percent', 0) > 70:
            opportunities['low_hanging_fruit'].append({
                'title': 'Optimize Memory Usage',
                'impact': 'medium',
                'description': 'Memory usage is above optimal levels',
                'effort': 'low',
                'recommendation': 'Implement object pooling and optimize data structures'
            })
            
        # Medium impact opportunities
        if metrics.get('avg_response_time_ms', 0) > 500:
            opportunities['medium_impact'].append({
                'title': 'Reduce Response Time',
                'impact': 'high',
                'description': 'Response times are above acceptable thresholds',
                'effort': 'medium',
                'recommendation': 'Optimize algorithms and implement caching'
            })
            
        if metrics.get('cache_hit_rate_percent', 0) < 80:
            opportunities['medium_impact'].append({
                'title': 'Improve Cache Performance',
                'impact': 'medium',
                'description': 'Cache hit rate is below optimal levels',
                'effort': 'medium',
                'recommendation': 'Optimize data access patterns and caching strategies'
            })
            
        # High impact opportunities
        if metrics.get('gc_overhead_percent', 0) > 30:
            opportunities['high_impact'].append({
                'title': 'Reduce Garbage Collection Overhead',
                'impact': 'high',
                'description': 'GC overhead is consuming significant CPU time',
                'effort': 'high',
                'recommendation': 'Implement object pooling and optimize memory allocation'
            })
            
        # Strategic improvements
        if metrics.get('throughput_variance', 0) > 0.5:
            opportunities['strategic_improvements'].append({
                'title': 'Stabilize Performance',
                'impact': 'medium',
                'description': 'High performance variance indicates instability',
                'effort': 'high',
                'recommendation': 'Identify and fix performance inconsistencies'
            })
            
        return opportunities
        
    def compare_hardware_performance(self, hardware_profiles: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare performance across different hardware configurations."""
        for i, profile in enumerate(hardware_profiles):
            self.hardware_comparator.add_hardware_profile(f"hardware_{i+1}", profile)
            
        comparison = self.hardware_comparator.compare_hardware()
        return comparison
        
    def generate_optimization_report(self, analysis_results: List[Dict[str, Any]], 
                                   output_path: str = None) -> str:
        """Generate comprehensive optimization report.
        
        Args:
            analysis_results: List of analysis results
            output_path: Output file path. If None, auto-generates.
            
        Returns:
            Path to generated report
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"performance_optimization_report_{timestamp}.html"
            
        html_content = self._create_optimization_report(analysis_results)
        with open(output_path, 'w') as f:
            f.write(html_content)
            
        print(f"Optimization report generated: {output_path}")
        return output_path
        
    def _create_optimization_report(self, analysis_results: List[Dict[str, Any]]) -> str:
        """Create optimization report HTML content."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Optimization Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }}
        .header {{ background-color: #6c5ce7; color: white; padding: 30px; border-radius: 5px; }}
        .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }}
        .metric-card {{ background: #f8f9fa; padding: 20px; border-radius: 5px; border-left: 4px solid #6c5ce7; }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #6c5ce7; }}
        .metric-label {{ color: #6c757d; font-size: 14px; }}
        .priority-critical {{ background-color: #ffebee; border-left-color: #f44336; }}
        .priority-high {{ background-color: #fff3e0; border-left-color: #ff9800; }}
        .priority-medium {{ background-color: #e3f2fd; border-left-color: #2196f3; }}
        .priority-low {{ background-color: #f5f5f5; border-left-color: #9e9e9e; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ border: 1px solid #ddd; padding: 12px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .bottleneck-critical {{ background-color: #ffcdd2; }}
        .bottleneck-high {{ background-color: #ffe0b2; }}
        .bottleneck-medium {{ background-color: #fff9c4; }}
        .bottleneck-low {{ background-color: #c8e6c9; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Optimization Report</h1>
        <p>Generated: {timestamp}</p>
        <p>Analysis Count: {len(analysis_results)}</p>
    </div>
    
    <div class="section">
        <h2>Overall Analysis Summary</h2>
        <div class="metric-grid">
    """
        
        # Calculate overall statistics
        total_bottlenecks = 0
        critical_issues = 0
        optimization_scores = []
        
        for result in analysis_results:
            summary = result.get('summary', {})
            total_bottlenecks += summary.get('total_bottlenecks', 0)
            critical_issues += summary.get('critical_issues', 0)
            optimization_scores.append(summary.get('optimization_score', 0))
            
        avg_optimization_score = statistics.mean(optimization_scores) if optimization_scores else 0
        
        html += f"""
            <div class="metric-card">
                <div class="metric-value">{total_bottlenecks}</div>
                <div class="metric-label">Total Bottlenecks</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{critical_issues}</div>
                <div class="metric-label">Critical Issues</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{avg_optimization_score:.1f}%</div>
                <div class="metric-label">Avg Optimization Score</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{len(analysis_results)}</div>
                <div class="metric-label">Analysis Sessions</div>
            </div>
        </div>
    </div>
    
    <div class="section">
        <h2>Detected Bottlenecks</h2>
    """
        
        # Display bottlenecks from all analyses
        for i, result in enumerate(analysis_results):
            bottlenecks = result.get('bottlenecks', {})
            detected = bottlenecks.get('detected_bottlenecks', [])
            
            if detected:
                html += f"""
        <h3>Analysis {i + 1} - {len(detected)} Bottlenecks Detected</h3>
        <table>
            <tr>
                <th>Type</th>
                <th>Severity</th>
                <th>Description</th>
                <th>Location</th>
                <th>Recommendation</th>
            </tr>
                """
                
                for bottleneck in detected:
                    severity = bottleneck.get('severity', 'low')
                    row_class = f"bottleneck-{severity}"
                    
                    html += f"""
                <tr class="{row_class}">
                    <td>{bottleneck.get('type', 'unknown')}</td>
                    <td>{severity}</td>
                    <td>{bottleneck.get('description', '')}</td>
                    <td>{bottleneck.get('location', '')}</td>
                    <td>{bottleneck.get('recommendation', '')}</td>
                </tr>
                    """
                    
                html += """
        </table>
                """
                
        html += """
    </div>
    
    <div class="section">
        <h2>Optimization Recommendations</h2>
    """
        
        # Display recommendations from all analyses
        all_recommendations = []
        for result in analysis_results:
            recommendations = result.get('recommendations', [])
            all_recommendations.extend(recommendations)
            
        # Group by priority
        priority_groups = defaultdict(list)
        for rec in all_recommendations:
            priority_groups[rec.get('priority', 'low')].append(rec)
            
        # Display by priority
        for priority in ['critical', 'high', 'medium', 'low']:
            recommendations = priority_groups.get(priority, [])
            if recommendations:
                html += f"""
        <div class="section priority-{priority}">
            <h3>{priority.upper()} Priority Recommendations ({len(recommendations)})</h3>
            <table>
                <tr>
                    <th>Title</th>
                    <th>Description</th>
                    <th>Actions</th>
                </tr>
                """
                
                for rec in recommendations:
                    html += f"""
                <tr>
                    <td><strong>{rec.get('title', '')}</strong></td>
                    <td>{rec.get('description', '')}</td>
                    <td><ul>
                """
                    
                    for action in rec.get('actions', []):
                        html += f"<li>{action}</li>"
                        
                    html += """
                    </ul></td>
                </tr>
                    """
                    
                html += """
            </table>
        </div>
                """
                
        html += """
    </div>
    
</body>
</html>
        """
        
        return html
        
    def _extract_metrics_from_profile(self, profile_data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract metrics from profile data."""
        metrics = {}
        
        # Extract from different sources
        if 'performance_profiler' in profile_data:
            metrics.update(profile_data['performance_profiler'])
        if 'memory_profiler' in profile_data:
            metrics.update(profile_data['memory_profiler'])
        if 'benchmark_suite' in profile_data:
            metrics.update(profile_data['benchmark_suite'])
            
        return metrics
        
    def _calculate_optimization_score(self, bottlenecks: Dict[str, Any]) -> float:
        """Calculate optimization score based on bottlenecks."""
        detected = bottlenecks.get('detected_bottlenecks', [])
        if not detected:
            return 100.0
            
        # Calculate based on severity distribution
        severity_weights = {'critical': 0.0, 'high': 0.3, 'medium': 0.6, 'low': 0.8, 'minimal': 1.0}
        total_penalty = 0
        
        for bottleneck in detected:
            severity = bottleneck.get('severity', 'minimal')
            total_penalty += severity_weights.get(severity, 1.0)
            
        max_penalty = len(detected) * severity_weights['critical']
        if max_penalty == 0:
            return 100.0
            
        optimization_score = max(0, 100 - (total_penalty / max_penalty) * 100)
        return optimization_score


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Brain Performance Analyzer")
    parser.add_argument("--profile-results", help="Path to performance profile results JSON file")
    parser.add_argument("--hardware-analysis", action="store_true", help="Analyze hardware performance comparison")
    parser.add_argument("--data-dir", help="Directory containing hardware performance data files")
    parser.add_argument("--optimize", action="store_true", help="Generate optimization recommendations")
    parser.add_argument("--target", nargs="+", choices=["cpu", "memory", "io", "algorithmic"], help="Target optimization areas")n    parser.add_argument("--trend-analysis", action="store_true", help="Analyze performance trends")
    parser.add_argument("--historical-data", help="Directory containing historical performance data")
    parser.add_argument("--report", action="store_true", help="Generate comprehensive performance report")
    parser.add_argument("--input", nargs="+", help="Input profile files for analysis")
    parser.add_argument("--output", help="Output report file path")
    parser.add_argument("--format", choices=["html", "json", "csv"], default="html", help="Report format")
    
    args = parser.parse_args()
    
    print("Starting NLM Performance Analysis...")
    
    analyzer = PerformanceAnalyzer()
    
    # Load profile data if specified
    profile_data = {}
    
    if args.profile_results:
        try:
            with open(args.profile_results, 'r') as f:
                profile_data = json.load(f)
        except Exception as e:
            print(f"Error loading profile results: {e}")
            sys.exit(1)
            
    if args.input:
        # Load multiple profile files
        combined_data = {}
        for input_file in args.input:
            try:
                with open(input_file, 'r') as f:
                    file_data = json.load(f)
                    combined_data.update(file_data)
            except Exception as e:
                print(f"Error loading {input_file}: {e}")
                continue
        profile_data.update(combined_data)
        
    # Run analysis based on arguments
    if args.profile_results or args.input:
        print("\nAnalyzing performance profile...")
        analysis_result = analyzer.analyze_performance(profile_data)
        
        # Generate report if requested
        if args.report or args.optimize:
            output_file = args.output or f"performance_analysis_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{args.format}"
            analyzer.generate_optimization_report([analysis_result], output_file)
            print(f"Performance analysis complete. Report saved to: {output_file}")
            
    if args.hardware_analysis or args.data_dir:
        print("\nAnalyzing hardware performance...")
        hardware_files = []
        
        if args.data_dir:
            data_dir = Path(args.data_dir)
            hardware_files = list(data_dir.glob("*.json"))
            
        hardware_profiles = []
        for hardware_file in hardware_files:
            try:
                with open(hardware_file, 'r') as f:
                    hardware_profiles.append(json.load(f))
            except Exception as e:
                print(f"Error loading {hardware_file}: {e}")
                continue
                
        if hardware_profiles:
            hardware_comparison = analyzer.compare_hardware_performance(hardware_profiles)
            print(f"Hardware comparison complete. Compared {len(hardware_profiles)} hardware profiles.")
            
    if args.trend_analysis or args.historical_data:
        print("\nAnalyzing performance trends...")
        historical_files = []
        
        if args.historical_data:
            hist_dir = Path(args.historical_data)
            historical_files = list(hist_dir.glob("*.json"))
            
        # Load historical data and analyze trends
        trend_data = []
        for hist_file in historical_files:
            try:
                with open(hist_file, 'r') as f:
                    trend_data.append(json.load(f))
            except Exception as e:
                print(f"Error loading {hist_file}: {e}")
                continue
                
        if trend_data:
            print(f"Loaded {len(trend_data)} historical data points for trend analysis.")
            # TODO: Implement trend analysis
            
    print("\nPerformance analysis complete!")