# Performance Examples

This directory contains example programs that demonstrate the use of the performance profiling system in NLM Phase 6.

## What This Includes

### 1. Performance Profiler Examples
**File**: `performance_examples.cpp`
- Example 1: Basic performance profiling setup and usage
- Example 2: Brain system-specific performance profiling
- Example 3: Phase 6 demo integration with performance profiling
- Example 4: Command-line usage demonstration
- Example 5: Performance analysis and reporting

### Key Features Demonstrated

1. **Basic Profiling**: Simple setup and teardown of performance monitoring
2. **Brain System Profiling**: Detailed profiling of individual brain systems
3. **Demo Integration**: Integration with Phase 6 demo functionality
4. **Command-Line Usage**: Proper command-line argument parsing
5. **Performance Analysis**: Comprehensive performance analysis and reporting

## Building Examples

### Build Requirements
The examples require the following libraries:
- NLM core libraries
- Performance profiling system
- Phase 6 demo components
- Visualization system

### Build Instructions
1. Ensure the main NLM project is built and installed
2. Navigate to the experiments directory
3. Build the examples:
```bash
g++ -std=c++17 -I../.. -I../../src -I../../src/core -I../../src/brain -I../../src/experiments performance_examples.cpp -o performance_examples
```

## Running Examples

### Available Examples
```
./performance_examples basic     - Basic performance profiling
./performance_examples brain      - Brain system profiling
./performance_examples demo       - Phase 6 demo integration
./performance_examples cli        - Command-line usage
./performance_examples analysis  - Performance analysis
./performance_examples all        - Run all examples
./performance_examples help       - Show help
```

### Example Outputs

#### Basic Performance Profiling
```
=== Example 1: Basic Performance Profiling ===
[PerformanceProfiler] Initialized with output directory: ./example_profiles
[PerformanceProfiler] Started monitoring
[PerformanceProfiler] Stopped. Total time: 1.234ms
[PerformanceProfiler] Summary:
  Total Time: 1.234ms
  Total Steps: 1000
  Simulation Speed: 810000 steps/second
  FPS: 810.0
  Memory per Step: 1.234B
  CPU Usage: 0.00%
  Peak Memory: 0B
  Spike Events: 0
  Plasticity Events: 0
  Development Events: 0
  Prediction Events: 0
  Memory Events: 0
Basic profiling completed. Results saved to example_profiles/basic_profiling.txt
```

#### Brain System Profiling
```
=== Example 2: Brain System Profiling ===
[PerformanceProfiler] Initialized with output directory: ./example_profiles
[PerformanceProfiler] Started monitoring
[PerformanceProfiler] Stopped. Total time: 15.678ms
[PerformanceProfiler] Summary:
  Total Time: 15.678ms
  Total Steps: 10
  Simulation Speed: 637784 steps/second
  FPS: 637.78
  Memory per Step: 12.345B
  CPU Usage: 0.00%
  Peak Memory: 0B
  Spike Events: 0
  Plasticity Events: 0
  Development Events: 0
  Prediction Events: 0
  Memory Events: 0
Brain system profiling completed. Results saved to example_profiles/brain_profiling.json
```

#### Phase 6 Demo Integration
```
=== Example 3: Phase 6 Demo Performance Integration ===
[PerformanceProfiler] Initialized with output directory: ./example_profiles
[PerformanceProfiler] Started monitoring
[Phase6DemoProfiling] Metrics exported to: example_profiles/demo_metrics.txt
[Phase6DemoProfiling] Performance analysis exported to: example_profiles/demo_analysis.txt
Phase 6 demo performance integration completed.
Metrics saved to example_profiles/demo_metrics.txt
Analysis saved to example_profiles/demo_analysis.txt
```

#### Command-Line Usage
```
=== Example 4: Command-Line Usage ===
Command-line options for performance profiling:

Visualization options:
  --help                    Display help information
  --width [pixels]          Set window width
  --height [pixels]         Set window height
  --profile-console [0|1]   Enable/disable console output

Performance profiling options:
  --phase6-profile [dir]     Enable profiling with output directory
  --phase6-profile-format [fmt] Specify output format (text, json, csv)
  --phase6-profile-console [0|1] Enable/disable console output
  --phase6-profile-interval [ms] Sampling interval in milliseconds
  --phase6-profile-memory    Enable memory profiling
  --phase6-profile-cpu       Enable CPU profiling
  --phase6-profile-system    Enable system-wide profiling

Example command-line usage:
./Phase6Demo --phase6-profile ./profiles --phase6-profile-format json --phase6-profile-console 1
```

