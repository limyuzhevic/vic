# Performance Monitoring and Profiling Tools for NLM

## Overview

Performance monitoring and profiling are critical for optimizing NLM simulations, identifying bottlenecks, understanding computational requirements, and ensuring efficient resource utilization. This document outlines comprehensive performance monitoring and profiling tools that can be added to the NLM framework.

## Current Performance Monitoring Capabilities

### 1. Basic Performance Tracking
```python
# Current basic performance monitoring (if available)
brain = pynlm.createBrain(config)
brain.initialize()

# Basic performance metrics
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
end_time = time.time()
elapsed = end_time - start_time
print(f"Simulation completed in {elapsed:.2f} seconds")
print(f"Steps per second: {1000/elapsed:.2f}")
```

### 2. Basic Resource Monitoring
```python
# Current resource monitoring (if available)
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
print(f"CPU usage: {process.cpu_percent()}%")
```

## Comprehensive Performance Monitoring Framework

### 1. Advanced Performance Profiler

#### PerformanceProfiler Class
```python
class PerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations"""
    
    def __init__(self, brain, world=None):
        self.brain = brain
        self.world = world
        self.timings = []
        self.memory_samples = []
        self.cpu_samples = []
        self.step_metrics = {}
        self.start_time = None
        self.end_time = None
        self.session_id = str(uuid.uuid4())
        
    def start_session(self):
        """Start performance profiling session"""
        self.start_time = time.time()
        self.timings.clear()
        self.memory_samples.clear()
        self.cpu_samples.clear()
        self.step_metrics.clear()
        
        # Record initial system state
        self.recordSystemState()
        
        return self.session_id
        
    def stop_session(self):
        """Stop performance profiling session"""
        self.end_time = time.time()
        self.recordSystemState()
        
        return {
            'session_id': self.session_id,
            'total_duration': self.end_time - self.start_time,
            'timings': self.timings,
            'memory_samples': self.memory_samples,
            'cpu_samples': self.cpu_samples,
            'step_metrics': self.step_metrics,
            'summary': self.generateSummary()
        }
        
    def recordSystemState(self):
        """Record current system state"""
        import psutil
        
        current_time = time.time()
        memory_info = psutil.Process().memory_info()
        cpu_percent = psutil.cpu_percent(interval=None)
        
        self.timings.append({
            'timestamp': current_time,
            'elapsed': current_time - self.start_time if self.start_time else 0,
            'memory_rss': memory_info.rss,
            'memory_vms': memory_info.vms,
            'memory_shared': memory_info.shared,
            'memory_text': memory_info.text,
            'memory_data': memory_info.data,
            'memory_dirty': memory_info.dirty,
            'cpu_percent': cpu_percent
        })
        
    def recordStepMetrics(self, step, elapsed_time, additional_metrics=None):
        """Record metrics for a single simulation step"""
        step_data = {
            'step': step,
            'timestamp': time.time(),
            'elapsed_time': elapsed_time,
            'steps_per_second': 1.0 / elapsed_time if elapsed_time > 0 else 0,
            'brain_state': self.captureBrainState(),
            'memory_usage': self.getCurrentMemoryUsage(),
            'cpu_usage': self.getCurrentCpuUsage()
        }
        
        if additional_metrics:
            step_data.update(additional_metrics)
            
        self.step_metrics[step] = step_data
        
    def captureBrainState(self):
        """Capture current brain state for performance analysis"""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'region_count': self.brain.getRegionCount(),
            'memory_load': self.getBrainMemoryLoad()
        }
        
    def getCurrentMemoryUsage(self):
        """Get current memory usage in bytes"""
        import psutil
        memory_info = psutil.Process().memory_info()
        return {
            'rss': memory_info.rss,
            'vms': memory_info.vms,
            'shared': memory_info.shared,
            'text': memory_info.text,
            'data': memory_info.data
        }
        
    def getCurrentCpuUsage(self):
        """Get current CPU usage percentage"""
        import psutil
        return psutil.cpu_percent(interval=None)
        
    def getBrainMemoryLoad(self):
        """Estimate memory load from brain state"""
        total_elements = (self.brain.getTotalNeuronCount() + 
                         self.brain.getTotalSynapseCount() + 
                         self.brain.getRegionCount())
        
        # Estimate based on number of active elements
        active_elements = (self.brain.getActiveNeuronCount() + 
                          self.brain.getFiringNeuronCount())
        
        return min(1.0, active_elements / max(total_elements, 1))
        
    def generateSummary(self):
        """Generate performance summary"""
        if not self.timings:
            return None
            
        total_duration = self.end_time - self.start_time
        total_steps = len(self.step_metrics)
        
        # Calculate performance statistics
        elapsed_times = [t['elapsed_time'] for t in self.step_metrics.values()]
        memory_usages = [t['memory_usage']['rss'] for t in self.step_metrics.values()]
        cpu_usages = [t['cpu_usage'] for t in self.step_metrics.values()]
        
        summary = {
            'total_duration': total_duration,
            'total_steps': total_steps,
            'steps_per_second': total_steps / total_duration if total_duration > 0 else 0,
            'average_step_time': sum(elapsed_times) / len(elapsed_times) if elapsed_times else 0,
            'min_step_time': min(elapsed_times) if elapsed_times else 0,
            'max_step_time': max(elapsed_times) if elapsed_times else 0,
            'median_step_time': self.calculateMedian(elapsed_times),
            'memory_stats': {
                'average_rss': sum(m['rss'] for m in memory_usages) / len(memory_usages) if memory_usages else 0,
                'max_rss': max(m['rss'] for m in memory_usages) if memory_usages else 0,
                'memory_growth_rate': self.calculateMemoryGrowthRate(memory_usages)
            },
            'cpu_stats': {
                'average_cpu': sum(cpu_usages) / len(cpu_usages) if cpu_usages else 0,
                'max_cpu': max(cpu_usages) if cpu_usages else 0,
                'cpu_efficiency': self.calculateCpuEfficiency(cpu_usages)
            },
            'brain_state_stats': self.analyzeBrainStateVariations(),
            'performance_bottlenecks': self.identifyPerformanceBottlenecks(),
            'recommendations': self.generatePerformanceRecommendations()
        }
        
        return summary
        
    def calculateMedian(self, values):
        """Calculate median of values"""
        if not values:
            return 0
        sorted_values = sorted(values)
        n = len(sorted_values)
        if n % 2 == 1:
            return sorted_values[n // 2]
        else:
            return (sorted_values[n // 2 - 1] + sorted_values[n // 2]) / 2
        
    def calculateMemoryGrowthRate(self, memory_usages):
        """Calculate memory growth rate over time"""
        if len(memory_usages) < 2:
            return 0.0
            
        initial_memory = memory_usages[0]
        final_memory = memory_usages[-1]
        time_span = len(memory_usages) - 1
        
        growth_rate = (final_memory - initial_memory) / time_span
        return growth_rate
        
    def calculateCpuEfficiency(self, cpu_usages):
        """Calculate CPU efficiency (useful work vs. CPU time)"""
        if not cpu_usages:
            return 0.0
            
        # Simplified efficiency calculation
        # Actual efficiency depends on specific workload characteristics
        average_cpu = sum(cpu_usages) / len(cpu_usages)
        efficiency = min(1.0, 100.0 / average_cpu)  # Normalize to 0-1 scale
        
        return efficiency
        
    def analyzeBrainStateVariations(self):
        """Analyze variations in brain state over time"""
        variations = {}
        
        for metric_name in ['neuron_count', 'synapse_count', 'active_neurons', 
                           'firing_neurons', 'spike_count', 'region_count']:
            metric_values = [state[metric_name] for state in 
                           self.step_metrics.values() if metric_name in state['brain_state']]
            
            if metric_values:
                variations[metric_name] = {
                    'mean': sum(metric_values) / len(metric_values),
                    'std': self.calculateStdDev(metric_values),
                    'min': min(metric_values),
                    'max': max(metric_values),
                    'coefficient_of_variation': self.calculateCoefficientOfVariation(metric_values)
                }
                
        return variations
        
    def calculateCoefficientOfVariation(self, values):
        """Calculate coefficient of variation (std/mean)"""
        if not values or sum(values) == 0:
            return 0.0
        mean = sum(values) / len(values)
        std = self.calculateStdDev(values)
        return std / mean if mean > 0 else 0.0
        
    def identifyPerformanceBottlenecks(self):
        """Identify potential performance bottlenecks"""
        bottlenecks = []
        
        # Check for slow steps
        slow_steps = [(step, metrics['elapsed_time']) 
                     for step, metrics in self.step_metrics.items()
                     if metrics['elapsed_time'] > 0.1]  # Steps taking > 100ms
        
        if slow_steps:
            bottlenecks.append({
                'type': 'slow_steps',
                'description': f"Steps taking >100ms ({len(slow_steps)} instances)",
                'affected_steps': [step for step, _ in slow_steps],
                'average_time': sum(t for _, t in slow_steps) / len(slow_steps)
            })
            
        # Check for memory growth
        memory_growth_rate = self.calculateMemoryGrowthRate(
            [m['memory_usage']['rss'] for m in self.step_metrics.values()]
        )
        
        if memory_growth_rate > 1024 * 1024:  # > 1 MB per step
            bottlenecks.append({
                'type': 'memory_leak',
                'description': f"High memory growth rate: {memory_growth_rate / 1024 / 1024:.2f} MB/step",
                'severity': 'high' if memory_growth_rate > 10 * 1024 * 1024 else 'medium'
            })
            
        # Check for high CPU usage
        avg_cpu = sum(m['cpu_usage'] for m in self.step_metrics.values()) / len(self.step_metrics)
        if avg_cpu > 80:
            bottlenecks.append({
                'type': 'cpu_bound',
                'description': f"High average CPU usage: {avg_cpu:.1f}%",
                'severity': 'medium'
            })
            
        # Check for brain state variations
        for metric_name, stats in self.analyzeBrainStateVariations().items():
            cv = stats['coefficient_of_variation']
            if cv > 0.5:  # High variation coefficient
                bottlenecks.append({
                    'type': 'state_variations',
                    'description': f"High variation in {metric_name}: CV={cv:.2f}",
                    'severity': 'low'
                })
                
        return bottlenecks
        
    def generatePerformanceRecommendations(self):
        """Generate performance recommendations based on profiling data"""
        recommendations = []
        
        bottlenecks = self.identifyPerformanceBottlenecks()
        
        for bottleneck in bottlenecks:
            if bottleneck['type'] == 'slow_steps':
                recommendations.append({
                    'priority': 'high',
                    'category': 'optimization',
                    'suggestion': f"Optimize neural computations for steps {bottleneck['affected_steps']}",
                    'expected_improvement': '50-80% reduction in step time'
                })
            elif bottleneck['type'] == 'memory_leak':
                recommendations.append({
                    'priority': 'high',
                    'category': 'memory_management',
                    'suggestion': "Implement memory leak detection and cleanup",
                    'expected_improvement': 'Memory usage stabilization'
                })
            elif bottleneck['type'] == 'cpu_bound':
                recommendations.append({
                    'priority': 'medium',
                    'category': 'parallelization',
                    'suggestion': "Parallelize CPU-intensive computations",
                    'expected_improvement': '20-40% improvement in throughput'
                })
            elif bottleneck['type'] == 'state_variations':
                recommendations.append({
                    'priority': 'low',
                    'category': 'stability',
                    'suggestion': "Stabilize neural parameters to reduce variations",
                    'expected_improvement': 'More predictable performance'
                })
                
        return recommendations
```

