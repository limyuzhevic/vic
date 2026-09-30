# NLM Visualization and Analysis Examples

This directory contains examples demonstrating the visualization and analysis capabilities of the NLM (Neural Learning Machine) system.

## Overview

The visualization and analysis module provides comprehensive tools for:

1. **Neural Network Visualization** - Rendering neurons, synapses, and brain regions
2. **Spike Train Visualization** - Raster plots and frequency analysis
3. **Memory System Visualization** - Working memory, episodic memory, and associative memory
4. **Plasticity Visualization** - Synaptic weight changes and learning trajectories
5. **Network Connectivity Visualization** - Graph theory and community structure
6. **Statistical Analysis** - Neural activity statistics and time series analysis
7. **Pattern Analysis** - Pattern detection and sequence analysis
8. **Network Analysis** - Graph metrics and centrality measures
9. **Learning Analysis** - Learning efficiency and decision making analysis

## Usage Examples

### Basic Visualization

```bash
# Network visualization
./nlm --mode viz-network --viz-output brain_network.png

# Spike train visualization
./nlm --mode viz-spikes --viz-format csv

# Memory system visualization
./nlm --mode viz-memory

# Plasticity visualization
./nlm --mode viz-plasticity

# Weight matrix visualization
./nlm --mode viz-weights --viz-output weights.json
```

### Statistical Analysis

```bash
# Statistical analysis
./nlm --mode analyze-stats

# Pattern analysis with export
./nlm --mode analyze-patterns --viz-output pattern_data.json

# Network connectivity analysis
./nlm --mode analyze-network

# Learning efficiency analysis
./nlm --mode analyze-learning
```

### Combined Visualization and Analysis

```bash
# Combined visualization and analysis
./nlm --mode viz-network --mode analyze-stats --viz-output combined_results.json

# Export all visualization data
./nlm --viz --analyze --export-viz --viz-output all_data.json
```

## Python Integration

The NLM system includes Python bindings for advanced visualization and analysis:

### Installation

```bash
pip install pynlm[analysis,export]
pip install pynlm[realtime]
```

### Basic Usage

```python
import pynlm
from pynlm import visualization, analysis

# Load brain simulation
brain = pynlm.load_brain('brain_checkpoint.json')

# Create visualization
viz = visualization.NeuralVisualizer(brain)
viz.render_network()
viz.export_to_png('brain.png')

# Perform analysis
stats = analysis.StatisticalAnalysis(brain)
results = stats.compute_neural_statistics()

# Pattern detection
patterns = analysis.PatternAnalysis(brain)
detected = patterns.detect_patterns()
```

### Advanced Visualization

```python
# Real-time visualization
from pynlm.visualization import RealTimeVisualizer

viz = RealTimeVisualizer(brain, host='localhost', port=8080)
viz.start_streaming()
viz.render_network()
viz.render_spikes()
viz.render_memory()
```

### Analysis Examples

```python
# Network analysis
from pynlm.analysis import NetworkAnalysis

network = NetworkAnalysis(brain)
metrics = network.compute_graph_metrics()
communities = network.detect_communities()
centrality = network.compute_centrality_metrics()

# Learning analysis
from pynlm.analysis import LearningAnalysis

learning = LearningAnalysis(brain)
learning_metrics = learning.compute_learning_metrics()
phases = learning.identify_learning_phases()
consolidation = learning.compute_consolidation_metrics()
```

## API Reference

### Visualization Classes

#### NeuralVisualization
- `render_network(brain, scale=1.0)` - Render neural network structure
- `render_neurons(brain, colorScheme='activity')` - Render neurons with activity levels
- `render_synapses(brain, minWeight=0.0, maxWeight=1.0)` - Render synaptic connections
- `render_regions(brain, showRegionIds=False)` - Render neural regions
- `highlight_active_neurons(brain, neuronIds)` - Highlight specific neurons
- `export_to_png(filename, width=1920, height=1080)` - Export to PNG
- `export_to_json(filename, brain)` - Export to JSON
- `export_to_csv(filename, brain)` - Export to CSV

