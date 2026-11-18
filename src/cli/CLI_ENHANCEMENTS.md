// NLM Command Line Interface Enhancements
// This document describes the command-line options for the NLM demo executable
// and the enhanced control capabilities available for users and developers.

Command Line Options:

1. Basic Options
   --help, -h          Show help information and available options
   --version           Show version information
   --verbose, -v       Enable verbose logging output
   --quiet, -q         Suppress informational messages
   --log-level LEVEL   Set logging level (Debug, Info, Warning, Error, Critical)

2. Configuration Options
   --config=FILE       Load configuration from file
   --set KEY=VALUE     Set a configuration value
   --reset-config      Reset to default configuration

3. Simulation Control Options
   --steps=N           Number of simulation steps to run
   --timestep=DT       Set simulation timestep (in seconds)
   --neurons=N         Set number of neurons
   --regions=N         Set number of regions
   --seed=SEED         Set random seed for reproducibility

4. Test Options
   --test-mode MODE    Run specific test mode (connectivity, plasticity, stdp, learning)
   --test-steps=N      Number of steps for each test
   --compare-results   Compare results with baseline expectations

5. Output Options
   --output-dir DIR    Directory for output files (checkpoints, logs)
   --save-checkpoint   Save checkpoint at end of simulation
   --load-checkpoint=FILE Load checkpoint from file
   --export-stats      Export statistics to file
   --visualize         Generate visualization data

6. Advanced Options
   --enable-plasticity Enable/disable plasticity mechanisms
   --enable-development Enable/disable developmental processes
   --enable-curiosity   Enable/disable curiosity-driven exploration
   --enable-reward     Enable/disable reward-based learning
   --profile           Enable performance profiling
   --benchmark         Run benchmark tests

Implementation Details:

The main.cpp has been refactored to use the demo system with enhanced command-line parsing.
The enhanced CLI provides:

- Comprehensive option parsing with validation
- Default configuration management
- Flexible configuration overriding
- Detailed help and usage information
- Progress reporting and status indicators
- Error handling and recovery

Example Usage:

# Basic usage with default configuration
./nlm_demo

# Load custom configuration
./nlm_demo --config=my_config.cfg --verbose

# Run specific test mode
./nlm_demo --test-mode=plasticity --steps=1000 --save-checkpoint

# Set custom parameters
./nlm_demo --neurons=1000 --timestep=0.0005 --seed=12345

# Run with output and checkpointing
./nlm_demo --output-dir=./results --save-checkpoint --load-checkpoint=saved.brain

Future Enhancements:

1. Interactive command-line interface
2. Tab completion and auto-suggestions
3. Configuration validation and schema checking
4. Real-time parameter adjustment
5. Remote control interface
6. Command history and scripting support
