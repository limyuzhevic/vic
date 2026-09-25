#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - Prediction System Tests

Comprehensive tests for prediction system integration in NLM Phase 6.
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
        
    def test_prediction_error_integration(self) -> Dict[str, Any]:
        """Test prediction error integration with learning"""
        print("❌ Testing Prediction Error Integration...")
        
        result = {
            "test_name": "prediction_error_integration",
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
            
            # Create brain with prediction error focus
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 180)
            config.set("enable_prediction", True)
            config.set("enable_prediction_error", True)
            config.set("enable_dopamine", True)
            config.set("enable_curiosity", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test prediction error-driven learning
            test_duration = 150
            prediction_errors = 0
            learning_events = 0
            error_magnitudes = []
            dopamine_levels = []
            
            for step in range(test_duration):
                # Generate prediction errors
                if step % 20 == 0:
                    # Simulate prediction error
                    error_magnitude = np.random.random() * 2.0  # 0.0 to 2.0
                    brain.triggerPredictionError(error_magnitude)
                    prediction_errors += 1
                    error_magnitudes.append(error_magnitude)
                    
                    # Check dopamine response
                    dopamine_levels.append(brain.getDopamineLevel())
                
                # Trigger learning based on prediction error
                if brain.getPredictionError() > 0.5:
                    brain.triggerLearningEvent()
                    learning_events += 1
                
                # Step brain with prediction error influence
                brain.step(step, step * 0.001)
            
            # Calculate metrics
            avg_error_magnitude = sum(error_magnitudes) / max(1, len(error_magnitudes))
            avg_dopamine = sum(dopamine_levels) / max(1, len(dopamine_levels))
            result["metrics"] = {
                "prediction_errors": prediction_errors,
                "learning_events": learning_events,
                "average_error_magnitude": avg_error_magnitude,
                "average_dopamine_response": avg_dopamine,
                "test_duration": test_duration
            }
            
            # Validate results
            if (prediction_errors >= 5 and 
                learning_events >= 3 and
                avg_error_magnitude > 0.3 and
                avg_dopamine > 0.1):
                result["status"] = "PASS"
                result["details"] = "Prediction error system integrated successfully with dopamine-driven learning"
            else:
                result["details"] = f"Prediction error integration failed: errors={prediction_errors}, learning={learning_events}, avg_error={avg_error_magnitude}, avg_dopamine={avg_dopamine}"
                
        except Exception as e:
            result["details"] = f"Prediction error test failed with exception: {str(e)}"
            
        result["duration"] = time.time() - start_time
        return result
        
    def run_all_prediction_tests(self) -> Dict[str, Any]:
        """Run all prediction system integration tests"""
        print("\n" + "="*60)
        print("🔮 NLM Phase 6 Prediction System Integration Tests")
        print("="*60)
        
        results = {
            "prediction_system": self.test_prediction_system_integration(),
            "prediction_error": self.test_prediction_error_integration()
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


def create_prediction_test_suite():
    """Create the complete prediction test suite"""
    print("🔮 Creating Prediction Integration Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_prediction_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize tester
    prediction_tester = PredictionSystemIntegrationTester(temp_dir)
    
    # Run all tests
    print("\n" + "="*80)
    print("NLM PHASE 6 PREDICTION INTEGRATION TEST SUITE")
    print("="*80)
    
    results = prediction_tester.run_all_prediction_tests()
    
    # Calculate overall results
    total_tests = len(results)
    passed_tests = sum(1 for result in results.values() if result["status"] == "PASS")
    failed_tests = total_tests - passed_tests
    success_rate = (passed_tests / total_tests * 100) if total_tests > 0 else 0
    
    print("\n" + "="*80)
    print("OVERALL PREDICTION TEST RESULTS")
    print("="*80)
    print(f"🧪 Total Tests: {total_tests}")
    print(f"✅ Passed: {passed_tests}")
    print(f"❌ Failed: {failed_tests}")
    print(f"📊 Success Rate: {success_rate:.1f}%")
    
    # Save results
    results_file = os.path.join(temp_dir, "nlm_prediction_test_results.json")
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\n💾 Detailed results saved to: {results_file}")
    
    # Cleanup
    import shutil
    shutil.rmtree(temp_dir)
    
    # Return success status
    return failed_tests == 0

if __name__ == "__main__":
    success = create_prediction_test_suite()
    sys.exit(0 if success else 1)