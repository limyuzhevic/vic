#!/usr/bin/env python3
"""
NLM Data Processor
Data processing utilities for NLM simulation data
"""

import pynlm
import pandas as pd
import numpy as np
import json
import time
from typing import Dict, List, Optional, Any, Union, Callable
from dataclasses import dataclass, field
from enum import Enum
import logging
from pathlib import Path
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.decomposition import PCA, IncrementalPCA
from sklearn.cluster import KMeans
from sklearn.model_selection import train_test_split
import warnings
warnings.filterwarnings('ignore')

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class DataType(Enum):
    """Types of NLM data"""
    NEURON_DATA = "neuron_data"
    SYNAPSE_DATA = "synapse_data"
    REGIONAL_DATA = "regional_data"
    SPATIAL_DATA = "spatial_data"
    TEMPORAL_DATA = "temporal_data"
    BEHAVIORAL_DATA = "behavioral_data"
    PERFORMANCE_DATA = "performance_data"

class DataFormat(Enum):
    """Data formats"""
    RAW = "raw"
    PROCESSED = "processed"
    ANALYZED = "analyzed"
    MODEL_INPUT = "model_input"
    EXPORT = "export"

@dataclass
class DataFrameInfo:
    """Information about a DataFrame"""
    name: str
    shape: Tuple[int, int]
    columns: List[str]
    dtypes: Dict[str, str]
    memory_usage: Dict[str, int]
    missing_values: Dict[str, int]
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class ProcessingResult:
    """Result of data processing"""
    success: bool
    original_data: pd.DataFrame
    processed_data: pd.DataFrame
    transformations: List[str]
    metadata: Dict[str, Any] = field(default_factory=dict)
    errors: List[str] = field(default_factory=list)

