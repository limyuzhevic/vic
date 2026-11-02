# Performance Monitoring and Optimization for NLM

This document describes performance monitoring and optimization features for the NLM (Neural Learning Machine) framework.

## Overview

Performance monitoring is crucial for understanding how the neural simulation performs and identifying bottlenecks. NLM provides comprehensive performance tracking capabilities through its enhanced logging system and dedicated performance monitoring tools.

## Performance Monitoring Features

### 1. Core Performance Metrics

NLM tracks the following key performance metrics:

- **Log Processing Time**: Time spent processing log entries
- **Memory Usage**: Memory consumption of the logging system
- **Log Throughput**: Number of log entries processed per second
- **CPU Usage**: CPU utilization during simulation
- **I/O Operations**: File read/write operations

### 2. Performance Monitoring API

```python
import pynlm
import time

# Start performance monitoring
perf_monitor = pynlm.PerformanceMonitor()
perf_monitor.start()

# Run your simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)

# Stop monitoring and get results
perf_stats = perf_monitor.stop()

print(f"Simulation Time: {perf_stats.simulation_time:.2f} seconds")
print(f"Steps per Second: {perf_stats.steps_per_second:.2f}")
print(f"Log Throughput: {perf_stats.log_throughput:.2f} entries/sec")
print(f"Memory Usage: {perf_stats.memory_usage:.2f} MB")
print(f"CPU Usage: {perf_stats.cpu_usage:.2f}%")
```

### 3. Performance Profiling

NLM provides several profiling tools:

#### Profile Simulation
```python
def profile_simulation(brain, world, agent, steps):
    """Profile simulation performance"""
    profiler = pynlm.Profiler()
    profiler.start()
    
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    results = profiler.stop()
    return results

# Run profiling
results = profile_simulation(brain, world, agent, 1000)
print(f"Profile Results: {results}")
```

#### Memory Profiling
```python
def memory_profile():
    """Profile memory usage"""
    import tracemalloc
    
    tracemalloc.start()
    
    # Run your simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    print(f"Current Memory: {current / 1024 / 1024:.2f} MB")
    print(f"Peak Memory: {peak / 1024 / 1024:.2f} MB")
    
    return {
        'current_memory_mb': current / 1024 / 1024,
        'peak_memory_mb': peak / 1024 / 1024,
        'average_memory_per_step': current / 100 / 1024
    }
```

### 4. Performance Optimization Techniques

#### Neuron Optimization
```python
def optimize_neuron_parameters(brain, config):
    """Optimize neuron parameters for better performance"""
    # Adjust simulation parameters
    config.set_float("neuron_threshold", -55.0)  # Lower threshold for faster firing
    config.set_float("neuron_refractory_period", 2.0)  # Shorter refractory period
    config.set_int("neuron_connections", 100)  # More connections
    
    brain.initialize()
    
    # Test performance
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
    
    end_time = time.time()
    return end_time - start_time

# Test different configurations
configs = [
    {"neuron_threshold": -60.0, "refractory": 5.0},
    {"neuron_threshold": -55.0, "refractory": 3.0},
    {"neuron_threshold": -50.0, "refractory": 1.0},
]

best_config = None
best_time = float('inf')

for i, config_data in enumerate(configs):
    config = pynlm.create_default_config_from_dict(config_data)
    elapsed = optimize_neuron_parameters(brain, config)
    
    if elapsed < best_time:
        best_time = elapsed
        best_config = config_data

print(f"Best configuration: {best_config}, Time: {best_time:.2f}s")
```

#### World Optimization
```python
def optimize_world_simulation(world, steps):
    """Optimize world simulation parameters"""
    # Test different update frequencies
    update_frequencies = [0.01, 0.05, 0.1, 0.2]
    
    results = []
    
    for dt in update_frequencies:
        start_time = time.time()
        
        for i in range(steps):
            world.update(dt)
            world.update(0.1)  # Additional update for comparison
        
        elapsed = time.time() - start_time
        
        results.append({
            'dt': dt,
            'total_time': elapsed,
            'steps_per_second': steps / elapsed,
            'average_update_time': elapsed / steps
        })
    
    return results

# Find optimal update frequency
optimal_result = min(results, key=lambda x: x['average_update_time'])
print(f"Optimal update time: {optimal_result['dt']}, "
      f"Steps/sec: {optimal_result['steps_per_second']:.2f}")
```