### 2. Real-time Performance Monitoring

#### RealTimeMonitor Class
```python
class RealTimeMonitor:
    """Real-time performance monitoring for NLM simulations"""
    
    def __init__(self, brain, world=None, update_interval=1.0):
        self.brain = brain
        self.world = world
        self.update_interval = update_interval
        self.monitoring = False
        self.monitor_thread = None
        self.performance_data = []
        self.alert_callbacks = []
        self.thresholds = self.getDefaultThresholds()
        
    def getDefaultThresholds(self):
        """Get default performance thresholds"""
        return {
            'max_step_time': 0.5,  # Maximum acceptable step time (seconds)
            'max_memory_growth': 10 * 1024 * 1024,  # Maximum memory growth per second (bytes)
            'max_cpu_usage': 90,  # Maximum CPU usage percentage
            'min_steps_per_second': 100,  # Minimum throughput
            'memory_usage_threshold': 500 * 1024 * 1024  # Memory usage threshold (bytes)
        }
        
    def startMonitoring(self):
        """Start real-time performance monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitorLoop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def stopMonitoring(self):
        """Stop real-time performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
            
    def _monitorLoop(self):
        """Main monitoring loop"""
        while self.monitoring:
            try:
                self.collectPerformanceData()
                self.checkThresholds()
                self.notifyAlerts()
                time.sleep(self.update_interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self.update_interval)
                
    def collectPerformanceData(self):
        """Collect current performance data"""
        timestamp = time.time()
        
        # Capture system metrics
        import psutil
        memory_info = psutil.Process().memory_info()
        cpu_percent = psutil.cpu_percent(interval=None)
        
        # Capture brain metrics
        brain_metrics = {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
        # Calculate derived metrics
        performance_metrics = {
            'timestamp': timestamp,
            'memory_rss': memory_info.rss,
            'memory_vms': memory_info.vms,
            'cpu_percent': cpu_percent,
            'steps_per_second': self.calculateCurrentStepsPerSecond(),
            'brain_metrics': brain_metrics,
            'resource_efficiency': self.calculateResourceEfficiency(
                memory_info.rss, cpu_percent
            )
        }
        
        self.performance_data.append(performance_metrics)
        
        # Keep only recent data
        if len(self.performance_data) > 3600:  # Keep last hour of data
            self.performance_data.pop(0)
            
    def calculateCurrentStepsPerSecond(self):
        """Calculate current steps per second"""
        # This would require tracking step completion times
        # For simplicity, return a placeholder
        return 100.0
        
    def calculateResourceEfficiency(self, memory_bytes, cpu_percent):
        """Calculate resource efficiency metric"""
        # Simplified efficiency calculation
        memory_efficiency = min(1.0, 1024 * 1024 * 1024 / max(memory_bytes, 1))  # Target 1GB
        cpu_efficiency = min(1.0, 100.0 / max(cpu_percent, 1))  # Target <100% CPU
        
        return (memory_efficiency + cpu_efficiency) / 2.0
        
    def checkThresholds(self):
        """Check if any performance thresholds are exceeded"""
        if not self.performance_data:
            return
            
        latest = self.performance_data[-1]
        
        # Check memory usage
        if latest['memory_rss'] > self.thresholds['memory_usage_threshold']:
            self.triggerAlert('memory_usage', f"Memory usage high: {latest['memory_rss'] / 1024 / 1024:.2f} MB")
            
        # Check CPU usage
        if latest['cpu_percent'] > self.thresholds['max_cpu_usage']:
            self.triggerAlert('cpu_usage', f"CPU usage high: {latest['cpu_percent']:.1f}%")
            
        # Check steps per second
        if latest['steps_per_second'] < self.thresholds['min_steps_per_second']:
            self.triggerAlert('performance', f"Low performance: {latest['steps_per_second']:.1f} steps/sec")
            
    def triggerAlert(self, alert_type, message):
        """Trigger performance alert"""
        alert = {
            'timestamp': time.time(),
            'type': alert_type,
            'message': message,
            'severity': self.determineAlertSeverity(alert_type, message)
        }
        
        # Call alert callbacks
        for callback in self.alert_callbacks:
            callback(alert)
            
    def determineAlertSeverity(self, alert_type, message):
        """Determine alert severity"""
        if 'memory_usage' in alert_type:
            return 'high'
        elif 'cpu_usage' in alert_type or 'performance' in alert_type:
            return 'medium'
        else:
            return 'low'
            
    def addAlertCallback(self, callback):
        """Add callback function for performance alerts"""
        self.alert_callbacks.append(callback)
        
    def getPerformanceData(self, time_range=None):
        """Get performance data for specified time range"""
        if time_range is None:
            return self.performance_data
            
        cutoff_time = time.time() - time_range
        return [data for data in self.performance_data if data['timestamp'] > cutoff_time]
        
    def getStatistics(self, time_range=None):
        """Get performance statistics for specified time range"""
        data = self.getPerformanceData(time_range)
        
        if not data:
            return None
            
        steps_per_second = [d['steps_per_second'] for d in data]
        memory_usage = [d['memory_rss'] for d in data]
        cpu_usage = [d['cpu_percent'] for d in data]
        
        return {
            'duration': len(data) * self.update_interval,
            'steps_per_second': {
                'mean': sum(steps_per_second) / len(steps_per_second),
                'min': min(steps_per_second),
                'max': max(steps_per_second),
                'std': self.calculateStdDev(steps_per_second)
            },
            'memory_usage': {
                'mean': sum(memory_usage) / len(memory_usage),
                'min': min(memory_usage),
                'max': max(memory_usage),
                'growth_rate': self.calculateMemoryGrowthRate(memory_usage)
            },
            'cpu_usage': {
                'mean': sum(cpu_usage) / len(cpu_usage),
                'min': min(cpu_usage),
                'max': max(cpu_usage),
                'std': self.calculateStdDev(cpu_usage)
            },
            'resource_efficiency': {
                'mean': sum(d['resource_efficiency'] for d in data) / len(data),
                'min': min(d['resource_efficiency'] for d in data),
                'max': max(d['resource_efficiency'] for d in data)
            }
        }
```

