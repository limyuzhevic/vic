#!/bin/bash
# NLM Command Line Interface
# Enhanced command line interface for NLM Phase 6 integrated artificial brain

# Print help information
print_help() {
    echo "NLM Phase 6 - Integrated Artificial Brain"
    echo "======================================"
    echo ""
    echo "USAGE: nlm [OPTIONS] [ARGS...]"
    echo ""
    echo "Core Options:"
    echo "  --help, -h              Show this help message"
    echo "  --version, -v           Show version information"
    echo "  --config FILE, -c FILE   Load configuration from FILE"
    echo "  --neurons COUNT, -n CNT Set number of neurons (default: 1000)"
    echo "  --regions COUNT, -r CNT Set number of brain regions (default: 1)"
    echo "  --timestep MS, -t MS     Set simulation timestep in milliseconds (default: 1.0)"
    echo "  --seed NUM, -s NUM       Set random seed (default: 42)"
    echo ""
    echo "Advanced Options:"
    echo "  --checkpoint FILE, -k FILE Enable checkpointing with file path"
    echo "  --replay, -R             Enable memory replay functionality"
    echo "  --development, -D        Enable developmental plasticity"
    echo "  --memory-only           Run with minimal neural activity (test memory systems)"
    echo "  --cognition-only        Run with minimal neural activity (test cognition systems)"
    echo "  --neuromodulation-only   Run with minimal neural activity (test neuromodulation)"
    echo "  --simulation STEPS, -m STEPS Run only STEPS simulation steps (default: 10000)"
    echo ""
    echo "Performance Options:"
    echo "  --threads NUM, -T NUM    Set number of threads for parallel processing"
    echo "  --sparse, -S             Enable sparse connectivity optimization"
    echo "  --vectorize, -V          Enable SIMD vectorization"
    echo ""
    echo "Experiment Options:"
    echo "  --experiment MODE, -e MODE Run specific experiment (memory, cognition, neuromodulation, integration)"
    echo "  --validate, -V           Run integration validation tests only"
    echo ""
    echo "Persistence Options:"
    echo "  --save FILE, -S FILE     Save brain state to FILE and exit"
    echo "  --load FILE, -L FILE     Load brain state from FILE and continue"
    echo ""
    echo "Output Options:"
    echo "  --quiet, -q             Reduce output verbosity"
    echo "  --debug, -d             Enable debug logging"
    echo "  --benchmark, -b          Run performance benchmarks"
    echo ""
    echo "Advanced Configuration:"
    echo "  --config-help           Show all available configuration options"
    echo "  --preset NAME, -P NAME    Load preset configuration (fast, balanced, deep)"
    echo ""
    echo "Examples:"
    echo "  nlm --help                    Show help"
    echo "  nlm --neurons 500 --regions 2  Create small brain"
    echo "  nlm --checkpoint brain.bin --save Load/saved brain state"
    echo "  nlm --experiment integration   Run integration validation"
    echo "  nlm --preset fast             Quick test with minimal configuration"
    echo ""
    echo "For more information, visit: https://github.com/nlm-project/nlm"
}

# Print version information
print_version() {
    echo "NLM Phase 6: Final Integration"
    echo "Version: 6.0.0"
    echo "Build: $(date)"
    echo "Copyright: 2026 NLM Project"
    echo "License: MIT"
}

