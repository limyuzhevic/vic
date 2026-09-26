# NLM Visualization System

This directory contains the web-based visualization components for the Neural Learning Model (NLM) system.

## Overview

The web-based visualization system provides browser-based interactive visualizations of the NLM system components, including neural networks, memory systems, neuromodulation, plasticity, and development.

## Files

### index.html
Main entry point for the web-based visualization dashboard.

### visualization/
Directory containing visualization components:

#### neural-network-visualization.js
Visualization component for neural networks.

#### memory-visualization.js
Visualization component for memory systems.

#### neuromodulation-visualization.js
Visualization component for neuromodulation.

#### plasticity-visualization.js
Visualization component for plasticity.

#### development-visualization.js
Visualization component for development.

#### activity-monitor.js
Real-time activity monitoring component.

#### network-topology.js
Network topology visualization component.

### analysis/
Directory containing analysis components:

#### performance-analysis.js
Performance analysis component.

#### learning-analysis.js
Learning behavior analysis component.

#### network-analysis.js
Network dynamics analysis component.

#### system-analysis.js
System integration analysis component.

### components/
Directory containing reusable UI components:

#### dashboard.js
Main dashboard component.

#### control-panel.js
Control panel component.

#### data-source-manager.js
Data source management component.

#### export-manager.js
Export/import management component.

#### template-manager.js
Template management component.

#### performance-metrics.js
Performance metrics display component.

#### learning-curves.js
Learning curve visualization component.

#### network-graph.js
Network graph visualization component.

### styles/
Directory containing CSS stylesheets:

#### main.css
Main stylesheet.

#### visualization.css
Styles for visualization components.

#### analysis.css
Styles for analysis components.

#### components.css
Styles for UI components.

### scripts/
Directory containing JavaScript utilities:

#### utils.js
Utility functions.

#### data-processor.js
Data processing utilities.

#### chart-utils.js
Chart generation utilities.

## Usage

1. Clone this repository
2. Navigate to the web_visualization directory
3. Open index.html in a web browser
4. Use the controls to select visualization components
5. Use the analysis tabs to view analysis results
6. Export/import data using the export/import controls
7. Apply templates using the template manager

## Dependencies

- Web browser with modern JavaScript support
- WebSocket support for real-time updates
- WebGL support for 3D visualizations (optional)

## Build Instructions

1. Copy this directory to your web server
2. Configure the web server to serve the files
3. Ensure the web server has CORS enabled if loading from a different origin
4. Access the visualization system through your web browser

## Configuration

The web-based visualization system can be configured by modifying the configuration files in the configs directory.

## Documentation

- [Web Visualization Documentation](documentation/web-visualization.md)
- [Web Visualization API](documentation/web-visualization-api.md)
- [Web Visualization Examples](examples/web-visualization-examples.md)

## Support

For support with the web-based visualization system, please visit the NLM documentation or contact support.
