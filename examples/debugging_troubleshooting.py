#!/usr/bin/env python3
"""
NLM Debugging and Troubleshooting Examples

This file demonstrates debugging and troubleshooting techniques for NLM including
memory leak detection, neuromodulation debugging, simulation state inspection,
and performance analysis.

Requirements:

### 5. **Debugging and Testing Examples**
- **Memory Debugging**: Memory leak detection and debugging
- **Neuromodulation Debugging**: Signal monitoring and validation
- **Simulation Debugging**: Step-by-step simulation inspection
- **Performance Profiling**: CPU and memory profiling
"""

import pynlm
import logging
import time
import gc
import psutil
import os
from typing import List, Dict, Any, Optional, Tuple
from dataclasses import dataclass
from contextlib import contextmanager

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def print_header(title: str):
    """Print a formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

# ============================================================================
# 1. Memory Debugging Examples
# ============================================================================

class MemoryUsageTracker:
    """Track memory usage for debugging and leak detection."""
    
    def __init__(self, sample_interval: float = 0.1):
        self.sample_interval = sample_interval
        self.memory_samples = []
        self.allocation_records = []
        self.start_time = time.time()
        self.baseline_memory = self._get_process_memory()
        
    def _get_process_memory(self) -> float:
        """Get current process memory usage in MB."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / (1024 * 1024)
        
    def sample_memory(self, operation: str = None):
        """Sample current memory usage."""
        current_memory = self._get_process_memory()
        timestamp = time.time()
        
        sample = {
            'timestamp': timestamp,
            'memory_mb': current_memory,
            'operation': operation,
            'delta_from_baseline': current_memory - self.baseline_memory
        }
        
        self.memory_samples.append(sample)
        return sample
        
    def record_allocation(self, operation: str, size_mb: float):
        """Record memory allocation."""
        self.allocation_records.append({
            'timestamp': time.time(),
            'operation': operation,
            'size_mb': size_mb,
            'memory_before': self._get_process_memory()
        })
        
    def record_deallocation(self, operation: str):
        """Record memory deallocation."""
        if self.allocation_records:
            last_record = self.allocation_records[-1]
            last_record['memory_after'] = self._get_process_memory()
            last_record['deallocated'] = True
            
    def analyze_memory_growth(self) -> Dict[str, Any]:
        """Analyze memory growth for potential leaks."""
        if len(self.memory_samples) < 2:
            return {'status': 'insufficient_data', 'leaks_detected': False}
            
        # Calculate overall memory growth
        final_memory = self.memory_samples[-1]['memory_mb']
        initial_memory = self.memory_samples[0]['memory_mb']
        total_growth = final_memory - initial_memory
        
        # Check for steady memory growth
        recent_samples = self.memory_samples[-10:]  # Last 10 samples
        if len(recent_samples) >= 3:
            recent_growth = recent_samples[-1]['memory_mb'] - recent_samples[0]['memory_mb']
            growth_rate = recent_growth / len(recent_samples)
        else:
            growth_rate = 0
            
        # Check allocation/deallocation balance
        allocations = sum(r.get('size_mb', 0) for r in self.allocation_records)
        deallocations = sum(r.get('size_mb', 0) for r in self.allocation_records if r.get('deallocated'))
        
        # Detect potential leaks
        leaks_detected = (
            total_growth > 10.0 or  # More than 10MB total growth
            growth_rate > 0.5 or   # More than 0.5MB per sample
            abs(allocations - deallocations) > 5.0  # Significant imbalance
        )
        
        return {
            'status': 'analysis_complete',
            'leaks_detected': leaks_detected,
            'total_growth_mb': total_growth,
            'growth_rate_mb_per_sample': growth_rate,
            'allocation_mb': allocations,
            'deallocation_mb': deallocations,
            'allocation_deallocation_balance': deallocations / allocations if allocations > 0 else 0,
            'sample_count': len(self.memory_samples),
            'allocation_count': len(self.allocation_records)
        }
        
    def get_memory_timeline(self) -> List[Dict[str, float]]:
        """Get memory usage timeline."""
        return self.memory_samples.copy()
        
    def generate_memory_report(self) -> str:
        """Generate memory analysis report."""
        analysis = self.analyze_memory_growth()
        
        report = f"Memory Usage Analysis Report\n"
        report += f"=" * 50 + "\n"
        report += f"Total samples: {analysis['sample_count']}\n"
        report += f"Allocation records: {analysis['allocation_count']}\n"
        report += f"Total memory growth: {analysis['total_growth_mb']:.2f} MB\n"
        report += f"Growth rate: {analysis['growth_rate_mb_per_sample']:.3f} MB per sample\n"
        report += f"Allocations: {analysis['allocation_mb']:.2f} MB\n"
        report += f"Deallocations: {analysis['deallocation_mb']:.2f} MB\n"
        report += f"Allocation/Deallocation ratio: {analysis['allocation_deallocation_balance']:.2f}\n"
        
        if analysis['leaks_detected']:
            report += f"\n⚠️  MEMORY LEAK DETECTED!\n"
            report += f"Recommendations:\n"
            report += f"1. Check for unmanaged memory allocations\n"
            report += f"2. Ensure proper cleanup of allocated objects\n"
            report += f"3. Use memory pools for frequent allocations\n"
            report += f"4. Implement garbage collection if needed\n"
        else:
            report += f"\n✅ No significant memory leaks detected\n"
            
        return report
class GarbageCollectionDebugger:
    """Debug garbage collection and object lifecycle."""
    
    def __init__(self):
        self.generation_counts = {}
        self.object_instances = {}
        self.gc_events = []
        
    def track_object_creation(self, obj_id: str, obj_type: str, initial_size: int = 0):
        """Track object creation."""
        if obj_id not in self.object_instances:
            self.object_instances[obj_id] = {
                'type': obj_type,
                'initial_size': initial_size,
                'current_size': initial_size,
                'creation_time': time.time(),
                'last_gc_time': None,
                'generation': 0
            }
            self.generation_counts[obj_type] = self.generation_counts.get(obj_type, 0) + 1
            
    def track_object_growth(self, obj_id: str, additional_size: int = 0):
        """Track object size growth."""
        if obj_id in self.object_instances:
            self.object_instances[obj_id]['current_size'] += additional_size
            
    def record_gc_event(self, generation: int):
        """Record garbage collection event."""
        self.gc_events.append({
            'timestamp': time.time(),
            'generation': generation,
            'objects_collected': self._get_objects_collected(generation)
        })
        
    def _get_objects_collected(self, generation: int) -> List[str]:
        """Get objects collected in this GC generation."""
        collected = []
        
        for obj_id, obj_info in self.object_instances.items():
            if obj_info['generation'] < generation:
                collected.append(obj_id)
                
        return collected
        
    def update_object_generation(self, obj_id: str, new_generation: int):
        """Update object generation when it's garbage collected."""
        if obj_id in self.object_instances:
            self.object_instances[obj_id]['generation'] = new_generation
            self.object_instances[obj_id]['last_gc_time'] = time.time()
            
    def analyze_object_lifecycle(self) -> Dict[str, Any]:
        """Analyze object lifecycle and memory patterns."""
        if not self.object_instances:
            return {'status': 'no_objects', 'analysis_complete': False}
            
        analysis = {
            'total_objects': len(self.object_instances),
            'total_gc_events': len(self.gc_events),
            'object_types': list(self.generation_counts.keys()),
            'type_counts': self.generation_counts,
            'long_lived_objects': 0,
            'frequently_collected': [],
            'memory_leaks': []
        }
        
        # Analyze object lifetimes
        for obj_id, obj_info in self.object_instances.items():
            lifetime = time.time() - obj_info['creation_time']
            
            if lifetime > 10.0:  # Objects living longer than 10 seconds
                analysis['long_lived_objects'] += 1
                
            # Check if object was collected recently
            if obj_info['last_gc_time'] and (time.time() - obj_info['last_gc_time']) < 5.0:
                analysis['frequently_collected'].append(obj_id)
                
        # Check for memory leaks
        for obj_id, obj_info in self.object_instances.items():
            if obj_info['last_gc_time'] is None:
                # Object never collected
                age = time.time() - obj_info['creation_time']
                if age > 5.0:  # Older than 5 seconds
                    analysis['memory_leaks'].append({
                        'object_id': obj_id,
                        'type': obj_info['type'],
                        'age_seconds': age,
                        'current_size': obj_info['current_size']
                    })
        
        return analysis
        
    def generate_lifecycle_report(self) -> str:
        """Generate object lifecycle analysis report."""
        analysis = self.analyze_object_lifecycle()
        
        report = f"Object Lifecycle Analysis Report\n"
        report += f"=" * 50 + "\n"
        report += f"Total tracked objects: {analysis['total_objects']}\n"
        report += f"Garbage collection events: {analysis['total_gc_events']}\n"
        report += f"\nObject types and counts:\n"
        for obj_type, count in analysis['type_counts'].items():
            report += f"  {obj_type}: {count}\n"
            
        report += f"\nLong-lived objects (>10s): {analysis['long_lived_objects']}\n"
        report += f"Frequently collected objects: {len(analysis['frequently_collected'])}\n"
        
        if analysis['frequently_collected']:
            report += f"  {', '.join(analysis['frequently_collected'][:5])}\n"
            
        report += f"\nPotential memory leaks: {len(analysis['memory_leaks'])}\n"
        if analysis['memory_leaks']:
            for leak in analysis['memory_leaks']:
                report += f"  - {leak['object_id']} ({leak['type']}): age={leak['age_seconds']:.1f}s, size={leak['current_size']}\n"
                
        return report
