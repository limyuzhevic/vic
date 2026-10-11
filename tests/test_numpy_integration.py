#!/usr/bin/env python3
"""
NumPy Integration Unit Tests for NLM
Comprehensive unit tests for NLM NumPy integration functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import numpy as np
import pandas as pd
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.integration.NumPyIntegration import (
    NumPyIntegration,
    ArrayOperations,
    MatrixOperations,
    StatisticalAnalysis,
    ArrayConversions
)

class TestNumPyIntegration(unittest.TestCase):
    """Test cases for NumPy integration"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.integration = NumPyIntegration()
        
        # Mock pynlm to avoid dependency issues
        self.pynlm_patch = patch.dict('sys.modules', {
            'pynlm': Mock(),
        })
        self.mock_pynlm = self.pynlm_patch.start()
    
    def tearDown(self):
        """Clean up test fixtures"""
        self.pynlm_patch.stop()
    
    def testNumPyIntegrationInitialization(self):
        """Test NumPyIntegration initialization"""
        integration = NumPyIntegration()
        
        self.assertIsNotNone(integration.array_ops)
        self.assertIsNotNone(integration.matrix_ops)
        self.assertIsNotNone(integration.statistical_analysis)
        self.assertIsNotNone(integration.array_conversions)
    
    def testArrayOperationsBasicOperations(self):
        """Test basic array operations"""
        array_ops = ArrayOperations()
        
        # Create test arrays
        arr1 = np.array([1, 2, 3, 4, 5])
        arr2 = np.array([10, 20, 30, 40, 50])
        
        # Test element-wise operations
        result = array_ops.add_arrays(arr1, arr2)
        np.testing.assert_array_equal(result, np.array([11, 22, 33, 44, 55]))
        
        result = array_ops.subtract_arrays(arr1, arr2)
        np.testing.assert_array_equal(result, np.array([-9, -18, -27, -36, -45]))
        
        result = array_ops.multiply_arrays(arr1, arr2)
        np.testing.assert_array_equal(result, np.array([10, 40, 90, 160, 250]))
        
        result = array_ops.divide_arrays(arr1, arr2)
        np.testing.assert_array_almost_equal(result, np.array([0.1, 0.1, 0.1, 0.1, 0.1]))
        
        # Test aggregation operations
        result = array_ops.sum_array(arr1)
        self.assertEqual(result, 15)
        
        result = array_ops.mean_array(arr1)
        self.assertEqual(result, 3.0)
        
        result = array_ops.std_array(arr1)
        self.assertAlmostEqual(result, np.std(arr1))
        
        result = array_ops.min_array(arr1)
        self.assertEqual(result, 1)
        
        result = array_ops.max_array(arr1)
        self.assertEqual(result, 5)
    
    def testArrayOperationsStatisticalOperations(self):
        """Test array statistical operations"""
        array_ops = ArrayOperations()
        
        # Create test data
        data = np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0])
        
        # Test percentiles
        result = array_ops.percentile(data, 25)
        self.assertAlmostEqual(result, 3.25)
        
        result = array_ops.percentile(data, 50)
        self.assertEqual(result, 5.5)
        
        result = array_ops.percentile(data, 75)
        self.assertAlmostEqual(result, 7.75)
        
        # Test quantiles
        result = array_ops.quantile(data, 0.25)
        self.assertAlmostEqual(result, 3.25)
        
        result = array_ops.quantile(data, 0.5)
        self.assertEqual(result, 5.5)
        
        result = array_ops.quantile(data, 0.75)
        self.assertAlmostEqual(result, 7.75)
        
        # Test summary statistics
        summary = array_ops.get_summary_statistics(data)
        
        self.assertIn('mean', summary)
        self.assertIn('std', summary)
        self.assertIn('min', summary)
        self.assertIn('max', summary)
        self.assertIn('median', summary)
        self.assertIn('mode', summary)
        self.assertIn('range', summary)
        self.assertIn('interquartile_range', summary)
        
        self.assertAlmostEqual(summary['mean'], 5.5)
        self.assertAlmostEqual(summary['median'], 5.5)
        self.assertEqual(summary['min'], 1.0)
        self.assertEqual(summary['max'], 10.0)
        self.assertAlmostEqual(summary['range'], 9.0)
    
    def testArrayOperationsDataGeneration(self):
        """Test array data generation"""
        array_ops = ArrayOperations()
        
        # Test random data generation
        result = array_ops.generate_random_data(size=100, distribution='uniform')
        self.assertEqual(len(result), 100)
        self.assertTrue(np.all(result >= 0) and np.all(result <= 1))
        
        result = array_ops.generate_random_data(size=100, distribution='normal')
        self.assertEqual(len(result), 100)
        self.assertTrue(np.all(result > -10) and np.all(result < 10))
        
        result = array_ops.generate_random_data(size=100, distribution='exponential')
        self.assertEqual(len(result), 100)
        self.assertTrue(np.all(result >= 0))
        
        # Test deterministic data generation
        result = array_ops.generate_deterministic_data(pattern='linear', size=10)
        expected = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
        np.testing.assert_array_equal(result, expected)
        
        result = array_ops.generate_deterministic_data(pattern='sinusoidal', size=5)
        self.assertEqual(len(result), 5)
        self.assertTrue(np.all(result >= -1) and np.all(result <= 1))
        
        result = array_ops.generate_deterministic_data(pattern='step', size=5)
        expected = np.array([0, 0, 0, 1, 1])
        np.testing.assert_array_equal(result, expected)
    
    def testArrayConversions(self):
        """Test array conversions"""
        conversions = ArrayConversions()
        
        # Test Python to NumPy
        python_list = [1, 2, 3, 4, 5]
        numpy_array = conversions.python_to_numpy(python_list)
        self.assertIsInstance(numpy_array, np.ndarray)
        np.testing.assert_array_equal(numpy_array, np.array(python_list))
        
        # Test pandas to NumPy
        pandas_df = pd.DataFrame({'value': [1, 2, 3, 4, 5]})
        numpy_array = conversions.pandas_to_numpy(pandas_df)
        self.assertIsInstance(numpy_array, np.ndarray)
        
        # Test NumPy to pandas
        numpy_array = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        pandas_df = conversions.numpy_to_pandas(numpy_array)
        self.assertIsInstance(pandas_df, pd.DataFrame)
        self.assertIn('value', pandas_df.columns)
        
        # Test serialization
        data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        json_str = conversions.serialize_array(data)
        self.assertIsInstance(json_str, str)
        
        deserialized = conversions.deserialize_array(json_str)
        np.testing.assert_array_almost_equal(deserialized, data)
    
    def testMatrixOperations(self):
        """Test matrix operations"""
        matrix_ops = MatrixOperations()
        
        # Create test matrices
        matrix1 = np.array([[1, 2], [3, 4]])
        matrix2 = np.array([[5, 6], [7, 8]])
        
        # Test basic operations
        result = matrix_ops.add_matrices(matrix1, matrix2)
        expected = np.array([[6, 8], [10, 12]])
        np.testing.assert_array_equal(result, expected)
        
        result = matrix_ops.subtract_matrices(matrix1, matrix2)
        expected = np.array([[-4, -4], [-4, -4]])
        np.testing.assert_array_equal(result, expected)
        
        # Test matrix multiplication
        result = matrix_ops.multiply_matrices(matrix1, matrix2)
        expected = np.array([[19, 22], [43, 50]])
        np.testing.assert_array_equal(result, expected)
        
        # Test linear algebra operations
        matrix_square = np.array([[2, 1], [1, 2]])
        result = matrix_ops.invert_matrix(matrix_square)
        expected = np.array([[0.5, -0.25], [-0.25, 0.5]])
        np.testing.assert_array_almost_equal(result, expected, decimal=5)
        
        result = matrix_ops.determinant(matrix_square)
        self.assertEqual(result, 3.0)
        
        # Test eigenvalue decomposition
        result = matrix_ops.eigen_decomposition(matrix_square)
        self.assertIn('eigenvalues', result)
        self.assertIn('eigenvectors', result)
        self.assertEqual(len(result['eigenvalues']), 2)
        self.assertEqual(len(result['eigenvectors']), 2)
    
    def testStatisticalAnalysis(self):
        """Test statistical analysis"""
        analysis = StatisticalAnalysis()
        
        # Create test data
        data = np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0])
        
        # Test basic statistics
        stats = analysis.describe(data)
        
        self.assertIn('mean', stats)
        self.assertIn('std', stats)
        self.assertIn('median', stats)
        self.assertIn('min', stats)
        self.assertIn('max', stats)
        self.assertIn('range', stats)
        self.assertIn('variance', stats)
        
        self.assertAlmostEqual(stats['mean'], 5.5)
        self.assertAlmostEqual(stats['std'], np.std(data))
        self.assertEqual(stats['median'], 5.5)
        self.assertEqual(stats['min'], 1.0)
        self.assertEqual(stats['max'], 10.0)
        
        # Test distribution analysis
        dist_result = analysis.analyze_distribution(data, 'normal')
        
        self.assertIn('distribution', dist_result)
        self.assertIn('mean', dist_result)
        self.assertIn('std', dist_result)
        self.assertIn('skewness', dist_result)
        self.assertIn('kurtosis', dist_result)
        
        # Test hypothesis testing
        group1 = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        group2 = np.array([2.0, 3.0, 4.0, 5.0, 6.0])
        
        ttest_result = analysis.t_test(group1, group2)
        
        self.assertIn('t_statistic', ttest_result)
        self.assertIn('p_value', ttest_result)
        self.assertIn('degrees_of_freedom', ttest_result)
        self.assertIn('significant', ttest_result)
        
        # Test correlation analysis
        x = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        y = np.array([2.0, 4.0, 6.0, 8.0, 10.0])
        
        corr_result = analysis.correlation_analysis(x, y)
        
        self.assertIn('correlation_coefficient', corr_result)
        self.assertIn('p_value', corr_result)
        self.assertIn('significant', corr_result)
        
        self.assertAlmostEqual(corr_result['correlation_coefficient'], 1.0)
    
    def testNumPyIntegrationCreateBrainFromArray(self):
        """Test creating brain from NumPy arrays"""
        # Mock pynlm functions
        mock_config = Mock()
        mock_brain = Mock()
        
        with patch('pynlm.createDefaultConfig') as mock_create_config:
            with patch('pynlm.createBrain') as mock_create_brain:
                mock_create_config.return_value = mock_config
                mock_create_brain.return_value = mock_brain
                
                # Create test arrays
                neuron_data = np.array([1, 2, 3, 4, 5])
                region_data = np.array([1, 1, 2, 2, 3])
                connection_data = np.random.rand(5, 5)
                
                # Create brain from arrays
                brain = self.integration.create_brain_from_arrays(neuron_data, region_data, connection_data)
                
                self.assertEqual(brain, mock_brain)
                mock_create_brain.assert_called_once()
    
    def testNumPyIntegrationConvertBrainToArray(self):
        """Test converting brain to NumPy arrays"""
        # Mock brain
        mock_brain = Mock()
        mock_brain.total_neuron_count = Mock(return_value=100)
        mock_brain.total_synapse_count = Mock(return_value=1000)
        
        # Mock pynlm functions
        with patch('pynlm.createBrain') as mock_create_brain:
            mock_create_brain.return_value = mock_brain
            
            # Create brain and convert to arrays
            brain = self.integration.create_brain_from_arrays(
                np.ones(100), np.ones(100), np.random.rand(100, 100)
            )
            
            neuron_array = self.integration.convert_brain_to_array(brain, 'neurons')
            synapse_array = self.integration.convert_brain_to_array(brain, 'synapses')
            
            self.assertIsInstance(neuron_array, np.ndarray)
            self.assertIsInstance(synapse_array, np.ndarray)
            self.assertEqual(neuron_array.shape, (100,))
            self.assertEqual(synapse_array.shape, (100, 100))
    
    def testNumPyIntegrationBatchProcessing(self):
        """Test batch processing with NumPy"""
        # Create multiple datasets
        dataset1 = np.random.rand(100, 5)
        dataset2 = np.random.rand(200, 5)
        dataset3 = np.random.rand(150, 5)
        
        datasets = [dataset1, dataset2, dataset3]
        
        # Process datasets in batch
        batch_result = self.integration.batch_process(datasets, 'normalize')
        
        self.assertIsInstance(batch_result, list)
        self.assertEqual(len(batch_result), len(datasets))
        
        # Verify each dataset was processed
        for result in batch_result:
            self.assertIsInstance(result, np.ndarray)
            self.assertEqual(result.shape, (datasets[0].shape[0], datasets[0].shape[1]))
    
    def testNumPyIntegrationPerformanceAnalysis(self):
        """Test NumPy performance analysis"""
        # Create large dataset
        large_data = np.random.rand(10000, 10)
        
        # Performance test
        performance = self.integration.analyze_performance(large_data, 'comprehensive')
        
        self.assertIn('processing_time', performance)
        self.assertIn('memory_usage', performance)
        self.assertIn('throughput', performance)
        self.assertIn('efficiency', performance)
        
        # Memory efficiency test
        memory_efficiency = self.integration.analyze_memory_efficiency(large_data)
        
        self.assertIn('array_size', memory_efficiency)
        self.assertIn('memory_usage', memory_efficiency)
        self.assertIn('memory_efficiency_ratio', memory_efficiency)
    
    def testNumPyIntegrationErrorHandling(self):
        """Test error handling in NumPy integration"""
        # Test with invalid data
        invalid_data = "not an array"
        
        # Should handle error gracefully
        try:
            result = self.integration.array_ops.add_arrays(invalid_data, np.array([1, 2, 3]))
            self.fail("Expected exception")
        except Exception:
            pass  # Expected
        
        # Test with mismatched shapes
        arr1 = np.array([1, 2, 3])
        arr2 = np.array([[1, 2], [3, 4]])
        
        try:
            result = self.integration.array_ops.add_arrays(arr1, arr2)
            self.fail("Expected exception")
        except Exception:
            pass  # Expected

if __name__ == '__main__':
    unittest.main()
