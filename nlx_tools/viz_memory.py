#!/usr/bin/env python3
"""
NLM - Memory System Visualization Tool

Memory system activity visualization with sophisticated capabilities for researchers.

Features:
- Working memory visualization
- Episodic memory visualization
- Associative memory visualization
- Memory encoding/decoding visualization
- Memory consolidation visualization
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from typing import Dict, List, Optional, Tuple, Any
import time
import threading
from dataclasses import dataclass
from enum import Enum
import json
from datetime import datetime
import os

@dataclass
class MemoryVisualizationConfig:
    """Configuration for memory visualization."""
    update_rate: float = 20.0  # Hz
    memory_types: List[str] = None  # working, episodic, associative
    show_encoding: bool = True
    show_decoding: bool = True
    show_consolidation: bool = True
    max_memory_capacity: int = 1000
    heatmap_resolution: Tuple[int, int] = (800, 600)
    colormap: str = "viridis"
    animation_interval: int = 100
    window_size: Tuple[int, int] = (1200, 800)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 5

class MemoryVisualizationState(Enum):
    """Memory visualization state enumeration."""
    IDLE = "idle"
    RECORDING = "recording"
    ENCODING = "encoding"
    DECODING = "decoding"
    CONSOLIDATING = "consolidating"
    VISUALIZING = "visualizing"
    EXPORTING = "exporting"

class NLMemoryVisualizer:
    """
    Memory system activity visualization tool.
    
    Provides sophisticated memory visualization capabilities including working memory,
    episodic memory, associative memory, encoding/decoding processes, and
    consolidation visualization with comprehensive tracking and analysis.
    """
    
    def __init__(self, config: Optional[MemoryVisualizationConfig] = None):
        self.config = config or MemoryVisualizationConfig()
        if self.config.memory_types is None:
            self.config.memory_types = ["working", "episodic", "associative"]
        
        self.state = MemoryVisualizationState.IDLE
        self.data_history: List[Dict[str, Any]] = []
        self.memory_traces: Dict[str, List[Dict[str, Any]]] = {}
        self.current_time = 0.0
        self.animation_obj = None
        self.fig = None
        self.axes = None
        self.memory_plots = []
        self.encoding_plots = []
        self.decoding_plots = []
        self.consolidation_plots = []
        
        # Initialize memory buffers
        for mem_type in self.config.memory_types:
            self.memory_traces[mem_type] = []
            self._initialize_memory_buffers(mem_type)
        
        # Setup visualization
        self._setup_visualization()
    
    def _initialize_memory_buffers(self, mem_type: str):
        """Initialize memory buffers for each memory type."""
        if mem_type == "working":
            self.memory_traces[mem_type] = {
                'items': [],
                'activation_levels': [],
                'decay_rates': [],
                'capacity_utilization': 0.0
            }
        elif mem_type == "episodic":
            self.memory_traces[mem_type] = {
                'episodes': [],
                'timestamp': [],
                'encoding_strength': [],
                'retrieval_strength': [],
                'memory_indices': []
            }
        elif mem_type == "associative":
            self.memory_traces[mem_type] = {
                'associations': [],
                'connection_strengths': [],
                'pattern_completement_scores': [],
                'interference_levels': []
            }
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        plt.style.use('seaborn-v0_8')
        self.fig, self.axes = plt.subplots(
            4, 1, figsize=self.config.window_size, 
            gridspec_kw={'height_ratios': [1, 1, 1, 1], 'hspace': 0.4}
        )
        
        # Setup subplots
        self.memory_plots = []
        self.encoding_plots = []
        self.decoding_plots = []
        self.consolidation_plots = []
        
        for i, ax in enumerate(self.axes):
            ax.set_facecolor('#f8f9fa')
            ax.grid(True, alpha=0.3)
            ax.set_xlabel('Time (ms)', fontsize=10)
            
        # Assign specific purposes to each subplot
        self.memory_plots[0] = self.axes[0]
        self.memory_plots[1] = self.axes[1]
        self.memory_plots[2] = self.axes[2]
        self.memory_plots[3] = self.axes[3]
        
        self.memory_plots[0].set_title('Memory System Activity', fontsize=12, fontweight='bold')
        self.memory_plots[1].set_title('Encoding Process', fontsize=12, fontweight='bold')
        self.memory_plots[2].set_title('Decoding Process', fontsize=12, fontweight='bold')
        self.memory_plots[3].set_title('Consolidation Process', fontsize=12, fontweight='bold')
        
        plt.tight_layout()
    
    def start_memory_viz(self, memory_data: Dict[str, Any]):
        """
        Start memory visualization of neural activity.
        
        Args:
            memory_data: Dictionary containing memory system data
        """
        self.state = MemoryVisualizationState.VISUALIZING
        
        # Clear any existing animation
        if self.animation_obj:
            self.animation_obj.event_source.stop()
        
        # Initialize memory data
        self._initialize_memory_data(memory_data)
        
        # Start animation thread
        self.animation_thread = threading.Thread(target=self._animation_loop)
        self.animation_thread.daemon = True
        self.animation_thread.start()
        
        print(f"Starting memory system visualization...")
        print(f"Memory types: {self.config.memory_types}")
        print(f"Update rate: {self.config.update_rate} Hz")
        print(f"State: {self.state.value}")
    
    def _initialize_memory_data(self, memory_data: Dict[str, Any]):
        """Initialize memory data from simulation."""
        # Working memory
        if 'working_memory' in memory_data:
            wm_data = memory_data['working_memory']
            self.memory_traces["working"]["items"] = wm_data.get('items', [])
            self.memory_traces["working"]["activation_levels"] = wm_data.get('activation_levels', [])
            self.memory_traces["working"]["decay_rates"] = wm_data.get('decay_rates', [])
            self.memory_traces["working"]["capacity_utilization"] = wm_data.get('capacity_utilization', 0.0)
        
        # Episodic memory
        if 'episodic_memory' in memory_data:
            em_data = memory_data['episodic_memory']
            self.memory_traces["episodic"]["episodes"] = em_data.get('episodes', [])
            self.memory_traces["episodic"]["timestamp"] = em_data.get('timestamp', [])
            self.memory_traces["episodic"]["encoding_strength"] = em_data.get('encoding_strength', [])
            self.memory_traces["episodic"]["retrieval_strength"] = em_data.get('retrieval_strength', [])
            self.memory_traces["episodic"]["memory_indices"] = em_data.get('memory_indices', [])
        
        # Associative memory
        if 'associative_memory' in memory_data:
            am_data = memory_data['associative_memory']
            self.memory_traces["associative"]["associations"] = am_data.get('associations', [])
            self.memory_traces["associative"]["connection_strengths"] = am_data.get('connection_strengths', [])
            self.memory_traces["associative"]["pattern_completement_scores"] = am_data.get('pattern_completion_scores', [])
            self.memory_traces["associative"]["interference_levels"] = am_data.get('interference_levels', [])
    
    def _animation_loop(self):
        """Main animation loop for memory visualization."""
        while self.state == MemoryVisualizationState.VISUALIZING:
            # Update memory data
            self._update_memory_data()
            
            # Render plots
            self._render_plots()
            
            # Save if auto-save is enabled
            if self.config.auto_save and len(self.data_history) % self.config.save_interval == 0:
                self._save_current_frame()
            
            # Control animation speed
            time.sleep(1.0 / self.config.update_rate)
    
    def _update_memory_data(self):
        """Update memory visualization data."""
        self.current_time += 1.0 / self.config.update_rate * 1000  # Convert to ms
        
        # Simulate memory processes
        new_step_data = {
            'time': self.current_time,
            'working_memory': self._simulate_working_memory(),
            'episodic_memory': self._simulate_episodic_memory(),
            'associative_memory': self._simulate_associative_memory()
        }
        
        self.data_history.append(new_step_data)
        
        # Maintain data history size
        if len(self.data_history) > 500:
            self.data_history.pop(0)
    
    def _simulate_working_memory(self):
        """Simulate working memory dynamics."""
        # Simulate working memory items, activation, decay
        n_items = np.random.randint(0, 20)
        items = [f"item_{i}" for i in range(n_items)]
        activation_levels = np.random.beta(2, 5, n_items) * 100
        decay_rates = np.random.uniform(0.9, 0.99, n_items)
        capacity_utilization = min(n_items / self.config.max_memory_capacity, 1.0)
        
        return {
            'items': items,
            'activation_levels': activation_levels.tolist(),
            'decay_rates': decay_rates.tolist(),
            'capacity_utilization': capacity_utilization
        }
    
    def _simulate_episodic_memory(self):
        """Simulate episodic memory dynamics."""
        # Simulate episodic memory encoding/decoding
        n_episodes = np.random.randint(0, 5)
        episodes = [f"episode_{i}" for i in range(n_episodes)]
        timestamp = self.current_time
        encoding_strength = np.random.beta(2, 3, n_episodes) * 100
        retrieval_strength = np.random.beta(1, 4, n_episodes) * 100
        memory_indices = np.random.randint(0, self.config.max_memory_capacity, n_episodes)
        
        return {
            'episodes': episodes,
            'timestamp': timestamp,
            'encoding_strength': encoding_strength.tolist(),
            'retrieval_strength': retrieval_strength.tolist(),
            'memory_indices': memory_indices.tolist()
        }
    
    def _simulate_associative_memory(self):
        """Simulate associative memory dynamics."""
        # Simulate associative memory associations
        n_associations = np.random.randint(0, 10)
        associations = []
        connection_strengths = []
        pattern_completion_scores = []
        interference_levels = []
        
        for i in range(n_associations):
            associations.append(f"pair_{i}")
            connection_strengths.append(np.random.beta(2, 2) * 100)
            pattern_completion_scores.append(np.random.beta(1, 3) * 100)
            interference_levels.append(np.random.beta(0.5, 2) * 50)
        
        return {
            'associations': associations,
            'connection_strengths': connection_strengths,
            'pattern_completion_scores': pattern_completion_scores,
            'interference_levels': interference_levels
        }
    
    def _render_plots(self):
        """Render the current visualization plots."""
        if not self.fig:
            return
        
        # Clear axes
        for ax in self.memory_plots:
            ax.clear()
        
        # Plot working memory if enabled
        if "working" in self.config.memory_types:
            self._plot_working_memory()
        
        # Plot episodic memory if enabled
        if "episodic" in self.config.memory_types:
            self._plot_episodic_memory()
        
        # Plot associative memory if enabled
        if "associative" in self.config.memory_types:
            self._plot_associative_memory()
        
        # Update plot labels
        self._update_plot_labels()
        
        # Refresh the figure
        self.fig.canvas.draw()
    
    def _plot_working_memory(self):
        """Plot working memory visualization."""
        if len(self.data_history) < 10:
            return
        
        # Get recent working memory data
        recent_data = self.data_history[-10:]
        
        # Create heatmap for working memory items
        if any("working_memory" in step_data for step_data in recent_data):
            # Prepare data for heatmap
            heatmap_data = []
            for step_data in recent_data:
                if "working_memory" in step_data:
                    wm = step_data["working_memory"]
                    heatmap_row = [wm["capacity_utilization"]] + wm["activation_levels"]
                    heatmap_data.append(heatmap_row)
            
            if heatmap_data:
                heatmap_array = np.array(heatmap_data).T  # Transpose for better visualization
                
                # Create custom colormap for working memory
                working_cmap = LinearSegmentedColormap.from_list(
                    "working_memory", ["red", "yellow", "green"], N=256
                )
                
                # Display heatmap
                im = self.memory_plots[0].imshow(
                    heatmap_array, aspect='auto', cmap=working_cmap,
                    extent=[0, len(self.memory_plots[0].get_ylim()[0]), 0, len(heatmap_data)],
                    origin='lower'
                )
                
                # Add colorbar
                plt.colorbar(im, ax=self.memory_plots[0], shrink=0.8)
    
    def _plot_episodic_memory(self):
        """Plot episodic memory visualization."""
        if len(self.data_history) < 10:
            return
        
        # Get recent episodic memory data
        recent_data = self.data_history[-10:]
        
        # Plot encoding and retrieval strengths
        time_series = [step['time'] for step in recent_data]
        
        for i, step_data in enumerate(recent_data):
            if "episodic_memory" in step_data:
                em = step_data["episodic_memory"]
                if em["encoding_strength"]:
                    self.memory_plots[1].plot(
                        time_series[i], np.mean(em["encoding_strength"]),
                        'bo-', linewidth=2, markersize=8, label=f"Encoding {i}"
                    )
                if em["retrieval_strength"]:
                    self.memory_plots[1].plot(
                        time_series[i], np.mean(em["retrieval_strength"]),
                        'ro-', linewidth=2, markersize=8, label=f"Retrieval {i}"
                    )
        
        self.memory_plots[1].set_ylim(0, 100)
        self.memory_plots[1].legend(loc='upper right', fontsize=8)
    
    def _plot_associative_memory(self):
        """Plot associative memory visualization."""
        if len(self.data_history) < 10:
            return
        
        # Get recent associative memory data
        recent_data = self.data_history[-10:]
        
        # Create network visualization for associative memory
        if any("associative_memory" in step_data for step_data in recent_data):
            # Collect all associations
            all_associations = []
            all_strengths = []
            
            for step_data in recent_data:
                if "associative_memory" in step_data:
                    am = step_data["associative_memory"]
                    all_associations.extend(am["associations"])
                    all_strengths.extend(am["connection_strengths"])
            
            if all_associations:
                # Create bar chart of association strengths
                y_pos = np.arange(len(all_associations))
                strengths = np.array(all_strengths)
                
                bars = self.memory_plots[2].barh(y_pos, strengths, alpha=0.7, color='steelblue')
                self.memory_plots[2].set_yticks(y_pos)
                self.memory_plots[2].set_yticklabels(all_associations)
                self.memory_plots[2].set_xlabel('Connection Strength')
                self.memory_plots[2].set_title('Associative Memory Connections')
                
                # Add value labels on bars
                for i, bar in enumerate(bars):
                    width = bar.get_width()
                    self.memory_plots[2].text(
                        width + 0.1, i, f'{width:.1f}',
                        va='center', fontsize=8
                    )
    
    def _update_plot_labels(self):
        """Update plot labels with current memory statistics."""
        current_time_ms = self.current_time
        
        # Update titles with current statistics
        if self.data_history:
            latest_data = self.data_history[-1]
            
            # Update working memory title with statistics
            if "working_memory" in latest_data:
                wm = latest_data["working_memory"]
                capacity_util = wm["capacity_utilization"] * 100
                avg_activation = np.mean(wm["activation_levels"]) if wm["activation_levels"] else 0
                
                self.memory_plots[0].set_title(
                    f'Working Memory (Capacity: {capacity_util:.1f}%, Avg Activation: {avg_activation:.1f})',
                    fontsize=12, fontweight='bold'
                )
        
        # Update time in title
        time_str = f'Time: {current_time_ms:.0f} ms'
        self.fig.suptitle(time_str, fontsize=14, fontweight='bold')
    
    def _save_current_frame(self):
        """Save current visualization frame."""
        if self.config.save_path:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{self.config.save_path}/memory_frame_{timestamp}.png"
            
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            try:
                self.fig.savefig(filename, dpi=150, bbox_inches='tight')
                print(f"Saved memory visualization frame: {filename}")
            except Exception as e:
                print(f"Warning: Could not save frame: {e}")
    
    def start_encoding(self, stimulus_data: Dict[str, Any]):
        """Start memory encoding visualization."""
        self.state = MemoryVisualizationState.ENCODING
        print(f"Starting memory encoding visualization with stimulus data...")
        self._simulate_encoding(stimulus_data)
    
    def start_decoding(self, retrieval_data: Dict[str, Any]):
        """Start memory decoding visualization."""
        self.state = MemoryVisualizationState.DECODING
        print(f"Starting memory decoding visualization with retrieval data...")
        self._simulate_decoding(retrieval_data)
    
    def start_consolidation(self, consolidation_data: Dict[str, Any]):
        """Start memory consolidation visualization."""
        self.state = MemoryVisualizationState.CONSOLIDATING
        print(f"Starting memory consolidation visualization...")
        self._simulate_consolidation(consolidation_data)
    
    def _simulate_encoding(self, stimulus_data: Dict[str, Any]):
        """Simulate encoding process."""
        # Add encoding-related data to episodic memory
        encoding_step = {
            'time': self.current_time,
            'type': 'encoding',
            'stimulus': stimulus_data.get('stimulus', ''),
            'strength': np.random.beta(2, 3) * 100,
            'duration': stimulus_data.get('duration', 100)
        }
        
        self.memory_traces["episodic"]["episodes"].append(encoding_step['stimulus'])
        self.memory_traces["episodic"]["encoding_strength"].append(encoding_step['strength'])
        self.memory_traces["episodic"]["timestamp"].append(encoding_step['time'])
    
    def _simulate_decoding(self, retrieval_data: Dict[str, Any]):
        """Simulate decoding process."""
        # Add decoding-related data to episodic memory
        decoding_step = {
            'time': self.current_time,
            'type': 'decoding',
            'target': retrieval_data.get('target', ''),
            'success': np.random.random() > 0.3,
            'confidence': np.random.beta(2, 2) * 100
        }
        
        self.memory_traces["episodic"]["episodes"].append(decoding_step['target'])
        self.memory_traces["episodic"]["retrieval_strength"].append(decoding_step['confidence'])
        self.memory_traces["episodic"]["timestamp"].append(decoding_step['time'])
    
    def _simulate_consolidation(self, consolidation_data: Dict[str, Any]):
        """Simulate consolidation process."""
        # Add consolidation-related data to working memory
        consolidation_step = {
            'time': self.current_time,
            'type': 'consolidation',
            'rate': consolidation_data.get('rate', 0.01),
            'items_consolidated': np.random.randint(1, 10),
            'stability_gain': np.random.beta(1, 3) * 100
        }
        
        self.memory_traces["working"]["capacity_utilization"] = min(
            self.memory_traces["working"]["capacity_utilization"] + consolidation_step['rate'],
            1.0
        )
    
    def stop(self):
        """Stop the visualization."""
        self.state = MemoryVisualizationState.IDLE
        
        # Stop animation thread
        if hasattr(self, 'animation_thread') and self.animation_thread.is_alive():
            self.animation_thread.join(timeout=5.0)
        
        print("Memory visualization stopped")
    
    def export_data(self, output_path: str, format: str = "json"):
        """
        Export visualization data to file.
        
        Args:
            output_path: Path to output file
            format: Export format (json, csv, npy)
        """
        self.state = MemoryVisualizationState.EXPORTING
        
        try:
            if format.lower() == "json":
                export_data = {
                    'config': self.config.__dict__,
                    'data_history': self.data_history,
                    'memory_traces': self.memory_traces,
                    'current_time': self.current_time,
                    'timestamp': datetime.now().isoformat()
                }
                
                with open(output_path, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                    
            elif format.lower() == "csv":
                import pandas as pd
                
                # Convert data history to DataFrame
                df_data = []
                for step_data in self.data_history:
                    df_data.append({
                        'time': step_data['time'],
                        'working_memory_capacity': step_data.get('working_memory', {}).get('capacity_utilization', 0),
                        'episodic_memory_count': len(step_data.get('episodic_memory', {}).get('episodes', [])),
                        'associative_memory_count': len(step_data.get('associative_memory', {}).get('associations', []))
                    })
                
                df = pd.DataFrame(df_data)
                df.to_csv(output_path, index=False)
                
            elif format.lower() == "npy":
                np.savez(output_path, 
                        data_history=np.array(self.data_history, dtype=object),
                        current_time=self.current_time,
                        config=self.config.__dict__)
            
            print(f"Data exported to: {output_path}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
        
        finally:
            self.state = MemoryVisualizationState.IDLE

def main():
    """Main entry point for nlx-viz-memory tool."""
    print("NLM - Neural Memory Visualization Tool")
    print("=" * 50)
    print("Memory system activity visualization with sophisticated capabilities")
    print()
    
    # Example usage
    config = MemoryVisualizationConfig(
        update_rate=20.0,
        memory_types=["working", "episodic", "associative"],
        show_encoding=True,
        show_decoding=True,
        show_consolidation=True,
        max_memory_capacity=1000,
        window_size=(1200, 800)
    )
    
    # Create memory visualizer
    visualizer = NLMemoryVisualizer(config)
    
    # Example memory data structure
    memory_data = {
        'working_memory': {
            'items': ['item1', 'item2', 'item3'],
            'activation_levels': [85.5, 67.2, 92.1],
            'decay_rates': [0.95, 0.97, 0.93],
            'capacity_utilization': 0.3
        },
        'episodic_memory': {
            'episodes': ['episode1', 'episode2'],
            'timestamp': 1000.0,
            'encoding_strength': [78.5, 92.3],
            'retrieval_strength': [65.2, 88.7],
            'memory_indices': [10, 45]
        },
        'associative_memory': {
            'associations': ['pair1', 'pair2', 'pair3'],
            'connection_strengths': [85.5, 67.2, 92.1],
            'pattern_completion_scores': [78.5, 92.3, 65.2],
            'interference_levels': [12.5, 8.3, 15.7]
        }
    }
    
    print("Example configuration created successfully!")
    print(f"Memory types: {config.memory_types}")
    print(f"Update rate: {config.update_rate} Hz")
    print(f"Window size: {config.window_size}")
    print()
    print("To use the visualizer:")
    print("  visualizer = NLMemoryVisualizer(config)")
    print("  visualizer.start_memory_viz(memory_data)")
    print("  visualizer.start_encoding(stimulus_data)")
    print("  visualizer.start_decoding(retrieval_data)")
    print("  visualizer.start_consolidation(consolidation_data)")
    print("  visualizer.stop()   # to stop")
    print("  visualizer.export_data('output.json', 'json')  # to export")

if __name__ == "__main__":
    main()