class MemoryLeakDetector:
    """Advanced memory leak detection using statistical analysis."""
    
    def __init__(self, window_size: int = 100):
        self.window_size = window_size
        self.memory_window = []
        self.allocation_patterns = {}
        
    def add_memory_sample(self, memory_mb: float, operation: str = None):
        """Add memory sample for analysis."""
        self.memory_window.append({
            'timestamp': time.time(),
            'memory_mb': memory_mb,
            'operation': operation
        })
        
        # Maintain window size
        if len(self.memory_window) > self.window_size:
            self.memory_window.pop(0)
            
        # Update operation patterns
        if operation:
            if operation not in self.allocation_patterns:
                self.allocation_patterns[operation] = []
            self.allocation_patterns[operation].append(memory_mb)
            
            # Keep only recent samples for each operation
            if len(self.allocation_patterns[operation]) > window_size:
                self.allocation_patterns[operation].pop(0)
                
    def detect_leaks(self) -> Dict[str, Any]:
        """Detect memory leaks using statistical analysis."""
        if len(self.memory_window) < 10:
            return {'status': 'insufficient_data', 'leaks_detected': False}
            
        # Calculate trends
        recent_memory = [s['memory_mb'] for s in self.memory_window[-10:]]
        older_memory = [s['memory_mb'] for s in self.memory_window[:-10]] if len(self.memory_window) > 10 else []
        
        # Linear regression for trend detection
        trend = self._calculate_linear_trend(recent_memory)
        
        # Statistical outlier detection
        outliers = self._detect_outliers(recent_memory)
        
        # Operation-specific leak detection
        operation_leaks = {}
        for operation, memory_values in self.allocation_patterns.items():
            if len(memory_values) >= 10:
                op_trend = self._calculate_linear_trend(memory_values[-10:])
                op_outliers = self._detect_outliers(memory_values[-10:])
                
                operation_leaks[operation] = {
                    'trend': op_trend,
                    'outliers': len(op_outliers),
                    'average': sum(memory_values[-10:]) / len(memory_values[-10:]) if memory_values else 0,
                    'potential_leak': abs(op_trend['slope']) > 0.01 or len(op_outliers) > 3
                }
        
        # Overall leak detection
        leaks_detected = (
            trend['slope'] > 0.01 or  # Positive trend
            len(outliers) > 3 or      # Many outliers
            any(op_info['potential_leak'] for op_info in operation_leaks.values())
        )
        
        return {
            'status': 'analysis_complete',
            'leaks_detected': leaks_detected,
            'overall_trend': trend,
            'outliers': outliers,
            'operation_leaks': operation_leaks,
            'window_size': len(self.memory_window),
            'sample_rate': 1.0 / (self.memory_window[-1]['timestamp'] - self.memory_window[0]['timestamp']) if len(self.memory_window) > 1 else 0
        }
        
    def _calculate_linear_trend(self, values: List[float]) -> Dict[str, Any]:
        """Calculate linear trend using simple regression."""
        if len(values) < 2:
            return {'slope': 0, 'intercept': 0, 'r_squared': 0}
            
        n = len(values)
        x = list(range(n))
        y = values
        
        # Calculate sums
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        sum_x2 = sum(xi ** 2 for xi in x)
        sum_y2 = sum(yi ** 2 for yi in y)
        
        # Calculate slope and intercept
        denominator = n * sum_x2 - sum_x ** 2
        if denominator == 0:
            return {'slope': 0, 'intercept': 0, 'r_squared': 0}
            
        slope = (n * sum_xy - sum_x * sum_y) / denominator
        intercept = (sum_y - slope * sum_x) / n
        
        # Calculate R-squared
        ss_total = sum((yi - sum_y / n) ** 2 for yi in y)
        ss_residual = sum((yi - (slope * xi + intercept)) ** 2 for xi, yi in zip(x, y))
        
        r_squared = 1 - (ss_residual / ss_total) if ss_total > 0 else 0
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_squared,
            'prediction': f"memory_mb = {slope:.4f} * time + {intercept:.2f}"
        }
        
    def _detect_outliers(self, values: List[float]) -> List[int]:
        """Detect outliers using IQR method."""
        if len(values) < 4:
            return []
            
        # Calculate quartiles
        sorted_values = sorted(values)
        q1 = sorted_values[len(values) // 4]
        q3 = sorted_values[3 * len(values) // 4]
        iqr = q3 - q1
        
        # Define outlier bounds
        lower_bound = q1 - 1.5 * iqr
        upper_bound = q3 + 1.5 * iqr
        
        # Detect outliers
        outliers = []
        for i, value in enumerate(values):
            if value < lower_bound or value > upper_bound:
                outliers.append(i)
                
        return outliers


def memory_leak_detection_example():
    """Demonstrate memory leak detection techniques."""
    print_header("1.1 Memory Leak Detection")
    
    print("\nMemory leak detection and analysis:")
    
    # Initialize memory tracker
    memory_tracker = MemoryUsageTracker(sample_interval=0.05)
    
    # Simulate memory allocations
    print("\n1.1.1 Simulating memory allocations:")
    
    # Create memory-intensive objects
    large_objects = []
    
    for i in range(10):
        # Allocate large memory blocks
        size_mb = 5.0 + i * 2.0  # Increasing size
        memory_tracker.record_allocation(f"object_{i}", size_mb)
        
        # Create object
        obj = {'data': [0.0] * (1000 * 1000)}  # 1 million floats
        large_objects.append(obj)
        
        # Sample memory
        sample = memory_tracker.sample_memory(f"create_object_{i}")
        print(f"  Object {i}: {size_mb:.1f} MB allocated, memory: {sample['memory_mb']:.2f} MB")
        
        # Sometimes free objects
        if i % 3 == 0 and i > 0:
            freed_obj = large_objects.pop(0)
            memory_tracker.record_deallocation(f"free_object_{i//3}")
            print(f"  Object {i//3} freed")
    
    # Force garbage collection
    print("\n1.1.2 Garbage collection...")
    gc.collect()
    memory_tracker.sample_memory("after_gc")
    
    # Analyze memory leaks
    print("\n1.1.3 Memory leak analysis:")
    
    memory_report = memory_tracker.generate_memory_report()
    print(memory_report)
    
    # Simulate memory leak
    print("\n1.1.4 Simulating memory leak...")
    
    # Create objects that won't be freed
    leaked_objects = []
    for i in range(5):
        obj = {'data': [0.0] * (2000 * 1000)}  # Larger objects
        leaked_objects.append(obj)
        memory_tracker.sample_memory(f"leaked_object_{i}")
    
    print("  Created objects without proper cleanup")
    print("  Memory should show continuous growth")
    
    # Final analysis
    final_analysis = memory_tracker.analyze_memory_growth()
    
    print(f"\nFinal memory analysis:")
    print(f"  Status: {'LEAK DETECTED' if final_analysis['leaks_detected'] else 'No leak detected'}")
    print(f"  Total growth: {final_analysis['total_growth_mb']:.2f} MB")
    print(f"  Growth rate: {final_analysis['growth_rate_mb_per_sample']:.3f} MB per sample")
    
    # Cleanup
    del large_objects
    del leaked_objects
    gc.collect()
    
    return memory_tracker


def garbage_collection_debugging_example():
    """Demonstrate garbage collection debugging."""
    print_header("1.2 Garbage Collection Debugging")
    
    print("\nGarbage collection debugging and object lifecycle analysis:")
    
    # Initialize GC debugger
    gc_debugger = GarbageCollectionDebugger()
    
    # Simulate object lifecycle
    print("\n1.2.1 Simulating object lifecycle:")
    
    # Create objects
    for i in range(5):
        obj_id = f"object_{i}"
        gc_debugger.track_object_creation(obj_id, "simulation_object", initial_size=100)
        
        # Grow object
        for j in range(5):
            gc_debugger.track_object_growth(obj_id, 50)
            time.sleep(0.01)  # Simulate work
        
        # Sometimes collect object
        if i % 2 == 0:
            gc.collect()
            gc_debugger.record_gc_event(1)
            gc_debugger.update_object_generation(obj_id, 1)
            print(f"  Object {obj_id} collected in GC generation 1")
        else:
            print(f"  Object {obj_id} retained")
    
    # Record some GC events
    for gen in range(2, 5):
        gc.collect()
        gc_debugger.record_gc_event(gen)
        print(f"  GC generation {gen} completed")
    
    # Analyze object lifecycle
    print("\n1.2.2 Object lifecycle analysis:")
    
    lifecycle_report = gc_debugger.generate_lifecycle_report()
    print(lifecycle_report)
    
    # Check for memory leaks
    analysis = gc_debugger.analyze_object_lifecycle()
    
    print(f"\nObject lifecycle analysis:")
    print(f"  Total objects: {analysis['total_objects']}")
    print(f"  GC events: {analysis['total_gc_events']}")
    print(f"  Long-lived objects: {analysis['long_lived_objects']}")
    print(f"  Frequently collected: {len(analysis['frequently_collected'])}")
    print(f"  Potential leaks: {len(analysis['memory_leaks'])}")
    
    if analysis['memory_leaks']:
        print(f"  Leaked objects: {', '.join([leak['object_id'] for leak in analysis['memory_leaks']])}")
    
    return gc_debugger


def advanced_memory_leak_detection_example():
    """Demonstrate advanced memory leak detection with statistical analysis."""
    print_header("1.3 Advanced Memory Leak Detection")
    
    print("\nAdvanced memory leak detection using statistical analysis:")
    
    # Initialize leak detector
    leak_detector = MemoryLeakDetector(window_size=50)
    
    # Simulate memory usage patterns
    print("\n1.3.1 Simulating memory usage patterns:")
    
    # Pattern 1: Normal memory usage
    print("  Pattern 1: Normal memory usage")
    for i in range(30):
        # Normal allocation/deallocation cycle
        memory_mb = 50 + (i % 5) * 2  # Fluctuates around 50-60MB
        leak_detector.add_memory_sample(memory_mb, "normal_cycle")
        time.sleep(0.01)
    
    # Pattern 2: Memory leak
    print("  Pattern 2: Memory leak (simulated)")
    for i in range(30):
        # Leaky allocation - memory keeps growing
        memory_mb = 80 + i * 1.5  # Linearly increasing
        leak_detector.add_memory_sample(memory_mb, "leaky_cycle")
        time.sleep(0.01)
    
    # Pattern 3: Burst allocation
    print("  Pattern 3: Burst allocation")
    for i in range(30):
        # Burst pattern - high memory then cleanup
        if i < 10:
            memory_mb = 100 + i * 10  # Burst
        elif i < 25:
            memory_mb = 150 - (i - 10) * 2  # Cleanup
        else:
            memory_mb = 50 + (i - 25) * 1  # Recovery
            
        leak_detector.add_memory_sample(memory_mb, "burst_cycle")
        time.sleep(0.01)
    
    # Detect leaks
    print("\n1.3.2 Leak detection results:")
    
    leak_analysis = leak_detector.detect_leaks()
    
    print(f"  Overall status: {'LEAK DETECTED' if leak_analysis['leaks_detected'] else 'No leak detected'}")
    print(f"  Trend slope: {leak_analysis['overall_trend']['slope']:.4f}")
    print(f"  R-squared: {leak_analysis['overall_trend']['r_squared']:.3f}")
    print(f"  Number of outliers: {len(leak_analysis['outliers'])}")
    
    print(f"\nOperation-specific leaks:")
    for operation, op_info in leak_analysis['operation_leaks'].items():
        status = "⚠️ LEAK" if op_info['potential_leak'] else "✅ OK"
        print(f"  {operation}: {status}, slope={op_info['trend']:.4f}, outliers={op_info['outliers']}")
    
    # Generate detailed report
    print(f"\n1.3.3 Detailed analysis:")
    
    # Check for specific leak patterns
    if leak_analysis['leaks_detected']:
        print(f"  🚨 Memory leak confirmed!")
        
        # Identify which pattern caused the leak
        leaky_operations = [op for op, info in leak_analysis['operation_leaks'].items() 
                          if info['potential_leak']]
        
        if leaky_operations:
            print(f"  Leaky operations: {', '.join(leaky_operations)}")
            
            # Find most leaky operation
            most_leaky = max(leaky_operations, key=lambda op: abs(leak_analysis['operation_leaks'][op]['trend']['slope']))
            slope = leak_analysis['operation_leaks'][most_leaky]['trend']['slope']
            
            print(f"  Most leaky operation: {most_leaky}")
            print(f"  Growth rate: {slope:.4f} MB per time unit")
            
            # Predict future memory usage
            future_memory = leak_analysis['overall_trend']['intercept'] + leak_analysis['overall_trend']['slope'] * 100
            print(f"  Predicted memory usage in 100 more samples: {future_memory:.1f} MB")
    
    return leak_detector


def memory_debugging_examples_summary():
    """Print summary of memory debugging examples."""
    print_header("Memory Debugging Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Memory usage tracking and sampling")
    print("✓ Memory leak detection using multiple techniques")
    print("✓ Garbage collection debugging and object lifecycle analysis")
    print("✓ Advanced statistical leak detection")
    print("✓ Memory allocation/deallocation tracking")
    print("✓ Object lifecycle analysis and leak identification")
    print("✓ Trend analysis and outlier detection")
    print("✓ Comprehensive reporting and analysis tools")

# ============================================================================
# 2. Neuromodulation Debugging Examples
# ============================================================================

class SignalValidator:
    """Validate neuromodulator signals and detect anomalies."""
    
    def __init__(self):
        self.signal_history = {}
        self.validation_rules = {
            'dopamine': {
                'normal_range': (0.0, 1.0),
                'valid_transitions': [(0.0, 1.0), (1.0, 0.0)],
                'max_rate_of_change': 0.2,
                'min_duration': 0.1
            },
            'acetylcholine': {
                'normal_range': (0.0, 1.0),
                'valid_transitions': [(0.0, 0.5), (0.5, 1.0), (1.0, 0.5), (0.5, 0.0)],
                'max_rate_of_change': 0.15,
                'min_duration': 0.2
            },
            'norepinephrine': {
                'normal_range': (0.0, 1.0),
                'valid_transitions': [(0.0, 0.3), (0.3, 0.7), (0.7, 1.0), (1.0, 0.7), (0.7, 0.3), (0.3, 0.0)],
                'max_rate_of_change': 0.25,
                'min_duration': 0.15
            },
            'serotonin': {
                'normal_range': (0.0, 1.0),
                'valid_transitions': [(0.0, 0.5), (0.5, 0.0)],
                'max_rate_of_change': 0.1,
                'min_duration': 0.3
            }
        }
        
    def validate_signal(self, neuromodulator_name: str, signal_value: float, 
                       timestamp: float) -> Dict[str, Any]:
        """Validate a neuromodulator signal."""
        if neuromodulator_name not in self.signal_history:
            self.signal_history[neuromodulator_name] = []
            
        history = self.signal_history[neuromodulator_name]
        
        # Check basic validity
        rules = self.validation_rules.get(neuromodulator_name, {})
        normal_range = rules.get('normal_range', (0.0, 1.0))
        
        is_valid = (
            normal_range[0] <= signal_value <= normal_range[1]
        )
        
        # Check transition validity
        transition_valid = True
        if len(history) > 0 and is_valid:
            last_value = history[-1]['value']
            valid_transitions = rules.get('valid_transitions', [])
            if (last_value, signal_value) not in valid_transitions:
                transition_valid = False
                
        # Check rate of change
        rate_valid = True
        if len(history) > 0 and is_valid and transition_valid:
            max_rate = rules.get('max_rate_of_change', 1.0)
            time_diff = timestamp - history[-1]['timestamp']
            if time_diff > 0:
                rate_of_change = abs(signal_value - history[-1]['value']) / time_diff
                rate_valid = rate_of_change <= max_rate
                
        # Check duration
        duration_valid = True
        if len(history) > 0 and is_valid:
            min_duration = rules.get('min_duration', 0.0)
            time_since_last = timestamp - history[-1]['timestamp']
            duration_valid = time_since_last >= min_duration
        
        # Overall validity
        valid = is_valid and transition_valid and rate_valid and duration_valid
        
        # Record signal
        signal_record = {
            'timestamp': timestamp,
            'value': signal_value,
            'valid': valid,
            'is_valid_component': {
                'basic': is_valid,
                'transition': transition_valid,
                'rate': rate_valid,
                'duration': duration_valid
            }
        }
        
        history.append(signal_record)
        
        # Return validation result
        result = {
            'valid': valid,
            'neuromodulator': neuromodulator_name,
            'signal_value': signal_value,
nvalidation_details': signal_record['is_valid_component'],
            'warnings': []
        }
        
        if not valid:
            if not is_valid:
                result['warnings'].append(f"Signal {signal_value} outside normal range {normal_range}")
            if not transition_valid:
                result['warnings'].append(f"Invalid transition from {history[-1]['value']} to {signal_value}")
            if not rate_valid:
                result['warnings'].append(f"Rate of change too high")
            if not duration_valid:
                result['warnings'].append(f"Signal duration too short")
                
        return result
        
    def analyze_signal_quality(self, neuromodulator_name: str) -> Dict[str, Any]:
        """Analyze signal quality for a neuromodulator."""
        if neuromodulator_name not in self.signal_history:
            return {'status': 'no_data', 'quality_score': 0}
            
        history = self.signal_history[neuromodulator_name]
        
        if not history:
            return {'status': 'no_data', 'quality_score': 0}
            
        # Calculate quality metrics
        valid_signals = [s for s in history if s['valid']]
        validity_ratio = len(valid_signals) / len(history)
        
        # Signal stability (coefficient of variation)
        values = [s['value'] for s in history]
        mean_value = sum(values) / len(values)
        if mean_value > 0:
            variance = sum((v - mean_value) ** 2 for v in values) / len(values)
            std_dev = variance ** 0.5
            coefficient_of_variation = std_dev / mean_value
        else:
            coefficient_of_variation = 0
        
        # Signal dynamics
        if len(history) >= 2:
            time_diffs = [h['timestamp'] - prev['timestamp'] for h, prev in zip(history[1:], history[:-1])]
            avg_time_diff = sum(time_diffs) / len(time_diff_diff)
        else:
            avg_time_diff = 0
        
        # Quality score (weighted combination)
        quality_score = (
            validity_ratio * 0.5 +  # Validity importance
            max(0, 1 - coefficient_of_variation) * 0.3 +  # Stability importance
            min(avg_time_diff, 1.0) * 0.2  # Temporal consistency
        )
        
        return {
            'status': 'analysis_complete',
            'total_samples': len(history),
            'valid_samples': len(valid_signals),
            'validity_ratio': validity_ratio,
            'mean_value': mean_value,
            'coefficient_of_variation': coefficient_of_variation,
            'average_sample_interval': avg_time_diff,
            'quality_score': quality_score,
            'quality_grade': self._get_quality_grade(quality_score)
        }
        
    def _get_quality_grade(self, score: float) -> str:
        """Convert quality score to letter grade."""
        if score >= 0.9:
            return "A+ (Excellent)"
        elif score >= 0.8:
            return "A (Very Good)"
        elif score >= 0.7:
            return "B (Good)"
        elif score >= 0.6:
            return "C (Fair)"
        elif score >= 0.5:
            return "D (Poor)"
        else:
            return "F (Failed)"
        
    def get_signal_anomalies(self, neuromodulator_name: str, threshold: float = 2.0) -> List[Dict[str, Any]]:
        """Get signal anomalies for a neuromodulator."""
        if neuromodulator_name not in self.signal_history:
            return []
            
        history = self.signal_history[neuromodulator_name]
        anomalies = []
        
        for i, signal in enumerate(history):
            if not signal['valid']:
                anomalies.append({
                    'index': i,
                    'timestamp': signal['timestamp'],
                    'value': signal['value'],
                    'issue_type': 'validation_failed',
                    'details': signal['is_valid_component']
                })
        
        # Statistical anomalies (outliers)
        values = [s['value'] for s in history]
        if len(values) >= 4:
            q1 = sorted(values)[len(values) // 4]
            q3 = sorted(values)[3 * len(values) // 4]
            iqr = q3 - q1
            lower_bound = q1 - threshold * iqr
            upper_bound = q3 + threshold * iqr
            
            for i, value in enumerate(values):
                if value < lower_bound or value > upper_bound:
                    anomalies.append({
                        'index': i,
                        'timestamp': history[i]['timestamp'],
                        'value': value,
                        'issue_type': 'statistical_outlier',
                        'bounds': (lower_bound, upper_bound)
                    })
        
        return anomalies
class NeuromodulationSignalMonitor:
    """Monitor and analyze neuromodulation signals in real-time."""
    
    def __init__(self, sampling_rate: float = 10.0):
        self.sampling_rate = sampling_rate
        self.monitoring_sessions = {}
        self.alert_thresholds = {
            'signal_variance': 0.5,
            'signal_rate_change': 0.3,
            'signal_duration': 0.2
        }
        
    def start_monitoring(self, session_name: str, neuromodulators: List[str]):
        """Start monitoring neuromodulator signals."""
        self.monitoring_sessions[session_name] = {
            'neuromodulators': neuromodulators,
            'signal_validators': {mod: SignalValidator() for mod in neuromodulators},
            'session_start': time.time(),
            'metrics': {
                'samples_collected': 0,
                'valid_signals': 0,
                'total_signals': 0,
                'alert_count': 0
            }
        }
        
    def record_signal(self, session_name: str, neuromodulator_name: str, 
                     signal_value: float, timestamp: float) -> Dict[str, Any]:
        """Record and validate a neuromodulator signal."""
        if session_name not in self.monitoring_sessions:
            raise ValueError(f"Unknown monitoring session: {session_name}")
            
        session = self.monitoring_sessions[session_name]
        
        if neuromodulator_name not in session['neuromodulators']:
            raise ValueError(f"Neuromodulator {neuromodulator_name} not in monitoring session")
            
        validator = session['signal_validators'][neuromodulator_name]
        
        # Validate signal
        validation_result = validator.validate_signal(neuromodulator_name, signal_value, timestamp)
        
        # Update metrics
        session['metrics']['total_signals'] += 1
        if validation_result['valid']:
            session['metrics']['valid_signals'] += 1
            
        # Check for alerts
        alerts = self._check_alerts(session, neuromodulator_name, signal_value, validation_result)
        if alerts:
            session['metrics']['alert_count'] += len(alerts)
            validation_result['alerts'] = alerts
        
        return validation_result
        
    def _check_alerts(self, session: Dict[str, Any], neuromodulator_name: str, 
                     signal_value: float, validation_result: Dict[str, Any]) -> List[str]:
        """Check for monitoring alerts."""
        alerts = []
        validator = session['signal_validators'][neuromodulator_name]
        
        # Check signal validity
        if not validation_result['valid']:
            alerts.append(f"Invalid signal: {signal_value}")
            
        # Check signal quality
        quality_analysis = validator.analyze_signal_quality(neuromodulator_name)
        if quality_analysis['quality_score'] < 0.7:
            alerts.append(f"Poor signal quality: {quality_analysis['quality_grade']}")
            
        # Check for anomalies
        anomalies = validator.get_signal_anomalies(neuromodulator_name)
        if anomalies:
            alerts.append(f"{len(anomalies)} signal anomalies detected")
            
        return alerts
        
    def get_session_summary(self, session_name: str) -> Dict[str, Any]:
        """Get monitoring session summary."""
        if session_name not in self.monitoring_sessions:
            raise ValueError(f"Unknown monitoring session: {session_name}")
            
        session = self.monitoring_sessions[session_name]
        
        # Calculate overall quality
        total_signals = session['metrics']['total_signals']
        valid_signals = session['metrics']['valid_signals']
        
        overall_quality = {
            'total_signals': total_signals,
            'valid_signals': valid_signals,
            'validity_ratio': valid_signals / total_signals if total_signals > 0 else 0,
            'alert_count': session['metrics']['alert_count'],
            'session_duration': time.time() - session['session_start'],
            'neuromodulators_monitored': session['neuromodulators']
        }
        
        # Analyze each neuromodulator
        neuromodulator_analyses = {}
        for mod_name, validator in session['signal_validators'].items():
            neuromodulator_analyses[mod_name] = validator.analyze_signal_quality(mod_name)
            
        return {
            'session_name': session_name,
            'overall_quality': overall_quality,
            'neuromodulator_analyses': neuromodulator_analyses,
            'session_start': session['session_start'],
            'last_update': time.time()
        }
        
    def stop_monitoring(self, session_name: str):
        """Stop monitoring session."""
        if session_name in self.monitoring_sessions:
            del self.monitoring_sessions[session_name]
class SignalVisualizer:
    """Visualize neuromodulation signals for debugging."""
    
    def __init__(self):
        self.plot_data = {}
        
    def add_signal_to_plot(self, session_name: str, neuromodulator_name: str,
                           signals: List[Tuple[float, float]]):
        """Add signals to visualization plot."""
        if session_name not in self.plot_data:
            self.plot_data[session_name] = {}
            
        self.plot_data[session_name][neuromodulator_name] = signals
        
    def generate_signal_plot_data(self, session_name: str) -> Dict[str, Any]:
        """Generate plot data for visualization."""
        if session_name not in self.plot_data:
            return {'status': 'no_data', 'plots': []}
            
        session_data = self.plot_data[session_name]
        plots = []
        
        for mod_name, signals in session_data.items():
            if len(signals) >= 2:
                timestamps = [s[0] for s in signals]
                values = [s[1] for s in signals]
                
                # Create time series data
                time_series = {
                    'name': f"{mod_name}_time_series",
                    'type': 'line',
                    'data': values,
                    'timestamps': timestamps,
                    'labels': {
                        'x': 'Time (s)',
                        'y': f'{mod_name} Signal Level',
                        'title': f'{mod_name} Signal Over Time'
                    }
                }
                
                # Create histogram
                if len(values) >= 10:
                    histogram = {
                        'name': f"{mod_name}_histogram",
                        'type': 'histogram',
                        'data': values,
                        'labels': {
                            'x': 'Signal Value',
                            'y': 'Frequency',
                            'title': f'{mod_name} Signal Distribution'
                        }
                    }
                    plots.extend([time_series, histogram])
                else:
                    plots.append(time_series)
        
        return {
            'status': 'data_generated',
            'session_name': session_name,
            'plots': plots,
            'plot_count': len(plots)
        }


def neuromodulation_signal_validation_example():
    """Demonstrate neuromodulation signal validation."""
    print_header("2.1 Neuromodulation Signal Validation")
    
    print("\nNeuromodulation signal validation and anomaly detection:")
    
    # Initialize signal validator
    validator = SignalValidator()
    
    # Test signal validation
    print("\n2.1.1 Signal validation tests:")
    
    # Valid signals
    valid_signals = [
        ("dopamine", 0.5, 1.0),
        ("acetylcholine", 0.3, 1.5),
        ("norepinephrine", 0.8, 2.0),
        ("serotonin", 0.2, 3.0)
    ]
    
    for mod_name, signal_value, timestamp in valid_signals:
        result = validator.validate_signal(mod_name, signal_value, timestamp)
        status = "✅ Valid" if result['valid'] else "❌ Invalid"
        print(f"  {mod_name} {signal_value} at {timestamp}: {status}")
        
    # Invalid signals
    invalid_signals = [
        ("dopamine", 1.5, 1.0),  # Out of range
        ("acetylcholine", 0.1, 2.0),  # Too fast change
        ("norepinephrine", 0.9, 2.5),  # Too short duration
        ("serotonin", -0.1, 3.0)  # Negative value
    ]
    
    for mod_name, signal_value, timestamp in invalid_signals:
        result = validator.validate_signal(mod_name, signal_value, timestamp)
        status = "✅ Valid" if result['valid'] else "❌ Invalid"
        print(f"  {mod_name} {signal_value} at {timestamp}: {status}")
        if not result['valid']:
            print(f"    Issues: {', '.join(result['warnings'])}")
    
    # Signal quality analysis
    print("\n2.1.2 Signal quality analysis:")
    
    for mod_name in ["dopamine", "acetylcholine", "norepinephrine", "serotonin"]:
        analysis = validator.analyze_signal_quality(mod_name)
        if analysis['status'] == 'analysis_complete':
            print(f"  {mod_name}:")
            print(f"    Quality score: {analysis['quality_score']:.3f} ({analysis['quality_grade']})")
            print(f"    Validity ratio: {analysis['validity_ratio']:.3f}")
            print(f"    Mean value: {analysis['mean_value']:.3f}")
            print(f"    Coefficient of variation: {analysis['coefficient_of_variation']:.3f}")
    
    # Anomaly detection
    print("\n2.1.3 Anomaly detection:")
    
    for mod_name in ["dopamine", "acetylcholine"]:
        anomalies = validator.get_signal_anomalies(mod_name)
        if anomalies:
            print(f"  {mod_name}: {len(anomalies)} anomalies detected")
            for anomaly in anomalies[:3]:  # Show first 3
                print(f"    - {anomaly['issue_type']} at {anomaly['timestamp']}: {anomaly['value']}")
        else:
            print(f"  {mod_name}: No anomalies detected")
    
    return validator


def neuromodulation_monitoring_example():
    """Demonstrate neuromodulation monitoring and alerting."""
    print_header("2.2 Neuromodulation Monitoring")
    
    print("\nNeuromodulation monitoring and real-time alerting:")
    
    # Initialize monitor
    monitor = NeuromodulationSignalMonitor(sampling_rate=5.0)
    
    # Start monitoring session
    monitor.start_monitoring(
        "learning_session",
        ["dopamine", "acetylcholine", "norepinephrine", "serotonin"]
    )
    
    # Simulate signal monitoring
    print("\n2.2.1 Simulating signal monitoring:")
    
    # Simulate normal operation
    print("  Normal operation simulation:")
    for i in range(10):
        timestamp = i * 0.2
        
        # Record signals
        signals = {
            'dopamine': 0.5 + (i % 3 - 1) * 0.2,
            'acetylcholine': 0.3 + (i % 4 - 2) * 0.15,
            'norepinephrine': 0.7 + (i % 5 - 2) * 0.1,
            'serotonin': 0.4 + (i % 3 - 1) * 0.12
        }
        
        for mod_name, signal_value in signals.items():
            result = monitor.record_signal("learning_session", mod_name, signal_value, timestamp)
            if result.get('alerts'):
                print(f"    Alert at {timestamp}: {', '.join(result['alerts'])}")
        
        if i % 5 == 0:
            print(f"    Step {i}: Valid signals so far: {monitor.monitoring_sessions['learning_session']['metrics']['valid_signals']}")
    
    # Simulate abnormal operation
    print("\n  Abnormal operation simulation (injecting anomalies):")
    
    abnormal_timestamps = [15.0, 25.0, 35.0]
    for timestamp in abnormal_timestamps:
        # Inject abnormal signals
        monitor.record_signal("learning_session", "dopamine", 1.5, timestamp)  # Too high
        monitor.record_signal("learning_session", "acetylcholine", -0.1, timestamp)  # Negative
        monitor.record_signal("learning_session", "norepinephrine", 1.2, timestamp)  # Too high
        
        print(f"    Anomalies injected at {timestamp}")
    
    # Get session summary
    print("\n2.2.2 Session summary:")
    
    summary = monitor.get_session_summary("learning_session")
    print(f"  Session duration: {summary['overall_quality']['session_duration']:.1f}s")
    print(f"  Total signals: {summary['overall_quality']['total_signals']}")
    print(f"  Valid signals: {summary['overall_quality']['valid_signals']}")
    print(f"  Validity ratio: {summary['overall_quality']['validity_ratio']:.3f}")
    print(f"  Alert count: {summary['overall_quality']['alert_count']}")
    
    print(f"\nNeuromodulator quality analysis:")
    for mod_name, analysis in summary['neuromodulator_analyses'].items():
        if analysis['status'] == 'analysis_complete':
            status = "✅ Good" if analysis['quality_score'] >= 0.8 else "⚠️ Fair" if analysis['quality_score'] >= 0.6 else "❌ Poor"
            print(f"  {mod_name}: {analysis['quality_score']:.3f} ({analysis['quality_grade']}) - {status}")
    
    return monitor


def signal_visualization_example():
    """Demonstrate signal visualization for debugging."""
    print_header("2.3 Signal Visualization")
    
    print("\nSignal visualization and plotting for debugging:")
    
    # Initialize visualizer
    visualizer = SignalVisualizer()
    
    # Create sample signal data
    print("\n2.3.1 Creating sample signal data:")
    
    # Time series data
    base_time = 0.0
    time_step = 0.5
    
    dopamine_signals = []
    acetylcholine_signals = []
    norepinephrine_signals = []
    
    for i in range(20):
        t = base_time + i * time_step
        dopamine_signals.append((t, 0.5 + 0.3 * (1 if i % 3 == 0 else -1)))
        acetylcholine_signals.append((t, 0.4 + 0.2 * (1 if i % 4 == 0 else -1)))
        norepinephrine_signals.append((t, 0.7 + 0.1 * (1 if i % 5 == 0 else -1)))
    
    # Add some outliers
    outlier_times = [3.0, 7.0, 12.0, 15.0]
    for t in outlier_times:
        dopamine_signals.append((t, 1.5))  # Spike
        acetylcholine_signals.append((t, -0.1))  # Negative
        norepinephrine_signals.append((t, 1.8))  # Spike
    
    # Add to visualizer
    visualizer.add_signal_to_plot("debug_session", "dopamine", dopamine_signals)
    visualizer.add_signal_to_plot("debug_session", "acetylcholine", acetylcholine_signals)
    visualizer.add_signal_to_plot("debug_session", "norepinephrine", norepinephrine_signals)
    
    # Generate plot data
    print("\n2.3.2 Generating plot data:")
    
    plot_data = visualizer.generate_signal_plot_data("debug_session")
    
    if plot_data['status'] == 'data_generated':
        print(f"  Generated {plot_data['plot_count']} plots:")
        
        for plot in plot_data['plots']:
            plot_type = plot['type']
            plot_name = plot['name']
            data_points = len(plot['data']) if 'data' in plot else len(plot['timestamps']) if 'timestamps' in plot else 0
            
            print(f"    {plot_name}:")
            print(f"      Type: {plot_type}")
            print(f"      Data points: {data_points}")
            print(f"      X-axis: {plot['labels']['x']}")
            print(f"      Y-axis: {plot['labels']['y']}")
            print(f"      Title: {plot['labels']['title']}")
    
    print("\n2.3.3 Signal analysis for visualization:")
    
    # Analyze signals for visualization insights
    all_signals = {
        'dopamine': [s[1] for s in dopamine_signals],
        'acetylcholine': [s[1] for s in acetylcholine_signals],
        'norepinephrine': [s[1] for s in norepinephrine_signals]
    }
    
    for mod_name, signals in all_signals.items():
        mean_val = sum(signals) / len(signals)
        min_val = min(signals)
        max_val = max(signals)
        
        # Detect outliers visually
        outliers = [s for s in signals if s < 0 or s > 1.0]
        
        print(f"  {mod_name}:")
        print(f"    Range: [{min_val:.2f}, {max_val:.2f}]")
        print(f"    Mean: {mean_val:.2f}")
        print(f"    Outliers (>1.0 or <0): {len(outliers)}")
        
        if outliers:
            print(f"    Outlier values: {[f'{v:.2f}' for v in outliers]}")
    
    return visualizer


def neuromodulation_debugging_examples_summary():
    """Print summary of neuromodulation debugging examples."""
    print_header("Neuromodulation Debugging Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Signal validation and anomaly detection")
    print("✓ Neuromodulator signal quality analysis")
    print("✓ Real-time monitoring and alerting")
    print("✓ Signal visualization for debugging")
    print("✓ Validation rules and transition checking")
    print("✓ Performance metrics and alerting thresholds")
    print("✓ Comprehensive signal analysis and reporting")

# ============================================================================
# 3. Simulation Debugging Examples
# ============================================================================

class SimulationStateInspector:
    """Inspect and analyze simulation state step-by-step."""
    
    def __init__(self):
        self.state_history = []
        self.checkpoints = {}
        self.snapshot_count = 0
        
    def capture_state(self, step: int, brain: Any, world: Any, agent: Any,
                     description: str = "") -> Dict[str, Any]:
        """Capture current simulation state."""
        state = {
            'step': step,
            'timestamp': time.time(),
            'description': description,
            'brain_state': {
                'total_neurons': brain.getTotalNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'prediction_error': brain.getPredictionError(),
                'development_stage': brain.getDevelopmentalStage()
            },
            'world_state': {
                'time': world.getSimulationTime(),
                'width': world.getWidth(),
                'height': world.getHeight(),
                'percept': world.getSensoryPercept()
            },
            'agent_state': {
                'neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.5,
                'curiosity_level': agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.5,
                'novelty_level': agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.5,
                'action_history': []  # Would track actions in real implementation
            }
        }
        
        self.state_history.append(state)
        self.snapshot_count += 1
        
        return state
        
    def create_checkpoint(self, checkpoint_name: str, step: int):
        """Create a named checkpoint at current step."""
        if step < len(self.state_history):
            self.checkpoints[checkpoint_name] = self.state_history[step].copy()
            print(f"Created checkpoint '{checkpoint_name}' at step {step}")
            
    def compare_states(self, state1: Dict[str, Any], state2: Dict[str, Any], 
                      comparison_fields: List[str] = None) -> Dict[str, Any]:
        """Compare two simulation states."""
        if comparison_fields is None:
            comparison_fields = ['step', 'brain_state', 'world_state', 'agent_state']
            
        comparison = {
            'step1': state1.get('step', -1),
            'step2': state2.get('step', -1),
            'differences': {},
            'similarities': {},
            'overall_similarity': 0.0
        }
        
        for field in comparison_fields:
            if field in state1 and field in state2:
                val1 = state1[field]
                val2 = state2[field]
                
                if isinstance(val1, dict) and isinstance(val2, dict):
                    # Compare dictionaries
                    differences = {}
                    similarities = {}
                    
                    all_keys = set(val1.keys()) | set(val2.keys())
                    for key in all_keys:
                        if key in val1 and key in val2:
                            if val1[key] == val2[key]:
                                similarities[key] = val1[key]
                            else:
                                differences[key] = (val1[key], val2[key])
                        elif key in val1:
                            differences[key] = (val1[key], 'missing in state2')
                        else:
                            differences[key] = ('missing in state1', val2[key])
                    
                    comparison['differences'][field] = differences
                    comparison['similarities'][field] = similarities
                else:
                    # Compare simple values
                    if val1 == val2:
                        comparison['similarities'][field] = val1
                    else:
                        comparison['differences'][field] = (val1, val2)
        
        # Calculate overall similarity
        total_fields = len(comparison_fields)
        matched_fields = len(comparison['similarities'])
        comparison['overall_similarity'] = matched_fields / total_fields if total_fields > 0 else 0.0
        
        return comparison
        
    def detect_anomalies(self, sensitivity: float = 0.1) -> List[Dict[str, Any]]:
        """Detect anomalies in state history."""
        if len(self.state_history) < 5:
            return []
            
        anomalies = []
        
        # Analyze each state change
        for i in range(1, len(self.state_history)):
            prev_state = self.state_history[i-1]
            curr_state = self.state_history[i]
            
            # Check for sudden changes
            state_change = self.compare_states(prev_state, curr_state)
            
            # Check brain state anomalies
            brain_change = state_change['differences'].get('brain_state', {})
            if brain_change:
                for key, (prev_val, curr_val) in brain_change.items():
                    if isinstance(prev_val, (int, float)) and isinstance(curr_val, (int, float)):
                        abs_change = abs(curr_val - prev_val)
                        mean_brain_val = abs(prev_val) + abs(curr_val)
                        
                        if mean_brain_val > 0:
                            relative_change = abs_change / mean_brain_val
                            if relative_change > sensitivity:
                                anomalies.append({
                                    'step': curr_state['step'],
                                    'type': 'brain_state_anomaly',
                                    'field': f'brain_state.{key}',
                                    'previous_value': prev_val,
                                    'current_value': curr_val,
                                    'relative_change': relative_change,
                                    'severity': 'high' if relative_change > 0.5 else 'medium' if relative_change > 0.2 else 'low'
                                })
            
            # Check for state inconsistencies
            if curr_state['step'] != i:
                anomalies.append({
                    'step': curr_state['step'],
                    'type': 'step_mismatch',
                    'expected_step': i,
                    'actual_step': curr_state['step'],
                    'severity': 'high'
                })
        
        return anomalies
        
    def generate_state_analysis_report(self) -> Dict[str, Any]:
        """Generate comprehensive state analysis report."""
        if not self.state_history:
            return {'status': 'no_data', 'analysis_complete': False}
            
        analysis = {
            'total_snapshots': len(self.state_history),
            'step_range': (self.state_history[0]['step'], self.state_history[-1]['step']),
            'time_range': (self.state_history[0]['timestamp'], self.state_history[-1]['timestamp']),
            'brain_state_analysis': {},
            'world_state_analysis': {},
            'agent_state_analysis': {},
            'anomalies': self.detect_anomalies(),
            'checkpoints': list(self.checkpoints.keys()),
            'analysis_complete': True
        }
        
        # Analyze brain state
        brain_states = [s['brain_state'] for s in self.state_history]
        analysis['brain_state_analysis'] = {
            'neuron_count_range': (
                min(s['total_neurons'] for s in brain_states),
                max(s['total_neurons'] for s in brain_states)
            ),
            'firing_rate_range': (
                min(s['average_firing_rate'] for s in brain_states),
                max(s['average_firing_rate'] for s in brain_states)
            ),
            'prediction_error_range': (
                min(s['prediction_error'] for s in brain_states),
                max(s['prediction_error'] for s in brain_states)
            ),
            'average_total_neurons': sum(s['total_neurons'] for s in brain_states) / len(brain_states),
            'average_firing_rate': sum(s['average_firing_rate'] for s in brain_states) / len(brain_states),
            'average_prediction_error': sum(s['prediction_error'] for s in brain_states) / len(brain_states)
        }
        
        # Analyze world state
        world_states = [s['world_state'] for s in self.state_history]
        analysis['world_state_analysis'] = {
            'time_range': (
                min(s['time'] for s in world_states),
                max(s['time'] for s in world_states)
            ),
            'world_dimensions': {
                'width': world_states[0]['width'],
                'height': world_states[0]['height']
            }
        }
        
        # Analyze agent state
        agent_states = [s['agent_state'] for s in self.state_history]
        analysis['agent_state_analysis'] = {
            'neuromodulation_range': (
                min(s['neuromodulation_level'] for s in agent_states),
                max(s['neuromodulation_level'] for s in agent_states)
            ),
            'curiosity_range': (
                min(s['curiosity_level'] for s in agent_states),
                max(s['curiosity_level'] for s in agent_states)
            ),
            'average_neuromodulation': sum(s['neuromodulation_level'] for s in agent_states) / len(agent_states),
            'average_curiosity': sum(s['curiosity_level'] for s in agent_states) / len(agent_states)
        }
        
        return analysis
        
    def generate_state_report(self) -> str:
        """Generate formatted state analysis report."""
        analysis = self.generate_state_analysis_report()
        
        if not analysis['analysis_complete']:
            return "No state data available for analysis"
            
        report = f"Simulation State Analysis Report\n"
        report += f"=" * 50 + "\n"
        report += f"Total snapshots captured: {analysis['total_snapshots']}\n"
        report += f"Step range: {analysis['step_range'][0]} to {analysis['step_range'][1]}\n"
        report += f"Time range: {analysis['time_range'][0]:.1f}s to {analysis['time_range'][1]:.1f}s\n"
        
        report += f"\nBrain State Analysis:\n"
        brain = analysis['brain_state_analysis']
        report += f"  Total neurons: {brain['average_total_neurons']:.0f} (range: {brain['neuron_count_range'][0]}-{brain['neuron_count_range'][1]})\n"
        report += f"  Average firing rate: {brain['average_firing_rate']:.3f} (range: {brain['firing_rate_range'][0]:.3f}-{brain['firing_rate_range'][1]:.3f})\n"
        report += f"  Average prediction error: {brain['average_prediction_error']:.3f} (range: {brain['prediction_error_range'][0]:.3f}-{brain['prediction_error_range'][1]:.3f})\n"
        
        report += f"\nWorld State Analysis:\n"
        world = analysis['world_state_analysis']
        report += f"  Time: {world['time_range'][0]:.1f}s to {world['time_range'][1]:.1f}s\n"
        report += f"  Dimensions: {world['world_dimensions']['width']}x{world['world_dimensions']['height']}\n"
        
        report += f"\nAgent State Analysis:\n"
        agent = analysis['agent_state_analysis']
        report += f"  Neuromodulation: {agent['average_neuromodulation']:.3f} (range: {agent['neuromodulation_range'][0]:.3f}-{agent['neuromodulation_range'][1]:.3f})\n"
        report += f"  Curiosity: {agent['average_curiosity']:.3f} (range: {agent['curiosity_range'][0]:.3f}-{agent['curiosity_range'][1]:.3f})\n"
        
        report += f"\nCheckpoints created: {len(analysis['checkpoints'])}\n"
        if analysis['checkpoints']:
            report += f"  {', '.join(analysis['checkpoints'])}\n"
        
        anomalies = analysis['anomalies']
        report += f"\nAnomaly Detection:\n"
        report += f"  Total anomalies detected: {len(anomalies)}\n"
        if anomalies:
            high_severity = sum(1 for a in anomalies if a['severity'] == 'high')
            medium_severity = sum(1 for a in anomalies if a['severity'] == 'medium')
            low_severity = sum(1 for a in anomalies if a['severity'] == 'low')
            
            report += f"  High severity: {high_severity}\n"
            report += f"  Medium severity: {medium_severity}\n"
            report += f"  Low severity: {low_severity}\n"
            
            report += f"\nAnomaly details:\n"
            for anomaly in anomalies[:5]:  # Show first 5
                report += f"    Step {anomaly['step']}: {anomaly['type']} "
                report += f"({anomaly['severity']} severity)\n"
                
                if 'field' in anomaly:
                    report += f"      Field: {anomaly['field']}\n"
                    report += f"      Change: {anomaly['previous_value']:.3f} -> {anomaly['current_value']:.3f}\n"
                
                if 'expected_step' in anomaly:
                    report += f"      Expected: {anomaly['expected_step']}, Actual: {anomaly['actual_step']}\n"
        
        return report
class StateTransitionAnalyzer:
    """Analyze state transitions and detect patterns."""
    
    def __init__(self):
        self.transition_patterns = {}
        self.phase_transitions = []
        self.cycle_patterns = []
        
    def analyze_transitions(self, state_history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze state transitions for patterns."""
        if len(state_history) < 3:
            return {'status': 'insufficient_data', 'patterns': {}}
            
        transitions = []
        
        for i in range(1, len(state_history)):
            prev_state = state_history[i-1]
            curr_state = state_history[i]
            
            transition = {
                'from_step': prev_state['step'],
                'to_step': curr_state['step'],
                'step_change': curr_state['step'] - prev_state['step'],
                'brain_change': self._calculate_brain_change(prev_state['brain_state'], curr_state['brain_state']),
                'world_change': self._calculate_world_change(prev_state['world_state'], curr_state['world_state']),
                'agent_change': self._calculate_agent_change(prev_state['agent_state'], curr_state['agent_state']),
                'transition_time': curr_state['timestamp'] - prev_state['timestamp']
            }
            
            transitions.append(transition)
            
        # Detect patterns
        patterns = {
            'step_transitions': self._detect_step_patterns(transitions),
            'brain_dynamics': self._detect_brain_dynamics(transitions),
            'world_dynamics': self._detect_world_dynamics(transitions),
            'agent_dynamics': self._detect_agent_dynamics(transitions)
        }
        
        return {
            'status': 'analysis_complete',
            'total_transitions': len(transitions),
            'patterns': patterns,
            'average_transition_time': sum(t['transition_time'] for t in transitions) / len(transitions),
            'step_consistency': self._check_step_consistency(state_history)
        }
        
    def _calculate_brain_change(self, prev_brain: Dict[str, Any], curr_brain: Dict[str, Any]) -> Dict[str, float]:
        """Calculate brain state change."""
        change = {}
        
        for key in prev_brain:
            if key in curr_brain:
                prev_val = prev_brain[key]
                curr_val = curr_brain[key]
                
                if isinstance(prev_val, (int, float)) and isinstance(curr_val, (int, float)):
                    abs_change = abs(curr_val - prev_val)
                    rel_change = abs_change / (abs(prev_val) + abs(curr_val)) if (abs(prev_val) + abs(curr_val)) > 0 else 0
                    
                    change[key] = {
                        'absolute_change': abs_change,
                        'relative_change': rel_change,
                        'direction': 'increase' if curr_val > prev_val else 'decrease'
                    }
        
        return change
        
    def _calculate_world_change(self, prev_world: Dict[str, Any], curr_world: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate world state change."""
        return {
            'time_change': curr_world['time'] - prev_world['time'],
            'percept_change': self._calculate_percept_change(prev_world['percept'], curr_world['percept'])
        }
        
    def _calculate_percept_change(self, prev_percept: List[float], curr_percept: List[float]) -> float:
        """Calculate percept change."""
        if len(prev_percept) != len(curr_percept):
            return float('inf')
            
        squared_diff = sum((p1 - p2) ** 2 for p1, p2 in zip(prev_percept, curr_percept))
        return squared_diff ** 0.5
        
    def _calculate_agent_change(self, prev_agent: Dict[str, Any], curr_agent: Dict[str, Any]) -> Dict[str, float]:
        """Calculate agent state change."""
        change = {}
        
        for key in prev_agent:
            if key in curr_agent:
                prev_val = prev_agent[key]
                curr_val = curr_agent[key]
                
                if isinstance(prev_val, (int, float)) and isinstance(curr_val, (int, float)):
                    abs_change = abs(curr_val - prev_val)
                    rel_change = abs_change / (abs(prev_val) + abs(curr_val)) if (abs(prev_val) + abs(curr_val)) > 0 else 0
                    
                    change[key] = {
                        'absolute_change': abs_change,
                        'relative_change': rel_change,
                        'direction': 'increase' if curr_val > prev_val else 'decrease'
                    }
        
        return change
        
    def _detect_step_patterns(self, transitions: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect patterns in step transitions."""
        step_changes = [t['step_change'] for t in transitions]
        
        patterns = {
            'steady_state': sum(1 for change in step_changes if change == 1) / len(step_changes),
            'back_steps': sum(1 for change in step_changes if change < 0) / len(step_changes),
            'jumps': sum(1 for change in step_changes if change > 2) / len(step_changes)
        }
        
        return patterns
        
    def _detect_brain_dynamics(self, transitions: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect brain dynamics patterns."""
        brain_changes = [t['brain_change'] for t in transitions]
        
        # Analyze each brain state change
        dynamics = {
            'most_changed_neuron': {},
            'stability_index': 0,
            'volatility': 0
        }
        
        # Find most frequently changed neurons
        neuron_changes = {}
        for change in brain_changes:
            for key, change_info in change.items():
                if key not in neuron_changes:
                    neuron_changes[key] = {'total_change': 0, 'change_count': 0}
                neuron_changes[key]['total_change'] += change_info['absolute_change']
                neuron_changes[key]['change_count'] += 1
        
        if neuron_changes:
            # Sort by average change
            sorted_neurons = sorted(neuron_changes.items(),
                                   key=lambda x: x[1]['total_change'] / x[1]['change_count'],
                                   reverse=True)
            
            for i, (neuron, stats) in enumerate(sorted_neurons[:3]):
                dynamics['most_changed_neuron'][f'neuron_{i}'] = {
                    'average_change': stats['total_change'] / stats['change_count'],
                    'change_frequency': stats['change_count'] / len(transitions)
                }
        
        # Calculate stability and volatility
        all_changes = []
        for change in brain_changes:
            all_changes.extend(change.values())
            
        if all_changes:
            avg_change = sum(c['absolute_change'] for c in all_changes) / len(all_changes)
            dynamics['stability_index'] = 1 / (1 + avg_change)
            dynamics['volatility'] = sum(c['relative_change'] for c in all_changes) / len(all_changes)
        
        return dynamics
        
    def _detect_world_dynamics(self, transitions: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect world dynamics patterns."""
        time_changes = [t['world_change']['time_change'] for t in transitions]
        percept_changes = [t['world_change']['percept_change'] for t in transitions]
        
        return {
            'world_time_steadiness': sum(1 for change in time_changes if change <= 0.1) / len(time_changes),
            'percept_stability': 1 / (1 + sum(percept_changes) / len(percept_changes) if percept_changes else 1),
            'average_world_time_step': sum(time_changes) / len(time_changes)
        }
        
    def _detect_agent_dynamics(self, transitions: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect agent dynamics patterns."""
        agent_changes = [t['agent_change'] for t in transitions]
        
        dynamics = {
            'neuromodulation_stability': 0,
            'curiosity_volatility': 0
        }
        
        if agent_changes:
            neuromod_changes = [c['neuromodulation_level'] for c in agent_changes if 'neuromodulation_level' in c]
            curiosity_changes = [c['curiosity_level'] for c in agent_changes if 'curiosity_level' in c]
            
            if neuromod_changes:
                neuromod_avg_change = sum(c['absolute_change'] for c in neuromod_changes) / len(neuromod_changes)
                dynamics['neuromodulation_stability'] = 1 / (1 + neuromod_avg_change)
                
            if curiosity_changes:
                curiosity_avg_change = sum(c['absolute_change'] for c in curiosity_changes) / len(curiosity_changes)
                dynamics['curiosity_volatility'] = curiosity_avg_change
        
        return dynamics
        
    def _check_step_consistency(self, state_history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Check for step consistency issues."""
        expected_steps = list(range(state_history[0]['step'], state_history[-1]['step'] + 1))
        actual_steps = [s['step'] for s in state_history]
        
        missing_steps = set(expected_steps) - set(actual_steps)
        duplicate_steps = len(actual_steps) - len(set(actual_steps))
        
        return {
            'expected_steps': len(expected_steps),
            'actual_steps': len(actual_steps),
            'missing_steps': len(missing_steps),
            'duplicate_steps': duplicate_steps,
            'step_consistency': len(set(actual_steps)) / len(expected_steps) if expected_steps else 0
        }


def simulation_state_inspection_example():
    """Demonstrate simulation state inspection and analysis."""
    print_header("3.1 Simulation State Inspection")
    
    print("\nSimulation state inspection and step-by-step analysis:")
    
    # Create state inspector
    inspector = SimulationStateInspector()
    
    # Create simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Capture simulation states
    print("\n3.1.1 Capturing simulation states:")
    
    for step in range(10):
        # Capture state before step
        state_before = inspector.capture_state(
            step, brain, world, agent,
            description=f"Before step {step}"
        )
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Capture state after step
        state_after = inspector.capture_state(
            step, brain, world, agent,
            description=f"After step {step}"
        )
        
        # Show state differences
        comparison = inspector.compare_states(state_before, state_after)
        
        print(f"\n  Step {step}:")
        print(f"    State similarity: {comparison['overall_similarity']:.1%}")
        print(f"    Brain differences: {len(comparison['differences'].get('brain_state', {}))}")
        print(f"    World differences: {len(comparison['differences'].get('world_state', {}))}")
        print(f"    Agent differences: {len(comparison['differences'].get('agent_state', {}))}")
    
    # Create checkpoints
    print("\n3.1.2 Creating checkpoints:")
    
    inspector.create_checkpoint("initialization", 0)
    inspector.create_checkpoint("mid_simulation", 5)
    inspector.create_checkpoint("simulation_end", 9)
    
    # Detect anomalies
    print("\n3.1.3 Anomaly detection:")
    
    anomalies = inspector.detect_anomalies(sensitivity=0.1)
    if anomalies:
        print(f"  Detected {len(anomalies)} anomalies:")
        for anomaly in anomalies:
            print(f"    Step {anomaly['step']}: {anomaly['type']} "
                  f"({anomaly['severity']} severity)")
    else:
        print("  No anomalies detected in simulation")
    
    # Generate state analysis report
    print("\n3.1.4 State analysis report:")
    
    report = inspector.generate_state_report()
    print(report)
    
    return inspector


def state_transition_analysis_example():
    """Demonstrate state transition analysis."""
    print_header("3.2 State Transition Analysis")
    
    print("\nState transition analysis and pattern detection:")
    
    # Create transition analyzer
    analyzer = StateTransitionAnalyzer()
    
    # Create sample state history
    print("\n3.2.1 Creating sample state history:")
    
    state_history = []
    
    for step in range(10):
        state = {
            'step': step,
            'timestamp': step * 0.1,
            'brain_state': {
                'total_neurons': 1000 + step * 10,
                'firing_neurons': 100 + step * 5,
                'average_firing_rate': 0.5 + step * 0.01,
                'total_spikes': step * 50,
                'prediction_error': 0.1 - step * 0.005,
                'development_stage': 1.0 if step < 5 else 0.8
            },
            'world_state': {
                'time': step * 0.1,
                'width': 10,
                'height': 10,
                'percept': [0.1 * i for i in range(8)]
            },
            'agent_state': {
                'neuromodulation_level': 0.7 - step * 0.05,
                'curiosity_level': 0.5 + step * 0.03,
                'novelty_level': 0.4 + step * 0.02
            }
        }
        
        state_history.append(state)
        
        if step % 3 == 0:
            print(f"    Step {step}: neurons={state['brain_state']['total_neurons']}, "
                  f"firing_rate={state['brain_state']['average_firing_rate']:.3f}, "
                  f"neuromod={state['agent_state']['neuromodulation_level']:.3f}")
    
    # Analyze transitions
    print("\n3.2.2 Transition analysis:")
    
    analysis = analyzer.analyze_transitions(state_history)
    
    print(f"  Total transitions analyzed: {analysis['total_transitions']}")
    print(f"  Average transition time: {analysis['average_transition_time']:.3f}s")
    print(f"  Step consistency: {analysis['step_consistency']['step_consistency']:.1%}")
    
    print(f"\nTransition patterns:")
    
    step_patterns = analysis['patterns']['step_transitions']
    print(f"  Steady state: {step_patterns['steady_state']:.1%}")
    print(f"  Back steps: {step_patterns['back_steps']:.1%}")
    print(f"  Jumps: {step_patterns['jumps']:.1%}")
    
    brain_dynamics = analysis['patterns']['brain_dynamics']
    print(f"\nBrain dynamics:")
    print(f"  Stability index: {brain_dynamics['stability_index']:.3f}")
    print(f"  Volatility: {brain_dynamics['volatility']:.3f}")
    
    if brain_dynamics['most_changed_neuron']:
        print(f"  Most changed neurons:")
        for neuron_id, stats in brain_dynamics['most_changed_neuron'].items():
            print(f"    {neuron_id}: avg_change={stats['average_change']:.3f}, "
                  f"frequency={stats['change_frequency']:.1%}")
    
    world_dynamics = analysis['patterns']['world_dynamics']
    print(f"\nWorld dynamics:")
    print(f"  World time steadiness: {world_dynamics['world_time_steadiness']:.1%}")
    print(f"  Percept stability: {world_dynamics['percept_stability']:.3f}")
    print(f"  Average world time step: {world_dynamics['average_world_time_step']:.3f}s")
    
    agent_dynamics = analysis['patterns']['agent_dynamics']
    print(f"\nAgent dynamics:")
    print(f"  Neuromodulation stability: {agent_dynamics['neuromodulation_stability']:.3f}")
    print(f"  Curiosity volatility: {agent_dynamics['curiosity_volatility']:.3f}")
    
    return analyzer


def debugging_examples_summary():
    """Print summary of debugging and troubleshooting examples."""
    print_header("Debugging and Troubleshooting Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Memory usage tracking and leak detection")
    print("✓ Garbage collection debugging and object lifecycle analysis")
    print("✓ Advanced statistical memory leak detection")
    print("✓ Neuromodulation signal validation and anomaly detection")
    print("✓ Real-time monitoring and alerting")
    print("✓ Signal visualization for debugging")
    print("✓ Simulation state inspection and step-by-step analysis")
    print("✓ State transition analysis and pattern detection")
    print("✓ Anomaly detection and severity classification")
    print("✓ Comprehensive reporting and analysis tools")

# ============================================================================
# 4. Performance Profiling Examples
# ============================================================================

class PerformanceProfiler:
    """Profile simulation performance and identify bottlenecks."""
    
    def __init__(self):
        self.profile_data = {
            'function_calls': {},
            'memory_usage': [],
            'cpu_usage': [],
            'time_measurements': [],
            'bottlenecks': []
        }
        self.start_time = time.time()
        
    def start_profile(self, function_name: str):
        """Start profiling a function."""
        if function_name not in self.profile_data['function_calls']:
            self.profile_data['function_calls'][function_name] = {
                'call_count': 0,
                'total_time': 0,
                'min_time': float('inf'),
                'max_time': 0,
                'call_times': []
            }
            
    def end_profile(self, function_name: str):
        """End profiling a function."""
        if function_name in self.profile_data['function_calls']:
            call_data = self.profile_data['function_calls'][function_name]
            call_data['call_count'] += 1
            
    def record_memory_usage(self, operation: str = None):
        """Record current memory usage."""
        process = psutil.Process(os.getpid())
        memory_mb = process.memory_info().rss / (1024 * 1024)
        
        self.profile_data['memory_usage'].append({
            'timestamp': time.time(),
            'operation': operation,
            'memory_mb': memory_mb,
            'delta_from_baseline': memory_mb - self.profile_data.get('baseline_memory', memory_mb)
        })
        
    def record_cpu_usage(self, operation: str = None):
        """Record current CPU usage."""
        process = psutil.Process(os.getpid())
        cpu_percent = process.cpu_percent() / 100.0
        
        self.profile_data['cpu_usage'].append({
            'timestamp': time.time(),
            'operation': operation,
            'cpu_percent': cpu_percent
        })
        
    def record_time_measurement(self, operation: str, duration: float):
        """Record time measurement for an operation."""
        self.profile_data['time_measurements'].append({
            'timestamp': time.time(),noperation': operation,
            'duration': duration,
            'cumulative_time': sum(m['duration'] for m in self.profile_data['time_measurements'] if m['operation'] == operation)
        })
        
    def identify_bottlenecks(self) -> Dict[str, Any]:
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        # Analyze function calls
        for func_name, call_data in self.profile_data['function_calls'].items():
            if call_data['call_count'] > 0:
                avg_time = call_data['total_time'] / call_data['call_count']
                if avg_time > 0.1:  # Function takes more than 100ms on average
                    bottlenecks.append({
                        'type': 'function_slowdown',
                        'name': func_name,
                        'average_time': avg_time,
                        'call_count': call_data['call_count'],
                        'severity': 'high' if avg_time > 0.5 else 'medium' if avg_time > 0.1 else 'low'
                    })
        
        # Analyze memory usage
        if self.profile_data['memory_usage']:
            final_memory = self.profile_data['memory_usage'][-1]['memory_mb']
            baseline_memory = self.profile_data.get('baseline_memory', final_memory)
            memory_growth = final_memory - baseline_memory
            
            if memory_growth > 100:  # More than 100MB growth
                bottlenecks.append({
                    'type': 'memory_growth',
                    'name': 'overall_memory_usage',
                    'memory_growth_mb': memory_growth,
                    'severity': 'high' if memory_growth > 500 else 'medium' if memory_growth > 100 else 'low'
                })
        
        # Analyze time measurements
        if self.profile_data['time_measurements']:
            recent_measurements = self.profile_data['time_measurements'][-10:]
            avg_duration = sum(m['duration'] for m in recent_measurements) / len(recent_measurements)
            
            if avg_duration > 0.5:  # Average operation takes more than 500ms
                bottlenecks.append({
                    'type': 'overall_performance',
                    'name': 'simulation_step',
                    'average_duration': avg_duration,
                    'severity': 'high' if avg_duration > 1.0 else 'medium' if avg_duration > 0.5 else 'low'
                })
        
        self.profile_data['bottlenecks'] = bottlenecks
        
        return {
            'status': 'analysis_complete',
            'total_bottlenecks': len(bottlenecks),
            'bottlenecks': bottlenecks,
            'high_severity_count': sum(1 for b in bottlenecks if b['severity'] == 'high'),
            'medium_severity_count': sum(1 for b in bottlenecks if b['severity'] == 'medium'),
            'low_severity_count': sum(1 for b in bottlenecks if b['severity'] == 'low')
        }
        
    def generate_performance_report(self) -> str:
        """Generate performance analysis report."""
        bottleneck_analysis = self.identify_bottlenecks()
        
        report = f"Performance Analysis Report\n"
        report += f"=" * 50 + "\n"
        report += f"Total function profiles: {len(self.profile_data['function_calls'])}\n"
        report += f"Memory samples: {len(self.profile_data['memory_usage'])}\n"
        report += f"Time measurements: {len(self.profile_data['time_measurements'])}\n"
        report += f"CPU samples: {len(self.profile_data['cpu_usage'])}\n"
        report += f"\nPerformance Bottlenecks:\n"
        
        if bottleneck_analysis['total_bottlenecks'] > 0:
            for bottleneck in bottleneck_analysis['bottlenecks']:
                severity_icon = "🚨" if bottleneck['severity'] == 'high' else "⚠️" if bottleneck['severity'] == 'medium' else "ℹ️"
                report += f"  {severity_icon} {bottleneck['type'].replace('_', ' ')}: {bottleneck['name']}\n"
                
                if 'average_time' in bottleneck:
                    report += f"    Average time: {bottleneck['average_time']:.3f}s\n"
                    report += f"    Call count: {bottleneck.get('call_count', 'N/A')}\n"
                    
                if 'memory_growth_mb' in bottleneck:
                    report += f"    Memory growth: {bottleneck['memory_growth_mb']:.1f} MB\n"
                    
                if 'average_duration' in bottleneck:
                    report += f"    Average duration: {bottleneck['average_duration']:.3f}s\n"
                    
                report += f"    Severity: {bottleneck['severity']}\n"
        else:
            report += f"  ✅ No significant performance bottlenecks detected\n"
        
        # Generate recommendations
        report += f"\nPerformance Recommendations:\n"
        if bottleneck_analysis['high_severity_count'] > 0:
            report += f"  🚨 High priority issues detected:\n"
            report += f"    1. Investigate slow functions and optimize code\n"
            report += f"    2. Check for memory leaks and implement proper cleanup\n"
            report += f"    3. Consider parallel processing for CPU-bound tasks\n"
        
        if bottleneck_analysis['medium_severity_count'] > 0:
            report += f"  ⚠️  Medium priority issues:\n"
            report += f"    1. Optimize frequently called functions\n"
            report += f"    2. Monitor memory usage and implement pooling if needed\n"
        
        if bottleneck_analysis['low_severity_count'] > 0:
            report += f"  ℹ️  Low priority suggestions:\n"
            report += f"    1. Consider minor optimizations for consistency\n"
            report += f"    2. Implement periodic monitoring\n"
        
        return report
class CPUProfiler:
    """Profile CPU performance and identify hotspots."""
    
    def __init__(self):
        self.cpu_samples = []
        self.function_profiles = {}
        self.hotspots = []
        
    def sample_cpu(self, operation: str = None):
        """Sample current CPU performance."""
        process = psutil.Process(os.getpid())
        
        # Get CPU times
        cpu_times = process.cpu_times()
        
        # Get CPU utilization
        cpu_percent = process.cpu_percent(interval=0.1)
        
        sample = {
            'timestamp': time.time(),
            'operation': operation,
            'cpu_percent': cpu_percent,
            'user_time': cpu_times.user,
            'system_time': cpu_times.system,
            'total_time': cpu_times.user + cpu_times.system,
            'cpu_usage_ratio': cpu_percent / 100.0
        }
        
        self.cpu_samples.append(sample)
        return sample
        
    def profile_function(self, func_name: str, func_callable, *args, **kwargs):
        """Profile a function call."""
        start_time = time.time()
        
        # Start CPU sampling
        self.sample_cpu(f"start_{func_name}")
        
        try:
            # Execute function
            result = func_callable(*args, **kwargs)
            
            # End CPU sampling
            self.sample_cpu(f"end_{func_name}")
            
            # Calculate performance metrics
            end_time = time.time()
            duration = end_time - start_time
            
            # Get CPU usage during execution
            func_samples = [s for s in self.cpu_samples if s['operation'] in 
                          [f"start_{func_name}", f"end_{func_name}"]]
            
            if len(func_samples) >= 2:
                avg_cpu = sum(s['cpu_percent'] for s in func_samples) / len(func_samples)
            else:
                avg_cpu = 0
            
            # Record function profile
            if func_name not in self.function_profiles:
                self.function_profiles[func_name] = {
                    'call_count': 0,
                    'total_time': 0,
                    'total_cpu': 0,
                    'min_time': float('inf'),
                    'max_time': 0,
                    'avg_cpu': 0,
                    'call_details': []
                }
            
            func_profile = self.function_profiles[func_name]
            func_profile['call_count'] += 1
            func_profile['total_time'] += duration
            func_profile['total_cpu'] += avg_cpu
            func_profile['min_time'] = min(func_profile['min_time'], duration)
            func_profile['max_time'] = max(func_profile['max_time'], duration)
            
            func_profile['call_details'].append({
                'timestamp': end_time,
                'duration': duration,
                'cpu_usage': avg_cpu,
                'cpu_ratio': avg_cpu / 100.0
            })
            
            func_profile['avg_cpu'] = func_profile['total_cpu'] / func_profile['call_count']
            
            return result
            
        except Exception as e:
            # Log error
            error_sample = {
                'timestamp': time.time(),
                'operation': f"error_{func_name}",
                'error': str(e)
            }
            self.cpu_samples.append(error_sample)
            raise
            
    def identify_cpu_hotspots(self) -> Dict[str, Any]:
        """Identify CPU performance hotspots."""
        hotspots = []
        
        if not self.function_profiles:
            return {
                'status': 'no_data',
                'hotspots': [],
                'summary': 'No function profiles collected'
            }
        
        # Sort functions by average time per call
        sorted_functions = sorted(
            self.function_profiles.items(),
            key=lambda x: x[1]['total_time'] / x[1]['call_count'],
            reverse=True
        )
        
        # Identify hotspots
        for func_name, profile in sorted_functions:
            avg_time = profile['total_time'] / profile['call_count']
            total_time = profile['total_time']
            cpu_efficiency = profile['avg_cpu'] / avg_time if avg_time > 0 else 0
            
            # Determine hotspot severity
            if avg_time > 0.5 and cpu_efficiency < 0.5:  # Slow and inefficient
                severity = 'high'
            elif avg_time > 0.1:  # Slow
                severity = 'medium'
            else:
                severity = 'low'
            
            hotspots.append({
                'name': func_name,
                'call_count': profile['call_count'],
                'average_time': avg_time,
                'total_time': total_time,
                'min_time': profile['min_time'],
                'max_time': profile['max_time'],
                'average_cpu_usage': profile['avg_cpu'],
                'cpu_efficiency': cpu_efficiency,
                'severity': severity
            })
        
        self.hotspots = hotspots
        
        return {
            'status': 'analysis_complete',
            'total_functions_profiled': len(self.function_profiles),
            'hotspots': hotspots,
            'high_severity_count': sum(1 for h in hotspots if h['severity'] == 'high'),
            'medium_severity_count': sum(1 for h in hotspots if h['severity'] == 'medium'),
            'low_severity_count': sum(1 for h in hotspots if h['severity'] == 'low'),
            'total_time_spent': sum(h['total_time'] for h in hotspots)
        }
        
    def generate_cpu_report(self) -> str:
        """Generate CPU performance analysis report."""
        hotspot_analysis = self.identify_cpu_hotspots()
        
        report = f"CPU Performance Analysis Report\n"
        report += f"=" * 50 + "\n"
        report += f"Total functions profiled: {hotspot_analysis['total_functions_profiled']}\n"
        report += f"Total time analyzed: {hotspot_analysis['total_time_spent']:.2f}s\n"
        
        if hotspot_analysis['total_functions_profiled'] > 0:
            report += f"\nFunction Performance Rankings:\n"
            
            for i, hotspot in enumerate(hotspot_analysis['hotspots'][:10]):  # Top 10
                severity_icon = "🚨" if hotspot['severity'] == 'high' else "⚠️" if hotspot['severity'] == 'medium' else "ℹ️"
                
                report += f"  {i+1}. {severity_icon} {hotspot['name']}\n"
                report += f"     Calls: {hotspot['call_count']} | "
                report += f"Avg time: {hotspot['average_time']:.3f}s | "
                report += f"Total time: {hotspot['total_time']:.3f}s\n"
                report += f"     CPU efficiency: {hotspot['cpu_efficiency']:.2f} | "
                report += f"Avg CPU: {hotspot['average_cpu_usage']:.1f}%\n"
                report += f"     Min/Max time: {hotspot['min_time']:.3f}s / {hotspot['max_time']:.3f}s\n"
            
            report += f"\nBottleneck Analysis:\n"
            
            high_hotspots = [h for h in hotspot_analysis['hotspots'] if h['severity'] == 'high']
            medium_hotspots = [h for h in hotspot_analysis['hotspots'] if h['severity'] == 'medium']
            
            if high_hotspots:
                report += f"  🚨 High Priority Hotspots ({len(high_hotspots)}):\n"
                for hotspot in high_hotspots:
                    report += f"    - {hotspot['name']}: {hotspot['average_time']:.3f}s average, "
                    report += f"{hotspot['cpu_efficiency']:.2f} efficiency\n"
            
            if medium_hotspots:
                report += f"  ⚠️  Medium Priority Hotspots ({len(medium_hotspots)}):\n"
                for hotspot in medium_hotspots:
                    report += f"    - {hotspot['name']}: {hotspot['average_time']:.3f}s average, "
                    report += f"{hotspot['cpu_efficiency']:.2f} efficiency\n"
            
            # Calculate optimization opportunities
            report += f"\nOptimization Opportunities:\n"
            
            # Find functions with high CPU but low efficiency
            inefficient_functions = [
                h for h in hotspot_analysis['hotspots']
                if h['average_cpu'] > 50 and h['cpu_efficiency'] < 0.3  # High CPU, low efficiency
            ]
            
            if inefficient_functions:
                report += f"  🔧 CPU Optimization Targets:\n"
                for func in inefficient_functions:
                    report += f"    - {func['name']}: {func['average_cpu']:.1f}% CPU, {func['cpu_efficiency']:.2f} efficiency\n"
            
            # Find functions with high time but reasonable efficiency
            time_inefficient = [
                h for h in hotspot_analysis['hotspots']
                if h['average_time'] > 0.1 and h['cpu_efficiency'] > 0.5
            ]
            
            if time_inefficient:
                report += f"  ⏱️  Time Optimization Targets:\n"
                for func in time_inefficient[:3]:  # Top 3
                    report += f"    - {func['name']}: {func['average_time']:.3f}s average, "
                    report += f"call count: {func['call_count']}\n"
        else:
            report += f"\nNo performance data available for analysis\n"
        
        return report


def performance_profiling_example():
    """Demonstrate performance profiling techniques."""
    print_header("4.1 Performance Profiling")
    
    print("\nPerformance profiling and bottleneck identification:")
    
    # Initialize profiler
    profiler = PerformanceProfiler()
    
    # Set baseline memory
    process = psutil.Process(os.getpid())
    profiler.profile_data['baseline_memory'] = process.memory_info().rss / (1024 * 1024)
    
    # Profile system functions
    print("\n4.1.1 Profiling system functions:")
    
    def dummy_cpu_intensive_task(size: int = 1000000):
        """CPU-intensive task for profiling."""
        total = 0
        for i in range(size):
            total += i * 0.001
        return total
    
    def dummy_memory_intensive_task(size: int = 10000):
        """Memory-intensive task for profiling."""
        return [float(i) for i in range(size)]
    
    def dummy_io_intensive_task():
        """I/O intensive task for profiling."""
        import random
        data = [random.random() for _ in range(1000)]
        return sum(data)
    
    # Profile functions
    profiler.start_profile("cpu_intensive_task")
    cpu_result = dummy_cpu_intensive_task(500000)
    profiler.end_profile("cpu_intensive_task")
    
    profiler.start_profile("memory_intensive_task")
    mem_result = dummy_memory_intensive_task(5000)
    profiler.end_profile("memory_intensive_task")
    
    profiler.start_profile("io_intensive_task")
    io_result = dummy_io_intensive_task()
    profiler.end_profile("io_intensive_task")
    
    # Record system metrics
    print("\n4.1.2 Recording system metrics:")
    
    for i in range(5):
        profiler.sample_cpu(f"measurement_{i}")
        profiler.record_memory_usage(f"measurement_{i}")
        time.sleep(0.1)
    
    # Record time measurements
    print("\n4.1.3 Recording time measurements:")
    
    profiler.record_time_measurement("simulation_step", 0.15)
    profiler.record_time_measurement("simulation_step", 0.22)
    profiler.record_time_measurement("simulation_step", 0.18)
    profiler.record_time_measurement("simulation_step", 0.25)
    profiler.record_time_measurement("simulation_step", 0.12)
    
    # Generate performance report
    print("\n4.1.4 Performance analysis report:")
    
    performance_report = profiler.generate_performance_report()
    print(performance_report)
    
    return profiler


def cpu_profiling_example():
    """Demonstrate CPU profiling."""
    print_header("4.2 CPU Profiling")
    
    print("\nCPU profiling and hotspot identification:")
    
    # Initialize CPU profiler
    cpu_profiler = CPUProfiler()
    
    # Define test functions with different performance characteristics
    def fast_function():
        """Very fast function."""
        return sum(range(100))
    
    def medium_function():
        """Medium-speed function."""
        result = 0
        for i in range(10000):
            result += i * 0.001
        return result
    
    def slow_function():
        """Slow function with high CPU usage."""
        result = 0
        for i in range(100000):
            for j in range(10):
                result += i * j * 0.0001
        return result
    
    def very_slow_function():
        """Very slow function with high CPU and I/O."""
        import time
        time.sleep(0.05)  # Simulate I/O
        result = 0
        for i in range(500000):
            result += (i * 0.0001) ** 2
        return result
    
    # Profile functions multiple times
    print("\n4.2.1 Function profiling:")
    
    for _ in range(5):
        cpu_profiler.profile_function("fast_function", fast_function)
        time.sleep(0.01)
        
        cpu_profiler.profile_function("medium_function", medium_function)
        time.sleep(0.01)
        
        cpu_profiler.profile_function("slow_function", slow_function)
        time.sleep(0.05)
        
        cpu_profiler.profile_function("very_slow_function", very_slow_function)
        time.sleep(0.1)
    
    # Generate CPU report
    print("\n4.2.2 CPU performance report:")
    
    cpu_report = cpu_profiler.generate_cpu_report()
    print(cpu_report)
    
    # Sample CPU usage during operations
    print("\n4.2.3 Real-time CPU monitoring:")
    
    for i in range(10):
        sample = cpu_profiler.sample_cpu(f"monitoring_{i}")
        print(f"  Sample {i}: {sample['cpu_percent']:.1f}% CPU, "
              f"user: {sample['user_time']:.2f}s, system: {sample['system_time']:.2f}s")
        
        if i % 3 == 0:
            # Run CPU-intensive task
            result = dummy_cpu_intensive_task(200000)
            print(f"    Running CPU-intensive task...")
    
    return cpu_profiler


def profiling_examples_summary():
    """Print summary of performance profiling examples."""
    print_header("Performance Profiling Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Performance profiling with function call tracking")
    print("✓ Memory usage monitoring and analysis")
    print("✓ CPU usage tracking and sampling")
    print("✓ Bottleneck identification and severity classification")
    print("✓ Detailed performance reporting and analysis")
    print("✓ Real-time performance monitoring")
    print("✓ CPU hotspot detection and efficiency analysis")
    print("✓ Optimization opportunity identification")
    print("✓ Performance comparison across function types")

# ============================================================================
# Main Execution
# ============================================================================

def main():
    """Run all debugging and troubleshooting examples."""
    print("=" * 80)
    print("NLM Debugging and Troubleshooting Examples")
    print("Comprehensive collection demonstrating debugging techniques")
    print("=" * 80)
    
    # Run memory debugging examples
    memory_tracker = memory_leak_detection_example()
    
    # Run garbage collection debugging example
    gc_debugger = garbage_collection_debugging_example()
    
    # Run advanced memory leak detection example
    leak_detector = advanced_memory_leak_detection_example()
    
    # Run neuromodulation signal validation example
    signal_validator = neuromodulation_signal_validation_example()
    
    # Run neuromodulation monitoring example
    monitor = neuromodulation_monitoring_example()
    
    # Run signal visualization example
    visualizer = signal_visualization_example()
    
    # Run simulation state inspection example
    inspector = simulation_state_inspection_example()
    
    # Run state transition analysis example
    analyzer = state_transition_analysis_example()
    
    # Run performance profiling example
    profiler = performance_profiling_example()
    
    # Run CPU profiling example
    cpu_profiler = cpu_profiling_example()
    
    print("\n" + "=" * 80)
    print("All debugging and troubleshooting examples completed successfully!")
    print("=" * 80)
    
    print("\nDebugging and Troubleshooting Features Demonstrated:")
    print("1. Memory Leak Detection and Analysis")
    print("2. Garbage Collection Debugging and Object Lifecycle")
    print("3. Advanced Statistical Memory Leak Detection")
    print("4. Neuromodulation Signal Validation and Anomaly Detection")
    print("5. Real-Time Monitoring and Alerting")
    print("6. Signal Visualization for Debugging")
    print("7. Simulation State Inspection and Analysis")
    print("8. State Transition Pattern Detection")
    print("9. Performance Profiling and Bottleneck Identification")
    print("10. CPU Performance Analysis and Hotspot Detection")
    
    print("\nDebugging and Troubleshooting Capabilities:")
    print("• Memory usage tracking and analysis")
    print("• Object lifecycle and garbage collection debugging")
    print("• Statistical anomaly detection and pattern recognition")
    print("• Signal validation and quality assessment")
    print("• Real-time monitoring and alerting systems")
    print("• Visualization tools for debugging insights")
    print("• Comprehensive state inspection and comparison")
    print("• Performance profiling and optimization recommendations")
    print("• CPU hotspot detection and efficiency analysis")
    
    print("\nNext Steps for Debugging:")
    print("1. Start with memory leak detection and analysis")
    print("2. Implement signal validation and monitoring")
    print("3. Set up simulation state inspection")
    print("4. Profile performance and identify bottlenecks")
    print("5. Use visualization tools for deeper analysis")
    print("6. Apply optimization recommendations")
    print("7. Set up continuous monitoring for production")

if __name__ == "__main__":
    main()