#!/usr/bin/env python3
"""
NLM - Neural Network Visualization Tool

Network connectivity and topology visualization with sophisticated capabilities for researchers.

Features:
- Neural network graph visualization
- Synaptic connectivity matrix visualization
- Network topology visualization
- Region connectivity visualization
- Network dynamics visualization
- Community detection visualization
"""

import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from typing import Dict, List, Optional, Tuple, Any
import matplotlib.patches as mpatches
from scipy.sparse import csr_matrix
from scipy.sparse.csgraph import connected_components
import seaborn as sns
import itertools
from datetime import datetime
import os
import json
from dataclasses import dataclass

@dataclass
class NetworkVisualizationConfig:
    """Configuration for network visualization."""
    layout: str = "force_directed"  # force_directed, spring, circular, random
    node_size_type: str = "degree"  # degree, strength, betweenness
    edge_width_type: str = "strength"  # strength, weight
    show_labels: bool = True
    node_color_type: str = "region"  # region, degree, centrality
    edge_color_type: str = "weight"  # weight, type, strength
    highlight_nodes: List[int] = None
    highlight_edges: List[Tuple[int, int]] = None
    show_communities: bool = True
    community_detection_method: str = "louvain"  # louvain, kmeans, spectral
    window_size: Tuple[int, int] = (1400, 1000)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 10

