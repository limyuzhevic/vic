#!/usr/bin/env python3
"""
Data Export/Import Utilities for NLM Neural Data

This module provides comprehensive utilities for exporting and importing neural
simulation data in various formats, including JSON, CSV, pickle, HDF5, and
custom binary formats. It also includes data validation and transformation tools.
"""

import json
import pickle
import csv
import h5py
import numpy as np
import pandas as pd
from pathlib import Path
from typing import List, Dict, Tuple, Optional, Any, Union, BinaryIO
from dataclasses import dataclass, asdict
import warnings
import struct
import zlib
import base64

warnings.filterwarnings('ignore')

class ExportFormat(Enum):
    """Data export formats"""
    JSON = "json"
    CSV = "csv"
    PICKLE = "pickle"
    HDF5 = "hdf5"
    BINARY = "binary"
    NPZ = "npz"

class ImportError(Exception):
    """Exception raised when data import fails"""
    pass

class ExportError(Exception):
    """Exception raised when data export fails"""
    pass

@dataclass
class ExportOptions:
    """Options for data export"""
    format: ExportFormat = ExportFormat.JSON
    compress: bool = False
    include_metadata: bool = True
    validate: bool = True
    encoding: str = "utf-8"
    delimiter: str = ","
    compression_level: int = 6
    chunk_size: int = 1000

@dataclass
class ImportOptions:
    """Options for data import"""
    format: ExportFormat = ExportFormat.JSON
    decompress: bool = False
    validate_schema: bool = True
    encoding: str = "utf-8"
    delimiter: str = ","
    chunk_size: int = 1000

class DataValidator:
    """Validate neural data structures and formats"""
    
    @staticmethod
    def validate_neural_data(data: Any) -> Tuple[bool, List[str]]:
        """Validate neural data structure"""
        errors = []
        
        if data is None:
            errors.append("Data is None")
            return False, errors
        
        # Check if it's a numpy array
        if isinstance(data, np.ndarray):
            if data.size == 0:
                errors.append("Array is empty")
        
        # Check if it's a dictionary
        elif isinstance(data, dict):
            if not data:
                errors.append("Dictionary is empty")
            else:
                for key, value in data.items():
                    if not isinstance(key, str):
                        errors.append(f"Dictionary key {key} is not a string")
        
        # Check if it's a list
        elif isinstance(data, list):
            if not data:
                errors.append("List is empty")
        
        return len(errors) == 0, errors
    
    @staticmethod
    def validate_metadata(metadata: Dict[str, Any]) -> Tuple[bool, List[str]]:
        """Validate metadata structure"""
        errors = []
        
        if not isinstance(metadata, dict):
            errors.append("Metadata must be a dictionary")
            return False, errors
        
        for key, value in metadata.items():
            if not isinstance(key, str):
                errors.append(f"Metadata key {key} is not a string")
            if not isinstance(value, (str, int, float, bool, dict, list)):
                errors.append(f"Metadata value for key {key} has unsupported type")
        
        return len(errors) == 0, errors
