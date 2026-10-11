#!/usr/bin/env bash

# NLM Advanced Debug and Analysis Commands
# This script provides advanced debugging and analysis capabilities for the NLM brain

# Check if running with build directory argument
if [ ! -d "build" ]; then
    echo "Error: Build directory not found. Please run './build.sh' first."
    exit 1
fi

# Check if NLM executable exists
if [ ! -f "build/nlm" ]; then
    echo "Error: NLM executable not found in build directory. Please build the project first."
    exit 1
fi

# Check if CheckpointManager executable exists
if [ ! -f "build/CheckpointManager" ]; then
    echo "Warning: CheckpointManager not found. Some features will be unavailable."
fi

# Function to display help
usage() {
    echo "NLM Advanced Debug and Analysis Commands"
    echo "======================================="
    echo ""
    echo "Usage: ./nlm_debug.sh <command> [options]"
    echo ""
    echo "Commands:"
    echo "  profile <config_file> <output_file>  Profile NLM execution with timing"
    echo "  analyze <checkpoint_file>            Analyze a brain state checkpoint"
    echo "  diff <checkpoint1> <checkpoint2>     Compare two checkpoints"
    echo "  test <test_name>                    Run neural network test"
    echo "  stats <output_file>                 Collect neural statistics"
    echo "  monitor <duration> [interval]       Monitor memory usage"
    echo "  benchmark <test_name>               Run performance benchmark"
    echo "  replay <checkpoint_file>             Replay brain from checkpoint"
    echo ""
    echo "Tests available: connectivity, plasticity, memory, neuromodulation, development"
    echo ""
    echo "Examples:"
    echo "  ./nlm_debug.sh profile configs/default.cfg results.txt"
    echo "  ./nlm_debug.sh analyze checkpoint_001.bin"
    echo "  ./nlm_debug.sh test plasticity"
    echo "  ./nlm_debug.sh benchmark connectivity"
}

# Function to create a backup of current checkpoint
backup_checkpoint() {
    local source="$1"
    local dest="${2:-backup_$(date +%Y%m%d_%H%M%S)}.bin"
    
    echo "Creating backup: $source -> $dest"
    cp "$source" "$dest"
    echo "Backup created successfully."
}

# Function to list all checkpoints in directory
list_checkpoints() {
    local directory="${1:-.}"
    
    if [ ! -d "$directory" ]; then
        echo "Error: Directory '$directory' does not exist."
        return 1
    fi
    
    echo "Checkpoints in '$directory':"
    find "$directory" -name "checkpoint_*.bin" -type f | sort | head -20
}

# Function to run a performance benchmark
run_benchmark() {
    local test_name="$1"
    local iterations="${2:-10}"
    
    echo "Running benchmark: $test_name ($iterations iterations)"
    
    cd build
    case $test_name in
        "connectivity")
            for ((i=1; i<=$iterations; i++)); do
                echo "  Run $i/$iterations..."
                ./nlm_phase3_demo > /dev/null 2>&1
            done
            ;;
        "plasticity")
            for ((i=1; i<=$iterations; i++)); do
                echo "  Run $i/$iterations..."
                ./nlm_phase3_demo --test=plasticity > /dev/null 2>&1
            done
            ;;
        *)
            echo "Unknown benchmark: $test_name"
            exit 1
            ;;
    esac
    cd ..
    
    echo "Benchmark complete."
}

# Function to replay brain from checkpoint
replay_from_checkpoint() {
    local checkpoint_file="$1"
    
    echo "Replaying brain from checkpoint: $checkpoint_file"
    
    if [ ! -f "build/CheckpointManager" ]; then
        echo "Error: CheckpointManager not available"
        exit 1
    fi
    
    build/CheckpointManager replay "$checkpoint_file"
}

