#!/usr/bin/env bash

# NLM Performance Profiling Setup Script
# Comprehensive setup for NLM performance profiling and benchmarking

set -e

# Colors for output
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

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -f "pyproject.toml" ]; then
    print_error "This script must be run from the NLM project root directory."
    exit 1
fi

# Check for required tools
print_info "Checking for required tools..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake 3.16 or higher."
    exit 1
fi

# Check for C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    print_error "No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

# Check for Python
if ! command -v python3 &> /dev/null; then
    print_error "Python 3 is not installed. Please install Python 3.8 or higher."
    exit 1
fi

# Check for pip
if ! command -v pip3 &> /dev/null; then
    print_error "pip3 is not installed. Please install pip."
    exit 1
fi

print_success "All required tools are installed."

# Create build directory
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    print_info "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure CMake with performance support
print_info "Configuring CMake with performance profiling support..."
cmake -B "$BUILD_DIR" -DNLM_PERFORMANCE_ENABLED=ON -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_STANDARD=20 \
    -DPYTHON_EXECUTABLE=$(which python3) \
    "$BUILD_DIR"

# Build the project
print_info "Building NLM with performance profiling..."
cmake --build "$BUILD_DIR" --target all --config Debug -j$(nproc)

print_success "Build completed successfully."

# Check if Python bindings were built
if [ -f "$BUILD_DIR/python_bindings/pynlm.cpython*.so" ] || [ -f "$BUILD_DIR/python_bindings/pynlm.so" ]; then
    print_success "Python bindings built successfully."
else
    print_warning "Python bindings not found in expected location. Checking alternative locations..."
    find "$BUILD_DIR" -name "*.so" -o -name "*.pyd" 2>/dev/null | head -5
fi

# Install Python dependencies
print_info "Installing Python dependencies..."
pip3 install --upgrade pip
pip3 install pybind11 numpy pandas matplotlib nlohmann_json

# Install the profiling module
print_info "Installing NLM profiling module..."
pip3 install -e .

print_success "Python dependencies installed."

# Create example configuration files
print_info "Creating example configuration files..."

# Create benchmark_config.json
if [ ! -f "benchmark_config.json" ]; then
    cat > benchmark_config.json << 'EOF'
{
    "benchmarks": [
        {
            "name": "brain_performance",
            "description": "Basic brain performance test with varying neuron counts",
            "neuron_counts": [100, 500, 1000, 2000],
            "step_counts": [1000, 1000, 500, 200],
            "parameters": {
                "regions": "1",
                "connection_probability": "0.1"
            },
            "tags": ["brain", "performance", "scalability"],
            "enable_profiler": false
        },
        {
            "name": "memory_efficiency",
            "description": "Memory usage patterns for large networks",
            "neuron_counts": [500, 1000, 2000],
            "step_counts": [100, 50, 20],
            "parameters": {
                "regions": "2",
                "connection_probability": "0.05"
            },
            "tags": ["memory", "efficiency", "large_networks"],
            "enable_profiler": true
        }
    ],
    "profiler": {
        "output_format": "html",
        "output_file": "profiler_results.html",
        "max_memory_mb": 1024,
        "cpu_sampling_interval": 100
    },
    "regression_detection": {
        "enabled": true,
        "threshold_percent": 10,
        "alert_on_regression": true
    }
}
EOF
    print_success "Created benchmark_config.json"
fi

# Create profiler_config.ini
if [ ! -f "profiler_config.ini" ]; then
    cat > profiler_config.ini << 'EOF'
[profiler]
output_format = html
output_file = profiler_results.html
max_memory_mb = 1024
cpu_sampling_interval = 100

[benchmarks]
brain_performance = true
memory_efficiency = true
multithreading = true
event_driven = true
memory_leak_detection = true

[regression_detection]
enabled = true
threshold_percent = 10
alert_on_regression = true

[system_monitor]
enabled = true
monitoring_interval = 1000
metrics_output = system_metrics.csv
EOF
    print_success "Created profiler_config.ini"
fi

# Create example scripts
print_info "Creating example scripts..."

# Create run_benchmarks.sh
if [ ! -f "run_benchmarks.sh" ]; then
    cat > run_benchmarks.sh << 'EOF'
#!/bin/bash

# NLM Performance Benchmarking Script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please run setup.sh first."
    exit 1
fi

