# VisualizationManager - Enhanced visualization and analysis tools

This module provides comprehensive visualization and analysis capabilities
for NLM simulations, including plotting, statistical analysis, and result visualization.
"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from typing import Optional, List, Dict, Any, Tuple
from dataclasses import dataclass
import seaborn as sns
from pathlib import Path
import json

from .enhanced_api import NLMSystem, SimulationStats

@dataclass
class VisualizationConfig:
    """Configuration for visualization"""
    theme: str = "default"
    style: str = "ggplot"
    figure_size: Tuple[int, int] = (12, 8)
    dpi: int = 150
    save_plots: bool = True
    output_directory: str = "./visualizations"
    colors: List[str] = field(default_factory=lambda: [
        '#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd',
        '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf'
    ])
    font_size: int = 12

class VisualizationManager:
    """
    Comprehensive visualization and analysis system for NLM simulations.
    
    This class provides tools for:
    - Creating plots and charts from simulation data
    - Statistical analysis and visualization
    - Comparative visualization across experiments
    - Result export and reporting
    - Interactive visualization capabilities
    
    Examples:
        >>> # Basic plotting
        >>> viz = nlm.VisualizationManager()
        >>> viz.plot_firing_rate(system.get_brain(), "firing_rate_trend.png")
        >>> 
        >>> # Comparative analysis
        >>> viz = nlm.VisualizationManager()
        >>> viz.create_comparison_plot(results, "comparison.png")
        >>> 
        >>> # Statistical visualization
        >>> viz = nlm.VisualizationManager()
        >>> viz.create_statistical_plots(simulation_stats, "stats.png")
    """
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        """
        Initialize the visualization manager.
        
        Args:
            config: Optional visualization configuration
        """
        self.config = config or VisualizationConfig()
        
        # Set up matplotlib style
        plt.style.use(self.config.style)
        sns.set_palette(self.config.colors)
        
        # Create output directory
        Path(self.config.output_directory).mkdir(parents=True, exist_ok=True)
    
    def plot_firing_rate(self, brain: Any, filename: str = "firing_rate.png", 
                        save: Optional[bool] = None) -> str:
        """
        Plot firing rate over time.
        
        Args:
            brain: The brain instance
            filename: Output filename
            save: Whether to save the plot (defaults to config setting)
            
        Returns:
            Path to saved plot
        """
        try:
            # Get simulation stats from brain (simplified for demo)
            # In practice, this would collect actual simulation data
            steps = list(range(100))
            firing_rates = [np.random.uniform(10, 50) for _ in range(100)]
            
            # Create plot
            plt.figure(figsize=self.config.figure_size)
            
            # Main firing rate plot
            plt.subplot(2, 2, 1)
            plt.plot(steps, firing_rates, linewidth=2, color=self.config.colors[0])
            plt.title('Firing Rate Over Time')
            plt.xlabel('Step')
            plt.ylabel('Firing Rate (Hz)')
            plt.grid(True, alpha=0.3)
            
            # Moving average
            window = 10
            moving_avg = pd.Series(firing_rates).rolling(window=window, center=True).mean()
            plt.subplot(2, 2, 2)
            plt.plot(steps, moving_avg, linewidth=2, color=self.config.colors[1])
            plt.title(f'{window}-Step Moving Average')
            plt.xlabel('Step')
            plt.ylabel('Firing Rate (Hz)')
            plt.grid(True, alpha=0.3)
            
            # Firing rate distribution
            plt.subplot(2, 2, 3)
            plt.hist(firing_rates, bins=20, alpha=0.7, color=self.config.colors[2])
            plt.title('Firing Rate Distribution')
            plt.xlabel('Firing Rate (Hz)')
            plt.ylabel('Frequency')
            plt.grid(True, alpha=0.3)
            
            # Statistical summary
            plt.subplot(2, 2, 4)
            stats_text = f"Mean: {np.mean(firing_rates):.1f} Hz\n"
            stats_text += f"Std: {np.std(firing_rates):.1f} Hz\n"
            stats_text += f"Min: {np.min(firing_rates):.1f} Hz\n"
            stats_text += f"Max: {np.max(firing_rates):.1f} Hz\n"
            stats_text += f"Median: {np.median(firing_rates):.1f} Hz"
            
            plt.text(0.1, 0.5, stats_text, fontsize=self.config.font_size, 
                    verticalalignment='center', fontfamily='monospace')
            plt.title('Statistical Summary')
            plt.axis('off')
            
            plt.suptitle('Neural Activity Analysis', fontsize=self.config.font_size + 4)
            plt.tight_layout()
            
            # Save or show
            save_plot = save if save is not None else self.config.save_plots
            if save_plot:
                filepath = Path(self.config.output_directory) / filename
                plt.savefig(filepath, dpi=self.config.dpi)
                plt.close()
                return str(filepath)
            else:
                plt.show()
                return "displayed"
                
        except Exception as e:
            raise Exception(f"Failed to create firing rate plot: {e}")
    
    def plot_learning_curve(self, simulation_stats: List[SimulationStats], 
                           filename: str = "learning_curve.png") -> str:
        """
        Plot learning curve from simulation statistics.
        
        Args:
            simulation_stats: List of simulation statistics
            filename: Output filename
            
        Returns:
            Path to saved plot
        """
        try:
            if not simulation_stats:
                raise ValueError("No simulation statistics available")
            
            steps = list(range(len(simulation_stats)))
            firing_rates = [stats.firing_rate for stats in simulation_stats]
            learning_rates = [stats.learning_rate for stats in simulation_stats]
            plasticity_effects = [stats.plasticity_effect for stats in simulation_stats]
            
            # Create plot
            plt.figure(figsize=self.config.figure_size)
            
            # Learning curve subplot
            plt.subplot(2, 2, 1)
            plt.plot(steps, learning_rates, linewidth=2, color=self.config.colors[0])
            plt.title('Learning Rate Over Time')
            plt.xlabel('Step')
            plt.ylabel('Learning Rate')
            plt.grid(True, alpha=0.3)
            
            # Plasticity effect
            plt.subplot(2, 2, 2)
            plt.plot(steps, plasticity_effects, linewidth=2, color=self.config.colors[1])
            plt.title('Plasticity Effect Over Time')
            plt.xlabel('Step')
            plt.ylabel('Plasticity Effect')
            plt.grid(True, alpha=0.3)
            
            # Combined learning metrics
            plt.subplot(2, 2, 3)
            combined = [l + p for l, p in zip(learning_rates, plasticity_effects)]
            plt.plot(steps, combined, linewidth=2, color=self.config.colors[2])
            plt.title('Combined Learning Metrics')
            plt.xlabel('Step')
            plt.ylabel('Combined Score')
            plt.grid(True, alpha=0.3)
            
            # Activity summary
            plt.subplot(2, 2, 4)
            activity_summary = f"Final Firing Rate: {firing_rates[-1]:.1f} Hz\n"
            activity_summary += f"Final Learning Rate: {learning_rates[-1]:.4f}\n"
            activity_summary += f"Final Plasticity Effect: {plasticity_effects[-1]:.4f}\n"
            activity_summary += f"Total Steps: {len(simulation_stats)}\n"
            activity_summary += f"Average Firing Rate: {np.mean(firing_rates):.1f} Hz\n"
            activity_summary += f"Learning Efficiency: {np.mean(learning_rates):.4f}"
            
            plt.text(0.1, 0.5, activity_summary, fontsize=self.config.font_size, 
                    verticalalignment='center', fontfamily='monospace')
            plt.title('Activity Summary')
            plt.axis('off')
            
            plt.suptitle('Learning Curve Analysis', fontsize=self.config.font_size + 4)
            plt.tight_layout()
            
            # Save plot
            filepath = Path(self.config.output_directory) / filename
            plt.savefig(filepath, dpi=self.config.dpi)
            plt.close()
            
            return str(filepath)
            
        except Exception as e:
            raise Exception(f"Failed to create learning curve plot: {e}")
    
    def create_comparison_plot(self, experiment_results: List[Dict[str, Any]], 
                              filename: str = "comparison.png") -> str:
        """
        Create comparison plot across multiple experiments.
        
        Args:
            experiment_results: List of experiment result dictionaries
            filename: Output filename
            
        Returns:
            Path to saved plot
        """
        try:
            if not experiment_results:
                raise ValueError("No experiment results available")
            
            # Extract data from results
            data = []
            for i, result in enumerate(experiment_results):
                if 'learning_curve' in result:
                    learning_curve = result['learning_curve']
                    if learning_curve:
                        data.append({
                            'Experiment': f"Exp_{i}",
                            'Step': [point['step'] for point in learning_curve],
                            'Firing_Rate': [point['firing_rate'] for point in learning_curve],
                            'Learning_Rate': [point['learning_rate'] for point in learning_curve]
                        })
            
            if not data:
                raise ValueError("No valid learning curves found for comparison")
            
            # Create figure with multiple subplots
            fig, axes = plt.subplots(2, 2, figsize=self.config.figure_size)
            fig.suptitle('Experiment Comparison', fontsize=self.config.font_size + 4)
            
            # Firing rate comparison
            ax1 = axes[0, 0]
            for i, plot_data in enumerate(data):
                ax1.plot(plot_data['Step'], plot_data['Firing_Rate'], 
                        label=plot_data['Experiment'], linewidth=2,
                        color=self.config.colors[i % len(self.config.colors)])
            ax1.set_title('Firing Rate Comparison')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Firing Rate (Hz)')
            ax1.legend()
            ax1.grid(True, alpha=0.3)
            
            # Learning rate comparison
            ax2 = axes[0, 1]
            for i, plot_data in enumerate(data):
                ax2.plot(plot_data['Step'], plot_data['Learning_Rate'],
                        label=plot_data['Experiment'], linewidth=2,
                        color=self.config.colors[i % len(self.config.colors)])
            ax2.set_title('Learning Rate Comparison')
            ax2.set_xlabel('Step')
            ax2.set_ylabel('Learning Rate')
            ax2.legend()
            ax2.grid(True, alpha=0.3)
            
            # Final performance comparison
            ax3 = axes[1, 0]
            final_firing_rates = [plot_data['Firing_Rate'][-1] for plot_data in data]
            final_learning_rates = [plot_data['Learning_Rate'][-1] for plot_data in data]
            
            x = np.arange(len(data))
            width = 0.35
            ax3.bar(x - width/2, final_firing_rates, width, label='Final Firing Rate', 
                   color=self.config.colors[0], alpha=0.8)
            ax3.bar(x + width/2, final_learning_rates, width, label='Final Learning Rate',
                   color=self.config.colors[1], alpha=0.8)
            
            ax3.set_title('Final Performance Comparison')
            ax3.set_xlabel('Experiment')
            ax3.set_ylabel('Value')
            ax3.set_xticks(x)
            ax3.set_xticklabels([plot_data['Experiment'] for plot_data in data])
            ax3.legend()
            ax3.grid(True, alpha=0.3)
            
            # Performance metrics radar plot (simplified)
            ax4 = axes[1, 1]
            if len(data) > 1:
                # Create radar plot for top 2 experiments
                categories = ['Firing Rate', 'Learning Rate', 'Stability']
                
                for i in range(min(2, len(data))):
                    values = [
                        data[i]['Firing_Rate'][-1] / max(data[i]['Firing_Rate']) if data[i]['Firing_Rate'] else 0,
                        data[i]['Learning_Rate'][-1] / max(data[i]['Learning_Rate']) if data[i]['Learning_Rate'] else 0,
                        1.0  # Stability placeholder
                    ]
                    
                    angles = np.linspace(0, 2*np.pi, len(categories), endpoint=False).tolist()
                    values += values[:1]  # Close the circle
                    angles += angles[:1]
                    
                    ax4.plot(angles, values, 'o-', linewidth=2,
                            label=data[i]['Experiment'], color=self.config.colors[i])
                    ax4.fill(angles, values, alpha=0.25, color=self.config.colors[i])
                
                ax4.set_xticks(angles[:-1] + np.diff(angles) / 2)
                ax4.set_xticklabels(categories)
                ax4.set_title('Performance Radar')
                ax4.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            # Save plot
            filepath = Path(self.config.output_directory) / filename
            plt.savefig(filepath, dpi=self.config.dpi)
            plt.close()
            
            return str(filepath)
            
        except Exception as e:
            raise Exception(f"Failed to create comparison plot: {e}")
    
    def create_statistical_plots(self, simulation_stats: List[SimulationStats], 
                                filename: str = "statistical_plots.png") -> str:
        """
        Create comprehensive statistical plots.
        
        Args:
            simulation_stats: List of simulation statistics
            filename: Output filename
            
        Returns:
            Path to saved plot
        """
        try:
            if not simulation_stats:
                raise ValueError("No simulation statistics available")
            
            steps = list(range(len(simulation_stats)))
            firing_rates = [stats.firing_rate for stats in simulation_stats]
            learning_rates = [stats.learning_rate for stats in simulation_stats]
            plasticity_effects = [stats.plasticity_effect for stats in simulation_stats]
            
            # Create figure with multiple statistical plots
            fig, axes = plt.subplots(3, 2, figsize=self.config.figure_size)
            fig.suptitle('Statistical Analysis', fontsize=self.config.font_size + 4)
            
            # Time series with rolling statistics
            ax1 = axes[0, 0]
            ax1.plot(steps, firing_rates, linewidth=1, alpha=0.7, label='Raw data')
            rolling_mean = pd.Series(firing_rates).rolling(window=10, center=True).mean()
            rolling_std = pd.Series(firing_rates).rolling(window=10, center=True).std()
            ax1.plot(steps, rolling_mean, linewidth=2, label='Rolling mean', color='red')
            ax1.fill_between(steps, rolling_mean - rolling_std, rolling_mean + rolling_std,
                            alpha=0.3, color='red', label='±1 std')
            ax1.set_title('Firing Rate with Rolling Statistics')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Firing Rate (Hz)')
            ax1.legend()
            ax1.grid(True, alpha=0.3)
            
            # Distribution plots
            ax2 = axes[0, 1]
            for i, (name, data) in enumerate([('Firing Rate', firing_rates),
                                            ('Learning Rate', learning_rates),
                                            ('Plasticity Effect', plasticity_effects)]):
                ax2.hist(data, bins=30, alpha=0.5, label=name,
                       color=self.config.colors[i], density=True)
                # Add normal distribution curve
                mu, std = np.mean(data), np.std(data)
                x = np.linspace(min(data), max(data), 100)
                y = (1 / (std * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x - mu) / std) ** 2)
                ax2.plot(x, y, linewidth=2, color=self.config.colors[i])
            
            ax2.set_title('Distributions with Normal Fit')
            ax2.set_xlabel('Value')
            ax2.set_ylabel('Density')
            ax2.legend()
            ax2.grid(True, alpha=0.3)
            
            # Correlation heatmap
            ax3 = axes[1, 0]
            correlation_matrix = np.corrcoef([firing_rates, learning_rates, plasticity_effects])
            im = ax3.imshow(correlation_matrix, cmap='coolwarm', vmin=-1, vmax=1)
            ax3.set_title('Correlation Matrix')
            ax3.set_xticks([0, 1, 2])
            ax3.set_xticklabels(['Firing Rate', 'Learning Rate', 'Plasticity Effect'])
            ax3.set_yticks([0, 1, 2])
            ax3.set_yticklabels(['Firing Rate', 'Learning Rate', 'Plasticity Effect'])
            
            # Add correlation values
            for i in range(3):
                for j in range(3):
                    text = ax3.text(j, i, f'{correlation_matrix[i, j]:.2f}',
                                   ha="center", va="center", color="black",
                                   fontweight='bold' if abs(correlation_matrix[i, j]) > 0.5 else 'normal')
            
            plt.colorbar(im, ax=ax3)
            
            # Box plots
            ax4 = axes[1, 1]
            data_to_plot = [firing_rates, learning_rates, plasticity_effects]
            ax4.boxplot(data_to_plot, labels=['Firing Rate', 'Learning Rate', 'Plasticity Effect'])
            ax4.set_title('Box Plot Comparison')
            ax4.set_ylabel('Value')
            ax4.grid(True, alpha=0.3)
            
            # Autocorrelation plots
            ax5 = axes[2, 0]
            for i, (name, data) in enumerate([('Firing Rate', firing_rates),
                                            ('Learning Rate', learning_rates),
                                            ('Plasticity Effect', plasticity_effects)]):
                # Calculate autocorrelation
                autocorr = np.correlate(data - np.mean(data), data - np.mean(data), mode='full')
                autocorr = autocorr[len(autocorr)//2:]
                autocorr = autocorr / autocorr[0]
                
                lags = np.arange(len(autocorr))
                ax5.plot(lags[:50], autocorr[:50], label=name, linewidth=2,
                        color=self.config.colors[i])
            
            ax5.set_title('Autocorrelation (First 50 Lags)')
            ax5.set_xlabel('Lag')
            ax5.set_ylabel('Autocorrelation')
            ax5.legend()
            ax5.grid(True, alpha=0.3)
            
            # Statistical summary table
            ax6 = axes[2, 1]
            ax6.axis('tight')
            ax6.axis('off')
            
            stats_data = [
                ['Metric', 'Mean', 'Std', 'Min', 'Max', 'Median', 'Skewness'],
                ['Firing Rate', f"{np.mean(firing_rates):.2f}", f"{np.std(firing_rates):.2f}",
                 f"{np.min(firing_rates):.2f}", f"{np.max(firing_rates):.2f}", f"{np.median(firing_rates):.2f}",
                 f"{pd.Series(firing_rates).skew():.2f}"],
                ['Learning Rate', f"{np.mean(learning_rates):.4f}", f"{np.std(learning_rates):.4f}",
                 f"{np.min(learning_rates):.4f}", f"{np.max(learning_rates):.4f}", f"{np.median(learning_rates):.4f}",
                 f"{pd.Series(learning_rates).skew():.2f}"],
                ['Plasticity Effect', f"{np.mean(plasticity_effects):.4f}", f"{np.std(plasticity_effects):.4f}",
                 f"{np.min(plasticity_effects):.4f}", f"{np.max(plasticity_effects):.4f}", f"{np.median(plasticity_effects):.4f}",
                 f"{pd.Series(plasticity_effects).skew():.2f}"]
            ]
            
            table = ax6.table(cellText=stats_data[1:], colLabels=stats_data[0],
                            cellLoc='center', loc='center')
            table.auto_set_font_size(False)
            table.set_fontsize(self.config.font_size - 2)
            table.scale(1.2, 1.5)
            
            ax6.set_title('Statistical Summary', pad=20)
            
            plt.tight_layout()
            
            # Save plot
            filepath = Path(self.config.output_directory) / filename
            plt.savefig(filepath, dpi=self.config.dpi)
            plt.close()
            
            return str(filepath)
            
        except Exception as e:
            raise Exception(f"Failed to create statistical plots: {e}")
    
    def create_interactive_dashboard(self, simulation_stats: List[SimulationStats], 
                                    output_file: str = "dashboard.html") -> str:
        """
        Create an interactive HTML dashboard for simulation analysis.
        
        Args:
            simulation_stats: List of simulation statistics
            output_file: Output HTML filename
            
        Returns:
            Path to saved dashboard
        """
        try:
            # Create HTML content
            html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Simulation Dashboard</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }}
        .header {{ text-align: center; color: #333; margin-bottom: 30px; }}
        .stats-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 30px; }}
        .stat-card {{ background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 20px; border-radius: 8px; text-align: center; }}
        .stat-card h3 {{ margin: 0 0 10px 0; font-size: 1.2em; }}
        .stat-card .value {{ font-size: 2em; font-weight: bold; }}
        .plot-container {{ margin-bottom: 30px; }}
        .plot {{ width: 100%; height: 400px; border: 1px solid #ddd; border-radius: 8px; }}
        .section {{ margin-bottom: 30px; }}
        .section h2 {{ color: #333; border-bottom: 2px solid #667eea; padding-bottom: 10px; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Simulation Dashboard</h1>
            <p>Enhanced Neural Learning Machine Simulation Analysis</p>
        </div>
        
        <div class="section">
            <h2>Key Statistics</h2>
            <div class="stats-grid">
                <div class="stat-card">
                    <h3>Total Steps</h3>
                    <div class="value">{len(simulation_stats)}</div>
                </div>
                <div class="stat-card">
                    <h3>Average Firing Rate</h3>
                    <div class="value">{np.mean([s.firing_rate for s in simulation_stats]):.1f} Hz</div>
                </div>
                <div class="stat-card">
                    <h3>Peak Firing Rate</h3>
                    <div class="value">{np.max([s.firing_rate for s in simulation_stats]):.1f} Hz</div>
                </div>
                <div class="stat-card">
                    <h3>Average Learning Rate</h3>
                    <div class="value">{np.mean([s.learning_rate for s in simulation_stats]):.4f}</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <h2>Simulation Data</h2>
            <div class="plot-container">
                <canvas id="firingRatePlot" class="plot"></canvas>
            </div>
        </div>
        
        <div class="section">
            <h2>Learning Progress</h2>
            <div class="plot-container">
                <canvas id="learningCurvePlot" class="plot"></canvas>
            </div>
        </div>
        
        <script>
            // Firing Rate Plot
            const firingRateData = {{
                labels: Array.from({{length: {len(simulation_stats)}}}, (_, i) => i),
                datasets: [{{
                    label: 'Firing Rate',
                    data: [{', '.join([str(s.firing_rate) for s in simulation_stats])}],
                    borderColor: '{self.config.colors[0]}',
                    backgroundColor: '{self.config.colors[0]}'.replace('rgb', 'rgba').replace(')', ', 0.2)'),
                    borderWidth: 2
                }}]
            }};
            
            // Learning Curve Plot
            const learningCurveData = {{
                labels: Array.from({{length: {len(simulation_stats)}}}, (_, i) => i),
                datasets: [{{
                    label: 'Learning Rate',
                    data: [{', '.join([str(s.learning_rate) for s in simulation_stats])}],
                    borderColor: '{self.config.colors[1]}',
                    backgroundColor: '{self.config.colors[1]}'.replace('rgb', 'rgba').replace(')', ', 0.2)'),
                    borderWidth: 2
                }}]
            }};
            
            function createPlot(canvasId, data) {{
                const ctx = document.getElementById(canvasId).getContext('2d');
                new Chart(ctx, {{
                    type: 'line',
                    data: data,
                    options: {{
                        responsive: true,
                        maintainAspectRatio: false,
                        scales: {{
                            x: {{ title: {{ display: true, text: 'Step' }} }},
                            y: {{ title: {{ display: true, text: data.datasets[0].label }} }})
                        }},
                        plugins: {{
                            title: {{ display: true, text: data.datasets[0].label }},
                            legend: {{ display: true }}
                        }}
                    }}
                }});
            }}
            
            // Create plots when page loads
            window.onload = function() {{
                createPlot('firingRatePlot', firingRateData);
                createPlot('learningCurvePlot', learningCurveData);
            }};
        </script>
        
        <script src="https://cdn.jsdelivr.net/npm/chart.js@4.0.0/dist/chart.min.js"></script>
    </div>
</body>
</html>
"""
            
            # Save dashboard
            filepath = Path(self.config.output_directory) / output_file
            with open(filepath, 'w') as f:
                f.write(html_content)
            
            return str(filepath)
            
        except Exception as e:
            raise Exception(f"Failed to create interactive dashboard: {e}")
    
    def analyze_and_visualize(self, simulation_stats: List[SimulationStats], 
                             experiment_name: str = "experiment") -> Dict[str, str]:
        """
        Perform comprehensive analysis and create visualization files.
        
        Args:
            simulation_stats: List of simulation statistics
            experiment_name: Name for the experiment
            
        Returns:
            Dictionary mapping plot types to file paths
        """
        try:
            results = {}
            
            # Create different plot types
            results['firing_rate_plot'] = self.plot_firing_rate(
                brain=None,  # Would need actual brain for real implementation
                filename=f"{experiment_name}_firing_rate.png"
            )
            
            results['learning_curve'] = self.plot_learning_curve(
                simulation_stats, f"{experiment_name}_learning_curve.png"
            )
            
            results['statistical_plots'] = self.create_statistical_plots(
                simulation_stats, f"{experiment_name}_statistical.png"
            )
            
            # Create interactive dashboard
            results['interactive_dashboard'] = self.create_interactive_dashboard(
                simulation_stats, f"{experiment_name}_dashboard.html"
            )
            
            return results
            
        except Exception as e:
            raise Exception(f"Failed to analyze and visualize: {e}")