class NLNetworkVisualizer:
    """
    Network connectivity and topology visualization tool.
    
    Provides sophisticated network visualization capabilities including graph visualization,
    connectivity matrices, topology analysis, and community detection with interactive
    exploration capabilities for neural networks.
    """
    
    def __init__(self, config: Optional[NetworkVisualizationConfig] = None):
        self.config = config or NetworkVisualizationConfig()
        self.graph = None
        self.pos = None
        self.community_map = {}
        self.node_attributes = {}
        self.edge_attributes = {}
        self.figure = None
        self.ax = None
        self.layout_cache = {}
        
        # Initialize community detection mapping
        if self.config.highlight_nodes is None:
            self.config.highlight_nodes = []
        if self.config.highlight_edges is None:
            self.config.highlight_edges = []
        
        # Setup visualization
        self._setup_visualization()
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        plt.style.use('seaborn-v0_8-whitegrid')
        self.figure, self.ax = plt.subplots(figsize=self.config.window_size)
        self.ax.set_facecolor('#f8f9fa')
    
    def create_network_graph(self, connectivity_matrix: np.ndarray, 
                           region_labels: Optional[List[str]] = None,
                           node_attributes: Optional[Dict] = None):
        """
        Create network graph from connectivity matrix.
        
        Args:
            connectivity_matrix: Adjacency matrix of neural network
            region_labels: Labels for each region/node
            node_attributes: Additional attributes for nodes
        """
        # Create networkx graph from adjacency matrix
        self.graph = nx.from_numpy_array(connectivity_matrix)
        
        # Add region labels if provided
        if region_labels:
            for i, label in enumerate(region_labels):
                self.graph.nodes[i]['label'] = label
        
        # Add node attributes if provided
        if node_attributes:
            for node_id, attributes in node_attributes.items():
                for key, value in attributes.items():
                    self.graph.nodes[node_id][key] = value
        
        # Detect communities if enabled
        if self.config.show_communities:
            self._detect_communities()
        
        # Layout will be computed when needed
        self.pos = None
    
    def _detect_communities(self):
        """Detect communities in the network."""
        try:
            if self.config.community_detection_method == "louvain":
                import community as community_louvain
                partition = community_louvain.best_partition(self.graph)
            elif self.config.community_detection_method == "kmeans":
                from sklearn.cluster import KMeans
                # Use eigenvector centrality as feature for clustering
                centrality = nx.eigenvector_centrality(self.graph, max_iter=100)
                features = np.array([[centrality[node]] for node in self.graph.nodes()])
                kmeans = KMeans(n_clusters=3, random_state=42).fit(features)
                partition = {node: cluster for node, cluster in zip(self.graph.nodes(), kmeans.labels_)}
            elif self.config.community_detection_method == "spectral":
                # Use spectral clustering based on Laplacian
                from sklearn.cluster import SpectralClustering
                adjacency = nx.to_numpy_array(self.graph)
                spectral = SpectralClustering(n_clusters=3, random_state=42, affinity='nearest_neighbors')
                labels = spectral.fit_predict(adjacency)
                partition = {node: label for node, label in zip(self.graph.nodes(), labels)}
            else:
                partition = {node: 0 for node in self.graph.nodes()}
            
            self.community_map = partition
            
        except Exception as e:
            print(f"Warning: Could not perform community detection: {e}")
            self.community_map = {node: 0 for node in self.graph.nodes()}
    
    def visualize_connectivity(self, show: bool = True):
        """
        Visualize neural network connectivity.
        
        Args:
            show: Whether to display the visualization
        """
        if self.graph is None:
            raise ValueError("Network graph not created. Call create_network_graph() first.")
        
        # Compute layout
        self._compute_layout()
        
        # Clear the plot
        self.ax.clear()
        
        # Get node sizes based on type
        node_sizes = self._get_node_sizes()
        
        # Get edge widths based on type
        edge_widths = self._get_edge_widths()
        
        # Get node colors based on type
        node_colors = self._get_node_colors()
        
        # Get edge colors based on type
        edge_colors = self._get_edge_colors()
        
        # Draw the network
        nx.draw_networkx_nodes(
            self.graph, self.pos, ax=self.ax,
            node_size=node_sizes,
            node_color=node_colors,
            alpha=0.9,
            edgecolors='black',
            linewidths=1
        )
        
        nx.draw_networkx_edges(
            self.graph, self.pos, ax=self.ax,
            width=edge_widths,
            edge_color=edge_colors,
            alpha=0.7,
            arrows=False
        )
        
        # Add node labels if enabled
        if self.config.show_labels:
            nx.draw_networkx_labels(
                self.graph, self.pos, ax=self.ax,
                font_size=8,
                font_weight='bold',
                labels={i: self.graph.nodes[i].get('label', str(i)) for i in self.graph.nodes()}
            )
        
        # Highlight specific nodes and edges
        self._draw_highlights()
        
        # Add community visualization if enabled
        if self.config.show_communities and self.community_map:
            self._add_community_visualization()
        
        # Set plot properties
        self.ax.set_title('Neural Network Connectivity', fontsize=16, fontweight='bold')
        self.ax.set_axis_off()
        
        if show:
            plt.tight_layout()
            plt.show()
    
    def _compute_layout(self):
        """Compute network layout based on configuration."""
        if self.pos is not None:
            return
        
        if self.config.layout == "force_directed":
            self.pos = nx.spring_layout(
                self.graph, seed=42, k=1, iterations=50,
                weight='weight' if 'weight' in self.graph.edges()[0] else None
            )
        elif self.config.layout == "spring":
            self.pos = nx.spring_layout(self.graph, seed=42)
        elif self.config.layout == "circular":
            self.pos = nx.circular_layout(self.graph)
        elif self.config.layout == "random":
            self.pos = nx.random_layout(self.graph, seed=42)
        else:
            self.pos = nx.spring_layout(self.graph, seed=42)
    
    def _get_node_sizes(self):
        """Get node sizes based on configuration."""
        if self.config.node_size_type == "degree":
            return [self.graph.degree(node) * 100 for node in self.graph.nodes()]
        elif self.config.node_size_type == "strength":
            if 'weight' in self.graph.edges()[0]:
                return [self.graph.degree(node, weight='weight') * 100 for node in self.graph.nodes()]
            else:
                return [self.graph.degree(node) * 100 for node in self.graph.nodes()]
        elif self.config.node_size_type == "betweenness":
            try:
                betweenness = nx.betweenness_centrality(self.graph)
                return [betweenness[node] * 2000 for node in self.graph.nodes()]
            except:
                return [self.graph.degree(node) * 100 for node in self.graph.nodes()]
        else:
            return [100 for _ in self.graph.nodes()]
    
    def _get_node_colors(self):
        """Get node colors based on configuration."""
        if self.config.node_color_type == "degree":
            degrees = [self.graph.degree(node) for node in self.graph.nodes()]
            return self._normalize_colors(degrees)
        
        elif self.config.node_color_type == "centrality":
            try:
                betweenness = nx.betweenness_centrality(self.graph)
                centralities = [betweenness[node] for node in self.graph.nodes()]
                return self._normalize_colors(centralities)
            except:
                return plt.cm.tab10(np.arange(len(self.graph)) / len(self.graph))
        
        elif self.config.node_color_type == "region":
            # Use different colors for different communities
            if self.community_map:
                unique_communities = list(set(self.community_map.values()))
                colors = plt.cm.Set3(np.linspace(0, 1, len(unique_communities)))
                return [colors[self.community_map[node]] for node in self.graph.nodes()]
            else:
                return plt.cm.tab10(np.arange(len(self.graph)) / len(self.graph))
        
        else:
            return plt.cm.tab10(np.arange(len(self.graph)) / len(self.graph))
    
    def _get_edge_widths(self):
        """Get edge widths based on configuration."""
        if self.config.edge_width_type == "strength":
            if 'weight' in self.graph.edges()[0]:
                weights = [self.graph[u][v]['weight'] for u, v in self.graph.edges()]
                return [w * 3 for w in weights]
            else:
                degrees = [self.graph.degree(u) + self.graph.degree(v) for u, v in self.graph.edges()]
                return [d * 2 for d in degrees]
        else:
            return [1 for _ in self.graph.edges()]
    
    def _get_edge_colors(self):
        """Get edge colors based on configuration."""
        if self.config.edge_color_type == "weight":
            if 'weight' in self.graph.edges()[0]:
                weights = [self.graph[u][v]['weight'] for u, v in self.graph.edges()]
                return self._normalize_colors(weights)
            else:
                return ['gray' for _ in self.graph.edges()]
        else:
            return ['steelblue' for _ in self.graph.edges()]
    
    def _normalize_colors(self, values):
        """Normalize values to colormap range."""
        if not values:
            return plt.cm.viridis(np.arange(10) / 10)
        
        min_val = min(values)
        max_val = max(values)
        
        if max_val == min_val:
            return plt.cm.viridis(np.ones(len(values)))
        
        normalized = [(v - min_val) / (max_val - min_val) for v in values]
        return plt.cm.viridis(np.array(normalized))
    
    def _draw_highlights(self):
        """Draw highlighted nodes and edges."""
        # Highlight specific nodes
        if self.config.highlight_nodes:
            nx.draw_networkx_nodes(
                self.graph, self.pos, ax=self.ax,
                nodelist=self.config.highlight_nodes,
                node_size=[200 for _ in self.config.highlight_nodes],
                node_color='red',
                alpha=0.8,
                edgecolors='darkred',
                linewidths=2
            )
        
        # Highlight specific edges
        if self.config.highlight_edges:
            nx.draw_networkx_edges(
                self.graph, self.pos, ax=self.ax,
                edgelist=self.config.highlight_edges,
                width=4,
                edge_color='red',
                alpha=0.9
            )
    
    def _add_community_visualization(self):
        """Add community visualization to the plot."""
        if not self.community_map:
            return
        
        # Create patches for legend
        unique_communities = list(set(self.community_map.values()))
        colors = plt.cm.Set3(np.linspace(0, 1, len(unique_communities)))
        
        patches = []
        for community in unique_communities:
            patch = mpatches.Patch(color=colors[community], label=f'Community {community}')
            patches.append(patch)
        
        # Add legend
        self.ax.legend(handles=patches, loc='upper right', fontsize=8)
    
    def visualize_connectivity_matrix(self, show: bool = True):
        """
        Visualize synaptic connectivity matrix.
        
        Args:
            show: Whether to display the visualization
        """
        if self.graph is None:
            raise ValueError("Network graph not created. Call create_network_graph() first.")
        
        # Convert graph to adjacency matrix
        adjacency_matrix = nx.to_numpy_array(self.graph)
        
        # Create heatmap visualization
        plt.figure(figsize=self.config.window_size)
        
        # Create heatmap
        sns.heatmap(
            adjacency_matrix, 
            cmap='Blues', 
            square=True, 
            cbar=True,
            xticklabels=False,
            yticklabels=False
        )
        
        plt.title('Synaptic Connectivity Matrix', fontsize=16, fontweight='bold')
        
        if show:
            plt.tight_layout()
            plt.show()
    
    def visualize_network_topology(self, show: bool = True):
        """
        Visualize network topology analysis.
        
        Args:
            show: Whether to display the visualization
        """
        if self.graph is None:
            raise ValueError("Network graph not created. Call create_network_graph() first.")
        
        # Create figure with multiple subplots
        fig, axes = plt.subplots(2, 2, figsize=self.config.window_size)
        fig.suptitle('Network Topology Analysis', fontsize=16, fontweight='bold')
        
        # 1. Degree distribution
        degrees = [self.graph.degree(node) for node in self.graph.nodes()]
        axes[0, 0].hist(degrees, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
        axes[0, 0].set_xlabel('Degree')
        axes[0, 0].set_ylabel('Frequency')
        axes[0, 0].set_title('Degree Distribution')
        
        # 2. Degree distribution (log scale)
        axes[0, 1].hist(np.log10(np.array(degrees) + 1), bins=20, alpha=0.7, color='lightgreen', edgecolor='black')
        axes[0, 1].set_xlabel('Log10(Degree + 1)')
        axes[0, 1].set_ylabel('Frequency')
        axes[0, 1].set_title('Degree Distribution (Log Scale)')
        
        # 3. Network metrics
        try:
            # Calculate basic network metrics
            num_nodes = self.graph.number_of_nodes()
            num_edges = self.graph.number_of_edges()
            density = nx.density(self.graph)
            
            metrics_text = f"Number of nodes: {num_nodes}\n"
            metrics_text += f"Number of edges: {num_edges}\n"
            metrics_text += f"Network density: {density:.4f}\n"
            metrics_text += f"Average degree: {2 * num_edges / num_nodes:.2f}\n"
            
            if num_nodes > 2:
                try:
                    clustering = nx.average_clustering(self.graph)
                    metrics_text += f"Average clustering: {clustering:.4f}\n"
                except:
                    metrics_text += f"Average clustering: N/A\n"
                
                try:
                    diameter = nx.diameter(self.graph)
                    metrics_text += f"Network diameter: {diameter}\n"
                except:
                    metrics_text += f"Network diameter: N/A\n"
            
            axes[1, 0].text(0.1, 0.5, metrics_text, fontsize=12, 
                          verticalalignment='center', fontfamily='monospace')
            axes[1, 0].axis('off')
            axes[1, 0].set_title('Network Metrics')
            
        except Exception as e:
            axes[1, 0].text(0.1, 0.5, f"Error calculating metrics: {e}", 
                          fontsize=10, verticalalignment='center', fontfamily='monospace')
            axes[1, 0].axis('off')
            axes[1, 0].set_title('Network Metrics')
        
        # 4. Small-world coefficient (if graph is not too large)
        try:
            if self.graph.number_of_nodes() <= 100:
                # Calculate small-world coefficient
                ws = nx.small_worldness(self.graph)
                axes[1, 1].bar(['Small-World Coefficient'], [ws], 
                              color='lightcoral', alpha=0.7)
                axes[1, 1].set_ylabel('Small-World Coefficient')
                axes[1, 1].set_title('Small-World Topology')
                axes[1, 1].axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='Random Network')
                axes[1, 1].legend()
            else:
                axes[1, 1].text(0.1, 0.5, 'Graph too large\nfor small-world\nanalysis', 
                              fontsize=12, horizontalalignment='center', verticalalignment='center')
                axes[1, 1].axis('off')
                axes[1, 1].set_title('Small-World Topology')
        except Exception as e:
            axes[1, 1].text(0.1, 0.5, f"Error: {e}", 
                          fontsize=10, horizontalalignment='center', verticalalignment='center')
            axes[1, 1].axis('off')
            axes[1, 1].set_title('Small-World Topology')
        
        plt.tight_layout()
        
        if show:
            plt.show()
    
    def save_visualization(self, filename: str):
        """
        Save visualization to file.
        
        Args:
            filename: Path to save file
        """
        try:
            plt.savefig(filename, dpi=300, bbox_inches='tight')
            print(f"Visualization saved to: {filename}")
        except Exception as e:
            print(f"Error saving visualization: {e}")

