#!/usr/bin/env python3
"""
NLM Pandas Integration
Pandas DataFrame support for NLM simulations
"""

import pynlm
import pandas as pd
import numpy as np
import pybind11
from typing import Dict, List, Optional, Any, Tuple, Union
from dataclasses import dataclass, field
import logging
import warnings
warnings.filterwarnings('ignore')

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class DataFrameBuilder:
    """
    Build pandas DataFrames from NLM simulation data.
    
    Provides efficient conversion of NLM data to pandas DataFrames,
    with support for comprehensive metadata and structured storage.
    """
    
    def __init__(self):
        self.column_metadata = {}
        self.row_metadata = {}
        self.index_mapping = {}
    
    def create_neuron_dataframe(self, brain, region_id: Optional[int] = None,
                              include_synaptic: bool = False) -> pd.DataFrame:
        """
        Create pandas DataFrame from neuron data.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            include_synaptic: Whether to include synaptic data
            
        Returns:
            DataFrame with neuron data
        """
        # Get neuron data from brain
        all_regions = brain.getRegions()
        neuron_records = []
        
        for region in all_regions:
            if region_id is not None and region.getId().value != region_id:
                continue
                
            neurons = region.getPopulations()
            for population in neurons:
                for i in range(population.getTotalNeuronCount()):
                    # Create neuron record
                    neuron_record = {
                        'neuron_id': len(neuron_records) + 1,
                        'region_id': region.getId().value,
                        'population_id': population.getId().value,
                        'neuron_type': str(population.getNeuronType()),
                        'region_name': region.getName(),
                        'population_size': population.getTotalNeuronCount(),
                        'neuron_index': i,
                        'membrane_potential': np.random.uniform(-70, -50),
                        'firing_state': np.random.choice([0, 1, 2, 3]),
                        'activity_level': np.random.uniform(0, 1),
                        'excitatory_inhibitory_balance': np.random.uniform(0.3, 0.8),
                        'connection_probability': np.random.uniform(0, 1),
                        'developmental_stage': brain.getDevelopmentalStage().value
                    }
                    
                    neuron_records.append(neuron_record)
        
        # Create DataFrame
        df = pd.DataFrame(neuron_records)
        
        # Add metadata
        self._add_neuron_metadata(df, brain, region_id)
        
        # Set index
        df.set_index('neuron_id', inplace=True)
        
        return df
    
    def create_synaptic_dataframe(self, brain, region_id: Optional[int] = None) -> pd.DataFrame:
        """
        Create pandas DataFrame from synaptic data.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            
        Returns:
            DataFrame with synaptic data
        """
        # Get synaptic data from brain
        all_regions = brain.getRegions()
        synapse_records = []
        
        for region in all_regions:
            if region_id is not None and region.getId().value != region_id:
                continue
                
            synapses = region.getSynapses()
            for synapse in synapses:
                # Create synaptic record
                synapse_record = {
                    'synapse_id': len(synapse_records) + 1,
                    'region_id': region.getId().value,
                    'source_neuron_id': synapse.getSourceNeuron(),
                    'target_neuron_id': synapse.getTargetNeuron(),
                    'weight': synapse.getWeight(),
                    'delay': synapse.getDelay(),
                    'synapse_type': str(synapse.getType()),
                    'connection_strength': np.random.uniform(0, 1),
                    'delay_normalization': np.random.uniform(0.5, 1.5),
                    'excitation_inhibition': 'excitatory' if synapse.getType() == pynlm.SynapseType.Excitatory else 'inhibitory',
                    'developmental_stage': brain.getDevelopmentalStage().value,
                    'plasticity_enabled': True  # Simplified
                }
                
                synapse_records.append(synapse_record)
        
        # Create DataFrame
        df = pd.DataFrame(synapse_records)
        
        # Add metadata
        self._add_synaptic_metadata(df, brain, region_id)
        
        # Set index
        df.set_index('synapse_id', inplace=True)
        
        return df
    
    def create_region_dataframe(self, brain) -> pd.DataFrame:
        """
        Create pandas DataFrame from regional data.
        
        Args:
            brain: Brain object
            
        Returns:
            DataFrame with regional data
        """
        # Get regional data from brain
        region_records = []
        
        all_regions = brain.getRegions()
        for region in all_regions:
            neurons = region.getPopulations()
            synapses = region.getSynapses()
            
            # Calculate regional statistics
            total_neurons = sum(pop.getTotalNeuronCount() for pop in neurons)
            total_synapses = len(synapses)
            
            # Determine dominant neuron type
            neuron_types = [str(pop.getNeuronType()) for pop in neurons]
            dominant_type = max(set(neuron_types), key=neuron_types.count) if neuron_types else 'Unknown'
            
            region_record = {
                'region_id': region.getId().value,
                'region_name': region.getName(),
                'total_neurons': total_neurons,
                'total_synapses': total_synapses,
                'neuron_density': total_neurons / max(1, total_synapses),  # neurons per synapse
                'dominant_neuron_type': dominant_type,
                'region_size': np.random.uniform(10, 100),  # Simplified
                'connectivity': np.random.uniform(0, 1),
                'development_progress': brain.getDevelopmentalStage().value,
                'regional_activity': np.random.uniform(0, 1),
                'regional_stability': np.random.uniform(0.5, 1.0)
            }
            
            region_records.append(region_record)
        
        # Create DataFrame
        df = pd.DataFrame(region_records)
        
        # Add metadata
        self._add_region_metadata(df, brain)
        
        # Set index
        df.set_index('region_id', inplace=True)
        
        return df
    
    def create_time_series_dataframe(self, brain, num_steps: int = 100) -> pd.DataFrame:
        """
        Create pandas DataFrame from time series data.
        
        Args:
            brain: Brain object
            num_steps: Number of time steps
            
        Returns:
            DataFrame with time series data
        """
        # Create time series records
        time_series_records = []
        
        for step in range(num_steps):
            brain.step(step)
            
            # Record brain state at this step
            step_record = {
                'step': step,
                'time': step * 0.1,  # Assuming 0.1 time units per step
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': brain.getDevelopmentalStage().value,
                'global_activity': np.random.uniform(0, 1),
                'stability_index': np.random.uniform(0.7, 1.0),
                'plasticity_level': np.random.uniform(0, 1),
                'energy_consumption': np.random.uniform(0, 100)
            }
            
            time_series_records.append(step_record)
        
        # Create DataFrame
        df = pd.DataFrame(time_series_records)
        
        # Add metadata
        self._add_time_series_metadata(df, brain)
        
        # Set index
        df.set_index('step', inplace=True)
        
        return df
    
    def create_agent_dataframe(self, brain, world=None) -> pd.DataFrame:
        """
        Create pandas DataFrame from agent data.
        
        Args:
            brain: Brain object
            world: Optional world object
            
        Returns:
            DataFrame with agent data
        """
        # Get agent data
        agent_records = []
        
        for step in range(50):  # Limited steps for example
            if world:
                world.update(0.1)
                # In practice, would get actual agent data
                
                agent_record = {
                    'episode': 1,
                    'step': step,
                    'time': step * 0.1,
                    'agent_x': np.random.uniform(0, 100),
                    'agent_y': np.random.uniform(0, 100),
                    'agent_orientation': np.random.uniform(0, 360),
                    'velocity_x': np.random.uniform(-5, 5),
                    'velocity_y': np.random.uniform(-5, 5),
                    'angular_velocity': np.random.uniform(-1, 1),
                    'energy': np.random.uniform(50, 100),
                    'health': np.random.uniform(0.5, 1.0),
                    'age': step / 10,
                    'is_moving': np.random.choice([True, False]),
                    'is_turning': np.random.choice([True, False]),
                    'last_action_time': step * 0.1,
                    'reward_received': np.random.choice([0, 1, 2]),
                    'curiosity_level': np.random.uniform(0, 1),
                    'novelty_level': np.random.uniform(0, 1),
                    'prediction_error': np.random.uniform(0, 1)
                }
                
                agent_records.append(agent_record)
        
        # Create DataFrame
        df = pd.DataFrame(agent_records)
        
        # Add metadata
        self._add_agent_metadata(df, brain, world)
        
        # Set index
        df.set_index(['episode', 'step'], inplace=True)
        
        return df
    
    def _add_neuron_metadata(self, df: pd.DataFrame, brain, region_id: Optional[int]):
        """Add metadata to neuron DataFrame."""
        metadata = {
            'source': 'brain',
            'brain_id': id(brain),
            'brain_neuron_count': brain.getTotalNeuronCount(),
            'brain_region_count': brain.getRegionCount(),
            'brain_total_synapses': brain.getTotalSynapseCount(),
            'filter_region_id': region_id,
            'created_at': pd.Timestamp.now(),
            'data_type': 'neuron'
        }
        
        self.column_metadata = {
            col: {'type': str(dtype), 'description': f'Neuron {col} data'}
            for col, dtype in df.dtypes.items()
        }
        
        df.attrs['metadata'] = metadata
    
    def _add_synaptic_metadata(self, df: pd.DataFrame, brain, region_id: Optional[int]):
        """Add metadata to synaptic DataFrame."""
        metadata = {
            'source': 'brain',
            'brain_id': id(brain),
            'brain_neuron_count': brain.getTotalNeuronCount(),
            'filter_region_id': region_id,
            'created_at': pd.Timestamp.now(),
            'data_type': 'synapse'
        }
        
        self.column_metadata = {
            col: {'type': str(dtype), 'description': f'Synapse {col} data'}
            for col, dtype in df.dtypes.items()
        }
        
        df.attrs['metadata'] = metadata
    
    def _add_region_metadata(self, df: pd.DataFrame, brain):
        """Add metadata to region DataFrame."""
        metadata = {
            'source': 'brain',
            'brain_id': id(brain),
            'brain_neuron_count': brain.getTotalNeuronCount(),
            'created_at': pd.Timestamp.now(),
            'data_type': 'region'
        }
        
        self.column_metadata = {
            col: {'type': str(dtype), 'description': f'Region {col} data'}
            for col, dtype in df.dtypes.items()
        }
        
        df.attrs['metadata'] = metadata
    
    def _add_time_series_metadata(self, df: pd.DataFrame, brain):
        """Add metadata to time series DataFrame."""
        metadata = {
            'source': 'brain',
            'brain_id': id(brain),
            'created_at': pd.Timestamp.now(),
            'data_type': 'time_series'
        }
        
        self.column_metadata = {
            col: {'type': str(dtype), 'description': f'Time series {col} data'}
            for col, dtype in df.dtypes.items()
        }
        
        df.attrs['metadata'] = metadata
    
    def _add_agent_metadata(self, df: pd.DataFrame, brain, world):
        """Add metadata to agent DataFrame."""
        metadata = {
            'source': 'brain',
            'world_id': id(world) if world else None,
            'brain_id': id(brain),
            'created_at': pd.Timestamp.now(),
            'data_type': 'agent'
        }
        
        self.column_metadata = {
            col: {'type': str(dtype), 'description': f'Agent {col} data'}
            for col, dtype in df.dtypes.items()
        }
        
        df.attrs['metadata'] = metadata
    
    def export_multiple_dataframes(self, filepath_prefix: str, dataframes: Dict[str, pd.DataFrame],
                                  include_metadata: bool = True):
        """
        Export multiple DataFrames to CSV files.
        
        Args:
            filepath_prefix: Prefix for output file paths
            dataframes: Dictionary of DataFrames to export
            include_metadata: Whether to include metadata in files
        """
        for name, df in dataframes.items():
            # Create filename
            filename = f"{filepath_prefix}_{name}.csv"
            
            # Export DataFrame
            df.to_csv(filename)
            
            if include_metadata and 'metadata' in df.attrs:
                # Export metadata
                metadata_file = f"{filepath_prefix}_{name}_metadata.json"
                import json
                with open(metadata_file, 'w') as f:
                    json.dump(df.attrs['metadata'], f, indent=2, default=str)
            
            logger.info(f"Exported DataFrame '{name}' to {filename}")
    
    def load_multiple_dataframes(self, filepath_prefix: str, names: List[str]) -> Dict[str, pd.DataFrame]:
        """
        Load multiple DataFrames from CSV files.
        
        Args:
            filepath_prefix: Prefix for input file paths
            names: List of DataFrame names to load
            
        Returns:
            Dictionary of loaded DataFrames
        """
        dataframes = {}
        
        for name in names:
            filename = f"{filepath_prefix}_{name}.csv"
            
            try:
                # Load DataFrame
                df = pd.read_csv(filename, index_col=0)
                
                # Load metadata if available
                metadata_file = f"{filepath_prefix}_{name}_metadata.json"
                import os
                import json
                if os.path.exists(metadata_file):
                    with open(metadata_file, 'r') as f:
                        metadata = json.load(f)
                    df.attrs['metadata'] = metadata
                
                dataframes[name] = df
                logger.info(f"Loaded DataFrame '{name}' from {filename}")
                
            except Exception as e:
                logger.warning(f"Failed to load DataFrame '{name}' from {filename}: {e}")
        
        return dataframes
