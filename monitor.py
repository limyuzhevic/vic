#!/usr/bin/env python3
"""
Performance Monitoring and Debugging Tools for NLM

This tool provides real-time performance monitoring, debugging capabilities,
and system health checks for NLM simulations.
"""

import time
import psutil
import nlm
import threading
import sys
from datetime import datetime
from typing import Dict, List, Optional

class PerformanceMonitor:
    """Real-time performance monitoring for NLM simulations"""
    
    def __init__(self):
        self.monitoring = False
        self.stats_history: List[Dict] = []
        self.monitor_thread: Optional[threading.Thread] = None
        self.lock = threading.Lock()
    
    def start_monitoring(self, interval: float = 1.0):
        """Start real-time performance monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop,
            args=(interval,),
            daemon=True
        )
        self.monitor_thread.start()
        print(f"Performance monitoring started (interval: {interval}s)")
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        print("Performance monitoring stopped")
    
    def _monitor_loop(self, interval: float):
        """Background monitoring loop"""
        while self.monitoring:
            stats = self._collect_stats()
            with self.lock:
                self.stats_history.append(stats)
            time.sleep(interval)
    
    def _collect_stats(self) -> Dict:
        """Collect current system statistics"""
        process = psutil.Process()
        
        return {
            "timestamp": datetime.now().isoformat(),
            "cpu_percent": process.cpu_percent(),
            "memory_percent": process.memory_percent(),
            "memory_rss": process.memory_info().rss / 1024 / 1024,  # MB
            "memory_vms": process.memory_info().vms / 1024 / 1024,  # MB
            "num_threads": process.num_threads(),
            "create_time": datetime.fromtimestamp(process.create_time()).isoformat(),
            "open_files": len(process.open_files()),
        }
    
    def get_current_stats(self) -> Dict:
        """Get current performance statistics"""
        return self._collect_stats()
    
    def get_history(self) -> List[Dict]:
        """Get monitoring history"""
        with self.lock:
            return self.stats_history.copy()
    
    def get_summary(self) -> Dict:
        """Get performance summary"""
        if not self.stats_history:
            return {}
        
        cpu_values = [s["cpu_percent"] for s in self.stats_history]
        memory_values = [s["memory_percent"] for s in self.stats_history]
        
        return {
            "average_cpu": sum(cpu_values) / len(cpu_values),
            "max_cpu": max(cpu_values),
            "average_memory": sum(memory_values) / len(memory_values),
            "max_memory": max(memory_values),
            "data_points": len(self.stats_history),
            "duration": (datetime.fromisoformat(
                self.stats_history[-1]["timestamp"]
            ) - datetime.fromisoformat(self.stats_history[0]["timestamp"])).total_seconds()
        }
class SimulationProfiler:
    """Profile simulation performance and identify bottlenecks"""
    
    def __init__(self):
        self.profiling_data = []
        self.start_time = None
    
    def start_profile(self):
        """Start profiling a simulation"""
        self.start_time = time.time()
        self.profiling_data = []
        print("Simulation profiling started")
    
    def step(self, brain: nlm.Brain, step_num: int):
        """Record profiling data for a simulation step"""
        if not self.start_time:
            return
        
        step_start = time.time()
        
        # Run the step
        brain.run_steps(1)
        
        step_end = time.time()
        step_duration = step_end - step_start
        
        self.profiling_data.append({
            "step": step_num,
            "duration": step_duration,
            "timestamp": datetime.now().isoformat(),
            "brain_stats": brain.get_stats()
        })
    
    def stop_profile(self):
        """Stop profiling and return results"""
        if not self.start_time:
            return None
        
        total_duration = time.time() - self.start_time
        
        # Calculate statistics
        durations = [d["duration"] for d in self.profiling_data]
        
        profile_result = {
            "total_duration": total_duration,
            "average_step_time": sum(durations) / len(durations) if durations else 0,
            "min_step_time": min(durations) if durations else 0,
            "max_step_time": max(durations) if durations else 0,
            "steps_per_second": len(durations) / total_duration if total_duration > 0 else 0,
            "total_steps": len(self.profiling_data),
            "step_data": self.profiling_data
        }
        
        print(f"Profiling completed: {profile_result['total_steps']} steps")
        print(f"Average step time: {profile_result['average_step_time']:.4f}s")
        print(f"Steps per second: {profile_result['steps_per_second']:.1f}")
        
        return profile_result
    
    def identify_bottlenecks(self, profile_result: Dict) -> List[str]:
        """Identify potential performance bottlenecks"""
        bottlenecks = []
        
        if not profile_result:
            return bottlenecks
        
        # Check for step time increases
        step_times = [d["duration"] for d in profile_result["step_data"]]
        if len(step_times) > 10:
            recent_avg = sum(step_times[-10:]) / 10
            early_avg = sum(step_times[:10]) / 10
            
            if recent_avg > early_avg * 1.5:
                bottlenecks.append("Step execution time is increasing - potential memory leak or growing computational complexity")
        
        # Check for high CPU usage
        if profile_result["average_step_time"] > 0.1:  # 100ms per step
            bottlenecks.append("Steps are taking too long - consider reducing network size or optimizing code")
        
        # Check for memory growth
        memory_values = [d["brain_stats"]["memory_usage"] for d in profile_result["step_data"]]
        if len(memory_values) > 5:
            if memory_values[-1] > memory_values[0] * 2:
                bottlenecks.append("Memory usage is growing significantly - potential memory leak")
        
        return bottlenecks
class DebugLogger:
    """Enhanced debug logging for NLM simulations"""
    
    def __init__(self, log_file: str = "nlm_debug.log"):
        self.log_file = log_file
        self.log_level = "INFO"
        self.debug_enabled = False
    
    def enable_debug(self):
        """Enable debug logging"""
        self.debug_enabled = True
        print("Debug logging enabled")
    
    def log(self, message: str, level: str = "INFO"):
        """Log a message"""
        if not self.debug_enabled and level == "DEBUG":
            return
        
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log_entry = f"[{timestamp}] [{level}] {message}"
        
        print(log_entry)
        
        with open(self.log_file, "a") as f:
            f.write(log_entry + "\n")
    
    def debug_brain_state(self, brain: nlm.Brain, step: int):
        """Log detailed brain state for debugging"""
        self.log(f"=== Brain State at Step {step} ===", "DEBUG")
        
        stats = brain.get_stats()
        for key, value in stats.items():
            self.log(f"  {key}: {value}", "DEBUG")
        
        working_mem = brain.get_working_memory()
        if working_mem.get("available", False):
            self.log(f"  Working memory active traces: {working_mem.get('active_traces', 0)}", "DEBUG")
        
        episodic_mem = brain.get_episodic_memory()
        if episodic_mem.get("available", False):
            self.log(f"  Episodic memory episodes: {episodic_mem.get('episode_count', 0)}", "DEBUG")
        
        neuromod = brain.get_neuromodulation()
        if neuromod:
            self.log("  Neuromodulation levels:", "DEBUG")
            for key, value in neuromod.items():
                self.log(f"    {key}: {value}", "DEBUG")
class SystemHealthChecker:
    """Check system health and resources"""
    
    @staticmethod
    def check_system_health() -> Dict:
        """Perform comprehensive system health check"""
        process = psutil.Process()
        
        health_report = {
            "timestamp": datetime.now().isoformat(),
            "status": "HEALTHY",
            "issues": [],
            "recommendations": []
        }
        
        # Check memory usage
        memory_percent = process.memory_percent()
        if memory_percent > 80:
            health_report["status"] = "WARNING"
            health_report["issues"].append(f"High memory usage: {memory_percent:.1f}%")
            health_report["recommendations"].append("Consider reducing simulation size or adding memory monitoring")
        
        # Check CPU usage
        cpu_percent = process.cpu_percent()
        if cpu_percent > 90:
            health_report["status"] = "WARNING"
            health_report["issues"].append(f"High CPU usage: {cpu_percent:.1f}%")
            health_report["recommendations"].append("Consider reducing simulation complexity or optimizing code")
        
        # Check thread count
        thread_count = process.num_threads()
        if thread_count > 100:
            health_report["issues"].append(f"High thread count: {thread_count}")
            health_report["recommendations"].append("Consider reducing parallelism or optimizing threading")
        
        # Check for zombie processes
        try:
            children = process.children(recursive=True)
            zombie_children = [child for child in children if child.status() == psutil.STATUS_ZOMBIE]
            if zombie_children:
                health_report["status"] = "ERROR"
                health_report["issues"].append(f"Found {len(zombie_children)} zombie child processes")
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass
        
        # Check file handles
        open_files = len(process.open_files())
        if open_files > 1000:
            health_report["issues"].append(f"High number of open files: {open_files}")
            health_report["recommendations"].append("Close file handles properly in your code")
        
        return health_report
    
    @staticmethod
    def print_health_report(health_report: Dict):
        """Print formatted health report"""
        status_emoji = {"HEALTHY": "✅", "WARNING": "⚠️", "ERROR": "❌"}
        
        print("\n" + "="*50)
        print("SYSTEM HEALTH REPORT")
        print("="*50)
        print(f"Status: {status_emoji.get(health_report['status'], '?')} {health_report['status']}")
        print(f"Timestamp: {health_report['timestamp']}")
        print()
        
        if health_report['issues']:
            print("ISSUES:")
            for issue in health_report['issues']:
                print(f"  ❌ {issue}")
        else:
            print("✅ No issues detected")
        
        if health_report['recommendations']:
            print("\nRECOMMENDATIONS:")
            for rec in health_report['recommendations']:
                print(f"  💡 {rec}")
        
        print("="*50)
class PerformanceBenchmark:
    """Comprehensive performance benchmarking"""
    
    def __init__(self):
        self.benchmark_results = []
    
    def benchmark_configuration(self, config_name: str, brain_factory, steps: int) -> Dict:
        """Benchmark a specific configuration"""
        print(f"Benchmarking {config_name} ({steps} steps)...")
        
        # Create brain using provided factory
        brain = brain_factory()
        
        # Warm-up
        brain.run_steps(min(100, steps // 10))
        
        # Actual benchmark
        start_time = time.time()
        result = brain.run_steps(steps)
        end_time = time.time()
        
        duration = end_time - start_time
        
        benchmark_result = {
            "configuration": config_name,
            "steps": steps,
            "duration": duration,
            "steps_per_second": steps / duration,
            "average_step_time": duration / steps,
            "total_reward": result['total_reward'],
            "average_firing_rate": result['average_firing_rate'],
            "active_steps": result['active_steps'],
            "total_spikes": result['total_spikes'],
            "timestamp": datetime.now().isoformat()
        }
        
        self.benchmark_results.append(benchmark_result)
        
        print(f"  Duration: {duration:.2f}s")
        print(f"  Speed: {benchmark_result['steps_per_second']:.1f} steps/sec")
        print(f"  Average step time: {benchmark_result['average_step_time']*1000:.2f}ms")
        
        return benchmark_result
    
    def compare_configurations(self, configurations: List[tuple]) -> Dict:
        """Compare multiple configurations"""
        print("Comparing configurations...")
        
        results = []
        for config_name, brain_factory, steps in configurations:
            result = self.benchmark_configuration(config_name, brain_factory, steps)
            results.append(result)
        
        # Create comparison report
        comparison = {
            "timestamp": datetime.now().isoformat(),
            "configurations_tested": len(configurations),
            "results": results,
            "summary": self._generate_comparison_summary(results)
        }
        
        return comparison
    
    def _generate_comparison_summary(self, results: List[Dict]) -> Dict:
        """Generate summary of comparison results"""
        if not results:
            return {}
        
        # Sort by speed
        sorted_by_speed = sorted(results, key=lambda x: x['steps_per_second'], reverse=True)
        
        # Sort by reward
        sorted_by_reward = sorted(results, key=lambda x: x['total_reward'], reverse=True)
        
        return {
            "fastest": sorted_by_speed[0]['configuration'],
            "slowest": sorted_by_speed[-1]['configuration'],
            "best_reward": sorted_by_reward[0]['configuration'],
            "worst_reward": sorted_by_reward[-1]['configuration'],
            "average_speed": sum(r['steps_per_second'] for r in results) / len(results),
            "average_efficiency": sum(r['total_reward'] / r['duration'] for r in results) / len(results)
        }
    
    def print_benchmark_report(self, comparison: Dict):
        """Print formatted benchmark report"""
        print("\n" + "="*60)
        print("PERFORMANCE BENCHMARK REPORT")
        print("="*60)
        
        summary = comparison['summary']
        print(f"Configurations tested: {comparison['configurations_tested']}")
        print(f"Timestamp: {comparison['timestamp']}")
        print()
        
        print("SPEED RANKING:")
        for i, result in enumerate(comparison['results']):
            rank_emoji = ["🥇", "🥈", "🥉"][i] if i < 3 else "  "
            print(f"  {rank_emoji} {result['configuration']:15s}: {result['steps_per_second']:8.1f} steps/sec")
        
        print(f"\nEFFICIENCY RANKING (reward per second):")
        reward_sorted = sorted(comparison['results'], key=lambda x: x['total_reward'] / x['duration'], reverse=True)
        for i, result in enumerate(reward_sorted):
            rank_emoji = ["🥇", "🥈", "🥉"][i] if i < 3 else "  "
            efficiency = result['total_reward'] / result['duration']
            print(f"  {rank_emoji} {result['configuration']:15s}: {efficiency:8.3f} reward/sec")
        
        print(f"\nSUMMARY:")
        print(f"  Fastest:     {summary['fastest']}")
        print(f"  Most efficient: {summary['best_reward']}")
        print(f"  Average speed:  {summary['average_speed']:.1f} steps/sec")
        print(f"  Average efficiency: {summary['average_efficiency']:.3f} reward/sec")
        
        print("="*60)
def main():
    """Main function for the performance monitoring tool"""
    print("NLM Performance Monitoring and Debugging Tools")
    print("="*50)
    
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python3 monitor.py health          # Check system health")
        print("  python3 monitor.py benchmark      # Run performance benchmarks")
        print("  python3 monitor.py profile        # Profile a simulation")
        print("  python3 monitor.py monitor        # Real-time monitoring")
        return
    
    mode = sys.argv[1].lower()
    
    try:
        if mode == "health":
            # System health check
            checker = SystemHealthChecker()
            report = checker.check_system_health()
            checker.print_health_report(report)
            
        elif mode == "benchmark":
            # Performance benchmarks
            benchmark = PerformanceBenchmark()
            
            # Example configurations to test
            configurations = [
                ("Small", lambda: nlm.Brain(), 1000),
                ("Medium", lambda: nlm.Brain(), 5000),
                ("Large", lambda: nlm.Brain(), 10000),
            ]
            
            comparison = benchmark.compare_configurations(configurations)
            benchmark.print_benchmark_report(comparison)
            
        elif mode == "profile":
            # Simulation profiling
            profiler = SimulationProfiler()
            
            # Create brain and profile
            brain = nlm.Brain()
            profiler.start_profile()
            
            # Run simulation with profiling
            for step in range(100):
                profiler.step(brain, step)
                if step % 10 == 0:
                    print(f"Progress: {step}/100 steps")
            
            # Stop profiling
            profile_result = profiler.stop_profile()
            if profile_result:
                bottlenecks = profiler.identify_bottlenecks(profile_result)
                if bottlenecks:
                    print("\nPERFORMANCE BOTTLENECKS:")
                    for bottleneck in bottlenecks:
                        print(f"  ⚠️  {bottleneck}")
                else:
                    print("\n✅ No significant performance bottlenecks detected")
            
        elif mode == "monitor":
            # Real-time monitoring
            monitor = PerformanceMonitor()
            debug = DebugLogger()
            
            debug.enable_debug()
            
            print("Starting real-time monitoring...")
            print("Press Ctrl+C to stop monitoring")
            
            try:
                monitor.start_monitoring(interval=2.0)
                
                # Example simulation
                brain = nlm.Brain()
                
                for step in range(50):
                    result = brain.run_steps(1)
                    debug.log(f"Step {step}: reward={result['total_reward']:.3f}", "INFO")
                    time.sleep(0.5)
                
            except KeyboardInterrupt:
                print("\nMonitoring stopped by user")
            
            finally:
                monitor.stop_monitoring()
                
                # Print summary
                summary = monitor.get_summary()
                if summary:
                    print(f"\nMONITORING SUMMARY:")
                    print(f"  Data points collected: {summary['data_points']}")
                    print(f"  Duration: {summary['duration']:.1f}s")
                    print(f"  Average CPU: {summary['average_cpu']:.1f}%")
                    print(f"  Average memory: {summary['average_memory']:.1f}%")
        
        else:
            print(f"Unknown mode: {mode}")
            print("Available modes: health, benchmark, profile, monitor")
            
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
if __name__ == "__main__":
    main()