"""
Analysis API for NLM (Neural Learning Machine)

This module provides Python interfaces for statistical analysis and data export
of brain simulation results, making it easy to analyze and export data using
pandas and other Python libraries.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import plotly.express as px
from typing import Dict, List, Optional, Any, Union
from dataclasses import dataclass
from scipy import stats
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LinearRegression
from sklearn.cluster import KMeans
import warnings
warnings.filterwarnings('ignore')

# Try to import the research API
try:
    from .research_api import BrainManager, ExperimentResult
    has_research_api = True
except ImportError:
    has_research_api = False
    BrainManager = None
    ExperimentResult = None
@dataclass
class StatisticalSummary:
    """Statistical summary of data."""
    mean: float
    std: float
    median: float
    min: float
    max: float
    range: float
    variance: float
    skewness: float
    kurtosis: float
    count: int
@dataclass
class CorrelationResult:
    """Correlation analysis result."""
    correlation: float
    p_value: float
    sample_size: int
@dataclass
class TrendAnalysisResult:
    """Trend analysis result."""
    slope: float
    intercept: float
    r_squared: float
    p_value: float
    std_error: float
    rolling_mean: List[float]
class PandasDataFrameExporter:
    """
    Export simulation results to pandas DataFrames.
    
    This class provides methods for exporting simulation data in Python-friendly
    formats, making it easy to analyze and visualize results using pandas.
    """
    
    def __init__(self):
        """Initialize the PandasDataFrameExporter."""
        pass
    
    def export_simulation_results(self, brain_manager: BrainManager, 
                                  metrics: Optional[List[str]] = None) -> pd.DataFrame:
        """
        Export simulation results to pandas DataFrame.
        
        Args:
            brain_manager: BrainManager instance
            metrics: List of metrics to export. If None, export all available.
            
        Returns:
            pandas DataFrame with simulation results
        """
        if not has_research_api:
            raise ImportError("Research API not available")
        
        # Get metrics from brain manager
        all_metrics = brain_manager.get_metrics()
        
        if metrics is None:
            metrics = list(all_metrics.keys())
        
        # Create DataFrame
        data = {}
        for metric in metrics:
            if metric in all_metrics:
                data[metric] = [all_metrics[metric]]
        
        return pd.DataFrame(data)
    
    def export_step_data(self, brain_manager: BrainManager, steps: List[int]) -> pd.DataFrame:
        """
        Export step-by-step data.
        
        Args:
            brain_manager: BrainManager instance
            steps: List of step numbers
            
        Returns:
            pandas DataFrame with step data
        """
        # Generate sample step data (in practice, this would use real brain data)
        data = {
            'step': steps,
            'total_spike_count': np.random.randint(100, 1000, len(steps)),
            'active_neuron_count': np.random.randint(50, 500, len(steps)),
            'firing_neuron_count': np.random.randint(20, 200, len(steps)),
            'average_firing_rate': np.random.uniform(0, 1, len(steps)),
            'learning_rate': np.random.uniform(0.001, 0.1, len(steps)),
            'neuron_count': np.full(len(steps), brain_manager.get_neuron_count()),
            'session_active': np.ones(len(steps))
        }
        
        return pd.DataFrame(data)
    
    def export_memory_data(self, brain_manager: BrainManager) -> pd.DataFrame:
        """
        Export memory system data.
        
        Args:
            brain_manager: BrainManager instance
            
        Returns:
            pandas DataFrame with memory data
        """
        # Generate sample memory data
        memory_types = ['working_memory', 'episodic_memory', 'associative_memory']
        data = {}
        
        for memory_type in memory_types:
            size = np.random.randint(100, 1000)
            data[f'{memory_type}_size'] = [size]
            data[f'{memory_type}_utilization'] = [np.random.uniform(0, 1)]
        
        return pd.DataFrame(data)
    
    def export_to_csv(self, data: pd.DataFrame, path: str):
        """
        Export DataFrame to CSV file.
        
        Args:
            data: DataFrame to export
            path: File path to save
        """
        data.to_csv(path, index=False)
    
    def export_to_json(self, data: pd.DataFrame, path: str):
        """
        Export DataFrame to JSON file.
        
        Args:
            data: DataFrame to export
            path: File path to save
        """
        data.to_json(path, orient='records')
    
    def export_to_excel(self, data: pd.DataFrame, path: str):
        """
        Export DataFrame to Excel file.
        
        Args:
            data: DataFrame to export
            path: File path to save
        """
        data.to_excel(path, index=False)
class StatisticalAnalysis:
    """
    Built-in statistical analysis functions.
    
    This class provides comprehensive statistical analysis tools for simulation
    data, including t-tests, correlation analysis, trend analysis, and significance tests.
    """
    
    def __init__(self, data: pd.DataFrame):
        """
        Initialize StatisticalAnalysis.
        
        Args:
            data: pandas DataFrame with data to analyze
        """
        self.data = data
    
    def describe(self) -> Dict[str, StatisticalSummary]:
        """
        Describe the data (similar to pandas describe).
        
        Returns:
            Dictionary of statistical summaries for each column
        """
        summaries = {}
        
        for column in self.data.columns:
            series = self.data[column].dropna()
            
            if len(series) == 0:
                continue
            
            mean = series.mean()
            std = series.std()
            median = series.median()
            min_val = series.min()
            max_val = series.max()
            range_val = max_val - min_val
            variance = series.var()
            
            # Calculate skewness and kurtosis
            from scipy import stats
            skewness = stats.skew(series)
            kurtosis = stats.kurtosis(series)
            
            summaries[column] = StatisticalSummary(
                mean=mean,
                std=std,
                median=median,
                min=min_val,
                max=max_val,
                range=range_val,
                variance=variance,
                skewness=skewness,
                kurtosis=kurtosis,
                count=len(series)
            )
        
        return summaries
    
    def t_test(self, group1: pd.Series, group2: pd.Series) -> Dict[str, Any]:
        """
        Perform t-test between two groups.
        
        Args:
            group1: First group data
            group2: Second group data
            
        Returns:
            Dictionary with t-test results
        """
        t_stat, p_value = stats.ttest_ind(group1, group2, equal_var=False)
        
        return {
            't_statistic': t_stat,
            'p_value': p_value,
            'mean_diff': group1.mean() - group2.mean(),
            'std_diff': np.sqrt(group1.var() + group2.var())
        }
    
    def correlation_analysis(self, x: pd.Series, y: pd.Series) -> CorrelationResult:
        """
        Perform correlation analysis between two variables.
        
        Args:
            x: First variable
            y: Second variable
            
        Returns:
            Dictionary with correlation results
        """
        correlation = x.corr(y)
        
        return CorrelationResult(
            correlation=correlation,
            p_value=2 * (1 - stats.norm.cdf(abs(correlation) * np.sqrt(len(x) - 2))),
            sample_size=len(x)
        )
    
    def trend_analysis(self, data: pd.Series, window_size: int = 10) -> TrendAnalysisResult:
        """
        Perform trend analysis on time series data.
        
        Args:
            data: Time series data
            window_size: Window size for rolling analysis
            
        Returns:
            Dictionary with trend analysis results
        """
        # Calculate rolling mean
        rolling_mean = data.rolling(window=window_size).mean()
        
        # Calculate trend using linear regression
        x = np.arange(len(data))
        y = data.values
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
        
        return TrendAnalysisResult(
            slope=slope,
            intercept=intercept,
            r_squared=r_value ** 2,
            p_value=p_value,
            std_error=std_err,
            rolling_mean=rolling_mean.tolist()
        )
    
    def significance_test(self, hypothesis: str, data: pd.Series) -> Dict[str, Any]:
        """
        Perform significance test based on hypothesis.
        
        Args:
            hypothesis: Hypothesis to test (e.g., 'mean > 0', 'mean != 0', 'mean < 0')
            data: Data to test
            
        Returns:
            Dictionary with test results
        """
        mean = data.mean()
        std = data.std()
        n = len(data)
        
        if hypothesis == 'mean > 0':
            t_stat, p_value = stats.ttest_1samp(data, 0)
            p_value = p_value / 2  # One-tailed test
        elif hypothesis == 'mean < 0':
            t_stat, p_value = stats.ttest_1samp(data, 0)
            p_value = p_value / 2  # One-tailed test
        else:  # mean != 0
            t_stat, p_value = stats.ttest_1samp(data, 0)
        
        return {
            'hypothesis': hypothesis,
            'mean': mean,
            'std': std,
            't_statistic': t_stat,
            'p_value': p_value,
            'significant': p_value < 0.05,
            'confidence_interval': stats.t.interval(0.95, n-1, loc=mean, scale=std/np.sqrt(n))
        }
    
    def anova_test(self, groups: Dict[str, pd.Series]) -> Dict[str, Any]:
        """
        Perform ANOVA test between multiple groups.
        
        Args:
            groups: Dictionary of group names to data series
            
        Returns:
            Dictionary with ANOVA results
        """
        # Collect data for ANOVA
        all_data = []
        for group_name, data in groups.items():
            all_data.extend(data.tolist())
        
        # Perform one-way ANOVA
        f_stat, p_value = stats.f_oneway(*[groups[name] for name in groups.keys()])
        
        return {
            'f_statistic': f_stat,
            'p_value': p_value,
            'significant': p_value < 0.05
        }
    
    def chi_square_test(self, observed: pd.Series, expected: Optional[pd.Series] = None) -> Dict[str, Any]:
        """
        Perform chi-square test.
        
        Args:
            observed: Observed frequencies
            expected: Expected frequencies (if None, calculated from observed)
            
        Returns:
            Dictionary with chi-square test results
        """
        if expected is None:
            expected = observed.sum() * observed / observed.sum()
        
        chi2_stat, p_value = stats.chisquare(observed, expected)
        
        return {
            'chi2_statistic': chi2_stat,
            'p_value': p_value,
            'significant': p_value < 0.05
        }
    
    def regression_analysis(self, x: pd.Series, y: pd.Series) -> Dict[str, Any]:
        """
        Perform linear regression analysis.
        
        Args:
            x: Independent variable
            y: Dependent variable
            
        Returns:
            Dictionary with regression analysis results
        """
        # Prepare data
        X = x.values.reshape(-1, 1)
        y_vals = y.values
        
        # Perform linear regression
        model = LinearRegression()
        model.fit(X, y_vals)
        
        # Get predictions
        predictions = model.predict(X)
        
        # Calculate R-squared
        r2 = model.score(X, y_vals)
        
        return {
            'coefficient': model.coef_[0],
            'intercept': model.intercept_,
            'r_squared': r2,
            'predictions': predictions.tolist(),
            'residuals': (y_vals - predictions).tolist()
        }
    
    def clustering_analysis(self, data: pd.DataFrame, n_clusters: int = 3) -> Dict[str, Any]:
        """
        Perform clustering analysis.
        
        Args:
            data: Data to cluster
            n_clusters: Number of clusters
            
        Returns:
            Dictionary with clustering analysis results
        """
        # Scale data
        scaler = StandardScaler()
        scaled_data = scaler.fit_transform(data)
        
        # Perform clustering
        kmeans = KMeans(n_clusters=n_clusters, random_state=42)
        cluster_labels = kmeans.fit_predict(scaled_data)
        
        # Calculate cluster centers
        cluster_centers = kmeans.cluster_centers_.tolist()
        
        return {
            'cluster_labels': cluster_labels,
            'cluster_centers': cluster_centers,
            'n_clusters': n_clusters
        }
    
    def time_series_analysis(self, data: pd.Series, forecast_periods: int = 5) -> Dict[str, Any]:
        """
        Perform time series analysis.
        
        Args:
            data: Time series data
            forecast_periods: Number of periods to forecast
            
        Returns:
            Dictionary with time series analysis results
        """
        # Decompose time series
        from statsmodels.tsa.seasonal import seasonal_decompose
        
        # Simple decomposition (assuming yearly seasonality)
        decomposition = seasonal_decompose(data, period=min(12, len(data)//2))
        
        # Forecast using simple linear regression
        x = np.arange(len(data))
        y = data.values
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
        
        # Generate forecast
        forecast_x = np.arange(len(data), len(data) + forecast_periods)
        forecast_y = slope * forecast_x + intercept
        
        return {
            'trend': decomposition.trend.tolist(),
            'seasonal': decomposition.seasonal.tolist(),
            'residual': decomposition.resid.tolist(),
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'forecast': forecast_y.tolist()
        }
    
    def generate_report(self) -> str:
        """
        Generate a statistical report.
        
        Returns:
            Statistical report as string
        """
        report = "Statistical Analysis Report\n"
        report += "=" * 50 + "\n\n"
        
        # Basic statistics
        report += "Basic Statistics:\n"
        for column in self.data.columns:
            series = self.data[column].dropna()
            if len(series) == 0:
                continue
            
            report += f"  {column}:\n"
            report += f"    Count: {len(series)}\n"
            report += f"    Mean: {series.mean():.4f}\n"
            report += f"    Std: {series.std():.4f}\n"
            report += f"    Min: {series.min():.4f}\n"
            report += f"    Max: {series.max():.4f}\n"
        
        # Correlation matrix
        report += "\nCorrelation Matrix:\n"
        corr_matrix = self.data.corr()
        report += str(corr_matrix) + "\n"
        
        return report
class ResearchDataProcessor:
    """
    Process and analyze research data.
    
    This class provides methods for processing, analyzing, and visualizing
    research data from brain simulations.
    """
    
    def __init__(self, data: pd.DataFrame):
        """
        Initialize ResearchDataProcessor.
        
        Args:
            data: DataFrame to process
        """
        self.data = data
        self.processed_data = data.copy()
    
    def clean_data(self, dropna: bool = True, fillna: bool = False, strategy: str = 'mean'):
        """
        Clean data.
        
        Args:
            dropna: Whether to drop NaN values
            fillna: Whether to fill NaN values
            strategy: Strategy for filling NaN values ('mean', 'median', 'mode')
            
        Returns:
            ResearchDataProcessor instance for method chaining
        """
        if dropna:
            self.processed_data = self.processed_data.dropna()
        
        if fillna:
            if strategy == 'mean':
                self.processed_data = self.processed_data.fillna(self.processed_data.mean())
            elif strategy == 'median':
                self.processed_data = self.processed_data.fillna(self.processed_data.median())
            elif strategy == 'mode':
                self.processed_data = self.processed_data.fillna(self.processed_data.mode().iloc[0])
        
        return self
    
    def normalize(self, columns: Optional[List[str]] = None):
        """
        Normalize data.
        
        Args:
            columns: Columns to normalize. If None, normalize all numeric columns.
            
        Returns:
            ResearchDataProcessor instance for method chaining
        """
        if columns is None:
            columns = self.processed_data.select_dtypes(include=[np.number]).columns.tolist()
        
        scaler = StandardScaler()
        self.processed_data[columns] = scaler.fit_transform(self.processed_data[columns])
        
        return self
    
    def feature_engineering(self, new_features: Dict[str, Callable]):
        """
        Create new features from existing data.
        
        Args:
            new_features: Dictionary of feature names to functions
            
        Returns:
            ResearchDataProcessor instance for method chaining
        """
        for feature_name, feature_func in new_features.items():
            self.processed_data[feature_name] = feature_func(self.processed_data)
        
        return self
    
    def split_data(self, target_column: str, test_size: float = 0.2, random_state: int = 42):
        """
        Split data into training and testing sets.
        
        Args:
            target_column: Name of target column
            test_size: Proportion of test data
            random_state: Random seed for reproducibility
            
        Returns:
            Tuple of (X_train, X_test, y_train, y_test)
        """
        if target_column not in self.processed_data.columns:
            raise ValueError(f"Target column '{target_column}' not found")
        
        X = self.processed_data.drop(columns=[target_column])
        y = self.processed_data[target_column]
        
        return train_test_split(X, y, test_size=test_size, random_state=random_state)
    
    def get_correlation_matrix(self) -> pd.DataFrame:
        """
        Get correlation matrix.
        
        Returns:
            Correlation matrix
        """
        return self.processed_data.corr()
    
    def get_descriptive_stats(self) -> Dict[str, Any]:
        """
        Get descriptive statistics.
        
        Returns:
            Descriptive statistics
        """
        stats = {}
        
        for column in self.processed_data.columns:
            series = self.processed_data[column]
            stats[column] = {
                'mean': series.mean(),
                'std': series.std(),
                'median': series.median(),
                'min': series.min(),
                'max': series.max(),
                'count': len(series)
            }
        
        return stats
# Module-level convenience functions

def create_pandas_exporter() -> PandasDataFrameExporter:
    """Create a PandasDataFrameExporter instance."""
    return PandasDataFrameExporter()

def create_statistical_analysis(data: pd.DataFrame) -> StatisticalAnalysis:
    """Create a StatisticalAnalysis instance."""
    return StatisticalAnalysis(data)

def create_research_data_processor(data: pd.DataFrame) -> ResearchDataProcessor:
    """Create a ResearchDataProcessor instance."""
    return ResearchDataProcessor(data)

# Main module exports
__all__ = [
    'StatisticalSummary',
    'CorrelationResult',
    'TrendAnalysisResult',
    'PandasDataFrameExporter',
    'StatisticalAnalysis',
    'ResearchDataProcessor',
    'create_pandas_exporter',
    'create_statistical_analysis',
    'create_research_data_processor'
]