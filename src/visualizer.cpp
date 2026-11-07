=== NLM Visualizer ===

An interactive visualization tool for NLM Phase 6 simulation results with real-time updates and comprehensive analysis.

Usage:
  nlm_visualizer [options]

Options:
  --input-profile FILE     Profile file to load (default: profile.json)
  --mode MODE              Visualization mode: spikes, network, memory, prediction (default: network)
  --time-range START END   Time range to display (default: auto)
  --neuron-range MIN MAX   Neuron range for detailed view (default: auto)
  --width PIXELS           Display width in pixels (default: 1200)
  --height PIXELS          Display height in pixels (default: 800)
  --interactive            Enable interactive controls (default: true)
  --fullscreen             Start in fullscreen mode
  --auto-refresh           Auto-refresh display (default: true)
  --refresh-interval MS    Auto-refresh interval in milliseconds (default: 500)
  --export-viz FILE        Export visualization to file (PNG, SVG, PDF)
  --save-layout FILE       Save current layout configuration
  --load-layout FILE       Load layout configuration
  --show-heatmap           Display spike heat map overlay
  --show-memory           Display memory usage timeline
  --show-network          Display network connectivity graph
  --show-prediction       Display prediction system activity
  --show-neuron-states    Display individual neuron states
  --neuron-palette PAL     Color palette for neurons (default: standard)
  --help                   Show this help message

Examples:
  nlm_visualizer --mode spikes --time-range 0 1000
  nlm_visualizer --show-network --show-prediction
  nlm_visualizer --export-viz simulation.png
