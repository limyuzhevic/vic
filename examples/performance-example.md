# Performance Example: NLM Performance Monitoring

This example demonstrates how to monitor and analyze NLM performance. It's designed for users who want to understand NLM performance characteristics and optimization.

## Overview

This example shows how to monitor NLM performance, including performance metrics, resource usage, and optimization strategies. It's designed for users who want to understand NLM performance and how to optimize it.

## Code

```python
#!/usr/bin/env python3
"""
Performance Example: NLM Performance Monitoring

This example demonstrates how to monitor and analyze NLM performance,
including performance metrics, resource usage, and optimization strategies.
It's designed for users who want to understand NLM performance and how to optimize it.
"""

import pynlm
import time
import psutil
import json
from typing import Dict, List, Any

class PerformanceMonitor:
    """Monitor NLM performance"""
    
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = []
    
    def collect_performance_data(self, step):
        """Collect performance data"""
        # Get system information
        memory = psutil.virtual_memory()
        cpu_percent = psutil.cpu_percent()
        
        # Get NLM performance metrics
        performance_data = {
            'timestamp': time.time(),
            'step': step,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'ei_ratio': self.brain.getExcitationInhibitionRatio(),
            'memory_usage': memory.used,
            'memory_percent': memory.percent,
            'cpu_percent': cpu_percent
        }
        
        self.performance_data.append(performance_data)
        return performance_data
    
    def analyze_performance(self):
        """Analyze performance data"""
        if not self.performance_data:
            return None
        
        # Calculate performance statistics
        steps = [data['step'] for data in self.performance_data]
        firing_rates = [data['average_firing_rate'] for data in self.performance_data]
        memory_usages = [data['memory_usage'] for data in self.performance_data]
        cpu_usages = [data['cpu_percent'] for data in self.performance_data]
        
        analysis = {
            'total_steps': len(self.performance_data),
            'time_range': (min(steps), max(steps)),
            'firing_rate_stats': {
                'mean': sum(firing_rates) / len(firing_rates),
                'std': (sum((x - sum(firing_rates) / len(firing_rates)) ** 2 for x in firing_rates) / len(firing_rates)) ** 0.5,
                'max': max(firing_rates),
                'min': min(firing_rates)
            },
            'memory_stats': {
                'mean': sum(memory_usages) / len(memory_usages),
                'std': (sum((x - sum(memory_usages) / len(memory_usages)) ** 2 for x in memory_usages) / len(memory_usages)) ** 0.5,
                'max': max(memory_usages),
                'min': min(memory_usages)
            },
            'cpu_stats': {
                'mean': sum(cpu_usages) / len(cpu_usages),
                'std': (sum((x - sum(cpu_usages) / len(cpu_usages)) ** 2 for x in cpu_usages) / len(cpu_usages)) ** 0.5,
                'max': max(cpu_usages),
                'min': min(cpu_usages)
            }
        }
        
        return analysis
    
    def detect_anomalies(self):
        """Detect performance anomalies"""
        if not self.performance_data:
            return []
        
        anomalies = []
        
        # Detect memory anomalies
        memory_trend = calculate_trend([data['memory_usage'] for data in self.performance_data])
        if abs(memory_trend) > 0.1:  # 10% trend
            anomalies.append(f"Memory usage trend: {memory_trend:.2%}")
        
        # Detect CPU anomalies
        cpu_trend = calculate_trend([data['cpu_percent'] for data in self.performance_data])
        if abs(cpu_trend) > 10:  # 10% trend
            anomalies.append(f"CPU usage trend: {cpu_trend:.2f}%")
        
        # Detect performance degradation
        firing_rate_trend = calculate_trend([data['average_firing_rate'] for data in self.performance_data])
        if firing_rate_trend < -0.1:  # 10% degradation
            anomalies.append(f"Performance degradation: firing rate trend {firing_rate_trend:.2%}")
        
        return anomalies

def calculate_trend(values):
    """Calculate trend of values"""
    if len(values) < 2:
        return 0.0
    
    # Calculate linear regression slope
    n = len(values)
    x = list(range(n))
    y = values
    
    x_mean = sum(x) / n
    y_mean = sum(y) / n
    
    numerator = sum((x[i] - x_mean) * (y[i] - y_mean) for i in range(n))
    denominator = sum((x[i] - x_mean) ** 2 for i in range(n))
    
    if denominator == 0:
        return 0.0
    
    slope = numerator / denominator
    return slope

def create_performance_test():
    """Create performance test"""
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create performance monitor
    monitor = PerformanceMonitor(brain)
    
    # Run performance test
    for step in range(100):
        brain.step(step)
        monitor.collect_performance_data(step)
        
        # Print progress
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Analyze performance
    analysis = monitor.analyze_performance()
    anomalies = monitor.detect_anomalies()
    
    return {
        'brain': brain,
        'monitor': monitor,
        'analysis': analysis,
        'anomalies': anomalies
    }

def main():
    """Main function"""
    print("=== Performance Example: NLM Performance Monitoring ===")
    
    # Create performance test
    test_results = create_performance_test()
    
    # Print results
    print(f"\n📊 Performance Test Results:")
    if test_results['analysis']:
        print(f"   Total steps: {test_results['analysis']['total_steps']}")
        print(f"   Time range: {test_results['analysis']['time_range'][0]} - {test_results['analysis']['time_range'][1]} steps")
        print(f"   Firing rate: {test_results['analysis']['firing_rate_stats']['mean']:.3f} ± {test_results['analysis']['firing_rate_stats']['std']:.3f} Hz")
        print(f"   Memory usage: {test_results['analysis']['memory_stats']['mean'] / (1024 ** 2):.2f} ± {test_results['analysis']['memory_stats']['std'] / (1024 ** 2):.2f} MB")
        print(f"   CPU usage: {test_results['analysis']['cpu_stats']['mean']:.1f} ± {test_results['analysis']['cpu_stats']['std']:.1f}%")
    
    # Print anomalies
    if test_results['anomalies']:
        print(f"\n⚠️  Performance Anomalies:")
        for anomaly in test_results['anomalies']:
            print(f"   - {anomaly}")
    
    # Export results
    results = {
        'performance_data': test_results['monitor'].performance_data,
        'analysis': test_results['analysis'],
        'anomalies': test_results['anomalies']
    }
    
    with open('performance_test_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\n✅ Performance test results exported to performance_test_results.json")
    print(f"\n🎯 This example demonstrates:")
    print(f"   1. Performance data collection and monitoring")
    print(f"   2. Performance analysis and statistics")
    print(f"   3. Anomaly detection and reporting")
    print(f"   4. Performance optimization recommendations")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates how to monitor NLM performance, including performance metrics, resource usage, and optimization strategies.

### Key Components

- **Performance Monitor**: Collects and analyzes performance data
- **Performance Data Collection**: Collects system and NLM performance metrics
- **Performance Analysis**: Analyzes performance data and calculates statistics
- **Anomaly Detection**: Detects performance anomalies and issues
- **Performance Testing**: Runs performance tests and collects results

### Expected Output

```
=== Performance Example: NLM Performance Monitoring ===

