#!/usr/bin/env python3
"""
Final Verification Script
========================

This script performs a final verification of all NLM examples to ensure they
are properly structured and can be imported without errors.

File: examples/final_verification.py
"""

import sys
import os
import importlib.util
from datetime import datetime

# Add the examples directory to the path
examples_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, examples_dir)

def check_file_structure():
    """Check if all expected files exist and have proper structure."""
    print("🔍 Checking file structure...")
    
    expected_files = [
        "advanced_agent_simulation.py",
        "research_simulation.py",
        "performance_benchmark.py",
        "robust_simulation.py",
        "comprehensive_demo.py",
        "example_validator.py",
        "EXAMPLES_README.md"
    ]
    
    results = {}
    
    for filename in expected_files:
        filepath = os.path.join(examples_dir, filename)
        exists = os.path.exists(filepath)
        
        if exists:
            # Check if file is not empty
            with open(filepath, 'r') as f:
                content = f.read()
                is_not_empty = len(content.strip()) > 0
        
        results[filename] = {
            "exists": exists,
            "not_empty": is_not_empty if exists else False,
            "size": os.path.getsize(filepath) if exists else 0
        }
        
        status = "✅" if (exists and is_not_empty) else "❌"
        print(f"   {status} {filename}: {'OK' if exists and is_not_empty else 'MISSING/EMPTY'}")
    
    return results

def check_imports():
    """Check if all example files can be imported."""
    print("\n🔍 Checking imports...")
    
    example_files = [
        "advanced_agent_simulation",
        "research_simulation",
        "performance_benchmark",
        "robust_simulation",
        "comprehensive_demo"
    ]
    
    import_results = {}
    
    for module_name in example_files:
        filepath = os.path.join(examples_dir, f"{module_name}.py")
        
        try:
            # Try to import the module
            spec = importlib.util.spec_from_file_location(module_name, filepath)
            if spec is None:
                import_results[module_name] = {
                    "status": "error",
                    "error": "Cannot load module specification"
                }
                continue
            
            module = importlib.util.module_from_spec(spec)
            
            # Try to execute the module
            spec.loader.exec_module(module)
            
            import_results[module_name] = {
                "status": "success",
                "has_functions": True,
                "has_classes": any(not name.startswith('_') and callable(getattr(module, name)) 
                                 for name in dir(module))
            }
            
            status = "✅"
            print(f"   {status} {module_name}: IMPORTED SUCCESSFULLY")
            
        except Exception as e:
            import_results[module_name] = {
                "status": "error",
                "error": str(e)
            }
            
            status = "❌"
            print(f"   {status} {module_name}: IMPORT FAILED - {e}")
    
    return import_results

def check_documentation():
    """Check if documentation files are properly formatted."""
    print("\n🔍 Checking documentation...")
    
    doc_results = {}
    
    # Check README
    readme_path = os.path.join(examples_dir, "EXAMPLES_README.md")
    if os.path.exists(readme_path):
        with open(readme_path, 'r') as f:
            content = f.read()
        
        # Check for key sections
        key_sections = ["Overview", "Key Features", "How to Use", "Validation"]
        found_sections = []
        
        for section in key_sections:
            if section in content:
                found_sections.append(section)
        
        doc_results["EXAMPLES_README.md"] = {
            "exists": True,
            "has_sections": len(found_sections),
            "total_sections": len(key_sections),
            "word_count": len(content.split())
        }
        
        coverage = len(found_sections) / len(key_sections) * 100
        status = "✅" if coverage >= 0.8 else "⚠️"
        print(f"   {status} EXAMPLES_README.md: {len(found_sections)}/{len(key_sections)} sections")
    
    return doc_results

def main():
    """Run final verification."""
    print("="*70)
    print("🔍 NLM EXAMPLES FINAL VERIFICATION")
    print("="*70)
    print(f"Verified at: {datetime.now()}")
    print()
    
    # Run all checks
    file_results = check_file_structure()
    import_results = check_imports()
    doc_results = check_documentation()
    
    # Generate summary
    print("\n" + "="*70)
    print("📊 VERIFICATION SUMMARY")
    print("="*70)
    
    # File structure check
    files_ok = sum(1 for r in file_results.values() if r["exists"] and r["not_empty"])
    total_files = len(file_results)
    print(f"📁 Files check: {files_ok}/{total_files} ({files_ok/total_files*100:.1f}%)")
    
    # Import check
    imports_ok = sum(1 for r in import_results.values() if r["status"] == "success")
    total_imports = len(import_results)
    print(f"📦 Imports check: {imports_ok}/{total_imports} ({imports_ok/total_imports*100:.1f}%)")
    
    # Documentation check
    docs_ok = sum(1 for r in doc_results.values() if r.get("has_sections", 0) > 0)
    total_docs = len(doc_results)
    print(f"📚 Documentation check: {docs_ok}/{total_docs} ({docs_ok/total_docs*100:.1f}%)")
    
    # Overall status
    overall_ok = (files_ok == total_files and 
                  imports_ok == total_imports and 
                  docs_ok > 0)
    
    print(f"\n🎯 Overall status: {'✅ ALL CHECKS PASSED' if overall_ok else '❌ SOME CHECKS FAILED'}")
    
    if overall_ok:
        print("\n🎉 NLM examples are ready for use!")
        print("\nNext steps:")
        print("1. Ensure NLM Python bindings are built and available")
        print("2. Run individual examples to test functionality")
        print("3. Use examples as templates for your own applications")
        print("4. Refer to EXAMPLES_README.md for detailed documentation")
    else:
        print("\n❌ Some checks failed. Please review the issues above.")
    
    return 0 if overall_ok else 1

if __name__ == "__main__":
    sys.exit(main())
