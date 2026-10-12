# NLM Visualization API Guide

## Overview

This guide provides comprehensive documentation for the visualization APIs in the enhanced NLM Python bindings. The visualization APIs include both static (matplotlib) and interactive (plotly) visualizations for brain simulation results.

## What This Guide Covers

1. **Matplotlib Visualization**: Static plotting with matplotlib
2. **Plotly Visualization**: Interactive web-based visualization
3. **Real-time Visualization**: Live monitoring of simulation progress
4. **Dashboard Creation**: Multi-panel dashboards for comprehensive monitoring
5. **Parameter Exploration**: Visual exploration of parameter spaces
6. **Performance Optimization**: Tips for efficient visualization

## Quick Start

```python
import pynlm

# Create visualizer
visualizer = pynlm.create_matplotlib_visualizer()

# Generate plots
visualizer.plot_neural_activity()
visualizer.plot_memory_metrics()
visualizer.plot_learning_progress()

# Display plots
visualizer.show_all_plots()

# Save plots
visualizer.save_plot('neural_activity', 'neural_activity.png')
```

## Matplotlib Visualization

### MatplotlibVisualizer Class

The `MatplotlibVisualizer` class provides matplotlib-based visualization of brain simulation results.

**Constructor:**

```python
MatplotlibVisualizer(brain_manager=None)
```

**Parameters:**

- `brain_manager`: Optional BrainManager instance to visualize

**Methods:**

- `set_brain_manager(brain_manager)` - Set the brain manager to visualize
- `plot_neural_activity(step_range=None)` - Plot neural activity over time
- `plot_memory_metrics()` - Plot memory system metrics
- `plot_learning_progress()` - Plot learning progress
- `plot_integration_status()` - Plot integration status
- `show_all_plots()` - Display all generated plots
- `save_plot(name, path)` - Save a plot to file

**Example:**

```python
import pynlm

# Create visualizer
visualizer = pynlm.MatplotlibVisualizer()

# Plot neural activity
fig1 = visualizer.plot_neural_activity()

# Plot memory metrics
fig2 = visualizer.plot_memory_metrics()

# Plot learning progress
fig3 = visualizer.plot_learning_progress()

# Plot integration status
fig4 = visualizer.plot_integration_status()

# Display all plots
visualizer.show_all_plots()

# Save specific plot
visualizer.save_plot('neural_activity', 'neural_activity.png')
```

### Neural Activity Plot

The `plot_neural_activity()` method generates a plot of neural activity over time.

**Parameters:**

- `step_range`: Optional tuple of (start_step, end_step) to visualize

**Returns:**

- matplotlib Figure object

**Features:**

- Time series plot of neural activity
- Configurable step range
- Grid display for readability
- Custom styling

### Memory Metrics Plot

The `plot_memory_metrics()` method generates plots of memory system metrics.

**Returns:**

- matplotlib Figure object with 2x2 subplots showing:
  - Working memory utilization
  - Episodic memory storage
  - Memory consolidation effectiveness
  - Replay events

**Features:**

- Multiple memory system visualizations
- Time series plots
- Bar chart for replay events
- Consistent styling across subplots

### Learning Progress Plot

The `plot_learning_progress()` method generates plots of learning progress over time.

**Returns:**

- matplotlib Figure object with 2 subplots:
  - Learning rate over time
  - Learning performance over time

**Features:**

- Learning rate visualization
- Performance tracking
- Cumulative performance plot
- Grid display for readability

### Integration Status Plot

The `plot_integration_status()` method generates a comprehensive integration status plot.

**Returns:**

- matplotlib Figure object with 2x2 subplots:
  - Integration status bar chart
  - System metrics bar chart
  - Integration progress line chart
  - System health pie chart

**Features:**

- Multiple integration metrics
- Comparative analysis
- Progress tracking
- System health overview

## Plotly Visualization

### PlotlyVisualizer Class

The `PlotlyVisualizer` class provides interactive plotly-based visualization.

