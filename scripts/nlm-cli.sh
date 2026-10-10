#!/bin/bash
# NLM Command Line Parser - Enhanced Version

NLM_CommandLineParser() {
    local args=()
    local verbose_level=1
    local config_file="configs/default.cfg"
    local config_args=()
    local execution_mode="full"
    local output_format="text"
    local output_file=""
    local plot_output=""
    local checkpoint_dir=""
    local checkpoint_interval=1000
    local load_checkpoint=""
    local save_checkpoint=""
    local enable_replay=true
    local enable_development=true
    local enable_structural=true
    local enable_neuromodulation=true
    local enable_prediction=true
    local num_threads=1
    local use_optimization=true
    local enable_profiling=false
    local benchmark_mode=false
    local experiment_config=""
    local run_all_experiments=false
    local experiment_timeout=3600
    local dry_run=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help|-h)
                echo "NLM v0.1.0 - Phase 6: Final Integration"
                echo "Usage: ./nlm [OPTIONS] [CONFIG_FILE]"
                echo ""
                echo "General Options:"
                echo "  --help, -h              Show this help message"
                echo "  --version               Show version information"
                echo "  --verbose, -v LEVEL      Set verbosity level (0=quiet, 1=normal, 2=verbose, 3=debug)"
                echo "  --quiet, -q             Suppress all non-error output"
                echo ""
                echo "Configuration:"
                echo "  --config FILE            Load configuration from file (default: configs/default.cfg)"
                echo "  --config-arg KEY=VALUE   Set configuration value (overrides file)"
                echo "  --create-config FILE     Create default configuration file"
                echo ""
                echo "Execution:"
                echo "  --mode MODE              Execution mode (default: full)"
                echo "                          Available modes: full, demo, benchmark, test, explore"
                echo "  --steps N                Number of simulation steps (default: 1000)"
                echo "  --neurons N              Number of neurons (default: 1000)"
                echo "  --regions N              Number of regions (default: 1)"
                echo ""
                echo "Output:"
                echo "  --output FORMAT          Output format (default: text)"
                echo "                          Available formats: text, json, csv, plot, quiet"
                echo "  --output-file FILE       Write output to file"
                echo "  --plot-output DIR        Generate visualization plots to directory"
                echo "  --silent                Suppress all output except errors"
                echo ""
                echo "Advanced Options:"
                echo "  --checkpoint DIR         Enable checkpointing with directory"
                echo "  --checkpoint-interval N  Checkpoint interval in steps"
                echo "  --load-checkpoint FILE   Load from checkpoint"
                echo "  --save-checkpoint FILE   Save checkpoint at end"
                echo "  --no-replay              Disable memory replay"
                echo "  --no-development         Disable developmental plasticity"
                echo "  --no-structural         Disable structural plasticity"
                echo "  --no-neuromodulation    Disable neuromodulation"
                echo "  --no-prediction         Disable prediction system"
                echo ""
                echo "Performance:"
                echo "  --threads N              Number of threads for parallel processing (default: 1)"
                echo "  --no-optimization       Disable optimizations (for debugging)"
                echo "  --profile                Enable performance profiling"
                echo "  --benchmark-mode        Run quick benchmark before main execution"
                echo ""
                echo "Experiment Control:"
                echo "  --experiment CONFIG      Run specific experiment"
                echo "  --run-all                Run all available experiments"
                echo "  --experiment-timeout N   Timeout for experiment (seconds)"
                echo "  --dry-run                Parse arguments but don't run simulation"
                exit 0
                ;;
            --version)
                echo "NLM v0.1.0 - Phase 6: Final Integration"
                echo "Build: $(date)"
                exit 0
                ;;
            --verbose)
                if [[ $# -gt 1 && $2 =~ ^[0-9]$ ]]; then
                    verbose_level=$2
                    shift
                else
                    echo "Error: Verbosity level required (0-3)"
                    exit 1
                fi
                ;;
            --quiet)
                verbose_level=0
                ;;
            --config)
                if [[ $# -gt 1 ]]; then
                    config_file=$2
                    shift
                else
                    echo "Error: Config file required"
                    exit 1
                fi
                ;;
            --create-config)
                if [[ $# -gt 1 ]]; then
                    output_file=$2
                    create_default_config "$output_file"
                    exit 0
                else
                    echo "Error: Output file required for --create-config"
                    exit 1
                fi
                ;;
            --mode)
                if [[ $# -gt 1 ]]; then
                    execution_mode=$2
                    shift
                else
                    echo "Error: Mode required (--mode MODE)"
                    exit 1
                fi
                ;;
            --steps)
                if [[ $# -gt 1 && $2 =~ ^[0-9]+$ ]]; then
                    simulation_steps=$2
                    shift
                else
                    echo "Error: Number of steps required (integer)"
                    exit 1
                fi
                ;;
            --neurons)
                if [[ $# -gt 1 && $2 =~ ^[0-9]+$ ]]; then
                    neuron_count=$2
                    shift
                else
                    echo "Error: Number of neurons required (integer)"
                    exit 1
                fi
                ;;
            --regions)
                if [[ $# -gt 1 && $2 =~ ^[0-9]+$ ]]; then
                    region_count=$2
                    shift
                else
                    echo "Error: Number of regions required (integer)"
                    exit 1
                fi
                ;;
            --output)
                if [[ $# -gt 1 ]]; then
                    output_format=$2
                    shift
                else
                    echo "Error: Output format required (--output FORMAT)"
                    exit 1
                fi
                ;;
            --output-file)
                if [[ $# -gt 1 ]]; then
                    output_file=$2
                    shift
                else
                    echo "Error: Output file required (--output-file FILE)"
                    exit 1
                fi
                ;;
            --silent)
                verbose_level=0
                output_format="quiet"
                ;;
            --checkpoint)
                if [[ $# -gt 1 ]]; then
                    checkpoint_dir=$2
                    enable_replay=true
                    shift
                else
                    echo "Error: Checkpoint directory required (--checkpoint DIR)"
                    exit 1
                fi
                ;;
            --checkpoint-interval)
                if [[ $# -gt 1 && $2 =~ ^[0-9]+$ ]]; then
                    checkpoint_interval=$2
                    shift
                else
                    echo "Error: Checkpoint interval required (integer)"
                    exit 1
                fi
                ;;
            --load-checkpoint)
                if [[ $# -gt 1 ]]; then
                    load_checkpoint=$2
                    shift
                else
                    echo "Error: Checkpoint file required (--load-checkpoint FILE)"
                    exit 1
                fi
                ;;
            --save-checkpoint)
                if [[ $# -gt 1 ]]; then
                    save_checkpoint=$2
                    shift
                else
                    echo "Error: Checkpoint file required (--save-checkpoint FILE)"
                    exit 1
                fi
                ;;
            --no-replay)
                enable_replay=false
                ;;
            --no-development)
                enable_development=false
                ;;
            --no-structural)
                enable_structural=false
                ;;
            --no-neuromodulation)
                enable_neuromodulation=false
                ;;
            --no-prediction)
                enable_prediction=false
                ;;
            --threads)
                if [[ $# -gt 1 && $2 =~ ^[0-9]+$ ]]; then
                    num_threads=$2
                    shift
                else
                    echo "Error: Number of threads required (integer)"
                    exit 1
                fi
                ;;
            --no-optimization)
                use_optimization=false
                ;;
            --profile)
                enable_profiling=true
                ;;
            --benchmark-mode)
                benchmark_mode=true
                ;;
            --experiment)
                if [[ $# -gt 1 ]]; then
                    experiment_config=$2
                    shift
                else
                    echo "Error: Experiment configuration required (--experiment CONFIG)"
                    exit 1
                fi
                ;;
            --run-all)
                run_all_experiments=true
                ;;
            --experiment-timeout)
                if [[ $# -gt 1 && $2 =~ ^[0-9]+$ ]]; then
                    experiment_timeout=$2
                    shift
                else
                    echo "Error: Timeout required (integer seconds)"
                    exit 1
                fi
                ;;
            --dry-run)
                dry_run=true
                ;;
            --*)
                echo "Error: Unknown option $1"
                echo "Use --help for usage information"
                exit 1
                ;;
            *)
                if [[ $1 != *.* ]]; then
                    echo "Error: Positional arguments not allowed (except config file)"
                    exit 1
                fi
                config_file=$1
                ;;
        esac
        shift
    done
    
    # Validate execution mode
    case $execution_mode in
        full|demo|benchmark|test|explore)
            # Valid modes
            ;;
        *)
            echo "Error: Invalid execution mode '$execution_mode'"
            echo "Valid modes: full, demo, benchmark, test, explore"
            exit 1
            ;;
    esac
    
    # Validate output format
    case $output_format in
        text|json|csv|plot|quiet)
            # Valid formats
            ;;
        *)
            echo "Error: Invalid output format '$output_format'"
            echo "Valid formats: text, json, csv, plot, quiet"
            exit 1
            ;;
    esac
    
    # Check if config file exists (unless creating one)
    if [[ ! -f "$config_file" && "$config_file" != "configs/default.cfg" ]]; then
        echo "Warning: Config file '$config_file' does not exist"
        if [[ "$1" != "" ]]; then
            config_file="$1"
        fi
    fi
    
    # Create config directory if needed
    mkdir -p "configs"
    
    # Create default config if it doesn't exist
    if [[ ! -f "configs/default.cfg" ]]; then
        create_default_config "configs/default.cfg"
        echo "Created default configuration: configs/default.cfg"
    fi
    
    # Apply command line config arguments
    for arg in "${config_args[@]}"; do
        if [[ $arg == *"="* ]]; then
            key=${arg%%=*}
            value=${arg#*=}
            echo "Applying config argument: $key=$value"
        fi
    done
    
    # Display parsed configuration
    echo "NLM Command Line Parser - Parsed Configuration:"
    echo "========================================"
    echo "Verbose Level: $verbose_level"
    echo "Config File: $config_file"
    echo "Execution Mode: $execution_mode"
    echo "Output Format: $output_format"
    echo "Checkpoint Directory: $checkpoint_dir"
    echo "Enable Replay: $enable_replay"
    echo "Enable Development: $enable_development"
    echo "Enable Structural Plasticity: $enable_structural"
    echo "Enable Neuromodulation: $enable_neuromodulation"
    echo "Enable Prediction: $enable_prediction"
    echo "Number of Threads: $num_threads"
    echo "Experiment Config: $experiment_config"
    echo "Run All Experiments: $run_all_experiments"
    echo "Experiment Timeout: $experiment_timeout"
    echo "Dry Run: $dry_run"
    echo "Output File: $output_file"
    echo "Plot Output: $plot_output"
    echo "Benchmark Mode: $benchmark_mode"
    echo "Profile: $enable_profiling"
    echo ""
    
    return 0
}

