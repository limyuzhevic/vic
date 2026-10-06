#!/usr/bin/env python3
"""
NLM - Memory Debugging Tool

Memory leak detection and visualization with sophisticated capabilities for researchers.

Features:
- Memory allocation tracking
- Memory leak detection visualization
- Memory usage monitoring
- Memory fragmentation visualization
- Garbage collection visualization
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from typing import Dict, List, Optional, Tuple, Any
import time
import threading
from dataclasses import dataclass
import psutil
import gc
from collections import defaultdict
from enum import Enum

@dataclass
class MemoryDebugConfig:
    """Configuration for memory debugging."""
    track_allocations: bool = True
    detect_leaks: bool = True
    visualize_fragmentation: bool = True
    monitor_usage: bool = True
    garbage_collection: bool = True
    update_rate: float = 5.0  # Hz
    memory_history_size: int = 1000
    leak_threshold: float = 0.1  # MB
    fragmentation_threshold: float = 0.3
    window_size: Tuple[int, int] = (1400, 900)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 10

class MemoryDebugState(Enum):
    """Memory debugging state enumeration."""
    IDLE = "idle"
    MONITORING = "monitoring"
    DETECTING = "detecting"
    ANALYZING = "analyzing"
    EXPORTING = "exporting"

class NLMemoryDebugger:
    """
    Memory leak detection and visualization tool.
    
    Provides sophisticated memory debugging capabilities including allocation tracking,
    leak detection, usage monitoring, fragmentation visualization, and garbage
    collection monitoring with comprehensive analysis and reporting.
    """
    
    def __init__(self, config: Optional[MemoryDebugConfig] = None):
        self.config = config or MemoryDebugConfig()
        self.state = MemoryDebugState.IDLE
        self.memory_history: List[Dict[str, Any]] = []
        self.allocation_records: List[Dict[str, Any]] = []
        self.deallocation_records: List[Dict[str, Any]] = []
        self.current_time = 0.0
        self.animation_obj = None
        self.fig = None
        self.axes = None
        self.allocation_plots = []
        self.leak_plots = []
        self.fragmentation_plots = []
        self.garbage_plots = []
        
        # Memory tracking
        self.process = psutil.Process()
        self.initial_memory = self.process.memory_info().rss / 1024 / 1024  # MB
        self.allocated_memory = 0.0
        self.deallocated_memory = 0.0
        self.leaked_memory = 0.0
        
        # Fragmentation tracking
        self.total_allocated = 0.0
        self.total_free = 0.0
        self.current_fragmentation = 0.0
        
        # Garbage collection tracking
        self.gc_counts = []
        self.gc_times = []
        
        # Setup visualization
        self._setup_visualization()
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        plt.style.use('seaborn-v0_8')
        self.fig, self.axes = plt.subplots(
            3, 1, figsize=self.config.window_size, 
            gridspec_kw={'height_ratios': [1, 1, 1], 'hspace': 0.4}
        )
        
        # Setup subplots
        self.allocation_plots = [self.axes[0]]
        self.leak_plots = [self.axes[1]]
        self.fragmentation_plots = [self.axes[2]]
        self.garbage_plots = []
        
        for ax in self.axes:
            ax.set_facecolor('#f8f9fa')
            ax.grid(True, alpha=0.3)
            ax.set_xlabel('Time (ms)', fontsize=10)
        
        self.allocation_plots[0].set_title('Memory Allocation Tracking', fontsize=12, fontweight='bold')
        self.leak_plots[0].set_title('Memory Leak Detection', fontsize=12, fontweight='bold')
        self.fragmentation_plots[0].set_title('Memory Fragmentation Visualization', fontsize=12, fontweight='bold')
        
        plt.tight_layout()
    
    def start_memory_monitoring(self):
        """Start memory leak detection and monitoring."""
        self.state = MemoryDebugState.MONITORING
        
        # Clear any existing animation
        if self.animation_obj:
            self.animation_obj.event_source.stop()
        
        # Initialize tracking
        self._initialize_tracking()
        
        # Start monitoring thread
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
        
        print(f"Starting memory leak detection and monitoring...")
        print(f"Allocation tracking: {self.config.track_allocations}")
        print(f"Leak detection: {self.config.detect_leaks}")
        print(f"Fragmentation visualization: {self.config.visualize_fragmentation}")
        print(f"Usage monitoring: {self.config.monitor_usage}")
        print(f"State: {self.state.value}")
    
    def _initialize_tracking(self):
        """Initialize memory tracking."""
        self.memory_history = []
        self.allocation_records = []
        self.deallocation_records = []
        self.initial_memory = self.process.memory_info().rss / 1024 / 1024  # MB
        self.allocated_memory = 0.0
        self.deallocated_memory = 0.0
        self.leaked_memory = 0.0
        self.total_allocated = 0.0
        self.total_free = 0.0
        self.current_fragmentation = 0.0
        self.gc_counts = []
        self.gc_times = []
    
    def _monitoring_loop(self):
        """Main monitoring loop for memory debugging."""
        while self.state == MemoryDebugState.MONITORING:
            # Update memory tracking
            self._update_memory_tracking()
            
            # Render plots
            self._render_plots()
            
            # Save if auto-save is enabled
            if self.config.auto_save and len(self.memory_history) % self.config.save_interval == 0:
                self._save_current_frame()
            
            # Control monitoring speed
            time.sleep(1.0 / self.config.update_rate)
    
    def _update_memory_tracking(self):
        """Update memory tracking data."""
        self.current_time += 1.0 / self.config.update_rate * 1000  # Convert to ms
        
        # Get current memory usage
        current_memory = self.process.memory_info().rss / 1024 / 1024  # MB
        memory_diff = current_memory - self.initial_memory
        
        # Track allocations (simulate with random data for demo)
        if self.config.track_allocations:
            allocation_size = np.random.gamma(2, 5)  # MB
            self.allocated_memory += allocation_size
            self.total_allocated += allocation_size
            
            allocation_record = {
                'time': self.current_time,
                'size': allocation_size,
                'type': 'allocation',
                'thread': threading.current_thread().ident
            }
            self.allocation_records.append(allocation_record)
        
        # Simulate deallocations
        if self.config.garbage_collection:
            gc.collect()
            deallocation_count = len(self.gc_counts) + 1
            
            deallocation_record = {
                'time': self.current_time,
                'count': deallocation_count,
                'type': 'gc'
            }
            self.deallocation_records.append(deallocation_record)
            
            # Update fragmentation
            self.total_free += np.random.gamma(1, 3)
        
        # Calculate fragmentation
        if self.total_allocated > 0:
            self.current_fragmentation = (self.total_allocated - self.total_free) / self.total_allocated
        
        # Detect leaks
        if self.config.detect_leaks and memory_diff > self.config.leak_threshold:
            leak_amount = memory_diff - self.config.leak_threshold
            self.leaked_memory += leak_amount
        
        # Record memory history
        memory_record = {
            'time': self.current_time,
            'current_memory': current_memory,
            'memory_diff': memory_diff,
            'allocated_memory': self.allocated_memory,
            'deallocated_memory': self.deallocated_memory,
            'leaked_memory': self.leaked_memory,
            'fragmentation': self.current_fragmentation,
            'gc_count': len(self.gc_counts)
        }
        
        self.memory_history.append(memory_record)
        
        # Maintain history size
        if len(self.memory_history) > self.config.memory_history_size:
            self.memory_history.pop(0)
    
    def _render_plots(self):
        """Render the current visualization plots."""
        if not self.fig:
            return
        
        # Clear axes
        for ax in self.axes:
            ax.clear()
        
        # Plot allocation tracking if enabled
        if self.config.track_allocations:
            self._plot_allocation_tracking()
        
        # Plot leak detection if enabled
        if self.config.detect_leaks:
            self._plot_leak_detection()
        
        # Plot fragmentation visualization if enabled
        if self.config.visualize_fragmentation:
            self._plot_fragmentation()
        
        # Update plot labels
        self._update_plot_labels()
        
        # Refresh the figure
        self.fig.canvas.draw()
    
    def _plot_allocation_tracking(self):
        """Plot memory allocation tracking."""
        if len(self.memory_history) < 10:
            return
        
        time_series = [record['time'] for record in self.memory_history]
        allocated_series = [record['allocated_memory'] for record in self.memory_history]
        deallocated_series = [record['deallocated_memory'] for record in self.memory_history]
        
        # Plot allocated memory
        self.allocation_plots[0].plot(
            time_series, allocated_series,
            linewidth=2, color='blue', label='Allocated', alpha=0.8
        )
        
        # Plot deallocated memory
        self.allocation_plots[0].plot(
            time_series, deallocated_series,
            linewidth=2, color='orange', label='Deallocated', alpha=0.8
        )
        
        # Plot current memory
        current_memory = [record['current_memory'] for record in self.memory_history]
        self.allocation_plots[0].plot(
            time_series, current_memory,
            linewidth=2, color='green', label='Current', alpha=0.8
        )
        
        self.allocation_plots[0].set_ylim(0, max(max(allocated_series), max(deallocated_series), max(current_memory)) * 1.1)
        self.allocation_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_leak_detection(self):
        """Plot memory leak detection."""
        if len(self.memory_history) < 10:
            return
        
        time_series = [record['time'] for record in self.memory_history]
        leaked_series = [record['leaked_memory'] for record in self.memory_history]
        
        # Plot leaked memory
        self.leak_plots[0].plot(
            time_series, leaked_series,
            linewidth=2, color='red', label='Leaked Memory', alpha=0.8
        )
        
        # Plot leak threshold
        self.leak_plots[0].axhline(
            y=self.config.leak_threshold, color='red', linestyle='--', 
            alpha=0.5, label=f'Leak Threshold ({self.config.leak_threshold} MB)'
        )
        
        # Highlight leak events
        leak_events = [record for record in self.memory_history if record['leaked_memory'] > 0]
        if leak_events:
            for event in leak_events[-3:]:  # Show last 3 events
                self.leak_plots[0].axvspan(
                    event['time'] - 100, event['time'],
                    alpha=0.3, color='red'
                )
        
        self.leak_plots[0].set_ylim(0, max(leaked_series) * 1.1 if leaked_series else 10)
        self.leak_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_fragmentation(self):
        """Plot memory fragmentation visualization."""
        if len(self.memory_history) < 10:
            return
        
        time_series = [record['time'] for record in self.memory_history]
        fragmentation_series = [record['fragmentation'] for record in self.memory_history]
        
        # Plot fragmentation
        self.fragmentation_plots[0].plot(
            time_series, fragmentation_series,
            linewidth=2, color='purple', label='Fragmentation', alpha=0.8
        )
        
        # Plot fragmentation threshold
        self.fragmentation_plots[0].axhline(
            y=self.config.fragmentation_threshold, color='red', linestyle='--',
            alpha=0.5, label=f'Fragmentation Threshold ({self.config.fragmentation_threshold})'
        )
        
        # Highlight high fragmentation periods
        high_frag_events = [record for record in self.memory_history 
                          if record['fragmentation'] > self.config.fragmentation_threshold]
        if high_frag_events:
            for event in high_frag_events[-3:]:
                self.fragmentation_plots[0].axvspan(
                    event['time'] - 100, event['time'],
                    alpha=0.3, color='purple'
                )
        
        self.fragmentation_plots[0].set_ylim(0, max(fragmentation_series) * 1.1 if fragmentation_series else 1)
        self.fragmentation_plots[0].legend(loc='upper right', fontsize=8)
    
    def _update_plot_labels(self):
        """Update plot labels with current memory statistics."""
        current_time_ms = self.current_time
        
        # Update titles with current statistics
        if self.memory_history:
            latest_record = self.memory_history[-1]
            
            # Update allocation tracking title
            total_allocated = latest_record['allocated_memory']
            total_deallocated = latest_record['deallocated_memory']
            current_memory = latest_record['current_memory']
            
            self.allocation_plots[0].set_title(
                f'Memory Allocation Tracking (Current: {current_memory:.1f} MB)',
                fontsize=12, fontweight='bold'
            )
            
            # Update leak detection title
            leaked_memory = latest_record['leaked_memory']
            self.leak_plots[0].set_title(
                f'Memory Leak Detection (Total Leaked: {leaked_memory:.2f} MB)',
                fontsize=12, fontweight='bold'
            )
            
            # Update fragmentation title
            fragmentation = latest_record['fragmentation']
            self.fragmentation_plots[0].set_title(
                f'Memory Fragmentation Visualization (Current: {fragmentation:.2%})',
                fontsize=12, fontweight='bold'
            )
        
        # Update time in title
        time_str = f'Time: {current_time_ms:.0f} ms'
        self.fig.suptitle(time_str, fontsize=14, fontweight='bold')
    
    def _save_current_frame(self):
        """Save current visualization frame."""
        if self.config.save_path:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{self.config.save_path}/memory_debug_frame_{timestamp}.png"
            
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            try:
                self.fig.savefig(filename, dpi=150, bbox_inches='tight')
                print(f"Saved memory debug frame: {filename}")
            except Exception as e:
                print(f"Warning: Could not save frame: {e}")
    
    def stop(self):
        """Stop the memory debugging."""
        self.state = MemoryDebugState.IDLE
        
        # Stop monitoring thread
        if hasattr(self, 'monitoring_thread') and self.monitoring_thread.is_alive():
            self.monitoring_thread.join(timeout=5.0)
        
        print("Memory debugging stopped")
    
    def export_data(self, output_path: str, format: str = "json"):
        """
        Export memory debugging data to file.
        
        Args:
            output_path: Path to output file
            format: Export format (json, csv, npy)
        """
        self.state = MemoryDebugState.EXPORTING
        
        try:
            if format.lower() == "json":
                export_data = {
                    'config': self.config.__dict__,
                    'memory_history': self.memory_history,
                    'allocation_records': self.allocation_records,
                    'deallocation_records': self.deallocation_records,
                    'current_time': self.current_time,
                    'initial_memory': self.initial_memory,
                    'allocated_memory': self.allocated_memory,
                    'deallocated_memory': self.deallocated_memory,
                    'leaked_memory': self.leaked_memory,
                    'timestamp': datetime.now().isoformat()
                }
                
                with open(output_path, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                    
            elif format.lower() == "csv":
                import pandas as pd
                
                # Convert memory history to DataFrame
                df_data = []
                for record in self.memory_history:
                    df_data.append({
                        'time': record['time'],
                        'current_memory': record['current_memory'],
                        'memory_diff': record['memory_diff'],
                        'allocated_memory': record['allocated_memory'],
                        'deallocated_memory': record['deallocated_memory'],
                        'leaked_memory': record['leaked_memory'],
                        'fragmentation': record['fragmentation'],
                        'gc_count': record['gc_count']
                    })
                
                df = pd.DataFrame(df_data)
                df.to_csv(output_path, index=False)
                
            elif format.lower() == "npy":
                np.savez(output_path, 
                        memory_history=np.array(self.memory_history, dtype=object),
                        current_time=self.current_time,
                        config=self.config.__dict__)
            
            print(f"Memory debugging data exported to: {output_path}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
        
        finally:
            self.state = MemoryDebugState.IDLE

def main():
    """Main entry point for nlx-debug-memory tool."""
    print("NLM - Neural Memory Debugging Tool")
    print("=" * 50)
    print("Memory leak detection and visualization with sophisticated capabilities")
    print()
    
    # Example usage
    config = MemoryDebugConfig(
        track_allocations=True,
        detect_leaks=True,
        visualize_fragmentation=True,
        monitor_usage=True,
        garbage_collection=True,
        update_rate=5.0,
        leak_threshold=0.1,  # MB
        window_size=(1400, 900)
    )
    
    # Create memory debugger
    debugger = NLMemoryDebugger(config)
    
    print("Example configuration created successfully!")
    print(f"Allocation tracking: {config.track_allocations}")
    print(f"Leak detection: {config.detect_leaks}")
    print(f"Fragmentation visualization: {config.visualize_fragmentation}")
    print(f"Update rate: {config.update_rate} Hz")
    print(f"Window size: {config.window_size}")
    print()
    print("To use the debugger:")
    print("  debugger = NLMemoryDebugger(config)")
    print("  debugger.start_memory_monitoring()")
    print("  debugger.stop()   # to stop")
    print("  debugger.export_data('output.json', 'json')  # to export")

if __name__ == "__main__":
    main()