class NeuralDataExporter:
    """Export neural data to various formats"""
    
    def __init__(self, options: Optional[ExportOptions] = None):
        self.options = options or ExportOptions()
    
    def export(self, data: Any, filepath: str, 
               metadata: Optional[Dict[str, Any]] = None) -> bool:
        """Export neural data to file"""
        try:
            # Validate data
            if self.options.validate:
                is_valid, errors = DataValidator.validate_neural_data(data)
                if not is_valid:
                    raise ExportError(f"Data validation failed: {', '.join(errors)}")
            
            # Validate metadata
            if metadata and self.options.validate:
                is_valid, errors = DataValidator.validate_metadata(metadata)
                if not is_valid:
                    raise ExportError(f"Metadata validation failed: {', '.join(errors)}")
            
            # Export based on format
            if self.options.format == ExportFormat.JSON:
                self._export_json(data, filepath, metadata)
            elif self.options.format == ExportFormat.CSV:
                self._export_csv(data, filepath, metadata)
            elif self.options.format == ExportFormat.PICKLE:
                self._export_pickle(data, filepath, metadata)
            elif self.options.format == ExportFormat.HDF5:
                self._export_hdf5(data, filepath, metadata)
            elif self.options.format == ExportFormat.BINARY:
                self._export_binary(data, filepath, metadata)
            elif self.options.format == ExportFormat.NPZ:
                self._export_npz(data, filepath, metadata)
            else:
                raise ExportError(f"Unsupported export format: {self.options.format}")
            
            return True
            
        except Exception as e:
            raise ExportError(f"Export failed: {str(e)}")
    
    def _export_json(self, data: Any, filepath: str, 
                    metadata: Optional[Dict[str, Any]] = None):
        """Export data to JSON format"""
        export_data = {
            "data": data,
            "metadata": metadata or {}
        }
        
        if self.options.compress:
            # Compress JSON data
            json_str = json.dumps(export_data, indent=2, default=self._json_default)
            compressed_data = zlib.compress(json_str.encode(self.options.encoding), 
                                           self.options.compression_level)
            with open(filepath, 'wb') as f:
                f.write(compressed_data)
        else:
            with open(filepath, 'w', encoding=self.options.encoding) as f:
                json.dump(export_data, f, indent=2, default=self._json_default)
    
    def _export_csv(self, data: Any, filepath: str,
                   metadata: Optional[Dict[str, Any]] = None):
        """Export data to CSV format"""
        if isinstance(data, np.ndarray):
            df = pd.DataFrame(data)
        elif isinstance(data, dict):
            df = pd.DataFrame([data])
        else:
            raise ExportError(f"Cannot export data type {type(data)} to CSV")
        
        # Add metadata as comments
        with open(filepath, 'w', encoding=self.options.encoding, newline='') as f:
            if metadata:
                f.write(f"# Metadata: {json.dumps(metadata)}\n")
            df.to_csv(f, index=False, sep=self.options.delimiter)
    
    def _export_pickle(self, data: Any, filepath: str,
                      metadata: Optional[Dict[str, Any]] = None):
        """Export data to pickle format"""
        export_data = {
            "data": data,
            "metadata": metadata or {}
        }
        
        with open(filepath, 'wb') as f:
            pickle.dump(export_data, f, protocol=pickle.HIGHEST_PROTOCOL)
    
    def _export_hdf5(self, data: Any, filepath: str,
                    metadata: Optional[Dict[str, Any]] = None):
        """Export data to HDF5 format"""
        with h5py.File(filepath, 'w') as f:
            # Store data
            if isinstance(data, np.ndarray):
                f.create_dataset('data', data=data)
            elif isinstance(data, dict):
                for key, value in data.items():
                    if isinstance(value, np.ndarray):
                        f.create_dataset(key, data=value)
            
            # Store metadata
            if metadata:
                meta_group = f.create_group('metadata')
                for key, value in metadata.items():
                    if isinstance(value, (str, int, float, bool)):
                        meta_group.attrs[key] = value
                    elif isinstance(value, np.ndarray):
                        meta_group.create_dataset(key, data=value)
                    else:
                        meta_group.attrs[key] = str(value)
    
    def _export_binary(self, data: Any, filepath: str,
                      metadata: Optional[Dict[str, Any]] = None):
        """Export data to binary format"""
        # Convert to bytes
        data_bytes = pickle.dumps(data)
        
        # Add metadata
        export_data = {
            "data": data_bytes,
            "metadata": metadata or {}
        }
        
        # Write header
        with open(filepath, 'wb') as f:
            # Write metadata length
            metadata_str = json.dumps(export_data["metadata"])
            metadata_bytes = metadata_str.encode(self.options.encoding)
            f.write(struct.pack('I', len(metadata_bytes)))
            f.write(metadata_bytes)
            
            # Write data length and data
            f.write(struct.pack('I', len(data_bytes)))
            f.write(data_bytes)
    
    def _export_npz(self, data: Any, filepath: str,
                    metadata: Optional[Dict[str, Any]] = None):
        """Export data to NPZ format"""
        npz_data = {}
        
        if isinstance(data, np.ndarray):
            npz_data['data'] = data
        elif isinstance(data, dict):
            npz_data.update(data)
        
        if metadata:
            npz_data['metadata'] = metadata
        
        np.savez_compressed(filepath, **npz_data)
    
    @staticmethod
    def _json_default(obj):
        """JSON default serializer for complex objects"""
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        elif isinstance(obj, np.integer):
            return int(obj)
        elif isinstance(obj, np.floating):
            return float(obj)
        elif isinstance(obj, datetime):
            return obj.isoformat()
        elif isinstance(obj, type):
            return str(obj)
        else:
            raise TypeError(f"Object of type {type(obj)} is not JSON serializable")