create_default_config() {
    local output_file="$1"
    cat << EOF > "$output_file"
# NLM Default Configuration File
# Generated on $(date)
# Edit this file to customize simulation parameters

# ==================== SIMULATION PARAMETERS ====================
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
max_steps = 10000
random_seed = 42
verbose_level = 1

# ==================== NEURAL DYNAMICS ====================
membrane_potential_rest = -70.0
membrane_potential_reset = -65.0
membrane_potential_threshold = -55.0
refractory_period = 5
membrane_capacitance = 1.0
time_constant = 20.0
leak_conductance = 0.001

# ==================== SYNAPTIC PROPERTIES ====================
connection_probability = 0.1
initial_weight_range = [0.1, 0.4]
delay_steps = 1
inhibitory_weight_range = [-0.1, -0.4]

# ==================== PLASTICITY RULES ====================
# STDP parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Hebbian learning
hebbian_learning_rate = 0.001
hebbian_time_constant = 100.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
structural_weight_range = [0.01, 0.1]

# ==================== NEUROMODULATION ====================
# Dopamine system
dopamine_level = 1.0
dopamine_sensitivity = 0.5
dopamine_decay = 0.99

# Novelty detection
novelty_threshold = 0.1
novelty_decay = 0.95

# Curiosity system
curiosity_level = 0.5
curiosity_threshold = 0.3
curiosity_decay = 0.98

# Prediction error
prediction_error_gain = 0.1
prediction_error_decay = 0.99

# ==================== MEMORY SYSTEMS ====================
# Working memory
working_memory_capacity = 1000
working_memory_decay = 0.9
working_memory_update_rate = 0.01

# Episodic memory
episodic_max_episodes = 1000
episodic_decay_rate = 0.01
episodic_consolidation_interval = 100
episodic_replay_frequency = 0.1

# Semantic memory
semantic_memory_capacity = 5000
semantic_similarity_threshold = 0.7

# Procedural memory
procedural_learning_rate = 0.001
procedural_forgetting_rate = 0.001

# Associative memory
associative_strength_threshold = 0.5
associative_decay = 0.98

# ==================== COGNITION SYSTEMS ====================
# Attention system
attention_excitation_strength = 1.5
attention_inhibition_strength = 0.5
attention_decay = 0.9
attention_focus_threshold = 0.8

# Neural planner
planner_planning_depth = 5
planner_action_selection_threshold = 0.7
planner_discount_factor = 0.95
planner_exploration_rate = 0.1

# Concept formation
concept_learning_rate = 0.001
concept_similarity_threshold = 0.6
concept_max_concepts = 1000
concept_pruning_threshold = 0.01

# ==================== DEVELOPMENTAL SYSTEM ====================
# Developmental stages
initial_plasticity_rate = 1.0
critical_period_plasticity = 0.8
maturation_plasticity = 0.5
adult_plasticity = 0.2

# Synaptogenesis and pruning
initial_synaptogenesis_rate = 0.001
initial_pruning_rate = 0.0001
development_stage_transition_steps = 1000

# Neural maturation
initial_excitability = 1.0
development_excitability_change = 0.01
development_threshold_change = 0.005

# ==================== ENVIRONMENT PARAMETERS ====================
# Sensory processing
visual_sensitivity = 1.0
audio_sensitivity = 1.0
internal_signal_gain = 1.0

# Motor system
max_action_rate = 10.0
movement_threshold = 0.5
action_selection_probability = 0.5

# Reward and punishment
reward_magnitude = 1.0
punishment_magnitude = -1.0
reward_decay = 0.99

# ==================== CHECKPOINTING ====================
checkpoint_enabled = true
checkpoint_interval = 1000
checkpoint_directory = ./checkpoints
checkpoint_compression = balanced
checkpoint_version = 1
max_checkpoints = 10

# ==================== EXPERIMENT PARAMETERS ====================
# Experiment runner
experiment_timeout = 3600
experiment_retry_attempts = 3
experiment_save_results = true

# Metrics collection
collect_metrics = true
metrics_update_frequency = 10
metrics_export_format = csv

# ==================== VISUALIZATION ====================
visualization_enabled = true
visualization_update_frequency = 100
visualization_output_format = png
visualization_dimensions = [1920, 1080]

# ==================== PERFORMANCE ====================
# Parallel processing
enable_multithreading = true
num_threads = 1

# Memory management
enable_memory_pooling = true
memory_pool_size = 10000
efficient_vector_allocation = true

# Optimization
optimization_level = 3
enable_vectorization = true
enable_loop_unrolling = true
enable_cache_optimization = true

# Debug and profiling
debug_mode = false
profile_memory_usage = false
profile_cpu_usage = false
profile_network_traffic = false

# ==================== MISCELLANEOUS ====================
# Logging
log_level = info
log_file = nlm.log
log_rotation_size = 10485760
log_keep_days = 30

# Randomness
random_number_generator = mt19937
random_seed_auto = false

# Safety limits
simulation_step_limit = 1000000
neuron_count_limit = 100000
synapse_count_limit = 1000000

# Error handling
error_reporting = true
error_stack_trace = true
error_continue_on_fatal = false
EOF
    echo "Created default configuration: $output_file"
}

# Execute main command line parsing
NLM_CommandLineParser "$@"