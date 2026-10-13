#!/usr/bin/env bash
# nlm - NLM Management CLI Entry Point

# Source the CLI implementation
source "$(dirname "$0")/nlm_cli.sh"

# Check if nlm binary exists
NLM_BIN="$(dirname "$0")/build/nlm"
if [ ! -f "$NLM_BIN" ]; then
    print_warning "NLM binary not found. Building from source..."
    if [ -f "CMakeLists.txt" ]; then
        # Try to build
        mkdir -p build
        cd build
        if cmake .. -DCMAKE_BUILD_TYPE=Release; then
            if make -j4; then
                cd ..
                print_success "NLM built successfully"
            else
                print_error "Failed to build NLM"
                exit 1
            fi
        else
            print_error "Failed to configure NLM build"
            exit 1
        fi
    else
        print_error "CMakeLists.txt not found. Cannot build NLM."
        exit 1
    fi
    NLM_BIN="$(dirname "$0")/build/nlm"
fi

# Handle command-line arguments
if [ $# -eq 0 ]; then
    print_help
    exit 0
fi

COMMAND="$1"
shift

# Parse global options
VERBOSE=0
QUIET=0
CONFIG_FILE="configs/default.cfg"

while [ $# -gt 0 ]; do
    case "$1" in
        "--verbose"|-v)
            VERBOSE=1
            shift
            ;;
        "--quiet"|-q)
            QUIET=1
            shift
            ;;
        "--config")
            if [ $# -lt 2 ]; then
                print_error "--config requires a file argument"
                exit 1
            fi
            CONFIG_FILE="$2"
            shift 2
            ;;
        "--help"|-h)
            print_help
            exit 0
            ;;
        "--version")
            echo "NLM CLI v0.1.0"
            exit 0
            ;;
        *)
            # Pass remaining arguments to the main command
            break
            ;;
    esac
done

# Execute commands
case "$COMMAND" in
    "run")
        execute_run_command "$@"
        ;;
    "init")
        execute_init_command "$@"
        ;;
    "save")
        execute_save_command "$@"
        ;;
    "load")
        execute_load_command "$@"
        ;;
    "config")
        execute_config_command "$@"
        ;;
    "info")
        execute_info_command
        ;;
    "status")
        execute_status_command "$@"
        ;;
    "stats")
        execute_stats_command "$@"
        ;;
    "simulate")
        execute_simulate_command "$@"
        ;;
    "visualize")
        execute_visualize_command "$@"
        ;;
    "benchmark")
        execute_benchmark_command "$@"
        ;;
    "help")
        if [ $# -gt 0 ]; then
            print_command_help "$1"
        else
            print_help
        fi
        ;;
    *)
        print_error "Unknown command: $COMMAND"
        print_help
        exit 1
        ;;
esac

