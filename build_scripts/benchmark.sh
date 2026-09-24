#!/bin/bash

# benchmark.sh - NLM Benchmark Script
# Run performance benchmarks for NLM project

set -euo pipefail

# Default configuration
BUILD_DIR="build"
BENCHMARK_EXE="$BUILD_DIR/benchmark"
RESULTS_DIR="benchmark_results"
TIMEOUT_SECONDS=1800
VERBOSE=false
REPORT_FORMAT="json"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Print usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Run performance benchmarks for NLM project.

Options:
  --build-dir DIR              Build directory (default: build)
  --benchmark EXE              Benchmark executable (default: build/benchmark)
  --results-dir DIR            Results directory (default: benchmark_results)
  --timeout SECONDS            Benchmark timeout in seconds (default: 1800)
  --verbose                    Enable verbose output
  --report-format FORMAT        Report format (json, csv, html, console) (default: json)
  --iterations N               Number of iterations (default: auto)
  --warmup-iterations N        Number of warmup iterations (default: 3)
  --memory-profile             Enable memory profiling
  --cpu-profile                Enable CPU profiling
  --results-file FILE          Specific results file (overrides default)
  -h, --help                   Show this help message

Examples:
  $0                                    # Run default benchmarks
  $0 --verbose --iterations 100        # Run benchmarks with 100 iterations
  $0 --memory-profile                   # Run with memory profiling
  $0 --report-format csv                 # Generate CSV report
  $0 --results-dir myresults             # Save results to custom directory
EOF
}

# Check benchmark dependencies
check_benchmark_dependencies() {
    log_info "Checking benchmark dependencies..."
    
    # Check if benchmark executable exists
    if [[ ! -f "$BENCHMARK_EXE" ]]; then
        log_error "Benchmark executable not found: $BENCHMARK_EXE"
        log_info "Available executables in $BUILD_DIR:"
        ls -la "$BUILD_DIR/" | grep -E "(benchmark|perf)" || true
        exit 1
    fi
    
    # Check if benchmark executable is runnable
    if [[ ! -x "$BENCHMARK_EXE" ]]; then
        log_error "Benchmark executable is not executable: $BENCHMARK_EXE"
        exit 1
    fi
    
    log_success "Benchmark executable found: $BENCHMARK_EXE"
}

# Setup results directory
setup_results_dir() {
    log_info "Setting up results directory: $RESULTS_DIR"
    
    # Create results directory
    mkdir -p "$RESULTS_DIR"
    
    # Change to results directory
    cd "$RESULTS_DIR"
}

# Run benchmarks
run_benchmarks() {
    log_info "Running benchmarks..."
    
    # Build benchmark arguments
    local benchmark_args=()
    
    # Add common arguments
    benchmark_args+=("--benchmark_out=$RESULTS_DIR/benchmark_results.$REPORT_FORMAT")
    benchmark_args+=("--benchmark_out_format=$REPORT_FORMAT")
    
    # Add specific arguments if provided
    if [[ -n "${BENCHMARK_ITERATIONS:-}" ]]; then
        benchmark_args+=("--benchmark_iterations=${BENCHMARK_ITERATIONS}")
    fi
    
    if [[ -n "${BENCHMARK_WARMUP_ITERATIONS:-}" ]]; then
        benchmark_args+=("--benchmark_warmup_iterations=${BENCHMARK_WARMUP_ITERATIONS}")
    fi
    
    # Add profiling flags
    if [[ "$MEMORY_PROFILE" == true ]]; then
        benchmark_args+=("--benchmark_memory_profile=true")
    fi
    
    if [[ "$CPU_PROFILE" == true ]]; then
        benchmark_args+=("--benchmark_cpu_profile=true")
    fi
    
    # Add timeout
    benchmark_args+=("--benchmark_duration="$TIMEOUT_SECONDS"s")
    
    # Run verbose benchmark if requested
    if [[ "$VERBOSE" == true ]]; then
        log_info "Running verbose benchmark..."
        "$BENCHMARK_EXE" "${benchmark_args[@]}" --benchmark_display_aggregates_only
    else
        log_info "Running benchmark with output to $RESULTS_DIR/benchmark_results.$REPORT_FORMAT"
        if "$BENCHMARK_EXE" "${benchmark_args[@]}"; then
            log_success "Benchmarks completed successfully"
        else
            log_error "Benchmarks failed"
            exit 1
        fi
    fi
}

