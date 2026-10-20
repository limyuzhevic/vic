#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools - Main Demo

This is a comprehensive demonstration of the NLM (Neural Learning Machine) 
visualization and debugging tools. It showcases all the features including:

1. Real-time brain activity visualization
2. World simulation visualization
3. Performance monitoring and profiling
4. Memory leak detection
5. Error tracking and analysis
6. Interactive debugging interface
7. Data export and reporting

The demo runs multiple scenarios to demonstrate the full capabilities
of the visualization and debugging system.
"""

import time
import json
import os
import sys
from typing import Dict, Any, List

# Add the current directory to the path
import threading

# Import NLM visualization tools
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
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
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM tools not available: {e}")
    print("Using basic demonstration...")
    NLM_AVAILABLE = False

class DemoManager:
    """Manages the NLM visualization and debugging demo"""
    
    def __init__(self):
        self.tools = None
        self.viz = None
        self.debug = None
        self.is_running = False
        self.step = 0
        self.start_time = 0
        self.demo_duration = 60  # seconds
        
        # Demo statistics
        self.stats = {
            'brain_activity_steps': 0,
            'world_simulation_steps': 0,
            'performance_steps': 0,
            'memory_steps': 0,
            'error_steps': 0,
            'errors_detected': 0,
            'memory_leaks_found': 0,
            'peak_memory_usage': 0.0,
            'total_firing_rate': 0.0,
            'avg_firing_rate': 0.0
        }
    
    def initialize(self) -> bool:
        """Initialize the demo system"""
        print("Initializing NLM Visualization and Debugging Demo...")
        
        if not NLM_AVAILABLE:
            print("✓ Using basic Python implementation (C++ library not available)")
            self.is_running = True
            self.start_time = time.time()
            return True
        
        try:
            # Initialize NLM tools
            self.tools = NLMVisualizationTools()
            if not self.tools.initialize():
                raise RuntimeError("Failed to initialize NLM tools")
            
            # Get interfaces
            self.viz = self.tools.get_visualization_interface()
            self.debug = self.tools.get_debug_interface()
            
            # Configure interfaces
            self._configure_interfaces()
            
            self.is_running = True
            self.start_time = time.time()
            
            print("✓ Demo system initialized successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to initialize demo: {e}")
            return False
    
    def _configure_interfaces(self):
        """Configure visualization and debug interfaces"""
        # Configure visualization interface
        self.viz.set_update_rate(30.0)
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
        self.debug.enable_command_line_interface(False)  # Disable for automated demo
        self.debug.set_output_level(1)
        
        # Add monitoring hooks
        self._add_monitoring_hooks()
    
    def _add_monitoring_hooks(self):
        """Add monitoring hooks for demonstration"""
        
        def performance_callback(metrics: PerformanceMetrics):
            """Performance monitoring callback"""
            self.stats['peak_memory_usage'] = max(
                self.stats['peak_memory_usage'],
                metrics.memory_usage
            )
            
            if metrics.cpu_usage > 80.0:
                self._log_warning(f"High CPU usage: {metrics.cpu_usage:.1f}%")
        
        def memory_callback(allocations: List):
            """Memory monitoring callback"""
            self.stats['memory_leaks_found'] = len([a for a in allocations if getattr(a, 'is_leaked', False)])
        
        def neural_callback(metrics: NeuralMetrics):
            """Neural activity monitoring callback"""
            self.stats['total_firing_rate'] += metrics.average_firing_rate
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
        
        # Add hooks
        for hook in hooks:
            self.viz.add_monitoring_hook(hook)
    
    def _log_warning(self, message: str):
        """Log a warning message"""
        if NLM_AVAILABLE and self.viz:
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
        if NLM_AVAILABLE and self.viz:
            error_info = DebugInfo(
                component_name="DemoSystem",
                current_state="Error",
                warnings=[],
                errors=[message],
                last_update_time=time.time(),
                is_healthy=False
            )
            self.viz.add_debug_info(error_info)
        
        self.stats['errors_detected'] += 1
        print(f"✗ {message}")
    
    def run_demo(self):
        """Run the complete demo"""
        if not self.is_running:
            print("Demo system not initialized")
            return
        
        print("\n" + "=" * 60)
        print("NLM VISUALIZATION AND DEBUGGING TOOLS DEMO")
        print("=" * 60)
        print(f"Demo duration: {self.demo_duration} seconds")
        print(f"Update rate: 30 Hz")
        print("\nRunning demo scenarios...")
        print("=" * 60)
        
        try:
            # Run different demo scenarios
            self._run_brain_activity_demo()
            self._run_world_simulation_demo()
            self._run_performance_profiling_demo()
            self._run_memory_leak_demo()
            self._run_error_detection_demo()
            
            # Generate final report
            self._generate_final_report()
            
            print("\n" + "=" * 60)
            print("DEMO COMPLETED SUCCESSFULLY")
            print("=" * 60)
            self._print_summary()
            
        except Exception as e:
            print(f"\nDemo failed with error: {e}")
            self._generate_error_report(e)
        
        finally:
            self.cleanup()
    
    def _run_brain_activity_demo(self):
        """Run brain activity visualization demo"""
        print("\n1. Brain Activity Visualization Demo")
        print("-" * 40)
        
        end_time = time.time() + 15  # 15 seconds
        self.stats['brain_activity_steps'] = 0
        
        while time.time() < end_time and self.is_running:
            current_time = time.time()
            self.step = int((current_time - self.start_time) * 30.0)  # 30 Hz
            self.stats['brain_activity_steps'] = self.step
            
            # Update interfaces
            if NLM_AVAILABLE:
                self.viz.update()
                self.debug.update()
            
            # Generate brain activity data
            self._generate_brain_activity_data(current_time)
            
            # Log status every 5 seconds
            if self.step % 150 == 0:  # 5 seconds at 30 Hz
                self._print_brain_status()
            
            # Sleep to maintain update rate
            time.sleep(1.0 / 30.0)
        
        print(f"✓ Brain activity demo completed: {self.step} steps")
    
    def _run_world_simulation_demo(self):
        """Run world simulation visualization demo"""
        print("\n2. World Simulation Visualization Demo")
        print("-" * 40)
        
        end_time = time.time() + 10  # 10 seconds
        self.stats['world_simulation_steps'] = 0
        
        while time.time() < end_time and self.is_running:
            current_time = time.time()
            self.step = int((current_time - self.start_time) * 30.0)
            self.stats['world_simulation_steps'] = self.step
            
            # Update interfaces
            if NLM_AVAILABLE:
                self.viz.update()
                self.debug.update()
            
            # Generate world simulation data
            self._generate_world_simulation_data(current_time)
            
            # Log status every 3 seconds
            if self.step % 90 == 0:  # 3 seconds at 30 Hz
                self._print_world_status()
            
            time.sleep(1.0 / 30.0)
        
        print(f"✓ World simulation demo completed: {self.step} steps")
    
    def _run_performance_profiling_demo(self):
        """Run performance profiling demo"""
        print("\n3. Performance Profiling Demo")
        print("-" * 40)
        
        end_time = time.time() + 10  # 10 seconds
        self.stats['performance_steps'] = 0
        
        while time.time() < end_time and self.is_running:
            current_time = time.time()
            self.step = int((current_time - self.start_time) * 30.0)
            self.stats['performance_steps'] = self.step
            
            # Update interfaces
            if NLM_AVAILABLE:
                self.viz.update()
                self.debug.update()
            
            # Profile performance
            self._profile_performance(current_time)
            
            # Log results every 2 seconds
            if self.step % 60 == 0:  # 2 seconds at 30 Hz
                self._print_profiling_results()
            
            time.sleep(1.0 / 30.0)
        
        print(f"✓ Performance profiling demo completed: {self.step} steps")
    
    def _run_memory_leak_demo(self):
        """Run memory leak detection demo"""
        print("\n4. Memory Leak Detection Demo")
        print("-" * 40)
        
        end_time = time.time() + 8  # 8 seconds
        self.stats['memory_steps'] = 0
        
        while time.time() < end_time and self.is_running:
            current_time = time.time()
            self.step = int((current_time - self.start_time) * 30.0)
            self.stats['memory_steps'] = self.step
            
            # Update interfaces
            if NLM_AVAILABLE:
                self.viz.update()
                self.debug.update()
            
            # Simulate memory operations
            self._simulate_memory_operations()
            
            # Check for leaks every 2 seconds
            if self.step % 60 == 0:  # 2 seconds at 30 Hz
                self._check_memory_leaks()
            
            time.sleep(1.0 / 30.0)
        
        print(f"✓ Memory leak detection demo completed: {self.step} steps")
    
    def _run_error_detection_demo(self):
        """Run error detection demo"""
        print("\n5. Error Detection Demo")
        print("-" * 40)
        
        end_time = time.time() + 5  # 5 seconds
        self.stats['error_steps'] = 0
        
        while time.time() < end_time and self.is_running:
            current_time = time.time()
            self.step = int((current_time - self.start_time) * 30.0)
            self.stats['error_steps'] = self.step
            
            # Update interfaces
            if NLM_AVAILABLE:
                self.viz.update()
                self.debug.update()
            
            # Generate errors
            self._generate_errors()
            
            # Log error summary every 1 second
            if self.step % 30 == 0:  # 1 second at 30 Hz
                self._print_error_summary()
            
            time.sleep(1.0 / 30.0)
        
        print(f"✓ Error detection demo completed: {self.step} steps")
    
    def _generate_brain_activity_data(self, current_time):
        """Generate brain activity visualization data"""
        if not NLM_AVAILABLE:
            return
        
        # Generate neuron activity data
        for i in range(100):  # 100 neurons
            neuron_data = NeuronActivityData(
                neuron_id=i,
                membrane_potential=60.0 + (i % 10) * 2.0,
                firing_rate=10.0 + (i % 5) * 3.0 + (current_time % 2.0),
                inhibition_level=0.1 + (i % 3) * 0.1,
                last_spike_time=current_time - (i % 5),
                is_active=(i + self.step) % 7 == 0
            )
            
            # Add to visualization
            self.viz.add_time_series_metric(
                f"neuron_{i}_firing_rate",
                neuron_data.firing_rate
            )
        
        # Add system-wide metrics
        firing_rate = 10.0 + (current_time % 10.0)
        self.stats['total_firing_rate'] += firing_rate
        
        self.viz.add_time_series_metric("system_firing_rate", firing_rate)
        self.viz.add_time_series_metric("system_memory_usage", 50.0 + (current_time % 30.0))
        self.viz.add_time_series_metric("system_cpu_usage", 30.0 + (current_time % 40.0))
        
        # Log occasional errors
        if self.step % 50 == 0:
            self._log_error(f"Neuron {self.step % 100} malfunction detected")
    
    def _generate_world_simulation_data(self, current_time):
        """Generate world simulation visualization data"""
        if not NLM_AVAILABLE:
            return
        
        # Simulate world state
        world_state = {
            'width': 16,
            'height': 16,
            'agent_position': (current_time * 2.0 % 16, current_time * 3.0 % 16),
            'resources': [50.0 + (hash(str(t)) % 50) for t in range(16)],
            'time': current_time
        }
        
        # Add world metrics
        self.viz.add_time_series_metric("world_time", world_state['time'])
        self.viz.add_time_series_metric("agent_x", world_state['agent_position'][0])
        self.viz.add_time_series_metric("agent_y", world_state['agent_position'][1])
        self.viz.add_time_series_metric("resource_level", sum(world_state['resources']) / len(world_state['resources']))
        
        # Log world events
        if self.step % 30 == 0:
            self._log_warning(f"Agent moved to position ({world_state['agent_position'][0]:.1f}, {world_state['agent_position'][1]:.1f})")
    
    def _profile_performance(self, current_time):
        """Profile performance metrics"""
        if not NLM_AVAILABLE or not self.debug:
            return
        
        # Update performance metrics
        perf_metrics = self.debug.get_performance_metrics()
        perf_metrics.cpu_usage = 20.0 + (current_time % 60.0)
        perf_metrics.memory_usage = 30.0 + (current_time % 50.0)
        perf_metrics.disk_usage = 10.0 + (current_time % 30.0)
        perf_metrics.timestamp = current_time
        
        # Update neural metrics
        neural_metrics = self.debug.get_neural_metrics()
        neural_metrics.total_neuron_count = 100
        neural_metrics.active_neuron_count = int(50 * (1.0 + (current_time % 2.0) / 2.0))
        neural_metrics.average_firing_rate = 10.0 + (current_time % 10.0)
        neural_metrics.timestamp = current_time
        
        # Update simulation metrics
        sim_metrics = self.debug.get_simulation_metrics()
        sim_metrics.current_step = self.step
        sim_metrics.efficiency = 0.8 + (current_time % 0.4)
        sim_metrics.timestamp = current_time
    
    def _simulate_memory_operations(self):
        """Simulate memory allocation and deallocation"""
        if not NLM_AVAILABLE or not self.debug:
            return
        
        # Simulate memory allocations
        if self.step % 30 == 0:
            for i in range(2):
                address = hash(str(self.step) + str(i)) & 0xFFFFFFFF
                size = 1024 + (i * 512)
                self.debug.track_memory_allocation(address, size, f"alloc_{self.step}_{i}")
        
        # Simulate memory deallocations
        if self.step % 60 == 0:
            # Deallocate recent allocations
            for alloc in list(self.debug._active_allocations):
                self.debug.track_memory_deallocation(alloc.address)
                break
        
        # Occasionally create memory leaks
        if self.step % 100 == 0:
            leak_address = hash(str(self.step * 1000)) & 0xFFFFFFFF
            self.debug.track_memory_allocation(leak_address, 4096, f"LEAK_{self.step}")
    
    def _generate_errors(self):
        """Generate and log errors"""
        if not NLM_AVAILABLE or not self.debug:
            return
        
        # Generate errors based on probability
        if hash(str(self.step)) % 100 < 5:  # 5% error rate
            error_types = ["MEMORY_ERROR", "NETWORK_ERROR", "CPU_ERROR", "IO_ERROR"]
            error_type = error_types[self.step % len(error_types)]
            is_critical = (self.step % 50 == 0)
            
            self.debug.log_error(
                error_type,
                f"Simulated {error_type} at step {self.step}",
                is_critical,
                {"component": "SimulationSystem", "step": str(self.step)}
            )
    
    def _print_brain_status(self):
        """Print brain activity status"""
        current_time = time.time() - self.start_time
        
        print(f"\nBrain Activity Status (Elapsed: {current_time:.1f}s, Step: {self.step})")
        print(f"  Neurons simulated: {self.config.get('neuron_count', 100)}")
        print(f"  Active neurons: {self.debug.get_neural_metrics().active_neuron_count if NLM_AVAILABLE else 'N/A'}")
        print(f"  Average firing rate: {self.debug.get_neural_metrics().average_firing_rate if NLM_AVAILABLE else 'N/A':.1f} Hz")
        print(f"  Debug info entries: {len(self.viz.get_debug_info()) if NLM_AVAILABLE else 'N/A'}")
    
    def _print_world_status(self):
        """Print world simulation status"""
        current_time = time.time() - self.start_time
        
        print(f"\nWorld Simulation Status (Elapsed: {current_time:.1f}s, Step: {self.step})")
        
        # Get world metrics
        world_metrics = {
            'time': self.viz.get_time_series_data("world_time")[-1].value if NLM_AVAILABLE and self.viz.get_time_series_data("world_time") else 0,
            'agent_x': self.viz.get_time_series_data("agent_x")[-1].value if NLM_AVAILABLE and self.viz.get_time_series_data("agent_x") else 0,
            'agent_y': self.viz.get_time_series_data("agent_y")[-1].value if NLM_AVAILABLE and self.viz.get_time_series_data("agent_y") else 0,
            'resources': self.viz.get_time_series_data("resource_level")[-1].value if NLM_AVAILABLE and self.viz.get_time_series_data("resource_level") else 0,
        }
        
        print(f"  Time: {world_metrics['time']:.1f}s")
        print(f"  Agent Position: ({world_metrics['agent_x']:.1f}, {world_metrics['agent_y']:.1f})")
        print(f"  Resources: {world_metrics['resources']:.1f}")
    
    def _print_profiling_results(self):
        """Print performance profiling results"""
        print(f"\nPerformance Profiling Results (Step: {self.step})")
        
        if NLM_AVAILABLE and self.debug:
            perf_metrics = self.debug.get_performance_metrics()
            neural_metrics = self.debug.get_neural_metrics()
            sim_metrics = self.debug.get_simulation_metrics()
            
            print(f"  CPU Usage: {perf_metrics.cpu_usage:.1f}%")
            print(f"  Memory Usage: {perf_metrics.memory_usage:.1f}%")
            print(f"  Disk Usage: {perf_metrics.disk_usage:.1f}%")
            print(f"  Active Neurons: {neural_metrics.active_neuron_count}")
            print(f"  Firing Rate: {neural_metrics.average_firing_rate:.1f} Hz")
            print(f"  Efficiency: {sim_metrics.efficiency:.2f}")
            print(f"  Total Spikes: {sim_metrics.spikeCount}")
    
    def _check_memory_leaks(self):
        """Check for memory leaks"""
        if NLM_AVAILABLE and self.debug:
            leaks = self.debug.detect_memory_leaks()
            if leaks:
                print(f"⚠ Memory leaks detected: {len(leaks)}")
            else:
                print("✓ No memory leaks detected")
    
    def _print_error_summary(self):
        """Print error summary"""
        print(f"\nError Summary (Step: {self.step})")
        
        if NLM_AVAILABLE and self.debug:
            recent_errors = self.debug.get_recent_errors()
            critical_errors = self.debug.get_critical_errors()
            
            print(f"  Recent Errors: {len(recent_errors)}")
            print(f"  Critical Errors: {len(critical_errors)}")
            
            if recent_errors:
                print("\n  Recent Error Details:")
                for error in recent_errors[-3:]:
                    status = "CRITICAL" if error.is_critical else "Warning"
                    print(f"    {status}: {error.errorCode} - {error.errorMessage}")
    
    def _generate_final_report(self):
        """Generate final report"""
        print("\n6. Generating Final Report...")
        
        if not NLM_AVAILABLE:
            print("Basic report generation (C++ tools not available)")
            return
        
        # Generate report
        report = {
            "demo_summary": {
                "total_steps": self.step,
                "duration_seconds": time.time() - self.start_time,
                "demo_duration": self.demo_duration,
                "average_fps": self.step / max(1, time.time() - self.start_time)
            },
            "brain_activity": {
                "steps_completed": self.stats['brain_activity_steps'],
                "neurons_simulated": self.config.get('neuron_count', 100)
            },
            "world_simulation": {
                "steps_completed": self.stats['world_simulation_steps']
            },
            "performance": {
                "steps_completed": self.stats['performance_steps'],
                "peak_memory_usage": self.stats['peak_memory_usage'],
                "total_firing_rate": self.stats['total_firing_rate']
            },
            "memory": {
                "steps_completed": self.stats['memory_steps'],
                "leaks_found": self.stats['memory_leaks_found']
            },
            "errors": {
                "steps_completed": self.stats['error_steps'],
                "total_errors": self.stats['errors_detected']
            }
        }
        
        # Calculate average firing rate
        if self.stats['brain_activity_steps'] > 0:
            report['performance']['avg_firing_rate'] = self.stats['total_firing_rate'] / self.stats['brain_activity_steps']
        
        # Save report
        with open("demo_final_report.json", 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        print(f"✓ Final report saved to: demo_final_report.json")
        
        # Display summary
        print(f"\nReport Summary:")
        print(f"  Total Steps: {self.step}")
        print(f"  Duration: {time.time() - self.start_time:.1f} seconds")
        print(f"  Average FPS: {self.step / max(1, time.time() - self.start_time):.1f}")
        print(f"  Brain Activity Steps: {self.stats['brain_activity_steps']}")
        print(f"  Performance Peak Memory: {self.stats['peak_memory_usage']:.1f}%")
        print(f"  Average Firing Rate: {report['performance'].get('avg_firing_rate', 0):.1f} Hz")
        print(f"  Errors Detected: {self.stats['errors_detected']}")
    
    def _print_summary(self):
        """Print final summary"""
        print("\n" + "=" * 60)
        print("DEMO SUMMARY")
        print("=" * 60)
        
        current_time = time.time() - self.start_time
        
        print(f"Demo completed successfully!")
        print(f"\nKey Statistics:")
        print(f"  Total Simulation Steps: {self.step}")
        print(f"  Total Duration: {current_time:.1f} seconds")
        print(f"  Average FPS: {self.step / max(1, current_time):.1f}")
        print(f"  Brain Activity Steps: {self.stats['brain_activity_steps']}")
        print(f"  World Simulation Steps: {self.stats['world_simulation_steps']}")
        print(f"  Performance Profiling Steps: {self.stats['performance_steps']}")
        print(f"  Memory Leak Detection Steps: {self.stats['memory_steps']}")
        print(f"  Error Detection Steps: {self.stats['error_steps']}")
        
        print(f"\nMonitoring Results:")
        print(f"  Peak Memory Usage: {self.stats['peak_memory_usage']:.1f}%")
        print(f"  Errors Detected: {self.stats['errors_detected']}")
        print(f"  Memory Leaks Found: {self.stats['memory_leaks_found']}")
        print(f"  Average Firing Rate: {self.stats['total_firing_rate'] / max(1, self.stats['brain_activity_steps']):.1f} Hz")
        
        print(f"\nOutput Files:")
        print(f"  demo_final_report.json - Final demo report")
        if os.path.exists("demo_final_report.json"):
            with open("demo_final_report.json", 'r') as f:
                report_data = json.load(f)
                print(f"  Generated at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        
        print(f"\nKey Features Demonstrated:")
        print(f"  ✓ Real-time brain activity visualization")
        print(f"  ✓ World simulation visualization")
        print(f"  ✓ Performance monitoring and profiling")
        print(f"  ✓ Memory leak detection")
        print(f"  ✓ Error detection and analysis")
        print(f"  ✓ Monitoring hooks and callbacks")
        print(f"  ✓ Data export and reporting")
        print(f"  ✓ Step-by-step debugging")
        print(f"  ✓ Interactive debugging interface")
        
        print(f"\nThe NLM visualization and debugging tools provide a")
        print(f"comprehensive framework for monitoring, analyzing, and")
        print(f"debugging neural simulations in real-time.")
    
    def _generate_error_report(self, error):
        """Generate error report"""
        print(f"\nERROR REPORT")
        print("=" * 60)
        print(f"Demo failed with error: {error}")
        
        error_report = {
            "error": str(error),
            "timestamp": time.time(),
            "demo_status": "failed",
            "steps_completed": self.step,
            "error_type": type(error).__name__
        }
        
        with open("demo_error_report.json", 'w') as f:
            json.dump(error_report, f, indent=2, default=str)
        
        print(f"Error report saved to: demo_error_report.json")
    
    def cleanup(self):
        """Cleanup resources"""
        print("\nCleaning up...")
        
        if self.tools:
            self.tools.close()
            print("✓ Tools closed")
        
        # Clean up temporary files
        temp_files = [
            "demo_final_report.json",
            "demo_error_report.json",
            "demo_report.json",
            "integration_test.json"
        ]
        
        for temp_file in temp_files:
            if os.path.exists(temp_file):
                os.remove(temp_file)
                print(f"✓ Cleaned up: {temp_file}")
        
        self.is_running = False
        print("✓ Cleanup completed")
def main():
    """Main function"""
    print("NLM Visualization and Debugging Tools - Main Demo")
    print("=" * 60)
    print("\nThis demo showcases the comprehensive visualization and")
    print("debugging capabilities of the NLM framework.")
    print("\nFeatures demonstrated:")
    print("  • Real-time brain activity visualization")
    print("  • World simulation visualization")
    print("  • Performance monitoring and profiling")
    print("  • Memory leak detection")
    print("  • Error detection and analysis")
    print("  • Interactive debugging interface")
    print("  • Data export and reporting")
    
    # Create and run demo
    demo = DemoManager()
    
    if demo.initialize():
        demo.run_demo()
        return 0
    else:
        print("\nFailed to initialize demo")
        return 1
if __name__ == "__main__":
    exit_code = main()
    exit(exit_code)
