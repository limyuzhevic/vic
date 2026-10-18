# NLM Profiling Tutorial

This tutorial provides step-by-step instructions for using the NLM profiling and debugging tools to analyze and optimize your neural network performance.

## Introduction

Welcome to the NLM profiling tutorial! This guide will help you:

1. Set up the profiling environment
2. Profile different aspects of your neural network
3. Analyze profiling results
4. Optimize your neural network performance

## Prerequisites

Before you begin, ensure you have:

- NLM project built and working
- Basic understanding of C++ and neural networks
- Access to terminal/command line

## Step 1: Basic Profiling Setup

### Installing Profiling Tools

The NLM profiling tools are included with the main codebase. To use them:

1. Build the NLM project:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

2. The profiling tools will be available as part of the `nlm-cli` command.

### Running Your First Profile

Let's start with a simple CPU profile of a neural network:

```bash
# Navigate to your NLM project
nlm-cli profile-cpu --help

# Start CPU profiling on a demo program
nlm-cli profile-cpu --program nlm_phase6_demo --output cpu_profile.txt
```

This command will:
- Start CPU profiling for the `nlm_phase6_demo` program
- Monitor CPU utilization, memory access, and instruction counts
- Analyze branch prediction and cache performance
- Save profiling data to `cpu_profile.txt`

## Step 2: Understanding Profiling Commands

### Available Profiling Commands

The NLM provides comprehensive profiling capabilities:

#### CPU Profiling
- `profile-cpu`: Basic CPU usage profiling
- `profile-cpu-details`: Detailed CPU analysis
- `profile-flame-graph`: Visual flame graph generation
- `profile-call-graph`: Call graph visualization
- `profile-thread`: Thread profiling
- `profile-context-switch`: Context switching analysis
- `profile-cache`: Cache performance analysis
- `profile-branch`: Branch prediction analysis

#### Memory Profiling
- `profile-memory`: Memory usage profiling
- `profile-memory-leak`: Memory leak detection
- `profile-allocations`: Allocation tracking
- `profile-garbage`: Garbage collection analysis
- `profile-heap`: Heap performance analysis
- `profile-stack`: Stack usage profiling
- `profile-object`: Object lifecycle profiling
- `profile-dump`: Memory dump analysis

#### I/O Profiling
- `profile-disk`: Disk I/O profiling
- `profile-network`: Network I/O profiling
- `profile-file`: File I/O profiling
- `profile-database`: Database I/O profiling
- `profile-stream`: Stream I/O analysis

#### Time Profiling
- `profile-elapsed`: Elapsed time profiling
- `profile-cpu-time`: CPU time profiling
- `profile-wall-clock`: Wall-clock time profiling
- `profile-system`: System time profiling
- `profile-user`: User time profiling
- `profile-wait`: Wait time analysis
- `profile-latency`: Latency profiling
- `profile-throughput`: Throughput analysis

### Command Line Options

Most profiling commands share common options:

```bash
Common Options:
  --program, -p     Program to profile
  --output, -o      Output file for profile data
  --time, -t        Profiling duration in seconds
  --help, -h        Show help information
  --verbose, -v     Enable verbose output
```

## Step 3: CPU Profiling Deep Dive

### Running Detailed CPU Analysis

For more detailed CPU profiling:

```bash
# Enable detailed CPU analysis with flame graph
nlm-cli profile-flame-graph --program nlm_phase6_demo \
                           --output flame_graph.html \
                           --detailed

# Profile with specific core count
nlm-cli profile-cpu --program nlm_phase6_demo \
                   --output cpu_profile.txt \
                   --cores 4
```

### Interpreting CPU Profile Results

The CPU profiling results include:

1. **CPU Utilization**: Percentage of CPU time used
2. **Per-core Metrics**: Individual core performance
3. **Branch Prediction**: How well branches were predicted
4. **Cache Performance**: Cache hit/miss ratios
5. **Context Switches**: Frequency of thread switches
6. **Instruction Counts**: Number of instructions executed

### Analyzing Flame Graphs

Flame graphs provide a visual representation of call stacks:

- **Wide bars**: Functions that consume a lot of time
- **Narrow bars**: Functions called frequently but quickly
- **Color coding**: Different colors represent different time periods

