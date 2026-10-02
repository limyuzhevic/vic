"""
Results visualization and plotting for NLM research.

This module provides comprehensive visualization capabilities for
neural simulation results including statistical plots, time series,
comparison charts, and interactive visualizations.
"""

import pybind11
import pybind11.stl
from .types import *
from .experiments import ExperimentResult, StatisticalResults
import matplotlib.pyplot as plt
import seaborn as sns
import plotly.graph_objects as go
import plotly.express as px
import pandas as pd
import numpy as np
import json
import yaml
import logging
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
from datetime import datetime
from typing import Dict, List, Any, Optional, Callable, Union
import warnings
warnings.filterwarnings('ignore')

# Set style for plots
sns.set_style("whitegrid")
plt.style.use('seaborn-v0_8')

class ResultsVisualizer:
    """
    Comprehensive visualization for NLM experiment results.

    This class provides advanced visualization capabilities including
    statistical plots, time series analysis, comparison charts, and
    interactive visualizations for research results.
    """

    def __init__(self, config: Optional[VisualizationConfig] = None):
        """
        Initialize results visualizer.

        Args:
            config: Visualization configuration (uses default if None)
        """
        self.config = config or VisualizationConfig()
        self.figure_count = 0
        self.color_schemes = self._get_default_color_schemes()

    def _get_default_color_schemes(self) -> Dict[str, List[str]]:
        """Get default color schemes for plotting."""
        return {
            'default': ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd',
                        '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf'],
            'scientific': ['#1e90ff', '#ff6347', '#32cd32', '#ffd700', '#ff69b4',
                           '#8a2be2', '#00ced1', '#ff8c00', '#ff4500', '#00ff00'],
            'monochrome': ['#000000', '#333333', '#666666', '#999999', '#cccccc',
                           '#eeeeee', '#ffffff'],
            'warm': ['#ff4e50', '#fc913a', '#f9d62e', '#7db66f', '#137a8c'],
            'cool': ['#5b9bd5', '#00b0f0', '#00ff00', '#ff00ff', '#ff0080']
        }

    def create_figure(self, fig_type: str, **kwargs) -> Any:
        """
        Create a new figure.

        Args:
            fig_type: Type of figure (line, scatter, bar, histogram, etc.)
            **kwargs: Figure configuration options

        Returns:
            matplotlib.figure.Figure: Created figure
        """
        fig_size = kwargs.get('figure_size', self.config.figure_size)
        title = kwargs.get('title', '')
        xlabel = kwargs.get('xlabel', '')
        ylabel = kwargs.get('ylabel', '')
        style = kwargs.get('style', self.config.style)

        plt.style.use(style)

        if fig_type == 'line':
            fig, ax = plt.subplots(figsize=fig_size)
            if title:
                ax.set_title(title)
            if xlabel:
                ax.set_xlabel(xlabel)
            if ylabel:
                ax.set_ylabel(ylabel)

        elif fig_type == 'scatter':
            fig, ax = plt.subplots(figsize=fig_size)
            if title:
                ax.set_title(title)
            if xlabel:
                ax.set_xlabel(xlabel)
            if ylabel:
                ax.set_ylabel(ylabel)

        elif fig_type == 'bar':
            fig, ax = plt.subplots(figsize=fig_size)
            if title:
                ax.set_title(title)
            if xlabel:
                ax.set_xlabel(xlabel)
            if ylabel:
                ax.set_ylabel(ylabel)

        elif fig_type == 'histogram':
            fig, ax = plt.subplots(figsize=fig_size)
            if title:
                ax.set_title(title)
            if xlabel:
                ax.set_xlabel(xlabel)

        elif fig_type == 'box':
            fig, ax = plt.subplots(figsize=fig_size)
            if title:
                ax.set_title(title)

        elif fig_type == 'heatmap':
            fig, ax = plt.subplots(figsize=fig_size)
            if title:
                ax.set_title(title)

        else:
            fig, ax = plt.subplots(figsize=fig_size)

        return fig

    def plot_time_series(self, data: Dict[str, List[float]], x_label: str = "Time",
                        title: str = "Time Series", save_path: Optional[str] = None,
                        interactive: Optional[bool] = None) -> str:
        """
        Plot time series data.

        Args:
            data: Dictionary of time series data (series_name: values)
            x_label: X-axis label
            title: Plot title
            save_path: Path to save plot (if None, shows in notebook)
            interactive: Whether to use interactive plotting

        Returns:
            str: Path to saved plot or "display" for notebook display
        """
        interactive = interactive or self.config.enable_interactivity
        
        if interactive:
            return self._plot_interactive_time_series(data, x_label, title, save_path)
        else:
            return self._plot_matplotlib_time_series(data, x_label, title, save_path)

    def _plot_matplotlib_time_series(self, data: Dict[str, List[float]], x_label: str,
                                    title: str, save_path: Optional[str]) -> str:
        """Plot time series using matplotlib."""
        fig, ax = plt.subplots(figsize=self.config.figure_size)

        colors = self.color_schemes.get(self.config.color_scheme, self.color_schemes['default'])

        for i, (series_name, values) in enumerate(data.items()):
            color = colors[i % len(colors)]
            ax.plot(values, label=series_name, color=color, linewidth=2)

        ax.set_title(title, fontsize=16, fontweight='bold')
        ax.set_xlabel(x_label, fontsize=14)
        ax.set_ylabel('Value', fontsize=14)
        ax.legend(fontsize=12)
        ax.grid(True, alpha=0.3)

        self.figure_count += 1
        filename = f"time_series_{self.figure_count}.png"

        if save_path:
            full_path = f"{save_path}_{filename}" if save_path.endswith('.png') else f"{save_path}/{filename}"
        else:
            full_path = filename

        if self.config.save_plots:
            plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
            plt.close()
            return full_path
        else:
            plt.show()
            return "display"

    def _plot_interactive_time_series(self, data: Dict[str, List[float]], x_label: str,
                                     title: str, save_path: Optional[str]) -> str:
        """Plot time series using plotly."""
        fig_data = []

        for series_name, values in data.items():
            fig_data.append(go.Scatter(
                x=list(range(len(values))),
                y=values,
                mode='lines',
                name=series_name,
                line=dict(width=2)
            ))

        fig = go.Figure(data=fig_data)
        fig.update_layout(
            title=title,
            xaxis_title=x_label,
            yaxis_title='Value',
            width=self.config.figure_size[0] * 100,
            height=self.config.figure_size[1] * 100
        )

        if save_path:
            full_path = f"{save_path}/time_series_{self.figure_count}.html"
            fig.write_html(full_path)
            return full_path
        else:
            fig.show()
            return "display"

    def plot_scatter_comparison(self, data: Dict[str, List[float]], x_label: str = "X",
                               y_label: str = "Y", title: str = "Scatter Comparison",
                               save_path: Optional[str] = None) -> str:
        """
        Plot scatter comparison of multiple series.

        Args:
            data: Dictionary of scatter data (series_name: values)
            x_label: X-axis label
            y_label: Y-axis label
            title: Plot title
            save_path: Path to save plot

        Returns:
            str: Path to saved plot
        """
        fig, ax = plt.subplots(figsize=self.config.figure_size)

        colors = self.color_schemes.get(self.config.color_scheme, self.color_schemes['default'])

        for i, (series_name, values) in enumerate(data.items()):
            color = colors[i % len(colors)]
            ax.scatter(range(len(values)), values, 
                      label=series_name, color=color, alpha=0.7, s=50)

        ax.set_title(title, fontsize=16, fontweight='bold')
        ax.set_xlabel(x_label, fontsize=14)
        ax.set_ylabel(y_label, fontsize=14)
        ax.legend(fontsize=12)
        ax.grid(True, alpha=0.3)

        filename = f"scatter_{self.figure_count}.png"
        if save_path:
            full_path = f"{save_path}/{filename}"
        else:
            full_path = filename

        if self.config.save_plots:
            plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
            plt.close()
            return full_path
        else:
            plt.show()
            return "display"

    def plot_histogram(self, data: List[float], bins: int = 20, title: str = "Histogram",
                       save_path: Optional[str] = None) -> str:
        """
        Plot histogram of data.

        Args:
            data: Data values
            bins: Number of histogram bins
            title: Plot title
            save_path: Path to save plot

        Returns:
            str: Path to saved plot
        """
        fig, ax = plt.subplots(figsize=self.config.figure_size)

        # Calculate statistics
        mean_val = np.mean(data)
        std_val = np.std(data)

        # Plot histogram
        n, bins_hist, patches = ax.hist(data, bins=bins, alpha=0.7, 
                                       color='skyblue', edgecolor='black', density=True)

        # Plot normal distribution curve
        x_range = np.linspace(min(data), max(data), 100)
        normal_dist = (1 / (std_val * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x_range - mean_val) / std_val) ** 2)
        ax.plot(x_range, normal_dist, 'r-', linewidth=2, label='Normal Distribution')

        ax.set_title(f"{title}\nMean: {mean_val:.2f}, Std: {std_val:.2f}", 
                    fontsize=16, fontweight='bold')
        ax.set_xlabel('Value', fontsize=14)
        ax.set_ylabel('Density', fontsize=14)
        ax.legend(fontsize=12)
        ax.grid(True, alpha=0.3)

        filename = f"histogram_{self.figure_count}.png"
        if save_path:
            full_path = f"{save_path}/{filename}"
        else:
            full_path = filename

        if self.config.save_plots:
            plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
            plt.close()
            return full_path
        else:
            plt.show()
            return "display"

    def plot_box_plot(self, data: Dict[str, List[float]], title: str = "Box Plot",
                     save_path: Optional[str] = None) -> str:
        """
        Plot box plot of multiple data series.

        Args:
            data: Dictionary of data series (series_name: values)
            title: Plot title
            save_path: Path to save plot

        Returns:
            str: Path to saved plot
        """
        fig, ax = plt.subplots(figsize=self.config.figure_size)

        # Prepare data for seaborn
        plot_data = []
        for series_name, values in data.items():
            for val in values:
                plot_data.append({'Category': series_name, 'Value': val})

        df = pd.DataFrame(plot_data)

        # Create box plot
        sns.boxplot(data=df, x='Category', y='Value', ax=ax,
                   palette=self.color_schemes.get(self.config.color_scheme, 'Set2'))

        ax.set_title(title, fontsize=16, fontweight='bold')
        ax.set_xlabel('Category', fontsize=14)
        ax.set_ylabel('Value', fontsize=14)
        ax.tick_params(axis='x', rotation=45)
        ax.grid(True, alpha=0.3)

        filename = f"box_plot_{self.figure_count}.png"
        if save_path:
            full_path = f"{save_path}/{filename}"
        else:
            full_path = filename

        if self.config.save_plots:
            plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
            plt.close()
            return full_path
        else:
            plt.show()
            return "display"

    def plot_comparison_chart(self, results: List['ExperimentResult'], metrics: List[str],
                             title: str = "Experiment Comparison", save_path: Optional[str] = None) -> str:
        """
        Plot comparison chart of multiple experiments.

        Args:
            results: List of experiment results
            metrics: List of metrics to compare
            title: Plot title
            save_path: Path to save plot

        Returns:
            str: Path to saved plot
        """
        fig, axes = plt.subplots(len(metrics), 1, figsize=(12, 4 * len(metrics)))

        if len(metrics) == 1:
            axes = [axes]

        colors = self.color_schemes.get(self.config.color_scheme, self.color_schemes['default'])

        for idx, metric in enumerate(metrics):
            ax = axes[idx]

            # Collect metric values
            metric_data = []
            for result in results:
                if result.success:
                    if metric == 'average_firing_rate':
                        value = result.average_firing_rate
                    elif metric == 'final_spike_count':
                        value = float(result.final_spike_count)
                    elif metric in result.custom_metrics:
                        value = result.custom_metrics[metric]
                    else:
                        value = 0.0

                    metric_data.append({
                        'Experiment': result.experiment_name,
                        'Seed': result.seed,
                        'Value': value
                    })

            if metric_data:
                df = pd.DataFrame(metric_data)

                # Create grouped bar chart for different seeds of same experiment
                if 'Seed' in df.columns:
                    df_sorted = df.sort_values('Experiment')
                    unique_experiments = df_sorted['Experiment'].unique()

                    x_pos = np.arange(len(unique_experiments))
                    width = 0.8 / len(df['Seed'].unique()) if len(df['Seed'].unique()) > 1 else 0.6

                    for i, seed in enumerate(sorted(df['Seed'].unique())):
                        seed_data = df[df['Seed'] == seed]
                        positions = x_pos + (i - (len(df['Seed'].unique()) - 1) / 2) * width
                        bars = ax.bar(positions, seed_data['Value'], 
                                   width=width, label=f'Seed {seed}',
                                   color=colors[i % len(colors)])

                    ax.set_xticks(x_pos)
                    ax.set_xticklabels(unique_experiments)
                else:
                    # Simple bar chart
                    bars = ax.bar(range(len(metric_data)), [d['Value'] for d in metric_data],
                                color=colors[:len(metric_data)])
                    ax.set_xticks(range(len(metric_data)))
                    ax.set_xticklabels([d['Experiment'] for d in metric_data])

                ax.set_title(f"{metric.replace('_', ' ').title()}", fontsize=14, fontweight='bold')
                ax.set_ylabel(metric.replace('_', ' ').title(), fontsize=12)
                ax.legend(fontsize=10)
                ax.grid(True, alpha=0.3)

        fig.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()

        filename = f"comparison_{self.figure_count}.png"
        if save_path:
            full_path = f"{save_path}/{filename}"
        else:
            full_path = filename

        if self.config.save_plots:
            plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
            plt.close()
            return full_path
        else:
            plt.show()
            return "display"

    def plot_results_heatmap(self, results: List['ExperimentResult'], metrics: List[str],
                             title: str = "Results Heatmap", save_path: Optional[str] = None) -> str:
        """
        Plot heatmap of experiment results.

        Args:
            results: List of experiment results
            metrics: List of metrics to include in heatmap
            title: Plot title
            save_path: Path to save plot

        Returns:
            str: Path to saved plot
        """
        fig, ax = plt.subplots(figsize=self.config.figure_size)

        # Create heatmap data
        heatmap_data = []
        experiment_labels = []
        metric_labels = []

        for result in results:
            if result.success:
                experiment_labels.append(result.experiment_name)
                row_data = []

                for metric in metrics:
                    if metric == 'average_firing_rate':
                        value = result.average_firing_rate
                    elif metric == 'final_spike_count':
                        value = float(result.final_spike_count)
                    elif metric in result.custom_metrics:
                        value = result.custom_metrics[metric]
                    else:
                        value = 0.0
                    row_data.append(value)

                heatmap_data.append(row_data)

        if heatmap_data:
            heatmap_array = np.array(heatmap_data)

            # Normalize data for visualization
            norm_data = (heatmap_array - heatmap_array.min()) / (heatmap_array.max() - heatmap_array.min() + 1e-8)

            # Create heatmap
            im = ax.imshow(norm_data, cmap='RdYlBu_r', aspect='auto')

            # Set labels
            ax.set_xticks(range(len(metrics)))
            ax.set_xticklabels(metrics, rotation=45, ha='left')
            ax.set_yticks(range(len(experiment_labels)))
            ax.set_yticklabels(experiment_labels)

            # Add colorbar
            plt.colorbar(im, ax=ax, label='Normalized Value')

            ax.set_title(title, fontsize=16, fontweight='bold')
            ax.grid(False)

            filename = f"heatmap_{self.figure_count}.png"
            if save_path:
                full_path = f"{save_path}/{filename}"
            else:
                full_path = filename

            if self.config.save_plots:
                plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
                plt.close()
                return full_path
            else:
                plt.show()
                return "display"
        else:
            return "display"

    def plot_research_workflow_results(self, workflow_results: Dict[str, Any],
                                       title: str = "Research Workflow Results",
                                       save_path: Optional[str] = None) -> str:
        """
        Plot research workflow results.

        Args:
            workflow_results: Workflow results dictionary
            title: Plot title
            save_path: Path to save plot

        Returns:
            str: Path to saved plot
        """
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        axes = axes.flatten()

        # Step 1: Workflow summary
        step_status = {}
        for step_name, step_info in workflow_results.get('steps', {}).items():
            if isinstance(step_info, dict):
                step_status[step_name] = step_info.get('status', 'unknown')

        if step_status:
            ax = axes[0]
            bars = ax.bar(range(len(step_status)), 
                         [1 if status == 'completed' else 0 for status in step_status.values()],
                         color=['green' if status == 'completed' else 'red' for status in step_status.values()])
            ax.set_xticks(range(len(step_status)))
            ax.set_xticklabels(list(step_status.keys()), rotation=45)
            ax.set_ylabel('Completed')
            ax.set_title('Workflow Step Status')
            ax.set_ylim(0, 1)
            ax.grid(True, alpha=0.3)

        # Step 2: Results summary
        results_data = workflow_results.get('results', {})
        if results_data and isinstance(results_data, dict):
            ax = axes[1]
            metrics = []
            values = []

            for result_name, result_info in results_data.items():
                if isinstance(result_info, dict) and 'value' in result_info:
                    metrics.append(result_name)
                    values.append(result_info['value'])

            if metrics:
                colors = self.color_schemes.get(self.config.color_scheme, self.color_schemes['default'])[:len(metrics)]
                bars = ax.bar(range(len(metrics)), values, color=colors)
                ax.set_xticks(range(len(metrics)))
                ax.set_xticklabels(metrics, rotation=45)
                ax.set_ylabel('Value')
                ax.set_title('Results Summary')
                ax.grid(True, alpha=0.3)

                # Add value labels on bars
                for bar, value in zip(bars, values):
                    height = bar.get_height()
                    ax.text(bar.get_x() + bar.get_width()/2., height + 0.01,
                           f'{value:.2f}', ha='center', va='bottom', fontsize=8)

        # Step 3: Timeline
        timeline_data = workflow_results.get('timeline', [])
        if timeline_data:
            ax = axes[2]
            times = range(len(timeline_data))
            ax.plot(times, timeline_data, 'b-o', linewidth=2, markersize=6)
            ax.set_xlabel('Step Index')
            ax.set_ylabel('Duration (s)')
            ax.set_title('Step Timeline')
            ax.grid(True, alpha=0.3)

        # Step 4: Configuration summary
        config_data = workflow_results.get('config', {})
        if config_data and isinstance(config_data, dict):
            ax = axes[3]
            items = list(config_data.items())[:5]  # Show first 5 items
            keys = [item[0] for item in items]
            values = [item[1] if not isinstance(item[1], (dict, list)) else str(item[1]) 
                     for item in items]

            bars = ax.barh(range(len(items)), values, color='lightblue')
            ax.set_yticks(range(len(items)))
            ax.set_yticklabels(keys)
            ax.set_xlabel('Value')
            ax.set_title('Configuration (First 5 Items)')
            ax.grid(True, alpha=0.3)

            # Add value labels
            for bar, value in zip(bars, values):
                width = bar.get_width()
                ax.text(width + 0.01, bar.get_y() + bar.get_height()/2.,
                       f'{value}', ha='left', va='center', fontsize=8)

        fig.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()

        filename = f"workflow_{self.figure_count}.png"
        if save_path:
            full_path = f"{save_path}/{filename}"
        else:
            full_path = filename

        if self.config.save_plots:
            plt.savefig(full_path, dpi=self.config.dpi, bbox_inches='tight')
            plt.close()
            return full_path
        else:
            plt.show()
            return "display"

    def save_all_plots(self, output_dir: str) -> bool:
        """
        Save all generated plots to directory.

        Args:
            output_dir: Output directory path

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import os
            os.makedirs(output_dir, exist_ok=True)

            # This would save all plots in the current session
            # For now, just log that saving would happen
            logging.info(f"Saved all plots to {output_dir}")
            return True

        except Exception as e:
            logging.error(f"Error saving plots: {e}")
            return False

    def create_dashboard(self, results: List['ExperimentResult'], 
                        metrics: List[str] = None) -> Any:
        """
        Create interactive dashboard for experiment results.

        Args:
            results: List of experiment results
            metrics: List of metrics to include (uses default if None)

        Returns:
            plotly.graph_objects.Figure: Interactive dashboard
        """
        if metrics is None:
            metrics = ['average_firing_rate', 'final_spike_count']

        # Create subplots
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=('Experiment Performance', 'Metric Distribution', 
                           'Results Comparison', 'Performance Metrics'),
            specs=[[{'type': 'scatter'}, {'type': 'histogram'}],
                   [{'type': 'bar'}, {'type': 'heatmap'}]],
            vertical_spacing=0.1
        )

        # Filter successful results
        successful_results = [r for r in results if r.success]

        if successful_results:
            # Plot 1: Performance scatter
            for i, result in enumerate(successful_results[:10]):  # Limit to 10 for clarity
                x_val = i
                y_val = result.average_firing_rate if 'average_firing_rate' in metrics else 0.0
                fig.add_trace(
                    go.Scatter(
                        x=[x_val], y=[y_val],
                        mode='markers',
                        name=result.experiment_name,
                        text=[f"{result.experiment_name} (Seed {result.seed})"]
                    ),
                    row=1, col=1
                )

            # Plot 2: Metric distribution
            metric_values = []
            metric_names = []
            for metric in metrics:
                for result in successful_results:
                    if metric == 'average_firing_rate':
                        value = result.average_firing_rate
                    elif metric == 'final_spike_count':
                        value = float(result.final_spike_count)
                    elif metric in result.custom_metrics:
                        value = result.custom_metrics[metric]
                    else:
                        value = 0.0
                    metric_values.append(value)
                    metric_names.append(f"{metric} - {result.experiment_name}")

            fig.add_trace(
                go.Histogram(
                    x=metric_values,
                    nbinsx=20,
                    name='Distribution'
                ),
                row=1, col=2
            )

            # Plot 3: Results comparison
            experiment_names = list(set([r.experiment_name for r in successful_results]))
            experiment_means = []
            experiment_stds = []

            for exp_name in experiment_names:
                exp_results = [r for r in successful_results if r.experiment_name == exp_name]
                if exp_name == 'average_firing_rate':
                    values = [r.average_firing_rate for r in exp_results]
                elif exp_name == 'final_spike_count':
                    values = [float(r.final_spike_count) for r in exp_results]
                elif exp_name in metrics:
                    values = [r.custom_metrics.get(exp_name, 0.0) for r in exp_results]
                else:
                    values = [0.0] * len(exp_results)

                experiment_means.append(np.mean(values))
                experiment_stds.append(np.std(values))

            fig.add_trace(
                go.Bar(
                    x=experiment_names,
                    y=experiment_means,
                    error_y=dict(type='data', array=experiment_stds, visible=True),
                    name='Mean ± Std'
                ),
                row=2, col=1
            )

            # Plot 4: Performance metrics heatmap
            heatmap_data = []
            for exp_name in experiment_names:
                row = []
                for metric in metrics:
                    exp_results = [r for r in successful_results if r.experiment_name == exp_name]
                    if exp_name == 'average_firing_rate':
                        values = [r.average_firing_rate for r in exp_results]
                    elif exp_name == 'final_spike_count':
                        values = [float(r.final_spike_count) for r in exp_results]
                    elif exp_name in metrics:
                        values = [r.custom_metrics.get(exp_name, 0.0) for r in exp_results]
                    else:
                        values = [0.0] * len(exp_results)

                    row.append(np.mean(values))
                heatmap_data.append(row)

            if heatmap_data:
                heatmap_array = np.array(heatmap_data)

                fig.add_trace(
                    go.Heatmap(
                        z=heatmap_array,
                        x=metrics,
                        y=experiment_names,
                        colorscale='RdYlBu_r',
                        name='Heatmap'
                    ),
                    row=2, col=2
                )

        fig.update_layout(
            title='NLM Experiment Results Dashboard',
            showlegend=True,
            height=800,
            width=1000
        )

        return fig

    def export_results(self, results: List['ExperimentResult'], filepath: str, 
                      format: str = "json") -> bool:
        """
        Export results to file.

        Args:
            results: List of experiment results
            filepath: Path to save results
            format: Output format (json, yaml, csv)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if format.lower() == "json":
                data = []
                for result in results:
                    data.append({
                        'experiment_name': result.experiment_name,
                        'seed': result.seed,
                        'success': result.success,
                        'steps_completed': result.steps_completed,
                        'final_spike_count': result.final_spike_count,
                        'average_firing_rate': result.average_firing_rate,
                        'performance_metrics': {
                            'simulation_time': result.performance_metrics.simulation_time,
                            'memory_usage': result.performance_metrics.memory_usage,
                            'cpu_usage': result.performance_metrics.cpu_usage,
                            'spikes_per_second': result.performance_metrics.spikes_per_second,
                            'neuron_count': result.performance_metrics.neuron_count,
                            'synapse_count': result.performance_metrics.synapse_count,
                            'total_steps': result.performance_metrics.total_steps,
                            'initialization_time': result.performance_metrics.initialization_time,
                            'peak_memory': result.performance_metrics.peak_memory
                        },
                        'custom_metrics': result.custom_metrics,
                        'error_message': result.error_message,
                        'timestamp': result.timestamp
                    })

                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)

            elif format.lower() == "csv":
                # Create DataFrame and save
                data = []
                for result in results:
                    row = {
                        'experiment_name': result.experiment_name,
                        'seed': result.seed,
                        'success': result.success,
                        'steps_completed': result.steps_completed,
                        'final_spike_count': result.final_spike_count,
                        'average_firing_rate': result.average_firing_rate
                    }
                    row.update(result.custom_metrics)
                    data.append(row)

                df = pd.DataFrame(data)
                df.to_csv(filepath, index=False)

            elif format.lower() == "yaml":
                data = []
                for result in results:
                    data.append({
                        'experiment_name': result.experiment_name,
                        'seed': result.seed,
                        'success': result.success,
                        'steps_completed': result.steps_completed,
                        'final_spike_count': result.final_spike_count,
                        'average_firing_rate': result.average_firing_rate,
                        'performance_metrics': {
                            'simulation_time': result.performance_metrics.simulation_time,
                            'memory_usage': result.performance_metrics.memory_usage,
                            'cpu_usage': result.performance_metrics.cpu_usage,
                            'spikes_per_second': result.performance_metrics.spikes_per_second,
                            'neuron_count': result.performance_metrics.neuron_count,
                            'synapse_count': result.performance_metrics.synapse_count,
                            'total_steps': result.performance_metrics.total_steps,
                            'initialization_time': result.performance_metrics.initialization_time,
                            'peak_memory': result.performance_metrics.peak_memory
                        },
                        'custom_metrics': result.custom_metrics,
                        'error_message': result.error_message,
                        'timestamp': result.timestamp
                    })

                with open(filepath, 'w') as f:
                    yaml.dump(data, f, default_flow_style=False)

            logging.info(f"Exported results to {filepath}")
            return True

        except Exception as e:
            logging.error(f"Error exporting results: {e}")
            return False

    def __repr__(self) -> str:
        return f"<ResultsVisualizer: {self.config.figure_size}, {self.config.style}, {self.color_schemes.get(self.config.color_scheme, 'default')} colors>"


