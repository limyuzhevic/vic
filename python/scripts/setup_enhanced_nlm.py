#!/usr/bin/env python3
"""
Enhanced NLM Setup and Verification Script

This script sets up and verifies the enhanced NLM Python bindings installation,
testing all the new functionality and improvements.

Usage:
    python setup_enhanced_nlm.py [--verify-only] [--test-only]
"""

import sys
import os
import subprocess
import json
import tempfile
from pathlib import Path
from typing import Dict, Any

# Add current directory to Python path
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

def log_info(message: str):
    """Log info message."""
    print(f"[INFO] {message}")

def log_success(message: str):
    """Log success message."""
    print(f"[SUCCESS] {message}")

def log_error(message: str):
    """Log error message."""
    print(f"[ERROR] {message}")

def log_warning(message: str):
    """Log warning message."""
    print(f"[WARNING] {message}")

class EnhancedNLMSetup:
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.python_bindings_dir = self.project_root / "python"
        self.scripts_dir = self.project_root / "python" / "scripts"
        self.examples_dir = self.project_root / "python" / "examples"
        self.docs_dir = self.project_root / "python" / "docs"
        
    def check_environment(self) -> bool:
        """Check if the environment is properly set up."""
        log_info("Checking environment setup...")
        
        # Check Python version
        python_version = sys.version_info
        if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
            log_error("Python 3.8+ is required")
            return False
        
        log_info(f"✓ Python version: {python_version.major}.{python_version.minor}.{python_version.micro}")
        
        # Check pynlm module
        try:
            import pynlm
            log_info(f"✓ pynlm module imported successfully")
            log_info(f"  Version: {getattr(pynlm, '__version__', 'unknown')}")
        except ImportError as e:
            log_error(f"Failed to import pynlm: {e}")
            return False
        
        # Check required modules
        required_modules = ['numpy', 'matplotlib', 'pandas', 'seaborn']
        for module in required_modules:
            try:
                __import__(module)
                log_info(f"✓ Module {module} available")
            except ImportError:
                log_warning(f"Module {module} not available - some features may be limited")
        
        return True
    
    def test_bindings_functionality(self) -> Dict[str, Any]:
        """Test core NLM bindings functionality."""
        log_info("Testing core bindings functionality...")
        
        results = {
            'basic_functionality': False,
            'enhanced_config': False,
            'error_handling': False,
            'helper_functions': False,
            'agent_simulation': False
        }
        
        try:
            import pynlm
            
            # Test basic functionality
            try:
                brain = pynlm.createBrain(pynlm.createDefaultConfig())
                brain.initialize()
                brain.step(0)
                
                stats = brain.getStats()
                if 'neuron_count' in stats and stats['neuron_count'] > 0:
                    results['basic_functionality'] = True
                    log_success("✓ Basic functionality working")
            except Exception as e:
                log_error(f"Basic functionality test failed: {e}")
            
            # Test enhanced configuration
            try:
                config = pynlm.create_simulation_config(500, 0.1, "default")
                if config.getInt("brain.neuron_count", 0) == 500:
                    results['enhanced_config'] = True
                    log_success("✓ Enhanced configuration working")
            except Exception as e:
                log_error(f"Enhanced configuration test failed: {e}")
            
            # Test error handling
            try:
                pynlm.validate_brain(None)
            except pynlm.ResourceError:
                results['error_handling'] = True
                log_success("✓ Error handling working")
            
            # Test helper functions
            try:
                from pynlm.helper_utils import SimulationOptions
                options = SimulationOptions()
                if options.num_steps > 0:
                    results['helper_functions'] = True
                    log_success("✓ Helper functions working")
            except Exception as e:
                log_error(f"Helper functions test failed: {e}")
            
            # Test agent simulation
            try:
                brain, world, agent = pynlm.create_agent_simulation(
                    steps=10,
                    enable_learning=True,
                    enable_development=True
                )
                if brain is not None and world is not None and agent is not None:
                    results['agent_simulation'] = True
                    log_success("✓ Agent simulation working")
            except Exception as e:
                log_error(f"Agent simulation test failed: {e}")
                
        except Exception as e:
            log_error(f"Core bindings test failed: {e}")
        
        return results
    
    def test_cli_tools(self) -> bool:
        """Test CLI tools availability."""
        log_info("Testing CLI tools...")
        
        # Check if CLI scripts exist
        cli_files = [
            self.scripts_dir / "nlm_cli.py",
            self.scripts_dir / "visualize.py"
        ]
        
        for file_path in cli_files:
            if file_path.exists():
                log_info(f"✓ CLI script found: {file_path.name}")
            else:
                log_error(f"CLI script not found: {file_path.name}")
                return False
        
        # Check if scripts are executable
        for file_path in cli_files:
            if file_path.exists() and os.access(file_path, os.X_OK):
                log_info(f"✓ CLI script is executable: {file_path.name}")
            else:
                log_warning(f"CLI script is not executable: {file_path.name}")
        
        return True
    
    def test_examples(self) -> Dict[str, bool]:
        """Test example scripts."""
        log_info("Testing example scripts...")
        
        results = {}
        example_files = list(self.examples_dir.glob("*.py"))
        
        for example_file in example_files:
            try:
                # Try to import the example module
                import importlib.util
                spec = importlib.util.spec_from_file_location(
                    example_file.stem, example_file
                )
                if spec is not None:
                    module = importlib.util.module_from_spec(spec)
                    spec.loader.exec_module(module)
                    results[example_file.stem] = True
                    log_success(f"✓ Example script loaded: {example_file.stem}")
                else:
                    results[example_file.stem] = False
                    log_error(f"✗ Failed to load example script: {example_file.stem}")
            except Exception as e:
                results[example_file.stem] = False
                log_error(f"✗ Example script {example_file.stem} failed: {e}")
        
        return results
    
    def generate_setup_report(self, test_results: Dict[str, Any], cli_status: bool,
                            example_results: Dict[str, bool]) -> str:
        """Generate a comprehensive setup report."""
        report = {
            "timestamp": str(os.pytime() if hasattr(os, 'pytime') else "unknown"),
            "python_version": f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}",
            "test_results": test_results,
            "cli_tools_available": cli_status,
            "example_scripts": example_results,
            "summary": "Setup completed",
            "status": "success" if all(test_results.values()) else "partial"
        }
        
        return json.dumps(report, indent=2)
    
    def save_report(self, report: str, filename: str = "nlm_enhanced_setup_report.json"):
        """Save setup report to file."""
        report_path = self.project_root / filename
        with open(report_path, 'w') as f:
            f.write(report)
        log_info(f"Setup report saved to: {report_path}")
    
    def run_verification(self) -> bool:
        """Run complete verification process."""
        log_info("Starting Enhanced NLM Setup Verification")
        log_info("=" * 60)
        
        # Step 1: Check environment
        if not self.check_environment():
            log_error("Environment check failed")
            return False
        
        # Step 2: Test bindings functionality
        test_results = self.test_bindings_functionality()
        
        # Step 3: Test CLI tools
        cli_status = self.test_cli_tools()
        
        # Step 4: Test examples
        example_results = self.test_examples()
        
        # Step 5: Generate and save report
        report = self.generate_setup_report(test_results, cli_status, example_results)
        self.save_report(report)
        
        # Print summary
        print("\n" + "=" * 60)
        print("SETUP VERIFICATION SUMMARY")
        print("=" * 60)
        
        print("\nCore Bindings Test Results:")
        for test, result in test_results.items():
            status = "✓ PASS" if result else "✗ FAIL"
            print(f"  {test}: {status}")
        
        print(f"\nCLI Tools: {'✓ PASS' if cli_status else '✗ FAIL'}")
        
        print("\nExample Scripts:")
        for example, result in example_results.items():
            status = "✓ PASS" if result else "✗ FAIL"
            print(f"  {example}: {status}")
        
        overall_status = all(test_results.values()) and cli_status and all(example_results.values())
        
        if overall_status:
            log_success("\n✓ All tests passed! Enhanced NLM setup is complete.")
            print("\nYou can now use the enhanced NLM Python bindings with:")
            print("  • pynlm - Main Python interface")
            print("  • python scripts/nlm_cli.py - Command line interface")
            print("  • python scripts/visualize.py - Visualization tools")
            print("  • python examples/complete_examples.py - Usage examples")
        else:
            log_warning("\n⚠ Some tests failed. Setup is partially complete.")
            print("\nSome features may not be available. Check the report for details.")
        
        return overall_status

    def run_test_mode(self):
        """Run tests only."""
        log_info("Running tests only mode")
        
        if self.check_environment():
            test_results = self.test_bindings_functionality()
            
            passed = sum(test_results.values())
            total = len(test_results)
            
            print(f"\nTest Results: {passed}/{total} passed")
            
            if passed == total:
                log_success("All tests passed!")
            else:
                log_warning(f"{total - passed} tests failed")
        else:
            log_error("Environment check failed, skipping tests")

def main():
    """Main entry point."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="Enhanced NLM Setup and Verification",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "--verify-only", action="store_true",
        help="Only verify setup, do not generate reports"
    )
    
    parser.add_argument(
        "--test-only", action="store_true",
        help="Only run tests, do not generate setup"
    )
    
    parser.add_argument(
        "--output", default="nlm_enhanced_setup_report.json",
        help="Output report filename"
    )
    
    args = parser.parse_args()
    
    setup = EnhancedNLMSetup()
    
    if args.test_only:
        setup.run_test_mode()
    elif args.verify_only:
        success = setup.run_verification()
        sys.exit(0 if success else 1)
    else:
        success = setup.run_verification()
        sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
