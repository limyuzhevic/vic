# NLM Analysis API Guide

## Overview

This guide provides comprehensive documentation for the statistical analysis and data export APIs in the enhanced NLM Python bindings. The analysis APIs provide comprehensive statistical analysis tools and data export capabilities for brain simulation results.

## What This Guide Covers

1. **Pandas DataFrame Exporter**: Export simulation results to pandas DataFrames
2. **Statistical Analysis**: Comprehensive statistical analysis tools
3. **Data Processing**: Data cleaning, normalization, and feature engineering
4. **Machine Learning Integration**: Integration with scikit-learn for ML workflows
5. **Time Series Analysis**: Advanced time series analysis capabilities
6. **Performance Optimization**: Tips for efficient statistical analysis

## Quick Start

```python
import pynlm
import pandas as pd

# Create sample data
data = pd.DataFrame({
    'group1': [1.2, 1.5, 1.8, 2.0, 2.2],
    'group2': [1.0, 1.3, 1.6, 1.9, 2.1],
    'time': [0, 10, 20, 30, 40]
})

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(data)

# Perform t-test
t_test_results = analysis.t_test(data['group1'], data['group2'])
print(f"T-test p-value: {t_test_results['p_value']}")

# Perform correlation analysis
corr_results = analysis.correlation_analysis(data['time'], data['group1'])
print(f"Correlation: {corr_results.correlation}")

# Generate report
report = analysis.generate_report()
print(report)
```

## Pandas DataFrame Exporter

### PandasDataFrameExporter Class

The `PandasDataFrameExporter` class provides methods for exporting simulation data to pandas DataFrames.

**Constructor:**

```python
PandasDataFrameExporter()
```

**Methods:**

- `export_simulation_results(brain_manager, metrics=None)` - Export simulation results
- `export_step_data(brain_manager, steps)` - Export step-by-step data
- `export_memory_data(brain_manager)` - Export memory data
- `export_to_csv(data, path)` - Export DataFrame to CSV
- `export_to_json(data, path)` - Export DataFrame to JSON
- `export_to_excel(data, path)` - Export DataFrame to Excel

**Example:**

```python
import pynlm

# Create exporter
exporter = pynlm.create_pandas_exporter()

# Export simulation results
brain_manager = pynlm.BrainManager()
results_df = exporter.export_simulation_results(brain_manager)

# Export step-by-step data
steps = list(range(100))
step_data = exporter.export_step_data(brain_manager, steps)

# Export memory data
memory_data = exporter.export_memory_data(brain_manager)

# Save to files
exporter.export_to_csv(results_df, 'simulation_results.csv')
exporter.export_to_json(step_data, 'step_data.json')
exporter.export_to_excel(memory_data, 'memory_data.xlsx')
```

### Data Export Methods

#### Export Simulation Results

The `export_simulation_results()` method exports simulation results to a pandas DataFrame.

**Parameters:**

- `brain_manager`: BrainManager instance
- `metrics`: Optional list of metrics to export (default: all available)

**Returns:**

- pandas DataFrame with simulation results

#### Export Step Data

The `export_step_data()` method exports step-by-step simulation data.

**Parameters:**

- `brain_manager`: BrainManager instance
- `steps`: List of step numbers

**Returns:**

- pandas DataFrame with step-by-step data including:
  - Step number
  - Total spike count
  - Active neuron count
  - Firing neuron count
  - Average firing rate
  - Learning rate
  - Neuron count
  - Session active flag

#### Export Memory Data

The `export_memory_data()` method exports memory system data.

**Parameters:**

- `brain_manager`: BrainManager instance

**Returns:**

- pandas DataFrame with memory system data including:
  - Working memory size and utilization
  - Episodic memory size and utilization
  - Associative memory size and utilization

## Statistical Analysis

### StatisticalAnalysis Class

The `StatisticalAnalysis` class provides comprehensive statistical analysis tools.

**Constructor:**

```python
StatisticalAnalysis(data)
```

**Parameters:**

- `data`: pandas DataFrame with data to analyze