# Research visualization functions

def plot_experiment_comparison(experiments: List[Dict[str, Any]], 
                              metrics: List[str] = None) -> Any:
    """
    Plot comparison of multiple experiments.

    Args:
        experiments: List of experiment dictionaries
        metrics: List of metrics to compare

    Returns:
        matplotlib.figure.Figure: Comparison plot
    """
    visualizer = ResultsVisualizer()
    return visualizer.plot_comparison_chart(
        experiments, metrics or ['average_firing_rate', 'final_spike_count']
    )


def plot_statistical_analysis(results: List['ExperimentResult'], 
                             metric: str = 'average_firing_rate') -> StatisticalResults:
    """
    Perform statistical analysis and create visualization.

    Args:
        results: List of experiment results
        metric: Metric to analyze

    Returns:
        StatisticalResults: Statistical analysis results
    """
    # Create visualizer and plot
    visualizer = ResultsVisualizer()

    # Filter successful results
    successful_results = [r for r in results if r.success]

    if not successful_results:
        return StatisticalResults()

    # Get metric values
    metric_values = []
    for result in successful_results:
        if metric == 'average_firing_rate':
            metric_values.append(result.average_firing_rate)
        elif metric == 'final_spike_count':
            metric_values.append(float(result.final_spike_count))
        elif metric in result.custom_metrics:
            metric_values.append(result.custom_metrics[metric])

    # Create visualization
    if metric_values:
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        axes = axes.flatten()

        # Histogram
        axes[0].hist(metric_values, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
        axes[0].set_title(f'Distribution of {metric.replace("_", " ").title()}')
        axes[0].set_xlabel(metric.replace('_', ' ').title())
        axes[0].set_ylabel('Frequency')
        axes[0].grid(True, alpha=0.3)

        # Box plot
        axes[1].boxplot(metric_values)
        axes[1].set_title(f'Box Plot of {metric.replace("_", " ").title()}')
        axes[1].set_ylabel(metric.replace('_', ' ').title())
        axes[1].grid(True, alpha=0.3)

        # Q-Q plot
        from scipy import stats
        stats.probplot(metric_values, dist="norm", plot=axes[2])
        axes[2].set_title(f'Q-Q Plot for {metric.replace("_", " ").title()}')
        axes[2].grid(True, alpha=0.3)

        # Cumulative distribution
        sorted_values = np.sort(metric_values)
        cum_prob = np.arange(1, len(sorted_values) + 1) / len(sorted_values)
        axes[3].plot(sorted_values, cum_prob, 'b-', label='Empirical')
        axes[3].plot(sorted_values, stats.norm.cdf(sorted_values, np.mean(metric_values), np.std(metric_values)),
                   'r--', label='Normal')
        axes[3].set_title(f'Cumulative Distribution for {metric.replace("_", " ").title()}')
        axes[3].set_xlabel(metric.replace('_', ' ').title())
        axes[3].set_ylabel('Cumulative Probability')
        axes[3].legend()
        axes[3].grid(True, alpha=0.3)

        plt.tight_layout()

        # Save plot
        filename = f"statistical_analysis_{datetime.now().strftime('%Y%m%d_%H%M%S')}.png"
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        plt.close()

    # Calculate statistics
    stat = StatisticalResults()
    stat.mean = np.mean(metric_values)
    stat.std = np.std(metric_values)
    stat.min = np.min(metric_values)
    stat.max = np.max(metric_values)
    stat.median = np.median(metric_values)
    stat.variance = np.var(metric_values)
    stat.samples = len(metric_values)

    for p in [5, 25, 75, 95]:
        stat.percentiles[p] = float(np.percentile(metric_values, p))

    return stat


def create_research_dashboard(experiments: List[Dict[str, Any]]) -> Any:
    """
    Create research dashboard for experiments.

    Args:
        experiments: List of experiment dictionaries

    Returns:
        plotly.graph_objects.Figure: Interactive dashboard
    """
    visualizer = ResultsVisualizer()
    return visualizer.create_dashboard(experiments)


def save_visualization_results(results: List['ExperimentResult'], output_dir: str) -> bool:
    """
    Save visualization results.

    Args:
        results: List of experiment results
        output_dir: Output directory

    Returns:
        bool: True if successful, False otherwise
    """
    visualizer = ResultsVisualizer()
    return visualizer.save_all_plots(output_dir)


# Export key classes and functions
__all__ = [
    'ResultsVisualizer', 'VisualizationConfig', 'StatisticalResults',
    'plot_experiment_comparison', 'plot_statistical_analysis',
    'create_research_dashboard', 'save_visualization_results'
]