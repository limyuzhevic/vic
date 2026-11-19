#!/bin/bash

# NLM (Neural Learning Machine) Build Script
# A comprehensive build and test script for NLM with all improvements

# Configuration
BUILD_DIR="${1:-build}"
INSTALL_DIR="${2:-/usr/local}"
GENERATE_DOCS="${3:-false}"
RUN_TESTS="${4:-true}"
GENERATE_CONFIGS="${5:-true}"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check if a directory exists
check_directory() {
    local dir="$1"
    local create="$2"
    
    if [ ! -d "$dir" ]; then
        if [ "$create" = "true" ]; then
            print_info "Creating directory: $dir"
            mkdir -p "$dir"
        else
            print_error "Directory not found: $dir"
            return 1
        fi
    fi
}

# Function to check if a file exists
check_file() {
    local file="$1"
    local desc="$2"
    
    if [ ! -f "$file" ]; then
        print_error "Required file not found: $file ($desc)"
        return 1
    fi
}

# Function to check CMake version
check_cmake_version() {
    local min_version="3.16"
    local cmake_version
    
    if ! command_exists cmake; then
        print_error "CMake not found. Please install CMake."
        return 1
    fi
    
    cmake_version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
    if [ -z "$cmake_version" ]; then
        print_warning "Could not determine CMake version"
        return 0
    fi
    
    # Compare versions
    local major_version="${cmake_version%.*}"
    local major_min_version="${min_version%.*}"
    
    if (( $(echo "$major_version >= $major_min_version" | bc -l) )); then
        print_success "CMake version $cmake_version found"
        return 0
    else
        print_error "CMake version $cmake_version is required (minimum $min_version)"
        return 1
    fi
}

# Function to check compiler
check_compiler() {
    if ! command_exists g++; then
        print_warning "g++ not found, checking for clang++"
        if ! command_exists clang++; then
            print_error "No C++ compiler found. Please install g++ or clang++."
            return 1
        else
            CXX_COMPILER="clang++"
        fi
    else
        CXX_COMPILER="g++"
    fi
    
    print_success "Using C++ compiler: $CXX_COMPILER"
}

# Function to check Python
check_python() {
    if ! command_exists python3; then
        print_warning "python3 not found, checking for python"
        if ! command_exists python; then
            print_error "Python not found. Python is required for Python bindings."
            return 1
        else
            PYTHON="python"
        fi
    else
        PYTHON="python3"
    fi
    
    # Check Python version
    local python_version
    python_version=$($PYTHON -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}')")
    
    # Require Python 3.8+
    local major_version="${python_version%.*}"
    if (( $(echo "$major_version >= 3.8" | bc -l) )); then
        print_success "Python $python_version found"
        return 0
    else
        print_error "Python $python_version is required (minimum 3.8)"
        return 1
    fi
}

# Function to build NLM
build_nlm() {
    local build_dir="$BUILD_DIR"
    
    print_info "Building NLM in $build_dir"
    
    # Create build directory
    check_directory "$build_dir true"
    
    # Configure with CMake
    print_info "Configuring NLM with CMake..."
    
    cd "$build_dir" || return
    
    cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
        -DPYTHON_EXECUTABLE="$PYTHON" \
        -DNLM_BUILD_PYTHON_BINDINGS=ON \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
    
    if [ $? -eq 0 ]; then
        print_success "CMake configuration successful"
    else
        print_error "CMake configuration failed"
        return 1
    fi
    
    # Build NLM
    print_info "Building NLM..."
    
    make -j$(nproc) || {
        print_error "Build failed"
        return 1
    }
    
    if [ $? -eq 0 ]; then
        print_success "NLM build successful"
    else
        print_error "NLM build failed"
        return 1
    fi
    
    # Build Python bindings
    print_info "Building Python bindings..."
    
    if [ -f "pynlm" ]; then
        print_success "Python bindings already built"
    else
        print_warning "Python bindings not built in build directory"
        cd .. || return
        python3 -m pip install -e . || {
            print_error "Python bindings installation failed"
            return 1
        }
    fi
    
    cd ..
}

# Function to run tests
run_tests() {
    local build_dir="$BUILD_DIR"
    
    if [ "$RUN_TESTS" = "false" ]; then
        print_info "Skipping tests as requested"
        return 0
    fi
    
    print_info "Running tests..."
    
    cd "$build_dir" || return
    
    # Run C++ tests
    if [ -f "CTestTestproject.cmake" ]; then
        print_info "Running C++ tests..."
        ctest --output-on-failure || {
            print_error "C++ tests failed"
            return 1
        }
    else
        print_warning "C++ test target not found"
    fi
    
    # Run Python tests
    print_info "Running Python tests..."
    
    if command_exists pytest; then
        cd ..
        python3 -m pytest tests/ -v || {
            print_error "Python tests failed"
            return 1
        }
    else
        print_warning "pytest not available"
    fi
    
    cd ..
}

