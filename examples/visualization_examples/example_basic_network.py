#!/usr/bin/env python3
"""
Example: Basic Neural Network Visualization

This example demonstrates how to create a basic neural network visualization
using the NLM visualization system.
"""

import sys
import os
import json
import matplotlib.pyplot as plt
import networkx as nx
import numpy as np
from pathlib import Path

# Add parent directory to path to import NLM modules
sys.path.append(str(Path(__file__).parent.parent.parent / "src"))

def create_basic_network():
    """Create a sample neural network graph."""
    # Create a simple scale-free network (Barabasi-Albert model)
    G = nx.barabasi_albert_graph(n=100, m=3)
    
    # Assign neuron types
    for node in G.nodes():
        # Neuron types: excitatory (0), inhibitory (1), special (2)
        if node % 10 == 0:
            G.nodes[node]['type'] = 'special'
        elif node % 3 == 0:
            G.nodes[node]['type'] = 'inhibitory'
        else:
            G.nodes[node]['type'] = 'excitatory'
    
    # Assign regions
    regions = ['cortex', 'hippocampus', 'amygdala', 'thalamus']
    for node in G.nodes():
        G.nodes[node]['region'] = np.random.choice(regions)
    
    # Assign synaptic weights
    for u, v in G.edges():
        G.edges[u, v]['weight'] = np.random.uniform(0.1, 1.0)
        G.edges[u, v]['delay'] = np.random.uniform(1, 20)
        G.edges[u, v]['is_excitatory'] = np.random.choice([True, False], p=[0.8, 0.2])
    
    return G

def visualize_network_basic(G, output_path):
    """Create a basic network visualization."""
    
    # Set up the figure
    plt.figure(figsize=(12, 10))
    
    # Get node positions using a force-directed layout
    pos = nx.spring_layout(G, seed=42)
    
    # Draw nodes by type
    neuron_types = {'excitatory': 'skyblue', 'inhibitory': 'red', 'special': 'gold'}
    
    for neuron_type, color in neuron_types.items():
        nodes = [node for node in G.nodes() if G.nodes[node]['type'] == neuron_type]
        nx.draw_networkx_nodes(G, pos, nodelist=nodes, 
                              node_color=color, node_size=100, 
                              label=neuron_type)
    
    # Draw edges
    nx.draw_networkx_edges(G, pos, alpha=0.5, edge_color='gray', width=0.5)
    
    # Add labels for special neurons
    special_nodes = [node for node in G.nodes() if G.nodes[node]['type'] == 'special']
    nx.draw_networkx_labels(G, pos, nodelist=special_nodes, 
                           font_size=8, font_weight='bold')
    
    # Add region boundaries
    region_colors = {'cortex': '#87CEEB', 'hippocampus': '#98FB98', 
                     'amygdala': '#FFB6C1', 'thalamus': '#DDA0DD'}
    
    for region, color in region_colors.items():
        nodes = [node for node in G.nodes() if G.nodes[node]['region'] == region]
        if nodes:
            # Draw region polygon (simplified)
            region_pos = [pos[node] for node in nodes]
            region_poly = plt.Polygon(region_pos, closed=True, 
                                     facecolor=region_colors[region], 
                                     alpha=0.1)
            plt.gca().add_patch(region_poly)
    
    # Title and labels
    plt.title('Neural Network Structure', fontsize=16, fontweight='bold')
    plt.xlabel('Network Topology')
    plt.ylabel('Functional Organization')
    
    # Add legend
    plt.legend(scatter=[],
              labels=list(neuron_types.keys()),
              loc='upper right',
              frameon=True,
              fancybox=True,
              shadow=True)
    
    # Save the figure
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()

