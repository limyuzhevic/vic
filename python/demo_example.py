#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools Demo

This demo showcases the enhanced visualization and debugging capabilities
for the NLM (Neural Learning Machine) framework. It provides a comprehensive
example of how to use the new visualization and debugging tools.

The demo includes:
1. Brain visualization with real-time activity monitoring
2. World simulation visualization
3. Performance monitoring and debugging
4. Memory leak detection
5. Error tracking and analysis
6. Interactive debugging interface
7. Data export and reporting
"""

import time
import json
import os
from datetime import datetime
from typing import Dict, Any, List

# Add the current directory to the path
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm_visualization_tools import (
    VisualizationInterface,
    DebugInterface,
    VisualizationMode,
    DebugMode,
    MonitoringMode,
    OutputFormat,
    NeuronActivityData,
    TimeSeriesDataPoint,
    DebugInfo,
    MonitoringHook,
    PerformanceMetrics,
    NeuralMetrics,
    ErrorInfo,
    NLMVisualizationTools
)

class NLMVisualizationDemo:
    """Demo class for NLM visualization and debugging tools"""
    
    def __init__(self, library_path: str = None):
        self.library_path = library_path
        self.viz = None
        self.debug = None
        self.tools = None
        self.start_time = 0
        self.step = 0
        
        # Demo configuration
        self.config = {
            'demo_duration': 30,  # seconds
            'update_rate': 10,   # Hz
            'neuron_count': 100,
            'firing_rate_range': (5.0, 20.0),
            'memory_leak_probability': 0.01,
            'error_rate': 0.05,
        }
    
    def initialize(self) -> bool:
        """Initialize the demo"""
        print("Initializing NLM Visualization and Debugging Demo...")
        
        try:
            # Initialize tools
            self.tools = NLMVisualizationTools(self.library_path)
            if not self.tools.initialize():
                raise RuntimeError("Failed to initialize NLM tools")
            
            # Get interfaces
            self.viz = self.tools.get_visualization_interface()
            self.debug = self.tools.get_debug_interface()
            
            # Configure visualization interface
            self.viz.set_update_rate(self.config['update_rate'])
            self.viz.set_visualization_mode(VisualizationMode.REAL_TIME)
            self.viz.enable_real_time_updates(True)
            self.viz.enable_debug_overlay(True)
            
            # Configure debug interface
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.set_monitoring_mode(MonitoringMode.ACTIVE)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
            self.debug.enable_error_detection(True)
            self.debug.enable_command_line_interface(False)  # Disable for demo
            self.debug.set_output_level(1)
            
            # Add monitoring hooks
            self._add_monitoring_hooks()
            
            self.start_time = time.time()
            
            print("✓ Demo initialized successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to initialize demo: {e}")
            return False
    
    def _add_monitoring_hooks(self):
        """Add monitoring hooks for demonstration"""
        
        def performance_callback(metrics: PerformanceMetrics):
            """Performance monitoring callback"""
            # Check for performance issues
            if metrics.cpu_usage > 80.0:
                self._log_warning(f"High CPU usage: {metrics.cpu_usage:.1f}%")
            if metrics.memory_usage > 80.0:
                self._log_warning(f"High memory usage: {metrics.memory_usage:.1f}%")
        
        def memory_callback(allocations: List[MemoryAllocation]):
            """Memory monitoring callback"""
            leaks = [a for a in allocations if a.is_leaked]
            if leaks:
                self._log_error(f"Memory leaks detected: {len(leaks)}")
        
        def neural_callback(metrics: NeuralMetrics):
            """Neural activity monitoring callback"""
            if metrics.average_firing_rate > 15.0:
                self._log_warning(f"High firing rate: {metrics.average_firing_rate:.1f} Hz")
        
        # Create monitoring hooks
        hooks = [
            MonitoringHook(
                hook_name="performance_monitor",
                callback=performance_callback,
                last_execution=time.time(),
                execution_interval=1.0,
                is_active=True,
                hook_type="periodic"
            ),
            MonitoringHook(
                hook_name="memory_monitor",
                callback=memory_callback,
                last_execution=time.time(),
                execution_interval=2.0,
                is_active=True,
                hook_type="periodic"
            ),
            MonitoringHook(
                hook_name="neural_monitor",
                callback=neural_callback,
                last_execution=time.time(),
                execution_interval=1.5,
                is_active=True,
                hook_type="periodic"
            )
        ]
        
        # Add hooks to visualization interface
        for hook in hooks:
            self.viz.add_monitoring_hook(hook)
    
    def _log_warning(self, message: str):
        """Log a warning message"""
        warning_info = DebugInfo(
            component_name="DemoSystem",
            current_state="Warning",
            warnings=[message],
            errors=[],
            last_update_time=time.time(),
            is_healthy=False
        )
        self.viz.add_debug_info(warning_info)
        print(f"⚠ {message}")
    
    def _log_error(self, message: str):
        """Log an error message"""
        error_info = DebugInfo(
            component_name="DemoSystem",
            current_state="Error",
            warnings=[],
            errors=[message],
            last_update_time=time.time(),
            is_healthy=False
        )
        self.viz.add_debug_info(error_info)
        print(f"✗ {message}")
    
    def run_brain_activity_demo(self, duration: int):
        """Run brain activity visualization demo"""
        print(f"\nRunning Brain Activity Visualization Demo ({duration} seconds)...")
        
        end_time = time.time() + duration
        
        while time.time() < end_time:
            current_time = time.time()
            elapsed = current_time - self.start_time
            self.step = int(elapsed * self.config['update_rate'])
            
            # Update interfaces
            self.viz.update()
            self.debug.update()
            
            # Generate demo data
            self._generate_brain_activity_data()
            
            # Log status periodically
            if self.step % 10 == 0:
                self._print_status()
            
            # Sleep to maintain update rate
            time.sleep(1.0 / self.config['update_rate'])
        
        print("✓ Brain activity demo completed")
    
    def run_world_simulation_demo(self, duration: int):
        """Run world simulation visualization demo"""
        print(f"\nRunning World Simulation Visualization Demo ({duration} seconds)...")
        
        end_time = time.time() + duration
        
        while time.time() < end_time:
            current_time = time.time()
            elapsed = current_time - self.start_time
            self.step = int(elapsed * self.config['update_rate'])
            
            # Update interfaces
            self.viz.update()
            self.debug.update()
            
            # Generate world simulation data
            self._generate_world_simulation_data()
            
            # Log status periodically
            if self.step % 20 == 0:
                self._print_world_status()
            
            # Sleep to maintain update rate
            time.sleep(1.0 / self.config['update_rate'])
        
        print("✓ World simulation demo completed")
    
    def _generate_brain_activity_data(self):
        """Generate brain activity visualization data"""
        current_time = time.time() - self.start_time
        
        # Generate neuron activity data
        for i in range(self.config['neuron_count']):
            neuron_data = NeuronActivityData(
                neuron_id=i,
                membrane_potential=60.0 + (i % 10) * 2.0,
                firing_rate=10.0 + (i % 5) * 3.0 + (current_time % 2.0),
                inhibition_level=0.1 + (i % 3) * 0.1,
                last_spike_time=current_time - (i % 5),
                is_active=(i + self.step) % 7 == 0
            )
            
            # Add to visualization (simplified)
            if self.step % 5 == 0:  # Less frequent updates
                self.viz.add_time_series_metric(
                    f"neuron_{i}_firing_rate",
                    neuron_data.firing_rate
                )
        
        # Add system-wide metrics
        firing_rate = 10.0 + (current_time % 10.0)
        self.viz.add_time_series_metric("system_firing_rate", firing_rate)
        self.viz.add_time_series_metric("system_memory_usage", 50.0 + (current_time % 30.0))
        self.viz.add_time_series_metric("system_cpu_usage", 30.0 + (current_time % 40.0))
        
        # Log occasional errors
        if self.step % 50 == 0 and self.config['error_rate'] > 0:
            error_prob = self.config['error_rate']
            if hash(str(self.step)) % 100 < error_prob * 100:
                self._log_error(f"Neuron {self.step % self.config['neuron_count']} malfunction detected")
    
    def _generate_world_simulation_data(self):
        """Generate world simulation visualization data"""
        current_time = time.time() - self.start_time
        
        # Simulate world state
        world_state = {
            'width': 16,
            'height': 16,
            'agent_position': (current_time * 2.0 % 16, current_time * 3.0 % 16),
            'resources': [50.0 + (hash(str(t)) % 50) for t in range(16)],
            'time': current_time
        }
        
        # Add world metrics to visualization
        self.viz.add_time_series_metric("world_time", world_state['time'])
        self.viz.add_time_series_metric("agent_x", world_state['agent_position'][0])
        self.viz.add_time_series_metric("agent_y", world_state['agent_position'][1])
        self.viz.add_time_series_metric("resource_level", sum(world_state['resources']) / len(world_state['resources']))
        
        # Log world events
        if self.step % 30 == 0:
            self._log_warning(f"Agent moved to position ({world_state['agent_position'][0]:.1f}, {world_state['agent_position'][1]:.1f})")
    
    def _print_status(self):
        """Print current status"""
        current_time = time.time() - self.start_time
        
        print(f"\n--- Step {self.step} Status (Elapsed: {current_time:.1f}s) ---")
        
        # Get metrics
        perf_metrics = self.debug.get_performance_metrics()
        neural_metrics = self.debug.get_neural_metrics()
        
        print(f"Performance: CPU={perf_metrics.cpu_usage:.1f}%, Memory={perf_metrics.memory_usage:.1f}%")
        print(f"Neural Activity: {neural_metrics.active_neuron_count} active neurons, {neural_metrics.average_firing_rate:.1f} Hz avg firing rate")
        print(f"Debug Info: {len(self.viz.get_debug_info())} entries")
        print(f"Monitoring Hooks: {len(self.viz.get_monitoring_hooks())} active")
        
        # Show recent debug info
        debug_infos = self.viz.get_debug_info()
        if debug_infos:
            recent_info = debug_infos[-3:]
            print("Recent Debug Info:")
            for info in recent_info:
                status = "✓" if info.is_healthy else "⚠" if info.warnings else "✗"
                print(f"  {status} {info.component_name}: {info.current_state}")
    
    def _print_world_status(self):
        """Print world simulation status"""
        current_time = time.time() - self.start_time
        
        print(f"\n--- World Simulation Status (Step {self.step}) ---")
        
        # Get world metrics
        world_metrics = {
            'time': self.viz.get_time_series_data("world_time")[-1].value if self.viz.get_time_series_data("world_time") else 0,
            'agent_x': self.viz.get_time_series_data("agent_x")[-1].value if self.viz.get_time_series_data("agent_x") else 0,
            'agent_y': self.viz.get_time_series_data("agent_y")[-1].value if self.viz.get_time_series_data("agent_y") else 0,
            'resources': self.viz.get_time_series_data("resource_level")[-1].value if self.viz.get_time_series_data("resource_level") else 0,
        }
        
        print(f"Time: {world_metrics['time']:.1f}s")
        print(f"Agent Position: ({world_metrics['agent_x']:.1f}, {world_metrics['agent_y']:.1f})")
        print(f"Resources: {world_metrics['resources']:.1f}")
        
        # Show world debug info
        debug_infos = self.viz.get_debug_info()
        world_infos = [info for info in debug_infos if "World" in info.component_name]
        if world_infos:
            print("World Events:")
            for info in world_infos[-2:]:
                status = "✓" if info.is_healthy else "⚠"
                print(f"  {status} {info.current_state}")
    
    def run_performance_profiling_demo(self, duration: int):
        """Run performance profiling demo"""
        print(f"\nRunning Performance Profiling Demo ({duration} seconds)...")
        
        end_time = time.time() + duration
        
        while time.time() < end_time:
            current_time = time.time()
            elapsed = current_time - self.start_time
            self.step = int(elapsed * self.config['update_rate'])
            
            # Update interfaces
            self.viz.update()
            self.debug.update()
            
            # Profile different aspects
            self._profile_performance()
            
            # Log profiling results periodically
            if self.step % 20 == 0:
                self._print_profiling_results()
            
            time.sleep(1.0 / self.config['update_rate'])
        
        print("✓ Performance profiling demo completed")
    
    def _profile_performance(self):
        """Profile performance metrics"""
        current_time = time.time() - self.start_time
        
        # Simulate performance metrics
        perf_metrics = self.debug.get_performance_metrics()
        perf_metrics.cpu_usage = 20.0 + (current_time % 60.0)  # 20-80%
        perf_metrics.memory_usage = 30.0 + (current_time % 50.0)  # 30-80%
        perf_metrics.disk_usage = 10.0 + (current_time % 30.0)  # 10-40%
        perf_metrics.timestamp = current_time
        
        # Update neural metrics
        neural_metrics = self.debug.get_neural_metrics()
        neural_metrics.total_neuron_count = self.config['neuron_count']
        neural_metrics.active_neuron_count = int(
            self.config['neuron_count'] * 0.5 * (1.0 + (current_time % 2.0) / 2.0)
        )
        neural_metrics.spiking_neuron_count = int(
            neural_metrics.active_neuron_count * 0.3 * (0.5 + (current_time % 1.0) / 2.0)
        )
        neural_metrics.average_firing_rate = (
            self.config['firing_rate_range'][0] + 
            (self.config['firing_rate_range'][1] - self.config['firing_rate_range'][0]) *
            (0.5 + (current_time % 1.0) / 2.0)
        )
        neural_metrics.timestamp = current_time
        
        # Update simulation metrics
        sim_metrics = self.debug.get_simulation_metrics()
        sim_metrics.current_step = self.step
        sim_metrics.simulation_time = current_time
        sim_metrics.wall_clock_time = current_time
        sim_metrics.steps_per_second = self.config['update_rate']
        sim_metrics.efficiency = 0.8 + (current_time % 0.4)  # 0.8-1.2
        sim_metrics.spike_count = int(self.step * 2.5 + (current_time % 10.0))
        sim_metrics.event_count = int(self.step * 1.5 + (current_time % 5.0))
        sim_metrics.timestamp = current_time
    
    def _print_profiling_results(self):
        """Print performance profiling results"""
        print(f"\n--- Performance Profiling (Step {self.step}) ---")
        
        perf_metrics = self.debug.get_performance_metrics()
        neural_metrics = self.debug.get_neural_metrics()
        sim_metrics = self.debug.get_simulation_metrics()
        
        print(f"Performance Metrics:")
        print(f"  CPU Usage: {perf_metrics.cpu_usage:.1f}%")
        print(f"  Memory Usage: {perf_metrics.memory_usage:.1f}%")
        print(f"  Disk Usage: {perf_metrics.disk_usage:.1f}%")
        print(f"  Network Sent: {perf_metrics.networkBytesSent / 1024:.1f} KB")
        print(f"  Network Received: {perf_metrics.networkBytesReceived / 1024:.1f} KB")
        
        print(f"\nNeural Metrics:")
        print(f"  Total Neurons: {neural_metrics.total_neuron_count}")
        print(f"  Active Neurons: {neural_metrics.active_neuron_count}")
        print(f"  Spiking Neurons: {neural_metrics.spiking_neuron_count}")
        print(f"  Average Firing Rate: {neural_metrics.average_firing_rate:.1f} Hz")
        print(f"  Excitatory/Inhibitory Ratio: {neural_metrics.excitationInhibitionRatio:.2f}")
        
        print(f"\nSimulation Metrics:")
        print(f"  Current Step: {sim_metrics.current_step}")
        print(f"  Simulation Time: {sim_metrics.simulation_time:.1f}s")
        print(f"  Wall Clock Time: {sim_metrics.wallClockTime:.1f}s")
        print(f"  Steps per Second: {sim_metrics.stepsPerSecond:.1f}")
        print(f"  Efficiency: {sim_metrics.efficiency:.2f}")
        print(f"  Total Spikes: {sim_metrics.spikeCount}")
        print(f"  Total Events: {sim_metrics.eventCount}")
    
    def run_memory_leak_demo(self, duration: int):
        """Run memory leak detection demo"""
        print(f"\nRunning Memory Leak Detection Demo ({duration} seconds)...")
        
        end_time = time.time() + duration
        
        while time.time() < end_time:
            current_time = time.time()
            elapsed = current_time - self.start_time
            self.step = int(elapsed * self.config['update_rate'])
            
            # Update interfaces
            self.viz.update()
            self.debug.update()
            
            # Simulate memory allocations and deallocations
            self._simulate_memory_operations()
            
            # Check for memory leaks
            if self.step % 25 == 0:
                self._check_memory_leaks()
            
            time.sleep(1.0 / self.config['update_rate'])
        
        print("✓ Memory leak detection demo completed")
    
    def _simulate_memory_operations(self):
        """Simulate memory allocation and deallocation"""
        current_time = time.time() - self.start_time
        
        # Simulate memory allocations (every second)
        if self.step % int(self.config['update_rate']) == 0:
            # Allocate some memory
            for i in range(3):
                address = hash(str(self.step) + str(i)) & 0xFFFFFFFF
                size = 1024 + (i * 512)  # 1KB to 2KB
                self.debug.track_memory_allocation(address, size, f"allocation_{self.step}_{i}")
        
        # Simulate memory deallocations (every 2 seconds)
        if self.step % int(self.config['update_rate'] * 2) == 0:
            # Deallocate some memory
            allocated_count = len(self.debug._active_allocations)
            for i in range(min(2, allocated_count)):
                if allocated_count > 0:
                    # Get the most recent allocation
                    allocation = self.debug._active_allocations[-1]
                    self.debug.track_memory_deallocation(allocation.address)
        
        # Occasionally create memory leaks (simulated)
        if self.step % 100 == 0 and self.config['memory_leak_probability'] > 0:
            leak_prob = self.config['memory_leak_probability']
            if hash(str(self.step)) % 100 < leak_prob * 100:
                # Simulate a memory leak
                leak_address = hash(str(self.step * 1000)) & 0xFFFFFFFF
                leak_size = 4096  # 4KB leak
                self.debug.track_memory_allocation(leak_address, leak_size, f"LEAK_{self.step}")
                # Don't deallocate this one (simulating a leak)
    
    def _check_memory_leaks(self):
        """Check for memory leaks"""
        leaks = self.debug.detect_memory_leaks()
        if leaks:
            print(f"⚠ Memory leaks detected: {len(leaks)}")
            for leak in leaks[:3]:  # Show first 3 leaks
                print(f"  Leak at {leak.address:#x} ({leak.size} bytes from {leak.allocation_site})")
        else:
            print("✓ No memory leaks detected")
    
    def run_error_detection_demo(self, duration: int):
        """Run error detection demo"""
        print(f"\nRunning Error Detection Demo ({duration} seconds)...")
        
        end_time = time.time() + duration
        
        while time.time() < end_time:
            current_time = time.time()
            elapsed = current_time - self.start_time
            self.step = int(elapsed * self.config['update_rate'])
            
            # Update interfaces
            self.viz.update()
            self.debug.update()
            
            # Generate and log errors
            self._generate_errors()
            
            # Log error summary periodically
            if self.step % 30 == 0:
                self._print_error_summary()
            
            time.sleep(1.0 / self.config['update_rate'])
        
        print("✓ Error detection demo completed")
    
    def _generate_errors(self):
        """Generate and log errors"""
        current_time = time.time() - self.start_time
        
        # Generate errors based on error rate
        error_prob = self.config['error_rate']
        if hash(str(self.step)) % 100 < error_prob * 100:
            # Generate different types of errors
            error_types = ["MEMORY_ERROR", "NETWORK_ERROR", "CPU_ERROR", "IO_ERROR"]
            error_type = error_types[self.step % len(error_types)]
            
            # Determine if error is critical
            is_critical = (self.step % 50 == 0)
            
            # Log the error
            self.debug.log_error(
                error_type,
                f"Simulated {error_type} at step {self.step}",
                is_critical,
                {
                    "component": "SimulationSystem",
                    "timestamp": str(current_time),
                    "step": str(self.step)
                }
            )
    
    def _print_error_summary(self):
        """Print error summary"""
        print(f"\n--- Error Summary (Step {self.step}) ---")
        
        recent_errors = self.debug.get_recent_errors()
        critical_errors = self.debug.get_critical_errors()
        
        print(f"Recent Errors: {len(recent_errors)}")
        print(f"Critical Errors: {len(critical_errors)}")
        
        if recent_errors:
            print("\nRecent Error Details:")
            for error in recent_errors[-3:]:
                status = "CRITICAL" if error.is_critical else "Warning"
                print(f"  {status}: {error.error_code} - {error.errorMessage}")
    
    def generate_report(self) -> Dict[str, Any]:
        """Generate a comprehensive report"""
        report = {
            "demo_configuration": self.config,
            "simulation_summary": {
                "steps_completed": self.step,
                "duration_seconds": time.time() - self.start_time,
                "average_fps": self.step / max(1, time.time() - self.start_time)
            },
            "performance_metrics": self.debug.get_performance_metrics().__dict__,
            "neural_metrics": self.debug.get_neural_metrics().__dict__,
            "simulation_metrics": self.debug.get_simulation_metrics().__dict__,
            "debug_info": [info.__dict__ for info in self.viz.get_debug_info()],
            "time_series_data": [point.__dict__ for point in self.viz._time_series_data],
            "monitoring_hooks": [hook.__dict__ for hook in self.viz._monitoring_hooks],
            "memory_allocations": [alloc.__dict__ for alloc in self.debug._active_allocations],
            "errors": [
                error.__dict__ for error in self.debug.get_recent_errors()
            ]
        }
        
        return report
    
    def export_report(self, filename: str, format: OutputFormat = OutputFormat.JSON):
        """Export report to file"""
        if format == OutputFormat.JSON:
            report = self.generate_report()
            with open(filename, 'w') as f:
                json.dump(report, f, indent=2, default=str)
            print(f"✓ Report exported to: {filename}")
        else:
            print(f"Export format {format.value} not implemented")
    
    def cleanup(self):
        """Cleanup and close resources"""
        print("\nCleaning up...")
        
        if self.tools:
            self.tools.close()
            print("✓ Tools closed")
        
        # Clean up temporary files
        temp_files = [
            "nlm_visualization_demo.json",
            "brain_activity_demo.json",
            "world_simulation_demo.json",
            "performance_profiling_demo.json",
            "memory_leak_demo.json",
            "error_detection_demo.json"
        ]
        
        for temp_file in temp_files:
            if os.path.exists(temp_file):
                os.remove(temp_file)
                print(f"✓ Cleaned up: {temp_file}")
    
    def run_complete_demo(self):
        """Run complete demo with all components"""
        print("\n" + "=" * 60)
        print("NLM VISUALIZATION AND DEBUGGING TOOLS - COMPLETE DEMO")
        print("=" * 60)
        
        # Initialize
        if not self.initialize():
            return False
        
        # Run demos
        try:
            # Brain activity visualization
            self.run_brain_activity_demo(15)
            
            # World simulation visualization
            self.run_world_simulation_demo(15)
            
            # Performance profiling
            self.run_performance_profiling_demo(10)
            
            # Memory leak detection
            self.run_memory_leak_demo(10)
            
            # Error detection
            self.run_error_detection_demo(5)
            
            # Generate and export final report
            print("\n6. Generating Final Report...")
            final_report = self.generate_report()
            
            # Export report
            self.export_report("complete_demo_report.json", OutputFormat.JSON)
            
            print("✓ Complete demo completed successfully")
            
            return True
            
        except Exception as e:
            print(f"✗ Demo failed: {e}")
            return False
        
        finally:
            self.cleanup()
def main():
    """Main function"""
    print("NLM Visualization and Debugging Tools Demo")
    print("=" * 60)
    
    # Create and run demo
    demo = NLMVisualizationDemo()
    
    if demo.run_complete_demo():
        print("\n" + "=" * 60)
        print("DEMONSTRATION COMPLETED SUCCESSFULLY")
        print("=" * 60)
        print("\nKey features demonstrated:")
        print("  ✓ Real-time brain activity visualization")
        print("  ✓ World simulation visualization")
        print("  ✓ Performance monitoring and profiling")
        print("  ✓ Memory leak detection")
        print("  ✓ Error detection and analysis")
        print("  ✓ Monitoring hooks and callbacks")
        print("  ✓ Data export and reporting")
        print("  ✓ Step-by-step debugging")
        print("\nThe NLM visualization and debugging tools provide a")
        print("comprehensive framework for monitoring, analyzing, and")
        print("debugging neural simulations in real-time.")
        
        return 0
    else:
        print("\nDemonstration failed!")
        return 1
if __name__ == "__main__":
    exit_code = main()
    exit(exit_code)
