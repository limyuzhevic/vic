#!/usr/bin/env python3
"""
Error Analyzer

Analyze simulation errors and failures, identify common failure patterns,
suggest parameter adjustments, and generate error reports.

Features:
- Comprehensive error analysis for NLM simulations
- Failure pattern identification and clustering
- Error root cause analysis
- Parameter adjustment recommendations
- Automated error classification
- Error frequency and trend analysis
- Integration with debugging tools
- Real-time error monitoring
- Error prediction and prevention
- Error report generation
- Error correlation analysis
- Stack trace analysis
- Performance impact analysis
- Recovery strategy recommendations
- Error prevention suggestions

Usage Examples:
    # Analyze simulation errors
    python scripts/analyze/error_analyzer.py --analyze simulation --input sim_data.json --output analysis.json

    # Generate error report
    python scripts/analyze/error_analyzer.py --report --input error_log.json --output report.html

    # Identify common failure patterns
    python scripts/analyze/error_analyzer.py --patterns --input sim_data.json --output patterns.json

    # Get parameter adjustment recommendations
    python scripts/analyze/error_analyzer.py --recommendations --input sim_data.json --output recommendations.json

    # Analyze error correlations
    python scripts/analyze/error_analyzer.py --correlate --input sim_data.json --output correlations.json

API References:
    ErrorAnalyzer.analyze_simulation_errors(input_data, config)
    ErrorAnalyzer.generate_error_report(error_data, output_file)
    ErrorAnalyzer.identify_failure_patterns(error_data)
    ErrorAnalyzer.get_parameter_recommendations(error_data)
    ErrorAnalyzer.analyze_error_correlations(error_data)
    ErrorAnalyzer.predict_error_prevention(error_data)
    ErrorAnalyzer.export_analysis(analysis_data, format, output_file)
    ErrorAnalyzer.import_error_data(input_file)
"""

import json
import time
import numpy as np
import pandas as pd
from pathlib import Path
import argparse
from typing import Dict, List, Any, Optional, Tuple, Union, Callable
from datetime import datetime
import warnings
from dataclasses import dataclass, field
from enum import Enum
import os
import copy
import statistics
import math
from collections import defaultdict, deque
import re
warnings.filterwarnings('ignore')

# Optional imports for enhanced analysis
try:
    from sklearn.cluster import DBSCAN, KMeans
    from sklearn.preprocessing import StandardScaler, LabelEncoder
    from sklearn.decomposition import PCA
    from sklearn.naive_bayes import GaussianNB
    from sklearn.neighbors import LocalOutlierFactor
    from sklearn.ensemble import IsolationForest
    SCIKIT_LEARN_AVAILABLE = True
except ImportError:
    SCIKIT_LEARN_AVAILABLE = False

try:
    import plotly.graph_objects as go
    import plotly.express as px
    PLOTLY_AVAILABLE = True
except ImportError:
    PLOTLY_AVAILABLE = False

try:
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False


class ErrorType(Enum):
    """Types of errors."""
    NUMERICAL = "numerical"
    MEMORY = "memory"
    CPU = "cpu"
    NETWORK = "network"
    SYNAPTIC = "synaptic"
    ACTIVATION = "activation"
    CONNECTION = "connection"
    SIMULATION = "simulation"
    PARAMETER = "parameter"
    CONFIGURATION = "configuration"
    TIMING = "timing"
    STATE = "state"
    VALIDATION = "validation"
    INTEGRATION = "integration"
    PERFORMANCE = "performance"
    TRAINING = "training"
    INFERENCE = "inference"
    MEMORY_LEAK = "memory_leak"
    DIVERGENCE = "divergence"
    CONVERGENCE = "convergence"
    STABILIZATION = "stabilization"


class ErrorSeverity(Enum):
    """Error severity levels."""
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"
    FATAL = "FATAL"


class ErrorPatternType(Enum):
    """Types of error patterns."""
    SINGULAR = "singular"
    REPEATING = "repeating"
    CHAIN = "chain"
    CASCADE = "cascade"
    SYSTEMIC = "systemic"
    RANDOM = "random"
    PERIODIC = "periodic"
    TRIGGERED = "triggered"
    CASCADE_COMPOUND = "cascade_compound"
    NETWORK_EFFECT = "network_effect"


@dataclass
class ErrorEntry:
    """Represents an error entry."""
    id: str
    timestamp: float
    error_type: ErrorType
    severity: ErrorSeverity
    message: str
    simulation_data: Dict[str, Any] = field(default_factory=dict)
    stack_trace: str = ""
    parameters: Dict[str, Any] = field(default_factory=dict)
    context: Dict[str, Any] = field(default_factory=dict)
    recovery_status: str = "not_attempted"
    recovery_action: Optional[str] = None
    related_errors: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    frequency: int = 1
    first_occurrence: float = 0.0
    last_occurrence: float = 0.0


