#!/usr/bin/env python3
"""
NLM - Neural Learning Machine Activity Visualization Tool

Real-time neural activity visualization with sophisticated capabilities for researchers.

Features:
- Live neural population activity visualization
- Spike train visualization with multiple channels
- Population firing rate visualization
- Membrane potential visualization
- Synaptic activity visualization
- Real-time animation of neural dynamics
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from typing import Dict, List, Optional, Tuple, Any
import time
import threading
from dataclasses import dataclass
from dataclasses import dataclass
from enum import Enum
import json
from datetime import datetime
import os

@dataclass
class ActivityVisualizationConfig:
    """Configuration for activity visualization."""
    update_rate: float = 30.0  # Hz
    channels: int = 128
    colormap: str = "diverging"
    show_spikes: bool = True
    show_firing_rate: bool = True
    show_membrane_potential: bool = True
    show_synaptic_activity: bool = True
    animation_interval: int = 50
    window_size: Tuple[int, int] = (1200, 800)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 10

class ActivityVisualizationState(Enum):
    """Visualization state enumeration."""
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    RECORDING = "recording"
    EXPORTING = "exporting"

class NLActivityVisualizer:
    """
    Real-time neural activity visualization tool.
    
    Provides sophisticated visualization capabilities for neural activity including
    population activity, spike trains, firing rates, membrane potentials, and
    synaptic activity with real-time animation and export capabilities.
    """
    
    def __init__(self, config: Optional[ActivityVisualizationConfig] = None):
        self.config = config or ActivityVisualizationConfig()
        self.state = ActivityVisualizationState.IDLE
        self.data_buffer: List[Dict[str, Any]] = []
        self.max_buffer_size = 1000
        self.current_time = 0.0
        self.animation_obj = None
        self.fig = None
        self.axes = None
        self.spine_plots = []
        self.firing_rate_plots = []
        self.membrane_plots = []
        self.synaptic_plots = []
        self.recorded_data = []
        
        # Initialize colormaps
        self._initialize_colormaps()
        
        # Setup visualization
        self._setup_visualization()
        
    def _initialize_colormaps(self):
        """Initialize visualization colormaps."""
        # Diverging colormap for membrane potentials
        self.diverging_cmap = LinearSegmentedColormap.from_list(
            "diverging", ["blue", "cyan", "yellow", "red"], N=256
        )
        
        # Sequential colormap for firing rates
        self.sequential_cmap = plt.cm.viridis
        
        # Activity colormap for spike trains
        self.spike_cmap = plt.cm.binary
        
        # Synaptic colormap
        self.synaptic_cmap = plt.cm.plasma
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        try:
            plt.style.use('seaborn-v0_8')
            self.fig, self.axes = plt.subplots(
                4, 1, figsize=self.config.window_size, 
                gridspec_kw={'height_ratios': [1, 1, 1, 1], 'hspace': 0.3}
            )
            
            # Setup subplots
            self.spine_plots = []
            for i, ax in enumerate(self.axes):
                ax.set_facecolor('#f8f9fa')
                ax.grid(True, alpha=0.3)
                ax.set_xlabel('Time (ms)', fontsize=10)
                self.spine_plots.append(ax)
            
            self.spine_plots[0].set_title('Spike Raster Plot', fontsize=12, fontweight='bold')
            self.spine_plots[1].set_title('Population Firing Rate', fontsize=12, fontweight='bold')
            self.spine_plots[2].set_title('Membrane Potential', fontsize=12, fontweight='bold')
            self.spine_plots[3].set_title('Synaptic Activity', fontsize=12, fontweight='bold')
            
            plt.tight_layout()
            
        except Exception as e:
            print(f"Warning: Could not set up visualization style: {e}")
            # Fallback to basic matplotlib setup
            self.fig, self.axes = plt.subplots(4, 1, figsize=self.config.window_size)
            self.spine_plots = list(self.axes)
    
    def start_real_time_viz(self, brain_data: Dict[str, Any]):
        """
        Start real-time visualization of neural activity.
        
        Args:
            brain_data: Dictionary containing neural activity data
        """
        self.state = ActivityVisualizationState.RUNNING
        
        # Clear any existing animation
        if self.animation_obj:
            self.animation_obj.event_source.stop()
        
        # Initialize data buffers
        self._initialize_data_buffers(brain_data)
        
        # Start animation thread
        self.animation_thread = threading.Thread(target=self._animation_loop)
        self.animation_thread.daemon = True
        self.animation_thread.start()
        
        print(f"Starting real-time neural activity visualization...")
        print(f"Update rate: {self.config.update_rate} Hz")
        print(f"Channels: {self.config.channels}")
        print(f"State: {self.state.value}")
    
    def _initialize_data_buffers(self, brain_data: Dict[str, Any]):
        """Initialize data buffers for visualization."""
        self.spike_data = []
        self.firing_rate_data = []
        self.membrane_potential_data = []
        self.synaptic_activity_data = []
        
        # Get initial data from brain simulation
        if 'spikes' in brain_data:
            self.spike_data = brain_data['spikes']
        if 'firing_rates' in brain_data:
            self.firing_rate_data = brain_data['firing_rates']
        if 'membrane_potentials' in brain_data:
            self.membrane_potential_data = brain_data['membrane_potentials']
        if 'synaptic_activities' in brain_data:
            self.synaptic_activity_data = brain_data['synaptic_activities']
    
    def _animation_loop(self):
        """Main animation loop for real-time updates."""
        while self.state == ActivityVisualizationState.RUNNING:
            # Update visualization data
            self._update_data()
            
            # Render plots
            self._render_plots()
            
            # Save if auto-save is enabled
            if self.config.auto_save and len(self.recorded_data) % self.config.save_interval == 0:
                self._save_current_frame()
            
            # Control animation speed
            time.sleep(1.0 / self.config.update_rate)
    
    def _update_data(self):
        """Update visualization data with new neural activity."""
        self.current_time += 1.0 / self.config.update_rate * 1000  # Convert to ms
        
        # Simulate or update with actual data (in real implementation, this would
        # receive data from the neural simulation)
        new_step_data = {
            'time': self.current_time,
            'spikes': np.random.poisson(0.5, self.config.channels),
            'firing_rates': np.random.gamma(2, 1, self.config.channels),
            'membrane_potentials': np.random.normal(0, 1, self.config.channels),
            'synaptic_activities': np.random.exponential(0.1, self.config.channels)
        }
        
        # Add to data buffer
        self.data_buffer.append(new_step_data)
        
        # Maintain buffer size
        if len(self.data_buffer) > self.max_buffer_size:
            self.data_buffer.pop(0)
    
    def _render_plots(self):
        """Render the current visualization plots."""
        if not self.fig:
            return
        
        # Clear axes
        for ax in self.spine_plots:
            ax.clear()
        
        # Plot spike raster if enabled
        if self.config.show_spikes and self.data_buffer:
            self._plot_spike_raster()
        
        # Plot firing rate if enabled
        if self.config.show_firing_rate and self.data_buffer:
            self._plot_firing_rate()
        
        # Plot membrane potential if enabled
        if self.config.show_membrane_potential and self.data_buffer:
            self._plot_membrane_potential()
        
        # Plot synaptic activity if enabled
        if self.config.show_synaptic_activity and self.data_buffer:
            self._plot_synaptic_activity()
        
        # Update titles and labels
        self._update_plot_labels()
        
        # Refresh the figure
        self.fig.canvas.draw()
    
    def _plot_spike_raster(self):
        """Plot spike raster visualization."""
        # Extract spike data from buffer
        spike_times = []
        spike_channels = []
        
        for step_data in self.data_buffer[-100:]:  # Last 100 time steps
            for i, spike_count in enumerate(step_data['spikes']):
                if spike_count > 0:
                    spike_times.extend([step_data['time']] * int(spike_count))
                    spike_channels.extend([i] * int(spike_count))
        
        if spike_times:
            self.spine_plots[0].scatter(
                spike_times, spike_channels, 
                s=1, c='black', marker='|', alpha=0.7, linewidths=0.5
            )
        
        # Set axis limits
        self.spine_plots[0].set_xlim(max(0, self.current_time - 2000), self.current_time)
        self.spine_plots[0].set_ylim(-1, self.config.channels)
    
    def _plot_firing_rate(self):
        """Plot population firing rate visualization."""
        # Calculate average firing rate across channels
        if len(self.data_buffer) < 10:
            return
        
        recent_data = self.data_buffer[-10:]  # Last 10 time steps
        avg_firing_rates = [np.mean([step['firing_rates'] for step in recent_data], axis=0)]
        
        # Convert to time series
        time_series = [step['time'] for step in recent_data]
        firing_rate_series = np.mean(avg_firing_rates, axis=0)
        
        # Plot each channel's firing rate
        for i in range(min(self.config.channels, 10)):  # Limit to first 10 channels for clarity
            self.spine_plots[1].plot(
                time_series, firing_rate_series[:, i], 
                linewidth=1, alpha=0.7, label=f'Neuron {i}'
            )
        
        # Plot overall average
        self.spine_plots[1].plot(
            time_series, np.mean(firing_rate_series, axis=0),
            linewidth=2, color='red', label='Average', fontweight='bold'
        )
        
        self.spine_plots[1].set_ylim(0, max(2, np.max(firing_rate_series) * 1.1))
        self.spine_plots[1].legend(loc='upper right', fontsize=8)
    
    def _plot_membrane_potential(self):
        """Plot membrane potential visualization."""
        if len(self.data_buffer) < 10:
            return
        
        # Get recent membrane potential data
        recent_data = self.data_buffer[-10:]
        membrane_potentials = np.array([step['membrane_potentials'] for step in recent_data])
        
        # Plot membrane potentials for first few channels
        time_series = [step['time'] for step in recent_data]
        
        for i in range(min(self.config.channels, 5)):
            self.spine_plots[2].plot(
                time_series, membrane_potentials[:, i],
                linewidth=1.5, alpha=0.8, label=f'Neuron {i}'
            )
        
        self.spine_plots[2].axhline(y=0, color='gray', linestyle='--', alpha=0.5)
        self.spine_plots[2].set_ylim(-2, 2)
        self.spine_plots[2].legend(loc='upper right', fontsize=8)
    
    def _plot_synaptic_activity(self):
        """Plot synaptic activity visualization."""
        if len(self.data_buffer) < 10:
            return
        
        # Get recent synaptic activity data
        recent_data = self.data_buffer[-10:]
        synaptic_activities = np.array([step['synaptic_activities'] for step in recent_data])
        
        # Create heatmap for synaptic activity
        time_series = [step['time'] for step in recent_data]
        
        # Plot synaptic activity as a heatmap
        im = self.spine_plots[3].imshow(
            synaptic_activities.T, aspect='auto', 
            cmap=self.synaptic_cmap, alpha=0.8,
            extent=[time_series[0], time_series[-1], 0, self.config.channels],
            origin='lower'
        )
        
        self.spine_plots[3].set_xlabel('Time (ms)')
        self.spine_plots[3].set_ylabel('Channel')
        
        # Add colorbar
        plt.colorbar(im, ax=self.spine_plots[3], shrink=0.8)
    
    def _update_plot_labels(self):
        """Update plot labels with current time and statistics."""
        current_time_ms = self.current_time
        
        # Update titles with current statistics
        if self.data_buffer:
            latest_data = self.data_buffer[-1]
            
            # Update firing rate title with statistics
            avg_firing_rate = np.mean(latest_data['firing_rates'])
            self.spine_plots[1].set_title(
                f'Population Firing Rate (Avg: {avg_firing_rate:.2f} Hz)',
                fontsize=12, fontweight='bold'
            )
            
            # Update membrane potential title with statistics
            mean_potential = np.mean(latest_data['membrane_potentials'])
            std_potential = np.std(latest_data['membrane_potentials'])
            self.spine_plots[2].set_title(
                f'Membrane Potential (μV: {mean_potential:.2f} ± {std_potential:.2f})',
                fontsize=12, fontweight='bold'
            )
        
        # Update time in title
        time_str = f'Time: {current_time_ms:.0f} ms'
        self.fig.suptitle(time_str, fontsize=14, fontweight='bold')
    
    def _save_current_frame(self):
        """Save current visualization frame."""
        if self.config.save_path:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{self.config.save_path}/activity_frame_{timestamp}.png"
            
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            try:
                self.fig.savefig(filename, dpi=150, bbox_inches='tight')
                print(f"Saved visualization frame: {filename}")
            except Exception as e:
                print(f"Warning: Could not save frame: {e}")
    
    def pause(self):
        """Pause the visualization."""
        self.state = ActivityVisualizationState.PAUSED
        print("Visualization paused")
    
    def resume(self):
        """Resume the visualization."""
        self.state = ActivityVisualizationState.RUNNING
        print("Visualization resumed")
    
    def stop(self):
        """Stop the visualization."""
        self.state = ActivityVisualizationState.IDLE
        
        # Stop animation thread
        if hasattr(self, 'animation_thread') and self.animation_thread.is_alive():
            self.animation_thread.join(timeout=5.0)
        
        print("Visualization stopped")
    
    def export_data(self, output_path: str, format: str = "json"):
        """
        Export visualization data to file.
        
        Args:
            output_path: Path to output file
            format: Export format (json, csv, npy)
        """
        self.state = ActivityVisualizationState.EXPORTING
        
        try:
            if format.lower() == "json":
                export_data = {
                    'config': self.config.__dict__,
                    'data_buffer': self.data_buffer,
                    'recorded_data': self.recorded_data,
                    'current_time': self.current_time,
                    'timestamp': datetime.now().isoformat()
                }
                
                with open(output_path, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                    
            elif format.lower() == "csv":
                import pandas as pd
                
                # Convert data buffer to DataFrame
                df_data = []
                for step_data in self.data_buffer:
                    df_data.append({
                        'time': step_data['time'],
                        'spike_count': np.sum(step_data['spikes']),
                        'avg_firing_rate': np.mean(step_data['firing_rates']),
                        'mean_membrane_potential': np.mean(step_data['membrane_potentials']),
                        'mean_synaptic_activity': np.mean(step_data['synaptic_activities'])
                    })
                
                df = pd.DataFrame(df_data)
                df.to_csv(output_path, index=False)
                
            elif format.lower() == "npy":
                np.savez(output_path, 
                        data_buffer=np.array(self.data_buffer, dtype=object),
                        current_time=self.current_time,
                        config=self.config.__dict__)
            
            print(f"Data exported to: {output_path}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
        
        finally:
            self.state = ActivityVisualizationState.IDLE

def main():
    """Main entry point for nlx-viz-activity tool."""
    print("NLM - Neural Activity Visualization Tool")
    print("=" * 50)
    print("Real-time neural activity visualization with sophisticated capabilities")
    print()
    
    # Example usage
    config = ActivityVisualizationConfig(
        update_rate=30.0,
        channels=128,
        show_spikes=True,
        show_firing_rate=True,
        show_membrane_potential=True,
        show_synaptic_activity=True,
        animation_interval=50,
        window_size=(1400, 900)
    )
    
    # Create visualizer
    visualizer = NLActivityVisualizer(config)
    
    # Example brain data structure
    brain_data = {
        'spikes': np.random.poisson(0.5, config.channels),
        'firing_rates': np.random.gamma(2, 1, config.channels),
        'membrane_potentials': np.random.normal(0, 1, config.channels),
        'synaptic_activities': np.random.exponential(0.1, config.channels)
    }
    
    print("Example configuration created successfully!")
    print(f"Channels: {config.channels}")
    print(f"Update rate: {config.update_rate} Hz")
    print(f"Window size: {config.window_size}")
    print()
    print("To use the visualizer:")
    print("  visualizer = NLActivityVisualizer(config)")
    print("  visualizer.start_real_time_viz(brain_data)")
    print("  visualizer.pause()  # to pause")
    print("  visualizer.resume() # to resume")
    print("  visualizer.stop()   # to stop")
    print("  visualizer.export_data('output.json', 'json')  # to export")

if __name__ == "__main__":
    main()