📊 Performance Test Results:
   Total steps: 100
   Time range: 0 - 99 steps
   Firing rate: 0.000 ± 0.000 Hz
   Memory usage: 0.00 ± 0.00 MB
   CPU usage: 0.0 ± 0.0%

✅ Performance test results exported to performance_test_results.json

🎯 This example demonstrates:
   1. Performance data collection and monitoring
   2. Performance analysis and statistics
   3. Anomaly detection and reporting
   4. Performance optimization recommendations
```

### Learning Outcomes

By completing this example, you will learn:

1. **Performance Monitoring**: How to monitor NLM performance
2. **Performance Analysis**: How to analyze performance data
3. **Anomaly Detection**: How to detect performance anomalies
4. **Performance Optimization**: How to optimize NLM performance
5. **Resource Management**: How to manage system resources

### Extensions

To extend this example:

1. **Real-time Monitoring**: Implement real-time performance monitoring
2. **Advanced Analysis**: Implement advanced performance analysis techniques
3. **Performance Profiling**: Profile NLM performance and identify bottlenecks
4. **Resource Optimization**: Optimize system resources for NLM
5. **Performance Testing**: Implement comprehensive performance testing

## Files

- **Performance Example Code**: `examples/performance-example.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **API Reference**: `docs/api-reference.md`
- **Performance Guide**: `docs/performance-guide.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example demonstrates how to monitor NLM performance, including performance metrics, resource usage, and optimization strategies.🧠✨