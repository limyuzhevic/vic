#!/usr/bin/env python3
"""
Example 7: Real-time Monitoring

This example demonstrates real-time monitoring and analysis of NLM
simulations. It focuses on live data collection, visualization, and
automated monitoring of simulation performance.

Key features demonstrated:
- Real-time data collection and monitoring
- Live visualization of simulation metrics
- Automated alerting and threshold monitoring
- Performance tracking and trend analysis
- Alert and notification system
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)
import time
import numpy as np
import matplotlib.pyplot as plt
import threading
import queue
from collections import deque
import json
from datetime import datetime
from typing import Dict, List, Callable, Optional

class RealTimeMonitor:
    """
    Real-time monitoring system for NLM simulations.
    
    This class provides tools for:
    - Live data collection and monitoring
    - Real-time visualization
    - Automated alerting
    - Performance tracking
    - Event-driven analysis
    """
    
    def __init__(self, update_interval=0.1):
        self.update_interval = update_interval
        self.data_queue = queue.Queue()
        self.monitoring_data = deque(maxlen=1000)
        self.alert_rules = []
        self.alert_history = []
        self.visualization_data = deque(maxlen=100)
        self.monitoring_active = False
        self.monitor_thread = None
        self.callbacks = []
    
    def start_monitoring(self, experiment_runner):
        """Start real-time monitoring of an experiment."""
        if self.monitoring_active:
            print("Monitoring is already active")
            return
        
        self.monitoring_active = True
        self.monitor_thread = threading.Thread(target=self._monitoring_loop, args=(experiment_runner,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print(f"✓ Started real-time monitoring (interval: {self.update_interval}s)")
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitoring_active = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        print("✓ Stopped real-time monitoring")
    
    def add_alert_rule(self, condition, message, severity="info"):
        """
        Add an alert rule.
        
        Args:
            condition: Function that takes metrics and returns True if alert should trigger
            message: Alert message
            severity: Alert severity (info, warning, error, critical)
        """
        self.alert_rules.append({
            'condition': condition,
            'message': message,
            'severity': severity,
            'triggered': False,
            'last_triggered': None
        })
    
    def _monitoring_loop(self, experiment_runner):
        """Main monitoring loop."""
        while self.monitoring_active:
            try:
                # Get current metrics
                metrics = self._collect_metrics(experiment_runner)
                
                # Add to monitoring data
                self.monitoring_data.append({
                    'timestamp': time.time(),
                    'step': experiment_runner.current_step,
                    'metrics': metrics
                })
                
                # Check alert rules
                self._check_alert_rules(metrics)
                
                # Update visualization data
                self._update_visualization_data(metrics)
                
                # Call callbacks
                for callback in self.callbacks:
                    try:
                        callback(metrics)
                    except Exception as e:
                        print(f"Error in monitoring callback: {e}")
                
                # Sleep
                time.sleep(self.update_interval)
                
            except Exception as e:
                print(f"Error in monitoring loop: {e}")
                time.sleep(1.0)
    
    def _collect_metrics(self, experiment_runner):
        """Collect current simulation metrics."""
        metrics = {}
        
        # Brain metrics
        if hasattr(experiment_runner, 'brain') and experiment_runner.brain:
            brain = experiment_runner.brain
            metrics['brain'] = {
                'total_neurons': brain.getTotalNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': brain.getDevelopmentalStage(),
                'simulation_time': time.time()
            }
        
        # Agent metrics
        if hasattr(experiment_runner, 'agent') and experiment_runner.agent:
            agent = experiment_runner.agent
            metrics['agent'] = {
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'development_stage': agent.getDevelopmentalStage()
            }
        
        # World metrics
        if hasattr(experiment_runner, 'world') and experiment_runner.world:
            world = experiment_runner.world
            body = world.getAgentBody()
            if body:
                metrics['world'] = {
                    'agent_position': {'x': body.x, 'y': body.y},
                    'agent_energy': body.energy,
                    'agent_age': body.age,
                    'simulation_time': world.getSimulationTime(),
                    'agent_speed': np.hypot(body.velocityX, body.velocityY)
                }
        
        # System metrics
        metrics['system'] = {
            'step': experiment_runner.current_step,
            'state': str(experiment_runner.state),
            'time': time.time()
        }
        
        return metrics
    
    def _check_alert_rules(self, metrics):
        """Check all alert rules against current metrics."""
        for alert_rule in self.alert_rules:
            try:
                if alert_rule['condition'](metrics):
                    # Trigger alert
                    alert = {
                        'rule_name': alert_rule['message'],
                        'message': alert_rule['message'],
                        'severity': alert_rule['severity'],
                        'timestamp': time.time(),
                        'step': metrics.get('system', {}).get('step', 0),
                        'metrics': metrics
                    }
                    
                    self.alert_history.append(alert)
                    alert_rule['triggered'] = True
                    alert_rule['last_triggered'] = time.time()
                    
                    # Print alert
                    severity_icon = {'info': 'ℹ', 'warning': '⚠', 'error': '✗', 'critical': '🚨'}
                    print(f"{severity_icon[alert_rule['severity']]} ALERT: {alert_rule['message']} (step {alert['step']})")
                
            except Exception as e:
                print(f"Error checking alert rule {alert_rule['message']}: {e}")
    
    def _update_visualization_data(self, metrics):
        """Update visualization data."""
        self.visualization_data.append({
            'timestamp': time.time(),
            'metrics': metrics
        })
    
    def get_monitoring_summary(self):
        """Get summary of monitoring data."""
        if not self.monitoring_data:
            return {'error': 'No monitoring data available'}
        
        latest_data = self.monitoring_data[-1]
        
        summary = {
            'current_step': latest_data['step'],
            'current_time': latest_data['timestamp'],
            'metrics_keys': list(latest_data['metrics'].keys()),
            'alert_count': len([a for a in self.alert_history if a['severity'] == 'critical']),
            'warning_count': len([a for a in self.alert_history if a['severity'] == 'warning']),
            'total_alerts': len(self.alert_history),
            'data_points': len(self.monitoring_data)
        }
        
        # Add brain metrics if available
        if 'brain' in latest_data['metrics']:
            brain = latest_data['metrics']['brain']
            summary['brain'] = {
                'neurons': brain['total_neurons'],
                'firing_rate': brain['firing_rate'],
                'total_spikes': brain['total_spikes']
            }
        
        # Add agent metrics if available
        if 'agent' in latest_data['metrics']:
            agent = latest_data['metrics']['agent']
            summary['agent'] = {
                'curiosity': agent['curiosity_level'],
                'novelty': agent['novelty_level'],
                'prediction_error': agent['prediction_error']
            }
        
        return summary
    
    def save_monitoring_data(self, filename):
        """Save monitoring data to file."""
        data_to_save = {
            'monitoring_data': list(self.monitoring_data),
            'alert_history': self.alert_history,
            'alert_rules': self.alert_rules,
            'visualization_data': list(self.visualization_data),
            'summary': self.get_monitoring_summary()
        }
        
        with open(filename, 'w') as f:
            json.dump(data_to_save, f, indent=2, default=str)
        
        print(f"✓ Monitoring data saved to {filename}")
    
    def load_monitoring_data(self, filename):
        """Load monitoring data from file."""
        try:
            with open(filename, 'r') as f:
                data = json.load(f)
            
            self.monitoring_data = deque(data['monitoring_data'], maxlen=1000)
            self.alert_history = data['alert_history']
            self.alert_rules = data['alert_rules']
            self.visualization_data = deque(data['visualization_data'], maxlen=100)
            
            print(f"✓ Monitoring data loaded from {filename}")
            return True
        except Exception as e:
            print(f"Error loading monitoring data: {e}")
            return False

def create_monitoring_alert_rules():
    """Create common monitoring alert rules."""
    alert_rules = []
    
    # Alert 1: Low firing rate
    def low_firing_rate(metrics):
        if 'brain' in metrics:
            firing_rate = metrics['brain']['firing_rate']
            return firing_rate < 1.0 and metrics['brain']['total_neurons'] > 100
        return False
    
    alert_rules.append({
        'condition': low_firing_rate,
        'message': 'Low firing rate detected - neurons may be inactive',
        'severity': 'warning'
    })
    
    # Alert 2: High firing rate
    def high_firing_rate(metrics):
        if 'brain' in metrics:
            firing_rate = metrics['brain']['firing_rate']
            return firing_rate > 20.0 and metrics['brain']['total_neurons'] > 100
        return False
    
    alert_rules.append({
        'condition': high_firing_rate,
        'message': 'High firing rate detected - possible runaway excitation',
        'severity': 'error'
    })
    
    # Alert 3: High prediction error
    def high_prediction_error(metrics):
        if 'agent' in metrics:
            error = metrics['agent']['prediction_error']
            return error > 0.5
        return False
    
    alert_rules.append({
        'condition': high_prediction_error,
        'message': 'High prediction error - model learning instability',
        'severity': 'warning'
    })
    
    # Alert 4: Agent energy depletion
    def low_agent_energy(metrics):
        if 'world' in metrics:
            energy = metrics['world']['agent_energy']
            return energy < 10.0 and energy > 0
        return False
    
    alert_rules.append({
        'condition': low_agent_energy,
        'message': 'Low agent energy - possible exhaustion',
        'severity': 'error'
    })
    
    # Alert 5: Curiosity spike
    def high_curiosity(metrics):
        if 'agent' in metrics:
            curiosity = metrics['agent']['curiosity_level']
            return curiosity > 0.8
        return False
    
    alert_rules.append({
        'condition': high_curiosity,
        'message': 'High curiosity level - increased exploration behavior',
        'severity': 'info'
    })
    
    return alert_rules

class RealTimeVisualizer:
    """
    Real-time visualization system for NLM monitoring.
    
    This class provides tools for:
    - Live plotting of simulation metrics
    - Real-time data visualization
    - Interactive plotting capabilities
    - Multi-panel displays
    """
    
    def __init__(self, figsize=(15, 10)):
        self.figsize = figsize
        self.fig = None
        self.axes = None
        self.plots = {}
        self.data_history = {}
        self.update_interval = 1.0
        self.plots_enabled = False
    
    def setup_plots(self, metric_categories):
        """
        Setup real-time plotting.
        
        Args:
            metric_categories: Dictionary of metric categories to plot
        """
        try:
            plt.ion()  # Turn on interactive mode
            
            self.fig, self.axes = plt.subplots(2, 3, figsize=self.figsize)
            self.fig.suptitle('NLM Real-Time Monitoring', fontsize=16)
            
            # Flatten axes for easy access
            self.axes = self.axes.flatten()
            
            # Initialize data history for each category
            for category, metrics in metric_categories.items():
                self.data_history[category] = {
                    'timestamps': [],
                    'data': {metric: [] for metric in metrics},
                    'colors': plt.cm.Set1(np.linspace(0, 1, len(metrics)))
                }
            
            self.plots_enabled = True
            print("✓ Real-time visualization setup complete")
            
        except Exception as e:
            print(f"Error setting up plots: {e}")
            self.plots_enabled = False
    
    def update_plots(self, metrics, timestamp):
        """Update real-time plots with new data."""
        if not self.plots_enabled:
            return
        
        try:
            # Update data history
            for category, category_metrics in metrics.items():
                if category in self.data_history:
                    history = self.data_history[category]
                    history['timestamps'].append(timestamp)
                    
                    for metric_name, value in category_metrics.items():
                        if metric_name in history['data']:
                            history['data'][metric_name].append(value)
            
            # Limit data history
            for category in self.data_history:
                history = self.data_history[category]
                max_points = 100
                if len(history['timestamps']) > max_points:
                    history['timestamps'] = history['timestamps'][-max_points:]
                    for metric in history['data']:
                        history['data'][metric] = history['data'][metric][-max_points:]
            
            # Redraw plots
            self._redraw_plots()
            
        except Exception as e:
            print(f"Error updating plots: {e}")
    
    def _redraw_plots(self):
        """Redraw all plots."""
        if not self.plots_enabled:
            return
        
        self.fig.clear()
        self.fig.suptitle('NLM Real-Time Monitoring - Updated: ' + 
                         time.strftime('%H:%M:%S'), fontsize=14)
        
        ax_idx = 0
        
        for category, history in self.data_history.items():
            if ax_idx >= len(self.axes):
                break
            
            ax = self.axes[ax_idx]
            
            # Plot each metric in this category
            for metric_name, data in history['data'].items():
                if len(data) > 1:
                    ax.plot(history['timestamps'], data, 
                           label=metric_name, color='blue', alpha=0.7, linewidth=1)
            
            ax.set_title(category.replace('_', ' ').title())
            ax.set_xlabel('Time')
            ax.set_ylabel('Value')
            ax.legend(loc='upper right', fontsize=8)
            ax.grid(True, alpha=0.3)
            
            # Format y-axis
            ax.tick_params(axis='both', which='major', labelsize=8)
            
            ax_idx += 1
        
        # Hide unused axes
        for i in range(ax_idx, len(self.axes)):
            self.axes[i].set_visible(False)
        
        # Adjust layout
        plt.tight_layout()
        
        # Update display
        plt.pause(0.01)
    
    def save_plots(self, filename):
        """Save current plots to file."""
        if self.fig:
            self.fig.savefig(filename, dpi=150, bbox_inches='tight')
            print(f"✓ Plots saved to {filename}")
    
    def close_plots(self):
        """Close all plots."""
        if self.fig:
            plt.close(self.fig)
            self.fig = None
            self.axes = None
            self.plots_enabled = False
        print("✓ Plots closed")

def create_realtime_monitoring_example():
    """Demonstrate real-time monitoring setup."""
    print("=== Example 7: Real-Time Monitoring ===\n")
    
    # Create experiment
    print("1. Setting up experiment with real-time monitoring...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create experiment runner
    from advanced_nlm_tools import NLMExperimentRunner, ExperimentConfig
    
    runner = NLMExperimentRunner(brain, world, agent)
    
    # Create real-time monitor
    print("\n2. Setting up real-time monitoring...")
    monitor = RealTimeMonitor(update_interval=0.5)
    
    # Add alert rules
    alert_rules = create_monitoring_alert_rules()
    for rule in alert_rules:
        monitor.add_alert_rule(rule['condition'], rule['message'], rule['severity'])
    
    # Add monitoring callbacks
    def metrics_callback(metrics):
        """Callback to process metrics."""
        print(f"  [Callback] Step {metrics['system']['step']}: "
              f"Firing rate = {metrics.get('brain', {}).get('firing_rate', 0):.2f} Hz, "
              f"Curiosity = {metrics.get('agent', {}).get('curiosity_level', 0):.2f}")
    
    monitor.callbacks.append(metrics_callback)
    
    # Create experiment configuration
    experiment_config = ExperimentConfig(
        name="Real-Time Monitoring Experiment",
        max_steps=300,
        timestep=0.1,
        save_checkpoints=False,
        verbose=False  # Set to False to reduce console output
    )
    
    # Start monitoring
    print(f"\n3. Starting real-time monitoring...")
    monitor.start_monitoring(runner)
    
    # Setup visualization (optional)
    print(f"\n4. Setting up visualization...")
    visualizer = RealTimeVisualizer()
    
    metric_categories = {
        'brain': ['firing_rate', 'total_spikes', 'firing_neuron_count'],
        'agent': ['curiosity_level', 'novelty_level', 'prediction_error'],
        'world': ['agent_energy']
    }
    
    visualizer.setup_plots(metric_categories)
    
    # Add monitor callback for visualization
    def visualization_callback(metrics):
        visualizer.update_plots(metrics, time.time())
    
    monitor.callbacks.append(visualization_callback)
    
    # Run experiment
    print(f"\n5. Running experiment with monitoring...")
    print(f"   Experiment: {experiment_config.name}")
    print(f"   Duration: {experiment_config.max_steps} steps")
    print(f"   Monitoring interval: {monitor.update_interval}s")
    print(f"   Alert rules: {len(monitor.alert_rules)}")
    
    start_time = time.time()
    result = runner.run_experiment(experiment_config)
    end_time = time.time()
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Close visualization
    visualizer.close_plots()
    
    # Get monitoring summary
    print(f"\n=== Monitoring Summary ===")
    summary = monitor.get_monitoring_summary()
    
    print(f"Experiment completed in {end_time - start_time:.2f}s")
    print(f"Steps completed: {result.steps_completed}")
    print(f"Alert count: {summary['alert_count']} critical, {summary['warning_count']} warning")
    print(f"Total alerts: {summary['total_alerts']}")
    print(f"Data points collected: {summary['data_points']}")
    
    if 'brain' in summary:
        print(f"\nBrain metrics:")
        print(f"  Neurons: {summary['brain']['neurons']}")
        print(f"  Firing rate: {summary['brain']['firing_rate']:.3f} Hz")
        print(f"  Total spikes: {summary['brain']['total_spikes']}")
    
    if 'agent' in summary:
        print(f"\nAgent metrics:")
        print(f"  Curiosity: {summary['agent']['curiosity']:.3f}")
        print(f"  Novelty: {summary['agent']['novelty']:.3f}")
        print(f"  Prediction error: {summary['agent']['prediction_error']:.3f}")
    
    # Save monitoring data
    monitor_filename = f"monitoring_data_{int(time.time())}.json"
    monitor.save_monitoring_data(monitor_filename)
    
    return result, monitor, visualizer

def example_monitoring_visualization():
    """Demonstrate monitoring visualization."""
    print("\n=== Monitoring Visualization Example ===\n")
    
    # Create mock monitoring data
    print("1. Creating mock monitoring data...")
    
    import random
    
    # Generate synthetic monitoring data
    timestamps = [time.time() - (100 - i) * 0.1 for i in range(100)]
    
    monitoring_data = {
        'brain': {
            'firing_rate': [random.uniform(1, 15) for _ in range(100)],
            'total_spikes': [random.randint(50, 500) for _ in range(100)],
            'firing_neuron_count': [random.randint(10, 100) for _ in range(100)]
        },
        'agent': {
            'curiosity_level': [random.uniform(0.1, 0.9) for _ in range(100)],
            'novelty_level': [random.uniform(0.1, 0.8) for _ in range(100)],
            'prediction_error': [random.uniform(0, 1) for _ in range(100)]
        },
        'world': {
            'agent_energy': [random.uniform(20, 100) for _ in range(100)]
        }
    }
    
    # Create visualization
    print("2. Creating visualization...")
    visualizer = RealTimeVisualizer()
    
    metric_categories = {
        'brain': ['firing_rate', 'total_spikes'],
        'agent': ['curiosity_level', 'novelty_level'],
        'world': ['agent_energy']
    }
    
    visualizer.setup_plots(metric_categories)
    
    # Simulate data updates
    for i, timestamp in enumerate(timestamps):
        # Update visualization
        visualizer.update_plots(monitoring_data, timestamp)
        
        # Add alerts periodically
        if i % 20 == 0:
            # Simulate an alert
            alert = {
                'rule_name': 'Test Alert',
                'message': f'Test alert at step {i}',
                'severity': 'info',
                'timestamp': timestamp,
                'step': i,
                'metrics': monitoring_data
            }
        
        time.sleep(0.05)
    
    # Save plots
    visualizer.save_plots("monitoring_visualization.png")
    
    # Close visualization
    visualizer.close_plots()
    
    print("✓ Visualization completed and saved")
    
    return monitoring_data

if __name__ == "__main__":
    print("NLM Advanced Features - Example 7: Real-Time Monitoring")
    print("=" * 65)
    
    try:
        # Run real-time monitoring example
        result, monitor, visualizer = example_realtime_monitoring_example()
        
        # Run visualization example
        visualization_data = example_monitoring_visualization()
        
        print("\n" + "=" * 65)
        print("Example 7 completed successfully!")
        print("Key insights:")
        print("  - Real-time monitoring enables live system observation")
        print("  - Automated alerting detects anomalies and issues")
        print("  - Visualization provides intuitive data presentation")
        print("  - Callback system enables custom data processing")
        print("  - Monitoring data can be saved for later analysis")
        
    except Exception as e:
        print(f"\nError in example 7: {e}")
        import traceback
        traceback.print_exc()