**Methods:**

- `describe()` - Describe the data (similar to pandas describe)
- `t_test(group1, group2)` - Perform t-test between two groups
- `correlation_analysis(x, y)` - Perform correlation analysis
- `trend_analysis(data, window_size=10)` - Perform trend analysis
- `significance_test(hypothesis, data)` - Perform significance test
- `anova_test(groups)` - Perform ANOVA test
- `chi_square_test(observed, expected=None)` - Perform chi-square test
- `regression_analysis(x, y)` - Perform linear regression analysis
- `clustering_analysis(data, n_clusters=3)` - Perform clustering analysis
- `time_series_analysis(data, forecast_periods=5)` - Perform time series analysis
- `generate_report()` - Generate statistical report

**Example:**

```python
import pynlm
import pandas as pd

# Create sample data
data = pd.DataFrame({
    'group1': [1.2, 1.5, 1.8, 2.0, 2.2],
    'group2': [1.0, 1.3, 1.6, 1.9, 2.1],
    'time': [0, 10, 20, 30, 40],
    'category': ['A', 'B', 'A', 'B', 'A']
})

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(data)

# Describe data
describe_results = analysis.describe()
for column, stats in describe_results.items():
    print(f"{column}: mean={stats.mean:.3f}, std={stats.std:.3f}")

# Perform t-test
t_test_results = analysis.t_test(data['group1'], data['group2'])
print(f"T-test: t={t_test_results['t_statistic']:.3f}, p={t_test_results['p_value']:.3f}")

# Perform correlation analysis
corr_results = analysis.correlation_analysis(data['time'], data['group1'])
print(f"Correlation: r={corr_results.correlation:.3f}, p={corr_results.p_value:.3f}")

# Perform ANOVA test
anova_results = analysis.anova_test({
    'A': data[data['category'] == 'A']['group1'],
    'B': data[data['category'] == 'B']['group1']
})
print(f"ANOVA: F={anova_results['f_statistic']:.3f}, p={anova_results['p_value']:.3f}")
```

### Data Description

The `describe()` method provides comprehensive statistical description of the data.

**Returns:**

- Dictionary of StatisticalSummary objects for each column

**StatisticalSummary Structure:**

- `mean`: Arithmetic mean
- `std`: Standard deviation
- `median`: Median value
- `min`: Minimum value
- `max`: Maximum value
- `range`: Range (max - min)
- `variance`: Variance
- `skewness`: Skewness coefficient
- `kurtosis`: Kurtosis coefficient
- `count`: Number of non-null values

### Hypothesis Testing

#### T-Test

The `t_test()` method performs Student's t-test between two independent groups.

**Parameters:**

- `group1`: First group data
- `group2`: Second group data

**Returns:**

- Dictionary with results:
  - `t_statistic`: T-statistic value
  - `p_value`: P-value
  - `mean_diff`: Difference in means
  - `std_diff`: Difference in standard deviations

#### ANOVA Test

The `anova_test()` method performs one-way analysis of variance between multiple groups.

**Parameters:**

- `groups`: Dictionary of group names to data series

**Returns:**

- Dictionary with results:
  - `f_statistic`: F-statistic value
  - `p_value`: P-value
  - `significant`: Whether result is statistically significant (p < 0.05)

#### Chi-Square Test

The `chi_square_test()` method performs chi-square test for independence.

**Parameters:**

- `observed`: Observed frequencies
- `expected`: Expected frequencies (optional, calculated if not provided)

**Returns:**

- Dictionary with results:
  - `chi2_statistic`: Chi-square statistic
  - `p_value`: P-value
  - `significant`: Whether result is statistically significant (p < 0.05)

### Correlation and Regression Analysis

#### Correlation Analysis

The `correlation_analysis()` method performs Pearson correlation analysis between two variables.

**Parameters:**

- `x`: First variable
- `y`: Second variable

**Returns:**

- CorrelationResult object with:
  - `correlation`: Correlation coefficient
  - `p_value`: P-value
  - `sample_size`: Number of observations

