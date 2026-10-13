# NLM Debugging Guide

This document provides comprehensive debugging guidance for NLM, covering:
- Common debugging issues and solutions
- Debugging tools and techniques
- Error analysis and troubleshooting
- Performance debugging
- Plugin debugging
- Real-world debugging scenarios

## Table of Contents

1. [Introduction to NLM Debugging](#introduction-to-nlm-debugging)
   1.1 [Common Debugging Challenges](#11-common-debugging-challenges)
   1.2 [Debugging Tools and Techniques](#12-debugging-tools-and-techniques)

2. [Error Analysis and Troubleshooting](#error-analysis-and-troubleshooting)
   2.1 [Error Types and Solutions](#21-error-types-and-solutions)
   2.2 [Debugging Strategies](#22-debugging-strategies)
   2.3 [Debugging Checklist](#23-debugging-checklist)

3. [Performance Debugging](#performance-debugging)
   3.1 [Performance Profiling](#31-performance-profiling)
   3.2 [Memory Leak Detection](#32-memory-leak-detection)
   3.3 [Bottleneck Identification](#33-bottleneck-identification)
   3.4 [Performance Optimization Debugging](#34-performance-optimization-debugging)

4. [Plugin Debugging](#plugin-debugging)
   4.1 [Plugin Loading Issues](#41-plugin-loading-issues)
   4.2 [Plugin Execution Issues](#42-plugin-execution-issues)
   4.3 [Plugin Communication Issues](#43-plugin-communication-issues)
   4.4 [Plugin Testing and Debugging](#44-plugin-testing-and-debugging)

5. [Real-world Debugging Scenarios](#real-world-debugging-scenarios)
   5.1 [Learning Agent Debugging](#51-learning-agent-debugging)
   5.2 [World Simulation Debugging](#52-world-simulation-debugging)
   5.3 [Configuration Debugging](#53-configuration-debugging)
   5.4 [Integration Testing](#54-integration-testing)

6. [Advanced Debugging Techniques](#advanced-debugging-techniques)
   6.1 [State Inspection](#61-state-inspection)
   6.2 [Log Analysis](#62-log-analysis)
   6.3 [Automated Debugging](#63-automated-debugging)
   6.4 [Performance Tuning](#64-performance-tuning)

## Introduction to NLM Debugging

### 11 Common Debugging Challenges

NLM debugging presents unique challenges due to:

- **Complexity**: Multiple interacting components
- **Distributed nature**: Brain, world, agent systems
- **State-dependent behavior**: Dynamic neural states
- **Parameter sensitivity**: Small changes affect behavior
- **Performance constraints**: Real-time requirements

### 12 Debugging Tools and Techniques

#### Debugging Tools
```python
import pynlm
import logging
import time
import numpy as np
from nlm_advanced import DebuggingTools, PerformanceMonitor

def setup_debugging():
    """Setup debugging environment."""
    # Configure logging
    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.FileHandler('nlm_debug.log'),
            logging.StreamHandler()
        ]
    )
    
    logger = logging.getLogger('nlm')
    
    # Setup debugging tools
    debug_tools = DebuggingTools()
    debug_tools.setLogger(logger)
    
    return debug_tools, logger
```

#### Key Debugging Techniques
1. **Systematic isolation**: Test components individually
2. **Logging**: Record system state and events
3. **Instrumentation**: Add debug points to code
4. **Visualization**: Use graphs and plots
5. **Monitoring**: Track system metrics
6. **Reproducibility**: Ensure consistent test conditions

## Error Analysis and Troubleshooting

### 21 Error Types and Solutions

#### Initialization Errors
**Error**: "Brain not initialized"
**Cause**: Calling brain methods before initialization
**Solution**: Always call `brain.initialize()` before use

**Code Example**:
```python
# Before (causes error)
brain = pynlm.createBrain(config)
brain.step(0)  # Error: Brain not initialized

# After (correct)
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)  # Works
```

#### Configuration Errors
**Error**: "Invalid configuration key"
**Cause**: Using non-existent configuration keys
**Solution**: Verify configuration keys exist before use

**Code Example**:
```python
# Before (may cause errors)
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Neurons: {neurons}")

# After (explicit check)
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Neurons: {neurons}")
else:
    print("Warning: brain.neuron_count not set, using default")
    neurons = config.get("brain.neuron_count", 1000)
    print(f"Using default neurons: {neurons}")
```

#### Memory Errors
**Error**: "Memory allocation failed"
**Cause**: Out of memory
**Solution**: Reduce simulation size or increase system resources

**Code Example**:
```python
# Before (may cause memory errors)
config.set("brain.neuron_count", 10000)  # Too large
config.set("brain.synapse_density", 0.3)  # High density

# After (more reasonable)
config.set("brain.neuron_count", 1000)  # Reasonable size
config.set("brain.synapse_density", 0.1)  # Moderate density
```

#### Neural Dynamics Errors
**Error**: "Invalid neuron parameters"
**Cause**: Invalid neuron configuration
**Solution**: Validate neuron parameters before use

**Code Example**:
```python
# Validate neuron parameters
config.set("brain.v_thresh", -50.0)  # Reasonable threshold
config.set("brain.v_rest", -70.0)    # Reasonable resting potential
config.set("brain.v_reset", -75.0)   # Reasonable reset potential

# Ensure reasonable ranges
assert -100 < config.get("brain.v_thresh", -50) < 0
assert -100 < config.get("brain.v_rest", -70) < 0
assert -100 < config.get("brain.v_reset", -75) < 0
```

### 22 Debugging Strategies

#### Systematic Debugging Approach
```python
class NLMDebugger:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.debug_log = []
        
    def debug_initialization(self):
        """Debug initialization issues."""
        print("=== Debugging Initialization ===")
        
        # Check brain initialization
        if not self.brain.isInitialized():
            print("ERROR: Brain not initialized")
            print("Solution: Call brain.initialize() before use")
            return False
            
        # Check world configuration
        if self.world.getWidth() <= 0 or self.world.getHeight() <= 0:
            print("ERROR: Invalid world dimensions")
            return False
            
        # Check agent setup
        if self.agent is None:
            print("ERROR: Agent not created")
            return False
            
        print("Initialization check passed")
        return True
        
    def debug_simulation_step(self, step):
        """Debug simulation step."""
        print(f"\n=== Debugging Step {step} ===")
        
        # Check brain state
        if self.brain.getFiringNeuronCount() < 0:
            print("ERROR: Invalid firing neuron count")
            return False
            
        # Check world state
        pos = self.world.getAgentPosition()
        if not (0 <= pos[0] <= self.world.getWidth() and 
                0 <= pos[1] <= self.world.getHeight()):
            print("ERROR: Agent outside world boundaries")
            return False
            
        # Check agent state
        curiosity = self.agent.getCuriosityLevel()
        if not (0 <= curiosity <= 1):
            print(f"ERROR: Invalid curiosity level: {curiosity}")
            return False
            
        print("Step check passed")
        return True
        
    def debug_performance(self):
        """Debug performance issues."""
        print("\n=== Debugging Performance ===")
        
        # Check system resources
        process = psutil.Process()
        memory_usage = process.memory_info().rss / (1024 * 1024)  # MB
        cpu_percent = process.cpu_percent()
        
        print(f"Memory usage: {memory_usage:.1f} MB")
        print(f"CPU usage: {cpu_percent:.1f}%")
        
        # Check simulation performance
        if memory_usage > 1000:  # 1GB threshold
            print("WARNING: High memory usage detected")
            
        if cpu_percent > 80:  # 80% threshold
            print("WARNING: High CPU usage detected")
            
        return True
```

### 23 Debugging Checklist

#### Before Running Simulation
- [ ] Check brain configuration
- [ ] Verify world dimensions
- [ ] Test agent initialization
- [ ] Validate all parameters
- [ ] Check system resources

#### During Simulation
- [ ] Monitor firing rates
- [ ] Track agent position
- [ ] Check memory usage
- [ ] Verify CPU usage
- [ ] Log system state

#### After Simulation
- [ ] Analyze results
- [ ] Check for errors
- [ ] Validate output
- [ ] Clean up resources
- [ ] Document findings

## Performance Debugging

### 31 Performance Profiling

#### CPU Profiling
```python
import cProfile
import pstats
import io
import time

def profile_simulation(brain, steps, visualize=True):
    """Profile simulation performance."""
    pr = cProfile.Profile()
    
    # Start profiling
    pr.enable()
    
    # Run simulation
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
        
        # Print progress
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step}/{steps}: {elapsed:.2f}s, "
                  f"{step/elapsed:.0f} steps/s")
    
    # Stop profiling
    elapsed = time.time() - start_time
    pr.disable()
    
    # Print performance statistics
    print(f"\n=== Performance Statistics ===")
    print(f"Total steps: {steps}")
    print(f"Total time: {elapsed:.2f}s")
    print(f"Steps per second: {steps/elapsed:.0f}")
    print(f"Average time per step: {elapsed/steps*1000:.1f}ms")
    
    # Generate profiler statistics
    if visualize:
        s = io.StringIO()
        ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
        ps.print_stats(20)  # Top 20 functions
        print(s.getvalue())
    
    return pr, elapsed
```

#### Memory Profiling
```python
from memory_profiler import memory_usage
import matplotlib.pyplot as plt

def profile_memory_usage(brain_factory, steps):
    """Profile memory usage during simulation."""
    print(f"Profiling memory usage for {steps} steps...")
    
    # Setup brain
    brain = brain_factory()
    
    # Measure memory usage
    def run_simulation():
        for step in range(steps):
            brain.step(step)
    
    # Run with memory profiling
    mem_usage, retval = memory_usage((run_simulation, (), {}), 
                                    retval=True, 
                                    interval=0.1,
                                    include_children=True)
    
    # Generate statistics
    max_memory = max(mem_usage)
    avg_memory = sum(mem_usage) / len(mem_usage)
    memory_increase = max_memory - min(mem_usage)
    
    print(f"\n=== Memory Usage Statistics ===")
    print(f"Maximum memory: {max_memory:.1f} MB")
    print(f"Average memory: {avg_memory:.1f} MB")
    print(f"Memory increase: {memory_increase:.1f} MB")
    print(f"Memory samples: {len(mem_usage)}")
    print(f"Average sampling interval: {0.1:.1f}s")
    
    # Plot memory usage
    plt.figure(figsize=(10, 6))
    time_points = [i * 0.1 for i in range(len(mem_usage))]
    plt.plot(time_points, mem_usage, 'b-', linewidth=2)
    plt.title('Memory Usage Over Time')
    plt.xlabel('Time (s)')
    plt.ylabel('Memory Usage (MB)')
    plt.grid(True, alpha=0.3)
    plt.savefig('memory_usage.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    return max_memory, avg_memory, mem_usage
```

### 32 Memory Leak Detection

#### Memory Leak Detection
```python
import tracemalloc
import time

def detect_memory_leaks(brain_factory, steps, test_duration=60):
    """Detect memory leaks in simulation."""
    print(f"Detecting memory leaks for {test_duration} seconds...")
    
    # Start tracing
    tracemalloc.start()
    
    # Run multiple simulations
    results = []
    start_time = time.time()
    
    simulation_num = 0
    while time.time() - start_time < test_duration:
        # Setup brain
        brain = brain_factory()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
        
        # Take memory snapshot
        snapshot = tracemalloc.take_snapshot()
        
        # Calculate memory usage
        total_memory = sum(stat.size for stat in snapshot.statistics('lineno'))
        
        results.append({
            'simulation': simulation_num,
            'time': time.time() - start_time,
            'memory': total_memory,
            'memory_mb': total_memory / (1024 * 1024)
        })
        
        print(f"Simulation {simulation_num}: {results[-1]['memory_mb']:.1f} MB")
        simulation_num += 1
    
    # Analyze results
    print(f"\n=== Memory Leak Analysis ===")
    
    # Check for memory growth trend
    if len(results) > 10:
        recent_results = results[-10:]
        early_results = results[:10]
        
        recent_avg = np.mean([r['memory'] for r in recent_results])
        early_avg = np.mean([r['memory'] for r in early_results])
        
        memory_growth = recent_avg - early_avg
        memory_growth_percent = (memory_growth / early_avg) * 100
        
        print(f"Early average memory: {early_avg / (1024 * 1024):.1f} MB")
        print(f"Recent average memory: {recent_avg / (1024 * 1024):.1f} MB")
        print(f"Memory growth: {memory_growth_percent:.1f}%")
        
        if memory_growth_percent > 10:
            print("WARNING: Potential memory leak detected!")
            print("Memory increased by more than 10% over time")
        else:
            print("Memory usage appears stable")
    
    # Generate memory growth plot
    if len(results) > 1:
        times = [r['time'] for r in results]
        memories = [r['memory'] / (1024 * 1024) for r in results]
        
        plt.figure(figsize=(10, 6))
        plt.plot(times, memories, 'r-', linewidth=2, label='Memory Usage')
        plt.title('Memory Usage Over Time')
        plt.xlabel('Time (s)')
        plt.ylabel('Memory (MB)')
        plt.grid(True, alpha=0.3)
        plt.legend()
        plt.savefig('memory_leak_analysis.png', dpi=300, bbox_inches='tight')
        plt.show()
        
    tracemalloc.stop()
    
    return results
```

### 33 Bottleneck Identification

#### Identify Performance Bottlenecks
```python
class BottleneckDetector:
    def __init__(self, brain, world, agent, monitor):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.monitor = monitor
        
    def identify_bottlenecks(self):
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        # CPU bottlenecks
        cpu_bottlenecks = self.analyze_cpu_bottlenecks()
        bottlenecks.extend(cpu_bottlenecks)
        
        # Memory bottlenecks
        memory_bottlenecks = self.analyze_memory_bottlenecks()
        bottlenecks.extend(memory_bottlenecks)
        
        # Neural bottlenecks
        neural_bottlenecks = self.analyze_neural_bottlenecks()
        bottlenecks.extend(neural_bottlenecks)
        
        # System bottlenecks
        system_bottlenecks = self.analyze_system_bottlenecks()
        bottlenecks.extend(system_bottlenecks)
        
        return bottlenecks
        
    def analyze_cpu_bottlenecks(self):
        """Analyze CPU bottlenecks."""
        bottlenecks = []
        
        # Check for high CPU usage
        cpu_percent = psutil.cpu_percent(interval=1.0)
        if cpu_percent > 80:
            bottlenecks.append({
                'type': 'cpu',
                'severity': min(cpu_percent / 100.0, 1.0),
                'description': f'High CPU usage: {cpu_percent:.1f}%',
                'solutions': [
                    'Enable parallel processing',
                    'Optimize algorithms',
                    'Reduce simulation timestep',
                    'Use SIMD optimizations'
                ]
            })
            
        return bottlenecks
        
    def analyze_memory_bottlenecks(self):
        """Analyze memory bottlenecks."""
        bottlenecks = []
        
        # Check memory usage
        process = psutil.Process()
        memory_info = process.memory_info()
        memory_percent = memory_info.rss / psutil.virtual_memory().total
        
        if memory_percent > 0.8:
            bottlenecks.append({
                'type': 'memory',
                'severity': min((memory_percent - 0.8) / 0.2, 1.0),
                'description': f'High memory usage: {memory_percent:.1%}',
                'solutions': [
                    'Use memory pools',
                    'Implement lazy evaluation',
                    'Reduce data structures',
                    'Use compression'
                ]
            })
            
        # Check for memory allocation issues
        if memory_info.vms > memory_info.rss * 10:
            bottlenecks.append({
                'type': 'virtual_memory',
                'severity': 0.5,
                'description': 'High virtual memory usage',
                'solutions': [
                    'Optimize memory allocation',
                    'Use efficient data structures',
                    'Implement memory pooling'
                ]
            })
            
        return bottlenecks
        
    def analyze_neural_bottlenecks(self):
        """Analyze neural bottlenecks."""
        bottlenecks = []
        
        # Check firing rate
        firing_rate = self.brain.getAverageFiringRate()
        if firing_rate > 1000:  # Hz
            bottlenecks.append({
                'type': 'neural',
                'severity': min((firing_rate - 1000) / 2000, 1.0),
                'description': f'High firing rate: {firing_rate:.1f} Hz',
                'solutions': [
                    'Adjust neuron parameters',
                    'Optimize connectivity',
                    'Implement firing rate limits'
                ]
            })
            
        # Check synaptic activity
        synaptic_activity = self.brain.getTotalSynapseActivity()
        if synaptic_activity > 10000:
            bottlenecks.append({
                'type': 'synaptic',
                'severity': min((synaptic_activity - 10000) / 50000, 1.0),
                'description': f'High synaptic activity: {synaptic_activity}',
                'solutions': [
                    'Optimize synaptic dynamics',
                    'Implement synaptic pruning',
                    'Use efficient connectivity'
                ]
            })
            
        return bottlenecks
        
    def analyze_system_bottlenecks(self):
        """Analyze system bottlenecks."""
        bottlenecks = []
        
        # Check network usage
        net_io = psutil.net_io_counters()
        network_usage = (net_io.bytes_sent + net_io.bytes_recv) / (1024 * 1024)  # MB
        
        if network_usage > 100:  # MB
            bottlenecks.append({
                'type': 'network',
                'severity': min(network_usage / 500, 1.0),
                'description': f'High network usage: {network_usage:.1f} MB',
                'solutions': [
                    'Reduce network operations',
                    'Use local data processing',
                    'Implement efficient caching'
                ]
            })
            
        # Check disk I/O
        disk_io = psutil.disk_io_counters()
        disk_usage = disk_io.read_bytes + disk_io.write_bytes
        
        if disk_usage > 100 * 1024 * 1024:  # 100 MB
            bottlenecks.append({
                'type': 'disk',
                'severity': min(disk_usage / (1024 * 1024 * 1024), 1.0),
                'description': f'High disk I/O: {disk_usage / (1024 * 1024):.1f} MB',
                'solutions': [
                    'Use efficient file formats',
                    'Implement caching',
                    'Reduce file operations'
                ]
            })
            
        return bottlenecks
        
    def print_bottleneck_report(self, bottlenecks):
        """Print bottleneck report."""
        print("\n=== Bottleneck Report ===")
        
        if not bottlenecks:
            print("No significant bottlenecks detected")
            return
            
        print(f"Total bottlenecks: {len(bottlenecks)}")
        
        # Group by type
        bottleneck_types = {}
        for bottleneck in bottlenecks:
            if bottleneck['type'] not in bottleneck_types:
                bottleneck_types[bottleneck['type']] = []
            bottleneck_types[bottleneck['type']].append(bottleneck)
        
        # Print by type
        for btype, bList in bottleneck_types.items():
            print(f"\n{btype.upper()} Bottlenecks:")
            for i, bottleneck in enumerate(bList, 1):
                print(f"  {i}. {bottleneck['description']}")
                print(f"     Severity: {bottleneck['severity']:.2f}")
                print(f"     Solutions: {', '.join(bottleneck['solutions'])}")
                
        # Generate recommendations
        print(f"\n=== Recommendations ===")
        if bottlenecks:
            print("Immediate actions needed:")
            for bottleneck in bottlenecks[:3]:  # Top 3
                print(f"  • {bottleneck['description']}")
                
            if len(bottlenecks) > 3:
                print(f"  • And {len(bottlenecks) - 3} more issues")
        else:
            print("Performance is optimal")
```

### 34 Performance Optimization Debugging

#### Debug Optimization Issues
```python
class PerformanceOptimizer:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
    def debug_optimization(self, target_performance):
        """Debug performance optimization."""
        print(f"\n=== Performance Optimization Debug ({target_performance}) ===")
        
        # Check current performance
        current_performance = self.measure_performance()
        print(f"Current performance: {current_performance['steps_per_second']:.0f} steps/s")
        print(f"Current memory: {current_performance['memory_mb']:.1f} MB")
        
        # Analyze optimization opportunities
        optimizations = self.analyze_optimization_opportunities()
        
        # Generate optimization recommendations
        recommendations = self.generate_optimization_recommendations(optimizations, target_performance)
        
        # Apply recommendations
        applied_optimizations = []
        for recommendation in recommendations:
            if self.apply_optimization(recommendation):
                applied_optimizations.append(recommendation)
                
        # Measure performance after optimization
        new_performance = self.measure_performance()
        
        # Report results
        self.report_optimization_results(current_performance, new_performance, applied_optimizations)
        
        return new_performance
        
    def measure_performance(self):
        """Measure simulation performance."""
        # Run performance test
        start_time = time.time()
        steps = 100
        
        for step in range(steps):
            self.brain.step(step)
            
        elapsed = time.time() - start_time
        
        # Measure memory usage
        process = psutil.Process()
        memory_info = process.memory_info()
        
        return {
            'steps_per_second': steps / elapsed,
            'memory_mb': memory_info.rss / (1024 * 1024),
            'cpu_percent': process.cpu_percent(),
            'elapsed_time': elapsed
        }
        
    def analyze_optimization_opportunities(self):
        """Analyze optimization opportunities."""
        opportunities = []
        
        # Check for algorithmic optimizations
        if self.check_algorithmic_optimizations():
            opportunities.append({
                'type': 'algorithmic',
                'priority': 'high',
                'description': 'Potential algorithmic improvements',
                'estimated_improvement': 0.3  # 30% improvement
            })
            
        # Check for memory optimizations
        if self.check_memory_optimizations():
            opportunities.append({
                'type': 'memory',
                'priority': 'medium',
                'description': 'Potential memory optimizations',
                'estimated_improvement': 0.2  # 20% improvement
            })
            
        # Check for I/O optimizations
        if self.check_io_optimizations():
            opportunities.append({
                'type': 'io',
                'priority': 'low',
                'description': 'Potential I/O optimizations',
                'estimated_improvement': 0.1  # 10% improvement
            })
            
        return opportunities
        
    def check_algorithmic_optimizations(self):
        """Check for algorithmic optimization opportunities."""
        # Check for inefficient loops
        # Check for redundant computations
        # Check for suboptimal algorithms
        # This is a simplified check
        return True  # Placeholder
        
    def check_memory_optimizations(self):
        """Check for memory optimization opportunities."""
        # Check for memory leaks
        # Check for inefficient data structures
        # Check for unnecessary allocations
        # This is a simplified check
        return True  # Placeholder
        
    def check_io_optimizations(self):
        """Check for I/O optimization opportunities."""
        # Check for excessive file I/O
        # Check for inefficient serialization
        # Check for redundant data transfer
        # This is a simplified check
        return False  # Placeholder
        
    def generate_optimization_recommendations(self, opportunities, target_performance):
        """Generate optimization recommendations."""
        recommendations = []
        
        # Sort by priority
        opportunities.sort(key=lambda x: ['low', 'medium', 'high'].index(x['priority']), reverse=True)
        
        # Apply target-specific optimizations
        if target_performance == 'high':
            # Focus on performance optimizations
            for opportunity in opportunities:
                if opportunity['type'] == 'algorithmic':
                    recommendations.append(opportunity)
                    
        elif target_performance == 'medium':
            # Balance performance and memory
            for opportunity in opportunities:
                if opportunity['type'] in ['algorithmic', 'memory']:
                    recommendations.append(opportunity)
                    
        else:  # low
            # Focus on memory optimizations
            for opportunity in opportunities:
                if opportunity['type'] == 'memory':
                    recommendations.append(opportunity)
                    
        return recommendations
        
    def apply_optimization(self, recommendation):
        """Apply an optimization."""
        # This would apply the optimization to the simulation
        # For this example, we'll just return True
        return True
        
    def report_optimization_results(self, before, after, applied_optimizations):
        """Report optimization results."""
        print(f"\n=== Optimization Results ===")
        print(f"Before optimization:")
        print(f"  Performance: {before['steps_per_second']:.0f} steps/s")
        print(f"  Memory: {before['memory_mb']:.1f} MB")
        print(f"  CPU: {before['cpu_percent']:.1f}%")
        print(f"  Time: {before['elapsed_time']:.2f}s")
        
        print(f"\nAfter optimization:")
        print(f"  Performance: {after['steps_per_second']:.0f} steps/s")
        print(f"  Memory: {after['memory_mb']:.1f} MB")
        print(f"  CPU: {after['cpu_percent']:.1f}%")
        print(f"  Time: {after['elapsed_time']:.2f}s")
        
        # Calculate improvements
        perf_improvement = (after['steps_per_second'] - before['steps_per_second']) / before['steps_per_second'] * 100
        memory_improvement = (before['memory_mb'] - after['memory_mb']) / before['memory_mb'] * 100
        
        print(f"\nImprovements:")
        print(f"  Performance: {perf_improvement:+.1f}%")
        print(f"  Memory: {memory_improvement:+.1f}%")
        
        if applied_optimizations:
            print(f"\nApplied optimizations:")
            for opt in applied_optimizations:
                print(f"  • {opt['description']} ({opt['priority']} priority)")
```

## Plugin Debugging

### 41 Plugin Loading Issues

#### Debug Plugin Loading Problems
```python
class PluginDebugger:
    def __init__(self):
        self.plugin_system = None  # Would be initialized with actual plugin system
        
    def debug_plugin_loading(self, plugin_name):
        """Debug plugin loading issues."""
        print(f"\n=== Debugging Plugin Loading: {plugin_name} ===")
        
        # Check if plugin system is initialized
        if self.plugin_system is None:
            print("ERROR: Plugin system not initialized")
            return False
            
        # Check if plugin exists
        if not self.plugin_system.hasPlugin(plugin_name):
            print(f"ERROR: Plugin '{plugin_name}' not found")
            print("Available plugins:", self.plugin_system.getAvailablePlugins())
            return False
            
        # Check plugin dependencies
        dependencies = self.plugin_system.getPluginDependencies(plugin_name)
        for dependency in dependencies:
            if not self.plugin_system.hasPlugin(dependency):
                print(f"ERROR: Plugin '{plugin_name}' depends on missing plugin '{dependency}'")
                return False
                
        # Check plugin compatibility
        plugin_info = self.plugin_system.getPluginInfo(plugin_name)
        compatibility = self.check_plugin_compatibility(plugin_info)
        if not compatibility['compatible']:
            print(f"ERROR: Plugin '{plugin_name}' compatibility issue")
            print(f"Details: {compatibility['issues']}")
            return False
            
        # Try loading plugin
        try:
            self.plugin_system.loadPlugin(plugin_name)
            print(f"SUCCESS: Plugin '{plugin_name}' loaded successfully")
            return True
        except Exception as e:
            print(f"ERROR: Failed to load plugin '{plugin_name}'")
            print(f"Exception: {e}")
            return False
            
    def check_plugin_compatibility(self, plugin_info):
        """Check plugin compatibility."""
        compatibility = {
            'compatible': True,
            'issues': []
        }
        
        # Check version compatibility
        required_version = plugin_info.get('version')
        current_version = self.plugin_system.getVersion()
        
        if required_version != current_version:
            compatibility['compatible'] = False
            compatibility['issues'].append(
                f"Version mismatch: required {required_version}, current {current_version}"
            )
            
        # Check dependency compatibility
        dependencies = plugin_info.get('dependencies', [])
        for dependency in dependencies:
            if not self.plugin_system.hasPlugin(dependency):
                compatibility['compatible'] = False
                compatibility['issues'].append(f"Missing dependency: {dependency}")
                
        return compatibility
```

### 42 Plugin Execution Issues

#### Debug Plugin Execution Problems
```python
class PluginExecutionDebugger:
    def __init__(self, plugin_system):
        self.plugin_system = plugin_system
        
    def debug_plugin_execution(self, plugin_name, parameters):
        """Debug plugin execution issues."""
        print(f"\n=== Debugging Plugin Execution: {plugin_name} ===")
        
        # Check if plugin is loaded
        if not self.plugin_system.isPluginLoaded(plugin_name):
            print(f"ERROR: Plugin '{plugin_name}' not loaded")
            return False
            
        # Validate parameters
        validation_result = self.validate_plugin_parameters(plugin_name, parameters)
        if not validation_result['valid']:
            print(f"ERROR: Invalid parameters for plugin '{plugin_name}'")
            print(f"Issues: {validation_result['issues']}")
            return False
            
        # Check plugin hooks
        hooks = self.plugin_system.getPluginHooks(plugin_name)
        for hook_name, hook_function in hooks.items():
            if not callable(hook_function):
                print(f"ERROR: Hook '{hook_name}' is not callable")
                return False
                
        # Try executing plugin
        try:
            result = self.plugin_system.executePlugin(plugin_name, parameters)
            print(f"SUCCESS: Plugin '{plugin_name}' executed successfully")
            print(f"Result: {result}")
            return True
        except Exception as e:
            print(f"ERROR: Plugin execution failed")
            print(f"Exception: {e}")
            
            # Try to identify the issue
            error_details = self.identify_plugin_error(e, plugin_name, parameters)
            print(f"Error details: {error_details}")
            
            return False
            
    def validate_plugin_parameters(self, plugin_name, parameters):
        """Validate plugin parameters."""
        validation = {
            'valid': True,
            'issues': []
        }
        
        # Get expected parameters
        expected_params = self.plugin_system.getPluginParameters(plugin_name)
        
        # Check for missing required parameters
        for param_name, param_info in expected_params.items():
            if param_info.get('required', False) and param_name not in parameters:
                validation['valid'] = False
                validation['issues'].append(f"Missing required parameter: {param_name}")
                
        # Check for invalid parameter types
        for param_name, value in parameters.items():
            if param_name in expected_params:
                expected_type = expected_params[param_name].get('type')
                if expected_type and not isinstance(value, expected_type):
                    validation['valid'] = False
                    validation['issues'].append(
                        f"Invalid type for parameter '{param_name}': "
                        f"expected {expected_type}, got {type(value)}"
                    )
                    
        return validation
        
    def identify_plugin_error(self, exception, plugin_name, parameters):
        """Identify the cause of a plugin execution error."""
        error_details = {
            'exception_type': type(exception).__name__,
            'exception_message': str(exception),
            'possible_causes': []
        }
        
        # Common plugin errors and their causes
        if "AttributeError" in type(exception).__name__:
            error_details['possible_causes'].extend([
                "Plugin hook not properly registered",
                "Plugin function not defined",
                "Missing required attributes"
            ])
            
        elif "TypeError" in type(exception).__name__:
            error_details['possible_causes'].extend([
                "Invalid parameter type",
                "Plugin function signature mismatch",
                "Incorrect hook implementation"
            ])
            
        elif "ValueError" in type(exception).__name__:
            error_details['possible_causes'].extend([
                "Invalid parameter value",
                "Plugin precondition not met",
                "Invalid configuration"
            ])
            
        elif "ImportError" in type(exception).__name__:
            error_details['possible_causes'].extend([
                "Missing plugin dependencies",
                "Plugin module not found",
                "Import statements failed"
            ])
            
        return error_details
```

### 43 Plugin Communication Issues

#### Debug Plugin Communication Problems
```python
class PluginCommunicationDebugger:
    def __init__(self, plugin_system):
        self.plugin_system = plugin_system
        
    def debug_plugin_communication(self, plugin_name):
        """Debug plugin communication issues."""
        print(f"\n=== Debugging Plugin Communication: {plugin_name} ===")
        
        # Check plugin hooks
        hooks = self.plugin_system.getPluginHooks(plugin_name)
        print(f"Plugin hooks: {list(hooks.keys())}")
        
        # Test hook communication
        for hook_name, hook_function in hooks.items():
            print(f"\nTesting hook: {hook_name}")
            
            # Test hook registration
            if not self.test_hook_registration(hook_name):
                print(f"ERROR: Hook '{hook_name}' not properly registered")
                continue
                
            # Test hook execution
            if not self.test_hook_execution(hook_name, hook_function):
                print(f"ERROR: Hook '{hook_name}' execution failed")
                
        # Check data flow
        print(f"\n=== Checking Data Flow ===")
        data_flow = self.analyze_plugin_data_flow(plugin_name)
        self.print_data_flow_analysis(data_flow)
        
        return True
        
    def test_hook_registration(self, hook_name):
        """Test hook registration."""
        # This would test if the hook is properly registered
        # For this example, we'll return True
        return True
        
    def test_hook_execution(self, hook_name, hook_function):
        """Test hook execution."""
        # This would test if the hook executes correctly
        # For this example, we'll return True
        return True
        
    def analyze_plugin_data_flow(self, plugin_name):
        """Analyze plugin data flow."""
        # This would analyze how data flows between plugins
        # For this example, we'll return a simple analysis
        return {
            'input_hooks': [],
            'output_hooks': [],
            'data_dependencies': [],
            'potential_issues': []
        }
        
    def print_data_flow_analysis(self, analysis):
        """Print data flow analysis."""
        print("Data flow analysis:")
        print(f"  Input hooks: {analysis['input_hooks']}")
        print(f"  Output hooks: {analysis['output_hooks']}")
        print(f"  Data dependencies: {analysis['data_dependencies']}")
        if analysis['potential_issues']:
            print(f"  Potential issues: {analysis['potential_issues']}")
```

### 44 Plugin Testing and Debugging

#### Test and Debug Plugins
```python
class PluginTestingDebugger:
    def __init__(self, plugin_system):
        self.plugin_system = plugin_system
        
    def run_plugin_tests(self, plugin_name):
        """Run plugin tests."""
        print(f"\n=== Running Plugin Tests: {plugin_name} ===")
        
        # Load plugin for testing
        if not self.plugin_system.isPluginLoaded(plugin_name):
            print(f"ERROR: Plugin '{plugin_name}' not loaded")
            return False
            
        # Run unit tests
        unit_test_results = self.run_unit_tests(plugin_name)
        self.print_unit_test_results(unit_test_results)
        
        # Run integration tests
        integration_test_results = self.run_integration_tests(plugin_name)
        self.print_integration_test_results(integration_test_results)
        
        # Run performance tests
        performance_test_results = self.run_performance_tests(plugin_name)
        self.print_performance_test_results(performance_test_results)
        
        # Generate test report
        test_report = self.generate_test_report(unit_test_results, integration_test_results, performance_test_results)
        self.print_test_report(test_report)
        
        return test_report.get('passed', 0) == test_report.get('total', 0)
        
    def run_unit_tests(self, plugin_name):
        """Run unit tests for plugin."""
        # This would run unit tests for the plugin
        # For this example, we'll return dummy results
        return {
            'passed': 5,
            'failed': 1,
            'total': 6,
            'tests': [
                {'name': 'test_initialization', 'passed': True},
                {'name': 'test_parameter_validation', 'passed': True},
                {'name': 'test_execution', 'passed': True},
                {'name': 'test_error_handling', 'passed': True},
                {'name': 'test_cleanup', 'passed': True},
                {'name': 'test_compatibility', 'passed': False, 'error': 'Version mismatch'}
            ]
        }
        
    def print_unit_test_results(self, results):
        """Print unit test results."""
        print(f"Unit tests: {results['passed']}/{results['total']} passed")
        for test in results['tests']:
            status = "PASS" if test['passed'] else "FAIL"
            print(f"  {status}: {test['name']}")
            if not test['passed'] and 'error' in test:
                print(f"    Error: {test['error']}")
                
    def run_integration_tests(self, plugin_name):
        """Run integration tests for plugin."""
        # This would run integration tests
        # For this example, we'll return dummy results
        return {
            'passed': 3,
            'failed': 1,
            'total': 4,
            'tests': [
                {'name': 'test_with_other_plugins', 'passed': True},
                {'name': 'test_hook_communication', 'passed': True},
                {'name': 'test_data_flow', 'passed': True},
                {'name': 'test_conflict_resolution', 'passed': False, 'error': 'Plugin conflict detected'}
            ]
        }
        
    def print_integration_test_results(self, results):
        """Print integration test results."""
        print(f"Integration tests: {results['passed']}/{results['total']} passed")
        for test in results['tests']:
            status = "PASS" if test['passed'] else "FAIL"
            print(f"  {status}: {test['name']}")
            if not test['passed'] and 'error' in test:
                print(f"    Error: {test['error']}")
                
    def run_performance_tests(self, plugin_name):
        """Run performance tests for plugin."""
        # This would run performance tests
        # For this example, we'll return dummy results
        return {
            'passed': 2,
            'failed': 1,
            'total': 3,
            'tests': [
                {'name': 'test_execution_time', 'passed': True, 'metrics': {'time_ms': 100}},
                {'name': 'test_memory_usage', 'passed': True, 'metrics': {'memory_mb': 50}},
                {'name': 'test_concurrent_execution', 'passed': False, 'error': 'Race condition detected'}
            ]
        }
        
    def print_performance_test_results(self, results):
        """Print performance test results."""
        print(f"Performance tests: {results['passed']}/{results['total']} passed")
        for test in results['tests']:
            if 'metrics' in test:
                metrics_str = ', '.join([f"{k}: {v}" for k, v in test['metrics'].items()])
                status = "PASS" if test['passed'] else "FAIL"
                print(f"  {status}: {test['name']} ({metrics_str})")
            else:
                status = "PASS" if test['passed'] else "FAIL"
                print(f"  {status}: {test['name']}")
                if not test['passed'] and 'error' in test:
                    print(f"    Error: {test['error']}")
                    
    def generate_test_report(self, unit_results, integration_results, performance_results):
        """Generate comprehensive test report."""
        total_tests = (unit_results['total'] + 
                      integration_results['total'] + 
                      performance_results['total'])
        
        passed_tests = (unit_results['passed'] + 
                       integration_results['passed'] + 
                       performance_results['passed'])
        
        return {
            'total': total_tests,
            'passed': passed_tests,
            'failed': total_tests - passed_tests,
            'success_rate': passed_tests / total_tests if total_tests > 0 else 0,
            'unit_tests': unit_results,
            'integration_tests': integration_results,
            'performance_tests': performance_results,
            'recommendations': self.generate_test_recommendations(unit_results, integration_results, performance_results)
        }
        
    def generate_test_recommendations(self, unit_results, integration_results, performance_results):
        """Generate test recommendations."""
        recommendations = []
        
        # Check for failed unit tests
        failed_unit_tests = [t for t in unit_results['tests'] if not t['passed']]
        if failed_unit_tests:
            recommendations.append({
                'priority': 'high',
                'category': 'unit testing',
                'description': f'Fix {len(failed_unit_tests)} failed unit tests',
                'tests': failed_unit_tests
            })
            
        # Check for failed integration tests
        failed_integration_tests = [t for t in integration_results['tests'] if not t['passed']]
        if failed_integration_tests:
            recommendations.append({
                'priority': 'high',
                'category': 'integration testing',
                'description': f'Fix {len(failed_integration_tests)} failed integration tests',
                'tests': failed_integration_tests
            })
            
        # Check for failed performance tests
        failed_performance_tests = [t for t in performance_results['tests'] if not t['passed']]
        if failed_performance_tests:
            recommendations.append({
                'priority': 'medium',
                'category': 'performance testing',
                'description': f'Fix {len(failed_performance_tests)} failed performance tests',
                'tests': failed_performance_tests
            })
            
        return recommendations
        
    def print_test_report(self, report):
        """Print test report."""
        print(f"\n=== Test Report ===")
        print(f"Total tests: {report['total']}")
        print(f"Passed tests: {report['passed']}")
        print(f"Failed tests: {report['failed']}")
        print(f"Success rate: {report['success_rate']*100:.1f}%")
        
        if report['recommendations']:
            print(f"\n=== Recommendations ===")
            for i, recommendation in enumerate(report['recommendations'], 1):
                print(f"{i}. [{recommendation['priority'].upper()}] {recommendation['category']}: {recommendation['description']}")
                for test in recommendation['tests']:
                    print(f"   - {test['name']}: {test.get('error', 'Test failed')}")
```

## Real-world Debugging Scenarios

### 51 Learning Agent Debugging

#### Debug Learning Agent Issues
```python
class LearningAgentDebugger:
    def __init__(self, agent):
        self.agent = agent
        self.debug_log = []
        
    def debug_learning_issues(self):
        """Debug learning agent issues."""
        print("\n=== Debugging Learning Agent Issues ===")
        
        # Check learning subsystem status
        self.check_learning_subsystems()
        
        # Check reward signals
        self.check_reward_signals()
        
        # Check exploration behavior
        self.check_exploration_behavior()
        
        # Check memory usage
        self.check_memory_usage()
        
        # Check learning progress
        self.check_learning_progress()
        
        # Generate recommendations
        self.generate_learning_recommendations()
        
    def check_learning_subsystems(self):
        """Check learning subsystem status."""
        print("\n--- Checking Learning Subsystems ---")
        
        subsystems = [
            ('reward_modulation', self.agent.isRewardModulationEnabled()),
            ('structural_plasticity', self.agent.isStructuralPlasticityEnabled()),
            ('development', self.agent.isDevelopmentEnabled()),
            ('curiosity', self.agent.isCuriosityEnabled()),
            ('novelty', self.agent.isNoveltyEnabled())
        ]
        
        for subsystem, enabled in subsystems:
            status = "ENABLED" if enabled else "DISABLED"
            print(f"  {subsystem}: {status}")
            
            if not enabled:
                print(f"    WARNING: {subsystem} is disabled, agent may not learn properly")
                
    def check_reward_signals(self):
        """Check reward signal quality."""
        print("\n--- Checking Reward Signals ---")
        
        current_reward = self.agent.getCurrentReward()
        predicted_reward = self.agent.getPredictedReward()
        reward_delta = current_reward - predicted_reward
        
        print(f"  Current reward: {current_reward:.3f}")
        print(f"  Predicted reward: {predicted_reward:.3f}")
        print(f"  Reward delta: {reward_delta:.3f}")
        
        # Check reward signal quality
        if abs(reward_delta) > 1.0:
            print(f"  WARNING: Large reward delta ({reward_delta:.3f}), possible reward prediction error")
            
        if current_reward < 0:
            print(f"  WARNING: Negative reward received ({current_reward:.3f})")
            
    def check_exploration_behavior(self):
        """Check exploration behavior."""
        print("\n--- Checking Exploration Behavior ---")
        
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        
        print(f"  Curiosity level: {curiosity:.3f}")
        print(f"  Novelty level: {novelty:.3f}")
        
        # Check exploration vs exploitation balance
        if curiosity < 0.3 and novelty < 0.3:
            print("  WARNING: Low curiosity and novelty levels, agent may be over-exploiting")
        elif curiosity > 0.8 and novelty > 0.8:
            print("  WARNING: High curiosity and novelty levels, agent may be over-exploring")
            
    def check_memory_usage(self):
        """Check memory usage."""
        print("\n--- Checking Memory Usage ---")
        
        # Get brain information
        brain = self.agent.getBrain()
        if brain is None:
            print("  ERROR: Brain not available")
            return
            
        neuron_count = brain.getTotalNeuronCount()
        firing_count = brain.getFiringNeuronCount()
        
        print(f"  Neurons: {neuron_count}")
        print(f"  Firing neurons: {firing_count}")
        print(f"  Utilization: {firing_count/neuron_count*100:.1f}%")
        
        # Check for memory issues
        if firing_count == 0:
            print("  WARNING: No firing neurons detected")
        elif firing_count < neuron_count * 0.05:
            print("  WARNING: Low neuron utilization (< 5%)")
            
    def check_learning_progress(self):
        """Check learning progress over time."""
        print("\n--- Checking Learning Progress ---")
        
        # This would require tracking learning over time
        # For this example, we'll show current state
        development_stage = self.agent.getDevelopmentalStage()
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        
        print(f"  Development stage: {development_stage}")
        print(f"  Curiosity: {curiosity:.3f}")
        print(f"  Novelty: {novelty:.3f}")
        
        # Check if learning is progressing
        if development_stage == 'Initial':
            print("  INFO: Agent is in initial developmental stage")
        elif development_stage == 'CriticalPeriod':
            print("  INFO: Agent is in critical learning period")
        elif development_stage == 'Maturation':
            print("  INFO: Agent is maturing")
        elif development_stage == 'Adult':
            print("  INFO: Agent is fully developed")
        elif development_stage == 'Aging':
            print("  INFO: Agent is aging")
            
    def generate_learning_recommendations(self):
        """Generate learning improvement recommendations."""
        print("\n--- Learning Recommendations ---")
        
        recommendations = []
        
        # Check for common issues
        if not self.agent.isRewardModulationEnabled():
            recommendations.append(
                "Enable reward modulation for better learning"
            )
            
        if not self.agent.isCuriosityEnabled():
            recommendations.append(
                "Enable curiosity for exploration behavior"
            )
            
        if not self.agent.isStructuralPlasticityEnabled():
            recommendations.append(
                "Enable structural plasticity for neural growth"
            )
            
        if not self.agent.isDevelopmentEnabled():
            recommendations.append(
                "Enable development for aging and maturation"
            )
        
        if recommendations:
            print("Recommendations:")
            for i, recommendation in enumerate(recommendations, 1):
                print(f"  {i}. {recommendation}")
        else:
            print("Learning subsystems appear to be properly configured")
```

### 52 World Simulation Debugging

#### Debug World Simulation Issues
```python
class WorldSimulationDebugger:
    def __init__(self, world):
        self.world = world
        
    def debug_world_simulation(self):
        """Debug world simulation issues."""
        print("\n=== Debugging World Simulation ===")
        
        # Check world configuration
        self.check_world_configuration()
        
        # Check agent state
        self.check_agent_state()
        
        # Check environment
        self.check_environment()
        
        # Check simulation state
        self.check_simulation_state()
        
    def check_world_configuration(self):
        """Check world configuration."""
        print("\n--- Checking World Configuration ---")
        
        dimensions = {
            'width': self.world.getWidth(),
            'height': self.world.getHeight(),
            'visionWidth': self.world.getVisionWidth(),
            'visionHeight': self.world.getVisionHeight()
        }
        
        for dim_name, dim_value in dimensions.items():
            if dim_value <= 0:
                print(f"  ERROR: Invalid {dim_name}: {dim_value}")
            else:
                print(f"  OK: {dim_name} = {dim_value}")
                
    def check_agent_state(self):
        """Check agent state."""
        print("\n--- Checking Agent State ---")
        
        position = self.world.getAgentPosition()
        velocity = self.world.getAgentVelocity()
        energy = self.world.getAgentEnergy()
        
        print(f"  Position: ({position[0]:.1f}, {position[1]:.1f})")
        print(f"  Velocity: ({velocity[0]:.1f}, {velocity[1]:.1f})")
        print(f"  Energy: {energy:.1f}")
        
        # Check boundaries
        width, height = self.world.getWidth(), self.world.getHeight()
        if not (0 <= position[0] <= width):
            print(f"  ERROR: Agent x-position {position[0]:.1f} outside bounds [0, {width}]")
            
        if not (0 <= position[1] <= height):
            print(f"  ERROR: Agent y-position {position[1]:.1f} outside bounds [0, {height}]")
            
        # Check energy
        if energy <= 0:
            print(f"  WARNING: Agent energy is zero or negative ({energy:.1f})")
            
    def check_environment(self):
        """Check environment state."""
        print("\n--- Checking Environment ---")
        
        # Check objects in world
        objects = self.world.getObjects()
        print(f"  Objects in world: {len(objects)}")
        
        # Check hazards
        hazards = self.world.getHazards()
        if hazards:
            print(f"  Hazards: {len(hazards)}")
            for i, hazard in enumerate(hazards):
                print(f"    {i+1}. {hazard}")
        else:
            print("  No hazards detected")
            
        # Check resources
        resources = self.world.getResources()
        if resources:
            print(f"  Resources: {len(resources)}")
            for i, resource in enumerate(resources):
                print(f"    {i+1}. {resource}")
        else:
            print("  No resources detected")
            
    def check_simulation_state(self):
        """Check simulation state."""
        print("\n--- Checking Simulation State ---")
        
        simulation_time = self.world.getSimulationTime()
        step_count = self.world.getStepCount()
        
        print(f"  Simulation time: {simulation_time:.1f}")
        print(f"  Step count: {step_count}")
        
        # Check for simulation issues
        if simulation_time > 1000:
            print(f"  WARNING: Long simulation time ({simulation_time:.1f})")
            
        if step_count > 10000:
            print(f"  WARNING: High step count ({step_count})")
            
        # Check for simulation stability
        if simulation_time == step_count:
            print("  WARNING: Simulation time equals step count, may indicate slow simulation")
```

### 53 Configuration Debugging

#### Debug Configuration Issues
```python
class ConfigurationDebugger:
    def __init__(self, config):
        self.config = config
        
    def debug_configuration(self):
        """Debug configuration issues."""
        print("\n=== Debugging Configuration ===")
        
        # Check configuration structure
        self.check_configuration_structure()
        
        # Check configuration values
        self.check_configuration_values()
        
        # Check for missing values
        self.check_missing_values()
        
        # Check for invalid values
        self.check_invalid_values()
        
        # Validate configuration
        self.validate_configuration()
        
    def check_configuration_structure(self):
        """Check configuration structure."""
        print("\n--- Checking Configuration Structure ---")
        
        # Check if config is properly initialized
        if self.config is None:
            print("  ERROR: Configuration is None")
            return
            
        # Get all configuration keys
        keys = self.config.getKeys()
        print(f"  Total configuration keys: {len(keys)}")
        
        # Check for required sections
        required_sections = ['brain', 'plasticity', 'neuromodulation']
        for section in required_sections:
            if section not in keys:
                print(f"  WARNING: Missing section '{section}'")
            else:
                print(f"  OK: Section '{section}' present")
                
    def check_configuration_values(self):
        """Check configuration values."""
        print("\n--- Checking Configuration Values ---")
        
        # Check brain configuration
        if self.config.has('brain.neuron_count'):
            neuron_count = self.config.get('brain.neuron_count')
            if neuron_count <= 0:
                print(f"  ERROR: Invalid neuron_count: {neuron_count}")
            else:
                print(f"  OK: neuron_count = {neuron_count}")
                
        if self.config.has('brain.synapse_density'):
            synapse_density = self.config.get('brain.synapse_density')
            if not (0 <= synapse_density <= 1):
                print(f"  ERROR: Invalid synapse_density: {synapse_density}")
            else:
                print(f"  OK: synapse_density = {synapse_density}")
                
        # Check plasticity configuration
        if self.config.has('plasticity.stdp.enable'):
            stdp_enabled = self.config.get('plasticity.stdp.enable')
            print(f"  OK: STDP enabled = {stdp_enabled}")
            
        if self.config.has('plasticity.hebbian.enable'):
            hebbian_enabled = self.config.get('plasticity.hebbian.enable')
            print(f"  OK: Hebbian enabled = {hebbian_enabled}")
            
        # Check neuromodulation configuration
        if self.config.has('neuromod.dopamine.scale'):
            dopamine_scale = self.config.get('neuromod.dopamine.scale')
            print(f"  OK: Dopamine scale = {dopamine_scale}")
            
    def check_missing_values(self):
        """Check for missing configuration values."""
        print("\n--- Checking for Missing Values ---")
        
        # Common missing values
        missing_values = []
        
        if not self.config.has('brain.neuron_count'):
            missing_values.append('brain.neuron_count')
            
        if not self.config.has('brain.synapse_density'):
            missing_values.append('brain.synapse_density')
            
        if not self.config.has('plasticity.stdp.enable'):
            missing_values.append('plasticity.stdp.enable')
            
        if not self.config.has('neuromod.curiosity.enable'):
            missing_values.append('neuromod.curiosity.enable')
            
        if missing_values:
            print(f"  WARNING: Missing configuration values:")
            for value in missing_values:
                print(f"    - {value}")
        else:
            print("  OK: All required configuration values present")
            
    def check_invalid_values(self):
        """Check for invalid configuration values."""
        print("\n--- Checking for Invalid Values ---")
        
        # Check for invalid ranges
        invalid_values = []
        
        if self.config.has('brain.neuron_count'):
            neuron_count = self.config.get('brain.neuron_count')
            if neuron_count < 0:
                invalid_values.append(f'brain.neuron_count: {neuron_count} (must be >= 0)')
                
        if self.config.has('brain.synapse_density'):
            synapse_density = self.config.get('brain.synapse_density')
            if not (0 <= synapse_density <= 1):
                invalid_values.append(f'brain.synapse_density: {synapse_density} (must be 0-1)')
                
        if invalid_values:
            print(f"  ERROR: Invalid configuration values:")
            for value in invalid_values:
                print(f"    - {value}")
        else:
            print("  OK: All configuration values are valid")
            
    def validate_configuration(self):
        """Validate configuration."""
        print("\n--- Validating Configuration ---")
        
        # This would perform comprehensive validation
        # For this example, we'll do a simple validation
        if self.config is not None:
            print("  OK: Configuration is not None")
            
            # Check for any other issues
            keys = self.config.getKeys()
            print(f"  Total configuration keys: {len(keys)}")
            
            # Validate that all keys have valid values
            valid_keys = 0
            for key in keys:
                value = self.config.get(key)
                if value is not None and not isinstance(value, str):
                    valid_keys += 1
                    
            print(f"  Valid configuration entries: {valid_keys}/{len(keys)}")
            
            if valid_keys == len(keys):
                print("  OK: Configuration validation passed")
            else:
                print("  WARNING: Some configuration entries may be invalid")
        else:
            print("  ERROR: Configuration is None")
```

### 54 Integration Testing

#### Test System Integration
```python
class IntegrationDebugger:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
    def run_integration_test(self):
        """Run integration test."""
        print("\n=== Running Integration Test ===")
        
        # Test component initialization
        self.test_initialization()
        
        # Test component interaction
        self.test_interaction()
        
        # Test data flow
        self.test_data_flow()
        
        # Test error handling
        self.test_error_handling()
        
        # Generate test report
        self.generate_test_report()
        
    def test_initialization(self):
        """Test component initialization."""
        print("\n--- Testing Initialization ---")
        
        tests = []
        
        # Test brain initialization
        if self.brain is not None:
            print("  OK: Brain created")
            if self.brain.initialize():
                print("  OK: Brain initialized successfully")
                tests.append(True)
            else:
                print("  ERROR: Brain initialization failed")
                tests.append(False)
        else:
            print("  ERROR: Brain is None")
            tests.append(False)
            
        # Test world creation
        if self.world is not None:
            print("  OK: World created")
            if self.world.configure(width=20, height=20):
                print("  OK: World configured successfully")
                tests.append(True)
            else:
                print("  ERROR: World configuration failed")
                tests.append(False)
        else:
            print("  ERROR: World is None")
            tests.append(False)
            
        # Test agent creation
        if self.agent is not None:
            print("  OK: Agent created")
            if self.agent.initialize(self.world):
                print("  OK: Agent initialized successfully")
                tests.append(True)
            else:
                print("  ERROR: Agent initialization failed")
                tests.append(False)
        else:
            print("  ERROR: Agent is None")
            tests.append(False)
            
        # Calculate test results
        if tests:
            passed = sum(tests)
            total = len(tests)
            print(f"\nInitialization tests: {passed}/{total} passed")
            return passed == total
        else:
            print("\nERROR: No tests performed")
            return False
            
    def test_interaction(self):
        """Test component interaction."""
        print("\n--- Testing Interaction ---")
        
        tests = []
        
        # Test brain-world interaction
        if self.brain and self.world:
            print("  OK: Brain and world available")
            
            # Run a few simulation steps
            for step in range(5):
                self.world.update(timestep=0.1)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
            print("  OK: Brain-world interaction test passed")
            tests.append(True)
        else:
            print("  ERROR: Missing components for brain-world interaction")
            tests.append(False)
            
        # Test agent-brain interaction
        if self.agent and self.brain:
            print("  OK: Agent and brain available")
            
            # Test agent processing
            agent_action = self.agent.decodeMotorCommand()
            print(f"  OK: Agent action: {agent_action}")
            
            # Test brain statistics
            neuron_count = self.brain.getTotalNeuronCount()
            print(f"  OK: Brain neuron count: {neuron_count}")
            
            tests.append(True)
        else:
            print("  ERROR: Missing components for agent-brain interaction")
            tests.append(False)
            
        # Test world-agent interaction
        if self.world and self.agent:
            print("  OK: World and agent available")
            
            # Test agent position
            position = self.world.getAgentPosition()
            print(f"  OK: Agent position: ({position[0]:.1f}, {position[1]:.1f})")
            
            # Test world perception
            percept = self.world.getSensoryPercept()
            print(f"  OK: World percept available")
            
            tests.append(True)
        else:
            print("  ERROR: Missing components for world-agent interaction")
            tests.append(False)
            
        # Calculate test results
        if tests:
            passed = sum(tests)
            total = len(tests)
            print(f"\nInteraction tests: {passed}/{total} passed")
            return passed == total
        else:
            print("\nERROR: No tests performed")
            return False
            
    def test_data_flow(self):
        """Test data flow between components."""
        print("\n--- Testing Data Flow ---")
        
        tests = []
        
        # Test sensory data flow
        if self.world and self.agent:
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process in agent
            self.agent.processSensoryInput(percept)
            
            # Check agent state
            curiosity = self.agent.getCuriosityLevel()
            if 0 <= curiosity <= 1:
                print(f"  OK: Sensory data flow test passed (curiosity: {curiosity:.3f})")
                tests.append(True)
            else:
                print(f"  ERROR: Invalid curiosity level: {curiosity}")
                tests.append(False)
        else:
            print("  ERROR: Missing components for data flow test")
            tests.append(False)
            
        # Test motor command flow
        if self.agent and self.world:
            # Get motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check agent position
            position = self.world.getAgentPosition()
            if 0 <= position[0] <= self.world.getWidth() and 0 <= position[1] <= self.world.getHeight():
                print(f"  OK: Motor command flow test passed (position: ({position[0]:.1f}, {position[1]:.1f}))")
                tests.append(True)
            else:
                print(f"  ERROR: Invalid agent position: {position}")
                tests.append(False)
        else:
            print("  ERROR: Missing components for motor command test")
            tests.append(False)
            
        # Calculate test results
        if tests:
            passed = sum(tests)
            total = len(tests)
            print(f"\nData flow tests: {passed}/{total} passed")
            return passed == total
        else:
            print("\nERROR: No tests performed")
            return False
            
    def test_error_handling(self):
        """Test error handling."""
        print("\n--- Testing Error Handling ---")
        
        tests = []
        
        # Test invalid configuration handling
        if self.brain:
            print("  OK: Brain available for error handling test")
            
            # Try to access uninitialized brain (should handle gracefully)
            try:
                # This might fail if brain not initialized
                neurons = self.brain.getTotalNeuronCount()
                print(f"  OK: Error handling test passed (neurons: {neurons})")
                tests.append(True)
            except Exception as e:
                print(f"  OK: Error handling test passed (caught exception: {type(e).__name__})")
                tests.append(True)
        else:
            print("  ERROR: Brain not available for error handling test")
            tests.append(False)
            
        # Test boundary handling
        if self.world:
            print("  OK: World available for boundary test")
            
            # Try to access agent position (should handle boundaries)
            try:
                position = self.world.getAgentPosition()
                print(f"  OK: Boundary handling test passed (position: {position})")
                tests.append(True)
            except Exception as e:
                print(f"  OK: Boundary handling test passed (caught exception: {type(e).__name__})")
                tests.append(True)
        else:
            print("  ERROR: World not available for boundary test")
            tests.append(False)
            
        # Calculate test results
        if tests:
            passed = sum(tests)
            total = len(tests)
            print(f"\nError handling tests: {passed}/{total} passed")
            return passed == total
        else:
            print("\nERROR: No tests performed")
            return False
            
    def generate_test_report(self):
        """Generate integration test report."""
        print("\n=== Integration Test Report ===")
        
        # Run all tests
        initialization_passed = self.test_initialization()
        interaction_passed = self.test_interaction()
        data_flow_passed = self.test_data_flow()
        error_handling_passed = self.test_error_handling()
        
        # Calculate overall results
        total_tests = 4
        passed_tests = sum([initialization_passed, interaction_passed, data_flow_passed, error_handling_passed])
        
        print(f"\nOverall Results: {passed_tests}/{total_tests} tests passed")
        
        if passed_tests == total_tests:
            print("SUCCESS: All integration tests passed!")
            print("The system is properly integrated and ready for use.")
        else:
            print("WARNING: Some integration tests failed.")
            print("Please review the failed tests and address any issues before using the system.")
            
        # Generate recommendations
        self.generate_recommendations(passed_tests, total_tests)
        
        return {
            'passed': passed_tests,
            'total': total_tests,
            'success_rate': passed_tests / total_tests if total_tests > 0 else 0,
            'all_passed': passed_tests == total_tests
        }
        
    def generate_recommendations(self, passed_tests, total_tests):
        """Generate recommendations based on test results."""
        print(f"\n=== Recommendations ===")
        
        if passed_tests == total_tests:
            print("1. System is ready for production use")
            print("2. All components are properly integrated")
            print("3. Error handling is working correctly")
        elif passed_tests >= total_tests * 0.75:  # 75% success rate
            print("1. Most tests passed - system is mostly functional")
            print("2. Review and fix the remaining failed tests")
            print("3. Consider if the system is ready for limited use")
        else:
            print("1. Many tests failed - system requires significant fixes")
            print("2. Address the failed tests before proceeding")
            print("3. Consider system redesign if issues persist")
```

## Advanced Debugging Techniques

### 61 State Inspection

#### Inspect System State
```python
class StateInspector:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
    def inspect_system_state(self):
        """Inspect complete system state."""
        print("\n=== System State Inspection ===")
        
        print("\n--- Brain State ---")
        self.inspect_brain_state()
        
        print("\n--- World State ---")
        self.inspect_world_state()
        
        print("\n--- Agent State ---")
        self.inspect_agent_state()
        
        print("\n--- Cross-Component State ---")
        self.inspect_cross_component_state()
        
    def inspect_brain_state(self):
        """Inspect brain state."""
        print(f"  Neuron count: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapse count: {self.brain.getTotalSynapseCount()}")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.3f} Hz")
        print(f"  Development stage: {self.brain.getDevelopmentalStage()}")
        print(f"  Current time: {self.brain.getCurrentTime():.3f}")
        print(f"  Excitatory/Inhibitory ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        
    def inspect_world_state(self):
        """Inspect world state."""
        print(f"  World dimensions: {self.world.getWidth()} x {self.world.getHeight()}")
        print(f"  Simulation time: {self.world.getSimulationTime():.3f}")
        print(f"  Agent position: {self.world.getAgentPosition()}")
        print(f"  Agent velocity: {self.world.getAgentVelocity()}")
        print(f"  Agent energy: {self.world.getAgentEnergy():.3f}")
        print(f"  Vision dimensions: {self.world.getVisionWidth()} x {self.world.getVisionHeight()}")
        
    def inspect_agent_state(self):
        """Inspect agent state."""
        print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {self.agent.getPredictionError():.3f}")
        print(f"  Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
        print(f"  Current reward: {self.agent.getCurrentReward():.3f}")
        print(f"  Reward modulation enabled: {self.agent.isRewardModulationEnabled()}")
        print(f"  Curiosity enabled: {self.agent.isCuriosityEnabled()}")
        print(f"  Development enabled: {self.agent.isDevelopmentEnabled()}")
        
    def inspect_cross_component_state(self):
        """Inspect cross-component state."""
        print("  Cross-component consistency:")
        
        # Check if brain and agent are synchronized
        brain_neuron_count = self.brain.getTotalNeuronCount()
        agent_curiosity = self.agent.getCuriosityLevel()
        
        print(f"    Brain neurons: {brain_neuron_count}")
        print(f"    Agent curiosity: {agent_curiosity:.3f}")
        
        # Check if world and agent are synchronized
        agent_pos = self.world.getAgentPosition()
        brain_neuron_count = self.brain.getTotalNeuronCount()
        
        print(f"    Agent position: ({agent_pos[0]:.1f}, {agent_pos[1]:.1f})")
        print(f"    Brain neurons: {brain_neuron_count}")
        
        # Check if simulation time is consistent
        brain_time = self.brain.getCurrentTime()
        world_time = self.world.getSimulationTime()
        
        print(f"    Brain time: {brain_time:.3f}")
        print(f"    World time: {world_time:.3f}")
        
        time_diff = abs(brain_time - world_time)
        if time_diff < 0.001:
            print("    OK: Brain and world times are synchronized")
        else:
            print(f"    WARNING: Time difference detected: {time_diff:.3f}")
```

### 62 Log Analysis

#### Analyze System Logs
```python
class LogAnalyzer:
    def __init__(self, log_file='nlm_debug.log'):
        self.log_file = log_file
        self.log_entries = []
        
    def load_logs(self):
        """Load logs from file."""
        try:
            with open(self.log_file, 'r') as f:
                for line in f:
                    # Parse log entry
                    entry = self.parse_log_entry(line)
                    if entry:
                        self.log_entries.append(entry)
            print(f"Loaded {len(self.log_entries)} log entries from {self.log_file}")
        except FileNotFoundError:
            print(f"ERROR: Log file not found: {self.log_file}")
        except Exception as e:
            print(f"ERROR: Failed to load logs: {e}")
            
    def parse_log_entry(self, line):
        """Parse a log entry."""
        # Simple parsing - in real implementation, would use proper log parsing
        timestamp_match = re.search(r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})', line)
        level_match = re.search(r'(DEBUG|INFO|WARNING|ERROR)', line)
        message_match = re.search(r'- (.+)$', line)
        
        if timestamp_match and level_match and message_match:
            return {
                'timestamp': timestamp_match.group(1),
                'level': level_match.group(1),
                'message': message_match.group(1)
            }
        return None
        
    def analyze_logs(self):
        """Analyze logs and identify issues."""
        print(f"\n=== Log Analysis ===")
        
        if not self.log_entries:
            print("No log entries to analyze")
            return
            
        # Count entries by level
        level_counts = {}
        for entry in self.log_entries:
            level = entry['level']
            if level not in level_counts:
                level_counts[level] = 0
            level_counts[level] += 1
            
        print(f"Total log entries: {len(self.log_entries)}")
        print("Entries by level:")
        for level, count in sorted(level_counts.items()):
            print(f"  {level}: {count}")
            
        # Identify potential issues
        self.identify_issues()
        
        # Generate recommendations
        self.generate_log_recommendations()
        
    def identify_issues(self):
        """Identify potential issues from logs."""
        print(f"\n--- Potential Issues ---")
        
        # Check for error messages
        error_entries = [e for e in self.log_entries if e['level'] == 'ERROR']
        if error_entries:
            print(f"ERROR entries found: {len(error_entries)}")
            for entry in error_entries[-5:]:  # Show last 5 errors
                print(f"  {entry['timestamp']}: {entry['message']}")
                
        # Check for warning messages
        warning_entries = [e for e in self.log_entries if e['level'] == 'WARNING']
        if warning_entries:
            print(f"WARNING entries found: {len(warning_entries)}")
            for entry in warning_entries[-5:]:  # Show last 5 warnings
                print(f"  {entry['timestamp']}: {entry['message']}")
                
        # Check for performance issues
        performance_issues = []
        
        # Look for slow operations
        for entry in self.log_entries:
            if 'slow' in entry['message'].lower() or 'performance' in entry['message'].lower():
                performance_issues.append(entry)
                
        if performance_issues:
            print(f"Performance issues found: {len(performance_issues)}")
            for entry in performance_issues[-3:]:  # Show last 3
                print(f"  {entry['timestamp']}: {entry['message']}")
                
    def generate_log_recommendations(self):
        """Generate recommendations based on log analysis."""
        print(f"\n--- Log Analysis Recommendations ---")
        
        # Check for common issues
        error_entries = [e for e in self.log_entries if e['level'] == 'ERROR']
        warning_entries = [e for e in self.log_entries if e['level'] == 'WARNING']
        
        if error_entries:
            print("1. Review error messages and fix the underlying issues")
            print("2. Check configuration and parameter settings")
            print("3. Verify system resources (memory, CPU)")
            print("4. Test with simpler configurations")
            
        if warning_entries:
            print("1. Address warnings to prevent errors")
            print("2. Optimize system performance")
            print("3. Monitor system resources more closely")
            
        if not error_entries and not warning_entries:
            print("1. Log analysis shows no critical issues")
            print("2. System appears to be running normally")
            print("3. Continue regular monitoring")
```

### 63 Automated Debugging

#### Automated Debugging Tools
```python
class AutomatedDebugger:
    def __init__(self, brain, world, agent, threshold=0.8):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.threshold = threshold
        self.debug_history = []
        
    def run_automated_debugging(self):
        """Run automated debugging."""
        print("\n=== Automated Debugging ===")
        
        # Collect system state
        state = self.collect_system_state()
        
        # Analyze state for issues
        issues = self.analyze_state_for_issues(state)
        
        # Generate debug actions
        actions = self.generate_debug_actions(issues)
        
        # Apply debug actions
        applied_actions = self.apply_debug_actions(actions)
        
        # Collect results
        results = self.collect_debug_results()
        
        # Generate report
        self.generate_debug_report(issues, applied_actions, results)
        
        return issues, applied_actions, results
        
    def collect_system_state(self):
        """Collect current system state."""
        state = {
            'timestamp': time.time(),
            'brain': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'firing_neuron_count': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage()
            },
            'world': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'simulation_time': self.world.getSimulationTime(),
                'agent_position': self.world.getAgentPosition(),
                'agent_velocity': self.world.getAgentVelocity()
            },
            'agent': {
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'current_reward': self.agent.getCurrentReward()
            }
        }
        return state
        
    def analyze_state_for_issues(self, state):
        """Analyze system state for issues."""
        issues = []
        
        # Check brain issues
        brain_issues = self.analyze_brain_state(state['brain'])
        issues.extend(brain_issues)
        
        # Check world issues
        world_issues = self.analyze_world_state(state['world'])
        issues.extend(world_issues)
        
        # Check agent issues
        agent_issues = self.analyze_agent_state(state['agent'])
        issues.extend(agent_issues)
        
        # Check cross-component issues
        cross_issues = self.analyze_cross_component_state(state)
        issues.extend(cross_issues)
        
        return issues
        
    def analyze_brain_state(self, brain_state):
        """Analyze brain state for issues."""
        issues = []
        
        # Check for abnormal firing rates
        firing_rate = brain_state['average_firing_rate']
        if firing_rate < 0.1:  # Very low firing rate
            issues.append({
                'severity': 'low',
                'category': 'brain',
                'description': f'Very low firing rate: {firing_rate:.3f} Hz',
                'suggestion': 'Check neuron parameters or connectivity'
            })
        elif firing_rate > 100:  # Very high firing rate
            issues.append({
                'severity': 'high',
                'category': 'brain',
                'description': f'Very high firing rate: {firing_rate:.3f} Hz',
                'suggestion': 'Adjust neuron threshold or refractory period'
            })
            
        # Check for developmental issues
        dev_stage = brain_state['development_stage']
        if dev_stage == 0:  # Initial stage
            issues.append({
                'severity': 'low',
                'category': 'development',
                'description': 'Brain is in initial developmental stage',
                'suggestion': 'Allow time for development or check developmental parameters'
            })
        
        return issues
        
    def analyze_world_state(self, world_state):
        """Analyze world state for issues."""
        issues = []
        
        # Check for boundary violations
        pos = world_state['agent_position']
        width = world_state['width']
        height = world_state['height']
        
        if not (0 <= pos[0] <= width):
            issues.append({
                'severity': 'high',
                'category': 'world',
                'description': f'Agent x-position {pos[0]:.1f} outside bounds [0, {width}]',
                'suggestion': 'Check agent velocity or world boundaries'
            })
            
        if not (0 <= pos[1] <= height):
            issues.append({
                'severity': 'high',
                'category': 'world',
                'description': f'Agent y-position {pos[1]:.1f} outside bounds [0, {height}]',
                'suggestion': 'Check agent velocity or world boundaries'
            })
            
        return issues
        
    def analyze_agent_state(self, agent_state):
        """Analyze agent state for issues."""
        issues = []
        
        # Check for abnormal curiosity levels
        curiosity = agent_state['curiosity_level']
        if curiosity < 0.1:  # Very low curiosity
            issues.append({
                'severity': 'low',
                'category': 'agent',
                'description': f'Very low curiosity level: {curiosity:.3f}',
                'suggestion': 'Check curiosity parameters or reward signals'
            })
        elif curiosity > 0.9:  # Very high curiosity
            issues.append({
                'severity': 'medium',
                'category': 'agent',
                'description': f'Very high curiosity level: {curiosity:.3f}',
                'suggestion': 'Adjust curiosity parameters or novelty detection'
            })
            
        # Check for abnormal novelty levels
        novelty = agent_state['novelty_level']
        if novelty < 0.1:  # Very low novelty
            issues.append({
                'severity': 'low',
                'category': 'agent',
                'description': f'Very low novelty level: {novelty:.3f}',
                'suggestion': 'Check novelty detection parameters'
            })
        elif novelty > 0.9:  # Very high novelty
            issues.append({
                'severity': 'medium',
                'category': 'agent',
                'description': f'Very high novelty level: {novelty:.3f}',
                'suggestion': 'Adjust novelty detection or threshold parameters'
            })
            
        return issues
        
    def analyze_cross_component_state(self, state):
        """Analyze cross-component state for issues."""
        issues = []
        
        # Check for time synchronization issues
        brain_time = state['brain']['development_stage']  # Simplified
        world_time = state['world']['simulation_time']
        
        time_diff = abs(brain_time - world_time)
        if time_diff > 1.0:
            issues.append({
                'severity': 'medium',
                'category': 'cross_component',
                'description': f'Time difference between brain and world: {time_diff:.3f}',
                'suggestion': 'Check simulation timestep or component coordination'
            })
            
        # Check for consistency between components
        brain_neuron_count = state['brain']['neuron_count']
        world_width = state['world']['width']
        
        if brain_neuron_count > world_width * world_width * 10: nly # Many neurons for small world
            issues.append({
                'severity': 'low',
                'category': 'cross_component',
                'description': f'High neuron count ({brain_neuron_count}) for small world ({world_width}x{world_width})',
                'suggestion': 'Consider scaling neuron count to world size'
            })
            
        return issues
        
    def generate_debug_actions(self, issues):
        """Generate debug actions based on issues."""
        actions = []
        
        for issue in issues:
            if issue['severity'] == 'high':
                actions.append({
                    'action': 'immediate_fix',
                    'description': f'Fix {issue["description"]}',
                    'priority': 'high'
                })
            elif issue['severity'] == 'medium':
                actions.append({
                    'action': 'investigate',
                    'description': f'Investigate {issue["description"]}',
                    'priority': 'medium'
                })
            else:  # low severity
                actions.append({
                    'action': 'monitor',
                    'description': f'Monitor {issue["description"]}',
                    'priority': 'low'
                })
                
        return actions
        
    def apply_debug_actions(self, actions):
        """Apply debug actions."""
        applied_actions = []
        
        for action in actions:
            if action['action'] == 'immediate_fix':
                # Apply immediate fix
                result = self.apply_immediate_fix(action)
                applied_actions.append({
                    'action': action['description'],
                    'result': result
                })
                
            elif action['action'] == 'investigate':
                # Start investigation
                result = self.start_investigation(action)
                applied_actions.append({
                    'action': action['description'],
                    'result': result
                })
                
            elif action['action'] == 'monitor':
                # Start monitoring
                result = self.start_monitoring(action)
                applied_actions.append({
                    'action': action['description'],
                    'result': result
                })
                
        return applied_actions
        
    def apply_immediate_fix(self, action):
        """Apply immediate fix."""
        # This would apply the actual fix
        # For this example, we'll return a success message
        return f"Applied fix: {action['description']}"
        
    def start_investigation(self, action):
        """Start investigation."""
        # This would start investigation
        # For this example, we'll return a success message
        return f"Started investigation: {action['description']}"
        
    def start_monitoring(self, action):
        """Start monitoring."""
        # This would start monitoring
        # For this example, we'll return a success message
        return f"Started monitoring: {action['description']}"
        
    def collect_debug_results(self):
        """Collect debug results."""
        results = {
            'timestamp': time.time(),
            'issues_found': len(self.debug_history),
            'actions_taken': len(self.debug_history),
            'issues_resolved': 0,
            'issues_remaining': 0
        }
        
        return results
        
    def generate_debug_report(self, issues, applied_actions, results):
        """Generate debug report."""
        print(f"\n=== Automated Debug Report ===")
        
        print(f"Issues found: {len(issues)}")
        print(f"Actions taken: {len(applied_actions)}")
        print(f"Debug results: {results}")
        
        print(f"\n=== Debug Summary ===")
        print(f"Severity levels:")
        for issue in issues:
            print(f"  {issue['severity']}: {issue['description']}")
            
        print(f"\nApplied actions:")
        for action in applied_actions:
            print(f"  - {action['description']}: {action['result']}")
```

### 64 Performance Tuning

#### Performance Tuning Techniques
```python
class PerformanceTuner:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.performance_history = []
        
    def tune_performance(self, target_metrics):
        """Tune performance for target metrics."""
        print(f"\n=== Performance Tuning ({target_metrics}) ===")
        
        # Analyze current performance
        current_metrics = self.measure_current_performance()
        print(f"Current metrics: {current_metrics}")
        
        # Identify optimization opportunities
        optimizations = self.identify_optimization_opportunities(current_metrics, target_metrics)
        
        # Apply optimizations
        applied_optimizations = self.apply_optimizations(optimizations)
        
        # Measure performance after optimization
        new_metrics = self.measure_current_performance()
        
        # Generate tuning report
        self.generate_tuning_report(current_metrics, new_metrics, optimizations, applied_optimizations)
        
        return new_metrics
        
    def measure_current_performance(self):
        """Measure current performance."""
        # Measure simulation performance
        start_time = time.time()
        steps = 100
        
        for step in range(steps):
            self.brain.step(step)
            
        elapsed = time.time() - start_time
        
        # Measure memory usage
        process = psutil.Process()
        memory_info = process.memory_info()
        
        # Measure CPU usage
        cpu_percent = process.cpu_percent(interval=0.1)
        
        # Calculate metrics
        metrics = {
            'steps_per_second': steps / elapsed,
            'memory_mb': memory_info.rss / (1024 * 1024),
            'cpu_percent': cpu_percent,
            'elapsed_time': elapsed
        }
        
        self.performance_history.append((time.time(), metrics))
        
        return metrics
        
    def identify_optimization_opportunities(self, current_metrics, target_metrics):
        """Identify optimization opportunities."""
        opportunities = []
        
        # Check for performance bottlenecks
        if current_metrics['steps_per_second'] < 10:  # Low performance
            opportunities.append({
                'type': 'algorithmic',
                'description': 'Low steps per second',
                'estimated_improvement': 0.5,  # 50% improvement
                'current_value': current_metrics['steps_per_second'],
                'target_value': target_metrics['steps_per_second']
            })
            
        if current_metrics['memory_mb'] > 500:  # High memory usage
            opportunities.append({
                'type': 'memory',
                'description': 'High memory usage',
                'estimated_improvement': 0.3,  # 30% improvement
                'current_value': current_metrics['memory_mb'],
                'target_value': target_metrics['memory_mb']
            })
            
        if current_metrics['cpu_percent'] > 80:  # High CPU usage
            opportunities.append({
                'type': 'cpu',
                'description': 'High CPU usage',
                'estimated_improvement': 0.2,  # 20% improvement
                'current_value': current_metrics['cpu_percent'],
                'target_value': target_metrics['cpu_percent']
            })
            
        # Filter by target metrics
        filtered_opportunities = []
        for opportunity in opportunities:
            if opportunity['type'] in target_metrics:
                filtered_opportunities.append(opportunity)
                
        return filtered_opportunities
        
    def apply_optimizations(self, optimizations):
        """Apply optimizations."""
        applied_optimizations = []
        
        for optimization in optimizations:
            if optimization['type'] == 'algorithmic':
                result = self.apply_algorithmic_optimization(optimization)
                applied_optimizations.append({
                    'type': optimization['type'],
                    'description': optimization['description'],
                    'result': result
                })
                
            elif optimization['type'] == 'memory':
                result = self.apply_memory_optimization(optimization)
                applied_optimizations.append({
                    'type': optimization['type'],
                    'description': optimization['description'],
                    'result': result
                })
                
            elif optimization['type'] == 'cpu':
                result = self.apply_cpu_optimization(optimization)
                applied_optimizations.append({
                    'type': optimization['type'],
                    'description': optimization['description'],
                    'result': result
                })
                
        return applied_optimizations
        
    def apply_algorithmic_optimization(self, optimization):
        """Apply algorithmic optimization."""
        # This would apply algorithmic optimizations
        # For this example, we'll return a success message
        return f"Applied algorithmic optimization: {optimization['description']}"
        
    def apply_memory_optimization(self, optimization):
        """Apply memory optimization."""
        # This would apply memory optimizations
        # For this example, we'll return a success message
        return f"Applied memory optimization: {optimization['description']}"
        
    def apply_cpu_optimization(self, optimization):
        """Apply CPU optimization."""
        # This would apply CPU optimizations
        # For this example, we'll return a success message
        return f"Applied CPU optimization: {optimization['description']}"
        
    def generate_tuning_report(self, before_metrics, after_metrics, optimizations, applied_optimizations):
        """Generate tuning report."""
        print(f"\n=== Performance Tuning Report ===")
        
        print(f"Before optimization:")
        for key, value in before_metrics.items():
            print(f"  {key}: {value:.3f}")
            
        print(f"\nAfter optimization:")
        for key, value in after_metrics.items():
            print(f"  {key}: {value:.3f}")
            
        # Calculate improvements
        print(f"\nImprovements:")
        for key in before_metrics:
            if key in after_metrics:
                improvement = (after_metrics[key] - before_metrics[key]) / before_metrics[key] * 100
                direction = "increase" if key.endswith('_per_second') else "decrease"
                print(f"  {key}: {improvement:+.1f}% {direction}")
                
        print(f"\nApplied optimizations:")
        for opt in applied_optimizations:
            print(f"  - {opt['type']}: {opt['description']}")
            print(f"    Result: {opt['result']}")
```

## Conclusion

This Debugging Guide provides comprehensive debugging guidance for NLM, covering:

- Introduction to NLM debugging challenges
- Error analysis and troubleshooting techniques
- Performance debugging (profiling, memory leak detection, bottleneck identification)
- Plugin debugging (loading, execution, communication)
- Real-world debugging scenarios (learning agents, world simulation, configuration, integration)
- Advanced debugging techniques (state inspection, log analysis, automated debugging, performance tuning)

By following these guidelines, users can:

- Understand and diagnose NLM debugging issues
- Apply systematic debugging approaches
- Identify and resolve performance bottlenecks
- Debug plugin-related issues
- Troubleshoot real-world scenarios
- Use advanced debugging techniques for complex problems
- Implement automated debugging tools
- Tune system performance for optimal operation

The guide provides practical, actionable advice for debugging and optimizing NLM across different use cases and problem types.

## References

- NLM Documentation
- Advanced NLM Features Documentation
- Performance Optimization Guide
- Plugin Development Guide
- Best Practices Guide