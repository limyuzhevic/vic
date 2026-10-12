#!/usr/bin/env python3
"""
Simple Code Quality Analysis for NLM Project
"""

import os
import re
from pathlib import Path

def analyze_project():
    print("🚀 NLM Project Code Quality Analysis")
    print("=" * 60)
    
    project_path = Path("/workspace/app")
    
    # Find all C++ files
    cpp_files = list(project_path.rglob("*.cpp"))
    header_files = list(project_path.rglob("*.hpp"))
    total_files = cpp_files + header_files
    
    print(f"📁 Total C++/Header files: {len(total_files)}")
    
    # Analyze key files
    key_files = {
        "src/brain/Brain.cpp": "Main Brain Implementation",
        "src/agent/AgentBrain.cpp": "Agent Interface", 
        "src/main.cpp": "Main Demo",
        "src/core/Config/Config.cpp": "Configuration System",
        "src/experiments/Phase6Demo.cpp": "Phase 6 Integration"
    }
    
    print(f"\n📊 KEY FILES ANALYSIS:")
    for file_path, description in key_files.items():
        full_path = project_path / file_path
        if full_path.exists():
            with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            lines = len(content.split('\n'))
            non_empty_lines = len([line for line in content.split('\n') if line.strip()])
            class_count = len(re.findall(r'\bclass\s+\w+', content))
            function_count = len(re.findall(r'^\s*(?:bool|void|double|float|\w+)\s+\w*\s*\(', content, re.MULTILINE))
            
            print(f"\n📖 {description} ({file_path}):")
            print(f"   📄 Total lines: {lines:,}")
            print(f"   📝 Non-empty lines: {non_empty_lines:,}")
            print(f"   🏛️  Classes: {class_count}")
            print(f"   🔧 Functions: {function_count}")
            
            # Identify issues
            issues = []
            if lines > 500:
                issues.append("Extremely large file - needs refactoring")
            if class_count > 1:
                issues.append("Multiple classes in single file - consider separation")
            
            if issues:
                print(f"   ⚠️  Issues:")
                for issue in issues:
                    print(f"      • {issue}")
        else:
            print(f"\n❌ File not found: {file_path}")
    
    print(f"\n🎯 EXECUTIVE SUMMARY")
    print("=" * 50)
    print("✅ Analysis completed successfully")
    print(f"📊 Analyzed {len(total_files)} C++/Header files")
    print("🔧 Ready to implement improvement plan")
    
    # Save analysis results
    results = {
        'total_files': len(total_files),
        'cpp_files': len(cpp_files),
        'header_files': len(header_files),
        'timestamp': str(__import__('datetime').datetime.now())
    }
    
    import json
    with open('/workspace/app/analysis_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"💾 Analysis saved to: /workspace/app/analysis_results.json")
    return results

if __name__ == "__main__":
    analyze_project()