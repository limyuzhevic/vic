"""
Visualization interface for NLM Python bindings

This module provides visualization and analysis interface for neural network simulations.
"""

import json
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Dict, Any, List, Optional, Tuple
import plotly.graph_objects as go
import plotly.io as pio
import pandas as pd
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.manifold import TSNE
import umap

class VisualizationPipeline:
    """Complete visualization pipeline for neural network analysis."""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.setup_visualization_tools()
    
    def setup_visualization_tools(self):
        """Setup visualization tools and dependencies."""
        # Configure matplotlib
        plt.style.use('seaborn-v0_8')
        plt.rcParams['figure.figsize'] = (12, 8)
        plt.rcParams['font.size'] = 10
        
        # Configure seaborn
        sns.set_style('whitegrid')
        
        # Configure plotly
        pio.templates.default = 'plotly_dark' if self.config.get('dark_mode', False) else 'plotly_white'
    
    def create_comprehensive_dashboard(
        self,
        brain_data: Dict[str, Any],
        output_dir: str = "visualization_output",
        generate_reports: bool = True
    ) -> Dict[str, str]:
        """
        Create comprehensive visualization dashboard.
        
        Args:
            brain_data: Brain data dictionary
            output_dir: Output directory
            generate_reports: Generate analysis reports
        
        Returns:
            Dictionary of generated file paths
        """
        import os
        os.makedirs(output_dir, exist_ok=True)
        
        # Generate individual visualizations
        visualizations = self._create_all_visualizations(brain_data, output_dir)
        
        # Generate interactive dashboard
        dashboard_file = self._create_interactive_dashboard(brain_data, output_dir)
        
        # Generate analysis reports
        if generate_reports:
            reports = self._generate_analysis_reports(brain_data, output_dir)
        else:
            reports = {}
        
        # Combine all generated files
        generated_files = {**visualizations, **reports}
        generated_files['dashboard'] = dashboard_file
        
        print(f"Generated {len(generated_files)} visualization files in: {output_dir}")
        return generated_files
    
    def _create_all_visualizations(self, brain_data: Dict[str, Any], output_dir: str) -> Dict[str, str]:
        """Create all visualization types."""
        generated_files = {}
        
        # 1. Neural network structure visualization
        structure_file = self._create_neural_structure_viz(brain_data, output_dir)
        generated_files['structure'] = structure_file
        
        # 2. Spike activity visualization
        spike_file = self._create_spike_activity_viz(brain_data, output_dir)
        generated_files['spike_activity'] = spike_file
        
        # 3. Network topology visualization
        topology_file = self._create_network_topology_viz(brain_data, output_dir)
        generated_files['topology'] = topology_file
        
        # 4. Weight distribution visualization
        weight_file = self._create_weight_distribution_viz(brain_data, output_dir)
        generated_files['weight_distribution'] = weight_file
        
        # 5. Activity correlation visualization
        correlation_file = self._create_activity_correlation_viz(brain_data, output_dir)
        generated_files['activity_correlation'] = correlation_file
        
        # 6. Dimensionality reduction visualization
        dimred_file = self._create_dimensionality_reduction_viz(brain_data, output_dir)
        generated_files['dimensionality_reduction'] = dimred_file
        
        # 7. Plasticity dynamics visualization
        plasticity_file = self._create_plasticity_dynamics_viz(brain_data, output_dir)
        generated_files['plasticity'] = plasticity_file
        
        # 8. Performance time series visualization
        performance_file = self._create_performance_time_series_viz(brain_data, output_dir)
        generated_files['performance'] = performance_file
        
        return generated_files
    
    def _create_neural_structure_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create neural structure visualization."""
        fig = go.Figure()
        
        # Extract neural data
        neurons = brain_data.get('neurons', [])
        synapses = brain_data.get('synapses', [])
        
        if not neurons:
            # Create placeholder if no neurons
            fig.add_trace(go.Scatter(x=[], y=[], mode='markers', name='Neurons'))
            fig.update_layout(title='Neural Structure Visualization', width=800, height=600)
        else:
            # Create neuron positions
            x = [neuron.get('x', 0) for neuron in neurons]
            y = [neuron.get('y', 0) for neuron in neurons]
            sizes = [neuron.get('radius', 10) * 2 for neuron in neurons]
            colors = self._get_neuron_colors(neurons)
            
            # Add neurons
            fig.add_trace(go.Scatter(
                x=x,
                y=y,
                mode='markers',
                marker=dict(size=sizes, color=colors, opacity=0.8, 
                           colorbar=dict(title='Neuron Type'),
                           colorscale='Viridis'),
                text=[f'Neuron {i}' for i in range(len(x))],
                hovertemplate='<b>Neuron %{text}</b><br>X: %{x:.2f}<br>Y: %{y:.2f}<extra></extra>',
                name='Neurons'
            ))
            
            # Add synapses if available
            if synapses and len(synapses) > 0:
                # For large networks, sample synapses
                sample_size = min(1000, len(synapses))
                if len(synapses) > sample_size:
                    indices = np.random.choice(len(synapses), sample_size, replace=False)
                    synapses = [synapses[i] for i in indices]
                
                syn_x = []
                syn_y = []
                syn_w = []
                
                for synapse in synapses:
                    source_neuron = next((n for n in neurons if n.get('id') == synapse.get('source_neuron_id')), None)
                    target_neuron = next((n for n in neurons if n.get('id') == synapse.get('target_neuron_id')), None)
                    
                    if source_neuron and target_neuron:
                        syn_x.extend([source_neuron.get('x', 0), target_neuron.get('x', 0), None])
                        syn_y.extend([source_neuron.get('y', 0), target_neuron.get('y', 0), None])
                        syn_w.extend([0, synapse.get('weight', 0), None])
                
                if syn_x:
                    fig.add_trace(go.Scatter(
                        x=syn_x,
                        y=syn_y,
                        mode='lines',
                        line=dict(width=1, color=syn_w, colorscale='RdBu', colorbar=dict(title='Weight')),
                        hovertemplate='<b>Synapse</b><br>Weight: %{line.color:.3f}<extra></extra>',
                        name='Synapses'
                    ))
        
        # Update layout
        fig.update_layout(
            title='Neural Network Structure',
            xaxis=dict(title='X Position', showgrid=True, zeroline=False),
            yaxis=dict(title='Y Position', showgrid=True, zeroline=False),
            hoverlabel=dict(bgcolor='white', font_size=12),
            width=800,
            height=600,
            showlegend=True
        )
        
        # Save figure
        output_file = f"{output_dir}/neural_structure.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_spike_activity_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create spike activity visualization."""
        fig = go.Figure()
        
        # Extract spike data
        spike_times = brain_data.get('spike_times', [])
        spike_neurons = brain_data.get('spike_neurons', [])
        
        if not spike_times:
            fig.add_trace(go.Scatter(x=[], y=[], mode='markers', name='Spikes'))
            fig.update_layout(title='Spike Activity', width=800, height=600)
        else:
            # Create spike raster plot
            fig.add_trace(go.Scatter(
                x=spike_times,
                y=spike_neurons,
                mode='markers',
                marker=dict(size=4, color='red', opacity=0.7),
                text=[f'Neuron {neuron} at {time:.2f}s' for neuron, time in zip(spike_neurons, spike_times)],
                hovertemplate='<b>%{text}</b><extra></extra>',
                name='Spikes'
            ))
            
            # Add time line
            max_time = max(spike_times) if spike_times else 10.0
            fig.add_trace(go.Scatter(
                x=[0, max_time],
                y=[0, 0],
                mode='lines',
                line=dict(color='black', width=2),
                showlegend=False,
                hovertemplate='<extra></extra>'
            ))
        
        # Update layout
        fig.update_layout(
            title='Spike Activity Visualization',
            xaxis=dict(title='Time (s)', showgrid=True),
            yaxis=dict(title='Neuron ID', autorange='reversed'),
            width=800,
            height=600,
            showlegend=False
        )
        
        # Save figure
        output_file = f"{output_dir}/spike_activity.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_network_topology_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create network topology visualization."""
        fig = go.Figure()
        
        # Create network metrics from brain data
        metrics = self._calculate_network_metrics(brain_data)
        
        # Create subplots
        fig = go.Figure()
        
        # Add network statistics
        if metrics:
            # Create bar chart for network properties
            properties = list(metrics.keys())
            values = list(metrics.values())
            
            fig.add_trace(go.Bar(
                x=properties,
                y=values,
                marker_color='steelblue',
                opacity=0.8,
                name='Network Properties'
            ))
        
        # Update layout
        fig.update_layout(
            title='Network Topology Analysis',
            xaxis=dict(title='Property'),
            yaxis=dict(title='Value'),
            width=800,
            height=600
        )
        
        # Save figure
        output_file = f"{output_dir}/network_topology.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_weight_distribution_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create weight distribution visualization."""
        fig = go.Figure()
        
        # Extract synapse data
        synapses = brain_data.get('synapses', [])
        
        if synapses:
            weights = [synapse.get('weight', 0) for synapse in synapses]
            
            # Create histogram
            fig.add_trace(go.Histogram(
                x=weights,
                nbinsx=30,
                marker_color='steelblue',
                opacity=0.7,
                name='Weight Distribution'
            ))
            
            # Add normal distribution overlay
            import scipy.stats as stats
            x_axis = np.linspace(min(weights), max(weights), 100)
            pdf = stats.norm.pdf(x_axis, np.mean(weights), np.std(weights))
            
            fig.add_trace(go.Scatter(
                x=x_axis,
                y=pdf,
                mode='lines',
                line=dict(color='red', width=2),
                name='Normal Distribution'
            ))
        
        # Update layout
        fig.update_layout(
            title='Synapse Weight Distribution',
            xaxis=dict(title='Weight'),
            yaxis=dict(title='Frequency'),
            width=800,
            height=600,
            showlegend=True
        )
        
        # Save figure
        output_file = f"{output_dir}/weight_distribution.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_activity_correlation_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create activity correlation visualization."""
        fig = go.Figure()
        
        # Extract activity data
        step_data = brain_data.get('step_data', [])
        
        if step_data:
            # Convert to pandas DataFrame
            df = pd.DataFrame(step_data)
            
            # Calculate correlations between different metrics
            if 'spikes' in df.columns and 'firing_rate' in df.columns:
                correlation = df['spikes'].corr(df['firing_rate'])
                
                fig.add_trace(go.Indicator(
                    mode='number+delta',
                    value=correlation,
                    title='Spike-Activity Correlation',
                    delta={'reference': 0}
                ))
        
        # Update layout
        fig.update_layout(
            title='Activity Correlation Analysis',
            width=400,
            height=300
        )
        
        # Save figure
        output_file = f"{output_dir}/activity_correlation.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_dimensionality_reduction_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create dimensionality reduction visualization."""
        fig = go.Figure()
        
        # Extract neuron data
        neurons = brain_data.get('neurons', [])
        
        if neurons and len(neurons) >= 2:
            # Prepare data for dimensionality reduction
            features = []
            for neuron in neurons:
                feature_vector = [
                    neuron.get('x', 0),
                    neuron.get('y', 0),
                    neuron.get('radius', 10),
                    neuron.get('firing_rate', 0),
                    neuron.get('adaptation_level', 0)
                ]
                features.append(feature_vector)
            
            X = np.array(features)
            
            # Try UMAP first (better for visualization)
            try:
                reducer = umap.UMAP(n_components=2, random_state=42)
                X_reduced = reducer.fit_transform(X)
                method = 'UMAP'
            except:
                # Fall back to PCA
                reducer = PCA(n_components=2)
                X_reduced = reducer.fit_transform(X)
                method = 'PCA'
            
            # Create scatter plot
            fig.add_trace(go.Scatter(
                x=X_reduced[:, 0],
                y=X_reduced[:, 1] if X_reduced.shape[1] > 1 else np.zeros(X_reduced.shape[0]),
                mode='markers',
                marker=dict(
                    size=8,
                    color=self._get_neuron_colors(neurons),
                    opacity=0.7,
                    colorbar=dict(title='Neuron Type')
                ),
                text=[f'Neuron {i}' for i in range(len(X))],
                hovertemplate='<b>Neuron %{text}</b><br>PC1: %{x:.2f}<br>PC2: %{y:.2f}<extra></extra>',
                name=f'Dimension Reduced ({method})'
            ))
            
            # Add cluster labels if requested
            if self.config.get('show_clusters', False) and len(neurons) >= 10:
                kmeans = KMeans(n_clusters=min(5, len(neurons)), random_state=42)
                cluster_labels = kmeans.fit_predict(X)
                
                fig.add_trace(go.Scatter(
                    x=X_reduced[:, 0],
                    y=X_reduced[:, 1] if X_reduced.shape[1] > 1 else np.zeros(X_reduced.shape[0]),
                    mode='markers',
                    marker=dict(
                        size=8,
                        color=cluster_labels,
                        colorscale='Viridis',
                        showscale=False,
                        opacity=0.8
                    ),
                    text=[f'Neuron {i} (Cluster {label})' for i, label in enumerate(cluster_labels)],
                    hovertemplate='<b>Neuron %{text}</b><br>PC1: %{x:.2f}<br>PC2: %{y:.2f}<extra></extra>',
                    name='Clusters'
                ))
        
        # Update layout
        fig.update_layout(
            title='Dimensionality Reduction Visualization',
            xaxis=dict(title='Component 1'),
            yaxis=dict(title='Component 2'),
            width=800,
            height=600
        )
        
        # Save figure
        output_file = f"{output_dir}/dimensionality_reduction.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_plasticity_dynamics_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create plasticity dynamics visualization."""
        fig = go.Figure()
        
        # Extract plasticity data
        plasticity_events = brain_data.get('plasticity_events', [])
        step_data = brain_data.get('step_data', [])
        
        if plasticity_events:
            # Create timeline of plasticity events
            events_df = pd.DataFrame(plasticity_events)
            
            fig.add_trace(go.Scatter(
                x=events_df['step'],
                y=events_df['magnitude'],
                mode='markers',
                marker=dict(size=8, color='red', opacity=0.7),
                text=[f"Event at step {step}: {mag:.4f}" for step, mag in zip(events_df['step'], events_df['magnitude'])],
                hovertemplate='<b>%{text}</b><extra></extra>',
                name='Plasticity Events'
            ))
        
        # Add weight change over time if step data available
        if step_data:
            step_df = pd.DataFrame(step_data)
            if 'weight_change' in step_df.columns:
                fig.add_trace(go.Scatter(
                    x=step_df['step'],
                    y=step_df['weight_change'],
                    mode='lines',
                    line=dict(color='blue', width=2),
                    name='Weight Change'
                ))
        
        # Update layout
        fig.update_layout(
            title='Plasticity Dynamics',
            xaxis=dict(title='Step'),
            yaxis=dict(title='Magnitude'),
            width=800,
            height=600,
            showlegend=True
        )
        
        # Save figure
        output_file = f"{output_dir}/plasticity_dynamics.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_performance_time_series_viz(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create performance time series visualization."""
        fig = go.Figure()
        
        # Extract performance data
        step_data = brain_data.get('step_data', [])
        
        if step_data:
            step_df = pd.DataFrame(step_data)
            
            # Create multiple subplots
            metrics = []
            for col in ['spikes', 'firing_rate', 'weight_change']:
                if col in step_df.columns:
                    metrics.append(col)
            
            for i, metric in enumerate(metrics):
                fig.add_trace(go.Scatter(
                    x=step_df['step'],
                    y=step_df[metric],
                    mode='lines',
                    line=dict(width=2),
                    name=metric.replace('_', ' ').title()
                ))
        
        # Update layout
        fig.update_layout(
            title='Performance Time Series',
            xaxis=dict(title='Step'),
            yaxis=dict(title='Value'),
            width=800,
            height=600,
            showlegend=True
        )
        
        # Save figure
        output_file = f"{output_dir}/performance_time_series.html"
        fig.write_html(output_file)
        
        return output_file
    
    def _create_interactive_dashboard(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Create interactive dashboard."""
        fig = go.Figure()
        
        # Create dashboard layout
        dashboard_html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Neural Network Visualization Dashboard</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .dashboard {{ display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; }}
        .plot {{ border: 1px solid #ddd; padding: 10px; background: white; }}
        .header {{ background: #f0f0f0; padding: 20px; border-radius: 5px; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>Neural Network Visualization Dashboard</h1>
        <p>Interactive visualization of neural network simulation results</p>
    </div>
    
    <div class="dashboard">
        <div class="plot" id="neural-structure"></div>
        <div class="plot" id="spike-activity"></div>
        <div class="plot" id="network-topology"></div>
        <div class="plot" id="weight-distribution"></div>
    </div>
    
    <script>
        // Load and display neural network structure
        fetch('neural_structure.html')
            .then(response => response.text())
            .then(html => {{
                document.getElementById('neural-structure').innerHTML = html;
            }});
        
        // Load and display spike activity
        fetch('spike_activity.html')
            .then(response => response.text())
            .then(html => {{
                document.getElementById('spike-activity').innerHTML = html;
            }});
        
        // Load and display network topology
        fetch('network_topology.html')
            .then(response => response.text())
            .then(html => {{
                document.getElementById('network-topology').innerHTML = html;
            }});
        
        // Load and display weight distribution
        fetch('weight_distribution.html')
            .then(response => response.text())
            .then(html => {{
                document.getElementById('weight-distribution').innerHTML = html;
            }});
    </script>
</body>
</html>
        """
        
        # Save dashboard
        output_file = f"{output_dir}/interactive_dashboard.html"
        with open(output_file, 'w') as f:
            f.write(dashboard_html)
        
        return output_file
    
    def _generate_analysis_reports(self, brain_data: Dict[str, Any], output_dir: str) -> Dict[str, str]:
        """Generate analysis reports."""
        reports = {}
        
        # Generate summary report
        summary_file = self._generate_summary_report(brain_data, output_dir)
        reports['summary'] = summary_file
        
        # Generate detailed report
        detailed_file = self._generate_detailed_report(brain_data, output_dir)
        reports['detailed'] = detailed_file
        
        # Generate statistical report
        stats_file = self._generate_statistical_report(brain_data, output_dir)
        reports['statistical'] = stats_file
        
        return reports
    
    def _calculate_network_metrics(self, brain_data: Dict[str, Any]) -> Dict[str, float]:
        """Calculate network metrics from brain data."""
        metrics = {}
        
        # Extract neural data
        neurons = brain_data.get('neurons', [])
        synapses = brain_data.get('synapses', [])
        step_data = brain_data.get('step_data', [])
        
        if neurons:
            # Calculate basic metrics
            metrics['neuron_count'] = len(neurons)
            metrics['synapse_count'] = len(synapses) if synapses else 0
            
            # Calculate connectivity density
            if len(neurons) > 1:
                potential_connections = len(neurons) * (len(neurons) - 1) / 2
                actual_connections = len(synapses) if synapses else 0
                metrics['connectivity_density'] = actual_connections / potential_connections if potential_connections > 0 else 0.0
            
            # Calculate average properties
            if step_data:
                step_df = pd.DataFrame(step_data)
                if 'firing_rate' in step_df.columns:
                    metrics['average_firing_rate'] = float(step_df['firing_rate'].mean())
                
                if 'spikes' in step_df.columns:
                    metrics['average_spikes_per_step'] = float(step_df['spikes'].mean())
        
        return metrics
    
    def _get_neuron_colors(self, neurons: List[Dict[str, Any]]) -> List[str]:
        """Get colors for neurons based on type."""
        colors = []
        for neuron in neurons:
            neuron_type = neuron.get('type', 'internal')
            
            # Map neuron type to color
            color_map = {
                'excitatory': '#ff6b6b',  # Red
                'inhibitory': '#4ecdc4',  # Teal
                'sensory': '#45b7d1',  # Blue
                'motor': '#f9ca24',  # Yellow
                'internal': '#96ceb4',  # Green
                'modulatory': '#dda0dd'  # Purple
            }
            
            colors.append(color_map.get(neuron_type, '#gray'))
        
        return colors
    
    def _generate_summary_report(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Generate summary report."""
        summary = {
            'simulation_summary': self._get_simulation_summary(brain_data),
            'network_properties': self._get_network_properties(brain_data),
            'performance_metrics': self._get_performance_metrics(brain_data),
            'key_findings': self._get_key_findings(brain_data)
        }
        
        # Save summary report
        output_file = f"{output_dir}/summary_report.json"
        with open(output_file, 'w') as f:
            json.dump(summary, f, indent=2)
        
        return output_file
    
    def _generate_detailed_report(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Generate detailed report."""
        detailed = {
            'raw_data': brain_data,
            'analysis': self._perform_detailed_analysis(brain_data),
            'visualizations': self._get_generated_visualizations(),
            'recommendations': self._get_recommendations(brain_data)
        }
        
        # Save detailed report
        output_file = f"{output_dir}/detailed_report.json"
        with open(output_file, 'w') as f:
            json.dump(detailed, f, indent=2)
        
        return output_file
    
    def _generate_statistical_report(self, brain_data: Dict[str, Any], output_dir: str) -> str:
        """Generate statistical report."""
        stats = {
            'statistical_analysis': self._perform_statistical_analysis(brain_data),
            'hypothesis_tests': self._perform_hypothesis_tests(brain_data),
            'confidence_intervals': self._calculate_confidence_intervals(brain_data),
            'significance_tests': self._perform_significance_tests(brain_data)
        }
        
        # Save statistical report
        output_file = f"{output_dir}/statistical_report.json"
        with open(output_file, 'w') as f:
            json.dump(stats, f, indent=2)
        
        return output_file
    
    def _get_simulation_summary(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Get simulation summary."""
        return {
            'simulation_duration': brain_data.get('duration', 10.0),
            'time_step': brain_data.get('timestep', 0.001),
            'total_spikes': brain_data.get('total_spikes', 0),
            'total_neurons': brain_data.get('total_neurons', 0),
            'total_synapses': brain_data.get('total_synapses', 0)
        }
    
    def _get_network_properties(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Get network properties."""
        return {
            'connectivity': brain_data.get('connectivity', 0.15),
            'brain_type': brain_data.get('brain_type', 'standard'),
            'plasticity_enabled': brain_data.get('plasticity_enabled', True),
            'development_enabled': brain_data.get('development_enabled', False)
        }
    
    def _get_performance_metrics(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Get performance metrics."""
        return {
            'firing_rate': brain_data.get('average_firing_rate', 0.0),
            'weight_change': brain_data.get('weight_change_magnitude', 0.0),
            'memory_usage': brain_data.get('memory_footprint', 0)
        }
    
    def _get_key_findings(self, brain_data: Dict[str, Any]) -> List[str]:
        """Get key findings from analysis."""
        findings = []
        
        # Analyze key metrics
        firing_rate = brain_data.get('average_firing_rate', 0.0)
        if firing_rate > 1.0:
            findings.append(f"High firing rate detected: {firing_rate:.2f} Hz")
        
        weight_change = brain_data.get('weight_change_magnitude', 0.0)
        if weight_change > 0.1:
            findings.append(f"Significant weight changes: {weight_change:.3f}")
        
        if firing_rate < 0.1:
            findings.append("Low firing rate observed - network may be quiescent")
        
        if weight_change < 0.01:
            findings.append("Minimal weight changes - plasticity may be ineffective")
        
        return findings
    
    def _perform_detailed_analysis(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform detailed analysis."""
        analysis = {}
        
        # Analyze spike statistics
        spike_times = brain_data.get('spike_times', [])
        if spike_times:
            spike_array = np.array(spike_times)
            analysis['spike_statistics'] = {
                'mean_interval': float(np.mean(np.diff(spike_array))) if len(spike_times) > 1 else 0.0,
                'cv_interval': float(np.std(np.diff(spike_array)) / np.mean(np.diff(spike_array))) if len(spike_times) > 1 else 0.0,
                'firing_rate': len(spike_times) / brain_data.get('duration', 10.0)
            }
        
        # Analyze neural activity
        neurons = brain_data.get('neurons', [])
        if neurons:
            analysis['neural_activity'] = {
                'activity_variance': float(np.var([n.get('firing_rate', 0) for n in neurons])),
                'max_firing_rate': float(max([n.get('firing_rate', 0) for n in neurons])),
                'min_firing_rate': float(min([n.get('firing_rate', 0) for n in neurons]))
            }
        
        return analysis
    
    def _perform_statistical_analysis(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform statistical analysis."""
        stats = {}
        
        # Simple statistical tests
        step_data = brain_data.get('step_data', [])
        if step_data:
            step_df = pd.DataFrame(step_data)
            
            if 'spikes' in step_df.columns:
                stats['spike_statistics'] = {
                    'mean': float(step_df['spikes'].mean()),
                    'std': float(step_df['spikes'].std()),
                    'skewness': float(step_df['spikes'].skew()),
                    'kurtosis': float(step_df['spikes'].kurtosis())
                }
        
        return stats
    
    def _perform_hypothesis_tests(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform hypothesis tests."""
        return {}
    
    def _calculate_confidence_intervals(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate confidence intervals."""
        return {}
    
    def _perform_significance_tests(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform significance tests."""
        return {}
    
    def _get_generated_visualizations(self) -> List[str]:
        """Get list of generated visualizations."""
        return [
            'neural_structure.html',
            'spike_activity.html',
            'network_topology.html',
            'weight_distribution.html',
            'activity_correlation.html',
            'dimensionality_reduction.html',
            'plasticity_dynamics.html',
            'performance_time_series.html'
        ]
    
    def _get_recommendations(self, brain_data: Dict[str, Any]) -> List[str]:
        """Get recommendations based on analysis."""
        recommendations = []
        
        # Analyze results and provide recommendations
        firing_rate = brain_data.get('average_firing_rate', 0.0)
        weight_change = brain_data.get('weight_change_magnitude', 0.0)
        
        if firing_rate < 0.1:
            recommendations.append("Consider increasing excitation or decreasing inhibition in the network")
        
        if weight_change < 0.01:
            recommendations.append("Review plasticity parameters - current settings may not be effective")
        
        if firing_rate > 10.0:
            recommendations.append("Network is highly active - consider implementing homeostatic mechanisms")
        
        recommendations.append("Continue monitoring network dynamics for long-term stability")
        
        return recommendations

# High-level interface functions
def visualize_neural_network(
    brain_data: Dict[str, Any],
    filename: str,
    config: Optional[Dict[str, Any]] = None
) -> Dict[str, str]:
    """
    Create comprehensive neural network visualization.
    
    Args:
        brain_data: Brain data dictionary
        filename: Base filename for output files
        config: Visualization configuration
    
    Returns:
        Dictionary of generated file paths
    """
    visualizer = VisualizationPipeline(config)
    return visualizer.create_comprehensive_dashboard(brain_data, filename)

def create_interactive_dashboard(
    brain_data: Dict[str, Any],
    filename: str,
    config: Optional[Dict[str, Any]] = None
) -> str:
    """
    Create interactive dashboard.
    
    Args:
        brain_data: Brain data dictionary
        filename: Output filename
        config: Visualization configuration
    
    Returns:
        Generated filename
    """
    visualizer = VisualizationPipeline(config)
    return visualizer.create_interactive_dashboard(brain_data, filename)

# Print module information
print("NLM Visualization Interface v0.1.0")
print("Features:")
print("  - Comprehensive visualization pipeline")
print("  - Multiple visualization types")
print("  - Interactive dashboards")
print("  - Statistical analysis")
print("  - Color schemes and themes")
print("  - Data export capabilities")
print("  - Responsive web interfaces")
print()
