"""
Advanced utilities for NLM (Neural Learning Machine) Python users.

This module provides enhanced functionality, utilities, and advanced features
for experienced NLM users who want more control and capabilities.
"""

import os
import json
import pickle
import hashlib
from pathlib import Path
from typing import Dict, Any, Optional, List, Union
import numpy as np

# Try to import pynlm, but make it optional for utility functions
# that don't require the actual NLM library
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
class NLMExperimentLogger:
    """Advanced experiment logging and analysis for NLM simulations."""

    def __init__(self, experiment_name: str, log_dir: str = "nlm_logs"):
        """Initialize experiment logger.

        Args:
            experiment_name: Name of the experiment
            log_dir: Directory for storing experiment logs
        """
        self.experiment_name = experiment_name
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(exist_ok=True)

        # Create experiment directory
        self.exp_dir = self.log_dir / experiment_name
        self.exp_dir.mkdir(exist_ok=True)

        # Initialize data storage
        self.step_data = []
        self.config = {}
        self.metadata = {
            'experiment_name': experiment_name,
            'start_time': None,
            'parameters': {}
        }

        # Setup log files
        self.step_log_file = self.exp_dir / "steps.jsonl"
        self.config_file = self.exp_dir / "config.json"
        self.metadata_file = self.exp_dir / "metadata.json"
        self.analysis_file = self.exp_dir / "analysis.json"

    def log_step(self, step_data: Dict[str, Any]) -> None:
        """Log step data.

        Args:
            step_data: Dictionary containing step information
        """
        self.step_data.append(step_data)

        # Write to JSONL file immediately for streaming
        with open(self.step_log_file, 'a') as f:
            f.write(json.dumps(step_data) + '\n')

    def set_config(self, config: Dict[str, Any]) -> None:
        """Set experiment configuration.

        Args:
            config: Configuration dictionary
        """
        self.config = config
        with open(self.config_file, 'w') as f:
            json.dump(config, f, indent=2)

    def set_metadata(self, **kwargs) -> None:
        """Set experiment metadata.

        Args:
            **kwargs: Metadata key-value pairs
        """
        self.metadata.update(kwargs)
        with open(self.metadata_file, 'w') as f:
            json.dump(self.metadata, f, indent=2)

    def analyze(self) -> Dict[str, Any]:
        """Analyze experiment data.

        Returns:
            Dictionary containing analysis results
        """
        if not self.step_data:
            return {'error': 'No step data available for analysis'}

        analysis = {
            'total_steps': len(self.step_data),
            'statistics': self._compute_statistics(),
            'patterns': self._detect_patterns(),
            'conclusions': self._draw_conclusions()
        }

        with open(self.analysis_file, 'w') as f:
            json.dump(analysis, f, indent=2)

        return analysis

    def _compute_statistics(self) -> Dict[str, Any]:
        """Compute basic statistics."""
        if not self.step_data:
            return {}

        stats = {}
        numeric_fields = ['firing_neurons', 'total_spikes', 'average_firing_rate',
                         'curiosity_level', 'novelty_level', 'prediction_error']

        for field in numeric_fields:
            values = [step.get(field) for step in self.step_data
                     if step.get(field) is not None]
            if values:
                stats[field] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'median': np.median(values)
                }

        # Add trend information
        if len(self.step_data) > 1:
            stats['trends'] = self._compute_trends()

        return stats

    def _detect_patterns(self) -> Dict[str, Any]:
        """Detect patterns in the data."""
        patterns = {}

        # Simple pattern detection examples
        if len(self.step_data) >= 10:
            # Check for learning patterns (decreasing prediction error)
            errors = [step.get('prediction_error', 0) for step in self.step_data[-10:]]
            if len(errors) >= 5:
                decreasing_count = sum(1 for i in range(1, len(errors))
                                     if errors[i] < errors[i-1])
                if decreasing_count >= 3:
                    patterns['learning_pattern'] = 'decreasing_prediction_error'

            # Check for stability (low variance in recent steps)
            if len(self.step_data) >= 20:
                recent_rates = [step.get('average_firing_rate', 0)
                              for step in self.step_data[-10:]]
                if recent_rates:
                    rate_std = np.std(recent_rates)
                    if rate_std < 0.1:  # Low standard deviation
                        patterns['stability_pattern'] = 'low_firing_variance'

        return patterns

    def _draw_conclusions(self) -> List[str]:
        """Draw conclusions from analysis."""
        conclusions = []

        if not self.step_data:
            return conclusions

        # Basic sanity checks
        if len(self.step_data) < 10:
            conclusions.append("Insufficient data for reliable conclusions")
            return conclusions

        # Check for convergence
        recent_steps = self.step_data[-min(20, len(self.step_data)):]
        if len(recent_steps) >= 5:
            firing_rates = [step.get('average_firing_rate', 0) for step in recent_steps]
            if firing_rates:
                rate_range = max(firing_rates) - min(firing_rates)
                if rate_range < 0.05:  # Very stable firing rate
                    conclusions.append("Brain appears to have reached stable firing patterns")

        # Check for exploration (high curiosity)
        curiosity_levels = [step.get('curiosity_level', 0) for step in self.step_data
                           if step.get('curiosity_level') is not None]
        if curiosity_levels:
            avg_curiosity = np.mean(curiosity_levels)
            if avg_curiosity > 0.5:
                conclusions.append("Brain shows high levels of exploratory behavior")

        return conclusions

    def _compute_trends(self) -> Dict[str, float]:
        """Compute simple trend analysis."""
        if len(self.step_data) < 2:
            return {}

        trends = {}
        numeric_fields = ['firing_neurons', 'total_spikes', 'average_firing_rate']

        for field in numeric_fields:
            values = [step.get(field) for step in self.step_data
                     if step.get(field) is not None]
            if len(values) >= 2:
                # Simple linear trend (slope)
                x = np.arange(len(values))
                y = np.array(values)
                slope = np.polyfit(x, y, 1)[0]
                trends[f'{field}_trend'] = slope

        return trends

    def export_data(self, format: str = 'json') -> str:
        """Export experiment data.

        Args:
            format: Export format ('json', 'csv', 'pickle')

        Returns:
            Path to exported file
        """
        if format == 'json':
            export_file = self.exp_dir / f"{self.experiment_name}_export.json"
            with open(export_file, 'w') as f:
                json.dump(self.step_data, f, indent=2)
        elif format == 'csv':
            import pandas as pd
            export_file = self.exp_dir / f"{self.experiment_name}_export.csv"
            df = pd.DataFrame(self.step_data)
            df.to_csv(export_file, index=False)
        elif format == 'pickle':
            export_file = self.exp_dir / f"{self.experiment_name}_export.pkl"
            with open(export_file, 'wb') as f:
                pickle.dump(self.step_data, f)
        else:
            raise ValueError(f"Unsupported format: {format}")

        return str(export_file)
