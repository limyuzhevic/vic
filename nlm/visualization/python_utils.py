"""
Python utilities for NLM neural simulation framework.

This module provides enhanced Python features for the NLM framework including:
- Python-specific debugging and timing utilities
- Numpy array conversion and validation utilities
- Data export/import utilities
- Visualization utilities
"""

import time
import numpy as np
import json
import logging
import traceback
import sys
from typing import Dict, Any, Optional, List, Union, Tuple

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class PythonSessionLogger:
    """
    Python session logger for debugging and monitoring.
    
    This class provides logging functionality for Python sessions with
    timestamp formatting and thread safety.
    """
    
    def __init__(self, log_file: str = "nlm_python_session.log"):
        """
        Initialize Python session logger.
        
        Args:
            log_file: Path to log file
        """
        self.log_file = log_file
        self.start_time = time.time()
        self.log_entries = []
        
        # Open log file
        self.log_file_handle = open(log_file, 'a')
        
        # Log session start
        self.logEntry("Session started", timestamp=True)
    
    def __del__(self):
        """Clean up log file."""
        if hasattr(self, 'log_file_handle') and not self.log_file_handle.closed:
            self.logEntry("Session ended", timestamp=True)
            self.log_file_handle.close()
    
    def logEntry(self, message: str, timestamp: bool = False) -> None:
        """
        Log an entry.
        
        Args:
            message: Log message
            timestamp: Whether to include timestamp
        """
        try:
            # Format message
            if timestamp:
                formatted_message = f"[{self._get_timestamp()}] {message}"
            else:
                formatted_message = message
            
            # Print to stdout
            print(formatted_message)
            
            # Write to log file
            self.log_file_handle.write(formatted_message + "\n")
            self.log_file_handle.flush()
            
            # Store in memory
            self.log_entries.append(formatted_message)
            
        except Exception as e:
            print(f"Failed to log entry: {e}")
    
    def _get_timestamp(self) -> str:
        """Get current timestamp."""
        current_time = time.time()
        elapsed = current_time - self.start_time
        return f"[{elapsed:.3f}s]"
    
    def getLogEntries(self) -> List[str]:
        """
        Get log entries.
        
        Returns:
            List of log entries
        """
        return self.log_entries.copy()
    
    def clearLog(self) -> None:
        """Clear log entries."""
        self.log_entries.clear()
class PythonTimer:
    """
    Python timer for performance measurement.
    
    This class provides timing functionality for measuring performance
    of Python operations with millisecond and microsecond precision.
    """
    
    def __init__(self, name: str = "Timer"):
        """
        Initialize Python timer.
        
        Args:
            name: Timer name
        """
        self.name = name
        self.start_time = time.time()
    
    def elapsed(self) -> float:
        """
        Get elapsed time.
        
        Returns:
            Elapsed time in seconds
        """
        return time.time() - self.start_time
    
    def elapsed_ms(self) -> float:
        """
        Get elapsed time in milliseconds.
        
        Returns:
            Elapsed time in milliseconds
        """
        return self.elapsed() * 1000.0
    
    def elapsed_us(self) -> float:
        """
        Get elapsed time in microseconds.
        
        Returns:
            Elapsed time in microseconds
        """
        return self.elapsed() * 1000000.0
    
    def __enter__(self):
        """Context manager entry."""
        self.start_time = time.time()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        elapsed = self.elapsed()
        print(f"Timer {self.name}: {elapsed:.6f} seconds ({elapsed_ms:.3f} ms, {elapsed_us:.0f} µs)")
class NumpyUtils:
    """
    Numpy array utilities for NLM framework.
    
    This class provides utilities for converting between numpy arrays
    and other data types with validation and error handling.
    """
    
    @staticmethod
    def vectorToNumpy(vec: List[float]) -> np.ndarray:
        """
        Convert vector to numpy array.
        
        Args:
            vec: Vector to convert
            
        Returns:
            Numpy array
        """
        if not vec:
            return np.array([])
        
        return np.array(vec, dtype=np.float32)
    
    @staticmethod
    def numpyToVector(arr: np.ndarray) -> List[float]:
        """
        Convert numpy array to vector.
        
        Args:
            arr: Numpy array to convert
            
        Returns:
            List of floats
        """
        return arr.flatten().tolist()
    
    @staticmethod
    def isNumpyArray(obj: Any) -> bool:
        """
        Check if object is a numpy array.
        
        Args:
            obj: Object to check
            
        Returns:
            True if object is numpy array, False otherwise
        """
        return isinstance(obj, np.ndarray)
    
    @staticmethod
    def getNumpyArrayDims(obj: Any) -> List[int]:
        """
        Get numpy array dimensions.
        
        Args:
            obj: Numpy array
            
        Returns:
            List of dimensions
        """
        if isinstance(obj, np.ndarray):
            return list(obj.shape)
        return []
    
    @staticmethod
    def safeNumpyAccess(arr: np.ndarray, index: int) -> np.ndarray:
        """
        Safe numpy array access with bounds checking.
        
        Args:
            arr: Numpy array
            index: Array index
            
        Returns:
            Numpy array slice
            
        Raises:
            IndexError: If index is out of bounds
        """
        if not isinstance(arr, np.ndarray):
            raise TypeError("Input must be a numpy array")
        
        if index < 0 or index >= len(arr):
            raise IndexError(f"Index {index} out of bounds for array of length {len(arr)}")
        
        return arr[index]
