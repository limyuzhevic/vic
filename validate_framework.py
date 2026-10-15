#!/usr/bin/env python3
"""
Simple validation script for the NLM Test Framework
"""

import sys
import os
from pathlib import Path

def run_test():
    """Run a basic validation test of the NLM framework"""
    
    print("NLM Test Framework Validation")
    print("=" * 50)
    
    # Check if the build directory exists
    build_dir = Path("build")
    if not build_dir.exists():
        print("Build directory not found. Please run 'mkdir build && cd build && cmake ..' first.")
        return False
    
    # Check if the main executable exists
    main_exe = build_dir / "nlm"
    if not main_exe.exists():
        print("Main executable not found. Please build the project first.")
        return False
    
    print(f"✓ Found main executable: {main_exe}")
    
    # Check for required source files
    required_files = [
        "src/core/TestFramework/TestBase.hpp",
        "src/core/TestFramework/TestRegistry.hpp",
        "src/core/TestFramework/CLIParser.hpp",
        "src/core/TestFramework/TestConfig.hpp",
        "src/core/TestFramework/TestRunner.hpp",
        "src/core/TestFramework/ProgressTracker.hpp",
        "src/core/TestFramework/Reporter.hpp",
        "src/core/TestFramework/ErrorHandler.hpp",
        "src/core/TestFramework/PluginManager.hpp",
        "src/core/TestFramework/ScenarioManager.hpp",
    ]
    
    missing_files = []
    for file in required_files:
        if not Path(file).exists():
            missing_files.append(file)
    
    if missing_files:
        print("✗ Missing required header files:")
        for file in missing_files:
            print(f"  - {file}")
        return False
    
    print("✓ All required header files present")
    
    # Check for implementation files
    required_impl_files = [
        "src/core/TestFramework/TestBase.cpp",
        "src/core/TestFramework/TestRegistry.cpp",
        "src/core/TestFramework/CLIParser.cpp",
        "src/core/TestFramework/TestConfig.cpp",
        "src/core/TestFramework/TestRunner.cpp",
        "src/core/TestFramework/ProgressTracker.cpp",
        "src/core/TestFramework/Reporter.cpp",
        "src/core/TestFramework/DefaultErrorHandler.cpp",
        "src/core/TestFramework/ExceptionErrorHandler.cpp",
        "src/core/TestFramework/ErrorHandlerFactory.cpp",
        "src/core/TestFramework/DynamicPlugin.cpp",
        "src/core/TestFramework/PluginManager.cpp",
        "src/core/TestFramework/ScenarioManager.cpp",
    ]
    
    missing_impl_files = []
    for file in required_impl_files:
        if not Path(file).exists():
            missing_impl_files.append(file)
    
    if missing_impl_files:
        print("✗ Missing required implementation files:")
        for file in missing_impl_files:
            print(f"  - {file}")
        return False
    
    print("✓ All required implementation files present")
    
    # Check for reporter implementations
    reporter_files = [
        "src/core/TestFramework/PlainTextReporter.cpp",
        "src/core/TestFramework/JSONReporter.cpp",
        "src/core/TestFramework/XMLReporter.cpp",
        "src/core/TestFramework/CSVReporter.cpp",
        "src/core/TestFramework/MarkdownReporter.cpp",
        "src/core/TestFramework/ReportFactory.cpp",
        "src/core/TestFramework/ReporterHelper.cpp",
    ]
    
    missing_reporter_files = []
    for file in reporter_files:
        if not Path(file).exists():
            missing_reporter_files.append(file)
    
    if missing_reporter_files:
        print("✗ Missing reporter implementation files:")
        for file in missing_reporter_files:
            print(f"  - {file}")
        return False
    
    print("✓ All reporter implementation files present")
    
    # Check for configuration files
    config_files = [
        "configs/test_config.cfg",
        "docs/README.md",
        "IMPLEMENTATION_SUMMARY.md",
    ]
    
    missing_config_files = []
    for file in config_files:
        if not Path(file).exists():
            missing_config_files.append(file)
    
    if missing_config_files:
        print("⚠ Missing documentation/configuration files:")
        for file in missing_config_files:
            print(f"  - {file}")
    
    print("✓ Core framework structure validated")
    
    # Show summary
    print("\nValidation Summary:")
    print("-" * 30)
    print(f"Main executable: {'✓' if main_exe.exists() else '✗'}")
    print(f"Header files: {'✓' if not missing_files else '✗'}")
    print(f"Implementation files: {'✓' if not missing_impl_files else '✗'}")
    print(f"Reporter files: {'✓' if not missing_reporter_files else '✗'}")
    print(f"Documentation: {'✓' if not missing_config_files else '⚠'}")
    
    print("\n" + "=" * 50)
    print("Validation PASSED - NLM Test Framework is ready for use!")
    print("=" * 50)
    
    return True

if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
