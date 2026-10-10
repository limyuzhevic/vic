# NLM Performance Profiling Documentation

## Overview

This document provides comprehensive documentation for the NLM (熙然) Performance Profiling System. It covers profiling methodology, tools, analysis techniques, and optimization strategies for neural simulations.

## Profiling Overview

Performance profiling in NLM is a systematic approach to measuring, analyzing, and optimizing the computational performance of neural simulations. The profiling system provides comprehensive insights into:

- **Computational Performance**: CPU usage, execution time, and throughput
- **Memory Usage**: Memory allocation patterns, consumption, and efficiency
- **I/O Performance**: File I/O, network I/O, and data transfer rates
- **System Resource Utilization**: CPU, memory, and GPU usage
- **Scalability**: Performance characteristics across different scales

The NLM profiling system is designed to support:

- **Real-time Profiling**: Live profiling of running simulations
- **Post-run Analysis**: Analysis of completed simulation runs
- **Comparative Analysis**: Comparison between different configurations
- **Automated Profiling**: Scheduled and continuous profiling
- **Custom Metrics**: User-defined performance metrics

## Profiling Architecture

### 1. Profiling Components

```
┌─────────────────────────────────────────────────────────────┐
│                    PROFILING FRAMEWORK                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │  Data Collector │  │   Data Analyzer │  │  Report Generator │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│         │                       │                      │
│         ▼                       ▼                      ▼ │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │    CPU Probe    │  │   Memory Probe  │  │   I/O Probe  │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│         │                       │                      │
│         ▼                       ▼                      ▼ │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │ Event Sampler   │  │   Allocator     │  │  File Monitor│ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2. Profiling Layers

#### Layer 1: Instrumentation Layer

- **Event Tracing**: High-resolution timestamp capture
- **Function Call Tracing**: Detailed function entry/exit tracking
- **System Call Monitoring**: Kernel-level operation recording
- **Memory Allocation Tracking**: Heap allocation/deallocation monitoring

#### Layer 2: Data Collection Layer

- **Sampling**: Periodic data collection at specified intervals
- **Trigger-based Collection**: Data collection on specific events
- **Continuous Collection**: Uninterrupted data streaming
- **Selective Collection**: Collection of specific metrics only

#### Layer 3: Data Analysis Layer

- **Statistical Analysis**: Mean, median, variance, standard deviation
- **Temporal Analysis**: Time-series analysis, trend detection
- **Comparative Analysis**: Cross-run and cross-configuration analysis
- **Pattern Recognition**: Identification of performance patterns

#### Layer 4: Report Generation Layer

- **Visual Reports**: Graphical representations of profiling data
- **Text Reports**: Human-readable summary reports
- **JSON Reports**: Structured data for further processing
- **Interactive Reports**: Web-based dashboards and visualizations

### 3. Profiling Modes

#### Mode 1: Sampling Profiling

**Purpose**: Lightweight profiling with minimal overhead

**Characteristics**:
- Collects data at regular intervals
- Lower overhead, suitable for long-running simulations
- Good for overall performance trends
- Limited detail on specific events

**Use Cases**:
- Monitoring system performance over time
- Identifying performance bottlenecks
- Comparing different simulation configurations

#### Mode 2: Instrumentation Profiling

**Purpose**: Detailed profiling with comprehensive data collection

**Characteristics**:
- Records all relevant events and activities
- Higher overhead but more detailed analysis
- Suitable for specific performance issues
- Good for deep-dive analysis

**Use Cases**:
- Debugging performance issues
- Analyzing specific function calls
- Memory leak detection
- I/O pattern analysis

#### Mode 3: Hybrid Profiling

**Purpose**: Balanced approach combining benefits of both modes

**Characteristics**:
- Intelligent sampling based on system activity
- Focused instrumentation on critical sections
- Adaptive profiling based on workload
- Optimized for both accuracy and efficiency

**Use Cases**:
- Production system monitoring
- Automated performance testing
- Continuous integration profiling

## Profiling Tools

### 1. Built-in Profiling Tools

#### `nlm profile`

**Description**: Command-line interface for managing profiling sessions.

**Commands**:

| Command | Description | Options |
|---------|-------------|---------|
| `start` | Start profiling session | `--duration`, `--output-file`, `--format` |
| `stop` | Stop profiling session | `--analyze`, `--export` |
| `report` | Generate profiling report | `--file`, `--type`, `--format` |

**Example**:

```bash
# Start profiling
nlm profile start --duration 300 --output-file perf_data.pstats

