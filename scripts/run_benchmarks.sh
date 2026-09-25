#!/bin/bash
# scripts/run_benchmarks.sh
# Benchmark execution script for NLM

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
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

# Parse command line arguments
VERBOSE=false
BENCHMARK_DIR="${PROJECT_ROOT}/benchmarks"
OUTPUT_FILE="benchmark_results.json"
INTEGRATION=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -o|--output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        -i|--integration)
            INTEGRATION=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose           Enable verbose output"
            echo "  -o, --output FILE       Save results to FILE"
            echo "  -i, --integration       Run integration benchmarks"
            echo "  -h, --help              Show this help message"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create benchmark directory if it doesn't exist
mkdir -p "$BENCHMARK_DIR"

# Build the project
log_info "Building NLM for benchmarks..."
cd "$PROJECT_ROOT"
if ! cmake -B "${PROJECT_ROOT}/build_benchmarks" -S "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release -DNLM_BENCHMARKS=ON; then
    log_error "Build failed"
    exit 1
fi

if ! cmake --build "${PROJECT_ROOT}/build_benchmarks" --config Release --parallel $(nproc); then
    log_error "Build failed"
    exit 1
fi

log_info "Build completed successfully"

# Run benchmarks
log_info "Running NLM benchmarks..."
cd "${PROJECT_ROOT}/build_benchmarks"

if [ "$INTEGRATION" = true ]; then
    if [ -f "./nlm_benchmarks_integration" ]; then
        log_info "Running integration benchmarks..."
        if [ "$VERBOSE" = true ]; then
            ./nlm_benchmarks_integration --verbose > "${BENCHMARK_DIR}/integration_benchmarks.log" 2>&1
        else
            ./nlm_benchmarks_integration > "${BENCHMARK_DIR}/integration_benchmarks.log" 2>&1
        fi
    else
        log_error "Integration benchmark executable not found"
        exit 1
    fi
else
    if [ -f "./nlm_benchmarks" ]; then
        log_info "Running standard benchmarks..."
        if [ "$VERBOSE" = true ]; then
            ./nlm_benchmarks --verbose > "${BENCHMARK_DIR}/benchmarks.log" 2>&1
        else
            ./nlm_benchmarks > "${BENCHMARK_DIR}/benchmarks.log" 2>&1
        fi
    else
        log_error "Benchmark executable not found"
        exit 1
    fi
fi

# Collect results
log_info "Collecting benchmark results..."
cd "$BENCHMARK_DIR"

# Find and copy benchmark result files
for file in *.json; do
    if [ -f "$file" ]; then
        log_info "Found benchmark results: $file"
    fi
done

# Generate summary report
if [ -f "benchmarks.log" ] || [ -f "integration_benchmarks.log" ]; then
    log_info "Benchmark execution completed"
else
    log_warn "No benchmark logs found"
fi

# Save results if output file specified
if [ "$OUTPUT_FILE" != "" ]; then
    log_info "Saving benchmark results to: $OUTPUT_FILE"
    # Copy benchmark results (actual implementation depends on benchmark format)
    for file in *.json; do
        if [ -f "$file" ]; then
            cp "$file" "$OUTPUT_FILE"
            log_info "Saved: $file -> $OUTPUT_FILE"
        fi
    done
fi

log_info "Benchmark execution completed"

exit 0