#### Performance Analysis
```
=== Example 5: Performance Analysis ===
=== Performance Summary ===
Total Time: 12.345ms
Steps: 50
Speed: 405000 steps/second
Memory per Step: 1.234B
CPU Usage: 0.00%
Peak Memory: 0B
Spike Events: 4
Plasticity Events: 3
Development Events: 2
Prediction Events: 2
Memory Events: 5
Performance Grade: GOOD

=== Phase 6 Demo Performance Report ===
Generated: System Information
Total Steps: 50
Total Runtime: 0.012 seconds
Average FPS: 4050.0
Average Step Time: 2.470ms
Peak Memory Usage: 1.234B
Performance Grade: GOOD

=== Detailed Metrics ===
Total Steps: 50 steps
Total Runtime: 0.012 seconds
Average FPS: 4050.0 frames/second
Average Step Time: 2.470 microseconds
Peak Memory Usage: 1.234 bytes
Spike Events: 4 events
Plasticity Events: 3 events
Development Events: 2 events
Prediction Events: 2 events
Memory Events: 5 events

=== Performance Bottleneck Analysis ===
No significant bottlenecks detected.
System performance appears optimal within current constraints.

=== Memory Performance Analysis ===
Peak Memory Usage: 1.234 bytes
Memory Efficiency: 0.025 bytes/step
Memory usage is within acceptable limits.

=== CPU Performance Analysis ===
Average FPS: 4050.0 frames/second
Average Step Time: 2.470 microseconds
CPU performance is optimal.

=== Visualization Performance Analysis ===
Average FPS: 4050.0 frames/second
Average Step Time: 2.470 microseconds
Visualization performance is smooth.

=== Performance Trends Analysis ===
FPS Trend: 0.000%
Trend: Stable performance

Performance analysis completed successfully.
```

## Output Files Generated

### Example-Specific Outputs
```
./example_profiles/
├── basic_profiling.txt              - Basic profiling results
├── brain_profiling.json             - Brain system profiling results
├── demo_metrics.txt                - Phase 6 demo metrics
├── demo_analysis.txt               - Phase 6 demo performance analysis
└── system_info.txt                  - System information
```

### Format-Specific Files
- **Text Files**: Human-readable summaries with performance grades
- **JSON Files**: Structured data with timestamps and metrics
- **CSV Files**: Tabular data for spreadsheet analysis (if applicable)

## Integration with Phase6Demo

### Key Features
1. **Automatic Profiling**: Performance profiling is automatically enabled/disabled based on command-line arguments
2. **Real-Time Monitoring**: System metrics are collected during simulation
3. **Step-by-Step Analysis**: Each simulation step is profiled individually
4. **Performance Reporting**: Comprehensive performance reports are generated
5. **Bottleneck Detection**: Automatic identification of performance bottlenecks

### Integration Points
- **Command-Line Parsing**: Phase 6 demo command-line parsing includes performance options
- **Profiling Setup**: Performance profiler is initialized with demo configuration
- **Metrics Collection**: System metrics are collected during simulation
- **Export and Analysis**: Results are exported and analyzed after simulation

## Usage in Phase6Demo

### Basic Integration
```bash
./Phase6Demo --phase6-profile
```

### Advanced Integration
```bash
./Phase6Demo \
  --phase6-profile ./demo_profiles \
  --phase6-profile-format json \
  --phase6-profile-console 1 \
  --phase6-profile-interval 10 \
  --phase6-profile-memory \
  --phase6-profile-cpu \
  --phase6-profile-system
```

### Output Directory Structure
```
demo_profiles/
├── phase6_demo_performance_[timestamp].txt
├── phase6_demo_performance_[timestamp].json
├── phase6_demo_performance_[timestamp].csv
├── phase6_demo_performance_[timestamp]_analysis.txt
├── phase6_demo_performance_[timestamp]_summary.txt
└── system_info.txt
```

## Files Modified

### Core Performance Files
- **PerformanceProfiler.hpp**: Performance profiler interface
- **PerformanceProfiler.cpp**: Performance profiler implementation
- **Phase6DemoPerformance.hpp**: Demo integration interface
- **Phase6DemoPerformance.cpp**: Demo integration implementation

### Enhanced Demo Files
- **Phase6Demo.cpp**: Enhanced demo with performance integration

### Example Files
- **performance_examples.cpp**: Example programs demonstrating usage

## Building and Running Instructions

### 1. Build the Performance Examples
```bash
# Navigate to experiments directory
cd /path/to/nlm/src/experiments

# Build examples
g++ -std=c++17 -I../.. -I../../src -I../../src/core -I../../src/brain -I../../src/experiments performance_examples.cpp -o performance_examples
```