#### Regression Analysis

The `regression_analysis()` method performs linear regression analysis.

**Parameters:**

- `x`: Independent variable
- `y`: Dependent variable

**Returns:**

- Dictionary with results:
  - `coefficient`: Regression coefficient
  - `intercept`: Intercept value
  - `r_squared`: R-squared value
  - `predictions`: Predicted values
  - `residuals`: Residual values

### Time Series Analysis

The `time_series_analysis()` method performs comprehensive time series analysis.

**Parameters:**

- `data`: Time series data
- `forecast_periods`: Number of periods to forecast

**Returns:**

- Dictionary with results:
  - `trend`: Trend component
  - `seasonal`: Seasonal component
  - `residual`: Residual component
  - `slope`: Linear regression slope
  - `intercept`: Linear regression intercept
  - `r_squared`: R-squared value
  - `forecast`: Forecast values

### Clustering Analysis

The `clustering_analysis()` method performs clustering analysis using K-means.

**Parameters:**

- `data`: Data to cluster
- `n_clusters`: Number of clusters

**Returns:**

- Dictionary with results:
  - `cluster_labels`: Cluster assignment for each data point
  - `cluster_centers`: Cluster centroids
  - `n_clusters`: Number of clusters

## Data Processing

### ResearchDataProcessor Class

The `ResearchDataProcessor` class provides data processing and preprocessing capabilities.

**Constructor:**

```python
ResearchDataProcessor(data)
```

**Parameters:**

- `data`: DataFrame to process

**Methods:**

- `clean_data(dropna=True, fillna=False, strategy='mean')` - Clean data
- `normalize(columns=None)` - Normalize data
- `feature_engineering(new_features)` - Create new features
- `split_data(target_column, test_size=0.2, random_state=42)` - Split data
- `get_correlation_matrix()` - Get correlation matrix
- `get_descriptive_stats()` - Get descriptive statistics

**Example:**

```python
import pynlm
import pandas as pd
import numpy as np

# Create sample data
data = pd.DataFrame({
    'feature1': np.random.randn(100),
    'feature2': np.random.randn(100),
    'target': np.random.randn(100),
    'category': np.random.choice(['A', 'B', 'C'], 100)
})

# Create data processor
data_processor = pynlm.create_research_data_processor(data)

# Clean data
data_processor.clean_data(dropna=True, fillna=True, strategy='mean')

# Normalize data
data_processor.normalize()

# Feature engineering
data_processor.feature_engineering({
    'squared_feature1': lambda df: df['feature1'] ** 2,
    'interaction': lambda df: df['feature1'] * df['feature2']
})

# Split data
X_train, X_test, y_train, y_test = data_processor.split_data('target')

# Get correlation matrix
corr_matrix = data_processor.get_correlation_matrix()

# Get descriptive statistics
describe_stats = data_processor.get_descriptive_stats()
```

### Data Cleaning

The `clean_data()` method cleans and preprocesses data.

**Parameters:**

- `dropna`: Whether to drop NaN values (default: True)
- `fillna`: Whether to fill NaN values (default: False)
- `strategy`: Strategy for filling NaN values ('mean', 'median', 'mode') (default: 'mean')

**Returns:**

- ResearchDataProcessor instance for method chaining

### Data Normalization

The `normalize()` method normalizes numeric data using standardization.

**Parameters:**

- `columns`: Columns to normalize (default: all numeric columns)

**Returns:**

- ResearchDataProcessor instance for method chaining

### Feature Engineering

The `feature_engineering()` method creates new features from existing data.

**Parameters:**

- `new_features`: Dictionary of feature names to functions

**Returns:**

- ResearchDataProcessor instance for method chaining

### Data Splitting

The `split_data()` method splits data into training and testing sets.

**Parameters:**

- `target_column`: Name of target column
- `test_size`: Proportion of test data (default: 0.2)
- `random_state`: Random seed for reproducibility (default: 42)

**Returns:**

- Tuple of (X_train, X_test, y_train, y_test)

## Machine Learning Integration

### Integration with scikit-learn