### 3. Simulation Benchmarking

#### BenchmarkSuite Class
```python
class BenchmarkSuite:
    """Comprehensive benchmarking suite for NLM simulations"""
    
    def __init__(self, brain, world=None):
        self.brain = brain
        self.world = world
        self.benchmarks = {}
        self.results = {}
        self.configurations = []
        
    def addBenchmark(self, name, benchmark_func, description, category='general'):
        """Add benchmark test"""
        self.benchmarks[name] = {
            'function': benchmark_func,
            'description': description,
            'category': category,
            'parameters': self.getBenchmarkParameters(benchmark_func)
        }
        
    def getBenchmarkParameters(self, benchmark_func):
        """Extract parameters from benchmark function"""
        # Use function signature to determine parameters
        sig = inspect.signature(benchmark_func)
        parameters = {}
        
        for param_name, param in sig.parameters.items():
            if param_name != 'self':
                default_value = param.default if param.default != inspect.Parameter.empty else None
                parameters[param_name] = {
                    'type': self.inferParameterType(param.annotation),
                    'default': default_value,
                    'required': default_value is None
                }
                
        return parameters
        
    def inferParameterType(self, annotation):
        """Infer parameter type from annotation"""
        type_map = {
            int: 'integer',
            float: 'float',
            str: 'string',
            bool: 'boolean',
            list: 'array',
            dict: 'object'
        }
        
        return type_map.get(annotation, 'object')
        
    def runBenchmark(self, name, **kwargs):
        """Run specific benchmark"""
        if name not in self.benchmarks:
            raise ValueError(f"Benchmark '{name}' not found")
            
        benchmark = self.benchmarks[name]
        profiler = PerformanceProfiler(self.brain, self.world)
        
        # Start profiling
        session_id = profiler.start_session()
        
        # Run benchmark
        start_time = time.time()
        result = benchmark['function'](**kwargs)
        end_time = time.time()
        
        # Stop profiling
        profile_data = profiler.stop_session()
        
        # Store results
        self.results[f"{name}_{session_id}"] = {
            'benchmark_name': name,
            'result': result,
            'execution_time': end_time - start_time,
            'profile_data': profile_data,
            'parameters': kwargs
        }
        
        return result, profile_data
        
    def runBenchmarkSuite(self, **kwargs):
        """Run all benchmarks in suite"""
        results = {}
        
        for name, benchmark in self.benchmarks.items():
            try:
                result, profile_data = self.runBenchmark(name, **kwargs)
                results[name] = {
                    'result': result,
                    'success': True,
                    'profile_data': profile_data
                }
            except Exception as e:
                results[name] = {
                    'result': None,
                    'success': False,
                    'error': str(e),
                    'profile_data': None
                }
                
        return results
        
    def createConfigurationSuite(self, base_config, variations):
        """Create configuration suite for systematic testing"""
        configurations = []
        
        # Create base configuration
        base_result = self.runBenchmark('default_configuration', config=base_config)
        configurations.append({
            'name': 'base',
            'parameters': base_config,
            'results': base_result
        })
        
        # Create varied configurations
        for variation_name, variation_params in variations.items():
            varied_config = self.mergeConfigurations(base_config, variation_params)
            result = self.runBenchmark(variation_name, config=varied_config)
            configurations.append({
                'name': variation_name,
                'parameters': varied_config,
                'results': result
            })
            
        self.configurations = configurations
        return configurations
        
    def mergeConfigurations(self, base_config, variation_params):
        """Merge base configuration with variation parameters"""
        merged_config = copy.deepcopy(base_config)
        
        for param_name, param_value in variation_params.items():
            # Navigate to nested parameters
            keys = param_name.split('.')
            current = merged_config
            
            for key in keys[:-1]:
                if key not in current:
                    current[key] = {}
                current = current[key]
                
            # Set the final parameter
            current[keys[-1]] = param_value
            
        return merged_config
        
    def analyzeConfigurationSuite(self):
        """Analyze configuration suite results"""
        if not self.configurations:
            return None
            
        analysis = {
            'total_configurations': len(self.configurations),
            'successful_configurations': sum(1 for config in self.configurations 
                                           if config['results'] and config['results'].get('success', False)),
            'performance_comparison': self.comparePerformanceAcrossConfigurations(),
            'optimization_opportunities': self.identifyOptimizationOpportunities(),
            'recommendations': self.generateConfigurationRecommendations()
        }
        
        return analysis
        
    def comparePerformanceAcrossConfigurations(self):
        """Compare performance across different configurations"""
        comparison = {}
        
        # Group configurations by category
        for config in self.configurations:
            if config['results'] and config['results'].get('success', False):
                category = config['name'].split('_')[0] if '_' in config['name'] else config['name']
                
                if category not in comparison:
                    comparison[category] = []
                    
                comparison[category].append({
                    'name': config['name'],
                    'execution_time': config['results']['profile_data']['summary']['total_duration'],
                    'steps_per_second': config['results']['profile_data']['summary']['steps_per_second'],
                    'memory_usage': config['results']['profile_data']['summary']['memory_stats']['average_rss']
                })
                
        return comparison
        
    def identifyOptimizationOpportunities(self):
        """Identify optimization opportunities across configurations"""
        opportunities = []
        
        if len(self.configurations) < 2:
            return opportunities
            
        # Compare best vs. worst performers
        all_results = []
        for config in self.configurations:
            if config['results'] and config['results'].get('success', False):
                all_results.append(config['results'])
                
        if not all_results:
            return opportunities
            
        # Find best and worst performers
        best = max(all_results, key=lambda x: x['profile_data']['summary']['steps_per_second'])
        worst = min(all_results, key=lambda x: x['profile_data']['summary']['steps_per_second'])
        
        # Analyze differences
        if best['profile_data']['summary']['steps_per_second'] > 0:
            speedup = (best['profile_data']['summary']['steps_per_second'] / 
                      worst['profile_data']['summary']['steps_per_second'])
            
            opportunities.append({
                'type': 'performance_optimization',
                'description': f"Potential {speedup:.1f}x performance improvement",
                'category': 'systematic_parameter_tuning'
            })
            
        # Compare memory usage
        best_memory = best['profile_data']['summary']['memory_stats']['average_rss']
        worst_memory = worst['profile_data']['summary']['memory_stats']['average_rss']
        
        if worst_memory > 0:
            memory_improvement = (worst_memory - best_memory) / worst_memory
            
            opportunities.append({
                'type': 'memory_optimization',
                'description': f"Potential {memory_improvement:.1%} memory improvement",
                'category': 'memory_efficiency'
            })
            
        return opportunities
        
    def generateConfigurationRecommendations(self):
        """Generate recommendations for configuration optimization"""
        recommendations = []
        
        comparison = self.comparePerformanceAcrossConfigurations()
        
        for category, configs in comparison.items():
            if len(configs) < 2:
                continue
                
            # Find best configuration in this category
            best_config = max(configs, key=lambda x: x['steps_per_second'])
            
            # Analyze parameters that make this configuration good
            for config in self.configurations:
                if config['name'] == best_config['name'] and config['results']:
                    profile_data = config['results']['profile_data']
                    
                    # Extract key performance parameters
                    steps_per_second = profile_data['summary']['steps_per_second']
                    avg_step_time = profile_data['summary']['average_step_time']
                    memory_usage = profile_data['summary']['memory_stats']['average_rss']
                    
                    recommendations.append({
                        'configuration': best_config['name'],
                        'optimal_parameters': self.extractOptimalParameters(config['parameters']),
                        'performance_characteristics': {
                            'steps_per_second': steps_per_second,
                            'average_step_time': avg_step_time,
                            'memory_usage_mb': memory_usage / 1024 / 1024
                        },
                        'target_use_cases': self.determineTargetUseCase(steps_per_second, memory_usage)
                    })
                    
                    break
                    
        return recommendations
        
    def extractOptimalParameters(self, parameters):
        """Extract optimal parameters from configuration"""
        # This would analyze which parameters contribute to good performance
        optimal_params = {}
        
        for param_name, param_value in parameters.items():
            if self.isPerformanceCriticalParameter(param_name):
                optimal_params[param_name] = param_value
                
        return optimal_params
        
    def isPerformanceCriticalParameter(self, param_name):
        """Check if parameter is likely to affect performance"""
        performance_critical = [
            'neural.neuron_count',
            'neural.synapse_density',
            'neural.connection_probability',
            'simulation.timestep',
            'plasticity.enable',
            'memory.enable',
            'development.enable'
        ]
        
        return any(critical in param_name for critical in performance_critical)
        
    def determineTargetUseCase(self, steps_per_second, memory_usage):
        """Determine target use case for configuration"""
        if steps_per_second > 1000 and memory_usage < 500 * 1024 * 1024:
            return ['real_time_interaction', 'mobile_devices', 'embedded_systems']
        elif steps_per_second > 100 and memory_usage < 2 * 1024 * 1024 * 1024:
            return ['research_simulation', 'laboratory_research', 'small_scale_deployment']
        elif steps_per_second > 10 and memory_usage < 10 * 1024 * 1024 * 1024:
            return ['large_scale_research', 'high_performance_computing', 'server_deployment']
        else:
            return ['legacy_compatibility', 'testing', 'development']
```

