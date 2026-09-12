#!/usr/bin/env python3
"""
NLM Visualize Tool - Visualize Brain States and Neural Activity

Create visual representations of brain states, neural activity, and experiment results.
"""

import argparse
import sys
import os
from pathlib import Path
import json
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, Any, List, Optional

# Mock import for NLM
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Visualization will use mock data.")

VISUALIZATION_TYPES = {
    'ascii': 'ASCII Text-Based Visualization',
    'graph': 'Graph/Graphviz Format',
    'json': 'JSON Structured Output'
}

def create_ascii_neural_network(neuron_count: int, firing_count: int, 
                              avg_firing_rate: float) -> str:
    """Create ASCII visualization of neural network activity."""
    lines = []
    lines.append("=== NLM Neural Network Activity ===")
    lines.append(f"Neurons: {neuron_count:,}")
    lines.append(f"Active Neurons: {firing_count:,} ({firing_count/neuron_count*100:.1f}%)")
    lines.append(f"Average Firing Rate: {avg_firing_rate:.2f} Hz")
    lines.append("")
    
    # Create a simple ASCII representation
    active_percentage = firing_count / neuron_count if neuron_count > 0 else 0
    
    lines.append("Activity Pattern:")
    grid_size = 20
    active_cells = int(grid_size * grid_size * active_percentage)
    
    # Create activity heatmap
    for row in range(grid_size):
        line = ""
        for col in range(grid_size):
            cell_index = row * grid_size + col
            if cell_index < active_cells:
                line += "■ "
            else:
                line += "□ "
        lines.append(line)
    
    lines.append("")
    lines.append("Legend: ■ = Active, □ = Inactive")
    return "\n".join(lines)

def create_graphviz_network(neuron_count: int, regions: List[str]) -> str:
    """Create Graphviz DOT format representation."""
    dot = []
    dot.append("digraph NLM_Brain {")
    dot.append("  rankdir=TB;")
    dot.append("  node [shape=box, style=filled];")
    dot.append("  ")
    
    # Define regions
    for i, region in enumerate(regions):
        color = f"lightblue{i % 4 + 1}"
        dot.append(f"  region{i} [label=\"{region}\", fillcolor={color}];")
    
    # Create connections
    for i in range(min(len(regions) - 1, 4)):
        dot.append(f"  region{i} -> region{i+1} [label=\"{10 + i * 5} synapses\"];")
    
    dot.append("}")
    return "\n".join(dot)

def analyze_checkpoint_file(filepath: str) -> Dict[str, Any]:
    """Analyze checkpoint file and extract statistics for visualization."""
    stats = {
        'type': 'checkpoint',
        'size': os.path.getsize(filepath),
        'neuron_count': 0,
        'synapse_count': 0,
        'regions': ['Sensory', 'Internal', 'Motor', 'Cognitive'],
        'activity_levels': {'sensory': 0.3, 'internal': 0.7, 'motor': 0.4, 'cognitive': 0.5},
        'timestamp': time.time()
    }
    
    try:
        # Try to parse as binary checkpoint
        with open(filepath, 'rb') as f:
            data = f.read(1024)
            if len(data) >= 4:
                # Look for magic number
                if data[:4] == b'NLM1' or data[:4] == b'NLM2':
                    stats['format_version'] = 'nlm_binary_v1'
                else:
                    stats['format_version'] = 'unknown_binary'
    except Exception as e:
        stats['error'] = str(e)
    
    return stats

