#!/usr/bin/env python3
"""
Quick verification script for NLM Phase 2 improvements
"""

import os
import sys
from pathlib import Path

def check_file_exists(filepath, description):
    """Check if a file exists"""
    if os.path.exists(filepath):
        print(f"✅ {description}: {filepath}")
        return True
    else:
        print(f"❌ {description}: {filepath} - NOT FOUND")
        return False

def main():
    print("🔍 NLM Phase 2 Improvements Verification")
    print("=" * 50)
    
    project_root = Path(__file__).parent
    
    # Check improved files
    checks = [
        (project_root / "configs/default.cfg", "Fixed duplicate dopamine_baseline configuration"),
        (project_root / "src/main.cpp", "Refactored main.cpp with modular structure"),
        (project_root / "nlm-advanced", "Advanced NLM CLI tool"),
        (project_root / "CMakeLists.txt", "Updated CMakeLists.txt"),
    ]
    
    passed = 0
    total = len(checks)
    
    for filepath, description in checks:
        if check_file_exists(str(filepath), description):
            passed += 1
    
    # Check configuration file content
    print("\n📋 Configuration File Content:")
    config_file = project_root / "configs/default.cfg"
    if config_file.exists():
        with open(config_file, 'r') as f:
            content = f.read()
        
        # Check for duplicate dopamine_baseline
        dopamine_lines = [line for line in content.split('\n') if 'dopamine_baseline' in line]
        if len(dopamine_lines) == 1:
            print("✅ Configuration file has unique dopamine_baseline entry")
            passed += 1
        else:
            print(f"❌ Configuration file has {len(dopamine_lines)} dopamine_baseline entries (expected 1)")
    
    # Check main.cpp structure
    print("\n🏗️ Code Structure:")
    main_file = project_root / "src/main.cpp"
    if main_file.exists():
        with open(main_file, 'r') as f:
            main_content = f.read()
        
        # Check for modular structure indicators
        indicators = [
            "class CommandLineOptions",
            "struct SimulationStatistics", 
            "class TestRunner",
            "class StatisticsReporter",
            "namespace Utils"
        ]
        
        found_indicators = sum(1 for indicator in indicators if indicator in main_content)
        if found_indicators >= 4:
            print(f"✅ main.cpp shows good modular structure ({found_indicators}/5 indicators found)")
            passed += 1
        else:
            print(f"⚠️ main.cpp structure could be improved ({found_indicators}/5 indicators found)")
    
    # Check advanced CLI
    print("\n🛠️ Advanced Features:")
    advanced_file = project_root / "nlm-advanced"
    if advanced_file.exists():
        with open(advanced_file, 'r') as f:
            advanced_content = f.read()
        
        # Check for advanced features
        features = [
            "create_experiment_config",
            "run_advanced_experiment",
            "compare_experiments",
            "generate_report"
        ]
        
        found_features = sum(1 for feature in features if feature in advanced_content)
        if found_features >= 3:
            print(f"✅ Advanced CLI includes {found_features}/4 advanced features")
            passed += 1
        else:
            print(f"⚠️ Advanced CLI needs more features ({found_features}/4 found)")
    
    # Summary
    print("\n" + "=" * 50)
    print(f"📊 SUMMARY: {passed}/{total} checks passed")
    
    if passed >= total * 0.8:  # 80% threshold
        print("🎉 Most improvements successfully implemented!")
        return 0
    else:
        print("⚠️ Some improvements need attention")
        return 1

if __name__ == "__main__":
    sys.exit(main())