# Function to generate documentation
build_docs() {
    if [ "$GENERATE_DOCS" = "true" ]; then
        print_info "Generating documentation..."
        
        # Build doxygen documentation
        if command_exists doxygen; then
            print_info "Building doxygen documentation..."
            
            cd docs
            doxygen Doxyfile
            
            if [ $? -eq 0 ]; then
                print_success "Documentation generated successfully"
                cd .. || return
            else
                print_error "Documentation generation failed"
                cd .. || return
                return 1
            fi
        else
            print_warning "doxygen not found, skipping documentation generation"
        fi
        
        # Generate API documentation
        print_info "Generating API documentation..."
        
        if command_exists python3; then
            cd .. || return
            python3 -m pydoc -o docs/api src 2>/dev/null || {
                print_warning "API documentation generation partially failed"
            }
        fi
        
        cd ..
    else
        print_info "Skipping documentation generation as requested"
    fi
}

# Function to generate configurations
generate_configs() {
    if [ "$GENERATE_CONFIGS" = "false" ]; then
        print_info "Skipping configuration generation as requested"
        return 0
    fi
    
    print_info "Generating configuration files..."
    
    # Create configs directory
    mkdir -p configs
    
    # Generate default configuration
    cat > configs/default.json << 'EOF'
{
    "name": "default_configuration",
    "version": "1.0",
    "description": "Default NLM configuration",
    
    "neural": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": false,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    },
    
    "environment": {
        "world_width": 50,
        "world_height": 50,
        "vision_range": 16,
        "action_interval": 0.1
    }
}
EOF

    # Generate expert configuration
    cat > configs/expert.json << 'EOF'
{
    "name": "expert_configuration",
    "version": "2.0",
    "expert_mode": true,
    "description": "Expert NLM configuration",
    
    "neural": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.05,
        "enable_advanced_dynamics": true,
        "enable_sparse_connectivity": true,
        "enable_event_driven": true
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "enable_homeostatic": true,
        "stdp_ltp_weight": 0.02,
       _tdp_ltd_weight": 0.015
    },
    
    "memory": {
        "working_memory_capacity": 200,
        "episodic_memory_limit": 100,
        "enable_replay": true,
        "replay_window": 2000
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_selectivity": 0.8
    }
}
EOF

    # Generate minimal configuration
    cat > configs/minimal.json << 'EOF'
{
    "name": "minimal_configuration",
    "version": "1.0",
    "description": "Minimal NLM configuration",
    
    "neural": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    }
}
EOF

    # Generate research configuration
    cat > configs/research.json << 'EOF'
{
    "name": "research_configuration",
    "version": "3.0",
    "expert_mode": true,
    "description": "Research NLM configuration",
    
    "neural": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.08,
        "enable_advanced_dynamics": true,
        "enable_sparse_connectivity": true
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "enable_homeostatic": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    },
    
    "memory": {
        "working_memory_capacity": 100,
        "enable_replay": true,
        "replay_window": 1000
    },
    
    "advanced_features": {
        "enable_multithreading": true,
        "thread_count": 8,
        "enable_simd": true,
        "checkpoint_interval": 1000
    }
}
EOF

    print_success "Configuration files generated"
}