# Function implementations
execute_run_command() {
    local steps=""
    local neurons=""
    local regions=""
    local interactive=""
    local background=""
    local continue_flag=""
    local checkpoint_dir=""

    while [ $# -gt 0 ]; do
        case "$1" in
            "--steps")
                if [ $# -lt 2 ]; then
                    print_error "--steps requires a value"
                    exit 1
                fi
                steps="$2"
                shift 2
                ;;
            "--neurons")
                if [ $# -lt 2 ]; then
                    print_error "--neurons requires a value"
                    exit 1
                fi
                neurons="$2"
                shift 2
                ;;
            "--regions")
                if [ $# -lt 2 ]; then
                    print_error "--regions requires a value"
                    exit 1
                fi
                regions="$2"
                shift 2
                ;;
            "--interactive")
                interactive="--interactive"
                shift
                ;;
            "--background")
                background="--background"
                shift
                ;;
            "--continue")
                continue_flag="--continue"
                shift
                ;;
            "--checkpoint")
                if [ $# -lt 2 ]; then
                    print_error "--checkpoint requires a directory"
                    exit 1
                fi
                checkpoint_dir="--checkpoint $2"
                shift 2
                ;;
            *)
                if [ -z "$steps" ] && [[ "$1" =~ ^[0-9]+$ ]]; then
                    steps="$1"
                    shift
                else
                    print_error "Unknown option: $1"
                    exit 1
                fi
                ;;
        esac
    done

    print_info "Running NLM simulation..."
    if [ ! -z "$steps" ]; then
        print_info "Steps: $steps"
    fi
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD --config $CONFIG_FILE"
    [ ! -z "$steps" ] && CMD="$CMD --steps $steps"
    [ ! -z "$neurons" ] && CMD="$CMD --neuron-count $neurons"
    [ ! -z "$regions" ] && CMD="$CMD --region-count $regions"
    [ ! -z "$interactive" ] && CMD="$CMD --interactive"
    [ ! -z "$background" ] && CMD="$CMD --background"
    [ ! -z "$continue_flag" ] && CMD="$CMD --continue"
    [ ! -z "$checkpoint_dir" ] && CMD="$CMD $checkpoint_dir"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_init_command() {
    local neurons=""
    local regions=""
    local seed=""
    local timestep=""
    local stdp_ltp=""
    local stdp_ltd=""
    local synaptogenesis=""
    local pruning=""

    while [ $# -gt 0 ]; do
        case "$1" in
            "--neurons")
                if [ $# -lt 2 ]; then
                    print_error "--neurons requires a value"
                    exit 1
                fi
                neurons="$2"
                shift 2
                ;;
            "--regions")
                if [ $# -lt 2 ]; then
                    print_error "--regions requires a value"
                    exit 1
                fi
                regions="$2"
                shift 2
                ;;
            "--seed")
                if [ $# -lt 2 ]; then
                    print_error "--seed requires a value"
                    exit 1
                fi
                seed="$2"
                shift 2
                ;;
            "--timestep")
                if [ $# -lt 2 ]; then
                    print_error "--timestep requires a value"
                    exit 1
                fi
                timestep="$2"
                shift 2
                ;;
            "--stdp-ltp")
                if [ $# -lt 2 ]; then
                    print_error "--stdp-ltp requires a value"
                    exit 1
                fi
                stdp_ltp="$2"
                shift 2
                ;;
            "--stdp-ltd")
                if [ $# -lt 2 ]; then
                    print_error "--stdp-ltd requires a value"
                    exit 1
                fi
                stdp_ltd="$2"
                shift 2
                ;;
            "--synaptogenesis")
                if [ $# -lt 2 ]; then
                    print_error "--synaptogenesis requires a value"
                    exit 1
                fi
                synaptogenesis="$2"
                shift 2
                ;;
            "--pruning")
                if [ $# -lt 2 ]; then
                    print_error "--pruning requires a value"
                    exit 1
                fi
                pruning="$2"
                shift 2
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done

    print_info "Initializing NLM brain..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD --config $CONFIG_FILE"
    [ ! -z "$neurons" ] && CMD="$CMD --neuron-count $neurons"
    [ ! -z "$regions" ] && CMD="$CMD --region-count $regions"
    [ ! -z "$seed" ] && CMD="$CMD --random-seed $seed"
    [ ! -z "$timestep" ] && CMD="$CMD --simulation-timestep $timestep"
    [ ! -z "$stdp_ltp" ] && CMD="$CMD --stdp-ltp-weight $stdp_ltp"
    [ ! -z "$stdp_ltd" ] && CMD="$CMD --stdp-ltd-weight $stdp_ltd"
    [ ! -z "$synaptogenesis" ] && CMD="$CMD --synaptogenesis-rate $synaptogenesis"
    [ ! -z "$pruning" ] && CMD="$CMD --pruning-rate $pruning"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_save_command() {
    local file="$1"
    local compress=""
    local metadata=""
    local memory=""
    local development=""
    local neuromodulation=""

    if [ $# -eq 0 ]; then
        print_error "Save command requires a file argument"
        exit 1
    fi

    while [ $# -gt 0 ]; do
        case "$1" in
            "--compress")
                if [ $# -lt 2 ]; then
                    print_error "--compress requires a level"
                    exit 1
                fi
                compress="$2"
                shift 2
                ;;
            "--metadata")
                metadata="--metadata"
                shift
                ;;
            "--memory")
                memory="--memory"
                shift
                ;;
            "--development")
                development="--development"
                shift
                ;;
            "--neuromodulation")
                neuromodulation="--neuromodulation"
                shift
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Saving brain state to $file..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD save $file"
    [ ! -z "$compress" ] && CMD="$CMD --compress $compress"
    [ ! -z "$metadata" ] && CMD="$CMD --metadata"
    [ ! -z "$memory" ] && CMD="$CMD --memory"
    [ ! -z "$development" ] && CMD="$CMD --development"
    [ ! -z "$neuromodulation" ] && CMD="$CMD --neuromodulation"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_load_command() {
    local file="$1"
    local validate=""
    local force=""
    local metadata=""
    local memory=""
    local development=""
    local neuromodulation=""

    if [ $# -eq 0 ]; then
        print_error "Load command requires a file argument"
        exit 1
    fi

    while [ $# -gt 0 ]; do
        case "$1" in
            "--validate")
                validate="--validate"
                shift
                ;;
            "--force")
                force="--force"
                shift
                ;;
            "--metadata")
                metadata="--metadata"
                shift
                ;;
            "--memory")
                memory="--memory"
                shift
                ;;
            "--development")
                development="--development"
                shift
                ;;
            "--neuromodulation")
                neuromodulation="--neuromodulation"
                shift
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Loading brain state from $file..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD load $file"
    [ ! -z "$validate" ] && CMD="$CMD --validate"
    [ ! -z "$force" ] && CMD="$CMD --force"
    [ ! -z "$metadata" ] && CMD="$CMD --metadata"
    [ ! -z "$memory" ] && CMD="$CMD --memory"
    [ ! -z "$development" ] && CMD="$CMD --development"
    [ ! -z "$neuromodulation" ] && CMD="$CMD --neuromodulation"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_config_command() {
    local key="$1"
    local value="$2"

    if [ -z "$key" ]; then
        print_error "Config command requires a key argument"
        exit 1
    fi

    if [ ! -z "$value" ]; then
        print_info "Setting config $key to $value"
        # Build and run command
        CMD="$NLM_BIN"
        CMD="$CMD config set $key $value"
    else
        print_info "Getting config value for $key"
        # Build and run command
        CMD="$NLM_BIN"
        CMD="$CMD config get $key"
    fi

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_info_command() {
    print_info "NLM System Information"
    print_info "======================"
    
    # Display version info
    echo "Version: NLM v0.1.0 (Phase 6: Final Integration)"
    echo "Build Type: Release"
    echo "C++ Standard: C++20"
    
    # Check for config file
    if [ -f "$CONFIG_FILE" ]; then
        echo "Config File: $CONFIG_FILE"
    else
        echo "Config File: $CONFIG_FILE (default - not found)"
    fi
    
    # Display available commands
    echo ""
    echo "Available Commands:"
    echo "  run      - Run NLM simulation"
    echo "  init     - Initialize NLM brain"
    echo "  save     - Save brain state to checkpoint"
    echo "  load     - Load brain state from checkpoint"
    echo "  config   - Get or set configuration values"
    echo "  info     - Display system information"
    echo "  status   - Show current brain status"
    echo "  stats    - Display detailed performance statistics"
    echo "  simulate - Run simulation for specified steps"
    echo "  visualize - Launch visualization interface"
    echo "  benchmark - Run performance benchmarks"
    
    # Display system requirements
    echo ""
    echo "System Requirements:"
    echo "  RAM: 4GB minimum, 16GB recommended"
    echo "  CPU: Multi-core processor"
    echo "  Storage: 1GB for brain data, additional for checkpoints"
    
    echo ""
    print_success "NLM CLI is ready to use"
}

execute_status_command() {
    local detailed=""
    local neurons=""
    local synapses=""
    local memory=""
    local development=""
    local neuromodulation=""

    while [ $# -gt 0 ]; do
        case "$1" in
            "--detailed")
                detailed="--detailed"
                shift
                ;;
            "--neurons")
                neurons="--neurons"
                shift
                ;;
            "--synapses")
                synapses="--synapses"
                shift
                ;;
            "--memory")
                memory="--memory"
                shift
                ;;
            "--development")
                development="--development"
                shift
                ;;
            "--neuromodulation")
                neuromodulation="--neuromodulation"
                shift
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Getting NLM brain status..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD status"
    [ ! -z "$detailed" ] && CMD="$CMD --detailed"
    [ ! -z "$neurons" ] && CMD="$CMD --neurons"
    [ ! -z "$synapses" ] && CMD="$CMD --synapses"
    [ ! -z "$memory" ] && CMD="$CMD --memory"
    [ ! -z "$development" ] && CMD="$CMD --development"
    [ ! -z "$neuromodulation" ] && CMD="$CMD --neuromodulation"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_stats_command() {
    local spikes=""
    local firing_rate=""
    local plasticity=""
    local memory_stats=""
    local network_stats=""
    local performance=""

    while [ $# -gt 0 ]; do
        case "$1" in
            "--spikes")
                spikes="--spikes"
                shift
                ;;
            "--firing-rate")
                firing_rate="--firing-rate"
                shift
                ;;
            "--plasticity")
                plasticity="--plasticity"
                shift
                ;;
            "--memory")
                memory_stats="--memory"
                shift
                ;;
            "--network")
                network_stats="--network"
                shift
                ;;
            "--performance")
                performance="--performance"
                shift
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Getting NLM brain statistics..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD stats"
    [ ! -z "$spikes" ] && CMD="$CMD --spikes"
    [ ! -z "$firing_rate" ] && CMD="$CMD --firing-rate"
    [ ! -z "$plasticity" ] && CMD="$CMD --plasticity"
    [ ! -z "$memory_stats" ] && CMD="$CMD --memory"
    [ ! -z "$network_stats" ] && CMD="$CMD --network"
    [ ! -z "$performance" ] && CMD="$CMD --performance"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_simulate_command() {
    local steps="$1"
    local config_file="$CONFIG_FILE"
    local visualize=""
    local checkpoint_dir=""
    local interval=""
    local log_level=""
    local output_file=""

    if [ $# -eq 0 ]; then
        print_error "Simulate command requires a steps argument"
        exit 1
    fi

    while [ $# -gt 1 ]; do
        case "$1" in
            "--config")
                if [ $# -lt 2 ]; then
                    print_error "--config requires a file"
                    exit 1
                fi
                config_file="$2"
                shift 2
                ;;
            "--visualize")
                visualize="--visualize"
                shift
                ;;
            "--checkpoint")
                if [ $# -lt 2 ]; then
                    print_error "--checkpoint requires a directory"
                    exit 1
                fi
                checkpoint_dir="$2"
                shift 2
                ;;
            "--interval")
                if [ $# -lt 2 ]; then
                    print_error "--interval requires a value"
                    exit 1
                fi
                interval="$2"
                shift 2
                ;;
            "--log-level")
                if [ $# -lt 2 ]; then
                    print_error "--log-level requires a level"
                    exit 1
                fi
                log_level="$2"
                shift 2
                ;;
            "--output")
                if [ $# -lt 2 ]; then
                    print_error "--output requires a file"
                    exit 1
                fi
                output_file="$2"
                shift 2
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Running simulation for $steps steps..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD simulate $steps"
    CMD="$CMD --config $config_file"
    [ ! -z "$visualize" ] && CMD="$CMD --visualize"
    [ ! -z "$checkpoint_dir" ] && CMD="$CMD --checkpoint $checkpoint_dir"
    [ ! -z "$interval" ] && CMD="$CMD --interval $interval"
    [ ! -z "$log_level" ] && CMD="$CMD --log-level $log_level"
    [ ! -z "$output_file" ] && CMD="$CMD --output $output_file"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_visualize_command() {
    local fullscreen=""
    local output_format="terminal"
    local port="8080"
    local host="localhost"
    local neurons=""
    local animate=""
    local statistics=""

    while [ $# -gt 0 ]; do
        case "$1" in
            "--fullscreen")
                fullscreen="--fullscreen"
                shift
                ;;
            "--output")
                if [ $# -lt 2 ]; then
                    print_error "--output requires a format"
                    exit 1
                fi
                output_format="$2"
                shift 2
                ;;
            "--port")
                if [ $# -lt 2 ]; then
                    print_error "--port requires a port number"
                    exit 1
                fi
                port="$2"
                shift 2
                ;;
            "--host")
                if [ $# -lt 2 ]; then
                    print_error "--host requires a host"
                    exit 1
                fi
                host="$2"
                shift 2
                ;;
            "--neurons")
                if [ $# -lt 2 ]; then
                    print_error "--neurons requires a number"
                    exit 1
                fi
                neurons="$2"
                shift 2
                ;;
            "--animate")
                animate="--animate"
                shift
                ;;
            "--statistics")
                statistics="--statistics"
                shift
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Launching visualization interface..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD visualize"
    [ ! -z "$fullscreen" ] && CMD="$CMD --fullscreen"
    [ "$output_format" != "terminal" ] && CMD="$CMD --output $output_format"
    CMD="$CMD --port $port"
    CMD="$CMD --host $host"
    [ ! -z "$neurons" ] && CMD="$CMD --neurons $neurons"
    [ ! -z "$animate" ] && CMD="$CMD --animate"
    [ ! -z "$statistics" ] && CMD="$CMD --statistics"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}

