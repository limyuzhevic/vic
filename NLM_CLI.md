NLM (熙然) Neural Learning Machine - Unified Command Line Interface
Version 0.1.0

Copyright (C) 2026 NLM Authors
License: MIT

== NLM Phase 6: Unified Brain Simulation Interface ==

This is a comprehensive command-line interface for the NLM (熙然) experimental artificial brain project.
It integrates all functionality from previous phases into a unified, user-friendly interface
for both beginners and expert users.

== USAGE ==

nlm [mode] [options]

Common modes:
  demo                    Run integrated brain demonstration (recommended for beginners)
  experiment               Run specific experiments (expert mode)
  visualize                Open visualization dashboard
  benchmark                Run performance benchmarks
  help                    Show help information

Get detailed help for any command:
  nlm <command> --help

== MODES ==

=== DEMO (BEGINNER MODE) ===

The demo mode is designed for users new to NLM. It provides a simple, guided introduction
to the brain simulation with automated configuration and real-time feedback.

Basic command:
  nlm demo

Advanced demo options:
  nlm demo --episodes 10 --steps 500 --config custom_config.json
  nlm demo --quiet --progress
  nlm demo --save-checkpoints --load-checkpoint latest.bin

Options:
  --episodes N         Number of episodes to run (default: 5)
  --steps N            Steps per episode (default: 2000)
  --config FILE        Configuration file (JSON/YAML)
  --quiet              Suppress non-essential output
  --progress           Show progress indicators
  --save-checkpoints    Enable checkpoint saving
  --load-checkpoint F  Load checkpoint from file
  --enable-learning     Enable neuromodulation and plasticity
  --enable-curiosity    Enable curiosity-driven exploration
  --enable-development  Enable developmental processes
  --enable-visualization Enable real-time visualization (requires --display)
  --display            Use display for visualization (default: false)
  --checkpoint-dir DIR  Directory for checkpoint files (default: ./checkpoints)

=== EXPERIMENT (EXPERT MODE) ===

Expert mode provides full control over brain simulation parameters and integrates
specialized experiments from all previous phases.

Basic commands:
  nlm experiment phase3      Run Phase 3: World Interaction experiments
  nlm experiment phase4      Run Phase 4: Cognitive Emergent Abilities
  nlm experiment phase5      Run Phase 5: Performance & Architecture
  nlm experiment phase6      Run Phase 6: Integrated Brain Systems
  nlm experiment custom      Run custom configuration experiments

Experiment-specific options:
  --brain-config FILE     Brain configuration file
  --world-config FILE     World configuration file
  --experiment-config FILE Experiment-specific configuration
  --trials N             Number of trials (default: 100)
  --duration N           Simulation duration in seconds (default: 60)
  --checkpoint-dir DIR    Directory for checkpoint files
  --save-interval N      Save checkpoint every N steps (default: 1000)
  --resume               Resume from last checkpoint
  --profile              Enable performance profiling
  --debug                Enable debug logging

=== VISUALIZE ===

Interactive visualization and monitoring dashboard for real-time brain activity analysis.

Usage:
  nlm visualize                    Start real-time monitoring
  nlm visualize --fps 30            Set frame rate (default: 10)
  nlm visualize --duration 60       Run for specified duration
  nlm visualize --fullscreen        Full screen mode
  nlm visualize --port 8080         Web dashboard port (default: 8080)
  nlm visualize --host localhost    Web dashboard host

Visualization includes:
  - Real-time neural activity patterns
  - Memory system status (working, episodic, associative)
  - Neuromodulation signals (dopamine, curiosity, novelty)
  - Prediction system performance
  - Cognitive system activity
  - Development stage progression

=== BENCHMARK ===

Performance benchmarking and comparison tools for evaluating NLM performance
across different configurations and scenarios.

