#!/usr/bin/env python3
"""
Advanced Data Management Tools for NLM Neural Data

This module provides comprehensive data management tools for neural simulation data,
including database integration, data pipeline utilities, data quality assessment,
data validation, data versioning, and data compression.
"""

import json
import pickle
import csv
import h5py
import numpy as np
import pandas as pd
import sqlite3
import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker, declarative_base
from sqlalchemy import Column, Integer, String, Float, DateTime, Boolean, Text, JSON, LargeBinary
from sqlalchemy.ext.declarative import declarative_base
from typing import List, Dict, Tuple, Optional, Any, Union, Callable, Generator
from dataclasses import dataclass, field
from enum import Enum
import warnings
import zlib
import base64
import hashlib
import os
import sys
from datetime import datetime, timezone
from pathlib import Path
from abc import ABC, abstractmethod
import threading
import queue
import itertools
import shutil
import jsonlines
import minio
from minio import Minio
from minio.error import MinioError

warnings.filterwarnings('ignore')

class DatabaseType(Enum):
    """Database types"""
    SQLITE = "sqlite"
    POSTGRESQL = "postgresql"
    MYSQL = "mysql"
    MONGODB = "mongodb"
    REDIS = "redis"

class DataFormat(Enum):
    """Data formats"""
    JSON = "json"
    CSV = "csv"
    HDF5 = "hdf5"
    PICKLE = "pickle"
    PARQUET = "parquet"
    ORC = "orc"
    AVRO = "avro"
    PROTOBUF = "protobuf"
    FEATHER = "feather"
    ARROW = "arrow"

class DataQualityIssue(Enum):
    """Types of data quality issues"""
    MISSING_VALUES = "missing_values"
    DUPLICATE_ROWS = "duplicate_rows"
    OUTLIERS = "outliers"
    INCONSISTENT_SCHEMA = "inconsistent_schema"
    NULL_CONSTRAINT_VIOLATION = "null_constraint_violation"
    TYPE_MISMATCH = "type_mismatch"
    CORRUPTION = "corruption"
    PERFORMANCE_ISSUES = "performance_issues"

@dataclass
class DatabaseConfig:
    """Configuration for database connection"""
    db_type: DatabaseType
    host: str = "localhost"
    port: int = 5432
    database: str = "nlm_neural_data"
    username: str = "postgres"
    password: str = "password"
    ssl_mode: str = "require"
    connection_pool_size: int = 5
    timeout: int = 30
    schema: Optional[str] = None

@dataclass
class PipelineConfig:
    """Configuration for data pipeline"""
    name: str
    input_format: DataFormat
    output_format: DataFormat
    input_path: str
    output_path: str
    transformation_steps: List[Dict[str, Any]] = field(default_factory=list)
    parallel_processing: bool = False
    batch_size: int = 1000
    validation_rules: Optional[List[Dict[str, Any]]] = None
    monitoring_enabled: bool = True

@dataclass
class DataQualityReport:
    """Report for data quality assessment"""
    file_path: str
    format: DataFormat
    total_rows: int
    total_columns: int
    missing_values: int
    duplicate_rows: int
    outlier_count: int
    schema_issues: List[Dict[str, Any]]
    quality_score: float
    recommendations: List[str]
    timestamp: datetime = field(default_factory=datetime.now)
    issues: List[Dict[str, Any]] = field(default_factory=list)

@dataclass
class ValidationRule:
    """Rule for data validation"""
    rule_id: str
    rule_type: str
    column: str
    condition: str
    parameters: Dict[str, Any]
    error_message: str
    is_required: bool = False
    allow_null: bool = True

@dataclass
class VersionInfo:
    """Version information for dataset"""
    version_id: str
    dataset_id: str
    version_number: int
    created_at: datetime
    created_by: str
    file_path: str
    checksum: str
    metadata: Dict[str, Any] = field(default_factory=dict)
    parent_version_id: Optional[str] = None

@dataclass
class CompressionInfo:
    """Information for compressed data"""
    original_size: int
    compressed_size: int
    compression_ratio: float
    algorithm: str
    compressed_path: str
    decompression_info: Dict[str, Any] = field(default_factory=dict)