class NeuralDataImporter:
    """Import neural data from various formats"""
    
    def __init__(self, options: Optional[ImportOptions] = None):
        self.options = options or ImportOptions()
    
    def import_data(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import neural data from file"""
        try:
            # Import based on format
            if self.options.format == ExportFormat.JSON:
                return self._import_json(filepath)
            elif self.options.format == ExportFormat.CSV:
                return self._import_csv(filepath)
            elif self.options.format == ExportFormat.PICKLE:
                return self._import_pickle(filepath)
            elif self.options.format == ExportFormat.HDF5:
                return self._import_hdf5(filepath)
            elif self.options.format == ExportFormat.BINARY:
                return self._import_binary(filepath)
            elif self.options.format == ExportFormat.NPZ:
                return self._import_npz(filepath)
            else:
                raise ImportError(f"Unsupported import format: {self.options.format}")
        
        except Exception as e:
            raise ImportError(f"Import failed: {str(e)}")
    
    def _import_json(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import data from JSON format"""
        with open(filepath, 'r', encoding=self.options.encoding) as f:
            data = json.load(f)
        
        return data.get('data'), data.get('metadata')
    
    def _import_csv(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import data from CSV format"""
        # Read metadata from comments
        metadata = {}
        with open(filepath, 'r', encoding=self.options.encoding) as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith('# Metadata:'):
                    metadata_str = line[12:].strip()
                    try:
                        metadata = json.loads(metadata_str)
                    except:
                        pass
                    break
        
        # Read CSV data
        df = pd.read_csv(filepath, sep=self.options.delimiter)
        return df.to_dict('records'), metadata if metadata else None
    
    def _import_pickle(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import data from pickle format"""
        with open(filepath, 'rb') as f:
            data = pickle.load(f)
        
        return data.get('data'), data.get('metadata')
    
    def _import_hdf5(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import data from HDF5 format"""
        with h5py.File(filepath, 'r') as f:
            if 'data' in f:
                if isinstance(f['data'], h5py.Dataset):
                    data = np.array(f['data'])
                else:
                    data = dict(f['data'])
            else:
                data = dict(f)
            
            metadata = {}
            if 'metadata' in f:
                meta_group = f['metadata']
                for key in meta_group.attrs:
                    metadata[key] = meta_group.attrs[key]
                for key in meta_group:
                    if isinstance(meta_group[key], h5py.Dataset):
                        metadata[key] = np.array(meta_group[key])
            
            return data, metadata if metadata else None
    
    def _import_binary(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import data from binary format"""
        with open(filepath, 'rb') as f:
            # Read metadata length
            metadata_len_bytes = f.read(4)
            if len(metadata_len_bytes) < 4:
                raise ImportError("File too short for metadata length")
            
            metadata_len = struct.unpack('I', metadata_len_bytes)[0]
            metadata_bytes = f.read(metadata_len)
            
            # Read data length
            data_len_bytes = f.read(4)
            if len(data_len_bytes) < 4:
                raise ImportError("File too short for data length")
            
            data_len = struct.unpack('I', data_len_bytes)[0]
            data_bytes = f.read(data_len)
        
        # Parse metadata
        metadata_str = metadata_bytes.decode(self.options.encoding)
        metadata = json.loads(metadata_str)
        
        # Parse data
        data = pickle.loads(data_bytes)
        
        return data, metadata if metadata else None
    
    def _import_npz(self, filepath: str) -> Tuple[Any, Optional[Dict[str, Any]]]:
        """Import data from NPZ format"""
        npz_data = np.load(filepath, allow_pickle=True)
        
        data = {}
        metadata = {}
        
        for key in npz_data.files:
            if key == 'metadata':
                metadata = dict(npz_data[key])
            else:
                data[key] = npz_data[key]
        
        return data, metadata if metadata else None
class NeuralDataTransformer:
    """Transform neural data for different formats and purposes"""
    
    @staticmethod
    def convert_to_dataframe(data: Union[List[float], np.ndarray, Dict[str, Any]]) -> pd.DataFrame:
        """Convert neural data to pandas DataFrame"""
        if isinstance(data, np.ndarray):
            if data.ndim == 1:
                return pd.DataFrame({'values': data})
            elif data.ndim == 2:
                return pd.DataFrame(data)
            else:
                raise ValueError(f"Unsupported array dimensions: {data.ndim}")
        elif isinstance(data, dict):
            return pd.DataFrame([data])
        else:
            raise ValueError(f"Unsupported data type: {type(data)}")
    
    @staticmethod
    def extract_features(data: Dict[str, Any], 
                        feature_names: Optional[List[str]] = None) -> np.ndarray:
        """Extract features from neural data dictionary"""
        if feature_names is None:
            feature_names = list(data.keys())
        
        features = []
        for name in feature_names:
            if name in data:
                value = data[name]
                if isinstance(value, (int, float)):
                    features.append(value)
                elif isinstance(value, list):
                    features.extend(value)
                elif isinstance(value, np.ndarray):
                    features.extend(value.flatten())
        
        return np.array(features)
    
    @staticmethod
    def normalize_data(data: np.ndarray, method: str = "standard") -> np.ndarray:
        """Normalize neural data"""
        if method == "standard":
            scaler = StandardScaler()
            return scaler.fit_transform(data.reshape(-1, 1)).flatten()
        elif method == "minmax":
            scaler = MinMaxScaler()
            return scaler.fit_transform(data.reshape(-1, 1)).flatten()
        elif method == "normalize":
            scaler = Normalizer()
            return scaler.fit_transform(data.reshape(-1, 1)).flatten()
        else:
            return data
class DataBatchProcessor:
    """Process neural data in batches"""
    
    def __init__(self, batch_size: int = 1000):
        self.batch_size = batch_size
    
    def process_in_batches(self, data: List[Any], processor: Callable[[Any], Any]) -> List[Any]:
        """Process data in batches"""
        results = []
        
        for i in range(0, len(data), self.batch_size):
            batch = data[i:i + self.batch_size]
            batch_results = [processor(item) for item in batch]
            results.extend(batch_results)
        
        return results
# Convenience functions

def export_neural_data(data: Any, filepath: str, options: Optional[ExportOptions] = None) -> bool:
    """Convenience function to export neural data"""
    exporter = NeuralDataExporter(options)
    return exporter.export(data, filepath)

def import_neural_data(filepath: str, options: Optional[ImportOptions] = None) -> Tuple[Any, Optional[Dict[str, Any]]]:
    """Convenience function to import neural data"""
    importer = NeuralDataImporter(options)
    return importer.import_data(filepath)

def convert_to_dataframe(data: Any) -> pd.DataFrame:
    """Convenience function to convert neural data to DataFrame"""
    return NeuralDataTransformer.convert_to_dataframe(data)

# Export all classes and functions
__all__ = [
    'ExportFormat', 'ImportFormat', 'ExportOptions', 'ImportOptions',
    'DataValidator', 'NeuralDataExporter', 'NeuralDataImporter', 
    'NeuralDataTransformer', 'DataBatchProcessor',
    'export_neural_data', 'import_neural_data', 'convert_to_dataframe'
]