class DataValidator:
    """Validate and clean NLM data"""
    
    def __init__(self):
        self.validation_rules = {}
    
    def add_rule(self, field_name: str, rule_type: str, parameters: Dict = None):
        """
        Add a validation rule for a field.
        
        Args:
            field_name: Name of the field to validate
            rule_type: Type of validation rule
            parameters: Parameters for the rule
        """
        if parameters is None:
            parameters = {}
        
        self.validation_rules[field_name] = {
            'type': rule_type,
            'parameters': parameters
        }
    
    def validate_neuron_data(self, neuron_df: pd.DataFrame) -> Dict[str, Any]:
        """
        Validate neuron data.
        
        Args:
            neuron_df: DataFrame with neuron data
            
        Returns:
            Dictionary with validation results
        """
        results = {
            'is_valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        # Required columns
        required_columns = ['neuron_id', 'region_id', 'neuron_type', 'membrane_potential', 'firing_state']
        missing_columns = [col for col in required_columns if col not in neuron_df.columns]
        if missing_columns:
            results['is_valid'] = False
            results['errors'].append(f"Missing required columns: {missing_columns}")
        
        # Check data types
        expected_types = {
            'neuron_id': 'int64',
            'region_id': 'int64',
            'neuron_type': 'object',
            'membrane_potential': 'float64',
            'firing_state': 'int64'
        }
        
        for col, expected_type in expected_types.items():
            if col in neuron_df.columns:
                actual_type = str(neuron_df[col].dtype)
                if expected_type not in actual_type:
                    results['warnings'].append(
                        f"Column '{col}' has type {actual_type}, expected {expected_type}"
                    )
        
        # Check for invalid values
        if 'firing_state' in neuron_df.columns:
            invalid_states = neuron_df[~neuron_df['firing_state'].isin([0, 1, 2, 3])]
            if not invalid_states.empty:
                results['errors'].append(
                    f"Invalid firing states found in {len(invalid_states)} rows"
                )
        
        # Check for missing values
        missing_counts = neuron_df.isnull().sum()
        for col, count in missing_counts.items():
            if count > 0:
                results['warnings'].append(
                    f"Column '{col}' has {count} missing values"
                )
        
        # Check for duplicate neuron IDs
        if 'neuron_id' in neuron_df.columns:
            duplicate_ids = neuron_df[neuron_df.duplicated(subset=['neuron_id'])]
            if not duplicate_ids.empty:
                results['errors'].append(
                    f"Found {len(duplicate_ids)} duplicate neuron IDs"
                )
        
        return results
    
    def validate_time_series_data(self, time_series_df: pd.DataFrame, time_col: str = 'step') -> Dict[str, Any]:
        """
        Validate time series data.
        
        Args:
            time_series_df: DataFrame with time series data
            time_col: Name of time column
            
        Returns:
            Dictionary with validation results
        """
        results = {
            'is_valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        if time_col not in time_series_df.columns:
            results['warnings'].append(f"Time column '{time_col}' not found")
            # Try to find a suitable time column
            possible_time_cols = [col for col in time_series_df.columns 
                                if 'time' in col.lower() or 'step' in col.lower() or 'frame' in col.lower()]
            if possible_time_cols:
                results['suggestions'].append(f"Consider using column '{possible_time_cols[0]}' as time column")
        
        # Check for duplicate time points
        if time_col in time_series_df.columns:
            duplicate_times = time_series_df[time_series_df.duplicated(subset=[time_col])]
            if not duplicate_times.empty:
                results['warnings'].append(
                    f"Found {len(duplicate_times)} duplicate time points"
                )
        
        # Check for monotonic time values
        if time_col in time_series_df.columns:
            time_values = time_series_df[time_col].dropna()
            if not time_values.empty:
                if not (time_values.diff().dropna() >= 0).all():
                    results['warnings'].append("Time values are not monotonically increasing")
        
        return results
    
    def validate_spatial_data(self, spatial_df: pd.DataFrame) -> Dict[str, Any]:
        """
        Validate spatial data.
        
        Args:
            spatial_df: DataFrame with spatial data
            
        Returns:
            Dictionary with validation results
        """
        results = {
            'is_valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        # Check for required spatial coordinates
        required_coords = ['x', 'y', 'z']
        missing_coords = [col for col in required_coords if col not in spatial_df.columns]
        if missing_coords:
            results['warnings'].append(f"Missing coordinate columns: {missing_coords}")
        
        # Check coordinate ranges
        for coord in required_coords:
            if coord in spatial_df.columns:
                coord_values = spatial_df[coord].dropna()
                if not coord_values.empty:
                    min_val = coord_values.min()
                    max_val = coord_values.max()
                    if min_val < 0:
                        results['warnings'].append(f"Coordinate '{coord}' has negative values (min: {min_val})")
                    if max_val > 1000:  # Assuming world coordinates should be reasonable
                        results['warnings'].append(f"Coordinate '{coord}' has large values (max: {max_val})")
        
        return results
class DataNormalizer:
    """Normalize and scale NLM data"""
    
    def __init__(self):
        self.scalers = {}
        self.pca = None
        self.is_fitted = False
    
    def fit_scalers(self, data: pd.DataFrame, numeric_columns: List[str] = None):
        """
        Fit scalers for numeric columns.
        
        Args:
            data: DataFrame with data to fit
            numeric_columns: List of numeric columns to scale
        """
        if numeric_columns is None:
            numeric_columns = data.select_dtypes(include=[np.number]).columns.tolist()
        
        for column in numeric_columns:
            if column in data.columns:
                # Use MinMaxScaler for most features, StandardScaler for others
                if 'firing' in column.lower() or 'rate' in column.lower() or 'frequency' in column.lower():
                    self.scalers[column] = MinMaxScaler(feature_range=(0, 1))
                else:
                    self.scalers[column] = StandardScaler()
                
                # Fit scaler
                self.scalers[column].fit(data[[column]].dropna())
        
        self.is_fitted = True
    
    def transform_data(self, data: pd.DataFrame, columns: List[str] = None) -> pd.DataFrame:
        """
        Transform data using fitted scalers.
        
        Args:
            data: DataFrame with data to transform
            columns: List of columns to transform (None for all)
            
        Returns:
            Transformed DataFrame
        """
        if not self.is_fitted:
            raise ValueError("Scalers not fitted. Call fit_scalers first.")
        
        result = data.copy()
        transform_columns = columns if columns else list(self.scalers.keys())
        
        for column in transform_columns:
            if column in self.scalers and column in result.columns:
                try:
                    result[column] = self.scalers[column].transform(result[[column]].dropna())
                except Exception as e:
                    logger.warning(f"Failed to transform column {column}: {e}")
        
        return result
    
    def fit_transform(self, data: pd.DataFrame, numeric_columns: List[str] = None) -> pd.DataFrame:
        """
        Fit scalers and transform data.
        
        Args:
            data: DataFrame with data to process
            numeric_columns: List of numeric columns to scale
            
        Returns:
            Transformed DataFrame
        """
        self.fit_scalers(data, numeric_columns)
        return self.transform_data(data)
    
    def fit_pca(self, data: pd.DataFrame, n_components: int = 2, numeric_columns: List[str] = None):
        """
        Fit PCA for dimensionality reduction.
        
        Args:
            data: DataFrame with data to fit
            n_components: Number of principal components
            numeric_columns: List of numeric columns to use
        """
        if numeric_columns is None:
            numeric_columns = data.select_dtypes(include=[np.number]).columns.tolist()
        
        # Prepare data
        X = data[numeric_columns].dropna()
        
        # Fit PCA
        self.pca = PCA(n_components=n_components)
        self.pca.fit(X)
        
        return self.pca
    
    def transform_pca(self, data: pd.DataFrame, numeric_columns: List[str] = None) -> pd.DataFrame:
        """
        Transform data using fitted PCA.
        
        Args:
            data: DataFrame with data to transform
            numeric_columns: List of numeric columns to transform
            
        Returns:
            Transformed DataFrame with PCA components
        """
        if self.pca is None:
            raise ValueError("PCA not fitted. Call fit_pca first.")
        
        if numeric_columns is None:
            numeric_columns = list(self.pca.feature_names_in_)
        
        # Prepare data
        X = data[numeric_columns].dropna()
        
        # Transform
        X_transformed = self.pca.transform(X)
        
        # Create DataFrame with component columns
        component_columns = [f'pc{i+1}' for i in range(X_transformed.shape[1])]
        result = pd.DataFrame(X_transformed, columns=component_columns, index=X.index)
        
        return result
    
    def get_feature_importance(self) -> Dict[str, float]:
        """
        Get feature importance from PCA.
        
        Returns:
            Dictionary with feature importance scores
        """
        if self.pca is None:
            raise ValueError("PCA not fitted. Call fit_pca first.")
        
        # Get explained variance ratio
        explained_variance = self.pca.explained_variance_ratio_
        
        # Distribute importance to original features
        feature_importance = {}
        n_features = len(self.pca.feature_names_in_)
        
        for i, feature in enumerate(self.pca.feature_names_in_):
            # Calculate importance as sum of loadings squared for each component
            component_importance = np.sum(self.pca.components_[i] ** 2)
            feature_importance[feature] = component_importance
        
        return feature_importance
class DataAggregator:
    """Aggregate NLM data at different levels"""
    
    def aggregate_by_region(self, neuron_data: pd.DataFrame) -> pd.DataFrame:
        """
        Aggregate neuron data by region.
        
        Args:
            neuron_data: DataFrame with neuron data
            
        Returns:
            Aggregated DataFrame by region
        """
        if 'region_id' not in neuron_data.columns:
            raise ValueError("Region ID column not found in neuron data")
        
        # Group by region and aggregate
        region_stats = neuron_data.groupby('region_id').agg({
            'neuron_id': 'count',
            'membrane_potential': ['mean', 'std', 'min', 'max'],
            'firing_state': lambda x: (x == 1).sum() / len(x) * 100,  # Percentage firing
            'neuron_type': lambda x: (x == 'Excitatory').sum() / len(x) * 100
        }).round(4)
        
        # Flatten column multi-index
        region_stats.columns = ['_'.join(col).strip() if col[1] else col[0] for col in region_stats.columns]
        
        return region_stats
    
    def aggregate_by_time_step(self, time_series_data: pd.DataFrame, time_col: str = 'step') -> pd.DataFrame:
        """
        Aggregate time series data by time step.
        
        Args:
            time_series_data: DataFrame with time series data
            time_col: Name of time column
            
        Returns:
            Aggregated DataFrame by time step
        """
        if time_col not in time_series_data.columns:
            raise ValueError(f"Time column '{time_col}' not found")
        
        # Group by time step and compute statistics
        time_stats = time_series_data.groupby(time_col).agg({
            'value': ['mean', 'std', 'min', 'max', 'count']
        }).round(4)
        
        # Flatten column multi-index
        time_stats.columns = ['_'.join(col).strip() if col[1] else col[0] for col in time_stats.columns]
        
        return time_stats
    
    def create_neuron_populations(self, neuron_data: pd.DataFrame, 
                                population_definitions: Dict[str, Dict]) -> pd.DataFrame:
        """
        Create neuron populations based on definitions.
        
        Args:
            neuron_data: DataFrame with neuron data
            population_definitions: Dictionary defining populations
                                  (e.g., {'region_id': 1, 'neuron_type': 'Excitatory'})
            
        Returns:
            DataFrame with population assignments
        """
        populations = []
        
        for pop_name, criteria in population_definitions.items():
            # Filter neurons based on criteria
            filtered_neuron = neuron_data.copy()
            for column, value in criteria.items():
                filtered_neuron = filtered_neuron[filtered_neuron[column] == value]
            
            # Add population label
            filtered_neuron['population'] = pop_name
            populations.append(filtered_neuron)
        
        # Combine all populations
        if populations:
            return pd.concat(populations, ignore_index=True)
        else:
            return pd.DataFrame()
    
    def create_synaptic_network(self, neuron_data: pd.DataFrame, synapse_data: pd.DataFrame) -> pd.DataFrame:
        """
        Create synaptic network representation.
        
        Args:
            neuron_data: DataFrame with neuron data
            synapse_data: DataFrame with synapse data
            
        Returns:
            DataFrame representing synaptic network
        """
        # Merge neuron data for source and target neurons
        network = synapse_data.merge(
            neuron_data[['neuron_id', 'neuron_type', 'region_id']].rename(columns={
                'neuron_id': 'source_neuron_id',
                'neuron_type': 'source_neuron_type',
                'region_id': 'source_region_id'
            }),
            on='source_neuron_id',
            how='left'
        ).merge(
            neuron_data[['neuron_id', 'neuron_type', 'region_id']].rename(columns={
                'neuron_id': 'target_neuron_id',
                'neuron_type': 'target_neuron_type',
                'region_id': 'target_region_id'
            }),
            on='target_neuron_id',
            how='left'
        )
        
        return network
class DataExporter:
    """Export NLM data to various formats"""
    
    def to_csv(self, data: pd.DataFrame, filepath: str, include_index: bool = True):
        """
        Export data to CSV file.
        
        Args:
            data: DataFrame to export
            filepath: Output file path
            include_index: Whether to include DataFrame index
        """
        data.to_csv(filepath, index=include_index)
        logger.info(f"Exported data to CSV: {filepath}")
    
    def to_json(self, data: pd.DataFrame, filepath: str, orient: str = 'records'):
        """
        Export data to JSON file.
        
        Args:
            data: DataFrame to export
            filepath: Output file path
            orient: JSON orientation ('records', 'columns', 'index', 'values')
        """
        data.to_json(filepath, orient=orient, indent=2)
        logger.info(f"Exported data to JSON: {filepath}")
    
    def to_parquet(self, data: pd.DataFrame, filepath: str, compression: str = 'snappy'):
        """
        Export data to Parquet file.
        
        Args:
            data: DataFrame to export
            filepath: Output file path
            compression: Compression algorithm
        """
        data.to_parquet(filepath, compression=compression)
        logger.info(f"Exported data to Parquet: {filepath}")
    
    def to_hdf(self, data: pd.DataFrame, filepath: str, key: str = 'data'):
        """
        Export data to HDF5 file.
        
        Args:
            data: DataFrame to export
            filepath: Output file path
            key: HDF5 key name
        """
        data.to_hdf(filepath, key=key, mode='w')
        logger.info(f"Exported data to HDF5: {filepath}")
    
    def export_summary(self, data: pd.DataFrame, filepath: str):
        """
        Export data summary to JSON file.
        
        Args:
            data: DataFrame to analyze and summarize
            filepath: Output file path
        """
        summary = {
            'shape': data.shape,
            'columns': data.columns.tolist(),
            'dtypes': {col: str(dtype) for col, dtype in data.dtypes.items()},
            'missing_values': data.isnull().sum().to_dict(),
            'describe': data.describe().to_dict(),
            'memory_usage': data.memory_usage(deep=True).to_dict()
        }
        
        with open(filepath, 'w') as f:
            json.dump(summary, f, indent=2, default=str)
        
        logger.info(f"Exported data summary to JSON: {filepath}")
class DataProcessor:
    """
    Process NLM simulation data.
    
    Provides comprehensive data processing capabilities including:
    - Data validation and cleaning
    - Normalization and scaling
    - Feature engineering and aggregation
    - Export in various formats
    - Quality assessment and reporting
    """
    
    def __init__(self):
        self.validator = DataValidator()
        self.normalizer = DataNormalizer()
        self.aggregator = DataAggregator()
        self.exporter = DataExporter()
        self.processing_history = []
    
    def validate_data(self, data: pd.DataFrame, data_type: DataType) -> Dict[str, Any]:
        """
        Validate data according to type.
        
        Args:
            data: DataFrame to validate
            data_type: Type of data
            
        Returns:
            Validation results
        """
        if data_type == DataType.NEURON_DATA:
            return self.validator.validate_neuron_data(data)
        elif data_type == DataType.TEMPORAL_DATA:
            return self.validator.validate_time_series_data(data)
        elif data_type == DataType.SPATAL_DATA:
            return self.validator.validate_spatial_data(data)
        else:
            # Generic validation
            return {
                'is_valid': True,
                'errors': [],
                'warnings': [],
                'suggestions': ['No specific validation rules for this data type']
            }
    
    def clean_data(self, data: pd.DataFrame, data_type: DataType, 
                   strategy: str = 'auto') -> pd.DataFrame:
        """
        Clean data according to data type and strategy.
        
        Args:
            data: DataFrame to clean
            data_type: Type of data
            strategy: Cleaning strategy ('auto', 'dropna', 'fillna', 'interpolate')
            
        Returns:
            Cleaned DataFrame
        """
        result = data.copy()
        
        # Remove duplicate rows
        result = result.drop_duplicates()
        
        # Handle missing values based on strategy
        if strategy == 'dropna':
            result = result.dropna()
        elif strategy == 'fillna':
            for column in result.columns:
                if result[column].dtype in ['float64', 'int64']:
                    result[column] = result[column].fillna(result[column].median())
        elif strategy == 'interpolate':
            for column in result.columns:
                if result[column].dtype in ['float64', 'int64']:
                    result[column] = result[column].interpolate()
        
        return result
    
    def normalize_data(self, data: pd.DataFrame, columns: List[str] = None,
                       fit: bool = True) -> pd.DataFrame:
        """
        Normalize data using appropriate scaling.
        
        Args:
            data: DataFrame to normalize
            columns: List of columns to normalize (None for all numeric)
            fit: Whether to fit new scalers
            
        Returns:
            Normalized DataFrame
        """
        if fit:
            return self.normalizer.fit_transform(data, columns)
        else:
            return self.normalizer.transform_data(data, columns)
    
    def create_features(self, data: pd.DataFrame, feature_rules: Dict[str, Callable]) -> pd.DataFrame:
        """
        Create new features based on rules.
        
        Args:
            data: DataFrame to process
            feature_rules: Dictionary mapping feature names to creation functions
            
        Returns:
            DataFrame with new features
        """
        result = data.copy()
        
        for feature_name, feature_func in feature_rules.items():
            try:
                result[feature_name] = feature_func(data)
                logger.info(f"Created feature: {feature_name}")
            except Exception as e:
                logger.warning(f"Failed to create feature {feature_name}: {e}")
        
        return result
    
    def reduce_dimensions(self, data: pd.DataFrame, n_components: int = 2,
                         method: str = 'pca') -> pd.DataFrame:
        """
        Reduce data dimensions using specified method.
        
        Args:
            data: DataFrame to reduce
            n_components: Number of components
            method: Reduction method ('pca', 'tsne', 'umap')
            
        Returns:
            Reduced dimension DataFrame
        """
        if method == 'pca':
            # Fit PCA if not already fitted
            if not self.normalizer.is_fitted or self.normalizer.pca is None:
                self.normalizer.fit_pca(data, n_components)
            
            return self.normalizer.transform_pca(data)
        
        else:
            logger.warning(f"Method {method} not implemented. Using PCA.")
            return self.reduce_dimensions(data, n_components, 'pca')
    
    def analyze_patterns(self, data: pd.DataFrame, analysis_type: str = 'clustering') -> Dict[str, Any]:
        """
        Analyze patterns in data.
        
        Args:
            data: DataFrame to analyze
            analysis_type: Type of analysis ('clustering', 'correlation', 'outliers', 'trends')
            
        Returns:
            Dictionary with analysis results
        """
        results = {}
        
        if analysis_type == 'clustering':
            # K-means clustering
            numeric_data = data.select_dtypes(include=[np.number])
            if len(numeric_data.columns) > 0:
                kmeans = KMeans(n_clusters=min(5, len(numeric_data)), random_state=42)
                clusters = kmeans.fit_predict(numeric_data)
                results['clusters'] = clusters
                results['cluster_centers'] = kmeans.cluster_centers_.tolist()
                results['n_clusters'] = len(set(clusters))
        
        elif analysis_type == 'correlation':
            # Compute correlation matrix
            numeric_data = data.select_dtypes(include=[np.number])
            if len(numeric_data.columns) > 1:
                correlation_matrix = numeric_data.corr()
                results['correlation_matrix'] = correlation_matrix.to_dict()
                results['high_correlation_pairs'] = self._find_high_correlation_pairs(correlation_matrix)
        
        elif analysis_type == 'outliers':
            # Detect outliers using IQR method
            numeric_data = data.select_dtypes(include=[np.number])
            if len(numeric_data.columns) > 0:
                outliers = {}
                for column in numeric_data.columns:
                    Q1 = numeric_data[column].quantile(0.25)
                    Q3 = numeric_data[column].quantile(0.75)
                    IQR = Q3 - Q1
                    lower_bound = Q1 - 1.5 * IQR
                    upper_bound = Q3 + 1.5 * IQR
                    
                    column_outliers = numeric_data[
                        (numeric_data[column] < lower_bound) | (numeric_data[column] > upper_bound)
                    ]
                    outliers[column] = column_outliers.index.tolist()
                
                results['outliers'] = outliers
                results['outlier_counts'] = {col: len(indices) for col, indices in outliers.items()}
        
        elif analysis_type == 'trends':
            # Simple trend analysis
            numeric_data = data.select_dtypes(include=[np.number])
            if len(numeric_data.columns) > 0:
                trends = {}
                for column in numeric_data.columns:
                    # Compute linear trend
                    x = np.arange(len(numeric_data[column]))
                    y = numeric_data[column].values
                    
                    if len(x) > 1:
                        slope = np.polyfit(x, y, 1)[0]
                        trends[column] = slope
                
                results['trends'] = trends
                results['trend_summary'] = {
                    'increasing_columns': [col for col, slope in trends.items() if slope > 0],
                    'decreasing_columns': [col for col, slope in trends.items() if slope < 0],
                    'stable_columns': [col for col, slope in trends.items() if abs(slope) < 0.01]
                }
        
        return results
    
    def _find_high_correlation_pairs(self, correlation_matrix: pd.DataFrame, threshold: float = 0.8) -> List[Dict[str, Any]]:
        """
        Find highly correlated column pairs.
        
        Args:
            correlation_matrix: Correlation matrix
            threshold: Correlation threshold
            
        Returns:
            List of highly correlated pairs
        """
        pairs = []
        columns = correlation_matrix.columns
        
        for i in range(len(columns)):
            for j in range(i + 1, len(columns)):
                col1, col2 = columns[i], columns[j]
                corr = correlation_matrix.loc[col1, col2]
                if abs(corr) >= threshold:
                    pairs.append({
                        'column1': col1,
                        'column2': col2,
                        'correlation': corr,
                        'relationship': 'positive' if corr > 0 else 'negative'
                    })
        
        return sorted(pairs, key=lambda x: abs(x['correlation']), reverse=True)
    
    def process_data(self, data: pd.DataFrame, data_type: DataType, 
                    steps: List[str] = None) -> ProcessingResult:
        """
        Process data through a series of steps.
        
        Args:
            data: DataFrame to process
            data_type: Type of data
            steps: List of processing steps (default: ['validate', 'clean', 'normalize'])
            
        Returns:
            ProcessingResult
        """
        if steps is None:
            steps = ['validate', 'clean', 'normalize']
        
        transformations = []
        errors = []
        original_data = data.copy()
        processed_data = data.copy()
        
        try:
            # Validate data
            if 'validate' in steps:
                validation_result = self.validate_data(processed_data, data_type)
                transformations.append('validation')
                
                if not validation_result.get('is_valid', True):
                    errors.extend(validation_result.get('errors', []))
            
            # Clean data
            if 'clean' in steps:
                cleaned_data = self.clean_data(processed_data, data_type, 'auto')
                processed_data = cleaned_data
                transformations.append('cleaning')
            
            # Normalize data
            if 'normalize' in steps:
                normalized_data = self.normalize_data(processed_data)
                processed_data = normalized_data
                transformations.append('normalization')
            
            # Create features if requested
            if 'features' in steps:
                # Simple feature creation based on data type
                if data_type == DataType.NEURON_DATA:
                    feature_rules = {
                        'firing_rate': lambda df: df['firing_state'].apply(lambda x: 100 if x == 1 else 0),
                        'membrane_potential_normalized': lambda df: (df['membrane_potential'] - 
                                                                   df['membrane_potential'].mean()) / 
                                                                   df['membrane_potential'].std()
                    }
                else:
                    feature_rules = {}
                
                if feature_rules:
                    feature_data = self.create_features(processed_data, feature_rules)
                    processed_data = feature_data
                    transformations.append('feature_creation')
            
            # Analyze patterns if requested
            if 'analysis' in steps:
                analysis_result = self.analyze_patterns(processed_data, 'clustering')
                transformations.append('analysis')
            
            result = ProcessingResult(
                success=len(errors) == 0,
                original_data=original_data,
                processed_data=processed_data,
                transformations=transformations,
                errors=errors
            )
            
            # Record processing history
            self.processing_history.append({
                'timestamp': time.time(),
                'data_type': data_type.value,
                'steps': steps,
                'transformations': transformations,
                'success': result.success,
                'error_count': len(errors)
            })
            
            return result
            
        except Exception as e:
            error_msg = str(e)
            errors.append(error_msg)
            
            result = ProcessingResult(
                success=False,
                original_data=original_data,
                processed_data=processed_data,
                transformations=transformations,
                errors=errors
            )
            
            logger.error(f"Data processing failed: {error_msg}")
            return result
    
    def export_processed_data(self, processed_data: pd.DataFrame, filepath: str, 
                            format: str = 'csv', include_metadata: bool = True):
        """
        Export processed data to file.
        
        Args:
            processed_data: Processed DataFrame
            filepath: Output file path
            format: Export format ('csv', 'json', 'parquet', 'hdf')
            include_metadata: Whether to include metadata
        """
        if format == 'csv':
            self.exporter.to_csv(processed_data, filepath)
        elif format == 'json':
            self.exporter.to_json(processed_data, filepath)
        elif format == 'parquet':
            self.exporter.to_parquet(processed_data, filepath)
        elif format == 'hdf':
            self.exporter.to_hdf(processed_data, filepath)
        
        if include_metadata:
            self.exporter.export_summary(processed_data, f"{filepath}_summary.json")
        
        logger.info(f"Exported processed data to {filepath}")
    
    def get_processing_summary(self) -> Dict[str, Any]:
        """
        Get summary of processing history.
        
        Returns:
            Dictionary with processing summary
        """
        if not self.processing_history:
            return {}
        
        total_processed = len(self.processing_history)
        successful = sum(1 for record in self.processing_history if record['success'])
        failed = total_processed - successful
        
        return {
            'total_processing_sessions': total_processed,
            'successful_sessions': successful,
            'failed_sessions': failed,
            'success_rate': successful / total_processed if total_processed > 0 else 0,
            'recent_sessions': self.processing_history[-5:] if len(self.processing_history) >= 5 else self.processing_history
        }


# Convenience functions for easy usage
def quick_process(neuron_data: pd.DataFrame, target_file: str = None) -> ProcessingResult:
    """
    Quick process of neuron data with default pipeline.
    
    Args:
        neuron_data: DataFrame with neuron data
        target_file: Optional output file path
        
    Returns:
        ProcessingResult
    """
    processor = DataProcessor()
    
    # Default processing steps
    result = processor.process_data(
        neuron_data,
        DataType.NEURON_DATA,
        steps=['validate', 'clean', 'normalize', 'analysis']
    )
    
    if result.success and target_file:
        processor.export_processed_data(result.processed_data, target_file)
    
    return result


def analyze_neuron_patterns(neuron_data: pd.DataFrame) -> Dict[str, Any]:
    """
    Analyze patterns in neuron data.
    
    Args:
        neuron_data: DataFrame with neuron data
        
    Returns:
        Dictionary with pattern analysis results
    """
    processor = DataProcessor()
    
    # Analysis steps
    result = processor.process_data(
        neuron_data,
        DataType.NEURON_DATA,
        steps=['validate', 'normalize', 'analysis']
    )
    
    if result.success:
        return result.metadata
    else:
        return {'success': False, 'errors': result.errors}


if __name__ == "__main__":
    # Example usage of DataProcessor
    print("NLM Data Processor Example")
    
    # Create sample neuron data
    import random
    
    neuron_data = pd.DataFrame({
        'neuron_id': range(1, 101),
        'region_id': [1] * 50 + [2] * 50,
        'neuron_type': ['Excitatory'] * 60 + ['Inhibitory'] * 40,
        'membrane_potential': [random.uniform(-70, -50) for _ in range(100)],
        'firing_state': [random.choice([0, 1, 2, 3]) for _ in range(100)],
        'timestamp': [time.time() - i * 0.1 for i in range(100)]
    })
    
    # Process data
    result = quick_process(neuron_data, "processed_neuron_data.csv")
    
    print(f"Processing completed: {result.success}")
    print(f"Transformations: {result.transformations}")
    print(f"Errors: {result.errors}")
    print(f"Original shape: {result.original_data.shape}")
    print(f"Processed shape: {result.processed_data.shape}")
    
    # Get processing summary
    processor = DataProcessor()
    summary = processor.get_processing_summary()
    print(f"Processing summary: {summary}")
    
    print("Data processor example completed successfully!")