Usage:
  nlm benchmark                              Standard benchmark suite
  nlm benchmark --suite N                     Run specific benchmark suite (1-5)
  nlm benchmark --compare A,B                Compare configurations A and B
  nlm benchmark --output JSON|CSV|TABLE       Output format
  nlm benchmark --iterations N                Number of iterations (default: 5)
  nlm benchmark --config FILE                 Configuration file
  nlm benchmark --metrics N                    Comma-separated metrics (default: all)

Available benchmark suites:
  Suite 1: Basic Performance (Phase 2)
  Suite 2: Plasticity & Learning (Phase 3)
  Suite 3: Cognitive Emergence (Phase 4)
  Suite 4: Architecture Scaling (Phase 5)
  Suite 5: Integration Verification (Phase 6)

Available metrics:
  - neural_speed: Neural computation speed
  - memory_efficiency: Memory usage patterns
  - plasticity_rate: Synaptic change rates
  - prediction_accuracy: Prediction system performance
  - cognitive_emergence: Cognitive ability development
  - checkpoint_performance: Save/load efficiency
  - network_scalability: Network expansion capabilities
  - energy_efficiency: Power consumption optimization

== CONFIGURATION ==

=== CONFIGURATION FILES ===

NLM supports multiple configuration file formats:

JSON (.json): Modern, structured format
YAML (.yaml/.yml): Human-readable format
INI (.ini): Traditional format
Plain text (.cfg): Legacy format (key=value)

Example JSON configuration:
{
  "nlm_version": "0.1.0",
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001
  },
  "world": {
    "width": 20,
    "height": 20,
    "vision_width": 16,
    "vision_height": 16
  },
  "experiments": {
    "phase": 6,
    "enable_checkpoints": true,
    "enable_replay": true,
    "enable_development": true
  },
  "visualization": {
    "enabled": false,
    "port": 8080,
    "refresh_rate": 10
  }
}

Example YAML configuration:
```yaml
nlm_version: "0.1.0"
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  simulation_timestep: 0.001
world:
  width: 20
  height: 20
  vision_width: 16
  vision_height: 16
experiments:
  phase: 6
  enable_checkpoints: true
  enable_replay: true
  enable_development: true
visualization:
  enabled: false
  port: 8080
  refresh_rate: 10
```

=== CONFIGURATION PRESETS ===

Built-in configuration presets for common use cases:

Beginner presets:
  --preset beginner       Minimal configuration for learning and exploration
  --preset research      Research-oriented configuration with comprehensive logging
  --preset gaming        Optimized for interactive applications

Expert presets:
  --preset performance   Performance-optimized with SIMD and multi-threading
  --preset research      Research-grade configuration with all features enabled
  --preset production    Production-ready with checkpointing and error recovery

To use presets: nlm demo --preset beginner --episodes 20

== ADVANCED FEATURES ==

=== NEUROMODULATION CONTROL ===

Advanced control over neuromodulation systems:

  --dopamine LEVEL      Set dopamine level (0.0 to 2.0)
  --curiosity LEVEL    Set curiosity level (0.0 to 2.0)
  --novelty LEVEL      Set novelty detection level (0.0 to 2.0)
  --prediction-error    Enable prediction error tracking
  --reward-modulation  Enable reward-based plasticity

Example: nlm experiment phase6 --dopamine 1.5 --curiosity 0.8 --novelty 0.6

=== PLASTICITY CONTROL ===

Fine-grained control over synaptic plasticity:

  --stdp-weights LTP,LTD Configure STDP weights (comma-separated)
  --hebbian-strength N  Set Hebbian learning strength (0.0 to 2.0)
  --structural-rate N   Synaptogenesis/pruning rate (0.0 to 0.001)
  --eligibility-trace   Enable synaptic eligibility traces
  --replay-mode MODE    Memory replay mode (replay-only/consolidation)

Example: nlm experiment phase6 --stdp-weights 0.01,0.012 --hebbian-strength 1.5

=== DEVELOPMENT CONTROL ===

