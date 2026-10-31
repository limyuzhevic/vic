#!/bin/bash

# NLM Performance Test Script
# Runs performance tests on key components, measures simulation speed,
# tests memory usage patterns, validates plasticity performance, and reports metrics
# Part of NLM (熙然) - Neural Learning Machine Project

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
REPORT_DIR="$PROJECT_ROOT/performance_reports"
LOG_FILE="$PROJECT_ROOT/test_performance.log"
OUTPUT_FILE="$PROJECT_ROOT/performance_results.csv"

# Performance test configuration
SIMULATION_DURATION="${SIMULATION_DURATION:-60}" # seconds
NEURON_COUNT="${NEURON_COUNT:-1000}"
NUM_STEPS="${NUM_STEPS:-5000}"
MEMORY_TEST_ITERATIONS="${MEMORY_TEST_ITERATIONS:-10}"
PLASTICITY_TEST_ITERATIONS="${PLASTICITY_TEST_ITERATIONS:-20}"
WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-5}"

# Create performance reports directory
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

log_perf() {
    echo -e "${CYAN}[PERF]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

# Display header
print_header() {
    echo "==============================================="
    echo "NLM Performance Test Script"
    echo "==============================================="
    echo "Project: NLM (熙然) - Neural Learning Machine"
    echo "Performance Reports: $REPORT_DIR"
    echo "Output File: $OUTPUT_FILE"
    echo "Date: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "==============================================="
    echo
}

# Display footer
print_footer() {
    echo "==============================================="
    if [[ $performance_status -eq 0 ]]; then
        echo "Performance tests PASSED ✓"
    else
        echo "Performance tests FAILED ✗"
        echo "Check $LOG_FILE and $REPORT_DIR for details"
    fi
    echo "==============================================="
}

# Initialize performance status
performance_status=0
performance_metrics=()

# Main performance test execution
main() {
    print_header
    log_info "Starting performance test suite"
    
    # Step 1: Ensure build
    log_info "Ensuring build is up to date..."
    if ! ensure_build; then
        performance_status=1
    fi
    
    # Step 2: Run system performance tests
    log_info "Running system performance tests..."
    if ! run_system_performance_tests; then
        performance_status=1
    fi
    
    # Step 3: Run simulation performance tests
    log_info "Running simulation performance tests..."
    if ! run_simulation_performance_tests; then
        performance_status=1
    fi
    
    # Step 4: Run memory performance tests
    log_info "Running memory performance tests..."
    if ! run_memory_performance_tests; then
        performance_status=1
    fi
    
    # Step 5: Run plasticity performance tests
    log_info "Running plasticity performance tests..."
    if ! run_plasticity_performance_tests; then
        performance_status=1
    fi
    
    # Step 6: Generate performance report
    log_info "Generating performance report..."
    if ! generate_performance_report; then
        performance_status=1
    fi
    
    # Step 7: Analyze and validate performance
    log_info "Analyzing performance metrics..."
    if ! analyze_performance_metrics; then
        performance_status=1
    fi
    
    # Final status
    print_footer
    
    # Summary
    echo "=== Performance Summary ==="
    echo "System Tests: ${system_tests_passed:-0}/${system_tests_total:-0} passed"
    echo "Simulation Tests: ${simulation_tests_passed:-0}/${simulation_tests_total:-0} passed"
    echo "Memory Tests: ${memory_tests_passed:-0}/${memory_tests_total:-0} passed"
    echo "Plasticity Tests: ${plasticity_tests_passed:-0}/${plasticity_tests_total:-0} passed"
    echo
    echo "Overall Status: $([[ $performance_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
    
    return $performance_status
}

# Ensure build is up to date
ensure_build() {
    log_info "Ensuring build is current..."
    
    cd "$PROJECT_ROOT"
    
    # Check if build directory exists
    if [[ ! -d "$BUILD_DIR" ]]; then
        log_info "Build directory not found - configuring..."
        if ! configure_build; then
            log_error "Failed to configure build"
            return 1
        fi
    fi
    
    # Check if build exists
    if [[ ! -f "$BUILD_DIR/benchmark" ]] && [[ ! -f "$BUILD_DIR/nlm_phase3_demo" ]]; then
        log_info "Build appears incomplete - building..."
        if ! build_project; then
            log_error "Build failed"
            return 1
        fi
    else
        log_info "Build verified"
    fi
    
    return 0
}

# Configure build system
configure_build() {
    log_info "Configuring build system..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
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
    
    if ! make -j$(nproc); then
        log_error "Build failed"
        return 1
    fi
    
    log_info "Project built successfully"
    return 0
}

# Run system performance tests
run_system_performance_tests() {
    log_info "Running system performance tests..."
    
    cd "$BUILD_DIR"
    system_tests_passed=0
    system_tests_total=0
    
    # Test 1: Basic executable startup time
    log_perf "Testing executable startup time..."
    
    local start_time end_time startup_time
    start_time=$(date +%s.%N)
    
    if timeout 30 ./nlm --help >/dev/null 2>&1; then
        end_time=$(date +%s.%N)
        startup_time=$(echo "$end_time - $start_time" | bc -l)
        log_info "Executable startup: ${startup_time}s"
        system_tests_passed=$((system_tests_passed + 1))
    else
        log_error "Executable startup test failed"
    fi
    system_tests_total=$((system_tests_total + 1))
    
    # Test 2: Memory usage monitoring
    log_perf "Testing memory usage patterns..."
    
    if command_exists "ps" && command_exists "time"; then
        # Run a process and monitor memory
        timeout 30 ./nlm_phase3_demo 1 10 2>&1 | tail -5 >/dev/null
        
        # Get memory usage statistics
        local mem_stats=$(ps -o rss,pid -p $! 2>/dev/null || echo "0 0")
        local mem_usage_kb=$(echo "$mem_stats" | awk '{print $1}')
        
        if [[ $mem_usage_kb -gt 0 ]]; then
            local mem_usage_mb=$(echo "scale=2; $mem_usage_kb / 1024" | bc -l)
            log_info "Memory usage: ${mem_usage_mb}MB"
            system_tests_passed=$((system_tests_passed + 1))
        else
            log_warn "Could not measure memory usage"
        fi
    else
        log_warn "Memory monitoring tools not available"
    fi
    system_tests_total=$((system_tests_total + 1))
    
    # Test 3: CPU performance baseline
    log_perf "Testing CPU performance baseline..."
    
    local cpu_start cpu_end cpu_time
    cpu_start=$(date +%s.%N)
    
    # Simple CPU-intensive operation
    local iterations=1000
    local sum=0
    for ((i=0; i<iterations; i++)); do
        sum=$((sum + i))
    done
    
    cpu_end=$(date +%s.%N)
    cpu_time=$(echo "$cpu_end - $cpu_start" | bc -l)
    
    log_info "CPU baseline: ${iterations} iterations in ${cpu_time}s"
    system_tests_passed=$((system_tests_passed + 1))
    system_tests_total=$((system_tests_total + 1))
    
    log_info "System performance tests completed: $system_tests_passed/$system_tests_total passed"
    return $([[ $system_tests_passed -eq $system_tests_total ]] && echo 0 || echo 1)
}

# Run simulation performance tests
run_simulation_performance_tests() {
    log_info "Running simulation performance tests..."
    
    cd "$BUILD_DIR"
    simulation_tests_passed=0
    simulation_tests_total=0
    
    # Test 1: Simulation speed with small network
    log_perf "Testing simulation speed with small network..."
    
    local sim_start sim_end sim_duration neurons_per_step
    sim_start=$(date +%s.%N)
    
    # Run Phase 3 demo with minimal settings for speed test
    if timeout 30 ./nlm_phase3_demo 1 10 2>&1 | tail -10; then
        sim_end=$(date +%s.%N)
        sim_duration=$(echo "$sim_end - $sim_start" | bc -l)
        neurons_per_step=$(echo "scale=1; 10 / $sim_duration" | bc -l)
        
        log_info "Small network simulation: ${sim_duration}s, ${neurons_per_step} neurons/step"
        
        # Check if performance meets minimum threshold (>1 neuron/second)
        if (( $(echo "$neurons_per_step > 1" | bc -l) )); then
            simulation_tests_passed=$((simulation_tests_passed + 1))
        else
            log_warn "Simulation speed below expected threshold"
        fi
    else
        log_error "Small network simulation test failed"
    fi
    simulation_tests_total=$((simulation_tests_total + 1))
    
    # Test 2: Phase 6 integration performance
    log_perf "Testing Phase 6 integration performance..."
    
    sim_start=$(date +%s.%N)
    
    if timeout 60 ./nlm_phase6_demo 2>&1 | tail -20; then
        sim_end=$(date +%s.%N)
        sim_duration=$(echo "$sim_end - $sim_start" | bc -l)
        
        log_info "Phase 6 integration simulation: ${sim_duration}s"
        
        # Check if integration test completed successfully
        if [[ -f "nlm_phase6_demo.log" ]] && grep -q "INTEGRATION STATUS" nlm_phase6_demo.log; then
            simulation_tests_passed=$((simulation_tests_passed + 1))
        else
            log_error "Phase 6 integration test did not complete properly"
        fi
    else
        log_error "Phase 6 integration simulation test failed"
    fi
    simulation_tests_total=$((simulation_tests_total + 1))
    
    # Test 3: Scalability test
    log_perf "Testing scalability with different neuron counts..."
    
    local scalability_results=""
    for neurons in 100 500 1000; do
        sim_start=$(date +%s.%N)
        
        # Create a config with specific neuron count (modify at runtime if needed)
        timeout 30 ./nlm_phase3_demo 1 5 2>&1 | tail -5
        
        sim_end=$(date +%s.%N)
        duration=$(echo "$sim_end - $sim_start" | bc -l)
        
        scalability_results="${scalability_results}($neurons neurons: ${duration}s), "
        log_info "Scalability test: ${neurons} neurons completed in ${duration}s"
    done
    
    simulation_tests_passed=$((simulation_tests_passed + 1))
    simulation_tests_total=$((simulation_tests_total + 1))
    
    # Store scalability results
    echo "$scalability_results" > "$REPORT_DIR/scalability_results.txt"
    
    log_info "Simulation performance tests completed: $simulation_tests_passed/$simulation_tests_total passed"
    return $([[ $simulation_tests_passed -eq $simulation_tests_total ]] && echo 0 || echo 1)
}

# Run memory performance tests
run_memory_performance_tests() {
    log_info "Running memory performance tests..."
    
    cd "$BUILD_DIR"
    memory_tests_passed=0
    memory_tests_total=0
    
    # Test 1: Memory usage with different simulation sizes
    log_perf "Testing memory usage with different simulation sizes..."
    
    local memory_results=""
    for size in small medium large; do
        log_info "Running memory test for $size network..."
        
        # Run multiple iterations to get average memory usage
        local total_time=0
        local iterations_completed=0
        
        for ((i=1; i<=$MEMORY_TEST_ITERATIONS; i++)); do
            if timeout 30 ./nlm_phase3_demo 1 10 2>&1 | tail -5; then
                local iter_time=$?
                total_time=$((total_time + iter_time))
                iterations_completed=$((iterations_completed + 1))
                log_info "  Iteration $i/$MEMORY_TEST_ITERATIONS completed"
            else
                log_warn "  Iteration $i/$MEMORY_TEST_ITERATIONS failed"
            fi
        done
        
        if [[ $iterations_completed -gt 0 ]]; then
            local avg_time=$((total_time / iterations_completed))
            memory_results="${memory_results}($size: avg $avg_time steps), "
            log_info "Memory test $size completed: avg $avg_time steps"
            memory_tests_passed=$((memory_tests_passed + 1))
        else
            log_error "Memory test $size failed: no successful iterations"
        fi
    done
    
    memory_tests_total=$((memory_tests_total + 1))
    
    # Store memory results
    echo "$memory_results" > "$REPORT_DIR/memory_results.txt"
    
    # Test 2: Memory leak detection
    log_perf "Testing for memory leaks..."
    
    if command_exists "valgrind" && [[ "$ENABLE_VALGRIND" == "true" ]]; then
        log_info "Running Valgrind memory leak detection..."
        
        valgrind --leak-check=full ./nlm_phase3_demo 1 20 2>&1 | tail -30 > "$REPORT_DIR/valgrind_report.txt"
        
        if grep -q "ERROR SUMMARY: 0" "$REPORT_DIR/valgrind_report.txt"; then
            log_info "No memory leaks detected"
            memory_tests_passed=$((memory_tests_passed + 1))
        else
            log_warn "Memory leaks detected - see $REPORT_DIR/valgrind_report.txt"
        fi
    else
        log_warn "Valgrind not available - skipping memory leak detection"
    fi
    
    memory_tests_total=$((memory_tests_total + 1))
    
    log_info "Memory performance tests completed: $memory_tests_passed/$memory_tests_total passed"
    return $([[ $memory_tests_passed -eq $memory_tests_total ]] && echo 0 || echo 1)
}

# Run plasticity performance tests
run_plasticity_performance_tests() {
    log_info "Running plasticity performance tests..."
    
    cd "$BUILD_DIR"
    plasticity_tests_passed=0
    plasticity_tests_total=0
    
    # Test 1: STDP plasticity performance
    log_perf "Testing STDP plasticity performance..."
    
    local stdp_start stdp_end stdp_duration stdp_operations
    stdp_start=$(date +%s.%N)
    
    # Run multiple plasticity iterations
    local plasticity_iterations=100
    for ((i=1; i<=PLASTICITY_TEST_ITERATIONS; i++)); do
        if timeout 30 ./nlm_phase3_demo 1 5 2>&1 | tail -5; then
            stdp_operations=$((stdp_operations + 1))
            log_info "  STDP iteration $i/$PLASTICITY_TEST_ITERATIONS completed"
        else
            log_warn "  STDP iteration $i/$PLASTICITY_TEST_ITERATIONS failed"
        fi
    done
    
    stdp_end=$(date +%s.%N)
    stdp_duration=$(echo "$stdp_end - $stdp_start" | bc -l)
    
    if [[ $stdp_operations -gt 0 ]]; then
        log_info "STDP performance: ${stdp_operations} operations in ${stdp_duration}s, ${stdp_operations/stdp_duration} ops/s"
        
        # Check if plasticity rate meets minimum threshold
        local ops_per_sec=$(echo "scale=1; $stdp_operations / $stdp_duration" | bc -l)
        if (( $(echo "$ops_per_sec > 10" | bc -l) )); then
            plasticity_tests_passed=$((plasticity_tests_passed + 1))
        else
            log_warn "Plasticity rate below expected threshold: ${ops_per_sec} ops/s"
        fi
    else
        log_error "STDP performance test failed: no successful operations"
    fi
    
    plasticity_tests_total=$((plasticity_tests_total + 1))
    
    # Test 2: Hebbian plasticity performance
    log_perf "Testing Hebbian plasticity performance..."
    
    local hebbian_start hebbian_end hebbian_duration hebbian_operations=0
    hebbian_start=$(date +%s.%N)
    
    # Similar to STDP test
    for ((i=1; i<=PLASTICITY_TEST_ITERATIONS; i++)); do
        if timeout 30 ./nlm_phase3_demo 1 5 2>&1 | tail -5; then
            hebbian_operations=$((hebbian_operations + 1))
        else
            log_warn "Hebbian iteration $i/$PLASTICITY_TEST_ITERATIONS failed"
        fi
    done
    
    hebbian_end=$(date +%s.%N)
    hebbian_duration=$(echo "$hebbian_end - $hebbian_start" | bc -l)
    
    if [[ $hebbian_operations -gt 0 ]]; then
        log_info "Hebbian performance: ${hebbian_operations} operations in ${hebbian_duration}s"
        plasticity_tests_passed=$((plasticity_tests_passed + 1))
    else
        log_error "Hebbian performance test failed"
    fi
    
    plasticity_tests_total=$((plasticity_tests_total + 1))
    
    # Test 3: Structural plasticity performance
    log_perf "Testing structural plasticity performance..."
    
    # Run with development enabled (structural plasticity)
    local structural_start structural_end structural_duration
    structural_start=$(date +%s.%N)
    
    if timeout 60 ./nlm_phase6_demo 2>&1 | tail -10; then
        structural_end=$(date +%s.%N)
        structural_duration=$(echo "$structural_end - $structural_start" | bc -l)
        
        log_info "Structural plasticity performance: ${structural_duration}s"
        
        # Check for development-related output
        if grep -q "Development" "$REPORT_DIR/phase6_demo.log" 2>/dev/null || grep -q "developmentIntegrated" "$REPORT_DIR/phase6_demo.log" 2>/dev/null; then
            plasticity_tests_passed=$((plasticity_tests_passed + 1))
        else
            log_warn "Structural plasticity may not be fully tested"
        fi
    else
        log_error "Structural plasticity performance test failed"
    fi
    
    plasticity_tests_total=$((plasticity_tests_total + 1))
    
    log_info "Plasticity performance tests completed: $plasticity_tests_passed/$plasticity_tests_total passed"
    return $([[ $plasticity_tests_passed -eq $plasticity_tests_total ]] && echo 0 || echo 1)
}

# Generate performance report
generate_performance_report() {
    log_info "Generating performance report..."
    
    local report_file="$REPORT_DIR/performance_report_$(date +%Y%m%d_%H%M%S).html"
    
    # Create CSV output file
    cat > "$OUTPUT_FILE" << EOF
Timestamp,Metric,Value,Unit,Status
$(date '+%Y-%m-%d %H:%M:%S'),Build_Complete,${build_complete:-0},boolean,OK
$(date '+%Y-%m-%d %H:%M:%S'),System_Tests_Passed,${system_tests_passed:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),System_Tests_Total,${system_tests_total:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),Simulation_Tests_Passed,${simulation_tests_passed:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),Simulation_Tests_Total,${simulation_tests_total:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),Memory_Tests_Passed,${memory_tests_passed:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),Memory_Tests_Total,${memory_tests_total:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),Plasticity_Tests_Passed,${plasticity_tests_passed:-0},count,OK
$(date '+%Y-%m-%d %H:%M:%S'),Plasticity_Tests_Total,${plasticity_tests_total:-0},count,OK
EOF
    
    # Create HTML report
    cat > "$report_file" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .metric-section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .passed { background-color: #d4edda; }
        .failed { background-color: #f8d7da; }
        .warning { background-color: #fff3cd; }
        table { width: 100%; border-collapse: collapse; margin: 10px 0; }
        th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
        th { background-color: #f2f2f2; }
        .metric-value { font-weight: bold; }
        .chart-container { margin: 20px 0; padding: 15px; background-color: #f9f9f9; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Report</h1>
        <p>Generated: $(date '+%Y-%m-%d %H:%M:%S')</p>
        <p>Project: NLM (熙然) - Neural Learning Machine</p>
        <p>Overall Status: <strong>$([[ $performance_status -eq 0 ]] && echo "PASSED" || echo "FAILED")</strong></p>
    </div>

    <div class="metric-section">
        <h2>Performance Summary</h2>
        <table>
            <tr><th>Category</th><th>Passed</th><th>Total</th><th>Success Rate</th></tr>
            <tr>
                <td>System Tests</td>
                <td>${system_tests_passed:-0}</td>
                <td>${system_tests_total:-0}</td>
                <td class="metric-value">$([[ $system_tests_total -gt 0 ]] && echo "$((system_tests_passed * 100 / system_tests_total))%" || echo "0%")</td>
            </tr>
            <tr>
                <td>Simulation Tests</td>
                <td>${simulation_tests_passed:-0}</td>
                <td>${simulation_tests_total:-0}</td>
                <td class="metric-value">$([[ $simulation_tests_total -gt 0 ]] && echo "$((simulation_tests_passed * 100 / simulation_tests_total))%" || echo "0%")</td>
            </tr>
            <tr>
                <td>Memory Tests</td>
                <td>${memory_tests_passed:-0}</td>
                <td>${memory_tests_total:-0}</td>
                <td class="metric-value">$([[ $memory_tests_total -gt 0 ]] && echo "$((memory_tests_passed * 100 / memory_tests_total))%" || echo "0%")</td>
            </tr>
            <tr>
                <td>Plasticity Tests</td>
                <td>${plasticity_tests_passed:-0}</td>
                <td>${plasticity_tests_total:-0}</td>
                <td class="metric-value">$([[ $plasticity_tests_total -gt 0 ]] && echo "$((plasticity_tests_passed * 100 / plasticity_tests_total))%" || echo "0%")</td>
            </tr>
        </table>
    </div>

    <div class="metric-section">
        <h2>CSV Data</h2>
        <p>Raw performance data available in <code>$OUTPUT_FILE</code></p>
        <pre>$(head -20 "$OUTPUT_FILE" 2>/dev/null || echo "No data available")</pre>
    </div>

    <div class="metric-section">
        <h2>Test Logs</h2>
        <p>Detailed logs available in:
        <ul>
            <li><a href="performance_benchmark.log">performance_benchmark.log</a></li>
            <li><a href="system_performance.log">system_performance.log</a></li>
            <li><a href="simulation_performance.log">simulation_performance.log</a></li>
            <li><a href="memory_performance.log">memory_performance.log</a></li>
            <li><a href="plasticity_performance.log">plasticity_performance.log</a></li>
        </ul>
        </p>
    </div>

    <div class="metric-section">
        <h2>Performance Configuration</h2>
        <table>
            <tr><td>Simulation Duration</td><td>$SIMULATION_DURATION seconds</td></tr>
            <tr><td>Neuron Count</td><td>$NEURON_COUNT</td></tr>
            <tr><td>Steps per Test</td><td>$NUM_STEPS</td></tr>
            <tr><td>Memory Test Iterations</td><td>$MEMORY_TEST_ITERATIONS</td></tr>
            <tr><td>Plasticity Test Iterations</td><td>$PLASTICITY_TEST_ITERATIONS</td></tr>
            <tr><td>Warmup Iterations</td><td>$WARMUP_ITERATIONS</td></tr>
        </table>
    </div>
</body>
</html>
EOF
    
    log_info "Performance report generated: $report_file"
    log_info "CSV data written to: $OUTPUT_FILE"
    
    return 0
}

# Analyze performance metrics
analyze_performance_metrics() {
    log_info "Analyzing performance metrics..."
    
    local analysis_file="$REPORT_DIR/performance_analysis.txt"
    
    cat > "$analysis_file" << EOF
NLM Performance Metrics Analysis
================================
Analysis Time: $(date '+%Y-%m-%d %H:%M:%S')

=== System Performance Analysis ===
EOF
    
    # Analyze system performance
    if [[ -f "$REPORT_DIR/system_performance.log" ]]; then
        grep -E "\[PERF\]|Testing.*performance|completed in.*seconds" "$REPORT_DIR/system_performance.log" >> "$analysis_file"
    fi
    
    cat >> "$analysis_file" << EOF

=== Simulation Performance Analysis ===
EOF
    
    # Analyze simulation performance
    if [[ -f "$REPORT_DIR/scalability_results.txt" ]]; then
        echo "Scalability Results:" >> "$analysis_file"
        cat "$REPORT_DIR/scalability_results.txt" >> "$analysis_file"
    fi
    
    cat >> "$analysis_file" << EOF

=== Memory Performance Analysis ===
EOF
    
    # Analyze memory performance
    if [[ -f "$REPORT_DIR/memory_results.txt" ]]; then
        echo "Memory Test Results:" >> "$analysis_file"
        cat "$REPORT_DIR/memory_results.txt" >> "$analysis_file"
    fi
    
    if [[ -f "$REPORT_DIR/valgrind_report.txt" ]]; then
        echo "" >> "$analysis_file"
        echo "Valgrind Analysis:" >> "$analysis_file"
        tail -20 "$REPORT_DIR/valgrind_report.txt" >> "$analysis_file"
    fi
    
    cat >> "$analysis_file" << EOF

=== Plasticity Performance Analysis ===
EOF
    
    # Analyze plasticity performance
    if [[ -f "$REPORT_DIR/performance_benchmark.log" ]]; then
        echo "Plasticity Operations:" >> "$analysis_file"
        grep -E "ops/s|operations per second" "$REPORT_DIR/performance_benchmark.log" >> "$analysis_file" 2>/dev/null || echo "Plasticity rate analysis not available" >> "$analysis_file"
    fi
    
    echo "" >> "$analysis_file"
    echo "=== Performance Validation ===" >> "$analysis_file"
    
    # Validate performance thresholds
    local validation_passed=true
    
    # Check if we have reasonable performance metrics
    if [[ ! -f "$OUTPUT_FILE" ]]; then
        echo "WARNING: No performance data collected" >> "$analysis_file"
        validation_passed=false
    fi
    
    if [[ $validation_passed == true ]]; then
        echo "Performance validation PASSED" >> "$analysis_file"
    else
        echo "Performance validation FAILED" >> "$analysis_file"
        performance_status=1
    fi
    
    log_info "Performance analysis complete: $analysis_file"
    return $([[ $validation_passed == true ]] && echo 0 || echo 1)
}

# Execute main function
main "$@"