class DatabaseManager:
    """Comprehensive database management for neural data"""
    
    def __init__(self, config: Optional[DatabaseConfig] = None):
        self.config = config or DatabaseConfig(DatabaseType.SQLITE)
        self.connection_pool = {}
        self._init_connection()
    
    def _init_connection(self):
        """Initialize database connection"""
        try:
            if self.config.db_type == DatabaseType.SQLITE:
                self.connection = sqlite3.connect(
                    f"sqlite:///neural_data.db", 
                    timeout=self.config.timeout
                )
                self.connection.row_factory = sqlite3.Row
                
                # Create neural data tables
                self._create_neural_tables()
                
            elif self.config.db_type == DatabaseType.POSTGRESQL:
                self.connection_string = (
                    f"postgresql://{self.config.username}:{self.config.password}"
                    f"@{self.config.host}:{self.config.port}/{self.config.database}"
                )
                
                # Test connection
                try:
                    psycopg2.connect(self.connection_string)
                    self.connection_string += f"?sslmode={self.config.ssl_mode}"
                except:
                    self.connection_string = self.connection_string.replace("postgresql://", "postgresql://")
                    
            self.engine = create_engine(self.connection_string, pool_size=self.config.connection_pool_size)
            self.Session = sessionmaker(bind=self.engine)
            
        except Exception as e:
            raise DatabaseConnectionError(f"Failed to initialize database: {e}")
    
    def _create_neural_tables(self):
        """Create neural data specific tables"""
        cursor = self.connection.cursor()
        
        # Neural simulation runs table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS simulation_runs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                run_id TEXT UNIQUE,
                brain_config TEXT,
                world_config TEXT,
                duration REAL,
                target_fps INTEGER,
                start_time TIMESTAMP,
                end_time TIMESTAMP,
                status TEXT,
                metadata TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        # Neurons table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS neurons (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                neuron_id TEXT UNIQUE,
                run_id TEXT,
                neuron_type TEXT,
                position_x REAL,
                position_y REAL,
                position_z REAL,
                potential REAL,
                threshold REAL,
                firing_rate REAL,
                is_active BOOLEAN,
                connections TEXT,
                metadata TEXT,
                FOREIGN KEY (run_id) REFERENCES simulation_runs (run_id)
            )
        ''')
        
        # Spikes table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS spikes (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                spike_id TEXT UNIQUE,
                run_id TEXT,
                neuron_id TEXT,
                timestamp REAL,
                spike_type TEXT,
                amplitude REAL,
                metadata TEXT,
                FOREIGN KEY (run_id) REFERENCES simulation_runs (run_id),
                FOREIGN KEY (neuron_id) REFERENCES neurons (neuron_id)
            )
        ''')
        
        # Brain regions table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS brain_regions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                region_id TEXT UNIQUE,
                run_id TEXT,
                region_type TEXT,
                x REAL, y REAL, z REAL,
                width REAL, height REAL, depth REAL,
                neuron_count INTEGER,
                metadata TEXT,
                FOREIGN KEY (run_id) REFERENCES simulation_runs (run_id)
            )
        ''')
        
        # World objects table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS world_objects (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                object_id TEXT UNIQUE,
                run_id TEXT,
                object_type TEXT,
                x REAL, y REAL, z REAL,
                width REAL, height REAL, depth REAL,
                metadata TEXT,
                FOREIGN KEY (run_id) REFERENCES simulation_runs (run_id)
            )
        ''')
        
        # Analysis results table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS analysis_results (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                result_id TEXT UNIQUE,
                run_id TEXT,
                analysis_type TEXT,
                parameters TEXT,
                results TEXT,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (run_id) REFERENCES simulation_runs (run_id)
            )
        ''')
        
        self.connection.commit()
        cursor.close()
    
    def store_simulation_run(self, run_id: str, brain_config: Dict, world_config: Dict,
                            duration: float, target_fps: int, start_time: datetime,
                            end_time: datetime, status: str, metadata: Dict = None) -> bool:
        """Store simulation run information"""
        try:
            cursor = self.connection.cursor()
            metadata_json = json.dumps(metadata) if metadata else None
            
            cursor.execute('''
                INSERT INTO simulation_runs (run_id, brain_config, world_config, duration,
                                          target_fps, start_time, end_time, status, metadata)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (run_id, json.dumps(brain_config), json.dumps(world_config),
                  duration, target_fps, start_time, end_time, status, metadata_json))
            
            self.connection.commit()
            cursor.close()
            return True
            
        except Exception as e:
            print(f"Error storing simulation run: {e}")
            return False
    
    def store_neuron_data(self, run_id: str, neurons: List[Dict]) -> bool:
        """Store neuron data"""
        try:
            cursor = self.connection.cursor()
            
            for neuron in neurons:
                neuron_id = neuron.get('neuron_id')
                cursor.execute('''
                    INSERT INTO neurons (neuron_id, run_id, neuron_type, position_x, position_y,
                                        position_z, potential, threshold, firing_rate, is_active,
                                        connections, metadata)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', (
                    neuron_id, run_id,
                    neuron.get('neuron_type', 'excitatory'),
                    neuron.get('position_x', 0.0), neuron.get('position_y', 0.0), neuron.get('position_z', 0.0),
                    neuron.get('potential', -70.0), neuron.get('threshold', -55.0),
                    neuron.get('firing_rate', 0.0), neuron.get('is_active', False),
                    json.dumps(neuron.get('connections', {})),
                    json.dumps(neuron.get('metadata', {}))
                ))
            
            self.connection.commit()
            cursor.close()
            return True
            
        except Exception as e:
            print(f"Error storing neuron data: {e}")
            return False
    
    def store_spike_data(self, run_id: str, spikes: List[Dict]) -> bool:
        """Store spike data"""
        try:
            cursor = self.connection.cursor()
            
            for spike in spikes:
                spike_id = spike.get('spike_id')
                cursor.execute('''
                    INSERT INTO spikes (spike_id, run_id, neuron_id, timestamp,
                                      spike_type, amplitude, metadata)
                    VALUES (?, ?, ?, ?, ?, ?, ?)
                ''', (
                    spike_id, run_id,
                    spike.get('neuron_id'), spike.get('timestamp', 0.0),
                    spike.get('spike_type', 'regular'), spike.get('amplitude', 1.0),
                    json.dumps(spike.get('metadata', {}))
                ))
            
            self.connection.commit()
            cursor.close()
            return True
            
        except Exception as e:
            print(f"Error storing spike data: {e}")
            return False
    
    def query_simulation_runs(self, status: Optional[str] = None) -> List[Dict]:
        """Query simulation runs with optional status filter"""
        try:
            cursor = self.connection.cursor()
            
            if status:
                cursor.execute('''
                    SELECT * FROM simulation_runs WHERE status = ?
                    ORDER BY start_time DESC
                ''', (status,))
            else:
                cursor.execute('''
                    SELECT * FROM simulation_runs
                    ORDER BY start_time DESC
                ''')
            
            columns = [description[0] for description in cursor.description]
            results = []
            
            for row in cursor.fetchall():
                results.append(dict(zip(columns, row)))
            
            cursor.close()
            return results
            
        except Exception as e:
            print(f"Error querying simulation runs: {e}")
            return []
    
    def close(self):
        """Close database connection"""
        if hasattr(self, 'connection'):
            self.connection.close()
class PostgreSQLManager(DatabaseManager):
    """PostgreSQL-specific database manager"""
    
    def __init__(self, config: Optional[DatabaseConfig] = None):
        if config is None:
            config = DatabaseConfig(DatabaseType.POSTGRESQL)
        elif config.db_type != DatabaseType.POSTGRESQL:
            raise ValueError("Config must specify PostgreSQL database type")
        
        super().__init__(config)
    
    def create_vector_extensions(self):
        """Create PostgreSQL vector extensions for neural similarity search"""
        try:
            connection = psycopg2.connect(self.connection_string)
            connection.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
            
            cursor = connection.cursor()
            
            # Create extension for vector similarity
            cursor.execute('CREATE EXTENSION IF NOT EXISTS vector')
            
            # Create neural embedding table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS neural_embeddings (
                    id SERIAL PRIMARY KEY,
                    neuron_id TEXT,
                    embedding vector(384),
                    neuron_type TEXT,
                    firing_rate REAL,
                    position_x REAL,
                    position_y REAL,
                    position_z REAL,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    run_id TEXT
                )
            ''')
            
            # Create index for cosine similarity search
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_neural_embeddings_embedding 
                ON neural_embeddings USING ivfflat (embedding vector_cosine_ops)
            ''')
            
            # Create hnsw index for better performance
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_neural_embeddings_hnsw 
                ON neural_embeddings USING hnsw (embedding vector_cosine_ops)
            ''')
            
            connection.commit()
            cursor.close()
            connection.close()
            
            return True
            
        except Exception as e:
            print(f"Error creating vector extensions: {e}")
            return False
    
    def search_similar_neurons(self, embedding: np.ndarray, run_id: Optional[str] = None,
                              top_k: int = 10) -> List[Dict]:
        """Search for similar neurons using vector similarity"""
        try:
            connection = psycopg2.connect(self.connection_string)
            
            cursor = connection.cursor()
            
            # Prepare query
            query = '''
                SELECT neuron_id, neuron_type, firing_rate, position_x, position_y, position_z,
                       embedding <=> %s::vector as cosine_distance
                FROM neural_embeddings
            '''
            
            params = [embedding.tolist()]
            
            if run_id:
                query += ' WHERE run_id = %s'
                params.append(run_id)
            
            query += ' ORDER BY cosine_distance LIMIT %s'
            params.append(top_k)
            
            cursor.execute(query, params)
            
            results = []
            for row in cursor.fetchall():
                results.append({
                    'neuron_id': row[0],
                    'neuron_type': row[1],
                    'firing_rate': row[2],
                    'position': (row[3], row[4], row[5]),
                    'cosine_distance': row[6],
                    'similarity': 1 - row[6]
                })
            
            cursor.close()
            connection.close()
            
            return results
            
        except Exception as e:
            print(f"Error searching similar neurons: {e}")
            return []

