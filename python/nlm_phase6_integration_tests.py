#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - System Integration Tests

Comprehensive tests for all 6 brain systems working together in NLM Phase 6.
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

class SystemIntegrationTester:
    """Comprehensive system integration tests for NLM Phase 6"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.test_results = {}
        
    def test_all_systems_integration(self) -> Dict[str, Any]:
        """Test all 6 brain systems working together"""
        print("🔗 Testing All Systems Integration...")
        
        result = {
            "test_name": "all_systems_integration",
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
            
            # Create comprehensive brain configuration with all systems
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 500)
            config.set("region_count", 3)
            
            # Enable all Phase 6 systems
            config.set("enable_working_memory", True)
            config.set("enable_episodic_memory", True)
            config.set("enable_associative_memory", True)
            config.set("enable_dopamine", True)
            config.set("enable_curiosity", True)
            config.set("enable_novelty", True)
            config.set("enable_prediction", True)
            config.set("enable_prediction_error", True)
            config.set("enable_attention", True)
            config.set("enable_planning", True)
            config.set("enable_concept_formation", True)
            config.set("enable_development", True)
            config.set("enable_replay", True)
            config.set("enable_checkpointing", True)
            
            # Set system interaction parameters
            config.set("working_memory_size", 100)
            config.set("episodic_memory_capacity", 50)
            config.set("curiosity_threshold", 0.5)
            config.set("novelty_sensitivity", 1.0)
            config.set("prediction_confidence_threshold", 0.6)
            config.set("development_stage_progression", True)
            config.set("checkpoint_interval", 100)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test all systems integration
            test_duration = 500
            
            # Track system activities
            system_activities = {
                "working_memory": 0,
                "episodic_memory": 0,
                "associative_memory": 0,
                "dopamine_modulation": 0,
                "curiosity_driven": 0,
                "novelty_detection": 0,
                "prediction_making": 0,
                "attention_shifts": 0,
                "planning_cycles": 0,
                "concept_formation": 0,
                "development_stages": 0,
                "replay_events": 0,
                "checkpoint_events": 0
            }
            
            # Track system coherence metrics
            system_coherence_metrics = {
                "cross_system_interactions": 0,
                "system_feedback_loops": 0,
                "emergent_behavior": 0,
                "system_harmony_score": 0.0
            }
            
            for step in range(test_duration):
                # Working memory operations
                if step % 20 == 0:
                    brain.addWorkingMemoryEntry(f"trace_{step}", 0.7 + np.random.random() * 0.3)
                    system_activities["working_memory"] += 1
                
                # Episodic memory operations
                if step % 30 == 0:
                    brain.encodeEpisodicMemory(f"episode_{step // 30}", 0.8 + np.random.random() * 0.2)
                    system_activities["episodic_memory"] += 1
                
                # Associative memory operations
                if step % 25 == 0:
                    brain.formAssociativeMemory(f"association_{step // 25}", np.random.random(5))
                    system_activities["associative_memory"] += 1
                
                # Neuromodulation operations
                if brain.getCuriosityLevel() > 0.3:
                    brain.triggerExplorationBehavior()
                    system_activities["curiosity_driven"] += 1
                
                if brain.detectNovelty() > 0.5:
                    system_activities["novelty_detection"] += 1
                
                if np.random.random() < 0.1:  # 10% chance of reward
                    brain.applyRewardModulation(1.0, step * 0.001)
                    system_activities["dopamine_modulation"] += 1
                
                # Prediction system operations
                if step % 15 == 0:
                    brain.makePrediction()
                    system_activities["prediction_making"] += 1
                
                # Attention system operations
                if step % 40 == 0:
                    brain.getAttentionState()
                    system_activities["attention_shifts"] += 1
                
                # Planning system operations
                if step % 50 == 0:
                    brain.evaluateActionSequence()
                    system_activities["planning_cycles"] += 1
                
                # Concept formation
                if np.random.random() < 0.05:  # 5% chance
                    brain.formConcept()
                    system_activities["concept_formation"] += 1
                
                # Development system
                if step % 100 == 0:
                    brain.updateDevelopment(step * 0.001)
                    system_activities["development_stages"] += 1
                
                # Replay system
                if np.random.random() < 0.1:  # 10% chance of replay
                    brain.triggerEpisodicReplay()
                    system_activities["replay_events"] += 1
                
                # Checkpoint system
                if step % config.get("checkpoint_interval", 100) == 0:
                    brain.saveCheckpoint()
                    system_activities["checkpoint_events"] += 1
                
                # Cross-system interactions
                if (brain.getWorkingMemoryActiveTraces() > 50 and 
                    brain.getCuriosityLevel() > 0.2 and
                    brain.getPredictionError() > 0.3):
                    system_coherence_metrics["cross_system_interactions"] += 1
                
                if (brain.getDopamineLevel() > 0.5 and
                    brain.getAttentionFocus() > 0.7 and
                    brain.getDevelopmentStage() > 0.3):
                    system_coherence_metrics["system_feedback_loops"] += 1
                
                # Step the brain
                brain.step(step, step * 0.001)
            
            # Calculate final metrics
            total_system_activities = sum(system_activities.values())
            total_coherence_metrics = sum(system_coherence_metrics.values())
            avg_harmony_score = (system_coherence_metrics.get("system_harmony_score", 0.0) +
                                (1.0 if total_system_activities > 0 else 0.0))
            
            result["metrics"] = {
                "total_system_activities": total_system_activities,
                "average_activity_per_system": total_system_activities / len(system_activities),
                "system_activities_breakdown": system_activities,
                "cross_system_interactions": system_coherence_metrics["cross_system_interactions"],
                "system_feedback_loops": system_coherence_metrics["system_feedback_loops"],
                "system_harmony_score": avg_harmony_score,
                "test_duration": test_duration
            }
            
            # Validate results - all systems should be active and coherent
            validation_criteria = {
                "working_memory": system_activities["working_memory"] >= 10,
                "episodic_memory": system_activities["episodic_memory"] >= 8,
                "associative_memory": system_activities["associative_memory"] >= 8,
                "dopamine_modulation": system_activities["dopamine_modulation"] >= 5,
                "curiosity_driven": system_activities["curiosity_driven"] >= 10,
                "novelty_detection": system_activities["novelty_detection"] >= 5,
                "prediction_making": system_activities["prediction_making"] >= 10,
                "attention_shifts": system_activities["attention_shifts"] >= 5,
                "planning_cycles": system_activities["planning_cycles"] >= 5,
                "concept_formation": system_activities["concept_formation"] >= 3,
                "development_stages": system_activities["development_stages"] >= 2,
                "replay_events": system_activities["replay_events"] >= 3,
                "checkpoint_events": system_activities["checkpoint_events"] >= 1
            }
            
            passed_criteria = sum(1 for criterion in validation_criteria.values() if criterion)
            all_passed = passed_criteria == len(validation_criteria)
            
            if all_passed and avg_harmony_score > 0.5:
                result["status"] = "PASS"
                result["details"] = f"All systems integrated successfully with {passed_criteria}/{len(validation_criteria)} criteria passed and harmony score {avg_harmony_score:.2f}"
            else:
                result["details"] = f"System integration incomplete: {passed_criteria}/{len(validation_criteria)} criteria passed, harmony score {avg_harmony_score:.2f}"
                
        except Exception as e:
            result["details"] = f"All systems integration test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_system_cohesion(self) -> Dict[str, Any]:
        """Test system cohesion metrics"""
        print("🔗 Testing System Cohesion Metrics...")
        
        result = {
            "test_name": "system_cohesion",
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
            
            # Create brain for cohesion testing
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 300)
            config.set("region_count", 2)
            
            # Enable all systems for cohesion testing
            for system in ["working_memory", "episodic_memory", "associative_memory",
                          "dopamine", "curiosity", "novelty", "prediction",
                          "prediction_error", "attention", "planning", "concept_formation",
                          "development", "replay"]:
                config.set(f"enable_{system}", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test system cohesion over extended period
            test_duration = 400
            
            cohesion_metrics = {
                "system_synchronization": 0,
                "information_flow_efficiency": 0,
                "resource_sharing_effectiveness": 0,
                "emergent_coherence": 0,
                "adaptation_capability": 0,
                "integration_quality": 0.0
            }
            
            for step in range(test_duration):
                # Measure system synchronization
                if step % 50 == 0:
                    # Get system states
                    wm_activity = brain.getWorkingMemoryActiveTraces()
                    dopamine_level = brain.getDopamineLevel()
                    curiosity_level = brain.getCuriosityLevel()
                    attention_entropy = brain.getAttentionState().get("entropy", 0.0)
                    
                    # Calculate synchronization score
                    system_states = [wm_activity / 100.0, dopamine_level / 2.0, 
                                   curiosity_level / 1.5, 1.0 - (attention_entropy / 2.0)]
                    avg_system_state = sum(system_states) / len(system_states)
                    std_system_state = np.std(system_states)
                    
                    if std_system_state < 0.3:  # Low variance indicates good synchronization
                        cohesion_metrics["system_synchronization"] += 1
                
                # Measure information flow efficiency
                if step % 75 == 0:
                    # Simulate information flow
                    prediction = brain.makePrediction()
                    concept = brain.formConcept()
                    memory_entry = brain.addWorkingMemoryEntry(f"cohesion_trace_{step}", 0.8)
                    
                    # Check for efficient flow
                    if (prediction.get("confidence", 0.0) > 0.6 and
                        concept.get("confidence", 0.0) > 0.5 and
                        memory_entry > 0.7):
                        cohesion_metrics["information_flow_efficiency"] += 1
                
                # Measure resource sharing effectiveness
                if step % 100 == 0:
                    # Check resource sharing across systems
                    total_resources = (brain.getWorkingMemoryCapacity() +
                                    brain.getEpisodicMemoryCapacity() +
                                    brain.getAssociativeMemoryCapacity())
                    
                    if total_resources > 0:
                        resource_utilization = (brain.getWorkingMemoryActiveTraces() +
                                              brain.getEpisodicMemoryEpisodes() +
                                              brain.getAssociativeMemoryStrength()) / total_resources
                        
                        if resource_utilization > 0.3 and resource_utilization < 0.8:
                            cohesion_metrics["resource_sharing_effectiveness"] += 1
                
                # Step brain
                brain.step(step, step * 0.001)
            
            # Calculate final cohesion metrics
            total_measures = sum(1 for v in cohesion_metrics.items() if isinstance(v, int))
            if total_measures > 0:
                integration_quality = (
                    cohesion_metrics["system_synchronization"] / max(1, test_duration // 50) +
                    cohesion_metrics["information_flow_efficiency"] / max(1, test_duration // 75) +
                    cohesion_metrics["resource_sharing_effectiveness"] / max(1, test_duration // 100)
                ) / 3.0
                
                result["metrics"] = {
                    "system_synchronization": cohesion_metrics["system_synchronization"],
                    "information_flow_efficiency": cohesion_metrics["information_flow_efficiency"],
                    "resource_sharing_effectiveness": cohesion_metrics["resource_sharing_effectiveness"],
                    "emergent_coherence": cohesion_metrics.get("emergent_coherence", 0),
                    "adaptation_capability": cohesion_metrics.get("adaptation_capability", 0),
                    "integration_quality": integration_quality,
                    "cohesion_score": (cohesion_metrics["system_synchronization"] +
                                      cohesion_metrics["information_flow_efficiency"] +
                                      cohesion_metrics["resource_sharing_effectiveness"]) / total_measures,
                    "test_duration": test_duration
                }
                
                # Validate cohesion results
                if (integration_quality > 0.4 and
                    cohesion_metrics["system_synchronization"] >= 2 and
                    cohesion_metrics["information_flow_efficiency"] >= 2):
                    result["status"] = "PASS"
                    result["details"] = f"System cohesion excellent with integration quality {integration_quality:.2f} and coherence score {(cohesion_metrics['system_synchronization'] + cohesion_metrics['information_flow_efficiency'] + cohesion_metrics['resource_sharing_effectiveness']) / total_measures:.2f}"
                else:
                    result["details"] = f"System cohesion inadequate: integration_quality={integration_quality:.2f}, sync={cohesion_metrics['system_synchronization']}, flow={cohesion_metrics['information_flow_efficiency']}"
            else:
                result["details"] = "No cohesion metrics calculated"
                
        except Exception as e:
            result["details"] = f"System cohesion test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_integration_tests(self) -> Dict[str, Any]:
        """Run all system integration tests"""
        print("\n" + "="*60)
        print("🔗 NLM Phase 6 System Integration Tests")
        print("="*60)
        
        results = {
            "all_systems": self.test_all_systems_integration(),
            "system_cohesion": self.test_system_cohesion()
        }
        
        # Calculate summary
        total_tests = len(results)
        passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
        failed_tests = total_tests - passed_tests
        
        print("\n" + "="*60)
        print("SYSTEM INTEGRATION TEST SUMMARY")
        print("="*60)
        print(f"🧪 Total Tests: {total_tests}")
        print(f"✅ Passed: {passed_tests}")
        print(f"❌ Failed: {failed_tests}")
        
        for test_name, result in results.items():
            status_icon = "✅" if result["status"] == "PASS" else "❌"
            print(f"{status_icon} {test_name.replace('_', ' ').title()}: {result['details']}")
        
        return results


def create_integration_test_suite():
    """Create the complete integration test suite"""
    print("🔗 Creating System Integration Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_integration_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize tester
    integration_tester = SystemIntegrationTester(temp_dir)
    
    # Run all tests
    print("\n" + "="*80)
    print("NLM PHASE 6 SYSTEM INTEGRATION TEST SUITE")
    print("="*80)
    
    results = integration_tester.run_all_integration_tests()
    
    # Calculate overall results
    total_tests = len(results)
    passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
    failed_tests = total_tests - passed_tests
    success_rate = (passed_tests / total_tests * 100) if total_tests > 0 else 0
    
    print("\n" + "="*80)
    print("OVERALL INTEGRATION TEST RESULTS")
    print("="*80)
    print(f"🧪 Total Tests: {total_tests}")
    print(f"✅ Passed: {passed_tests}")
    print(f"❌ Failed: {failed_tests}")
    print(f"📊 Success Rate: {success_rate:.1f}%")
    
    # Save results
    results_file = os.path.join(temp_dir, "nlm_integration_test_results.json")
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\n💾 Detailed results saved to: {results_file}")
    
    # Cleanup
    import shutil
    shutil.rmtree(temp_dir)
    
    # Return success status
    return failed_tests == 0

if __name__ == "__main__":
    success = create_integration_test_suite()
    sys.exit(0 if success else 1)