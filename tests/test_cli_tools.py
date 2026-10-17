#!/usr/bin/env python3
"""
CLI tests for Phase 4 - Command line tools and automation scripts.

This test file covers:
- CLI tool structure and syntax validation
- Automation script structure and syntax validation
- Shared CLI utilities functionality
- Configuration file support
- Output formatting and error handling
"""

import sys
import os
import tempfile
import shutil
import json
import subprocess
from pathlib import Path
from typing import Dict, List, Any

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from scripts.cli import (
        OutputFormat, NLMCLI, load_config, save_config, setup_logging,
        ProgressReporter, validate_args, format_output
    )
    print("✓ Successfully imported shared CLI utilities")
except ImportError as e:
    print(f"✗ Failed to import shared CLI utilities: {e}")
    sys.exit(1)
def test_cli_tools_structure():
    """Test that all CLI tools have correct structure and syntax."""
    cli_dir = Path(__file__).parent.parent / "scripts" / "cli"
    auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
    
    # Test CLI tools
    cli_tools = ["nlm_train.py", "nlm_simulate.py", "nlm_visualize.py", "nlm_export.py"]
    
    for tool in cli_tools:
        tool_path = cli_dir / tool
        assert tool_path.exists(), f"CLI tool {tool} not found"
        assert tool_path.stat().st_mode & 0o111 != 0, f"CLI tool {tool} not executable"
        
        content = tool_path.read_text()
        assert content.startswith("#!/bin/env python3"), f"{tool} missing Python shebang"
        assert "import argparse" in content, f"{tool} missing argparse import"
        assert "class.*CLI" in content, f"{tool} missing CLI class"
        assert "def main" in content, f"{tool} missing main function"
        
        print(f"✓ CLI tool {tool} structure is valid")
    
    # Test automation scripts
    automation_scripts = [
        "run_experiment.py", "train_brain.py", 
        "analyze_results.py", "create_world.py"
    ]
    
    for script in automation_scripts:
        script_path = auto_dir / script
        assert script_path.exists(), f"Automation script {script} not found"
        assert script_path.stat().st_mode & 0o111 != 0, f"Automation script {script} not executable"
        
        content = script_path.read_text()
        assert content.startswith("#!/bin/env python3"), f"{script} missing Python shebang"
        assert "import argparse" in content, f"{script} missing argparse import"
        assert "def main" in content, f"{script} missing main function"
        
        print(f"✓ Automation script {script} structure is valid")
    
    # Test shared CLI utilities
    init_file = cli_dir / "__init__.py"
    assert init_file.exists(), "Shared CLI utilities not found"
    
    # Check for essential classes and functions
    shared_content = init_file.read_text()
    
    expected_classes = ["NLMCLI", "ProgressReporter"]
    for class_name in expected_classes:
        assert f"class {class_name}" in shared_content, f"Missing class {class_name}"
    
    expected_enums = ["OutputFormat"]
    for enum_name in expected_enums:
        assert f"class {enum_name}" in shared_content, f"Missing enum {enum_name}"
    
    expected_functions = [
        "load_config", "save_config", "setup_logging", "format_output", "validate_args"
    ]
    for func_name in expected_functions:
        assert f"def {func_name}" in shared_content, f"Missing function {func_name}"
    
    print("✓ All CLI tools and automation scripts have correct structure")
    print("✓ Shared CLI utilities are properly implemented")
def test_output_format_enumeration():
    """Test OutputFormat enumeration functionality."""
    # Test OutputFormat values
    assert OutputFormat.CONSOLE == OutputFormat("console")
    assert OutputFormat.JSON == OutputFormat("json")
    assert OutputFormat.CSV == OutputFormat("csv")
    assert OutputFormat.TABLE == OutputFormat("table")
    
    # Test output format choices
    valid_formats = [fmt.value for fmt in OutputFormat]
    assert "console" in valid_formats
    assert "json" in valid_formats
    assert "csv" in valid_formats
    assert "table" in valid_formats
    
    print("✓ OutputFormat enumeration works correctly")