**Constructor:**

```python
PlotlyVisualizer(brain_manager=None)
```

**Parameters:**

- `brain_manager`: Optional BrainManager instance to visualize

**Methods:**

- `set_brain_manager(brain_manager)` - Set the brain manager to visualize
- `create_interactive_dashboard()` - Create interactive dashboard
- `plot_real_time_metrics()` - Create real-time metrics visualization
- `create_exploration_plots()` - Create parameter exploration plots
- `show_interactive(figure_names=None)` - Display interactive plots
- `save_interactive(name, path)` - Save interactive plot to HTML

**Example:**

```python
import pynlm

# Create visualizer
visualizer = pynlm.PlotlyVisualizer()

# Create dashboard
dashboard = visualizer.create_interactive_dashboard()

# Create exploration plots
plots = visualizer.create_exploration_plots()

# Display plots
visualizer.show_interactive()

# Save dashboard
dashboard.save_html("dashboard.html")
```

### Interactive Dashboard

The `create_interactive_dashboard()` method creates an interactive dashboard with multiple panels.

**Returns:**

- plotly Figure object with:
  - Neural activity time series
  - Learning progress overlay
  - Interactive hover information
  - Responsive layout

**Features:**

- Multi-panel display
- Interactive hover
- Responsive design
- Real-time updates capability

### Real-Time Metrics Visualization

The `plot_real_time_metrics()` method creates real-time metrics visualization.

**Returns:**

- plotly Figure object with:
  - Neural activity over time
  - Firing rate overlay
  - Learning rate overlay
  - Multiple y-axes

**Features:**

- Real-time data visualization
- Multiple metrics
- Interactive zooming
- Hover information

### Parameter Exploration Plots

The `create_exploration_plots()` method creates parameter exploration visualizations.

**Returns:**

- Dictionary of plotly Figure objects:
  - `parameter_exploration`: Scatter plot of parameter combinations
  - `performance_comparison`: Box plot of performance by brain type

**Features:**

- Parameter space exploration
- Performance comparison
- Statistical insights
- Interactive plotting

## Real-Time Visualization

### Real-Time Monitoring

Real-time visualization allows you to monitor brain simulations as they run:

```python
import pynlm
from pynlm import ResearchSession
import time

# Create real-time visualizer
visualizer = pynlm.PlotlyVisualizer()

# Create research session
with ResearchSession() as session:
    for step in range(1000):
        session.run_step(step)
        
        # Update real-time plot every 100 steps
        if step % 100 == 0:
            visualizer.plot_real_time_metrics()
            visualizer.show_interactive()
        
        time.sleep(0.01)  # Small delay to control simulation speed
```

### Live Updates

The visualization APIs support live updates during simulation:

1. **Dynamic Data**: Update plots with new simulation data
2. **Real-time Metrics**: Display current performance metrics
3. **Interactive Controls**: Add sliders and buttons for interaction
4. **Web Dashboard**: Create web-based dashboards

## Dashboard Creation

### Multi-Panel Dashboard

Create comprehensive dashboards with multiple panels:

```python
import pynlm
import plotly.graph_objects as go
import numpy as np

# Create custom dashboard
def create_custom_dashboard(brain_data):
    fig = go.Figure()
    
    # Add neural activity
    fig.add_trace(go.Scatter(
        x=np.arange(len(brain_data['neural_activity'])),
        y=brain_data['neural_activity'],
        name='Neural Activity',
        line=dict(color='blue')
    ))
    
    # Add learning progress
    fig.add_trace(go.Scatter(
        x=np.arange(len(brain_data['learning_progress'])),
        y=brain_data['learning_progress'],
        name='Learning Progress',
        yaxis='y2',
        line=dict(color='red')
    ))
    
    # Add memory metrics
    fig.add_trace(go.Scatter(
        x=np.arange(len(brain_data['memory_metrics'])),
        y=brain_data['memory_metrics'],
        name='Memory Metrics',
        yaxis='y3',
        line=dict(color='green')
    ))
    
    # Update layout
    fig.update_layout(
        title='Brain Simulation Dashboard',
        xaxis=dict(title='Step'),
        yaxis=dict(title='Neural Activity', side='left'),
        yaxis2=dict(title='Learning Progress', overlaying='y', side='right'),
        yaxis3=dict(title='Memory Metrics', overlaying='y', side='right', position=0.9),
        height=600
    )
    
    return fig
```

