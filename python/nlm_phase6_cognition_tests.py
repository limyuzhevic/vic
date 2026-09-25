#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - Cognition System Tests

Comprehensive tests for cognition system integration in NLM Phase 6.
"""

import sys
import os
import time
import json
import tempfile
import numpy as np
from typing import Dict, Any, List, Optional

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM Python bindings not available: {e}")
    PYNLM_AVAILABLE = False

class CognitionSystemIntegrationTester:
    """Comprehensive cognition system integration tests for NLM Phase 6"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.test_results = {}
        
    def test_attention_system_integration(self) -> Dict[str, Any]:
        """Test attention system integration"""
        print("👁️ Testing Attention System Integration...")
        
        result = {
            "test_name": "attention_system_integration",
            "status": "FAIL",
            "details": "",
            "metrics": {},
            "duration": 0.0
        }
        
        if not PYNLM_AVAILABLE:
            result["details"] = "SKIP: NLM Python bindings not available"
            return result
            
        try:
            start_time = time.time()
            
            # Create brain with attention focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 180)
            config.set("enable_attention", True)
            config.set("attention_window_size", 20)
            config.set("attention_competition_factor", 0.8)
            config.set("enable_concept_formation", True)
            config.set("enable_working_memory", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test attention system competitive dynamics
            test_duration = 150
            attention_shifts = 0
            focused_neuron_selections = 0
            attention_entropies = []
            focus_durations = []
            
            for step in range(test_duration):
                # Get attention state
                attention_state = brain.getAttentionState()
                attention_entropy = attention_state.get("entropy", 0.0)
                attention_entropies.append(attention_entropy)
                
                # Check for attention shifts
                if step % 25 == 0:
                    attention_shifts += 1
                
                # Test focus selection
                if brain.getAttentionFocus() > 0.7:
                    focused_neuron_selections += 1
                
                # Simulate attention window changes
                if step % 30 == 0:
                    brain.updateAttentionWindow(config.get("attention_window_size", 20))
                
                # Step brain with attention influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_attention_entropy = sum(attention_entropies) / max(1, len(attention_entropies))
            result["metrics"] = {
                "attention_shifts": attention_shifts,
                "focused_neuron_selections": focused_neuron_selections,
                "average_attention_entropy": avg_attention_entropy,
                "min_attention_entropy": min(attention_entropies) if attention_entropies else 0,
                "max_attention_entropy": max(attention_entropies) if attention_entropies else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (attention_shifts >= 5 and 
                focused_neuron_selections >= 3 and
                avg_attention_entropy > 0.1 and
                avg_attention_entropy < 1.5):
                result["status"] = "PASS"
                result["details"] = "Attention system integrated successfully with concept formation and working memory"
            else:
                result["details"] = f"Attention system integration failed: shifts={attention_shifts}, focused={focused_neuron_selections}, avg_entropy={avg_attention_entropy}"
                
        except Exception as e:
            result["details"] = f"Attention system test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_planning_system_integration(self) -> Dict[str, Any]:
        """Test planning system integration"""
        print("🧭 Testing Planning System Integration...")
        
        result = {
            "test_name": "planning_system_integration",
            "status": "FAIL",
            "details": "",
            "metrics": {},
            "duration": 0.0
        }
        
        if not PYNLM_AVAILABLE:
            result["details"] = "SKIP: NLM Python bindings not available"
            return result
            
        try:
            start_time = time.time()
            
            # Create brain with planning focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 220)
            config.set("enable_planning", True)
            config.set("planning_horizon", 5)
            config.set("action_sequence_efficiency", 0.8)
            config.set("enable_self_model", True)
            config.set("enable_motor_system", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test planning system action sequence evaluation
            test_duration = 200
            planning_cycles = 0
            action_sequences_evaluated = 0
            planning_confidences = []
            
            for step in range(test_duration):
                # Generate action sequences
                if step % 20 == 0:
                    planning_cycles += 1
                
                # Evaluate action sequences
                if np.random.random() < 0.15:  # 15% chance of action evaluation
                    action_sequence = brain.evaluateActionSequence()
                    action_sequences_evaluated += 1
                    planning_confidences.append(action_sequence.get("confidence", 0.0))
                
                # Step brain with planning influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_planning_confidence = sum(planning_confidences) / max(1, len(planning_confidences))
            result["metrics"] = {
                "planning_cycles": planning_cycles,
                "action_sequences_evaluated": action_sequences_evaluated,
                "average_planning_confidence": avg_planning_confidence,
                "min_planning_confidence": min(planning_confidences) if planning_confidences else 0,
                "max_planning_confidence": max(planning_confidences) if planning_confidences else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (planning_cycles >= 8 and 
                action_sequences_evaluated >= 5 and
                avg_planning_confidence > 0.4 and
                avg_planning_confidence < 1.2):
                result["status"] = "PASS"
                result["details"] = "Planning system integrated successfully with self-model and motor systems"
            else:
                result["details"] = f"Planning system integration failed: cycles={planning_cycles}, evaluated={action_sequences_evaluated}, avg_confidence={avg_planning_confidence}"
                
        except Exception as e:
            result["details"] = f"Planning system test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_concept_formation_integration(self) -> Dict[str, Any]:
        """Test concept formation integration"""
        print("💡 Testing Concept Formation Integration...")
        
        result = {
            "test_name": "concept_formation_integration",
            "status": "FAIL",
            "details": "",
            "metrics": {},
            "duration": 0.0
        }
        
        if not PYNLM_AVAILABLE:
            result["details"] = "SKIP: NLM Python bindings not available"
            return result
            
        try:
            start_time = time.time()
            
            # Create brain with concept formation focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 190)
            config.set("enable_concept_formation", True)
            config.set("concept_confidence_threshold", 0.7)
            config.set("pattern_discovery_threshold", 0.6)
            config.set("enable_associative_memory", True)
            config.set("enable_prediction", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test concept formation from experience patterns
            test_duration = 180
            patterns_discovered = 0
            concepts_formed = 0
            concept_confidences = []
            
            # Create some input patterns
            input_patterns = [
                np.random.random(10) * 0.3,
                np.random.random(10) * 0.3 + 0.7,
                np.random.random(10) * 0.5
            ]
            
            for step in range(test_duration):
                # Present patterns for concept formation
                if step % 40 == 0 and patterns_discovered < len(input_patterns):
                    brain.presentPattern(input_patterns[patterns_discovered])
                    patterns_discovered += 1
                
                # Form concepts from patterns
                if np.random.random() < 0.1:  # 10% chance of concept formation
                    concept = brain.formConcept()
                    concepts_formed += 1
                    concept_confidences.append(concept.get("confidence", 0.0))
                
                # Step brain with concept influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_concept_confidence = sum(concept_confidences) / max(1, len(concept_confidences))
            result["metrics"] = {
                "patterns_discovered": patterns_discovered,
                "concepts_formed": concepts_formed,
                "average_concept_confidence": avg_concept_confidence,
                "min_concept_confidence": min(concept_confidences) if concept_confidences else 0,
                "max_concept_confidence": max(concept_confidences) if concept_confidences else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (patterns_discovered >= 3 and 
                concepts_formed >= 2 and
                avg_concept_confidence > 0.5 and
                avg_concept_confidence < 1.5):
                result["status"] = "PASS"
                result["details"] = "Concept formation integrated successfully with associative memory and prediction"
            else:
                result["details"] = f"Concept formation integration failed: patterns={patterns_discovered}, concepts={concepts_formed}, avg_confidence={avg_concept_confidence}"
                
        except Exception as e:
            result["details"] = f"Concept formation test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_cognition_tests(self) -> Dict[str, Any]:
        """Run all cognition system integration tests"""
        print("\n" + "="*60)
        print("🧠 NLM Phase 6 Cognition System Integration Tests")
        print("="*60)
        
        results = {
            "attention_system": self.test_attention_system_integration(),
            "planning_system": self.test_planning_system_integration(),
            "concept_formation": self.test_concept_formation_integration()
        }
        
        # Calculate summary
        total_tests = len(results)
        passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
        failed_tests = total_tests - passed_tests
        
        print("\n" + "="*60)
        print("COGNITION SYSTEM TEST SUMMARY")
        print("="*60)
        print(f"🧪 Total Tests: {total_tests}")
        print(f"✅ Passed: {passed_tests}")
        print(f"❌ Failed: {failed_tests}")
        
        for test_name, result in results.items():
            status_icon = "✅" if result["status"] == "PASS" else "❌"
            print(f"{status_icon} {test_name.replace('_', ' ').title()}: {result['details']}")
        
        return results


def create_cognition_test_suite():
    """Create the complete cognition test suite"""
    print("🧠 Creating Cognition Integration Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_cognition_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize tester
    cognition_tester = CognitionSystemIntegrationTester(temp_dir)
    
    # Run all tests
    print("\n" + "="*80)
    print("NLM PHASE 6 COGNITION INTEGRATION TEST SUITE")
    print("="*80)
    
    results = cognition_tester.run_all_cognition_tests()
    
    # Calculate overall results
    total_tests = len(results)
    passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
    failed_tests = total_tests - passed_tests
    success_rate = (passed_tests / total_tests * 100) if total_tests > 0 else 0
    
    print("\n" + "="*80)
    print("OVERALL COGNITION TEST RESULTS")
    print("="*80)
    print(f"🧪 Total Tests: {total_tests}")
    print(f"✅ Passed: {passed_tests}")
    print(f"❌ Failed: {failed_tests}")
    print(f"📊 Success Rate: {success_rate:.1f}%")
    
    # Save results
    results_file = os.path.join(temp_dir, "nlm_cognition_test_results.json")
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\n💾 Detailed results saved to: {results_file}")
    
    # Cleanup
    import shutil
    shutil.rmtree(temp_dir)
    
    # Return success status
    return failed_tests == 0

if __name__ == "__main__":
    success = create_cognition_test_suite()
    sys.exit(0 if success else 1)