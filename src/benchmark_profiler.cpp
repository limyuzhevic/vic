=== NLM Performance Profiler ===

A powerful profiling tool for NLM Phase 6 simulations with detailed timing analysis and performance metrics.

Usage:
  nlm_profiler [options] [--] [brain_args]

Options:
  --step-count N          Number of simulation steps (default: 10000)
  --neuron-count N        Neuron count (default: 1000)
  --region-count N        Region count (default: 1)
  --connection-prob F     Connection probability (default: 0.1)
  --output-profile FILE   Profile output file (default: profile.json)
  --output-timing FILE    Timing summary file (default: timing.txt)
  --output-spikes FILE    Spike activity file (default: spikes.json)
  --output-memory FILE    Memory usage file (default: memory.json)
  --resolution MS         Profiling resolution in milliseconds (default: 1000)
  --wall-clock           Include wall-clock timing (default: true)
  --cpu-only             Only profile CPU usage (no wall-clock)
  --detailed            Enable detailed per-step profiling
  --timeline            Generate timeline visualization
  --heatmap             Generate spike heat map
  --memory-usage        Track memory consumption throughout simulation
  --write-profile       Write complete profile data
  --help                Show this help message

Examples:
  nlm_profiler --step-count 5000 --neuron-count 500
  nlm_profiler --resolution 100 --output-profile my_profile.json
  nlm_profiler --detailed --timeline
