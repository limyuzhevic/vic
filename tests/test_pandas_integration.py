#!/usr/bin/env python3
"""
Pandas Integration Unit Tests for NLM
Comprehensive unit tests for NLM Pandas integration functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import pandas as pd
import numpy as np
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.integration.PandasIntegration import (
    PandasIntegration,
    DataFrames,
    SeriesOperations,
    DataFrameOperations,
    StatisticalAnalysis as PandasStatisticalAnalysis,
    DataFiltering
)

class TestPandasIntegration(unittest.TestCase):
    """Test cases for Pandas integration"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.integration = PandasIntegration()
        
        # Mock pynlm to avoid dependency issues
        self.pynlm_patch = patch.dict('sys.modules', {
            'pynlm': Mock(),
        })
        self.mock_pynlm = self.pynlm_patch.start()
    
    def tearDown(self):
        """Clean up test fixtures"""
        self.pynlm_patch.stop()
    
    def testPandasIntegrationInitialization(self):
        """Test PandasIntegration initialization"""
        integration = PandasIntegration()
        
        self.assertIsNotNone(integration.dataframes)
        self.assertIsNotNone(integration.series_ops)
        self.assertIsNotNone(integration.dataframe_ops)
        self.assertIsNotNone(integration.statistical_analysis)
        self.assertIsNotNone(integration.data_filtering)
    
    def testDataFramesInitialization(self):
        """Test DataFrames initialization"""
        dataframes = DataFrames()
        
        self.assertEqual(dataframes.dataframes, {})
        self.assertEqual(dataframes.series, {})
    
    def testDataFramesCreateDataFrame(self):
        """Test creating data frames"""
        dataframes = DataFrames()
        
        # Create sample data
        data1 = {
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        }
        
        df1 = dataframes.create_dataframe('neuron_data', data1)
        
        self.assertIsInstance(df1, pd.DataFrame)
        self.assertEqual(len(df1), 5)
        self.assertIn('neuron_id', df1.columns)
        self.assertIn('region_id', df1.columns)
        self.assertIn('membrane_potential', df1.columns)
        self.assertIn('firing_state', df1.columns)
        
        # Store and retrieve
        dataframes.store_dataframe('neuron_data', df1)
        retrieved = dataframes.get_dataframe('neuron_data')
        
        self.assertIsInstance(retrieved, pd.DataFrame)
        self.assertEqual(len(retrieved), 5)
    
    def testDataFramesCreateSeries(self):
        """Test creating series"""
        dataframes = DataFrames()
        
        # Create sample data
        series_data = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], name='values')
        
        s1 = dataframes.create_series('values', series_data)
        
        self.assertIsInstance(s1, pd.Series)
        self.assertEqual(len(s1), 5)
        self.assertEqual(s1.name, 'values')
        
        # Store and retrieve
        dataframes.store_series('values', s1)
        retrieved = dataframes.get_series('values')
        
        self.assertIsInstance(retrieved, pd.Series)
        self.assertEqual(len(retrieved), 5)
    
    def testSeriesOperationsBasicOperations(self):
        """Test basic series operations"""
        series_ops = SeriesOperations()
        
        # Create sample series
        s1 = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0])
        s2 = pd.Series([10.0, 20.0, 30.0, 40.0, 50.0])
        
        # Test arithmetic operations
        result = series_ops.add_series(s1, s2)
        expected = pd.Series([11.0, 22.0, 33.0, 44.0, 55.0])
        pd.testing.assert_series_equal(result, expected)
        
        result = series_ops.subtract_series(s1, s2)
        expected = pd.Series([-9.0, -18.0, -27.0, -36.0, -45.0])
        pd.testing.assert_series_equal(result, expected)
        
        result = series_ops.multiply_series(s1, s2)
        expected = pd.Series([10.0, 40.0, 90.0, 160.0, 250.0])
        pd.testing.assert_series_equal(result, expected)
        
        result = series_ops.divide_series(s1, s2)
        expected = pd.Series([0.1, 0.1, 0.1, 0.1, 0.1])
        pd.testing.assert_series_equal(result, expected)
        
        # Test aggregation operations
        result = series_ops.sum_series(s1)
        self.assertEqual(result, 15.0)
        
        result = series_ops.mean_series(s1)
        self.assertEqual(result, 3.0)
        
        result = series_ops.std_series(s1)
        self.assertAlmostEqual(result, np.std(s1))
        
        result = series_ops.min_series(s1)
        self.assertEqual(result, 1.0)
        
        result = series_ops.max_series(s1)
        self.assertEqual(result, 5.0)
    
    def testSeriesOperationsStatisticalOperations(self):
        """Test series statistical operations"""
        series_ops = SeriesOperations()
        
        # Create sample series
        s1 = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0])
        
        # Test percentiles
        result = series_ops.percentile(s1, 25)
        self.assertAlmostEqual(result, 3.25)
        
        result = series_ops.percentile(s1, 50)
        self.assertEqual(result, 5.5)
        
        result = series_ops.percentile(s1, 75)
        self.assertAlmostEqual(result, 7.75)
        
        # Test quantiles
        result = series_ops.quantile(s1, 0.25)
        self.assertAlmostEqual(result, 3.25)
        
        result = series_ops.quantile(s1, 0.5)
        self.assertEqual(result, 5.5)
        
        result = series_ops.quantile(s1, 0.75)
        self.assertAlmostEqual(result, 7.75)
        
        # Test summary statistics
        summary = series_ops.get_summary_statistics(s1)
        
        self.assertIn('mean', summary)
        self.assertIn('std', summary)
        self.assertIn('median', summary)
        self.assertIn('min', summary)
        self.assertIn('max', summary)
        self.assertIn('range', summary)
        self.assertIn('interquartile_range', summary)
        
        self.assertAlmostEqual(summary['mean'], 5.5)
        self.assertAlmostEqual(summary['median'], 5.5)
        self.assertEqual(summary['min'], 1.0)
        self.assertEqual(summary['max'], 10.0)
        self.assertAlmostEqual(summary['range'], 9.0)
    
    def testSeriesOperationsDataHandling(self):
        """Test series data handling"""
        series_ops = SeriesOperations()
        
        # Create sample series
        s1 = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0])
        
        # Test missing values
        s2 = pd.Series([1.0, np.nan, 3.0, np.nan, 5.0])
        
        result = series_ops.fill_missing(s2, 0.0)
        self.assertEqual(result.isna().sum(), 0)
        
        # Test interpolation
        result = series_ops.interpolate(s2)
        self.assertEqual(result.isna().sum(), 0)
        
        # Test rolling window operations
        result = series_ops.rolling_mean(s1, window=3)
        self.assertEqual(len(result), 5)
        self.assertTrue(pd.notna(result).all())
        
        # Test cumulative operations
        result = series_ops.cumulative_sum(s1)
        expected = pd.Series([1.0, 3.0, 6.0, 10.0, 15.0])
        pd.testing.assert_series_equal(result, expected)
    
    def testDataFrameOperations(self):
        """Test data frame operations"""
        dataframe_ops = DataFrameOperations()
        
        # Create sample data frames
        df1 = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        df2 = pd.DataFrame({
            'neuron_id': [6, 7, 8, 9, 10],
            'region_id': [1, 2, 1, 2, 3],
            'membrane_potential': [-69.0, -66.0, -73.0, -67.0, -70.0],
            'firing_state': [1, 0, 1, 0, 1]
        })
        
        # Test concatenation
        result = dataframe_ops.concatenate_dataframes([df1, df2])
        self.assertEqual(len(result), 10)
        self.assertTrue((result['neuron_id'] >= 1).all())
        
        # Test filtering
        result = dataframe_ops.filter_dataframe(df1, 'region_id', 1)
        self.assertEqual(len(result), 2)
        self.assertTrue((result['region_id'] == 1).all())
        
        # Test grouping
        result = dataframe_ops.groupby_and_aggregate(df1, 'region_id', {'firing_state': 'mean'})
        self.assertEqual(len(result), 3)
        self.assertIn('firing_state_mean', result.columns)
        
        # Test sorting
        result = dataframe_ops.sort_dataframe(df1, 'neuron_id')
        self.assertTrue((result['neuron_id'].sort_values().reset_index(drop=True) == 
                        pd.Series([1, 2, 3, 4, 5])).all())
        
        # Test merging
        result = dataframe_ops.merge_dataframes(df1, df2, 'neuron_id')
        self.assertEqual(len(result), 0)  # No matching neuron_ids
    
    def testPandasStatisticalAnalysis(self):
        """Test Pandas statistical analysis"""
        analysis = PandasStatisticalAnalysis()
        
        # Create sample data
        df = pd.DataFrame({
            'group': ['A', 'A', 'A', 'B', 'B', 'B', 'C', 'C', 'C'],
            'value1': [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0],
            'value2': [10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0]
        })
        
        # Test descriptive statistics
        stats = analysis.describe(df)
        
        self.assertIn('shape', stats)
        self.assertIn('columns', stats)
        self.assertIn('dtypes', stats)
        self.assertIn('describe', stats)
        
        self.assertEqual(stats['shape'], (9, 3))
        self.assertIn('group', stats['columns'])
        self.assertIn('value1', stats['columns'])
        self.assertIn('value2', stats['columns'])
        
        # Test correlation analysis
        corr_result = analysis.correlation_analysis(df, ['value1', 'value2'])
        
        self.assertIn('correlation_matrix', corr_result)
        self.assertIn('high_correlation_pairs', corr_result)
        self.assertIn('correlation_summary', corr_result)
        
        # Test hypothesis testing
        group1 = df[df['group'] == 'A']['value1']
        group2 = df[df['group'] == 'B']['value1']
        
        ttest_result = analysis.t_test(group1, group2)
        
        self.assertIn('t_statistic', ttest_result)
        self.assertIn('p_value', ttest_result)
        self.assertIn('degrees_of_freedom', ttest_result)
        self.assertIn('significant', ttest_result)
        
        # Test regression analysis
        X = df[['value1', 'value2']]
        y = df['group']
        
        reg_result = analysis.regression_analysis(X, y)
        
        self.assertIn('model', reg_result)
        self.assertIn('coefficients', reg_result)
        self.assertIn('r_squared', reg_result)
        self.assertIn('p_values', reg_result)
    
    def testDataFiltering(self):
        """Test data filtering"""
        filtering = DataFiltering()
        
        # Create sample data
        df = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
            'region_id': [1, 1, 2, 2, 3, 3, 1, 2, 3, 1],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0, -69.0, -67.0, -66.0, -73.0, -70.0],
            'firing_state': [0, 1, 0, 1, 0, 1, 1, 0, 0, 1],
            'activity_level': ['low', 'high', 'medium', 'high', 'low', 'medium', 'high', 'low', 'medium', 'high']
        })
        
        # Test numeric filtering
        result = filtering.filter_numeric(df, 'neuron_id', 5, 10)
        self.assertEqual(len(result), 6)
        self.assertTrue((result['neuron_id'] >= 5).all())
        
        # Test categorical filtering
        result = filtering.filter_categorical(df, 'activity_level', ['high', 'medium'])
        self.assertEqual(len(result), 7)
        self.assertTrue(result['activity_level'].isin(['high', 'medium']).all())
        
        # Test conditional filtering
        result = filtering.filter_conditions(df, 'region_id', 1, 'neuron_id', 5)
        self.assertEqual(len(result), 3)  # 3 neurons in region 1, neuron_id <= 5
        
        # Test outlier detection
        result = filtering.detect_outliers(df, 'membrane_potential', method='iqr')
        self.assertIn('outliers', result)
        self.assertIn('total_outliers', result)
        self.assertIn('outlier_indices', result)
        
        # Test duplicate removal
        duplicate_df = pd.concat([df, df.iloc[:3]], ignore_index=True)
        result = filtering.remove_duplicates(duplicate_df)
        self.assertEqual(len(result), len(df))
    
    def testPandasIntegrationCreateBrainFromDataFrame(self):
        """Test creating brain from Pandas DataFrames"""
        # Mock pynlm functions
        mock_config = Mock()
        mock_brain = Mock()
        
        with patch('pynlm.createDefaultConfig') as mock_create_config:
            with patch('pynlm.createBrain') as mock_create_brain:
                mock_create_config.return_value = mock_config
                mock_create_brain.return_value = mock_brain
                
                # Create sample dataframes
                neuron_data = pd.DataFrame({
                    'neuron_id': range(100),
                    'region_id': [1] * 50 + [2] * 50,
                    'membrane_potential': np.random.randn(100)
                })
                
                region_data = pd.DataFrame({
                    'region_id': [1, 2, 3],
                    'neuron_count': [50, 50, 0]
                })
                
                connection_data = pd.DataFrame({
                    'source_neuron': range(100),
                    'target_neuron': range(1, 101),
                    'weight': np.random.rand(100)
                })
                
                # Create brain from dataframes
                brain = self.integration.create_brain_from_dataframes(
                    neuron_data, region_data, connection_data
                )
                
                self.assertEqual(brain, mock_brain)
                mock_create_brain.assert_called_once()
    
    def testPandasIntegrationConvertBrainToDataFrame(self):
        """Test converting brain to Pandas DataFrames"""
        # Mock brain
        mock_brain = Mock()
        mock_brain.total_neuron_count = Mock(return_value=100)
        mock_brain.total_synapse_count = Mock(return_value=1000)
        
        # Mock pynlm functions
        with patch('pynlm.createBrain') as mock_create_brain:
            mock_create_brain.return_value = mock_brain
            
            # Create brain and convert to dataframes
            brain = self.integration.create_brain_from_dataframes(
                pd.DataFrame({'neuron_id': range(100)}),
                pd.DataFrame({'region_id': [1, 2]}),
                pd.DataFrame({'source_neuron': range(100), 'target_neuron': range(1, 101), 'weight': np.ones(100)})
            )
            
            neuron_df = self.integration.convert_brain_to_dataframe(brain, 'neurons')
            synapse_df = self.integration.convert_brain_to_dataframe(brain, 'synapses')
            
            self.assertIsInstance(neuron_df, pd.DataFrame)
            self.assertIsInstance(synapse_df, pd.DataFrame)
            self.assertEqual(neuron_df.shape, (100,))
            self.assertEqual(synapse_df.shape, (100, 3))
    
    def testPandasIntegrationBatchProcessing(self):
        """Test batch processing with Pandas"""
        # Create multiple datasets
        dataset1 = pd.DataFrame({'value': range(100), 'group': ['A'] * 50 + ['B'] * 50})
        dataset2 = pd.DataFrame({'value': range(200, 400), 'group': ['A'] * 100 + ['B'] * 100})
        dataset3 = pd.DataFrame({'value': range(400, 550), 'group': ['A'] * 50 + ['B'] * 50})
        
        datasets = [dataset1, dataset2, dataset3]
        
        # Process datasets in batch
        batch_result = self.integration.batch_process(datasets, 'analyze')
        
        self.assertIsInstance(batch_result, list)
        self.assertEqual(len(batch_result), len(datasets))
        
        # Verify each dataset was processed
        for result in batch_result:
            self.assertIsInstance(result, pd.DataFrame)
            self.assertEqual(len(result.columns), len(datasets[0].columns))
    
    def testPandasIntegrationErrorHandling(self):
        """Test error handling in Pandas integration"""
        # Test with invalid data
        invalid_data = "not a dataframe"
        
        # Should handle error gracefully
        try:
            result = self.integration.dataframe_ops.concat_dataframes(invalid_data, pd.DataFrame())
            self.fail("Expected exception")
        except Exception:
            pass  # Expected
        
        # Test with mismatched columns
        df1 = pd.DataFrame({'a': [1, 2, 3]})
        df2 = pd.DataFrame({'b': [4, 5, 6]})
        
        try:
            result = self.integration.dataframe_ops.merge_dataframes(df1, df2, 'non_existent_column')
            self.fail("Expected exception")
        except Exception:
            pass  # Expected

if __name__ == '__main__':
    unittest.main()
