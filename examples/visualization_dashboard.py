import pynlm
import numpy as np
import time
import threading
import json
from datetime import datetime
from typing import Dict, List, Optional
from collections import defaultdict

class VisualizationDashboard:
    """
    Interactive visualization dashboard for NLM brain monitoring and analysis.
    Displays real-time neural activity, memory states, and system metrics.
    """
    
    def __init__(self, brain: pynlm.Brain, update_interval: float = 0.5):
        """
        Initialize visualization dashboard.
        
        Args:
            brain: The brain instance to visualize
            update_interval: Dashboard update interval in seconds
        """
        self.brain = brain
        self.update_interval = update_interval
        self.running = False
        self.visualizer = pynlm.createVisualizer(brain)
        
        # Performance metrics
        self.metrics_history = {
            "step": [],
            "time": [],
            "firing_rate": [],
            "memory_usage": [],
            "prediction_error": [],
            "neuromodulation": {"dopamine": [], "curiosity": [], "novelty": []},
            "region_activity": defaultdict(lambda: []),
            "synaptic_strength": [],
            "energy_level": [],
            "health_level": []
        }
        
        # Visualization components
        self.network_viz = None
        self.performance_viz = None
        self.memory_viz = None
        self.dashboard_viz = None
        
        # System monitors
        self.system_monitors = {
            "neural": True,
            "memory": True,
            "prediction": True,
            "neuromodulation": True,
            "development": True,
            "performance": True
        }
    
    def start_monitoring(self, duration: Optional[float] = None):
        """
        Start real-time monitoring and visualization.
        
        Args:
            duration: Monitoring duration in seconds (None for continuous)
        """
        self.running = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitoring_thread.start()
        
        print(f"Visualization dashboard started monitoring...")
        print(f"Update interval: {self.update_interval} seconds")
        print(f"Monitored systems: {', '.join(k for k, v in self.system_monitors.items() if v)}")
        
        # Wait for completion or keyboard interrupt
        try:
            if duration:
                time.sleep(duration)
                self.stop_monitoring()
            else:
                # Wait for user interrupt
                while self.running:
                    time.sleep(1)
        except KeyboardInterrupt:
            self.stop_monitoring()
    
    def _monitoring_loop(self):
        """Main monitoring loop for data collection and visualization updates."""
        start_time = time.time()
        step_count = 0
        
        while self.running:
            current_time = time.time()
            step_count += 1
            
            # Collect metrics
            self._collect_metrics(step_count, current_time)
            
            # Update visualizations
            self._update_visualizations()
            
            # Save metrics periodically
            if step_count % 100 == 0:
                self._save_metrics()
            
            # Sleep for update interval
            time.sleep(self.update_interval)
        
        end_time = time.time()
        print(f"Monitoring completed. Total time: {end_time - start_time:.2f} seconds")
    
    def _collect_metrics(self, step: int, timestamp: float):
        """Collect current metrics from brain systems."""
        self.metrics_history["step"].append(step)
        self.metrics_history["time"].append(timestamp)
        
        # Neural metrics
        self.metrics_history["firing_rate"].append(self.brain.getAverageFiringRate())
        
        # Memory metrics
        if self.brain.getWorkingMemory():
            self.metrics_history["memory_usage"].append(
                self.brain.getWorkingMemory().getActiveMemoryCount()
            )
        
        # Prediction metrics
        if self.brain.getPredictionSystem():
            self.metrics_history["prediction_error"].append(
                self.brain.getPredictionSystem().getPredictionError()
            )
        
        # Neuromodulation metrics
        if self.brain.getDopamine():
            self.metrics_history["neuromodulation"]["dopamine"].append(
                self.brain.getDopamine().getCurrentLevel()
            )
        if self.brain.getCuriosity():
            self.metrics_history["neuromodulation"]["curiosity"].append(
                self.brain.getCuriosity().getCurrentLevel()
            )
        if self.brain.getNovelty():
            self.metrics_history["neuromodulation"]["novelty"].append(
                self.brain.getNovelty().getCurrentLevel()
            )
        
        # Regional activity
        for region in self.brain.getRegions():
            activity = region.getAverageFiringRate()
            self.metrics_history["region_activity"][region.getName()].append(activity)
        
        # Synaptic metrics
        total_synapses = self.brain.getTotalSynapseCount()
        if total_synapses > 0:
            avg_strength = self.brain.getAverageSynapticWeight()
            self.metrics_history["synaptic_strength"].append(avg_strength)
        
        # World state metrics (if agent is available)
        # This would require access to the agent and world
    
    def _update_visualizations(self):
        """Update all visualization components."""
        if not self.visualizer:
            return
        
        # Network visualization
        self.network_viz = self.visualizer.createNetworkView()
        self.network_viz.updateLayout()
        
        # Performance dashboard
        self.performance_viz = self.visualizer.createPerformanceDashboard()
        self.performance_viz.updateMetrics(self.metrics_history)
        
        # Memory visualization
        self.memory_viz = self.visualizer.createMemoryViewer()
        self.memory_viz.updateMemoryState()
        
        # Interactive dashboard
        self.dashboard_viz = self.visualizer.createInteractiveDashboard()
        self.dashboard_viz.refresh()
    
    def _save_metrics(self):
        """Save collected metrics to file."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"dashboard_metrics_{timestamp}.json"
        
        with open(filename, 'w') as f:
            json.dump(self.metrics_history, f, indent=2)
        
        print(f"Metrics saved to {filename}")
    
    def generate_report(self, output_file: str):
        """Generate comprehensive analysis report."""
        report = {
            "metadata": {
                "generated_at": datetime.now().isoformat(),
                "total_samples": len(self.metrics_history["step"]),
                "brain_configuration": "advanced_visualization_config",
                "monitoring_duration": len(self.metrics_history["time"]) * self.update_interval
            },
            "neural_dynamics": self._analyze_neural_dynamics(),
            "memory_performance": self._analyze_memory_performance(),
            "prediction_accuracy": self._analyze_prediction_performance(),
            "neuromodulation_dynamics": self._analyze_neuromodulation_dynamics(),
            "regional_dynamics": self._analyze_regional_dynamics(),
            "system_health": self._analyze_system_health(),
            "performance_metrics": self._analyze_performance_metrics()
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Comprehensive report generated: {output_file}")
        return report
    
    def _analyze_neural_dynamics(self) -> Dict:
        """Analyze neural dynamics from metrics."""
        if not self.metrics_history["firing_rate"]:
            return {}
        
        firing_rates = self.metrics_history["firing_rate"]
        return {
            "avg_firing_rate": float(np.mean(firing_rates)),
            "max_firing_rate": float(np.max(firing_rates)),
            "min_firing_rate": float(np.min(firing_rates)),
            "firing_rate_variance": float(np.var(firing_rates)),
            "stability": 1.0 / (1.0 + np.std(firing_rates)),
            "trend": "increasing" if np.mean(firing_rates[-10:]) > np.mean(firing_rates[:10]) else "decreasing" if np.mean(firing_rates[-10:]) < np.mean(firing_rates[:10]) else "stable"
        }
    
    def _analyze_memory_performance(self) -> Dict:
        """Analyze memory system performance."""
        if not self.metrics_history["memory_usage"]:
            return {}
        
        memory_usage = self.metrics_history["memory_usage"]
        return {
            "avg_memory_usage": float(np.mean(memory_usage)),
            "max_memory_usage": float(np.max(memory_usage)),
            "memory_efficiency": float(np.mean(memory_usage) / max(np.max(memory_usage), 1)),
            "utilization_trend": "increasing" if np.mean(memory_usage[-10:]) > np.mean(memory_usage[:10]) else "stable"
        }
    
    def _analyze_prediction_performance(self) -> Dict:
        """Analyze prediction system performance."""
        if not self.metrics_history["prediction_error"]:
            return {}
        
        prediction_errors = self.metrics_history["prediction_error"]
        return {
            "avg_prediction_error": float(np.mean(prediction_errors)),
            "prediction_stability": 1.0 / (1.0 + np.std(prediction_errors)),
            "improvement_rate": float(np.mean([prediction_errors[i-1] - prediction_errors[i] for i in range(1, len(prediction_errors))])),
            "error_trend": "decreasing" if np.mean(prediction_errors[-10:]) < np.mean(prediction_errors[:10]) else "increasing" if np.mean(prediction_errors[-10:]) > np.mean(prediction_errors[:10]) else "stable"
        }
    
    def _analyze_neuromodulation_dynamics(self) -> Dict:
        """Analyze neuromodulation system dynamics."""
        dynamics = {}
        
        for neuromodulator, levels in self.metrics_history["neuromodulation"].items():
            if levels:
                dynamics[neuromodulator] = {
                    "avg_level": float(np.mean(levels)),
                    "std_level": float(np.std(levels)),
                    "stability": 1.0 / (1.0 + np.std(levels)),
                    "activation_percentage": (sum(1 for level in levels if level > 0.5) / len(levels)) * 100
                }
        
        return dynamics
    
    def _analyze_regional_dynamics(self) -> Dict:
        """Analyze regional activity dynamics."""
        dynamics = {}
        
        for region_name, activities in self.metrics_history["region_activity"].items():
            if activities:
                dynamics[region_name] = {
                    "mean_activity": float(np.mean(activities)),
                    "max_activity": float(np.max(activities)),
                    "min_activity": float(np.min(activities)),
                    "activity_variance": float(np.var(activities)),
                    "stability": 1.0 / (1.0 + np.std(activities)),
                    "dominant_region": region_name == max(self.metrics_history["region_activity"].items(), key=lambda x: np.mean(x[1]) if x[1] else 0)[0]
                }
        
        return dynamics
    
    def _analyze_system_health(self) -> Dict:
        """Analyze overall system health."""
        health_metrics = {
            "neural_health": 0.0,
            "memory_health": 0.0,
            "prediction_health": 0.0,
            "neuromodulation_health": 0.0,
            "overall_health": 0.0
        }
        
        # Neural health based on firing rate stability
        if self.metrics_history["firing_rate"]:
            firing_stability = health_metrics["neural_health"] = 1.0 / (1.0 + np.std(self.metrics_history["firing_rate"]))
        
        # Memory health based on efficiency
        if self.metrics_history["memory_usage"]:
            memory_efficiency = np.mean(self.metrics_history["memory_usage"]) / max(np.max(self.metrics_history["memory_usage"]), 1)
            health_metrics["memory_health"] = memory_efficiency
        
        # Prediction health based on error reduction
        if self.metrics_history["prediction_error"]:
            prediction_stability = 1.0 / (1.0 + np.std(self.metrics_history["prediction_error"]))
            health_metrics["prediction_health"] = prediction_stability
        
        # Neuromodulation health based on stability
        neuromod_stability = 1.0
        for neuromodulator, levels in self.metrics_history["neuromodulation"].items():
            if levels:
                neuromod_stability *= (1.0 / (1.0 + np.std(levels)))
        
        if self.metrics_history["neuromodulation"]["dopamine"] or self.metrics_history["neuromodulation"]["curiosity"] or self.metrics_history["neuromodulation"]["novelty"]:
            health_metrics["neuromodulation_health"] = neuromod_stability
        
        # Overall health (weighted average)
        health_metrics["overall_health"] = (
            health_metrics["neural_health"] * 0.3 +
            health_metrics["memory_health"] * 0.25 +
            health_metrics["prediction_health"] * 0.25 +
            health_metrics["neuromodulation_health"] * 0.2
        )
        
        return health_metrics
    
    def _analyze_performance_metrics(self) -> Dict:
        """Analyze performance metrics."""
        return {
            "monitoring_efficiency": len(self.metrics_history["step"]) / max(time.time() - self.metrics_history["time"][0] if self.metrics_history["time"] else 1, 1),
            "data_collection_rate": len(self.metrics_history["step"]) / max(self.update_interval * len(self.metrics_history["time"]), 1),
            "system_uptime": time.time() - (self.metrics_history["time"][0] if self.metrics_history["time"] else time.time()),
            "active_monitoring_systems": sum(1 for v in self.system_monitors.values() if v)
        }
    
    def set_system_monitor(self, system: str, enabled: bool):
        """Enable or disable system monitoring."""
        if system in self.system_monitors:
            self.system_monitors[system] = enabled
        else:
            raise ValueError(f"Unknown monitoring system: {system}")
    
    def get_system_status(self) -> Dict:
        """Get current system status."""
        return {
            "running": self.running,
            "update_interval": self.update_interval,
            "monitored_systems": self.system_monitors,
            "data_points_collected": len(self.metrics_history["step"]),
            "last_update": datetime.now().isoformat(),
            "health_score": self._analyze_system_health()["overall_health"]
        }
    
    def stop_monitoring(self):
        """Stop the monitoring loop."""
        self.running = False
        if hasattr(self, 'monitoring_thread'):
            self.monitoring_thread.join(timeout=5.0)
        print("Monitoring stopped.")

class InteractiveBrainExperiment:
    """
    Interactive brain experiment with real-time visualization and control.
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.dashboard = VisualizationDashboard(brain)
        self.experiment_running = False
    
    def configure_monitoring(self, systems: Dict[str, bool]):
        """
        Configure which systems to monitor.
        
        Args:
            systems: Dictionary of system names and their monitoring status
        """
        for system, enabled in systems.items():
            self.dashboard.set_system_monitor(system, enabled)
    
    def run_experimental_session(self, duration: float, config_name: str = "research"):
        """
        Run experimental session with real-time visualization.
        
        Args:
            duration: Session duration in seconds
            config_name: Configuration name ("research", "production", "exploration")
        """
        print("Starting Interactive Brain Experiment...")
        print(f"Experiment configuration: {config_name}")
        print(f"Duration: {duration} seconds")
        print(f"Monitoring systems: {self.dashboard.get_system_status()['monitored_systems']}")
        print("Press Ctrl+C to stop early")
        
        try:
            self.dashboard.start_monitoring(duration)
            
            # Generate final report
            report_file = f"experiment_report_{config_name}_{int(time.time())}.json"
            report = self.dashboard.generate_report(report_file)
            
            print(f"\n=== Experiment Results ===")
            print(f"Experiment completed successfully!")
            print(f"Report saved to: {report_file}")
            
            # Display key metrics
            print(f"\nKey Performance Metrics:")
            print(f"  Overall Health Score: {report['system_health']['overall_health']:.3f}")
            print(f"  Neural Stability: {report['neural_dynamics']['stability']:.3f}")
            print(f"  Memory Efficiency: {report['memory_performance']['memory_efficiency']:.2%}")
            print(f"  Prediction Accuracy: {report['prediction_accuracy']['prediction_stability']:.2%}")
            
            # Health status
            health_score = report['system_health']['overall_health']
            if health_score > 0.8:
                status = "Excellent"
            elif health_score > 0.6:
                status = "Good"
            elif health_score > 0.4:
                status = "Fair"
            else:
                status = "Poor"
            
            print(f"  System Health: {status}")
            
            return report
            
        except KeyboardInterrupt:
            print("\nExperiment interrupted by user")
            self.dashboard.stop_monitoring()
        except Exception as e:
            print(f"Error during experiment: {e}")
            self.dashboard.stop_monitoring()