The analysis APIs integrate seamlessly with scikit-learn for machine learning workflows:

```python
import pynlm
import pandas as pd
from sklearn.linear_model import LogisticRegression
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import cross_val_score

# Create sample data
data = pd.DataFrame({
    'feature1': np.random.randn(100),
    'feature2': np.random.randn(100),
    'target': np.random.randint(0, 2, 100)
})

# Create data processor
data_processor = pynlm.create_research_data_processor(data)
data_processor.clean_data().normalize()

# Split data
X_train, X_test, y_train, y_test = data_processor.split_data('target')

# Create machine learning models
lr_model = LogisticRegression(random_state=42)
rf_model = RandomForestClassifier(random_state=42)

# Train models
lr_model.fit(X_train, y_train)
rf_model.fit(X_train, y_train)

# Evaluate models
lr_score = lr_model.score(X_test, y_test)
rf_score = rf_model.score(X_test, y_test)

print(f"Logistic Regression Accuracy: {lr_score:.3f}")
print(f"Random Forest Accuracy: {rf_score:.3f}")

# Cross-validation
lr_cv_scores = cross_val_score(lr_model, X_train, y_train, cv=5)
rf_cv_scores = cross_val_score(rf_model, X_train, y_train, cv=5)

print(f"Logistic Regression CV: {lr_cv_scores.mean():.3f} (+/- {lr_cv_scores.std():.3f})")
print(f"Random Forest CV: {rf_cv_scores.mean():.3f} (+/- {rf_cv_scores.std():.3f})")
```

## Time Series Analysis

### Time Series Decomposition

The `time_series_analysis()` method performs time series decomposition and forecasting.

**Features:**

- **Trend Analysis**: Identifies long-term patterns
- **Seasonal Analysis**: Detects periodic patterns
- **Residual Analysis**: Analyzes random variations
- **Forecasting**: Predicts future values

**Example:**

```python
import pynlm
import pandas as pd
import numpy as np

# Create time series data
time_index = pd.date_range('2020-01-01', periods=100, freq='D')
data = pd.Series(
    np.random.randn(100).cumsum() + np.sin(np.arange(100) * 2 * np.pi / 50),
    index=time_index
)

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(pd.DataFrame({'data': data}))

# Perform time series analysis
ts_results = analysis.time_series_analysis(data, forecast_periods=10)

print("Trend component (first 5 values):", ts_results['trend'].head().tolist())
print("Seasonal component (first 5 values):", ts_results['seasonal'].head().tolist())
print("Forecast values:", ts_results['forecast'])
```

## Performance Optimization

### Efficient Statistical Analysis

1. **Vectorized Operations**: Use vectorized operations where possible
2. **Memory Management**: Monitor memory usage for large datasets
3. **Algorithm Selection**: Choose appropriate algorithms for dataset size
4. **Parallel Processing**: Use parallel processing for independent computations

### Data Processing Optimization

1. **Lazy Evaluation**: Use lazy evaluation for large datasets
2. **Caching**: Cache processed data when needed multiple times
3. **Data Sampling**: Sample data for large datasets
4. **Memory Efficiency**: Use memory-efficient data structures

### Visualization Performance

1. **Batch Operations**: Generate multiple plots before displaying
2. **Resource Management**: Monitor system resources
3. **Adaptive Quality**: Adjust quality based on system resources

## Examples

### Example 1: Basic Statistical Analysis