#### SpikeVisualization
- `render_spike_raster(brain, timeWindow=10.0, binWidth=0.001, normalize=True)` - Spike raster plot
- `render_spike_frequency(brain, timeWindow=60.0, binWidth=0.01)` - Spike frequency plot
- `render_population_activity(brain, type=NeuronType::Interneuron)` - Population activity
- `render_spike_times(spikes, title='Spike Times')` - Spike times for specific neurons
- `export_spike_data(filename, brain)` - Export spike data

#### MemoryVisualization
- `render_working_memory(brain, showDetails=True)` - Working memory contents
- `render_episodic_memory(brain, maxEntries=100)` - Episodic memory traces
- `render_associative_memory(brain, activationThreshold=0.5)` - Associative memory network
- `render_prediction_system(brain, showPredictions=True)` - Prediction system
- `render_cognitive_system(brain)` - Cognitive system state

#### PlasticityVisualization
- `render_weight_changes(brain, timeWindow=60.0, showSTDP=True, showHebbian=True)` - Synaptic weight changes
- `render_plasticity_activity(brain, rule='stdp')` - Plasticity rule activity
- `render_structural_plasticity(brain)` - Structural plasticity
- `render_learning_trajectories(brain, numTrajectories=10)` - Learning trajectories

#### NetworkVisualization
- `render_connectivity_matrix(brain, normalized=True)` - Connectivity matrix
- `render_small_world_metrics(brain)` - Small-world metrics
- `render_community_structure(brain)` - Community structure
- `render_hub_bottleneck_analysis(brain)` - Hub and bottleneck analysis
- `render_network_evolution(brain, historySteps=100)` - Network evolution
- `compute_network_metrics(brain)` - Compute network metrics

### Analysis Classes

#### StatisticalAnalysis
- `compute_neural_statistics(brain, window=1.0)` - Neural activity statistics
- `compute_population_statistics(brain)` - Population statistics
- `compute_time_series_analysis(timeSeries, samplingRate=1000.0)` - Time series analysis
- `compute_ks_statistic(data1, data2)` - Kolmogorov-Smirnov test
- `compute_pearson_correlation(data1, data2)` - Pearson correlation
- `compute_spearman_correlation(data1, data2)` - Spearman correlation
- `export_statistics(filename, stats)` - Export statistics
- `export_time_series_data(filename, stats)` - Export time series data

#### PatternAnalysis
- `detect_patterns(brain, window=1.0)` - Pattern detection
- `analyze_sequences(brain, sequenceLength=5)` - Sequence analysis
- `cluster_neurons(brain, numClusters=3)` - Neuron clustering
- `extract_features(brain)` - Feature extraction
- `match_pattern(features, patternType)` - Pattern matching
- `export_patterns(filename, patterns)` - Export patterns
- `export_features(filename, features)` - Export features

#### NetworkAnalysis
- `compute_graph_metrics(brain)` - Graph theory metrics
- `compute_centrality_metrics(brain)` - Centrality measures
- `detect_communities(brain)` - Community detection
- `compute_resilience_metrics(brain)` - Network resilience
- `export_graph_metrics(filename, metrics)` - Export graph metrics
- `export_centrality_metrics(filename, metrics)` - Export centrality metrics
- `export_community_metrics(filename, metrics)` - Export community metrics

#### LearningAnalysis
- `compute_learning_metrics(brain, learningWindow=60.0)` - Learning efficiency metrics
- `compute_consolidation_metrics(brain)` - Memory consolidation metrics
- `compute_decision_metrics(brain)` - Decision making metrics
- `identify_learning_phases(brain, threshold=0.1)` - Learning phase identification
- `export_learning_metrics(filename, metrics)` - Export learning metrics
- `export_consolidation_metrics(filename, metrics)` - Export consolidation metrics
- `export_decision_metrics(filename, metrics)` - Export decision metrics

### Command Line Interface

#### Visualization Commands
```
--viz, --visualize              Enable basic visualization
--viz-network                   Network visualization
--viz-spikes                    Spike train visualization
--viz-memory                    Memory system visualization
--viz-plasticity                Plasticity visualization
--viz-weights                   Weight matrix visualization
--export-viz                    Export visualization to file
--viz-output <file>             Visualization output file
--viz-format <format>           Visualization format (png, json, csv)
```