## Parameter Exploration

### Visual Parameter Tuning

Visualize parameter tuning results:

```python
import pynlm
import pandas as pd

# Create parameter tuning API
param_tuning_api = pynlm.ParameterTuningAPI()

# Tune learning rates
learning_rate_results = param_tuning_api.tune_learning_rate(
    learning_rates=[0.001, 0.005, 0.01, 0.05, 0.1],
    n_trials=20
)

# Create visualizer
visualizer = pynlm.PlotlyVisualizer()

# Create exploration plots
plots = visualizer.create_exploration_plots()

# Add parameter tuning results to plots
# (Implementation would depend on specific data structure)
```

## Performance Optimization

### Efficient Plotting

1. **Batch Operations**: Generate all plots before displaying
2. **Memory Management**: Clear plots when no longer needed
3. **Data Sampling**: Sample data for large datasets
4. **Caching**: Cache generated plots

### Real-Time Performance

1. **Efficient Updates**: Use efficient plot update methods
2. **Hardware Acceleration**: Use hardware-accelerated rendering
3. **Resource Management**: Monitor system resources
4. **Adaptive Quality**: Adjust quality based on system resources

## Examples

### Example 1: Static Visualization

```python
import pynlm

# Create visualizer
visualizer = pynlm.MatplotlibVisualizer()

# Generate and save visualizations
visualizer.plot_neural_activity(step_range=(0, 100))
visualizer.save_plot('neural_activity', 'static_neural_activity.png')

visualizer.plot_memory_metrics()
visualizer.save_plot('memory_metrics', 'static_memory_metrics.png')

print("Static visualizations saved successfully")
```

### Example 2: Interactive Dashboard

```python
import pynlm

# Create interactive visualizer
visualizer = pynlm.PlotlyVisualizer()

# Create dashboard
dashboard = visualizer.create_interactive_dashboard()

# Create exploration plots
plots = visualizer.create_exploration_plots()

# Display all plots
visualizer.show_interactive()

# Save dashboard for sharing
dashboard.save_html("brain_simulation_dashboard.html")

print("Interactive dashboard created and saved")
```

### Example 3: Real-Time Monitoring

```python
import pynlm
from pynlm import ResearchSession
import time

# Create visualizer
visualizer = pynlm.PlotlyVisualizer()

# Create research session
with ResearchSession() as session:
    # Create initial plot
    visualizer.plot_real_time_metrics()
    
    # Run simulation with real-time updates
    for step in range(500):
        session.run_step(step)
        
        # Update plot every 50 steps
        if step % 50 == 0:
            visualizer.plot_real_time_metrics()
            visualizer.show_interactive()
        
        # Small delay to control visualization speed
        time.sleep(0.1)

print("Real-time monitoring completed")
```

### Example 4: Combined Visualization

```python
import pynlm
import pandas as pd
import numpy as np

# Create comprehensive visualization
brain_manager = pynlm.BrainManager()

# Configure brain
brain_manager.set_learning_rate(0.01)
brain_manager.set_neuron_count(1000)

# Create visualizers
matplotlib_visualizer = pynlm.MatplotlibVisualizer(brain_manager)
plotly_visualizer = pynlm.PlotlyVisualizer(brain_manager)

# Run experiment
results = brain_manager.run_experiment(steps=200)

# Generate all visualizations
matplotlib_visualizer.plot_neural_activity()
matplotlib_visualizer.plot_memory_metrics()
matplotlib_visualizer.plot_learning_progress()

# Display static plots
matplotlib_visualizer.show_all_plots()

# Create interactive dashboard
interactive_dashboard = plotly_visualizer.create_interactive_dashboard()

# Create exploration plots
exploration_plots = plotly_visualizer.create_exploration_plots()

# Display interactive plots
plotly_visualizer.show_interactive()

print("Comprehensive visualization completed")
```