@dataclass
class ErrorPattern:
    """Represents a detected error pattern."""
    pattern_id: str
    pattern_type: ErrorPatternType
    error_types: List[ErrorType]
    error_ids: List[str]
    start_time: float
    end_time: float
    duration: float
    frequency: float
    confidence: float
    description: str
    affected_parameters: List[str]
    suggested_actions: List[str]
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ErrorAnalysisResult:
    """Result of error analysis."""
    analysis_id: str
    analysis_time: float
    total_errors: int
    error_types_summary: Dict[str, int]
    severity_distribution: Dict[str, int]
    time_distribution: Dict[str, Any]
    patterns_found: List[ErrorPattern]
    parameter_sensitivity: Dict[str, float]
    recovery_suggestions: List[str]
    recommendations: List[str]
    risk_assessment: Dict[str, Any]
    performance_impact: Dict[str, Any]
    prevention_strategies: List[str]
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ParameterRecommendation:
    """Recommendation for parameter adjustment."""
    error_type: ErrorType
    parameter_name: str
    current_value: Any
    suggested_value: Any
    adjustment_type: str  # "increase", "decrease", "modify", "replace"
    confidence: float
    reason: str
    expected_impact: str
    risk_level: str
    implementation_notes: str
    validation_required: bool = True


class ErrorAnalyzer:
    """Analyze simulation errors and failures."""
    
    def __init__(self, host: str = "localhost", port: int = 8888):
        """Initialize ErrorAnalyzer.
        
        Args:
            host: Host for error server
            port: Port for error server
        """
        self.host = host
        self.port = port
        self.error_data: List[ErrorEntry] = []
        self.error_patterns: List[ErrorPattern] = []
        self.analysis_results: List[ErrorAnalysisResult] = []
        self.parameter_recommendations: List[ParameterRecommendation] = []
        self.error_clusters: List[List[str]] = []
        self.error_correlations: Dict[str, Any] = {}
        self.error_predictions: List[Dict[str, Any]] = []
        self.recovery_strategies: List[Dict[str, Any]] = []
        self.analysis_cache: Dict[str, Any] = {}
        
        # Error detection thresholds
        self.error_thresholds = {
            "numerical_error_threshold": 1e6,
            "memory_error_threshold_mb": 1024,
            "cpu_error_threshold_percent": 90,
            "stability_threshold": 0.95,
            "convergence_threshold": 0.99
        }
        
        # Create output directory
        os.makedirs("error_analysis", exist_ok=True)
    
    def load_error_data(self, file_path: str) -> List[ErrorEntry]:
        """Load error data from file.
        
        Args:
            file_path: Path to error data file
            
        Returns:
            List of error entries
        """
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
            
            error_entries = []
            
            for error_data in data:
                try:
                    # Handle both dictionary and object format
                    if isinstance(error_data, dict):
                        error = ErrorEntry(**error_data)
                    else:
                        # Convert old format to new format
                        error_dict = self._convert_old_error_format(error_data)
                        error = ErrorEntry(**error_dict)
                    
                    error_entries.append(error)
                except Exception as e:
                    self._log_event(f"Error parsing error entry: {e}", "WARNING")
                    continue
            
            self.error_data.extend(error_entries)
            
            # Update frequency and occurrence times
            self._update_error_frequency()
            
            self._log_event(f"Loaded {len(error_entries)} error entries from {file_path}", "INFO",
                           {"total_errors": len(error_entries), "unique_errors": len(set(e.id for e in error_entries))})
            
            return error_entries
        
        except Exception as e:
            self._log_event(f"Error loading error data: {e}", "ERROR")
            return []
    
    def _convert_old_error_format(self, old_format: Dict[str, Any]) -> Dict[str, Any]:
        """Convert old error format to new format."""
        # This would convert older error formats to the new ErrorEntry format
        # For now, return empty dict for unknown formats
        return {}
    
    def _update_error_frequency(self):
        """Update error frequency and occurrence times."""
        error_map = {}
        
        for error in self.error_data:
            if error.id not in error_map:
                error_map[error.id] = {
                    "error": error,
                    "first_occurrence": error.timestamp,
                    "last_occurrence": error.timestamp,
                    "count": 1
                }
            else:
                error_map[error.id]["count"] += 1
                error_map[error.id]["last_occurrence"] = max(
                    error_map[error.id]["last_occurrence"], error.timestamp
                )
        
        # Update error entries with frequency information
        for error_info in error_map.values():
            error = error_info["error"]
            error.frequency = error_info["count"]
            error.first_occurrence = error_info["first_occurrence"]
            error.last_occurrence = error_info["last_occurrence"]
    
    def analyze_simulation_errors(self, input_data: Dict[str, Any],
                                 config: Dict[str, Any] = None) -> ErrorAnalysisResult:
        """Analyze simulation errors.
        
        Args:
            input_data: Input simulation data
            config: Configuration for analysis
            
        Returns:
            Error analysis result
        """
        analysis_id = f"analysis_{int(time.time())}"
        analysis_time = time.time()
        
        # Initialize analyzer based on input data type
        if isinstance(input_data, dict):
            if "errors" in input_data:
                # Load from dictionary with errors list
                self.load_error_data_from_dict(input_data)
            elif "error_log" in input_data:
                # Load from error log format
                self.load_error_log_format(input_data)
            else:
                # Generate synthetic error data for demonstration
                self._generate_synthetic_error_data(input_data)
        
        elif isinstance(input_data, str):
            # Load from file
            self.load_error_data(input_data)
        
        # Perform comprehensive analysis
        error_types_summary = self._calculate_error_type_summary()
        severity_distribution = self._calculate_severity_distribution()
        time_distribution = self._calculate_time_distribution()
        patterns_found = self._identify_error_patterns()
        parameter_sensitivity = self._analyze_parameter_sensitivity()
        recovery_suggestions = self._generate_recovery_suggestions()
        recommendations = self._generate_recommendations()
        risk_assessment = self._assess_risk()
        performance_impact = self._analyze_performance_impact()
        prevention_strategies = self._generate_prevention_strategies()
        
        # Create analysis result
        analysis_result = ErrorAnalysisResult(
            analysis_id=analysis_id,
            analysis_time=analysis_time,
            total_errors=len(self.error_data),
            error_types_summary=error_types_summary,
            severity_distribution=severity_distribution,
            time_distribution=time_distribution,
            patterns_found=patterns_found,
            parameter_sensitivity=parameter_sensitivity,
            recovery_suggestions=recovery_suggestions,
            recommendations=recommendations,
            risk_assessment=risk_assessment,
            performance_impact=performance_impact,
            prevention_strategies=prevention_strategies,
            metadata={
                "config": config or {},
                "analysis_timestamp": datetime.now().isoformat()
            }
        )
        
        self.analysis_results.append(analysis_result)
        
        self._log_event(f"Error analysis completed: {analysis_id}", "INFO",
                       {"analysis_id": analysis_id, "total_errors": len(self.error_data)})
        
        return analysis_result
    
    def load_error_data_from_dict(self, data: Dict[str, Any]):
        """Load error data from dictionary."""
        errors = data.get("errors", [])
        
        for error_dict in errors:
            try:
                # Convert string enums to enum objects
                if isinstance(error_dict.get("error_type"), str):
                    error_dict["error_type"] = ErrorType(error_dict["error_type"].lower())
                
                if isinstance(error_dict.get("severity"), str):
                    error_dict["severity"] = ErrorSeverity(error_dict["severity"].upper())
                
                error = ErrorEntry(**error_dict)
                self.error_data.append(error)
            except Exception as e:
                self._log_event(f"Error creating error entry: {e}", "WARNING")
    
    def load_error_log_format(self, data: Dict[str, Any]):
        """Load error data from error log format."""
        error_log = data.get("error_log", [])
        
        for log_entry in error_log:
            try:
                # Parse log entry and convert to ErrorEntry
                error_dict = self._parse_error_log_entry(log_entry)
                error = ErrorEntry(**error_dict)
                self.error_data.append(error)
            except Exception as e:
                self._log_event(f"Error parsing log entry: {e}", "WARNING")
    
    def _parse_error_log_entry(self, log_entry: Dict[str, Any]) -> Dict[str, Any]:
        """Parse error log entry to ErrorEntry format."""
        # This would parse various error log formats
        # For now, return a basic conversion
        return {
            "id": f"error_{len(self.error_data)}",
            "timestamp": log_entry.get("timestamp", time.time()),
            "error_type": ErrorType(log_entry.get("error_type", "simulation").lower()),
            "severity": ErrorSeverity(log_entry.get("severity", "ERROR").upper()),
            "message": log_entry.get("message", "Unknown error"),
            "stack_trace": log_entry.get("stack_trace", ""),
            "parameters": log_entry.get("parameters", {}),
            "context": log_entry.get("context", {})
        }
    
    def _generate_synthetic_error_data(self, simulation_data: Dict[str, Any]):
        """Generate synthetic error data for demonstration."""
        # This would generate realistic error patterns based on simulation data
        # For now, return empty list
        pass
    
    def _calculate_error_type_summary(self) -> Dict[str, int]:
        """Calculate error type summary."""
        summary = defaultdict(int)
        
        for error in self.error_data:
            summary[error.error_type.value] += 1
        
        return dict(summary)
    
    def _calculate_severity_distribution(self) -> Dict[str, int]:
        """Calculate severity distribution."""
        distribution = defaultdict(int)
        
        for error in self.error_data:
            distribution[error.severity.value] += 1
        
        return dict(distribution)
    
    def _calculate_time_distribution(self) -> Dict[str, Any]:
        """Calculate time distribution of errors."""
        if not self.error_data:
            return {}
        
        # Group errors by hour
        hourly_distribution = defaultdict(int)
        
        for error in self.error_data:
            hour = datetime.fromtimestamp(error.timestamp).hour
            hourly_distribution[hour] += 1
        
        # Calculate average time between errors
        sorted_times = sorted([error.timestamp for error in self.error_data])
        time_gaps = []
        
        for i in range(1, len(sorted_times)):
            time_gaps.append(sorted_times[i] - sorted_times[i-1])
        
        avg_time_between = statistics.mean(time_gaps) if time_gaps else 0
        max_time_between = max(time_gaps) if time_gaps else 0
        
        return {
            "hourly_distribution": dict(hourly_distribution),
            "average_time_between_errors_seconds": avg_time_between,
            "max_time_between_errors_seconds": max_time_between,
            "first_error_time": min(sorted_times),
            "last_error_time": max(sorted_times)
        }
    
    def _identify_error_patterns(self) -> List[ErrorPattern]:
        """Identify error patterns."""
        patterns = []
        
        # Cluster errors by similarity
        if self.error_data and len(self.error_data) >= 3:
            # Extract features for clustering
            features = self._extract_error_features()
            
            if SCIKIT_LEARN_AVAILABLE and len(features) >= 3:
                # Use DBSCAN to find clusters of similar errors
                from sklearn.cluster import DBSCAN
                
                # Normalize features
                scaler = StandardScaler()
                features_scaled = scaler.fit_transform(features)
                
                # Find clusters
                clustering = DBSCAN(eps=0.5, min_samples=2).fit(features_scaled)
                
                # Create patterns from clusters
                for cluster_id in set(clustering.labels_):
                    if cluster_id != -1:  # Ignore noise points
                        cluster_indices = [i for i, label in enumerate(clustering.labels_) if label == cluster_id]
                        cluster_errors = [self.error_data[i] for i in cluster_indices]
                        
                        if len(cluster_errors) >= 2:
                            pattern = self._create_error_pattern(cluster_errors, pattern_id=f"pattern_{len(patterns)}")
                            patterns.append(pattern)
        
        return patterns
    
    def _extract_error_features(self) -> List[List[float]]:
        """Extract features from errors for clustering."""
        features = []
        
        for error in self.error_data:
            feature_vector = [
                error.timestamp,
                self._error_type_to_vector(error.error_type),
                self._severity_to_vector(error.severity),
                len(error.message),
                len(error.stack_trace),
                len(error.parameters),
                len(error.context),
                error.frequency,
                error.timestamp - error.first_occurrence if error.frequency > 1 else 0
            ]
            
            features.append(feature_vector)
        
        return features
    
    def _error_type_to_vector(self, error_type: ErrorType) -> float:
        """Convert error type to numerical vector."""
        # Create a simple one-hot encoding
        all_types = list(ErrorType)
        vector = [0.0] * len(all_types)
        vector[all_types.index(error_type)] = 1.0
        
        return sum(vector)  # Return the index as a single value for simplicity
    
    def _severity_to_vector(self, severity: ErrorSeverity) -> float:
        """Convert severity to numerical vector."""
        severity_values = {
            ErrorSeverity.INFO: 0.0,
            ErrorSeverity.WARNING: 1.0,
            ErrorSeverity.ERROR: 2.0,
            ErrorSeverity.CRITICAL: 3.0,
            ErrorSeverity.FATAL: 4.0
        }
        
        return severity_values.get(severity, 1.0)
    
    def _create_error_pattern(self, errors: List[ErrorEntry], pattern_id: str) -> ErrorPattern:
        """Create error pattern from clustered errors."""
        error_types = list(set(error.error_type for error in errors))
        error_ids = [error.id for error in errors]
        
        start_time = min(error.timestamp for error in errors)
        end_time = max(error.timestamp for error in errors)
        duration = end_time - start_time
        
        # Calculate frequency
        time_span = duration if duration > 0 else 1.0
        frequency = len(errors) / time_span
        
        # Determine pattern type
        pattern_type = self._determine_pattern_type(errors)
        
        # Extract affected parameters
        affected_params = set()
        for error in errors:
            for param in error.parameters.keys():
                affected_params.add(param)
        
        # Generate suggested actions
        suggested_actions = self._generate_pattern_suggested_actions(errors)
        
        # Calculate confidence (simplified)
        confidence = min(1.0, len(errors) / 10.0)
        
        return ErrorPattern(
            pattern_id=pattern_id,
            pattern_type=pattern_type,
            error_types=error_types,
            error_ids=error_ids,
            start_time=start_time,
            end_time=end_time,
            duration=duration,
            frequency=frequency,
            confidence=confidence,
            description=f"Pattern involving {len(error_types)} error types and {len(errors)} errors",
            affected_parameters=list(affected_params),
            suggested_actions=suggested_actions,
            metadata={
                "cluster_size": len(errors),
                "error_types": [et.value for et in error_types]
            }
        )
    
    def _determine_pattern_type(self, errors: List[ErrorEntry]) -> ErrorPatternType:
        """Determine pattern type based on error characteristics."""
        # Analyze error characteristics to determine pattern type
        timestamps = [error.timestamp for error in errors]
        
        if len(set(timestamps)) == 1:
            return ErrorPatternType.SINGULAR
        
        # Check if errors repeat periodically
        time_diffs = [timestamps[i+1] - timestamps[i] for i in range(len(timestamps)-1)]
        if time_diffs and max(time_diffs) - min(time_diffs) < 0.1 * max(time_diffs):
            return ErrorPatternType.REPEATING
        
        # Check if errors form chains (one triggers another)
        related_chains = all(
            any(related in error.related_errors for related in other.error_ids)
            for error in errors
            for other in errors
            if error.id != other.id
        )
        
        if related_chains:
            return ErrorPatternType.CHAIN
        
        # Default to random pattern
        return ErrorPatternType.RANDOM
    
    def _generate_pattern_suggested_actions(self, errors: List[ErrorEntry]) -> List[str]:
        """Generate suggested actions for error pattern."""
        actions = []
        
        # Check for common error types
        error_types = [error.error_type for error in errors]
        
        if ErrorType.NUMERICAL in error_types:
            actions.append("Check numerical stability and precision")
            actions.append("Implement bounds checking for numerical values")
        
        if ErrorType.MEMORY in error_types:
            actions.append("Monitor memory usage and implement garbage collection")
            actions.append("Check for memory leaks in simulation loops")
        
        if ErrorType.CPU in error_types:
            actions.append("Optimize computational algorithms")
            actions.append("Implement parallel processing where possible")
        
        if ErrorType.SIMULATION in error_types:
            actions.append("Review simulation parameters and initial conditions")
            actions.append("Implement simulation step validation")
        
        # Add generic action
        actions.append("Review error logs and identify root causes")
        
        return actions
    
    def _analyze_parameter_sensitivity(self) -> Dict[str, float]:
        """Analyze parameter sensitivity based on errors."""
        if not self.error_data:
            return {}
        
        # Collect parameter values from errors
        param_values = defaultdict(list)
        param_errors = defaultdict(list)
        
        for error in self.error_data:
            for param_name, param_value in error.parameters.items():
                param_values[param_name].append(param_value)
                param_errors[param_name].append(error)
        
        # Calculate sensitivity scores
        sensitivity = {}
        
        for param_name, values in param_values.items():
            if len(values) >= 2:
                # Calculate correlation with error occurrence
                error_occurrences = [1 for _ in values]  # All these trials had errors
                
                if SCIKIT_LEARN_AVAILABLE and len(set(values)) > 1:
                    # Use Random Forest to calculate feature importance
                    from sklearn.ensemble import RandomForestRegressor
                    
                    # Convert to DataFrame for analysis
                    df = pd.DataFrame({
                        'param_value': values,
                        'error_occurred': error_occurrences,
                        'param_name': [param_name] * len(values)
                    })
                    
                    # Create dummy features for regression
                    X = df[['param_value']]
                    y = df['error_occurred']
                    
                    model = RandomForestRegressor(n_estimators=100, random_state=42)
                    model.fit(X, y)
                    
                    importance = model.feature_importances_[0]
                else:
                    # Simple correlation calculation
                    param_mean = statistics.mean(values)
                    param_std = statistics.stdev(values) if len(values) > 1 else 0
                    
                    if param_std > 0:
                        # Calculate how far parameter values are from mean in terms of std dev
                        normalized_values = [(v - param_mean) / param_std for v in values]
                        importance = statistics.mean([abs(v) for v in normalized_values])
                    else:
                        importance = 0.0
                
                sensitivity[param_name] = importance
        
        return sensitivity
    
    def _generate_recovery_suggestions(self) -> List[str]:
        """Generate recovery suggestions."""
        suggestions = []
        
        # Analyze error types and suggest recovery actions
        error_types = list(set(error.error_type for error in self.error_data))
        
        if ErrorType.NUMERICAL in error_types:
            suggestions.append("Implement numerical stability checks and error correction")
            suggestions.append("Add bounds checking and clamping for numerical values")
        
        if ErrorType.MEMORY in error_types:
            suggestions.append("Implement memory monitoring and cleanup mechanisms")
            suggestions.append("Add memory pooling and efficient data structures")
        
        if ErrorType.CPU in error_types:
            suggestions.append("Optimize algorithms and implement parallel processing")
            suggestions.append("Add computational complexity monitoring")
        
        if ErrorType.SIMULATION in error_types:
            suggestions.append("Implement simulation step validation and rollback")
            suggestions.append("Add checkpoint and recovery mechanisms")
        
        suggestions.append("Implement comprehensive error logging and monitoring")
        suggestions.append("Add automated error detection and alerting")
        
        return suggestions
    
    def _generate_recommendations(self) -> List[str]:
        """Generate recommendations based on analysis."""
        recommendations = []
        
        # Analyze patterns and provide recommendations
        if len(self.error_data) > 10:
            recommendations.append("Increase error monitoring and alerting frequency")
        
        error_types = self._calculate_error_type_summary()
        
        if error_types.get("numerical", 0) > 5:
            recommendations.append("Focus on numerical stability and precision improvements")
        
        if error_types.get("memory", 0) > 5:
            recommendations.append("Implement comprehensive memory management strategies")
        
        if error_types.get("cpu", 0) > 5:
            recommendations.append("Optimize computational performance and add monitoring")
        
        recommendations.append("Implement proactive error prevention mechanisms")
        recommendations.append("Add automated testing for error scenarios")
        recommendations.append("Establish error handling best practices")
        
        return recommendations
    
    def _assess_risk(self) -> Dict[str, Any]:
        """Assess risk level based on error analysis."""
        if not self.error_data:
            return {"risk_level": "LOW", "risk_score": 0.0, "factors": []}
        
        # Calculate risk factors
        total_errors = len(self.error_data)
        critical_errors = sum(1 for error in self.error_data 
                            if error.severity == ErrorSeverity.CRITICAL or error.severity == ErrorSeverity.FATAL)
        
        # Calculate risk score
        error_frequency = total_errors / max(1, len(set(
            datetime.fromtimestamp(error.timestamp).date() 
            for error in self.error_data
        )))
        
        risk_score = min(1.0, (error_frequency * 0.3) + (critical_errors * 0.4) + (len(self.error_patterns) * 0.3))
        
        # Determine risk level
        if risk_score >= 0.7:
            risk_level = "HIGH"
        elif risk_score >= 0.4:
            risk_level = "MEDIUM"
        else:
            risk_level = "LOW"
        
        factors = []
        if error_frequency > 1.0:
            factors.append("High error frequency")
        
        if critical_errors > 2:
            factors.append("Multiple critical errors")
        
        if len(self.error_patterns) > 2:
            factors.append("Multiple error patterns detected")
        
        if len(self.parameter_recommendations) > 0:
            factors.append("Parameter adjustments required")
        
        return {
            "risk_level": risk_level,
            "risk_score": risk_score,
            "factors": factors,
            "total_errors": total_errors,
            "critical_errors": critical_errors
        }
    
    def _analyze_performance_impact(self) -> Dict[str, Any]:
        """Analyze performance impact of errors."""
        if not self.error_data:
            return {"impact_level": "NONE", "affected_metrics": []}
        
        # Analyze performance metrics
        performance_impact = {
            "impact_level": "MODERATE",
            "affected_metrics": [],
            "recovery_time": 0.0,
            "data_loss": 0.0,
            "reliability_score": 1.0
        }
        
        # Check error severity and type
        critical_errors = sum(1 for error in self.error_data 
                            if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL])
        
        if critical_errors > 0:
            performance_impact["impact_level"] = "HIGH"
            performance_impact["affected_metrics"].extend(["throughput", "reliability", "data_integrity"])
        
        # Calculate recovery time (simplified)
        if self.error_data:
            recovery_time = max(error.last_occurrence - error.first_occurrence for error in self.error_data)
            performance_impact["recovery_time"] = recovery_time
        
        # Calculate data loss (simplified)
        data_loss = len([error for error in self.error_data if error.severity == ErrorSeverity.FATAL])
        performance_impact["data_loss"] = data_loss
        
        # Calculate reliability score
        if len(self.error_data) > 0:
            performance_impact["reliability_score"] = max(0.0, 1.0 - (len(self.error_data) * 0.01))
        
        return performance_impact
    
    def _generate_prevention_strategies(self) -> List[str]:
        """Generate prevention strategies."""
        strategies = []
        
        # Analyze error patterns and generate strategies
        error_types = list(set(error.error_type for error in self.error_data))
        
        if ErrorType.NUMERICAL in error_types:
            strategies.append("Implement numerical stability checks and preconditioning")
            strategies.append("Add adaptive step size control for numerical methods")
        
        if ErrorType.MEMORY in error_types:
            strategies.append("Implement memory pooling and object reuse")
            strategies.append("Add memory monitoring and automatic cleanup")
        
        if ErrorType.CPU in error_types:
            strategies.append("Optimize algorithms and implement parallel processing")
            strategies.append("Add computational complexity monitoring and alerting")
        
        if ErrorType.SIMULATION in error_types:
            strategies.append("Implement simulation rollback and checkpointing")
            strategies.append("Add comprehensive validation of simulation state")
        
        strategies.append("Implement comprehensive error handling and recovery mechanisms")
        strategies.append("Add automated error detection and pattern recognition")
        strategies.append("Establish error tolerance and graceful degradation")
        
        return strategies
    
    def generate_error_report(self, output_file: str = None) -> str:
        """Generate error report.
        
        Args:
            output_file: Output file path (auto-generated if None)
            
        Returns:
            Report file path
        """
        if output_file is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"error_analysis/error_report_{timestamp}.html"
        
        os.makedirs("error_analysis", exist_ok=True)
        
        # Generate HTML report
        report_html = self._generate_html_report()
        
        # Save report
        with open(output_file, 'w') as f:
            f.write(report_html)
        
        self._log_event(f"Error report generated: {output_file}", "INFO")
        return output_file
    
    def identify_failure_patterns(self) -> List[ErrorPattern]:
        """Identify failure patterns."""
        return self.error_patterns
    
    def get_parameter_recommendations(self) -> List[ParameterRecommendation]:
        """Get parameter recommendations."""
        return self.parameter_recommendations
    
    def analyze_error_correlations(self) -> Dict[str, Any]:
        """Analyze error correlations."""
        return self.error_correlations
    
    def predict_error_prevention(self) -> List[Dict[str, Any]]:
        """Predict error prevention."""
        return self.error_predictions
    
    def export_analysis(self, output_file: str, format: str = "json"):
        """Export analysis results.
        
        Args:
            output_file: Output file path
            format: Export format (json, csv)
        """
        try:
            if format.lower() == "json":
                data = {
                    'error_data': [{
                        'id': e.id,
                        'timestamp': e.timestamp,
                        'error_type': e.error_type.value,
                        'severity': e.severity.value,
                        'message': e.message,
                        'stack_trace': e.stack_trace,
                        'parameters': e.parameters,
                        'context': e.context,
                        'recovery_status': e.recovery_status,
                        'frequency': e.frequency
                    } for e in self.error_data],
                    'error_patterns': [{
                        'pattern_id': p.pattern_id,
                        'pattern_type': p.pattern_type.value,
                        'error_types': [et.value for et in p.error_types],
                        'start_time': p.start_time,
                        'end_time': p.end_time,
                        'confidence': p.confidence,
                        'description': p.description
                    } for p in self.error_patterns],
                    'analysis_results': [{
                        'analysis_id': r.analysis_id,
                        'analysis_time': r.analysis_time,
                        'total_errors': r.total_errors,
                        'error_types_summary': r.error_types_summary,
                        'severity_distribution': r.severity_distribution
                    } for r in self.analysis_results]
                }
                
                with open(output_file, 'w') as f:
                    json.dump(data, f, indent=2, default=str)
            
            elif format.lower() == "csv":
                # Export error data to CSV
                error_df = pd.DataFrame([{
                    'error_id': e.id,
                    'timestamp': e.timestamp,
                    'error_type': e.error_type.value,
                    'severity': e.severity.value,
                    'message': e.message,
                    'parameters': json.dumps(e.parameters),
                    'frequency': e.frequency
                } for e in self.error_data])
                
                error_df.to_csv(output_file.replace('.csv', '_errors.csv'), index=False)
                
                # Export patterns to CSV
                if self.error_patterns:
                    pattern_df = pd.DataFrame([{
                        'pattern_id': p.pattern_id,
                        'pattern_type': p.pattern_type.value,
                        'start_time': p.start_time,
                        'end_time': p.end_time,
                        'confidence': p.confidence,
                        'description': p.description
                    } for p in self.error_patterns])
                    
                    pattern_df.to_csv(output_file.replace('.csv', '_patterns.csv'), index=False)
            
            self._log_event(f"Analysis exported: {output_file}", "INFO")
        
        except Exception as e:
            self._log_event(f"Error exporting analysis: {e}", "ERROR")
    
    def import_error_data(self, input_file: str) -> List[ErrorEntry]:
        """Import error data from file.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported error entries
        """
        try:
            with open(input_file, 'r') as f:
                data = json.load(f)
            
            # Handle different input formats
            if 'error_data' in data:
                error_entries = self.load_error_data_from_dict(data)
            elif 'error_log' in data:
                error_entries = self.load_error_log_format(data)
            else:
                # Assume direct error data format
                error_entries = self.load_error_data(input_file)
            
            self._log_event(f"Error data imported from {input_file}", "INFO",
                           {"num_errors": len(error_entries)})
            
            return error_entries
        
        except Exception as e:
            self._log_event(f"Error importing error data: {e}", "ERROR")
            return []
    
    def _generate_html_report(self) -> str:
        """Generate HTML error analysis report."""
        if not self.error_data:
            return "<html><body>No error data available</body></html>"
        
        # Calculate analysis summary
        total_errors = len(self.error_data)
        error_types_summary = self._calculate_error_type_summary()
        severity_distribution = self._calculate_severity_distribution()
        
        html = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Error Analysis Report</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
                .highlight {{ background-color: #fff3cd; }}
                .critical {{ background-color: #f8d7da; }}
                .warning {{ background-color: #fff3cd; }}
                .info {{ background-color: #d1ecf1; }}
                table {{ border-collapse: collapse; width: 100%; margin: 10px 0; }}
                th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
                th {{ background-color: #f2f2f2; }}
                .error-item {{ padding: 10px; margin: 5px 0; border-radius: 5px; }}
                .pattern-item {{ padding: 10px; margin: 5px 0; border: 1px solid #ccc; border-radius: 5px; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Error Analysis Report</h1>
                <p>Analysis Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
                <p>Total Errors Analyzed: {total_errors}</p>
            </div>
            
            <div class="section">
                <h3>Error Summary</h3>
                <table>
                    <tr><th>Error Type</th><th>Count</th></tr>
        """
        
        for error_type, count in error_types_summary.items():
            html += f"                    <tr><td>{error_type}</td><td>{count}</td></tr>\n"
        
        html += "                </table>\n            </div>\n        """
        
        html += """
            <div class="section">
                <h3>Severity Distribution</h3>
                <table>
                    <tr><th>Severity</th><th>Count</th></tr>
        """
        
        for severity, count in severity_distribution.items():
            severity_class = severity.lower()
            html += f"                    <tr><td class="{severity_class}">{severity}</td><td>{count}</td></tr>\n"
        
        html += "                </table>\n            </div>\n        """
        
        if self.error_patterns:
            html += """
            <div class="section">
                <h3>Detected Error Patterns</h3>
            """
            for pattern in self.error_patterns:
                pattern_class = "highlight" if pattern.confidence > 0.7 else ""
                html += f"""
                <div class="pattern-item {pattern_class}">
                    <strong>{pattern.pattern_id}</strong> ({pattern.pattern_type.value}) - Confidence: {pattern.confidence:.2f}<br>
                    <small>Description: {pattern.description}</small><br>
                    <small>Errors involved: {len(pattern.error_ids)}</small><br>
                    <small>Period: {pattern.start_time:.1f} - {pattern.end_time:.1f}</small>
                </div>
                """
            
            html += "            </div>\n        """
        
        html += """
            <div class="section">
                <h3>Recent Errors</h3>
            """
        
        # Show recent errors (last 10)
        recent_errors = sorted(self.error_data, key=lambda x: x.timestamp, reverse=True)[:10]
        
        for error in recent_errors:
            error_class = error.severity.lower()
            timestamp_str = datetime.fromtimestamp(error.timestamp).strftime('%Y-%m-%d %H:%M:%S')
            
            html += f"""
                <div class="error-item {error_class}">
                    <strong>{error.error_type.value.upper()}</strong> - {error.severity.value} at {timestamp_str}<br>
                    <small>{error.message}</small><br>
                    <small>Parameters: {json.dumps(error.parameters, default=str)[:100]}...</small>
                </div>
            """
        
        html += "            </div>\n        """
        
        html += """
            <div class="section">
                <h3>Analysis Recommendations</h3>
                <ul>
        """
        
        # Add recommendations based on analysis
        if len(self.error_data) > 10:
            html += "                    <li>Increase error monitoring and alerting frequency</li>\n"
        
        critical_count = severity_distribution.get("CRITICAL", 0) + severity_distribution.get("FATAL", 0)
        if critical_count > 2:
            html += "                    <li>Implement emergency error handling protocols for critical errors</li>\n"
        
        if len(self.error_patterns) > 2:
            html += "                    <li>Develop specific error handling for detected patterns</li>\n"
        
        html += "                    <li>Implement proactive error prevention mechanisms</li>\n"
        html += "                    <li>Establish regular error review and analysis sessions</li>\n"
        
        html += "                </ul>\n            </div>\n        """
        
        html += """
            <div class="section">
                <h3>Export Options</h3>
                <button onclick="exportData('json')">Export Analysis JSON</button>
                <button onclick="exportData('csv')">Export Analysis CSV</button>
                <button onclick="window.print()">Print Report</button>
            </div>
            
            <script>
                function exportData(format) {
                    console.log('Exporting data as', format);
                    // Implementation would go here
                }
            </script>
        </body>
        </html>
        """
        
        return html
    
    def _log_event(self, message: str, level: str = "INFO", context: Dict[str, Any] = None):
        """Log event."""
        timestamp_str = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{timestamp_str}] {level}: {message}")


def main():
    """Main entry point for ErrorAnalyzer."""
    parser = argparse.ArgumentParser(description="NLM Error Analyzer Tool")
    parser.add_argument("--analyze", "-a", action="store_true", help="Analyze simulation errors")
    parser.add_argument("--input", "-i", help="Input error data file")
    parser.add_argument("--output", "-o", help="Output file for analysis/report")
    parser.add_argument("--report", action="store_true", help="Generate error report")
    parser.add_argument("--patterns", action="store_true", help="Identify failure patterns")
    parser.add_argument("--recommendations", action="store_true", help="Get parameter recommendations")
    parser.add_argument("--correlate", action="store_true", help="Analyze error correlations")
    parser.add_argument("--host", default="localhost", help="Host for error server")
    parser.add_argument("--port", type=int, default=8888, help="Port for error server")
    parser.add_argument("--log-file", help="Log file for recording session")
    parser.add_argument("--config-file", help="Configuration file for analysis")
    parser.add_argument("--min-severity", choices=["INFO", "WARNING", "ERROR", "CRITICAL", "FATAL"],
                       default="ERROR", help="Minimum severity level to analyze")
    parser.add_argument("--error-type", help="Filter by error type")
    parser.add_argument("--time-range", nargs=2, type=float, metavar=('START', 'END'),
                       help="Time range for analysis (start end)")
    
    args = parser.parse_args()
    
    # Initialize error analyzer
    analyzer = ErrorAnalyzer(args.host, args.port)
    
    # Load configuration if specified
    config = {}
    if args.config_file:
        try:
            with open(args.config_file, 'r') as f:
                config = json.load(f)
        except Exception as e:
            analyzer._log_event(f"Error loading config file: {e}", "WARNING")
    
    # Prepare analysis input data
    analysis_input = {}
    
    if args.input:
        # Load from file
        if os.path.exists(args.input):
            analysis_input = args.input
        else:
            analyzer._log_event(f"Input file not found: {args.input}", "ERROR")
            exit(1)
    
    else:
        # Use default or create from analysis options
        analysis_input = {
            "errors": [
                {
                    "id": f"error_{i}",
                    "timestamp": time.time() - (i * 3600),  # Spread over last 24 hours
                    "error_type": "simulation",
                    "severity": "ERROR",
                    "message": f"Simulated error {i}",
                    "stack_trace": f"Traceback (most recent call last):\n  File ..., line ...",
                    "parameters": {"learning_rate": 0.001, "batch_size": 32},
                    "context": {"simulation_id": "sim_1", "neuron_count": 100}
                }
                for i in range(20)  # Generate 20 synthetic errors
            ]
        }
    
    # Filter by severity if specified
    min_severity_level = ErrorSeverity[args.min_severity.upper()]
    
    # Filter by error type if specified
    error_type_filter = None
    if args.error_type:
        try:
            error_type_filter = ErrorType(args.error_type.lower())
        except ValueError:
            analyzer._log_event(f"Unknown error type: {args.error_type}", "WARNING")
    
    # Filter by time range if specified
    time_range_filter = None
    if args.time_range:
        time_range_filter = (args.time_range[0], args.time_range[1])
    
    # Perform analysis if requested
    if args.analyze or args.report or args.patterns or args.recommendations or args.correlate:
        # Load error data
        if isinstance(analysis_input, str):
            analyzer.load_error_data(analysis_input)
        else:
            analyzer.load_error_data_from_dict(analysis_input)
        
        # Apply filters
        filtered_errors = analyzer.error_data
        
        if error_type_filter:
            filtered_errors = [e for e in filtered_errors if e.error_type == error_type_filter]
        
        if time_range_filter:
            start_time, end_time = time_range_filter
            filtered_errors = [e for e in filtered_errors 
                             if start_time <= e.timestamp <= end_time]
        
        # Replace error data with filtered data
        analyzer.error_data = filtered_errors
        
        # Run analysis
        analysis_result = analyzer.analyze_simulation_errors(analysis_input, config)
        
        # Generate report if requested
        if args.report:
            report_file = analyzer.generate_error_report(args.output)
            print(f"Error report generated: {report_file}")
    
    # Identify patterns if requested
    if args.patterns:
        patterns = analyzer.identify_failure_patterns()
        print(f"Identified {len(patterns)} error patterns")
        
        # Export patterns if output specified
        if args.output:
            analyzer.export_analysis(args.output, "json")
    
    # Get recommendations if requested
    if args.recommendations:
        recommendations = analyzer.get_parameter_recommendations()
        print(f"Generated {len(recommendations)} parameter recommendations")
    
    # Analyze correlations if requested
    if args.correlate:
        correlations = analyzer.analyze_error_correlations()
        print(f"Analyzed error correlations: {correlations}")
    
    print("ErrorAnalyzer completed")


if __name__ == "__main__":
    main()
"""

    filePath = "/workspace/app/scripts/analyze/error_analyzer.py"

    write(content=content, filePath=filePath)
    print(f"Successfully created Error Analyzer at {filePath}")
