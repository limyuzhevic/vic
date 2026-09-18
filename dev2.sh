#!/usr/bin/env bash

# NLM Development and Testing Environment
# This script provides comprehensive development and testing tools for NLM Phase 6

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Configuration
PYTHON_DIR="${SCRIPT_DIR}/python"
SOURCE_DIR="${SCRIPT_DIR}/src"
DOCS_DIR="${SCRIPT_DIR}/docs"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Performance metrics
PERF_START_TIME=""
PERF_END_TIME=""

# Logging
LOG_FILE="${SCRIPT_DIR}/dev.log"
function log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "${LOG_FILE}"
}

# Error handling
function handle_error() {
    log "ERROR: Command failed with exit code $?"
    echo -e "${RED}Error occurred. Check ${LOG_FILE} for details.${NC}"
    exit 1
}

trap handle_error ERR

# Performance tracking
function start_perf() {
    PERF_START_TIME="$(date +%s)"
}

function end_perf() {
    PERF_END_TIME="$(date +%s)"
    local elapsed=$((PERF_END_TIME - PERF_START_TIME))
    echo "Performance: ${elapsed} seconds"
}

# Check dependencies
function check_dependencies() {
    log "Checking development dependencies..."
    
    local deps_ok=true
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null; then
        echo -e "${RED}Error: g++ not found${NC}"
        deps_ok=false
    else
        echo -e "${GREEN}✓ g++ found${NC}"
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: cmake not found${NC}"
        deps_ok=false
    else
        echo -e "${GREEN}✓ cmake found${NC}"
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}Error: python3 not found${NC}"
        deps_ok=false
    else
        echo -e "${GREEN}✓ python3 found${NC}"
    fi
    
    # Check for pip
    if ! command -v pip3 &> /dev/null; then
        echo -e "${YELLOW}Warning: pip3 not found. Python bindings may not work.${NC}"
    else
        echo -e "${GREEN}✓ pip3 found${NC}"
    fi
    
    if [ "$deps_ok" = false ]; then
        echo -e "${RED}Missing dependencies. Please install required tools.${NC}"
        return 1
    fi
    
    log "All dependencies satisfied"
    return 0
}

# Initialize project
function initialize_project() {
    log "Initializing NLM Phase 6 development environment..."
    
    # Create build directory
    if [ ! -d "${BUILD_DIR}" ]; then
        echo "Creating build directory..."
        mkdir -p "${BUILD_DIR}"
    fi
    
    # Check for required files
    if [ ! -f "${SCRIPT_DIR}/pyproject.toml" ]; then
        echo -e "${YELLOW}Warning: pyproject.toml not found. Python bindings may not be properly configured.${NC}"
    fi
    
    if [ ! -f "${SCRIPT_DIR}/CMakeLists.txt" ]; then
        echo -e "${RED}Error: CMakeLists.txt not found in project root.${NC}"
        return 1
    fi
    
    log "Project initialization completed"
    return 0
}

# Build NLM
function build_nlm() {
    log "Building NLM..."
    
    cd "${BUILD_DIR}"
    
    # Configure with CMake
    echo "Configuring CMake..."
    cmake "${SCRIPT_DIR}" -DCMAKE_BUILD_TYPE=Release -GNinja
    
    # Build
    echo "Building NLM..."
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ NLM build completed successfully${NC}"
        log "NLM build completed successfully"
        return 0
    else
        echo -e "${RED}Error: NLM build failed${NC}"
        log "NLM build failed"
        return 1
    fi
}

# Build Python bindings
function build_python_bindings() {
    log "Building Python bindings..."
    
    cd "${SCRIPT_DIR}"
    
    if [ ! -f "pyproject.toml" ]; then
        echo -e "${YELLOW}Warning: pyproject.toml not found. Skipping Python bindings build.${NC}"
        log "Python bindings skipped (pyproject.toml not found)"
        return 0
    fi
    
    echo "Building Python bindings..."
    pip3 install -e . > "${LOG_FILE}" 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Python bindings built successfully${NC}"
        log "Python bindings built successfully"
        return 0
    else
        echo -e "${RED}Error: Python bindings build failed${NC}"
        log "Python bindings build failed"
        return 1
    fi
}

# Run tests
function run_tests() {
    log "Running NLM tests..."
    
    cd "${BUILD_DIR}"
    
    if [ ! -f "CTestTestfile.cmake" ]; then
        echo -e "${YELLOW}Warning: No tests found. Build may have been incomplete.${NC}"
        log "No tests found"
        return 0
    fi
    
    echo "Running tests..."
    ctest --output-on-failure
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ All tests passed${NC}"
        log "All tests passed"
        return 0
    else
        echo -e "${RED}Error: Some tests failed${NC}"
        log "Some tests failed"
        return 1
    fi
}

