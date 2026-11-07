# NLM Performance Documentation - Enhanced Edition

## Comprehensive Performance Testing Guide

### Overview

Phase 5 introduces comprehensive performance optimizations enabling NLM to scale from thousands to millions of neurons. This document provides detailed guidance for performance benchmarking, configuration, and optimization.

## Quick Start: Performance Testing

### Getting Started with Performance Testing

1. **Build with Performance Support**
   ```bash
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
   make -j$(nproc)
   ```

2. **Run Basic Benchmark**
   ```bash
   ./nlm_benchmark
   ```

3. **Run Integration Test**
   ```bash
   ./nlm_phase6_demo
   ```

## Benchmark Configuration Examples

### Example 1: Small-Scale Testing (Development)

**Configuration File:** `configs/performance_small.cfg`

```ini
# Small-scale development configuration
# Optimized for fast iteration and testing

# Core simulation parameters
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Neuromodulation
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm_small_benchmark.log

# Performance settings
max_simulation_steps = 5000
simulation_time_limit = 0.0
visualization_enabled = false
```

**Build Command:**
```bash
cd build
make nlm_small_benchmark
./nlm_small_benchmark --config configs/performance_small.cfg
```

### Example 2: Medium-Scale Production Testing

**Configuration File:** `configs/performance_medium.cfg`

```ini
# Medium-scale production configuration
# Optimized for comprehensive performance measurement

# Core simulation parameters
random_seed = 42
simulation_timestep = 0.0005
neuron_count = 50000
region_count =  lengthy(2)
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation
dopamine_baseline = 0.2
reward_discount_factor = 0.95

# Environment
environment_name = GridWorld
environment_width = 50
environment_height = 50

# Logging
log_level = WARNING
log_to_file = true
log_filename = nlm_medium_benchmark.log

# Performance settings
max_simulation_steps = 10000
simulation_time_limit = 10.0
visualization_enabled = false
visualization_update_rate = 30.0

# Memory and performance
replay_interval = 1000
consolidation_interval = 5000
checkpoint_dir = ./medium_checkpoints
```

**Build Command:**
```bash
cd build
make nlm_medium_benchmark
./nlm_medium_benchmark --config configs/performance_medium.cfg
```

### Example 3: Large-Scale Stress Testing

**Configuration File:** `configs/performance_large.cfg`

```ini
# Large-scale stress testing configuration
# Optimized for maximum performance measurement

# Core simulation parameters
random_seed = 999
simulation_timestep = 0.0001
neuron_count = 500000
region_count = 4
connection_probability = 0.08

# Plasticity parameters
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_tau = 30.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000001

# Neuromodulation
dopamine_baseline = 0.15
reward_discount_factor = 0.90

# Environment
environment_name = LargeWorld
environment_width = 100
environment_height = 100

# Logging
log_level = ERROR
log_to_file = true
log_filename = nlm_large_benchmark.log

# Performance settings
max_simulation_steps = 100000
simulation_time_limit = 60.0
visualization_enabled = false
visualization_update_rate = 10.0

# Memory and performance
replay_interval = 5000
consolidation_interval = 20000
checkpoint_dir = ./large_checkpoints
```

**Build Command:**
```bash
cd build
make nlm_large_benchmark
./nlm_large_benchmark --config configs/performance_large.cfg
```

## Performance Benchmark Scripts

### Python Performance Monitoring Script

**File:** `scripts/monitor_performance.py`

