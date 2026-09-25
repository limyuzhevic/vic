#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - Development System Tests

Comprehensive tests for development system integration in NLM Phase 6.
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

class DevelopmentSystemIntegrationTester:
    """Comprehensive development system integration tests for NLM Phase 6"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.test_results = {}
        
    def test_development_stages_integration(self) -> Dict[str, Any]:
        """Test developmental stages integration"""
        print("🌱 Testing Developmental Stages Integration...")
        
        result = {
            "test_name": "development_stages_integration",
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
            
            # Create brain with development focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 150)
            config.set("enable_development", True)
            config.set("development_stage_start", 0.0)
            config.set("development_stage_end", 1.0)
            config.set("plasticity_decay_rate", 0.01)
            config.set("enable_curiosity", True)
            config.set("enable_novelty", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test developmental stages over time
            test_duration = 300
            stage_transitions = 0
            plasticity_events = 0
            curiosity_detections = 0
            development_stages = []
            
            for step in range(test_duration):
                # Update development system
                brain.updateDevelopment(step * 0.001)
                
                # Track development stage
                current_stage = brain.getDevelopmentStage()
                development_stages.append(current_stage)
                
                # Check for stage transitions
                if step % 50 == 0 and current_stage != development_stages[0] if development_stages else True:
                    stage_transitions += 1
                
                # Test plasticity modulation
                if np.random.random() < 0.03:  # 3% chance of plasticity event
                    brain.triggerPlasticityModulation()
                    plasticity_events += 1
                
                # Test curiosity during development
                if brain.getCuriosityLevel() > 0.4:
                    curiosity_detections += 1
                
                # Step brain
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            unique_stages = len(set(development_stages))
            avg_stage = sum(development_stages) / max(1, len(development_stages))
            result["metrics"] = {
                "stage_transitions": stage_transitions,
                "plasticity_events": plasticity_events,
                "curiosity_detections": curiosity_detections,
                "unique_development_stages": unique_stages,
                "average_development_stage": avg_stage,
                "test_duration": test_duration
            }
            
            # Validate results
            if (stage_transitions >= 3 and 
                plasticity_events >= 5 and
                curiosity_detections >= 10 and
                unique_stages >= 3 and
                avg_stage > 0.0 and
                avg_stage < 1.0):
                result["status"] = "PASS"
                result["details"] = "Development stages integrated successfully with curiosity and plasticity systems"
            else:
                result["details"] = f"Development stages integration failed: transitions={stage_transitions}, plasticity={plasticity_events}, curiosity={curiosity_detections}, stages={unique_stages}, avg_stage={avg_stage}"
                
        except Exception as e:
            result["details"] = f"Development stages test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_synaptogenesis_integration(self) -> Dict[str, Any]:
        """Test synaptogenesis integration with structural development"""
        print("🔗 Testing Synaptogenesis Integration...")
        
        result = {
            "test_name": "synaptogenesis_integration",
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
            
            # Create brain with synaptogenesis focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 200)
            config.set("enable_development", True)
            config.set("synaptogenesis_rate", 0.001)
            config.set("pruning_rate", 0.00005)
            config.set("enable_associative_memory", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test synaptogenesis and pruning dynamics
            test_duration = 200
            synaptogenesis_events = 0
            pruning_events = 0
            synapse_counts = []
            
            for step in range(test_duration):
                # Trigger synaptogenesis
                if np.random.random() < config.get("synaptogenesis_rate", 0.001):
                    brain.triggerSynaptogenesis()
                    synaptogenesis_events += 1
                
                # Trigger pruning
                if np.random.random() < config.get("pruning_rate", 0.00005):
                    brain.triggerPruning()
                    pruning_events += 1
                
                # Track synapse count
                synapse_counts.append(brain.getSynapseCount())
                
                # Step brain
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_synapse_count = sum(synapse_counts) / max(1, len(synapse_counts))
            max_synapse_count = max(synapse_counts) if synapse_counts else 0
            min_synapse_count = min(synapse_counts) if synapse_counts else 0
            
            result["metrics"] = {
                "synaptogenesis_events": synaptogenesis_events,
                "pruning_events": pruning_events,
                "average_synapse_count": avg_synapse_count,
                "max_synapse_count": max_synapse_count,
                "min_synapse_count": min_synapse_count,
                "synapse_count_variance": max_synapse_count - min_synapse_count,
                "test_duration": test_duration
            }
            
            # Validate results
            if (synaptogenesis_events >= 3 and 
                pruning_events >= 1 and
                avg_synapse_count > 0 and
                avg_synapse_count < 10000):
                result["status"] = "PASS"
                result["details"] = "Synaptogenesis integrated successfully with pruning and associative memory"
            else:
                result["details"] = f"Synaptogenesis integration failed: synaptogenesis={synaptogenesis_events}, pruning={pruning_events}, avg_synapses={avg_synapse_count}"
                
        except Exception as e:
            result["details"] = f"Synaptogenesis test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_development_tests(self) -> Dict[str, Any]:
        """Run all development system integration tests"""
        print("\n" + "="*60)
        print("🌱 NLM Phase 6 Development System Integration Tests")
        print("="*60)
        
        results = {
            "development_stages": self.test_development_stages_integration(),
            "synaptogenesis": self.test_synaptogenesis_integration()
        }
        
        # Calculate summary
        total_tests = len(results)
        passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
        failed_tests = total_tests - passed_tests
        
        print("\n" + "="*60)
        print("DEVELOPMENT SYSTEM TEST SUMMARY")
        print("="*60)
        print(f"🧪 Total Tests: {total_tests}")
        print(f"✅ Passed: {passed_tests}")
        print(f"❌ Failed: {failed_tests}")
        
        for test_name, result in results.items():
            status_icon = "✅" if result["status"] == "PASS" else "❌"
            print(f"{status_icon} {test_name.replace('_', ' ').title()}: {result['details']}")
        
        return results


def create_development_test_suite():
    """Create the complete development test suite"""
    print("🌱 Creating Development Integration Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_development_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize tester
    development_tester = DevelopmentSystemIntegrationTester(temp_dir)
    
    # Run all tests
    print("\n" + "="*80)
    print("NLM PHASE 6 DEVELOPMENT INTEGRATION TEST SUITE")
    print("="*80)
    
    results = development_tester.run_all_development_tests()
    
    # Calculate overall results
    total_tests = len(results)
    passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
    failed_tests = total_tests - passed_tests
    success_rate = (passed_tests / total_tests * 100) if total_tests > 0 else 0
    
    print("\n" + "="*80)
    print("OVERALL DEVELOPMENT TEST RESULTS")
    print("="*80)
    print(f"🧪 Total Tests: {total_tests}")
    print(f"✅ Passed: {passed_tests}")
    print(f"❌ Failed: {failed_tests}")
    print(f"📊 Success Rate: {success_rate:.1f}%")
    
    # Save results
    results_file = os.path.join(temp_dir, "nlm_development_test_results.json")
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\n💾 Detailed results saved to: {results_file}")
    
    # Cleanup
    import shutil
    shutil.rmtree(temp_dir)
    
    # Return success status
    return failed_tests == 0

if __name__ == "__main__":
    success = create_development_test_suite()
    sys.exit(0 if success else 1)