def analyze_network_properties(G):
    """Analyze network properties for visualization."""
    analysis = {}
    
    # Basic network statistics
    analysis['nodes'] = G.number_of_nodes()
    analysis['edges'] = G.number_of_edges()
    analysis['density'] = nx.density(G)
    analysis['average_clustering'] = nx.average_clustering(G)
    analysis['average_degree'] = np.mean([d for n, d in G.degree()])
    analysis['degree_std'] = np.std([d for n, d in G.degree()])
    
    # Community detection
    communities = list(nx.algorithms.community.louvain_communities(G))
    analysis['num_communities'] = len(communities)
    analysis['community_sizes'] = [len(c) for c in communities]
    
    # Modularity
    analysis['modularity'] = nx.algorithms.community.modularity(G, communities)
    
    # Hubs and isolates
    analysis['hubs'] = len([n for n, d in G.degree() if d > np.percentile([d for n, d in G.degree()], 90)])
    analysis['isolates'] = len([n for n, d in G.degree() if d == 0])
    
    # Region distribution
    region_counts = {}
    for node in G.nodes():
        region = G.nodes[node]['region']
        region_counts[region] = region_counts.get(region, 0) + 1
    analysis['region_distribution'] = region_counts
    
    # Neuron type distribution
    type_counts = {}
    for node in G.nodes():
        neuron_type = G.nodes[node]['type']
        type_counts[neuron_type] = type_counts.get(neuron_type, 0) + 1
    analysis['neuron_type_distribution'] = type_counts
    
    # Synaptic properties
    excitatory_edges = sum(1 for u, v in G.edges() if G.edges[u, v]['is_excitatory'])
    inhibitory_edges = sum(1 for u, v in G.edges() if not G.edges[u, v]['is_excitatory'])
    analysis['excitatory_synapses'] = excitatory_edges
    analysis['inhibitory_synapses'] = inhibitory_edges
    analysis['excitatory_ratio'] = excitatory_edges / (excitatory_edges + inhibitory_edges)
    
    # Weight statistics
    weights = [G.edges[u, v]['weight'] for u, v in G.edges()]
    analysis['min_weight'] = np.min(weights)
    analysis['max_weight'] = np.max(weights)
    analysis['avg_weight'] = np.mean(weights)
    analysis['weight_std'] = np.std(weights)
    
    return analysis

def save_analysis(analysis, output_path):
    """Save network analysis to JSON file."""
    with open(output_path, 'w') as f:
        json.dump(analysis, f, indent=2)

def create_animation_script(G, output_path):
    """Create a Python script for creating animations."""
    script = '''#!/usr/bin/env python3
"""
Neural Network Animation Script

This script creates animations of neural network dynamics.
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import networkx as nx
import numpy as np
import json
from pathlib import Path
import sys

def create_network_snapshot(G, time_step, output_dir):
    """Create a snapshot of the network at a given time step."""
    plt.figure(figsize=(10, 8))
    
    # Get node positions (keep same for animation continuity)
    pos = nx.spring_layout(G, seed=42)
    
    # Draw network
    nx.draw_networkx_edges(G, pos, alpha=0.3, edge_color='gray', width=0.5)
    
    # Color nodes by activity level (simulated)
    node_colors = []
    for node in G.nodes():
        # Simulate activity based on time step
        activity = (time_step + node) % 10 / 10.0
        node_colors.append(activity)
    
    nodes = nx.draw_networkx_nodes(G, pos, node_color=node_colors, 
                                 cmap=plt.cm.viridis, node_size=100,
                                 vmin=0, vmax=1)
    
    plt.title(f"Neural Network Dynamics - Time Step {time_step}")
    plt.colorbar(nodes, label="Activity Level")
    
    # Save frame
    frame_path = output_dir / f"frame_{time_step:04d}.png"
    plt.savefig(frame_path, dpi=150, bbox_inches='tight')
    plt.close()
    
    return frame_path

def main():
    # Load network data
    network_file = Path(__file__).parent / "network_data.json"
    if not network_file.exists():
        print("Network data file not found. Run the basic visualization example first.")
        return
    
    with open(network_file, 'r') as f:
        network_data = json.load(f)
    
    # Reconstruct network
    G = nx.Graph()
    for node in network_data['nodes']:
        G.add_node(node['id'], **node['attributes'])
    
    for edge in network_data['edges']:
        G.add_edge(edge['source'], edge['target'], **edge['attributes'])
    
    # Create output directory
    output_dir = Path(__file__).parent / "animation_frames"
    output_dir.mkdir(exist_ok=True)
    
    # Create animation frames
    num_frames = 30
    for i in range(num_frames):
        create_network_snapshot(G, i, output_dir)
    
    print(f"Created {num_frames} animation frames in {output_dir}")
    
    # Create animation using ffmpeg (if available)
    try:
        import matplotlib.animation as anim
        frames = [output_dir / f"frame_{i:04d}.png" for i in range(num_frames)]
        
        # Create animation
        fig, ax = plt.subplots(figsize=(10, 8))
        
        def update(frame):
            ax.clear()
            pos = nx.spring_layout(G, seed=42)
            nx.draw_networkx_edges(G, pos, ax=ax, alpha=0.3, edge_color='gray', width=0.5)
            nx.draw_networkx_nodes(G, pos, ax=ax, node_color='lightblue', node_size=100)
            ax.set_title(f"Neural Network Dynamics - Time Step {frame}")
        
        anim = anim.FuncAnimation(fig, update, frames=num_frames, interval=100, blit=False)
        
        # Save animation
        anim.save(output_dir / "neural_network_animation.gif", 
                 writer='pillow', fps=10, dpi=100)
        
        plt.close()
        print("Animation saved as neural_network_animation.gif")
        
    except ImportError:
        print("ffmpeg not available. Video animation skipped.")

if __name__ == "__main__":
    main()
'''
    
    # Write script to file
    script_path = output_path / "create_animation.py"
    with open(script_path, 'w') as f:
        f.write(script)
    
    # Make script executable
    os.chmod(script_path, 0o755)
    
    return script_path