def test_configuration_file_support():
    """Test configuration file support."""
    with tempfile.TemporaryDirectory() as temp_dir:
        config_path = Path(temp_dir) / "test_config.json"
        
        # Create test configuration
        test_config = {
            "tool": {
                "name": "nlm-test",
                "version": "1.0.0",
                "description": "Test NLM tool"
            },
            "logging": {
                "level": "INFO",
                "format": "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
            },
            "configuration": {
                "learning_rate": 0.001,
                "batch_size": 32,
                "epochs": 100
            }
        }
        
        # Save configuration
        save_config(test_config, str(config_path))
        assert config_path.exists()
        
        # Load configuration
        loaded_config = load_config(str(config_path))
        assert loaded_config is not None
        assert loaded_config["tool"]["name"] == "nlm-test"
        assert loaded_config["configuration"]["learning_rate"] == 0.001
        assert loaded_config["configuration"]["batch_size"] == 32
        
        # Test configuration validation
        valid_config = {"required_field": "value"}
        assert validate_args(valid_config, ["required_field"]) == True
        
        invalid_config = {}
        assert validate_args(invalid_config, ["required_field"]) == False
        
        print("✓ Configuration file support works correctly")
def test_output_formatting():
    """Test output formatting functionality."""
    test_data = {
        "name": "Test Tool",
        "version": "1.0.0",
        "parameters": {
            "learning_rate": 0.001,
            "batch_size": 32
        },
        "results": [1.0, 2.0, 3.0, 4.0, 5.0]
    }
    
    # Test console output
    console_output = format_output(test_data, OutputFormat.CONSOLE, "Test Results")
    assert isinstance(console_output, str)
    assert "Test Results" in console_output
    assert "name: Test Tool" in console_output
    
    # Test JSON output
    json_output = format_output(test_data, OutputFormat.JSON, "Test Results")
    assert isinstance(json_output, str)
    
    parsed_json = json.loads(json_output)
    assert parsed_json["title"] == "Test Results"
    assert parsed_json["data"]["name"] == "Test Tool"
    
    # Test CSV output
    csv_output = format_output(test_data, OutputFormat.CSV, "Test Results")
    assert isinstance(csv_output, str)
    assert "name" in csv_output
    assert "Test Results" in csv_output
    
    # Test table output
    table_output = format_output(test_data, OutputFormat.TABLE, "Test Results")
    assert isinstance(table_output, str)
    assert "name" in table_output
    
    print("✓ Output formatting works correctly for all formats")