```python
import pynlm
import pandas as pd
import numpy as np

# Create sample data
data = pd.DataFrame({
    'group1': np.random.normal(100, 15, 50),
    'group2': np.random.normal(105, 20, 50),
    'time': np.arange(50),
    'value': np.random.exponential(2, 50)
})

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(data)

# Describe data
print("Data Description:")
describe_results = analysis.describe()
for column, stats in describe_results.items():
    print(f"  {column}: mean={stats.mean:.2f}, std={stats.std:.2f}, min={stats.min:.2f}, max={stats.max:.2f}")

# Perform t-test
print("\nT-Test Results:")
t_test_results = analysis.t_test(data['group1'], data['group2'])
print(f"  t-statistic: {t_test_results['t_statistic']:.3f}")
print(f"  p-value: {t_test_results['p_value']:.3f}")
print(f"  significant: {t_test_results['significant']}")

# Perform correlation analysis
print("\nCorrelation Analysis:")
corr_results = analysis.correlation_analysis(data['time'], data['value'])
print(f"  correlation: {corr_results.correlation:.3f}")
print(f"  p-value: {corr_results.p_value:.3f}")
print(f"  sample size: {corr_results.sample_size}")

# Generate report
print("\nStatistical Report:")
report = analysis.generate_report()
print(report)
```

### Example 2: Comprehensive Data Processing

```python
import pynlm
import pandas as pd
import numpy as np
from sklearn.datasets import make_classification

# Generate sample dataset
X, y = make_classification(
    n_samples=1000,
    n_features=10,
    n_informative=5,
    n_redundant=2,
    random_state=42
)

# Create DataFrame
data = pd.DataFrame(X, columns=[f'feature_{i}' for i in range(10)])
data['target'] = y

# Add some realistic patterns
data['feature_1'] = data['feature_1'] + np.random.normal(5, 2, 1000)
data['feature_2'] = data['feature_2'] * np.random.uniform(0.5, 1.5, 1000)
data['feature_3'] = np.where(data['target'] == 1, np.random.normal(2, 1, 1000), np.random.normal(-2, 1, 1000))

# Add missing values
data.loc[10:20, 'feature_5'] = np.nan
data.loc[30:40, 'feature_7'] = np.nan

print("Original data shape:", data.shape)
print("Missing values:", data.isnull().sum())

# Create data processor
data_processor = pynlm.create_research_data_processor(data)

# Clean data
data_processor.clean_data(dropna=False, fillna=True, strategy='mean')
print("\nAfter cleaning - missing values:", data_processor.processed_data.isnull().sum())

# Normalize data
data_processor.normalize()
print("\nNormalized data statistics:")
normalized_stats = data_processor.get_descriptive_stats()
for column, stats in normalized_stats.items():
    print(f"  {column}: mean={stats['mean']:.3f}, std={stats['std']:.3f}")

# Feature engineering
data_processor.feature_engineering({
    'squared_feature_1': lambda df: df['feature_1'] ** 2,
    'interaction_feature_1_2': lambda df: df['feature_1'] * df['feature_2'],
    'log_feature_3': lambda df: np.log1p(np.abs(df['feature_3']))
})

print("\nAfter feature engineering - data shape:", data_processor.processed_data.shape)

# Split data
X_train, X_test, y_train, y_test = data_processor.split_data('target')
print(f"\nTrain/test split: {X_train.shape[0]} train, {X_test.shape[0]} test")

# Get correlation matrix
print("\nCorrelation matrix (top 10 correlations):")
corr_matrix = data_processor.get_correlation_matrix().abs().unstack()
df_corr = pd.DataFrame(corr_matrix)

# Get upper triangle without diagonal
upper = df_corr.where(np.triu(np.ones(df_corr.shape), k=1).astype(bool))

# Find top 10 correlations
top_correlations = upper.stack().sort_values(ascending=False).head(10)
for idx, corr in top_correlations.items():
    print(f"  {idx}: {corr:.3f}")
```

### Example 3: Time Series Analysis