### 5. Real-time Performance Monitoring

```python
import threading
import time
from collections import deque

class RealTimePerformanceMonitor:
    """Real-time performance monitoring during simulation"""
    
    def __init__(self, window_size=100):
        self.window_size = window_size
        self.step_times = deque(maxlen=window_size)
        self.log_times = deque(maxlen=window_size)
        self.cpu_usage = deque(maxlen=window_size)
        self.memory_usage = deque(maxlen=window_size)
        self.is_monitoring = False
        self.monitor_thread = None
    
    def start_monitoring(self):
        """Start real-time monitoring"""
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop real-time monitoring"""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join()
    
    def _monitor_loop(self):
        """Main monitoring loop"""
        while self.is_monitoring:
            start_time = time.time()
            
            # Collect metrics
            current_cpu = self._get_cpu_usage()
            current_memory = self._get_memory_usage()
            
            self.cpu_usage.append(current_cpu)
            self.memory_usage.append(current_memory)
            
            # Calculate processing time
            processing_time = time.time() - start_time
            self.step_times.append(processing_time)
            
            time.sleep(0.1)  # Monitor every 100ms
    
    def get_performance_summary(self):
        """Get performance summary"""
        if not self.step_times:
            return None
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        avg_cpu = sum(self.cpu_usage) / len(self.cpu_usage) if self.cpu_usage else 0
        avg_memory = sum(self.memory_usage) / len(self.memory_usage) if self.memory_usage else 0
        
        return {
            'avg_step_time_ms': avg_step_time * 1000,
            'steps_per_second': 1.0 / avg_step_time if avg_step_time > 0 else 0,
            'avg_cpu_usage': avg_cpu,
            'avg_memory_usage_mb': avg_memory / 1024 / 1024,
            'min_step_time_ms': min(self.step_times) * 1000 if self.step_times else 0,
            'max_step_time_ms': max(self.step_times) * 1000 if self.step_times else 0,
            'step_time_std': self._std_dev(self.step_times) * 1000 if len(self.step_times) > 1 else 0
        }
    
    def _get_cpu_usage(self):
        """Get current CPU usage"""
        # Implement CPU usage measurement
        import psutil
        return psutil.cpu_percent()
    
    def _get_memory_usage(self):
        """Get current memory usage"""
        # Implement memory usage measurement
        import psutil
        return psutil.virtual_memory().used
    
    def _std_dev(self, values):
        """Calculate standard deviation"""
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
```

### 6. Automated Performance Testing

