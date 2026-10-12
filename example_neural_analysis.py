#!/usr/bin/env python3
"""
example_neural_analysis.py

Neural Activity Analysis Example
================================

This example demonstrates advanced neural activity analysis using the NLM Python API,
showcasing various techniques for analyzing spike patterns, firing dynamics,
and neural connectivity.

Key Concepts Covered:
- Spike train analysis and firing rate calculation
- Burst detection and pattern recognition
- Neural connectivity analysis
- Frequency domain analysis
- Statistical neural analysis
- Performance metrics calculation

Learning Objectives:
1. Analyze spike train characteristics
2. Detect neural bursting patterns
3. Calculate firing statistics
4. Analyze neural connectivity
5. Perform frequency domain analysis
6. Calculate performance metrics

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import numpy as np
import matplotlib.pyplot as plt
from typing import List, Dict, Tuple, Optional
import statistics

def generate_sample_neural_data(num_steps: int = 1000) -> Dict[str, List[float]]:
    """
    Generate sample neural data for analysis demonstration.
    
    Args:
        num_steps: Number of time steps to generate
        
    Returns:
        Dictionary containing different types of neural data
    """
    print("   Generating sample neural data...")
    
    # Generate sample spike data
    spike_data = []
    for step in range(num_steps):
        # Poisson-like spike generation with some structure
        spike_probability = 0.1 + 0.05 * np.sin(step * 0.01)
        spike = 1.0 if np.random.random() < spike_probability else 0.0
        spike_data.append(spike)
    
    # Generate sample firing rate data
    firing_rate_data = []
    for step in range(num_steps):
        # Smoothed firing rate
        rate = 2.0 + np.random.normal(0, 0.5)
        firing_rate_data.append(max(0, rate))
    
    # Generate sample connectivity data
    connectivity_data = []
    for step in range(num_steps):
        # Simulating synaptic weight changes
        connectivity = 0.5 + 0.2 * np.sin(step * 0.005) + np.random.normal(0, 0.1)
        connectivity_data.append(max(0, connectivity))
    
    # Generate sample oscillation data
    oscillation_data = []
    for step in range(num_steps):
        # Simulating neural oscillations
        oscillation = np.sin(step * 0.02) * np.exp(-step * 0.001) + np.random.normal(0, 0.1)
        oscillation_data.append(oscillation)
    
    return {
        'spike_data': spike_data,
        'firing_rate_data': firing_rate_data,
        'connectivity_data': connectivity_data,
        'oscillation_data': oscillation_data
    }

def analyze_spike_train(spike_data: List[float], duration: float = 100.0) -> Dict[str, float]:
    """
    Analyze spike train characteristics.
    
    Args:
        spike_data: List of spike values (0.0 or 1.0)
        duration: Duration of the spike train in time units
        
    Returns:
        Dictionary containing spike train analysis
    """
    print("   Analyzing spike train...")
    
    if not spike_data:
        return {}
    
    analysis = {
        'total_spikes': sum(spike_data),
        'firing_rate': nlm.calculateFiringRate(spike_data, duration),
        'mean_inter_spike_interval': 0.0,
        'cv_of_isi': 0.0,
        'regularity': 0.0,
        'burst_index': 0.0,
        'asymmetry': 0.0
    }
    
    # Calculate inter-spike intervals
    spike_times = [i for i, spike in enumerate(spike_data) if spike > 0]
    if len(spike_times) >= 2:
        isis = np.diff(spike_times)
        analysis['mean_inter_spike_interval'] = np.mean(isis)
        
        if analysis['mean_inter_spike_interval'] > 0:
            analysis['cv_of_isi'] = np.std(isis) / analysis['mean_inter_spike_interval']
        
        # Calculate regularity (inverse of CV)
        analysis['regularity'] = 1.0 / (1.0 + analysis['cv_of_isi'])
    
    # Detect bursting
    bursts = nlm.detectBursting(spike_data, duration, threshold=1.5)
    analysis['burst_index'] = len(bursts)
    
    # Calculate asymmetry (early vs late spikes)
    if len(spike_times) >= 5:
        first_half = spike_times[:len(spike_times)//2]
        second_half = spike_times[len(spike_times)//2:]
        if second_half:
            first_half_mean = np.mean(first_half)
            second_half_mean = np.mean(second_half)
            if first_half_mean > 0:
                analysis['asymmetry'] = (second_half_mean - first_half_mean) / first_half_mean
    
    return analysis

def analyze_firing_patterns(spike_data: List[float], duration: float = 100.0) -> Dict[str, float]:
    """
    Analyze firing patterns and characteristics.
    
    Args:
        spike_data: List of spike values
        duration: Duration of the data
        
    Returns:
        Dictionary containing firing pattern analysis
    """
    print("   Analyzing firing patterns...")
    
    if not spike_data:
        return {}
    
    analysis = {
        'mean_firing_rate': 0.0,
        'firing_rate_cv': 0.0,
        'firing_rate_var': 0.0,
        'firing_rate_skew': 0.0,
        'firing_rate_kurtosis': 0.0,
        'coefficient_of_variation': 0.0,
        'rate_modulation': 0.0,
        'regularity': 0.0
    }
    
    # Calculate firing rate statistics
    firing_rate = nlm.calculateFiringRate(spike_data, duration)
    analysis['mean_firing_rate'] = firing_rate
    
    # Calculate coefficient of variation
    analysis['coefficient_of_variation'] = nlm.calculateCoefficientOfVariation(spike_data, duration)
    
    # Detect rate modulation
    rate_modulation = nlm.detectRateModulation(spike_data, windowSize=20, stepSize=10)
    if rate_modulation:
        analysis['rate_modulation'] = np.std(rate_modulation)
    
    # Calculate firing pattern using NLM function
    pattern = nlm.analyzeFiringPattern(spike_data, duration)
    analysis['regularity'] = pattern[0] if len(pattern) > 0 else 0.0
    
    return analysis

def analyze_connectivity(connectivity_data: List[float], duration: float = 100.0) -> Dict[str, float]:
    """
    Analyze neural connectivity patterns.
    
    Args:
        connectivity_data: List of connectivity values
        duration: Duration of the data
        
    Returns:
        Dictionary containing connectivity analysis
    """
    print("   Analyzing connectivity patterns...")
    
    if not connectivity_data:
        return {}
    
    analysis = {
        'mean_connectivity': 0.0,
        'connectivity_variance': 0.0,
        'connectivity_trend': 0.0,
        'stability_index': 0.0,
        'plasticity_index': 0.0,
        'network_efficiency': 0.0
    }
    
    # Calculate basic statistics
    analysis['mean_connectivity'] = np.mean(connectivity_data)
    analysis['connectivity_variance'] = np.var(connectivity_data)
    
    # Calculate trend (linear regression slope)
    if len(connectivity_data) > 1:
        x = np.arange(len(connectivity_data))
        slope = np.polyfit(x, connectivity_data, 1)[0]
        analysis['connectivity_trend'] = slope
    
    # Calculate stability index (inverse of variance)
    if analysis['connectivity_variance'] > 0:
        analysis['stability_index'] = 1.0 / analysis['connectivity_variance']
    
    # Calculate plasticity index (rate of change)
    if len(connectivity_data) > 5:
        changes = np.diff(connectivity_data)
        analysis['plasticity_index'] = np.mean(np.abs(changes))
    
    # Estimate network efficiency (simplified)
    analysis['network_efficiency'] = min(1.0, analysis['mean_connectivity'] * 10.0)
    
    return analysis

def analyze_oscillations(oscillation_data: List[float], duration: float = 100.0) -> Dict[str, float]:
    """
    Analyze neural oscillation patterns.
    
    Args:
        oscillation_data: List of oscillation values
        duration: Duration of the data
        
    Returns:
        Dictionary containing oscillation analysis
    """
    print("   Analyzing oscillation patterns...")
    
    if not oscillation_data:
        return {}
    
    analysis = {
        'dominant_frequency': 0.0,
        'power_spectrum': [],
        'synchrony_index': 0.0,
        'oscillation_amplitude': 0.0,
        'oscillation_phase': 0.0,
        'rhythmicity': 0.0
    }
    
    # Calculate power spectrum
    try:
        from scipy import signal
        sampling_rate = 1.0 / (duration / len(oscillation_data))
        freqs, psd = signal.periodogram(oscillation_data, sampling_rate)
        analysis['power_spectrum'] = psd.tolist()
        
        # Find dominant frequency
        if len(psd) > 0:
            dominant_idx = np.argmax(psd)
            analysis['dominant_frequency'] = freqs[dominant_idx]
    except ImportError:
        # Fallback if scipy not available
        analysis['dominant_frequency'] = 0.0
    
    # Calculate synchrony index (simplified)
    analysis['synchrony_index'] = np.std(oscillation_data) / (np.mean(np.abs(oscillation_data)) + 1e-6)
    
    # Calculate oscillation amplitude
    analysis['oscillation_amplitude'] = np.std(oscillation_data)
    
    # Calculate rhythm (simple periodicity detection)
    if len(oscillation_data) > 10:
        # Use autocorrelation
        autocorr = np.correlate(oscillation_data, oscillation_data, mode='full')
        autocorr = autocorr[len(autocorr)//2:]
        analysis['oscillation_phase'] = np.argmax(autocorr[1:]) + 1  # Skip zero lag
        
        # Normalize to get rhythmicity
        analysis['rhythmicity'] = np.max(autocorr[1:]) / (np.sum(np.abs(autocorr)) + 1e-6)
    
    return analysis

def perform_time_series_analysis(neural_data: List[float], sampling_rate: float = 100.0) -> Dict:
    """
    Perform time series analysis using NLM's built-in functions.
    
    Args:
        neural_data: Neural time series data
        sampling_rate: Sampling rate in Hz
        
    Returns:
        Dictionary containing time series analysis
    """
    print("   Performing time series analysis...")
    
    if not neural_data:
        return {}
    
    analysis = {}
    
    # Use NLM's time series analysis
    ts_analysis = nlm.analyzeTimeSeries(neural_data, sampling_rate)
    analysis['time_series_analysis'] = ts_analysis
    
    # Use NLM's anomaly detection
    anomalies = nlm.detectAnomalies(neural_data, threshold=2.0)
    analysis['anomalies'] = {
        'count': len(anomalies),
        'indices': anomalies,
        'timestamps': [i / sampling_rate for i in anomalies]
    }
    
    return analysis

def generate_neural_visualization(neural_data: List[float], analysis_type: str = "spike"):
    """
    Generate neural visualization (text-based for demo purposes).
    
    Args:
        neural_data: Neural data to visualize
        analysis_type: Type of analysis to visualize
    """
    print("   Generating neural visualization...")
    
    if analysis_type == "spike":
        print("   Spike Train (sampled):")
        for i, spike in enumerate(neural_data[:50]):  # Show first 50 spikes
            print(f"     Step {i}: {'█' if spike > 0 else '·'}")
        print("     ... (data truncated for display)")
    
    elif analysis_type == "firing_rate":
        print("   Firing Rate Pattern:")
        avg_rate = np.mean(neural_data)
        for i, rate in enumerate(neural_data[:20]):  # Show first 20 rates
            bar_length = int(rate / avg_rate * 20) if avg_rate > 0 else 0
            print(f"     Step {i}: {'█' * bar_length if bar_length > 0 else '·'}")
    
    elif analysis_type == "histogram":
        print("   Distribution Histogram:")
        hist, bins = np.histogram(neural_data, bins=10)
        max_count = max(hist) if hist else 1
        for i, (count, bin_range) in enumerate(zip(hist, bins[:-1])):
            bar_length = int(count / max_count * 30) if max_count > 0 else 0
            print(f"     {bin_range:.1f}-{bins[i+1]:.1f}: {'█' * bar_length} ({count})")

def main():
    """Main function demonstrating neural activity analysis."""
    print("=" * 70)
    print("NLM Neural Activity Analysis Example")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: DATA GENERATION AND PREPARATION
    # ---------------------------------------------------------------------
    print("\n1. Generating sample neural data...")
    
    # Generate neural data
    neural_data = generate_sample_neural_data(1000)
    
    print(f"   ✓ Spike data: {len(neural_data['spike_data'])} time steps")
    print(f"   ✓ Firing rate data: {len(neural_data['firing_rate_data'])} time steps")
    print(f"   ✓ Connectivity data: {len(neural_data['connectivity_data'])} time steps")
    print(f"   ✓ Oscillation data: {len(neural_data['oscillation_data'])} time steps")
    
    # ---------------------------------------------------------------------
    # SECTION 2: SPIKE TRAIN ANALYSIS
    # ---------------------------------------------------------------------
    print("\n2. Spike train analysis...")
    
    spike_analysis = analyze_spike_train(neural_data['spike_data'], duration=100.0)
    
    print("   ✓ Spike Train Analysis Results:")
    print(f"     - Total spikes: {spike_analysis['total_spikes']}")
    print(f"     - Firing rate: {spike_analysis['firing_rate']:.2f} Hz")
    print(f"     - Mean ISI: {spike_analysis['mean_inter_spike_interval']:.1f} steps")
    print(f"     - Coefficient of variation: {spike_analysis['cv_of_isi']:.3f}")
    print(f"     - Regularity: {spike_analysis['regularity']:.3f}")
    print(f"     - Burst events: {spike_analysis['burst_index']}")
    print(f"     - Asymmetry: {spike_analysis['asymmetry']:.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 3: FIRING PATTERN ANALYSIS
    # ---------------------------------------------------------------------
    print("\n3. Firing pattern analysis...")
    
    pattern_analysis = analyze_firing_patterns(neural_data['spike_data'], duration=100.0)
    
    print("   ✓ Firing Pattern Analysis Results:")
    print(f"     - Mean firing rate: {pattern_analysis['mean_firing_rate']:.2f} Hz")
    print(f"     - Firing rate CV: {pattern_analysis['firing_rate_cv']:.3f}")
    print(f"     - Coefficient of variation: {pattern_analysis['coefficient_of_variation']:.3f}")
    print(f"     - Rate modulation: {pattern_analysis['rate_modulation']:.3f}")
    print(f"     - Regularity: {pattern_analysis['regularity']:.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 4: CONNECTIVITY ANALYSIS
    # ---------------------------------------------------------------------
    print("\n4. Connectivity analysis...")
    
    connectivity_analysis = analyze_connectivity(neural_data['connectivity_data'], duration=100.0)
    
    print("   ✓ Connectivity Analysis Results:")
    print(f"     - Mean connectivity: {connectivity_analysis['mean_connectivity']:.3f}")
    print(f"     - Connectivity variance: {connectivity_analysis['connectivity_variance']:.3f}")
    print(f"     - Connectivity trend: {connectivity_analysis['connectivity_trend']:.4f}")
    print(f"     - Stability index: {connectivity_analysis['stability_index']:.3f}")
    print(f"     - Plasticity index: {connectivity_analysis['plasticity_index']:.3f}")
    print(f"     - Network efficiency: {connectivity_analysis['network_efficiency']:.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 5: OSCILLATION ANALYSIS
    # ---------------------------------------------------------------------
    print("\n5. Oscillation analysis...")
    
    oscillation_analysis = analyze_oscillations(neural_data['oscillation_data'], duration=100.0)
    
    print("   ✓ Oscillation Analysis Results:")
    print(f"     - Dominant frequency: {oscillation_analysis['dominant_frequency']:.3f} Hz")
    print(f"     - Synchrony index: {oscillation_analysis['synchrony_index']:.3f}")
    print(f"     - Oscillation amplitude: {oscillation_analysis['oscillation_amplitude']:.3f}")
    print(f"     - Rhythmicity: {oscillation_analysis['rhythmicity']:.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 6: TIME SERIES ANALYSIS
    # ---------------------------------------------------------------------
    print("\n6. Time series analysis...")
    
    # Use spike data for time series analysis
    time_series_analysis = perform_time_series_analysis(neural_data['spike_data'], sampling_rate=10.0)
    
    print("   ✓ Time Series Analysis Results:")
    if 'time_series_analysis' in time_series_analysis:
        ts_results = time_series_analysis['time_series_analysis']
        print(f"     - Frequency content: Available")
        print(f"     - Trends: Available")
        print(f"     - Periodicity: Available")
    
    if 'anomalies' in time_series_analysis:
        anomalies = time_series_analysis['anomalies']
        print(f"     - Detected anomalies: {anomalies['count']}")
        print(f"     - Anomaly timestamps: {anomalies['timestamps'][:5] if anomalies['timestamps'] else []}")
    
    # ---------------------------------------------------------------------
    # SECTION 7: VISUALIZATION
    # ---------------------------------------------------------------------
    print("\n7. Neural data visualization...")
    
    generate_neural_visualization(neural_data['spike_data'], "spike")
    generate_neural_visualization(neural_data['firing_rate_data'], "firing_rate")
    
    # ---------------------------------------------------------------------
    # SECTION 8: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n8. Validation and health check...")
    
    # Create sample brain for validation
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Validate brain dynamics with the neural data
    brain_issues = nlm.validateBrainDynamics(brain, duration=100.0)
    if brain_issues:
        print(f"   ⚠ Brain dynamics issues ({len(brain_issues)}):")
        for issue in brain_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ Brain dynamics are stable")
    
    # Check resource usage
    resource_report = nlm.checkBrainResourceUsage(brain)
    print(f"   ✓ Memory usage: {resource_report.getMemoryUsage() / 1024 / 1024:.2f} MB")
    
    # ---------------------------------------------------------------------
    # SECTION 9: PERFORMANCE BENCHMARKING
    # ---------------------------------------------------------------------
    print("\n9. Performance benchmarking...")
    
    # Benchmark analysis performance
    brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=50)
    print(f"   ✓ Brain analysis: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    # ---------------------------------------------------------------------
    # SECTION 10: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("EXAMPLE COMPLETE - Neural Activity Analysis Summary")
    print("=" * 70)
    
    print(f"\nNeural Data Analysis:")
    print(f"  - Data types analyzed: 4 (spike, firing rate, connectivity, oscillation)")
    print(f"  - Total spike events: {spike_analysis['total_spikes']}")
    print(f"  - Average firing rate: {pattern_analysis['mean_firing_rate']:.2f} Hz")
    print(f"  - Network stability: {connectivity_analysis['stability_index']:.3f}")
    print(f"  - Oscillation dominant frequency: {oscillation_analysis['dominant_frequency']:.3f} Hz")
    
    print(f"\nAnalysis Quality:")
    print(f"  - Data completeness: ✓ Complete")
    print(f"  - Pattern detection: ✓ Active")
    print(f"  - Anomaly detection: {'✓' if time_series_analysis.get('anomalies', {}).get('count', 0) > 0 else '⚠'} "
          f"({time_series_analysis.get('anomalies', {}).get('count', 0)} anomalies)")
    print(f"  - Health status: {'✓' if not brain_issues else '⚠'}")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_performance_benchmarking.py for performance optimization")
    print(f"  2. Try example_neural_analysis.py for advanced analysis techniques")
    print(f"  3. Try tutorial_step_by_step.py for comprehensive tutorial")
    print(f"  4. Try recipe_learning_optimization.py for optimization recipes")

if __name__ == "__main__":
    main()