#### Analysis Commands
```
--analyze                        Enable basic analysis
--analyze-stats                  Statistical analysis
--analyze-patterns               Pattern analysis
--analyze-network                Network connectivity analysis
--analyze-learning               Learning efficiency analysis
```

#### Examples
```
./nlm --mode viz-network --viz-output brain_network.png
./nlm --mode viz-spikes --viz-format csv
./nlm --mode analyze-stats
./nlm --mode analyze-patterns --viz-output pattern_data.json
./nlm --viz --analyze --export-viz --viz-output all_data.json
```

## Performance Considerations

- **Efficient rendering for large networks**: Adaptive sampling and LOD (Level of Detail)
- **Streaming support for real-time visualization**: Compressed streaming with delta encoding
- **Multi-format export support**: Efficient serialization for each format
- **Memory-efficient data structures**: Custom allocators and pool-based memory management
- **GPU acceleration**: OpenGL/Vulkan integration for large-scale rendering

## Integration with Existing Systems

### Existing Monitoring Systems
- Compatible with NLM's existing logging system
- Integrates with Brain's status reporting
- Supports checkpoint-based visualization

### Python Bindings
- Full Python API for advanced visualization
- Real-time WebSocket support for live updates
- Integration with matplotlib, plotly, and other visualization libraries

### Export Capabilities
- JSON format for structured data
- CSV format for tabular data
- PNG format for raster images
- HTML format for interactive visualizations
- WebSocket support for real-time streaming

## File Structure

```
src/
├── visualization/
│   ├── NeuralVisualization.hpp
│   ├── NeuralVisualization.cpp
│   ├── SpikeVisualization.hpp
│   ├── SpikeVisualization.cpp
│   ├── MemoryVisualization.hpp
│   ├── MemoryVisualization.cpp
│   ├── PlasticityVisualization.hpp
│   ├── PlasticityVisualization.cpp
│   ├── NetworkVisualization.hpp
│   ├── NetworkVisualization.cpp
│   └── VisualizationInterface.hpp
├── analysis/
│   ├── StatisticalAnalysis.hpp
│   ├── StatisticalAnalysis.cpp
│   ├── PatternAnalysis.hpp
│   ├── PatternAnalysis.cpp
│   ├── NetworkAnalysis.hpp
│   ├── NetworkAnalysis.cpp
│   └── LearningAnalysis.hpp
│   └── LearningAnalysis.cpp
├── examples/
│   ├── basic_visualization.py
│   ├── analysis_examples.py
│   └── real_time_viz.py
└── tests/
    ├── visualization_tests.cpp
    ├── analysis_tests.cpp
    └── integration_tests.cpp
```

## Dependencies

### C++ Dependencies
- Standard C++20
- Optional: OpenGL/Vulkan for hardware-accelerated rendering
- Optional: CUDA for GPU-accelerated analysis

### Python Dependencies
- numpy >= 1.20
- matplotlib >= 3.5.0
- scipy >= 1.7.0
- pandas >= 1.3.0
- Optional: plotly >= 5.0.0
- Optional: graphviz >= 0.20
- Optional: networkx >= 3.0
- Optional: websockets >= 10.0
- Optional: pyyaml >= 6.0
- Optional: pillow >= 9.0.0

## License

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Acknowledgments

This visualization and analysis module is part of the NLM (Neural Learning Machine) project, an experimental artificial developmental brain system implementing real spiking neural computation.

Special thanks to the open-source community for providing the foundational libraries and frameworks used in this implementation.

## Changelog

### Version 0.1.0 (2026-09-30)
- Initial release of visualization and analysis module
- Basic neural network visualization
- Statistical analysis framework
- Pattern detection and analysis
- Network connectivity analysis
- Learning efficiency analysis
- Command line interface integration
- Python bindings with optional dependencies

### Future Plans
- GPU-accelerated rendering
- Advanced machine learning integration
- Enhanced real-time streaming capabilities
- Interactive visualization tools
- Performance monitoring and optimization
- Documentation and tutorials

## Support

For questions, issues, or feature requests, please visit:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Community: https://discord.gg/nlm

---
*This documentation was generated as part of the NLM visualization and analysis module implementation.*