# Function to create demonstration scripts
create_demos() {
    print_info "Creating demonstration scripts..."
    
    # Create demo directory
    mkdir -p demos
    
    # Create quick start demo
    cat > demos/quick_start.sh << 'EOF'
#!/bin/bash

# Quick Start Demo for NLM

# This script provides a quick start guide for NLM users

echo "=== NLM Quick Start Demo ==="
echo ""

# Check if NLM is built
if [ ! -f "build/nlm" ]; then
    echo "NLM executable not found. Please build NLM first."
    echo ""
    echo "To build NLM:"
    echo "  mkdir build"
    echo "  cd build"
    echo "  cmake .."
    echo "  make"
    echo ""
    exit 1
fi

echo "1. Running basic neural computation demo..."
echo ""
build/nlm
echo ""

echo "2. Running world interaction demo..."
echo ""
build/nlm_phase3_demo 2 200
echo ""

echo "3. Running cognitive mechanisms demo..."
echo ""
build/nlm_phase4_demo 50
echo ""

echo "4. Running advanced brain personality demo..."
echo ""
build/nlm_phase5_demo
echo ""

echo "5. Running Phase 6 integration demo..."
echo ""
build/nlm_phase6_demo --config configs/expert.json
echo ""

echo "=== Quick Start Demo Complete ==="
echo ""
echo "For more information:"
echo "  - Documentation: https://nlm.readthedocs.io"
echo "  - GitHub: https://github.com/nlm-project/nlm"
echo "  - Demos: https://nlm-project.github.io/demos"
EOF

    # Make demo executable
    chmod +x demos/quick_start.sh
    
    # Create configuration demo
    cat > demos/configuration_demo.py << 'EOF'
#!/usr/bin/env python3

"""
Configuration Demo for NLM

This script demonstrates NLM configuration management.
"""

import json
import os
import sys
from pathlib import Path

def main():
    print("=== NLM Configuration Demo ===")
    print()
    
    # Create configs directory
    configs_dir = Path("configs")
    configs_dir.mkdir(exist_ok=True)
    
    # Load default configuration
    default_config = {
        "name": "default_configuration",
        "version": "1.0",
        "description": "Default NLM configuration",
        
        "neural": {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001
        },
        
        "plasticity": {
            "enable_stdp": True,
            "enable_hebbian": True,
            "enable_structural": False,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012
        }
    }
    
    # Save default configuration
    with open(configs_dir / "default.json", "w") as f:
        json.dump(default_config, f, indent=2)
    
    print("1. Created default configuration")
    print()
    
    # Load and modify configuration
    with open(configs_dir / "default.json", "r") as f:
        config = json.load(f)
    
    config["name"] = "expert_configuration"
    config["expert_mode"] = True
    config["neural"]["neuron_count"] = 10000
    config["neural"]["region_count"] = 5
    
    # Save modified configuration
    with open(configs_dir / "expert.json", "w") as f:
        json.dump(config, f, indent=2)
    
    print("2. Created expert configuration")
    print()
    
    # Load and validate configuration
    with open(configs_dir / "expert.json", "r") as f:
        expert_config = json.load(f)
    
    # Validate configuration
    errors = []
    
    if "neural" not in expert_config:
        errors.append("Missing 'neural' section")
    else:
        if "neuron_count" not in expert_config["neural"]:
            errors.append("Missing 'neuron_count' in neural section")
        elif expert_config["neural"]["neuron_count"] <= 0:
            errors.append("'neuron_count' must be greater than 0")
    
    if errors:
        print("3. Configuration validation failed:")
        for error in errors:
            print(f"   - {error}")
    else:
        print("3. Configuration validation passed")
    
    print()
    print("=== Configuration Demo Complete ===")
    print()
    print("Configuration files created:")
    for config_file in configs_dir.glob("*.json"):
        print(f"  - {config_file.name}")

if __name__ == "__main__":
    main()
EOF

    # Make demo executable
    chmod +x demos/configuration_demo.py
    
    # Create analysis demo
    cat > demos/analysis_demo.py << 'EOF'
#!/usr/bin/env python3

"""
Analysis Demo for NLM

This script demonstrates NLM analysis capabilities.
"""

import json
import os
import sys
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np

def analyze_pattern_discrimination(results_file):
    """Analyze pattern discrimination results."""
    print("Analyzing pattern discrimination results...")
    
    # Load results
    with open(results_file, "r") as f:
        results = json.load(f)
    
    # Extract metrics
    metrics = results.get("metrics", {})
    
    # Print metrics
    print(f"  Pattern separation: {metrics.get('pattern_separation', 'N/A')}")
    print(f"  Pattern discrimination: {metrics.get('pattern_discrimination', 'N/A')}")
    print(f"  Discrimination threshold: {metrics.get('discrimination_threshold', 'N/A')}")
    
    # Create visualization
    plt.figure(figsize=(10, 6))
    
    # Plot discrimination results
    if "patterns" in metrics:
        patterns = metrics["patterns"]
        discrimination_values = [p.get("discrimination", 0) for p in patterns]
        
        plt.bar(range(len(discrimination_values)), discrimination_values)
        plt.xlabel("Pattern Index")
        plt.ylabel("Discrimination Value")
        plt.title("Pattern Discrimination Analysis")
        
        # Save plot
        plt.savefig("pattern_discrimination.png")
        plt.close()
    
    print("  Pattern discrimination analysis completed")
    return True

def analyze_learning_curves(results_file):
    """Analyze learning curves."""
    print("Analyzing learning curves...")
    
    # Load results
    with open(results_file, "r") as f:
        results = json.load(f)
    
    # Extract metrics
    metrics = results.get("metrics", {})
    
    # Print metrics
    print(f"  Learning rate: {metrics.get('learning_rate', 'N/A')}")
    print(f"  Convergence time: {metrics.get('convergence_time', 'N/A')}")
    print(f"  Final performance: {metrics.get('final_performance', 'N/A')}")
    
    # Create visualization
    plt.figure(figsize=(10, 6))
    
    if "learning_data" in metrics:
        learning_data = metrics["learning_data"]
        
        if isinstance(learning_data, dict):
            for metric_name, values in learning_data.items():
                plt.plot(values, label=metric_name)
        elif isinstance(learning_data, list):
            plt.plot(learning_data)
        
        plt.xlabel("Episode")
        plt.ylabel("Metric Value")
        plt.title("Learning Curves")
        plt.legend()
        
        # Save plot
        plt.savefig("learning_curves.png")
        plt.close()
    
    print("  Learning curve analysis completed")
    return True

def main():
    print("=== NLM Analysis Demo ===")
    print()
    
    # Check if results files exist
    results_dir = Path("results")
    results_dir.mkdir(exist_ok=True)
    
    # Create sample results files
    sample_pattern_results = {
        "experiment": "pattern_discrimination",
        "metrics": {
            "pattern_separation": 0.85,
            "pattern_discrimination": 0.78,
            "discrimination_threshold": 0.5,
            "patterns": [
                {"id": 0, "discrimination": 0.9},
                {"id": 1, "discrimination": 0.8},
                {"id": 2, "discrimination": 0.7},
                {"id": 3, "discrimination": 0.6}
            ]
        }
    }
    
    sample_learning_results = {
        "experiment": "learning_curves",
        "metrics": {
            "learning_rate": 0.05,
            "convergence_time": 100,
            "final_performance": 0.9,
            "learning_data": {
                "performance": [0.1, 0.3, 0.5, 0.7, 0.8, 0.85, 0.88, 0.9],
                "reward": [0.0, 0.2, 0.4, 0.6, 0.7, 0.75, 0.8, 0.85]
            }
        }
    }
    
    # Save sample results
    with open(results_dir / "pattern_discrimination.json", "w") as f:
        json.dump(sample_pattern_results, f, indent=2)
    
    with open(results_dir / "learning_curves.json", "w") as f:
        json.dump(sample_learning_results, f, indent=2)
    
    print("1. Created sample results files")
    print()
    
    # Analyze pattern discrimination
    analyze_pattern_discrimination(str(results_dir / "pattern_discrimination.json"))
    print()
    
    # Analyze learning curves
    analyze_learning_curves(str(results_dir / "learning_curves.json"))
    print()
    
    print("=== Analysis Demo Complete ===")
    print()
    print("Analysis files created:")
    print("  - pattern_discrimination.png")
    print("  - learning_curves.png")

if __name__ == "__main__":
    main()
EOF

    # Make demo executable
    chmod +x demos/analysis_demo.py
    
    print_success "Demo scripts created"
}

