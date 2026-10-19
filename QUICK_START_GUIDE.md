# NLM Visualization and Monitoring - Quick Start Guide

## Overview
This guide provides a quick introduction to the enhanced visualization and monitoring capabilities for the NLM (Neural Learning Machine) framework.

## What Was Implemented

### Core Visualization System
- **Real-time neural network visualization** with 3D rendering
- **Memory system monitoring** and replay visualization
- **Neuromodulation tracking** for dopamine, curiosity, and novelty signals
- **Plasticity visualization** with STDP traces and learning curves
- **Development stage visualization** with maturation tracking
- **Prediction system visualization** with confidence intervals and decision trees
- **Performance monitoring** with resource usage tracking

### Python Integration
- **Real-time visualization APIs** for Python scripts
- **Monitoring dashboards** for system overview
- **Data export** to multiple formats (JSON, CSV, NumPy, etc.)
- **Web-based visualization interfaces** for browser access
- **Mobile visualization** support

### Development Tools
- **Custom visualization builders** with Python API
- **Real-time plotting and graphing utilities**
- **Analysis report generation**
- **Plugin system** for visualization extensions
- **Performance-optimized rendering pipeline**

## Getting Started

### 1. Install Dependencies
```bash
# Clone the NLM repository
cd /path/to/nlm

# Install the visualization package in development mode
pip install -e .

# Install additional Python dependencies (if needed)
pip install matplotlib plotly dash flask psutil
```

### 2. Basic Usage Examples

#### Basic Visualization
```python
import pynlm
import pynlm_viz

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 100)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create visualization system
viz = pynlm_viz.VisualizationSystem()

# Set up brain data
viz.setBrainData(brain)

# Start visualization
viz.startVisualization(30.0)  # 30 FPS

# Main loop
while viz.isRunning():
    # Update brain (in real simulation)
    viz.update(brain)
    
    # Render visualization
    viz.render()
    
    # Small delay
    import time
    time.sleep(1.0 / 30.0)
```

#### Real-time Monitoring
```python
import pynlm
import pynlm_viz

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create monitor
monitor = pynlm_viz.RealTimeMonitor()
monitor.initialize(config)
monitor.startMonitoring(10.0)  # 10 Hz update

# Monitor loop
while monitor.isMonitoring():
    # Get latest metrics
    metrics = monitor.getLatestMetrics()
    
    # Display information
    print(f"Step: {metrics.simulationStep}")
    print(f"Firing Rate: {metrics.avgFiringRate:.2f} Hz")
    print(f"Active Neurons: {metrics.activeNeuronCount}")
    print(f"Total Spikes: {metrics.totalSpikeCount}")
    print(f"Memory Load: {metrics.workingMemoryLoad:.2%}")
    print(f"Dopamine Level: {metrics.dopamineLevel:.2f}")
    print("-" * 50)
    
    # Small delay
    import time
    time.sleep(1.0)
```

#### Web Dashboard
```python
from pynlm_viz.dashboard import VisualizationDashboard
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create dashboard
dashboard = VisualizationDashboard()
dashboard.create_dashboard_files()

print("Starting dashboard server...")
print("Open your browser to: http://localhost:8080")
print("Press Ctrl+C to stop the server")

# Start dashboard server
dashboard.start_server(brain, host="localhost", port=8080)
```

#### Data Export
```python
from pynlm_viz.export import DataExporter
import pynlm
import json

# Create brain and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate a few steps
for step in range(100):
    brain.step(step)

# Create exporter
exporter = DataExporter()

# Export brain state
exporter.export_brain_state(brain, "brain_state.json")

# Export simulation data
simulation_data = {
    'steps': list(range(100)),
    'firing_rates': [brain.getAverageFiringRate() for _ in range(100)],
    'spike_counts': [brain.getTotalSpikeCount() for _ in range(100)],
}

exporter.export_data(simulation_data, "simulation_data.json")

# Create analysis report
exporter.create_analysis_report(simulation_data, "simulation_report.json")
```

### 3. Advanced Features

#### Multiple Visualization Types
```python
# Create visualization system
viz = pynlm_viz.VisualizationSystem()

# Add different visualization types
viz.addVisualization(pynlm_viz.VisualizationType.NETWORK)
viz.addVisualization(pynlm_viz.VisualizationType.MEMORY)
viz.addVisualization(pynlm_viz.VisualizationType.NEUROMODULATION)
viz.addVisualization(pynlm_viz.VisualizationType.PLASTICITY)
viz.addVisualization(pynlm_viz.VisualizationType.DEVELOPMENT)
viz.addVisualization(pynlm_viz.VisualizationType.PREDICTION)
```

#### Custom Visualization Builder
```python
from pynlm_viz.NLMVisualizationBuilder import VisualizationBuilder

# Create custom visualization builder
builder = VisualizationBuilder(brain, monitor)

# Build complete dashboard
dashboard = builder.build_complete_dashboard()

# Export configuration
builder.export_visualization_config("custom_dashboard.json")
```

#### Command-Line Tools
```bash
# Run comprehensive demo
python examples/comprehensive_viz_demo.py

# Run specific example
python examples/agent_world.py
python examples/plasticity_experiment.py
python examples/neuromodulation_study.py

# Export data
python -m pynlm_viz export --help
```