```python
#!/usr/bin/env python3
"""
Performance monitoring script for NLM benchmarks.
Provides real-time monitoring and automated testing.
"""

import subprocess
import time
import json
import psutil
import argparse
from datetime import datetime

class NLMBenchmarkMonitor:
    def __init__(self, config_file):
        self.config_file = config_file
        self.benchmark_process = None
    
    def start_monitoring(self):
        """Start benchmark with real-time monitoring"""
        print(f"Starting NLM benchmark monitoring with config: {self.config_file}")
        
        # Start the benchmark process
        cmd = ["/path/to/nlm_benchmark", "--config", self.config_file]
        self.benchmark_process = subprocess.Popen(cmd, stdout=subprocess.PIPE, 
                                                stderr=subprocess.PIPE, text=True)
        
        # Monitor system resources
        monitor_pid = self.benchmark_process.pid
        print(f"Benchmark PID: {monitor_pid}")
        
        try:
            while True:
                # Check if process is still running
                if self.benchmark_process.poll() is not None:
                    print("Benchmark completed")
                    break
                
                # Get process memory and CPU usage
                process = psutil.Process(monitor_pid)
                mem_info = process.memory_info()
                cpu_percent = process.cpu_percent()
                
                print(f"Time: {time.time():.1f}s | "
                      f"CPU: {cpu_percent:.1f}% | "
                      f"Memory: {mem_info.rss / 1024 / 1024:.1f} MB")
                
                time.sleep(5)  # Monitor every 5 seconds
        
        except KeyboardInterrupt:
            print("Monitoring interrupted")
            self.benchmark_process.terminate()
        
        # Get final results
        stdout, stderr = self.benchmark_process.communicate()
        print("Benchmark output:")
        print(stdout)
        if stderr:
            print("Errors:")
            print(stderr)

def main():
    parser = argparse.ArgumentParser(description="Monitor NLM performance benchmarks")
    parser.add_argument("--config", required=True, help="Configuration file path")
    
    args = parser.parse_args()
    
    monitor = NLMBenchmarkMonitor(args.config)
    monitor.start_monitoring()

if __name__ == "__main__":
    main()
```

### Automated Performance Test Suite

**File:** `scripts/run_performance_suite.py`

```python
#!/usr/bin/env python3
"""
Automated performance test suite for NLM.
Runs multiple benchmarks and compares results.
"""

import subprocess
import json
import os
from pathlib import Path

class PerformanceTestSuite:
    def __init__(self, results_dir="results"):
        self.results_dir = Path(results_dir)
        self.results_dir.mkdir(exist_ok=True)
    
    def run_benchmark(self, name, neurons, steps, config_path=None):
        """Run a single benchmark"""
        print(f"Running benchmark: {name} ({neurons} neurons, {steps} steps)")
        
        cmd = ["nlm_benchmark"]
        if neurons:
            cmd.extend(["--neurons", str(neurons)])
        if steps:
            cmd.extend(["--steps", str(steps)])
        if config_path:
            cmd.extend(["--config", config_path])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        # Parse output (simplified - real implementation would parse properly)
        benchmark_result = {
            "name": name,
            "neurons": neurons,
            "steps": steps,
            "exit_code": result.returncode,
            "output": result.stdout,
            "error": result.stderr,
            "timestamp": datetime.now().isoformat()
        }
        
        # Save result
        result_file = self.results_dir / f"{name}.json"
        with open(result_file, 'w') as f:
            json.dump(benchmark_result, f, indent=2)
        
        return benchmark_result
    
    def run_suite(self):
        """Run the complete performance test suite"""
        print("Starting NLM Performance Test Suite")
        
        benchmarks = [
            ("small", 1000, 1000),
            ("medium", 10000, 5000),
            ("large", 50000, 1000),
        ]
        
        results = []
        for name, neurons, steps in benchmarks:
            result = self.run_benchmark(name, neurons, steps, "configs/default.cfg")
            results.append(result)
        
        # Generate report
        self.generate_report(results)
        
        return results
    
    def generate_report(self, results):
        """Generate performance test report"""
        print("\nPerformance Test Report")
        print("=" * 50)
        
        for result in results:
            if result["exit_code"] == 0:
                print(f"✓ {result['name']}: PASSED")
            else:
                print(f"✗ {result['name']}: FAILED")
                print(f"  Error: {result['error']}")
        
        # Save summary
        summary = {
            "total_tests": len(results),
            "passed_tests": sum(1 for r in results if r["exit_code"] == 0),
            "failed_tests": sum(1 for r in results if r["exit_code"] != 0),
            "results": results
        }
        
        summary_file = self.results_dir / "performance_summary.json"
        with open(summary_file, 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"\nSummary saved to: {summary_file}")

def main():
    suite = PerformanceTestSuite()
    suite.run_suite()

if __name__ == "__main__":
    main()
```

