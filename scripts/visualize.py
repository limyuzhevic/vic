#!/usr/bin/env python3
"""
Visualization Script

Creates basic brain visualizations, neuron connectivity plots,
and time series visualizations for NLM simulations.
Supports multiple output formats (PNG, SVG, PDF).

Usage: python scripts/visualize.py [input_file] [options]
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from pathlib import Path
import argparse
import json
import sys
from typing import Dict, List, Any, Optional
from datetime import datetime
import networkx as nx

class BrainVisualizer:
    """Creates visualizations for brain simulation data."""
    
    def __init__(self, style: str = "default"):
        """Initialize visualizer.
        
        Args:
            style: Matplotlib style to use
        """
        self.style = style
        plt.style.use(style)
        self.figures = {}
        
    def create_neuron_connectivity(self, neurons: List[Dict], connections: List[Dict], 
                                  output_path: str = None, format: str = "png") -> str:
        """Create neuron connectivity plot.
        
        Args:
            neurons: List of neuron data dictionaries
            connections: List of connection data dictionaries
            output_path: Output file path
            format: Output format (png, svg, pdf)
            
        Returns:
            Path to saved visualization
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"neuron_connectivity_{timestamp}.{format}"
        
        fig, axes = plt.subplots(1, 2, figsize=(15, 7))
        
        # Extract neuron positions and types
        neuron_positions = {}
        neuron_types = []
        
        for i, neuron in enumerate(neurons):
            x = neuron.get('x', np.random.uniform(0, 10))
            y = neuron.get('y', np.random.uniform(0, 10))
            neuron_positions[i] = (x, y)
            neuron_types.append(neuron.get('type', 'regular'))
        
        # Create network graph
        G = nx.Graph()
        
        # Add nodes
        for i, pos in neuron_positions.items():
            node_type = neuron_types[i]
            if node_type == 'excitatory':
                node_color = 'red'
            elif node_type == 'inhibitory':
                node_color = 'blue'
            elif node_type == 'interneuron':
                node_color = 'green'
            else:
                node_color = 'gray'
            
            G.add_node(i, pos=pos, color=node_color, type=node_type)
        
        # Add edges
        for conn in connections:
            source = conn.get('source', -1)
            target = conn.get('target', -1)
            if source in neuron_positions and target in neuron_positions:
                weight = conn.get('weight', 1.0)
                G.add_edge(source, target, weight=weight)
        
        # Draw network
        pos = nx.get_node_attributes(G, 'pos')
        node_colors = [G.nodes[node]['color'] for node in G.nodes()]
        
        # Draw regular network
        axes[0].set_title("Neuron Connectivity Graph")
        nx.draw_networkx_nodes(G, pos, node_color=node_colors, node_size=100, ax=axes[0])
        nx.draw_networkx_edges(G, pos, alpha=0.5, ax=axes[0])
        nx.draw_networkx_labels(G, pos, font_size=8, ax=axes[0])
        axes[0].set_aspect('equal')
        
        # Draw zoomed-in view of high-density areas
        axes[1].set_title("High-Density Region")
        
        # Find regions with high neuron density
        x_coords = [pos[node][0] for node in G.nodes()]
        y_coords = [pos[node][1] for node in G.nodes()]
        
        # Simple clustering based on proximity
        from sklearn.cluster import DBSCAN
        coords = np.column_stack([x_coords, y_coords])
        clustering = DBSCAN(eps=1.0, min_samples=5).fit(coords)
        
        # Highlight clusters
        unique_clusters = set(clustering.labels_)
        for cluster_id in unique_clusters:
            if cluster_id >= 0:  # Not noise
                cluster_points = coords[clustering.labels_ == cluster_id]
                axes[1].scatter(cluster_points[:, 0], cluster_points[:, 1], 
                              s=50, alpha=0.7, label=f"Cluster {cluster_id}")
        
        # Draw all neurons for context
        axes[1].scatter(x_coords, y_coords, c='gray', alpha=0.3, s=10)
        
        axes[1].set_aspect('equal')
        axes[1].legend()
        
        plt.tight_layout()
        
        # Save figure
        plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    def create_time_series(self, time_data: Dict[str, Any], output_path: str = None, 
                          format: str = "png") -> str:
        """Create time series visualization.
        
        Args:
            time_data: Dictionary with time series data
            output_path: Output file path
            format: Output format (png, svg, pdf)
            
        Returns:
            Path to saved visualization
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"time_series_{timestamp}.{format}"
        
        # Create subplots
        fig, axes = plt.subplots(3, 2, figsize=(15, 10))
        fig.suptitle('NLM Simulation Time Series', fontsize=16)
        
        time_points = time_data.get('time', [])
        
        # Plot membrane potentials
        if 'membrane_potentials' in time_data:
            ax = axes[0, 0]
            for neuron_id, potentials in time_data['membrane_potentials'].items():
                ax.plot(time_points, potentials, alpha=0.7, label=f'Neuron {neuron_id}')
            ax.set_xlabel('Time (ms)')
            ax.set_ylabel('Membrane Potential (mV)')
            ax.set_title('Membrane Potential Dynamics')
            ax.legend(loc='upper right', fontsize='small')
            ax.grid(True, alpha=0.3)
        
        # Plot firing rates
        if 'firing_rates' in time_data:
            ax = axes[0, 1]
            for neuron_id, rates in time_data['firing_rates'].items():
                ax.plot(time_points, rates, alpha=0.7, label=f'Neuron {neuron_id}')
            ax.set_xlabel('Time (ms)')
            ax.set_ylabel('Firing Rate (Hz)')
            ax.set_title('Firing Rate Dynamics')
            ax.legend(loc='upper right', fontsize='small')
            ax.grid(True, alpha=0.3)
        
        # Plot synaptic weights
        if 'synaptic_weights' in time_data:
            ax = axes[1, 0]
            weights = time_data['synaptic_weights']
            
            # Create heatmap of weight evolution
            time_steps = len(weights)
            if time_steps > 0:
                # Convert to numpy array for better visualization
                weights_array = np.array(weights)
                
                # Plot weight changes for first 100 connections
                plot_steps = min(100, weights_array.shape[1])
                im = ax.imshow(weights_array[:, :plot_steps], 
                             aspect='auto', cmap='RdYlBu', interpolation='nearest')
                ax.set_xlabel('Connection Index')
                ax.set_ylabel('Time Step')
                ax.set_title('Synaptic Weight Evolution')
                plt.colorbar(im, ax=ax)
        
        # Plot activity maps
        if 'activity_maps' in time_data:
            ax = axes[1, 1]\n            activity_maps = time_data['activity_maps']
            
            # Show last activity map
            if activity_maps:
                last_map = activity_maps[-1]
                if isinstance(last_map, dict) and 'weights' in last_map:
                    weights = last_map['weights']
                    if weights:
                        ax.imshow(weights, cmap='hot', interpolation='nearest')
                        ax.set_title('Final Activity Map')
        
        # Plot neuromodulation
        if 'neuromodulation' in time_data:
            ax = axes[2, 0]
            neuromod = time_data['neuromodulation']
            
            # Plot dopamine, curiosity, novelty
            for modulator in ['dopamine', 'curiosity', 'novelty', 'prediction_error']:
                if modulator in neuromod:
                    values = neuromod[modulator]
                    ax.plot(time_points, values, label=modulator, alpha=0.7)
            
            ax.set_xlabel('Time (ms)')
            ax.set_ylabel('Modulation Strength')
            ax.set_title('Neuromodulation Dynamics')
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        # Plot memory traces
        if 'memory' in time_data:
            ax = axes[2, 1]
            memory = time_data['memory']
            
            # Plot working memory
            if 'working_memory' in memory:
                wm_trace = memory['working_memory']
                ax.plot(time_points, wm_trace, label='Working Memory', alpha=0.7)
            
            # Plot episodic memory
            if 'episodic_memory' in memory:
                em_trace = memory['episodic_memory']
                ax.plot(time_points, em_trace, label='Episodic Memory', alpha=0.7)
            
            ax.set_xlabel('Time (ms)')
            ax.set_ylabel('Memory Strength')
            ax.set_title('Memory Dynamics')
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        # Save figure
        plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    def create_brain_atlas(self, regions: Dict[str, Any], output_path: str = None, 
                          format: str = "png") -> str:
        """Create brain atlas visualization.
        
        Args:
            regions: Dictionary with brain region data
            output_path: Output file path
            format: Output format (png, svg, pdf)
            
        Returns:
            Path to saved visualization
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"brain_atlas_{timestamp}.{format}"
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('NLM Brain Atlas', fontsize=16)
        
        # Plot 1: Region types
        ax = axes[0, 0]
        region_types = {}
        for region_name, region_data in regions.items():
            rtype = region_data.get('type', 'default')
            if rtype not in region_types:
                region_types[rtype] = []
            region_types[rtype].append(region_name)
        
        colors = plt.cm.Set3(np.linspace(0, 1, len(region_types)))
        for i, (rtype, region_list) in enumerate(region_types.items()):
            ax.barh(range(len(region_list)), [1] * len(region_list), 
                   left=[0] * len(region_list), color=colors[i], 
                   label=f"{rtype.title()} regions", alpha=0.7)
            ax.set_yticks(range(len(region_list)))
            ax.set_yticklabels(region_list)
        
        ax.set_title("Region Types")
        ax.set_xlabel("Count")
        ax.legend()
        
        # Plot 2: Neuron density distribution
        ax = axes[0, 1]
        densities = []
        region_names = []
        
        for region_name, region_data in regions.items():
            if 'neuron_count' in region_data:
                densities.append(region_data['neuron_count'])
                region_names.append(region_name)
        
        if densities:
            bars = ax.barh(region_names, densities, color='steelblue', alpha=0.7)
            ax.set_title("Neuron Density by Region")
            ax.set_xlabel("Neuron Count")
            
            # Add count labels
            for bar in bars:
                width = bar.get_width()
                ax.text(width, bar.get_y() + bar.get_height()/2, 
                       f'{int(width)}', ha='left', va='center')
        
        # Plot 3: Connection strength heatmap (simplified)
        ax = axes[1, 0]
        if len(regions) >= 4:
            # Sample region connections
            region_names = list(regions.keys())[:4]
            n = len(region_names)
            
            # Create random connection matrix for visualization
            conn_matrix = np.random.rand(n, n) * 100
            np.fill_diagonal(conn_matrix, 0)  # No self-connections
            
            im = ax.imshow(conn_matrix, cmap='YlOrRd', interpolation='nearest')
            ax.set_xticks(range(n))
            ax.set_yticks(range(n))
            ax.set_xticklabels(region_names)
            ax.set_yticklabels(region_names)
            ax.set_title("Inter-Region Connectivity")
            plt.colorbar(im, ax=ax)
        
        # Plot 4: Activity summary
        ax = axes[1, 1]
        activity_data = []
        
        for region_name, region_data in regions.items():
            if 'activity' in region_data:
                activity_data.append(region_data['activity'])
            else:
                # Generate synthetic activity if not present
                activity_data.append(np.random.random() * 0.8 + 0.1)
        
        if activity_data:
            bars = ax.barh(region_names, activity_data, color='darkgreen', alpha=0.7)
            ax.set_title("Region Activity Levels")
            ax.set_xlabel("Activity (0-1)")
            
            # Add value labels
            for bar in bars:
                width = bar.get_width()
                ax.text(width, bar.get_y() + bar.get_height()/2, 
                       f'{width:.2f}', ha='left', va='center')
        
        plt.tight_layout()
        
        # Save figure
        plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    def create_animation(self, frame_data: List[Dict], output_path: str = None,
                        format: str = "mp4", fps: int = 10) -> str:
        """Create animation from simulation frames.
        
        Args:
            frame_data: List of frame data dictionaries
            output_path: Output file path
            format: Output format (mp4, gif)
            fps: Frames per second
            
        Returns:
            Path to saved animation
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"simulation_animation_{timestamp}.{format}"
        
        # Create figure for animation
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Initialize plot elements
        scatter = ax.scatter([], [], s=50, c=[], cmap='viridis', alpha=0.7)
        ax.set_xlim(0, 10)
        ax.set_ylim(0, 10)
        ax.set_title("Simulation Animation")
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        
        # Add colorbar
        cbar = plt.colorbar(scatter)
        cbar.set_label('Neuron Type')
        
        # Animation update function
        def update(frame):
            frame_neurons = frame.get('neurons', [])
            if frame_neurons:
                x = [neuron.get('x', 0) for neuron in frame_neurons]
                y = [neuron.get('y', 0) for neuron in frame_neurons]
                types = [neuron.get('type', 0) for neuron in frame_neurons]
                
                scatter.set_offsets(np.column_stack([x, y]))
                scatter.set_array(np.array(types))
            
            ax.set_title(f"Simulation Animation - Frame {frame.get('frame', 0)}")
            return scatter,
        
        # Create animation
        anim = animation.FuncAnimation(
            fig, update, frames=frame_data, 
            interval=1000/fps, blit=True, repeat=False
        )
        
        # Save animation
        anim.save(output_path, writer='ffmpeg', fps=fps, 
                 extra_args=['-vcodec', 'libx264'] if format == 'mp4' else [])
        
        plt.close()
        return output_path


def load_simulation_data(file_path: str) -> Dict[str, Any]:
    """Load simulation data from JSON file.
    
    Args:
        file_path: Path to JSON data file
        
    Returns:
        Loaded simulation data
    """
    try:
        with open(file_path, 'r') as f:
            data = json.load(f)
        return data
    except Exception as e:
        print(f"Error loading simulation data: {e}")
        return {}


def main():
    """Main entry point for the visualization script."""
    parser = argparse.ArgumentParser(description="Visualize NLM simulation data")
    parser.add_argument("input", help="Input JSON file or data file")
    parser.add_argument("-o", "--output", help="Output file path")
    parser.add_argument("-f", "--format", choices=["png", "svg", "pdf", "mp4", "gif"], 
                       default="png", help="Output format")
    parser.add_argument("-t", "--type", choices=["connectivity", "timeseries", 
                       "atlas", "animation"], default="connectivity",
                       help="Type of visualization")
    parser.add_argument("-s", "--style", choices=["default", "classic", "ggplot", "seaborn"],
                       default="default", help="Plot style")
    parser.add_argument("-c", "--components", nargs="+", default=["connectivity", "timeseries", 
                       "atlas"], help="Components to visualize")
    
    args = parser.parse_args()
    
    # Initialize visualizer
    visualizer = BrainVisualizer(args.style)
    
    # Load simulation data
    sim_data = load_simulation_data(args.input)
    
    if not sim_data:
        print(f"Error: Could not load simulation data from {args.input}")
        sys.exit(1)
    
    output_files = []
    
    # Generate requested visualizations
    for component in args.components:
        try:
            if component == "connectivity":
                # For connectivity, we need neurons and connections
                neurons = sim_data.get('neurons', [])
                connections = sim_data.get('connections', [])
                output = visualizer.create_neuron_connectivity(
                    neurons, connections, args.output, args.format)
                output_files.append(output)
                
            elif component == "timeseries":
                output = visualizer.create_time_series(
                    sim_data, args.output, args.format)
                output_files.append(output)
                
            elif component == "atlas":
                regions = sim_data.get('regions', {})
                output = visualizer.create_brain_atlas(
                    regions, args.output, args.format)
                output_files.append(output)
                
            elif component == "animation":
                # For animation, we need frame data
                frames = sim_data.get('frames', [])
                if frames:
                    output = visualizer.create_animation(
                        frames, args.output, args.format)
                    output_files.append(output)
                else:
                    print("Warning: No frame data found for animation")
        
        except Exception as e:
            print(f"Error generating {component} visualization: {e}")
    
    # Print summary
    print("=" * 60)
    print("VISUALIZATION SUMMARY")
    print("=" * 60)
    print(f"Input file: {args.input}")
    print(f"Output format: {args.format}")
    print(f"Components generated: {len(output_files)}")
    
    for i, output_file in enumerate(output_files, 1):
        print(f"  {i}. {output_file}")
    
    print("=" * 60)


if __name__ == "__main__":
    main()
