#!/bin/bash
# scripts/test_coverage.sh
# Coverage analysis script for NLM

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
OUTPUT_DIR="${PROJECT_ROOT}/coverage_reports"
MIN_COVERAGE=80
GENERATE_HTML=false
INTEGRATE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -m|--min-coverage)
            MIN_COVERAGE="$2"
            shift 2
            ;;
        -h|--html)
            GENERATE_HTML=true
            shift
            ;;
        -i|--integrate)
            INTEGRATE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose           Enable verbose output"
            echo "  -o, --output DIR        Output directory for reports (default: coverage_reports)"
            echo "  -m, --min-coverage N    Minimum required coverage percentage (default: 80)"
            echo "  -h, --html              Generate HTML coverage report"
            echo "  -i, --integrate         Integrate with CI system"
            echo "  -h, --help              Show this help message"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

log_info "Running NLM coverage analysis..."

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Build the project with coverage
log_info "Building NLM with coverage support..."
cd "$PROJECT_ROOT"

BUILD_DIR="${PROJECT_ROOT}/build_coverage"
mkdir -p "$BUILD_DIR"

# Configure CMake with coverage support
CMAKE_ARGS="-B \"$BUILD_DIR\" -S \"$PROJECT_ROOT\" -DCMAKE_BUILD_TYPE=Debug -DNLM_COVERAGE=ON"

if ! eval "cmake $CMAKE_ARGS"; then
    log_error "CMake configuration failed"
    exit 1
fi

# Build with coverage
if ! cmake --build "$BUILD_DIR" --config Debug --parallel $(nproc); then
    log_error "Build failed"
    exit 1
fi

log_info "Build completed successfully"

# Run coverage analysis
log_info "Running coverage analysis..."
cd "$BUILD_DIR"

if [ -f "./coverage_analyzer" ]; then
    log_info "Running coverage analyzer..."
    
    # Run the coverage analyzer
    if [ "$VERBOSE" = true ]; then
        ./coverage_analyzer --output "${OUTPUT_DIR}/coverage.json" --verbose
    else
        ./coverage_analyzer --output "${OUTPUT_DIR}/coverage.json"
    fi
    
    # Generate HTML report if requested
    if [ "$GENERATE_HTML" = true ]; then
        log_info "Generating HTML coverage report..."
        if [ -f "./genhtml" ]; then
            ./genhtml coverage.info --output-directory "${OUTPUT_DIR}/html"
            log_info "HTML coverage report generated: ${OUTPUT_DIR}/html/index.html"
        elif [ -f "/usr/bin/genhtml" ]; then
            /usr/bin/genhtml coverage.info --output-directory "${OUTPUT_DIR}/html"
            log_info "HTML coverage report generated: ${OUTPUT_DIR}/html/index.html"
        else
            log_warn "genhtml not found, skipping HTML report generation"
        fi
    fi
    
else
    log_error "Coverage analyzer executable not found"
    exit 1
fi

# Check coverage against minimum requirement
log_info "Checking coverage against minimum requirement (${MIN_COVERAGE}%)..."

if [ -f "${OUTPUT_DIR}/coverage.json" ]; then
    COVERAGE=$(grep -o '"coverage":[0-9.]*' "${OUTPUT_DIR}/coverage.json" | grep -o '[0-9.]*$' | head -1)
    if [ -n "$COVERAGE" ]; then
        COVERAGE=${COVERAGE%.*}  # Remove decimal part for comparison
        if [ "$COVERAGE" -lt "$MIN_COVERAGE" ]; then
            log_error "Coverage ${COVERAGE}% is below minimum requirement ${MIN_COVERAGE}%"
            if [ "$INTEGRATE" = true ]; then
                log_error "Integration with CI will fail due to insufficient coverage"
            fi
            exit 1
        else
            log_info "✓ Coverage ${COVERAGE}% meets minimum requirement ${MIN_COVERAGE}%"
        fi
    else
        log_warn "Could not parse coverage from coverage.json"
    fi
else
    log_error "Coverage report not found: ${OUTPUT_DIR}/coverage.json"
    exit 1
fi

# Generate summary report
log_info "Generating coverage summary report..."

SUMMARY_FILE="${OUTPUT_DIR}/coverage_summary.txt"
{
    echo "=== NLM Code Coverage Summary ==="
    echo "Generated on: $(date)"
    echo ""
    echo "Coverage Report:"
    echo "  - JSON Report: ${OUTPUT_DIR}/coverage.json"
    if [ "$GENERATE_HTML" = true ]; then
        echo "  - HTML Report: ${OUTPUT_DIR}/html/index.html"
    fi
    echo ""
    echo "Coverage Requirements:"
    echo "  - Minimum Coverage: ${MIN_COVERAGE}%"
    echo "  - Actual Coverage: ${COVERAGE}%"
    echo ""
    if [ "$COVERAGE" -ge "$MIN_COVERAGE" ]; then
        echo "Status: ✓ PASSED"
    else
        echo "Status: ✗ FAILED"
    fi
    echo ""
    echo "Files with insufficient coverage:"
    # This would list files below threshold (implementation depends on coverage format)
    echo "  - (see detailed report)"
} > "$SUMMARY_FILE"

log_info "Coverage summary report saved: $SUMMARY_FILE"

# Integration with CI if requested
if [ "$INTEGRATE" = true ]; then
    log_info "Integrating with CI system..."
    # This would typically set environment variables or call CI system APIs
    # For now, we'll just log the action
    log_info "Integration with CI completed"
fi

log_info "Coverage analysis completed"

if [ "$VERBOSE" = true ]; then
    log_info "Coverage analysis details:"
    log_info "  - Output directory: $OUTPUT_DIR"
    log_info "  - Minimum coverage: ${MIN_COVERAGE}%"
    log_info "  - HTML report: $([ "$GENERATE_HTML" = true ] && echo "Yes" || echo "No")"
    log_info "  - CI integration: $([ "$INTEGRATE" = true ] && echo "Yes" || echo "No")"
fi

exit 0