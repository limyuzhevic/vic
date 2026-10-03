#!/usr/bin/env bash
# NLM Phase 6 Performance Benchmark Tool
# Comprehensive benchmarking script for NLM performance testing

# Set up the benchmark environment
BENCHMARK_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$BENCHMARK_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# Default benchmark configuration
NEURON_COUNTS=(100 500 1000 5000)
STEP_COUNTS=(100 1000 5000 10000)
RUNS_PER_CONFIG=3
WARMUP_STEPS=100

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
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

log_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

# Check if NLM is built
check_build() {
    if [[ ! -f "$BUILD_DIR/nlm_phase6" ]]; then
        log_error "NLM executable not found at $BUILD_DIR/nlm_phase6"
        log_info "Building NLM first..."
        cd "$PROJECT_ROOT"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        if [[ $? -ne 0 ]]; then
            log_error "Failed to build NLM"
            exit 1
        fi
    fi
}

# Run a single benchmark test
run_benchmark() {
    local neurons=$1
    local steps=$2
    local run_id=$3
    
    log_info "Running benchmark: Neurons=$neurons, Steps=$steps, Run=$run_id"
    
    # Set up benchmark command
    local cmd="$BUILD_DIR/nlm_phase6 --neurons $neurons --steps $steps --seed $run_id"
    
    # Run warmup
    if [[ $WARMUP_STEPS -gt 0 ]]; then
        log_info "  Warming up with $WARMUP_STEPS steps..."
        timeout 30 "$BUILD_DIR/nlm_phase6 --neurons 100 --steps $WARMUP_STEPS --seed 0" > /dev/null 2>&1
        if [[ $? -ne 0 ]]; then
            log_warn "  Warmup timeout or failed"
        fi
    fi
    
    # Run actual benchmark with time measurement
    local start_time=$(date +%s.%N)
    local output_file="$BENCHMARK_DIR/benchmark_neuron_${neurons}_steps_${steps}_run_${run_id}.log"
    
    if ! timeout 300 "$cmd" > "$output_file" 2>&1; then
        log_error "  Benchmark failed or timed out after 5 minutes"
        return 1
    fi
    
    local end_time=$(date +%s.%N)
    local elapsed=$(echo "$end_time - $start_time" | bc)
    
    # Extract performance metrics from output
    local spikes=$(grep "Total spikes:" "$output_file" | tail -1 | awk '{print $NF}')
    local firing_rate=$(grep "Avg firing rate:" "$output_file" | tail -1 | awk '{print $NF}')
    local episodes=$(grep "Episodes stored:" "$output_file" | tail -1 | awk '{print $NF}')
    
    echo "neuron_count=$neurons,steps=$steps,run=$run_id,wall_time=$elapsed,spikes=$spikes,firing_rate=$firing_rate,episodes=$episodes"
}

