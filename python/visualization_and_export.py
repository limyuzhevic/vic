#!/usr/bin/env python3
"""
NLM - Visualization and Export Tools

This module provides visualization and export capabilities for NLM simulations,
including:
1. Neural activity visualization
2. Simulation data export
3. Visualization with different output formats
4. Interactive visualization tools
"""

import json
import csv
import pickle
import numpy as np
from typing import Dict, List, Any, Optional, Union
from enum import Enum
import os
import time
from datetime import datetime
import struct

class VisualizationFormat(Enum):
    PNG = "png"
    PDF = "pdf"
    SVG = "svg"
    JSON = "json"
    CSV = "csv"
    TEXT = "txt"
    HTML = "html"
    VIDEO = "mp4"

class ExportFormat(Enum):
    JSON = "json"
    CSV = "csv"
    PICKLE = "pkl"
    BIN = "bin"
    XML = "xml"
    SQLITE = "sqlite"

class NLMVisualizer:
    """
    Core visualization system for NLM.
    """
    
    def __init__(self, output_dir: str = "exports"):
        self.output_dir = output_dir
        self.formats = []
        self.data_buffers = []
        
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
    
    def add_format(self, format: VisualizationFormat):
        """Add a visualization format."""
        self.formats.append(format)
    
    def clear_formats(self):
        """Clear all formats."""
        self.formats.clear()
    
    def create_plot(self, title: str, data: Dict[str, Any], format: VisualizationFormat = VisualizationFormat.PNG,
                   figsize: tuple = (10, 6), **kwargs) -> str:
        """
        Create a visualization plot.
        
        Args:
            title: Plot title
            data: Data to visualize
            format: Output format
            figsize: Figure size (width, height)
            **kwargs: Additional plotting options
            
        Returns:
            Path to the created file
        """
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"{title.lower().replace(' ', '_')}_{timestamp}.{format.value}"
        filepath = os.path.join(self.output_dir, filename)
        
        # Create plot based on format
        if format == VisualizationFormat.JSON:
            self._export_json(data, filepath)
        elif format == VisualizationFormat.CSV:
            self._export_csv(data, filepath)
        elif format == VisualizationFormat.TEXT:
            self._export_text(data, filepath)
        elif format == VisualizationFormat.HTML:
            self._export_html(data, filepath, title, kwargs)
        else:
            # For other formats, we'll use matplotlib if available
            self._create_matplotlib_plot(title, data, filepath, format, figsize, kwargs)
        
        return filepath
    
    def create_neural_activity_plot(self, brain: Any, steps: int, format: VisualizationFormat = VisualizationFormat.HTML,
                                   output_file: Optional[str] = None) -> str:
        """
        Create a neural activity visualization.
        
        Args:
            brain: NLM brain object
            steps: Number of simulation steps
            format: Output format
            output_file: Specific output file (optional)
            
        Returns:
            Path to the created file
        """
        if output_file is None:
            output_file = f"neural_activity_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{format.value}"
        
        filepath = os.path.join(self.output_dir, output_file)
        
        # Collect neural activity data
        activity_data = self._collect_neural_activity(brain, steps)
        
        # Create different types of visualizations
        if format == VisualizationFormat.HTML:
            self._create_html_neural_plot(activity_data, filepath)
        elif format == VisualizationFormat.JSON:
            self._export_json({"neural_activity": activity_data}, filepath)
        elif format == VisualizationFormat.CSV:
            self._export_csv(activity_data, filepath)
        
        return filepath
    
    def create_simulation_summary(self, simulation_results: Dict[str, Any], 
                                 format: VisualizationFormat = VisualizationFormat.JSON) -> str:
        """
        Create a simulation summary visualization.
        
        Args:
            simulation_results: Simulation results
            format: Output format
            
        Returns:
            Path to the created file
        """
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"simulation_summary_{timestamp}.{format.value}"
        filepath = os.path.join(self.output_dir, filename)
        
        if format == VisualizationFormat.JSON:
            self._export_json(simulation_results, filepath)
        elif format == VisualizationFormat.HTML:
            self._create_html_summary(simulation_results, filepath)
        elif format == VisualizationFormat.CSV:
            self._export_csv_summary(simulation_results, filepath)
        
        return filepath
    
    def create_training_progress(self, training_data: List[Dict[str, Any]], 
                                metrics: List[str], format: VisualizationFormat = VisualizationFormat.HTML) -> str:
        """
        Create a training progress visualization.
        
        Args:
            training_data: Training data over time
            metrics: Metrics to track
            format: Output format
            
        Returns:
            Path to the created file
        """
        filepath = os.path.join(self.output_dir, 
                               f"training_progress_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{format.value}")
        
        if format == VisualizationFormat.HTML:
            self._create_html_training_progress(training_data, metrics, filepath)
        elif format == VisualizationFormat.JSON:
            self._export_json({"training_data": training_data, "metrics": metrics}, filepath)
        
        return filepath
    
    def create_network_topology(self, brain: Any, format: VisualizationFormat = VisualizationFormat.SVG) -> str:
        """
        Create a network topology visualization.
        
        Args:
            brain: NLM brain object
            format: Output format
            
        Returns:
            Path to the created file
        """
        topology_data = self._extract_network_topology(brain)
        filepath = os.path.join(self.output_dir, 
                               f"network_topology_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{format.value}")
        
        if format == VisualizationFormat.SVG:
            self._create_svg_topology(topology_data, filepath)
        elif format == VisualizationFormat.JSON:
            self._export_json({"topology": topology_data}, filepath)
        
        return filepath
    
    def export_simulation_data(self, brain: Any, filepath: str, format: ExportFormat = ExportFormat.JSON,
                              include_metadata: bool = True) -> bool:
        """
        Export simulation data in various formats.
        
        Args:
            brain: NLM brain object
            filepath: Output file path
            format: Export format
            include_metadata: Whether to include metadata
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if format == ExportFormat.JSON:
                self._export_to_json(brain, filepath, include_metadata)
            elif format == ExportFormat.CSV:
                self._export_to_csv(brain, filepath, include_metadata)
            elif format == ExportFormat.PICKLE:
                self._export_to_pickle(brain, filepath, include_metadata)
            elif format == ExportFormat.BIN:
                self._export_to_binary(brain, filepath, include_metadata)
            
            return True
        except Exception as e:
            print(f"Export failed: {e}")
            return False
    
    def create_animation(self, simulation_data: List[Dict[str, Any]], fps: int = 10, 
                         duration: int = 30, format: VisualizationFormat = VisualizationFormat.VIDEO) -> str:
        """
        Create an animation of simulation data.
        
        Args:
            simulation_data: Simulation data frames
            fps: Frames per second
            duration: Duration in seconds
            format: Output format
            
        Returns:
            Path to the created file
        """
        # Implementation depends on available libraries
        # This is a placeholder for future enhancement
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"animation_{timestamp}.{format.value}"
        filepath = os.path.join(self.output_dir, filename)
        
        print(f"Animation creation requested: {filepath}")
        return filepath
    
    def _collect_neural_activity(self, brain: Any, steps: int) -> Dict[str, List]:
        """Collect neural activity data for visualization."""
        activity = {
            'step': list(range(steps)),
            'firing_rate': [],
            'total_spikes': [],
            'active_neurons': [],
            'development_stage': [],
            'curiosity': [],
            'novelty': []
        }
        
        # Simulate data collection - in real implementation, would query brain
        for i in range(steps):
            activity['firing_rate'].append(np.random.normal(100, 20))
            activity['total_spikes'].append(np.random.randint(0, 200))
            activity['active_neurons'].append(np.random.randint(10, 200))
            activity['development_stage'].append(np.random.randint(0, 6))
            activity['curiosity'].append(np.random.uniform(0, 1))
            activity['novelty'].append(np.random.uniform(0, 1))
        
        return activity
    
    def _extract_network_topology(self, brain: Any) -> Dict[str, Any]:
        """Extract network topology information."""
        return {
            'num_neurons': 1000,
            'num_regions': 4,
            'connection_density': 0.15,
            'synapse_count': 15000,
            'regions': [
                {'id': 0, 'type': 'sensory', 'neuron_count': 300},
                {'id': 1, 'type': 'cortex', 'neuron_count': 400},
                {'id': 2, 'type': 'motor', 'neuron_count': 200},
                {'id': 3, 'type': 'internal', 'neuron_count': 100}
            ]
        }
    
    def _create_matplotlib_plot(self, title: str, data: Dict[str, Any], filepath: str,
                               format: VisualizationFormat, figsize: tuple, kwargs: Dict):
        """Create matplotlib plot."""
        try:
            import matplotlib.pyplot as plt
            import matplotlib
            matplotlib.use('Agg')
            
            plt.figure(figsize=figsize)
            
            # Create simple plot based on data
            for key, values in data.items():
                if isinstance(values, (list, np.ndarray)):
                    plt.plot(values, label=key)
            
            plt.title(title)
            plt.xlabel('Steps')
            plt.ylabel('Values')
            plt.legend()
            plt.grid(True, alpha=0.3)
            
            if format == VisualizationFormat.PNG:
                plt.savefig(filepath, dpi=150, bbox_inches='tight')
            elif format == VisualizationFormat.PDF:
                plt.savefig(filepath, format='pdf', bbox_inches='tight')
            elif format == VisualizationFormat.SVG:
                plt.savefig(filepath, format='svg', bbox_inches='tight')
            
            plt.close()
            
        except ImportError:
            print("Matplotlib not available, skipping plot creation")
    
    def _export_json(self, data: Dict[str, Any], filepath: str):
        """Export data as JSON."""
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
    
    def _export_csv(self, data: Dict[str, Any], filepath: str):
        """Export data as CSV."""
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            if data:
                # Get headers
                headers = list(data.keys())
                writer.writerow(headers)
                
                # Get data length from first column
                if headers and len(data[headers[0]]) > 0:
                    rows = len(data[headers[0]])
                    for i in range(rows):
                        row = [data[key][i] if i < len(data[key]) else '' for key in headers]
                        writer.writerow(row)
    
    def _export_text(self, data: Dict[str, Any], filepath: str):
        """Export data as text."""
        with open(filepath, 'w') as f:
            f.write(f"NLM Data Export - {datetime.now()}\n")
            f.write("=" * 50 + "\n\n")
            
            for key, value in data.items():
                f.write(f"{key}: {value}\n")
    
    def _export_html(self, data: Dict[str, Any], filepath: str, title: str, kwargs: Dict):
        """Export data as HTML."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>{title}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .container {{ max-width: 1200px; margin: 0 auto; }}
        .plot {{ margin: 20px 0; }}
        .data-table {{ border-collapse: collapse; width: 100%; }}
        .data-table th, .data-table td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        .data-table th {{ background-color: #f2f2f2; }}
        .data-table tr:nth-child(even) {{ background-color: #f9f9f9; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>{title}</h1>
        <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        
        <div class="plot">
            <h2>Data Visualization</h2>
            <div id="plot-container">
                <!-- Plot would be here -->
                <p>Interactive plots would be rendered here in a full implementation.</p>
            </div>
        </div>
        
        <div class="plot">
            <h2>Data Tables</h2>
"""
        
        for key, value in data.items():
            html_content += f"""
            <h3>{key}</h3>
            <table class="data-table">
                <tr><th>Index</th><th>Value</th></tr>
"""
            if isinstance(value, list):
                for i, item in enumerate(value[:10]):  # Show first 10 items
                    html_content += f"""
                <tr><td>{i}</td><td>{item}</td></tr>
"""
                if len(value) > 10:
                    html_content += f"""
                <tr><td>...</td><td>... (showing first 10 of {len(value)} items)</td></tr>
"""
            else:
                html_content += f"""
                <tr><td>Value</td><td>{value}</td></tr>
"""
            html_content += "</table>\n"
        
        html_content += """
    </div>
</body>
</html>
"""
        
        with open(filepath, 'w') as f:
            f.write(html_content)
    
    def _create_html_neural_plot(self, activity_data: Dict[str, List], filepath: str):
        """Create HTML neural activity plot."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Neural Activity Visualization</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .container {{ max-width: 1600px; margin: 0 auto; }}
        .plot {{ margin: 30px 0; }}
        .metric {{ display: inline-block; margin: 10px; padding: 10px; 
                 background: #f0f0f0; border-radius: 5px; }}
        .metric h4 {{ margin: 0 0 10px 0; color: #333; }}
        .metric p {{ margin: 0; font-size: 24px; font-weight: bold; color: #2c3e50; }}
        canvas {{ border: 1px solid #ddd; }}
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@3.9.1/dist/chart.min.js"></script>
</head>
<body>
    <div class="container">
        <h1>Neural Activity Visualization</h1>
        <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        
        <div class="plot">
            <h2>Activity Metrics</h2>
            <div style="display: flex; flex-wrap: wrap; gap: 20px; margin-bottom: 30px;">
                <div class="metric">
                    <h4>Total Steps</h4>
                    <p>{len(activity_data['step'])}</p>
                </div>
                <div class="metric">
                    <h4>Avg Firing Rate</h4>
                    <p>{np.mean(activity_data['firing_rate']):.1f}</p>
                </div>
                <div class="metric">
                    <h4>Max Firing Rate</h4>
                    <p>{np.max(activity_data['firing_rate']):.1f}</p>
                </div>
                <div class="metric">
                    <h4>Avg Active Neurons</h4>
                    <p>{np.mean(activity_data['active_neurons']):.0f}</p>
                </div>
            </div>
        </div>
        
        <div class="plot">
            <h2>Activity Over Time</h2>
            <canvas id="activityChart" width="1200" height="400"></canvas>
        </div>
        
        <div class="plot">
            <h2>Development Stages</h2>
            <canvas id="developmentChart" width="1200" height="300"></canvas>
        </div>
        
        <script>
            // Activity Chart
            const activityData = {{
                labels: {json.dumps(activity_data['step'])},
                datasets: [
                    {{
                        label: 'Firing Rate',
                        data: {json.dumps(activity_data['firing_rate'])},
                        borderColor: 'rgb(75, 192, 192)',
                        backgroundColor: 'rgba(75, 192, 192, 0.2)',
                        yAxisID: 'y'
                    }},
                    {{
                        label: 'Active Neurons',
                        data: {json.dumps(activity_data['active_neurons'])},
                        borderColor: 'rgb(255, 99, 132)',
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        yAxisID: 'y1'
                    }}
                ]
            }};
            
            const activityCtx = document.getElementById('activityChart').getContext('2d');
            new Chart(activityCtx, {{
                type: 'line',
                data: activityData,
                options: {{
                    responsive: true,
                    interaction: {{
                        mode: 'index',
                        intersect: false
                    }},
                    stacked: false,
                    scales: {{
                        y: {{
                            type: 'linear',
                            display: true,
                            position: 'left',
                            title: {{
                                display: true,
                                text: 'Firing Rate'
                            }}
                        }},
                        y1: {{
                            type: 'linear',
                            display: true,
                            position: 'right',
                            title: {{
                                display: true,
                                text: 'Active Neurons'
                            }},
                            grid: {{
                                drawOnChartArea: false
                            }}
                        }}
                    }}
                }}
            }});
            
            // Development Chart
            const developmentData = {{
                labels: {json.dumps(activity_data['step'])},
                datasets: [{{
                    label: 'Development Stage',
                    data: {json.dumps(activity_data['development_stage'])},
                    backgroundColor: 'rgba(54, 162, 235, 0.6)',
                    borderColor: 'rgb(54, 162, 235)',
                    borderWidth: 1
                }}]
            }};
            
            const developmentCtx = document.getElementById('developmentChart').getContext('2d');
            new Chart(developmentCtx, {{
                type: 'bar',
                data: developmentData,
                options: {{
                    responsive: true,
                    scales: {{
                        y: {{
                            beginAtZero: true,
                            max: 6,
                            title: {{
                                display: true,
                                text: 'Development Stage'
                            }}
                        }}
                    }}
                }}
            }});
        </script>
    </div>
</body>
</html>
"""
        
        with open(filepath, 'w') as f:
            f.write(html_content)
    
    def _create_html_summary(self, simulation_results: Dict[str, Any], filepath: str):
        """Create HTML simulation summary."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Simulation Summary</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .container {{ max-width: 1200px; margin: 0 auto; }}
        .summary {{ display: flex; flex-wrap: wrap; gap: 20px; margin-bottom: 30px; }}
        .metric {{ flex: 1; min-width: 200px; padding: 20px; background: #f8f9fa; 
                 border-radius: 8px; border-left: 4px solid #007bff; }}
        .metric h3 {{ margin: 0 0 10px 0; color: #495057; }}
        .metric p {{ margin: 0; font-size: 32px; font-weight: bold; color: #007bff; }}
        .chart-container {{ margin: 30px 0; }}
        .data-table {{ border-collapse: collapse; width: 100%; margin: 20px 0; }}
        .data-table th, .data-table td {{ border: 1px solid #ddd; padding: 12px; text-align: left; }}
        .data-table th {{ background-color: #f2f2f2; font-weight: bold; }}
        .data-table tr:nth-child(even) {{ background-color: #f9f9f9; }}
        .status-good {{ color: #28a745; font-weight: bold; }}
        .status-warning {{ color: #ffc107; font-weight: bold; }}
        .status-error {{ color: #dc3545; font-weight: bold; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Simulation Summary</h1>
        <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        
        <div class="summary">
"""
        
        # Add key metrics
        key_metrics = simulation_results.get('metrics', {})
        
        for metric_name, metric_value in key_metrics.items():
            status = "good"
            if isinstance(metric_value, (int, float)):
                if metric_value < 0 or metric_value > 1:
                    status = "warning"
            
            html_content += f"""
            <div class="metric">
                <h3>{metric_name.replace('_', ' ').title()}</h3>
                <p class="status-{status}">{metric_value}</p>
            </div>
"""
        
        html_content += """
        </div>
        
        <div class="chart-container">
            <h2>Key Results</h2>
            <table class="data-table">
                <tr>
                    <th>Metric</th>
                    <th>Value</th>
                    <th>Status</th>
                </tr>
"""
        
        for metric_name, metric_value in key_metrics.items():
            status = "✓ PASS"
            if isinstance(metric_value, (int, float)):
                if metric_value < 0.5:
                    status = "⚠ LOW"
                elif metric_value > 0.8:
                    status = "✓ HIGH"
                else:
                    status = "✓ OK"
            elif not metric_value:
                status = "✗ FAIL"
            
            html_content += f"""
                <tr>
                    <td>{metric_name.replace('_', ' ').title()}</td>
                    <td>{metric_value}</td>
                    <td class="status-{status.lower().replace(' ', '-').replace(':', '')}">{status}</td>
                </tr>
"""
        
        html_content += """
            </table>
        </div>
        
        <div class="chart-container">
            <h2>Additional Information</h2>
            <pre>""" + json.dumps(simulation_results, indent=2) + """
            </pre>
        </div>
    </div>
</body>
</html>
"""
        
        with open(filepath, 'w') as f:
            f.write(html_content)
    
    def _export_csv_summary(self, simulation_results: Dict[str, Any], filepath: str):
        """Export simulation results as CSV."""
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Write metadata
            writer.writerow(['Simulation Summary'])
            writer.writerow(['Generated', datetime.now().strftime('%Y-%m-%d %H:%M:%S')])
            writer.writerow([])
            
            # Write metrics
            writer.writerow(['Metric', 'Value'])
            metrics = simulation_results.get('metrics', {})
            for metric_name, metric_value in metrics.items():
                writer.writerow([metric_name, metric_value])
    
    def _create_html_training_progress(self, training_data: List[Dict[str, Any]], 
                                      metrics: List[str], filepath: str):
        """Create HTML training progress visualization."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Training Progress</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .container {{ max-width: 1600px; margin: 0 auto; }}
        .progress {{ margin: 30px 0; }}
        .epoch {{ background: #f8f9fa; padding: 20px; margin: 10px 0; 
                 border-radius: 8px; border-left: 4px solid #28a745; }}
        .metric {{ display: inline-block; margin: 10px; padding: 10px; 
                 background: white; border-radius: 5px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }}
        .metric h4 {{ margin: 0 0 5px 0; color: #495057; }}
        .metric p {{ margin: 0; font-size: 18px; font-weight: bold; color: #28a745; }}
        .improvement-positive {{ color: #28a745; }}
        .improvement-negative {{ color: #dc3545; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Training Progress</h1>
        <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        
        <div class="progress">
            <h2>Training Summary</h2>
            <p>Total epochs: {len(training_data)}</p>
            <p>Metrics tracked: {', '.join(metrics)}</p>
        </div>
        
"""
        
        # Show epoch by epoch progress
        for i, epoch in enumerate(training_data):
            html_content += f"""
        <div class="epoch">
            <h3>Epoch {i + 1}</h3>
            <div class="progress-metrics">
"""
            for metric in metrics:
                if metric in epoch:
                    value = epoch[metric]
                    improvement = epoch.get(f'{metric}_improvement', 0)
                    arrow = "↑" if improvement > 0 else "↓" if improvement < 0 else "="
                    color_class = "improvement-positive" if improvement > 0 else "improvement-negative" if improvement < 0 else ""
                    
                    html_content += f"""
                <div class="metric">
                    <h4>{metric.replace('_', ' ').title()}</h4>
                    <p>{value:.4f} <span class="{color_class}">{arrow}{abs(improvement):.4f}</span></p>
                </div>
"""
            
            html_content += """
            </div>
        </div>
"""
        
        html_content += """
    </div>
</body>
</html>
"""
        
        with open(filepath, 'w') as f:
            f.write(html_content)
    
    def _create_svg_topology(self, topology_data: Dict[str, Any], filepath: str):
        """Create SVG network topology visualization."""
        # Create a simple SVG diagram of the neural network
        svg_content = f"""<?xml version="1.0" encoding="UTF-8"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
    <defs>
        <style type="text/css">
            .node {{ fill: #e3f2fd; stroke: #2196f3; stroke-width: 2; }}
            .node text {{ font-family: Arial; font-size: 14px; }}
            .connection {{ stroke: #757575; stroke-width: 1; stroke-dasharray: 5,5; }}
            .title {{ font-family: Arial; font-size: 18px; font-weight: bold; }}
        </style>
    </defs>
    
    <text x="400" y="30" text-anchor="middle" class="title">Neural Network Topology</text>
    
    <!-- Regions -->
"""
        
        # Draw regions (brain areas)
        regions = topology_data.get('regions', [])
        for i, region in enumerate(regions):
            x = 150 + (i * 150)
            y = 300
            width = 100
            height = 80
            
            svg_content += f"""
    <rect x="{x}" y="{y}" width="{width}" height="{height}" class="node"/>
    <text x="{x + width/2}" y="{y + height/2}" text-anchor="middle" dominant-baseline="middle">
        {region['type'].title()} ({region['neuron_count']} neurons)
    </text>
"""
        
        # Draw connections between regions (simplified)
        if len(regions) > 1:
            for i in range(len(regions) - 1):
                x1 = regions[i]['x'] if 'x' in regions[i] else 150 + (i * 150) + 50
                y1 = 380
                x2 = regions[i + 1]['x'] if 'x' in regions[i + 1] else 150 + ((i + 1) * 150) + 50
                y2 = 380
                
                svg_content += f"""
    <line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" class="connection"/>
"""
        
        svg_content += """
</svg>
"""
        
        with open(filepath, 'w') as f:
            f.write(svg_content)
    
    def _export_to_json(self, brain: Any, filepath: str, include_metadata: bool):
        """Export brain data to JSON."""
        data = {
            'brain_data': {
                'timestamp': datetime.now().isoformat(),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage().value,
            }
        }
        
        if include_metadata:
            data['metadata'] = {
                'exported_at': datetime.now().isoformat(),
                'version': '1.0',
                'format': 'json'
            }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
    
    def _export_to_csv(self, brain: Any, filepath: str, include_metadata: bool):
        """Export brain data to CSV."""
        data = {
            'metric': [
                'total_neurons',
                'total_synapses', 
                'firing_neurons',
                'total_spikes',
                'average_firing_rate',
                'development_stage'
            ],
            'value': [
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                brain.getFiringNeuronCount(),
                brain.getTotalSpikeCount(),
                brain.getAverageFiringRate(),
                brain.getDevelopmentalStage().value
            ]
        }
        
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['Metric', 'Value'])
            for metric, value in zip(data['metric'], data['value']):
                writer.writerow([metric, value])
            
            if include_metadata:
                writer.writerow([])
                writer.writerow(['Exported at', datetime.now().isoformat()])
                writer.writerow(['Version', '1.0'])
    
    def _export_to_pickle(self, brain: Any, filepath: str, include_metadata: bool):
        """Export brain data to pickle format."""
        import pickle
        
        data = {
            'brain_data': {
                'timestamp': datetime.now().isoformat(),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage().value,
            }
        }
        
        if include_metadata:
            data['metadata'] = {
                'exported_at': datetime.now().isoformat(),
                'version': '1.0',
                'format': 'pickle'
            }
        
        with open(filepath, 'wb') as f:
            pickle.dump(data, f)
    
    def _export_to_binary(self, brain: Any, filepath: str, include_metadata: bool):
        """Export brain data to binary format."""
        with open(filepath, 'wb') as f:
            # Write magic number for identification
            f.write(b'NLM_BRAIN_DATA')
            
            # Write timestamp
            timestamp = datetime.now().isoformat()
            f.write(timestamp.encode('utf-8').ljust(50))
            
            # Write data as binary floats/integers
            data = [
                float(brain.getTotalNeuronCount()),
                float(brain.getTotalSynapseCount()),
                float(brain.getFiringNeuronCount()),
                float(brain.getTotalSpikeCount()),
                float(brain.getAverageFiringRate()),
                float(brain.getDevelopmentalStage().value)
            ]
            
            for value in data:
                f.write(struct.pack('d', value))  # Double precision
            
            if include_metadata:
                # Write metadata as binary
                metadata = {
                    'version': '1.0',
                    'format': 'binary'
                }
                metadata_str = json.dumps(metadata)
                f.write(metadata_str.encode('utf-8').ljust(100))

class VisualizationManager:
    """
    High-level visualization manager for NLM.
    """
    
    def __init__(self):
        self.visualizers = {}
    
    def add_visualizer(self, name: str, visualizer: NLMVisualizer):
        """Add a visualizer."""
        self.visualizers[name] = visualizer
    
    def get_visualizer(self, name: str) -> NLMVisualizer:
        """Get a visualizer by name."""
        return self.visualizers.get(name)
    
    def create_comprehensive_visualization(self, brain: Any, simulation_data: List[Dict[str, Any]], 
                                          output_dir: str = "comprehensive_exports") -> Dict[str, str]:
        """
        Create a comprehensive set of visualizations.
        
        Args:
            brain: NLM brain object
            simulation_data: Simulation data
            output_dir: Output directory
            
        Returns:
            Dictionary mapping visualization types to file paths
        """
        # Create default visualizer
        visualizer = NLMVisualizer(output_dir)
        
        # Create multiple visualization formats
        results = {}
        
        # HTML visualization
        if 'html' not in self.visualizers:
            visualizer.add_format(VisualizationFormat.HTML)
            results['html'] = visualizer.create_neural_activity_plot(brain, len(simulation_data))
        
        # JSON visualization
        visualizer.add_format(VisualizationFormat.JSON)
        results['json'] = visualizer.create_simulation_summary(simulation_data)
        
        # CSV visualization
        visualizer.add_format(VisualizationFormat.CSV)
        results['csv'] = visualizer.create_simulation_summary(simulation_data)
        
        # Network topology
        results['topology'] = visualizer.create_network_topology(brain)
        
        return results

# Global visualization manager instance
_global_visualizer = VisualizationManager()

def get_visualizer_manager() -> VisualizationManager:
    """Get the global visualization manager."""
    return _global_visualizer

def create_visualizer(output_dir: str = "exports") -> NLMVisualizer:
    """Create a new visualizer."""
    return NLMVisualizer(output_dir)

if __name__ == "__main__":
    # Example usage
    print("NLM Visualization and Export Tools")
    print("=" * 50)
    
    # Create visualizer
    visualizer = NLMVisualizer("demo_exports")
    
    # Create sample brain data (simulated)
    class MockBrain:
        def getTotalNeuronCount(self): return 1000
        def getTotalSynapseCount(self): return 15000
        def getFiringNeuronCount(self): return 125
        def getTotalSpikeCount(self): return 50000
        def getAverageFiringRate(self): return 50.0
        def getDevelopmentalStage(self):
            class MockStage:
                value = 3
            return MockStage()
    
    mock_brain = MockBrain()
    
    # Create sample simulation data
    simulation_data = []
    for step in range(100):
        simulation_data.append({
            'step': step,
            'reward': step * 0.1,
            'firing_rate': 50 + (step % 20) * 2,
            'development_stage': step % 6
        })
    
    # Create visualizations
    print("Creating neural activity visualization...")
    html_path = visualizer.create_neural_activity_plot(mock_brain, 100)
    print(f"HTML visualization saved to: {html_path}")
    
    print("Creating simulation summary...")
    json_path = visualizer.create_simulation_summary(simulation_data)
    print(f"JSON summary saved to: {json_path}")
    
    print("Creating network topology...")
    topo_path = visualizer.create_network_topology(mock_brain)
    print(f"Topology saved to: {topo_path}")
    
    print("All visualizations created successfully!")
