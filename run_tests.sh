#!/bin/bash

# NLM Test Runner Script
# Builds the project if needed, runs all unit tests, demo tests (Phase 3, 4, 6), and integration tests
# Generates test reports and validates test results
# Part of NLM (熙然) - Neural Learning Machine Project

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
TEST_DIR="$PROJECT_ROOT/tests"
REPORT_DIR="$PROJECT_ROOT/test_reports"
LOG_FILE="$PROJECT_ROOT/run_tests.log"
BUILD_CONFIG="${BUILD_CONFIG:-Release}"
PYTHON="${PYTHON:-python3}"
CMAKE="${CMAKE:-cmake}"
MAKE="${MAKE:-make}"

# Test configuration
NUM_UNIT_TESTS="${NUM_UNIT_TESTS:-auto}"
NUM_DEMO_EPISODES="${NUM_DEMO_EPISODES:-5}"
NUM_DEMO_STEPS="${NUM_DEMO_STEPS:-200}"
ENABLE_PERFORMANCE_TESTS="${ENABLE_PERFORMANCE_TESTS:-true}"

# Timing configuration
TOTAL_TIMEOUT="${TOTAL_TIMEOUT:-3600}" # 1 hour in seconds
MAX_TEST_TIME="${MAX_TEST_TIME:-300}" # 5 minutes per test in seconds

# Create test reports directory
mkdir -p "$REPORT_DIR"

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

