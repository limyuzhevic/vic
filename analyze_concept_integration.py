#!/usr/bin/env python3
"""Script to analyze and fix the concept formation integration issues."""

import os
import re
from pathlib import Path

def check_concept_formation_integration():
    """Check where concept formation is (or isn't) integrated."""
    issues = []
    
    # Check Brain.cpp for concept formation integration
    brain_cpp_path = "/workspace/app/src/brain/Brain.cpp"
    with open(brain_cpp_path, 'r') as f:
        content = f.read()
        
    # Check if concept formation is called in step()
    if "presentExperience" in content and "if (pImpl->conceptFormation)" in content:
        # Check if concept formation is actually called
        if "getConceptFormation()->presentExperience" not in content:
            issues.append("ConceptFormation exists but presentExperience() not called in Brain::step()")
        else:
            issues.append("ConceptFormation.presentExperience() is called (good!)")
    else:
        issues.append("ConceptFormation presentExperience() not integrated")
    
    # Check if concept formation step is properly implemented
    step_section_start = content.find("// ========== STEP 10: Update concept formation ==========")
    if step_section_start != -1:
        step_section = content[step_section_start:step_section_start + 200]
        if "Would process current neural activity patterns" in step_section:
            issues.append("Concept formation step is just a comment, not implemented")
    
    # Check Brain.hpp for interface
    brain_hpp_path = "/workspace/app/src/brain/Brain.hpp"
    with open(brain_hpp_path, 'r') as f:
        brain_hpp = f.read()
        
    if "ConceptFormation* getConceptFormation()" in brain_hpp:
        issues.append("Brain has getConceptFormation() accessor (good!)")
    
    # Check ConceptFormation for key methods
    cf_hpp_path = "/workspace/app/src/cognition/ConceptFormation.hpp"
    with open(cf_hpp_path, 'r') as f:
        cf_hpp = f.read()
        
    if "presentExperience" in cf_hpp:
        issues.append("ConceptFormation has presentExperience() method (good!)")
    
    # Check if concept formation has integration hooks
    if "updateConcept" in cf_hpp:
        issues.append("ConceptFormation has updateConcept() method (good!)")
    
    return issues

def analyze_sensory_pipeline():
    """Analyze the sensory processing pipeline."""
    issues = []
    
    # Check Brain.cpp for sensoryPattern usage
    brain_cpp_path = "/workspace/app/src/brain/Brain.cpp"
    with open(brain_cpp_path, 'r') as f:
        content = f.read()
        
    # Count how many times sensoryPattern is used
    sensory_pattern_count = content.count("sensoryPattern")
    issues.append(f"sensoryPattern is used {sensory_pattern_count} times in Brain.cpp")
    
    # Check if sensoryPattern is populated
    if "sensoryPattern" in content and "store(sensoryPattern" not in content:
        issues.append("sensoryPattern is used but never populated from sensory input")
    
    # Check receiveSensoryInput
    if "receiveSensoryInput" in content:
        issues.append("Brain has receiveSensoryInput() method")
        # Check if it populates sensoryPattern
        if "sensoryPattern.push_back" not in content:
            issues.append("receiveSensoryInput() doesn't populate sensoryPattern")
    
    return issues

def analyze_memory_integration():
    """Analyze memory system integration."""
    issues = []
    
    brain_cpp_path = "/workspace/app/src/brain/Brain.cpp"
    with open(brain_cpp_path, 'r') as f:
        content = f.read()
        
    # Check working memory integration
    if "getWorkingMemory" in content:
        issues.append("Brain has getWorkingMemory() accessor")
        
    # Check episodic memory integration
    if "getEpisodicMemory" in content:
        issues.append("Brain has getEpisodicMemory() accessor")
    
    # Check attention integration
    if "getAttention" in content:
        issues.append("Brain has getAttention() accessor")
    
    return issues

def main():
    print("=" * 70)
    print("Concept Formation Integration Analysis")
    print("=" * 70)
    
    print("\n1. Concept Formation Integration Check:")
    print("-" * 70)
    cf_issues = check_concept_formation_integration()
    for issue in cf_issues:
        print(f"  {issue}")
    
    print("\n2. Sensory Processing Pipeline Analysis:")
    print("-" * 70)
    sensory_issues = analyze_sensory_pipeline()
    for issue in sensory_issues:
        print(f"  {issue}")
    
    print("\n3. Memory System Integration Analysis:")
    print("-" * 70)
    memory_issues = analyze_memory_integration()
    for issue in memory_issues:
        print(f"  {issue}")
    
    print("\n" + "=" * 70)
    print("Summary:")
    print("-" * 70)
    print("The main issue is that ConceptFormation::presentExperience() exists")
    print("but is not called in the main Brain::step() loop.")
    print("Concept formation needs to be integrated into the sensory processing")
    print("pipeline and connected to memory systems for proper operation.")
    print("=" * 70)

if __name__ == "__main__":
    main()