Control brain development processes:

  --developmental-stage STAGE      Set developmental stage (initial/critical/maturation/adult)
  --plasticity-modifier N          Developmental plasticity modifier (0.1 to 2.0)
  --critical-period-length N       Critical period duration in steps
  --maturation-threshold N         Maturity threshold

Example: nlm experiment phase6 --developmental-stage maturation --plasticity-modifier 0.7

=== MEMORY SYSTEM CONTROL ===

Advanced memory system configuration:

  --working-memory-capacity N    Working memory capacity in neurons
  --episodic-capacity N          Episodic memory capacity in episodes
  --episodic-decay RATE          Episodic memory decay rate (0.0 to 1.0)
  --associative-threshold N      Association formation threshold

Example: nlm experiment phase6 --working-memory-capacity 200 --episodic-capacity 500

=== PREDICTION SYSTEM CONTROL ===

Configure prediction and inference systems:

  --prediction-horizon N          Prediction time horizon in steps
  --prediction-confidence N       Prediction confidence threshold (0.0 to 1.0)
  --prediction-learning RATE      Prediction learning rate
  --reinforcement-learning RATE   Reinforcement learning rate

Example: nlm experiment phase6 --prediction-horizon 10 --prediction-confidence 0.8

=== COGNITION SYSTEM CONTROL ===

Configure cognitive and planning systems:

  --planning-depth N              Action planning depth (1 to 10)
  --attention-strength N          Attentional selection strength
  --concept-threshold N           Concept formation threshold
  --self-model-learning RATE      Self-model learning rate

Example: nlm experiment phase6 --planning-depth 5 --attention-strength 1.2

== OUTPUT & LOGGING ==

=== OUTPUT FORMATS ===

Multiple output formats for different use cases:

  --output-format TEXT            Plain text output (default)
  --output-format JSON             Machine-readable JSON output
  --output-format CSV              Comma-separated values
  --output-format YAML             YAML output

JSON output example:
{
  "experiment": "phase6",
  "total_reward": 15.7,
  "avg_firing_rate": 8.3,
  "memory_episodes_stored": 23,
  "performance_metrics": {
    "prediction_accuracy": 0.85,
    "planning_success": 0.72,
    "memory_retention": 0.91
  },
  "integration_status": {
    "working_memory": "CONNECTED",
    "episodic_memory": "CONNECTED",
    "neuromodulation": "CONNECTED"
  }
}

=== LOGGING LEVELS ===

Control logging verbosity:

  --log-level TRACE              Detailed trace logging
  --log-level DEBUG              Debug information
  --log-level INFO               Standard information (default)
  --log-level WARN               Warning messages only
  --log-level ERROR              Error messages only
  --log-level QUIET              No log output

=== STATISTICAL OUTPUT ===

Configure statistical analysis and reporting:

  --statistics SUMMARY            Basic statistics
  --statistics DETAILED          Detailed statistical analysis
  --statistics NONE              No statistics
  --stat-metrics N               Specific metrics to track
  --stat-interval N              Statistics calculation interval

Example: nlm experiment phase6 --statistics DETAILED --stat-interval 100

== ERROR HANDLING ==

=== ERROR MODES ===

Control error handling and recovery:

  --error-mode HALT              Stop on error (default)
  --error-mode CONTINUE          Continue despite errors
  --error-mode RETRY             Retry failed operations
  --error-mode GRACEFUL          Graceful degradation on errors

Example: nlm experiment phase6 --error-mode CONTINUE --resume

=== CHECKPOINT RECOVERY ===

Robust checkpoint system with recovery options:

  --checkpoint-retry N            Number of checkpoint retry attempts (default: 3)
  --checkpoint-timeout N          Checkpoint timeout in seconds (default: 30)
  --auto-recover                  Attempt automatic recovery from corrupted checkpoints
  --checkpoint-compression        Enable checkpoint compression

== INTERACTIVE FEATURES ==