## Installation

### Requirements

The visualization APIs require:

- Python >= 3.8
- NumPy >= 1.20
- Pandas >= 1.3.0
- Matplotlib >= 3.5.0
- Plotly >= 5.0.0
- SciPy >= 1.7.0

### Installation

```bash
# Install with visualization support
pip install pynlm[visualization]

# Install with all dependencies
pip install pynlm[dev,examples,visualization,advanced_ml]
```

## Troubleshooting

### Common Issues

1. **Import Errors**: Ensure all dependencies are installed
2. **Plot Display Issues**: Check for display environment (use `Agg` backend for headless)
3. **Memory Issues**: Monitor memory usage when generating large plots
4. **Performance Issues**: Optimize data before plotting

### Tips for Best Performance

1. **Use Agg Backend for Headless**: Set `matplotlib.use('Agg')` for headless environments
2. **Limit Data Points**: Use a reasonable number of data points for large datasets
3. **Cache Plots**: Cache frequently used plots
4. **Optimize Memory**: Clear plots when no longer needed

### Visualization Best Practices

1. **Consistent Styling**: Use consistent styling across all plots
2. **Clear Labels**: Provide clear axis labels and titles
3. **Color Schemes**: Use accessible color schemes
4. **Responsive Design**: Ensure plots are responsive

## File Formats

### Supported File Formats

#### Static Plots (Matplotlib)

- PNG: High-quality static images
- PDF: Vector graphics for publications
- SVG: Scalable vector graphics
- JPG: Compressed images for web

#### Interactive Plots (Plotly)

- HTML: Interactive web pages
- JSON: Data for web applications
- JavaScript: Embedded interactivity

### File Saving

```python
# Save matplotlib plot
matplotlib_visualizer.save_plot('neural_activity', 'neural_activity.png')
matplotlib_visualizer.save_plot('neural_activity', 'neural_activity.pdf')

# Save plotly plot
plotly_visualizer.save_interactive('dashboard', 'dashboard.html')
plotly_visualizer.save_interactive('dashboard', 'dashboard.json')
```

## Conclusion

The NLM visualization APIs provide comprehensive visualization capabilities for brain simulation results. Whether you need static plots for publications, interactive dashboards for exploration, or real-time monitoring for research, the visualization APIs provide the tools you need.

The visualization APIs integrate seamlessly with the rest of the NLM enhanced bindings, providing a complete solution for visualization and analysis of brain simulation results.

## References

1. Matplotlib Documentation: https://matplotlib.org/stable/index.html
2. Plotly Documentation: https://plotly.com/python/
3. NLM Visualization Guide: https://nlm.readthedocs.io/en/latest/viz_api.html

## Version History

- **Version 0.1.0**: Initial release of visualization APIs
- **Version 0.2.0**: Added interactive plotly visualization
- **Version 0.3.0**: Added real-time monitoring capabilities
- **Version 0.4.0**: Added dashboard creation
- **Version 0.5.0**: Added parameter exploration and performance optimizations

## Future Roadmap

1. **3D Visualization**: Add 3D visualization capabilities
2. **Virtual Reality**: Support for VR visualization
3. **Augmented Reality**: AR overlay for real-world applications
4. **Cloud Visualization**: Cloud-based visualization services
5. **Real-time Streaming**: Real-time data streaming for large simulations

## Acknowledgments

The NLM visualization APIs are built on the contributions of the matplotlib and plotly communities. We thank them for providing these powerful visualization tools.

## License

This project is licensed under the MIT License. See the LICENSE file for details.