```python
class PerformanceTestSuite:
    """Automated performance testing suite"""
    
    def __init__(self, output_file="performance_report.html"):
        self.output_file = output_file
        self.results = []
    
    def run_performance_tests(self, test_scenarios):
        """Run performance tests for different scenarios"""
        print("Running performance tests...")
        
        for scenario in test_scenarios:
            print(f"\nTesting scenario: {scenario['name']}")
            
            results = self._run_scenario_test(scenario)
            self.results.append(results)
            
            self._print_scenario_results(scenario, results)
        
        self._generate_report()
        return self.results
    
    def _run_scenario_test(self, scenario):
        """Run a single performance test"""
        import time
        import tracemalloc
        
        # Start memory tracking
        tracemalloc.start()
        
        # Create simulation components
        brain = pynlm.create_brain(scenario['config'])
        world = pynlm.create_simple_world()
        world.configure(**scenario['world_config'])
        agent = pynlm.create_agent_brain(brain)
        
        # Start timing
        start_time = time.time()
        
        # Run simulation
        for step in range(scenario['steps']):
            world.update(scenario['world_dt'])
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Stop timing
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Get performance stats
        logger = nlm.Logger.getGlobal()
        perf_stats = logger.getPerformanceStats()
        
        return {
            'scenario': scenario['name'],
            'steps': scenario['steps'],
            'total_time': elapsed,
            'steps_per_second': scenario['steps'] / elapsed,
            'current_memory_mb': current / 1024 / 1024,
            'peak_memory_mb': peak / 1024 / 1024,
            'average_memory_per_step': current / scenario['steps'],
            'log_count': perf_stats.logCount,
            'average_processing_time': perf_stats.processingTime,
            'cpu_usage': 0,  # Would need actual measurement
        }
    
    def _print_scenario_results(self, scenario, results):
        """Print results for a scenario"""
        r = results
        print(f"  Steps: {r['steps']}")
        print(f"  Total Time: {r['total_time']:.2f}s")
        print(f"  Steps/sec: {r['steps_per_second']:.2f}")
        print(f"  Peak Memory: {r['peak_memory_mb']:.2f} MB")
        print(f"  Memory per Step: {r['average_memory_per_step']:.2f} bytes")
        print(f"  Logs Processed: {r['log_count']}")
        print(f"  Avg Processing Time: {r['average_processing_time']:.2f} μs")
    
    def _generate_report(self):
        """Generate performance report"""
        html_content = self._generate_html_report()
        
        with open(self.output_file, 'w') as f:
            f.write(html_content)
        
        print(f"\nPerformance report saved to: {self.output_file}")
    
    def _generate_html_report(self):
        """Generate HTML performance report"""
        html = """
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }
        .scenario { border: 1px solid #ddd; margin: 10px 0; padding: 15px; border-radius: 5px; }
        .best { background-color: #d4edda; border-color: #c3e6cb; }
        table { width: 100%; border-collapse: collapse; margin: 10px 0; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
        .metric { font-weight: bold; color: #2c3e50; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Report</h1>
        <p>Generated on: """ + time.strftime("%Y-%m-%d %H:%M:%S") + """
    </div>
"""
        
        # Find best scenario for each metric
        if self.results:
            best_steps_per_second = max(self.results, key=lambda x: x['steps_per_second'])
            best_memory = min(self.results, key=lambda x: x['peak_memory_mb'])
        
        html += """
    <h2>Overview</h2>
    <table>
        <tr><th>Scenario</th><th>Steps</th><th>Total Time (s)</th><th>Steps/sec</th><th>Peak Memory (MB)</th></tr>
"""
        
        for result in self.results:
            best_class = ""
            if result == best_steps_per_second or result == best_memory:
                best_class = " class=\"best\""
            
            html += f"""
        <tr{best_class}>
            <td>{result['scenario']}</td>
            <td>{result['steps']}</td>
            <td>{result['total_time']:.2f}</td>
            <td>{result['steps_per_second']:.2f}</td>
            <td>{result['peak_memory_mb']:.2f}</td>
        </tr>
"""
        
        html += """
    </table>
    
    <h2>Detailed Results</h2>
"""
        
        for result in self.results:
            html += f"""
    <div class="scenario">
        <h3>{result['scenario']}</h3>
        <table>
            <tr><td><strong>Steps:</strong></td><td>{result['steps']}</td></tr>
            <tr><td><strong>Total Time:</strong></td><td>{result['total_time']:.2f} seconds</td></tr>
            <tr><td><strong>Steps per Second:</strong></td><td class=\"metric\">{result['steps_per_second']:.2f}</td></tr>
            <tr><td><strong>Peak Memory:</strong></td><td>{result['peak_memory_mb']:.2f} MB</td></tr>
            <tr><td><strong>Memory per Step:</strong></td><td>{result['average_memory_per_step']:.2f} bytes</td></tr>
            <tr><td><strong>Logs Processed:</strong></td><td>{result['log_count']}</td></tr>
            <tr><td><strong>Average Processing Time:</strong></td><td>{result['average_processing_time']:.2f} μs</td></tr>
        </table>
    </div>
"""
        
        html += """
</body>
</html>
"""
        
        return html
```