```python
import pynlm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Create time series data
time_index = pd.date_range('2020-01-01', periods=365, freq='D')

# Generate synthetic time series with trend, seasonality, and noise
trend = np.linspace(0, 100, 365)
seasonality = 20 * np.sin(2 * np.pi * np.arange(365) / 365)
noise = np.random.normal(0, 5, 365)
data = pd.Series(trend + seasonality + noise, index=time_index, name='economic_indicator')

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(pd.DataFrame({'data': data}))

# Perform time series analysis
ts_results = analysis.time_series_analysis(data, forecast_periods=30)

# Create visualization
fig, axes = plt.subplots(4, 1, figsize=(12, 10))

# Plot original data
axes[0].plot(data.index, data.values, label='Original')
axes[0].set_title('Original Time Series')
axes[0].set_ylabel('Value')
axes[0].legend()

# Plot trend component
axes[1].plot(data.index, ts_results['trend'], label='Trend', color='orange')
axes[1].set_title('Trend Component')
axes[1].set_ylabel('Trend')
axes[1].legend()

# Plot seasonal component
axes[2].plot(data.index, ts_results['seasonal'], label='Seasonal', color='green')
axes[2].set_title('Seasonal Component')
axes[2].set_ylabel('Seasonality')
axes[2].legend()

# Plot residuals
axes[3].plot(data.index, ts_results['residual'], label='Residual', color='red')
axes[3].set_title('Residual Component')
axes[3].set_ylabel('Residual')
axes[3].set_xlabel('Date')
axes[3].legend()

plt.tight_layout()
plt.show()

# Plot forecast
fig2, ax = plt.subplots(figsize=(10, 6))
ax.plot(data.index, data.values, label='Historical', color='blue')
forecast_index = pd.date_range(data.index[-1], periods=31, freq='D')
ax.plot(forecast_index, ts_results['forecast'], label='Forecast', color='red', linestyle='--')
ax.set_title('Time Series Forecast')
ax.set_ylabel('Value')
ax.set_xlabel('Date')
ax.legend()
plt.tight_layout()
plt.show()

print(f"Forecast for next 30 days: {ts_results['forecast'][:5].tolist()}...")
```

### Example 4: Machine Learning Pipeline

```python
import pynlm
import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, classification_report
from sklearn.ensemble import RandomForestClassifier

# Create synthetic dataset
from sklearn.datasets import make_classification

X, y = make_classification(
    n_samples=1000,
    n_features=20,
    n_informative=10,
    n_redundant=5,
    n_clusters_per_class=2,
    random_state=42
)

# Create feature names
feature_names = [f'feature_{i}' for i in range(20)]
data = pd.DataFrame(X, columns=feature_names)
data['target'] = y

# Add some realistic patterns
data['feature_0'] = data['feature_0'] + np.random.normal(2, 1, 1000)
data['feature_1'] = data['feature_1'] * np.random.uniform(0.5, 2, 1000)
data['feature_2'] = np.where(data['target'] == 1, np.random.normal(1, 0.5, 1000), np.random.normal(-1, 0.5, 1000))

# Create data processor
data_processor = pynlm.create_research_data_processor(data)

# Preprocess data
data_processor.clean_data(dropna=True, fillna=False)
data_processor.normalize()

# Get correlation matrix
corr_matrix = data_processor.get_correlation_matrix()

# Remove highly correlated features to avoid multicollinearity
corr_matrix_abs = corr_matrix.abs()
upper = corr_matrix_abs.where(np.triu(np.ones(corr_matrix_abs.shape), k=1).astype(bool))
to_drop = [column for column in upper.columns if any(upper[column] > 0.95)]

print(f"Dropping {len(to_drop)} highly correlated features: {to_drop}")
data_processor.processed_data = data_processor.processed_data.drop(columns=to_drop)

# Split data
X_train, X_test, y_train, y_test = data_processor.split_data('target', test_size=0.2, random_state=42)

# Create and train Random Forest classifier
rf_model = RandomForestClassifier(n_estimators=100, random_state=42)
rf_model.fit(X_train, y_train)

# Make predictions
y_pred = rf_model.predict(X_test)

# Evaluate model
accuracy = accuracy_score(y_test, y_pred)
print(f"\nModel Accuracy: {accuracy:.3f}")

# Get feature importance
feature_importance = pd.DataFrame({
    'feature': X_train.columns,
    'importance': rf_model.feature_importances_
}).sort_values('importance', ascending=False)

print("\nTop 10 Feature Importances:")
print(feature_importance.head(10))

# Perform clustering analysis
clustering_results = data_processor.clustering_analysis(data_processor.processed_data.drop('target', axis=1), n_clusters=5)

print(f"\nClustering Results:")
print(f"Number of clusters: {clustering_results['n_clusters']}")
print(f"Cluster sizes: {np.bincount(clustering_results['cluster_labels'])}")

# Create statistical analysis for individual features
print("\nStatistical Analysis for Key Features:")
analysis = pynlm.create_statistical_analysis(data_processor.processed_data)
for feature in ['feature_0', 'feature_1', 'feature_2']:
    if feature in analysis.data.columns:
        feature_stats = analysis.describe()[feature]
        print(f"  {feature}: mean={feature_stats.mean:.3f}, std={feature_stats.std:.3f}")
```

