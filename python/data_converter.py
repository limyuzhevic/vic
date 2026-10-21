#!/usr/bin/env python3
"""
Data Converter Tool

Convert between different data formats for neural network simulation.
Provides comprehensive data conversion capabilities for NLM frameworks.
"""

import numpy as np
import pandas as pd
import json
import h5py
import csv
import sys
import os
from datetime import datetime
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum

@dataclass
class DataConverterConfig:
    """Configuration for data converter"""
    input_format: str = "auto"  # "auto", "hdf5", "json", "csv", "npy", "numpy", "pandas", "mat"
    output_format: str = "hdf5"  # "hdf5", "json", "csv", "npy", "numpy", "pandas", "mat"
    conversion_mode: str = "standard"  # "standard", "preserve_structure", "flatten"
    include_metadata: bool = True
    compress_output: bool = True
    validate_converted: bool = True
    output_path: str = "converted_data"
    
    # Format-specific options
    hdf5_compression: str = "gzip"
    hdf5_compression_level: int = 4
    csv_delimiter: str = ","
    json_indent: int = 2
    
    # Data transformation options
    normalize_values: bool = False
    scale_values: bool = False
    filter_nan: bool = True
    remove_duplicates: bool = False
class DataConverter:
    """
    Data converter tool for NLM data format conversion.
    
    Features:
    - Convert HDF5 to JSON format
    - Convert JSON to CSV format
    - Convert NumPy arrays to Pandas DataFrames
    - Convert between different data structures
    - Preserve metadata and provenance
    - Validate converted data
    - Support for multiple input/output formats
    """
    
    def __init__(self, config: DataConverterConfig = None):
        self.config = config or DataConverterConfig()
        self.conversion_metadata = {}
        
        # Initialize converter components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize converter components and data structures"""
        # Initialize conversion metadata
        self.conversion_metadata = {
            "input_format": self.config.input_format,
            "output_format": self.config.output_format,
            "conversion_mode": self.config.conversion_mode,
            "timestamp": self._get_timestamp(),
            "converter": "DataConverter"
        }
    
    def convert_data(self, 
                    input_data: Any,
                    input_format: Optional[str] = None,
                    output_format: Optional[str] = None,
                    filename: str = None) -> Dict[str, Any]:
        """
        Convert data from input format to output format.
        
        Args:
            input_data: Input data to convert
            input_format: Input format (uses config if None)
            output_format: Output format (uses config if None)
            filename: Output filename (uses config output_path if None)
            
        Returns:
            Dictionary with conversion results
        """
        result = {
            "success": False,
            "data": {},
            "metadata": {},
            "output_file": None,
            "input_format": input_format or self.config.input_format,
            "output_format": output_format or self.config.output_format,
            "conversion_metadata": self.conversion_metadata
        }
        
        try:
            # Determine input format
            actual_input_format = input_format or self._detect_input_format(input_data)
            
            # Convert based on input and output formats
            converted_data = self._convert_between_formats(
                input_data, actual_input_format, 
                output_format or self.config.output_format
            )
            
            # Validate converted data if requested
            if self.config.validate_converted:
                self._validate_converted_data(converted_data, output_format or self.config.output_format)
            
            # Save to file if filename provided
            if filename:
                output_file = self._save_converted_data(converted_data, filename)
                result["output_file"] = output_file
            
            # Update result
            result["data"] = converted_data
            result["metadata"] = self.conversion_metadata
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _detect_input_format(self, input_data: Any) -> str:
        """Detect input data format"""
        if isinstance(input_data, dict):
            if "h5" in str(type(input_data)) or "hdf5" in str(type(input_data)):
                return "hdf5"
            elif isinstance(input_data, (pd.DataFrame, pd.Series)):
                return "pandas"
            elif all(isinstance(v, (int, float, str)) for v in input_data.values()):
                return "json"
            else:
                return "dict"
        elif isinstance(input_data, np.ndarray):
            return "npy"
        elif isinstance(input_data, str):
            if input_data.endswith(".h5") or input_data.endswith(".hdf5"):
                return "hdf5"
            elif input_data.endswith(".json"):
                return "json"
            elif input_data.endswith(".csv"):
                return "csv"
            elif input_data.endswith(".npy"):
                return "npy"
            else:
                return "auto"
        elif hasattr(input_data, 'read'):  # File-like object
            return "auto"
        else:
            return "auto"
    
    def _convert_between_formats(self, input_data: Any, 
                                input_format: str,
                                output_format: str) -> Any:
        """Convert between different data formats"""
        conversion_map = {
            ("hdf5", "json"): self._convert_hdf5_to_json,
            ("hdf5", "csv"): self._convert_hdf5_to_csv,
            ("hdf5", "npy"): self._convert_hdf5_to_npy,
            ("json", "hdf5"): self._convert_json_to_hdf5,
            ("json", "csv"): self._convert_json_to_csv,
            ("json", "npy"): self._convert_json_to_npy,
            ("csv", "hdf5"): self._convert_csv_to_hdf5,
            ("csv", "json"): self._convert_csv_to_json,
            ("csv", "npy"): self._convert_csv_to_npy,
            ("npy", "hdf5"): self._convert_npy_to_hdf5,
            ("npy", "json"): self._convert_npy_to_json,
            ("npy", "csv"): self._convert_npy_to_csv,
            ("pandas", "hdf5"): self._convert_pandas_to_hdf5,
            ("pandas", "json"): self._convert_pandas_to_json,
            ("pandas", "csv"): self._convert_pandas_to_csv,
            ("pandas", "npy"): self._convert_pandas_to_npy,
            ("auto", "hdf5"): self._convert_auto_to_hdf5,
            ("auto", "json"): self._convert_auto_to_json,
            ("auto", "csv"): self._convert_auto_to_csv,
            ("auto", "npy"): self._convert_auto_to_npy,
        }
        
        key = (input_format, output_format)
        if key in conversion_map:
            return conversion_map[key](input_data)
        else:
            # Default conversion (return as-is with transformation)
            return self._convert_generic(input_data, output_format)
    
    def _convert_hdf5_to_json(self, input_data: Any) -> Dict[str, Any]:
        """Convert HDF5 data to JSON"""
        if isinstance(input_data, h5py.File):
            data = {}
            for key in input_data.keys():
                if isinstance(input_data[key], h5py.Group):
                    data[key] = self._hdf5_group_to_dict(input_data[key])
                elif isinstance(input_data[key], h5py.Dataset):
                    data[key] = input_data[key][:]
            return data
        elif isinstance(input_data, str):  # HDF5 filename
            with h5py.File(input_data, 'r') as f:
                return self._hdf5_file_to_dict(f)
        else:
            return self._convert_generic(input_data, "json")
    
    def _convert_hdf5_to_csv(self, input_data: Any) -> pd.DataFrame:
        """Convert HDF5 data to CSV"""
        if isinstance(input_data, h5py.File):
            data_frames = {}
            for key in input_data.keys():
                if isinstance(input_data[key], h5py.Dataset):
                    data_frames[key] = pd.DataFrame(input_data[key][:])
            return pd.concat(data_frames, ignore_index=True) if data_frames else pd.DataFrame()
        elif isinstance(input_data, str):  # HDF5 filename
            with h5py.File(input_data, 'r') as f:
                data_frames = {}
                for key in f.keys():
                    if isinstance(f[key], h5py.Dataset):
                        data_frames[key] = pd.DataFrame(f[key][:])
                return pd.concat(data_frames, ignore_index=True) if data_frames else pd.DataFrame()
        else:
            return self._convert_generic(input_data, "csv")
    
    def _convert_hdf5_to_npy(self, input_data: Any) -> Dict[str, np.ndarray]:
        """Convert HDF5 data to NumPy arrays"""
        if isinstance(input_data, h5py.File):
            arrays = {}
            for key in input_data.keys():
                if isinstance(input_data[key], h5py.Dataset):
                    arrays[key] = input_data[key][:]
            return arrays
        elif isinstance(input_data, str):  # HDF5 filename
            with h5py.File(input_data, 'r') as f:
                arrays = {}
                for key in f.keys():
                    if isinstance(f[key], h5py.Dataset):
                        arrays[key] = f[key][:]
                return arrays
        else:
            return self._convert_generic(input_data, "npy")
    
    def _convert_json_to_hdf5(self, input_data: Any, filename: str = None) -> str:
        """Convert JSON data to HDF5"""
        if filename is None:
            filename = f"{self.config.output_path}.h5"
        
        with h5py.File(filename, 'w') as f:
            self._dict_to_hdf5(input_data, f, "root")
        
        return filename
    
    def _convert_json_to_csv(self, input_data: Any) -> pd.DataFrame:
        """Convert JSON data to CSV"""
        if isinstance(input_data, dict):
            # Flatten nested dictionaries for CSV
            flattened = self._flatten_dict_for_csv(input_data)
            return pd.DataFrame(flattened)
        else:
            return self._convert_generic(input_data, "csv")
    
    def _convert_json_to_npy(self, input_data: Any) -> Dict[str, np.ndarray]:
        """Convert JSON data to NumPy arrays"""
        if isinstance(input_data, dict):
            arrays = {}
            for key, value in input_data.items():
                if isinstance(value, list):
                    arrays[key] = np.array(value)
                elif isinstance(value, dict):
                    # Recursively convert nested dictionaries
                    arrays[key] = np.array(list(value.values()))
                else:
                    arrays[key] = np.array([value])
            return arrays
        else:
            return self._convert_generic(input_data, "npy")
    
    def _convert_csv_to_hdf5(self, input_data: Any, filename: str = None) -> str:
        """Convert CSV data to HDF5"""
        if filename is None:
            filename = f"{self.config.output_path}.h5"
        
        if isinstance(input_data, str):  # CSV filename
            df = pd.read_csv(input_data, delimiter=self.config.csv_delimiter)
        elif isinstance(input_data, pd.DataFrame):
            df = input_data
        else:
            df = self._convert_generic(input_data, "csv")
        
        with h5py.File(filename, 'w') as f:
            f.create_dataset("data", data=df.values)
            f.create_dataset("columns", data=df.columns.tolist())
            f.create_dataset("index", data=df.index.tolist())
        
        return filename
    
    def _convert_csv_to_json(self, input_data: Any) -> Dict[str, Any]:
        """Convert CSV data to JSON"""
        if isinstance(input_data, str):  # CSV filename
            df = pd.read_csv(input_data, delimiter=self.config.csv_delimiter)
            return df.to_dict(orient='records')
        elif isinstance(input_data, pd.DataFrame):
            return df.to_dict(orient='records')
        else:
            return self._convert_generic(input_data, "json")
    
    def _convert_csv_to_npy(self, input_data: Any) -> Dict[str, np.ndarray]:
        """Convert CSV data to NumPy arrays"""
        if isinstance(input_data, str):  # CSV filename
            df = pd.read_csv(input_data, delimiter=self.config.csv_delimiter)
            return {col: df[col].values for col in df.columns}
        elif isinstance(input_data, pd.DataFrame):
            return {col: df[col].values for col in df.columns}
        else:
            return self._convert_generic(input_data, "npy")
    
    def _convert_npy_to_hdf5(self, input_data: Any, filename: str = None) -> str:
        """Convert NumPy data to HDF5"""
        if filename is None:
            filename = f"{self.config.output_path}.h5"
        
        if isinstance(input_data, dict):
            with h5py.File(filename, 'w') as f:
                self._dict_to_hdf5(input_data, f, "root")
        elif isinstance(input_data, (np.ndarray, list)):
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=np.asarray(input_data))
        else:
            return self._convert_generic(input_data, "hdf5")
        
        return filename
    
    def _convert_npy_to_json(self, input_data: Any) -> Dict[str, Any]:
        """Convert NumPy data to JSON"""
        if isinstance(input_data, dict):
            return {key: value.tolist() if isinstance(value, np.ndarray) else value 
                   for key, value in input_data.items()}
        elif isinstance(input_data, np.ndarray):
            return input_data.tolist()
        else:
            return self._convert_generic(input_data, "json")
    
    def _convert_npy_to_csv(self, input_data: Any) -> pd.DataFrame:
        """Convert NumPy data to CSV"""
        if isinstance(input_data, dict):
            # Convert each array to a column
            df_data = {}
            for key, value in input_data.items():
                if isinstance(value, np.ndarray):
                    if value.ndim == 1:
                        df_data[key] = value
                    elif value.ndim == 2:
                        # Treat as multiple columns
                        for i in range(value.shape[1]):
                            df_data[f"{key}_{i}"] = value[:, i]
            return pd.DataFrame(df_data)
        elif isinstance(input_data, np.ndarray):
            return pd.DataFrame(input_data)
        else:
            return self._convert_generic(input_data, "csv")
    
    def _convert_pandas_to_hdf5(self, input_data: Any, filename: str = None) -> str:
        """Convert Pandas data to HDF5"""
        if filename is None:
            filename = f"{self.config.output_path}.h5"
        
        if isinstance(input_data, pd.DataFrame):
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=input_data.values)
                f.create_dataset("columns", data=input_data.columns.tolist())
                f.create_dataset("index", data=input_data.index.tolist())
        else:
            return self._convert_generic(input_data, "hdf5")
        
        return filename
    
    def _convert_pandas_to_json(self, input_data: Any) -> Dict[str, Any]:
        """Convert Pandas data to JSON"""
        if isinstance(input_data, pd.DataFrame):
            return input_data.to_dict(orient='records')
        else:
            return self._convert_generic(input_data, "json")
    
    def _convert_pandas_to_csv(self, input_data: Any, filename: str = None) -> str:
        """Convert Pandas data to CSV"""
        if filename is None:
            filename = f"{self.config.output_path}.csv"
        
        if isinstance(input_data, pd.DataFrame):
            input_data.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        else:
            return self._convert_generic(input_data, "csv")
        
        return filename
    
    def _convert_pandas_to_npy(self, input_data: Any) -> Dict[str, np.ndarray]:
        """Convert Pandas data to NumPy arrays"""
        if isinstance(input_data, pd.DataFrame):
            return {col: input_data[col].values for col in input_data.columns}
        else:
            return self._convert_generic(input_data, "npy")
    
    def _convert_auto_to_hdf5(self, input_data: Any, filename: str = None) -> str:
        """Auto-detect and convert to HDF5"""
        if filename is None:
            filename = f"{self.config.output_path}.h5"
        
        if isinstance(input_data, dict):
            with h5py.File(filename, 'w') as f:
                self._dict_to_hdf5(input_data, f, "root")
        elif isinstance(input_data, pd.DataFrame):
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=input_data.values)
                f.create_dataset("columns", data=input_data.columns.tolist())
                f.create_dataset("index", data=input_data.index.tolist())
        elif isinstance(input_data, np.ndarray):
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=input_data)
        else:
            return self._convert_generic(input_data, "hdf5")
        
        return filename
    
    def _convert_auto_to_json(self, input_data: Any) -> Dict[str, Any]:
        """Auto-detect and convert to JSON"""
        if isinstance(input_data, pd.DataFrame):
            return input_data.to_dict(orient='records')
        elif isinstance(input_data, np.ndarray):
            return input_data.tolist()
        elif isinstance(input_data, dict):
            return input_data
        else:
            return self._convert_generic(input_data, "json")
    
    def _convert_auto_to_csv(self, input_data: Any, filename: str = None) -> str:
        """Auto-detect and convert to CSV"""
        if filename is None:
            filename = f"{self.config.output_path}.csv"
        
        if isinstance(input_data, pd.DataFrame):
            input_data.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        elif isinstance(input_data, dict):
            df = pd.DataFrame(input_data)
            df.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        elif isinstance(input_data, np.ndarray):
            df = pd.DataFrame(input_data)
            df.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        else:
            return self._convert_generic(input_data, "csv")
        
        return filename
    
    def _convert_auto_to_npy(self, input_data: Any) -> Dict[str, np.ndarray]:
        """Auto-detect and convert to NumPy arrays"""
        if isinstance(input_data, pd.DataFrame):
            return {col: input_data[col].values for col in input_data.columns}
        elif isinstance(input_data, dict):
            return {key: np.array(value) for key, value in input_data.items()}
        elif isinstance(input_data, np.ndarray):
            return {"data": input_data}
        else:
            return self._convert_generic(input_data, "npy")
    
    def _convert_generic(self, input_data: Any, output_format: str) -> Any:
        """Generic conversion for unsupported formats"""
        if output_format == "hdf5":
            # Create a simple HDF5 file
            filename = f"{self.config.output_path}.h5"
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=np.array([input_data]))
            return filename
        elif output_format == "json":
            return json.dumps(input_data)
        elif output_format == "csv":
            return str(input_data)  # Simplified
        elif output_format == "npy":
            return {"data": np.array([input_data])}
        else:
            return input_data
    
    def _hdf5_file_to_dict(self, h5_file: h5py.File) -> Dict[str, Any]:
        """Convert HDF5 file to dictionary"""
        data = {}
        for key in h5_file.keys():
            if isinstance(h5_file[key], h5py.Group):
                data[key] = self._hdf5_group_to_dict(h5_file[key])
            elif isinstance(h5_file[key], h5py.Dataset):
                data[key] = h5_file[key][:]
        return data
    
    def _hdf5_group_to_dict(self, h5_group: h5py.Group) -> Dict[str, Any]:
        """Convert HDF5 group to dictionary"""
        data = {}
        for key in h5_group.keys():
            if isinstance(h5_group[key], h5py.Group):
                data[key] = self._hdf5_group_to_dict(h5_group[key])
            elif isinstance(h5_group[key], h5py.Dataset):
                data[key] = h5_group[key][:]
        return data
    
    def _dict_to_hdf5(self, data: Dict[str, Any], h5_file: h5py.File, group_name: str):
        """Convert dictionary to HDF5"""
        for key, value in data.items():
            current_path = f"{group_name}/{key}" if group_name else key
            if isinstance(value, dict):
                h5_file.create_group(current_path)
                self._dict_to_hdf5(value, h5_file, current_path)
            elif isinstance(value, np.ndarray):
                if self.config.compress_output:
                    h5_file.create_dataset(current_path, data=value, 
                                          compression=self.config.hdf5_compression,
                                          compression_opts=self.config.hdf5_compression_level)
                else:
                    h5_file.create_dataset(current_path, data=value)
            elif isinstance(value, (str, int, float, bool)):
                h5_file.attrs[current_path] = value
            else:
                h5_file.create_dataset(current_path, data=str(value))
    
    def _flatten_dict_for_csv(self, data: Dict[str, Any]) -> Dict[str, List]:
        """Flatten nested dictionary for CSV export"""
        flattened = {}
        
        def _flatten(obj, prefix=""):
            if isinstance(obj, dict):
                for key, value in obj.items():
                    _flatten(value, f"{prefix}{key}/")
            elif isinstance(obj, list):
                for i, item in enumerate(obj):
                    _flatten(item, f"{prefix}{i}/")
            else:
                # Find first non-empty key path
                keys = [k for k in prefix.split('/') if k]
                if keys:
                    last_key = keys[-1]
                    flattened[last_key] = obj
        
        _flatten(data)
        return flattened
    
    def _validate_converted_data(self, data: Any, output_format: str):
        """Validate converted data"""
        if output_format == "hdf5":
            # Check if HDF5 file can be read
            if isinstance(data, str):  # Filename
                try:
                    with h5py.File(data, 'r') as f:
                        pass
                except:
                    raise ValueError(f"Invalid HDF5 file: {data}")
        
        elif output_format == "json":
            # Check if data can be serialized to JSON
            try:
                json.dumps(data)
            except:
                raise ValueError("Invalid JSON data")
        
        elif output_format == "csv":
            # Check if data can be converted to DataFrame
            try:
                if isinstance(data, str):
                    pd.read_csv(data)
                elif isinstance(data, pd.DataFrame):
                    pass
            except:
                raise ValueError("Invalid CSV data")
        
        elif output_format == "npy":
            # Check if data can be converted to NumPy array
            try:
                if isinstance(data, dict):
                    for value in data.values():
                        np.array(value)
                elif isinstance(data, (list, tuple)):
                    np.array(data)
            except:
                raise ValueError("Invalid NumPy data")
    
    def _save_converted_data(self, data: Any, filename: str) -> str:
        """Save converted data to file"""
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        if self.config.output_format == "hdf5":
            return self._convert_to_hdf5(data, filename)
        elif self.config.output_format == "json":
            return self._convert_to_json(data, filename)
        elif self.config.output_format == "csv":
            return self._convert_to_csv(data, filename)
        elif self.config.output_format == "npy":
            return self._convert_to_npy(data, filename)
        else:
            # Default: save as text file
            with open(filename, 'w') as f:
                f.write(str(data))
            return filename
    
    def _convert_to_hdf5(self, data: Any, filename: str) -> str:
        """Convert and save as HDF5"""
        if isinstance(data, dict):
            with h5py.File(filename, 'w') as f:
                self._dict_to_hdf5(data, f, "root")
        elif isinstance(data, pd.DataFrame):
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=data.values)
                f.create_dataset("columns", data=data.columns.tolist())
        elif isinstance(data, np.ndarray):
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=data)
        else:
            with h5py.File(filename, 'w') as f:
                f.create_dataset("data", data=np.array([data]))
        
        return filename
    
    def _convert_to_json(self, data: Any, filename: str) -> str:
        """Convert and save as JSON"""
        with open(filename, 'w') as f:
            json.dump(self._convert_to_json_serializable(data), f, indent=self.config.json_indent)
        
        return filename
    
    def _convert_to_csv(self, data: Any, filename: str) -> str:
        """Convert and save as CSV"""
        if isinstance(data, pd.DataFrame):
            data.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        elif isinstance(data, dict):
            df = pd.DataFrame(data)
            df.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        elif isinstance(data, np.ndarray):
            df = pd.DataFrame(data)
            df.to_csv(filename, index=False, delimiter=self.config.csv_delimiter)
        else:
            with open(filename, 'w') as f:
                f.write(str(data))
        
        return filename
    
    def _convert_to_npy(self, data: Any, filename: str) -> str:
        """Convert and save as NumPy file"""
        np.savez(filename, **self._convert_to_numpy(data))
        return filename
    
    def _convert_to_numpy(self, data: Any) -> Dict[str, np.ndarray]:
        """Convert data to NumPy arrays for NPZ export"""
        if isinstance(data, pd.DataFrame):
            return {col: data[col].values for col in data.columns}
        elif isinstance(data, dict):
            return {key: np.array(value) for key, value in data.items()}
        elif isinstance(data, np.ndarray):
            return {"data": data}
        else:
            return {"data": np.array([data])}
    
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
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        return datetime.now().isoformat()
    
    def batch_convert(self, 
                     input_files: List[str],
                     input_format: str = None,
                     output_format: str = None,
                     output_directory: str = None) -> Dict[str, Any]:
        """
        Convert multiple files in batch.
        
        Args:
            input_files: List of input file paths
            input_format: Input format (uses config if None)
            output_format: Output format (uses config if None)
            output_directory: Output directory (uses config output_path if None)
            
        Returns:
            Dictionary with batch conversion results
        """
        results = {
            "total_files": len(input_files),
            "successful_conversions": 0,
            "failed_conversions": 0,
            "results": []
        }
        
        for i, input_file in enumerate(input_files):
            # Determine output filename
            base_name = os.path.splitext(os.path.basename(input_file))[0]
            output_filename = f"{output_directory or self.config.output_path}/{base_name}"
            
            try:
                # Load input data
                if self.config.input_format in ["hdf5", "json", "csv"]:
                    with open(input_file, 'r') as f:
                        input_data = f.read()
                else:
                    input_data = input_file
                
                # Convert data
                result = self.convert_data(
                    input_data,
                    input_format or self.config.input_format,
                    output_format or self.config.output_format,
                    output_filename
                )
                
                results["results"].append(result)
                if result["success"]:
                    results["successful_conversions"] += 1
                else:
                    results["failed_conversions"] += 1
                    
            except Exception as e:
                results["results"].append({
                    "success": False,
                    "input_file": input_file,
                    "error": str(e)
                })
                results["failed_conversions"] += 1
        
        return results


def create_data_converter(config_dict: Dict[str, Any] = None) -> DataConverter:
    """
    Factory function to create DataConverter with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured DataConverter instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = DataConverterConfig(**config_dict)
    return DataConverter(config)