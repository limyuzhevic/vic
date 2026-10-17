#!/usr/bin/env python3
"""
Automation script tests for Phase 4 - Command line tools and automation scripts.

This test file focuses specifically on automation script functionality:
- run_experiment.py automation workflow tests
- train_brain.py training automation tests
- analyze_results.py results analysis automation tests
- create_world.py world creation automation tests
"""

import sys
import os
import tempfile
import shutil
import json
import time
from pathlib import Path
from typing import Dict, List, Any

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

def test_automation_script_structure():
    """Test automation script structure and common patterns."""
    auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
    
    # Test all automation scripts
    automation_scripts = [
        "run_experiment.py", "train_brain.py",
        "analyze_results.py", "create_world.py"
    ]
    
    for script_name in automation_scripts:
        script_path = auto_dir / script_name
        assert script_path.exists(), f"Automation script {script_name} not found"
        
        content = script_path.read_text()
        
        # Check for common automation script patterns
        assert "#!/bin/env python3" in content, f"{script_name} missing Python shebang"
        assert "import argparse" in content, f"{script_name} missing argparse"
        assert "import json" in content, f"{script_name} missing json import"
        assert "import logging" in content, f"{script_name} missing logging import"
        assert "def main(" in content, f"{script_name} missing main function"
        assert "if __name__ ==" in content, f"{script_name} missing main guard"
        
        # Check for automation-specific patterns
        if "run_experiment" in script_name:
            assert "experiment" in content.lower(), f"{script_name} missing experiment functionality"
        elif "train_brain" in script_name:
            assert "train" in content.lower(), f"{script_name} missing training functionality"
        elif "analyze_results" in script_name:
            assert "analyze" in content.lower(), f"{script_name} missing analysis functionality"
        elif "create_world" in script_name:
            assert "world" in content.lower(), f"{script_name} missing world functionality"
        
        print(f"✓ Automation script {script_name} has correct structure")