class DataPipeline:
    """Data pipeline for neural data processing"""
    
    def __init__(self, config: Optional[PipelineConfig] = None):
        self.config = config or PipelineConfig(
            name="default_neural_pipeline",
            input_format=DataFormat.HDF5,
            output_format=DataFormat.PARQUET,
            input_path="",
            output_path="",
            parallel_processing=True,
            batch_size=1000
        )
        
        self.progress_queue = queue.Queue()
        self.active_threads = []
        self.is_running = False
    
    def add_transformation(self, step_type: str, parameters: Dict[str, Any]) -> bool:
        """Add transformation step to pipeline"""
        try:
            step = {
                'type': step_type,
                'parameters': parameters,
                'enabled': True
            }
            
            self.config.transformation_steps.append(step)
            return True
            
        except Exception as e:
            print(f"Error adding transformation step: {e}")
            return False
    
    def process_data(self, input_data: Union[str, np.ndarray, pd.DataFrame],
                    callback: Optional[Callable] = None) -> Dict[str, Any]:
        """Process data through the pipeline"""
        result = {
            'success': False,
            'steps_processed': 0,
            'total_steps': len(self.config.transformation_steps),
            'progress': 0,
            'output_data': None,
            'errors': []
        }
        
        try:
            # Start processing
            self.is_running = True
            result['steps_processed'] = 0
            
            # Convert input data to DataFrame
            if isinstance(input_data, str):
                if self.config.input_format == DataFormat.CSV:
                    input_df = pd.read_csv(input_data)
                elif self.config.input_format == DataFormat.HDF5:
                    input_df = self._read_hdf5(input_data)
                elif self.config.input_format == DataFormat.PICKLE:
                    with open(input_data, 'rb') as f:
                        input_df = pickle.load(f)
                elif self.config.input_format == DataFormat.JSON:
                    with open(input_data, 'r') as f:
                        input_df = pd.read_json(f)
                else:
                    raise ValueError(f"Unsupported input format: {self.config.input_format}")
            elif isinstance(input_data, np.ndarray):
                input_df = pd.DataFrame(input_data)
            elif isinstance(input_data, pd.DataFrame):
                input_df = input_data.copy()
            else:
                raise ValueError("Unsupported input data type")
            
            current_data = input_df
            
            # Process each transformation step
            for step in self.config.transformation_steps:
                if not self.is_running:
                    break
                
                step_name = step['type']
                step_params = step['parameters']
                
                try:
                    # Apply transformation
                    current_data = self._apply_transformation(current_data, step_name, step_params)
                    
                    # Update progress
                    result['steps_processed'] += 1
                    result['progress'] = (result['steps_processed'] / 
                                       result['total_steps']) * 100
                    
                    # Call callback if provided
                    if callback:
                        callback(result['progress'], f"Processing step: {step_name}")
                    
                except Exception as e:
                    error_msg = f"Error in step '{step_name}': {str(e)}"
                    result['errors'].append(error_msg)
                    print(error_msg)
                    
                    if step.get('stop_on_error', True):
                        break
            
            if not result['errors']:
                # Convert to output format
                result['output_data'] = self._convert_output_format(current_data)
                result['success'] = True
            
        except Exception as e:
            error_msg = f"Pipeline processing failed: {str(e)}"
            result['errors'].append(error_msg)
            print(error_msg)
            
        finally:
            self.is_running = False
        
        return result
    
    def _apply_transformation(self, data: pd.DataFrame, step_type: str, 
                           parameters: Dict[str, Any]) -> pd.DataFrame:
        """Apply specific transformation step"""
        
        if step_type == 'normalize':
            return self._normalize_data(data, parameters)
        
        elif step_type == 'filter':
            return self._filter_data(data, parameters)
        
        elif step_type == 'aggregate':
            return self._aggregate_data(data, parameters)
        
        elif step_type == 'feature_extraction':
            return self._extract_features(data, parameters)
        
        elif step_type == 'remove_outliers':
            return self._remove_outliers(data, parameters)
        
        elif step_type == 'impute_missing':
            return self._impute_missing_values(data, parameters)
        
        elif step_type == 'encode_categoricals':
            return self._encode_categoricals(data, parameters)
        
        elif step_type == 'scale':
            return self._scale_data(data, parameters)
        
        elif step_type == 'windowing':
            return self._window_data(data, parameters)
        
        elif step_type == 'decompose':
            return self._decompose_time_series(data, parameters)
        
        else:
            raise ValueError(f"Unknown transformation step: {step_type}")
    
    def _normalize_data(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Normalize numerical features"""
        method = parameters.get('method', 'zscore')
        columns = parameters.get('columns')
        
        if method == 'zscore':
            from sklearn.preprocessing import StandardScaler
            scaler = StandardScaler()
            if columns:
                data[columns] = scaler.fit_transform(data[columns])
            else:
                data = pd.DataFrame(scaler.fit_transform(data), columns=data.columns)
        
        elif method == 'minmax':
            from sklearn.preprocessing import MinMaxScaler
            scaler = MinMaxScaler()
            if columns:
                data[columns] = scaler.fit_transform(data[columns])
            else:
                data = pd.DataFrame(scaler.fit_transform(data), columns=data.columns)
        
        elif method == 'robust':
            from sklearn.preprocessing import RobustScaler
            scaler = RobustScaler()
            if columns:
                data[columns] = scaler.fit_transform(data[columns])
            else:
                data = pd.DataFrame(scaler.fit_transform(data), columns=data.columns)
        
        return data
    
    def _filter_data(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Filter data based on conditions"""
        conditions = parameters.get('conditions', [])
        
        for condition in conditions:
            column = condition.get('column')
            operator = condition.get('operator', '==')
            value = condition.get('value')
            
            if column not in data.columns:
                continue
            
            if operator == '==':
                data = data[data[column] == value]
            elif operator == '!=':
                data = data[data[column] != value]
            elif operator == '>':
                data = data[data[column] > value]
            elif operator == '<':
                data = data[data[column] < value]
            elif operator == '>=':
                data = data[data[column] >= value]
            elif operator == '<=':
                data = data[data[column] <= value]
        
        return data
    
    def _aggregate_data(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Aggregate data by time or other dimensions"""
        group_by = parameters.get('group_by', [])
        aggregations = parameters.get('aggregations', {'value': 'mean'})
        
        if not group_by:
            return data
        
        try:
            aggregated = data.groupby(group_by).agg(aggregations).reset_index()
            return aggregated
        except Exception:
            return data
    
    def _extract_features(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Extract features from time series data"""
        feature_functions = parameters.get('features', [])
        window_size = parameters.get('window_size', 10)
        
        new_features = {}
        
        for func_name in feature_functions:
            if func_name == 'rolling_mean':
                for col in data.select_dtypes(include=[np.number]).columns:
                    new_features[f"{col}_rolling_mean"] = data[col].rolling(window=window_size).mean()
            
            elif func_name == 'rolling_std':
                for col in data.select_dtypes(include=[np.number]).columns:
                    new_features[f"{col}_rolling_std"] = data[col].rolling(window=window_size).std()
            
            elif func_name == 'moving_average':
                for col in data.select_dtypes(include=[np.number]).columns:
                    new_features[f"{col}_moving_avg"] = data[col].rolling(window=window_size).mean()
            
            elif func_name == 'diff':
                for col in data.select_dtypes(include=[np.number]).columns:
                    new_features[f"{col}_diff"] = data[col].diff()
            
            elif func_name == 'pct_change':
                for col in data.select_dtypes(include=[np.number]).columns:
                    new_features[f"{col}_pct_change"] = data[col].pct_change()
        
        # Add new features to dataframe
        if new_features:
            features_df = pd.DataFrame(new_features)
            data = pd.concat([data.reset_index(drop=True), features_df.reset_index(drop=True)], axis=1)
        
        return data
    
    def _remove_outliers(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Remove outliers from data"""
        columns = parameters.get('columns', [])
        method = parameters.get('method', 'zscore')
        threshold = parameters.get('threshold', 3.0)
        
        if not columns:
            columns = data.select_dtypes(include=[np.number]).columns.tolist()
        
        if method == 'zscore':
            from scipy import stats
            for col in columns:
                z_scores = np.abs(stats.zscore(data[col]))
                data = data[z_scores < threshold]
        
        elif method == 'iqr':
            for col in columns:
                Q1 = data[col].quantile(0.25)
                Q3 = data[col].quantile(0.75)
                IQR = Q3 - Q1
                lower_bound = Q1 - threshold * IQR
                upper_bound = Q3 + threshold * IQR
                data = data[(data[col] >= lower_bound) & (data[col] <= upper_bound)]
        
        return data
    
    def _impute_missing_values(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Impute missing values"""
        strategy = parameters.get('strategy', 'mean')
        columns = parameters.get('columns', [])
        fill_value = parameters.get('fill_value', None)
        
        if not columns:
            columns = data.select_dtypes(include=[np.number]).columns.tolist()
        
        if strategy == 'mean':
            for col in columns:
                data[col].fillna(data[col].mean(), inplace=True)
        
        elif strategy == 'median':
            for col in columns:
                data[col].fillna(data[col].median(), inplace=True)
        
        elif strategy == 'mode':
            for col in columns:
                if data[col].dtype == 'object':
                    data[col].fillna(data[col].mode().iloc[0] if not data[col].mode().empty else '', inplace=True)
                else:
                    data[col].fillna(data[col].median(), inplace=True)
        
        elif strategy == 'forward_fill':
            data.fillna(method='ffill', inplace=True)
        
        elif strategy == 'backward_fill':
            data.fillna(method='bfill', inplace=True)
        
        elif strategy == 'constant' and fill_value is not None:
            data.fillna(fill_value, inplace=True)
        
        return data
    
    def _encode_categoricals(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Encode categorical variables"""
        encoding_method = parameters.get('method', 'onehot')
        columns = parameters.get('columns', [])
        
        if not columns:
            columns = data.select_dtypes(include=['object']).columns.tolist()
        
        if encoding_method == 'onehot':
            from sklearn.preprocessing import OneHotEncoder
            encoder = OneHotEncoder(sparse_output=False, drop='first')
            encoded = encoder.fit_transform(data[columns])
            encoded_df = pd.DataFrame(encoded, columns=encoder.get_feature_names_out(columns))
            data = pd.concat([data.drop(columns, axis=1), encoded_df], axis=1)
        
        elif encoding_method == 'label':
            from sklearn.preprocessing import LabelEncoder
            for col in columns:
                le = LabelEncoder()
                data[col] = le.fit_transform(data[col])
        
        return data
    
    def _scale_data(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Scale numerical data"""
        method = parameters.get('method', 'standard')
        columns = parameters.get('columns', [])
        
        if not columns:
            columns = data.select_dtypes(include=[np.number]).columns.tolist()
        
        if method == 'standard':
            from sklearn.preprocessing import StandardScaler
            scaler = StandardScaler()
            data[columns] = scaler.fit_transform(data[columns])
        
        elif method == 'minmax':
            from sklearn.preprocessing import MinMaxScaler
            scaler = MinMaxScaler()
            data[columns] = scaler.fit_transform(data[columns])
        
        elif method == 'robust':
            from sklearn.preprocessing import RobustScaler
            scaler = RobustScaler()
            data[columns] = scaler.fit_transform(data[columns])
        
        return data
    
    def _window_data(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Create sliding windows for time series"""
        window_size = parameters.get('window_size', 10)
        stride = parameters.get('stride', 1)
        
        if data.empty:
            return data
        
        # Get numeric columns
        numeric_cols = data.select_dtypes(include=[np.number]).columns.tolist()
        
        if not numeric_cols:
            return data
        
        # Create windows
        windows = []
        
        for i in range(0, len(data) - window_size + 1, stride):
            window = data.iloc[i:i + window_size]
            windows.append(window)
        
        if windows:
            # Convert to DataFrame
            window_data = pd.concat(windows, ignore_index=True)
            # Add window index
            window_data['window_index'] = np.repeat(range(len(windows)), window_size)
            return window_data
        
        return data
    
    def _decompose_time_series(self, data: pd.DataFrame, parameters: Dict[str, Any]) -> pd.DataFrame:
        """Decompose time series into trend, seasonality, and residual"""
        from statsmodels.tsa.seasonal import seasonal_decompose
        
        # Find time series column
        time_series_cols = data.select_dtypes(include=[np.number]).columns.tolist()
        
        if not time_series_cols:
            return data
        
        # Assume first numeric column is time series
        ts_col = time_series_cols[0]
        ts_values = data[ts_col]
        
        # Determine period
        period = parameters.get('period', 12 if len(ts_values) > 12 else len(ts_values) // 4)
        
        if len(ts_values) > period:
            try:
                decomposition = seasonal_decompose(ts_values, model='additive', period=period)
                
                # Add decomposition components
                data['trend'] = decomposition.trend
                data['seasonal'] = decomposition.seasonal
                data['residual'] = decomposition.resid
                
            except Exception as e:
                print(f"Decomposition failed: {e}")
        
        return data
    
    def _read_hdf5(self, filepath: str) -> pd.DataFrame:
        """Read data from HDF5 file"""
        try:
            with h5py.File(filepath, 'r') as f:
                # Read datasets
                data = {}
                for key in f.keys():
                    if isinstance(f[key], h5py.Dataset):
                        data[key] = f[key][()]
                
                return pd.DataFrame(data)
        except Exception as e:
            print(f"Error reading HDF5 file: {e}")
            return pd.DataFrame()
    
    def _convert_output_format(self, data: pd.DataFrame) -> Union[str, bytes, Dict[str, Any]]:
        """Convert data to output format"""
        if self.config.output_format == DataFormat.CSV:
            output_path = f"{self.config.output_path}/data.csv"
            data.to_csv(output_path, index=False)
            return output_path
        
        elif self.config.output_format == DataFormat.PARQUET:
            output_path = f"{self.config.output_path}/data.parquet"
            data.to_parquet(output_path, index=False)
            return output_path
        
        elif self.config.output_format == DataFormat.HDF5:
            output_path = f"{self.config.output_path}/data.h5"
            with h5py.File(output_path, 'w') as f:
                for col in data.columns:
                    f.create_dataset(col, data=data[col].values)
            return output_path
        
        elif self.config.output_format == DataFormat.PICKLE:
            output_path = f"{self.config.output_path}/data.pkl"
            with open(output_path, 'wb') as f:
                pickle.dump(data, f)
            return output_path
        
        elif self.config.output_format == DataFormat.JSON:
            output_path = f"{self.config.output_path}/data.json"
            data.to_json(output_path, orient='records')
            return output_path
        
        else:
            # Return as dictionary
            return data.to_dict(orient='records')
    
    def stop(self):
        """Stop pipeline processing"""
        self.is_running = False
        
        for thread in self.active_threads:
            thread.join(timeout=1.0)
        
        self.active_threads.clear()

class DataQualityAssessor:
    """Comprehensive data quality assessment"""
    
    def __init__(self):
        self.validation_rules = []
    
    def add_validation_rule(self, rule: ValidationRule):
        """Add validation rule"""
        self.validation_rules.append(rule)
    
    def assess_data_quality(self, data: Union[str, pd.DataFrame, np.ndarray],
                          format: Optional[DataFormat] = None) -> DataQualityReport:
        """Assess data quality and return comprehensive report"""
        if isinstance(data, str):
            return self._assess_file_quality(data, format)
        elif isinstance(data, pd.DataFrame):
            return self._assess_dataframe_quality(data)
        elif isinstance(data, np.ndarray):
            return self._assess_array_quality(data)
        else:
            raise ValueError("Unsupported data type")
    
    def _assess_file_quality(self, filepath: str, format: Optional[DataFormat]) -> DataQualityReport:
        """Assess quality of file data"""
        try:
            # Determine format if not provided
            if format is None:
                if filepath.endswith('.csv'):
                    format = DataFormat.CSV
                elif filepath.endswith('.json'):
                    format = DataFormat.JSON
                elif filepath.endswith('.h5') or filepath.endswith('.hdf5'):
                    format = DataFormat.HDF5
                elif filepath.endswith('.pkl') or filepath.endswith('.pickle'):
                    format = DataFormat.PICKLE
                else:
                    format = DataFormat.JSON  # Default
            
            # Load data
            if format == DataFormat.CSV:
                df = pd.read_csv(filepath)
            elif format == DataFormat.JSON:
                df = pd.read_json(filepath)
            elif format == DataFormat.HDF5:
                with h5py.File(filepath, 'r') as f:
                    data_dict = {}
                    for key in f.keys():
                        if isinstance(f[key], h5py.Dataset):
                            data_dict[key] = f[key][()]
                    df = pd.DataFrame(data_dict)
            elif format == DataFormat.PICKLE:
                with open(filepath, 'rb') as f:
                    df = pickle.load(f)
            else:
                df = pd.read_csv(filepath)  # fallback
            
            return self._assess_dataframe_quality(df, filepath)
            
        except Exception as e:
            return DataQualityReport(
                file_path=filepath,
                format=format or DataFormat.JSON,
                total_rows=0,
                total_columns=0,
                missing_values=0,
                duplicate_rows=0,
                outlier_count=0,
                schema_issues=[{'issue': 'Load Error', 'description': str(e)}],
                quality_score=0.0,
                recommendations=['Fix file loading error'],
                issues=[{'type': 'CORRUPTION', 'description': str(e)}]
            )
    
    def _assess_dataframe_quality(self, df: pd.DataFrame, file_path: str = "") -> DataQualityReport:
        """Assess quality of DataFrame"""
        issues = []
        schema_issues = []
        
        # Basic statistics
        total_rows = len(df)
        total_columns = len(df.columns)
        
        # Check for missing values
        missing_counts = df.isnull().sum()
        missing_total = missing_counts.sum()
        
        if missing_total > 0:
            for col, count in missing_counts.items():
                if count > 0:
                    percentage = (count / total_rows) * 100
                    issues.append({
                        'type': DataQualityIssue.MISSING_VALUES,
                        'column': col,
                        'count': int(count),
                        'percentage': percentage,
                        'severity': 'high' if percentage > 50 else 'medium' if percentage > 20 else 'low'
                    })
        
        # Check for duplicate rows
        duplicate_rows = df.duplicated().sum()
        if duplicate_rows > 0:
            percentage = (duplicate_rows / total_rows) * 100
            issues.append({
                'type': DataQualityIssue.DUPLICATE_ROWS,
                'count': int(duplicate_rows),
                'percentage': percentage,
                'severity': 'high' if percentage > 30 else 'medium' if percentage > 10 else 'low'
            })
        
        # Check for outliers (using IQR method)
        outlier_count = 0
        for col in df.select_dtypes(include=[np.number]).columns:
            Q1 = df[col].quantile(0.25)
            Q3 = df[col].quantile(0.75)
            IQR = Q3 - Q1
            lower_bound = Q1 - 1.5 * IQR
            upper_bound = Q3 + 1.5 * IQR
            col_outliers = ((df[col] < lower_bound) | (df[col] > upper_bound)).sum()
            outlier_count += col_outliers
        
        if outlier_count > 0:
            issues.append({
                'type': DataQualityIssue.OUTLIERS,
                'count': int(outlier_count),
                'severity': 'medium'
            })
        
        # Check schema consistency
        for col in df.columns:
            dtype = df[col].dtype
            if dtype == 'object':
                # Check for mixed types
                try:
                    pd.to_numeric(df[col], errors='coerce').isnull().any()
                    # If there are numeric values, it's likely mixed
                    if df[col].apply(lambda x: isinstance(x, (int, float))).any():
                        schema_issues.append({
                            'type': 'MIXED_TYPES',
                            'column': col,
                            'description': f'Column has mixed types: {dtype}'
                        })
                except:
                    pass
            
            # Check for empty strings
            if dtype == 'object':
                empty_strings = (df[col] == '').sum()
                if empty_strings > 0:
                    schema_issues.append({
                        'type': 'EMPTY_STRINGS',
                        'column': col,
                        'count': int(empty_strings),
                        'description': f'Column has {empty_strings} empty strings'
                    })
        
        # Check for null constraint violations
        for rule in self.validation_rules:
            if not rule.allow_null and rule.column in df.columns:
                null_count = df[rule.column].isnull().sum()
                if null_count > 0:
                    issues.append({
                        'type': DataQualityIssue.NULL_CONSTRAINT_VIOLATION,
                        'column': rule.column,
                        'count': int(null_count),
                        'description': f'Null values violate validation rule: {rule.error_message}'
                    })
        
        # Calculate quality score
        quality_score = self._calculate_quality_score(df, issues, schema_issues)
        
        # Generate recommendations
        recommendations = self._generate_recommendations(issues, schema_issues)
        
        return DataQualityReport(
            file_path=file_path,
            format=DataFormat.JSON,  # Default for DataFrame
            total_rows=total_rows,
            total_columns=total_columns,
            missing_values=missing_total,
            duplicate_rows=duplicate_rows,
            outlier_count=outlier_count,
            schema_issues=schema_issues,
            quality_score=quality_score,
            recommendations=recommendations,
            issues=[issue if isinstance(issue, dict) else {'type': str(issue), 'description': 'Quality issue'} for issue in issues]
        )
    
    def _assess_array_quality(self, arr: np.ndarray) -> DataQualityReport:
        """Assess quality of numpy array"""
        total_elements = arr.size
        
        # Check for NaN values
        nan_count = np.isnan(arr).sum()
        
        # Check for Inf values
        inf_count = np.isinf(arr).sum()
        
        issues = []
        
        if nan_count > 0:
            percentage = (nan_count / total_elements) * 100
            issues.append({
                'type': DataQualityIssue.MISSING_VALUES,
                'count': int(nan_count),
                'percentage': percentage,
                'severity': 'high' if percentage > 50 else 'medium' if percentage > 20 else 'low'
            })
        
        if inf_count > 0:
            percentage = (inf_count / total_elements) * 100
            issues.append({
                'type': DataQualityIssue.OUTLIERS,
                'count': int(inf_count),
                'percentage': percentage,
                'severity': 'medium'
            })
        
        # Calculate quality score
        quality_score = 100.0
        if nan_count > 0:
            quality_score -= min(50, (nan_count / total_elements) * 100)
        if inf_count > 0:
            quality_score -= min(30, (inf_count / total_elements) * 100)
        
        recommendations = self._generate_recommendations(issues, [])
        
        return DataQualityReport(
            file_path="",
            format=DataFormat.JSON,
            total_rows=arr.shape[0] if arr.ndim > 1 else 1,
            total_columns=arr.shape[1] if arr.ndim > 1 else 1,
            missing_values=int(nan_count),
            duplicate_rows=0,
            outlier_count=int(inf_count),
            schema_issues=[],
            quality_score=quality_score,
            recommendations=recommendations,
            issues=issues
        )
    
    def _calculate_quality_score(self, df: pd.DataFrame, issues: List[Dict], 
                                schema_issues: List[Dict]) -> float:
        """Calculate overall data quality score"""
        base_score = 100.0
        
        # Deduct for missing values
        if issues:
            missing_penalty = sum(issue['percentage'] for issue in issues 
                                if issue.get('type') == DataQualityIssue.MISSING_VALUES) / 100
            base_score -= min(50, missing_penalty * 100)
        
        # Deduct for duplicates
        if any(issue.get('type') == DataQualityIssue.DUPLICATE_ROWS for issue in issues):
            duplicate_penalty = next((issue['percentage'] for issue in issues 
                                    if issue.get('type') == DataQualityIssue.DUPLICATE_ROWS), 0) / 100
            base_score -= min(30, duplicate_penalty * 100)
        
        # Deduct for schema issues
        schema_penalty = min(20, len(schema_issues) * 4)
        base_score -= schema_penalty
        
        return max(0.0, base_score)
    
    def _generate_recommendations(self, issues: List[Dict], schema_issues: List[Dict]) -> List[str]:
        """Generate recommendations based on issues"""
        recommendations = []
        
        for issue in issues:
            if issue.get('type') == DataQualityIssue.MISSING_VALUES:
                recommendations.append(
                    f"Fill missing values in column '{issue.get('column', 'unknown')}' "
                    f"using appropriate imputation strategy"
                )
            
            elif issue.get('type') == DataQualityIssue.DUPLICATE_ROWS:
                recommendations.append(
                    "Remove duplicate rows to improve data quality"
                )
            
            elif issue.get('type') == DataQualityIssue.OUTLIERS:
                recommendations.append(
                    "Review and handle outliers using robust statistical methods"
                )
        
        for schema_issue in schema_issues:
            if schema_issue.get('type') == 'MIXED_TYPES':
                recommendations.append(
                    f"Standardize data types in column '{schema_issue.get('column', 'unknown')}'"
                )
            
            elif schema_issue.get('type') == 'EMPTY_STRINGS':
                recommendations.append(
                    f"Clean empty strings in column '{schema_issue.get('column', 'unknown')}'"
                )
        
        if not recommendations:
            recommendations.append("Data quality is good. Keep up the good work!")
        
        return recommendations
class DataValidator:
    """Validate neural data against rules"""
    
    def __init__(self, rules: Optional[List[ValidationRule]] = None):
        self.rules = rules or []
    
    def add_rule(self, rule: ValidationRule):
        """Add validation rule"""
        self.rules.append(rule)
    
    def validate_data(self, data: Union[str, pd.DataFrame, np.ndarray],
                     format: Optional[DataFormat] = None) -> Dict[str, Any]:
        """Validate data against rules"""
        validation_results = {
            'passed': True,
            'errors': [],
            'warnings': [],
            'data': None
        }
        
        try:
            # Load data
            if isinstance(data, str):
                if format == DataFormat.CSV:
                    df = pd.read_csv(data)
                elif format == DataFormat.JSON:
                    df = pd.read_json(data)
                elif format == DataFormat.HDF5:
                    with h5py.File(data, 'r') as f:
                        data_dict = {}
                        for key in f.keys():
                            if isinstance(f[key], h5py.Dataset):
                                data_dict[key] = f[key][()]
                        df = pd.DataFrame(data_dict)
                else:
                    df = pd.read_csv(data)  # fallback
            elif isinstance(data, pd.DataFrame):
                df = data.copy()
            elif isinstance(data, np.ndarray):
                df = pd.DataFrame(data)
            else:
                raise ValueError("Unsupported data type")
            
            validation_results['data'] = df
            
            # Apply validation rules
            for rule in self.rules:
                rule_errors = self._apply_rule(df, rule)
                validation_results['errors'].extend(rule_errors)
            
            # Check for errors
            validation_results['passed'] = len(validation_results['errors']) == 0
            
        except Exception as e:
            validation_results['errors'].append({
                'rule': 'DATA_LOADING',
                'message': f"Error loading data: {str(e)}",
                'severity': 'high'
            })
            validation_results['passed'] = False
        
        return validation_results
    
    def _apply_rule(self, df: pd.DataFrame, rule: ValidationRule) -> List[Dict[str, Any]]:
        """Apply single validation rule"""
        errors = []
        
        if rule.column not in df.columns:
            if rule.is_required:
                errors.append({
                    'rule': rule.rule_id,
                    'message': f"Required column '{rule.column}' is missing",
                    'severity': 'high'
                })
            return errors
        
        column_data = df[rule.column]
        
        # Check null values
        null_count = column_data.isnull().sum()
        if null_count > 0 and not rule.allow_null:
            errors.append({
                'rule': rule.rule_id,
                'message': f"Column '{rule.column}' has {null_count} null values, but nulls are not allowed",
                'severity': 'high'
            })
        
        # Apply specific validation based on rule type
        if rule.rule_type == 'range':
            min_val = rule.parameters.get('min')
            max_val = rule.parameters.get('max')
            
            if min_val is not None:
                below_min = (column_data < min_val).sum()
                if below_min > 0:
                    errors.append({
                        'rule': rule.rule_id,
                        'message': f"Column '{rule.column}' has {below_min} values below minimum {min_val}",
                        'severity': 'medium'
                    })
            
            if max_val is not None:
                above_max = (column_data > max_val).sum()
                if above_max > 0:
                    errors.append({
                        'rule': rule.rule_id,
                        'message': f"Column '{rule.column}' has {above_max} values above maximum {max_val}",
                        'severity': 'medium'
                    })
        
        elif rule.rule_type == 'pattern':
            pattern = rule.parameters.get('pattern')
            if pattern:
                invalid = ~column_data.astype(str).str.contains(pattern, na=False)
                invalid_count = invalid.sum()
                if invalid_count > 0:
                    errors.append({
                        'rule': rule.rule_id,
                        'message': f"Column '{rule.column}' has {invalid_count} values that don't match pattern '{pattern}'",
                        'severity': 'medium'
                    })
        
        elif rule.rule_type == 'enum':
            allowed_values = rule.parameters.get('allowed_values', [])
            invalid = ~column_data.isin(allowed_values)
            invalid_count = invalid.sum()
            if invalid_count > 0:
                errors.append({
                    'rule': rule.rule_id,
                    'message': f"Column '{rule.column}' has {invalid_count} values not in allowed list {allowed_values}",
                    'severity': 'medium'
                })
        
        elif rule.rule_type == 'unique':
            duplicate_count = column_data.duplicated().sum()
            if duplicate_count > 0:
                errors.append({
                    'rule': rule.rule_id,
                    'message': f"Column '{rule.column}' has {duplicate_count} duplicate values",
                    'severity': 'low'
                })
        
        return errors
class DataVersionManager:
    """Manage data versioning for neural datasets"""
    
    def __init__(self, storage_path: str = "./data_versions"):
        self.storage_path = Path(storage_path)
        self.storage_path.mkdir(exist_ok=True)
        self.version_registry = {}
    
    def create_version(self, dataset_id: str, file_path: str, metadata: Dict[str, Any],
                      created_by: str, parent_version_id: Optional[str] = None) -> VersionInfo:
        """Create new version of dataset"""
        try:
            # Calculate checksum
            with open(file_path, 'rb') as f:
                file_content = f.read()
                checksum = hashlib.sha256(file_content).hexdigest()
            
            # Determine version number
            dataset_versions = [v for v in self.version_registry.values() 
                              if v.dataset_id == dataset_id]
            version_number = len(dataset_versions) + 1
            
            # Create version ID
            version_id = f"{dataset_id}_v{version_number}"
            
            # Copy file to version directory
            version_dir = self.storage_path / dataset_id / f"v{version_number}"
            version_dir.mkdir(parents=True, exist_ok=True)
            
            shutil.copy2(file_path, version_dir / Path(file_path).name)
            
            # Create version info
            version_info = VersionInfo(
                version_id=version_id,
                dataset_id=dataset_id,
                version_number=version_number,
                created_at=datetime.now(timezone.utc),
                created_by=created_by,
                file_path=str(version_dir / Path(file_path).name),
                checksum=checksum,
                metadata=metadata,
                parent_version_id=parent_version_id
            )
            
            # Register version
            self.version_registry[version_id] = version_info
            
            return version_info
            
        except Exception as e:
            raise VersionError(f"Failed to create version: {str(e)}")
    
    def get_version(self, version_id: str) -> Optional[VersionInfo]:
        """Get version by ID"""
        return self.version_registry.get(version_id)
    
    def get_dataset_versions(self, dataset_id: str) -> List[VersionInfo]:
        """Get all versions for a dataset"""
        return [v for v in self.version_registry.values() if v.dataset_id == dataset_id]
    
    def compare_versions(self, version_id1: str, version_id2: str) -> Dict[str, Any]:
        """Compare two versions"""
        v1 = self.get_version(version_id1)
        v2 = self.get_version(version_id2)
        
        if not v1 or not v2:
            raise VersionError("One or both versions not found")
        
        # Calculate checksum differences
        checksums_different = v1.checksum != v2.checksum
        
        # Calculate metadata differences
        metadata_diff = {}
        for key in set(v1.metadata.keys()) | set(v2.metadata.keys()):
            if key not in v1.metadata:
                metadata_diff[key] = {'added': v2.metadata[key]}
            elif key not in v2.metadata:
                metadata_diff[key] = {'removed': v1.metadata[key]}
            elif v1.metadata[key] != v2.metadata[key]:
                metadata_diff[key] = {
                    'old': v1.metadata[key],
                    'new': v2.metadata[key]
                }
        
        return {
            'version1_id': version_id1,
            'version2_id': version_id2,
            'checksums_different': checksums_different,
            'metadata_differences': metadata_diff,
            'parent1': v1.parent_version_id,
            'parent2': v2.parent_version_id,
            'created1': v1.created_at,
            'created2': v2.created_at,
            'created_by1': v1.created_by,
            'created_by2': v2.created_by
        }
    
    def restore_version(self, version_id: str, target_path: str) -> bool:
        """Restore version to target path"""
        version = self.get_version(version_id)
        
        if not version:
            return False
        
        try:
            shutil.copy2(version.file_path, target_path)
            return True
        except Exception:
            return False
    
    def export_version(self, version_id: str, export_path: str) -> bool:
        """Export version to another location"""
        version = self.get_version(version_id)
        
        if not version:
            return False
        
        try:
            export_file = Path(export_path) / f"{version.dataset_id}_v{version.version_number}.hdf5"
            shutil.copy2(version.file_path, export_file)
            return True
        except Exception:
            return False
class DataCompressor:
    """Compress and decompress neural data"""
    
    def __init__(self, compression_level: int = 6):
        self.compression_level = compression_level
        self.supported_algorithms = ['gzip', 'lzma', 'zstd', 'blosc']
    
    def compress_data(self, data: Union[str, np.ndarray, pd.DataFrame],
                     output_path: str, algorithm: str = 'gzip') -> CompressionInfo:
        """Compress data using specified algorithm"""
        try:
            # Get original size
            if isinstance(data, str):
                original_size = os.path.getsize(data)
            elif isinstance(data, pd.DataFrame):
                original_size = data.memory_usage(deep=True).sum()
            elif isinstance(data, np.ndarray):
                original_size = data.nbytes
            else:
                raise ValueError("Unsupported data type for compression")
            
            # Compress based on algorithm
            if algorithm == 'gzip':
                compressed_data = self._compress_gzip(data, output_path)
            elif algorithm == 'lzma':
                compressed_data = self._compress_lzma(data, output_path)
            elif algorithm == 'zstd':
                compressed_data = self._compress_zstd(data, output_path)
            elif algorithm == 'blosc':
                compressed_data = self._compress_blosc(data, output_path)
            else:
                raise ValueError(f"Unsupported compression algorithm: {algorithm}")
            
            # Get compressed size
            compressed_size = os.path.getsize(output_path)
            
            # Calculate compression ratio
            compression_ratio = (1 - compressed_size / original_size) * 100 if original_size > 0 else 0
            
            return CompressionInfo(
                original_size=original_size,
                compressed_size=compressed_size,
                compression_ratio=compression_ratio,
                algorithm=algorithm,
                compressed_path=output_path,
                decompression_info={'algorithm': algorithm, 'compression_level': self.compression_level}
            )
            
        except Exception as e:
            raise CompressionError(f"Failed to compress data: {str(e)}")
    
    def decompress_data(self, input_path: str, output_path: str, algorithm: str = 'gzip') -> bool:
        """Decompress data"""
        try:
            if algorithm == 'gzip':
                self._decompress_gzip(input_path, output_path)
            elif algorithm == 'lzma':
                self._decompress_lzma(input_path, output_path)
            elif algorithm == 'zstd':
                self._decompress_zstd(input_path, output_path)
            elif algorithm == 'blosc':
                self._decompress_blosc(input_path, output_path)
            else:
                raise ValueError(f"Unsupported decompression algorithm: {algorithm}")
            
            return True
            
        except Exception as e:
            raise CompressionError(f"Failed to decompress data: {str(e)}")
    
    def _compress_gzip(self, data: Union[str, np.ndarray, pd.DataFrame], 
                       output_path: str) -> bytes:
        """Compress using gzip algorithm"""
        if isinstance(data, str):
            with open(data, 'rb') as f:
                file_data = f.read()
        elif isinstance(data, pd.DataFrame):
            file_data = pickle.dumps(data)
        elif isinstance(data, np.ndarray):
            file_data = data.tobytes()
        else:
            raise ValueError("Unsupported data type")
        
        compressed_data = zlib.compress(file_data, self.compression_level)
        
        with open(output_path, 'wb') as f:
            f.write(compressed_data)
        
        return compressed_data
    
    def _decompress_gzip(self, input_path: str, output_path: str):
        """Decompress using gzip algorithm"""
        with open(input_path, 'rb') as f:
            compressed_data = f.read()
        
        decompressed_data = zlib.decompress(compressed_data)
        
        # Try to infer original format and save appropriately
        try:
            # Try to load as pickle (for DataFrame)
            data = pickle.loads(decompressed_data)
            data.to_hdf(output_path, key='data', format='table')
        except:
            # Try to load as numpy array
            try:
                data = np.frombuffer(decompressed_data, dtype=np.uint8)
                data.tofile(output_path + '.bin')
            except:
                # Save as raw bytes
                with open(output_path, 'wb') as f:
                    f.write(decompressed_data)
    
    def _compress_lzma(self, data: Union[str, np.ndarray, pd.DataFrame], 
                       output_path: str) -> bytes:
        """Compress using LZMA algorithm"""
        try:
            import lzma
            
            if isinstance(data, str):
                with open(data, 'rb') as f:
                    file_data = f.read()
            elif isinstance(data, pd.DataFrame):
                file_data = pickle.dumps(data)
            elif isinstance(data, np.ndarray):
                file_data = data.tobytes()
            else:
                raise ValueError("Unsupported data type")
            
            compressed_data = lzma.compress(file_data, preset=self.compression_level)
            
            with open(output_path, 'wb') as f:
                f.write(compressed_data)
            
            return compressed_data
        except ImportError:
            raise CompressionError("LZMA compression not available. Install with: pip install lzma")
    
    def _decompress_lzma(self, input_path: str, output_path: str):
        """Decompress using LZMA algorithm"""
        try:
            import lzma
            
            with open(input_path, 'rb') as f:
                compressed_data = f.read()
            
            decompressed_data = lzma.decompress(compressed_data)
            
            # Save as appropriate format
            with open(output_path, 'wb') as f:
                f.write(decompressed_data)
                
        except ImportError:
            raise CompressionError("LZMA decompression not available. Install with: pip install lzma")
    
    def _compress_zstd(self, data: Union[str, np.ndarray, pd.DataFrame], 
                       output_path: str) -> bytes:
        """Compress using zstd algorithm"""
        try:
            import zstd
            
            if isinstance(data, str):
                with open(data, 'rb') as f:
                    file_data = f.read()
            elif isinstance(data, pd.DataFrame):
                file_data = pickle.dumps(data)
            elif isinstance(data, np.ndarray):
                file_data = data.tobytes()
            else:
                raise ValueError("Unsupported data type")
            
            compressed_data = zstd.compress(file_data)
            
            with open(output_path, 'wb') as f:
                f.write(compressed_data)
            
            return compressed_data
        except ImportError:
            raise CompressionError("Zstd compression not available. Install with: pip install zstd")
    
    def _decompress_zstd(self, input_path: str, output_path: str):
        """Decompress using zstd algorithm"""
        try:
            import zstd
            
            with open(input_path, 'rb') as f:
                compressed_data = f.read()
            
            decompressed_data = zstd.decompress(compressed_data)
            
            # Save as appropriate format
            with open(output_path, 'wb') as f:
                f.write(decompressed_data)
                
        except ImportError:
            raise CompressionError("Zstd decompression not available. Install with: pip install zstd")
    
    def _compress_blosc(self, data: Union[str, np.ndarray, pd.DataFrame], 
                        output_path: str) -> bytes:
        """Compress using blosc algorithm"""
        try:
            import blosc
            
            if isinstance(data, str):
                with open(data, 'rb') as f:
                    file_data = f.read()
            elif isinstance(data, pd.DataFrame):
                file_data = pickle.dumps(data)
            elif isinstance(data, np.ndarray):
                file_data = data.tobytes()
            else:
                raise ValueError("Unsupported data type")
            
            compressed_data = blosc.compress(file_data, cname='zstd', compression=9)
            
            with open(output_path, 'wb') as f:
                f.write(compressed_data)
            
            return compressed_data
        except ImportError:
            raise CompressionError("Blosc compression not available. Install with: pip install blosc")
    
    def _decompress_blosc(self, input_path: str, output_path: str):
        """Decompress using blosc algorithm"""
        try:
            import blosc
            
            with open(input_path, 'rb') as f:
                compressed_data = f.read()
            
            decompressed_data = blosc.decompress(compressed_data)
            
            # Save as appropriate format
            with open(output_path, 'wb') as f:
                f.write(decompressed_data)
                
        except ImportError:
            raise CompressionError("Blosc decompression not available. Install with: pip install blosc")
class AdvancedDataManager:
    """Main advanced data manager for neural data"""
    
    def __init__(self):
        self.database_manager = DatabaseManager()
        self.pipeline = DataPipeline()
        self.data_quality_assessor = DataQualityAssessor()
        self.data_validator = DataValidator()
        self.version_manager = DataVersionManager()
        self.compressor = DataCompressor()
    
    def store_simulation_data(self, run_id: str, brain_config: Dict, world_config: Dict,
                              duration: float, target_fps: int, start_time: datetime,
                              end_time: datetime, status: str, neurons: List[Dict],
                              spikes: List[Dict], metadata: Dict = None) -> bool:
        """Store complete simulation data"""
        try:
            # Store simulation run
            if not self.database_manager.store_simulation_run(
                run_id, brain_config, world_config, duration, target_fps,
                start_time, end_time, status, metadata
            ):
                return False
            
            # Store neuron data
            if not self.database_manager.store_neuron_data(run_id, neurons):
                return False
            
            # Store spike data
            if not self.database_manager.store_spike_data(run_id, spikes):
                return False
            
            return True
            
        except Exception as e:
            print(f"Error storing simulation data: {e}")
            return False
    
    def analyze_data_quality(self, data: Union[str, pd.DataFrame, np.ndarray],
                            format: Optional[DataFormat] = None) -> DataQualityReport:
        """Analyze data quality"""
        return self.data_quality_assessor.assess_data_quality(data, format)
    
    def validate_data(self, data: Union[str, pd.DataFrame, np.ndarray],
                     rules: Optional[List[ValidationRule]] = None) -> Dict[str, Any]:
        """Validate data against rules"""
        validator = DataValidator(rules) if rules else self.data_validator
        return validator.validate_data(data)
    
    def process_neural_pipeline(self, input_data: Union[str, np.ndarray, pd.DataFrame],
                                callback: Optional[Callable] = None) -> Dict[str, Any]:
        """Process neural data through pipeline"""
        return self.pipeline.process_data(input_data, callback)
    
    def create_data_version(self, dataset_id: str, file_path: str, metadata: Dict[str, Any],
                           created_by: str, parent_version_id: Optional[str] = None) -> VersionInfo:
        """Create data version"""
        return self.version_manager.create_version(
            dataset_id, file_path, metadata, created_by, parent_version_id
        )
    
    def compress_neural_data(self, data: Union[str, np.ndarray, pd.DataFrame],
                            output_path: str, algorithm: str = 'gzip') -> CompressionInfo:
        """Compress neural data"""
        return self.compressor.compress_data(data, output_path, algorithm)
    
    def get_database_stats(self) -> Dict[str, Any]:
        """Get database statistics"""
        try:
            connection = sqlite3.connect('neural_data.db')
            cursor = connection.cursor()
            
            stats = {}
            
            # Get table counts
            tables = ['simulation_runs', 'neurons', 'spikes', 'brain_regions', 'world_objects', 'analysis_results']
            
            for table in tables:
                cursor.execute(f"SELECT COUNT(*) FROM {table}")
                count = cursor.fetchone()[0]
                stats[f'{table}_count'] = count
            
            # Get simulation runs summary
            cursor.execute('SELECT status, COUNT(*) FROM simulation_runs GROUP BY status')
            status_stats = dict(cursor.fetchall())
            stats['simulation_status_counts'] = status_stats
            
            cursor.close()
            connection.close()
            
            return stats
            
        except Exception as e:
            print(f"Error getting database stats: {e}")
            return {}

# Convenience functions

def create_data_manager() -> AdvancedDataManager:
    """Convenience function to create data manager"""
    return AdvancedDataManager()

def store_simulation_data(database_manager: DatabaseManager, run_id: str,
                          brain_config: Dict, world_config: Dict, duration: float,
                          target_fps: int, start_time: datetime, end_time: datetime,
                          status: str, neurons: List[Dict], spikes: List[Dict],
                          metadata: Dict = None) -> bool:
    """Convenience function to store simulation data"""
    manager = AdvancedDataManager()
    return manager.store_simulation_data(
        run_id, brain_config, world_config, duration, target_fps,
        start_time, end_time, status, neurons, spikes, metadata
    )

def analyze_data_quality(data: Union[str, pd.DataFrame, np.ndarray],
                        format: Optional[DataFormat] = None) -> DataQualityReport:
    """Convenience function to analyze data quality"""
    manager = AdvancedDataManager()
    return manager.analyze_data_quality(data, format)

# Export all classes and functions
__all__ = [
    'DatabaseType', 'DatabaseConfig', 'DataFormat', 'DataQualityIssue',
    'PipelineConfig', 'DataQualityReport', 'ValidationRule', 'VersionInfo',
    'CompressionInfo', 'DatabaseManager', 'PostgreSQLManager',
    'DatabaseConnectionError', 'VersionError', 'CompressionError',
    'DataPipeline', 'DataQualityAssessor', 'DataValidator', 'DataVersionManager',
    'DataCompressor', 'AdvancedDataManager',
    'create_data_manager', 'store_simulation_data', 'analyze_data_quality'
]