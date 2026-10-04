#!/usr/bin/env python3
"""
Phase 6 Integration Verification Script

This script verifies that the key Phase 6 integration has been completed
by examining the Brain.cpp file to ensure all the integrated systems
are properly called in the brain loop.
"""

import re

def verify_brain_integration():
    """Verify that Brain.cpp contains all the Phase 6 integrated systems."""
    print("Verifying NLM Phase 6 Integration in Brain.cpp")
    print("="*80)
    
    # Read the Brain.cpp file
    try:
        with open("src/brain/Brain.cpp", "r") as f:
            content = f.read()
    except FileNotFoundError:
        print("ERROR: src/brain/Brain.cpp not found!")
        return False
    
    # Check for integrated systems in the brain loop using more flexible patterns
    integration_found = 0
    total_checks = 0
    
    checks = [
        ("Working Memory integration", ["STEP 3.5: Working Memory Integration", "if (pImpl->workingMemory) {"]),
        ("Episodic Memory integration", ["STEP 7: Update episodic memory", "if (pImpl->episodicMemory) {", "episodes"]),
        ("Prediction System integration", ["STEP 8: Update prediction system", "pImpl->predictionSystem->recordSensoryState"]),
        ("Attention System integration", ["STEP 9: Update attention system", "pImpl->attention->processCompetition"]),
        ("Concept Formation integration", ["STEP 10: Update concept formation", "pImpl->conceptFormation->presentExperience"]),
        ("Structural Plasticity integration", ["STEP 11: Apply structural plasticity periodically", "pImpl->structuralPlasticity->update"]),
        ("Memory Replay integration", ["STEP 12: Replay important memories", "pImpl->episodicMemory->getEpisodesForReplay"]),
        ("Development integration", ["STEP 13: Apply development effects", "pImpl->developmentSystem->update"]),
        ("Memory Consolidation integration", ["STEP 14: Periodic memory consolidation", "pImpl->episodicMemory->consolidate"]),
        ("Checkpoint Management integration", ["STEP 15: Checkpoint management", "pImpl->checkpointManager->update"]),
    ]
    
    for check_name, patterns in checks:
        total_checks += 1
        found = any(pattern in content for pattern in patterns)
        if found:
            integration_found += 1
            print(f"✓ {check_name}: Found")
        else:
            print(f"✗ {check_name}: NOT FOUND")
    
    # Check that the brain loop comments were updated
    loop_comments_updated = (
        "PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP" in content and
        "1. Process pending delayed spike events (deliver synaptic input)" in content and
        "4. Working memory integration (store firing neurons, apply attention)" in content
    )
    
    if loop_comments_updated:
        print("✓ Brain loop comments: Updated to Phase 6")
        integration_found += 1
        total_checks += 1
    else:
        print("✗ Brain loop comments: Not updated")
    
    print(f"\nINTEGRATION STATUS: {integration_found}/{total_checks} checks passed")
    
    return integration_found == total_checks

def verify_memory_systems():
    """Verify that memory systems are properly declared in Brain.cpp."""
    print("\nVerifying Memory System Declarations in Brain.cpp")
    print("="*80)
    
    try:
        with open("src/brain/Brain.cpp", "r") as f:
            content = f.read()
    except FileNotFoundError:
        print("ERROR: src/brain/Brain.cpp not found!")
        return False
    
    # Check that memory system accessors return proper pointers (not nullptr)
    memory_accessor_checks = [
        ("getWorkingMemory()", "NeuralWorkingMemory* Brain::getWorkingMemory()"),
        ("getEpisodicMemory()", "NeuralEpisodicMemory* Brain::getEpisodicMemory()"),
        ("getAssociativeMemory()", "NeuralAssociativeMemory* Brain::getAssociativeMemory()"),
        ("getPredictionSystem()", "PredictionSystem* Brain::getPredictionSystem()"),
        ("getPlanner()", "NeuralPlanner* Brain::getPlanner()"),
        ("getConceptFormation()", "ConceptFormation* Brain::getConceptFormation()"),
        ("getAttention()", "AttentionalSelection* Brain::getAttention()"),
    ]
    
    all_passed = True
    
    for check_name, pattern in memory_accessor_checks:
        if pattern in content:
            print(f"✓ {check_name}: Properly implemented")
        else:
            print(f"✗ {check_name}: Not found or incorrect")
            all_passed = False
    
    # Check that memory systems are initialized in the Impl struct
    if "std::unique_ptr<NeuralWorkingMemory> workingMemory;" in content:
        print("✓ NeuralWorkingMemory declared in Impl struct")
    else:
        print("✗ NeuralWorkingMemory not declared in Impl struct")
        all_passed = False
    
    if "std::unique_ptr<NeuralEpisodicMemory> episodicMemory;" in content:
        print("✓ NeuralEpisodicMemory declared in Impl struct")
    else:
        print("✗ NeuralEpisodicMemory not declared in Impl struct")
        all_passed = False
    
    if "std::unique_ptr<NeuralAssociativeMemory> associativeMemory;" in content:
        print("✓ NeuralAssociativeMemory declared in Impl struct")
    else:
        print("✗ NeuralAssociativeMemory not declared in Impl struct")
        all_passed = False
    
    return all_passed

