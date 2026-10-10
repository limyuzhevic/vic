"""Enhanced Visualization APIs for Expert Users"""

from typing import Dict, List, Optional, Callable, Any, Union, Tuple
import time
import base64
from io import BytesIO
import math

# Optional numpy import
try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None

# Mock numpy functions for environments without numpy
if not NUMPY_AVAILABLE:
    class MockNumpy:
        def sin(self, x):
            return math.sin(x)
        
        def cos(self, x):
            return math.cos(x)
        
        def random(self):
            import random
            return random.random()
        
        def mean(self, data):
            return sum(data) / len(data) if data else 0
        
        def std(self, data):
            if not data or len(data) < 2:
                return 0.0
            mean = sum(data) / len(data)
            variance = sum((x - mean) ** 2 for x in data) / len(data)
            return variance ** 0.5
        
        def median(self, data):
            sorted_data = sorted(data)
            n = len(sorted_data)
            if n % 2 == 1:
                return sorted_data[n // 2]
            return (sorted_data[n // 2 - 1] + sorted_data[n // 2]) / 2
        
        def max(self, data):
            return max(data) if data else 0
        
        def min(self, data):
            return min(data) if data else 0
        
        def sum(self, data):
            return sum(data)
        
        def sqrt(self, x):
            return x ** 0.5
        
        def percentile(self, data, p):
            sorted_data = sorted(data)
            n = len(sorted_data)
            if n == 0:
                return 0
            index = (p / 100) * (n - 1)
            if index == int(index):
                return sorted_data[int(index)]
            return sorted_data[int(index)] + (sorted_data[int(index) + 1] - sorted_data[int(index)]) * (index - int(index))
    
    np = MockNumpy()

# Try to import matplotlib with fallback
try:
    import matplotlib
    matplotlib.use('Agg')  # Non-interactive backend
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    plt = None

class AdvancedPlotting:
    """Advanced plotting capabilities for expert users"""
    
    def __init__(self, plot_style: str = "scientific", output_format: str = "png"):
        self.plot_style = plot_style
        self.output_format = output_format
        self.plot_history = []
        self.plot_config = {
            'figure_size': (12, 8),
            'dpi': 150,
            'font_size': 10,
            'line_width': 2,
            'marker_size': 6
        }
    
    def create_neural_activity_plot(self, brain_data: Dict, time_series: bool = True,
                                   save_path: Optional[str] = None) -> Dict:
        """Create neural activity visualization"""
        plot_data = {
            'title': f"Neural Activity Analysis",
            'data': brain_data,
            'time_series': time_series,
            'timestamp': time.time()
        }
        
        # Create matplotlib visualization
        fig, axes = plt.subplots(2, 2, figsize=self.plot_config['figure_size'])
        fig.suptitle('Neural Activity Analysis', fontsize=16, fontweight='bold')
        
        # Plot 1: Neuron activity over time
        if time_series and 'activity_over_time' in brain_data:
            ax = axes[0, 0]
            ax.plot(brain_data['activity_over_time'])
            ax.set_title('Neuron Activity Over Time')
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Activity Level')
            ax.grid(True, alpha=0.3)
        
        # Plot 2: Neuron type distribution
        if 'neuron_types' in brain_data:
            ax = axes[0, 1]
            type_counts = brain_data['neuron_types']
            ax.pie(type_counts.values(), labels=type_counts.keys(), autopct='%1.1f%%')
            ax.set_title('Neuron Type Distribution')
        
        # Plot 3: Connection strength heatmap
        if 'connection_matrix' in brain_data:
            ax = axes[1, 0]
            im = ax.imshow(brain_data['connection_matrix'], cmap='viridis', aspect='auto')
            ax.set_title('Connection Strength Matrix')
            plt.colorbar(im, ax=ax)
        
        # Plot 4: Spike statistics
        if 'spike_stats' in brain_data:
            ax = axes[1, 1]
            stats = brain_data['spike_stats']
            ax.bar(stats.keys(), stats.values())
            ax.set_title('Spike Statistics')
            ax.set_ylabel('Count')
            ax.tick_params(axis='x', rotation=45)
        
        plt.tight_layout()
        
        # Save or encode plot
        if save_path:
            plt.savefig(save_path, dpi=self.plot_config['dpi'])
            plot_result = {'status': 'saved', 'path': save_path}
        else:
            # Convert to base64 for in-memory usage
            buffer = BytesIO()
            plt.savefig(buffer, format=self.output_format, dpi=self.plot_config['dpi'])
            buffer.seek(0)
            encoded = base64.b64encode(buffer.getvalue()).decode()
            plot_result = {'status': 'encoded', 'data': encoded, 'format': self.output_format}
        
        plt.close()
        
        self.plot_history.append({
            'type': 'neural_activity',
            'data': brain_data,
            'timestamp': time.time(),
            'result': plot_result
        })
        
        return plot_result
    
    def create_connection_density_plot(self, brain: Any, threshold: float = 0.1,
                                      save_path: Optional[str] = None) -> Dict:
        """Create connection density visualization"""
        plot_data = {
            'title': f"Connection Density Analysis (threshold: {threshold})",
            'brain': brain,
            'threshold': threshold,
            'timestamp': time.time()
        }
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=self.plot_config['figure_size'])
        
        # Create synthetic connection data for visualization
        if hasattr(brain, 'get_connections'):
            connections = brain.get_connections()
        else:
            # Generate synthetic data
            connections = np.random.rand(100, 100) * 0.5
        
        # Apply threshold
        binary_connections = (connections > threshold).astype(float)
        
        # Plot 1: Connection density heatmap
        im = ax1.imshow(binary_connections, cmap='hot', aspect='equal')
        ax1.set_title('Connection Density Matrix')
        plt.colorbar(im, ax=ax1)
        
        # Plot 2: Degree distribution
        degrees = np.sum(binary_connections, axis=1)
        ax2.hist(degrees, bins=20, alpha=0.7, edgecolor='black')
        ax2.set_title('Degree Distribution')
        ax2.set_xlabel('Degree')
        ax2.set_ylabel('Count')
        
        plt.tight_layout()
        
        # Save or encode
        if save_path:
            plt.savefig(save_path, dpi=self.plot_config['dpi'])
            plot_result = {'status': 'saved', 'path': save_path}
        else:
            buffer = BytesIO()
            plt.savefig(buffer, format=self.output_format, dpi=self.plot_config['dpi'])
            buffer.seek(0)
            encoded = base64.b64encode(buffer.getvalue()).decode()
            plot_result = {'status': 'encoded', 'data': encoded, 'format': self.output_format}
        
        plt.close()
        
        self.plot_history.append({
            'type': 'connection_density',
            'data': plot_data,
            'timestamp': time.time(),
            'result': plot_result
        })
        
        return plot_result
    
    def create_real_time_visualization(self, data_stream: Callable, update_interval: float = 1.0,
                                      max_points: int = 1000, save_path: Optional[str] = None) -> Dict:
        """Create real-time visualization"""
        real_time_data = {
            'title': 'Real-Time Neural Visualization',
            'data_stream': data_stream,
            'update_interval': update_interval,
            'max_points': max_points,
            'timestamp': time.time()
        }
        
        fig, ax = plt.subplots(figsize=self.plot_config['figure_size'])
        
        # Initialize empty plot
        line, = ax.plot([], [], 'b-', linewidth=self.plot_config['line_width'],
                       marker='o', markersize=self.plot_config['marker_size'])
        
        ax.set_title('Real-Time Neural Activity')
        ax.set_xlabel('Time')
        ax.set_ylabel('Activity')
        ax.grid(True, alpha=0.3)
        
        # Initialize data buffer
        x_data = []
        y_data = []
        
        # Simulate real-time data
        def update_plot(frame):
            # Get data from stream
            stream_data = data_stream()
            
            # Update data
            x_data.append(len(x_data))
            y_data.append(np.mean(stream_data) if stream_data else 0)
            
            # Limit data points
            if len(x_data) > max_points:
                x_data.pop(0)
                y_data.pop(0)
            
            # Update plot
            line.set_data(x_data, y_data)
            ax.relim()
            ax.autoscale_view()
        
        # In real implementation, this would use animation
        # For this simulation, we'll just create the plot structure
        
        # Save or encode
        if save_path:
            plt.savefig(save_path, dpi=self.plot_config['dpi'])
            plot_result = {'status': 'saved', 'path': save_path}
        else:
            buffer = BytesIO()
            plt.savefig(buffer, format=self.output_format, dpi=self.plot_config['dpi'])
            buffer.seek(0)
            encoded = base64.b64encode(buffer.getvalue()).decode()
            plot_result = {'status': 'encoded', 'data': encoded, 'format': self.output_format}
        
        plt.close()
        
        self.plot_history.append({
            'type': 'real_time',
            'data': real_time_data,
            'timestamp': time.time(),
            'result': plot_result
        })
        
        return plot_result
    
    def create_custom_visualization_pipeline(self, pipeline_steps: List[Callable],
                                           input_data: Any) -> Dict:
        """Create custom visualization pipeline"""
        pipeline_result = {
            'title': 'Custom Visualization Pipeline',
            'steps': len(pipeline_steps),
            'input_data': input_data,
            'timestamp': time.time()
        }
        
        current_data = input_data
        
        for i, step in enumerate(pipeline_steps):
            try:
                current_data = step(current_data)
                pipeline_result[f'step_{i}'] = {
                    'status': 'success',
                    'data_transformed': isinstance(current_data, dict),
                    'timestamp': time.time()
                }
            except Exception as e:
                pipeline_result[f'step_{i}'] = {
                    'status': 'error',
                    'error': str(e),
                    'timestamp': time.time()
                }
                break
        
        # Final visualization
        fig, ax = plt.subplots(figsize=self.plot_config['figure_size'])
        
        if isinstance(current_data, dict) and 'x' in current_data and 'y' in current_data:
            ax.plot(current_data['x'], current_data['y'], 'b-')
        
        ax.set_title('Visualization Pipeline Result')
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.grid(True, alpha=0.3)
        
        # Save or encode
        if pipeline_result.get('output_path'):
            plt.savefig(pipeline_result['output_path'], dpi=self.plot_config['dpi'])
            pipeline_result['plot_status'] = 'saved'
        else:
            buffer = BytesIO()
            plt.savefig(buffer, format=self.output_format, dpi=self.plot_config['dpi'])
            buffer.seek(0)
            encoded = base64.b64encode(buffer.getvalue()).decode()
            pipeline_result['plot_data'] = encoded
            pipeline_result['plot_format'] = self.output_format
            pipeline_result['plot_status'] = 'encoded'
        
        plt.close()
        
        self.plot_history.append({
            'type': 'custom_pipeline',
            'data': pipeline_result,
            'timestamp': time.time()
        })
        
        return pipeline_result
    
    def export_data_for_external_tools(self, data: Any, export_format: str = "csv",
                                      file_path: Optional[str] = None) -> Dict:
        """Export data for external tools"""
        export_result = {
            'format': export_format,
            'data_size': len(str(data)) if hasattr(data, '__len__') else 1,
            'timestamp': time.time()
        }
        
        if export_format == "csv":
            # Convert to CSV
            if isinstance(data, dict):
                import pandas as pd
                df = pd.DataFrame(data)
                csv_data = df.to_csv(index=False)
            elif isinstance(data, list):
                import pandas as pd
                df = pd.DataFrame(data)
                csv_data = df.to_csv(index=False)
            else:
                csv_data = str(data)
            
            export_result['data'] = csv_data
            
        elif export_format == "json":
            import json
            export_result['data'] = json.dumps(data, indent=2)
            
        elif export_format == "hdf5":
            try:
                import h5py
                if file_path:
                    with h5py.File(file_path, 'w') as f:
                        self._write_hdf5_recursive(f, data, '/')
                    export_result['status'] = 'saved'
                    export_result['path'] = file_path
                else:
                    export_result['error'] = 'File path required for HDF5 export'
            except ImportError:
                export_result['error'] = 'h5py not installed'
                
        elif export_format == "parquet":
            try:
                import pyarrow as pa
                import pyarrow.parquet as pq
                
                if isinstance(data, dict):
                    table = pa.table(data)
                elif isinstance(data, list):
                    table = pa.table(data)
                else:
                    export_result['error'] = 'Unsupported data format for Parquet'
                    return export_result
                
                if file_path:
                    pq.write_table(table, file_path)
                    export_result['status'] = 'saved'
                    export_result['path'] = file_path
                else:
                    export_result['error'] = 'File path required for Parquet export'
            except ImportError:
                export_result['error'] = 'pyarrow not installed'
        
        # Save file if path provided and format supported
        if file_path and export_format in ['csv', 'json'] and 'data' in export_result:
            with open(file_path, 'w') as f:
                f.write(export_result['data'])
            export_result['status'] = 'saved'
        
        self.plot_history.append({
            'type': 'export',
            'data': export_result,
            'timestamp': time.time()
        })
        
        return export_result
    
    def _write_hdf5_recursive(self, h5_group: Any, data: Any, path: str) -> None:
        """Recursively write data to HDF5"""
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, (dict, list)):
                    self._write_hdf5_recursive(h5_group.create_group(f"{path}_{key}"), value, f"{path}_{key}")
                else:
                    h5_group[key] = value
        elif isinstance(data, list):
            # Convert list to dataset
            h5_group[path] = np.array(data)
        else:
            # Scalar value
            h5_group[path] = data

class RealTimeVisualizer:
    """Real-time visualization for expert users"""
    
    def __init__(self, visualizer_id: str, update_callback: Callable,
                 output_callback: Callable, buffer_size: int = 100):
        self.visualizer_id = visualizer_id
        self.update_callback = update_callback
        self.output_callback = output_callback
        self.buffer_size = buffer_size
        self.data_buffer = []
        self.is_running = False
        self.last_update = 0.0
        self.visualization_stats = {
            'frames_processed': 0,
            'update_times': [],
            'memory_usage': 0
        }
    
    def start_visualization(self, initial_data: Optional[Any] = None) -> None:
        """Start real-time visualization"""
        self.is_running = True
        self.last_update = time.time()
        
        if initial_data:
            self.data_buffer.append({
                'timestamp': self.last_update,
                'data': initial_data,
                'frame_id': 0
            })
    
    def update_visualization(self, new_data: Any) -> Dict:
        """Update visualization with new data"""
        if not self.is_running:
            return {'error': 'Visualizer not running'}
        
        current_time = time.time()
        update_time = current_time - self.last_update
        
        # Add to buffer
        self.data_buffer.append({
            'timestamp': current_time,
            'data': new_data,
            'frame_id': len(self.data_buffer) - 1
        })
        
        # Maintain buffer size
        if len(self.data_buffer) > self.buffer_size:
            self.data_buffer.pop(0)
        
        # Record update time
        self.visualization_stats['update_times'].append(update_time)
        self.visualization_stats['frames_processed'] += 1
        
        # Call update callback
        update_result = self.update_callback(new_data, current_time)
        
        # Call output callback
        output_result = self.output_callback(self.data_buffer)
        
        self.last_update = current_time
        
        return {
            'status': 'success',
            'update_time': update_time,
            'buffer_size': len(self.data_buffer),
            'update_result': update_result,
            'output_result': output_result,
            'stats': self.get_visualization_stats()
        }
    
    def stop_visualization(self) -> Dict:
        """Stop real-time visualization"""
        self.is_running = False
        
        return {
            'status': 'stopped',
            'total_frames': len(self.data_buffer),
            'total_update_time': sum(self.visualization_stats['update_times']),
            'average_update_time': (
                sum(self.visualization_stats['update_times']) / len(self.visualization_stats['update_times'])
                if self.visualization_stats['update_times'] else 0
            ),
            'final_stats': self.get_visualization_stats()
        }
    
    def get_visualization_stats(self) -> Dict:
        """Get visualization statistics"""
        update_times = self.visualization_stats['update_times']
        
        return {
            'frames_processed': self.visualization_stats['frames_processed'],
            'current_buffer_size': len(self.data_buffer),
            'average_update_time': sum(update_times) / len(update_times) if update_times else 0,
            'min_update_time': min(update_times) if update_times else 0,
            'max_update_time': max(update_times) if update_times else 0,
            'is_running': self.is_running,
            'visualizer_id': self.visualizer_id
        }
    
    def get_recent_data(self, num_frames: int = 10) -> List[Dict]:
        """Get recent visualization data"""
        return self.data_buffer[-num_frames:]

class VisualizationPipeline:
    """Custom visualization pipelines for expert users"""
    
    def __init__(self, pipeline_id: str):
        self.pipeline_id = pipeline_id
        self.pipeline_steps = []
        self.pipeline_config = {
            'input_format': 'auto',
            'output_format': 'auto',
            'quality': 'high',
            'parallel_processing': False,
            'caching': True
        }
        self.pipeline_stats = {
            'total_executions': 0,
            'successful_executions': 0,
            'failed_executions': 0,
            'average_execution_time': 0.0
        }
        self.pipeline_history = []
    
    def add_step(self, step_name: str, step_function: Callable,
                step_config: Optional[Dict] = None) -> None:
        """Add processing step to pipeline"""
        self.pipeline_steps.append({
            'name': step_name,
            'function': step_function,
            'config': step_config or {},
            'order': len(self.pipeline_steps)
        })
    
    def configure(self, config: Dict) -> None:
        """Configure pipeline"""
        for key, value in config.items():
            if key in self.pipeline_config:
                self.pipeline_config[key] = value
    
    def execute(self, input_data: Any) -> Dict:
        """Execute pipeline"""
        start_time = time.time()
        execution_result = {
            'pipeline_id': self.pipeline_id,
            'input_data': input_data,
            'steps_executed': 0,
            'status': 'success',
            'output_data': None,
            'execution_time': 0.0,
            'step_results': {}
        }
        
        try:
            current_data = input_data
            
            for step in self.pipeline_steps:
                step_start = time.time()
                step_result = {
                    'step_name': step['name'],
                    'order': step['order'],
                    'status': 'success'
                }
                
                try:
                    current_data = step['function'](current_data, **step['config'])
                    step_result['execution_time'] = time.time() - step_start
                    step_result['output_type'] = type(current_data).__name__
                    
                except Exception as e:
                    step_result['status'] = 'error'
                    step_result['error'] = str(e)
                    step_result['execution_time'] = time.time() - step_start
                    execution_result['status'] = 'error'
                    execution_result['step_results'][step['name']] = step_result
                    break
                
                execution_result['step_results'][step['name']] = step_result
                execution_result['steps_executed'] += 1
            
            execution_result['output_data'] = current_data
            execution_result['execution_time'] = time.time() - start_time
            
            # Update statistics
            self.pipeline_stats['total_executions'] += 1
            if execution_result['status'] == 'success':
                self.pipeline_stats['successful_executions'] += 1
            else:
                self.pipeline_stats['failed_executions'] += 1
            
            execution_time = execution_result['execution_time']
            total_time = self.pipeline_stats['average_execution_time'] * (
                self.pipeline_stats['total_executions'] - 1
            ) + execution_time
            self.pipeline_stats['average_execution_time'] = total_time / self.pipeline_stats['total_executions']
            
            # Record in history
            self.pipeline_history.append({
                'timestamp': time.time(),
                'result': execution_result,
                'execution_time': execution_result['execution_time']
            })
            
        except Exception as e:
            execution_result['status'] = 'error'
            execution_result['error'] = str(e)
            execution_result['execution_time'] = time.time() - start_time
            self.pipeline_stats['failed_executions'] += 1
        
        return execution_result
    
    def get_pipeline_stats(self) -> Dict:
        """Get pipeline statistics"""
        return self.pipeline_stats
    
    def get_pipeline_history(self, limit: int = 10) -> List[Dict]:
        """Get pipeline execution history"""
        return self.pipeline_history[-limit:]

class DataExporter:
    """Data export for external tools for expert users"""
    
    def __init__(self, export_config: Optional[Dict] = None):
        self.export_config = export_config or {
            'formats': ['json', 'csv', 'parquet'],
            'compression': 'gzip',
            'include_metadata': True,
            'quality_threshold': 0.95
        }
        self.export_history = []
        self.export_stats = {
            'total_exports': 0,
            'successful_exports': 0,
            'failed_exports': 0,
            'exported_data_sizes': []
        }
    
    def export_data(self, data: Any, export_format: str = "json",
                   file_path: Optional[str] = None, options: Optional[Dict] = None) -> Dict:
        """Export data in specified format"""
        export_options = {**self.export_config, **(options or {})}
        
        export_result = {
            'format': export_format,
            'data_size': len(str(data)) if hasattr(data, '__len__') else 1,
            'file_path': file_path,
            'options': export_options,
            'timestamp': time.time()
        }
        
        try:
            if export_format == "json":
                import json
                export_data = json.dumps(data, indent=2)
                if file_path:
                    with open(file_path, 'w') as f:
                        f.write(export_data)
                export_result['data'] = export_data
                
            elif export_format == "csv":
                import pandas as pd
                if isinstance(data, dict):
                    df = pd.DataFrame(data)
                elif isinstance(data, list):
                    df = pd.DataFrame(data)
                else:
                    export_result['error'] = 'Unsupported data format for CSV export'
                    return export_result
                
                csv_data = df.to_csv(index=False)
                export_result['data'] = csv_data
                
                if file_path:
                    df.to_csv(file_path, index=False)
                    
            elif export_format == "parquet":
                try:
                    import pyarrow as pa
                    import pyarrow.parquet as pq
                    
                    if isinstance(data, dict):
                        table = pa.table(data)
                    elif isinstance(data, list):
                        table = pa.table(data)
                    else:
                        export_result['error'] = 'Unsupported data format for Parquet export'
                        return export_result
                    
                    if file_path:
                        pq.write_table(table, file_path)
                        export_result['status'] = 'saved'
                    
                    # Convert to bytes for memory usage
                    buffer = BytesIO()
                    pq.write_table(table, buffer)
                    export_result['data'] = base64.b64encode(buffer.getvalue()).decode()
                    
                except ImportError:
                    export_result['error'] = 'pyarrow not installed for Parquet export'
                    
            elif export_format == "hdf5":
                try:
                    import h5py
                    
                    if file_path:
                        with h5py.File(file_path, 'w') as f:
                            self._write_hdf5_recursive(f, data, '/')
                        export_result['status'] = 'saved'
                        
                except ImportError:
                    export_result['error'] = 'h5py not installed for HDF5 export'
            
            export_result['status'] = 'success'
            self.export_stats['total_exports'] += 1
            self.export_stats['successful_exports'] += 1
            self.export_stats['exported_data_sizes'].append(export_result['data_size'])
            
        except Exception as e:
            export_result['status'] = 'error'
            export_result['error'] = str(e)
            self.export_stats['total_exports'] += 1
            self.export_stats['failed_exports'] += 1
        
        self.export_history.append({
            'format': export_format,
            'result': export_result,
            'timestamp': time.time()
        })
        
        return export_result
    
    def _write_hdf5_recursive(self, h5_group: Any, data: Any, path: str) -> None:
        """Recursively write data to HDF5"""
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, (dict, list)):
                    self._write_hdf5_recursive(h5_group.create_group(f"{path}_{key}"), value, f"{path}_{key}")
                else:
                    h5_group[key] = value
        elif isinstance(data, list):
            h5_group[path] = np.array(data)
        else:
            h5_group[path] = data
    
    def get_export_stats(self) -> Dict:
        """Get export statistics"""
        return self.export_stats
    
    def get_export_history(self, limit: int = 10) -> List[Dict]:
        """Get export history"""
        return self.export_history[-limit:]

# Expert visualization exports
__all__ = [
    'AdvancedPlotting', 'RealTimeVisualizer', 'VisualizationPipeline',
    'DataExporter'
]