def main():
    """Main entry point for nlx-viz-network tool."""
    print("NLM - Neural Network Visualization Tool")
    print("=" * 50)
    print("Network connectivity and topology visualization")
    print()
    
    # Example usage
    config = NetworkVisualizationConfig(
        layout="force_directed",
        node_size_type="degree",
        edge_width_type="strength",
        show_labels=True,
        node_color_type="region",
        show_communities=True,
        window_size=(1400, 1000)
    )
    
    # Create network visualizer
    visualizer = NLNetworkVisualizer(config)
    
    # Example connectivity matrix (5 regions)
    connectivity_matrix = np.array([
        [0, 0.8, 0.1, 0.0, 0.0],
        [0.8, 0, 0.6, 0.2, 0.1],
        [0.1, 0.6, 0, 0.3, 0.2],
        [0.0, 0.2, 0.3, 0, 0.5],
        [0.0, 0.1, 0.2, 0.5, 0]
    ])
    
    region_labels = ['Visual Cortex', 'Auditory Cortex', 'Prefrontal Cortex', 
                    'Motor Cortex', 'Somatosensory Cortex']
    
    # Create and visualize network
    visualizer.create_network_graph(connectivity_matrix, region_labels)
    visualizer.visualize_connectivity()
    
    print("Example network visualization created successfully!")
    print(f"Regions: {len(region_labels)}")
    print(f"Layout: {config.layout}")
    print(f"Node color type: {config.node_color_type}")
    print()
    print("To use the visualizer:")
    print("  visualizer = NLNetworkVisualizer(config)")
    print("  visualizer.create_network_graph(connectivity_matrix, region_labels)")
    print("  visualizer.visualize_connectivity()")
    print("  visualizer.visualize_connectivity_matrix()")
    print("  visualizer.save_visualization('network.png')")

if __name__ == "__main__":
    main()