# Function to run the main NLM executable with profiling
profile_nlm() {
    local config_file="${1:-configs/default.cfg}"
    local output_file="${2:-profile_results.txt}"
    
    echo "Profiling NLM with config: $config_file"
    echo "Output will be written to: $output_file"
    
    cd build
    time ./nlm --config "$config_file" > "$output_file" 2>&1
    cd ..
    
    echo "Profiling complete. Results saved to $output_file"
}

# Function to analyze brain state checkpoint
analyze_checkpoint() {
    local checkpoint_file="$1"
    
    if [ ! -f "build/CheckpointManager" ]; then
        echo "Error: CheckpointManager binary not found in build directory"
        exit 1
    fi
    
    echo "Analyzing checkpoint file: $checkpoint_file"
    build/CheckpointManager analyze "$checkpoint_file"
}

# Function to compare two brain states
diff_checkpoints() {
    local checkpoint1="$1"
    local checkpoint2="$2"
    
    if [ ! -f "build/CheckpointManager" ]; then
        echo "Error: CheckpointManager binary not found in build directory"
        exit 1
    fi
    
    echo "Comparing checkpoints: $checkpoint1 vs $checkpoint2"
    build/CheckpointManager diff "$checkpoint1" "$checkpoint2"
}

# Function to run a neural network test
run_test() {
    local test_name="$1"
    shift
    
    echo "Running test: $test_name"
    
    cd build
    case $test_name in
        "connectivity")
            ./nlm_phase3_demo
            ;;
        "plasticity")
            # Run plasticity-specific test
            ./nlm_phase3_demo --test=plasticity
            ;;
        "memory")
            # Run memory test
            ./nlm_phase3_demo --test=memory
            ;;
        "neurogulation")
            # Run neuromodulation test
            ./nlm_phase3_demo --test=neuromodulation
            ;;
        "development")
            # Run development test
            ./nlm_phase3_demo --test=development
            ;;
        *)
            echo "Unknown test: $test_name"
            exit 1
            ;;
    esac
    cd ..
}

# Function to collect neural statistics
collect_stats() {
    local output_file="${1:-neural_stats.json}"
    
    echo "Collecting neural statistics..."
    
    if [ ! -f "build/nlm" ]; then
        echo "Error: NLM executable not found. Please build the project first."
        exit 1
    fi
    
    # Run NLM with debug flag to collect statistics
    cd build
    ./nlm --debug-stats > /dev/null 2>&1
    # Capture the stats that would have been printed
    ./nlm --debug-stats --stats-only --output "$output_file"
    cd ..
    
    echo "Neural statistics collected and saved to $output_file"
}

# Function to monitor memory usage
monitor_memory() {
    local duration="${1:-60}"  # seconds
    local interval="${2:-5}"  # seconds
    
    echo "Monitoring memory usage for $duration seconds (every $interval seconds)"
    
    if [ ! -f "build/nlm" ]; then
        echo "Error: NLM executable not found. Please build the project first."
        exit 1
    fi
    
    cd build
    
    # Start the NLM process
    ./nlm &
    NLM_PID=$!
    
    # Monitor memory usage
    for ((i=0; i<duration; i+=interval)); do
        ps -p $NLM_PID -o %mem,rss,vsz --no-headers
        sleep $interval
    done
    
    # Kill the process
    kill $NLM_PID
    
    cd ..
    echo "Memory monitoring complete."
}

# Parse command line arguments
if [ $# -lt 1 ]; then
    usage
    exit 1
fi

# Execute commands
case $1 in
    profile)
        profile_nlm "$2" "$3"
        ;;
    analyze)
        analyze_checkpoint "$2"
        ;;
    diff)
        diff_checkpoints "$2" "$3"
        ;;
    test)
        run_test "$2" "$@"
        ;;
    stats)
        collect_stats "$2"
        ;;
    monitor)
        monitor_memory "$2" "$3"
        ;;
    benchmark)
        run_benchmark "$2" "$3"
        ;;
    replay)
        replay_from_checkpoint "$2"
        ;;
    --help|-h)
        usage
        ;;
    *)
        echo "Unknown command: $1"
        usage
        exit 1
        ;;
esac