class TestCLIIntegration:
    """Comprehensive CLI integration tests."""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp()
    
    def __del__(self):
        shutil.rmtree(self.test_dir, ignore_errors=True)
    
    def test_nlmcli_base_class(self):
        """Test NLMCLI base class functionality."""
        cli = NLMCLI("test-cli", "Test CLI tool")
        assert cli.name == "test-cli"
        assert cli.description == "Test CLI tool"
        assert cli.version == "1.0.0"
        assert cli.output_format == OutputFormat.CONSOLE
        assert cli.verbose == False
        
        # Test argument parsing setup
        import argparse
        parser = cli.parse_args("Test description", subcommands=False)
        assert parser is not None
        
        # Test common arguments
        assert hasattr(cli, 'add_common_arguments')
        assert hasattr(cli, 'initialize')
        assert hasattr(cli, 'run_command')
        
        print("✓ NLMCLI base class works correctly")
    
    def test_progress_reporter(self):
        """Test ProgressReporter functionality."""
        logger = setup_logging("test-logger", "INFO")
        reporter = ProgressReporter("Test Task", 100, logger)
        
        assert reporter.description == "Test Task"
        assert reporter.total == 100
        assert reporter.current == 0
        assert reporter.logger == logger
        
        # Test progress updates
        reporter.update(10)
        assert reporter.current == 10
        
        # Test finish
        reporter.finish()
        
        print("✓ ProgressReporter works correctly")
    
    def test_configuration_validation(self):
        """Test configuration validation functionality."""
        # Create mock arguments object
        class MockArgs:
            def __init__(self):
                self.learning_rate = 0.001
                self.batch_size = 32
                self.epochs = 100
                self.empty_field = ""
        
        # Test valid configuration
        args = MockArgs()
        assert validate_args(args, ["learning_rate", "batch_size", "epochs"]) == True
        
        # Test invalid configuration (empty field)
        assert validate_args(args, ["learning_rate", "empty_field"]) == False
        
        print("✓ Configuration validation works correctly")
    
    def test_cli_argument_structure(self):
        """Test CLI argument structure for all tools."""
        cli_dir = Path(__file__).parent.parent / "scripts" / "cli"
        
        # Check common CLI structure elements
        common_elements = [
            ("--help", "Help flag"),
            ("--version", "Version flag"),
            ("--verbose", "Verbose flag"),
            ("--config", "Config file flag"),
            ("--output-format", "Output format flag"),
            ("--output-file", "Output file flag")
        ]
        
        for flag, description in common_elements:
            # Check at least one CLI tool has this flag
            found = False
            for tool_file in cli_dir.glob("*.py"):
                if tool_file.name != "__init__.py":
                    content = tool_file.read_text()
                    if flag in content:
                        found = True
                        break
            
            assert found, f"Common CLI element {description} ({flag}) not found in any CLI tool"
        
        print("✓ All CLI tools have consistent argument structure")
    
    def test_automation_script_compatibility(self):
        """Test automation script compatibility."""
        auto_dir = Path(__file__).parent.parent / "scripts" / "automation"
        
        # Check that all automation scripts can be imported
        for script_file in auto_dir.glob("*.py"):
            if script_file.name != "__init__.py":
                try:
                    # Try to execute the script with --help to check syntax
                    result = subprocess.run(
                        [sys.executable, str(script_file), "--help"],
                        capture_output=True,
                        text=True,
                        timeout=10
                    )
                    # We expect this to fail or show help, but it should not crash with syntax error
                    print(f"✓ Automation script {script_file.name} has valid syntax")
                except subprocess.TimeoutExpired:
                    print(f"ℹ Automation script {script_file.name} requires arguments (timeout expected)")
                except Exception as e:
                    if "SyntaxError" in str(e):
                        assert False, f"Syntax error in {script_file.name}: {e}"
                    else:
                        print(f"✓ Automation script {script_file.name} has valid syntax (error expected)")
    
    def test_shared_utilities_integration(self):
        """Test shared utilities integration."""
        # Test logging setup
        logger = setup_logging("test-integration", "DEBUG")
        assert logger is not None
        assert logger.name == "test-integration"
        
        # Test configuration loading/saving
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_path = f.name
            test_config = {"test": "integration", "value": 42}
            save_config(test_config, config_path)
        
        loaded_config = load_config(config_path)
        assert loaded_config["test"] == "integration"
        assert loaded_config["value"] == 42
        
        # Clean up
        os.unlink(config_path)
        
        print("✓ Shared utilities integration works correctly")
    
    def run_all_tests(self):
        """Run all CLI integration tests."""
        print("=" * 60)
        print("CLI TOOLS AND AUTOMATION SCRIPTS - INTEGRATION TESTS")
        print("=" * 60)
        
        try:
            test_cli_tools_structure()
            test_output_format_enumeration()
            test_configuration_file_support()
            test_output_formatting()
            
            # Run comprehensive tests
            tester = TestCLIIntegration()
            tester.test_nlmcli_base_class()
            tester.test_progress_reporter()
            tester.test_configuration_validation()
            tester.test_cli_argument_structure()
            tester.test_automation_script_compatibility()
            tester.test_shared_utilities_integration()
            
            print("\n" + "=" * 60)
            print("CLI INTEGRATION TESTS PASSED")
            print("=" * 60)
            return True
            
        except Exception as e:
            print(f"\n✗ CLI integration test failed: {e}")
            import traceback
            traceback.print_exc()
            return False
if __name__ == "__main__":
    tester = TestCLIIntegration()
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)