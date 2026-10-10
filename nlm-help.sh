#!/usr/bin/env bash
# NLM Command Line Interface - Improved Version

# Environment setup
export NLM_VERSION="0.1.0"
export NLM_PHASE="Phase 6: Final Integration"

# Color codes for output
export NLM_COLOR_GREEN="\033[1;32m"
export NLM_COLOR_BLUE="\033[1;34m"
export NLM_COLOR_YELLOW="\033[1;33m"
export NLM_COLOR_RED="\033[1;31m"
export NLM_COLOR_CYAN="\033[1;36m"
export NLM_COLOR_RESET="\033[0m"
export NLM_COLOR_BOLD="\033[1m"

# Helper functions
print_help() {
    cat << EOF
${NLM_COLOR_BOLD}${NLM_COLOR_GREEN}NLM - Neural Learning Machine${NLM_COLOR_RESET}
${NLM_COLOR_CYAN}${NLM_PHASE}${NLM_COLOR_RESET}

USAGE:
    ./nlm [OPTIONS] [CONFIG_FILE]
    ./nlm_phase3_demo [OPTIONS]
    ./nlm_phase4_demo [OPTIONS]

OPTIONS:
    ${NLM_COLOR_BLUE}General Options:${NLM_COLOR_RESET}
        --help, -h              Show this help message
        --version               Show version information
        --verbose, -v LEVEL      Set verbosity level (0=quiet, 1=normal, 2=verbose, 3=debug)
        --quiet, -q             Suppress all non-error output

    ${NLM_COLOR_BLUE}Configuration:${NLM_COLOR_RESET}
        --config FILE            Load configuration from file (default: configs/default.cfg)
        --config-arg KEY=VALUE   Set configuration value (overrides file)
        --create-config FILE     Create default configuration file

    ${NLM_COLOR_BLUE}Execution:${NLM_COLOR_RESET}
        --mode MODE              Execution mode (default: full)
                              Available modes:
                              - full        : Run complete integration test
                              - demo        : Run Phase 6 demo only
                              - benchmark   : Run performance benchmark
                              - test        : Run quick tests
                              - explore     : Show brain exploration

        --steps N                Number of simulation steps (default: 1000)
        --neurons N              Number of neurons (default: 1000)
        --regions N              Number of regions (default: 1)

    ${NLM_COLOR_BLUE}Output:${NLM_COLOR_RESET}
        --output FORMAT          Output format (default: text)
                              Available formats:
                              - text           : Human readable output
                              - json           : JSON format
                              - csv            : CSV format
                              - plot           : Generate plots

        --output-file FILE       Write output to file
        --plot-output DIR        Generate visualization plots to directory
        --silent                Suppress all output except errors

    ${NLM_COLOR_BLUE}Advanced Options:${NLM_COLOR_RESET}
        --checkpoint DIR         Enable checkpointing with directory
        --checkpoint-interval N  Checkpoint interval in steps
        --load-checkpoint FILE   Load from checkpoint
        --save-checkpoint FILE   Save checkpoint at end
        --no-replay              Disable memory replay
        --no-development         Disable developmental plasticity
        --no-structural         Disable structural plasticity
        --no-neuromodulation    Disable neuromodulation
        --no-prediction         Disable prediction system

    ${NLM_COLOR_BLUE}Performance:${NLM_COLOR_RESET}
        --threads N              Number of threads for parallel processing (default: 1)
        --no-optimization       Disable optimizations (for debugging)
        --profile                Enable performance profiling
        --benchmark-mode        Run quick benchmark before main execution

    ${NLM_COLOR_BLUE}Experiment Control:${NLM_COLOR_RESET}
        --experiment CONFIG      Run specific experiment
        --run-all                Run all available experiments
        --experiment-timeout N   Timeout for experiment (seconds)
        --dry-run                Parse arguments but don't run simulation

EXAMPLES:
    ./nlm --help                           # Show help
    ./nlm --version                        # Show version
    ./nlm --config my_config.cfg           # Use custom config
    ./nlm --mode demo --verbose 2          # Run demo with verbose output
    ./nlm --mode benchmark --neurons 5000   # Benchmark with 5000 neurons
    ./nlm --experiment-config exp_config.yaml --output json
    ./nlm --checkpoint ./checkpoints --load-checkpoint latest.bin

CONFIGURATION FILE FORMAT:
    # NLM Configuration File (JSON/YAML/INI format supported)
    # Values can be specified as key=value or as JSON/YAML objects

    # Simulation parameters
    neuron_count = 1000
    region_count = 1
    simulation_timestep = 0.001
    max_steps = 10000

    # Plasticity parameters
    stdp_ltp_weight = 0.01
    stdp_ltd_weight = 0.012
    stdp_tau = 20.0

    # Neuromodulation
    dopamine_level = 1.0
    curiosity_threshold = 0.5

    # Memory
    working_memory_capacity = 1000
    episodic_max_episodes = 1000

    # Development
    synaptogenesis_rate = 0.0001
    pruning_rate = 0.00001

For more information, visit: https://github.com/nlm-project/nlm
EOF
}