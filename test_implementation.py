#!/usr/bin/env python3
"""
Simple Test for Enhanced NLM Python Bindings

This script demonstrates that the enhanced NLM Python bindings are properly structured
and can be imported and used correctly.
"""

import sys
import os
import json
import pandas as pd
import numpy as np

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_import():
    """Test that all modules can be imported successfully."""
    print("Testing import of enhanced NLM Python bindings...")
    
    try:
        # Test core imports
        import pynlm
        print("  ✓ pynlm imported successfully")
        
        # Test research API
        from pynlm.research_api import (
            BrainManager, ExperimentManager, ResearchSession, PythonConfig,
            MatplotlibVisualizer, PlotlyVisualizer, PandasDataFrameExporter,
            StatisticalAnalysis, LearningExperimentAPI, IntegrationTestAPI,
            ParameterTuningAPI, AblationStudyAPI, CrossValidationAPI
        )
        print("  ✓ Research API imports successful")
        
        # Test viz API
        from pynlm.viz_api import VisualizationInterface, MatplotlibVisualizer as MViz, PlotlyVisualizer as PViz
        print("  ✓ Visualization API imports successful")
        
        # Test analysis API
        from pynlm.analysis_api import PandasDataFrameExporter as AExporter, StatisticalAnalysis as SAnalysis, ResearchDataProcessor
        print("  ✓ Analysis API imports successful")
        
        return True
        
    except ImportError as e:
        print(f"  ✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"  ✗ Unexpected error: {e}")
        return False
def test_basic_functionality():
    """Test basic functionality of the enhanced bindings."""
    print("\nTesting basic functionality...")
    
    try:
        # Test PythonConfig
        from pynlm.research_api import PythonConfig
        
        config = PythonConfig(
            learning_rate=0.01,
            neuron_count=1000,
            brain_type='standard'
        )
        
        print(f"  ✓ PythonConfig created: learning_rate={config.learning_rate}, neuron_count={config.neuron_count}")
        
        # Test BrainManager
        from pynlm.research_api import BrainManager
        
        brain_manager = BrainManager()
        print("  ✓ BrainManager created successfully")
        
        # Test ResearchSession
        from pynlm.research_api import ResearchSession
        
        with ResearchSession() as session:
            print("  ✓ ResearchSession context manager works")
            session.run_step(0)
            metrics = session.collect_metrics()
            print(f"  ✓ Metrics collected: {len(metrics)} metrics")
        
        # Test ExperimentManager
        from pynlm.research_api import ExperimentManager
        
        exp_manager = ExperimentManager()
        print("  ✓ ExperimentManager created successfully")
        
        # Test Visualization
        from pynlm.viz_api import create_matplotlib_visualizer, create_plotly_visualizer
        
        viz = create_matplotlib_visualizer()
        print("  ✓ MatplotlibVisualizer created")
        
        plotly_viz = create_plotly_visualizer()
        print("  ✓ PlotlyVisualizer created")
        
        # Test Analysis
        from pynlm.analysis_api import create_statistical_analysis, create_pandas_exporter
        
        # Create sample data
        sample_data = pd.DataFrame({
            'group1': np.random.normal(100, 15, 50),
            'group2': np.random.normal(105, 20, 50),
            'time': np.arange(50),
            'value': np.random.exponential(2, 50)
        })
        
        analysis = create_statistical_analysis(sample_data)
        print("  ✓ StatisticalAnalysis created")
        
        exporter = create_pandas_exporter()
        print("  ✓ PandasDataFrameExporter created")
        
        # Test Research Workflows
        from pynlm.research_api import LearningExperimentAPI, IntegrationTestAPI, ParameterTuningAPI, AblationStudyAPI, CrossValidationAPI
        
        learning_api = LearningExperimentAPI()
        integration_api = IntegrationTestAPI()
        param_tuning_api = ParameterTuningAPI()
        ablation_api = AblationStudyAPI()
        cv_api = CrossValidationAPI()
        
        print("  ✓ All research API classes created")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_file_structure():
    """Test that the file structure is correct."""
    print("\nTesting file structure...")
    
    # Check that all expected files exist
    expected_files = [
        'python/__init__.py',
        'python/research_api.py',
        'python/viz_api.py',
        'python/analysis_api.py',
        'python/bindings_enhanced.cpp',
        'python/README.md',
        'python/RESEARCH_API_GUIDE.md',
        'python/VISUALIZATION_API.md',
        'python/ANALYSIS_API.md',
        'python/EXAMPLES.md',
        'python/test_enhanced_bindings.py'
    ]
    
    missing_files = []
    for file_path in expected_files:
        full_path = os.path.join(os.path.dirname(__file__), file_path)
        if os.path.exists(full_path):
            print(f"  ✓ {file_path}")
        else:
            print(f"  ✗ {file_path} - MISSING")
            missing_files.append(file_path)
    
    if not missing_files:
        print("  ✓ All expected files are present")
        return True
    else:
        print(f"  ✗ Missing {len(missing_files)} files")
        return False
def main():
    """Run all tests."""
    print("=" * 70)
    print("Enhanced NLM Python Bindings - Verification Test")
    print("=" * 70)
    
    # Test import
    import_success = test_import()
    
    # Test basic functionality
    basic_success = test_basic_functionality()
    
    # Test file structure
    structure_success = test_file_structure()
    
    # Summary
    print("\n" + "=" * 70)
    print("Test Summary")
    print("=" * 70)
    
    if import_success:
        print("✓ Import Tests: PASSED")
    else:
        print("✗ Import Tests: FAILED")
    
    if basic_success:
        print("✓ Basic Functionality Tests: PASSED")
    else:
        print("✗ Basic Functionality Tests: FAILED")
    
    if structure_success:
        print("✓ File Structure Tests: PASSED")
    else:
        print("✗ File Structure Tests: FAILED")
    
    if import_success and basic_success and structure_success:
        print("\n✓ All tests passed!")
        print("\nThe enhanced NLM Python bindings have been successfully implemented.")
        print("\nKey features implemented:")
        print("  • High-level BrainManager class for brain lifecycle management")
        print("  • Pythonic PythonConfig class for configuration management")
        print("  • ResearchSession class with context manager support")
        print("  • ExperimentManager for research experiment control")
        print("  • MatplotlibVisualizer for static visualization")
        print("  • PlotlyVisualizer for interactive visualization")
        print("  • PandasDataFrameExporter for data export")
        print("  • StatisticalAnalysis for statistical analysis")
        print("  • Research workflow APIs (learning, integration, tuning, ablation, validation)")
        print("  • Comprehensive documentation and examples")
        return True
    else:
        print("\n✗ Some tests failed.")
        return False
if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)