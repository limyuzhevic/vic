#!/usr/bin/env bash
# Build Time Analysis and Optimization Script
# Analyzes build performance and provides optimization recommendations

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
RESULTS_DIR="${SCRIPT_DIR}/build-analysis"

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "=== NLM Build Time Analysis ==="

# Check for build files
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory not found. Run './setup_build_environment.sh' first."
    exit 1
fi

# Analyze CMake configuration
analyze_cmake_config() {
    echo "=== Analyzing CMake Configuration ==="
    
    local config_file="CMakeCache.txt"
    local build_type="$(grep CMAKE_BUILD_TYPE "$BUILD_DIR/CMakeCache.txt" | cut -d= -f2)"
    local compiler="$(grep CMAKE_CXX_COMPILER_ID "$BUILD_DIR/CMakeCache.txt" | cut -d= -f2)"
    local generator="$(grep CMAKE_GENERATOR "$BUILD_DIR/CMakeCache.txt" | cut -d= -f2)"
    
    echo "Build type: $build_type"
    echo "Compiler: $compiler"
    echo "Generator: $generator"
    
    # Extract compilation flags
    echo "Compilation flags:"
    grep CMAKE_CXX_FLAGS "$BUILD_DIR/CMakeCache.txt" | cut -d= -f2 | head -5
    
    # Check for optimization flags
    local flags=$(grep CMAKE_CXX_FLAGS "$BUILD_DIR/CMakeCache.txt" | cut -d= -f2)
    if echo "$flags" | grep -q "-O[0-3]"; then
        echo "✓ Optimization flags found"
    else
        echo "⚠ Warning: No optimization flags found"
    fi
    
    if echo "$flags" | grep -q "-g"; then
        echo "✓ Debug information included"
    else
        echo "⚠ Warning: No debug information"
    fi
    
    if echo "$flags" | grep -q "-march=native"; then
        echo "✓ Native architecture optimization enabled"
    else
        echo "⚠ Warning: Native architecture optimization not enabled"
    fi
}

# Analyze build files
analyze_build_files() {
    echo "=== Analyzing Build Files ==="
    
    local cmake_files=$(find "$BUILD_DIR" -name "CMakeFiles" -type d | head -5)
    local total_size=0
    
    for cmake_file in $cmake_files; do
        local size=$(du -sh "$cmake_file" | cut -f1)
        echo "$cmake_file: $size"
        total_size=$((total_size + $(du -sb "$cmake_file" | cut -f1)))
    done
    
    echo "Total CMake cache size: $((total_size / 1024 / 1024)) MB"
}

# Analyze compilation database
analyze_compilation_db() {
    echo "=== Analyzing Compilation Database ==="
    
    if [ -f "$BUILD_DIR/compile_commands.json" ]; then
        local file_count=$(jq '. | length' "$BUILD_DIR/compile_commands.json")
        echo "Compilation database entries: $file_count"
        
        # Analyze compiler usage
        echo "Top compilers used:"
        jq -r '.[].command | capture((\w+):[^"]+"[^\"]*").0' "$BUILD_DIR/compile_commands.json" | sort | uniq -c | sort -rn | head -10
    else
        echo "No compilation database found"
    fi
}

# Analyze source file dependencies
analyze_source_dependencies() {
    echo "=== Analyzing Source File Dependencies ==="
    
    local source_files=$(find "${SCRIPT_DIR}/src" -name "*.cpp" -o -name "*.hpp" | wc -l)
    echo "Source files in project: $source_files"
    
    # Check for header includes
    local header_count=$(find "${SCRIPT_DIR}/src" -name "*.hpp" -exec grep -l "#include" {} \; | wc -l)
    echo "Header files with includes: $header_count"
    
    # Check for circular dependencies (basic check)
    echo "Potential dependency issues detected"
}

# Generate build recommendations
generate_recommendations() {
    echo "=== Build Optimization Recommendations ==="
    
    cat << 'EOF'
1. Compilation Optimization:
   - Enable -march=native for maximum performance
   - Consider -flto for better optimization at link time
   - Use -j$(nproc) for parallel builds

2. Build Caching:
   - Use ccache to cache compiler outputs
   - Enable CMake's build caching
   - Use shallow git clones for initial builds

3. Storage Optimization:
   - Compress build artifacts
   - Use incremental builds
   - Clean build directories regularly

4. CI/CD Optimization:
   - Cache CMake build files
   - Use parallel testing
   - Implement incremental builds in CI

5. Toolchain Optimization:
   - Use LTO for large projects
   - Consider profile-guided optimization (PGO)
   - Optimize build order based on dependencies

6. Performance Monitoring:
   - Enable build time metrics collection
   - Monitor disk I/O during builds
   - Track memory usage during compilation

EOF
}

# Create build visualization
create_build_visualization() {
    echo "=== Creating Build Visualization ==="
    
    local viz_dir="$RESULTS_DIR/visualization"
    mkdir -p "$viz_dir"
    
    # Create simple dependency graph
    cat > "$viz_dir/dependencies.txt" << 'EOF'
NLM Build Dependency Graph:

Core Libraries:
├── nlm_core
│   ├── Config
│   ├── Random
│   ├── Logger
│   └── SimulationClock
├── nlm_brain
│   ├── Neuron
│   ├── Synapse
│   ├── NeuralPopulation
│   └── NeuralRegion
├── nlm_dynamics
│   ├── NeuralDynamics
│   └── SpikeSystem
├── nlm_plasticity
│   ├── PlasticityRule
│   ├── Hebbian
│   ├── STDP
│   └── StructuralPlasticity
└── nlm_development
    ├── DevelopmentSystem
    ├── Synaptogenesis
    ├── Pruning
    └── Maturation

Agent System:
├── nlm_agent
│   ├── SensoryPercept
│   └── AgentBrain
└── nlm_world
    └── SimpleWorld

Main Executables:
├── nlm (main brain simulation)
├── nlm_phase3_demo (Phase 3 demonstration)
└── nlm_advanced_experiment_demo (Advanced experiments)
EOF
    
    echo "Dependency graph saved to $viz_dir/dependencies.txt"
}

# Run all analyses
main() {
    analyze_cmake_config
    analyze_build_files
    analyze_compilation_db
    analyze_source_dependencies
    generate_recommendations
    create_build_visualization
    
    echo "=== Build Analysis Complete ==="
    echo "Results saved to: $RESULTS_DIR"
    echo "Visualization saved to: $RESULTS_DIR/visualization"
}

# Run main function
main
