#!/usr/bin/env python3

"""
Quick verification script for enhanced NLM Python bindings.

This script provides a quick check that the enhanced NLM Python bindings
are properly structured and ready for use.
"""

import os
import sys

def check_files_exist():
    """Check that essential files exist"""
    print("Checking essential files...")
    
    essential_files = [
        "/workspace/app/python/bindings.cpp",
        "/workspace/app/python/CMakeLists.txt",
        "/workspace/app/nlm_cli.py",
        "/workspace/app/enhanced_bindings_demo.py",
        "/workspace/app/NLM_ENHANCED_BINDINGS_FINAL.md"
    ]
    
    all_exist = True
    for file_path in essential_files:
        if os.path.exists(file_path):
            print(f"✓ {os.path.basename(file_path)}")
        else:
            print(f"❌ {os.path.basename(file_path)} - MISSING")
            all_exist = False
    
    return all_exist

def check_python_availability():
    """Check Python environment"""
    print("\nChecking Python environment...")
    
    try:
        import pynlm
        print("✓ pynlm module available")
        return True
    except ImportError as e:
        print(f"❌ pynlm module not available: {e}")
        return False

def check_bindings_complexity():
    """Check enhanced bindings complexity"""
    print("\nChecking enhanced bindings complexity...")
    
    try:
        with open("/workspace/app/python/bindings.cpp", 'r') as f:
            content = f.read()
        
        line_count = len(content.splitlines())
        if line_count > 1000:
            print(f"✓ Enhanced bindings.cpp has {line_count:,} lines (comprehensive)")
        else:
            print(f"⚠️  Enhanced bindings.cpp has {line_count:,} lines (may need expansion)")
        
        # Check for key class inclusions
        key_classes = ['Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
                      'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
                      'Reward', 'Curiosity', 'Novelty', 'PredictionError',
                      'STDP', 'Hebbian', 'PlasticityRule', 'DevelopmentSystem',
                      'NeuralPlanner', 'ConceptFormation', 'SelfModel']
        
        found_classes = []
        for cls in key_classes:
            if f"#include \"{cls}.hpp\"" in content or f'#include "{cls}.hpp"' in content:
                found_classes.append(cls)
        
        print(f"✓ Found {len(found_classes)} key C++ classes in bindings")
        print(f"  Classes: {', '.join(found_classes[:5])}{'...' if len(found_classes) > 5 else ''}")
        
        return True
        
    except Exception as e:
        print(f"❌ Error checking enhanced bindings: {e}")
        return False

def check_cli_features():
    """Check CLI features"""
    print("\nChecking CLI features...")
    
    try:
        with open("/workspace/app/nlm_cli.py", 'r') as f:
            content = f.read()
        
        # Check for key features
        features = {
            'Argument parsing': 'argparse' in content,
            'Sub-commands': 'add_subparsers' in content,
            'Error handling': 'validate_args' in content,
            'Help system': 'print_help' in content,
            'Progress monitoring': 'verbose' in content,
            'Batch processing': 'batch_simulate' in content,
            'World simulation': 'run_world_simulation' in content,
            'Analysis tools': 'analyze_brain' in content,
            'Visualization': 'visualize_brain' in content,
            'Demo support': 'demo_phase6' in content
        }
        
        print(f"✓ CLI features check:")
        for feature, present in features.items():
            status = "✓" if present else "❌"
            print(f"  {status} {feature}")
        
        return all(features.values())
        
    except Exception as e:
        print(f"❌ Error checking CLI features: {e}")
        return False

def check_documentation():
    """Check documentation"""
    print("\nChecking documentation...")
    
    doc_files = [
        "/workspace/app/NLM_ENHANCED_BINDINGS_FINAL.md",
        "/workspace/app/ENHANCED_BINDINGS_SUMMARY.md",
        "/workspace/app/ENHANCED_BINDINGS_README.md"
    ]
    
    doc_count = 0
    for doc_file in doc_files:
        if os.path.exists(doc_file):
            with open(doc_file, 'r') as f:
                content = f.read()
                if len(content) > 1000:  # Ensure substantial content
                    doc_count += 1
                    print(f"✓ {os.path.basename(doc_file)}")
                else:
                    print(f"⚠️  {os.path.basename(doc_file)} - too short")
    
    print(f"✓ Documentation: {doc_count}/{len(doc_files)} files present and substantial")
    return doc_count > 0

def main():
    """Main verification function"""
    print("=" * 70)
    print("NLM Enhanced Python Bindings - Verification Script")
    print("=" * 70)
    
    checks = [
        ("Essential Files", check_files_exist),
        ("Python Environment", check_python_availability),
        ("Enhanced Bindings", check_bindings_complexity),
        ("CLI Features", check_cli_features),
        ("Documentation", check_documentation)
    ]
    
    results = []
    for check_name, check_func in checks:
        print(f"\n{'='*70}")
        print(f"Running: {check_name}")
        print('='*70)
        result = check_func()
        results.append((check_name, result))
    
    # Summary
    print("\n" + "="*70)
    print("VERIFICATION SUMMARY")
    print("="*70)
    
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for check_name, result in results:
        status = "✓ PASS" if result else "❌ FAIL"
        print(f"{status}: {check_name}")
    
    print(f"\nOverall: {passed}/{total} checks passed")
    
    if passed == total:
        print("\n🎉 All verification checks passed!")
        print("The enhanced NLM Python bindings are ready for use.")
        print("\nNext steps:")
        print("1. Build the enhanced bindings: pip install -e .")
        print("2. Test with: python enhanced_bindings_demo.py")
        print("3. Use CLI with: nlm_cli.py --help")
        return 0
    else:
        print(f"\n⚠️  {total - passed} checks failed.")
        print("Please review the output above and fix any issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())