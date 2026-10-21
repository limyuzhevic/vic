#!/usr/bin/env python3
"""
Memory Viewer Tool

Visualization of memory systems and traces in neural networks.
Provides comprehensive visualization of memory dynamics,
temporal patterns, and memory consolidation processes.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap, ListedColormap
import seaborn as sns
import pandas as pd
from datetime import datetime, timedelta
import sys
import os

@dataclass
class MemoryViewerConfig:
    """Configuration for memory viewer"""
    time_window: float = 30.0  # seconds
    refresh_rate: float = 30.0  # Hz
    show_memory_traces: bool = True
    show_memory_consolidation: bool = True
    show_memory_capacity: bool = True
    show_recall_patterns: bool = True
    show_memory_types: bool = True
    animate: bool = True
    color_scheme: str = "memory"  # "memory", "temporal", "type", "activity"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "gif", "mp4", "html"
    output_path: str = "memory_viewer"
    
    # Memory system types
    show_working_memory: bool = True
    show_episodic_memory: bool = True
    show_semantic_memory: bool = True
    show_procedural_memory: bool = True
    show_long_term_memory: bool = True
    
    # Visualization modes
    view_mode: str = "temporal"  # "temporal", "spatial", "system", "consolidation"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (18, 12)
    dpi: int = 150
    animation_interval: int = 200  # milliseconds
    
    # Filtering and analysis
    filter_type: str = None  # "moving_average", "exponential", "bandpass"
    filter_cutoff: float = 5.0  # Hz
    show_retrieval_patterns: bool = True
    show_forgetting_curve: bool = True
    show_encoding_patterns: bool = True
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    smooth_traces: bool = True
    interpolation: str = "linear"  # "none", "linear", "cubic"
    
    # Memory-specific parameters
    encoding_threshold: float = 0.5
    retrieval_threshold: float = 0.7
    consolidation_rate: float = 0.1
class MemoryViewer:
    """
    Visualization tool for memory systems and traces in neural networks.
    
    Features:
    - Real-time memory trace visualization
    - Memory system dynamics and interactions
    - Memory capacity and load analysis
    - Temporal memory patterns and consolidation
    - Recall and retrieval pattern visualization
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: MemoryViewerConfig = None):
        self.config = config or MemoryViewerConfig()
        self.memory_traces = []  # List of memory trace arrays
        self.consolidation_history = []  # Memory consolidation over time
        self.recall_patterns = []  # Recall and retrieval patterns
        self.encoding_patterns = []  # Encoding patterns
        self.memory_systems = {}  # Memory system data
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize viewer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize viewer components and data structures"""
        # Memory system data structures
        self.working_memory = []  # Working memory traces
        self.episodic_memory = []  # Episodic memory traces
        self.semantic_memory = []  # Semantic memory traces
        self.procedural_memory = []  # Procedural memory traces
        self.long_term_memory = []  # Long-term memory traces
        
        # Memory system metadata
        self.memory_metadata = {
            'working_memory': {'capacity': 10, 'decay_rate': 0.95, 'encoding_rate': 0.8},
            'episodic_memory': {'capacity': 100, 'decay_rate': 0.98, 'encoding_rate': 0.6},
            'semantic_memory': {'capacity': 1000, 'decay_rate': 0.99, 'encoding_rate': 0.4},
            'procedural_memory': {'capacity': 50, 'decay_rate': 0.97, 'encoding_rate': 0.7},
            'long_term_memory': {'capacity': 5000, 'decay_rate': 0.999, 'encoding_rate': 0.2}
        }
        
        # Initialize colormaps for different memory types
        self._initialize_colormaps()
        
        # Setup data buffers
        self._setup_data_buffers()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different memory visualizations"""
        # Memory system colormaps
        self.working_memory_cmap = LinearSegmentedColormap.from_list(
            'working_memory', ['red', 'orange', 'yellow', 'white']
        )
        
        self.episodic_memory_cmap = LinearSegmentedColormap.from_list(
            'episodic_memory', ['blue', 'cyan', 'lightblue', 'white']
        )
        
        self.semantic_memory_cmap = LinearSegmentedColormap.from_list(
            'semantic_memory', ['green', 'lime', 'lightgreen', 'white']
        )
        
        self.procedural_memory_cmap = LinearSegmentedColormap.from_list(
            'procedural_memory', ['purple', 'magenta', 'pink', 'white']
        )
        
        self.long_term_memory_cmap = LinearSegmentedColormap.from_list(
            'long_term_memory', ['brown', 'orange', 'yellow', 'white']
        )
        
        # Temporal memory colormap
        self.temporal_cmap = LinearSegmentedColormap.from_list(
            'temporal', ['darkred', 'red', 'orange', 'yellow', 'green', 'blue', 'purple']
        )
        
        # Type-based colormap
        self.type_cmap = ListedColormap([
            '#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7',
            '#DDA0DD', '#FFB6C1', '#B0E0E6', '#F0E68C', '#D2B48C'
        ])
    
    def _setup_data_buffers(self):
        """Setup data buffers for visualization"""
        self.max_history = 1000  # Maximum number of time steps to store
        self.trace_buffer = []
        self.consolidation_buffer = []
        self.recall_buffer = []
        self.encoding_buffer = []
    
    def add_memory_data(self, 
                       working_memory: np.ndarray,
                       episodic_memory: np.ndarray,
                       semantic_memory: np.ndarray,
                       procedural_memory: np.ndarray,
                       long_term_memory: np.ndarray,
                       consolidation: Dict[str, Any],
                       recall_patterns: Dict[str, Any],
                       encoding_patterns: Dict[str, Any],
                       time_step: float = 1.0):
        """
        Add memory data for visualization.
        
        Args:
            working_memory: Working memory trace data (N x T)
            episodic_memory: Episodic memory trace data (M x T)
            semantic_memory: Semantic memory trace data (P x T)
            procedural_memory: Procedural memory trace data (Q x T)
            long_term_memory: Long-term memory trace data (R x T)
            consolidation: Memory consolidation metrics
            recall_patterns: Recall and retrieval patterns
            encoding_patterns: Encoding patterns
            time_step: Current time step
        """
        # Store memory traces
        if working_memory is not None:
            self.working_memory.append(working_memory.copy())
        if episodic_memory is not None:
            self.episodic_memory.append(episodic_memory.copy())
        if semantic_memory is not None:
            self.semantic_memory.append(semantic_memory.copy())
        if procedural_memory is not None:
            self.procedural_memory.append(procedural_memory.copy())
        if long_term_memory is not None:
            self.long_term_memory.append(long_term_memory.copy())
        
        # Store other memory data
        self.consolidation_history.append(consolidation.copy())
        self.recall_patterns.append(recall_patterns.copy())
        self.encoding_patterns.append(encoding_patterns.copy())
        
        # Update buffers
        self.trace_buffer.append({
            'working_memory': working_memory.copy() if working_memory is not None else None,
            'episodic_memory': episodic_memory.copy() if episodic_memory is not None else None,
            'semantic_memory': semantic_memory.copy() if semantic_memory is not None else None,
            'procedural_memory': procedural_memory.copy() if procedural_memory is not None else None,
            'long_term_memory': long_term_memory.copy() if long_term_memory is not None else None
        })
        
        self.consolidation_buffer.append(consolidation)
        self.recall_buffer.append(recall_patterns)
        self.encoding_buffer.append(encoding_patterns)
        
        # Maintain buffer size
        if len(self.trace_buffer) > self.max_history:
            self.trace_buffer.pop(0)
            self.consolidation_buffer.pop(0)
            self.recall_buffer.pop(0)
            self.encoding_buffer.pop(0)
        
        # Update current time
        self.current_time += time_step
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time memory visualization.
        
        Args:
            initial_data: Optional initial memory data
        """
        if initial_data:
            # Add initial data
            for i, (wm, em, sm, pm, ltm, cons, recall, encode, t) in enumerate(zip(
                initial_data.get('working_memory', []),
                initial_data.get('episodic_memory', []),
                initial_data.get('semantic_memory', []),
                initial_data.get('procedural_memory', []),
                initial_data.get('long_term_memory', []),
                initial_data.get('consolidation', []),
                initial_data.get('recall_patterns', []),
                initial_data.get('encoding_patterns', []),
                initial_data.get('time_steps', [1.0] * len(initial_data.get('working_memory', [])))
            )):
                self.add_memory_data(wm, em, sm, pm, ltm, cons, recall, encode, time_step=t)
        
        self.is_running = True
        
        # Create visualization
        self.fig = plt.figure(figsize=self.config.figure_size)
        
        # Create subplot layout based on view mode
        if self.config.view_mode == "temporal":
            self._create_temporal_view()
        elif self.config.view_mode == "spatial":
            self._create_spatial_view()
        elif self.config.view_mode == "system":
            self._create_system_view()
        elif self.config.view_mode == "consolidation":
            self._create_consolidation_view()
        else:
            self._create_temporal_view()
        
        # Setup animation
        self.animation_obj = animation.FuncAnimation(
            self.fig, self._update_plot, interval=self.config.animation_interval,
            blit=False, cache_frame_data=False
        )
        
        plt.show()
    
    def _create_temporal_view(self):
        """Create temporal view visualization"""
        # Create grid of subplots for temporal analysis
        gs = plt.GridSpec(4, 5, hspace=0.3, wspace=0.3)
        
        # Memory trace evolution (working memory)
        self.ax_working_trace = plt.subplot(gs[0, :2])
        
        # Episodic memory traces
        self.ax_episodic_trace = plt.subplot(gs[0, 2:4])
        
        # Semantic memory traces
        self.ax_semantic_trace = plt.subplot(gs[0, 4])
        
        # Memory system interaction
        self.ax_system_interaction = plt.subplot(gs[1, :5])
        
        # Consolidation process
        self.ax_consolidation = plt.subplot(gs[2, :3])
        
        # Recall patterns
        self.ax_recall_patterns = plt.subplot(gs[2, 3:])
        
        # Encoding patterns
        self.ax_encoding_patterns = plt.subplot(gs[3, :3])
        
        # Forgetting curve
        self.ax_forgetting_curve = plt.subplot(gs[3, 3:])
        
        # Initialize temporal plot elements
        self._initialize_temporal_elements()
    
    def _create_spatial_view(self):
        """Create spatial view visualization"""
        # Create grid for spatial memory visualization
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Working memory spatial distribution
        self.ax_wm_spatial = plt.subplot(gs[0, 0])
        
        # Episodic memory spatial distribution
        self.ax_em_spatial = plt.subplot(gs[0, 1])
        
        # Semantic memory spatial distribution
        self.ax_sm_spatial = plt.subplot(gs[0, 2])
        
        # Procedural memory spatial distribution
        self.ax_pm_spatial = plt.subplot(gs[1, 0])
        
        # Long-term memory spatial distribution
        self.ax_ltm_spatial = plt.subplot(gs[1, 1])
        
        # Memory system interactions
        self.ax_system_interactions = plt.subplot(gs[1, 2])
        
        # Memory capacity utilization
        self.ax_capacity_utilization = plt.subplot(gs[2, 0])
        
        # Memory efficiency heatmap
        self.ax_efficiency_heatmap = plt.subplot(gs[2, 1])
        
        # Memory performance metrics
        self.ax_performance_metrics = plt.subplot(gs[2, 2])
        
        # Initialize spatial elements
        self._initialize_spatial_elements()
    
    def _create_system_view(self):
        """Create system view visualization"""
        # Create multi-panel system view
        gs = plt.GridSpec(3, 4, hspace=0.4, wspace=0.3)
        
        # Memory system overview
        self.ax_system_overview = plt.subplot(gs[0, :4])
        
        # Working memory dynamics
        self.ax_wm_dynamics = plt.subplot(gs[1, 0])
        
        # Episodic memory dynamics
        self.ax_em_dynamics = plt.subplot(gs[1, 1])
        
        # Semantic memory dynamics
        self.ax_sm_dynamics = plt.subplot(gs[1, 2])
        
        # Procedural memory dynamics
        self.ax_pm_dynamics = plt.subplot(gs[1, 3])
        
        # Long-term memory dynamics
        self.ax_ltm_dynamics = plt.subplot(gs[2, :4])
        
        # Initialize system elements
        self._initialize_system_elements()
    
    def _create_consolidation_view(self):
        """Create consolidation view visualization"""
        # Create consolidation-focused view
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Consolidation rate over time
        self.ax_consolidation_rate = plt.subplot(gs[0, 0])
        
        # Memory strength evolution
        self.ax_memory_strength = plt.subplot(gs[0, 1])
        
        # Integration processes
        self.ax_integration_processes = plt.subplot(gs[0, 2])
        
        # Synaptic consolidation
        self.ax_synaptic_consolidation = plt.subplot(gs[1, 0])
        
        # Systems consolidation
        self.ax_systems_consolidation = plt.subplot(gs[1, 1])
n        
        # Reconsolidation processes
        self.ax_reconsolidation = plt.subplot(gs[1, 2])
        
        # Memory stability analysis
        self.ax_stability_analysis = plt.subplot(gs[2, 0])
        
        # Forgetting and retention
        self.ax_forgetting_retention = plt.subplot(gs[2, 1])
        
        # Memory transfer efficiency
        self.ax_transfer_efficiency = plt.subplot(gs[2, 2])
        
        # Initialize consolidation elements
        self._initialize_consolidation_elements()
    
    def _initialize_temporal_elements(self):
        """Initialize temporal view plot elements"""
        if hasattr(self, 'ax_working_trace'):
            # Working memory trace
            self.wm_trace_lines = []
            self.ax_working_trace.set_title("Working Memory Traces", fontsize=12, fontweight='bold')
            self.ax_working_trace.set_xlabel("Time Step")
            self.ax_working_trace.set_ylabel("Memory Strength")
            self.ax_working_trace.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_episodic_trace'):
            # Episodic memory traces
            self.em_trace_lines = []
            self.ax_episodic_trace.set_title("Episodic Memory Traces", fontsize=12, fontweight='bold')
            self.ax_episodic_trace.set_xlabel("Time Step")
            self.ax_episodic_trace.set_ylabel("Memory Strength")
            self.ax_episodic_trace.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_semantic_trace'):
            # Semantic memory traces
            self.sm_trace_plot, = self.ax_semantic_trace.plot([], [], 'g-', linewidth=2)
            self.ax_semantic_trace.set_title("Semantic Memory Traces", fontsize=12, fontweight='bold')
            self.ax_semantic_trace.set_xlabel("Time Step")
            self.ax_semantic_trace.set_ylabel("Memory Strength")
            self.ax_semantic_trace.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_system_interaction'):
            # System interaction plot
            self.system_interaction_plot, = self.ax_system_interaction.plot([], [], 'b-', linewidth=2)
            self.ax_system_interaction.set_title("System Interaction", fontsize=12, fontweight='bold')
            self.ax_system_interaction.set_xlabel("Time Step")
            self.ax_system_interaction.set_ylabel("Interaction Strength")
            self.ax_system_interaction.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_consolidation'):
            # Consolidation plot
            self.consolidation_plot, = self.ax_consolidation.plot([], [], 'r-', linewidth=2)
            self.ax_consolidation.set_title("Memory Consolidation", fontsize=12, fontweight='bold')
            self.ax_consolidation.set_xlabel("Time Step")
            self.ax_consolidation.set_ylabel("Consolidation Rate")
            self.ax_consolidation.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_recall_patterns'):
            # Recall patterns plot
            self.recall_plot, = self.ax_recall_patterns.plot([], [], 'm-', linewidth=2)
            self.ax_recall_patterns.set_title("Recall Patterns", fontsize=12, fontweight='bold')
            self.ax_recall_patterns.set_xlabel("Time Step")
            self.ax_recall_patterns.set_ylabel("Recall Probability")
            self.ax_recall_patterns.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_encoding_patterns'):
            # Encoding patterns plot
            self.encoding_plot, = self.ax_encoding_patterns.plot([], [], 'c-', linewidth=2)
            self.ax_encoding_patterns.set_title("Encoding Patterns", fontsize=12, fontweight='bold')
            self.ax_encoding_patterns.set_xlabel("Time Step")
            self.ax_encoding_patterns.set_ylabel("Encoding Rate")
            self.ax_encoding_patterns.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_forgetting_curve'):
            # Forgetting curve plot
            self.forgetting_plot, = self.ax_forgetting_curve.plot([], [], 'orange', linewidth=2)
            self.ax_forgetting_curve.set_title("Forgetting Curve", fontsize=12, fontweight='bold')
            self.ax_forgetting_curve.set_xlabel("Time Since Encoding")
            self.ax_forgetting_curve.set_ylabel("Memory Retention")
            self.ax_forgetting_curve.grid(True, alpha=0.3)
    
    def _initialize_spatial_elements(self):
        """Initialize spatial view plot elements"""
        # Initialize all spatial plot elements
        # (Implementation would continue with spatial-specific initialization)
    
    def _initialize_system_elements(self):
        """Initialize system view plot elements"""
        # Initialize all system plot elements
        # (Implementation would continue with system-specific initialization)
    
    def _initialize_consolidation_elements(self):
        """Initialize consolidation view plot elements"""
        # Initialize all consolidation plot elements
        # (Implementation would continue with consolidation-specific initialization)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "temporal":
            self._update_temporal_view(frame)
        elif self.config.view_mode == "spatial":
            self._update_spatial_view(frame)
        elif self.config.view_mode == "system":
            self._update_system_view(frame)
        elif self.config.view_mode == "consolidation":
            self._update_consolidation_view(frame)
    
    def _update_temporal_view(self, frame):
        """Update temporal view elements"""
        if not self.working_memory:
            return
        
        current_idx = min(frame, len(self.working_memory) - 1)
        
        # Update working memory traces
        if hasattr(self, 'ax_working_trace') and hasattr(self, 'wm_trace_lines'):
            self._update_working_memory_trace(current_idx)
        
        # Update episodic memory traces
        if hasattr(self, 'ax_episodic_trace') and hasattr(self, 'em_trace_lines'):
            self._update_episodic_memory_trace(current_idx)
        
        # Update semantic memory traces
        if hasattr(self, 'ax_semantic_trace'):
            self._update_semantic_memory_trace(current_idx)
        
        # Update system interaction
        if hasattr(self, 'ax_system_interaction'):
            self._update_system_interaction(current_idx)
        
        # Update consolidation
        if hasattr(self, 'ax_consolidation'):
            self._update_consolidation(current_idx)
        
        # Update recall patterns
        if hasattr(self, 'ax_recall_patterns'):
            self._update_recall_patterns(current_idx)
        
        # Update encoding patterns
        if hasattr(self, 'ax_encoding_patterns'):
            self._update_encoding_patterns(current_idx)
        
        # Update forgetting curve
        if hasattr(self, 'ax_forgetting_curve'):
            self._update_forgetting_curve(current_idx)
    
    def _update_working_memory_trace(self, current_idx: int):
        """Update working memory trace visualization"""
        if not self.working_memory:
            return
        
        wm_trace = self.working_memory[current_idx]
        
        # Convert to list of time series
        if len(self.wm_trace_lines) == 0:
            # Initialize lines for each memory item
            for i in range(wm_trace.shape[0]):
                line, = self.ax_working_trace.plot([], [], linewidth=2)
                self.wm_trace_lines.append(line)
        
        # Update line data
        time_steps = list(range(min(current_idx + 1, len(self.working_memory))))
        
        for i, line in enumerate(self.wm_trace_lines):
            if i < wm_trace.shape[0]:
                y_data = wm_trace[i, :min(current_idx + 1, wm_trace.shape[1])]
                line.set_data(time_steps[:len(y_data)], y_data)
    
    def _update_episodic_memory_trace(self, current_idx: int):
        """Update episodic memory trace visualization"""
        # Similar to working memory but for episodic memory
        # (Implementation would continue)
    
    def _update_semantic_memory_trace(self, current_idx: int):
        """Update semantic memory trace visualization"""
        # Similar to working memory but for semantic memory
        # (Implementation would continue)
    
    def _update_system_interaction(self, current_idx: int):
        """Update system interaction visualization"""
        # Update system interaction plot
        if hasattr(self, 'system_interaction_plot'):
            # Calculate system interaction strength
            if self.working_memory and current_idx < len(self.working_memory):
                wm = self.working_memory[current_idx]
                interaction_strength = np.mean(wm) if wm.size > 0 else 0.0
                
                x_data = list(range(current_idx + 1))
                self.system_interaction_plot.set_data(x_data, [interaction_strength] * len(x_data))
    
    def _update_consolidation(self, current_idx: int):
        """Update consolidation visualization"""
        # Update consolidation plot
        if hasattr(self, 'consolidation_plot') and self.consolidation_history:
            if current_idx < len(self.consolidation_history):
                consolidation = self.consolidation_history[current_idx]
                consolidation_rate = consolidation.get('consolidation_rate', 0.0)
                
                x_data = list(range(current_idx + 1))
                self.consolidation_plot.set_data(x_data, [consolidation_rate] * len(x_data))
    
    def _update_recall_patterns(self, current_idx: int):
        """Update recall patterns visualization"""
        # Update recall patterns plot
        if hasattr(self, 'recall_plot') and self.recall_patterns:
            if current_idx < len(self.recall_patterns):
                recall = self.recall_patterns[current_idx]
                recall_prob = recall.get('recall_probability', 0.0)
                
                x_data = list(range(current_idx + 1))
                self.recall_plot.set_data(x_data, [recall_prob] * len(x_data))
    
    def _update_encoding_patterns(self, current_idx: int):
        """Update encoding patterns visualization"""
        # Update encoding patterns plot
        if hasattr(self, 'encoding_plot') and self.encoding_patterns:
            if current_idx < len(self.encoding_patterns):
                encoding = self.encoding_patterns[current_idx]
                encoding_rate = encoding.get('encoding_rate', 0.0)
                
                x_data = list(range(current_idx + 1))
                self.encoding_plot.set_data(x_data, [encoding_rate] * len(x_data))
    
    def _update_forgetting_curve(self, current_idx: int):
        """Update forgetting curve visualization"""
        # Update forgetting curve plot
        if hasattr(self, 'forgetting_plot'):
            # Generate forgetting curve based on memory decay
            time_since_encoding = np.linspace(0, 100, 50)
            # Simple exponential decay model
            retention = np.exp(-0.05 * time_since_encoding)
            
            self.forgetting_plot.set_data(time_since_encoding, retention)
            self.ax_forgetting_curve.set_xlim(0, max(time_since_encoding))
            self.ax_forgetting_curve.set_ylim(0, 1.1)
    
    def create_static_visualization(self, output_file: str = None) -> Dict[str, Any]:
        """
        Create static visualization for analysis and reporting.
        
        Args:
            output_file: Optional output filename
            
        Returns:
            Dictionary with visualization data and results
        """
        result = {
            "success": False,
            "data": {},
            "plots": {},
            "statistics": {},
            "output_files": []
        }
        
        try:
            if not self.working_memory and not self.episodic_memory:
                result["error"] = "No memory data available"
                return result
            
            # Create comprehensive memory visualization
            fig = self._create_comprehensive_memory_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_memory_statistics()
            
            result["plots"] = "comprehensive memory visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_memory_visualization(self):
        """Create comprehensive memory visualization"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "temporal":
            return self._create_temporal_memory_comprehensive()
        elif self.config.view_mode == "spatial":
            return self._create_spatial_memory_comprehensive()
        elif self.config.view_mode == "system":
            return self._create_system_memory_comprehensive()
        elif self.config.view_mode == "consolidation":
            return self._create_consolidation_memory_comprehensive()
        else:
            return self._create_temporal_memory_comprehensive()
    
    def _create_temporal_memory_comprehensive(self):
        """Create comprehensive temporal memory visualization"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Panel 1: Working memory dynamics
        self._plot_working_memory_dynamics(fig, gs[0, :2])
        
        # Panel 2: Episodic memory dynamics
        self._plot_episodic_memory_dynamics(fig, gs[0, 2:])
        
        # Panel 3: Memory system interaction
        self._plot_memory_system_interaction(fig, gs[1, :4])
        
        # Panel 4: Consolidation process
        self._plot_consolidation_process(fig, gs[2, :2])
        
        # Panel 5: Recall patterns
        self._plot_recall_patterns(fig, gs[2, 2:])
        
        # Panel 6: Encoding patterns
        self._plot_encoding_patterns(fig, gs[3, :2])
        
        # Panel 7: Forgetting curve
        self._plot_forgetting_curve(fig, gs[3, 2:])
        
        fig.suptitle("Comprehensive Memory Visualization", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_working_memory_dynamics(self, fig, ax):
        """Plot working memory dynamics"""
        if self.working_memory:
            # Calculate working memory statistics
            final_wm = self.working_memory[-1]
            
            if final_wm.size > 0:
                memory_strength = np.mean(final_wm)
                memory_variance = np.var(final_wm)
                memory_capacity_usage = np.mean(final_wm > self.config.encoding_threshold)
                
                # Create radar chart or bar chart
                categories = ['Strength', 'Variance', 'Capacity Usage', 'Stability']
                values = [memory_strength, memory_variance, memory_capacity_usage, 1.0 - memory_variance]
                
                angles = np.linspace(0, 2*np.pi, len(categories), endpoint=False)
                values += values[:1]  # Close the circle
                angles += angles[:1]
                
                ax.plot(angles, values, 'o-', linewidth=2)
                ax.fill(angles, values, alpha=0.25)
                ax.set_thetagrids(angles * 180/np.pi, categories)
                ax.set_title("Working Memory Dynamics", fontsize=12, fontweight='bold')
                ax.set_ylim(0, max(values) * 1.1)
    
    def _plot_episodic_memory_dynamics(self, fig, ax):
        """Plot episodic memory dynamics"""
        if self.episodic_memory:
            # Similar to working memory but for episodic memory
            # (Implementation would continue)
            pass
    
    def _plot_memory_system_interaction(self, fig, ax):
        """Plot memory system interaction"""
        # Calculate interaction between memory systems
        if self.working_memory and self.episodic_memory and len(self.working_memory) > 1:
            # Calculate correlation between systems
            wm = self.working_memory[-1].flatten()
            em = self.episodic_memory[-1].flatten() if self.episodic_memory else np.array([])
            
            if em.size > 0 and len(wm) == len(em):
                correlation = np.corrcoef(wm, em)[0, 1]
                
                # Create interaction heatmap or correlation plot
                ax.bar(['Working Memory', 'Episodic Memory'], [np.mean(wm), np.mean(em)], 
                      color=['skyblue', 'lightcoral'], alpha=0.7)
                ax.set_ylabel("Memory Strength")
                ax.set_title("Memory System Interaction", fontsize=12, fontweight='bold')
                ax.set_ylim(0, max(np.mean(wm), np.mean(em)) * 1.2)
    
    def _plot_consolidation_process(self, fig, ax):
        """Plot consolidation process"""
        if self.consolidation_history:
            # Plot consolidation over time
            time_steps = list(range(len(self.consolidation_history)))
            consolidation_rates = [m.get('consolidation_rate', 0.0) for m in self.consolidation_history]
            
            ax.plot(time_steps, consolidation_rates, 'r-', linewidth=2, alpha=0.7)
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Consolidation Rate")
            ax.set_title("Memory Consolidation Process", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
    
    def _plot_recall_patterns(self, fig, ax):
        """Plot recall patterns"""
        if self.recall_patterns:
            # Plot recall patterns over time
            time_steps = list(range(len(self.recall_patterns)))
            recall_probs = [m.get('recall_probability', 0.0) for m in self.recall_patterns]
            
            ax.plot(time_steps, recall_probs, 'm-', linewidth=2, alpha=0.7)
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Recall Probability")
            ax.set_title("Recall Patterns", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
    
    def _plot_encoding_patterns(self, fig, ax):
        """Plot encoding patterns"""
        if self.encoding_patterns:
            # Plot encoding patterns over time
            time_steps = list(range(len(self.encoding_patterns)))
            encoding_rates = [m.get('encoding_rate', 0.0) for m in self.encoding_patterns]
            
            ax.plot(time_steps, encoding_rates, 'c-', linewidth=2, alpha=0.7)
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Encoding Rate")
            ax.set_title("Encoding Patterns", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
    
    def _plot_forgetting_curve(self, fig, ax):
        """Plot forgetting curve"""
        # Generate forgetting curve based on memory decay
        time_since_encoding = np.linspace(0, 100, 50)
        # Simple exponential decay model
        retention = np.exp(-0.05 * time_since_encoding)
        
        ax.plot(time_since_encoding, retention, 'orange', linewidth=3)
        ax.set_xlabel("Time Since Encoding")
        ax.set_ylabel("Memory Retention")
        ax.set_title("Forgetting Curve", fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, max(time_since_encoding))
        ax.set_ylim(0, 1.1)
    
    def _compute_memory_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive memory statistics"""
        stats = {
            "time_steps": len(self.working_memory) if self.working_memory else 0,
            "memory_systems": {},
            "consolidation_statistics": {},
            "recall_statistics": {},
            "encoding_statistics": {},
            "overall_performance": {}
        }
        
        # Memory system statistics
        if self.working_memory:
            wm = self.working_memory[-1]
            stats["memory_systems"]["working_memory"] = {
                "capacity_usage": float(np.mean(wm > self.config.encoding_threshold) if wm.size > 0 else 0.0),
                "average_strength": float(np.mean(wm) if wm.size > 0 else 0.0),
                "max_strength": float(np.max(wm) if wm.size > 0 else 0.0),
                "min_strength": float(np.min(wm) if wm.size > 0 else 0.0),
                "std_strength": float(np.std(wm) if wm.size > 0 else 0.0)
            }
        
        if self.episodic_memory:
            em = self.episodic_memory[-1]
            stats["memory_systems"]["episodic_memory"] = {
                "capacity_usage": float(np.mean(em > self.config.encoding_threshold) if em.size > 0 else 0.0),
                "average_strength": float(np.mean(em) if em.size > 0 else 0.0),
                "max_strength": float(np.max(em) if em.size > 0 else 0.0),
                "min_strength": float(np.min(em) if em.size > 0 else 0.0),
                "std_strength": float(np.std(em) if em.size > 0 else 0.0)
            }
        
        # Consolidation statistics
        if self.consolidation_history:
            consolidation_rates = [m.get('consolidation_rate', 0.0) for m in self.consolidation_history]
            stats["consolidation_statistics"] = {
                "initial_rate": consolidation_rates[0] if consolidation_rates else 0.0,
                "final_rate": consolidation_rates[-1] if consolidation_rates else 0.0,
                "average_rate": float(np.mean(consolidation_rates) if consolidation_rates else 0.0),
                "rate_variance": float(np.var(consolidation_rates) if consolidation_rates else 0.0)
            }
        
        # Recall statistics
        if self.recall_patterns:
            recall_probs = [m.get('recall_probability', 0.0) for m in self.recall_patterns]
            stats["recall_statistics"] = {
                "initial_recall": recall_probs[0] if recall_probs else 0.0,
                "final_recall": recall_probs[-1] if recall_probs else 0.0,
                "average_recall": float(np.mean(recall_probs) if recall_probs else 0.0),
                "recall_stability": float(1.0 - np.std(recall_probs) if recall_probs else 0.0)
            }
        
        # Encoding statistics
        if self.encoding_patterns:
            encoding_rates = [m.get('encoding_rate', 0.0) for m in self.encoding_patterns]
            stats["encoding_statistics"] = {
                "initial_encoding": encoding_rates[0] if encoding_rates else 0.0,
                "final_encoding": encoding_rates[-1] if encoding_rates else 0.0,
                "average_encoding": float(np.mean(encoding_rates) if encoding_rates else 0.0),
                "encoding_efficiency": float(np.mean(encoding_rates) / max(np.mean(recall_probs), 0.001) 
                                           if recall_probs and encoding_rates else 0.0)
            }
        
        # Overall performance
        stats["overall_performance"] = {
            "memory_efficiency": float(np.mean([stats["memory_systems"].get("working_memory", {}).get("capacity_usage", 0.0),
                                               stats["memory_systems"].get("episodic_memory", {}).get("capacity_usage", 0.0)])),
            "consolidation_effectiveness": float(stats["consolidation_statistics"].get("average_rate", 0.0)),
            "recall_accuracy": float(stats["recall_statistics"].get("average_recall", 0.0)),
            "encoding_success_rate": float(stats["encoding_statistics"].get("average_encoding", 0.0))
        }
        
        return stats
    
    def export_memory_data(self, filename: str) -> str:
        """
        Export memory data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "working_memory": [wm.tolist() for wm in self.working_memory],
            "episodic_memory": [em.tolist() for em in self.episodic_memory],
            "semantic_memory": [sm.tolist() for sm in self.semantic_memory],
            "procedural_memory": [pm.tolist() for pm in self.procedural_memory],
            "long_term_memory": [ltm.tolist() for ltm in self.long_term_memory],
            "consolidation_history": self.consolidation_history,
            "recall_patterns": self.recall_patterns,
            "encoding_patterns": self.encoding_patterns,
            "statistics": self._compute_memory_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_memory_viewer(config_dict: Dict[str, Any] = None) -> MemoryViewer:
    """
    Factory function to create MemoryViewer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured MemoryViewer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = MemoryViewerConfig(**config_dict)
    return MemoryViewer(config)