## Optimization Recommendations

### 1. Neuron Optimization
- Use batch processing for neuron updates
- Implement efficient spike propagation algorithms
- Optimize memory layout for neuron data

### 2. Synapse Optimization
- Use sparse matrix representations for large networks
- Implement efficient synaptic weight updates
- Optimize for parallel processing

### 3. World Optimization
- Use spatial partitioning for large worlds
- Implement efficient collision detection
- Optimize object updates and rendering

### 4. Logging Optimization
- Use asynchronous logging for high-throughput scenarios
- Implement log compression and deduplication
- Use memory-mapped files for large log files

### 5. Memory Optimization
- Use object pooling for frequently allocated objects
- Implement efficient memory management
- Use streaming for large data processing

## Performance Benchmarking

### Basic Benchmark
```python
def benchmark_neuron_operations():
    """Benchmark basic neuron operations"""
    import time
    
    # Create test neurons
    neurons = []
    for i in range(1000):
        neuron = nlm.Neuron(nlm.NeuronId(i))
        neurons.append(neuron)
    
    # Benchmark step operations
    start_time = time.time()
    
    for i, neuron in enumerate(neurons):
        neuron.setMembranePotential(-60.0 + i * 0.1)
        neuron.injectCurrent(1.0)
        neuron.decrementRefractory()
        
        # Simulate firing
        if neuron.isFiring():
            neuron.recordSpike(time.time())
    
    elapsed = time.time() - start_time
    
    print(f"Processed {len(neurons)} neurons in {elapsed:.4f}s")
    print(f"Operations per second: {len(neurons) / elapsed:.2f}")
    
    return len(neurons) / elapsed
```

### Synapse Benchmark
```python
def benchmark_synaptic_operations():
    """Benchmark synaptic operations"""
    import time
    
    # Create test neurons and synapses
    neurons = [nlm.Neuron(nlm.NeuronId(i)) for i in range(100)]
    
    # Benchmark synaptic transmission
    start_time = time.time()
    
    for i in range(1000):
        for neuron in neurons:
            neuron.receiveExcitatoryInput(1.0)
            neuron.receiveInhibitoryInput(0.5)
    
    elapsed = time.time() - start_time
    
    print(f"Processed {1000 * len(neurons)} synaptic operations in {elapsed:.4f}s")
    print(f"Operations per second: {(1000 * len(neurons)) / elapsed:.2f}")
    
    return (1000 * len(neurons)) / elapsed
```

## Performance Monitoring Tools