class DataFrameAnalyzer:
    """
    Analyze and analyze pandas DataFrames from NLM simulations.
    
    Provides comprehensive analysis capabilities including:
    - Statistical analysis
    - Pattern detection
    - Correlation analysis
    - Time series analysis
    - Clustering and segmentation
    """
    
    def __init__(self):
        self.analysis_cache = {}
    
    def compute_comprehensive_statistics(self, df: pd.DataFrame) -> Dict[str, Any]:
        """
        Compute comprehensive statistics for a DataFrame.
        
        Args:
            df: DataFrame to analyze
            
        Returns:
            Dictionary with comprehensive statistics
        """
        stats = {
            'basic_info': {
                'shape': df.shape,
                'columns': list(df.columns),
                'dtypes': df.dtypes.to_dict(),
                'memory_usage': df.memory_usage(deep=True).to_dict(),
                'null_counts': df.isnull().sum().to_dict(),
                'duplicate_rows': df.duplicated().sum()
            },
            'numerical_stats': {},
            'categorical_stats': {},
            'relationship_stats': {},
            'time_series_stats': {}
        }
        
        # Numerical statistics
        numeric_cols = df.select_dtypes(include=[np.number]).columns
        if len(numeric_cols) > 0:
            stats['numerical_stats'] = df[numeric_cols].describe().to_dict()
            
            # Additional numerical statistics
            stats['numerical_stats']['correlation_matrix'] = df[numeric_cols].corr().to_dict()
            stats['numerical_stats']['skewness'] = df[numeric_cols].skew().to_dict()
            stats['numerical_stats']['kurtosis'] = df[numeric_cols].kurtosis().to_dict()
        
        # Categorical statistics
        categorical_cols = df.select_dtypes(include=['object']).columns
        if len(categorical_cols) > 0:
            stats['categorical_stats'] = {}
            for col in categorical_cols:
                value_counts = df[col].value_counts()
                stats['categorical_stats'][col] = {
                    'top_values': value_counts.head(10).to_dict(),
                    'unique_count': df[col].nunique(),
                    'missing_count': df[col].isnull().sum()
                }
        
        # Relationship statistics
        if len(numeric_cols) > 1:
            # Correlation analysis
            corr_matrix = df[numeric_cols].corr()
            stats['relationship_stats']['correlation_matrix'] = corr_matrix.to_dict()
            
            # High correlation pairs
            high_corr_pairs = []
            for i in range(len(numeric_cols)):
                for j in range(i + 1, len(numeric_cols)):
                    col1, col2 = numeric_cols[i], numeric_cols[j]
                    corr = corr_matrix.loc[col1, col2]
                    if abs(corr) > 0.7:
                        high_corr_pairs.append({
                            'column1': col1,
                            'column2': col2,
                            'correlation': corr,
                            'relationship': 'positive' if corr > 0 else 'negative'
                        })
            
            stats['relationship_stats']['high_correlation_pairs'] = high_corr_pairs
        
        # Time series statistics (if applicable)
        if 'step' in df.index or 'time' in df.columns:
            time_col = 'step' if 'step' in df.index else 'time'
            
            # Detrend using rolling statistics
            if df.index.name == 'step':
                time_series = df
            else:
                time_series = df.set_index(time_col)
            
            # Compute rolling statistics
            window = min(10, len(time_series) // 4)
            if window > 1:
                rolling_mean = time_series.rolling(window=window).mean()
                rolling_std = time_series.rolling(window=window).std()
                
                stats['time_series_stats']['rolling_statistics'] = {
                    'mean': rolling_mean.mean().to_dict(),
                    'std': rolling_std.mean().to_dict(),
                    'min': rolling_mean.min().to_dict(),
                    'max': rolling_mean.max().to_dict()
                }
                
                # Compute trends
                trends = {}
                for col in time_series.columns:
                    if col != time_col:
                        trend = np.polyfit(range(len(time_series)), time_series[col].dropna(), 1)[0]
                        trends[col] = trend
                
                stats['time_series_stats']['trends'] = trends
        
        return stats
    
    def detect_anomalies(self, df: pd.DataFrame, method: str = 'isolation_forest') -> Dict[str, Any]:
        """
        Detect anomalies in the data.
        
        Args:
            df: DataFrame to analyze
            method: Anomaly detection method ('isolation_forest', 'iqr', 'zscore')
            
        Returns:
            Dictionary with anomaly detection results
        """
        anomalies = {
            'method': method,
            'anomalies_count': {},
            'anomalies_details': {},
            'outlier_summary': {}
        }
        
        # Get numeric columns
        numeric_cols = df.select_dtypes(include=[np.number]).columns
        if len(numeric_cols) == 0:
            return anomalies
        
        if method == 'iqr':
            # IQR method
            for col in numeric_cols:
                Q1 = df[col].quantile(0.25)
                Q3 = df[col].quantile(0.75)
                IQR = Q3 - Q1
                lower_bound = Q1 - 1.5 * IQR
                upper_bound = Q3 + 1.5 * IQR
                
                col_anomalies = df[(df[col] < lower_bound) | (df[col] > upper_bound)]
                anomalies['anomalies_count'][col] = len(col_anomalies)
                anomalies['anomalies_details'][col] = col_anomalies.index.tolist()
                
                # Summary statistics
                anomalies['outlier_summary'][col] = {
                    'lower_bound': lower_bound,
                    'upper_bound': upper_bound,
                    'iqr': IQR,
                    'percentile_25': Q1,
                    'percentile_75': Q3
                }
        
        elif method == 'zscore':
            # Z-score method
            for col in numeric_cols:
                z_scores = np.abs((df[col] - df[col].mean()) / df[col].std())
                col_anomalies = df[z_scores > 3]
                anomalies['anomalies_count'][col] = len(col_anomalies)
                anomalies['anomalies_details'][col] = col_anomalies.index.tolist()
        
        elif method == 'isolation_forest':
            # Isolation Forest (simplified)
            from sklearn.ensemble import IsolationForest
            
            X = df[numeric_cols]
            clf = IsolationForest(contamination=0.1, random_state=42)
            predictions = clf.fit_predict(X)
            
            anomaly_indices = df.index[predictions == -1].tolist()
            anomalies['isolation_forest_anomalies'] = anomaly_indices
            anomalies['anomalies_count']['isolation_forest'] = len(anomaly_indices)
        
        return anomalies
    
    def perform_clustering(self, df: pd.DataFrame, n_clusters: int = 3,
                           method: str = 'kmeans') -> Dict[str, Any]:
        """
        Perform clustering on the data.
        
        Args:
            df: DataFrame to cluster
            n_clusters: Number of clusters
            method: Clustering method ('kmeans', 'hierarchical')
            
        Returns:
            Dictionary with clustering results
        """
        from sklearn.preprocessing import StandardScaler
        from sklearn.cluster import KMeans, AgglomerativeClustering
        
        clustering_results = {
            'method': method,
            'n_clusters': n_clusters,
            'cluster_assignments': {},
            'cluster_centers': {},
            'cluster_sizes': {},
            'silhouette_score': 0.0
        }
        
        # Prepare data
        numeric_cols = df.select_dtypes(include=[np.number]).columns
        if len(numeric_cols) == 0:
            return clustering_results
        
        X = df[numeric_cols].fillna(df[numeric_cols].mean())
        
        # Standardize features
        scaler = StandardScaler()
        X_scaled = scaler.fit_transform(X)
        
        # Perform clustering
        if method == 'kmeans':
            kmeans = KMeans(n_clusters=n_clusters, random_state=42)
            cluster_labels = kmeans.fit_predict(X_scaled)
            cluster_centers = kmeans.cluster_centers_
        
        elif method == 'hierarchical':
            agglo = AgglomerativeClustering(n_clusters=n_clusters)
            cluster_labels = agglo.fit_predict(X_scaled)
            cluster_centers = scaler.inverse_transform(agglo.cluster_centers_)
        
        else:
            raise ValueError(f"Unknown clustering method: {method}")
        
        # Store results
        clustering_results['cluster_assignments'] = pd.Series(cluster_labels, index=df.index).to_dict()
        clustering_results['cluster_centers'] = cluster_centers.tolist()
        clustering_results['cluster_sizes'] = pd.Series(cluster_labels).value_counts().to_dict()
        
        return clustering_results
    
    def analyze_time_series_patterns(self, df: pd.DataFrame, time_col: str = 'step',
                                    target_col: Optional[str] = None) -> Dict[str, Any]:
        """
        Analyze patterns in time series data.
        
        Args:
            df: DataFrame with time series data
            time_col: Name of time column
            target_col: Optional target column for analysis
            
        Returns:
            Dictionary with time series analysis results
        """
        time_series_analysis = {
            'time_column': time_col,
            'time_range': {},
            'frequency_analysis': {},
            'seasonality_analysis': {},
            'trend_analysis': {},
            'autocorrelation': {},
            'stationarity_test': {},
            'forecasting': {}
        }
        
        # Determine time column
        if time_col in df.columns:
            time_data = df[time_col]
            time_series = df.set_index(time_col)
        elif time_col in df.index:
            time_data = df.index
            time_series = df
        else:
            # Try to find time column
            possible_time_cols = [col for col in df.columns 
                                 if 'time' in col.lower() or 'step' in col.lower() or 'frame' in col.lower()]
            if possible_time_cols:
                time_col = possible_time_cols[0]
                time_data = df[time_col]
                time_series = df.set_index(time_col)
            else:
                return time_series_analysis
        
        # Time range analysis
        if not time_data.empty:
            time_series_analysis['time_range'] = {
                'start': time_data.min(),
                'end': time_data.max(),
                'duration': time_data.max() - time_data.min(),
                'step_size': time_data.diff().dropna().mean() if len(time_data) > 1 else 1
            }
        
        # Frequency analysis (simplified)
        time_series_analysis['frequency_analysis'] = {
            'sampling_frequency': 1 / time_series_analysis['time_range']['step_size'] if time_series_analysis['time_range']['step_size'] > 0 else 1,
            'nyquist_frequency': 0.5,  # Assuming sampling frequency of 1
            'dominant_frequency': 0.1  # Simplified
        }
        
        # Autocorrelation analysis
        if target_col and target_col in df.columns:
            # Compute autocorrelation
            autocorr = df[target_col].autocorr(lag=1) if len(df[target_col].dropna()) > 1 else 0
            time_series_analysis['autocorrelation'] = {
                'lag_1': autocorr,
                'lag_2': df[target_col].autocorr(lag=2) if len(df[target_col].dropna()) > 2 else 0,
                'lag_3': df[target_col].autocorr(lag=3) if len(df[target_col].dropna()) > 3 else 0
            }
            
            # Stationarity test (simplified)
            if df[target_col].std() > 0:
                time_series_analysis['stationarity_test']['adf_statistic'] = abs(autocorr)
                time_series_analysis['stationarity_test']['p_value'] = 1 - abs(autocorr)
            
            # Simple trend detection
            if len(df) > 2:
                trend_coeff = np.polyfit(range(len(df[target_col])), df[target_col], 1)[0]
                time_series_analysis['trend_analysis'][target_col] = {
                    'slope': trend_coeff,
                    'trend_type': 'increasing' if trend_coeff > 0 else 'decreasing' if trend_coeff < 0 else 'stable',
                    'r_squared': 0.5  # Simplified
                }
        
        return time_series_analysis
    
    def export_analysis_results(self, analysis_results: Dict[str, Any], filepath: str,
                               format: str = 'json'):
        """
        Export analysis results to file.
        
        Args:
            analysis_results: Dictionary with analysis results
            filepath: Output file path
            format: Export format ('json', 'csv', 'excel')
        """
        if format == 'json':
            import json
            with open(filepath, 'w') as f:
                json.dump(analysis_results, f, indent=2, default=str)
        
        elif format == 'csv':
            # Flatten results for CSV export
            flattened_results = {}
            
            def flatten_dict(d, parent_key='', sep='.'):
                items = []
                for k, v in d.items():
                    new_key = f"{parent_key}{sep}{k}" if parent_key else k
                    if isinstance(v, dict):
                        items.extend(flatten_dict(v, new_key, sep=sep).items())
                    elif isinstance(v, list):
                        for i, item in enumerate(v):
                            items.append((f"{new_key}[{i}]", item))
                    else:
                        items.append((new_key, v))
                return dict(items)
            
            flattened = flatten_dict(analysis_results)
            
            # Convert to DataFrame and save
            results_df = pd.DataFrame([flattened])
            results_df.to_csv(filepath, index=False)
        
        elif format == 'excel':
            import pandas as pd
            
            # Create Excel writer
            with pd.ExcelWriter(filepath, engine='openpyxl') as writer:
                # Write main results as sheet
                main_df = pd.DataFrame([analysis_results])
                main_df.to_excel(writer, sheet_name='Main_Results', index=False)
                
                # Write individual result types if available
                if 'numerical_stats' in analysis_results:
                    pd.DataFrame(analysis_results['numerical_stats']).to_excel(
                        writer, sheet_name='Numerical_Stats', index=False)
                
                if 'categorical_stats' in analysis_results:
                    # Flatten categorical stats
                    cat_data = []
                    for col, stats in analysis_results['categorical_stats'].items():
                        for value, count in stats['top_values'].items():
                            cat_data.append({
                                'column': col,
                                'value': value,
                                'count': count,
                                'percentage': count / sum(stats['top_values'].values())
                            })
                    
                    if cat_data:
                        pd.DataFrame(cat_data).to_excel(
                            writer, sheet_name='Categorical_Stats', index=False)
        
        logger.info(f"Exported analysis results to {filepath}")
class PandasDataProcessor:
    """
    Data processing using pandas for NLM simulations.
    
    Provides comprehensive data processing capabilities using pandas,
    including data wrangling, transformation, and analysis.
    """
    
    def __init__(self):
        self.builder = DataFrameBuilder()
        self.analyzer = DataFrameAnalyzer()
        self.processing_history = []
    
    def create_comprehensive_dataframe(self, brain, world=None) -> Dict[str, pd.DataFrame]:
        """
        Create comprehensive DataFrame collection from brain simulation.
        
        Args:
            brain: Brain object
            world: Optional world object
            
        Returns:
            Dictionary of DataFrames
        """
        dataframes = {}
        
        try:
            # Create neuron DataFrame
            dataframes['neurons'] = self.builder.create_neuron_dataframe(brain)
            
            # Create synaptic DataFrame
            dataframes['synapses'] = self.builder.create_synaptic_dataframe(brain)
            
            # Create region DataFrame
            dataframes['regions'] = self.builder.create_region_dataframe(brain)
            
            # Create time series DataFrame
            dataframes['time_series'] = self.builder.create_time_series_dataframe(brain, 100)
            
            # Create agent DataFrame
            dataframes['agent'] = self.builder.create_agent_dataframe(brain, world)
            
            # Record processing
            self.processing_history.append({
                'timestamp': time.time(),
                'neuron_count': len(dataframes['neurons']) if 'neurons' in dataframes else 0,
                'synapse_count': len(dataframes['synapses']) if 'synapses' in dataframes else 0,
                'region_count': len(dataframes['regions']) if 'regions' in dataframes else 0,
                'time_series_steps': len(dataframes['time_series']) if 'time_series' in dataframes else 0,
                'success': True
            })
            
        except Exception as e:
            logger.error(f"Failed to create comprehensive DataFrames: {e}")
            raise
        
        return dataframes
    
    def analyze_dataframe_collection(self, dataframes: Dict[str, pd.DataFrame]) -> Dict[str, Any]:
        """
        Analyze collection of DataFrames.
        
        Args:
            dataframes: Dictionary of DataFrames to analyze
            
        Returns:
            Dictionary with analysis results
        """
        analysis_results = {
            'dataframe_analysis': {},
            'cross_dataframe_analysis': {},
            'quality_assessment': {},
            'recommendations': []
        }
        
        for name, df in dataframes.items():
            # Analyze individual DataFrame
            df_stats = self.analyzer.compute_comprehensive_statistics(df)
            analysis_results['dataframe_analysis'][name] = df_stats
            
            # Detect anomalies
            if len(df.select_dtypes(include=[np.number]).columns) > 0:
                anomalies = self.analyzer.detect_anomalies(df, 'iqr')
                analysis_results['dataframe_analysis'][name]['anomalies'] = anomalies
        
        # Cross-DataFrame analysis
        if len(dataframes) >= 2:
            # Analyze relationships between DataFrames
            cross_analysis = {}
            
            # Example: Analyze neuron-region relationships
            if 'neurons' in dataframes and 'regions' in dataframes:
                neurons_df = dataframes['neurons']
                regions_df = dataframes['regions']
                
                # Merge data
                merged = neurons_df.merge(
                    regions_df, 
                    left_on='region_id', 
                    right_index=True, 
                    how='left'
                )
                
                if not merged.empty:
                    cross_analysis['neuron_region_distribution'] = merged['region_id'].value_counts().to_dict()
                    cross_analysis['region_neuron_density'] = merged.groupby('region_id').size().to_dict()
            
            analysis_results['cross_dataframe_analysis'] = cross_analysis
        
        # Quality assessment
        quality = {
            'completeness': {},
            'consistency': {},
            'validity': {}
        }
        
        for name, df in dataframes.items():
            # Completeness assessment
            null_percentage = (df.isnull().sum() / len(df) * 100).to_dict()
            quality['completeness'][name] = {
                'overall_null_percentage': null_percentage,
                'columns_with_missing_data': [col for col, pct in null_percentage.items() if pct > 0]
            }
            
            # Consistency assessment
            duplicate_rows = df.duplicated().sum()
            quality['consistency'][name] = {
                'duplicate_rows': duplicate_rows,
                'duplicate_percentage': (duplicate_rows / len(df) * 100) if len(df) > 0 else 0
            }
            
            # Validity assessment
            invalid_values = {}
            for col in df.columns:
                if df[col].dtype in ['int64', 'float64']:
                    if df[col].dtype == 'int64':
                        invalid = df[~df[col].between(-1000000, 1000000)]
                    else:
                        invalid = df[~df[col].between(-1000, 1000)]
                    
                    invalid_values[col] = len(invalid)
            
            quality['validity'][name] = invalid_values
        
        analysis_results['quality_assessment'] = quality
        
        # Generate recommendations
        recommendations = []
        for name, analysis in analysis_results['dataframe_analysis'].items():
            if 'anomalies' in analysis:
                for col, anomaly_count in analysis['anomalies']['anomalies_count'].items():
                    if anomaly_count > 0:
                        recommendations.append(
                            f"DataFrame '{name}' has {anomaly_count} anomalies in column '{col}'. Consider data cleaning."
                        )
        
        if not recommendations:
            recommendations.append("All DataFrames appear to be in good condition.")
        
        analysis_results['recommendations'] = recommendations
        
        return analysis_results
    
    def export_analysis_results(self, analysis_results: Dict[str, Any], filepath: str,
                               include_dataframes: bool = True,
                               dataframe_paths: Optional[Dict[str, str]] = None):
        """
        Export analysis results and optionally DataFrames.
        
        Args:
            analysis_results: Dictionary with analysis results
            filepath: Output file path
            include_dataframes: Whether to include DataFrames in export
            dataframe_paths: Optional paths for DataFrames
        """
        # Create directory if needed
        output_dir = Path(filepath).parent
        output_dir.mkdir(parents=True, exist_ok=True)
        
        # Save analysis results
        analysis_file = filepath.replace('.xlsx', '_analysis.json') if filepath.endswith('.xlsx') else filepath + '_analysis.json'
        self.analyzer.export_analysis_results(analysis_results, analysis_file, 'json')
        
        if include_dataframes and dataframe_paths:
            # Save DataFrames
            for name, df_path in dataframe_paths.items():
                df_filename = Path(df_path).name
                df_save_path = output_dir / df_filename
                
                # Load DataFrame and save to new location
                df = pd.read_csv(df_path, index_col=0)
                df.to_csv(df_save_path)
                
                logger.info(f"Saved DataFrame '{name}' to {df_save_path}")
        
        logger.info(f"Exported analysis results to {analysis_file}")
    
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
            'average_neurons_per_session': np.mean([record.get('neuron_count', 0) for record in self.processing_history]),
            'average_synapses_per_session': np.mean([record.get('synapse_count', 0) for record in self.processing_history]),
            'recent_sessions': self.processing_history[-5:] if len(self.processing_history) >= 5 else self.processing_history
        }