## Key Features

### Visualization Capabilities
- ✅ **Real-time neural network visualization** with interactive 3D rendering
- ✅ **Memory system monitoring** with replay and analysis
- ✅ **Neuromodulation tracking** for dopamine, curiosity, novelty
- ✅ **Plasticity visualization** with learning curves and patterns
- ✅ **Development stage visualization** with progression tracking
- ✅ **Prediction system visualization** with confidence intervals
- ✅ **Cross-platform compatibility** (web, desktop, mobile)

### Monitoring Capabilities
- ✅ **Real-time brain state monitoring** with configurable updates
- ✅ **Performance metrics collection** (CPU, memory, spike rates)
- ✅ **Resource usage monitoring** with alerting
- ✅ **Network connectivity analysis**
- ✅ **Learning progress tracking** with trend analysis
- ✅ **Error rate monitoring** and notification
- ✅ **Statistical analysis** and pattern detection

### Python Integration
- ✅ **Python visualization APIs** for real-time updates
- ✅ **Interactive monitoring dashboards**
- ✅ **Data export** to multiple formats
- ✅ **Web-based visualization interfaces**
- ✅ **Mobile visualization** support
- ✅ **Seamless integration** with existing NLM systems

### Development Tools
- ✅ **Custom visualization builders** with Python API
- ✅ **Real-time plotting and graphing utilities**
- ✅ **Comprehensive data export capabilities**
- ✅ **Plugin system** for extensions
- ✅ **Performance-optimized rendering pipeline**

## System Requirements

### Minimum Requirements
- **Python**: 3.8 or higher
- **CMake**: 3.16 or higher
- **C++ Compiler**: C++20 compatible
- **Memory**: 512 MB RAM minimum
- **Disk Space**: 100 MB for package installation

### Recommended for Full Experience
- **Python**: 3.10 or higher
- **OpenGL/Vulkan Support**: For hardware-accelerated rendering
- **Additional RAM**: 2 GB for large network visualization
- **SSD Storage**: For faster file operations
- **Monitor**: Dual monitor setup recommended for multi-view dashboards

## Troubleshooting

### Common Issues and Solutions

#### Issue: "ModuleNotFoundError: No module named 'pynlm_viz'"
**Solution**: Install the package in development mode:
```bash
pip install -e .
```

#### Issue: "ImportError: Cannot import name 'VisualizationSystem'"
**Solution**: Ensure the package is properly structured:
```bash
# Check directory structure
ls src/visualization/
# Should show __init__.py, cli.py, dashboard.py, etc.
```

#### Issue: "Web server port already in use"
**Solution**: Change the port in the dashboard configuration:
```python
# In dashboard.py
port = 8081  # Change to different port
```

#### Issue: "Performance is slow"
**Solution**: Optimize visualization settings:
```python
# Reduce update frequency
viz.setUpdateFrequency(15.0)  # 15 Hz instead of 60 Hz

# Disable high-quality rendering
viz.enableAntialiasing(False)
```

### Getting Help

#### Documentation
- **Online Documentation**: Check the README files in the visualization directory
- **Examples**: Look at the examples directory for working examples
- **API Reference**: Generated docstrings in the source code
- **Community Support**: GitHub issues for the NLM project

#### Support Channels
- **GitHub Repository**: Issues and pull requests
- **Documentation**: Markdown files in the repository
- **Examples**: Working example scripts in examples/
- **Troubleshooting**: Common issues and solutions section

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgements

The NLM Enhanced Visualization and Monitoring project is developed as part of the NLM (Neural Learning Machine) research project. Special thanks to the NLM development team and contributors.

## Contributors

- **NLM Development Team**: Core NLM framework development
- **Visualization Team**: Visualization system design and implementation
- **Python Integration Team**: Python bindings and API development
- **Documentation Team**: Comprehensive documentation and examples

## Future Enhancements

### Planned Features
1. **Virtual Reality Integration**: 3D VR visualization for immersive experience
2. **Machine Learning Integration**: Automated visualization generation
3. **Cloud-Based Rendering**: GPU-accelerated rendering in the cloud
4. **AR Integration**: Augmented reality visualization for mobile devices
5. **Collaboration Tools**: Real-time collaboration and sharing
6. **Performance Profiling**: Advanced profiling and optimization tools
7. **Export to 3D Formats**: STL, OBJ, and other 3D file formats
8. **Integration with External Tools**: MATLAB, Octave, R integration

### Performance Goals
1. **Real-time Rendering**: Sub-10ms update latency for interactive visualization
2. **Scalability**: Support for networks up to 1 million neurons
3. **Memory Efficiency**: < 2GB RAM for large network visualization
4. **Cross-platform Support**: Seamless operation across all platforms
5. **Offline Capability**: Full functionality without internet connection

## Conclusion

The NLM Enhanced Visualization and Monitoring system provides a comprehensive solution for real-time neural network visualization, monitoring, and analysis. The implementation includes Python integration, web-based interfaces, development tools, and seamless compatibility with existing NLM systems, enabling researchers and developers to gain deeper insights into neural network behavior and performance.

This system serves as a foundation for future enhancements and will continue to evolve as the NLM project advances. The modular design allows for easy extension and customization to meet specific research and development needs.