# Check if benchmark executable exists
if [ ! -f "$BUILD_DIR/benchmark" ]; then
    echo "Benchmark executable not found. Please run setup.sh first."
    exit 1
fi

# Parse command line arguments
CONFIG_FILE="benchmark_config.json"
PROFILER_ENABLED=false
OUTPUT_FORMAT="html"

while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --profiler)
            PROFILER_ENABLED=true
            shift
            ;;
        --format)
            OUTPUT_FORMAT="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --config FILE     Benchmark configuration file (default: benchmark_config.json)"
            echo "  --profiler        Enable performance profiler"
            echo "  --format FORMAT   Output format (csv, json, html) (default: html)"
            echo "  --help            Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Run benchmark
print_info "Running NLM benchmarks with configuration: $CONFIG_FILE"
print_info "Profiler enabled: $PROFILER_ENABLED"

"$BUILD_DIR/benchmark" --config "$CONFIG_FILE" --profiler "$PROFILER_ENABLED" --format "$OUTPUT_FORMAT"

print_success "Benchmarks completed successfully."
EOF
    chmod +x run_benchmarks.sh
    print_success "Created run_benchmarks.sh"
fi

# Create python_profiling_example.py
if [ ! -f "python_profiling_example.py" ]; then
    cat > python_profiling_example.py << 'EOF'
#!/usr/bin/env python3

"""
Example Python script demonstrating NLM performance profiling and benchmarking.

This script shows how to use the NLM performance profiling tools to:
1. Profile benchmark performance
2. Track memory usage
3. Compare results with baselines
4. Generate comprehensive reports
"""

from nlm.profiling_tools import NLMProfiler, BrainPerformanceProfiler, PerformanceComparison
import time
import random
import json
import os
from datetime import datetime

def memory_intensive_benchmark():
    """Benchmark memory allocation patterns."""
    print("Running memory-intensive benchmark...")
    
    # Simulate memory allocation
    data = []
    for i in range(1000):
        # Allocate memory and do some computation
        row = [random.random() for _ in range(100)]
        data.append(row)
    
    # Compute result
    result = sum(sum(row) for row in data)
    
    # Simulate memory cleanup
    del data
    
    return result

def cpu_intensive_benchmark():
    """Benchmark CPU-intensive operations."""
    print("Running CPU-intensive benchmark...")
    
    # Simulate CPU work
    result = 0
    for i in range(10000):
        result += i * 0.001
    
    return result

def neural_simulation_benchmark(neuron_count, steps):
    """Simulate neural network benchmarking."""
    print(f"Running neural simulation benchmark: {neuron_count} neurons, {steps} steps")
    
    # Mock neural simulation
    start_time = time.time()
    
    # Simulate neuron processing
    spikes = 0
    for i in range(steps):
        # Simulate spike generation
        if random.random() < 0.1:  # 10% chance of spike
            spikes += 1
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Calculate metrics
    steps_per_second = steps / duration
    spikes_per_step = spikes / steps
    
    return {
        'neurons': neuron_count,
        'steps': steps,
        'duration': duration,
        'spikes': spikes,
        'steps_per_second': steps_per_second,
        'spikes_per_step': spikes_per_step
    }