### 4. Performance Analysis Tools

#### PerformanceAnalyzer Class
```python
class PerformanceAnalyzer:
    """Advanced performance analysis for NLM simulations"""
    
    def __init__(self, profiler_data):
        self.profiler_data = profiler_data
        self.step_metrics = profiler_data.get('step_metrics', {})
        self.summary = profiler_data.get('summary', {})
        
    def analyzeStepPerformance(self):
        """Analyze performance of individual simulation steps"""
        if not self.step_metrics:
            return None
            
        analysis = {
            'total_steps': len(self.step_metrics),
            'performance_distribution': self.analyzePerformanceDistribution(),
            'step_correlations': self.analyzeStepCorrelations(),
            'bottleneck_identification': self.identifyPerformanceBottlenecks(),
            'optimization_potential': self.analyzeOptimizationPotential()
        }
        
        return analysis
        
    def analyzePerformanceDistribution(self):
        """Analyze distribution of step performance"""
        elapsed_times = [metrics['elapsed_time'] for metrics in self.step_metrics.values()]
        
        if not elapsed_times:
            return None
            
        # Calculate statistics
        mean_time = sum(elapsed_times) / len(elapsed_times)
        median_time = self.calculateMedian(elapsed_times)
        std_time = self.calculateStdDev(elapsed_times)
        
        # Identify outliers (steps > 2 standard deviations from mean)
        outliers = [t for t in elapsed_times if abs(t - mean_time) > 2 * std_time]
        
        return {
            'mean': mean_time,
            'median': median_time,
            'std': std_time,
            'min': min(elapsed_times),
            'max': max(elapsed_times),
            'count': len(elapsed_times),
            'outliers': {
                'count': len(outliers),
                'percentage': len(outliers) / len(elapsed_times) * 100,
                'times': outliers
            },
            'percentiles': {
                'p50': median_time,
                'p90': self.calculatePercentile(elapsed_times, 90),
                'p95': self.calculatePercentile(elapsed_times, 95),
                'p99': self.calculatePercentile(elapsed_times, 99)
            }
        }
        
    def analyzeStepCorrelations(self):
        """Analyze correlations between different performance metrics"""
        correlations = {}
        
        # Get all available metrics
        metrics = self.getAvailableMetrics()
        
        # Calculate correlations between metrics
        metric_names = list(metrics.keys())
        
        for i, metric1 in enumerate(metric_names):
            for j, metric2 in enumerate(metric_names[i+1:], i+1):
                correlation = self.calculateCorrelation(
                    metrics[metric1], metrics[metric2]
                )
                
                if abs(correlation) > 0.1:  # Only include meaningful correlations
                    correlations[f"{metric1}_vs_{metric2}"] = {
                        'correlation': correlation,
                        'strength': self.determineCorrelationStrength(correlation),
                        'significance': self.calculateCorrelationSignificance(correlation, len(metrics[metric1]))
                    }
                    
        return correlations
        
    def getAvailableMetrics(self):
        """Get all available performance metrics"""
        metrics = {}
        
        # Extract metrics from step data
        for step, data in self.step_metrics.items():
            for metric_name, metric_value in data.items():
                if metric_name not in metrics:
                    metrics[metric_name] = []
                metrics[metric_name].append(metric_value)
                
        return metrics
        
    def identifyPerformanceBottlenecks(self):
        """Identify performance bottlenecks"""
        bottlenecks = []
        
        # Analyze slow steps
        slow_steps_threshold = self.summary.get('average_step_time', 0) * 2
        slow_steps = [
            (step, metrics) for step, metrics in self.step_metrics.items()
            if metrics['elapsed_time'] > slow_steps_threshold
        ]
        
        if slow_steps:
            bottlenecks.append({
                'type': 'slow_steps',
                'description': f"Steps taking > {slow_steps_threshold:.3f}s",
                'affected_steps': [step for step, _ in slow_steps],
                'average_slow_time': sum(t['elapsed_time'] for _, t in slow_steps) / len(slow_steps),
                'severity': self.calculateBottleneckSeverity(len(slow_steps), slow_steps)
            })
            
        # Analyze memory growth
        memory_usages = [metrics['memory_usage']['rss'] for metrics in self.step_metrics.values()]
        if memory_usages:
            memory_growth = memory_usages[-1] - memory_usages[0]
            if memory_growth > 1024 * 1024:  # > 1MB growth
                bottlenecks.append({
                    'type': 'memory_growth',
                    'description': f"Memory growth of {memory_growth / 1024 / 1024:.2f} MB",
                    'severity': 'high' if memory_growth > 10 * 1024 * 1024 else 'medium'
                })
                
        # Analyze step time variations
        step_times = [metrics['elapsed_time'] for metrics in self.step_metrics.values()]
        if step_times:
            coefficient_of_variation = self.calculateCoefficientOfVariation(step_times)
            if coefficient_of_variation > 0.5:
                bottlenecks.append({
                    'type': 'performance_variability',
                    'description': f"High step time variability (CV={coefficient_of_variation:.2f})",
                    'severity': 'medium'
                })
                
        return bottlenecks
        
    def analyzeOptimizationPotential(self):
        """Analyze optimization potential"""
        potential = []
        
        # Check for memory optimization opportunities
        memory_stats = self.summary.get('memory_stats', {})
        if memory_stats.get('growth_rate', 0) > 1024 * 1024:  # > 1MB per step
            potential.append({
                'area': 'memory_optimization',
                'description': "Memory leak or inefficient memory usage detected",
                'potential_improvement': "10-50% reduction in memory usage",
                'effort': 'medium'
            })
            
        # Check for computational optimization opportunities
        if self.summary.get('cpu_stats', {}).get('average_cpu', 0) > 80:
            potential.append({
                'area': 'computational_optimization',
                'description': "CPU underutilized or computational bottlenecks",
                'potential_improvement': "20-40% improvement in computational efficiency",
                'effort': 'high'
            })
            
        # Check for step time optimization opportunities
        step_analysis = self.analyzeStepPerformance()
        if step_analysis and step_analysis['performance_distribution']['outliers']['percentage'] > 10:
            potential.append({
                'area': 'step_optimization',
                'description': "Significant number of slow steps detected",
                'potential_improvement': "30-60% reduction in step times",
                'effort': 'medium'
            })
            
        return potential
        
    def generatePerformanceReport(self):
        """Generate comprehensive performance report"""
        report = {
            'overview': {
                'total_duration': self.summary.get('total_duration', 0),
                'total_steps': self.summary.get('total_steps', 0),
                'steps_per_second': self.summary.get('steps_per_second', 0),
                'analysis_timestamp': time.time()
            },
            'performance_metrics': {
                'step_performance': self.analyzeStepPerformance(),
                'brain_state_variations': self.summary.get('brain_state_stats', {}),
                'resource_usage': self.analyzeResourceUsage(),
                'correlations': self.analyzeStepCorrelations()
            },
            'bottlenecks_and_issues': self.identifyPerformanceBottlenecks(),
            'optimization_potential': self.analyzeOptimizationPotential(),
            'recommendations': self.generatePerformanceRecommendations(),
            'visualization_data': self.prepareVisualizationData()
        }
        
        return report
        
    def prepareVisualizationData(self):
        """Prepare data for performance visualization"""
        visualization_data = {
            'time_series': {
                'step_times': [],
                'memory_usage': [],
                'cpu_usage': [],
                'steps_per_second': []
            },
            'scatter_plots': {
                'step_time_vs_memory': [],
                'step_time_vs_cpu': [],
                'memory_vs_cpu': []
            },
            'histograms': {
                'step_time_distribution': [],
                'memory_distribution': [],
                'cpu_distribution': []
            },
            'correlation_matrix': self.createCorrelationMatrix()
        }
        
        # Populate time series data
        for step, metrics in sorted(self.step_metrics.items()):
            visualization_data['time_series']['step_times'].append({
                'step': step,
                'time': metrics['timestamp'] - self.summary.get('total_duration', 0),
                'duration': metrics['elapsed_time']
            })
            
            visualization_data['time_series']['memory_usage'].append({
                'step': step,
                'time': metrics['timestamp'] - self.summary.get('total_duration', 0),
                'memory': metrics['memory_usage']['rss'] / (1024 * 1024)  # Convert to MB
            })
            
            visualization_data['time_series']['cpu_usage'].append({
                'step': step,
                'time': metrics['timestamp'] - self.summary.get('total_duration', 0),
                'cpu': metrics['cpu_usage']
            })
            
            visualization_data['time_series']['steps_per_second'].append({
                'step': step,
                'time': metrics['timestamp'] - self.summary.get('total_duration', 0),
                'steps_per_second': metrics['steps_per_second']
            })
            
        return visualization_data
```