To analyze flame graph output:
1. Open `flame_graph.html` in a web browser
2. Use mouse to zoom and pan
3. Click on bars to get detailed information
4. Export data for further analysis

## Step 4: Memory Profiling

### Detecting Memory Issues

Memory profiling is crucial for identifying memory leaks and inefficiencies:

```bash
# Profile memory usage with leak detection
nlm-cli profile-memory --program nlm_phase6_demo \
                      --detect-leaks \
                      --output memory_profile.txt

# Analyze heap usage
nlm-cli profile-heap --program nlm_phase6_demo \
                    --output heap_analysis.txt
```

### Common Memory Issues and Solutions

1. **Memory Leaks**: Objects not being garbage collected
   - Solution: Enable leak detection and analyze patterns
   - Fix: Review object lifecycle and cleanup code

2. **Memory Fragmentation**: Wasted memory due to allocation patterns
   - Solution: Use memory profiler to identify fragmentation
   - Fix: Optimize allocation/deallocation patterns

3. **Excessive Allocations**: Too many temporary objects
   - Solution: Profile allocation counts and sizes
   - Fix: Reuse objects and use pooling where appropriate

## Step 5: I/O and Network Profiling

### Analyzing Disk I/O

Disk I/O can be a bottleneck in neural network training:

```bash
# Profile disk I/O during training
nlm-cli profile-disk --program nlm_phase6_demo \
                    --track-all \
                    --output disk_profile.txt
```

### Network Performance Analysis

For distributed training or model serving:

```bash
# Profile network I/O
nlm-cli profile-network --program nlm_phase6_demo \
                       --protocol tcp \
                       --output network_profile.txt
```

## Step 6: Time Profiling

### Measuring Performance Metrics

Different time metrics provide different insights:

```bash
# Profile elapsed wall-clock time
nlm-cli profile-elapsed --program nlm_phase6_demo \
                        --output time_profile.txt

# Measure CPU time vs. wall-clock time
nlm-cli profile-cpu-time --program nlm_phase6_demo \
                         --output cpu_time_profile.txt

# Analyze latency and throughput
nlm-cli profile-latency --program nlm_phase6_demo \
                        --threshold 0.1 \
                        --output latency_report.txt
```

## Step 7: Advanced Profiling Techniques

### Using Multiple Profilers Simultaneously

For comprehensive analysis, use multiple profilers:

```bash
# Run CPU and memory profiling together
nlm-cli profile-cpu --program nlm_phase6_demo --output cpu_profile.txt
nlm-cli profile-memory --program nlm_phase6_demo --output memory_profile.txt

# Analyze combined results
nlm-cli analyze-performance --cpu-profile cpu_profile.txt \
                           --memory-profile memory_profile.txt \
                           --output combined_analysis.txt
```

### Real-time Profiling

For interactive performance analysis:

```bash
# Start real-time profiling
nlm-cli profile-cpu --program nlm_phase6_demo --realtime

# Monitor performance during execution
nlm-cli profile-memory --program nlm_phase6_demo --monitor
```

## Step 8: Debugging with Profiling Tools

### Breakpoint Analysis

Combine profiling with debugging:

```bash
# Start debugging with profiling
nlm-cli debug-start --program nlm_phase6_demo \
                   --breakpoints "brain.cpp:100,brain.cpp:200"

# Step through code with profiling
nlm-cli debug-step --profile
```

### Performance Debugging

Identify performance bottlenecks:

1. Use profiling to identify slow functions
2. Use debugging to examine those functions
3. Use instrumentation to add custom metrics
4. Iterate to optimize performance

## Step 9: Automation and Scripting

### Scripting Profiling Tasks

Automate profiling workflows:

```bash
#!/bin/bash
# Profile script for NLM

# Run multiple profiling scenarios
for scenario in baseline optimization; do
    echo "Profiling $scenario..."
    nlm-cli profile-cpu --program nlm_phase6_demo \
                       --output "cpu_${scenario}.txt" \
                       --cores 4
    nlm-cli profile-memory --program nlm_phase6_demo \
                          --output "mem_${scenario}.txt" \
                          --detect-leaks
    echo "Completed $scenario profiling"
done

# Generate comparison report
nlm-cli generate-comparison-report \
         --profiles "cpu_baseline.txt cpu_optimization.txt" \
         --output "comparison_report.html"
```