def create_report_script(G, analysis, output_path):
    """Create a script for generating comprehensive reports."""
    script = '''#!/usr/bin/env python3
"""
Neural Network Analysis Report Script

This script generates comprehensive analysis reports for neural networks.
"""

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np
import json
from pathlib import Path
import sys

def create_property_distribution_plots(G, output_dir):
    """Create plots for property distributions."""
    
    # Set up the figure
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    # 1. Degree distribution
    degrees = [d for n, d in G.degree()]
    sns.histplot(degrees, ax=axes[0], kde=True)
    axes[0].set_title('Node Degree Distribution')
    axes[0].set_xlabel('Degree')
    axes[0].set_ylabel('Frequency')
    
    # 2. Community size distribution
    communities = list(nx.algorithms.community.louvain_communities(G))
    community_sizes = [len(c) for c in communities]
    sns.histplot(community_sizes, ax=axes[1], kde=True)
    axes[1].set_title('Community Size Distribution')
    axes[1].set_xlabel('Community Size')
    axes[1].set_ylabel('Frequency')
    
    # 3. Synaptic weight distribution
    weights = [G.edges[u, v]['weight'] for u, v in G.edges()]
    sns.histplot(weights, ax=axes[2], kde=True)
    axes[2].set_title('Synaptic Weight Distribution')
    axes[2].set_xlabel('Weight')
    axes[2].set_ylabel('Frequency')
    
    # 4. Region distribution
    region_counts = {}
    for node in G.nodes():
        region = G.nodes[node]['region']
        region_counts[region] = region_counts.get(region, 0) + 1
    
    df_regions = pd.DataFrame(list(region_counts.items()), 
                              columns=['Region', 'Count'])
    sns.barplot(data=df_regions, x='Region', y='Count', ax=axes[3])
    axes[3].set_title('Region Distribution')
    axes[3].set_xlabel('Region')
    axes[3].set_ylabel('Count')
    
    # 5. Neuron type distribution
    type_counts = {}
    for node in G.nodes():
        neuron_type = G.nodes[node]['type']
        type_counts[neuron_type] = type_counts.get(neuron_type, 0) + 1
    
    df_types = pd.DataFrame(list(type_counts.items()), 
                           columns=['Type', 'Count'])
    sns.barplot(data=df_types, x='Type', y='Count', ax=axes[4])
    axes[4].set_title('Neuron Type Distribution')
    axes[4].set_xlabel('Neuron Type')
    axes[4].set_ylabel('Count')
    
    # 6. Excitatory vs Inhibitory synapses
    exc_synapses = sum(1 for u, v in G.edges() if G.edges[u, v]['is_excitatory'])
    inh_synapses = sum(1 for u, v in G.edges() if not G.edges[u, v]['is_excitatory'])
    
    axes[5].pie([exc_synapses, inh_synapses], labels=['Excitatory', 'Inhibitory'],
               autopct='%1.1f%%', colors=['skyblue', 'red'])
    axes[5].set_title('Synaptic Type Distribution')
    
    plt.tight_layout()
    plot_path = output_dir / "property_distributions.png"
    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
    plt.close()
    
    return plot_path

def create_analysis_report(analysis, output_path):
    """Create a comprehensive analysis report."""
    report = f"""
# Neural Network Analysis Report

## Network Overview
- **Nodes**: {analysis['nodes']}
- **Edges**: {analysis['edges']}
- **Network Density**: {analysis['density']:.4f}
- **Average Clustering Coefficient**: {analysis['average_clustering']:.4f}
- **Average Degree**: {analysis['average_degree']:.2f} ± {analysis['degree_std']:.2f}

## Network Properties
- **Number of Communities**: {analysis['num_communities']}
- **Modularity**: {analysis['modularity']:.4f}
- **Hubs**: {analysis['hubs']} ({analysis['hubs']/analysis['nodes']*100:.1f}%)
- **Isolates**: {analysis['isolates']}

## Synaptic Properties
- **Excitatory Synapses**: {analysis['excitatory_synapses']}
- **Inhibitory Synapses**: {analysis['inhibitory_synapses']}
- **Excitatory Ratio**: {analysis['excitatory_ratio']:.4f}
- **Weight Range**: {analysis['min_weight']:.3f} - {analysis['max_weight']:.3f}
- **Average Weight**: {analysis['avg_weight']:.3f} ± {analysis['weight_std']:.3f}

## Regional Distribution
"""
    for region, count in analysis['region_distribution'].items():
        percentage = count / analysis['nodes'] * 100
        report += f"- **{region}**: {count} neurons ({percentage:.1f}%)\n"
    
    report += "\n## Neuron Type Distribution\n"
    for neuron_type, count in analysis['neuron_type_distribution'].items():
        percentage = count / analysis['nodes'] * 100
        report += f"- **{neuron_type}**: {count} neurons ({percentage:.1f}%)\n"
    
    report += """
## Network Classification
- **Network Type**: Scale-free (Barabasi-Albert model)
- **Modeling Approach**: Synthetic neural network model
- **Analysis Purpose**: Visualization and analysis framework demonstration
- **Generated**: Automated analysis

## Recommendations
1. **Visualization**: Use force-directed layouts for interactive exploration
2. **Community Analysis**: Identify functional modules and network communities
3. **Regional Analysis**: Analyze brain region contributions and interactions
4. **Synaptic Dynamics**: Study synaptic strength and plasticity patterns
5. **Activity Patterns**: Investigate firing rate distributions and dynamics

## Conclusion
This network demonstrates the application of NLM visualization tools for analyzing neural network structure and function. The analysis provides insights into network topology, community structure, and functional organization.
"""
    
    with open(output_path, 'w') as f:
        f.write(report)
    
    return output_path

def main():
    # Load network data
    network_file = Path(__file__).parent / "network_data.json"
    if not network_file.exists():
        print("Network data file not found. Run the basic visualization example first.")
        return
    
    with open(network_file, 'r') as f:
        network_data = json.load(f)
    
    # Reconstruct network
    G = nx.Graph()
    for node in network_data['nodes']:
        G.add_node(node['id'], **node['attributes'])
    
    for edge in network_data['edges']:
        G.add_edge(edge['source'], edge['target'], **edge['attributes'])
    
    # Create output directory
    output_dir = Path(__file__).parent
    output_dir.mkdir(exist_ok=True)
    
    # Analyze network
    print("Analyzing network properties...")
    analysis = analyze_network_properties(G)
    
    # Save analysis
    analysis_path = output_dir / "network_analysis.json"
    with open(analysis_path, 'w') as f:
        json.dump(analysis, f, indent=2)
    print(f"Analysis saved to: {analysis_path}")
    
    # Create property distribution plots
    print("Creating property distribution plots...")
    plot_path = create_property_distribution_plots(G, output_dir)
    print(f"Plots saved to: {plot_path}")
    
    # Create analysis report
    print("Creating analysis report...")
    report_path = output_dir / "network_analysis_report.md"
    create_analysis_report(analysis, report_path)
    print(f"Report saved to: {report_path}")
    
    print("Analysis complete!")

if __name__ == "__main__":
    main()
'''
    
    # Write script to file
    script_path = output_path / "analyze_network.py"
    with open(script_path, 'w') as f:
        f.write(script)
    
    # Make script executable
    os.chmod(script_path, 0o755)
    
    return script_path

