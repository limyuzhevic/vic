# Performance Monitoring and Profiling
import time
import threading
import psutil
import numpy as np
from typing import Dict, List, Optional, Callable, Any
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt

class PerformanceMonitor:
    """Monitor and profile NLM brain performance."""
    
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = []
        self.monitor_thread = None
        self.stop_monitoring = False
        
    def start_monitoring(self, interval: float = 0.1):
        """Start continuous performance monitoring."""
        self.stop_monitoring = False
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop, 
            args=(interval,),
            daemon=True
        )
        self.monitor_thread.start()
        
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.stop_monitoring = True
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            
    def _monitor_loop(self, interval: float):
        """Internal monitoring loop."""
        while not self.stop_monitoring:
            data_point = {
                'time': time.time(),
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neutrons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'cpu_usage': psutil.cpu_percent(),
                'memory_usage': psutil.virtual_memory().percent
            }
            self.performance_data.append(data_point)
            time.sleep(interval)
            
    def get_performance_stats(self) -> Dict[str, float]:
        """Get summary statistics of performance data."""
        if not self.performance_data:
            return {}
            
        firing_rates = [d['average_firing_rate'] for d in self.performance_data]
        spike_counts = [d['spike_count'] for d in self.performance_data]
        
        return {
            'time_elapsed': self.performance_data[-1]['time'] - self.performance_data[0]['time'],
            'avg_firing_rate': np.mean(firing_rates),
            'max_firing_rate': np.max(firing_rates),
            'avg_spike_rate': np.mean(spike_counts),
            'max_spike_rate': np.max(spike_counts),
            'peak_cpu': np.max([d['cpu_usage'] for d in self.performance_data]),
            'peak_memory': np.max([d['memory_usage'] for d in self.performance_data]),
            'data_points': len(self.performance_data)
        }
        
    def plot_performance(self, metrics: List[str] = None):
        """Plot performance metrics."""
        if not metrics:
            metrics = ['average_firing_rate', 'spike_count', 'cpu_usage']
            
        time_data = [d['time'] for d in self.performance_data]
        
        fig, axes = plt.subplots(len(metrics), 1, figsize=(12, 4 * len(metrics)))
        if len(metrics) == 1:
            axes = [axes]
            
        for i, metric in enumerate(metrics):
            data = [d[metric] for d in self.performance_data]
            axes[i].plot(time_data, data, linewidth=1)
            axes[i].set_ylabel(metric.replace('_', ' ').title())
            axes[i].grid(True, alpha=0.3)
            
        axes[-1].set_xlabel('Time (s)')
        fig.suptitle('NLM Performance Monitoring')
        plt.tight_layout()
        plt.show()
        
    def save_performance_data(self, filepath: str):
        """Save performance data to file."""
        import pandas as pd
        df = pd.DataFrame(self.performance_data)
        df.to_csv(filepath, index=False)
        print(f"Performance data saved to {filepath}")


class BatchProcessor:
    """Process multiple brain simulations in batch."""
    
    def __init__(self, config: Optional[Dict] = None):
        self.config = config or {}
        self.results = []
        
    def run_batch(self, num_simulations: int, steps_per_simulation: int) -> List[Dict]:
        """Run multiple simulations in batch."""
        print(f"Running batch of {num_simulations} simulations, {steps_per_simulation} steps each")
        
        for i in range(num_simulations):
            print(f"  Simulation {i+1}/{num_simulations}")
            
            # Create new brain for each simulation
            brain_config = pynlm.createDefaultConfig()
            for key, value in self.config.items():
                if '.' in key:
                    # Handle nested keys
                    parts = key.split('.')
                    obj = brain_config
                    for part in parts[:-1]:
                        obj = getattr(obj, part) if hasattr(obj, part) else obj
                    setattr(obj, parts[-1], value)
                else:
                    brain_config.set(key, value)
                    
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            # Run simulation
            simulation_start = time.time()
            for step in range(steps_per_simulation):
                # Use default world update if available
                brain.step(step, step * 0.001)
                
            simulation_time = time.time() - simulation_start
            
            # Collect results
            result = {
                'simulation_id': i,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'simulation_time': simulation_time,
                'steps_per_second': steps_per_simulation / simulation_time if simulation_time > 0 else 0,
                'config': self.config
            }
            self.results.append(result)
            
        return self.results
        
    def get_batch_summary(self) -> Dict:
        """Get summary of batch results."""
        if not self.results:
            return {}
            
        firing_rates = [r['final_firing_rate'] for r in self.results]
        spike_counts = [r['total_spikes'] for r in self.results]
        times = [r['simulation_time'] for r in self.results]
        speeds = [r['steps_per_second'] for r in self.results]
        
        return {
            'num_simulations': len(self.results),
            'avg_firing_rate': np.mean(firing_rates),
            'std_firing_rate': np.std(firing_rates),
            'min_firing_rate': np.min(firing_rates),
            'max_firing_rate': np.max(firing_rates),
            'avg_spike_count': np.mean(spike_counts),
            'total_spikes': np.sum(spike_counts),
            'avg_simulation_time': np.mean(times),
            'total_time': np.sum(times),
            'avg_steps_per_second': np.mean(speeds),
            'config': self.config
        }
        
    def save_results(self, filepath: str):
        """Save batch results to file."""
        import pandas as pd
        df = pd.DataFrame(self.results)
        df.to_csv(filepath, index=False)
        print(f"Batch results saved to {filepath}")


def benchmark_configuration(config1: Dict, config2: Dict, steps: int = 1000) -> Dict:
    """Compare performance between two configurations."""
    print(f"Benchmarking configuration 1 vs configuration 2 for {steps} steps")
    
    # Run first configuration
    results1 = BatchProcessor(config1)
    run1_start = time.time()
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    for key, value in config1.items():
        if '.' in key:
            parts = key.split('.')
            obj = brain1.getConfig()
            for part in parts[:-1]:
                if hasattr(obj, part):
                    obj = getattr(obj, part)
            if hasattr(obj, parts[-1]):
                setattr(obj, parts[-1], value)
        else:
            brain1.getConfig().set(key, value)
    brain1.initialize()
    for step in range(steps):
        brain1.step(step, step * 0.001)
    time1 = time.time() - run1_start
    
    # Run second configuration
    results2 = BatchProcessor(config2)
    run2_start = time.time()
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    for key, value in config2.items():
        if '.' in key:
            parts = key.split('.')
            obj = brain2.getConfig()
            for part in parts[:-1]:
                if hasattr(obj, part):
                    obj = getattr(obj, part)
            if hasattr(obj, parts[-1]):
                setattr(obj, parts[-1], value)
        else:
            brain2.getConfig().set(key, value)
    brain2.initialize()
    for step in range(steps):
        brain2.step(step, step * 0.001)
    time2 = time.time() - run2_start
    
    return {
        'config1': {
            'final_firing_rate': brain1.getAverageFiringRate(),
            'total_spikes': brain1.getTotalSpikeCount(),
            'time_elapsed': time1,
            'steps_per_second': steps / time1 if time1 > 0 else 0
        },
        'config2': {
            'final_firing_rate': brain2.getAverageFiringRate(),
            'total_spikes': brain2.getTotalSpikeCount(),
            'time_elapsed': time2,
            'steps_per_second': steps / time2 if time2 > 0 else 0
        },
        'speedup': time1 / time2 if time2 > 0 else 0
    }