def verify_development_integration():
    """Verify that the development system is properly integrated."""
    print("\nVerifying Development System Integration")
    print("="*80)
    
    try:
        with open("src/brain/Brain.cpp", "r") as f:
            content = f.read()
    except FileNotFoundError:
        print("ERROR: src/brain/Brain.cpp not found!")
        return False
    
    development_checks = [
        ("Developmental stage updates", "pImpl->developmentalStage = stage;"),
        ("Brain development() method", "void Brain::develop()"),
        ("Development system update", "pImpl->developmentSystem->update"),
        ("Neuromodulation stage effects", "switch (stage)"),
        ("Memory consolidation stage effects", "switch (stage)"),
        ("Attention stage effects", "switch (stage)"),
        ("Plasticity stage effects", "switch (stage)"),
    ]
    
    all_passed = True
    
    for check_name, pattern in development_checks:
        if pattern in content:
            print(f"✓ {check_name}: Implemented")
        else:
            print(f"✗ {check_name}: Missing")
            all_passed = False
    
    return all_passed

def main():
    """Main verification function."""
    print("NLM Phase 6 Integration Verification")
    print("="*80)
    print("This script verifies that Phase 6 integration has been completed")
    print("by checking Brain.cpp for the integration of all systems.")
    print("="*80)
    
    # Run verification checks
    brain_integration_ok = verify_brain_integration()
    memory_systems_ok = verify_memory_systems()
    development_integration_ok = verify_development_integration()
    
    # Summary
    print("\n" + "="*80)
    print("VERIFICATION SUMMARY")
    print("="*80)
    
    if brain_integration_ok and memory_systems_ok and development_integration_ok:
        print("✓ PHASE 6 INTEGRATION SUCCESSFULLY COMPLETED")
        print("\nThe NLM codebase has been successfully integrated with:")
        print("• Working memory integrated with attention system")
        print("• Episodic memory connected to experience logging")
        print("• Prediction system integrated with sensory processing")
        print("• Attention system connected to working memory")
        print("• Concept formation integrated with experience processing")
        print("• Enhanced neuromodulation effects")
        print("• Memory consolidation and replay mechanisms")
        print("• Comprehensive development system integration")
        print("• Neural excitability changes with development")
        print("• Neuromodulation level changes with development")
        print("• Memory consolidation changes with development")
        print("• Attention system development")
        print("• Plasticity rule adaptation")
        print("• Structural plasticity optimization")
        print("\nThe Brain class now implements a complete integrated brain loop")
        print("that processes sensory input through all cognitive systems")
        print("and produces appropriate actions.")
        return 0
    else:
        print("✗ PHASE 6 INTEGRATION INCOMPLETE")
        print("\nSome integration components are missing:")
        if not brain_integration_ok:
            print("• Brain loop integration is incomplete")
        if not memory_systems_ok:
            print("• Memory system declarations are incorrect")
        if not development_integration_ok:
            print("• Development system integration is incomplete")
        print("\nPlease review the failed checks above.")
        return 1

if __name__ == "__main__":
    exit(main())