if __name__ == "__main__":
    print("Starting Visualization Dashboard and Interactive Analysis Example...\n")
    
    # Create sophisticated brain configuration
    config = pynlm.createDefaultConfig()
    
    # Configuration for visualization experiment
    config.set("visualization.enabled", True)
    config.set("visualization.update_interval", 0.5)
    config.set("visualization.real_time", True)
    config.set("visualization.performance_monitoring", True)
    config.set("visualization.memory_visualization", True)
    
    config.set("brain.region_count", 8)
    config.set("brain.neurons_per_region", 300)
    config.set("brain.initial_excitatory_ratio", 0.75)
    
    config.set("memory.working_memory_size", 200)
    config.set("memory.episodic_capacity", 3000)
    config.set("memory.associative_pattern_size", 80)
    
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.dopamine_gain", 1.8)
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.curiosity_threshold", 0.3)
    config.set("neuromodulation.enable_prediction_error", True)
    
    config.set("development.enabled", True)
    config.set("development.maturation_rate", 0.001)
    
    config.set("performance.enable_profiling", True)
    config.set("performance.max_memory_usage", 1024)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Configure visualization systems
    systems_to_monitor = {
        "neural": True,
        "memory": True,
        "prediction": True,
        "neuromodulation": True,
        "development": True,
        "performance": True
    }
    
    # Create interactive experiment
    experiment = InteractiveBrainExperiment(brain)
    experiment.configure_monitoring(systems_to_monitor)
    
    # Run visualization experiment
    experiment.run_experimental_session(duration=120, config_name="visualization_research")
    
    print(f"\n=== Example Complete ===")
    print(f"Visualization dashboard example completed successfully!")
    print(f"Example demonstrates real-time brain monitoring, interactive visualization,")
    print(f"and comprehensive performance analysis using NLM's visualization systems.")