=== REMOTE ACCESS ===

Enable remote monitoring and control:

  --remote-enable               Enable remote access
  --remote-host HOST            Remote host (default: localhost)
  --remote-port PORT            Remote port (default: 8080)
  --remote-password PASS       Remote access password
  --remote-api                  Enable API access

Example: nlm visualize --remote-enable --remote-host 0.0.0.0 --remote-port 8080

=== SIGNAL HANDLING ===

Handle system signals gracefully:

  --signal-handler ENABLE        Enable graceful signal handling
  --signal-handler RESTART       Restart on crash
  --signal-handler DUMP          Generate core dump on crash

Example: nlm experiment phase6 --signal-handler RESTART

== PERFORMANCE OPTIMIZATION ==

=== THREADING CONTROL ===

Control parallel processing:

  --threads N                   Number of CPU threads (0 = auto-detect, default: 0)
  --thread-affinity LIST        CPU affinity list
  --numa-optimization          Enable NUMA-aware scheduling
  --memory-pooling            Enable memory pooling for performance

Example: nlm experiment phase6 --threads 4 --memory-pooling

=== MEMORY MANAGEMENT ===

Control memory usage and allocation:

  --memory-limit N              Memory limit in MB (0 = unlimited, default: 0)
  --memory-optimize            Optimize memory usage
  --memory-monitor             Monitor memory usage

Example: nlm experiment phase6 --memory-limit 2048 --memory-monitor

=== CACHE MANAGEMENT ===

Configure caching strategies:

  --cache-enable                Enable caching
  --cache-size N                Cache size in MB (default: 256)
  --cache-policy POLICY         Cache eviction policy (LRU/LFU/ARC)
  --cache-warming               Enable cache warming

Example: nlm experiment phase6 --cache-size 512 --cache-policy LRU

== MIXED REALITY SUPPORT ==

=== VR/AR INTEGRATION ===

Support for mixed reality environments:

  --vr-mode                    Virtual reality mode
  --ar-mode                    Augmented reality mode
  --hololens                  Microsoft HoloLens support
  --unity-plugin               Unity plugin integration

Example: nlm visualize --vr-mode --fps 60

=== COLLABORATION FEATURES ===

Multi-user collaboration and sharing:

  --collaboration-mode         Enable collaborative mode
  --share-session              Share current session with others
  --remote-control             Enable remote control
  --session-logging           Enable session logging for sharing

Example: nlm experiment phase6 --collaboration-mode --share-session

== PLATFORM COMPATIBILITY ==

=== PLATFORM SPECIFIC OPTIONS ===

Platform-specific configuration:

macOS:
  --macos-metal              Use Metal for graphics acceleration
  --macos-coreml             Use CoreML for neural networks
  --macos-siri              Enable Siri integration

Linux:
  --linux-opengl            Use OpenGL for graphics
  --linux-vulkan            Use Vulkan for graphics
  --linux-wayland           Use Wayland display server

Windows:
  --windows-dx12            Use DirectX 12 for graphics
  --windows-uwp             Use UWP for app store
  --windows-mixed-reality  Mixed reality support

Example: nlm visualize --linux-opengl

== COMMAND ALIASES ==

Define custom command aliases:

  --alias name command         Create an alias for a command
  --list-aliases              List all defined aliases
  --remove-alias name          Remove an alias

Example: nlm demo --alias quick "demo --episodes 3 --steps 100"

== ENVIRONMENT VARIABLES ===

Environment variables for configuration:

  NLM_CONFIG_FILE              Path to configuration file
  NLM_LOG_LEVEL               Logging level (TRACE, DEBUG, INFO, WARN, ERROR, QUIET)
  NLM_HEADLESS                Headless mode (true/false)
  NLM_VISUALIZATION_PORT       Visualization server port
  NLM_REMOTE_PASSWORD         Remote access password
  NLM_PERFORMANCE_MODE         Performance optimization mode (balanced/fast/energy)