execute_benchmark_command() {
    local duration="60"
    local neurons="1000"
    local regions="1"
    local steps="10000"
    local memory_limit=""
    local detailed=""
    local report_format="text"

    while [ $# -gt 0 ]; do
        case "$1" in
            "--duration")
                if [ $# -lt 2 ]; then
                    print_error "--duration requires a value"
                    exit 1
                fi
                duration="$2"
                shift 2
                ;;
            "--neurons")
                if [ $# -lt 2 ]; then
                    print_error "--neurons requires a value"
                    exit 1
                fi
                neurons="$2"
                shift 2
                ;;
            "--regions")
                if [ $# -lt 2 ]; then
                    print_error "--regions requires a value"
                    exit 1
                fi
                regions="$2"
                shift 2
                ;;
            "--steps")
                if [ $# -lt 2 ]; then
                    print_error "--steps requires a value"
                    exit 1
                fi
                steps="$2"
                shift 2
                ;;
            "--memory")
                if [ $# -lt 2 ]; then
                    print_error "--memory requires a value"
                    exit 1
                fi
                memory_limit="$2"
                shift 2
                ;;
            "--detailed")
                detailed="--detailed"
                shift
                ;;
            "--report")
                if [ $# -lt 2 ]; then
                    print_error "--report requires a format"
                    exit 1
                fi
                report_format="$2"
                shift 2
                ;;
            *)
                shift
                ;;
        esac
    done

    print_info "Running performance benchmark for $duration seconds..."
    
    # Build and run command
    CMD="$NLM_BIN"
    CMD="$CMD benchmark"
    CMD="$CMD --duration $duration"
    CMD="$CMD --neurons $neurons"
    CMD="$CMD --regions $regions"
    CMD="$CMD --steps $steps"
    [ ! -z "$memory_limit" ] && CMD="$CMD --memory $memory_limit"
    [ ! -z "$detailed" ] && CMD="$CMD --detailed"
    CMD="$CMD --report $report_format"

    if [ $VERBOSE -eq 1 ]; then
        print_info "Running command: $CMD"
    fi

    # Execute the command
    $CMD
}