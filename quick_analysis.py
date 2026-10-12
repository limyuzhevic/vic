#!/usr/bin/env python3
"""
Quick Code Quality Analysis for NLM Project
"""

import os
import json
from pathlib import Path

def analyze_key_files():
    print("🚀 NLM Project Code Quality Analysis")
    print("=" * 60)
    
    project_path = Path("/workspace/app")
    
    # Key files to analyze
    key_files = [
        ("src/brain/Brain.cpp", "Main Brain Implementation - 1117 lines"),
        ("src/agent/AgentBrain.cpp", "Agent Interface - 350 lines"),
        ("src/main.cpp", "Main Demo - 449 lines"),
        ("src/core/Config/Config.cpp", "Configuration System - 215 lines"),
        ("src/experiments/Phase6Demo.cpp", "Phase 6 Integration - 85 lines")
    ]
    
    analysis = {}
    
    for file_path, description in key_files:
        full_path = project_path / file_path
        if full_path.exists():
            print(f"\n📖 {description} ({file_path}):")
            
            with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            lines = len(content.split('\n'))
            non_empty_lines = len([line for line in content.split('\n') if line.strip()])
            
            print(f"   📄 Total lines: {lines:,}")
            print(f"   📝 Non-empty lines: {non_empty_lines:,}")
            
            # Identify critical issues
            issues = []
            if lines > 800:
                issues.append("CRITICAL: File too large for maintainable code")
            if lines > 400 and "Brain.cpp" in file_path:
                issues.append("REQUIRES REFACTORING: Main brain logic needs separation")
            
            if issues:
                print(f"   🚨 CRITICAL ISSUES:")
                for issue in issues:
                    print(f"      • {issue}")
            
            analysis[file_path] = {
                'description': description,
                'total_lines': lines,
                'non_empty_lines': non_empty_lines,
                'issues': issues
            }
        else:
            print(f"\n❌ File not found: {file_path}")
            analysis[file_path] = {'error': 'File not found'}
    
    # Generate improvement recommendations
    print(f"\n🔧 IMPROVEMENT RECOMMENDATIONS")
    print("=" * 50)
    
    recommendations = [
        "1. REFACTOR Brain.cpp: Split into focused classes (NeuralCore, MemorySystem, Plasticity, etc.)",
        "2. IMPROVE Config.cpp: Replace key=value parser with JSON/YAML support",
        "3. SIMPLIFY AgentBrain: Extract motor decoding into separate class",
        "4. ENHANCE main.cpp: Add comprehensive command-line interface",
        "5. STRUCTURE experiments: Create unified experiment framework"
    ]
    
    for rec in recommendations:
        print(f"   ✅ {rec}")
    
    # Summary
    print(f"\n🎯 EXECUTIVE SUMMARY")
    print("=" * 50)
    print("✅ Analysis completed successfully")
    print(f"📊 Analyzed {len(key_files)} key files")
    print("🔧 Priority improvements identified")
    
    # Save analysis
    results = {
        'analysis': analysis,
        'recommendations': recommendations,
        'timestamp': str(__import__('datetime').datetime.now())
    }
    
    with open('/workspace/app/analysis_results.json', 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"\n💾 Analysis saved to: /workspace/app/analysis_results.json")
    return results

if __name__ == "__main__":
    analyze_key_files()