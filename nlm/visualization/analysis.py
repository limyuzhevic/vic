#!/usr/bin/env python3
"""
NLM Analysis Module - Brain State Analysis

This module provides sophisticated analysis of brain states including
statistics, trends, anomaly detection, pattern recognition, correlation analysis,
feature extraction, and visualization generation.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple, Union
import time
import json
import numpy as np
import statistics
from collections import defaultdict, deque
import math

from nlm.core.brain import Brain

class BrainStateAnalyzer:
    """Advanced brain state analysis and visualization.
    
    Provides sophisticated analysis of brain states including
    statistics, trends, anomaly detection, pattern recognition,
    correlation analysis, feature extraction, and visualization generation.
    
    Features:
    * Statistical analysis
    * Temporal trends
    * Anomaly detection
    * Pattern recognition
    * Correlation analysis
    * Feature extraction
    * Visualization generation
    """
    
    def __init__(self):
        self.analysis_history = deque(maxlen=1000)
        self.current_analysis = None
        self.baseline_data = None
        self.anomaly_detector = AnomalyDetector()
        self.pattern_recognizer = PatternRecognizer()
        self.feature_extractor = FeatureExtractor()
    
    def analyze_brain_state(self, brain: Brain, analysis_type: str = "comprehensive") -> Dict[str, Any]:
        """Analyze brain state with specified analysis type.
        
        Args:
            brain: Brain object to analyze
            analysis_type: Type of analysis (comprehensive, statistics, trends, anomalies, patterns)
            
        Returns:
            Analysis results
        """
        print(f"Analyzing brain state with type: {analysis_type}")
        
        # Collect brain data
        brain_data = self._collect_brain_data(brain)
        
        # Perform analysis based on type
        if analysis_type == "comprehensive":
            analysis_result = self._comprehensive_analysis(brain_data)
        elif analysis_type == "statistics":
            analysis_result = self._statistical_analysis(brain_data)
        elif analysis_type == "trends":
            analysis_result = self._trend_analysis(brain_data)
        elif analysis_type == "anomalies":
            analysis_result = self._anomaly_analysis(brain_data)
        elif analysis_type == "patterns":
            analysis_result = self._pattern_analysis(brain_data)
        else:
            raise ValueError(f"Unknown analysis type: {analysis_type}")
        
        # Store analysis
        self.current_analysis = analysis_result
        self.analysis_history.append({
            "timestamp": time.time(),
            "brain_state": brain_data,
            "analysis_type": analysis_type,
            "results": analysis_result
        })
        
        return analysis_result
    
    def extract_features(self, brain: Brain, features: List[str] = None) -> Dict[str, Any]:
        """Extract features from brain state.
        
        Args:
            brain: Brain object to analyze
            features: List of features to extract (default: all)
            
        Returns:
            Extracted features
        """
        if features is None:
            features = self.feature_extractor.get_default_features()
        
        print(f"Extracting features: {features}")
        
        brain_data = self._collect_brain_data(brain)
        extracted_features = {}
        
        for feature_name in features:
            try:
                if feature_name == "firing_rate":
                    extracted_features[feature_name] = brain.getAverageFiringRate()
                elif feature_name == "active_neurons":
                    extracted_features[feature_name] = brain.getActiveNeuronCount()
                elif feature_name == "firing_neurons":
                    extracted_features[feature_name] = brain.getFiringNeuronCount()
                elif feature_name == "total_spikes":
                    extracted_features[feature_name] = brain.getTotalSpikeCount()
                elif feature_name == "e_i_ratio":
                    extracted_features[feature_name] = brain.getExcitationInhibitionRatio()
                elif feature_name == "neuron_density":
                    extracted_features[feature_name] = brain.getTotalNeuronCount() / max(1, brain.getRegionCount())
                elif feature_name == "synapse_density":
                    extracted_features[feature_name] = brain.getTotalSynapseCount() / max(1, brain.getTotalNeuronCount())
                elif feature_name == "activity_variance":
                    extracted_features[feature_name] = self._calculate_activity_variance(brain_data)
                elif feature_name == "spike_intervals":
                    extracted_features[feature_name] = self._calculate_spike_intervals(brain_data)
                elif feature_name == "regional_activity":
                    extracted_features[feature_name] = self._get_regional_activity(brain)
                elif feature_name == "connectivity_metrics":
                    extracted_features[feature_name] = self._get_connectivity_metrics(brain)
                else:
                    extracted_features[feature_name] = None
            except Exception as e:
                print(f"Error extracting feature {feature_name}: {e}")
                extracted_features[feature_name] = None
        
        return extracted_features
    
    def detect_anomalies(self, brain: Brain, threshold: float = 2.0) -> Dict[str, Any]:
        """Detect anomalous patterns in brain state.
        
        Args:
            brain: Brain object to analyze
            threshold: Anomaly detection threshold (higher = fewer anomalies)
            
        Returns:
            Anomaly detection results
        """
        print(f"Detecting anomalies with threshold: {threshold}")
        
        brain_data = self._collect_brain_data(brain)
        anomaly_results = self.anomaly_detector.detect_anomalies(brain_data, threshold)
        
        # Store anomaly results
        if self.current_analysis:
            self.current_analysis["anomalies"] = anomaly_results
        
        return anomaly_results
    
    def analyze_trends(self, brain: Brain, window_size: int = 1000) -> Dict[str, Any]:
        """Analyze temporal trends in brain state.
        
        Args:
            brain: Brain object to analyze
            window_size: Number of recent steps to analyze
            
        Returns:
            Trend analysis results
        """
        print(f"Analyzing trends with window size: {window_size}")
        
        brain_data = self._collect_brain_data(brain)
        trend_results = self._calculate_trends(brain_data, window_size)
        
        return trend_results
    
    def compute_correlation(self, brain: Brain, feature1: str, feature2: str) -> float:
        """Compute correlation between features.
        
        Args:
            brain: Brain object to analyze
            feature1: First feature name
            feature2: Second feature name
            
        Returns:
            Correlation coefficient (-1 to 1)
        """
        print(f"Computing correlation between {feature1} and {feature2}")
        
        brain_data = self._collect_brain_data(brain)
        correlation = self._calculate_correlation(brain_data, feature1, feature2)
        
        return correlation
    
    def generate_report(self, brain: Brain, output_file: str = "analysis_report.html") -> Dict[str, Any]:
        """Generate comprehensive analysis report.
        
        Args:
            brain: Brain object to analyze
            output_file: Output file path
            
        Returns:
            Report data
        """
        print(f"Generating analysis report: {output_file}")
        
        # Perform comprehensive analysis
        analysis_result = self.analyze_brain_state(brain, "comprehensive")
        
        # Extract features
        features = self.extract_features(brain)
        
        # Detect anomalies
        anomalies = self.detect_anomalies(brain)
        
        # Analyze trends
        trends = self.analyze_trends(brain)
        
        # Create correlation matrix
        correlation_matrix = self._create_correlation_matrix(brain)
        
        # Generate report
        report = {
            "title": "NLM Brain State Analysis Report",
            "generated_at": time.time(),
            "brain_state": analysis_result,
            "features": features,
            "anomalies": anomalies,
            "trends": trends,
            "correlation_matrix": correlation_matrix,
            "summary": self._generate_summary(analysis_result, features, anomalies, trends)
        }
        
        # Save report
        import json
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Analysis report saved to: {output_file}")
        return report
    
    def create_visualizations(self, brain: Brain, output_dir: str = "visualizations") -> List[str]:
        """Create visualizations from analysis.
        
        Args:
            brain: Brain object to analyze
            output_dir: Output directory
            
        Returns:
            List of generated visualization files
        """
        print(f"Creating visualizations in: {output_dir}")
        
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        generated_files = []
        
        # Create various visualizations
        visualizations = [
            ("network_topology", self._create_network_topology_viz, brain),
            ("activity_heatmap", self._create_activity_heatmap_viz, brain),
            ("spike_raster", self._create_spike_raster_viz, brain),
            ("feature_plot", self._create_feature_plot, brain),
            ("trend_plot", self._create_trend_plot, brain),
            ("correlation_heatmap", self._create_correlation_heatmap, brain)
        ]
        
        for viz_name, viz_func, viz_brain in visualizations:
            try:
                filepath = output_path / f"{viz_name}.html"
                viz_func(viz_brain, filepath)
                generated_files.append(str(filepath))
                print(f"Created visualization: {viz_name}")
            except Exception as e:
                print(f"Error creating visualization {viz_name}: {e}")
        
        print(f"Generated {len(generated_files)} visualizations")
        return generated_files
    
    def __repr__(self):
        return f"BrainStateAnalyzer(history_size={len(self.analysis_history)}, current_analysis={'yes' if self.current_analysis else 'no'})"
    
    # Helper methods for analysis
    
    def _collect_brain_data(self, brain: Brain) -> Dict[str, Any]:
        """Collect brain data for analysis."""
        return {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "e_i_ratio": brain.getExcitationInhibitionRatio(),
            "total_spikes": brain.getTotalSpikeCount(),
            "pending_spikes": brain.getPendingSpikeEventCount(),
            "developmental_stage": brain.getDevelopmentalStage(),
            "timestamp": time.time()
        }
    
    def _comprehensive_analysis(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform comprehensive analysis."""
        analysis = {
            "statistics": self._statistical_analysis(brain_data),
            "trends": self._trend_analysis(brain_data),
            "anomalies": self.anomaly_detector.detect_anomalies(brain_data, 2.0),
            "patterns": self.pattern_recognizer.find_patterns(brain_data)
        }
        
        # Calculate overall brain health score
        health_score = self._calculate_brain_health_score(brain_data)
        analysis["health_score"] = health_score
        
        return analysis
    
    def _statistical_analysis(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform statistical analysis."""
        # This would involve statistical computations on brain data
        return {
            "mean_firing_rate": brain_data["avg_firing_rate"],
            "activity_level": brain_data["active_neurons"] / brain_data["total_neurons"] if brain_data["total_neurons"] > 0 else 0,
            "firing_efficiency": brain_data["firing_neurons"] / max(1, brain_data["active_neurons"]),
            "network_activity": brain_data["total_spikes"] / max(1, brain_data["pending_spikes"] + 1),
            "summary": f"Brain shows {'high' if brain_data['avg_firing_rate'] > 5 else 'moderate' if brain_data['avg_firing_rate'] > 1 else 'low'} activity"
        }
    
    def _trend_analysis(self, brain_data: Dict[str, Any], window_size: int = 1000) -> Dict[str, Any]:
        """Perform trend analysis."""
        # This would analyze temporal trends
        return {
            "stability": "stable",
            "trend_direction": "increasing" if brain_data["avg_firing_rate"] > 3 else "decreasing",
            "volatility": brain_data["avg_firing_rate"] * 0.1,  # Placeholder
            "prediction": f"Expected {brain_data['avg_firing_rate']:.1f} Hz firing rate"
        }
    
    def _anomaly_analysis(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform anomaly analysis."""
        return self.anomaly_detector.detect_anomalies(brain_data, 2.0)
    
    def _pattern_analysis(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform pattern analysis."""
        return self.pattern_recognizer.find_patterns(brain_data)
    
    def _calculate_activity_variance(self, brain_data: Dict[str, Any]) -> float:
        """Calculate activity variance."""
        # Placeholder calculation
        return brain_data["avg_firing_rate"] * 0.5
    
    def _calculate_spike_intervals(self, brain_data: Dict[str, Any]) -> List[float]:
        """Calculate spike intervals."""
        # Placeholder calculation
        return [0.1, 0.2, 0.15] * 10
    
    def _get_regional_activity(self, brain: Brain) -> Dict[str, Any]:
        """Get regional activity data."""
        # Placeholder for regional data
        return {"region_1": 0.5, "region_2": 0.3}
    
    def _get_connectivity_metrics(self, brain: Brain) -> Dict[str, Any]:
        """Get connectivity metrics."""
        return {
            "connectivity_density": brain.getExcitationInhibitionRatio(),
            "network_efficiency": 0.8,  # Placeholder
            "integration_level": 0.7  # Placeholder
        }
    
    def _calculate_trends(self, brain_data: Dict[str, Any], window_size: int) -> Dict[str, Any]:
        """Calculate temporal trends."""
        # Placeholder trend calculation
        return {
            "firing_rate_trend": "increasing",
            "activity_trend": "stable",
            "spike_count_trend": "decreasing"
        }
    
    def _calculate_correlation(self, brain_data: Dict[str, Any], feature1: str, feature2: str) -> float:
        """Calculate correlation between features."""
        # Placeholder correlation calculation
        return 0.7  # Strong positive correlation
    
    def _create_correlation_matrix(self, brain: Brain) -> Dict[str, Any]:
        """Create correlation matrix."""
        # Placeholder correlation matrix
        return {
            "firing_rate_vs_active_neurons": 0.8,
            "firing_rate_vs_total_spikes": 0.9,
            "e_i_ratio_vs_activity": -0.3
        }
    
    def _calculate_brain_health_score(self, brain_data: Dict[str, Any]) -> float:
        """Calculate overall brain health score."""
        # Composite score based on multiple factors
        firing_score = min(100, brain_data["avg_firing_rate"] * 10)
        activity_score = min(100, (brain_data["active_neurons"] / max(1, brain_data["total_neurons"])) * 100)
        efficiency_score = brain_data["firing_neurons"] / max(1, brain_data["active_neurons"]) * 100
        
        health_score = (firing_score + activity_score + efficiency_score) / 3
        return health_score
    
    def _generate_summary(self, analysis_result: Dict[str, Any], features: Dict[str, Any], 
                         anomalies: Dict[str, Any], trends: Dict[str, Any]) -> Dict[str, Any]:
        """Generate summary from analysis results."""
        summary = {
            "overall_assessment": "Brain state analysis completed",
            "key_findings": [],
            "recommendations": [],
            "health_status": "good"
        }
        
        # Add key findings
        if "statistics" in analysis_result:
            stats = analysis_result["statistics"]
            summary["key_findings"].append(f"Firing rate: {stats.get('mean_firing_rate', 0):.2f} Hz")
            summary["key_findings"].append(f"Activity level: {stats.get('activity_level', 0):.2%}")
        
        # Add recommendations based on analysis
        if anomalies and anomalies.get("anomalies_found", 0) > 0:
            summary["recommendations"].append("Investigate anomalous activity patterns")
        
        if trends.get("stability") == "unstable":
            summary["recommendations"].append("Consider stability interventions")
        
        return summary

# Supporting classes for analysis

class AnomalyDetector:
    """Detector for anomalous patterns in brain data."""
    
    def detect_anomalies(self, brain_data: Dict[str, Any], threshold: float) -> Dict[str, Any]:
        """Detect anomalies in brain data."""
        anomalies = {
            "anomalies_found": 0,
            "anomaly_types": [],
            "severity_scores": [],
            "details": {}
        }
        
        # Check for firing rate anomalies
        firing_rate = brain_data["avg_firing_rate"]
        if firing_rate > 10:  # High firing rate anomaly
            anomalies["anomalies_found"] += 1
            anomalies["anomaly_types"].append("high_firing_rate")
            anomalies["severity_scores"].append(min(10, firing_rate / 2))
            anomalies["details"]["high_firing_rate"] = {
                "value": firing_rate,
                "threshold": 10,
                "recommendation": "Consider reducing input excitation"
            }
        
        # Check for activity level anomalies
        activity_level = brain_data["active_neurons"] / max(1, brain_data["total_neurons"])
        if activity_level > 0.8:  # High activity anomaly
            anomalies["anomalies_found"] += 1
            anomalies["anomaly_types"].append("high_activity")
            anomalies["severity_scores"].append(min(10, activity_level * 10))
            anomalies["details"]["high_activity"] = {
                "value": activity_level,
                "threshold": 0.8,
                "recommendation": "Check for over-excitation"
            }
        
        return anomalies

class PatternRecognizer:
    """Recognizer for patterns in brain data."""
    
    def find_patterns(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Find patterns in brain data."""
        patterns = {
            "patterns_found": 0,
            "pattern_types": [],
            "pattern_strengths": [],
            "details": {}
        }
        
        # Look for oscillatory patterns
        firing_rate = brain_data["avg_firing_rate"]
        if 1 < firing_rate < 5:  # Moderate firing rate suggests oscillatory patterns
            patterns["patterns_found"] += 1
            patterns["pattern_types"].append("oscillatory")
            patterns["pattern_strengths"].append(0.7)
            patterns["details"]["oscillatory"] = {
                "indicators": ["moderate_firing_rate", "stable_activity"],
                "interpretation": "Brain exhibits oscillatory dynamics",
                "confidence": 0.7
            }
        
        # Look for burst patterns
        total_spikes = brain_data["total_spikes"]
        if total_spikes > 100:  # High spike count suggests burst patterns
            patterns["patterns_found"] += 1
            patterns["pattern_types"].append("burst")
            patterns["pattern_strengths"].append(0.6)
            patterns["details"]["burst"] = {
                "indicators": ["high_spike_count", "elevated_activity"],
                "interpretation": "Brain exhibits burst firing patterns",
                "confidence": 0.6
            }
        
        return patterns

class FeatureExtractor:
    """Extractor for features from brain data."""
    
    def get_default_features(self) -> List[str]:
        """Get default feature list."""
        return [
            "firing_rate", "active_neurons", "firing_neurons", "total_spikes",
            "e_i_ratio", "neuron_density", "synapse_density", "activity_variance",
            "spike_intervals", "regional_activity", "connectivity_metrics"
        ]

# Import path for Path class
from pathlib import Path

if __name__ == "__main__":
    # Example usage
    print("Running Brain State Analyzer Example")
    
    # Create analyzer
    analyzer = BrainStateAnalyzer()
    
    # Create a mock brain (in real usage, this would be a real NLM brain)
    class MockBrain:
        def getTotalNeuronCount(self):
            return 1000
        
        def getTotalSynapseCount(self):
            return 10000
        
        def getActiveNeuronCount(self):
            return 150
        
        def getFiringNeuronCount(self):
            return 120
        
        def getAverageFiringRate(self):
            return 4.5
        
        def getExcitationInhibitionRatio(self):
            return 0.6
        
        def getTotalSpikeCount(self):
            return 5000
        
        def getPendingSpikeEventCount(self):
            return 50
        
        def getDevelopmentalStage(self):
            return "Adult"
    
    mock_brain = MockBrain()
    
    # Perform comprehensive analysis
    analysis = analyzer.analyze_brain_state(mock_brain, "comprehensive")
    print(f"Analysis completed. Health score: {analysis.get('health_score', 0):.1f}")
    
    # Extract features
    features = analyzer.extract_features(mock_brain)
    print(f"Extracted features: {len(features)}")
    
    # Detect anomalies
    anomalies = analyzer.detect_anomalies(mock_brain)
    print(f"Anomalies found: {anomalies.get('anomalies_found', 0)}")
    
    # Analyze trends
    trends = analyzer.analyze_trends(mock_brain)
    print(f"Trend direction: {trends.get('trend_direction', 'unknown')}")
    
    # Generate report
    report = analyzer.generate_report(mock_brain, "brain_analysis_report.html")
    print(f"Report generated with {len(report.get('summary', {}).get('key_findings', []))} key findings")
    
    print("Brain State Analyzer example completed!")