# Print configuration options
print_config_help() {
    echo "NLM Phase 6 Configuration Options"
    echo "================================="
    echo ""
    echo "Core Neural Configuration:"
    echo "  neuron_count           Total number of neurons"
    echo "  region_count           Number of brain regions"
    echo "  connection_probability Probability of neural connections"
    echo "  simulation_timestep    Time step in seconds"
    echo "  random_seed            Random number generator seed"
    echo ""
    echo "Memory Systems:"
    echo "  working_memory_capacity    Capacity of working memory"
    echo "  episodic_memory_capacity   Capacity of episodic memory"
    echo "  consolidation_interval     Steps between memory consolidation"
    echo "  replay_interval            Steps between memory replay"
    echo ""
    echo "Plasticity Rules:"
    echo "  stdp_ltp_weight    Spike timing dependent plasticity LTP weight"
    echo "  stdp_ltd_weight    Spike timing dependent plasticity LTD weight"
    echo "  stdp_tau          STDP time constant"
    echo "  hebbian_learning_rate   Hebbian learning rate"
    echo ""
    echo "Neuromodulation:"
    echo "  dopamine_baseline   Baseline dopamine level"
    echo "  dopamine_peak      Peak dopamine level"
    echo "  curiosity_baseline  Baseline curiosity level"
    echo "  novelty_sensitivity Sensitivity to novel stimuli"
    echo ""
    echo "Development:"
    echo "  critical_period_start   Start of critical period"
    echo "  critical_period_end     End of critical period"
    echo "  maturation_progress_rate Rate of developmental maturation"
    echo ""
    echo "Performance:"
    echo "  sparse_connectivity_enabled Enable sparse connectivity optimization"
    echo "  simd_vectorization_enabled Enable SIMD vectorization"
    echo "  multithreading_enabled    Enable multithreaded processing"
    echo "  max_threads              Maximum number of threads"
    echo ""
    echo "Persistence:"
    echo "  checkpoint_enabled    Enable checkpoint save/load"
    echo "  checkpoint_path       Path for checkpoint files"
    echo "  auto_save_interval    Steps between automatic saves"
    echo ""
    echo "Presets:"
    echo "  fast     Quick test with minimal configuration"
    echo "  balanced Default balanced configuration"
    echo "  deep     Comprehensive configuration with all features"
    echo ""
    echo "For detailed documentation, see: docs/ARCHITECTURE.md"
}

# Load preset configuration
load_preset() {
    local preset=$1
    case $preset in
        fast)
            echo "Loading fast preset configuration..."
            # Minimal configuration for quick testing
            printf "--neurons 100 --regions 1 --timestep 0.01 --simulation 1000"
            ;;
        balanced)
            echo "Loading balanced preset configuration..."
            # Standard configuration for most use cases
            printf "--neurons 1000 --regions 2 --timestep 0.001 --simulation 10000"
            ;;
        deep)
            echo "Loading deep preset configuration..."
            # Comprehensive configuration with all features
            printf "--neurons 5000 --regions 4 --timestep 0.0005 --simulation 50000 --development --replay --checkpoint brain_deep.bin"
            ;;
        *)
            echo "Unknown preset: $preset"
            echo "Available presets: fast, balanced, deep"
            exit 1
            ;;
    esac
}

# Main command line argument parsing
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            print_help
            exit 0
            ;;
        --version|-v)
            print_version
            exit 0
            ;;
        --config-help)
            print_config_help
            exit 0
            ;;
        --preset|-P)
            if [[ $# -lt 2 ]]; then
                echo "Error: --preset requires a value"
                exit 1
            fi
            shift
            preset_args=$(load_preset "$1")
            # Execute the preset arguments
            set -- $preset_args "$@"
            # Continue with the rest of the arguments
            while [[ $# -gt 0 ]]; do
                case $1 in
                    --help|-h|--version|-v|--config-help|--preset|--experiment|--save|--load|--checkpoint|--replay|--development|--memory-only|--cognition-only|--neuromodulation-only|--simulation|--threads|--sparse|--vectorize|--validate|--quiet|--debug|--benchmark)
                        # Skip already processed arguments
                        shift
                        continue
                        ;;
                    *)
                        # Pass through to the actual NLM executable
                        exec ./nlm "$@"
                        ;;
                esac
                shift
            done
            print_help
            exit 0
            ;;
        --experiment|-e)
            if [[ $# -lt 2 ]]; then
                echo "Error: --experiment requires a value"
                exit 1
            fi
            experiment_mode="$2"
            shift 2
            ;;
        --help-advanced)
            echo "Advanced help would be shown here..."
            echo "Use --help for basic usage"
            exit 0
            ;;
        *)
            # Pass through to the actual NLM executable
            exec ./nlm "$@"
            ;;
    esac
    shift
done

# If no arguments were processed, show help
if [[ $# -eq 0 ]]; then
    print_help
fi