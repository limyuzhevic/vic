pynlm/visualization.py
Name: Visualization module
Summary: Provides visualization and plotting capabilities for brain data

from pynlm.interface import ScriptingInterface
import time
import threading
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass
from enum import Enum


class VisualizationBackend(Enum):
    """Visualization backend enumeration"""
    MATPLOTLIB = "matplotlib"
    PLOTLY = "plotly"
    Bokeh = "bokeh"
    MAYAVI = "mayavi"
    NONE = "none"


class PlotManager:
    """Manage plotting operations for brain data visualization"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.backend = VisualizationBackend.MATPLOTLIB
        self.plot_cache = {}
        self.update_listeners = []
        self.real_time_plots = {}
        self.figure_cache = {}
    
    def set_backend(self, backend: VisualizationBackend):
        """Set visualization backend"""
        self.backend = backend
    
    def create_neuron_activity_plot(self, neurons: List[Dict[str, Any]], 
                                   save_path: Optional[str] = None) -> Any:
        """Create neuron activity plot"""
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            if not neurons:
                return None
            
            # Extract neuron data
            neuron_ids = [n.get('id', i) for i, n in enumerate(neurons)]
            activities = [n.get('activity', n.get('potential', 0)) for n in neurons]
            types = [n.get('type', 'Internal') for n in neurons]
            firing_states = [n.get('firing_state', 'Resting') for n in neurons]
            
            # Create figure with subplots
            fig, axes = plt.subplots(2, 2, figsize=(15, 10))
            fig.suptitle('Neuron Activity Analysis', fontsize=16)
            
            # Plot 1: Activity bar chart
            colors = []
            for state in firing_states:
                if state == 'Active':
                    colors.append('red')
                elif state == 'Firing':
                    colors.append('orange')
                elif state == 'Refractory':
                    colors.append('yellow')
                else:
                    colors.append('blue')
            
            axes[0, 0].bar(neuron_ids, activities, color=colors, alpha=0.7)
            axes[0, 0].set_xlabel('Neuron ID')
            axes[0, 0].set_ylabel('Activity/Potential')
            axes[0, 0].set_title('Neuron Activity Distribution')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Activity histogram
            axes[0, 1].hist(activities, bins=20, alpha=0.7, color='green')
            axes[0, 1].set_xlabel('Activity Level')
            axes[0, 1].set_ylabel('Count')
            axes[0, 1].set_title('Activity Distribution')
            axes[0, 1].grid(True, alpha=0.3)
            
            # Plot 3: Neuron types pie chart
            type_counts = {}
            for neuron_type in types:
                type_counts[neuron_type] = type_counts.get(neuron_type, 0) + 1
            
            axes[1, 0].pie(type_counts.values(), labels=type_counts.keys(), autopct='%1.1f%%')
            axes[1, 0].set_title('Neuron Type Distribution')
            
            # Plot 4: Firing state timeline
            state_counts = {}
            for state in firing_states:
                state_counts[state] = state_counts.get(state, 0) + 1
            
            axes[1, 1].bar(range(len(state_counts)), list(state_counts.values()))
            axes[1, 1].set_xticks(range(len(state_counts)))
            axes[1, 1].set_xticklabels(list(state_counts.keys()))
            axes[1, 1].set_xlabel('Firing State')
            axes[1, 1].set_ylabel('Count')
            axes[1, 1].set_title('Firing State Distribution')
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise ImportError("Matplotlib is required for visualization")
        except Exception as e:
            raise RuntimeError(f"Failed to create neuron activity plot: {e}")
    
    def create_region_comparison_plot(self, regions: List[Dict[str, Any]],
                                    save_path: Optional[str] = None) -> Any:
        """Create region comparison plot"""
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            if not regions:
                return None
            
            region_names = [r.get('id', f'Region_{i}') for i, r in enumerate(regions)]
            neuron_counts = [r.get('neuron_count', 0) for r in regions]
            active_counts = [r.get('active_count', 0) for r in regions]
            firing_counts = [r.get('firing_count', 0) for r in regions]
            
            # Create figure with subplots
            fig, axes = plt.subplots(2, 2, figsize=(15, 12))
            fig.suptitle('Brain Region Analysis', fontsize=16)
            
            # Plot 1: Neuron count bar chart
            x_pos = np.arange(len(region_names))
            width = 0.35
            
            axes[0, 0].bar(x_pos - width/2, neuron_counts, width, label='Total Neurons', alpha=0.8)
            axes[0, 0].bar(x_pos + width/2, active_counts, width, label='Active Neurons', alpha=0.8)
            axes[0, 0].bar(x_pos + width, firing_counts, width, label='Firing Neurons', alpha=0.8)
            
            axes[0, 0].set_xlabel('Regions')
            axes[0, 0].set_ylabel('Neuron Count')
            axes[0, 0].set_title('Neuron Count by Region')
            axes[0, 0].set_xticks(x_pos)
            axes[0, 0].set_xticklabels(region_names, rotation=45, ha='right')
            axes[0, 0].legend()
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Region activity radar chart
            # Create polar coordinates for radar plot
            angles = np.linspace(0, 2 * np.pi, len(region_names), endpoint=False).tolist()
            angles += angles[:1]  # Close the loop
            
            # Normalize data for radar plot
            def normalize_data(data):
                max_val = max(data) if max(data) > 0 else 1
                return [d / max_val for d in data]
            
            for i, region in enumerate(regions):
                # Get metrics for radar plot
                metrics = [
                    region.get('neuron_count', 0) / max([r.get('neuron_count', 0) for r in regions]),
                    region.get('active_count', 0) / region.get('neuron_count', 1),
                    region.get('firing_count', 0) / region.get('neuron_count', 1),
                    len(region.get('neurons', [])) / 100,  # Normalize to 100 neurons
                    region.get('activity_level', 0.5)
                ]
                
                axes[1, 0].plot(angles, metrics + [metrics[0]], label=region_names[i], linewidth=2)
                axes[1, 0].fill(angles, metrics + [metrics[0]], alpha=0.25)
            
            axes[1, 0].set_xticks(angles[:-1])
            axes[1, 0].set_xticklabels(region_names)
            axes[1, 0].set_title('Region Activity Comparison')
            axes[1, 0].legend()
            axes[1, 0].grid(True)
            
            # Plot 3: Activity heatmap
            # Create a matrix representing activity across regions and metrics
            activity_matrix = []
            for region in regions:
                row = []
                for metric in ['neuron_count', 'active_count', 'firing_count']:
                    max_val = max([r.get(metric, 0) for r in regions])
                    row.append(region.get(metric, 0) / max_val if max_val > 0 else 0)
                activity_matrix.append(row)
            
            if activity_matrix:
                im = axes[1, 1].imshow(activity_matrix, cmap='RdYlGn', aspect='auto', vmin=0, vmax=1)
                axes[1, 1].set_xticks(range(len(['neuron_count', 'active_count', 'firing_count'])))
                axes[1, 1].set_xticklabels(['Total', 'Active', 'Firing'])
                axes[1, 1].set_yticks(range(len(region_names)))
                axes[1, 1].set_yticklabels(region_names)
                axes[1, 1].set_title('Activity Heatmap')
                plt.colorbar(im, ax=axes[1, 1])
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise ImportError("Matplotlib is required for visualization")
        except Exception as e:
            raise RuntimeError(f"Failed to create region comparison plot: {e}")
    
    def create_firing_rate_plot(self, times: List[float], rates: List[float],
                                save_path: Optional[str] = None) -> Any:
        """Create firing rate over time plot"""
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            fig, ax = plt.subplots(figsize=(12, 8))
            
            # Main firing rate plot
            ax.plot(times, rates, linewidth=2, label='Firing Rate', color='red')
            
            # Add moving average
            if len(rates) > 10:
                window = min(10, len(rates) // 4)
                moving_avg = np.convolve(rates, np.ones(window)/window, mode='valid')
                time_offset = window // 2
                smoothed_times = times[time_offset:time_offset + len(moving_avg)]
                ax.plot(smoothed_times, moving_avg, linewidth=2, 
                       label=f'Moving Average ({window} steps)', color='blue', alpha=0.8)
            
            # Add threshold lines
            avg_rate = np.mean(rates)
            std_rate = np.std(rates)
            
            ax.axhline(y=avg_rate, color='green', linestyle='--', alpha=0.7, label=f'Average ({avg_rate:.2f})')
            ax.axhline(y=avg_rate + std_rate, color='orange', linestyle=':', alpha=0.7, label=f'+1 std')
            ax.axhline(y=avg_rate - std_rate, color='orange', linestyle=':', alpha=0.7, label='-1 std')
            
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Firing Rate')
            ax.set_title('Firing Rate Over Time')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            # Add inset plot for statistics
            inset = ax.inset_axes([0.65, 0.65, 0.3, 0.25])
            inset.hist(rates, bins=20, alpha=0.7, color='gray')
            inset.set_xlabel('Firing Rate')
            inset.set_ylabel('Frequency')
            inset.set_title('Distribution')
            inset.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise ImportError("Matplotlib is required for visualization")
        except Exception as e:
            raise RuntimeError(f"Failed to create firing rate plot: {e}")
    
    def create_connectivity_matrix(self, adjacency_matrix: Dict[str, Any],
                                  save_path: Optional[str] = None) -> Any:
        """Create connectivity matrix visualization"""
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Convert adjacency matrix to numpy array
            if isinstance(adjacency_matrix, dict):
                # Handle dictionary format
                regions = list(adjacency_matrix.keys())
                size = len(regions)
                matrix = np.zeros((size, size))
                
                for i, region1 in enumerate(regions):
                    for j, region2 in enumerate(regions):
                        matrix[i, j] = adjacency_matrix[region1].get(region2, 0)
            else:
                matrix = np.array(adjacency_matrix)
            
            fig, axes = plt.subplots(2, 2, figsize=(15, 12))
            fig.suptitle('Connectivity Matrix Analysis', fontsize=16)
            
            # Plot 1: Full connectivity matrix
            im1 = axes[0, 0].imshow(matrix, cmap='hot', interpolation='nearest', vmin=0, vmax=1)
            axes[0, 0].set_title('Connectivity Matrix')
            axes[0, 0].set_xlabel('Target Region')
            axes[0, 0].set_ylabel('Source Region')
            plt.colorbar(im1, ax=axes[0, 0])
            
            # Plot 2: Sorted connectivity
            sorted_matrix = np.sort(matrix, axis=1)
            im2 = axes[0, 1].imshow(sorted_matrix, cmap='hot', interpolation='nearest', vmin=0, vmax=1)
            axes[0, 1].set_title('Sorted Connectivity (by column)')
            axes[0, 1].set_xlabel('Target Region (sorted)')
            axes[0, 1].set_ylabel('Source Region')
            plt.colorbar(im2, ax=axes[0, 1])
            
            # Plot 3: Connectivity statistics
            connectivity_stats = {
                'total_connections': np.sum(matrix),
                'avg_strength': np.mean(matrix),
                'max_strength': np.max(matrix),
                'min_strength': np.min(matrix),
                'std_strength': np.std(matrix),
                'density': np.sum(matrix > 0) / matrix.size
            }
            
            axes[1, 0].bar(connectivity_stats.keys(), connectivity_stats.values(), alpha=0.7)
            axes[1, 0].set_title('Connectivity Statistics')
            axes[1, 0].set_ylabel('Value')
            axes[1, 0].tick_params(axis='x', rotation=45)
            axes[1, 0].grid(True, alpha=0.3)
            
            # Plot 4: Strength distribution
            flat_matrix = matrix.flatten()
            axes[1, 1].hist(flat_matrix[flat_matrix > 0], bins=20, alpha=0.7, color='blue')
            axes[1, 1].set_xlabel('Connection Strength')
            axes[1, 1].set_ylabel('Frequency')
            axes[1, 1].set_title('Connection Strength Distribution')
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise ImportError("Matplotlib is required for visualization")
        except Exception as e:
            raise RuntimeError(f"Failed to create connectivity matrix: {e}")
    
    def create_real_time_plot(self, plot_id: str, data_provider: Callable,
                            update_interval: float = 1.0) -> Any:
        """Create real-time updating plot"""
        if self.backend == VisualizationBackend.NONE:
            return None
        
        # Create real-time plot
        self.real_time_plots[plot_id] = {
            'data_provider': data_provider,
            'update_interval': update_interval,
            'last_update': time.time(),
            'plot': None
        }
        
        # Start real-time updater thread
        updater_thread = threading.Thread(
            target=self._real_time_updater,
            args=(plot_id,),
            daemon=True
        )
        updater_thread.start()
        
        return f"Real-time plot {plot_id} started"
    
    def _real_time_updater(self, plot_id: str):
        """Internal real-time plot updater"""
        plot_info = self.real_time_plots.get(plot_id)
        if not plot_info:
            return
        
        try:
            while plot_id in self.real_time_plots:
                if time.time() - plot_info['last_update'] >= plot_info['update_interval']:
                    data = plot_info['data_provider']()
                    
                    # Update plot data
                    if plot_info['plot']:
                        # In a real implementation, this would update the plot
                        pass
                    
                    plot_info['last_update'] = time.time()
                    
                    # Notify listeners
                    for listener in self.update_listeners:
                        try:
                            listener(plot_id, data)
                        except Exception:
                            pass
                
                time.sleep(0.1)  # Small delay to prevent busy waiting
                
        except Exception as e:
            logger = __import__('logging').getLogger(__name__)
            logger.error(f"Real-time updater error for {plot_id}: {e}")
    
    def add_update_listener(self, listener: Callable):
        """Add listener for plot updates"""
        self.update_listeners.append(listener)
    
    def remove_update_listener(self, listener: Callable):
        """Remove update listener"""
        if listener in self.update_listeners:
            self.update_listeners.remove(listener)
    
    def export_plot(self, plot_id: str, filepath: str, format: str = 'png') -> bool:
        """Export plot to file"""
        try:
            if plot_id in self.plot_cache:
                fig = self.plot_cache[plot_id]
                
                if format.lower() == 'png':
                    fig.savefig(filepath, dpi=300, bbox_inches='tight')
                elif format.lower() == 'pdf':
                    fig.savefig(filepath, format='pdf', bbox_inches='tight')
                elif format.lower() == 'svg':
                    fig.savefig(filepath, format='svg', bbox_inches='tight')
                else:
                    raise ValueError(f"Unsupported format: {format}")
                
                logger = __import__('logging').getLogger(__name__)
                logger.info(f"Plot exported: {filepath}")
                
                return True
            else:
                raise ValueError(f"Plot not found: {plot_id}")
                
        except Exception as e:
            logger = __import__('logging').getLogger(__name__)
            logger.error(f"Failed to export plot: {e}")
            return False
    
    def cache_plot(self, plot_id: str, fig: Any):
        """Cache plot for later retrieval"""
        self.plot_cache[plot_id] = fig


class InteractivePlot:
    """Interactive plotting with mouse/keyboard events"""
    
    def __init__(self, fig: Any):
        self.fig = fig
        self.event_handlers = {}
        self.annotations = []
    
    def on_click(self, func: Callable):
        """Register click event handler"""
        self.event_handlers['click'] = func
    
    def on_hover(self, func: Callable):
        """Register hover event handler"""
        self.event_handlers['hover'] = func
    
    def on_key(self, func: Callable):
        """Register key event handler"""
        self.event_handlers['key'] = func
    
    def add_annotation(self, x: float, y: float, text: str, 
                      color: str = 'red', size: int = 12) -> Any:
        """Add annotation to plot"""
        try:
            import matplotlib.pyplot as plt
            
            annotation = self.fig.annotate(text, (x, y), 
                                          xytext=(5, 5), textcoords='offset points',
                                          color=color, fontsize=size,
                                          arrowprops=dict(arrowstyle='->'))
            self.annotations.append(annotation)
            return annotation
        except ImportError:
            raise ImportError("Matplotlib is required for annotations")


class VisualizationDashboard:
    """Comprehensive visualization dashboard"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.widgets = []
        self.layout = None
    
    def create_dashboard(self, dashboard_type: str = 'console') -> Any:
        """Create visualization dashboard"""
        if dashboard_type == 'console':
            return ConsoleDashboard(self.interface)
        elif dashboard_type == 'web':
            return WebDashboard(self.interface)
        else:
            raise ValueError(f"Unsupported dashboard type: {dashboard_type}")
    
    def add_widget(self, widget: Any):
        """Add widget to dashboard"""
        self.widgets.append(widget)