# Stop and analyze
nlm profile stop --analyze

# Generate report
nlm profile report --file profile.pstats --type performance
```

#### `nlm benchmark`

**Description**: Run performance benchmarks.

**Commands**:

| Command | Description | Options |
|---------|-------------|---------|
| `run` | Run benchmark suite | `--test`, `--neuron-count`, `--steps` |
| `compare` | Compare results | `--baseline`, `--current` |
| `analyze` | Analyze results | `--file`, `--metric` |

**Example**:

```bash
# Run neuron benchmarks
nlm benchmark run --test neuron --neuron-count 2000 --iterations 5

# Compare results
nlm benchmark compare --baseline baseline.json --current current.json
```

### 2. External Profiling Tools

#### Linux Performance Tools

| Tool | Purpose | Command |
|------|---------|---------|
| `perf` | CPU and system call profiling | `perf record ./nlm` |
| `valgrind` | Memory leak detection | `valgrind ./nlm` |
| `strace` | System call tracing | `strace -f ./nlm` |
| `time` | Time measurement | `time ./nlm` |
| `htop` | Process monitoring | `htop` |

#### Python Profiling Tools

| Tool | Purpose | Command |
|------|---------|---------|
| `cProfile` | Python function profiling | `python -m cProfile script.py` |
| `memory_profiler` | Memory usage profiling | `python -m memory_profiler script.py` |
| `line_profiler` | Line-by-line profiling | `python -m line_profiler script.py` |
| `snakeviz` | Visualization | `python -m snakeviz profile.stats` |

#### GPU Profiling Tools

| Tool | Purpose | Command |
|------|---------|---------|
| `nvprof` | NVIDIA GPU profiling | `nvprof ./nlm` |
| `nsys` | NVIDIA system profiler | `nsys profile ./nlm` |
| `rocprof` | AMD GPU profiler | `rocprof ./nlm` |

### 3. Integration Tools

#### API Integration

```python
import pynlm
import time
import pynlm.profiling as prof

# Create profiling-enabled simulation
def profiled_simulation(neuron_count=1000, steps=1000):
    """Run simulation with profiling."""
    
    # Start profiling
    with prof.ProfileSession("simulation_profile"):
        # Create and configure brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with profiling
        for step in range(steps):
            brain.step(step)
            
            # Collect profiling data
            prof.collect_metrics(step)
        
        # Generate report
        prof.generate_report("simulation_report.html")
    
    return brain

# Run profiled simulation
brain = profiled_simulation(2000, 500)
```

#### Configuration File Integration

```bash
# Profile simulation with configuration file
nlm profile start --config profiling_config.json