## Installation

### Requirements

The analysis APIs require:

- Python >= 3.8
- NumPy >= 1.20
- Pandas >= 1.3.0
- Matplotlib >= 3.5.0
- Plotly >= 5.0.0
- SciPy >= 1.7.0
- scikit-learn >= 1.0.0
- statsmodels >= 0.13.0

### Installation

```bash
# Install with analysis support
pip install pynlm[analysis]

# Install with all dependencies
pip install pynlm[dev,examples,visualization,advanced_ml]
```

## Troubleshooting

### Common Issues

1. **Import Errors**: Ensure all dependencies are installed
2. **Data Processing Issues**: Check for missing values and data types
3. **Statistical Test Errors**: Ensure groups have sufficient data
4. **Memory Issues**: Monitor memory usage for large datasets

### Tips for Best Performance

1. **Use Vectorized Operations**: Use vectorized operations where possible
2. **Monitor Memory Usage**: Monitor memory usage for large datasets
3. **Optimize Algorithms**: Use efficient algorithms for specific data types
4. **Use Appropriate Tests**: Use appropriate statistical tests for data distributions

## File Formats

### Supported File Formats

- CSV: Comma-separated values
- JSON: JavaScript Object Notation
- Excel: Microsoft Excel format
- Parquet: Columnar storage format
- HDF5: Hierarchical Data Format

### Data Export

```python
# Export DataFrame to CSV
exporter.export_to_csv(data, 'data.csv')

# Export DataFrame to JSON
exporter.export_to_json(data, 'data.json')

# Export DataFrame to Excel
exporter.export_to_excel(data, 'data.xlsx')
```

## Conclusion

The NLM analysis APIs provide comprehensive statistical analysis and data processing capabilities for brain simulation results. Whether you need basic descriptive statistics, advanced hypothesis testing, or machine learning integration, the analysis APIs provide the tools you need.

The analysis APIs integrate seamlessly with the rest of the NLM enhanced bindings, providing a complete solution for analysis and export of brain simulation results.

## References

1. NumPy Documentation: https://numpy.org/doc/stable/
2. Pandas Documentation: https://pandas.pydata.org/docs/
3. SciPy Documentation: https://docs.scipy.org/doc/scipy/
4. scikit-learn Documentation: https://scikit-learn.org/stable/
5. statsmodels Documentation: https://www.statsmodels.org/stable/index.html
6. NLM Analysis Guide: https://nlm.readthedocs.io/en/latest/analysis_api.html

## Version History

- **Version 0.1.0**: Initial release of analysis APIs
- **Version 0.2.0**: Added comprehensive statistical analysis
- **Version 0.3.0**: Added data processing capabilities
- **Version 0.4.0**: Added machine learning integration
- **Version 0.5.0**: Added performance optimizations and bug fixes

## Future Roadmap

1. **Advanced Analytics**: Add more advanced statistical methods
2. **Automated Reporting**: Add automated report generation
3. **Cloud Analytics**: Support for cloud-based analytics
4. **Real-time Analytics**: Real-time analytics for streaming data
5. **Interactive Analysis**: Interactive web-based analysis tools

## Acknowledgments

The NLM analysis APIs are built on the contributions of the NumPy, pandas, SciPy, scikit-learn, and statsmodels communities. We thank them for providing these powerful analysis tools.

## License

This project is licensed under the MIT License. See the LICENSE file for details.