# Analyze benchmark results
analyze_results() {
    log_info "Analyzing benchmark results..."
    
    # Check if results file exists
    local results_file="$RESULTS_DIR/benchmark_results.$REPORT_FORMAT"
    if [[ ! -f "$results_file" ]]; then
        log_warning "Benchmark results file not found: $results_file"
        return 0
    fi
    
    # Analyze based on format
    case $REPORT_FORMAT in
        "json")
            analyze_json_results
            ;;
        "csv")
            analyze_csv_results
            ;;
        "html")
            analyze_html_results
            ;;
        *)
            log_warning "Unsupported report format: $REPORT_FORMAT"
            ;;
    esac
}

# Analyze JSON benchmark results
analyze_json_results() {
    log_info "Analyzing JSON benchmark results..."
    
    # Check if jq is available
    if ! command -v jq &> /dev/null; then
        log_warning "jq not found, skipping JSON analysis"
        return 0
    fi
    
    # Extract key metrics
    local total_runs=$(jq '.benchmarks | length' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null || echo "0")
    local total_time=$(jq '.benchmarks | map(.real_time) | add' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{sum+=$1} END {printf "%.2f", sum}')
    local avg_time=$(jq '.benchmarks | map(.real_time) | add / length' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')
    local min_time=$(jq '.benchmarks | map(.real_time) | min' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')
    local max_time=$(jq '.benchmarks | map(.real_time) | max' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')
    
    # Extract benchmark names
    local benchmark_names=$(jq '.benchmarks[].name' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | sed 's/^ *//;s/ *$//' | tr '\n' ',' | sed 's/,$//')
    
    # Print summary
    echo "Benchmark Summary:"
    echo "================="
    echo "Total runs: $total_runs"
    echo "Total time: ${total_time}s"
    echo "Average time per run: ${avg_time}s"
    echo "Minimum time: ${min_time}s"
    echo "Maximum time: ${max_time}s"
    echo "Benchmarks: $benchmark_names"
    
    # Calculate standard deviation if possible
    local std_dev=$(jq '.benchmarks | map(.real_time) | add / length as $avg | map(($avg - .) * (. - $avg)) | add / length | sqrt' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')
    if [[ -n "$std_dev" && "$std_dev" != "0.00" ]]; then
        echo "Standard deviation: ${std_dev}s"
    fi
    
    # Generate HTML report if html format requested
    if [[ "$REPORT_FORMAT" == "json" ]]; then
        generate_html_report
    fi
}

# Analyze CSV benchmark results
analyze_csv_results() {
    log_info "Analyzing CSV benchmark results..."
    
    # Simple CSV analysis
    local csv_file="$RESULTS_DIR/benchmark_results.csv"
    if [[ -f "$csv_file" ]]; then
        echo "CSV Analysis:"
        echo "============"
        wc -l "$csv_file" | awk '{print "Lines: " $1}'
        head -1 "$csv_file" | tr ',' '\n' | nl | awk '{print "Column " $1 ": " $2}'
    fi
}

# Analyze HTML benchmark results
analyze_html_results() {
    log_info "Analyzing HTML benchmark results..."
    
    local html_file="$RESULTS_DIR/benchmark_results.html"
    if [[ -f "$html_file" ]]; then
        echo "HTML benchmark report generated"
        echo "View in browser: file://$PWD/$html_file"
    fi
}

# Generate HTML benchmark report
generate_html_report() {
    log_info "Generating HTML benchmark report..."
    
    # Create HTML report if jq is available
    if command -v jq &> /dev/null && [[ -f "$RESULTS_DIR/benchmark_results.json" ]]; then
        cat > "$RESULTS_DIR/benchmark_report.html" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>NLM Benchmark Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .metrics { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }
        .metric { background-color: #e9ecef; padding: 15px; border-radius: 5px; text-align: center; }
        .metric h3 { margin: 0; color: #495057; }
        .metric p { font-size: 24px; font-weight: bold; color: #007bff; }
        table { width: 100%; border-collapse: collapse; margin: 20px 0; }
        th, td { padding: 10px; text-align: left; border-bottom: 1px solid #dee2e6; }
        th { background-color: #f8f9fa; }
        .success { color: #28a745; }
        .warning { color: #ffc107; }
        .error { color: #dc3545; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Benchmark Report</h1>
        <p>Generated on $(date)</p>
    </div>
    
    <div class="metrics">
        <div class="metric">
            <h3>Total Runs</h3>
            <p>$(jq '.benchmarks | length' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null || echo "0")</p>
        </div>
        <div class="metric">
            <h3>Total Time</h3>
            <p>$(jq '.benchmarks | map(.real_time) | add | . * 1000 | floor / 1000' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')s</p>
        </div>
        <div class="metric">
            <h3>Average Time</h3>
            <p>$(jq '.benchmarks | map(.real_time) | add / length | . * 1000 | floor / 1000' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')s</p>
        </div>
        <div class="metric">
            <h3>Minimum Time</h3>
            <p>$(jq '.benchmarks | map(.real_time) | min | . * 1000 | floor / 1000' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')s</p>
        </div>
        <div class="metric">
            <h3>Maximum Time</h3>
            <p>$(jq '.benchmarks | map(.real_time) | max | . * 1000 | floor / 1000' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null | awk '{printf "%.2f", $1}')s</p>
        </div>
    </div>
    
    <h2>Benchmark Details</h2>
    <table>
        <thead>
            <tr>
                <th>Name</th>
                <th>Iterations</th>
                <th>Real Time (s)</th>
                <th>User Time (s)</th>
                <th>System Time (s)</th>
                <th>Complexity</th>
            </tr>
        </thead>
        <tbody>
            $(jq -r '.benchmarks[] | "<tr><td>" + .name + "</td><td>" + (.iterations | tostring) + "</td><td>" + (.real_time | . * 1000 | floor / 1000 | tostring) + "</td><td>" + (.user_time | . * 1000 | floor / 1000 | tostring) + "</td><td>" + (.system_time | . * 1000 | floor / 1000 | tostring) + "</td><td>" + .complexity + "</td></tr>"' "$RESULTS_DIR/benchmark_results.json" 2>/dev/null)
        </tbody>
    </table>
</body>
</html>
EOF
        log_success "HTML benchmark report generated: $RESULTS_DIR/benchmark_report.html"
    fi
}

# Main benchmark function
main() {
    log_info "Starting NLM benchmark process..."
    log_info "Benchmark executable: $BENCHMARK_EXE"
    log_info "Results directory: $RESULTS_DIR"
    log_info "Report format: $REPORT_FORMAT"
    
    # Check benchmark dependencies
    check_benchmark_dependencies
    
    # Setup results directory
    setup_results_dir
    
    # Run benchmarks
    run_benchmarks
    
    # Analyze results
    analyze_results
    
    log_success "Benchmark process completed successfully"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --benchmark)
            BENCHMARK_EXE="$2"
            shift 2
            ;;
        --results-dir)
            RESULTS_DIR="$2"
            shift 2
            ;;
        --timeout)
            TIMEOUT_SECONDS="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --report-format)
            REPORT_FORMAT="$2"
            shift 2
            ;;
        --iterations)
            BENCHMARK_ITERATIONS="$2"
            shift 2
            ;;
        --warmup-iterations)
            BENCHMARK_WARMUP_ITERATIONS="$2"
            shift 2
            ;;
        --memory-profile)
            MEMORY_PROFILE=true
            shift
            ;;
        --cpu-profile)
            CPU_PROFILE=true
            shift
            ;;
        --results-file)
            RESULTS_FILE="$2"
            RESULTS_DIR=$(dirname "$RESULTS_FILE")
            REPORT_FORMAT=$(basename "$RESULTS_FILE".*$ | cut -d. -f2)
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Run main function
main "$@"