# Run examples
function run_examples() {
    log "Running NLM examples..."
    
    cd "${BUILD_DIR}"
    
    # Check available executables
    local execs=("nlm" "nlm_phase3_demo" "nlm_phase4_demo" "nlm_phase6_demo")
    local ran_any=false
    
    for exe in "${execs[@]}"; do
        if [ -f "${exe}" ]; then
            echo "Running ${exe}..."
            ./"${exe}"
            
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}✓ ${exe} completed successfully${NC}"
                ran_any=true
            else
                echo -e "${RED}Error: ${exe} failed${NC}"
            fi
        fi
    done
    
    if [ "$ran_any" = false ]; then
        echo -e "${YELLOW}Warning: No executables found. Build may have been incomplete.${NC}"
        log "No executables found"
    fi
    
    log "Examples completed"
}

# Run benchmarks
function run_benchmarks() {
    log "Running NLM benchmarks..."
    
    cd "${BUILD_DIR}"
    
    if [ ! -f "nlm_benchmark" ]; then
        echo -e "${YELLOW}Warning: nlm_benchmark not found. Skipping benchmarks.${NC}"
        log "Benchmark executable not found"
        return 0
    fi
    
    echo "Running comprehensive benchmarks..."
    ./nlm_benchmark --mode comprehensive
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Benchmarks completed successfully${NC}"
        log "Benchmarks completed successfully"
        return 0
    else
        echo -e "${RED}Error: Benchmarks failed${NC}"
        log "Benchmarks failed"
        return 1
    fi
}

# Show documentation
function show_docs() {
    log "Displaying NLM documentation..."
    
    echo -e "${CYAN}=== NLM Phase 6 Documentation ===${NC}"
    echo ""
    echo -e "${YELLOW}Key Documentation Files:${NC}"
    echo "  ${DOCS_DIR}/README.md              - Project overview and setup"
    echo "  ${DOCS_DIR}/ARCHITECTURE.md        - Technical architecture"
    echo "  ${DOCS_DIR}/PHASE6_FINAL_AUDIT.md   - Phase 6 implementation details"
    echo "  ${DOCS_DIR}/SCIENCE.md             - Scientific background"
    echo "  ${DOCS_DIR}/EXPERIMENTS.md         - Experiment documentation"
    echo ""
    echo -e "${YELLOW}Key Source Files:${NC}"
    echo "  ${SOURCE_DIR}/agent/AgentBrain.hpp      - Agent brain interface"
    echo "  ${SOURCE_DIR}/agent/AgentBrain.cpp      - Agent brain implementation"
    echo "  ${SOURCE_DIR}/agent/SensoryPercept.hpp - Sensory perception"
    echo "  ${SOURCE_DIR}/world/SimpleWorld.hpp   - Environment world"
    echo ""
    echo -e "${YELLOW}Python Bindings:${NC}"
    echo "  ${PYTHON_DIR}/bindings.cpp             - PyBind11 bindings"
    echo "  pyproject.toml                         - Package configuration"
    echo ""
    echo "For detailed documentation, see the respective files."
}

# Test Python bindings
function test_python_bindings() {
    log "Testing Python bindings..."
    
    echo "Testing Python bindings..."
    
    if python3 -c "import pynlm; print('Python bindings loaded successfully')" 2>/dev/null; then
        echo -e "${GREEN}✓ Python bindings are working${NC}"
        
        # Test basic functionality
        echo "Testing basic functionality..."
        python3 << 'EOF'
import pynlm

# Test basic creation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test methods
print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Initial firing rate: {brain.getAverageFiringRate():.2f}")

# Test world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

print(f"✓ World configured: {world.getWidth()}x{world.getHeight()}")

# Test agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"✓ Agent created with sensory input size: {agent.getSensoryInputSize()}")
print(f"✓ Agent motor output size: {agent.getMotorOutputSize()}")

print("\n✓ All Python bindings tests passed")
EOF
        
        return 0
    else
        echo -e "${RED}Error: Python bindings are not working${NC}"
        log "Python bindings test failed"
        return 1
    fi
}

# Generate documentation
function generate_docs() {
    log "Generating documentation..."
    
    echo "Generating documentation..."
    
    # Check if Doxygen is available
    if command -v doxygen &> /dev/null; then
        echo "Generating API documentation with Doxygen..."
        doxygen Doxyfile 2>/dev/null || echo "Warning: Doxygen documentation generation failed"
        echo -e "${GREEN}✓ Documentation generated${NC}"
    else
        echo -e "${YELLOW}Warning: doxygen not found. Skipping API documentation generation.${NC}"
    fi
    
    # Copy documentation files
    echo "Copying documentation files..."
    
    if [ -f "${SCRIPT_DIR}/README.md" ]; then
        cp "${SCRIPT_DIR}/README.md" "${SCRIPT_DIR}/README_BUILD.md"
    fi
    
    echo -e "${GREEN}✓ Documentation generation completed${NC}"
}

