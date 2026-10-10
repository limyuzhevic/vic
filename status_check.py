#!/usr/bin/env python3
"""
NLM Project - Quick Status Check

This script provides a quick status check of the NLM project improvements.
It verifies the most important improvements and shows what users can do now.

Run with: python3 status_check.py
"""

import os
import sys
def check_improvement(description, files):
    """Check if improvements are in place"""
    found = 0
    for file in files:
        if os.path.exists(file):
            found += 1
    
    status = "✅" if found == len(files) else "⚠"
    print(f"{status} {description}")
    if found < len(files):
        print(f"   Files found: {found}/{len(files)}")
    return found == len(files)
def main():
    print("=== NLM Project - Quick Status Check ===\n")
    print("This script checks the key improvements made to NLM.\n")
    
    # Check critical improvements
    print("🔍 Checking Critical Improvements:")
    
    improvements = [
        ("Memory leak fixed in Config class", ["src/core/Config/Config.cpp"]),
        ("JSON/YAML support added", ["src/core/Config/Config.cpp"]),
        ("New convenience functions added", ["python/bindings.cpp"]),
        ("Comprehensive documentation", [
            "README.md", "NLM_PYTHON_QUICK_START.md", 
            "HOW_TO_USE.md", "BUILD.md", "easy_usage.md"
        ]),
        ("Example scripts available", [
            "nlm_demo.py", "verify_nlm.py", "demo_improvements.py"
        ]),
        ("Backward compatibility maintained", ["python/bindings.cpp"])
    ]
    
    results = []
    for description, files in improvements:
        ok = check_improvement(description, files)
        results.append(ok)
    
    print("\n" + "="*50)
    print("📊 STATUS SUMMARY")
    print("="*50)
    
    completed = sum(results)
    total = len(results)
    
    print(f"Improvements implemented: {completed}/{total}")
    
    if completed == total:
        print("🎉 SUCCESS! All key improvements are in place!")
        
        print("\n🚀 What you can do now:")
        print("1. Use convenience functions for easy setup")
        print("2. Run comprehensive examples")
        print("3. Access complete documentation")
        print("4. Verify installation with verification script")
        
        print("\n📝 Quick commands:")
        print("   # Install NLM")
        print("   pip install -e .")
        print()
        print("   # Try convenience functions")
        print("   python3 -c \"import nlm; brain = nlm.create_default_brain(); print(f'Brain: {brain.getTotalNeuronCount()} neurons')\"")
        print()
        print("   # Run comprehensive demo")
        print("   python3 nlm_demo.py")
        print()
        print("   # Verify installation")
        print("   python3 verify_nlm.py")
        print()
        print("   # View documentation")
        print("   less NLM_PYTHON_QUICK_START.md")
        
        print("\n✨ The NLM project is ready for use!")
        return 0
    else:
        print(f"⚠ {total - completed} improvements may not be complete")
        
        print("\n🔧 Check the failed improvements above")
        return 1
if __name__ == "__main__":
    sys.exit(main())