### 5. Automated Performance Testing

#### AutomatedPerformanceTester Class
```python
class AutomatedPerformanceTester:
    """Automated performance testing for NLM"""
    
    def __init__(self, brain_factory, world_factory=None):
        self.brain_factory = brain_factory
        self.world_factory = world_factory or (lambda: None)
        self.test_suite = []
        self.results = {}
        
    def addPerformanceTest(self, name, test_function, description, 
                          config_variations=None, duration=60):
        """Add performance test to test suite"""
        test = {
            'name': name,
            'function': test_function,
            'description': description,
            'config_variations': config_variations or [{}],
            'duration': duration,
            'parameters': self.getTestParameters(test_function)
        }
        self.test_suite.append(test)
        
    def getTestParameters(self, test_function):
        """Extract parameters from test function"""
        sig = inspect.signature(test_function)
        parameters = {}
        
        for param_name, param in sig.parameters.items():
            if param_name not in ['self', 'name', 'description']:
                default_value = param.default if param.default != inspect.Parameter.empty else None
                parameters[param_name] = {
                    'type': self.inferParameterType(param.annotation),
                    'default': default_value,
                    'required': default_value is None
                }
                
        return parameters
        
    def runPerformanceTestSuite(self, parallel=True):
        """Run entire performance test suite"""
        if parallel:
            return self._runParallelTests()
        else:
            return self._runSequentialTests()
            
    def _runSequentialTests(self):
        """Run tests sequentially"""
        results = {}
        
        for test in self.test_suite:
            print(f"Running test: {test['name']}")
            test_results = self._runSingleTest(test)
            results[test['name']] = test_results
            
        self.results.update(results)
        return results
        
    def _runParallelTests(self):
        """Run tests in parallel"""
        import concurrent.futures
        
        results = {}
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
            # Submit all tests
            future_to_test = {
                executor.submit(self._runSingleTest, test): test
                for test in self.test_suite
            }
            
            # Collect results
            for future in concurrent.futures.as_completed(future_to_test):
                test = future_to_test[future]
                try:
                    test_results = future.result()
                    results[test['name']] = test_results
                    print(f"Completed test: {test['name']}")
                except Exception as e:
                    results[test['name']] = {
                        'status': 'error',
                        'error': str(e),
                        'details': None
                    }
                    print(f"Test failed: {test['name']} - {e}")
                    
        self.results.update(results)
        return results
        
    def _runSingleTest(self, test):
        """Run single performance test"""
        test_start_time = time.time()
        
        try:
            # Generate configurations to test
            configurations = self.generateTestConfigurations(test)
            
            test_results = []
            
            for i, config in enumerate(configurations):
                print(f"  Running configuration {i + 1}/{len(configurations)}")
                
                # Create brain and world
                brain = self.brain_factory(config)
                world = self.world_factory(config) if self.world_factory else None
                
                # Initialize brain
                if hasattr(brain, 'initialize'):
                    brain.initialize()
                    
                # Run test function
                result = test['function'](brain, world, config)
                
                # Store results
                test_results.append({
                    'configuration_index': i,
                    'config': config,
                    'result': result,
                    'status': 'success'
                })
                
            test_end_time = time.time()
            
            return {
                'status': 'completed',
                'duration': test_end_time - test_start_time,
                'configurations_tested': len(configurations),
                'results': test_results
            }
            
        except Exception as e:
            return {
                'status': 'error',
                'error': str(e),
                'duration': time.time() - test_start_time
            }
            
    def generateTestConfigurations(self, test):
        """Generate test configurations"""
        if not test['config_variations']:
            return [{}]
            
        configurations = []
        
        # Base configuration
        base_config = {}
        configurations.append(base_config)
        
        # Generate variations
        for variation in test['config_variations']:
            varied_config = self.applyVariation(base_config, variation)
            configurations.append(varied_config)
            
        return configurations
        
    def applyVariation(self, base_config, variation):
        """Apply variation to configuration"""
        varied_config = copy.deepcopy(base_config)
        
        for param_name, param_value in variation.items():
            # Navigate to nested parameters
            keys = param_name.split('.')
            current = varied_config
            
            for key in keys[:-1]:
                if key not in current:
                    current[key] = {}
                current = current[key]
                
            # Set the final parameter
            current[keys[-1]] = param_value
            
        return varied_config
        
    def analyzeTestResults(self):
        """Analyze test results and generate insights"""
        analysis = {
            'successful_tests': 0,
            'failed_tests': 0,
            'average_performance': {},
            'performance_trends': {},
            'optimization_opportunities': [],
            'recommendations': []
        }
        
        # Analyze each test
        for test_name, test_result in self.results.items():
            if test_result['status'] == 'completed':
                analysis['successful_tests'] += 1
                
                # Analyze performance across configurations
                if test_result['results']:
                    test_performance = self.analyzeTestPerformance(test_result['results'])
                    analysis['average_performance'][test_name] = test_performance
                    analysis['performance_trends'][test_name] = self.identifyPerformanceTrends(
                        test_result['results']
                    )
            else:
                analysis['failed_tests'] += 1
                
        # Generate optimization opportunities
        analysis['optimization_opportunities'] = self.generateOptimizationOpportunities(analysis)
        
        # Generate recommendations
        analysis['recommendations'] = self.generateTestRecommendations(analysis)
        
        return analysis
        
    def analyzeTestPerformance(self, test_results):
        """Analyze performance of test results"""
        if not test_results:
            return None
            
        # Extract performance metrics
        execution_times = []
        memory_usages = []
        steps_per_second = []
        
        for result in test_results:
            if 'result' in result and result['result']:
                # Extract metrics from result
                metrics = self.extractPerformanceMetrics(result['result'])
                execution_times.append(metrics.get('execution_time', 0))
                memory_usages.append(metrics.get('memory_usage', 0))
                steps_per_second.append(metrics.get('steps_per_second', 0))
                
        if not execution_times:
            return None
            
        return {
            'execution_time': {
                'mean': sum(execution_times) / len(execution_times),
                'min': min(execution_times),
                'max': max(execution_times),
                'std': self.calculateStdDev(execution_times)
            },
            'memory_usage': {
                'mean': sum(memory_usages) / len(memory_usages),
                'min': min(memory_usages),
                'max': max(memory_usages),
                'std': self.calculateStdDev(memory_usages)
            },
            'steps_per_second': {
                'mean': sum(steps_per_second) / len(steps_per_second),
                'min': min(steps_per_second),
                'max': max(steps_per_second),
                'std': self.calculateStdDev(steps_per_second)
            }
        }
        
    def extractPerformanceMetrics(self, result):
        """Extract performance metrics from test result"""
        # This would depend on the specific test function
        # For now, return a placeholder
        return {
            'execution_time': 0,
            'memory_usage': 0,
            'steps_per_second': 0
        }
        
    def generateOptimizationOpportunities(self, analysis):
        """Generate optimization opportunities based on test results"""
        opportunities = []
        
        # Compare performance across different configurations
        for test_name, performance in analysis['average_performance'].items():
            if performance and 'execution_time' in performance:
                # Check for large variations in execution time
                time_std = performance['execution_time']['std']
                time_mean = performance['execution_time']['mean']
                
                if time_mean > 0 and time_std / time_mean > 0.2:
                    opportunities.append({
                        'test': test_name,
                        'area': 'execution_time_optimization',
                        'description': f"High variability in execution time (CV={time_std/time_mean:.2f})",
                        'potential_improvement': "20-40% reduction in execution time variability",
                        'priority': 'medium'
                    })
                    
        return opportunities
        
    def generateTestRecommendations(self, analysis):
        """Generate recommendations for performance testing"""
        recommendations = []
        
        # Recommend tests that consistently perform well
        if analysis['average_performance']:
            best_test = max(analysis['average_performance'].items(), 
                          key=lambda x: x[1]['steps_per_second']['mean'] if x[1] else 0)
            
            if best_test[1]:
                recommendations.append({
                    'type': 'best_practice',
                    'description': f"Configuration {best_test[0]} shows best performance",
                    'optimal_parameters': best_test[1].get('optimal_parameters', {}),
                    'target_metrics': {
                        'steps_per_second': best_test[1]['steps_per_second']['mean'],
                        'memory_usage': best_test[1]['memory_usage']['mean']
                    }
                })
                
        # Recommend tests that failed for debugging
        if analysis['failed_tests'] > 0:
            recommendations.append({
                'type': 'debugging',
                'description': f"{analysis['failed_tests']} tests failed, investigate failures",
                'action': 'Review test configurations and implementation'
            })
            
        return recommendations
```