class TestAutomationScripts:
    """Comprehensive automation script tests."""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp()
    
    def __del__(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def test_run_experiment_script(self):
        """Test run_experiment.py automation functionality."""
        # Test script structure
        script_path = Path(__file__).parent.parent / "scripts" / "automation" / "run_experiment.py"
        content = script_path.read_text()
        
        # Check for run_experiment specific patterns
        assert "run_experiment" in content.lower(), "Missing run_experiment functionality"
        assert "experiment" in content.lower(), "Missing experiment management"
        assert "progress" in content.lower(), "Missing progress tracking"
        assert "configuration" in content.lower(), "Missing configuration support"
        
        # Check for common automation features
        automation_features = [
            ("def run(", "Main run function"),
            ("setup_logging", "Logging setup"),
            ("parse_args", "Argument parsing"),
            ("validate_args", "Argument validation"),
            ("format_output", "Output formatting"),
            ("ProgressReporter", "Progress reporting")
        ]
        
        for feature, description in automation_features:
            assert feature in content, f"Missing {description} in run_experiment.py"
        
        print("✓ run_experiment.py automation structure is correct")
    
    def test_train_brain_script(self):
        """Test train_brain.py automation functionality."""
        # Test script structure
        script_path = Path(__file__).parent.parent / "scripts" / "automation" / "train_brain.py"
        content = script_path.read_text()
        
        # Check for train_brain specific patterns
        assert "train_brain" in content.lower(), "Missing train_brain functionality"
        assert "training" in content.lower(), "Missing training functionality"
        assert "model" in content.lower(), "Missing model management"
        assert "checkpoint" in content.lower(), "Missing checkpoint management"
        
        # Check for automation-specific features
        automation_features = [
            ("def train(", "Main train function"),
            ("def continue(", "Continue training function"),
            ("def import(", "Import functionality"),
            ("setup_logging", "Logging setup"),
            ("ProgressReporter", "Progress reporting"),
            ("save_config", "Configuration management"),
            ("load_config", "Configuration loading")
        ]
        
        for feature, description in automation_features:
            assert feature in content, f"Missing {description} in train_brain.py"
        
        print("✓ train_brain.py automation structure is correct")
    
    def test_analyze_results_script(self):
        """Test analyze_results.py automation functionality."""
        # Test script structure
        script_path = Path(__file__).parent.parent / "scripts" / "automation" / "analyze_results.py"
        content = script_path.read_text()
        
        # Check for analyze_results specific patterns
        assert "analyze_results" in content.lower(), "Missing analyze_results functionality"
        assert "analyze" in content.lower(), "Missing analysis functionality"
        assert "results" in content.lower(), "Missing results management"
        assert "statistical" in content.lower(), "Missing statistical analysis"
        assert "visualization" in content.lower(), "Missing visualization"
        
        # Check for automation-specific features
        automation_features = [
            ("def analyze(", "Main analyze function"),
            ("def validate(", "Validation function"),
            ("setup_logging", "Logging setup"),
            ("ProgressReporter", "Progress reporting"),
            ("format_output", "Output formatting"),
            ("create_default_config", "Configuration creation")
        ]
        
        for feature, description in automation_features:
            assert feature in content, f"Missing {description} in analyze_results.py"
        
        print("✓ analyze_results.py automation structure is correct")
    
    def test_create_world_script(self):
        """Test create_world.py automation functionality."""
        # Test script structure
        script_path = Path(__file__).parent.parent / "scripts" / "automation" / "create_world.py"
        content = script_path.read_text()
        
        # Check for create_world specific patterns
        assert "create_world" in content.lower(), "Missing create_world functionality"
        assert "world" in content.lower(), "Missing world creation"
        assert "environment" in content.lower(), "Missing environment setup"
        assert "agent" in content.lower(), "Missing agent management"
        assert "validation" in content.lower(), "Missing validation"
        
        # Check for automation-specific features
        automation_features = [
            ("def create(", "Main create function"),
            ("def export(", "Export function"),
            ("def validate(", "Validation function"),
            ("setup_logging", "Logging setup"),
            ("ProgressReporter", "Progress reporting"),
            ("load_config", "Configuration loading"),
            ("save_config", "Configuration saving")
        ]
        
        for feature, description in automation_features:
            assert feature in content, f"Missing {description} in create_world.py"
        
        print("✓ create_world.py automation structure is correct")
    
    def test_common_automation_features(self):
        """Test common automation features across all scripts."""
        auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
        
        # Check for common automation features in all scripts
        common_features = [
            ("Error handling", lambda c: "try:" in c and "except" in c),
            ("Configuration support", lambda c: "load_config" in c or "config" in c.lower()),
            ("Logging setup", lambda c: "setup_logging" in c),
            ("Progress reporting", lambda c: "ProgressReporter" in c),
            ("Output formatting", lambda c: "format_output" in c),
            ("Argument parsing", lambda c: "argparse" in c),
            ("Help documentation", lambda c: "help" in c.lower()),
        ]
        
        for feature_name, feature_check in common_features:
            features_found = 0
            total_scripts = 0
            
            for script_file in auto_dir.glob("*.py"):
                if script_file.name != "__init__.py":
                    total_scripts += 1
                    content = script_file.read_text()
                    if feature_check(content):
                        features_found += 1
            
            # Require at least 75% of scripts to have each common feature
            if total_scripts > 0:
                feature_percentage = features_found / total_scripts
                assert feature_percentage >= 0.75, \
                    f"Only {features_found}/{total_scripts} scripts have {feature_name.lower()} support"
            
            print(f"✓ {feature_name} support: {features_found}/{total_scripts} scripts")
    
    def test_script_error_handling(self):
        """Test that all scripts have proper error handling."""
        auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
        
        error_handling_patterns = [
            "try:",
            "except",
            "finally:",
            "raise",
            "ValueError",
            "TypeError",
            "ImportError"
        ]
        
        for script_file in auto_dir.glob("*.py"):
            if script_file.name != "__init__.py":
                content = script_file.read_text()
                
                # Check for basic error handling
                has_error_handling = any(pattern in content for pattern in error_handling_patterns)
                assert has_error_handling, f"{script_file.name} missing error handling"
        
        print("✓ All automation scripts have proper error handling")
    
    def test_configuration_file_support(self):
        """Test configuration file support in automation scripts."""
        auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
        
        for script_file in auto_dir.glob("*.py"):
            if script_file.name != "__init__.py":
                content = script_file.read_text()
                
                # Check for configuration file support
                assert "--config" in content, f"{script_file.name} missing --config flag"
                assert "load_config" in content, f"{script_file.name} missing load_config function"
                
                # Check for JSON configuration support
                assert "json" in content.lower(), f"{script_file.name} missing JSON support"
        
        print("✓ All automation scripts support configuration files")
    
    def run_all_tests(self):
        """Run all automation script tests."""
        print("=" * 60)
        print("AUTOMATION SCRIPTS - STRUCTURE AND SYNTAX TESTS")
        print("=" * 60)
        
        try:
            test_automation_script_structure()
            
            # Run comprehensive tests
            tester = TestAutomationScripts()
            tester.test_run_experiment_script()
            tester.test_train_brain_script()
            tester.test_analyze_results_script()
            tester.test_create_world_script()
            tester.test_common_automation_features()
            tester.test_script_error_handling()
            tester.test_configuration_file_support()
            
            print("\n" + "=" * 60)
            print("AUTOMATION SCRIPT TESTS PASSED")
            print("=" * 60)
            return True
            
        except Exception as e:
            print(f"\n✗ Automation script test failed: {e}")
            import traceback
            traceback.print_exc()
            return False
if __name__ == "__main__":
    tester = TestAutomationScripts()
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)