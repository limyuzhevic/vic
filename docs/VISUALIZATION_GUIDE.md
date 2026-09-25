# NLM Phase 6 Visualization Tools Documentation

## Overview

The NLM (Neural Learning Machine) Phase 6 Visualization Tools provide comprehensive real-time visualization capabilities for monitoring and analyzing the integrated artificial brain system.

## Features

### 1. Multi-Mode Visualization
- **Text Mode**: Simple terminal-based visualization
- **Text Advanced Mode**: Detailed text-based visualization with extensive metrics
- **Compact Mode**: Minimal display showing key metrics
- **Graphical Mode**: Advanced visualization with GUI (requires external libraries)
- **WebSocket Mode**: Real-time web-based visualization via WebSocket
- **Python API Mode**: Integration with Python visualization tools

### 2. Comprehensive Brain System Monitoring
- Neural network structure and connectivity
- Real-time neural activity and firing patterns
- Memory system integration (working memory, episodic memory)
- Neuromodulation system (dopamine, curiosity, novelty)
- Prediction system with confidence tracking
- Development system stages and effects
- Integration status of all 6 brain systems

### 3. Command-Line Interface
- Extensive command-line options for customization
- Help system with detailed documentation
- Performance monitoring
- Data export capabilities
- Automatic saving and periodic data collection

## Getting Started

### Building and Running

#### Basic Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Running with Different Visualization Modes

**Text Mode (default):**
```bash
./nlm_phase6_demo
```

**Text Advanced Mode:**
```bash
./nlm_phase6_demo --visualization-mode text-advanced --visualization-width 120 --visualization-height 80
```

**Compact Mode:**
```bash
./nlm_phase6_demo --visualization-mode compact --visualization-title "NLM Phase 6"
```

**WebSocket Mode:**
```bash
./nlm_phase6_demo --visualization-mode websocket --websocket-port 8080 --enable-export
```

**With Help:**
```bash
./nlm_phase6_demo --help
```

### Python Bindings

The visualization tools include Python bindings for integration with Python-based visualization tools:

```python
import pynlm_viz

# Create visualization interface
viz = pynlm_viz.VisualizationInterface(mode=pynlm_viz.VisualizationMode.TextAdvanced)
viz.initialize(width=120, height=80)

# Update with brain data
viz.update(brain, timestamp)

# Render visualization
viz.render()

# Get metrics
metrics = viz.getCurrentMetrics()
print(f"Firing Rate: {metrics.avg_firing_rate}")
print(f"Memory Integration: {metrics.memory_integrated}")

# Export data
viz.exportData("visualization_output.json")
```

## Visualization Modes

### Text Mode

The basic text mode provides essential information about the brain system state:

```
=== NLM Phase 6 Visualization (Text Mode) ===

System Status:
  Total Reward: 0.45
  Avg Firing Rate: 12.5 Hz
  Avg Synaptic Weight: 0.523

Memory Systems:
  Working Memory: 5 active traces
  Episodic Memory: 3 episodes stored
  Memory Integrated: YES
  Episodic Memory Integrated: YES

Neuromodulation:
  Dopamine Level: 0.72
  Curiosity Level: 0.58
  Novelty Level: 0.34
  Neuromodulation Integrated: YES

Prediction System:
  Prediction Confidence: 0.81
  Prediction Integrated: YES

Development System:
  Development Stage: 3.0
  Development Integrated: YES

Integration Status:
  Checkpointing Works: YES
  Replay Works: NO

Simulation:
  Time: 2500 steps
  Update Rate: 30 Hz
```

### Text Advanced Mode

Provides more detailed information with network structure, system integration status, and performance metrics.

### Compact Mode

Shows only the most essential metrics in a concise format:

```
NLM6: R:0.45, FR:12.5Hz, WM:5, EM:3, Dop:0.72, Cur:0.58, Pred:0.81, Dev:3.0, Sim:2500
```

## Command-Line Options

### Core Options

- `--visualization-mode <mode>`: Visualization mode (text, text-advanced, graphical, websocket, python, compact)
- `--visualization-title <title>`: Window/application title
- `--visualization-width <width>`: Window width in characters
- `--visualization-height <height>`: Window height in characters
- `--visualization-update-rate <hz>`: Update rate in Hz

