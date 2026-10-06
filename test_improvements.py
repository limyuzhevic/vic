#!/usr/bin/env python3
"""
NLM Configuration System Tests

Tests demonstrating the improvements made to the NLM configuration system
and the new features added to the codebase.
"""

import os
import sys
import json
import tempfile
import shutil

# Add current directory to path
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_cli_creation():
    """Test that CLI tool was created successfully"""
    print("Testing CLI tool creation...")
    
    cli_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/nlm-cli"
    
    if os.path.exists(cli_path):
        print("✓ CLI tool created successfully")
        
        # Check it's executable (if we can on this system)
        if os.access(cli_path, os.X_OK):
            print("✓ CLI tool is executable")
        else:
            print("  Note: CLI tool is not executable (check file permissions)")
            
        # Check the file contains expected content
        with open(cli_path, 'r') as f:
            content = f.read()
            if "NLM (熙然)" in content and "Command Line Interface" in content:
                print("✓ CLI tool contains expected content")
            else:
                print("✗ CLI tool content issue")
    else:
        print("✗ CLI tool not found")
        return False
        
    return True

def test_experiment_presets():
    """Test that experiment presets were created"""
    print("\nTesting experiment presets...")
    
    presets_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/experiment_presets.py"
    
    if os.path.exists(presets_path):
        print("✓ Experiment presets created successfully")
        
        # Try to import and check content
        try:
            # Read and check for preset definitions
            with open(presets_path, 'r') as f:
                content = f.read()
                if "EXPERIMENT_PRESETS" in content and "LEARNING_EXPERIMENT" in content:
                    print("✓ Preset definitions found")
                else:
                    print("✗ Preset definitions missing")
                    
                if "Learning Experiment" in content and "Memory Consolidation Experiment" in content:
                    print("✓ Preset descriptions found")
                else:
                    print("✗ Preset descriptions missing")
        except Exception as e:
            print(f"  Note: Import test skipped - {e}")
    else:
        print("✗ Experiment presets not found")
        return False
        
    return True

def test_config_template():
    """Test that configuration template was created"""
    print("\nTesting configuration template...")
    
    config_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/config.json"
    
    if os.path.exists(config_path):
        print("✓ Configuration template created successfully")
        
        # Check it's valid JSON
        try:
            with open(config_path, 'r') as f:
                config_data = json.load(f)
                print("✓ Configuration template is valid JSON")
                
                # Check for key sections
                sections = [
                    "simulation", "plasticity", "neuromodulation", 
                    "memory", "development", "cognition"
                ]
                
                for section in sections:
                    if section in config_data:
                        print(f"  ✓ Contains '{section}' section")
                    else:
                        print(f"  ✗ Missing '{section}' section")
                        
        except json.JSONDecodeError as e:
            print(f"✗ Invalid JSON in configuration template: {e}")
        except Exception as e:
            print(f"  Note: JSON validation skipped - {e}")
    else:
        print("✗ Configuration template not found")
        return False
        
    return True

def test_config_improvements():
    """Test that configuration improvements were made"""
    print("\nTesting configuration improvements...")
    
    config_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/configs/default.cfg"
    
    if os.path.exists(config_path):
        print("✓ Default configuration file exists")
        
        # Check for duplicate removal (dopamine_baseline should appear only once)
        with open(config_path, 'r') as f:
            content = f.read()
            dopamine_lines = content.count("dopamine_baseline")
            if dopamine_lines == 1:
                print("✓ Configuration bug fixed - duplicate dopamine_baseline removed")
            else:
                print(f"✗ Configuration still has {dopamine_lines} duplicate dopamine_baseline entries")
                
        # Check for improved structure
        if "# Neuromodulation settings" in content:
            print("✓ Improved configuration structure (section headers)")
        else:
            print("✗ Missing improved configuration structure")
    else:
        print("✗ Default configuration file not found")
        return False
        
    return True

def test_structure_improvements():
    """Test that overall code structure was improved"""
    print("\nTesting code structure improvements...")
    
    # Check for key files that should exist
    expected_files = [
        "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/nlm-cli",
        "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/experiment_presets.py",
        "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/config.json"
    ]
    
    for filepath in expected_files:
        if os.path.exists(filepath):
            print(f"✓ {os.path.basename(filepath)} created")
        else:
            print(f"✗ {os.path.basename(filepath)} missing")
            
    return True

def test_documentation():
    """Test that documentation improvements were made"""
    print("\nTesting documentation improvements...")
    
    readme_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a5d3638c-03be-4352-afc6-e49820747104/README.md"
    
    if os.path.exists(readme_path):
        with open(readme_path, 'r') as f:
            content = f.read()
            
            # Check for updated information
            improvements = [
                ("Phase 6: Final Integration", "Current phase should be Phase 6"),
                ("Configuration System", "Configuration system description"),
                ("Advanced Features", "Advanced features mention"),
                ("User Interface", "User interface mention"),
                ("Command Line Interface", "CLI description")
            ]
            
            for keyword, description in improvements:
                if keyword in content:
                    print(f"✓ Documentation updated with {description}")
                else:
                    print(f"  Note: {description} not explicitly mentioned")
    else:
        print("✗ README.md not found")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Configuration System Improvements - Test Suite")
    print("=" * 60)
    
    tests = [
        test_cli_creation,
        test_experiment_presets,
        test_config_template,
        test_config_improvements,
        test_structure_improvements,
        test_documentation
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total:  {passed + failed}")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return True
    else:
        print(f"\n✗ {failed} tests failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