class NLMBenchmark:
    """Performance benchmarking for NLM simulations."""

    def __init__(self, brain=None, world=None, agent=None):
        """Initialize benchmark.

        Args:
            brain: Optional brain instance
            world: Optional world instance
            agent: Optional agent instance
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.timings = {}
        self.memory_usage = {}

    def time_operation(self, operation_name: str, func, *args, **kwargs):
        """Time an operation.

        Args:
            operation_name: Name of the operation
            func: Function to time
            *args: Function arguments
            **kwargs: Function keyword arguments

        Returns:
            Result of the function
        """
        import time

        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()

        duration = end_time - start_time

        if operation_name not in self.timings:
            self.timings[operation_name] = []
        self.timings[operation_name].append(duration)

        return result

    def benchmark_step(self, steps: int = 100, timestep: float = 0.1):
        """Benchmark simulation step performance.

        Args:
            steps: Number of steps to run
            timestep: Time step size
        """
        if not all([self.brain, self.world, self.agent]):
            print("Error: Need brain, world, and agent for benchmarking")
            return

        print(f"Benchmarking {steps} simulation steps...")

        # Benchmark brain step
        def brain_step_op(step):
            self.brain.step(step)

        self.time_operation('brain_step', brain_step_op, 0)

        # Benchmark agent processing
        percept = self.world.getSensoryPercept()

        def process_input_op(percpt):
            self.agent.processSensoryInput(percpt)

        self.time_operation('process_input', process_input_op, percept)

        # Benchmark motor command decoding
        def decode_command_op():
            return self.agent.decodeMotorCommand()

        self.time_operation('decode_command', decode_command_op)

        # Benchmark world update
        def world_update_op(dt):
            self.world.update(dt)

        self.time_operation('world_update', world_update_op, timestep)

        # Run actual simulation for realistic benchmarking
        self._run_benchmark_simulation(steps, timestep)

    def _run_benchmark_simulation(self, steps: int, timestep: float):
        """Run a benchmark simulation."""
        import time

        start_time = time.time()

        for step in range(steps):
            # Update world
            self.world.update(timestep)

            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)

            # Brain step
            self.brain.step(step)

            # Decode motor command
            self.agent.decodeMotorCommand()

            # Apply to world
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())

            # Apply reward
            reward = self._calculate_reward(percept)
            self.agent.applyRewardModulation(reward, 0.0)

            # Update development
            self.agent.updateDevelopment(timestep)

        end_time = time.time()

        total_time = end_time - start_time
        steps_per_second = steps / total_time

        print(f"\nBenchmark Results:")
        print(f"  Total steps: {steps}")
        print(f"  Total time: {total_time:.2f} seconds")
        print(f"  Steps per second: {steps_per_second:.1f}")
        print(f"  Average time per step: {total_time/steps*1000:.1f} ms")

        # Print individual operation timings
        if self.timings:
            print(f"\nIndividual operation timings:")
            for op_name, times in self.timings.items():
                if times:
                    avg_time = np.mean(times) * 1000
                    print(f"  {op_name}: {avg_time:.1f} ms (avg)")

    def _calculate_reward(self, percept):
        """Calculate reward for benchmarking."""
        return 0.1  # Simple constant reward

    def memory_profile(self):
        """Profile memory usage (if available)."""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()

            self.memory_usage['rss'] = memory_info.rss / 1024 / 1024  # MB
            self.memory_usage['vms'] = memory_info.vms / 1024 / 1024  # MB
            self.memory_usage['shared'] = memory_info.shared / 1024 / 1024  # MB

            print(f"Memory usage: RSS={self.memory_usage['rss']:.1f} MB, "
                  f"VMS={self.memory_usage['vms']:.1f} MB")

        except ImportError:
            print("psutil not available - skipping memory profiling")

    def generate_report(self) -> str:
        """Generate benchmark report.

        Returns:
            Formatted report string
        """
        report_lines = [
            "NLM Benchmark Report",
            "=" * 50,
            f"Brain: {type(self.brain).__name__ if self.brain else 'N/A'}",
            f"World: {type(self.world).__name__ if self.world else 'N/A'}",
            f"Agent: {type(self.agent).__name__ if self.agent else 'N/A'}",
            "",
        ]

        if self.timings:
            report_lines.append("Operation Timings:")
            for op_name, times in self.timings.items():
                if times:
                    avg_time = np.mean(times) * 1000
                    min_time = np.min(times) * 1000
                    max_time = np.max(times) * 1000
                    report_lines.append(f"  {op_name}: avg={avg_time:.1f}ms, "
                                      f"min={min_time:.1f}ms, max={max_time:.1f}ms")

        if self.memory_usage:
            report_lines.append("\nMemory Usage:")
            for mem_type, value in self.memory_usage.items():
                report_lines.append(f"  {mem_type}: {value:.1f} MB")

        return "\n".join(report_lines)
class NLMVisualizer:
    """Visualization utilities for NLM simulations."""

    def __init__(self, output_dir: str = "nlm_visualizations"):
        """Initialize visualizer.

        Args:
            output_dir: Directory for storing visualizations
        """
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)

        # Check for available plotting libraries
        self.plotly_available = False
        self.matplotlib_available = False

        try:
            import matplotlib.pyplot as plt
            self.matplotlib_available = True
            self.plt = plt
        except ImportError:
            pass

        try:
            import plotly.graph_objects as go
            import plotly.io as pio
            self.plotly_available = True
            self.go = go
            self.pio = pio
        except ImportError:
            pass

    def create_training_curves(self, step_data: List[Dict],
                              output_file: str = "training_curves.html"):
        """Create training curves visualization.

        Args:
            step_data: List of step data dictionaries
            output_file: Output file name
        """
        if not self.plotly_available and not self.matplotlib_available:
            print("Warning: No plotting libraries available")
            return None

        # Extract relevant data
        steps = [step.get('step', i) for i, step in enumerate(step_data)]

        # Create figure with subplots
        if self.plotly_available:
            fig = self.go.Figure()

            # Add traces for different metrics
            metrics = ['firing_neurons', 'total_spikes', 'average_firing_rate',
                      'curiosity_level', 'novelty_level', 'prediction_error']

            for metric in metrics:
                values = [step.get(metric, 0) for step in step_data]
                if any(v is not None for v in values):
                    fig.add_trace(self.go.Scatter(
                        x=steps,
                        y=values,
                        mode='lines',
                        name=metric.replace('_', ' ').title(),
                        hovertemplate=f"{metric}: %{{y:.3f}}<extra></extra>"
                    ))

            fig.update_layout(
                title="NLM Simulation Training Curves",
                xaxis_title="Step",
                yaxis_title="Value",
                hovermode='x unified',
                showlegend=True,
                template='plotly_white'
            )

            # Save as HTML
            output_path = self.output_dir / output_file
            self.pio.write_html(fig, str(output_path), auto_open=False)
            print(f"Training curves saved to {output_path}")
            return str(output_path)

        elif self.matplotlib_available:
            # Create matplotlib visualization
            fig, axes = self.plt.subplots(2, 3, figsize=(15, 10))
            axes = axes.flatten()

            metrics = ['firing_neurons', 'total_spikes', 'average_firing_rate',
                      'curiosity_level', 'novelty_level', 'prediction_error']

            for idx, metric in enumerate(metrics):
                ax = axes[idx]
                values = [step.get(metric, 0) for step in step_data]
                ax.plot(steps, values)
                ax.set_title(metric.replace('_', ' ').title())
                ax.set_xlabel('Step')
                ax.set_ylabel('Value')
                ax.grid(True, alpha=0.3)

            # Hide unused subplots
            for idx in range(len(metrics), len(axes)):
                axes[idx].set_visible(False)

            fig.suptitle('NLM Simulation Training Curves', fontsize=16)
            fig.tight_layout()

            output_path = self.output_dir / f"{Path(output_file).stem}.png"
            fig.savefig(str(output_path), dpi=150, bbox_inches='tight')
            self.plt.close(fig)

            print(f"Training curves saved to {output_path}")
            return str(output_path)

        return None

    def create_heatmaps(self, step_data: List[Dict], output_file: str = "heatmaps.html"):
        """Create heatmap visualizations of neural activity patterns.

        Args:
            step_data: Step data with neural activity information
            output_file: Output file name
        """
        if not self.plotly_available:
            print("Warning: Plotly not available for heatmaps")
            return None

        # This is a placeholder for heatmap creation
        # In a real implementation, you would need neural activity matrices
        print("Heatmap visualization requires neural activity matrices - not implemented yet")
        return None

    def create_interactive_dashboard(self, step_data: List[Dict],
                                   output_file: str = "dashboard.html"):
        """Create an interactive dashboard for exploring simulation data.

        Args:
            step_data: Step data for the dashboard
            output_file: Output file name
        """
        if not self.plotly_available:
            print("Warning: Plotly not available for interactive dashboard")
            return None

        # Create interactive time series with zoom and pan capabilities
        fig = self.go.Figure()

        steps = [step.get('step', i) for i, step in enumerate(step_data)]

        # Add main time series
        metrics = ['firing_neurons', 'average_firing_rate', 'curiosity_level']
        for metric in metrics:
            values = [step.get(metric, 0) for step in step_data]
            fig.add_trace(self.go.Scatter(
                x=steps,
                y=values,
                mode='lines',
                name=metric.replace('_', ' ').title(),
                hovertemplate=f"{metric}: %{{y:.3f}}<extra></extra>"
            ))

        # Add range slider for zooming
        fig.update_layout(
            title="Interactive NLM Simulation Dashboard",
            xaxis_title="Step",
            yaxis_title="Value",
            hovermode='x unified',
            showlegend=True,
            xaxis_rangeslider_visible=True,
            template='plotly_white',
            height=600
        )

        output_path = self.output_dir / output_file
        self.pio.write_html(fig, str(output_path), auto_open=False)
        print(f"Interactive dashboard saved to {output_path}")
        return str(output_path)
# Backward compatibility functions
def create_default_config():
    """Create default configuration (backward compatibility)."""
    if PYNLM_AVAILABLE:
        return pynlm.createDefaultConfig()
    else:
        raise ImportError("pynlm is not available")
def create_brain(config=None):
    """Create brain (backward compatibility)."""
    if PYNLM_AVAILABLE:
        if config is None:
            config = create_default_config()
        return pynlm.createBrain(config)
    else:
        raise ImportError("pynlm is not available")
def create_simple_world():
    """Create simple world (backward compatibility)."""
    if PYNLM_AVAILABLE:
        return pynlm.createSimpleWorld()
    else:
        raise ImportError("pynlm is not available")
def create_agent_brain(brain):
    """Create agent brain interface (backward compatibility)."""
    if PYNLM_AVAILABLE:
        return pynlm.createAgentBrain(brain)
    else:
        raise ImportError("pynlm is not available")
# Global utility functions
def get_version():
    """Get NLM version information."""
    return "NLM Python Bindings v0.1.0"

def is_available():
    """Check if pynlm is available."""
    return PYNLM_AVAILABLE
# Export commonly used classes and functions
__all__ = [
    'NLMExperimentLogger',
    'NLMBenchmark',
    'NLMVisualizer',
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    'get_version',
    'is_available'
]