# Profiling configuration
{
    "duration": 300,
    "output_file": "simulation_profile.pstats",
    "format": "pstats",
    "sampling_rate": 1000,
    "cpu_profiling": true,
    "memory_profiling": true,
    "io_profiling": false,
    "metrics": ["time", "memory", "cpu", "gpu"],
    "threshold": 0.01
}
```

## Profiling Metrics

### 1. CPU Performance Metrics

| Metric | Unit | Description | Typical Range |
|--------|------|-------------|---------------|
| **Execution Time** | seconds | Total simulation time | 10-1000s |
| **CPU Time** | seconds | Processor time used | 80-95% of wall time |
| **CPU Utilization** | % | Average CPU usage | 50-100% |
| **Instructions Per Second** | IPS | Instructions executed per second | 10^9-10^12 |
| **Branch Misprediction Rate** | % | Mispredicted branches | 1-20% |
| **Cache Miss Rate** | % | L1/L2/L3 cache misses | 5-30% |

### 2. Memory Performance Metrics

| Metric | Unit | Description | Typical Range |
|--------|------|-------------|---------------|
| **Memory Usage** | MB | Total memory consumed | 100-10000 MB |
| **Peak Memory** | MB | Maximum memory usage | 150-120% of average |
| **Allocation Count** | count | Number of allocations | 10^4-10^8 |
| **Deallocation Count** | count | Number of deallocations | Similar to allocation |
| **Memory Throughput** | MB/s | Memory bandwidth | 10-500 GB/s |
| **Working Set Size** | MB | Active memory footprint | 200-5000 MB |

### 3. I/O Performance Metrics

| Metric | Unit | Description | Typical Range |
|--------|------|-------------|---------------|
| **File I/O** | MB/s | File read/write throughput | 10-500 MB/s |
| **Network I/O** | MB/s | Network data transfer | 1-100 MB/s |
| **Disk I/O** | IOPS | Input/output operations per second | 100-10000 IOPS |
| **Latency** | ms | Average response time | 0.1-10 ms |
| **Throughput** | req/s | Operations per second | 100-10000 req/s |

### 4. System Performance Metrics

| Metric | Unit | Description | Typical Range |
|--------|------|-------------|---------------|
| **CPU Temperature** | °C | Processor temperature | 30-85°C |
| **Memory Temperature** | °C | RAM temperature | 25-60°C |
| **Energy Consumption** | W | Power usage | 50-500 W |
| **Network Bandwidth** | Mbps | Network throughput | 100-10000 Mbps |

## Profiling Analysis

### 1. Statistical Analysis

#### Basic Statistics

```python
import numpy as np
import pandas as pd

def analyze_performance(data):
    """Analyze performance data."""
    
    # Convert to pandas DataFrame
    df = pd.DataFrame(data)
    
    # Basic statistics
    stats = {
        'mean_time': df['execution_time'].mean(),
        'median_time': df['execution_time'].median(),
        'std_time': df['execution_time'].std(),
        'min_time': df['execution_time'].min(),
        'max_time': df['execution_time'].max(),
        'mean_memory': df['memory_usage'].mean(),
        'peak_memory': df['memory_usage'].max(),
        'cpu_utilization': df['cpu_utilization'].mean()
    }
    
    return stats
```

#### Time Series Analysis

```python
import matplotlib.pyplot as plt
import seaborn as sns

def plot_performance_trends(data):
    """Plot performance trends over time."""
    
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Execution time trend
    axes[0, 0].plot(data['timestamp'], data['execution_time'])
    axes[0, 0].set_title('Execution Time Trend')
    axes[0, 0].set_xlabel('Time (s)')
    axes[0, 0].set_ylabel('Time (s)')
    
    # Memory usage trend
    axes[0, 1].plot(data['timestamp'], data['memory_usage'])
    axes[0, 1].set_title('Memory Usage Trend')
    axes[0, 1].set_xlabel('Time (s)')
    axes[0, 1].set_ylabel('Memory (MB)')
    
    # CPU utilization trend
    axes[1, 0].plot(data['timestamp'], data['cpu_utilization'])
    axes[1, 0].set_title('CPU Utilization Trend')
    axes[1, 0].set_xlabel('Time (s)')
    axes[1, 0].set_ylabel('CPU (%)')
    
    # I/O throughput trend
    axes[1, 1].plot(data['timestamp'], data['io_throughput'])
    axes[1, 1].set_title('I/O Throughput Trend')
    axes[1, 1].set_xlabel('Time (s)')
    axes[1, 1].set_ylabel('I/O (MB/s)')
    
    plt.tight_layout()
    plt.savefig('performance_trends.png')
    plt.show()