# Clean build directory
function clean_build() {
    log "Cleaning build directory..."
    
    if [ -d "${BUILD_DIR}" ]; then
        echo "Removing build directory..."
        rm -rf "${BUILD_DIR}"/*
        echo -e "${GREEN}✓ Build directory cleaned${NC}"
    else
        echo -e "${YELLOW}Warning: Build directory not found.${NC}"
    fi
}

# Show performance metrics
function show_performance_metrics() {
    log "Showing performance metrics..."
    
    if [ -f "${LOG_FILE}" ]; then
        echo -e "${CYAN}=== Development Log (Last 20 entries) ===${NC}"
        tail -n 20 "${LOG_FILE}"
    else
        echo -e "${YELLOW}No log file found. Run commands first to generate logs.${NC}"
    fi
}

# Show help
function show_help() {
    echo -e "${CYAN}=== NLM Development and Testing Environment ===${NC}"
    echo ""
    echo -e "${YELLOW}Usage:${NC}"
    echo "  $0 [command] [options]"
    echo ""
    echo -e "${YELLOW}Commands:${NC}"
    echo "  build              - Build NLM from source"
    echo "  build-python       - Build Python bindings only"
    echo "  test               - Run NLM tests"
    echo "  run                - Run NLM examples"
    echo "  benchmarks         - Run performance benchmarks"
    echo "  python             - Test Python bindings"
    echo "  docs               - Show documentation"
    echo "  generate-docs      - Generate API documentation"
    echo "  clean              - Clean build directory"
    echo "  logs               - Show development logs"
    echo "  performance        - Show performance metrics"
    echo ""
    echo -e "${YELLOW}Options:${NC}"
    echo "  --help             - Show this help message"
    echo "  --version          - Show version information"
    echo "  --log-file <file>  - Specify log file (default: dev.log)"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  $0 build"
    echo "  $0 build && $0 test"
    echo "  $0 build && $0 run"
    echo "  $0 build-python && $0 test && $0 run"
    echo ""
    echo -e "${YELLOW}For more information, see the README.md file.${NC}"
}

# Show version information
function show_version() {
    echo -e "${CYAN}=== NLM Phase 6 Development Environment ===${NC}"
    echo "Version: 2.0.0 (Phase 6.0)"
    echo "Build Date: $(date -I)"
    echo "Git Commit: $(git rev-parse --short HEAD 2>/dev/null || echo 'N/A')"
    echo ""
    echo "This development environment supports NLM Phase 6 integration."
    echo "It includes Python bindings, testing, benchmarks, and documentation generation."
}

# Parse command line arguments
function parse_args() {
    local command=""
    local log_file="${LOG_FILE}"
    
    # Parse positional arguments
    while [ $# -gt 0 ]; do
        case "$1" in
            build)
                command="build"
                shift
                ;;
            build-python)
                command="build-python"
                shift
                ;;
            test)
                command="test"
                shift
                ;;
            run)
                command="run"
                shift
                ;;
            benchmarks)
                command="benchmarks"
                shift
                ;;
            python)
                command="python"
                shift
                ;;
            docs)
                command="docs"
                shift
                ;;
            generate-docs)
                command="generate-docs"
                shift
                ;;
            clean)
                command="clean"
                shift
                ;;
            logs)
                command="logs"
                shift
                ;;
            performance)
                command="performance"
                shift
                ;;
            help)
                show_help
                exit 0
                ;;
            --help)
                show_help
                exit 0
                ;;
            --version)
                show_version
                exit 0
                ;;
            --log-file)
                if [ $# -gt 1 ]; then
                    log_file="$2"
                    LOG_FILE="${log_file}"
                    shift 2
                else
                    echo -e "${RED}Error: --log-file requires a filename${NC}"
                    exit 1
                fi
                ;;
            *)
                echo -e "${RED}Unknown argument: $1${NC}"
                show_help
                exit 1
                ;;
        esac
    done
    
    # If no command specified, show help
    if [ -z "$command" ]; then
        show_help
        exit 0
    fi
    
    # Execute the command
    case "$command" in
        build)
            start_perf
            check_dependencies && initialize_project && build_nlm && build_python_bindings && end_perf
            ;;
        build-python)
            start_perf
            build_python_bindings && end_perf
            ;;
        test)
            start_perf
            run_tests && end_perf
            ;;
        run)
            start_perf
            run_examples && end_perf
            ;;
        benchmarks)
            start_perf
            run_benchmarks && end_perf
            ;;
        python)
            start_perf
            test_python_bindings && end_perf
            ;;
        docs)
            show_docs
            ;;
        generate-docs)
            generate_docs
            ;;
        clean)
            clean_build
            ;;
        logs)
            show_performance_metrics
            ;;
        performance)
            echo -e "${CYAN}=== Performance Metrics ===${NC}"
            echo "Last build time: $(grep "Build completed successfully" "${LOG_FILE}" 2>/dev/null | tail -1 | cut -d' ' -f1,2 || echo "No build logs found")"
            echo "Last test run: $(grep "All tests passed" "${LOG_FILE}" 2>/dev/null | tail -1 | cut -d' ' -f1,2 || echo "No test logs found")"
            ;;
        *)
            echo -e "${RED}Unknown command: $command${NC}"
            exit 1
            ;;
    esac
}

# Main execution
parse_args "$@"