def visualize_brain_state(state: Dict[str, Any], output_type: str, 
                         output_file: Optional[str] = None) -> None:
    """Create visualization of brain state."""
    if output_type == 'ascii':
        # ASCII visualization
        if 'neuron_count' in state and 'firing_count' in state:
            ascii_viz = create_ascii_neural_network(
                state['neuron_count'],
                state['firing_count'],
                state.get('avg_firing_rate', 0.0)
            )
            print(ascii_viz)
        
    elif output_type == 'graph':
        # Graphviz visualization
        regions = state.get('regions', ['Sensory', 'Internal', 'Motor'])
        dot_graph = create_graphviz_network(len(regions), regions)
        print("=== Graphviz DOT Format ===")
        print(dot_graph)
        print("\nTo render this graph, use:")
        print("  dot -Tpng graph.dot -o brain.png")
        print("  dot -Tsvg graph.dot -o brain.svg")
        
    elif output_type == 'json':
        # Structured JSON output
        print("=== Brain State Analysis ===")
        print(json.dumps(state, indent=2, default=str))
    
    # Save to file if requested
    if output_file:
        try:
            with open(output_file, 'w') as f:
                if output_type == 'json':
                    json.dump(state, f, indent=2, default=str)
                else:
                    f.write(json.dumps(state, indent=2, default=str))
            print(f"\nVisualization saved to: {output_file}")
        except Exception as e:
            print(f"Error saving visualization: {e}")

def create_experiment_summary(experiment_results: Dict[str, Any], 
                              output_type: str) -> Dict[str, Any]:
    """Create summary visualization data from experiment results."""
    summary = {
        'type': 'experiment_summary',
        'timestamp': time.time(),
        'experiment_type': experiment_results.get('metadata', {}).get('experiment_type', 'unknown')
    }
    
    # Add key metrics based on experiment type
    if experiment_results['experiment_type'] == 'phase6':
        summary.update({
            'total_reward': experiment_results.get('total_reward', 0),
            'avg_firing_rate': experiment_results.get('avg_firing_rate', 0),
            'memory_episodes': experiment_results.get('memory_episodes', 0),
            'integration_score': experiment_results.get('integration_score', 0)
        })
    
    elif experiment_results['experiment_type'] == 'continual':
        summary.update({
            'performance_trajectory': experiment_results.get('performance_trajectory', []),
            'forgetting_rate': experiment_results.get('forgetting_rate', 0),
            'transfer_efficiency': experiment_results.get('transfer_efficiency', 0)
        })
    
    return summary

def main():
    parser = argparse.ArgumentParser(
        description="Visualize NLM brain states and neural activity",
        epilog="Examples:\n"
               "  nlm-visualize brain.bin --type ascii\n"
               "  nlm-visualize brain.bin --type graph --output graph.dot\n"
               "  nlm-visualize results.json --type json --output summary.json"
    )
    
    parser.add_argument('input', help='Path to brain file or experiment results (JSON)')
    parser.add_argument('--type', '-t', choices=list(VISUALIZATION_TYPES.keys()), 
                       default='ascii', help='Visualization type (default: ascii)')
    parser.add_argument('--output', '-o', help='Save visualization to file')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output with additional details')
    parser.add_argument('--summary', action='store_true',
                       help='Create experiment summary instead of raw state visualization')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.input):
        print(f"Error: Input file not found: {args.input}")
        sys.exit(1)
    
    # Load input data
    if args.input.endswith('.json'):
        # Try to load as JSON (could be checkpoint data or experiment results)
        try:
            with open(args.input, 'r') as f:
                data = json.load(f)
            
            if 'metadata' in data and 'experiment_type' in data.get('metadata', {}):
                # This looks like experiment results
                if args.summary:
                    summary = create_experiment_summary(data, args.type)
                    visualize_brain_state(summary, args.type, args.output)
                else:
                    visualize_brain_state(data, args.type, args.output)
            else:
                # This looks like checkpoint/analysis data
                visualize_brain_state(data, args.type, args.output)
                
        except json.JSONDecodeError:
            print(f"Error: Invalid JSON file: {args.input}")
            sys.exit(1)
        except Exception as e:
            print(f"Error loading input file: {e}")
            sys.exit(1)
    
    else:
        # Treat as binary checkpoint file
        state = analyze_checkpoint_file(args.input)
        visualize_brain_state(state, args.type, args.output)

if __name__ == '__main__':
    main()