# Main build and test process
main() {
    print_info "Starting NLM build and test process"
    
    # Check system requirements
    print_info "Checking system requirements..."
    
    check_cmake_version || {
        print_error "CMake check failed"
        exit 1
    }
    
    check_compiler || {
        print_error "Compiler check failed"
        exit 1
    }
    
    check_python || {
        print_error "Python check failed"
        exit 1
    }
    
    # Check for build directory
    if [ -d "$BUILD_DIR" ]; then
        print_warning "Build directory already exists. Overwriting..."
        rm -rf "$BUILD_DIR"
    fi
    
    # Build NLM
    build_nlm || {
        print_error "NLM build failed"
        exit 1
    }
    
    # Run tests
    run_tests || {
        print_error "Tests failed"
        exit 1
    }
    
    # Generate documentation
    build_docs || {
        print_error "Documentation generation failed"
        exit 1
    }
    
    # Generate configurations
    if [ "$GENERATE_CONFIGS" = "true" ]; then
        generate_configs || {
            print_error "Configuration generation failed"
            exit 1
        }
    fi
    
    # Create demos
    create_demos || {
        print_error "Demo creation failed"
        exit 1
    }
    
    print_success "NLM build and test process completed successfully"
    
    # Print summary
    echo ""
    echo "=== Build Summary ==="
    echo "Build directory: $BUILD_DIR"
    echo "Installation directory: $INSTALL_DIR"
    echo "Generate documentation: $GENERATE_DOCS"
    echo "Run tests: $RUN_TESTS"
    echo "Generate configurations: $GENERATE_CONFIGS"
    echo ""
    echo "Next steps:"
    echo "  1. Run NLM demos: ./build/nlm, ./build/nlm_phase3_demo, etc."
    echo "  2. Use configuration management: nlm-config list"
    echo "  3. Analyze results: nlm analyze pattern-discrimination results.dat"
    echo "  4. Run benchmarks: nlm benchmark"
    echo "  5. View documentation in $BUILD_DIR/docs"
    echo ""
}

# Run main function
main
