#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - Memory System Tests

Comprehensive tests for memory system integration in NLM Phase 6.
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

class MemorySystemIntegrationTester:
    """Comprehensive memory system integration tests for NLM Phase 6"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.test_results = {}
        
    def test_working_memory_integration(self) -> Dict[str, Any]:
        """Test working memory integration with other brain systems"""
        print("🧠 Testing Working Memory Integration...")
        
        result = {
            "test_name": "working_memory_integration",
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
            
            # Create a simple brain configuration with working memory focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 100)
            config.set("enable_working_memory", True)
            config.set("working_memory_size", 50)
            config.set("enable_replay", True)
            
            # Initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test working memory functionality
            # Working memory should maintain persistent activity
            test_duration = 100
            
            working_memory_active = 0
            max_working_memory_activity = 0
            
            for step in range(test_duration):
                # Get working memory status
                wm_active = brain.getWorkingMemoryActiveTraces()
                working_memory_active += wm_active
                max_working_memory_activity = max(max_working_memory_activity, wm_active)
                
                # Simulate some activity
                if step % 10 == 0:
                    brain.addWorkingMemoryEntry(f"trace_{step}", 0.8 + np.random.random() * 0.2)
                
                # Step the brain
                brain.step(step, step * 0.001)
                
                # Check working memory capacity
                if brain.getWorkingMemoryActiveTraces() > 50:
                    print(f"WARNING: Working memory capacity exceeded at step {step}")
                    break
            
            # Calculate metrics
            avg_working_memory_activity = working_memory_active / test_duration
            result["metrics"] = {
                "average_working_memory_activity": avg_working_memory_activity,
                "max_working_memory_activity": max_working_memory_activity,
                "working_memory_capacity": brain.getWorkingMemoryActiveTraces(),
                "test_duration": test_duration
            }
            
            # Validate results
            if (avg_working_memory_activity > 0.1 and 
                max_working_memory_activity <= 50 and
                brain.getWorkingMemoryActiveTraces() < 60):
                result["status"] = "PASS"
                result["details"] = "Working memory integrated successfully with neural processing"
            else:
                result["details"] = f"Working memory integration failed: avg_activity={avg_working_memory_activity}, max={max_working_memory_activity}"
                
        except Exception as e:
            result["details"] = f"Working memory test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_episodic_memory_integration(self) -> Dict[str, Any]:
        """Test episodic memory integration with other brain systems"""
        print("📚 Testing Episodic Memory Integration...")
        
        result = {
            "test_name": "episodic_memory_integration",
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
            
            # Create brain with episodic memory focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 200)
            config.set("enable_episodic_memory", True)
            config.set("episodic_memory_capacity", 20)
            config.set("replay_frequency", 0.1)  # Replay 10% of the time
            config.set("enable_curiosity", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test episodic memory encoding and replay
            test_duration = 200
            episodic_memory_episodes = 0
            replay_events = 0
            
            for step in range(test_duration):
                # Simulate experience encoding
                if step % 15 == 0:
                    # Create a unique experience trace
                    experience_id = f"episode_{step // 15}"
                    brain.encodeEpisodicMemory(experience_id, 0.7 + np.random.random() * 0.3)
                    episodic_memory_episodes += 1
                
                # Check for replay events
                if np.random.random() < config.get("replay_frequency", 0.1):
                    brain.triggerEpisodicReplay()
                    replay_events += 1
                
                # Step brain with episodic memory influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            result["metrics"] = {
                "episodic_memory_episodes": episodic_memory_episodes,
                "replay_events": replay_events,
                "replay_ratio": replay_events / max(1, test_duration // 15),
                "test_duration": test_duration
            }
            
            # Validate results
            if (episodic_memory_episodes >= 10 and 
                replay_events >= 5 and
                replay_events / max(1, episodic_memory_episodes) > 0.2):
                result["status"] = "PASS"
                result["details"] = "Episodic memory integrated successfully with curiosity and replay systems"
            else:
                result["details"] = f"Episodic memory integration failed: episodes={episodic_memory_episodes}, replays={replay_events}"
                
        except Exception as e:
            result["details"] = f"Episodic memory test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def test_associative_memory_integration(self) -> Dict[str, Any]:
        """Test associative memory integration with other brain systems"""
        print("🔗 Testing Associative Memory Integration...")
        
        result = {
            "test_name": "associative_memory_integration",
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
            
            # Create brain with associative memory focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 150)
            config.set("enable_associative_memory", True)
            config.set("associative_memory_strength", 0.8)
            config.set("enable_dopamine", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test associative memory formation and retrieval
            test_duration = 150
            association_formations = 0
            successful_retrievals = 0
            
            # Create some patterns to associate
            patterns = {
                "pattern_a": np.random.random(10) * 0.5,
                "pattern_b": np.random.random(10) * 0.5 + 0.5,
                "pattern_c": np.random.random(10) * 0.3 + 0.7
            }
            
            for step in range(test_duration):
                # Form associations
                if step % 20 == 0:
                    pattern_name = list(patterns.keys())[association_formations % len(patterns)]
                    brain.formAssociativeMemory(pattern_name, patterns[pattern_name], 0.9)
                    association_formations += 1
                
                # Attempt retrieval
                if step % 25 == 0 and association_formations > 0:
                    retrieval_pattern = list(patterns.keys())[np.random.randint(0, association_formations)]
                    retrieval_confidence = brain.retrieveAssociativeMemory(retrieval_pattern)
                    if retrieval_confidence > 0.5:
                        successful_retrievals += 1
                
                # Step brain with dopamine modulation
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            result["metrics"] = {
                "association_formations": association_formations,
                "successful_retrievals": successful_retrievals,
                "retrieval_success_rate": successful_retrievals / max(1, test_duration // 25),
                "test_duration": test_duration
            }
            
            # Validate results
            if (association_formations >= 5 and 
                successful_retrievals >= 3 and
                successful_retrievals / max(1, association_formations) > 0.4):
                result["status"] = "PASS"
                result["details"] = "Associative memory integrated successfully with dopamine modulation"
            else:
                result["details"] = f"Associative memory integration failed: formations={association_formations}, retrievals={successful_retrievals}"
                
        except Exception as e:
            result["details"] = f"Associative memory test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_memory_tests(self) -> Dict[str, Any]:
        """Run all memory system integration tests"""
        print("\n" + "="*60)
        print("🧠 NLM Phase 6 Memory System Integration Tests")
        print("="*60)
        
        results = {
            "working_memory": self.test_working_memory_integration(),
            "episodic_memory": self.test_episodic_memory_integration(),
            "associative_memory": self.test_associative_memory_integration()
        }
        
        # Calculate summary
        total_tests = len(results)
        passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
        failed_tests = total_tests - passed_tests
        
        print("\n" + "="*60)
        print("MEMORY SYSTEM TEST SUMMARY")
        print("="*60)
        print(f"🧪 Total Tests: {total_tests}")
        print(f"✅ Passed: {passed_tests}")
        print(f"❌ Failed: {failed_tests}")
        
        for test_name, result in results.items():
            status_icon = "✅" if result["status"] == "PASS" else "❌"
            print(f"{status_icon} {test_name.replace('_', ' ').title()}: {result['details']}")
        
        return results


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


class PredictionSystemIntegrationTester:
    """Comprehensive prediction system integration tests for NLM Phase 6"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.test_results = {}
        
    def test_prediction_system_integration(self) -> Dict[str, Any]:
        """Test prediction system integration with other brain systems"""
        print("🔮 Testing Prediction System Integration...")
        
        result = {
            "test_name": "prediction_system_integration",
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
            
            # Create brain with prediction focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 200)
            config.set("enable_prediction", True)
            config.set("prediction_confidence_threshold", 0.6)
            config.set("enable_prediction_error", True)
            config.set("enable_working_memory", True)
            config.set("enable_curiosity", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test prediction system with temporal sequence learning
            test_duration = 200
            predictions_made = 0
            prediction_errors = 0
            prediction_confidences = []
            
            for step in range(test_duration):
                # Make predictions
                if step % 15 == 0:
                    prediction = brain.makePrediction()
                    predictions_made += 1
                    prediction_confidences.append(prediction.get("confidence", 0.0))
                    
                    # Simulate prediction error
                    if np.random.random() < 0.2:  # 20% error rate
                        error = brain.getPredictionError()
                        prediction_errors += 1
                
                # Step brain with prediction influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_confidence = sum(prediction_confidences) / max(1, len(prediction_confidences))
            result["metrics"] = {
                "predictions_made": predictions_made,
                "prediction_errors": prediction_errors,
                "error_rate": prediction_errors / max(1, predictions_made),
                "average_prediction_confidence": avg_confidence,
                "min_prediction_confidence": min(prediction_confidences) if prediction_confidences else 0,
                "max_prediction_confidence": max(prediction_confidences) if prediction_confidences else 0,
                "test_duration": test_duration
            }
            
            # Validate results
            if (predictions_made >= 10 and 
                prediction_errors >= 2 and
                avg_confidence > 0.5 and
                avg_confidence < 1.0):
                result["status"] = "PASS"
                result["details"] = "Prediction system integrated successfully with curiosity and working memory systems"
            else:
                result["details"] = f"Prediction system integration failed: predictions={predictions_made}, errors={prediction_errors}, avg_confidence={avg_confidence}"
                
        except Exception as e:
            result["details"] = f"Prediction system test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_prediction_tests(self) -> Dict[str, Any]:
        """Run all prediction system integration tests"""
        print("\n" + "="*60)
        print("🔮 NLM Phase 6 Prediction System Integration Tests")
        print("="*60)
        
        results = {
            "prediction_system": self.test_prediction_system_integration()
        }
        
        # Calculate summary
        total_tests = len(results)
        passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
        failed_tests = total_tests - passed_tests
        
        print("\n" + "="*60)
        print("PREDICTION SYSTEM TEST SUMMARY")
        print("="*60)
        print(f"🧪 Total Tests: {total_tests}")
        print(f"✅ Passed: {passed_tests}")
        print(f"❌ Failed: {failed_tests}")
        
        for test_name, result in results.items():
            status_icon = "✅" if result["status"] == "PASS" else "❌"
            print(f"{status_icon} {test_name.replace('_', ' ').title()}: {result['details']}")
        
        return results


def create_nlm_test_suite():
    """Create the complete NLM Phase 6 test suite"""
    print("🚀 Creating NLM Phase 6 Integration Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_phase6_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize test suites
    memory_tester = MemorySystemIntegrationTester(temp_dir)
    neuromodulation_tester = NeuromodulationIntegrationTester(temp_dir)
    prediction_tester = PredictionSystemIntegrationTester(temp_dir)
    
    # Run all tests
    print("\n" + "="*80)
    print("NLM PHASE 6 INTEGRATION TEST SUITE")
    print("="*80)
    
    memory_results = memory_tester.run_all_memory_tests()
    neuromodulation_results = neuromodulation_tester.run_all_neuromodulation_tests()
    prediction_results = prediction_tester.run_all_prediction_tests()
    
    # Calculate overall results
    all_results = {
        "memory_system": memory_results,
        "neuromodulation_system": neuromodulation_results,
        "prediction_system": prediction_results
    }
    
    total_tests = (
        len(memory_results) + 
        len(neuromodulation_results) + 
        len(prediction_results)
    )
    
    passed_tests = (
        sum(1 for result in memory_results.values() if result["status"] == "PASS") +
        sum(1 for result in neuromodulation_results.values() if result["status"] == "PASS") +
        sum(1 for result in prediction_results.values() if result["status"] == "PASS")
    )
    
    failed_tests = total_tests - passed_tests
    success_rate = (passed_tests / total_tests * 100) if total_tests > 0 else 0
    
    print("\n" + "="*80)
    print("OVERALL TEST RESULTS")
    print("="*80)
    print(f"🧪 Total Tests: {total_tests}")
    print(f"✅ Passed: {passed_tests}")
    print(f"❌ Failed: {failed_tests}")
    print(f"📊 Success Rate: {success_rate:.1f}%")
    
    # Save results
    results_file = os.path.join(temp_dir, "nlm_phase6_test_results.json")
    with open(results_file, 'w') as f:
        json.dump(all_results, f, indent=2)
    
    print(f"\n💾 Detailed results saved to: {results_file}")
    
    # Cleanup
    import shutil
    shutil.rmtree(temp_dir)
    
    # Return success status
    return failed_tests == 0

if __name__ == "__main__":
    success = create_nlm_test_suite()
    sys.exit(0 if success else 1)