def main():
    """Main function demonstrating performance profiling."""
    print("=" * 60)
    print("NLM Performance Profiling Example")
    print("=" * 60)
    
    # Initialize profiler
    profiler = NLMProfiler()
    
    # Define benchmarks
    benchmarks = {
        "memory": memory_intensive_benchmark,
        "cpu": cpu_intensive_benchmark
    }
    
    print("\n1. Adding benchmarks to profiler...")
    for name, func in benchmarks.items():
        profiler.add_benchmark(name, func, category="basic")
        print(f"   Added: {name}")
    
    print("\n2. Running benchmarks...")
    results = profiler.run_benchmarks()
    
    print(f"\n3. Results summary:")
    for result in results:
        print(f"   {result['name']}: {result['duration']:.4f}s")
    
    # Export results
    print("\n4. Exporting results...")
    profiler.export_results("basic_benchmarks.csv", format="csv")
    profiler.generate_report("basic_benchmark_report.html")
    
    print("\n5. Running neural simulation benchmarks...")
    
    # Neural simulation benchmarks
    neural_benchmarks = []
    for neurons in [100, 500, 1000]:
        for steps in [100, 500, 1000]:
            neural_benchmarks.append(
                (f"neural_{neurons}_{steps}", lambda n=neurons, s=steps: neural_simulation_benchmark(n, s))
            )
    
    # Add neural benchmarks
    for name, func in neural_benchmarks:
        profiler.add_benchmark(name, func, category="neural")
    
    # Run neural benchmarks
    neural_results = profiler.run_benchmarks()
    
    print(f"\n6. Neural benchmarks completed: {len(neural_results)} results")
    
    # Generate comprehensive report
    print("\n7. Generating comprehensive report...")
    profiler.generate_report("comprehensive_report.html")
    
    print("\n8. Performance comparison with baseline...")
    
    # Create baseline (using current results as baseline)
    baseline_results = neural_results[:2]  # Use first two as baseline
    
    # Create current results
    current_results = neural_results[2:]
    
    # Compare
    comparator = PerformanceComparison()
    comparator.add_baseline("baseline", baseline_results)
    comparator.add_current("current", current_results)
    
    comparisons = comparator.compare_all()
    
    # Export comparisons
    comparator.export_comparisons("neural_comparisons.csv")
    comparator.generate_regression_report("neural_regression_report.html")
    
    print("\n" + "=" * 60)
    print("Example completed successfully!")
    print("=" * 60)
    print(f"\nFiles generated:")
    print(f"  - basic_benchmarks.csv (CSV results)")
    print(f"  - basic_benchmark_report.html (HTML report)")
    print(f"  - neural_comparisons.csv (comparison results)")
    print(f"  - neural_regression_report.html (regression report)")
    print(f"\nTip: View the HTML reports in a web browser for detailed analysis.")

if __name__ == "__main__":
    main()
EOF
    chmod +x python_profiling_example.py
    print_success "Created python_profiling_example.py"
fi

# Create quick_start.md
if [ ! -f "QUICK_START.md" ]; then
    cat > QUICK_START.md << 'EOF'
# Quick Start: NLM Performance Profiling

## Overview
This guide provides a quick introduction to using the NLM Performance Profiling and Benchmarking tools.

## Prerequisites

### Build Environment
- CMake 3.16 or higher
- C++ compiler (g++ or clang++)
- Python 3.8 or higher
- pip (Python package installer)

### Python Dependencies
```bash
pip install pybind11 numpy pandas matplotlib
```

## Installation Steps

### 1. Build NLM with Profiling Support
```bash
# Clone the NLM repository (if not already done)
git clone <repository-url>
cd nlm

# Run the setup script
./setup.sh
```

### 2. Install Python Module
```bash
cd nlm
pip install -e .
```

## Basic Usage

### Run Built-in Benchmarks
```bash
# Run all benchmarks
./run_benchmarks.sh

# Run specific benchmarks
./run_benchmarks.sh --profiler --format html
```

### Use Python API
```python
from nlm.profiling_tools import NLMProfiler, get_profiler

# Create profiler
profiler = NLMProfiler()

# Add a simple benchmark
def test_function():
    total = 0
    for i in range(10000):
        total += i
    return total

profiler.add_benchmark("test", test_function, "test_category")

# Run benchmarks
results = profiler.run_benchmarks()

# Export results
profiler.export_results("results.csv")
profiler.generate_report("report.html")
```

### Performance Comparison
```python
from nlm.profiling_tools import PerformanceComparison

# Create comparator
comparator = PerformanceComparison()

# Add baseline and current results
comparator.add_baseline("baseline", baseline_results)
comparator.add_current("current", current_results)

# Compare
comparisons = comparator.compare_all()

# Generate reports
comparator.export_comparisons("comparison.csv")
comparator.generate_regression_report("regression.html")
```

## Configuration

### Benchmark Configuration
Edit `benchmark_config.json` to customize benchmarks:

```json
{
    "benchmarks": [
        {
            "name": "brain_performance",
            "neuron_counts": [100, 500, 1000],
            "step_counts": [100, 500, 1000],
            "parameters": {"regions": "1"}
        }
    ]
}
```

### Profiler Configuration
Edit `profiler_config.ini` to configure profiling options.

## Example Scripts

### Python Example
Run the example script:
```bash
python python_profiling_example.py
```

This script demonstrates:
1. Adding various benchmarks
2. Running and collecting results
3. Exporting data in multiple formats
4. Comparing with baselines
5. Generating comprehensive reports