### Feature Toggles

- `--no-network-viz`: Disable network visualization
- `--no-activity-viz`: Disable activity visualization
- `--no-memory-viz`: Disable memory visualization
- `--no-neuromod-viz`: Disable neuromodulation visualization
- `--no-prediction-viz`: Disable prediction visualization
- `--no-development-viz`: Disable development visualization

### Export and Data Options

- `--enable-export`: Enable data export
- `--export-format <format>`: Export format (json, csv, txt)
- `--export-path <path>`: Export file path
- `--auto-save`: Automatically save visualization data

### WebSocket Options

- `--enable-websocket`: Enable WebSocket visualization server
- `--websocket-port <port>`: WebSocket server port

### Utility Options

- `--enable-help`: Show help information
- `--enable-performance`: Show performance metrics
- `--color-scheme <scheme>`: Color scheme (default, dark, light, bw)

## Python Examples

### Basic Visualization

```python
import pynlm_viz
import time

# Create visualization interface
viz = pynlm_viz.VisualizationInterface(
    mode=pynlm_viz.VisualizationMode.TextAdvanced
)

# Initialize with custom settings
viz.initialize(
    title="NLM Phase 6 Visualization",
    width=120,
    height=80
)

# Create a simple brain for demonstration
brain = create_test_brain()  # Your function to create a test brain

# Main visualization loop
for timestamp in range(0, 1000, 100):
    viz.update(brain, timestamp)
    viz.render()
    time.sleep(0.1)

viz.close()
```

### Real-time Visualization

```python
import pynlm_viz
import threading
import time

def update_brain_data():
    """Background thread to update brain data"""
    while running:
        # Update brain data from your simulation
        update_brain_from_simulation()
        time.sleep(0.1)

# Start background thread
running = True
thread = threading.Thread(target=update_brain_data)
thread.daemon = True
thread.start()

# Main visualization
viz = pynlm_viz.VisualizationInterface(
    mode=pynlm_viz.VisualizationMode.WebSocket
)
viz.initialize(websocket_port=8080)

while running:
    brain = get_current_brain()  # Get current brain from simulation
    viz.update(brain, time.time())
    viz.render()
    time.sleep(0.05)
```

### Metrics Analysis

```python
import pynlm_viz
import json

# Create visualization and get metrics
viz = pynlm_viz.VisualizationInterface()
metrics = viz.getCurrentMetrics()

# Analyze metrics
print("=== System Analysis ===")
print(f"Learning Progress: Reward={metrics.total_reward}, FiringRate={metrics.avg_firing_rate}")

if metrics.memory_integrated:
    print("✅ Memory system is integrated")
else:
    print("❌ Memory system needs integration")

if metrics.prediction_integrated:
    print("✅ Prediction system is integrated")
else:
    print("❌ Prediction system needs integration")

if metrics.development_integrated:
    print("✅ Development system is integrated")
else:
    print("❌ Development system needs integration")

# Export metrics for analysis
viz.exportMetrics("metrics_analysis.csv")
```

## Configuration Examples

### Example Configuration File

```json
{
    "visualization": {
        "mode": "text-advanced",
        "window_title": "NLM Phase 6 - Advanced Visualization",
        "width": 120,
        "height": 80,
        "update_rate": 30,
        "features": {
            "network": true,
            "activity": true,
            "memory": true,
            "neuromodulation": true,
            "prediction": true,
            "development": true
        },
        "export": {
            "enabled": true,
            "format": "json",
            "path": "./visualization_output",
            "auto_save": true,
            "interval": 30
        },
        "websocket": {
            "enabled": false,
            "port": 8080,
            "host": "localhost"
        }
    }
}
```

### Python Configuration

```python
config = pynlm_viz.VisualizationInterface.VisualizationConfig()
config.window_title = "NLM Phase 6 - Scientific Visualization"
config.width = 120
config.height = 80
config.enableTextVisualization = True
config.enableExport = True
config.exportFormat = "json"
config.exportPath = "./experiment_output"
config.autoSave = True
config.showHelp = True
config.showPerformance = True
config.colorScheme = "dark"

# Create visualization with config
viz = pynlm_viz.VisualizationInterface(pynlm_viz.VisualizationMode.TextAdvanced)
viz.setConfig(config)
viz.initialize()
```