### NLM Performance Dashboard
```python
class PerformanceDashboard:
    """Real-time performance dashboard"""
    
    def __init__(self):
        self.metrics = {}
        self.history = {}
    
    def update_metric(self, name, value):
        """Update a performance metric"""
        self.metrics[name] = value
        
        if name not in self.history:
            self.history[name] = []
        
        self.history[name].append(value)
        
        # Keep only recent history
        if len(self.history[name]) > 1000:
            self.history[name].pop(0)
    
    def get_chart_data(self, metric_name):
        """Get chart data for a metric"""
        if metric_name not in self.history:
            return None
        
        return {
            'labels': list(range(len(self.history[metric_name]))),
            'values': self.history[metric_name],
            'min': min(self.history[metric_name]),
            'max': max(self.history[metric_name]),
            'average': sum(self.history[metric_name]) / len(self.history[metric_name])
        }
    
    def generate_dashboard(self):
        """Generate performance dashboard HTML"""
        html = self._generate_html_dashboard()
        return html
    
    def _generate_html_dashboard(self):
        """Generate HTML dashboard"""
        html = """
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@3.7.0/dist/chart.min.js"></script>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .metric-card { background-color: #f8f9fa; border: 1px solid #dee2e6; 
                       border-radius: 5px; padding: 15px; margin: 10px 0; }
        .metric-value { font-size: 2em; font-weight: bold; color: #2c3e50; }
        .metric-label { font-size: 1.2em; color: #6c757d; }
        .chart-container { margin: 20px 0; }
    </style>
</head>
<body>
    <h1>NLM Performance Dashboard</h1>
    <p>Last updated: """ + time.strftime("%Y-%m-%d %H:%M:%S") + """
    
    <div id=\"metrics\"></div>
    
    <script>
        const metrics = {
"""
        
        for name, values in self.history.items():
            if values:
                html += f"""
            '{name}': {{
                labels: Array.from({{"length": values.length}}).map((_, i) => i),
                data: {values},
                borderColor: 'rgb({np.random.randint(0,255)}, {np.random.randint(0,255)}, {np.random.randint(0,255)})',
                backgroundColor: 'rgba({np.random.randint(0,255)}, {np.random.randint(0,255)}, {np.random.randint(0,255)}, 0.2)',
                tension: 0.1
            }}
"""
                
                if name != list(self.history.keys())[-1]:
                    html += ",\n"
        
        html += """
        };
        
        // Create charts
        const metricsContainer = document.getElementById('metrics');
        
        Object.entries(metrics).forEach(([name, data]) => {
            if (data.data && data.data.length > 0) {
                const card = document.createElement('div');
                card.className = 'metric-card';
                
                card.innerHTML = `
                    <h3>${name}</h3>
                    <div class=\"metric-value\">${data.data[data.data.length - 1].toFixed(2)}</div>
                    <div class=\"metric-label\">Current Value</div>
                    <div class=\"metric-label\">Min: ${Math.min(...data.data).toFixed(2)} | Max: ${Math.max(...data.data).toFixed(2)}</div>
                    <div class=\"chart-container\">
                        <canvas id=\"chart-${name}\"></canvas>
                    </div>
                `;
                
                metricsContainer.appendChild(card);
                
                // Create chart
                const ctx = document.getElementById(`chart-${name}`).getContext('2d');
                new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: data.labels,
                        datasets: [{
                            label: name,
                            data: data.data,
                            borderColor: data.borderColor,
                            backgroundColor: data.backgroundColor,
                            tension: data.tension
                        }]
                    },
                    options: {
                        responsive: true,
                        scales: {
                            y: {
                                beginAtZero: true
                            }
                        },
                        plugins: {
                            legend: {
                                display: false
                            }
                        }
                    }
                });
            }
        });
    </script>
</body>
</html>
"""
        
        return html
```

## Performance Optimization Checklist

### Before Running Production
1. **Enable Performance Monitoring**
   - Set up comprehensive logging
   - Configure memory tracking
   - Monitor CPU and memory usage

2. **Optimize Configuration**
   - Use appropriate neuron counts
   - Configure world dimensions
   - Set optimal simulation parameters

3. **Monitor System Resources**
   - Track memory usage
   - Monitor CPU utilization
   - Check I/O performance

4. **Benchmark Performance**
   - Run baseline performance tests
   - Identify bottlenecks
   - Optimize critical paths

### During Production
1. **Continuous Monitoring**
   - Track key performance metrics
   - Monitor system health
   - Detect performance degradation

2. **Dynamic Optimization**
   - Adjust parameters based on load
   - Scale resources as needed
   - Implement adaptive algorithms

3. **Alerting**
   - Set up performance alerts
   - Monitor thresholds
   - Take corrective action

### After Production
1. **Performance Analysis**
   - Analyze performance data
   - Identify optimization opportunities
   - Document findings

2. **Continuous Improvement**
   - Implement optimizations
   - Monitor impact
   - Validate improvements

3. **Documentation**
   - Update performance documentation
   - Share best practices
   - Train team members

## Conclusion

Performance monitoring and optimization are essential for maintaining and improving NLM performance. By implementing comprehensive performance monitoring, conducting regular benchmarks, and following optimization best practices, you can ensure that your NLM simulations run efficiently and effectively.

The tools and techniques described in this document provide a solid foundation for performance monitoring and optimization in NLM. Customize and extend these tools as needed to meet your specific requirements.

---

*Last updated: November 2, 2026*
*NLM Version: 0.1.0*
*Python Bindings: 1.0.0*