### Build Example
The setup script provides:
- Automated build with profiling support
- Configuration file generation
- Python environment setup
- Example scripts creation

## Advanced Usage

### Custom Benchmarks
Create custom benchmarks by implementing functions and adding them to the profiler.

### Integration with Brain Systems
Use the `BrainPerformanceProfiler` to profile brain-specific operations.

### Regression Detection
Automatically detect performance regressions by comparing with baselines.

## Output Formats

### CSV
- Tabular data for spreadsheet analysis
- Easy to parse programmatically
- Supports statistical analysis

### JSON
- Structured data for programmatic access
- Supports nested objects
- Human-readable with indentation

### HTML
- Interactive reports
- Visual graphs and charts
- Professional formatting

## Troubleshooting

### Common Issues

1. **Module not found**
   ```bash
   pip install -e .
   ```

2. **Build errors**
   ```bash
   # Ensure you have all dependencies
   apt-get update && apt-get install -y build-essential cmake python3-dev
   ```

3. **Profiler not working**
   - Check that performance infrastructure is enabled
   - Verify build includes profiling support

### Getting Help

- Read the [PROFILING_GUIDE.md](PROFILING_GUIDE.md) for comprehensive documentation
- Check the examples in the [examples](examples) directory
- Join the NLM community for support

## Performance Tips

### For Large Networks
- Use optimized brain implementations
- Enable multithreading
- Monitor memory usage

### For Detailed Analysis
- Use event-driven profiling
- Enable memory leak detection
- Monitor system resources

### For Regression Detection
- Set appropriate thresholds
- Use statistical significance testing
- Regular baseline updates

## License
This setup script and the profiling tools are part of the NLM project and are licensed under the MIT License.

## Version
1.0.0
EOF
    print_success "Created QUICK_START.md"
fi

# Create a simple test to verify installation
print_info "Creating installation verification script..."

cat > verify_installation.sh << 'EOF'
#!/bin/bash

echo "Verifying NLM Performance Profiling Installation..."

echo "1. Checking Python modules..."
python3 -c "import sys; from nlm.profiling_tools import NLMProfiler; print('Python module: OK')"

if [ $? -ne 0 ]; then
    echo "ERROR: Python module not available"
    exit 1
fi

echo "2. Checking build directory..."
if [ -d "build" ]; then
    if [ -f "build/benchmark" ]; then
        echo "Build directory: OK"
    else
        echo "WARNING: benchmark executable not found in build directory"
    fi
else
    echo "WARNING: build directory not found"
fi

echo "3. Checking configuration files..."
if [ -f "benchmark_config.json" ]; then
    echo "Configuration file: OK"
else
    echo "WARNING: benchmark_config.json not found"
fi

if [ -f "profiler_config.ini" ]; then
    echo "Profiler config: OK"
else
    echo "WARNING: profiler_config.ini not found"
fi

echo "4. Checking example scripts..."
if [ -f "python_profiling_example.py" ] && [ -x "python_profiling_example.py" ]; then
    echo "Example script: OK"
else
    echo "WARNING: python_profiling_example.py not found or not executable"
fi

echo "\nInstallation verification completed."
echo "\nNext steps:"
echo "1. Run './run_benchmarks.sh' to run built-in benchmarks"
echo "2. Run 'python python_profiling_example.py' to see Python examples"
echo "3. Read PROFILING_GUIDE.md for comprehensive documentation"
EOF

chmod +x verify_installation.sh

print_success "Installation verification script created."

# Run verification
print_info "Running installation verification..."
./verify_installation.sh

print_success "Installation completed successfully!"
print_info "\nYou can now start using the NLM performance profiling tools."
print_info "\nKey files created:"
print_info "  - benchmark_config.json    : Benchmark configuration"
print_info "  - profiler_config.ini      : Profiler configuration"
print_info "  - run_benchmarks.sh        : Benchmark runner script"
print_info "  - python_profiling_example.py : Python example script"
print_info "  - verify_installation.sh   : Installation verification script"
print_info "  - PROFILING_GUIDE.md       : Comprehensive documentation"
print_info "  - QUICK_START.md           : Quick start guide"

print_info "\nTo get started, run:"
print_info "  ./run_benchmarks.sh    # Run built-in benchmarks"
print_info "  python python_profiling_example.py  # See Python examples"
EOF
]