## Step 10: Performance Optimization

### Using Analysis Tools

Turn profiling data into optimization opportunities:

```bash
# Analyze CPU performance
nlm-cli analyze-cpu --profile cpu_profile.txt \
                   --metric utilization \
                   --output analysis.png

# Analyze memory usage
nlm-cli analyze-memory --profile memory_profile.txt \
                      --metric leaks \
                      --output memory_analysis.png

# Generate optimization recommendations
nlm-cli generate-performance-report --profile cpu_profile.txt \
                                   --format pdf \
                                   --output optimization_recommendations.pdf
```

### Applying Optimizations

Based on analysis results:

1. **Reduce Memory Allocations**: Use object pooling
2. **Improve Cache Locality**: Reorder data access patterns
3. **Optimize Branch Prediction**: Use predictable control flow
4. **Balance Core Usage**: Adjust parallelization strategies

## Step 11: System Health Monitoring

### Monitoring System Performance

Continuously monitor system health:

```bash
# Start system health monitoring
nlm-cli health-check --program nlm_phase6_demo --threshold 95

# Validate system configuration
nlm-cli health-validate --config configs/production.json
```

## Step 12: Integration with Development Workflow

### CI/CD Integration

Integrate profiling into your development pipeline:

```yaml
# Example GitHub Actions workflow
deploy:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v2
    - name: Build NLM
      run: mkdir build && cd build && cmake .. && make -j$(nproc)
    - name: Run Performance Tests
      run: |
        nlm-cli profile-cpu --program nlm_phase6_demo --output cpu_test.txt
        nlm-cli profile-memory --program nlm_phase6_demo --output mem_test.txt
        nlm-cli analyze-performance --cpu-profile cpu_test.txt \
                                   --memory-profile mem_test.txt \
                                   --output performance_report.html
    - name: Upload Reports
      uses: actions/upload-artifact@v2
      with:
        name: performance-reports
        path: performance_report.html
```

## Step 13: Advanced Topics

### Custom Profiling

Extend the profiling system:

```bash
# Add custom profiling hooks
nlm-cli instrument-code --program nlm_phase6_demo \
                       --metric custom_metric \
                       --interval 1000
```

### Real-time Visualization

Monitor performance in real-time:

```bash
# Use visualization tools for real-time monitoring
nlm-cli viz-neuron --profile cpu_profile.txt
nlm-cli viz-network --profile mem_profile.txt
```

## Summary

This tutorial has covered:

1. **Basic setup**: Installing and running profiling tools
2. **Command usage**: Understanding and using profiling commands
3. **Deep dive**: CPU profiling with flame graphs and call graphs
4. **Memory analysis**: Detecting leaks and optimizing memory usage
5. **I/O profiling**: Analyzing disk and network performance
6. **Time metrics**: Measuring different aspects of performance
7. **Advanced techniques**: Multiple profilers and real-time analysis
8. **Debugging**: Combining profiling with debugging
9. **Automation**: Scripting and CI/CD integration
10. **Optimization**: Using analysis results for improvements
11. **System monitoring**: Health checks and validation
12. **Integration**: Development workflow integration

## Next Steps

After mastering the basics:

1. **Explore advanced features**: GPU profiling, distributed profiling
2. **Learn from examples**: Study profiling reports from real applications
3. **Contribute**: Add new profiling tools or improve existing ones
4. **Research**: Stay updated with new profiling techniques and tools

## Resources

### Documentation
- Complete profiling tool documentation: `PROFILING_TOOLS.md`
- Profiling reference manual: `REFERENCE.md`
- Profiling best practices: `BEST_PRACTICES.md`

### Examples
- Profile analysis scripts
- Optimization workflows
- Integration examples

### Support
- Project documentation
- Community forums
- Issue tracking

## Conclusion

Profiling is an essential skill for modern software development, especially for performance-critical applications like neural networks. The NLM profiling tools provide comprehensive capabilities to analyze and optimize your neural network performance.

By following this tutorial, you'll be able to:

- Identify performance bottlenecks
- Analyze system behavior comprehensively
- Optimize neural network performance
- Integrate profiling into your development workflow
- Maintain high performance standards

Start profiling today and discover new opportunities for optimization in your neural network applications!