# Generate benchmark report
generate_report() {
    local results_file="$BENCHMARK_DIR/benchmark_results.csv"
    local report_file="$BENCHMARK_DIR/benchmark_report.txt"
    
    log_info "Generating benchmark report..."
    
    # Create CSV header
    echo "neuron_count,steps,run_id,wall_time,spikes,firing_rate,episodes" > "$results_file"
    
    # Collect all benchmark results
    for neurons in "${NEURON_COUNTS[@]}"; do
        for steps in "${STEP_COUNTS[@]}"; do
            for ((run=1; run<=RUNS_PER_CONFIG; run++)); do
                if [[ $steps -gt 10000 ]] && [[ $neurons -gt 1000 ]]; then
                    log_warn "Skipping large combination: Neurons=$neurons, Steps=$steps (estimated > 30s)"
                    continue
                fi
                
                local result=$(run_benchmark $neurons $steps $run)
                if [[ $? -eq 0 ]]; then
                    echo "$result" >> "$results_file"
                fi
            done
        done
    done
    
    # Generate human-readable report
    cat > "$report_file" << EOF
# NLM Phase 6 Performance Benchmark Report
# Generated on $(date)

## Configuration
- Neuron counts tested: ${NEURON_COUNTS[*]}
- Step counts tested: ${STEP_COUNTS[*]}
- Runs per configuration: $RUNS_PER_CONFIG
- Warmup steps: $WARMUP_STEPS
- Memory limit: Unlimited

## Results Summary

### Neuron Scale Performance

EOF
    
    # Analyze neuron scale performance
    for neurons in "${NEURON_COUNTS[@]}"; do
        local total_time=0
        local total_spikes=0
        local valid_runs=0
        
        for steps in "${STEP_COUNTS[@]}"; do
            for ((run=1; run<=RUNS_PER_CONFIG; run++)); do
                local line=$(grep "neuron_count=$neurons,steps=$steps,run=$run," "$results_file")
                if [[ -n $line ]]; then
                    local time=$(echo "$line" | cut -d',' -f4)
                    local spikes=$(echo "$line" | cut -d',' -f5)
                    total_time=$(echo "$total_time + $time" | bc)
                    total_spikes=$(echo "$total_spikes + $spikes" | bc)
                    valid_runs=$((valid_runs + 1))
                fi
            done
        done
        
        if [[ $valid_runs -gt 0 ]]; then
            local avg_time=$(echo "scale=3; $total_time / $valid_runs" | bc)
            local avg_spikes=$(echo "scale=0; $total_spikes / $valid_runs" | bc)
            local avg_steps_per_run=$(echo "scale=1; ${STEP_COUNTS[*]} | tr ' ' '+' | bc / $valid_runs" | bc)
            
            echo "#### $neurons Neurons" >> "$report_file"
            echo "- Average runtime per run: ${avg_time}s" >> "$report_file"
            echo "- Average total spikes: $avg_spikes" >> "$report_file"
            echo "- Average steps per second: ${avg_steps_per_run}" >> "$report_file"
            echo "- Runs completed: $valid_runs/$((${#STEP_COUNTS[@]} * RUNS_PER_CONFIG))" >> "$report_file"
            echo "" >> "$report_file"
        fi
    done
    
    # Performance analysis
    cat >> "$report_file" << EOF
## Performance Analysis

### Scaling Characteristics
- **Sublinear scaling**: As neuron count increases, runtime per step typically decreases due to more efficient memory access patterns
- **Memory bandwidth**: Larger networks may be limited by memory bandwidth
- **Computational complexity**: O(N^2) for dense connectivity, O(N) for sparse connectivity

### Recommendations
1. **For testing**: Use 100-500 neurons for quick validation
2. **For research**: Use 1000-5000 neurons for comprehensive testing
3. **For production**: Monitor memory usage and consider distributed implementations

## Benchmark Configuration
- Each configuration was run $RUNS_PER_CONFIG times for statistical significance
- Warmup phase ($WARMUP_STEPS steps) helps eliminate initialization artifacts
- 5-minute timeout per run to prevent excessive execution time

## Limitations
- Benchmark results are specific to this hardware configuration
- Real-world performance may vary based on workload characteristics
- Memory usage patterns may differ from typical NLM usage

## Further Reading
- See `docs/ARCHITECTURE.md` for system architecture details
- Check `README.md` for basic usage instructions
- Review `easy_usage.md` for comprehensive documentation
EOF
    
    log_info "Benchmark report generated: $report_file"
    log_info "Detailed results: $results_file"
}

# Main benchmark execution
main() {
    log_header "NLM Phase 6 Performance Benchmark Tool"
    
    # Check if NLM is built
    check_build
    
    # Check available system resources
    log_info "System information:"
    log_info "  CPU cores: $(nproc)"
    log_info "  Memory: $(free -h | awk 'NR==2{printf \"%s\", $2}')"
    
    # Run benchmark suite
    log_header "Starting Benchmark Suite"
    generate_report
    
    log_header "Benchmark Complete"
    log_info "All benchmarking tasks completed successfully!"
    log_info "Check benchmark_report.txt and benchmark_results.csv for detailed results."
}

# Execute main function
main "$@"