```

### 2. Comparative Analysis

#### Configuration Comparison

```python
import json
from pathlib import Path

def compare_configurations(baseline_file, current_file):
    """Compare performance between two configurations."""
    
    # Load results
    with open(baseline_file, 'r') as f:
        baseline = json.load(f)
    
    with open(current_file, 'r') as f:
        current = json.load(f)
    
    # Compare key metrics
    comparison = {
        'execution_time': {
            'baseline': baseline['execution_time'],
            'current': current['execution_time'],
            'improvement': (baseline['execution_time'] - current['execution_time']) / baseline['execution_time'] * 100
        },
        'memory_usage': {
            'baseline': baseline['memory_usage'],
            'current': current['memory_usage'],
            'improvement': (baseline['memory_usage'] - current['memory_usage']) / baseline['memory_usage'] * 100
        },
        'cpu_efficiency': {
            'baseline': baseline['cpu_efficiency'],
            'current': current['cpu_efficiency'],
            'improvement': (current['cpu_efficiency'] - baseline['cpu_efficiency']) / baseline['cpu_efficiency'] * 100
        }
    }
    
    return comparison

# Example usage
comparison = compare_configurations('baseline.json', 'current.json')
print(json.dumps(comparison, indent=2))
```

#### Performance Profiling

```python
import numpy as np
import matplotlib.pyplot as plt

def identify_performance_bottlenecks(data, threshold=0.1):
    """Identify performance bottlenecks."""
    
    bottlenecks = []
    
    # Analyze execution time
    execution_time = data['execution_time']
    mean_time = np.mean(execution_time)
    std_time = np.std(execution_time)
    
    # Identify outliers
    outliers = np.where(execution_time > mean_time + threshold * std_time)[0]
    
    for outlier in outliers:
        bottlenecks.append({
            'type': 'execution_time',
            'timestamp': data['timestamp'][outlier],
            'value': execution_time[outlier],
            'severity': 'high' if execution_time[outlier] > mean_time + 2 * std_time else 'medium'
        })
    
    # Analyze memory usage
    memory_usage = data['memory_usage']
    mean_memory = np.mean(memory_usage)
    memory_growth = (memory_usage[-1] - memory_usage[0]) / memory_usage[0] * 100
    
    if memory_growth > 50:
        bottlenecks.append({
            'type': 'memory_growth',
            'timestamp': data['timestamp'][-1],
            'value': memory_growth,
            'severity': 'high'
        })
    
    return bottlenecks
```

### 3. Visualization

#### Performance Dashboards

```python
def create_performance_dashboard(data):
    """Create interactive performance dashboard."""
    
    # Create figure with subplots
    fig = plt.figure(figsize=(15, 10))
    
    # Create subplots
    ax1 = plt.subplot(2, 3, 1)
    ax2 = plt.subplot(2, 3, 2)
    ax3 = plt.subplot(2, 3, 3)
    ax4 = plt.subplot(2, 3, 4)
    ax5 = plt.subplot(2, 3, 5)
    ax6 = plt.subplot(2, 3, 6)
    
    # Plot execution time
    ax1.plot(data['timestamp'], data['execution_time'])
    ax1.set_title('Execution Time')
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Time (s)')
    
    # Plot memory usage
    ax2.plot(data['timestamp'], data['memory_usage'])
    ax2.set_title('Memory Usage')
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('Memory (MB)')
    
    # Plot CPU utilization
    ax3.plot(data['timestamp'], data['cpu_utilization'])
    ax3.set_title('CPU Utilization')
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('CPU (%)')
    
    # Plot I/O throughput
    ax4.plot(data['timestamp'], data['io_throughput'])
    ax4.set_title('I/O Throughput')
    ax4.set_xlabel('Time (s)')
    ax4.set_ylabel('I/O (MB/s)')
    
    # Create histogram
    ax5.hist(data['execution_time'], bins=50, alpha=0.7)
    ax5.set_title('Execution Time Distribution')
    ax5.set_xlabel('Time (s)')
    ax5.set_ylabel('Frequency')
    
    # Create correlation heatmap
    corr_matrix = data[['execution_time', 'memory_usage', 'cpu_utilization']].corr()
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, ax=ax6)
    ax6.set_title('Performance Correlation')
    
    plt.tight_layout()
    plt.savefig('performance_dashboard.png')
    plt.show()
