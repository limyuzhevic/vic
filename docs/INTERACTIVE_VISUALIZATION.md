# NLM Interactive Visualization Quick Start Guide

## Overview
This document describes how to use the interactive visualization mode in the NLM (Neural Learning Machine) project.

## System Requirements
- Terminal with at least 70x30 characters
- ncurses development libraries
- C++20 compiler
- CMake 3.16 or higher

## Installation

### From Source
1. Clone the repository
2. Install dependencies:
   ```bash
   # Ubuntu/Debian
   sudo apt-get update && sudo apt-get install build-essential libncurses5-dev
   
   # CentOS/RHEL
   sudo yum install gcc-c++ ncurses-devel
   
   # Fedora
   sudo dnf install gcc-c++ ncurses-devel
   ```
3. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j$(nproc)
   ```

## Usage

### Basic Usage
Run NLM with interactive visualization mode:

```bash
./nlm --visualization
# or
./nlm --interactive
```

### Keyboard Controls

#### Navigation
- **Arrow Keys**: Pan view (when in Network view mode)
- **+/- Keys or '='**: Zoom in/out
- **8/2 Keys**: Increase/decrease simulation speed

#### View Modes
- **1**: Network View (default) - Shows neural network topology
- **2**: Activity View - Heatmap of neural activity
- **3**: Spike View - Raster plot of spikes over time
- **4**: Weight View - Weight matrix visualization
- **5**: Metrics View - Real-time dashboard with plots

#### Interaction
- **n**: Select next region
- **i**: Inspect current selection
- **[ and ]**: Decrease/increase activity threshold
- **s**: Save visualization state to file
- **e**: Export data to CSV
- **Space**: Pause/resume simulation

#### Help and Exit
- **?**: Show help screen
- **q/Q**: Quit interactive visualization

### Configuration

#### Command Line Arguments
```bash
./nlm --visualization --config configs/myconfig.cfg
```

#### Configuration File Options
Create a custom config file (JSON or key=value format) with visualization settings.

## Features

### 1. Real-Time Metrics Display
The interactive visualization shows real-time metrics including:
- Neural activity (firing rate percentage)
- Memory states
- Neuromodulation levels
- Developmental stage
- Connectivity patterns

### 2. Color-Coded Indicators
Different neuron types and states are color-coded:
- **Neuron Types**:
  - Excitatory: Red
  - Inhibitory: Blue
  - Sensory: Yellow
  - Motor: Green
  - Modulatory: Magenta

- **Firing States**:
  - Resting: White
  - Active: Yellow
  - Firing: Red
  - Refractory: Cyan

- **Developmental Stages**:
  - Initial: Red
  - CriticalPeriod: Yellow
  - Maturation: Yellow
  - Adult: Green
  - Aging: Blue

### 3. Zoom and Pan Controls
Explore different parts of the brain:
- **Zoom**: +/- or '=' keys to zoom in/out (0.5x to 3.0x)
- **Pan**: Arrow keys to move view (when in Network view)

### 4. Data Export
Save visualization data in multiple formats:
- **Save State**: Text file with current visualization state
- **Export CSV**: Time-series data for external analysis

### 5. Help System
Press **?** to display comprehensive help with all available commands.

## File Formats

### Visualization State (.txt)
- Current simulation state
- Visualization parameters
- Brain statistics
- Metrics history

### Exported Data (.csv)
- Time series data
- Neural activity
- Memory states
- Neuromodulation levels
- Connectivity metrics

## Troubleshooting

### Common Issues

1. **"Screen too small" error**
   - Ensure terminal is at least 70x30 characters
   - You can still use interactive mode with smaller screens (scaled display)

2. **Missing ncurses libraries**
   - Install the development packages for your distribution
   - On Ubuntu: `sudo apt-get install libncurses5-dev`
   - On CentOS: `sudo yum install ncurses-devel`

3. **Build failures**
   - Ensure you have a C++20 compatible compiler
   - Check that all dependencies are installed
   - Try building with `make -j1` if parallel build fails

4. **Interactive mode doesn't respond**
   - Press any key to ensure terminal is in proper mode
   - Check terminal settings (ensure raw mode is active)

### Debugging

To enable debug logging:
- Set log level to Debug in your config
- Check the console output for initialization messages

## Advanced Usage

### Custom Configuration
Create a custom config file with visualization settings:

```json
{
  "visualization": {
    "enabled": true,
    "update_rate": 30,
    "use_color": true,
    "show_debug_info": true
  }
}
```

### Automation
Use scripts to automate visualization tasks:

```bash
#!/bin/bash
# Save visualization state
./nlm --visualization --save_state v1_state.txt

# Run simulation and export data
./nlm --visualization --run_for 1000_steps > simulation_log.txt 2>&1
./nlm --visualization --export_data simulation_data.csv
```

## Future Enhancements

The interactive visualization system is designed to be extensible:

1. **Additional Visualization Modes**: More view types can be added
2. **Custom Metrics**: Users can define custom metrics to display
3. **Network Analysis Tools**: Advanced network analysis features
4. **Multi-window Displays**: Split-screen views for comparison
5. **Export Formats**: Additional export formats (JSON, XML, etc.)
6. **Command Scripts**: Save and replay command sequences

## Conclusion

The interactive visualization mode provides a powerful, real-time interface for exploring and understanding the NLM brain's dynamics. It combines comprehensive visualization capabilities with an intuitive keyboard interface, making it easy to interact with and analyze the neural network's behavior.

For more detailed information or to report issues, please refer to the project documentation or issue tracker.