### 6. Performance Monitoring Dashboard

#### PerformanceDashboard Class
```python
class PerformanceDashboard:
    """Interactive performance monitoring dashboard"""
    
    def __init__(self, profiler, monitor, tester):
        self.profiler = profiler
        self.monitor = monitor
        self.tester = tester
        self.current_view = 'overview'
        self.time_range = None
        
    def createOverviewView(self):
        """Create overview view of performance metrics"""
        overview = {
            'current_metrics': self.getCurrentMetrics(),
            'recent_trends': self.getRecentTrends(),
            'active_alerts': self.getActiveAlerts(),
            'performance_summary': self.getPerformanceSummary(),
            'bottleneck_indicators': self.getBottleneckIndicators()
        }
        
        return overview
        
    def createTimeSeriesView(self, metric, time_range=None):
        """Create time series visualization of metrics"""
        if time_range:
            self.time_range = time_range
            
        metrics_data = self.getTimeSeriesData(metric)
        
        return {
            'metric': metric,
            'data_points': metrics_data,
            'statistics': self.calculateTimeSeriesStatistics(metrics_data),
            'anomalies': self.detectAnomalies(metrics_data)
        }
        
    def createScatterPlotView(self, x_metric, y_metric, time_range=None):
        """Create scatter plot visualization"""
        if time_range:
            self.time_range = time_range
            
        x_data = self.getMetricData(x_metric)
        y_data = self.getMetricData(y_metric)
        
        return {
            'x_metric': x_metric,
            'y_metric': y_metric,
            'data_points': self.createScatterDataPoints(x_data, y_data),
            'correlation': self.calculateCorrelation(x_data, y_data),
            'clusters': self.findDataClusters(x_data, y_data)
        }
        
    def createHistogramView(self, metric, time_range=None):
        """Create histogram visualization"""
        if time_range:
            self.time_range = time_range
            
        metric_data = self.getMetricData(metric)
        
        return {
            'metric': metric,
            'histogram': self.createHistogram(metric_data),
            'statistics': self.calculateHistogramStatistics(metric_data),
            'distribution_fit': self.fitDistribution(metric_data)
        }
        
    def createCorrelationMatrixView(self, metrics=None):
        """Create correlation matrix visualization"""
        if not metrics:
            metrics = ['step_time', 'memory_usage', 'cpu_usage', 'steps_per_second']
            
        correlation_matrix = {}
        
        for metric1 in metrics:
            correlation_matrix[metric1] = {}
            for metric2 in metrics:
                correlation = self.calculateCorrelation(
                    self.getMetricData(metric1),
                    self.getMetricData(metric2)
                )
                correlation_matrix[metric1][metric2] = correlation
                
        return {
            'metrics': metrics,
            'correlation_matrix': correlation_matrix,
            'interpretation': self.interpretCorrelationMatrix(correlation_matrix)
        }
        
    def getCurrentMetrics(self):
        """Get current real-time metrics"""
        if not self.monitor.performance_data:
            return None
            
        latest = self.monitor.performance_data[-1]
        
        return {
            'timestamp': latest['timestamp'],
            'steps_per_second': latest['steps_per_second'],
            'memory_usage_mb': latest['memory_rss'] / (1024 * 1024),
            'cpu_usage_percent': latest['cpu_percent'],
            'brain_metrics': latest['brain_metrics']
        }
        
    def getRecentTrends(self, time_range=300):  # Last 5 minutes
        """Get recent performance trends"""
        current_time = time.time()
        cutoff_time = current_time - time_range
        
        recent_data = [d for d in self.monitor.performance_data if d['timestamp'] > cutoff_time]
        
        if not recent_data:
            return None
            
        return {
            'steps_per_second_trend': self.calculateTrend(
                [d['steps_per_second'] for d in recent_data]
            ),
            'memory_usage_trend': self.calculateTrend(
                [d['memory_rss'] for d in recent_data]
            ),
            'cpu_usage_trend': self.calculateTrend(
                [d['cpu_percent'] for d in recent_data]
            ),
            'average_performance': {
                'steps_per_second': sum(d['steps_per_second'] for d in recent_data) / len(recent_data),
                'memory_usage_mb': sum(d['memory_rss'] for d in recent_data) / len(recent_data) / (1024 * 1024),
                'cpu_usage_percent': sum(d['cpu_percent'] for d in recent_data) / len(recent_data)
            }
        }
        
    def getActiveAlerts(self):
        """Get currently active alerts"""
        # This would integrate with the monitoring system's alert system
        return []
        
    def getPerformanceSummary(self):
        """Get performance summary"""
        if not self.monitor.performance_data:
            return None
            
        all_data = self.monitor.getStatistics()
        
        return {
            'uptime': len(self.monitor.performance_data) * self.monitor.update_interval,
            'total_steps_completed': sum(d['steps_per_second'] * self.monitor.update_interval 
                                       for d in self.monitor.performance_data),
            'average_performance': all_data.get('steps_per_second', {}),
            'resource_efficiency': all_data.get('resource_efficiency', {})
        }
        
    def getBottleneckIndicators(self):
        """Get bottleneck indicators"""
        if not self.profiler.results:
            return None
            
        # Analyze profiling results for bottlenecks
        bottleneck_indicators = []
        
        for profile_key, profile_data in self.profiler.results.items():
            if profile_data.get('status') == 'completed':
                summary = profile_data['profile_data'].get('summary', {})
                
                # Check for performance issues
                if summary.get('steps_per_second', 0) < 100:
                    bottleneck_indicators.append({
                        'type': 'throughput',
                        'severity': 'high',
                        'description': f"Low throughput: {summary.get('steps_per_second', 0):.1f} steps/sec",
                        'affected_profiles': [profile_key]
                    })
                    
                memory_stats = summary.get('memory_stats', {})
                if memory_stats.get('growth_rate', 0) > 1024 * 1024:
                    bottleneck_indicators.append({
                        'type': 'memory',
                        'severity': 'medium',
                        'description': f"High memory growth rate: {memory_stats.get('growth_rate', 0) / 1024 / 1024:.2f} MB/step",
                        'affected_profiles': [profile_key]
                    })
                    
        return bottleneck_indicators
        
    def generateInteractiveReport(self):
        """Generate interactive performance report"""
        report = {
            'overview': self.createOverviewView(),
            'time_series': {
                'step_time': self.createTimeSeriesView('step_time'),
                'memory_usage': self.createTimeSeriesView('memory_usage'),
                'cpu_usage': self.createTimeSeriesView('cpu_usage'),
                'steps_per_second': self.createTimeSeriesView('steps_per_second')
            },
            'scatter_plots': {
                'step_time_vs_memory': self.createScatterPlotView('step_time', 'memory_usage'),
                'step_time_vs_cpu': self.createScatterPlotView('step_time', 'cpu_usage'),
                'memory_vs_cpu': self.createScatterPlotView('memory_usage', 'cpu_usage')
            },
            'histograms': {
                'step_time_distribution': self.createHistogramView('step_time'),
                'memory_distribution': self.createHistogramView('memory_usage'),
                'cpu_distribution': self.createHistogramView('cpu_usage')
            },
            'correlation_matrix': self.createCorrelationMatrixView(),
            'recommendations': self.generatePerformanceRecommendations()
        }
        
        return report
```

