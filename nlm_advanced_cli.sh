#!/usr/bin/env bash

# NLM Advanced Command Line Interface
# Enhanced version with comprehensive configuration options and advanced features

NAME="nlm_advanced"
VERSION="2.0.0"

# Default configuration values
DEFAULT_NEURON_COUNT=1000
DEFAULT_REGION_COUNT=1
DEFAULT_CONNECTION_PROBABILITY=0.1
DEFAULT_SIMULATION_TIMESTEP=0.001
DEFAULT_MAX_STEPS=10000
DEFAULT_CHECKPOINT_INTERVAL=10000
DEFAULT_ENABLE_DEVELOPMENT=true
DEFAULT_ENABLE_CHECKPOINTING=true
DEFAULT_ENABLE_REPLAY=true
DEFAULT_ENABLE_PLANNING=true
DEFAULT_ENABLE_CONCEPT_FORMATION=true
DEFAULT_PLANNING_DEPTH=5
DEFAULT_LOG_LEVEL="INFO"
DEFAULT_OUTPUT_FORMAT="text"
DEFAULT_VISUALIZATION=false

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_debug() {
    if [[ "$LOG_LEVEL" == "DEBUG" ]]; then
        echo -e "${CYAN}[DEBUG]${NC} $1"
    fi
}

# Display help information
usage() {
    cat << EOF
NLM Advanced Command Line Interface - Neural Learning Machine

Usage: $NAME [OPTIONS] [ARGS...]

Basic Options:
  --help, -h                    Show this help message
  --version                     Show version information
  --config FILE                 Load configuration from file
  --output-dir DIR              Set output directory for checkpoints and logs

Simulation Configuration:
  --neuron-count N              Number of neurons (default: $DEFAULT_NEURON_COUNT)
  --region-count R               Number of neural regions (default: $DEFAULT_REGION_COUNT)
  --connection-prob P            Connection probability (default: $DEFAULT_CONNECTION_PROBABILITY)
  --timestep T                  Simulation timestep (default: $DEFAULT_SIMULATION_TIMESTEP)
  --max-steps S                 Maximum simulation steps (default: $DEFAULT_MAX_STEPS)
  --simulation-mode MODE         Simulation mode: 'phase6', 'phase2', 'custom' (default: phase6)

Memory Systems:
  --enable-working-memory         Enable working memory system (default: true)
  --enable-episodic-memory       Enable episodic memory system (default: true)
  --enable-associative-memory     Enable associative memory system (default: false)
  --working-memory-capacity C    Working memory capacity in items (default: 100)
  --episodic-memory-capacity E   Maximum episodic memory episodes (default: 1000)

Cognitive Systems:
  --enable-planner               Enable neural planner (default: $DEFAULT_ENABLE_PLANNING)
  --enable-concept-formation     Enable concept formation (default: $DEFAULT_ENABLE_CONCEPT_FORMATION)
  --enable-attention             Enable attention system (default: true)
  --planning-depth D             Planning depth for neural planner (default: $DEFAULT_PLANNING_DEPTH)
  --attention-mode MODE          Attention mode: 'competitive', 'excitatory', 'inhibitory' (default: competitive)

Neuromodulation:
  --enable-dopamine             Enable dopamine neuromodulation (default: true)
  --enable-curiosity             Enable curiosity neuromodulation (default: true)
  --enable-novelty               Enable novelty detection (default: true)
  --enable-serotonin            Enable serotonin neuromodulation (default: false)
  --enable-norepinephrine        Enable norepinephrine neuromodulation (default: false)
  --enable- acetylcholine       Enable acetylcholine neuromodulation (default: false)
  --dopamine-baseline B         Dopamine baseline level (default: 0.1)
  --dopamine-peak P              Dopamine peak level (default: 1.0)

Development:
  --enable-development           Enable developmental stages (default: $DEFAULT_ENABLE_DEVELOPMENT)
  --development-rate RATE         Developmental rate (0.0-1.0, default: 0.01)
  --stage-initial-AGE           Age for initial stage (steps, default: 60)
  --stage-critical AGE           Age for critical period (steps, default: 300)
  --stage-maturation AGE         Age for maturation stage (steps, default: 900)

Persistence:
  --enable-checkpointing         Enable checkpointing (default: $DEFAULT_ENABLE_CHECKPOINTING)
  --checkpoint-interval I        Checkpoint save interval (steps, default: $DEFAULT_CHECKPOINT_INTERVAL)
  --checkpoint-dir DIR           Checkpoint directory (default: ./checkpoints)
  --max-checkpoints N            Maximum checkpoints to keep (default: 5)
  --checkpoint-compress          Enable checkpoint compression (default: true)
  --enable-replay                Enable memory replay (default: $DEFAULT_ENABLE_REPLAY)
  --replay-interval I            Replay interval (steps, default: 100)

Advanced Options:
  --log-level LEVEL              Logging level: DEBUG, INFO, WARN, ERROR (default: $DEFAULT_LOG_LEVEL)
  --output-format FORMAT          Output format: text, json, csv (default: $DEFAULT_OUTPUT_FORMAT)
  --enable-visualization          Enable visualization mode (default: $DEFAULT_VISUALIZATION)
  --visualization-port PORT       Visualization server port (default: 8080)
  --enable-metrics                Enable metrics collection (default: true)
  --metrics-interval INTERVAL     Metrics collection interval (steps, default: 1000)
  --profile-mode ENABLE           Enable profiling mode (default: false)
  --random-seed SEED              Random seed for reproducible simulations (default: auto)

Experiment Configuration:
  --experiment-mode MODE          Experiment mode: 'standard', 'ablation', 'scaling', 'continual' (default: standard)
  --ablation-target SYSTEM       Ablation target: 'memory', 'cognition', 'neuromodulation', 'all' (default: none)
  --scaling-factor FACTOR        Scaling factor for ablative experiments (default: 1.0)
  --continual-learning            Enable continual learning (default: false)
  --catastrophic-forgetting        Simulate catastrophic forgetting (default: false)

Multi-Agent:
  --multi-agent                  Enable multi-agent simulation (default: false)
  --agent-count COUNT             Number of agents (default: 2)
  --agent-interaction            Enable inter-agent communication (default: true)
  --agent-sharing                Share resources between agents (default: false)

Specialized Arguments (requires --simulation-mode custom):
  --custom-param NAME VALUE      Custom parameter for specialized simulation

Examples:
  $NAME --neuron-count 2000 --enable-planner --enable-concept-formation
  $NAME --simulation-mode phase6 --enable-checkpointing --checkpoint-dir ./simulations
  $NAME --experiment-mode ablation --ablation-target memory --scaling-factor 0.5
  $NAME --multi-agent --agent-count 5 --agent-interaction

For more information, visit: https://github.com/nlm-project/nlm
EOF
}