Example: export NLM_LOG_LEVEL=INFO && nlm demo

== COMPATIBILITY ==

This command-line interface is compatible with:
- NLM Phase 2: Full backward compatibility
- NLM Phase 3: Experimental interface extensions
- NLM Phase 4: Advanced features support
- NLM Phase 5: Performance optimizations
- NLM Phase 6: Complete integration

== EXAMPLES ==

=== BEGINNER EXAMPLES ===

Run a simple demonstration:
  nlm demo

Run with custom configuration:
  nlm demo --config my_config.json --episodes 10

Monitor brain activity:
  nlm visualize --fps 30 --duration 60

Save brain state:
  nlm demo --save-checkpoints --load-checkpoint latest.bin

=== INTERMEDIATE EXAMPLES ===

Run integrated brain experiment:
  nlm experiment phase6 --dopamine 1.5 --curiosity 0.8 --novelty 0.6

Run with remote access:
  nlm visualize --remote-enable --remote-host 0.0.0.0

Run with performance profiling:
  nlm experiment phase6 --profile --threads 4

=== ADVANCED EXAMPLES ===

Run comparative experiments:
  nlm benchmark --compare config1,config2,config3

Run with custom plasticity rules:
  nlm experiment phase6 --stdp-weights 0.01,0.015 --hebbian-strength 1.2

Run with advanced development control:
  nlm experiment phase6 --developmental-stage maturation --plasticity-modifier 0.7

=== SCRIPTING EXAMPLES ===

Python script integration:
```python
import subprocess
import json

# Run NLM with custom configuration
config = {
    "brain": {"neuron_count": 500},
    "experiments": {"phase": 6}
}

with open("config.json", "w") as f:
    json.dump(config, f)

subprocess.run([
    "nlm", "demo", "--config", "config.json",
    "--episodes", "5", "--steps", "1000"
])
```

Bash script integration:
```bash
#!/bin/bash

echo "Starting NLM brain simulation..."
nlm demo --episodes 3 --steps 500 --quiet

if [ $? -eq 0 ]; then
    echo "Simulation completed successfully"
    nlm visualize --fps 10 &
    sleep 30
    kill $!
else
    echo "Simulation failed"
    exit 1
fi
```

=== BEST PRACTICES ===

- Always specify --help for new commands
- Use configuration files for complex setups
- Enable checkpointing for long-running experiments
- Monitor system resources during extended runs
- Use remote access for distributed testing
- Always backup important configuration files

== TROUBLESHOOTING ==

=== COMMON ISSUES ===

If you encounter problems:

1. Configuration file not found:
   Check file path and permissions

2. Visualization not working:
   Ensure display is available or use --remote-enable

3. Performance issues:
   Reduce thread count or enable memory optimization

4. Checkpoint errors:
   Check disk space or try --checkpoint-compression

5. Remote access issues:
   Verify network connectivity and firewall settings

For detailed troubleshooting:
  nlm help troubleshooting

== ACKNOWLEDGMENTS ==

This unified command-line interface integrates functionality from:
- NLM Phase 2: Real neural computation
- NLM Phase 3: World interaction experiments
- NLM Phase 4: Cognitive emergence experiments
- NLM Phase 5: Performance optimizations
- NLM Phase 6: Complete brain integration

Special thanks to the NLM research team for their contributions to this project.

== VERSION INFORMATION ==

NLM CLI v0.1.0
Built with: CMake 3.16+, C++20
Python bindings: pybind11
Visualization: OpenGL/Vulkan
Performance: SIMD, threading, memory pooling

For more information:
  nlm help
  nlm help documentation
  https://nlm.readthedocs.io
  https://github.com/nlm-project/nlm

== END OF CLI GUIDE ==

This guide is continuously updated. For the latest features and improvements:
  Check the official NLM documentation
  Visit the NLM GitHub repository
  Join the NLM community forums