## Performance Configuration Guidelines

### Optimal Configuration for Different Use Cases

#### 1. Development and Testing

**Best Practices:**
- Small neuron counts (1k-10k) for fast iteration
- Enable detailed logging
- Use simple configurations
- Enable validation for debugging

**Recommended Settings:**
```ini
neuron_count = 5000
simulation_timestep = 0.001
log_level = DEBUG
max_simulation_steps = 1000
checkpoint_dir = ./dev_checkpoints
```

#### 2. Production Benchmarking

**Best Practices:**
- Medium to large scales (10k-100k neurons)
- Balance performance with observability
- Use structured logging
- Enable checkpointing for long runs

**Recommended Settings:**
```ini
neuron_count = 50000
simulation_timestep = 0.0005
log_level = INFO
max_simulation_steps = 10000
visualization_enabled = false
replay_interval = 1000
```

#### 3. Stress Testing

**Best Practices:**
- Maximum scale testing (100k+ neurons)
- Minimal logging overhead
- Extended run times
- Focus on raw performance metrics

**Recommended Settings:**
```ini
neuron_count = 200000
simulation_timestep = 0.0001
log_level = ERROR
max_simulation_steps = 50000
visualization_enabled = false
replay_interval = 10000
```

### Configuration Validation

#### Required Parameters

The following parameters must be present in any configuration:

- `environment_name`: World type (GridWorld, LargeWorld, etc.)
- `environment_width`: Grid width in cells
- `environment_height`: Grid height in cells

#### Optional Parameters with Defaults

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| `random_seed` | 42 | 0-∞ | Random seed for reproducibility |
| `simulation_timestep` | 0.001 | 0.0001-0.1 | Integration timestep |
| `neuron_count` | 1000 | 1-∞ | Total neurons in network |
| `region_count` | 2 | 1-∞ | Number of brain regions |
| `connection_probability` | 0.1 | 0.0-1.0 | Probability of synaptic connections |

#### Performance-Related Parameters

| Parameter | Default | Range | Impact |
|-----------|---------|-------|--------|
| `max_simulation_steps` | 10000 | 1-∞ | Simulation duration |
| `simulation_time_limit` | 0.0 | 0-∞ | Time limit in seconds |
| `visualization_update_rate` | 30.0 | 1-∞ | UI update frequency |
| `replay_interval` | 100 | 1-∞ | Memory replay frequency |
| `consolidation_interval` | 1000 | 1-∞ | Memory consolidation frequency |

## Performance Optimization Tips

### CPU Optimization

1. **Use Appropriate CPU Features**
   ```bash
   # Enable AVX-512 for maximum performance
   cmake -DNLM_ENABLE_AVX512=ON ..
   
   # Enable AVX2 (if AVX-512 not available)
   cmake -DNLM_ENABLE_AVX2=ON ..
   ```

2. **Thread Configuration**
   ```ini
   # Optimal thread count for different scales
   # Small (1k-10k neurons): 2-4 threads
   # Medium (10k-50k neurons): 4-8 threads  
   # Large (50k-100k neurons): 8-16 threads
   ```

3._FEATURES: "Performance optimizations for NLM benchmark simulations. Configure for optimal CPU and memory usage with vectorized operations and efficient data layouts.

This table continues with detailed configuration recommendations for different performance scenarios, covering CPU architecture, threading strategies, memory pool configurations, and event queue settings to maximize simulation efficiency and scalability.

It includes additional configuration parameters for fine-tuning benchmark performance across various computational environments, focusing on maximizing throughput and minimizing computational overhead.