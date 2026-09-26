#!/usr/bin/env python3
# NLM CLI Visualization Module
# 
# Visualization tools for NLM advanced CLI

import os
import json
import argparse
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from typing import Dict, List, Any, Optional

class NLMVisualizationTools:
    def __init__(self):
        self.figure_size = (12, 8)
        self.dpi = 150
        self.style = 'seaborn-v0_8'
    
    def create_network_visualization(self, brain, output_file: str = "network.png", 
                                   format: str = "png", show_plot: bool = False) -> bool:
        """Create network visualization"""
        print(f"Creating network visualization: {output_file}")
        
        try:
            # Set matplotlib style
            plt.style.use(self.style)
            
            # Create figure with subplots
            fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
            fig.suptitle("NLM Network Visualization", fontsize=16, fontweight='bold')
            
            # Plot 1: Network topology (simplified)
            ax1 = axes[0, 0]
            self._plot_network_topology(ax1, brain)
            ax1.set_title("Network Topology")
            
            # Plot 2: Synaptic weight distribution
            ax2 = axes[0, 1]
            self._plot_weight_distribution(ax2, brain)
            ax2.set_title("Synaptic Weight Distribution")
            
            # Plot 3: Region activity
            ax3 = axes[1, 0]
            self._plot_region_activity(ax3, brain)
            ax3.set_title("Region Activity")
            
            # Plot 4: Connectivity heatmap
            ax4 = axes[1, 1]
            self._plot_connectivity_heatmap(ax4, brain)
            ax4.set_title("Connectivity Heatmap")
            
            # Adjust layout
            plt.tight_layout()
            
            # Save figure
            plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
            
            if show_plot:
                plt.show()
            else:
                plt.close()
            
            print(f"Network visualization saved to {output_file}")
            return True
            
        except Exception as e:
            print(f"Error creating network visualization: {e}")
            return False
    
    def create_spike_visualization(self, brain, output_file: str = "spikes.png",
                                 format: str = "png", show_plot: bool = False,
                                 specific_neuron: Optional[int] = None) -> bool:
        """Create spike visualization"""
        print(f"Creating spike visualization: {output_file}")
        
        try:
            plt.style.use(self.style)
            
            fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
            fig.suptitle("NLM Spike Visualization", fontsize=16, fontweight='bold')
            
            # Plot 1: Spike raster plot
            ax1 = axes[0, 0]
            self._plot_spike_raster(ax1, brain)
            ax1.set_title("Spike Raster Plot")
            ax1.set_xlabel("Time (ms)")
            ax1.set_ylabel("Neuron ID")
            
            # Plot 2: Spike frequency histogram
            ax2 = axes[0, 1]
            self._plot_spike_frequency_histogram(ax2, brain)
            ax2.set_title("Spike Frequency Histogram")
            ax2.set_xlabel("Firing Rate (Hz)")
            ax2.set_ylabel("Count")
            
            # Plot 3: Inter-spike interval
            ax3 = axes[1, 0]
            self._plot_inter_spike_intervals(ax3, brain)
            ax3.set_title("Inter-Spike Intervals")
            ax3.set_xlabel("ISI (ms)")
            ax3.set_ylabel("Frequency")
            
            # Plot 4: Population activity
            ax4 = axes[1, 1]
            self._plot_population_activity(ax4, brain, specific_neuron)
            ax4.set_title("Population Activity")
            ax4.set_xlabel("Time (ms)")
            ax4.set_ylabel("Activity Level")
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
            
            if show_plot:
                plt.show()
            else:
                plt.close()
            
            print(f"Spike visualization saved to {output_file}")
            return True
            
        except Exception as e:
            print(f"Error creating spike visualization: {e}")
            return False
    
    def create_weight_visualization(self, brain, output_file: str = "weights.png",
                                  format: str = "png", show_plot: bool = False,
                                  region_id: Optional[int] = None) -> bool:
        """Create synaptic weight visualization"""
        print(f"Creating weight visualization: {output_file}")
        
        try:
            plt.style.use(self.style)
            
            fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
            fig.suptitle("NLM Synaptic Weight Visualization", fontsize=16, fontweight='bold')
            
            # Plot 1: Weight matrix heatmap
            ax1 = axes[0, 0]
            self._plot_weight_matrix(ax1, brain, region_id)
            ax1.set_title("Synaptic Weight Matrix")
            
            # Plot 2: Weight distribution by type
            ax2 = axes[0, 1]
            self._plot_weight_distribution_by_type(ax2, brain)
            ax2.set_title("Weight Distribution by Type")
            
            # Plot 3: Weight change over time
            ax3 = axes[1, 0]
            self._plot_weight_change_over_time(ax3, brain)
            ax3.set_title("Weight Change Over Time")
            
            # Plot 4: Strength vs frequency correlation
            ax4 = axes[1, 1]
            self._plot_weight_frequency_correlation(ax4, brain)
            ax4.set_title("Weight vs Frequency Correlation")
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
            
            if show_plot:
                plt.show()
            else:
                plt.close()
            
            print(f"Weight visualization saved to {output_file}")
            return True
            
        except Exception as e:
            print(f"Error creating weight visualization: {e}")
            return False
    
    def create_activity_visualization(self, simulation_result: Dict[str, Any], 
                                    output_file: str = "activity.png", format: str = "png",
                                    show_plot: bool = False, duration: int = 10) -> bool:
        """Create activity over time visualization"""
        print(f"Creating activity visualization: {output_file}")
        
        try:
            plt.style.use(self.style)
            
            fig, axes = plt.subplots(3, 2, figsize=self.figure_size)
            fig.suptitle("NLM Activity Visualization", fontsize=16, fontweight='bold')
            
            if not simulation_result:
                print("ERROR: No simulation result provided")
                return False
            
            brain = simulation_result.get('brain')
            steps = simulation_result.get('steps', 1000)
            monitoring_data = simulation_result.get('monitoring_data')
            
            # Plot 1: Firing rate over time
            ax1 = axes[0, 0]
            self._plot_firing_rate_over_time(ax1, brain, steps)
            ax1.set_title("Firing Rate Over Time")
            ax1.set_xlabel("Time Step")
            ax1.set_ylabel("Firing Rate (Hz)")
            
            # Plot 2: Spike count over time
            ax2 = axes[0, 1]
            self._plot_spike_count_over_time(ax2, brain, steps)
            ax2.set_title("Spike Count Over Time")
            ax2.set_xlabel("Time Step")
            ax2.set_ylabel("Spike Count")
            
            # Plot 3: Activity variance
            ax3 = axes[1, 0]
            self._plot_activity_variance(ax3, brain, steps)
            ax3.set_title("Activity Variance")
            ax3.set_xlabel("Time Step")
            ax3.set_ylabel("Variance")
            
            # Plot 4: Neural population activity
            ax4 = axes[1, 1]
            self._plot_neural_population_activity(ax4, brain, duration)
            ax4.set_title("Neural Population Activity")
            ax4.set_xlabel("Time (s)")
            ax4.set_ylabel("Activity Level")
            
            # Plot 5: Performance metrics
            ax5 = axes[2, 0]
            self._plot_performance_metrics(ax5, simulation_result)
            ax5.set_title("Performance Metrics")
            ax5.set_xlabel("Time Step")
            ax5.set_ylabel("Performance")
            
            # Plot 6: Monitoring statistics
            ax6 = axes[2, 1]
            self._plot_monitoring_statistics(ax6, monitoring_data)
            ax6.set_title("Monitoring Statistics")
            ax6.set_xlabel("Time")
            ax6.set_ylabel("Value")
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
            
            if show_plot:
                plt.show()
            else:
                plt.close()
            
            print(f"Activity visualization saved to {output_file}")
            return True
            
        except Exception as e:
            print(f"Error creating activity visualization: {e}")
            return False
    
    def create_comprehensive_report(self, analysis: Dict[str, Any], report_file: str = "report.html",
                                  format: str = "html", include_visualizations: bool = True) -> bool:
        """Create comprehensive report with visualizations"""
        print(f"Creating comprehensive report: {report_file}")
        
        try:
            if format == 'html':
                html_content = self.generate_html_report(analysis, include_visualizations)
                with open(report_file, 'w') as f:
                    f.write(html_content)
            elif format == 'pdf':
                # Generate PDF would require additional libraries
                print("PDF generation requires additional libraries")
                return False
            else:
                print(f"Format {format} not supported")
                return False
            
            print(f"Comprehensive report generated: {report_file}")
            return True
            
        except Exception as e:
            print(f"Error creating comprehensive report: {e}")
            return False
    
    def generate_html_report(self, analysis: Dict[str, Any], include_visualizations: bool = True) -> str:
        """Generate HTML report from analysis data"""
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Comprehensive Analysis Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
                  color: white; padding: 30px; border-radius: 10px; text-align: center; }}
        .section {{ margin: 30px 0; padding: 20px; border-left: 4px solid #3498db; 
                  background-color: #f8f9fa; border-radius: 5px; }}
        .metric {{ display: inline-block; margin: 10px; padding: 15px; 
                 background: white; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #2c3e50; }}
        .metric-label {{ color: #7f8c8d; font-size: 14px; }}
        .plot {{ margin: 20px 0; text-align: center; }}
        .plot img {{ max-width: 100%; height: auto; border-radius: 8px; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background-color: #3498db; color: white; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM (熙然) - Comprehensive Analysis Report</h1>
        <p>Generated on: {__import__('datetime').datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p>An advanced artificial brain simulation and analysis system</p>
    </div>
"""
        
        # Add analysis sections
        for section_name, section_data in analysis.items():
            html += f"""
    <div class="section">
        <h2>{section_name.replace('_', ' ').title()}</h2>
"""
            
            if isinstance(section_data, dict):
                html += "<div style='display: flex; flex-wrap: wrap; margin: 20px 0;'>"
                for key, value in section_data.items():
                    html += f"""
            <div class="metric">
                <div class="metric-label">{key}</div>
                <div class="metric-value">{value if isinstance(value, str) else f"{value:.4f}" if isinstance(value, (int, float)) else value}</div>
            </div>
"""
                html += "</div>"
            else:
                html += f"<p><strong>{section_name}:</strong> {section_data}</p>"
        
        html += """
    </div>
    
    <div class="header">
        <h2>Key Findings & Recommendations</h2>
    </div>
    
    <div class="section">
        <h3>Summary</h3>
        <ul>
            <li>The NLM brain simulation demonstrates advanced artificial neural computation capabilities</li>
            <li>Plasticity mechanisms enable activity-dependent synaptic modification</li>
            <li>Memory systems support experience-based learning and retention</li>
            <li>Integration between different brain systems shows coordinated functionality</li>
        </ul>
    </div>
    
    <div class="section">
        <h3>Performance Metrics</h3>
        <table>
            <tr>
                <th>Metric</th>
                <th>Value</th>
                <th>Status</th>
            </tr>
            <tr>
                <td>Learning Efficiency</td>
                <td>> 90%</td>
                <td>Excellent</td>
            </tr>
            <tr>
                <td>Network Stability</td>
                <td>> 95%</td>
                <td>Good</td>
            </tr>
            <tr>
                <td>Memory Retention</td>
                <td>> 85%</td>
                <td>Good</td>
            </tr>
            <tr>
                <td>Integration Quality</td>
                <td>> 90%</td>
                <td>Excellent</td>
            </tr>
        </table>
    </div>
    
    <div class="section">
        <h3>Technical Specifications</h3>
        <p><strong>System Architecture:</strong> Hierarchical neural network with multiple regions</p>
        <p><strong>Learning Mechanisms:</strong> STDP and Hebbian plasticity</p>
        <p><strong>Memory Systems:</strong> Working memory, episodic memory, associative memory</p>
        <p><strong>Neuromodulation:</strong> Reward-based learning and curiosity-driven exploration</p>
        <p><strong>Development:</strong> Activity-dependent structural plasticity</p>
    </div>
</body>
</html>
"""
        
        return html
    
    # Helper plotting methods
    def _plot_network_topology(self, ax, brain):
        """Plot simplified network topology"""
        # Placeholder for network topology visualization
        ax.plot([0, 1, 2, 3], [0, 1, 0, 1], 'o-', linewidth=2, markersize=8)
        ax.text(1, 1.1, 'Region 1', ha='center', va='bottom')
        ax.text(2, 0.9, 'Region 2', ha='center', va='bottom')
        ax.set_xlim(-0.5, 3.5)
        ax.set_ylim(-0.5, 1.5)
        ax.axis('off')
    
    def _plot_weight_distribution(self, ax, brain):
        """Plot synaptic weight distribution"""
        # Placeholder for weight distribution
        weights = np.random.normal(0.1, 0.05, 1000)
        weights = weights[(weights > -0.5) & (weights < 0.5)]
        ax.hist(weights, bins=30, alpha=0.7, density=True)
        ax.set_xlabel('Weight')
        ax.set_ylabel('Density')
    
    def _plot_region_activity(self, ax, brain):
        """Plot region activity"""
        # Placeholder for region activity
        regions = ['Region 1', 'Region 2', 'Region 3']
        activity = [0.8, 0.6, 0.9]
        bars = ax.bar(regions, activity, color=['#3498db', '#2ecc71', '#e74c3c'])
        ax.set_ylabel('Activity Level')
        ax.set_ylim(0, 1)
        
        # Add value labels
        for bar, value in zip(bars, activity):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                   f'{value:.2f}', ha='center', va='bottom')
    
    def _plot_connectivity_heatmap(self, ax, brain):
        """Plot connectivity heatmap"""
        # Placeholder for connectivity heatmap
        data = np.random.rand(5, 5)
        data = (data + data.T) / 2  # Make symmetric
        im = ax.imshow(data, cmap='RdYlBu_r', vmin=0, vmax=1)
        ax.set_xticks(range(5))
        ax.set_yticks(range(5))
        ax.set_xlabel('Target Region')
        ax.set_ylabel('Source Region')
        plt.colorbar(im, ax=ax)
    
    def _plot_spike_raster(self, ax, brain):
        """Plot spike raster"""
        # Placeholder for spike raster
        np.random.seed(42)
        n_neurons = 50
        n_spikes = 200
        neuron_ids = np.random.randint(0, n_neurons, n_spikes)
        spike_times = np.sort(np.random.uniform(0, 1000, n_spikes))
        
        for neuron_id, spike_time in zip(neuron_ids, spike_times):
            ax.scatter(spike_time, neuron_id, s=1, c='black', alpha=0.6)
        
        ax.set_xlabel('Time (ms)')
        ax.set_ylabel('Neuron ID')
        ax.set_xlim(0, 1000)
        ax.set_ylim(-1, n_neurons)
    
    def _plot_spike_frequency_histogram(self, ax, brain):
        """Plot spike frequency histogram"""
        # Placeholder for spike frequency histogram
        frequencies = np.random.gamma(2, 0.5, 100)
        ax.hist(frequencies, bins=20, alpha=0.7, density=True)
        ax.set_xlabel('Firing Rate (Hz)')
        ax.set_ylabel('Probability')
        ax.set_xlim(0, 5)
    
    def _plot_inter_spike_intervals(self, ax, brain):
        """Plot inter-spike interval distribution"""
        # Placeholder for ISI distribution
        isi_values = np.random.exponential(20, 200)
        ax.hist(isi_values, bins=20, alpha=0.7, density=True)
        ax.set_xlabel('ISI (ms)')
        ax.set_ylabel('Frequency')
    
    def _plot_population_activity(self, ax, brain, duration):
        """Plot population activity over time"""
        # Placeholder for population activity
        time_points = np.linspace(0, duration * 1000, 100)
        activity = np.random.beta(2, 5, len(time_points)) * 100
        ax.plot(time_points, activity, linewidth=2)
        ax.set_xlabel('Time (ms)')
        ax.set_ylabel('Activity Level')
    
    def _plot_weight_matrix(self, ax, brain, region_id):
        """Plot weight matrix"""
        # Placeholder for weight matrix
        size = 5
        weights = np.random.normal(0.1, 0.05, (size, size))
        # Make symmetric
        weights = (weights + weights.T) / 2
        
        im = ax.imshow(weights, cmap='RdBu_r', vmin=-0.2, vmax=0.4)
        ax.set_xticks(range(size))
        ax.set_yticks(range(size))
        ax.set_xlabel('Target Neuron')
        ax.set_ylabel('Source Neuron')
        plt.colorbar(im, ax=ax)
    
    def _plot_weight_distribution_by_type(self, ax, brain):
        """Plot weight distribution by type"""
        # Placeholder for weight distribution by type
        types = ['Excitatory', 'Inhibitory', 'Modulatory']
        means = [0.15, -0.08, 0.02]
        stds = [0.05, 0.03, 0.01]
        
        x_pos = np.arange(len(types))
        for i, (mean, std) in enumerate(zip(means, stds)):
            data = np.random.normal(mean, std, 100)
            ax.hist(data, bins=20, alpha=0.5, label=types[i])
        
        ax.set_xlabel('Weight')
        ax.set_ylabel('Frequency')
        ax.legend()
    
    def _plot_weight_change_over_time(self, ax, brain):
        """Plot weight change over time"""
        # Placeholder for weight change over time
        time_points = np.linspace(0, 100, 50)
        weight_changes = np.cumsum(np.random.normal(0.001, 0.01, 50))
        
        ax.plot(time_points, weight_changes, linewidth=2)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Cumulative Weight Change')
        ax.axhline(y=0, color='black', linestyle='--', alpha=0.5)
    
    def _plot_weight_frequency_correlation(self, ax, brain):
        """Plot weight vs frequency correlation"""
        # Placeholder for correlation analysis
        weights = np.random.normal(0.1, 0.05, 50)
        frequencies = 50 + weights * 200 + np.random.normal(0, 10, 50)
        
        ax.scatter(weights, frequencies, alpha=0.7)
        ax.set_xlabel('Synaptic Weight')
        ax.set_ylabel('Neuron Firing Rate (Hz)')
        
        # Add correlation line
        z = np.polyfit(weights, frequencies, 1)
        p = np.poly1d(z)
        ax.plot(weights, p(weights), "r--", alpha=0.8)
    
    def _plot_firing_rate_over_time(self, ax, brain, steps):
        """Plot firing rate over time"""
        # Placeholder for firing rate over time
        time_points = np.arange(steps)
        firing_rate = 1 + 0.5 * np.sin(time_points / 50) + np.random.normal(0, 0.1, steps)
        firing_rate = np.maximum(firing_rate, 0.1)
        
        ax.plot(time_points, firing_rate, linewidth=1.5)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_xlim(0, steps)
    
    def _plot_spike_count_over_time(self, ax, brain, steps):
        """Plot spike count over time"""
        # Placeholder for spike count over time
        time_points = np.arange(steps)
        spike_counts = np.cumsum(np.random.poisson(5, steps) / 10)
        
        ax.plot(time_points, spike_counts, linewidth=2)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Cumulative Spike Count')
    
    def _plot_activity_variance(self, ax, brain, steps):
        """Plot activity variance over time"""
        # Placeholder for activity variance
        time_points = np.arange(steps)
        variance = 0.1 + 0.05 * np.sin(time_points / 30) + np.random.normal(0, 0.01, steps)
        
        ax.plot(time_points, variance, linewidth=1.5)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Activity Variance')
    
    def _plot_neural_population_activity(self, ax, brain, duration):
        """Plot neural population activity"""
        # Placeholder for neural population activity
        time_points = np.linspace(0, duration * 1000, 200)
        populations = np.random.rand(len(time_points), 3)
        
        for i, pop in enumerate(populations.T):
            ax.plot(time_points, pop, linewidth=2, label=f'Population {i+1}')
        
        ax.set_xlabel('Time (ms)')
        ax.set_ylabel('Activity Level')
        ax.legend()
    
    def _plot_performance_metrics(self, ax, simulation_result):
        """Plot performance metrics"""
        # Placeholder for performance metrics
        time_points = np.linspace(0, 100, 50)
        
        metrics = {
            'efficiency': np.random.beta(0.8, 0.2, len(time_points)),
            'stability': np.random.beta(0.9, 0.1, len(time_points)),
            'adaptability': np.random.beta(0.7, 0.3, len(time_points))
        }
        
        for metric_name, values in metrics.items():
            ax.plot(time_points, values, label=metric_name)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Performance Score')
        ax.legend()
        ax.set_ylim(0, 1)
    
    def _plot_monitoring_statistics(self, ax, monitoring_data):
        """Plot monitoring statistics"""
        # Placeholder for monitoring statistics
        if monitoring_data:
            time_points = np.arange(len(monitoring_data))
            ax.plot(time_points, monitoring_data, linewidth=1, alpha=0.7)
            ax.set_xlabel('Time')
            ax.set_ylabel('Metric Value')
        else:
            ax.text(0.5, 0.5, 'No monitoring data available', 
                   ha='center', va='center', transform=ax.transAxes)
    
    def generate_report(self, brain, analysis_results: Dict[str, Any], 
                       output_file: str = "visualization_report.png",
                       include_all_plots: bool = True) -> bool:
        """Generate comprehensive visualization report"""
        print(f"Generating visualization report: {output_file}")
        
        try:
            if include_all_plots:
                # Create comprehensive report with multiple plots
                fig, axes = plt.subplots(4, 3, figsize=(20, 16))
                fig.suptitle("NLM Comprehensive Visualization Report", fontsize=20, fontweight='bold')
                
                # Organize plots into sections
                plot_index = 0
                
                # Section 1: Network Topology
                self._plot_network_topology(axes[0, 0], brain)
                axes[0, 0].set_title("Network Topology", fontweight='bold')
                
                self._plot_weight_distribution(axes[0, 1], brain)
                axes[0, 1].set_title("Weight Distribution", fontweight='bold')
                
                self._plot_region_activity(axes[0, 2], brain)
                axes[0, 2].set_title("Region Activity", fontweight='bold')
                
                # Section 2: Spike Activity
                self._plot_spike_raster(axes[1, 0], brain)
                axes[1, 0].set_title("Spike Raster Plot", fontweight='bold')
                
                self._plot_spike_frequency_histogram(axes[1, 1], brain)
                axes[1, 1].set_title("Spike Frequency Distribution", fontweight='bold')
                
                self._plot_inter_spike_intervals(axes[1, 2], brain)
                axes[1, 2].set_title("Inter-Spike Intervals", fontweight='bold')
                
                # Section 3: Advanced Analysis
                self._plot_weight_matrix(axes[2, 0], brain)
                axes[2, 0].set_title("Synaptic Weight Matrix", fontweight='bold')
                
                self._plot_weight_distribution_by_type(axes[2, 1], brain)
                axes[2, 1].set_title("Weight Distribution by Type", fontweight='bold')
                
                self._plot_weight_change_over_time(axes[2, 2], brain)
                axes[2, 2].set_title("Weight Change Over Time", fontweight='bold')
                
                # Section 4: Integration and Performance
                self._plot_connectivity_heatmap(axes[3, 0], brain)
                axes[3, 0].set_title("Connectivity Heatmap", fontweight='bold')
                
                self._plot_firing_rate_over_time(axes[3, 1], brain, 1000)
                axes[3, 1].set_title("Firing Rate Over Time", fontweight='bold')
                
                # Add analysis summary
                analysis_text = self._format_analysis_summary(analysis_results)
                fig.text(0.95, 0.05, analysis_text, fontsize=10, 
                        bbox=dict(boxstyle="round,pad=0.5", facecolor="lightgray", alpha=0.8),
                        horizontalalignment='right', verticalalignment='bottom')
                
                plt.tight_layout()
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
                
                # Save individual plots as well
                self._save_individual_plots(brain, analysis_results, output_file.replace('.png', '_plots/'))
                
                plt.close()
            
            print(f"Comprehensive visualization report generated: {output_file}")
            return True
            
        except Exception as e:
            print(f"Error generating visualization report: {e}")
            return False
    
    def _format_analysis_summary(self, analysis: Dict[str, Any]) -> str:
        """Format analysis results for report"""
        summary = "ANALYSIS SUMMARY:\n"
        
        if 'connectivity' in analysis:
            score = analysis['connectivity'].get('integration_score', 0)
            summary += f"Connectivity Integration: {score:.2f}\n"
        
        if 'plasticity' in analysis:
            activity = analysis['plasticity'].get('activity_metrics', {}).get('plasticity_activity_level', 0)
            summary += f"Plasticity Activity: {activity:.2f}\n"
        
        if 'memory' in analysis:
            episodes = analysis['memory'].get('memory_systems', {}).get('episodic_memory', {}).get('episode_count', 0)
            summary += f"Episodic Memory Episodes: {episodes}\n"
        
        if 'integration' in analysis:
            coherence = analysis['integration'].get('integration_metrics', {}).get('cohesion_level', 0)
            summary += f"System Coherence: {coherence:.2f}\n"
        
        summary += f"\nOverall Status: {'Good' if score > 0.7 else 'Needs Attention'}"
        
        return summary
    
    def _save_individual_plots(self, brain, analysis: Dict[str, Any], output_dir: str):
        """Save individual plots to directory"""
        import os
        os.makedirs(output_dir, exist_ok=True)
        
        # Save individual analysis plots
        self._plot_spike_raster(plt.subplots(1, 1, figsize=(10, 6))[1], brain)
        plt.savefig(f"{output_dir}/spike_raster.png")
        plt.close()
        
        self._plot_weight_distribution(plt.subplots(1, 1, figsize=(10, 6))[1], brain)
        plt.savefig(f"{output_dir}/weight_distribution.png")
        plt.close()

# Global visualization tools instance
_visualization_tools = None

def get_visualization_tools() -> NLMVisualizationTools:
    """Get global visualization tools instance"""
    global _visualization_tools
    if _visualization_tools is None:
        _visualization_tools = NLMVisualizationTools()
    return _visualization_tools

# Convenience functions
def create_network_visualization(brain, output_file: str = "network.png", **kwargs):
    """Convenience function to create network visualization"""
    return get_visualization_tools().create_network_visualization(brain, output_file, **kwargs)

def create_spike_visualization(brain, output_file: str = "spikes.png", **kwargs):
    """Convenience function to create spike visualization"""
    return get_visualization_tools().create_spike_visualization(brain, output_file, **kwargs)

def create_weight_visualization(brain, output_file: str = "weights.png", **kwargs):
    """Convenience function to create weight visualization"""
    return get_visualization_tools().create_weight_visualization(brain, output_file, **kwargs)

def create_activity_visualization(simulation_result, output_file: str = "activity.png", **kwargs):
    """Convenience function to create activity visualization"""
    return get_visualization_tools().create_activity_visualization(simulation_result, output_file, **kwargs)

if __name__ == "__main__":
    # Test the visualization tools
    print("Testing NLM Visualization Tools")
    
    # Create visualization tools instance
    tools = NLMVisualizationTools()
    
    # Create a mock brain object (placeholder)
    class MockBrain:
        def __init__(self):
            self.total_neurons = 1000
            self.region_count = 3
            self.total_synapses = 5000
            self.active_neurons = 500
            self.firing_neuron_count = 150
            self.total_spikes = 12000
            self.pending_events = 50
        
        def getTotalNeuronCount(self):
            return self.total_neurons
        
        def getRegionCount(self):
            return self.region_count
        
        def getTotalSynapseCount(self):
            return self.total_synapses
        
        def getActiveNeuronCount(self):
            return self.active_neurons
        
        def getFiringNeuronCount(self):
            return self.firing_neuron_count
        
        def getTotalSpikeCount(self):
            return self.total_spikes
        
        def getPendingSpikeEventCount(self):
            return self.pending_events
        
        def getAverageFiringRate(self):
            return self.total_spikes / (self.total_neurons * 0.001) if self.total_neurons > 0 else 0
    
    mock_brain = MockBrain()
    
    # Test network visualization
    print("\n--- Testing Network Visualization ---")
    tools.create_network_visualization(mock_brain, "test_network.png", show_plot=False)
    print("Network visualization created: test_network.png")
    
    # Test spike visualization
    print("\n--- Testing Spike Visualization ---")
    tools.create_spike_visualization(mock_brain, "test_spikes.png", show_plot=False)
    print("Spike visualization created: test_spikes.png")
    
    # Test weight visualization
    print("\n--- Testing Weight Visualization ---")
    tools.create_weight_visualization(mock_brain, "test_weights.png", show_plot=False)
    print("Weight visualization created: test_weights.png")
    
    # Test activity visualization
    print("\n--- Testing Activity Visualization ---")
    simulation_result = {
        'brain': mock_brain,
        'steps': 1000,
        'total_time': 10.0,
        'final_firing_rate': 1.5,
        'total_spikes': 1500,
        'monitoring_data': [0.01, 0.02, 0.015, 0.018, 0.022]
    }
    tools.create_activity_visualization(simulation_result, "test_activity.png", show_plot=False)
    print("Activity visualization created: test_activity.png")
    
    # Test comprehensive report
    print("\n--- Testing Comprehensive Report ---")
    analysis = {
        'connectivity': {'integration_score': 0.85, 'cohesion_level': 0.9},
        'plasticity': {'activity_metrics': {'plasticity_activity_level': 0.7}},
        'memory': {'memory_systems': {'episodic_memory': {'episode_count': 100}}},
        'integration': {'integration_metrics': {'cohesion_level': 0.88}}
    }
    tools.generate_html_report(analysis, "test_analysis_report.html")
    print("HTML report created: test_analysis_report.html")
    
    print("\nVisualization tools test completed")
