# NLM (Neural Learning Machine) - Phase 2 Integration Test Execution

## Quick Start

This document provides instructions for running the Phase 2 integration test that verifies all critical fixes have been implemented.

## Prerequisites

### Build Dependencies
```bash
# Install build tools
 sudo apt-get update
sudo apt-get install -y build-essential cmake g++ git

# Install Python and pip for Python bindings
 sudo apt-get install -y python3 python3-pip

# Install Python dependencies
 pip3 install pybind11 scikit-build-core pytest numpy
```

### Build Tools
```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Debug build for development, Release for production)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build all targets with parallel jobs
make -j$(nproc)
```

## Running Phase 2 Integration Test

### Method 1: Execute Main Demo (Recommended)
```bash
# Run the Phase 2 integration test demo
./nlm

# Expected output includes:
# - Initial NLM banner
# - Configuration loading and validation
# - Brain initialization
# - Test 1: Basic connectivity test
# - Test 2: Plasticity learning experiment  
# - Test 3: STDP verification
# - Final brain status report
# - Phase 2 completion confirmation
```

### Method 2: Run with Custom Configuration
```bash
# Create a custom configuration file
# or use default with command line arguments
./nlm --config configs/test.cfg

# Available command line options:
# --config <file>     : Load configuration from specified file
# --help              : Display help information
# --version           : Display version information
```

### Method 3: Run with Validation (Development Mode)
```bash
# For development, you can also run the demo directly
cd build
./nlm_phase2_demo
```

## Phase 6 Complete Brain Loop Integration

The NLM system implements a complete 14-step integrated brain loop:

### Step-by-Step Integration:
1. ✅ **Process pending delayed spike events** - Deliver synaptic input with proper timing
2. ✅ **Update neuron membrane potentials** - LIF dynamics with numerical stability
3. ✅ **Detect spikes and schedule events** - Thread-safe spike processing
4. ✅ **Update working memory** - Transient memory maintenance and competition
5. ✅ **Apply neuromodulation effects** - Dopamine, curiosity, novelty integration
6. ✅ **Apply plasticity rules** - STDP and Hebbian learning with bounds checking
7. ✅ **Update episodic memory** - Experience storage and consolidation
8. ✅ **Update prediction system** - Sensory prediction and error computation
9. ✅ **Update attention system** - Competitive selection dynamics
10. ✅ **Update concept formation** - Pattern discovery from experience
11. ✅ **Apply structural plasticity** - Synaptogenesis and pruning
12. ✅ **Replay important memories** - Consolidation during rest
13. ✅ **Apply development effects** - Plasticity rate modulation
14. ✅ **Collect statistics** - Performance monitoring and reporting

## Verification of Critical Fixes

### 1. Thread Synchronization
```bash
# The system should now handle concurrent access safely
# No race conditions during spike processing or plasticity updates
# Working memory access is properly synchronized
```

### 2. Memory Management
```bash
# No segmentation faults or undefined behavior
# All neuron state variables properly initialized
# Memory bounds checking prevents buffer overflows
```

### 3. Error Handling
```bash
# Clear error messages for file operations
# Configuration loading issues are properly reported
# Save/load operations include comprehensive error checking
```

### 4. Numerical Stability
```bash
# Voltage updates are bounded and stable
# Synaptic weights remain within valid ranges [-1.0, 1.0]
# No numerical explosions or instability
```

## Test Results

### Basic Connectivity Test
- **Expected**: >50 spikes generated
- **Verification**: Spikes propagate through network successfully
- **Status**: ✅ WORKING

### Plasticity Learning Experiment  
- **Expected**: Synaptic weight changes through experience
- **Verification**: Learning detected with measurable synaptic modifications
- **Status**: ✅ WORKING

### STDP Verification
- **Expected**: Pre-before-post produces potentiation (LTP)
- **Verification**: Weight increases detected, within valid bounds
- **Status**: ✅ WORKING

## Development and Testing

### For Development:
```bash
# Run with debugging flags
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with GDB for debugging
gdb ./nlm
# (use run command to start)

# Or run with valgrind for memory checking
valgrind --leak-check=full ./nlm
```

### For Production:
```bash
# Use Release build for best performance
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run the production demo
./nlm
```

## Monitoring and Diagnostics

### Runtime Statistics:
- Total spikes processed and per-neuron counts
- Working memory trace counts and active neurons
- Neuromodulation levels (dopamine, curiosity, novelty)
- Developmental stage and plasticity rates

### Performance Metrics:
- Simulation speed (steps per second)
- Memory usage and spike history management
- Neural population activity distribution

## Troubleshooting

### Common Issues:

#### "Brain initialization failed"
```bash
# Check configuration files
# Ensure all required parameters are set
# Verify build includes all dependencies
```

#### "File not found" errors
```bash
# Check working directory
cd /path/to/nlm/build
# Run from build directory or use absolute paths
```

#### "Segmentation fault"
```bash
# Run with valgrind for memory issues
valgrind --leak-check=full ./nlm
# Use gdb for debugging information
```

#### "No spikes generated"
```bash
# Check neuron parameters in configuration
# Verify sufficient input current injection
# Check threshold and reset potential values
```

## Next Steps for Advanced Users

### 1. Explore Phase 3 and 4 Demos:
```bash
# Run Phase 3 agent-environment interaction
./nlm_phase3_demo

# Run Phase 4 complete agent simulation
./nlm_phase4_demo
```

### 2. Extend the Brain:
```bash
# Add new regions with different neuron types
# Implement custom plasticity rules
# Add new neuromodulators
# Extend memory systems
```

### 3. Performance Optimization:
```bash
# Profile with performance analysis tools
# Optimize for specific hardware (SIMD, GPU)
# Implement network topologies
# Add parallel processing for large networks
```

## Summary

This Phase 2 integration test verifies that the NLM system implements:

1. ✅ **Real neural computation** with LIF dynamics
2. ✅ **Event-driven spike propagation** with delays
3. ✅ **Multiple plasticity mechanisms** (STDP, Hebbian, structural)
4. ✅ **Integrated memory systems** (working, episodic, associative)
5. ✅ **Neuromodulation** (dopamine, curiosity, novelty)
6. ✅ **Complete brain loop** integration
7. ✅ **Thread safety** and error handling
8. ✅ **Numerical stability** and bounds checking

The NLM Phase 2 integration is now ready for advanced development and research applications.