def main():
    """Main function."""
    print("Creating NLM visualization examples...")
    
    # Create output directory
    output_dir = Path(__file__).parent
    examples_dir = output_dir / "visualization_examples"
    examples_dir.mkdir(exist_ok=True)
    
    # Step 1: Create basic network
    print("1. Creating basic neural network...")
    G = create_basic_network()
    
    # Save network data
    network_data = {
        'nodes': [],
        'edges': []
    }
    
    for node in G.nodes():
        network_data['nodes'].append({
            'id': node,
            'attributes': dict(G.nodes[node])
        })
    
    for u, v in G.edges():
        network_data['edges'].append({
            'source': u,
            'target': v,
            'attributes': dict(G.edges[u, v])
        })
    
    network_file = examples_dir / "network_data.json"
    with open(network_file, 'w') as f:
        json.dump(network_data, f, indent=2)
    
    print(f"   Network data saved to: {network_file}")
    
    # Step 2: Create basic visualization
    print("2. Creating basic visualization...")
    visualize_path = examples_dir / "network_visualization.png"
    visualize_network_basic(G, visualize_path)
    print(f"   Visualization saved to: {visualize_path}")
    
    # Step 3: Analyze network
    print("3. Analyzing network properties...")
    analysis = analyze_network_properties(G)
    
    analysis_path = examples_dir / "network_analysis.json"
    save_analysis(analysis, analysis_path)
    print(f"   Analysis saved to: {analysis_path}")
    
    # Step 4: Create animation script
    print("4. Creating animation script...")
    animation_script = create_animation_script(G, examples_dir)
    print(f"   Animation script created: {animation_script}")
    
    # Step 5: Create analysis script
    print("5. Creating analysis script...")
    analysis_script = create_report_script(G, analysis, examples_dir)
    print(f"   Analysis script created: {analysis_script}")
    
    # Create README file
    readme_path = examples_dir / "README.md"
    with open(readme_path, 'w') as f:
        f.write(f'''# NLM Visualization Examples

This directory contains example scripts for using the NLM visualization system.

## Files

1. **network_data.json**: Generated neural network data
2. **network_visualization.png**: Basic network visualization
3. **network_analysis.json**: Network property analysis
4. **create_animation.py**: Script for creating animations
5. **analyze_network.py**: Script for comprehensive analysis

## Running Examples

### 1. Basic Visualization
```bash
# The network_visualization.png is already created
# This shows the neural network structure with communities and regions
```

### 2. Analysis
```bash
# Run the analysis script
cd visualization_examples
python analyze_network.py
```

This will:
- Load the network data
- Analyze network properties (degree distribution, communities, etc.)
- Create property distribution plots
- Generate a comprehensive analysis report

### 3. Animation
```bash
# Run the animation script
cd visualization_examples
python create_animation.py
```

This will create an animated visualization of network dynamics.

## Requirements

- Python 3.6+
- Matplotlib
- NetworkX
- NumPy
- Pandas
- Seaborn

## Installation

```bash
pip install matplotlib networkx numpy pandas seaborn
```

## Usage

1. **Load the network data**:
   ```python
   import json
   from pathlib import Path
   
   network_file = Path(__file__).parent / "visualization_examples" / "network_data.json"
   with open(network_file, 'r') as f:
       network_data = json.load(f)
   ```

2. **Reconstruct the network**:
   ```python
   import networkx as nx
   G = nx.Graph()
   
   for node in network_data['nodes']:
       G.add_node(node['id'], **node['attributes'])
   
   for edge in network_data['edges']:
       G.add_edge(edge['source'], edge['target'], **edge['attributes'])
   ```

3. **Visualize the network**:
   ```python
   import matplotlib.pyplot as plt
   import networkx as nx
   
   pos = nx.spring_layout(G, seed=42)
   nx.draw_networkx(G, pos, with_labels=True)
   plt.show()
   ```

## Technical Details

This example demonstrates the use of the NLM visualization system for:

- **Network Structure**: Scale-free networks with community structure
- **Neuron Types**: Excitatory, inhibitory, and special neurons
- **Brain Regions**: Anatomical and functional organization
- **Synaptic Properties**: Weight, delay, and excitation/inhibition
- **Visualization**: Force-directed layouts with interactive capabilities
- **Analysis**: Topological and statistical analysis

The example provides a foundation for more complex visualizations and analyses using the NLM framework.
''')
    
    print(f"\nAll examples created successfully in: {examples_dir}")
    print("\nNext steps:")
    print("1. Install required Python packages")
    print("2. Run the analysis script: python analyze_network.py")
    print("3. Run the animation script: python create_animation.py")
    print("4. Explore the generated visualizations and reports")
    
    return examples_dir

if __name__ == "__main__":
    main()