log_test() {
    echo -e "${BLUE}[TEST]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

# Display header
print_header() {
    echo "==============================================="
    echo "NLM Test Runner Script"
    echo "==============================================="
    echo "Project: NLM (熙然) - Neural Learning Machine"
    echo "Build Type: $BUILD_CONFIG"
    echo "Report Directory: $REPORT_DIR"
    echo "Date: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "==============================================="
    echo
}

# Display footer
print_footer() {
    echo "==============================================="
    if [[ $test_status -eq 0 ]]; then
        echo "All tests PASSED ✓"
    else
        echo "Some tests FAILED ✗"
        echo "Check $LOG_FILE and $REPORT_DIR for details"
    fi
    echo "==============================================="
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Initialize test status
test_status=0
unit_tests_passed=0
unit_tests_total=0
demo_tests_passed=0
demo_tests_total=0
performance_tests_passed=0
performance_tests_total=0

# Main test execution
main() {
    print_header
    log_info "Starting test suite execution"
    
    # Step 1: Ensure build
    log_info "Ensuring build is up to date..."
    if ! ensure_build; then
        test_status=1
    fi
    
    # Step 2: Run unit tests
    log_info "Running unit tests..."
    if ! run_unit_tests; then
        test_status=1
    fi
    
    # Step 3: Run demo tests (Phase 3, 4, 6)
    log_info "Running demo tests (Phase 3, 4, 6)..."
    if ! run_demo_tests; then
        test_status=1
    fi
    
    # Step 4: Run performance tests (if enabled)
    if [[ "$ENABLE_PERFORMANCE_TESTS" == "true" ]]; then
        log_info "Running performance tests..."
        if ! run_performance_tests; then
            test_status=1
        fi
    else
        log_warn "Performance tests disabled - skipping"
    fi
    
    # Step 5: Generate test reports
    log_info "Generating test reports..."
    if ! generate_test_reports; then
        test_status=1
    fi
    
    # Step 6: Validate test results
    log_info "Validating test results..."
    if ! validate_test_results; then
        test_status=1
    fi
    
    # Final status
    print_footer
    
    # Summary
    echo "=== Test Summary ==="
    echo "Unit Tests: $unit_tests_passed/$unit_tests_total passed"
    echo "Demo Tests: $demo_tests_passed/$demo_tests_total passed"
    echo "Performance Tests: $performance_tests_passed/$performance_tests_total passed"
    echo
    echo "Overall Status: $([[ $test_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
    
    return $test_status
}

# Ensure build is up to date
ensure_build() {
    log_info "Ensuring build is current..."
    
    cd "$PROJECT_ROOT"
    
    # Check if we need to rebuild
    local build_stale=0
    local build_files=(
        "CMakeLists.txt"
        "src/CMakeLists.txt"
        "pyproject.toml"
    )
    
    # Check if build directory exists
    if [[ ! -d "$BUILD_DIR" ]]; then
        log_info "Build directory not found - configuring..."
        if ! configure_build; then
            log_error "Failed to configure build"
            return 1
        fi
    fi
    
    # Check if CMakeLists.txt has changed since last build
    if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
        for file in "${build_files[@]}"; do
            if [[ "$file" -nt "$BUILD_DIR/CMakeCache.txt" ]]; then
                log_info "Build appears stale - $file modified after build"
                build_stale=1
                break
            fi
        done
    else
        build_stale=1
    fi
    
    # Rebuild if stale or necessary
    if [[ $build_stale -eq 1 ]]; then
        log_info "Rebuilding project..."
        if ! build_project; then
            log_error "Build failed"
            return 1
        fi
    else
        log_info "Build appears up to date - skipping rebuild"
    fi
    
    log_info "Build verified"
    return 0
}

# Configure build system
configure_build() {
    log_info "Configuring build system..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if ! $CMAKE .. -DCMAKE_BUILD_TYPE=$BUILD_CONFIG; then
        log_error "CMake configuration failed"
        return 1
    fi
    
    log_info "Build system configured successfully"
    return 0
}

# Build project
build_project() {
    log_info "Building project..."
    
    cd "$BUILD_DIR"
    
    if ! $MAKE -j$(nproc); then
        log_error "Build failed"
        return 1
    fi
    
    log_info "Project built successfully"
    return 0
}

# Run unit tests
run_unit_tests() {
    log_info "Running unit tests..."
    
    cd "$BUILD_DIR"
    
    # Run the C++ unit test executable
    log_test "Running C++ unit tests (test_main executable)..."
    
    # Check if test executable exists
    if [[ ! -f "test_main" ]]; then
        log_error "Unit test executable 'test_main' not found"
        return 1
    fi
    
    # Run unit tests with timeout
    timeout $MAX_TEST_TIME ./test_main 2>&1 | tee "$REPORT_DIR/unit_tests.log"
    
    local unit_test_exit_code=$?
    
    if [[ $unit_test_exit_code -eq 0 ]]; then
        log_info "Unit tests PASSED"
        unit_tests_passed=$((unit_tests_passed + 1))
        unit_tests_total=$((unit_tests_total + 1))
    elif [[ $unit_test_exit_code -eq 124 ]]; then
        log_error "Unit tests timed out after ${MAX_TEST_TIME}s"
        test_status=1
        unit_tests_total=$((unit_tests_total + 1))
    else
        log_error "Unit tests FAILED with exit code $unit_test_exit_code"
        test_status=1
        unit_tests_total=$((unit_tests_total + 1))
    fi
    
    # Analyze unit test output
    if [[ -f "$REPORT_DIR/unit_tests.log" ]]; then
        local test_count=$(grep -c "PASSED\|FAILED" "$REPORT_DIR/unit_tests.log" || echo 0)
        local passed_count=$(grep -c "PASSED" "$REPORT_DIR/unit_tests.log" || echo 0)
        
        log_info "Unit test results: $passed_count/$test_count tests passed"
        
        # Create unit test summary
        cat > "$REPORT_DIR/unit_tests_summary.json" << EOF
{
    "status": "$([[ $unit_test_exit_code -eq 0 ]] && echo "PASSED" || echo "FAILED")",
    "total_tests": $test_count,
    "passed_tests": $passed_count,
    "failed_tests": $((test_count - passed_count)),
    "exit_code": $unit_test_exit_code,
    "execution_time": "$(date '+%Y-%m-%d %H:%M:%S')"
}
EOF
    fi
    
    return $([[ $unit_test_exit_code -eq 0 ]] && echo 0 || echo 1)
}

# Run demo tests
run_demo_tests() {
    log_info "Running demo tests..."
    
    cd "$BUILD_DIR"
    
    # Test 1: Phase 3 demo
    log_test "Running Phase 3 demo..."
    
    if ! [[ -f "nlm_phase3_demo" ]]; then
        log_error "Phase 3 demo executable not found"
        return 1
    fi
    
    timeout $MAX_TEST_TIME ./nlm_phase3_demo $NUM_DEMO_EPISODES $NUM_DEMO_STEPS 2>&1 | tee "$REPORT_DIR/phase3_demo.log"
    
    local phase3_exit_code=$?
    
    if [[ $phase3_exit_code -eq 0 ]]; then
        log_info "Phase 3 demo PASSED"
        demo_tests_passed=$((demo_tests_passed + 1))
        demo_tests_total=$((demo_tests_total + 1))
    elif [[ $phase3_exit_code -eq 124 ]]; then
        log_error "Phase 3 demo timed out after ${MAX_TEST_TIME}s"
        test_status=1
        demo_tests_total=$((demo_tests_total + 1))
    else
        log_error "Phase 3 demo FAILED with exit code $phase3_exit_code"
        test_status=1
        demo_tests_total=$((demo_tests_total + 1))
    fi
    
    # Analyze Phase 3 demo output
    if [[ -f "$REPORT_DIR/phase3_demo.log" ]]; then
        if grep -q "PASSED\|SUCCESS" "$REPORT_DIR/phase3_demo.log"; then
            echo "Phase 3 demo completed successfully" > "$REPORT_DIR/phase3_demo_status.txt"
        else
            echo "Phase 3 demo failed or incomplete" > "$REPORT_DIR/phase3_demo_status.txt"
        fi
    fi
    
    # Test 2: Phase 4 demo
    log_test "Running Phase 4 demo..."
    
    if ! [[ -f "nlm_phase4_demo" ]]; then
        log_error "Phase 4 demo executable not found"
        return 1
    fi
    
    # Parse NUM_DEMO_TRIALS from Phase 4 demo structure
    local phase4_trials=${NUM_DEMO_TRIALS:-50}
    timeout $MAX_TEST_TIME ./nlm_phase4_demo $phase4_trials 2>&1 | tee "$REPORT_DIR/phase4_demo.log"
    
    local phase4_exit_code=$?
    
    if [[ $phase4_exit_code -eq 0 ]]; then
        log_info "Phase 4 demo PASSED"
        demo_tests_passed=$((demo_tests_passed + 1))
        demo_tests_total=$((demo_tests_total + 1))
    elif [[ $phase4_exit_code -eq 124 ]]; then
        log_error "Phase 4 demo timed out after ${MAX_TEST_TIME}s"
        test_status=1
        demo_tests_total=$((demo_tests_total + 1))
    else
        log_error "Phase 4 demo FAILED with exit code $phase4_exit_code"
        test_status=1
        demo_tests_total=$((demo_tests_total + 1))
    fi
    
    # Test 3: Phase 6 demo
    log_test "Running Phase 6 demo (integration)..."
    
    if ! [[ -f "nlm_phase6_demo" ]]; then
        log_error "Phase 6 demo executable not found"
        return 1
    fi
    
    # Run with smaller settings for faster testing
    timeout $MAX_TEST_TIME ./nlm_phase6_demo 2>&1 | tee "$REPORT_DIR/phase6_demo.log"
    
    local phase6_exit_code=$?
    
    if [[ $phase6_exit_code -eq 0 ]]; then
        log_info "Phase 6 demo PASSED"
        demo_tests_passed=$((demo_tests_passed + 1))
        demo_tests_total=$((demo_tests_total + 1))
    elif [[ $phase6_exit_code -eq 124 ]]; then
        log_error "Phase 6 demo timed out after ${MAX_TEST_TIME}s"
        test_status=1
        demo_tests_total=$((demo_tests_total + 1))
    else
        log_error "Phase 6 demo FAILED with exit code $phase6_exit_code"
        test_status=1
        demo_tests_total=$((demo_tests_total + 1))
    fi
    
    # Create demo test summary
    cat > "$REPORT_DIR/demo_tests_summary.json" << EOF
{
    "phase3": {
        "status": "$([[ $phase3_exit_code -eq 0 ]] && echo "PASSED" || echo "FAILED")",
        "exit_code": $phase3_exit_code,
        "execution_time": "$(date '+%Y-%m-%d %H:%M:%S')"
    },
    "phase4": {
        "status": "$([[ $phase4_exit_code -eq 0 ]] && echo "PASSED" || echo "FAILED")",
        "exit_code": $phase4_exit_code,
        "execution_time": "$(date '+%Y-%m-%d %H:%M:%S')"
    },
    "phase6": {
        "status": "$([[ $phase6_exit_code -eq 0 ]] && echo "PASSED" || echo "FAILED")",
        "exit_code": $phase6_exit_code,
        "execution_time": "$(date '+%Y-%m-%d %H:%M:%S')"
    },
    "total_passed": $demo_tests_passed,
    "total_tests": $demo_tests_total
}
EOF
    
    return $([[ $phase3_exit_code -eq 0 && $phase4_exit_code -eq 0 && $phase6_exit_code -eq 0 ]] && echo 0 || echo 1)
}

# Run performance tests
run_performance_tests() {
    log_info "Running performance tests..."
    
    cd "$PROJECT_ROOT"
    
    # Check if benchmark executable exists
    if [[ -f "$BUILD_DIR/benchmark" ]]; then
        log_test "Running benchmark executable..."
        
        timeout $MAX_TEST_TIME ./benchmark 2>&1 | tee "$REPORT_DIR/performance_benchmark.log"
        
        local benchmark_exit_code=$?
        
        if [[ $benchmark_exit_code -eq 0 ]]; then
            log_info "Performance benchmark PASSED"
            performance_tests_passed=$((performance_tests_passed + 1))
            performance_tests_total=$((performance_tests_total + 1))
        elif [[ $benchmark_exit_code -eq 124 ]]; then
            log_error "Performance benchmark timed out after ${MAX_TEST_TIME}s"
            test_status=1
            performance_tests_total=$((performance_tests_total + 1))
        else
            log_error "Performance benchmark FAILED with exit code $benchmark_exit_code"
            test_status=1
            performance_tests_total=$((performance_tests_total + 1))
        fi
        
        # Analyze benchmark results
        if [[ -f "$REPORT_DIR/performance_benchmark.log" ]]; then
            # Extract key performance metrics if available
            local sim_speed=$(grep -o "speed:[[:space:]]*[0-9.]*" "$REPORT_DIR/performance_benchmark.log" | head -1)
            local mem_usage=$(grep -o "memory:[[:space:]]*[0-9.]*" "$REPORT_DIR/performance_benchmark.log" | head -1)
            
            cat > "$REPORT_DIR/performance_metrics.json" << EOF
{
    "benchmark": {
        "status": "$([[ $benchmark_exit_code -eq 0 ]] && echo "PASSED" || echo "FAILED")",
        "exit_code": $benchmark_exit_code,
        "simulation_speed": "${sim_speed:-N/A}",
        "memory_usage": "${mem_usage:-N/A}",
        "execution_time": "$(date '+%Y-%m-%d %H:%M:%S')"
    }
}
EOF
        fi
    else
        log_warn "Benchmark executable not found - skipping performance tests"
    fi
    
    return $([[ $benchmark_exit_code -eq 0 ]] && echo 0 || echo 1)
}

# Generate test reports
generate_test_reports() {
    log_info "Generating comprehensive test reports..."
    
    local report_file="$REPORT_DIR/test_report_$(date +%Y%m%d_%H%M%S).html"
    
    cat > "$report_file" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .test-section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .passed { background-color: #d4edda; }
        .failed { background-color: #f8d7da; }
        .warning { background-color: #fff3cd; }
        table { width: 100%; border-collapse: collapse; margin: 10px 0; }
        th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
        th { background-color: #f2f2f2; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Test Report</h1>
        <p>Generated: $(date '+%Y-%m-%d %H:%M:%S')</p>
        <p>Project: NLM (熙然) - Neural Learning Machine</p>
        <p>Build Type: $BUILD_CONFIG</p>
        <p>Overall Status: <strong>$([[ $test_status -eq 0 ]] && echo "PASSED" || echo "FAILED")</strong></p>
    </div>

    <div class="test-section">
        <h2>Test Summary</h2>
        <table>
            <tr><th>Test Type</th><th>Passed</th><th>Total</th><th>Status</th></tr>
            <tr>
                <td>Unit Tests</td>
                <td>$unit_tests_passed</td>
                <td>$unit_tests_total</td>
                <td class="$([[ $unit_tests_passed -eq $unit_tests_total ]] && echo "passed" || echo "failed")">
                    $([[ $unit_tests_passed -eq $unit_tests_total ]] && echo "PASSED" || echo "FAILED")
                </td>
            </tr>
            <tr>
                <td>Demo Tests</td>
                <td>$demo_tests_passed</td>
                <td>$demo_tests_total</td>
                <td class="$([[ $demo_tests_passed -eq $demo_tests_total ]] && echo "passed" || echo "failed")">
                    $([[ $demo_tests_passed -eq $demo_tests_total ]] && echo "PASSED" || echo "FAILED")
                </td>
            </tr>
            <tr>
                <td>Performance Tests</td>
                <td>$performance_tests_passed</td>
                <td>$performance_tests_total</td>
                <td class="$([[ $performance_tests_passed -eq $performance_tests_total ]] && echo "passed" || echo "failed")">
                    $([[ $performance_tests_passed -eq $performance_tests_total ]] && echo "PASSED" || echo "FAILED")
                </td>
            </tr>
        </table>
    </div>

    <div class="test-section">
        <h2>Execution Logs</h2>
        <p>Detailed logs available in:
        <ul>
            <li><a href="unit_tests.log">unit_tests.log</a></li>
            <li><a href="phase3_demo.log">phase3_demo.log</a></li>
            <li><a href="phase4_demo.log">phase4_demo.log</a></li>
            <li><a href="phase6_demo.log">phase6_demo.log</a></li>
        </ul>
        </p>
    </div>

    <div class="test-section">
        <h2>Test Configuration</h2>
        <table>
            <tr><td>Build Type</td><td>$BUILD_CONFIG</td></tr>
            <tr><td>Python Executable</td><td>$PYTHON</td></tr>
            <tr><td>CMake Executable</td><td>$CMAKE</td></tr>
            <tr><td>Make Executable</td><td>$MAKE</td></tr>
            <tr><td>Number of CPU Cores</td><td>$(nproc)</td></tr>
        </table>
    </div>
</body>
</html>
EOF
    
    log_info "Test report generated: $report_file"
    
    # Copy log files to reports directory
    if [[ -d "$BUILD_DIR" ]]; then
        cp "$BUILD_DIR/test_main.log" "$REPORT_DIR/" 2>/dev/null || true
        cp "$BUILD_DIR/nlm_phase3_demo.log" "$REPORT_DIR/" 2>/dev/null || true
        cp "$BUILD_DIR/nlm_phase4_demo.log" "$REPORT_DIR/" 2>/dev/null || true
        cp "$BUILD_DIR/nlm_phase6_demo.log" "$REPORT_DIR/" 2>/dev/null || true
        cp "$BUILD_DIR/benchmark.log" "$REPORT_DIR/" 2>/dev/null || true
    fi
    
    return 0
}

# Validate test results
validate_test_results() {
    log_info "Validating test results..."
    
    local validation_errors=0
    
    # Check if test reports exist
    if [[ ! -d "$REPORT_DIR" ]] || [[ $(ls -la "$REPORT_DIR" | wc -l) -eq 2 ]]; then
        log_error "No test reports generated - validation failed"
        validation_errors=$((validation_errors + 1))
    fi
    
    # Check unit test summary
    if [[ -f "$REPORT_DIR/unit_tests_summary.json" ]]; then
        log_info "Unit test summary found"
    else
        log_error "Unit test summary not found"
        validation_errors=$((validation_errors + 1))
    fi
    
    # Check demo test summary
    if [[ -f "$REPORT_DIR/demo_tests_summary.json" ]]; then
        log_info "Demo test summary found"
    else
        log_error "Demo test summary not found"
        validation_errors=$((validation_errors + 1))
    fi
    
    # Check if any critical test failed
    if [[ $test_status -ne 0 ]]; then
        log_error "Critical tests failed - validation failed"
        validation_errors=$((validation_errors + 1))
    fi
    
    if [[ $validation_errors -eq 0 ]]; then
        log_info "Test validation PASSED"
        return 0
    else
        log_error "Test validation failed with $validation_errors errors"
        return 1
    fi
}

# Execute main function
main "$@"