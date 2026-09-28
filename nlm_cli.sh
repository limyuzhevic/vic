# Parse command line arguments
parse_arguments() {
    local experiment_type="$1"
    shift
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -q|--quiet)
                QUIET=true
                shift
                ;;
            --config)
                if [[ $# -gt 1 ]]; then
                    DEFAULT_CONFIG="$2"
                    shift 2
                else
                    echo "Error: --config requires a file argument"
                    exit 1
                fi
                ;;
            --save-config)
                if [[ $# -gt 1 ]]; then
                    EXPORT_CONFIG="$2"
                    shift 2
                else
                    echo "Error: --save-config requires a file argument"
                    exit 1
                fi
                ;;
            --parse-config)
                PARSE_CONFIG=true
                shift
                ;;
            --export-config)
                if [[ $# -gt 1 ]]; then
                    EXPORT_CONFIG="$2"
                    shift 2
                else
                    echo "Error: --export-config requires a file argument"
                    exit 1
                fi
                ;;
            --timestep)
                if [[ $# -gt 1 ]]; then
                    TIMESTEP="$2"
                    shift 2
                else
                    echo "Error: --timestep requires a value"
                    exit 1
                fi
                ;;
            --timesteps)
                if [[ $# -gt 1 ]]; then
                    TIMESTEPS="$2"
                    shift 2
                else
                    echo "Error: --timesteps requires a value"
                    exit 1
                fi
                ;;
            --world-width)
                if [[ $# -gt 1 ]]; then
                    WORLD_WIDTH="$2"
                    shift 2
                else
                    echo "Error: --world-width requires a value"
                    exit 1
                fi
                ;;
            --world-height)
                if [[ $# -gt 1 ]]; then
                    WORLD_HEIGHT="$2"
                    shift 2
                else
                    echo "Error: --world-height requires a value"
                    exit 1
                fi
                ;;
            --vision-width)
                if [[ $# -gt 1 ]]; then
                    VISION_WIDTH="$2"
                    shift 2
                else
                    echo "Error: --vision-width requires a value"
                    exit 1
                fi
                ;;
            --vision-height)
                if [[ $# -gt 1 ]]; then
                    VISION_HEIGHT="$2"
                    shift 2
                else
                    echo "Error: --vision-height requires a value"
                    exit 1
                fi
                ;;
            --neuron-count)
                if [[ $# -gt 1 ]]; then
                    NEURON_COUNT="$2"
                    shift 2
                else
                    echo "Error: --neuron-count requires a value"
                    exit 1
                fi
                ;;
            --region-count)
                if [[ $# -gt 1 ]]; then
                    REGION_COUNT="$2"
                    shift 2
                else
                    echo "Error: --region-count requires a value"
                    exit 1
                fi
                ;;
            --connection-prob)
                if [[ $# -gt 1 ]]; then
                    CONNECTION_PROBABILITY="$2"
                    shift 2
                else
                    echo "Error: --connection-prob requires a value"
                    exit 1
                fi
                ;;
            --seed)
                if [[ $# -gt 1 ]]; then
                    RANDOM_SEED="$2"
                    shift 2
                else
                    echo "Error: --seed requires a value"
                    exit 1
                fi
                ;;
            --enable-learning)
                ENABLE_LEARNING=true
                shift
                ;;
            --disable-learning)
                ENABLE_LEARNING=false
                shift
                ;;
            --enable-development)
                ENABLE_DEVELOPMENT=true
                shift
                ;;
            --disable-development)
                ENABLE_DEVELOPMENT=false
                shift
                ;;
            --enable-curiosity)
                ENABLE_CURIOSITY=true
                shift
                ;;
            --disable-curiosity)
                ENABLE_CURIOSITY=false
                shift
                ;;
            --enable-struct-plat)
                ENABLE_STRUCTURE_PLAT=true
                shift
                ;;
            --disable-struct-plat)
                ENABLE_STRUCTURE_PLAT=false
                shift
                ;;
            --simd)
                if [[ $# -gt 1 ]]; then
                    SIMD_TYPE="$2"
                    shift 2
                else
                    echo "Error: --simd requires a value"
                    exit 1
                fi
                ;;
            --threads)
                if [[ $# -gt 1 ]]; then
                    THREAD_COUNT="$2"
                    shift 2
                else
                    echo "Error: --threads requires a value"
                    exit 1
                fi
                ;;
            --memory-pool)
                USE_MEMORY_POOL=true
                shift
                ;;
            --disable-memory-pool)
                USE_MEMORY_POOL=false
                shift
                ;;
            --soa)
                USE_SOA=true
                shift
                ;;
            --disable-soa)
                USE_SOA=false
                shift
                ;;
            --event-queue)
                USE_EVENT_QUEUE=true
                shift
                ;;
            --disable-event-queue)
                USE_EVENT_QUEUE=false
                shift
                ;;
            --sparse-connect)
                USE_SPARSE_CONNECTIVITY=true
                shift
                ;;
            --disable-sparse-connect)
                USE_SPARSE_CONNECTIVITY=false
                shift
                ;;
            --multithreading)
                USE_MULTITHREADING=true
                shift
                ;;
            --disable-multithreading)
                USE_MULTITHREADING=false
                shift
                ;;
            --log-level)
                if [[ $# -gt 1 ]]; then
                    LOG_LEVEL="$2"
                    shift 2
                else
                    echo "Error: --log-level requires a value"
                    exit 1
                fi
                ;;
            --log-file)
                if [[ $# -gt 1 ]]; then
                    EXPERIMENT_LOG_FILE="$2"
                    shift 2
                else
                    echo "Error: --log-file requires a value"
                    exit 1
                fi
                ;;
            --benchmark)
                BENCHMARK=true
                shift
                ;;
            --bench-file)
                if [[ $# -gt 1 ]]; then
                    EXPERIMENT_LOG_FILE="$2"
                    shift 2
                else
                    echo "Error: --bench-file requires a value"
                    exit 1
                fi
                ;;
            --save-brain)
                if [[ $# -gt 1 ]]; then
                    SAVE_BRAIN="$2"
                    shift 2
                else
                    echo "Error: --save-brain requires a value"
                    exit 1
                fi
                ;;
            --load-brain)
                if [[ $# -gt 1 ]]; then
                    LOAD_BRAIN="$2"
                    shift 2
                else
                    echo "Error: --load-brain requires a value"
                    exit 1
                fi
                ;;
            --no-save-brain)
                SAVE_BRAIN=false
                shift
                ;;
            --continue-if-no-file)
                CONTINUE_IF_NO_FILE=true
                shift
                ;;
            --visualization)
                if [[ $# -gt 1 ]]; then
                    VISUALIZATION="$2"
                    shift 2
                else
                    echo "Error: --visualization requires a value"
                    exit 1
                fi
                ;;
            --viz-interval)
                if [[ $# -gt 1 ]]; then
                    VIZ_INTERVAL="$2"
                    shift 2
                else
                    echo "Error: --viz-interval requires a value"
                    exit 1
                fi
                ;;
            --viz-file)
                if [[ $# -gt 1 ]]; then
                    VIZ_FILE="$2"
                    shift 2
                else
                    echo "Error: --viz-file requires a value"
                    exit 1
                fi
                ;;
            --output-format)
                if [[ $# -gt 1 ]]; then
                    OUTPUT_FORMAT="$2"
                    shift 2
                else
                    echo "Error: --output-format requires a value"
                    exit 1
                fi
                ;;
            --export-metrics)
                if [[ $# -gt 1 ]]; then
                    EXPORT_METRICS="$2"
                    shift 2
                else
                    echo "Error: --export-metrics requires a value"
                    exit 1
                fi
                ;;
            --metrics-format)
                if [[ $# -gt 1 ]]; then
                    METRICS_FORMAT="$2"
                    shift 2
                else
                    echo "Error: --metrics-format requires a value"
                    exit 1
                fi
                ;;
            --stats-interval)
                if [[ $# -gt 1 ]]; then
                    STATS_INTERVAL="$2"
                    shift 2
                else
                    echo "Error: --stats-interval requires a value"
                    exit 1
                fi
                ;;
            --experiment-type)
                if [[ $# -gt 1 ]]; then
                    EXPERIMENT_TYPE="$2"
                    shift 2
                else
                    echo "Error: --experiment-type requires a value"
                    exit 1
                fi
                ;;
            --episodes)
                if [[ $# -gt 1 ]]; then
                    EPISODES="$2"
                    shift 2
                else
                    echo "Error: --episodes requires a value"
                    exit 1
                fi
                ;;
            --steps-per-episode)
                if [[ $# -gt 1 ]]; then
                    STEPS_PER_EPISODE="$2"
                    shift 2
                else
                    echo "Error: --steps-per-episode requires a value"
                    exit 1
                fi
                ;;
            --environment-type)
                if [[ $# -gt 1 ]]; then
                    ENVIRONMENT_TYPE="$2"
                    shift 2
                else
                    echo "Error: --environment-type requires a value"
                    exit 1
                fi
                ;;
            --goal-behaviors)
                GOAL_BEHAVIORS=true
                shift
                ;;
            --decay-rewards)
                DECAY_REWARDS=true
                shift
                ;;
            --enable-planning)
                ENABLE_PLANNING=true
                shift
                ;;
            --enable-attention)
                ENABLE_ATTENTION=true
                shift
                ;;
            --enable-concept)
                ENABLE_CONCEPT=true
                shift
                ;;
            --enable-prediction)
                ENABLE_PREDICTION=true
                shift
                ;;
            --enable-memory)
                ENABLE_MEMORY=true
                shift
                ;;
            --enable-neuromod)
                ENABLE_NEUROMOD=true
                shift
                ;;
            --enable-cognition)
                ENABLE_COGNITION=true
                shift
                ;;
            --integration-test)
                INTEGRATION_TEST=true
                shift
                ;;
            --custom-script)
                if [[ $# -gt 1 ]]; then
                    CUSTOM_SCRIPT="$2"
                    shift 2
                else
                    echo "Error: --custom-script requires a value"
                    exit 1
                fi
                ;;
            --script-args)
                if [[ $# -gt 1 ]]; then
                    SCRIPT_ARGS="$2"
                    shift 2
                else
                    echo "Error: --script-args requires a value"
                    exit 1
                fi
                ;;
            --experiment)
                if [[ $# -gt 1 ]]; then
                    EXPERIMENT_TYPE="$2"
                    shift 2
                else
                    echo "Error: --experiment requires a value"
                    exit 1
                fi
                ;;
            *)
                echo "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Help function
show_help() {
    cat << 'EOF'
NLM - Neural Learning Machine Command Line Interface
========================================================

USAGE:
    nlm [OPTIONS] [EXPERIMENT] [ARGS]

EXPERIMENTS:
    basic                 Run basic neural computation demo
    agent                 Run complete agent-environment interaction
    learning              Run learning experiment with plasticity
    phase3                Run Phase 3 world interaction demo (default)
    phase4                Run Phase 4 cognition demo
    phase6                Run Phase 6 integration test
    multiagent           Run multi-agent competition
    benchmark             Run performance benchmark

GLOBAL OPTIONS:
    -h, --help            Show this help message
    -v, --verbose         Enable verbose output
    -q, --quiet           Suppress non-error output
    --config FILE         Load configuration from file
    --save-config FILE    Save current configuration to file
    --parse-config        Parse and validate configuration
    --export-config FILE  Export configuration to JSON/YAML
    --timestep FLOAT      Simulation timestep (default: 0.001)
    --timesteps INT      Number of simulation steps (default: 1000)
    --world-width INT    World width (default: 20)
    --world-height INT   World height (default: 20)
    --vision-width INT   Vision width (default: 16)
    --vision-height INT  Vision height (default: 16)
    --neuron-count INT   Neuron count (default: 500)
    --region-count INT   Region count (default: 2)
    --connection-prob FLOAT   Connection probability (default: 0.1)
    --seed INT           Random seed (default: 42)

LEARNING OPTIONS:
    --enable-learning     Enable reward-based learning (default: enabled)
    --disable-learning    Disable learning
    --enable-development  Enable brain development (default: enabled)
    --disable-development Disable development
    --enable-curiosity    Enable curiosity-driven exploration (default: enabled)
    --disable-curiosity   Disable curiosity
    --enable-struct-plat Enable structural plasticity (default: enabled)
    --disable-struct-plat Disable structural plasticity

PERFORMANCE OPTIONS:
    --simd TYPE         SIMD instruction set (avx2, avx512, sse, scalar)
    --threads INT       Number of threads for parallel processing (default: 1)
    --memory-pool       Enable memory pool optimization
    --disable-memory-pool Disable memory pool optimization
    --soa               Enable Structure-of-Arrays optimization
    --disable-soa        Disable Structure-of-Arrays optimization
    --event-queue       Enable event queue optimization
    --disable-event-queue Disable event queue optimization
    --sparse-connect    Enable sparse connectivity
    --disable-sparse-connect Disable sparse connectivity
    --multithreading    Enable multithreading
    --disable-multithreading Disable multithreading

LOGGING OPTIONS:
    --log-level LEVEL   Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
    --log-file FILE     Log output to file in addition to console
    --benchmark         Enable performance benchmarking
    --bench-file FILE   Save benchmark results to file

EXECUTION OPTIONS:
    --save-brain FILE    Save final brain state to file
    --load-brain FILE    Load brain state from file before execution
    --no-save-brain      Disable brain state saving
    --continue-if-no-file Continue even if brain file not found (for loading)

VISUALIZATION OPTIONS:
    --visualization TYPE Visualization output type (console, file, network, animation)
    --viz-interval INT  Visualization update interval (default: 100)
    --viz-file FILE     Output visualization to file

OUTPUT OPTIONS:
    --output-format TYPE Output format (console, json, xml, yaml)
    --export-metrics FILE Export experiment metrics to file
    --metrics-format TYPE Format for metrics export (csv, json, sqlite)
    --stats-interval INT Interval for statistics output (default: 100)

EXPERIMENT-SPECIFIC OPTIONS:

Phase 3 Options (--experiment phase3):
    --episodes INT     Number of episodes (default: 5)
    --steps-per-episode INT Steps per episode (default: 200)
    --environment-type TYPE Environment type (maze, grid, maze_plus_obstacles)
    --goal-behaviors     Enable goal-directed behaviors
    --decay-rewards      Decay rewards over time

Phase 4 Options (--experiment phase4):
    --enable-planning    Enable neural planning
    --enable-attention   Enable attentional selection
    --enable-concept     Enable concept formation
    --enable-prediction  Enable prediction system

Phase 6 Options (--experiment phase6):
    --enable-memory      Enable all memory systems
    --enable-neuromod      Enable all neuromodulation systems
    --enable-cognition   Enable all cognition systems
    --integration-test   Run integration verification tests

CUSTOM EXPERIMENT:
    --custom-script FILE Run custom experiment script
    --script-args "ARGS" Arguments for custom script

EXAMPLES:
    # Run basic demo with custom settings
    nlm basic --neuron-count 1000 --connection-prob 0.05 --enable-curiosity

    # Run learning experiment with specific configuration
    nlm learning --timestep 0.01 --timesteps 5000 --enable-development

    # Run Phase 3 demo with detailed logging
    nlm phase3 --episodes 10 --steps-per-episode 500 --log-file phase3.log --log-level DEBUG

    # Run multi-agent competition
    nlm multiagent --threads 4 --memory-pool --soa

    # Save configuration for later use
    nlm phase3 --save-config my_experiment.cfg

    # Export configuration to JSON for analysis
    nlm phase3 --export-config experiment_config.json

    # Run benchmark to compare performance
    nlm benchmark --simd avx2 --threads 8 --memory-pool --soa --event-queue

EOF
    exit 0
}