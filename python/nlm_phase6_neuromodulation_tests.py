#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - Neuromodulation System Tests

Comprehensive tests for neuromodulation system integration in NLM Phase 6.
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

class NeuromodulationIntegrationTester:
    """Comprehensive neuromodulation integration tests for NLM Phase 6"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.test_results = {}
        
    def test_dopamine_neuromodulation_integration(self) -> Dict[str, Any]:
        """Test dopamine neuromodulation integration with other brain systems"""
        print("💊 Testing Dopamine Neuromodulation Integration...")
        
        result = {
            "test_name": "dopamine_neuromodulation_integration",
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
            
            # Create brain with dopamine focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 120)
            config.set("enable_dopamine", True)
            config.set("dopamine_scale", 1.0)
            config.set("enable_prediction_error", True)
            config.set("enable_working_memory", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test dopamine modulation of learning and plasticity
            test_duration = 100
            reward_signals = 0
            plasticity_events = 0
            dopamine_levels = []
            
            for step in range(test_duration):
                # Simulate reward prediction error
                if step % 20 == 0:
                    reward = 1.0 if np.random.random() > 0.3 else -0.5
                    brain.applyRewardModulation(reward, step * 0.001)
                    reward_signals += 1
                    
                    # Check dopamine level
                    dopamine_levels.append(brain.getDopamineLevel())
                
                # Test plasticity modulation
                if np.random.random() < 0.05:  # 5% chance of plasticity event
                    brain.triggerPlasticityModulation()
                    plasticity_events += 1
                
                # Step brain with dopamine influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_dopamine = sum(dopamine_levels) / max(1, len(dopamine_levels))
            result["metrics"] = {
                "reward_signals": reward_signals,
                "plasticity_events": plasticity_events,
                "average_dopamine_level": avg_dopamine,
                "max_dopamine_level": max(dopamine_levels) if dopamine_levels else 0,
                "min_dopamine_level": min(dopamine_levels) if dopamine_levels else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (reward_signals >= 3 and 
                plasticity_events >= 2 and
                avg_dopamine > 0.1 and
                avg_dopamine < 2.0):
                result["status"] = "PASS"
                result["details"] = "Dopamine neuromodulation integrated successfully with reward processing and plasticity"
            else:
                result["details"] = f"Dopamine integration failed: rewards={reward_signals}, plasticity={plasticity_events}, avg_dopamine={avg_dopamine}"
                
        except Exception as e:
            result["details"] = f"Dopamine test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_curiosity_integration(self) -> Dict[str, Any]:
        """Test curiosity system integration"""
        print("🔍 Testing Curiosity System Integration...")
        
        result = {
            "test_name": "curiosity_integration",
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
            
            # Create brain with curiosity focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 180)
            config.set("enable_curiosity", True)
            config.set("curiosity_threshold", 0.5)
            config.set("enable_novelty", True)
            config.set("enable_working_memory", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test curiosity-driven exploration
            test_duration = 150
            exploration_events = 0
            novelty_detections = 0
            curiosity_levels = []
            
            for step in range(test_duration):
                # Detect novelty
                if step % 25 == 0:
                    novelty_score = brain.detectNovelty()
                    novelty_detections += 1
                    curiosity_levels.append(brain.getCuriosityLevel())
                
                # Trigger curiosity-driven exploration
                if brain.getCuriosityLevel() > 0.3 and np.random.random() < 0.2:
                    brain.triggerExplorationBehavior()
                    exploration_events += 1
                
                # Step brain with curiosity influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_curiosity = sum(curiosity_levels) / max(1, len(curiosity_levels))
            result["metrics"] = {
                "novelty_detections": novelty_detections,
                "exploration_events": exploration_events,
                "average_curiosity_level": avg_curiosity,
                "max_curiosity_level": max(curiosity_levels) if curiosity_levels else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (novelty_detections >= 5 and 
                exploration_events >= 2 and
                avg_curiosity > 0.2 and
                avg_curiosity < 1.5):
                result["status"] = "PASS"
                result["details"] = "Curiosity system integrated successfully with novelty detection and exploration"
            else:
                result["details"] = f"Curiosity integration failed: novelty={novelty_detections}, exploration={exploration_events}, avg_curiosity={avg_curiosity}"
                
        except Exception as e:
            result["details"] = f"Curiosity test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_novelty_detection_integration(self) -> Dict[str, Any]:
        """Test novelty detection integration"""
        print("🆕 Testing Novelty Detection Integration...")
        
        result = {
            "test_name": "novelty_detection_integration",
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
            
            # Create brain with novelty detection focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 160)
            config.set("enable_novelty", True)
            config.set("novelty_sensitivity", 1.0)
            config.set("enable_associative_memory", True)
            config.set("enable_prediction_error", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test novelty detection with pattern learning
            test_duration = 120
            novelty_detections = 0
            learned_patterns = 0
            novelty_scores = []
            
            # Create some patterns to detect novelty
            baseline_patterns = [
                np.random.random(8) * 0.3,
                np.random.random(8) * 0.3 + 0.7
            ]
            
            for step in range(test_duration):
                # Create some baseline activity
                if step % 30 == 0 and learned_patterns < len(baseline_patterns):
                    brain.learnPattern(baseline_patterns[learned_patterns])
                    learned_patterns += 1
                
                # Detect novelty
                novelty_score = brain.detectNovelty()
                novelty_scores.append(novelty_score)
                
                if novelty_score > 0.5:
                    novelty_detections += 1
                
                # Step brain with novelty influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_novelty = sum(novelty_scores) / max(1, len(novelty_scores))
            result["metrics"] = {
                "learned_patterns": learned_patterns,
                "novelty_detections": novelty_detections,
                "average_novelty_score": avg_novelty,
                "max_novelty_score": max(novelty_scores) if novelty_scores else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (learned_patterns >= 2 and 
                novelty_detections >= 3 and
                avg_novelty > 0.3 and
                avg_novelty < 1.0):
                result["status"] = "PASS"
                result["details"] = "Novelty detection integrated successfully with pattern learning and prediction systems"
            else:
                result["details"] = f"Novelty detection integration failed: patterns={learned_patterns}, detections={novelty_detections}, avg_novelty={avg_novelty}"
                
        except Exception as e:
            result["details"] = f"Novelty detection test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_neuromodulation_tests(self) -> Dict[str, Any]:
        """Run all neuromodulation system integration tests"""
        print("\n" + "="*60)
        print("⚡ NLM Phase 6 Neuromodulation System Integration Tests")
        print("="*60)
        
        results = {
            "dopamine_neuromodulation": self.test_dopamine_neuromodulation_integration(),
            "curiosity": self.test_curiosity_integration(),
            "novelty_detection": self.test_novelty_detection_integration()
        }
        
        # Calculate summary
        total_tests = len(results)
        passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
        failed_tests = total_tests - passed_tests
        
        print("\n" + "="*60)
        print("NEUROMODULATION SYSTEM TEST SUMMARY")
        print("="*60)
        print(f"🧪 Total Tests: {total_tests}")
        print(f"✅ Passed: {passed_tests}")
        print(f"❌ Failed: {failed_tests}")
        
        for test_name, result in results.items():
            status_icon = "✅" if result["status"] == "PASS" else "❌"
            print(f"{status_icon} {test_name.replace('_', ' ').title()}: {result['details']}")
        
        return results


def create_neuromodulation_test_suite():
    """Create the complete neuromodulation test suite"""
    print("⚡ Creating Neuromodulation Integration Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_neuromodulation_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize tester
    neuromodulation_tester = NeuromodulationIntegrationTester(temp_dir)
    
    # Run all tests
    print("\n" + "="*80)
    print("NLM PHASE 6 NEUROMODULATION INTEGRATION TEST SUITE")
    print("="*80)
    
    results = neuromodulation_tester.run_all_neuromodulation_tests()
    
    # Calculate overall results
    total_tests = len(results)
    passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
    failed_tests = total_tests - passed_tests
    success_rate = (passed_tests / total_tests * 100) if total_tests > 0 else 0
    
    print("\n" + "="*80)
    print("OVERALL NEUROMODULATION TEST RESULTS")
    print("="*80)
    print(f"🧪 Total Tests: {total_tests}")
    print(f"✅ Passed: {passed_tests}")
    print(f"❌ Failed: {failed_tests}")
    print(f"📊 Success Rate: {success_rate:.1f}%")
    
    # Save results
    results_file = os.path.join(temp_dir, "nlm_neuromodulation_test_results.json")
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\n💾 Detailed results saved to: {results_file}")
    
    # Cleanup
    import shutil
    shutil.rmtree(temp_dir)
    
    # Return success status
    return failed_tests == 0

if __name__ == "__main__":
    success = create_neuromodulation_test_suite()
    sys.exit(0 if success else 1)