## Summary

These performance monitoring and profiling tools provide comprehensive capabilities for:

### 1. Advanced Performance Profiling
- **PerformanceProfiler**: Detailed profiling of individual simulation steps
- **Real-time monitoring**: Continuous performance tracking during simulations
- **Benchmarking**: Systematic performance testing with configurable variations

### 2. Performance Analysis Tools
- **PerformanceAnalyzer**: Advanced statistical analysis of performance data
- **Automated testing**: Automated performance testing with configuration variations
- **Interactive dashboard**: Real-time visualization of performance metrics

### 3. Performance Optimization
- **Bottleneck identification**: Automatic detection of performance bottlenecks
- **Trend analysis**: Identification of performance trends and patterns
- **Recommendations**: Data-driven recommendations for optimization

### 4. Monitoring and Alerting
- **Real-time monitoring**: Continuous monitoring of system performance
- **Alert system**: Automated alerts for performance issues
- **Dashboard integration**: Interactive visualizations and reports

### 5. Integration and Compatibility
- **Standardized API**: Consistent interface across all performance tools
- **Flexible configuration**: Support for various simulation scenarios
- **Extensible design**: Easy addition of new performance metrics and analysis methods

These tools enable developers to:
- **Optimize performance**: Identify and resolve performance bottlenecks
- **Monitor efficiency**: Track resource utilization and efficiency
- **Validate improvements**: Quantify performance improvements from optimizations
- **Compare configurations**: Systematically compare different simulation configurations
- **Generate insights**: Extract meaningful insights from performance data
- **Make informed decisions**: Base decisions on data-driven performance analysis

The comprehensive performance monitoring framework makes NLM simulations more efficient, reliable, and optimized for various use cases and deployment scenarios.