## Performance Tips

### For Large Brains

- Use `compact` or `text` mode for large brain simulations
- Increase visualization update rate for smoother rendering
- Use export mode to save data periodically

### For Limited Resources

- Reduce visualization update rate (e.g., 10 Hz instead of 30 Hz)
- Disable graphical features if not needed
- Use text modes instead of graphical
- Limit exported data frequency

### For Production

- Enable `auto-save` for continuous data collection
- Use `websocket` mode for remote monitoring
- Configure appropriate `export_path` for data storage
- Monitor performance with `--enable-performance`

## Common Issues and Solutions

### Issue: Visualization doesn't start

**Solution:**
```bash
# Check if dependencies are installed
./nlm_phase6_demo --visualization-mode text
```

### Issue: No data displayed

**Solution:**
- Ensure brain system is properly initialized
- Check that all components are integrated
- Verify visualization mode is correctly set

### Issue: Slow performance

**Solution:**
- Reduce visualization update rate
- Use simpler visualization mode
- Close unused visualization windows
- Check system resources

### Issue: Python bindings not working

**Solution:**
```bash
# Rebuild with Python bindings
pip install -e .
```

## Files Generated

### During Execution

- `visualization_output/step_*.json`: Visualization data snapshots
- `visualization_output/*.csv`: Metrics data
- `visualization_output/log.txt`: Visualization session logs

### Configuration Files

- `visualization_config.json`: User configuration
- `visualization_settings.cfg`: Alternative configuration format

## Advanced Features

### WebSocket Integration

The WebSocket mode allows real-time visualization from web browsers:

1. Start visualization with WebSocket enabled
2. Open web browser to `http://localhost:8080`
3. Connect to visualization stream
4. Receive real-time brain state updates

### Data Export

The export feature supports multiple formats:

- **JSON**: Human-readable with metadata
- **CSV**: Simple numerical data
- **Text**: Plain text reports

Export includes both snapshot data and metrics analysis.

### Performance Monitoring

Built-in performance monitoring tracks:
- Update rate and frame timing
- Memory usage
- CPU utilization
- Export performance

## Testing Visualization

### Unit Tests

Run the visualization tests:
```bash
./build/tests/visualization_test
```

### Integration Tests

Run the full integration test:
```bash
./nlm_phase6_demo --visualization-mode text-advanced
```

### Performance Tests

Run performance analysis:
```bash
./nlm_phase6_demo --visualization-mode compact --enable-performance
```

## Troubleshooting

### Common Problems

1. **Visualization Window Not Appearing**
   - Check terminal size
   - Verify console application capabilities
   - Try different visualization mode

2. **Data Not Updating**
   - Ensure brain system is running
   - Check update rate settings
   - Verify data collection is working

3. **Export Not Working**
   - Check write permissions
   - Verify directory exists
   - Check configuration settings

### Getting Help

For additional help:

1. Check the command-line help:
   ```bash
   ./nlm_phase6_demo --help
   ```

2. Read the documentation in `docs/`

3. Check the examples directory for more usage patterns

## Future Enhancements

### Planned Features

1. **3D Visualization**: Enhanced graphical visualization with 3D brain models
2. **Machine Learning Integration**: Integration with visualization ML models
3. **Multi-view Display**: Split-screen visualization for multiple perspectives
4. **Animation Export**: Export visualization as video or animated GIFs
5. **Collaboration Tools**: Real-time sharing of visualization sessions

### Research Integration

The visualization tools are designed to support:
- Scientific research and experimentation
- Educational demonstrations
- Real-time monitoring in production
- Performance analysis and optimization

## Conclusion

The NLM Phase 6 Visualization Tools provide comprehensive monitoring and analysis capabilities for the integrated artificial brain system. Whether you're running experiments, analyzing results, or demonstrating the system, the visualization tools provide the data and insights you need to understand and optimize the brain behavior.

For the best experience, start with text-advanced mode for detailed analysis, then explore other modes as needed. The command-line interface and Python bindings provide flexible integration with your existing workflows.