class ConsoleDashboard:
    """Console-based visualization dashboard"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.update_interval = 1.0
    
    def start(self, update_interval: float = 1.0):
        """Start console dashboard"""
        self.update_interval = update_interval
        
        while True:
            try:
                self._display_dashboard()
                time.sleep(update_interval)
            except KeyboardInterrupt:
                break
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Dashboard error: {e}")
                time.sleep(update_interval)
    
    def _display_dashboard(self):
        """Display dashboard in console"""
        print("\n" + "="*60)
        print("NLM BRAIN VISUALIZATION DASHBOARD")
        print("="*60)
        
        # System status
        print(f"\nSYSTEM STATUS:")
        print(f"  Mode: {self.interface.context.mode}")
        print(f"  Running: {self.interface.context.running}")
        
        # Brain state
        if self.interface.context.brain:
            state = self.interface.get_state()
            print(f"\nNEURAL NETWORK:")
            print(f"  Neurons: {state['total_neurons']}")
            print(f"  Synapses: {state['total_synapses']}")
            print(f"  Active: {state['active_neurons']}/{state['firing_neurons']}")
            print(f"  Firing Rate: {state['average_firing_rate']:.2f}")
            print(f"  Development: {state['developmental_stage']}")
        
        # Region information
        if self.interface.context.brain:
            regions = self.interface.context.brain.getRegions()
            if regions:
                print(f"\nNEURAL REGIONS ({len(regions)}):")
                for region in regions:
                    region_state = region.getState()
                    print(f"  Region {region.getId()}: {region_state.get('neuron_count', 0)} neurons, "
                          f"{region_state.get('active_count', 0)} active")
        
        # Resource usage
        resource_usage = self.interface.get_resource_usage()
        print(f"\nRESOURCE USAGE:")
        print(f"  Memory: {resource_usage.get('memory_mb', 0):.1f} MB")
        print(f"  CPU: {resource_usage.get('cpu_percent', 0):.1f}%")
        print(f"  Threads: {resource_usage.get('thread_count', 0)}")
        
        # Recent events
        print(f"\nRECENT ACTIVITY:")
        # In a real implementation, this would show recent events
        
        print("\n" + "="*60)
        print(f"Last updated: {time.strftime('%H:%M:%S')}")
        print("Press Ctrl+C to exit")
        print("="*60)


class WebDashboard:
    """Web-based visualization dashboard"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.server_url = "http://localhost:8080"
        self.dashboard_data = {}
    
    def start(self, port: int = 8080):
        """Start web dashboard server"""
        # In a real implementation, this would start a web server
        # For this example, we'll just return the URL
        self.server_url = f"http://localhost:{port}"
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Starting web dashboard on port {port}")
        
        return self.server_url
    
    def add_chart(self, chart_id: str, chart_type: str, data: Dict[str, Any]):
        """Add chart to web dashboard"""
        self.dashboard_data[chart_id] = {
            'type': chart_type,
            'data': data,
            'last_updated': time.time()
        }
    
    def update_chart(self, chart_id: str, new_data: Dict[str, Any]):
        """Update chart data"""
        if chart_id in self.dashboard_data:
            self.dashboard_data[chart_id]['data'] = new_data
            self.dashboard_data[chart_id]['last_updated'] = time.time()


# Export key functions and classes
__all__ = [
    'PlotManager',
    'VisualizationBackend',
    'InteractivePlot',
    'VisualizationDashboard',
    'ConsoleDashboard',
    'WebDashboard'
]