```

## Optimization Strategies

### 1. CPU Optimization

#### Vectorization

```cpp
// Example: Vectorized neural computation
void simulateNeuronsVectorized(
    float* inputs,
    float* weights,
    float* outputs,
    int count
) {
    // Use SIMD instructions for vectorized operations
    const int vector_size = 8;  // For AVX2
    int i = 0;
    
    for (; i + vector_size <= count; i += vector_size) {
        // Load vectors
        __m256 input_vec = _mm256_loadu_ps(inputs + i);
        __m256 weight_vec = _mm256_loadu_ps(weights + i);
        
        // Perform vector operations
        __m256 result_vec = _mm256_mul_ps(input_vec, weight_vec);
        
        // Store results
        _mm256_storeu_ps(outputs + i, result_vec);
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        outputs[i] = inputs[i] * weights[i];
    }
}
```

#### Threading

```cpp
#include <thread>
#include <vector>

void simulateNeuronsThreaded(
    float* inputs,
    float* weights,
    float* outputs,
    int count,
    int num_threads
) {
    int elements_per_thread = count / num_threads;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        int start = t * elements_per_thread;
        int end = (t == num_threads - 1) ? count : start + elements_per_thread;
        
        threads.emplace_back([=]() {
            for (int i = start; i < end; ++i) {
                outputs[i] = inputs[i] * weights[i];
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}
```

### 2. Memory Optimization

#### Memory Pooling

```cpp
// src/core/MemoryPool.hpp
#pragma once
#include <vector>
#include <unordered_map>

class MemoryPool {
public:
    template<typename T>
    T* allocate(size_t count = 1) {
        void* ptr = allocateMemory(sizeof(T) * count);
        return reinterpret_cast<T*>(ptr);
    }
    
    template<typename T>
    void deallocate(T* ptr, size_t count = 1) {
        freeMemory(ptr, sizeof(T) * count);
    }
    
    // Get current memory usage
    size_t getMemoryUsage() const { return total_memory_usage_; }
    
    // Get number of allocations
    size_t getAllocationCount() const { return allocation_count_; }
    
private:
    void* allocateMemory(size_t size);
    void freeMemory(void* ptr, size_t size);
    
    std::vector<void*> allocated_blocks_;
    std::unordered_map<size_t, size_t> memory_usage_;
    size_t total_memory_usage_ = 0;
    size_t allocation_count_ = 0;
};
```

#### Memory Reuse

```python
import pynlm

class MemoryOptimizedSimulation:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.memory_pool = MemoryPool()
    
    def reuse_memory(self, new_neuron_count):
        """Reuse memory from previous simulation."""
        if self.brain:
            # Clear previous brain
            del self.brain
            # Force garbage collection
            import gc
            gc.collect()
        
        # Update configuration
        self.config.set("brain.neuron_count", new_neuron_count)
        
        # Create new brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def run_simulation(self, steps):
        """Run simulation with memory optimization."""
        for step in range(steps):
            # Reuse memory for each step
            self.reuse_memory(max(1000, int(steps * (step + 1) / steps)))
            
            # Run step
            self.brain.step(step)
```

### 3. I/O Optimization

#### File I/O Optimization

```cpp
// src/io/optimized_file_io.hpp
#pragma once
#include <fstream>
#include <filesystem>

namespace nlm {
namespace io {

class OptimizedFileIO {
public:
    // Write simulation results with buffering
    static void writeResults(const std::string& filename,
                            const float* data,
                            size_t count,
                            bool compressed = false) {
        std::ofstream file(filename, std::ios::binary);
        
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        if (compressed) {
            // Write compressed data
            writeCompressed(file, data, count);
        } else {
            // Write raw data with buffering
            const size_t buffer_size = 1024 * 1024;  // 1MB buffer
            char* buffer = new char[buffer_size];
            
            for (size_t i = 0; i < count; i += buffer_size / sizeof(float)) {
                size_t chunk_size = std::min(buffer_size, (count - i) * sizeof(float));
                // Copy to buffer
                std::memcpy(buffer, data + i, chunk_size);
                file.write(buffer, chunk_size);
            }
            
            delete[] buffer;
        }
        
        file.close();
    }
    
    // Read simulation results with memory mapping
    static std::vector<float> readResults(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        // Get file size
        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Memory map file
        std::vector<float> data(file_size / sizeof(float));
        file.read(reinterpret_cast<char*>(data.data()), file_size);
        
        return data;
    }
    
private:
    static void writeCompressed(std::ofstream& file,
                                const float* data,
                                size_t count) {
        // Implement compression (e.g., zlib, LZ4)
    }
};

} // namespace io
} // namespace nlm
```

## Automated Profiling

### 1. Scheduled Profiling

#### Cron-based Profiling

```bash
# Add to crontab for daily profiling
0 2 * * * /path/to/nlm benchmark run --test neuron --neuron-count 2000 --output-file /path/to/results/daily_neuron_benchmark.json

# Weekly profiling
0 0 * * 0 /path/to/nlm profile start --duration 3600 --output-file /path/to/results/weekly_profile.pstats
```

#### Systemd Timer

```ini
# /etc/systemd/system/nlm-profiler.service
[Unit]
Description=NLM Profiler Service
After=network.target

[Service]
Type=oneshot
ExecStart=/usr/local/bin/nlm profile start --duration 300 --output-file /var/log/nlm/profile.pstats

[Install]
WantedBy=multi-user.target
```

```ini
# /etc/systemd/system/nlm-profiler.timer
[Unit]
Description=Run NLM profiler every 6 hours
Requires=nlm-profiler.service

[Timer]
OnBootSec=1min
OnUnitActiveSec=21600s

[Install]
WantedBy=timers.target
```

### 2. Continuous Profiling

#### Real-time Monitoring

```bash
# Continuous profiling with rolling windows
while true; do
    echo "Starting profiling at $(date)"
    nlm profile start --duration 300 --output-file profile_$(date +%Y%m%d_%H%M%S).pstats
    
    # Wait for profiling to complete
    wait $!
    
    # Analyze immediately
    nlm profile report --file profile_$(date +%Y%m%d_%H%M%S).pstats --format html
    
    # Wait before next profiling session
    sleep 300
    
done
```

#### Integration with Monitoring Systems

```python
import time
import subprocess
import threading
from datetime import datetime

class ContinuousProfiler:
    def __init__(self, simulation_command, interval=300):
        self.simulation_command = simulation_command
        self.interval = interval
        self.is_running = False
        self.profiler_thread = None
    
    def start_profiling(self):
        """Start continuous profiling."""
        self.is_running = True
        self.profiler_thread = threading.Thread(target=self._continuous_profile)
        self.profiler_thread.daemon = True
        self.profiler_thread.start()
        print(f"Started continuous profiling with interval {self.interval} seconds")
    
    def stop_profiling(self):
        """Stop continuous profiling."""
        self.is_running = False
        if self.profiler_thread:
            self.profiler_thread.join(timeout=10)
        print("Stopped continuous profiling")
    
    def _continuous_profile(self):
        """Internal profiling loop."""
        while self.is_running:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"profile_{timestamp}.pstats"
            
            print(f"Starting profiling at {timestamp}")
            
            # Start profiling
            subprocess.run([
                "nlm", "profile", "start",
                "--duration", str(self.interval),
                "--output-file", output_file
            ])
            
            # Wait for profiling to complete
            time.sleep(self.interval)
            
            # Analyze profile
            subprocess.run([
                "nlm", "profile", "report",
                "--file", output_file,
                "--format", "html"
            ])
            
            print(f"Completed profiling at {timestamp}")
```

## Profiling Configuration

### 1. Configuration File

#### `profiling_config.json`

```json
{
    "profiling": {
        "enabled": true,
        "mode": "hybrid",
        "sampling_rate": 1000,
        "memory_threshold": 100,
        "cpu_threshold": 80,
        "io_threshold": 500
    },
    
    "cpu_profiling": {
        "enabled": true,
        "sampling_interval": 1,
        "event_types": ["instructions", "cycles", "branches"],
        "threshold": 0.01
    },
    
    "memory_profiling": {
        "enabled": true,
        "allocation_tracking": true,
        "deallocation_tracking": true,
        "threshold": 10
    },
    
    "io_profiling": {
        "enabled": false,
        "file_monitoring": true,
        "network_monitoring": false,
        "threshold": 100
    },
    
    "output": {
        "format": "json",
        "directory": "profiles",
        "compression": true,
        "retention_days": 30
    },
    
    "analysis": {
        "auto_analysis": true,
        "report_generation": true,
        "visualization": true,
        "alerting": true,
        "thresholds": {
            "execution_time": 1000,
            "memory_usage": 100,
            "cpu_utilization": 80
        }
    }
}
```

### 2. Command Line Configuration

```bash
# Configure profiling via command line
cat > profiling_config.json << EOF
{
    "profiling": {
        "enabled": true,
        "mode": "sampling",
        "sampling_rate": 500
    },
    "output": {
        "format": "json",
        "directory": "custom_profiles"
    }
}
EOF

# Use configuration file
nlm profile start --config profiling_config.json
```

## Profiling Best Practices

### 1. Profiling Guidelines

#### Do's

- **Start with sampling**: Begin with sampling profiling to get baseline data
- **Use consistent configurations**: Use the same configuration for comparison
- **Profile realistic workloads**: Profile workloads that represent real usage
- **Validate results**: Always validate profiling results with known benchmarks
- **Document configurations**: Document all configuration parameters used

#### Don'ts

- **Don't profile production systems**: Avoid profiling in production environments
- **Don't profile with inappropriate tools**: Use appropriate profiling tools for the task
- **Don't ignore overhead**: Always account for profiling overhead in analysis
- **Don't sample too frequently**: Excessive sampling increases overhead
- **Don't store raw data**: Store only analyzed results when possible

### 2. Performance Analysis

#### Analyzing Performance Data

```python
def analyze_performance_data(data, thresholds):
    """Analyze performance data using defined thresholds."""
    
    analysis = {
        'execution_time': {
            'current': data['execution_time'],
            'threshold': thresholds['execution_time'],
            'status': 'OK' if data['execution_time'] < thresholds['execution_time'] else 'WARNING'
        },
        'memory_usage': {
            'current': data['memory_usage'],
            'threshold': thresholds['memory_usage'],
            'status': 'OK' if data['memory_usage'] < thresholds['memory_usage'] else 'WARNING'
        },
        'cpu_utilization': {
            'current': data['cpu_utilization'],
            'threshold': thresholds['cpu_utilization'],
            'status': 'OK' if data['cpu_utilization'] < thresholds['cpu_utilization'] else 'WARNING'
        }
    }
    
    return analysis

# Example thresholds
thresholds = {
    'execution_time': 1000,  # ms
    'memory_usage': 100,    # MB
    'cpu_utilization': 80   # %
}

# Analyze data
analysis = analyze_performance_data(benchmark_results, thresholds)
print(json.dumps(analysis, indent=2))
```

### 3. Reporting

#### Generating Reports

```python
import matplotlib.pyplot as plt
import pandas as pd

def generate_performance_report(data, output_file="performance_report.html"):
    """Generate comprehensive performance report."""
    
    # Create HTML report
    html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Performance Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; }}
        .section {{ margin: 20px 0; }}
        .metric {{ background-color: #f9f9f9; padding: 10px; margin: 10px 0; }}
        .good {{ border-left: 4px solid #00ff00; }}
        .warning {{ border-left: 4px solid #ffaa00; }}
        .critical {{ border-left: 4px solid #ff0000; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>Performance Report</h1>
        <p>Generated: {datetime.now()}</p>
    </div>
    
    <div class="section">
        <h2>Summary</h2>
        <div class="metric good">
            <h3>Execution Time</h3>
            <p>Current: {data['execution_time']:.2f} ms</p>
            <p>Average: {data['execution_time_mean']:.2f} ms</p>
            <p>Min: {data['execution_time_min']:.2f} ms</p>
            <p>Max: {data['execution_time_max']:.2f} ms</p>
        </div>
        
        <div class="metric good">
            <h3>Memory Usage</h3>
            <p>Current: {data['memory_usage']:.2f} MB</p>
            <p>Average: {data['memory_usage_mean']:.2f} MB</p>
            <p>Peak: {data['memory_usage_peak']:.2f} MB</p>
        </div>
        
        <div class="metric good">
            <h3>CPU Utilization</h3>
            <p>Current: {data['cpu_utilization']:.2f} %</p>
            <p>Average: {data['cpu_utilization_mean']:.2f} %</p>
            <p>Max: {data['cpu_utilization_max']:.2f} %</p>
        </div>
    </div>
    
    <div class="section">
        <h2>Performance Charts</h2>
        <img src="execution_time_chart.png" alt="Execution Time Chart">
        <img src="memory_usage_chart.png" alt="Memory Usage Chart">
        <img src="cpu_utilization_chart.png" alt="CPU Utilization Chart">
    </div>
    
    <div class="section">
        <h2>Recommendations</h2>
        <ul>
            <li>Consider increasing parallelism for better CPU utilization</li>
            <li>Optimize memory allocation to reduce peak memory usage</li>
            <li>Profile I/O operations to identify bottlenecks</li>
            <li>Implement caching for frequently accessed data</li>
        </ul>
    </div>
</body>
</html>
"""
    
    # Write HTML report
    with open(output_file, 'w') as f:
        f.write(html_content)
    
    print(f"Report generated: {output_file}")
```

## Conclusion

The NLM Performance Profiling System provides comprehensive tools for measuring, analyzing, and optimizing neural simulation performance. By following the guidelines and best practices outlined in this documentation, users can:

1. **Establish Baseline Performance**: Measure system performance before optimization
2. **Identify Bottlenecks**: Locate performance constraints in the system
3. **Compare Configurations**: Evaluate different system configurations
4. **Monitor Performance**: Continuously monitor system performance
5. **Generate Reports**: Create comprehensive performance documentation
6. **Optimize Performance**: Apply optimization strategies based on profiling data

The profiling system is designed to be flexible and extensible, allowing users to customize profiling for their specific needs. Whether you're running simple simulations or complex performance-critical applications, the NLM profiling tools provide the insights needed to achieve optimal performance.

For more information and additional examples, refer to:
- **USAGE.md** - Complete usage guide
- **INSTALLATION.md** - Installation and setup instructions
- **DEVELOPING.md** - Development and extension guide
- **ARCHITECTURE.md** - System architecture documentation
- **CLI.md** - Command-line interface reference
- **PROFILING.md** - Performance profiling documentation (this document)
- **PLUGINS.md** - Plugin development guide
- **TUTORIALS.md** - Step-by-step tutorials

The NLM profiling system continues to evolve, with new features and capabilities being added regularly. Stay updated with the latest documentation and tools to take full advantage of the profiling capabilities.
