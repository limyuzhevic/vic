#!/usr/bin/env python3
"""
Brain Exporter Tool

Export brain state in various formats for scientific analysis and reporting.
Provides comprehensive export capabilities for NLM brain states and data.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import json
import pickle
import h5py
import csv
import sys
import os

@dataclass
class BrainExporterConfig:
    """Configuration for brain exporter"""
    export_format: str = "hdf5"  # "hdf5", "json", "csv", "pickle", "npz", "edf"
    include_metadata: bool = True
    include_connectivity: bool = True
    include_plasticity: bool = True
    include_memory: bool = True
    include_activity: bool = True
    compress_data: bool = True
    include_timestamps: bool = True
    output_path: str = "brain_export"
    
    # Format-specific options
    hdf5_compression: str = "gzip"
    hdf5_compression_level: int = 4
    csv_delimiter: str = ","
    edf_sampling_rate: int = 1000
    
    # File organization
    create_subdirectories: bool = True
    subdirectory_structure: Dict[str, str] = None
    
    def __post_init__(self):
        if self.subdirectory_structure is None:
            self.subdirectory_structure = {
                "raw_data": "raw",
                "processed_data": "processed",
                "metadata": "meta",
                "visualization": "viz",
                "analysis": "analysis"
            }
class BrainExporter:
    """
    Brain exporter tool for NLM brain state export in various formats.
    
    Features:
    - Export brain state to HDF5 format with compression
    - Export to JSON format for interoperability
    - Export to CSV for spreadsheet analysis
    - Export to pickle format for Python objects
    - Export to NPZ format for NumPy arrays
    - Export to EDF format for EEG data
    - Include metadata and provenance information
    - Support for multiple file organization schemes
    - Preservation of scientific data quality
    """
    
    def __init__(self, config: BrainExporterConfig = None):
        self.config = config or BrainExporterConfig()
        self.export_data = {}
        self.export_metadata = {}
        
        # Initialize exporter components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize exporter components and data structures"""
        # Create subdirectories if requested
        if self.config.create_subdirectories:
            self._create_export_directories()
        
        # Initialize export data structure
        self.export_data = {
            "brain_state": {},
            "metadata": {},
            "provenance": {},
            "timestamps": {}
        }
    
    def _create_export_directories(self):
        """Create subdirectories for organized export"""
        base_dir = os.path.dirname(self.config.output_path) if os.path.dirname(self.config.output_path) else '.'
        
        for subdir_name, subdir_path in self.config.subdirectory_structure.items():
            dir_path = os.path.join(base_dir, subdir_path)
            os.makedirs(dir_path, exist_ok=True)
    
    def export_brain_state(self, 
                         brain_data: Dict[str, Any],
                         metadata: Optional[Dict[str, Any]] = None,
                         filename: str = None) -> Dict[str, Any]:
        """
        Export brain state data to file.
        
        Args:
            brain_data: Dictionary containing brain state data
            metadata: Optional metadata about the brain state
            filename: Output filename (uses config output_path if None)
            
        Returns:
            Dictionary with export results
        """
        result = {
            "success": False,
            "data": {},
            "metadata": {},
            "output_file": None,
            "export_format": self.config.export_format
        }
        
        try:
            # Set output filename
            if filename is None:
                filename = f"{self.config.output_path}.{self.config.export_format}"
            
            # Prepare export data
            self._prepare_export_data(brain_data, metadata)
            
            # Export based on format
            if self.config.export_format == "hdf5":
                output_file = self._export_to_hdf5(filename)
            elif self.config.export_format == "json":
                output_file = self._export_to_json(filename)
            elif self.config.export_format == "csv":
                output_file = self._export_to_csv(filename)
            elif self.config.export_format == "pickle":
                output_file = self._export_to_pickle(filename)
            elif self.config.export_format == "npz":
                output_file = self._export_to_npz(filename)
            elif self.config.export_format == "edf":
                output_file = self._export_to_edf(filename)
            else:
                result["error"] = f"Unsupported export format: {self.config.export_format}"
                return result
            
            # Update result
            result["output_file"] = output_file
            result["data"] = self.export_data
            result["metadata"] = self.export_metadata
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _prepare_export_data(self, brain_data: Dict[str, Any], metadata: Optional[Dict[str, Any]]):
        """Prepare data for export"""
        # Store brain state data
        self.export_data["brain_state"] = brain_data
        
        # Store metadata
        if metadata is not None:
            self.export_metadata.update(metadata)
        
        # Add standard metadata if missing
        if "export_timestamp" not in self.export_metadata:
            self.export_metadata["export_timestamp"] = self._get_timestamp()
        
        if "export_format" not in self.export_metadata:
            self.export_metadata["export_format"] = self.config.export_format
        
        # Add provenance information
        self.export_metadata["provenance"] = {
            "tool": "BrainExporter",
            "version": "1.0.0",
            "python_version": sys.version,
            "numpy_version": np.__version__
        }
    
    def _export_to_hdf5(self, filename: str) -> str:
        """Export data to HDF5 format"""
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        with h5py.File(filename, 'w') as f:
            # Store brain state data
            self._store_hdf5_data(f, self.export_data["brain_state"], "brain_state")
            
            # Store metadata
            self._store_hdf5_data(f, self.export_metadata, "metadata")
            
            # Store provenance
            if "provenance" in self.export_metadata:
                self._store_hdf5_data(f, self.export_metadata["provenance"], "provenance")
            
            # Store timestamps
            if "timestamps" in self.export_data:
                self._store_hdf5_data(f, self.export_data["timestamps"], "timestamps")
        
        return filename
    
    def _export_to_json(self, filename: str) -> str:
        """Export data to JSON format"""
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        # Convert numpy arrays to lists
        json_data = self._convert_to_json_serializable(self.export_data)
        
        with open(filename, 'w') as f:
            json.dump(json_data, f, indent=2)
        
        return filename
    
    def _export_to_csv(self, filename: str) -> str:
        """Export data to CSV format"""
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        with open(filename, 'w', newline='') as f:
            writer = csv.writer(f, delimiter=self.config.csv_delimiter)
            
            # Write metadata as header
            writer.writerow(["Key", "Value"])
            for key, value in self.export_metadata.items():
                if isinstance(value, (dict, list)):
                    value = str(value)
                writer.writerow([key, value])
            
            # Write brain state data
            writer.writerow([])  # Empty row for separation
            writer.writerow(["Data Type", "Value"])
            
            # Flatten brain state for CSV
            self._flatten_for_csv(self.export_data["brain_state"], writer)
        
        return filename
    
    def _export_to_pickle(self, filename: str) -> str:
        """Export data to pickle format"""
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        with open(filename, 'wb') as f:
            pickle.dump(self.export_data, f, protocol=pickle.HIGHEST_PROTOCOL)
        
        return filename
    
    def _export_to_npz(self, filename: str) -> str:
        """Export data to NPZ format"""
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        # Save numpy arrays
        np.savez(filename, **self._convert_to_numpy(self.export_data))
        
        return filename
    
    def _export_to_edf(self, filename: str) -> str:
        """Export data to EDF format (simplified)"""
        # Note: This is a simplified implementation
        # For real EDF export, you would need pyedflib or similar
        
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        # Create a simple EDF-like file (ASCII format for demonstration)
        with open(filename, 'w') as f:
            f.write("%%+ Measurements+ 1 1 1000 0 0 0+\n")
            f.write("# Original file created by BrainExporter+\n")
            f.write(f"# Export timestamp: {self._get_timestamp()}+\n")
            f.write(f"# Export format: {self.config.export_format}+\n")
            
            # Write sample data (simplified)
            f.write("1 0 0 RECORDINGS+\n")
            f.write("1 1 TR1 100.0 0 0+\n")
            f.write("0 1 0 0+\n")
        
        return filename
    
    def _store_hdf5_data(self, h5_file, data: Dict[str, Any], group_name: str):
        """Recursively store data in HDF5 file"""
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, (dict, list)):
                    self._store_hdf5_data(h5_file, value, f"{group_name}/{key}")
                elif isinstance(value, np.ndarray):
                    if self.config.compress_data:
                        h5_file.create_dataset(f"{group_name}/{key}", data=value, compression=self.config.hdf5_compression, compression_opts=self.config.hdf5_compression_level)
                    else:
                        h5_file.create_dataset(f"{group_name}/{key}", data=value)
                elif isinstance(value, (str, int, float, bool)):
                    # Store scalar values as attributes
                    h5_file.attrs[f"{group_name}/{key}"] = value
                else:
                    # Convert to string for storage
                    h5_file.create_dataset(f"{group_name}/{key}", data=str(value))
        elif isinstance(data, np.ndarray):
            if self.config.compress_data:
                h5_file.create_dataset(group_name, data=data, compression=self.config.hdf5_compression, compression_opts=self.config.hdf5_compression_level)
            else:
                h5_file.create_dataset(group_name, data=data)
        else:
            h5_file.attrs[group_name] = str(data)
    
    def _convert_to_json_serializable(self, data: Any) -> Any:
        """Convert data to JSON-serializable format"""
        if isinstance(data, dict):
            return {key: self._convert_to_json_serializable(value) for key, value in data.items()}
        elif isinstance(data, list):
            return [self._convert_to_json_serializable(item) for item in data]
        elif isinstance(data, np.ndarray):
            return data.tolist()
        elif isinstance(data, np.integer):
            return int(data)
        elif isinstance(data, np.floating):
            return float(data)
        else:
            return data
    
    def _flatten_for_csv(self, data: Dict[str, Any], writer):
        """Flatten data structure for CSV export"""
        def _flatten(obj, prefix=""):
            if isinstance(obj, dict):
                for key, value in obj.items():
                    _flatten(value, f"{prefix}{key}/")
            elif isinstance(obj, list):
                for i, item in enumerate(obj):
                    _flatten(item, f"{prefix}{i}/")
            elif isinstance(obj, np.ndarray):
                writer.writerow([f"{prefix}{key}", str(obj.shape)])
                for row in obj:
                    writer.writerow([f"{prefix}{key}[{i}]", row])
            else:
                writer.writerow([f"{prefix}{key}", obj])
        
        _flatten(data)
    
    def _convert_to_numpy(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Convert data to NumPy arrays for NPZ export"""
        np_data = {}
        
        def _convert(obj, key=""):
            if isinstance(obj, dict):
                for subkey, value in obj.items():
                    _convert(value, f"{key}{subkey}/")
            elif isinstance(obj, list):
                np_data[key] = np.array(obj)
            elif isinstance(obj, np.ndarray):
                np_data[key] = obj
            elif isinstance(obj, (int, float, str, bool)):
                np_data[key] = np.array(obj)
            else:
                np_data[key] = np.array(str(obj))
        
        _convert(data)
        return np_data
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        return datetime.now().isoformat()
    
    def export_multiple_formats(self, brain_data: Dict[str, Any], 
                               metadata: Optional[Dict[str, Any]] = None,
                               base_filename: str = "brain_export") -> Dict[str, Any]:
        """
        Export brain state to multiple formats.
        
        Args:
            brain_data: Brain state data
            metadata: Optional metadata
            base_filename: Base filename (format will be appended)
            
        Returns:
            Dictionary with export results for all formats
        """
        results = {}
        
        # Export to different formats
        formats = ["hdf5", "json", "csv", "pickle"]
        
        for fmt in formats:
            # Temporarily change config export format
            original_format = self.config.export_format
            self.config.export_format = fmt
            
            try:
                result = self.export_brain_state(brain_data, metadata, f"{base_filename}.{fmt}")
                results[fmt] = result
            except Exception as e:
                results[fmt] = {
                    "success": False,
                    "error": str(e),
                    "export_format": fmt
                }
            finally:
                # Restore original format
                self.config.export_format = original_format
        
        return results


def create_brain_exporter(config_dict: Dict[str, Any] = None) -> BrainExporter:
    """
    Factory function to create BrainExporter with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured BrainExporter instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = BrainExporterConfig(**config_dict)
    return BrainExporter(config)