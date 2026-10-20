#include "debugging/DebugVisualizer.hpp"

namespace nlm {
namespace debug {

void DebugVisualizer::generateVisualizationScript(const std::string& scriptFile) {
    std::ofstream file(scriptFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create visualization script: {}", scriptFile);
        return;
    }
    
    file << "#!/usr/bin/env python3\n";
    file << """
import numpy as np
import matplotlib.pyplot as plt
import json
import sys
from datetime import datetime

# Load NLM debug data
def load_debug_data(filename):
    data = []
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if line:
                try:
                    data.append(json.loads(line))
                except json.JSONDecodeError as e:
                    print(f"Warning: Could not parse line: {line}")
                    continue
    return data

# Create comprehensive visualization
def create_comprehensive_visualization(data_file):
    data = load_debug_data(data_file)
    
    if not data:
        print("No data to visualize")
        return
    
    print(f"Loaded {len(data)} data points")
    
    # Setup figure with multiple subplots
    fig, axes = plt.subplots(3, 3, figsize=(15, 12))
    fig.suptitle('NLM Neural Network Comprehensive Visualization', fontsize=16, fontweight='bold')
    
    # Time axis
    times = [d.get('timestamp', 0) for d in data]
    
    # 1. Neuron state evolution (first 5 neurons)
    ax1 = axes[0, 0]
    if data and 'neuronStates' in data[0]:
        for i in range(min(5, len(data[0]['neuronStates']))):
            neuron_traces = [d['neuronStates'][i] if len(d['neuronStates']) > i else 0 
                           for d in data]
            ax1.plot(times, neuron_traces, label=f'Neuron {i}', linewidth=2, alpha=0.8)
        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Membrane Potential (mV)')
        ax1.set_title('Neuron State Evolution')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
    
    # 2. Synaptic weight dynamics
    ax2 = axes[0, 1]
    if data and 'synapticWeights' in data[0]:
        weights = [d['synapticWeights'][0] if d['synapticWeights'] else 0 
                   for d in data]
        ax2.plot(times, weights, 'r-', linewidth=2, alpha=0.8)
        ax2.set_xlabel('Time (s)')
        ax2.set_ylabel('Synaptic Weight')
        ax2.set_title('Synaptic Weight Dynamics')
        ax2.grid(True, alpha=0.3)
    
    # 3. Spike activity heatmap
    ax3 = axes[0, 2]
    if data and 'spikeCounts' in data[0]:
        # Create heatmap of spike activity over time windows
        spike_data = [d['spikeCounts'][0] if d['spikeCounts'] else 0 for d in data]
        
        # Create 2D histogram
        if len(times) > 10:
            hist, xedges, yedges = np.histogram2d(times, spike_data, bins=[20, 20])
            im = ax3.imshow(hist.T, origin='lower', extent=[xedges[0], xedges[-1], 
                                                             yedges[0], yedges[-1]],
                          aspect='auto', cmap='hot', alpha=0.7)
            plt.colorbar(im, ax=ax3, label='Spike Count Density')
        
        ax3.set_xlabel('Time (s)')
        ax3.set_ylabel('Spike Count')
        ax3.set_title('Spike Activity Heatmap')
    
    # 4. Network statistics over time
    ax4 = axes[1, 0]
    if data and 'metadata' in data[0]:
        # Extract network statistics from metadata
        neuron_counts = [d['metadata'].get('neuronCount', 0) if 'metadata' in d else 0 
                        for d in data]
        synapse_counts = [d['metadata'].get('synapseCount', 0) if 'metadata' in d else 0 
                         for d in data]
        
        ax4.plot(times, neuron_counts, 'g-', label='Neurons', linewidth=2)
        ax4.plot(times, synapse_counts, 'b-', label='Synapses', linewidth=2)
        ax4.set_xlabel('Time (s)')
        ax4.set_ylabel('Count')
        ax4.set_title('Network Growth')
        ax4.legend()
        ax4.grid(True, alpha=0.3)
    
    # 5. Performance metrics
    ax5 = axes[1, 1]
    if data and 'metadata' in data[0]:
        # Performance metrics over time
        firing_rates = [d['metadata'].get('firingRate', 0) if 'metadata' in d else 0 
                       for d in data]
        
        ax5.plot(times, firing_rates, 'purple', linewidth=2, alpha=0.8)
        ax5.set_xlabel('Time (s)')
        ax5.set_ylabel('Firing Rate (Hz)')
        ax5.set_title('Network Performance')
        ax5.grid(True, alpha=0.3)
        
        # Add horizontal line for average
        if firing_rates:
            avg_firing = np.mean(firing_rates)
            ax5.axhline(y=avg_firing, color='orange', linestyle='--', alpha=0.7, label=f'Average: {avg_firing:.2f} Hz')
            ax5.legend()
    
    # 6. Network topology visualization (static)
    ax6 = axes[1, 2]
    ax6.set_aspect('equal')
    
    # Create a simple network graph visualization
    # This would require networkx in production
    ax6.scatter([], [], s=100, c='red', label='Excitatory Neurons', alpha=0.7)
    ax6.scatter([], [], s=100, c='blue', label='Inhibitory Neurons', alpha=0.7)
    ax6.scatter([], [], s=50, c='green', label='Synapses', alpha=0.5)
    
    ax6.set_xlim(-1, 1)
    ax6.set_ylim(-1, 1)
    ax6.set_xlabel('X Position')
    ax6.set_ylabel('Y Position')
    ax6.set_title('Network Topology (Schematic)')
    ax6.legend()
    ax6.grid(True, alpha=0.3)
    
    # 7. Memory usage over time
    ax7 = axes[2, 0]
    if data and 'metadata' in data[0]:
        memory_usage = [d['metadata'].get('memoryUsage', 0) if 'metadata' in d else 0 
                       for d in data]
        
        ax7.plot(times, memory_usage, 'cyan', linewidth=2, alpha=0.8)
        ax7.set_xlabel('Time (s)')
        ax7.set_ylabel('Memory Usage (MB)')
        ax7.set_title('Memory Usage Over Time')
        ax7.grid(True, alpha=0.3)
        
        # Add memory limit line
        if memory_usage:
            max_memory = max(memory_usage) if memory_usage else 1
            ax7.axhline(y=max_memory * 0.9, color='red', linestyle='--', alpha=0.7, label='90% Memory Limit')
            ax7.legend()
    
    # 8. Learning curve (weight change)
    ax8 = axes[2, 1]
    if data and 'metadata' in data[0]:
        weight_changes = [d['metadata'].get('weightChange', 0) if 'metadata' in d else 0 
                         for d in data]
        
        ax8.plot(times, weight_changes, 'orange', linewidth=2, alpha=0.8)
        ax8.set_xlabel('Time (s)')
        ax8.set_ylabel('Weight Change')
        ax8.set_title('Learning Curve (Synaptic Plasticity)')
        ax8.grid(True, alpha=0.3)
        
        # Mark learning phases
        if any(w != 0 for w in weight_changes):
            ax8.axhline(y=0, color='black', linestyle='-', alpha=0.5)
            ax8.fill_between(times, 0, weight_changes, alpha=0.3, color='green', label='Learning Phase')
            ax8.legend()
    
    # 9. Summary statistics
    ax9 = axes[2, 2]
    ax9.axis('off')
    
    # Create summary text
    summary_text = f"""NLM Network Analysis Summary
Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

Data Points: {len(data)}
Time Range: {min(times):.2f} - {max(times):.2f} seconds

Network Statistics:
• Total Neurons: {data[0]['metadata'].get('neuronCount', 0) if data and 'metadata' in data[0] else 'N/A'}
• Total Synapses: {data[0]['metadata'].get('synapseCount', 0) if data and 'metadata' in data[0] else 'N/A'}
• Final Firing Rate: {data[-1]['metadata'].get('firingRate', 0) if data and 'metadata' in data[-1] else 'N/A'} Hz
• Peak Memory: {max([d['metadata'].get('memoryUsage', 0) if 'metadata' in d else 0 for d in data]):.2f} MB

Learning Metrics:
• Total Weight Changes: {sum(abs(d['metadata'].get('weightChange', 0) if 'metadata' in d else 0) for d in data):.2f}
• Average Weight Change: {np.mean([d['metadata'].get('weightChange', 0) if 'metadata' in d else 0 for d in data]):.2f}

Performance:
• Average Steps/Second: {len(data) / (max(times) - min(times) if max(times) > min(times) else 1):.2f}
• Network Efficiency: {data[0]['metadata'].get('efficiency', 0) if data and 'metadata' in data[0] else 'N/A'}
""";
    
    ax9.text(0.1, 0.95, summary_text, fontsize=10, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8),
             fontfamily='monospace')
    
    plt.tight_layout()
    
    # Save high-quality figure
    output_file = 'nlm_comprehensive_visualization.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Visualization saved to: {output_file}")
    
    # Save interactive version
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) > 1:
        create_comprehensive_visualization(sys.argv[1])
    else:
        print("Usage: python3 visualize.py <data_file>")
        print("Example: python3 visualize.py nlm_debug_data.json")
        print("The data file should contain JSON objects with fields:")
        print("  - timestamp")
        print("  - neuronStates (array)")
        print("  - synapticWeights (array)")
        print("  - spikeCounts (array)")
        print("  - metadata (object with additional stats)")
""";
        
        NLM_LOG_INFO("Comprehensive visualization script generated: {}", scriptFile);
    }
};

} // namespace debug
} // namespace nlm