### 2. Build Phase6Demo with Performance Support
```bash
# Navigate to experiments directory
cd /path/to/nlm/src/experiments

# Build Phase6Demo with performance integration
g++ -std=c++17 -I../.. -I../../src -I../../src/core -I../../src/brain -I../../src/experiments Phase6Demo.cpp PerformanceProfiler.cpp Phase6DemoPerformance.cpp -o Phase6Demo
```

### 3. Running Examples
```bash
# Basic profiling example
./performance_examples basic

# Brain system profiling example
./performance_examples brain

# Phase 6 demo integration example
./performance_examples demo

# Command-line usage example
./performance_examples cli

# Performance analysis example
./performance_examples analysis

# All examples
./performance_examples all

# Help
./performance_examples help
```

### 4. Running Enhanced Phase6Demo
```bash
# Basic usage with profiling
./Phase6Demo --phase6-profile

# Advanced usage with custom settings
./Phase6Demo --phase6-profile ./custom_profiles --phase6-profile-format json --phase6-profile-console 1

# Help
./Phase6Demo --help
```

## Dependencies

### Required Libraries
- NLM Core Libraries (`src/core/`)
- Performance Profiling System (`src/experiments/`)
- Phase 6 Components (`src/experiments/`)
- Brain Systems (`src/brain/`, `src/memory/`, `src/neuromodulation/`, etc.)
- Visualization System (`src/visualization/`)

### Optional Dependencies
- External JSON library for JSON output (if needed)
- External CSV library for CSV output (if needed)
- External threading library for parallel processing

## Troubleshooting

### Common Issues

1. **Build Errors**
   - **Problem**: Missing includes or libraries
   - **Solution**: Ensure all required headers and libraries are available
   - **Example**: `#include "core/Config/Config.hpp"` and linking with NLM libraries

2. **Runtime Errors**
   - **Problem**: Performance profiler not properly initialized
   - **Solution**: Check command-line arguments and configuration
   - **Example**: Ensure `--phase6-profile` flag is used for profiling

3. **File Access Errors**
   - **Problem**: Cannot write to output directory
   - **Solution**: Verify output directory permissions and existence
   - **Example**: Create output directory before running

4. **Memory Issues**
   - **Problem**: High memory usage or memory leaks
   - **Solution**: Check for memory leaks in profiling code
   - **Example**: Monitor memory usage with `top` or `htop`

### Error Messages

- `[PerformanceProfiler] Initialized with output directory: [path]` - Profiler initialized successfully
- `[PerformanceProfiler] Started monitoring` - Profiling started
- `[PerformanceProfiler] Stopped. Total time: [time]` - Profiling stopped
- `[Phase6DemoProfiling] Metrics exported to: [file]` - Metrics exported successfully
- `[Phase6DemoProfiling] Performance analysis exported to: [file]` - Analysis exported successfully
- `[Phase6Demo] Performance profiling enabled: [status]` - Profiling status message

## Integration Checklist

### For Developers
1. **Environment Setup**: Ensure development environment is properly configured
2. **Build System**: Update build system to include performance profiling files
3. **Documentation**: Update documentation with performance profiling information
4. **Testing**: Add performance profiling tests to existing test suite
5. **Validation**: Test performance profiling with different scenarios

### For Users
1. **Installation**: Install NLM and performance profiling dependencies
2. **Configuration**: Configure performance profiling options as needed
3. **Testing**: Test performance profiling with example programs
4. **Analysis**: Review performance analysis reports for insights
5. **Optimization**: Use performance profiling data for optimization

## Future Enhancements

### Possible Improvements
1. **Real-Time Visualization**: Live performance graphs during simulation
2. **Automated Optimization**: Suggestions based on profiling results
3. **Performance Baselines**: Comparison with previous runs
4. **Multi-run Analysis**: Analysis across multiple simulation runs
5. **Hardware-specific Optimization**: Tailored optimizations for different hardware

### Extension Opportunities
- Integration with external monitoring tools
- Performance alerts and notifications
- Automated performance regression testing
- Performance profiling for different scenarios

## Conclusion

The performance examples provide comprehensive demonstrations of the NLM Phase 6 performance profiling system. They cover all major use cases and integration scenarios, making it easy for developers and users to understand and utilize the profiling capabilities.

The examples serve as both documentation and testing tools, helping ensure that the performance profiling system works correctly and provides valuable insights into system performance.

For more information, refer to the main performance profiling documentation (`PERFORMANCE_PROFILING.md`) and the implementation summary (`IMPLEMENTATION_SUMMARY.md`).
