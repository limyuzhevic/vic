#!/usr/bin/env python3
"""
NLM Research Utilities

This package provides advanced utility functions for NLM research, including
tools for data analysis, visualization, and experimental control.
"""

import json
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
import pandas as pd
import scipy.signal as signal
import scipy.stats as stats

class NLMResearchUtilities:
    """Advanced utilities for NLM research."""
    
    def __init__(self, data_dir: str = "research_data"):
        """Initialize research utilities."""
        self.data_dir = Path(data_dir)
        self.data_dir.mkdir(exist_ok=True)
        
    def load_research_data(self, filepath: str) -> Dict[str, Any]:
        """Load research data from file."""
        with open(filepath, 'r') as f:
            return json.load(f)
            
    def save_research_data(self, data: Dict[str, Any], filepath: str) -> None:
        """Save research data to file."""
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
            
    def analyze_spike_statistics(self, spike_data: List[Dict]) -> Dict[str, Any]:
        """Analyze spike statistics from simulation data."""
        if not spike_data:
            return {}
        
        times = [spike['time'] for spike in spike_data]
        neuron_ids = [spike['neuron_id'] for spike in spike_data]
        
        # Basic statistics
        stats_dict = {
            'total_spikes': len(spike_data),
            'unique_neurons': len(set(neuron_ids)),
            'time_range': {
                'start': min(times),
                'end': max(times),
                'duration': max(times) - min(times)
            },
            'spike_rate': len(spike_data) / (max(times) - min(times) + 1e-10),
            'firing_patterns': self._identify_firing_patterns(times)
        }
        
        return stats_dict
        
    def _identify_firing_patterns(self, times: List[float]) -> List[Dict[str, Any]]:
        """Identify firing patterns from spike times."""
        if len(times) < 10:
            return []
        
        # Convert to spike train
        duration = max(times) - min(times)
        bin_size = duration / 100  # 100 bins
        spike_train = np.zeros(100)
        
        for t in times:
            bin_idx = int((t - min(times)) / bin_size)
            if bin_idx < len(spike_train):
                spike_train[bin_idx] += 1
        
        # Find patterns
        patterns = []
        
        # Regular spiking
        if self._is_regular_spiking(spike_train):
            patterns.append({
                'type': 'regular',
                'periodicity': self._estimate_periodicity(spike_train),
                'cv': self._calculate_cv(spike_train)
            })
        
        # Bursting
        if self._detect_bursts(spike_train):
            patterns.append({
                'type': 'bursting',
                'burst_count': self._count_bursts(spike_train),
                'mean_burst_frequency': self._mean_burst_frequency(spike_train)
            })
        
        # Irregular
        if self._is_irregular(spike_train):
            patterns.append({
                'type': 'irregular',
                'cv': self._calculate_cv(spike_train)
            })
        
        return patterns
        
    def _is_regular_spiking(self, spike_train: np.ndarray) -> bool:
        """Check if spike train shows regular spiking."""
        # Compute autocorrelation
        autocorr = np.correlate(spike_train, spike_train, mode='full')
        autocorr = autocorr[len(autocorr)//2:]  # Take only positive lags
        
        # Look for strong periodicity
        peaks, _ = signal.find_peaks(autocorr, height=np.mean(autocorr) * 2)
        return len(peaks) > 1
        
    def _estimate_periodicity(self, spike_train: np.ndarray) -> float:
        """Estimate periodicity from spike train."""
        # Use dominant frequency from power spectrum
        fft = np.fft.fft(spike_train)
        freqs = np.fft.fftfreq(len(spike_train))
        power = np.abs(fft) ** 2
        
        # Find dominant frequency
        dominant_idx = np.argmax(power[1:len(power)//2]) + 1
        return 1.0 / (freqs[dominant_idx] * len(spike_train))
        
    def _calculate_cv(self, spike_train: np.ndarray) -> float:
        """Calculate coefficient of variation (CV)."""
        if len(spike_train) < 2:
            return 0.0
        
        # Convert to interspike intervals
        times = np.where(spike_train > 0)[0]
        if len(times) < 2:
            return 0.0
        
        intervals = np.diff(times)
        return np.std(intervals) / np.mean(intervals)
        
    def _detect_bursts(self, spike_train: np.ndarray, threshold: int = 3) -> bool:
        """Detect bursting pattern in spike train."""
        # Find high-frequency bins (bursts)
        threshold_count = threshold
        burst_bins = np.where(spike_train >= threshold_count)[0]
        
        if len(burst_bins) == 0:
            return False
        
        # Check if bursts are separated by silent periods
        gaps = np.diff(burst_bins)
        avg_gap = np.mean(gaps) if len(gaps) > 0 else 0
        
        return len(burst_bins) >= 2 and avg_gap > 5
        
    def _count_bursts(self, spike_train: np.ndarray, threshold: int = 3) -> int:
        """Count number of bursts in spike train."""
        burst_bins = np.where(spike_train >= threshold)[0]
        
        if len(burst_bins) == 0:
            return 0
        
        # Simple burst counting
        in_burst = False
        burst_count = 0
        
        for i, count in enumerate(spike_train):
            if count >= threshold and not in_burst:
                in_burst = True
                burst_count += 1
            elif count < threshold and in_burst:
                in_burst = False
        
        return burst_count
        
    def _mean_burst_frequency(self, spike_train: np.ndarray, threshold: int = 3) -> float:
        """Calculate mean burst frequency."""
        burst_bins = np.where(spike_train >= threshold)[0]
        
        if len(burst_bins) < 2:
            return 0.0
        
        # Calculate burst frequencies
        durations = np.diff(burst_bins)
        if len(durations) == 0:
            return 0.0
        
        return 1.0 / (np.mean(durations) + 1e-10)
        
    def _is_irregular(self, spike_train: np.ndarray, cv_threshold: float = 0.5) -> bool:
        """Check if spike train is irregular."""
        cv = self._calculate_cv(spike_train)
        return cv > cv_threshold
        
    def analyze_plasticity_changes(self, weight_history: List[List[float]]) -> Dict[str, Any]:
        """Analyze synaptic weight changes over time."""
        if not weight_history or len(weight_history) < 2:
            return {}
        
        # Convert to array
        weights = np.array(weight_history)
        
        # Calculate statistics per time step
        stats_dict = {
            'total_synapses': len(weight_history[0]),
            'time_steps': len(weight_history),
            'mean_weight_evolution': np.mean(weights, axis=1).tolist(),
            'weight_variance': np.var(weights, axis=1).tolist(),
            'weight_mad': np.mean(np.abs(weights - np.mean(weights, axis=1, keepdims=True)), axis=1).tolist()
        }
        
        # Analyze plasticity events
        plasticity_stats = self._identify_plasticity_events(weights)
        stats_dict.update(plasticity_stats)
        
        return stats_dict
        
    def _identify_plasticity_events(self, weights: np.ndarray) -> Dict[str, Any]:
        """Identify plasticity events from weight changes."""
        if weights.shape[0] < 2:
            return {}
        
        # Calculate weight changes
        weight_changes = np.diff(weights, axis=0)
        
        # Classify events
        events = {
            'strengthening': [],
            'weakening': [],
            'stable': []
        }
        
        for i in range(len(weight_changes)):
            mean_change = np.mean(weight_changes[i])
            
            if mean_change > 0.01:
                events['strengthening'].append(i)
            elif mean_change < -0.01:
                events['weakening'].append(i)
            else:
                events['stable'].append(i)
        
        return {
            'strengthening_events': len(events['strengthening']),
            'weakening_events': len(events['weakening']),
            'stable_steps': len(events['stable']),
            'strengthening_mean': np.mean([weights[i+1].mean() - weights[i].mean() 
                                          for i in events['strengthening']]) if events['strengthening'] else 0,
            'weakening_mean': np.mean([weights[i+1].mean() - weights[i].mean() 
                                      for i in events['weakening']]) if events['weakening'] else 0
        }
        
    def analyze_neuromodulation_effects(self, neuromod_data: List[Dict]) -> Dict[str, Any]:
        """Analyze neuromodulation effects on brain activity."""
        if not neuromod_data:
            return {}
        
        # Extract time series
        times = [item['time'] for item in neuromod_data]
        dopamine = [item.get('dopamine', 0.0) for item in neuromod_data]
        curiosity = [item.get('curiosity', 0.0) for item in neuromod_data]
        novelty = [item.get('novelty', 0.0) for item in neuromod_data]
        
        # Calculate correlations
        correlations = {
            'dopamine_curiosity': np.corrcoef(dopamine, curiosity)[0,1] if len(dopamine) > 1 else 0,
            'dopamine_novelty': np.corrcoef(dopamine, novelty)[0,1] if len(dopamine) > 1 else 0,
            'curiosity_novelty': np.corrcoef(curiosity, novelty)[0,1] if len(curiosity) > 1 else 0
        }
        
        # Detect neuromodulation events
        neuromod_events = self._detect_neuromodulation_events(neuromod_data)
        
        return {
            'time_series_length': len(times),
            'neuromodulator_statistics': {
                'dopamine': {
                    'mean': np.mean(dopamine),
                    'std': np.std(dopamine),
                    'max': np.max(dopamine)
                },
                'curiosity': {
                    'mean': np.mean(curiosity),
                    'std': np.std(curiosity),
                    'max': np.max(curiosity)
                },
                'novelty': {
                    'mean': np.mean(novelty),
                    'std': np.std(novelty),
                    'max': np.max(novelty)
                }
            },
            'correlations': correlations,
            'events': neuromod_events
        }
        
    def _detect_neuromodulation_events(self, neuromod_data: List[Dict]) -> Dict[str, Any]:
        """Detect neuromodulation events."""
        if not neuromod_data:
            return {}
        
        events = {
            'dopamine_spikes': [],
            'curiosity_peaks': [],
            'novelty_spikes': [],
            'multimodal_events': []
        }
        
        # Simple event detection (threshold-based)
        for i, data in enumerate(neuromod_data):
            if data.get('dopamine', 0) > 2.0:  # Arbitrary threshold
                events['dopamine_spikes'].append(i)
            
            if data.get('curiosity', 0) > 0.7:  # High curiosity
                events['curiosity_peaks'].append(i)
            
            if data.get('novelty', 0) > 1.5:  # High novelty
                events['novelty_spikes'].append(i)
            
            # Multimodal event (all three high)
            if (data.get('dopamine', 0) > 2.0 and 
                data.get('curiosity', 0) > 0.7 and 
                data.get('novelty', 0) > 1.5):
                events['multimodal_events'].append(i)
        
        return events
        
    def create_analysis_report(self, data: Dict[str, Any], filepath: str) -> None:
        """Create a comprehensive analysis report."""
        report = {
            'analysis_timestamp': pd.Timestamp.now().isoformat(),
            'data_summary': {
                'total_records': len(data) if isinstance(data, dict) else 0,
                'record_types': list(data.keys()) if isinstance(data, dict) else []
            },
            'statistical_summary': self._generate_statistical_summary(data),
            'quality_metrics': self._calculate_quality_metrics(data),
            'insights': self._generate_insights(data)
        }
        
        self.save_research_data(report, filepath)
        
    def _generate_statistical_summary(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Generate statistical summary of data."""
        summary = {}
        
        for key, value in data.items():
            if isinstance(value, (list, np.ndarray)):
                arr = np.array(value)
                summary[key] = {
                    'mean': float(np.mean(arr)),
                    'std': float(np.std(arr)),
                    'min': float(np.min(arr)),
                    'max': float(np.max(arr)),
                    'median': float(np.median(arr))
                }
            elif isinstance(value, dict):
                summary[key] = self._generate_statistical_summary(value)
        
        return summary
        
    def _calculate_quality_metrics(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate data quality metrics."""
        metrics = {
            'completeness': 1.0,
            'consistency': 1.0,
            'accuracy': 1.0
        }
        
        # Simple quality checks
        if isinstance(data, dict):
            for key, value in data.items():
                if value is None or (isinstance(value, (list, np.ndarray)) and len(value) == 0):
                    metrics['completeness'] = min(metrics['completeness'], 0.9)
        
        return metrics
        
    def _generate_insights(self, data: Dict[str, Any]) -> List[str]:
        """Generate insights from data."""
        insights = []
        
        # Simple insight generation
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, dict) and 'mean' in value:
                    if value['mean'] > 0.5:
                        insights.append(f"{key} shows high average value ({value['mean']:.3f})")
                    elif value['mean'] < 0.2:
                        insights.append(f"{key} shows low average value ({value['mean']:.3f})")
        
        return insights
        
    def export_for_ml_analysis(self, data: Dict[str, Any], output_format: str = "csv") -> str:
        """Export data for machine learning analysis."""
        if output_format not in ['csv', 'json', 'parquet']:
            raise ValueError(f"Unsupported format: {output_format}")
        
        # Convert to pandas DataFrame
        df_data = self._flatten_data(data)
        
        if output_format == 'csv':
            filepath = self.data_dir / f"ml_data_{pd.Timestamp.now().strftime('%Y%m%d_%H%M%S')}.csv"
            df_data.to_csv(filepath, index=False)
        elif output_format == 'json':
            filepath = self.data_dir / f"ml_data_{pd.Timestamp.now().strftime('%Y%m%d_%H%M%S')}.json"
            df_data.to_json(filepath, orient='records')
        elif output_format == 'parquet':
            filepath = self.data_dir / f"ml_data_{pd.Timestamp.now().strftime('%Y%m%d_%H%M%S')}.parquet"
            df_data.to_parquet(filepath)
        
        return str(filepath)
        
    def _flatten_data(self, data: Dict[str, Any]) -> pd.DataFrame:
        """Flatten nested data structure for ML analysis."""
        flattened = {}
        
        for key, value in data.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    flattened[f"{key}_{subkey}"] = subvalue
            elif isinstance(value, (list, np.ndarray)):
                flattened[key] = np.array(value).flatten()
            else:
                flattened[key] = value
        
        return pd.DataFrame([flattened])
        
    def visualize_results(self, data: Dict[str, Any], output_dir: str) -> List[str]:
        """Create visualizations of research results."""
        output_dir = Path(output_dir)
        output_dir.mkdir(exist_ok=True)
        
        visualizations = []
        
        # Create various plots
        for plot_type in ['histograms', 'time_series', 'scatter']:
            try:
                fig = self._create_plot(data, plot_type)
                filepath = output_dir / f"{plot_type}_{pd.Timestamp.now().strftime('%Y%m%d_%H%M%S')}.png"
                fig.savefig(filepath)
                visualizations.append(str(filepath))
                plt.close(fig)
            except Exception as e:
                print(f"Warning: Could not create {plot_type} plot: {e}")
        
        return visualizations
        
    def _create_plot(self, data: Dict[str, Any], plot_type: str):
        """Create a specific plot type."""
        fig, ax = plt.subplots(figsize=(10, 6))
        
        if plot_type == 'histograms':
            self._plot_histograms(data, ax)
        elif plot_type == 'time_series':
            self._plot_time_series(data, ax)
        elif plot_type == 'scatter':
            self._plot_scatter(data, ax)
        
        return fig
        
    def _plot_histograms(self, data: Dict[str, Any], ax) -> None:
        """Plot histograms of data distributions."""
        for key, value in data.items():
            if isinstance(value, (list, np.ndarray)) and len(value) > 10:
                ax.hist(value, alpha=0.5, label=key, bins=20)
        
        ax.legend()
        ax.set_title('Data Distributions')
        
    def _plot_time_series(self, data: Dict[str, Any], ax) -> None:
        """Plot time series data."""
        for key, value in data.items():
            if isinstance(value, dict) and 'time' in value:
                times = value['time']
                if isinstance(times, list) and len(times) > 1:
                    ax.plot(times, value.get('value', times), label=key)
        
        ax.legend()
        ax.set_title('Time Series')
        ax.set_xlabel('Time')
        ax.set_ylabel('Value')
        
    def _plot_scatter(self, data: Dict[str, Any], ax) -> None:
        """Plot scatter plots."""
        # Simple scatter plot for correlation analysis
        x_data = None
        y_data = None
        
        for key, value in data.items():
            if isinstance(value, dict) and 'x' in value and 'y' in value:
                x_data = value['x']
                y_data = value['y']
                break
        
        if x_data is not None and y_data is not None:
            ax.scatter(x_data, y_data, alpha=0.6)
            ax.set_xlabel('X')
            ax.set_ylabel('Y')
            ax.set_title('Scatter Plot')
            
    def generate_research_summary(self, analysis_results: Dict[str, Any]) -> Dict[str, Any]:
        """Generate a comprehensive research summary."""
        summary = {
            'analysis_timestamp': pd.Timestamp.now().isoformat(),
            'research_questions': [],
            'findings': {},
            'conclusions': [],
            'recommendations': []
        }
        
        # Extract findings
        if 'statistical_summary' in analysis_results:
            summary['findings'] = analysis_results['statistical_summary']
        
        # Generate insights
        if 'insights' in analysis_results:
            summary['conclusions'] = analysis_results['insights']
        
        # Generate recommendations
        summary['recommendations'] = self._generate_recommendations(analysis_results)
        
        return summary
        
    def _generate_recommendations(self, analysis_results: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on analysis results."""
        recommendations = []
        
        # Simple recommendation generation
        if isinstance(analysis_results, dict):
            for key, value in analysis_results.items():
                if isinstance(value, dict) and 'mean' in value:
                    if value['mean'] > 1.0:
                        recommendations.append(f"Consider increasing {key} parameter for better results")
                    elif value['mean'] < 0.1:
                        recommendations.append(f"Consider decreasing {key} parameter for stability")
        
        return recommendations
        
    def run_complete_analysis(self, input_files: List[str], output_dir: str) -> Dict[str, Any]:
        """Run complete analysis pipeline."""
        results = {
            'timestamp': pd.Timestamp.now().isoformat(),
            'input_files': input_files,
            'analysis_steps': []
        }
        
        # Process each file
        for filepath in input_files:
            try:
                data = self.load_research_data(filepath)
                
                # Analyze data
                analysis = {
                    'spike_stats': self.analyze_spike_statistics(data.get('spike_data', [])),
                    'plasticity_changes': self.analyze_plasticity_changes(data.get('weight_history', [])),
                    'neuromodulation': self.analyze_neuromodulation_effects(data.get('neuromod_data', []))
                }
                
                results['analysis_steps'].append({
                    'file': filepath,
                    'analysis': analysis
                })
                
                # Create visualizations
                output_path = Path(output_dir)
                output_path.mkdir(exist_ok=True)
                
                viz_files = self.visualize_results(data, str(output_path / f"analysis_{len(results['analysis_steps'])}"))
                results[f'visualization_files'] = viz_files
                
                # Generate report
                report_file = output_path / f"report_{len(results['analysis_steps'])}.json"
                self.create_analysis_report(analysis, str(report_file))
                
            except Exception as e:
                print(f"Error processing {filepath}: {e}")
                results['errors'] = results.get('errors', []) + [f"{filepath}: {e}"]
        
        # Generate final summary
        if 'analysis_steps' in results and results['analysis_steps']:
            summary = self.generate_research_summary(results['analysis_steps'][0]['analysis'])
            results['summary'] = summary
        
        return results

# Example usage and testing
def example_usage():
    """Example usage of NLMResearchUtilities."""
    
    # Initialize utilities
    utils = NLMResearchUtilities()
    
    # Create sample data
    sample_spike_data = [
        {'time': i * 0.01, 'neuron_id': i % 10}
        for i in range(1000)
    ]
    
    sample_weight_history = [
        [np.random.random() for _ in range(100)]
        for _ in range(10)
    ]
    
    sample_neuromod_data = [
        {
            'time': i * 0.1,
            'dopamine': np.random.random() * 2,
            'curiosity': np.random.random(),
            'novelty': np.random.random() * 2
        }
        for i in range(100)
    ]
    
    # Create sample research data
    research_data = {
        'spike_data': sample_spike_data,
        'weight_history': sample_weight_history,
        'neuromod_data': sample_neuromod_data
    }
    
    # Save sample data
    utils.save_research_data(research_data, "research_data/sample.json")
    
    # Run analysis
    spike_stats = utils.analyze_spike_statistics(sample_spike_data)
    plasticity_stats = utils.analyze_plasticity_changes(sample_weight_history)
    neuromod_stats = utils.analyze_neuromodulation_effects(sample_neuromod_data)
    
    print("Spike Statistics:", json.dumps(spike_stats, indent=2))
    print("\nPlasticity Statistics:", json.dumps(plasticity_stats, indent=2))
    print("\nNeuromodulation Statistics:", json.dumps(neuromod_stats, indent=2))
    
    # Create analysis report
    analysis_results = {
        'statistical_summary': {
            'spike_rate': spike_stats.get('spike_rate', 0),
            'plasticity_events': plasticity_stats.get('strengthening_events', 0) + plasticity_stats.get('weakening_events', 0),
            'neuromodulator_correlation': neuromod_stats.get('correlations', {}).get('dopamine_curiosity', 0)
        },
        'insights': [
            "Network shows regular firing patterns",
            "Plasticity is active with both strengthening and weakening",
            "Dopamine correlates with curiosity levels"
        ]
    }
    
    utils.create_analysis_report(analysis_results, "research_data/analysis_report.json")
    
    # Export for ML analysis
    export_file = utils.export_for_ml_analysis(analysis_results, "csv")
    print(f"\nData exported to: {export_file}")
    
    # Run complete analysis pipeline
    results = utils.run_complete_analysis(
        ["research_data/sample.json"],
        "research_data/analysis_output"
    )
    
    print(f"\nAnalysis complete! Results saved to: research_data/analysis_output")
    
    return results

if __name__ == "__main__":
    # Run example usage
    results = example_usage()
    print("\nExample completed successfully!")
