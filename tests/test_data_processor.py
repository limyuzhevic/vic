#!/usr/bin/env python3
"""
Data Processor Unit Tests for NLM
Comprehensive unit tests for NLM DataProcessor functionality
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

from python.data.DataProcessor import (
    DataProcessor,
    DataValidator,
    DataNormalizer,
    DataAggregator,
    DataExporter,
    DataType
)

class TestDataProcessor(unittest.TestCase):
    """Test cases for DataProcessor class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.processor = DataProcessor()
        
        # Mock pynlm to avoid dependency issues
        self.pynlm_patch = patch.dict('sys.modules', {
            'pynlm': Mock(),
        })
        self.mock_pynlm = self.pynlm_patch.start()
    
    def tearDown(self):
        """Clean up test fixtures"""
        self.pynlm_patch.stop()
    
    def testDataProcessorInitialization(self):
        """Test DataProcessor initialization"""
        processor = DataProcessor()
        
        self.assertIsNotNone(processor.validator)
        self.assertIsNotNone(processor.normalizer)
        self.assertIsNotNone(processor.aggregator)
        self.assertIsNotNone(processor.exporter)
        self.assertEqual(processor.processing_history, [])
    
    def testDataValidatorInitialization(self):
        """Test DataValidator initialization"""
        validator = DataValidator()
        
        self.assertEqual(validator.validation_rules, {})
    
    def testDataValidatorAddRule(self):
        """Test adding validation rules"""
        validator = DataValidator()
        
        validator.add_rule("neuron_id", "required")
        validator.add_rule("region_id", "numeric")
        
        self.assertIn("neuron_id", validator.validation_rules)
        self.assertIn("region_id", validator.validation_rules)
        self.assertEqual(validator.validation_rules["neuron_id"]["type"], "required")
        self.assertEqual(validator.validation_rules["region_id"]["type"], "numeric")
    
    def testDataValidatorValidateNeuronData(self):
        """Test validating neuron data"""
        validator = DataValidator()
        
        # Create valid neuron data
        neuron_data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Excitatory', 'Inhibitory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        result = validator.validate_neuron_data(neuron_data)
        
        self.assertTrue(result['is_valid'])
        self.assertEqual(len(result['errors']), 0)
        self.assertEqual(len(result['warnings']), 0)
        self.assertEqual(len(result['suggestions']), 0)
    
    def testDataValidatorValidateNeuronDataMissingColumns(self):
        """Test validation with missing required columns"""
        validator = DataValidator()
        
        # Create neuron data with missing columns
        neuron_data = pd.DataFrame({
            'neuron_id': [1, 2, 3],
            'region_id': [1, 1, 2],
            # Missing neuron_type, membrane_potential, firing_state
        })
        
        result = validator.validate_neuron_data(neuron_data)
        
        self.assertFalse(result['is_valid'])
        self.assertGreater(len(result['errors']), 0)
        self.assertIn("Missing required columns", result['errors'][0])
    
    def testDataValidatorValidateNeuronDataInvalidFiringStates(self):
        """Test validation with invalid firing states"""
        validator = DataValidator()
        
        # Create neuron data with invalid firing states
        neuron_data = pd.DataFrame({
            'neuron_id': [1, 2, 3],
            'region_id': [1, 1, 2],
            'neuron_type': ['Excitatory'] * 3,
            'membrane_potential': [-70.0] * 3,
            'firing_state': [0, 1, 99]  # 99 is invalid
        })
        
        result = validator.validate_neuron_data(neuron_data)
        
        self.assertFalse(result['is_valid'])
        self.assertGreater(len(result['errors']), 0)
        self.assertIn("Invalid firing states", result['errors'][0])
    
    def testDataValidatorValidateTimeSeriesData(self):
        """Test validating time series data"""
        validator = DataValidator()
        
        # Create valid time series data
        time_series_data = pd.DataFrame({
            'step': [1, 2, 3, 4, 5],
            'value': [10.0, 12.0, 15.0, 13.0, 18.0],
            'time': [0.0, 0.1, 0.2, 0.3, 0.4]
        })
        
        result = validator.validate_time_series_data(time_series_data)
        
        self.assertTrue(result['is_valid'])
        self.assertEqual(len(result['errors']), 0)
    
    def testDataValidatorValidateSpatialData(self):
        """Test validating spatial data"""
        validator = DataValidator()
        
        # Create valid spatial data
        spatial_data = pd.DataFrame({
            'x': [0.0, 1.0, 2.0, 3.0],
            'y': [0.0, 1.0, 2.0, 3.0],
            'z': [0.0, 0.0, 0.0, 0.0]
        })
        
        result = validator.validate_spatial_data(spatial_data)
        
        self.assertTrue(result['is_valid'])
        self.assertEqual(len(result['warnings']), 0)
    
    def testDataNormalizerInitialization(self):
        """Test DataNormalizer initialization"""
        normalizer = DataNormalizer()
        
        self.assertEqual(normalizer.scalers, {})
        self.assertIsNone(normalizer.pca)
        self.assertFalse(normalizer.is_fitted)
    
    def testDataNormalizerFitTransform(self):
        """Test fitting and transforming data"""
        normalizer = DataNormalizer()
        
        # Create sample data
        data = pd.DataFrame({
            'value1': [1.0, 2.0, 3.0, 4.0, 5.0],
            'value2': [10.0, 20.0, 30.0, 40.0, 50.0],
            'category': ['A', 'B', 'A', 'B', 'A']
        })
        
        # Fit and transform
        result = normalizer.fit_transform(data)
        
        self.assertIsInstance(result, pd.DataFrame)
        self.assertEqual(len(result), len(data))
        self.assertTrue(normalizer.is_fitted)
        self.assertGreater(len(normalizer.scalers), 0)
        
        # Test transforming again without refitting
        result2 = normalizer.transform_data(data)
        self.assertIsInstance(result2, pd.DataFrame)
    
    def testDataNormalizerFitPCA(self):
        """Test fitting PCA for dimensionality reduction"""
        normalizer = DataNormalizer()
        
        # Create sample data
        data = pd.DataFrame({
            'feature1': [1.0, 2.0, 3.0, 4.0, 5.0],
            'feature2': [10.0, 20.0, 30.0, 40.0, 50.0],
            'feature3': [100.0, 200.0, 300.0, 400.0, 500.0]
        })
        
        # Fit PCA
        pca = normalizer.fit_pca(data, n_components=2)
        
        self.assertIsNotNone(normalizer.pca)
        self.assertIsInstance(pca, Mock)  # Mock PCA object
        self.assertTrue(normalizer.is_fitted)
        
        # Test transforming with PCA
        pca_result = normalizer.transform_pca(data)
        self.assertIsInstance(pca_result, pd.DataFrame)
        self.assertIn('pc1', pca_result.columns)
        self.assertIn('pc2', pca_result.columns)
    
    def testDataAggregatorAggregateByRegion(self):
        """Test aggregating neuron data by region"""
        aggregator = DataAggregator()
        
        # Create sample neuron data
        neuron_data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5, 6],
            'region_id': [1, 1, 1, 2, 2, 2],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Excitatory', 'Inhibitory', 'Excitatory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0, -69.0],
            'firing_state': [0, 1, 0, 1, 0, 1]
        })
        
        result = aggregator.aggregate_by_region(neuron_data)
        
        self.assertIsInstance(result, pd.DataFrame)
        self.assertEqual(len(result), 2)  # Two regions
        self.assertIn('region_id', result.columns)
        self.assertIn('neuron_id_count', result.columns)
        self.assertIn('membrane_potential_mean', result.columns)
        self.assertIn('neuron_type_Excitatory_percentage', result.columns)
    
    def testDataAggregatorAggregateByTimeStep(self):
        """Test aggregating time series data by time step"""
        aggregator = DataAggregator()
        
        # Create sample time series data
        time_series_data = pd.DataFrame({
            'step': [1, 1, 1, 2, 2, 2, 3, 3, 3],
            'value': [10.0, 12.0, 14.0, 20.0, 22.0, 24.0, 30.0, 32.0, 34.0]
        })
        
        result = aggregator.aggregate_by_time_step(time_series_data)
        
        self.assertIsInstance(result, pd.DataFrame)
        self.assertEqual(len(result), 3)  # Three time steps
        self.assertIn('step', result.columns)
        self.assertIn('value_mean', result.columns)
        self.assertIn('value_std', result.columns)
    
    def testDataExporterToCSV(self):
        """Test exporting data to CSV"""
        exporter = DataExporter()
        
        # Create sample data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3],
            'region_id': [1, 2, 3],
            'membrane_potential': [-70.0, -65.0, -72.0]
        })
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False) as f:
            csv_file = f.name
        
        try:
            exporter.to_csv(data, csv_file)
            
            # Verify file was created and contains data
            self.assertTrue(os.path.exists(csv_file))
            
            # Read back and verify
            imported_data = pd.read_csv(csv_file)
            self.assertEqual(len(imported_data), len(data))
            self.assertEqual(imported_data['neuron_id'].tolist(), data['neuron_id'].tolist())
            
        finally:
            if os.path.exists(csv_file):
                os.unlink(csv_file)
    
    def testDataExporterToJSON(self):
        """Test exporting data to JSON"""
        exporter = DataExporter()
        
        # Create sample data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3],
            'region_id': [1, 2, 3],
            'membrane_potential': [-70.0, -65.0, -72.0]
        })
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json_file = f.name
        
        try:
            exporter.to_json(data, json_file)
            
            # Verify file was created
            self.assertTrue(os.path.exists(json_file))
            
            # Read back and verify
            with open(json_file, 'r') as f:
                imported_data = json.load(f)
            
            self.assertEqual(len(imported_data), len(data))
            
        finally:
            if os.path.exists(json_file):
                os.unlink(json_file)
    
    def testDataExporterExportSummary(self):
        """Test exporting data summary"""
        exporter = DataExporter()
        
        # Create sample data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            summary_file = f.name
        
        try:
            exporter.export_summary(data, summary_file)
            
            # Verify file was created
            self.assertTrue(os.path.exists(summary_file))
            
            # Read back and verify summary
            with open(summary_file, 'r') as f:
                summary = json.load(f)
            
            self.assertIn('shape', summary)
            self.assertIn('columns', summary)
            self.assertIn('dtypes', summary)
            self.assertIn('missing_values', summary)
            self.assertIn('describe', summary)
            self.assertIn('memory_usage', summary)
            
            self.assertEqual(summary['shape'], [5, 4])
            
        finally:
            if os.path.exists(summary_file):
                os.unlink(summary_file)
    
    def testDataProcessorValidateData(self):
        """Test overall data validation"""
        # Create sample neuron data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3],
            'region_id': [1, 2, 3],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0],
            'firing_state': [0, 1, 0]
        })
        
        result = self.processor.validate_data(data, DataType.NEURON_DATA)
        
        self.assertIn('is_valid', result)
        self.assertIn('errors', result)
        self.assertIn('warnings', result)
        self.assertIn('suggestions', result)
    
    def testDataProcessorCleanData(self):
        """Test data cleaning"""
        # Create sample data with issues
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Excitatory', 'Inhibitory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        # Add a duplicate row
        duplicate_row = data.iloc[0:1].copy()
        data_with_duplicates = pd.concat([data, duplicate_row], ignore_index=True)
        
        # Clean data
        cleaned_data = self.processor.clean_data(data_with_duplicates, DataType.NEURON_DATA, 'auto')
        
        self.assertIsInstance(cleaned_data, pd.DataFrame)
        self.assertEqual(len(cleaned_data), 5)  # Duplicates should be removed
        self.assertEqual(len(cleaned_data['neuron_id'].unique()), 5)  # All neuron IDs should be unique
    
    def testDataProcessorNormalizeData(self):
        """Test data normalization"""
        # Create sample data
        data = pd.DataFrame({
            'value1': [10.0, 20.0, 30.0, 40.0, 50.0],
            'value2': [100.0, 200.0, 300.0, 400.0, 500.0],
            'category': ['A', 'B', 'A', 'B', 'A']
        })
        
        # Normalize data
        normalized_data = self.processor.normalize_data(data)
        
        self.assertIsInstance(normalized_data, pd.DataFrame)
        self.assertEqual(len(normalized_data), len(data))
        
        # Check that numeric values are normalized (should be between 0 and 1 for firing rates)
        # For other values, it might be standardized
        self.assertTrue(normalized_data['value1'].min() >= -10)  # Should not be extreme
        self.assertTrue(normalized_data['value1'].max() <= 10)
    
    def testDataProcessorProcessData(self):
        """Test overall data processing pipeline"""
        # Create sample data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Excitatory', 'Inhibitory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        # Process data with validation, cleaning, and normalization
        result = self.processor.process_data(
            data, 
            DataType.NEURON_DATA, 
            steps=['validate', 'clean', 'normalize', 'analysis']
        )
        
        self.assertIsInstance(result, type(self.processor).__dict__.get('ProcessingResult', Mock))
        self.assertIn('success', result.__dict__ if hasattr(result, '__dict__') else {})
        self.assertIn('original_data', result.__dict__ if hasattr(result, '__dict__') else {})
        self.assertIn('processed_data', result.__dict__ if hasattr(result, '__dict__') else {})
        self.assertIn('transformations', result.__dict__ if hasattr(result, '__dict__') else {})
        self.assertIn('errors', result.__dict__ if hasattr(result, '__dict__') else {})
    
    def testQuickProcessFunction(self):
        """Test the convenience quick_process function"""
        from python.data.DataProcessor import quick_process
        
        # Create sample data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Excitatory', 'Inhibitory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        # Test quick processing
        result = quick_process(data, target_file=None)
        
        self.assertIsInstance(result, type(self.processor).__dict__.get('ProcessingResult', Mock))
        self.assertIn('success', result.__dict__ if hasattr(result, '__dict__') else {})
        self.assertIn('original_data', result.__dict__ if hasattr(result, '__dict__') else {})
        self.assertIn('processed_data', result.__dict__ if hasattr(result, '__dict__') else {})
    
    def testAnalyzeNeuronPatternsFunction(self):
        """Test the convenience analyze_neuron_patterns function"""
        from python.data.DataProcessor import analyze_neuron_patterns
        
        # Create sample data
        data = pd.DataFrame({
            'neuron_id': [1, 2, 3, 4, 5],
            'region_id': [1, 1, 2, 2, 3],
            'neuron_type': ['Excitatory', 'Inhibitory', 'Excitatory', 'Inhibitory', 'Internal'],
            'membrane_potential': [-70.0, -65.0, -72.0, -68.0, -71.0],
            'firing_state': [0, 1, 0, 1, 0]
        })
        
        # Test analysis
        result = analyze_neuron_patterns(data)
        
        self.assertIsInstance(result, dict)
        self.assertIn('success', result)
        self.assertIn('metadata', result)
        self.assertIn('errors', result)

if __name__ == '__main__':
    unittest.main()
