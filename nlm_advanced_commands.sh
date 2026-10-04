#!/bin/bash
#
# NLM Advanced Command Line Interface
# Comprehensive interface for power users and advanced operations
#

usage() {
    cat << EOF
NLM — Advanced Neural Learning Machine
Usage: nlm [OPTIONS] [ARGS...]

BASIC OPERATIONS:
  --help, -h                 Show this help message
  --version                  Show version information
  --demo, -d [trials]        Run Phase 6 integration demo (default: 50 trials)
  --experiment, -e TYPE [params]  Run specific experiment (demo, phase6, phase4, phase3)
  --benchmark                Run performance benchmarks
  --profile                  Profile memory usage and performance

ADVANCED COMMANDS:

  --config FILE              Load configuration from file
  --save-checkpoint PATH     Save current brain state to checkpoint
  --load-checkpoint PATH     Load brain state from checkpoint
  --export-neurons OUTPUT    Export neuron weights to file for analysis
  --import-neurons INPUT     Import neuron weights from file
  --analyze-biology          Analyze biological plausibility metrics
  --debug-spikes [rate]      Enable detailed spike analysis (Hz)
  --visualize NEURON_ID      Generate visualization for specific neuron

  --multi-agent COUNT        Run multi-agent comparison experiment
  --individual-differences   Test individual differences from random seeds
  --continual-learning       Run continual learning benchmark
  --transfer-learning TASKS  Test transfer across learning tasks

ADVANCED EXPERIMENTS:

  --plasticity-tests         Run comprehensive plasticity validation
  --memory-benchmarkSIZES   Test memory scaling with network size
  --development-stages      Simulate developmental trajectories
  --neuromodulation-comparison Compare different neuromodulation strategies
  --replay-efficiency        Test memory replay and consolidation efficiency
  --sleep-simulation        Simulate sleep/rest cycles and memory consolidation

PERFORMANCE OPTIMIZATION:
  --enable-performance       Enable performance optimizations (memory pools, SIMD, multithreading)
  --disable-performance      Disable performance optimizations (for debugging)
  --thread-count N           Set number of threads for parallel processing
  --simd-level LEVEL         Set SIMD optimization level (none, sse, avx2, avx512)
  --memory-pool-SIZE        Set memory pool size (in MB)

OUTPUT AND ANALYSIS:
  --output-format FORMAT     Output format: text, json, csv, binary
  --output-prefix PREFIX     Prefix for output files
  --metrics-file FILE        Save detailed metrics to file
  --spike-traces FILE        Export spike traces for analysis
  --connectivity-matrix     Output synaptic connectivity matrix
  --weight-distribution     Analyze weight distribution statistics

MONITORING AND DEBUGGING:
  --monitoring-interval SECONDS  Set monitoring interval (default: 5)
  --log-level LEVEL         Set logging level: debug, info, warning, error
  --log-file FILE           Write logs to file
  --profiling               Enable detailed profiling
  --validate-integrity      Validate system integrity and consistency

ENVIRONMENT AND RUNTIME:
  --max-steps STEPS         Set maximum simulation steps
  --timestep MILLISECONDS   Set simulation timestep (ms)
  --neuron-count COUNT      Set number of neurons
  --region-count COUNT      Set number of brain regions
  --connection-probABILITY [0.0-1.0] Set connection probability
  --random-seed SEED        Set random seed for reproducible experiments

DEVELOPMENT AND TESTING:
  --compile-info            Show compilation and build information
  --dependency-check         Check for missing dependencies
  --test-coverage          Run code coverage analysis
  --benchmark-suite        Run full benchmark suite

EXAMPLES:

  Basic usage:
    ./nlm --demo

  Advanced configuration:
    ./nlm --experiment phase6 --max-steps 10000 --neuron-count 5000 --output-format json

  Performance testing:
    ./nlm --enable-performance --thread-count 8 --simd-level avx2 --benchmark

  Multi-agent comparison:
    ./nlm --multi-agent 20 --random-seed 12345 --output-format csv

  Memory analysis:
    ./nlm --memory-benchmarkSIZES --neuron-count 1000,5000,10000 --output-format csv

  Developmental simulation:
    ./nlm --development-stages --max-steps 50000 --output-format json

  Neuromodulation research:
    ./nlm --neuromodulation-comparison --log-level debug --metrics-file neuromod_analysis.json

For more detailed information:
  Read the documentation in docs/ directory
  See examples in examples/ directory
  Check source code comments for implementation details

EOF
}