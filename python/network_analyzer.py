#!/usr/bin/env python3
"""
Network Analyzer Tool

Graph theory analysis of neural networks.
Provides comprehensive graph theoretical analysis of neural network structure
and connectivity patterns for scientific research.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import networkx as nx
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats, sparse
from sklearn.cluster import DBSCAN
from sklearn.metrics import silhouette_score
import sys
import os

@dataclass
class NetworkAnalyzerConfig:
    """Configuration for network analyzer"""
    graph_type: str = "directed"  # "directed", "undirected", "bipartite"
    analysis_depth: str = "comprehensive"  # "basic", "intermediate", "comprehensive"
    show_topology: bool = True
    show_centrality: bool = True
    show_communities: bool = True
    show_path_analysis: bool = True
    show_efficiency: bool = True
    show_clustering: bool = True
    show_dynamics: bool = True
    color_scheme: str = "default"  # "default", "centrality", "community", "hierarchical"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "json", "html"
    output_path: str = "network_analyzer"
    
    # Analysis parameters
    centrality_measures: List[str] = None  # "degree", "betweenness", "closeness", "eigenvector", "pagerank"
    path_length_types: List[str] = None  # "shortest", "longest", "average"
    community_detection_methods: List[str] = None  # "louvain", "label_propagation", "girvan_newman"
    clustering_measures: List[str] = None  # "transitivity", "clustering", "community_size"
    
    # Filtering and analysis
    threshold: float = 0.5
    min_degree: int = 1
    max_degree: int = None
    show_connected_components: bool = True
    show_network_metrics: bool = True
    
    # Display settings
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    show_legend: bool = True
    show_colorbar: bool = True
class NetworkAnalyzer:
    """
    Graph theory analysis tool for neural networks.
    
    Features:
    - Network topology and structure analysis
    - Centrality measures and node importance
    - Community detection and modular analysis
    - Path length and distance analysis
    - Network efficiency and robustness metrics
    - Clustering and modularity analysis
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: NetworkAnalyzerConfig = None):
        self.config = config or NetworkAnalyzerConfig()
        self.graph = None
        self.layout = None
        self.centrality_data = {}
        self.community_data = {}
        self.path_data = {}
        self.efficiency_data = {}
        self.clustering_data = {}
        
        # Initialize analyzer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize analyzer components and data structures"""
        # Default centrality measures
        if self.config.centrality_measures is None:
            self.config.centrality_measures = ["degree", "betweenness", "closeness"]
        
        # Default path length types
        if self.config.path_length_types is None:
            self.config.path_length_types = ["shortest", "average"]
        
        # Default community detection methods
        if self.config.community_detection_methods is None:
            self.config.community_detection_methods = ["louvain"]
        
        # Default clustering measures
        if self.config.clustering_measures is None:
            self.config.clustering_measures = ["transitivity", "clustering"]
    
    def create_graph_from_adjacency(self, 
                                   adjacency_matrix: np.ndarray,
                                   node_labels: Optional[List[str]] = None,
                                   directed: Optional[bool] = None) -> Dict[str, Any]:
        """
        Create network graph from adjacency matrix.
        
        Args:
            adjacency_matrix: Adjacency matrix (N x N)
            node_labels: Optional node labels
            directed: Whether graph should be directed
            
        Returns:
            Dictionary with graph creation results
        """
        result = {
            "success": False,
            "data": {},
            "graph_info": {},
            "analysis": {}
        }
        
        try:
            # Validate input
            if adjacency_matrix.ndim != 2 or adjacency_matrix.shape[0] != adjacency_matrix.shape[1]:
                raise ValueError("Adjacency matrix must be square (N x N)")
            
            n_nodes = adjacency_matrix.shape[0]
            
            # Create networkx graph
            if directed is None:
                directed = self.config.graph_type == "directed"
            
            if directed:
                self.graph = nx.DiGraph()
            else:
                self.graph = nx.Graph()
            
            # Add nodes
            if node_labels is None:
                node_labels = [f"Node_{i}" for i in range(n_nodes)]
            
            for i in range(n_nodes):
                self.graph.add_node(i, label=node_labels[i])
            
            # Add edges based on adjacency matrix
            threshold = self.config.threshold
            
            for i in range(n_nodes):
                for j in range(n_nodes):
                    weight = adjacency_matrix[i, j]
                    if weight > threshold:
                        if directed:
                            self.graph.add_edge(i, j, weight=weight)
                        else:
                            # For undirected graphs, add edge only once
                            if i < j:
                                self.graph.add_edge(i, j, weight=weight)
            
            # Compute layout
            self._compute_layout()
            
            # Compute graph statistics
            graph_info = self._compute_graph_info()
            
            # Perform analysis based on configuration
            analysis = self._perform_comprehensive_analysis()
            
            result["graph_info"] = graph_info
            result["analysis"] = analysis
            result["data"] = {
                "adjacency_matrix": adjacency_matrix.tolist(),
                "node_labels": node_labels,
                "directed": directed,
                "edge_count": len(self.graph.edges()),
                "node_count": len(self.graph.nodes())
            }
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _compute_layout(self):
        """Compute graph layout for visualization"""
        # Choose layout based on graph type
        if self.config.graph_type == "bipartite":
            # Try to partition nodes into two sets for bipartite layout
            try:
                self.layout = nx.bipartite_layout(self.graph, subset=self._find_bipartite_subset())
            except:
                # Fallback to spring layout
                self.layout = nx.spring_layout(self.graph, k=1, iterations=50, seed=42)
        else:
            # Use spring layout for general graphs
            self.layout = nx.spring_layout(self.graph, k=1, iterations=50, seed=42)
    
    def _find_bipartite_subset(self) -> List[int]:
        """Find bipartite subset for bipartite layout"""
        # Simple heuristic: assign even and odd indexed nodes to different sets
        nodes = list(self.graph.nodes())
        return nodes[::2]  # Every other node
    
    def _compute_graph_info(self) -> Dict[str, Any]:
        """Compute basic graph information"""
        if self.graph is None:
            return {}
        
        info = {
            "node_count": len(self.graph.nodes()),
            "edge_count": len(self.graph.edges()),
            "density": nx.density(self.graph),
            "is_directed": self.graph.is_directed(),
            "is_connected": nx.is_connected(self.graph) if not self.graph.is_directed() else False,
            "average_degree": np.mean([d for n, d in self.graph.degree()]) if not self.graph.is_directed() else None,
            "degree_distribution": {},
            "components": {}
        }
        
        # Compute degree distribution
        degrees = [d for n, d in self.graph.degree()]
        if degrees:
            info["degree_distribution"] = {
                "min": min(degrees),
                "max": max(degrees),
                "mean": np.mean(degrees),
                "std": np.std(degrees),
                "histogram": self._compute_degree_histogram(degrees)
            }
        
        # Compute connected components
        if not self.graph.is_directed():
            components = list(nx.connected_components(self.graph))
            info["components"] = {
                "number_of_components": len(components),
                "component_sizes": [len(c) for c in components],
                "largest_component_size": max([len(c) for c in components]) if components else 0
            }
        
        return info
    
    def _compute_degree_histogram(self, degrees: List[int]) -> Dict[str, List[int]]:
        """Compute degree histogram"""
        max_degree = max(degrees) if degrees else 0
        histogram = {}
        
        for degree in range(max_degree + 1):
            histogram[str(degree)] = degrees.count(degree)
        
        return histogram
    
    def _perform_comprehensive_analysis(self) -> Dict[str, Any]:
        """Perform comprehensive network analysis"""
        if self.graph is None:
            return {}
        
        analysis = {
            "centrality": {},
            "communities": {},
            "paths": {},
            "efficiency": {},
            "clustering": {},
            "dynamics": {}
        }
        
        # Compute centrality measures
        analysis["centrality"] = self._compute_centrality_measures()
        
        # Compute community structure
        analysis["communities"] = self._compute_communities()
        
        # Compute path analysis
        analysis["paths"] = self._compute_path_analysis()
        
        # Compute network efficiency
        analysis["efficiency"] = self._compute_efficiency()
        
        # Compute clustering measures
        analysis["clustering"] = self._compute_clustering_measures()
        
        # Compute dynamic properties (if requested)
        if self.config.show_dynamics:
            analysis["dynamics"] = self._compute_dynamics()
        
        return analysis
    
    def _compute_centrality_measures(self) -> Dict[str, Any]:
        """Compute centrality measures"""
        centrality = {}
        
        for measure in self.config.centrality_measures:
            try:
                if measure == "degree":
                    if self.graph.is_directed():
                        in_degree = dict(self.graph.in_degree())
                        out_degree = dict(self.graph.out_degree())
                        centrality["degree_in"] = in_degree
                        centrality["degree_out"] = out_degree
                    else:
                        centrality["degree"] = dict(self.graph.degree())
                
                elif measure == "betweenness":
                    if nx.is_connected(self.graph) or not self.graph.is_directed():
                        centrality["betweenness"] = nx.betweenness_centrality(self.graph)
                    else:
                        centrality["betweenness"] = {node: 0.0 for node in self.graph.nodes()}
                
                elif measure == "closeness":
                    if nx.is_connected(self.graph) or not self.graph.is_directed():
                        centrality["closeness"] = nx.closeness_centrality(self.graph)
                    else:
                        centrality["closeness"] = {node: 0.0 for node in self.graph.nodes()}
                
                elif measure == "eigenvector":
                    try:
                        centrality["eigenvector"] = nx.eigenvector_centrality(self.graph, max_iter=1000)
                    except:
                        centrality["eigenvector"] = {node: 0.0 for node in self.graph.nodes()}
                
                elif measure == "pagerank":
                    centrality["pagerank"] = nx.pagerank(self.graph, max_iter=1000)
                
            except Exception as e:
                print(f"Warning: Could not compute {measure} centrality: {e}")
                centrality[measure] = {node: 0.0 for node in self.graph.nodes()}
        
        return centrality
    
    def _compute_communities(self) -> Dict[str, Any]:
        """Compute community structure"""
        communities = {}
        
        for method in self.config.community_detection_methods:
            try:
                if method == "louvain":
                    # Use Louvain algorithm via networkx community API
                    if hasattr(nx, 'community'):
                        communities["louvain"] = nx.community.louvain_communities(self.graph, weight='weight')
                    else:
                        # Fallback to greedy modularity communities
                        communities["louvain"] = list(nx.algorithms.community.greedy_modularity_communities(self.graph))
                
                elif method == "label_propagation":
                    communities["label_propagation"] = list(nx.algorithms.community.label_propagation_communities(self.graph))
                
                elif method == "girvan_newman":
                    if nx.is_connected(self.graph) or not self.graph.is_directed():
                        communities["girvan_newman"] = list(nx.algorithms.community.girvan_newman(self.graph))
                    else:
                        communities["girvan_newman"] = [list(self.graph.nodes())]  # Single community
                
            except Exception as e:
                print(f"Warning: Could not compute {method} communities: {e}")
                communities[method] = [list(self.graph.nodes())]  # Single community fallback
        
        # Compute community metrics
        if communities:
            communities["metrics"] = self._compute_community_metrics(communities)
        
        return communities
    
    def _compute_community_metrics(self, communities: Dict[str, List]) -> Dict[str, Any]:
        """Compute community detection metrics"""
        metrics = {
            "number_of_communities": {},
            "community_sizes": {},
            "modularity": {},
            "conductance": {}
        }
        
        for method, community_list in communities.items():
            if method == "louvain" or method == "label_propagation":
                # Convert to partition format
                partition = {}
                for i, community in enumerate(community_list):
                    for node in community:
                        partition[node] = i
                
                metrics["number_of_communities"][method] = len(community_list)
                metrics["community_sizes"][method] = [len(c) for c in community_list]
                
                # Compute modularity
                try:
                    metrics["modularity"][method] = nx.algorithms.community.modularity(
                        self.graph, community_list
                    )
                except:
                    metrics["modularity"][method] = 0.0
                
                # Compute conductance
                try:
                    metrics["conductance"][method] = nx.algorithms.community.conductance(
                        self.graph, community_list
                    )
                except:
                    metrics["conductance"][method] = 1.0
        
        return metrics
    
    def _compute_path_analysis(self) -> Dict[str, Any]:
        """Compute path length analysis"""
        paths = {}
        
        try:
            # Check if graph is connected
            if nx.is_connected(self.graph) or not self.graph.is_directed():
                # Compute shortest paths
                paths["shortest_paths"] = self._compute_shortest_path_analysis()
                
                # Compute characteristic path length
                try:
                    paths["characteristic_path_length"] = nx.algorithms.shortest_pathscharacteristic_path_length(self.graph)
                except:
                    paths["characteristic_path_length"] = 0.0
                
                # Compute diameter
                try:
                    paths["diameter"] = nx.diameter(self.graph)
                except:
                    paths["diameter"] = 0
                
                # Compute radius
                try:
                    paths["radius"] = nx.radius(self.graph)
                except:
                    paths["radius"] = 0
                
            else:
                # Handle disconnected graphs
                paths["note"] = "Graph is disconnected - computing paths for each component"
                
                # Compute paths for each connected component
                if not self.graph.is_directed():
                    components = list(nx.connected_components(self.graph))
                    for i, component in enumerate(components):
                        subgraph = self.graph.subgraph(component)
                        try:
                            paths[f"component_{i}_shortest_paths"] = self._compute_shortest_path_analysis(subgraph)
                            paths[f"component_{i}_diameter"] = nx.diameter(subgraph) if len(component) > 1 else 0
                            paths[f"component_{i}_radius"] = nx.radius(subgraph) if len(component) > 1 else 0
                        except:
                            paths[f"component_{i}_shortest_paths"] = {"error": "Computation failed"}
        
            except Exception as e:
                paths["error"] = str(e)
        
        return paths
    
    def _compute_shortest_path_analysis(self, graph=None) -> Dict[str, Any]:
        """Compute shortest path analysis for a graph"""
        if graph is None:
            graph = self.graph
        
        try:
            # Compute shortest path lengths
            shortest_paths = dict(nx.all_pairs_shortest_path_length(graph))
            
            # Compute statistics
            all_lengths = []
            for source, targets in shortest_paths.items():
                all_lengths.extend(targets.values())
            
            if all_lengths:
                return {
                    "number_of_pairs": len(all_lengths),
                    "average_path_length": np.mean(all_lengths),
                    "median_path_length": np.median(all_lengths),
                    "std_path_length": np.std(all_lengths),
                    "min_path_length": min(all_lengths),
                    "max_path_length": max(all_lengths),
                    "path_length_distribution": self._compute_path_length_histogram(all_lengths)
                }
            else:
                return {"error": "No path lengths computed"}
            
        except Exception as e:
            return {"error": str(e)}
    
    def _compute_path_length_histogram(self, path_lengths: List[int]) -> Dict[str, int]:
        """Compute histogram of path lengths"""
        if not path_lengths:
            return {}
        
        max_length = max(path_lengths)
        histogram = {}
        
        for length in range(max_length + 1):
            histogram[str(length)] = path_lengths.count(length)
        
        return histogram
    
    def _compute_efficiency(self) -> Dict[str, Any]:n        """Compute network efficiency metrics"""
        efficiency = {}
        
        try:
            # Global efficiency
            efficiency["global"] = nx.algorithms.efficiency(self.graph)
            
            # Local efficiency
            efficiency["local"] = nx.algorithms.local_efficiency(self.graph)
            
            # Nodal efficiency
            efficiency["nodal"] = nx.algorithms.nodal_efficiency(self.graph)
            
        except Exception as e:
            efficiency["error"] = str(e)
        
        return efficiency
    
    def _compute_clustering_measures(self) -> Dict[str, Any]:
        """Compute clustering measures"""
        clustering = {}
        
        try:
            # Transitivity (fraction of triangles in graph)
            clustering["transitivity"] = nx.transitivity(self.graph)
            
            # Clustering coefficient for each node
            clustering["clustering_coefficient"] = nx.clustering(self.graph)
            
            # Average clustering coefficient
            clustering["average_clustering"] = nx.average_clustering(self.graph)
            
            # Global clustering coefficient
            try:
                clustering["global_clustering"] = nx.global_clustering(self.graph)
            except:
                clustering["global_clustering"] = 0.0
            
        except Exception as e:
            clustering["error"] = str(e)
        
        return clustering
    
    def _compute_dynamics(self) -> Dict[str, Any]:
        """Compute dynamic properties"""
        dynamics = {}
        
        try:
            # For directed graphs, compute PageRank for importance
            if self.graph.is_directed():
                dynamics["pagerank"] = nx.pagerank(self.graph, max_iter=1000)
            
            # Compute betweenness centrality for robustness
            if nx.is_connected(self.graph) or not self.graph.is_directed():
                dynamics["betweenness_centrality"] = nx.betweenness_centrality(self.graph)
            
            # Compute closeness centrality for reachability
            if nx.is_connected(self.graph) or not self.graph.is_directed():
                dynamics["closeness_centrality"] = nx.closeness_centrality(self.graph)
            
        except Exception as e:
            dynamics["error"] = str(e)
        
        return dynamics
    
    def create_visualization(self) -> Dict[str, Any]:
        """
        Create network visualization.
        
        Returns:
            Dictionary with visualization data and results
        """
        result = {
            "success": False,
            "data": {},
            "visualization": None,
            "statistics": {}
        }
        
        try:
            if self.graph is None:
                result["error"] = "No graph available for visualization"
                return result
            
            # Create figure with subplots
            fig, axes = plt.subplots(2, 3, figsize=(18, 12))
            fig.suptitle("Network Analysis Visualization", fontsize=16, fontweight='bold')
            
            axes = axes.flatten()
            
            # Plot 1: Network topology
            self._plot_network_topology(axes[0])
            
            # Plot 2: Centrality visualization
            self._plot_centrality_visualization(axes[1])
            
            # Plot 3: Community structure
            self._plot_community_structure(axes[2])
            
            # Plot 4: Path length distribution
            self._plot_path_length_distribution(axes[3])
            
            # Plot 5: Network efficiency
            self._plot_network_efficiency(axes[4])
            
            # Plot 6: Clustering analysis
            self._plot_clustering_analysis(axes[5])
            
            plt.tight_layout()
            
            result["visualization"] = fig
            result["success"] = True
            
            # Compute visualization statistics
            result["statistics"] = self._compute_graph_info()
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _plot_network_topology(self, ax):
        """Plot network topology"""
        if self.graph is None or self.layout is None:
            ax.text(0.5, 0.5, 'No graph available', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Draw network graph
        nx.draw_networkx_nodes(self.graph, self.layout, ax=ax, node_size=50, alpha=0.7)
        nx.draw_networkx_edges(self.graph, self.layout, ax=ax, alpha=0.3, width=1)
        
        ax.set_title("Network Topology", fontsize=12, fontweight='bold')
        ax.axis('off')
    
    def _plot_centrality_visualization(self, ax):
        """Plot centrality visualization"""
        if not self.centrality_data.get("degree"):
            ax.text(0.5, 0.5, 'No centrality data', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Create scatter plot of centrality measures
        nodes = list(self.centrality_data["degree"].keys())
        degree_values = [self.centrality_data["degree"].get(node, 0) for node in nodes]
        
        scatter = ax.scatter(range(len(nodes)), degree_values, s=50, alpha=0.7, c=degree_values, cmap='viridis')
        
        ax.set_title("Degree Centrality", fontsize=12, fontweight='bold')
        ax.set_xlabel("Node Index")
        ax.set_ylabel("Degree Centrality")
        ax.grid(True, alpha=0.3)
        
        # Add colorbar
        plt.colorbar(scatter, ax=ax)
    
    def _plot_community_structure(self, ax):
        """Plot community structure"""
        if not self.community_data.get("louvain"):
            ax.text(0.5, 0.5, 'No community data', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Get community partition
        communities = self.community_data["louvain"]
        partition = {}
        for i, community in enumerate(communities):
            for node in community:
                partition[node] = i
        
        # Assign colors to communities
        colors = plt.cm.Set3(np.linspace(0, 1, len(communities)))
        
        # Draw network with community colors
        for node in self.graph.nodes():
            community_id = partition.get(node, 0)
            ax.scatter(self.layout[node][0], self.layout[node][1], 
                      c=[colors[community_id]], s=50, alpha=0.7)
        
        # Draw edges
        nx.draw_networkx_edges(self.graph, self.layout, ax=ax, alpha=0.3, width=1)
        
        ax.set_title("Community Structure", fontsize=12, fontweight='bold')
        ax.axis('off')
    
    def _plot_path_length_distribution(self, ax):
        """Plot path length distribution"""
        if not self.path_data.get("shortest_paths"):
            ax.text(0.5, 0.5, 'No path data', ha='center', va='center', transform=ax.transAxes)
            return
        
        path_info = self.path_data["shortest_paths"]
        if "error" in path_info:
            ax.text(0.5, 0.5, 'Path computation failed', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Create histogram of path lengths
        histogram_data = path_info.get("path_length_distribution", {})
        if histogram_data:
            lengths = list(map(int, histogram_data.keys()))
            counts = list(histogram_data.values())
            
            ax.bar(lengths, counts, alpha=0.7, color='skyblue', edgecolor='black')
            ax.set_title("Path Length Distribution", fontsize=12, fontweight='bold')
            ax.set_xlabel("Path Length")
            ax.set_ylabel("Frequency")
            ax.grid(True, alpha=0.3)
    
    def _plot_network_efficiency(self, ax):
        """Plot network efficiency metrics"""
        if not self.efficiency_data:
            ax.text(0.5, 0.5, 'No efficiency data', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Create bar chart of efficiency metrics
        metrics = ['Global', 'Local', 'Nodal']
        values = [
            self.efficiency_data.get("global", 0.0),
            self.efficiency_data.get("local", 0.0),
            self.efficiency_data.get("nodal", 0.0)
        ]
        
        bars = ax.bar(metrics, values, alpha=0.7, color=['lightblue', 'lightgreen', 'lightcoral'])
        ax.set_title("Network Efficiency", fontsize=12, fontweight='bold')
        ax.set_ylabel("Efficiency")
        ax.set_ylim(0, max(values) * 1.1 if values else 1)
        ax.grid(True, alpha=0.3, axis='y')
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                   f'{value:.3f}', ha='center', va='bottom')
    
    def _plot_clustering_analysis(self, ax):
        """Plot clustering analysis"""
        if not self.clustering_data:
            ax.text(0.5, 0.5, 'No clustering data', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Create bar chart of clustering metrics
        metrics = ['Transitivity', 'Average Clustering']
        values = [
            self.clustering_data.get("transitivity", 0.0),
            self.clustering_data.get("average_clustering", 0.0)
        ]
        
        bars = ax.bar(metrics, values, alpha=0.7, color=['lightblue', 'lightgreen'])
        ax.set_title("Clustering Analysis", fontsize=12, fontweight='bold')
        ax.set_ylabel("Clustering Coefficient")
        ax.set_ylim(0, max(values) * 1.1 if values else 1)
        ax.grid(True, alpha=0.3, axis='y')
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                   f'{value:.3f}', ha='center', va='bottom')
    
    def export_analysis_results(self, filename: str) -> str:
        """
        Export analysis results to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "graph_info": self._compute_graph_info(),
            "centrality_data": self.centrality_data,
            "community_data": self.community_data,
            "path_data": self.path_data,
            "efficiency_data": self.efficiency_data,
            "clustering_data": self.clustering_data
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_network_analyzer(config_dict: Dict[str, Any] = None) -> NetworkAnalyzer:
    """
    Factory function to create NetworkAnalyzer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured NetworkAnalyzer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = NetworkAnalyzerConfig(**config_dict)
    return NetworkAnalyzer(config)