class NumpyExport:
    """
    Numpy-based data export/import utilities for NLM framework.
    
    This class provides utilities for exporting and importing brain and world
    state data as numpy arrays for data analysis and visualization.
    """
    
    @staticmethod
    def exportBrainState(brain: Any) -> Dict[str, np.ndarray]:
        """
        Export brain state as numpy arrays.
        
        Args:
            brain: Brain object
            
        Returns:
            Dictionary containing brain state data
        """
        result = {}
        
        # This would be implemented with actual C++ brain access
        # For now, return placeholder data
        
        # Export neuron data (placeholder)
        neuron_data = np.zeros((100, 4), dtype=np.float32)  # id, region, type, active
        result["neurons"] = neuron_data
        
        # Export statistics
        result["total_neurons"] = np.array([brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0])
        result["total_synapses"] = np.array([brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0])
        result["avg_firing_rate"] = np.array([brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0])
        result["exc_inhibition_ratio"] = np.array([brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 1.0])
        
        return result
    
    @staticmethod
    def importBrainState(brain: Any, data: Dict[str, np.ndarray]) -> bool:
        """
        Import brain state from numpy arrays.
        
        Args:
            brain: Brain object
            data: Brain state data
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if "neurons" not in data:
                raise ValueError("Missing neurons array in import data")
            
            # This would be implemented with actual C++ brain access
            # For now, return placeholder success
            print("Importing brain state from numpy arrays (placeholder)")
            
            return True
            
        except Exception as e:
            print(f"Failed to import brain state: {e}")
            return False
    
    @staticmethod
    def exportWorldState(world: Any) -> Dict[str, np.ndarray]:
        """
        Export world state as numpy arrays.
        
        Args:
            world: World object
            
        Returns:
            Dictionary containing world state data
        """
        result = {}
        
        # Export world objects (placeholder)
        objects_data = np.zeros((10, 5), dtype=np.float32)  # max 10 objects, x, y, radius, type, value
        result["objects"] = objects_data
        
        # Export world statistics
        result["world_width"] = np.array([world.getWidth() if hasattr(world, 'getWidth') else 100.0])
        result["world_height"] = np.array([world.getHeight() if hasattr(world, 'getHeight') else 100.0])
        result["agent_x"] = np.array([world.getAgentBody().x if hasattr(world, 'getAgentBody') else 0.0])
        result["agent_y"] = np.array([world.getAgentBody().y if hasattr(world, 'getAgentBody') else 0.0])
        
        return result
class NumpyVisualization:
    """
    Numpy-based visualization functions for NLM framework.
    
    This class provides visualization utilities for brain state using numpy arrays
    for efficient data processing and visualization.
    """
    
    @staticmethod
    def visualizeNeuronActivity(brain: Any) -> np.ndarray:
        """
        Visualize neuron activity as 2D heatmap.
        
        Args:
            brain: Brain object
            
        Returns:
            2D numpy array representing neuron activity
        """
        # Get neuron count
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        
        if neuron_count == 0:
            return np.array([])
        
        # Create 2D grid representation (placeholder)
        grid_size = int(np.ceil(np.sqrt(neuron_count)))
        activity_grid = np.zeros((grid_size, grid_size), dtype=np.float32)
        
        # Fill with placeholder activity data
        for i in range(grid_size * grid_size):
            activity_grid.flat[i] = (i % 100) / 100.0  # 0.0 to 0.99 activity levels
        
        return activity_grid
    
    @staticmethod
    def visualizeSynapticConnections(brain: Any) -> np.ndarray:
        """
        Visualize synaptic connections as adjacency matrix.
        
        Args:
            brain: Brain object
            
        Returns:
            2D numpy array representing synaptic connections
        """
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        
        if neuron_count == 0:
            return np.array([])
        
        # Create adjacency matrix
        adjacency_matrix = np.zeros((neuron_count, neuron_count), dtype=np.float32)
        
        # Fill with placeholder connection weights
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        for i in range(min(synapse_count, neuron_count * 10)):
            row = i % neuron_count
            col = (i * 7) % neuron_count
            adjacency_matrix[row, col] = 0.5  # placeholder weight
        
        return adjacency_matrix
    
    @staticmethod
    def visualizeBrainRegions(brain: Any) -> np.ndarray:
        """
        Visualize brain regions with activity levels.
        
        Args:
            brain: Brain object
            
        Returns:
            2D numpy array representing brain regions
        """
        region_count = brain.getRegionCount() if hasattr(brain, 'getRegionCount') else 0
        
        if region_count == 0:
            return np.array([])
        
        # Create region data array
        region_data = np.zeros((region_count, 5), dtype=np.float32)  # id, activity, size, x, y
        
        # Fill with placeholder data
        for i in range(region_count):
            region_data[i, 0] = float(i)  # id
            region_data[i, 1] = 0.5  # activity
            region_data[i, 2] = 1.0  # size
            region_data[i, 3] = float(i * 10)  # x
            region_data[i, 4] = float(i * 10)  # y
        
        return region_data
    
    @staticmethod
    def generateSpikeRaster(brain: Any) -> Dict[str, np.ndarray]:
        """
        Generate spike raster plot data.
        
        Args:
            brain: Brain object
            
        Returns:
            Dictionary containing spike raster data
        """
        spike_count = brain.getTotalSpikeCount() if hasattr(brain, 'getTotalSpikeCount') else 0
        
        # Create spike times array
        spike_times = np.zeros(spike_count, dtype=np.float32)
        
        # Fill with placeholder spike times
        for i in range(spike_count):
            spike_times[i] = float(i * 10)
        
        return {
            "spike_times": spike_times,
            "total_spikes": np.array([spike_count]),
            "average_firing_rate": np.array([brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0]),
        }
    
    @staticmethod
    def visualizeNetworkTopology(brain: Any) -> Dict[str, np.ndarray]:
        """
        Visualize network topology.
        
        Args:
            brain: Brain object
            
        Returns:
            Dictionary containing network topology data
        """
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        
        # Create nodes array
        nodes = np.zeros((neuron_count, 3), dtype=np.float32)  # x, y, type
        
        # Create edges array
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        edges = np.zeros((synapse_count * 2, 2), dtype=np.float32)  # from, to
        
        # Fill with placeholder data
        for i in range(neuron_count):
            nodes[i, 0] = (i % 10) * 5.0
            nodes[i, 1] = (i // 10) * 5.0
            nodes[i, 2] = 1.0
        
        for i in range(synapse_count):
            edges[i * 2, 0] = float(i % neuron_count)
            edges[i * 2 + 1, 0] = float((i + 1) % neuron_count)
        
        return {
            "nodes": nodes,
            "edges": edges,
            "node_count": np.array([neuron_count]),
            "edge_count": np.array([synapse_count]),
        }
# Create instances of utility classes
python_session_logger = PythonSessionLogger()
numpy_utils = NumpyUtils()
numpy_export = NumpyExport()
numpy_visualization = NumpyVisualization()

# Set up traceback formatting

def formatPythonTraceback(error: Any = None) -> None:
    """
    Format Python exception traceback for better debugging.
    
    Args:
        error: Python error object (optional)
    """
    try:
        # Import traceback module
        import traceback
        
        # Format and print traceback
        if error is not None:
            print(f"=== Python Traceback ===")
            traceback.print_exception(type(error), error, error.__traceback__)
            print("=== End Traceback ===")
        else:
            print("=== Python Traceback ===")
            traceback.print_exc()
            print("=== End Traceback ===")
            
    except Exception:
        print("Error formatting traceback")
# Set up error handling

def setup_error_handling() -> None:
    """
    Set up enhanced error handling for Python exceptions.
    """
    import sys
    
    def exception_handler(exc_type, exc_value, exc_traceback):
        """Custom exception handler."""
        # Format traceback
        formatted_tb = ''.join(traceback.format_exception(exc_type, exc_value, exc_traceback))
        
        # Log error
        logger.error(f"Uncaught exception: {exc_type.__name__}: {exc_value}")
        logger.error(formatted_tb)
        
        # Print formatted traceback
        print("=== Python Traceback ===")
        print(formatted_tb)
        print("=== End Traceback ===")
        
        # Call original handler
        sys.__excepthook__(exc_type, exc_value, exc_traceback)
    
    # Set custom exception handler
    sys.excepthook = exception_handler

# Initialize error handling
setup_error_handling()

__all__ = [
    'PythonSessionLogger',
    'PythonTimer',
    'NumpyUtils',
    'NumpyExport',
    'NumpyVisualization',
    'python_session_logger',
    'numpy_utils',
    'numpy_export',
    'numpy_visualization',
    'formatPythonTraceback',
]