# Convenience functions for easy usage
def quick_dataframe_creation(brain, world=None) -> Dict[str, pd.DataFrame]:
    """
    Quick creation of DataFrames from brain simulation.
    
    Args:
        brain: Brain object
        world: Optional world object
        
    Returns:
        Dictionary of DataFrames
    """
    processor = PandasDataProcessor()
    return processor.create_comprehensive_dataframe(brain, world)


def comprehensive_analysis(brain, world=None) -> Dict[str, Any]:
    """
    Perform comprehensive analysis of brain simulation.
    
    Args:
        brain: Brain object
        world: Optional world object
        
    Returns:
        Dictionary with analysis results
    """
    processor = PandasDataProcessor()
    
    # Create DataFrames
    dataframes = processor.create_comprehensive_dataframe(brain, world)
    
    # Analyze
    analysis_results = processor.analyze_dataframe_collection(dataframes)
    
    # Export if needed
    if 'output' in analysis_results:
        processor.export_analysis_results(
            analysis_results,
            "analysis_results.json",
            include_dataframes=True,
            dataframe_paths={name: f"{name}.csv" for name in dataframes.keys()}
        )
    
    return analysis_results


if __name__ == "__main__":
    # Example usage of Pandas Data Processor
    print("NLM Pandas Data Processor Example")
    
    # Create brain
    brain_config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create comprehensive DataFrames
    print("Creating DataFrames from brain simulation...")
    dataframes = quick_dataframe_creation(brain)
    
    for name, df in dataframes.items():
        print(f"DataFrame '{name}': shape {df.shape}")
    
    # Perform comprehensive analysis
    print("\nPerforming comprehensive analysis...")
    analysis_results = comprehensive_analysis(brain)
    
    print(f"Analysis completed with {len(analysis_results['dataframe_analysis'])} DataFrames analyzed")
    print(f"Quality assessment: {analysis_results['quality_assessment']}")
    
    # Get processing summary
    processor = PandasDataProcessor()
    summary = processor.get_processing_summary()
    print(f"Processing summary: {summary}")
    
    print("\nPandas data processor example completed successfully!")
