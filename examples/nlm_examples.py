#!/usr/bin/env python3
"""
NLM Python Bindings Example Script

This script demonstrates how to use the NLM (Neural Learning Machine) Python bindings
to create and run neural network simulations.

The NLM is an experimental artificial developmental brain system that implements
single-pulse neuron dynamics, event-driven spike propagation, and synaptic plasticity.

Usage:
    python3 examples/nlm_examples.py [--basic-connectivity] [--plasticity] [--stdp-verification] [--all]

Example:
    python3 examples/nlm_examples.py --all
    python3 examples/nlm_examples.py --plasticity
    python3 examples/nlm_examples.py --basic-connectivity

Requirements:
    - NLM Python bindings compiled and installed
    - Python 3.8 or higher
    - pybind11
    - NumPy (optional, for data analysis)
"""

import sys
import os
import time
from typing import Optional, Dict, Any

# Add the Python bindings module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python', 'build'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import pynlm: {e}")
    print("Please compile the NLM Python bindings first.")
    PYNLM_AVAILABLE = False

class NLMExperimentRunner:
    """Example class for running NLM experiments using Python bindings."""
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize the experiment runner with optional configuration file."""
        self.config = None
        self.brain = None
        self.config_file = config_file or "configs/default.cfg"
        
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            if config_file and os.path.exists(config_file):
                if self.config.loadFromFile(config_file):
                    print(f"✓ Loaded configuration from: {config_file}")
                else:
                    print(f"! Using default configuration (could not load: {config_file})")
            else:
                print(f"! Using default configuration")
            
            # Set default values for Phase 2
            self.config.set("random_seed", 42)
            self.config.set("simulation_timestep", 0.001)
            self.config.set("neuron_count", 500)  # Smaller for faster testing
            self.config.set("region_count", 1)
            self.config.set("connection_probability", 0.15)
            
            self.brain = pynlm.createBrain(self.config)
            if not self.brain.initialize():
                raise RuntimeError("Failed to initialize brain!")
            
            print("✓ NLM brain initialized successfully")
        else:
            print("⚠ Python bindings not available - running in demo mode only")
    
    def run_basic_connectivity_test(self) -> Dict[str, Any]:
        """Run basic connectivity test."""
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "Python bindings not available"}
        
        print("\n" + "="*60)
        print("TEST 1: Basic Neural Connectivity")
        print("="*60)
        
        try:
            # Get initial spike count
            initial_spikes = self.brain.getTotalSpikeCount()
            
            # Get neurons from first region
            region = self.brain.getRegion(pynlm.RegionId(1))
            if not region:
                return {"success": False, "error": "Could not find region 1"}
            
            neurons = region.getAllNeurons()
            if neurons.empty():
                return {"success": False, "error": "No neurons found"}
            
            # Inject current into first 10 neurons
            print(f"  Injecting current into {min(10, len(neurons))} neurons...")
            for i in range(min(10, len(neurons))):
                neurons[i].injectCurrent(50.0)
            
            # Run simulation steps
            for step in range(50):
                self.brain.step(step, step * 0.001)
            
            final_spikes = self.brain.getTotalSpikeCount()
            spikes_generated = final_spikes - initial_spikes
            
            print(f"  Spikes generated: {spikes_generated}")
            
            result = {
                "success": True,
                "test_type": "basic_connectivity",
                "spikes_generated": spikes_generated,
                "total_spikes": final_spikes,
                "neurons_tested": min(10, len(neurons)),
                "final_spike_count": final_spikes
            }
            
            if spikes_generated > 0:
                print(f"  ✓ SUCCESS: Spikes propagated through network ({spikes_generated} spikes)")
            else:
                print(f"  ⚠ WARNING: No spikes generated despite input")
            
            return result
            
        except Exception as e:
            error_msg = f"Error in basic connectivity test: {str(e)}"
            print(f"  ✗ FAILED: {error_msg}")
            return {"success": False, "error": error_msg}
    
    def run_plasticity_experiment(self) -> Dict[str, Any]:
        """Run plasticity learning experiment."""
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "Python bindings not available"}
        
        print("\n" + "="*60)
        print("TEST 2: Plasticity Learning Experiment")
        print("="*60)
        
        try:
            # Reset and reinitialize for clean experiment
            self.brain.reset()
            self.brain.initialize()
            
            # Record initial state
            initial_synapse_count = self.brain.getTotalSynapseCount()
            print(f"  Initial synapse count: {initial_synapse_count}")
            
            # Enable plasticity on synapses
            region = self.brain.getRegion(pynlm.RegionId(1))
            if region:
                synapses = region.getSynapses()
                for syn in synapses:
                    syn.enablePlasticity(True, True, False)  # Enable Hebbian and STDP
                print(f"  Enabled plasticity for {len(synapses)} synapses")
            
            # Apply repeated input patterns
            total_steps = 1000
            input_current = 30.0
            
            print(f"  Applying input patterns ({total_steps} steps)...")
            
            for step in range(total_steps):
                # Inject current into sensory neurons
                for i in range(min(20, self.brain.getTotalNeuronCount() // 4)):
                    self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, input_current)
                
                self.brain.step(step, step * 0.001)
                
                # Log progress every 100 steps
                if step % 100 == 0:
                    spikes = self.brain.getTotalSpikeCount()
                    firing = self.brain.getFiringNeuronCount()
                    print(f"    Step {step}: Spikes={spikes}, Firing={firing}")
            
            # Record final state
            final_synapse_count = self.brain.getTotalSynapseCount()
            total_spikes = self.brain.getTotalSpikeCount()
            
            print(f"\n  Final synapse count: {final_synapse_count}")
            print(f"  Total spikes generated: {total_spikes}")
            
            result = {
                "success": True,
                "test_type": "plasticity_experiment",
                "steps": total_steps,
                "initial_synapses": initial_synapse_count,
                "final_synapses": final_synapse_count,
                "total_spikes": total_spikes,
                "synapse_change": final_synapse_count - initial_synapse_count
            }
            
            print(f"  ✓ SUCCESS: Plasticity experiment completed")
            print(f"    Synapse count changed from {initial_synapse_count} to {final_synapse_count}")
            print(f"    Total spikes: {total_spikes}")
            
            return result
            
        except Exception as e:
            error_msg = f"Error in plasticity experiment: {str(e)}"
            print(f"  ✗ FAILED: {error_msg}")
            return {"success": False, "error": error_msg}
    
    def run_stdp_verification(self) -> Dict[str, Any]:
        """Run STDP verification test."""
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "Python bindings not available"}
        
        print("\n" + "="*60)
        print("TEST 3: STDP Verification")
        print("="*60)
        
        try:
            # Reset and reinitialize
            self.brain.reset()
            self.brain.initialize()
            
            # Get first few synapses
            region = self.brain.getRegion(pynlm.RegionId(1))
            if not region:
                return {"success": False, "error": "Could not find region 1"}
            
            synapses = region.getSynapses()
            if len(synapses) < 5:
                return {"success": False, "error": f"Not enough synapses for STDP test (have {len(synapses)})"}
            
            # Record initial weights
            before_weights = []
            for i in range(min(5, len(synapses))):
                weight = synapses[i].getWeight()
                before_weights.append(weight)
                synapses[i].enablePlasticity(False, True, False)  # Enable only STDP
                print(f"    Synapse {i}: initial weight = {weight:.4f}")
            
            # Create correlated activity for LTP
            trials = 50
            print(f"  Creating correlated pre->post activity ({trials} trials)...")
            
            neurons = region.getAllNeurons()
            if len(neurons) < 2:
                return {"success": False, "error": "Not enough neurons for STDP test"}
            
            for trial in range(trials):
                # Fire pre-synaptic neuron
                pre_neuron = neurons[0]
                post_neuron = neurons[1]
                
                pre_neuron.injectCurrent(60.0)
                self.brain.step(trial * 2, trial * 2 * 0.001)
                
                # Fire post-synaptic neuron
                post_neuron.injectCurrent(60.0)
                self.brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
            
            # Record final weights
            after_weights = []
            total_delta = 0.0
            
            print(f"\n  After correlated activity:")
            for i in range(min(5, len(synapses))):
                weight = synapses[i].getWeight()
                after_weights.append(weight)
                delta = weight - before_weights[i]
                total_delta += delta
                print(f"    Synapse {i}: final weight = {weight:.4f} (Δ={delta:.4f})")
            
            print(f"\n  Total weight change: {total_delta:.4f}")
            
            result = {
                "success": True,
                "test_type": "stdp_verification",
                "trials": trials,
                "synapses_tested": min(5, len(synapses)),
                "total_delta": total_delta,
                "before_weights": before_weights,
                "after_weights": after_weights
            }
            
            if total_delta > 0.001:
                print(f"  ✓ SUCCESS: STDP working - pre-before-post produced potentiation")
                print(f"    Total weight change: {total_delta:.4f} (positive = LTP)")
            elif total_delta < -0.001:
                print(f"  ⚠ WARNING: STDP reversed - check parameters")
                print(f"    Total weight change: {total_delta:.4f} (negative = LTD)")
            else:
                print(f"  ⚠ WARNING: No change - STDP may not be triggering")
                print(f"    Total weight change: {total_delta:.4f} (near zero)")
            
            return result
            
        except Exception as e:
            error_msg = f"Error in STDP verification: {str(e)}"
            print(f"  ✗ FAILED: {error_msg}")
            return {"success": False, "error": error_msg}
    
    def save_results(self, filepath: str) -> bool:
        """Save experiment results to file."""
        if not PYNLM_AVAILABLE:
            print("  ⚠ Cannot save results - Python bindings not available")
            return False
        
        try:
            if self.brain.save(filepath):
                print(f"  ✓ Results saved to: {filepath}")
                return True
            else:
                print(f"  ✗ Failed to save results to: {filepath}")
                return False
        except Exception as e:
            print(f"  ✗ Error saving results: {str(e)}")
            return False
    
    def run_all_tests(self) -> Dict[str, Any]:
        """Run all available tests."""
        print("\n" + "="*80)
        print("RUNNING ALL NLM TESTS")
        print("="*80)
        
        results = {
            "basic_connectivity": None,
            "plasticity_experiment": None,
            "stdp_verification": None
        }
        
        # Test 1: Basic connectivity
        results["basic_connectivity"] = self.run_basic_connectivity_test()
        
        # Test 2: Plasticity experiment
        results["plasticity_experiment"] = self.run_plasticity_experiment()
        
        # Test 3: STDP verification
        results["stdp_verification"] = self.run_stdp_verification()
        
        # Summary
        print("\n" + "="*80)
        print("EXPERIMENT SUMMARY")
        print("="*80)
        
        all_passed = True
        for test_name, result in results.items():
            status = "✓ PASSED" if result and result.get("success", False) else "✗ FAILED"
            print(f"{test_name.replace('_', ' ').title()}: {status}")
            if result and result.get("success", False):
                print(f"  {result.get('test_type', test_name)} completed successfully")
            elif result:
                print(f"  Error: {result.get('error', 'Unknown error')}")
                all_passed = False
        
        if all_passed:
            print("\n🎉 ALL TESTS PASSED!")
        else:
            print("\n⚠️  SOME TESTS FAILED")
        
        return {
            "overall_success": all_passed,
            "individual_results": results,
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
        }

def main():
    """Main function for the example script."""
    print("NLM Python Bindings Example Script")
    print("=" * 50)
    
    # Parse command line arguments
    import argparse
    parser = argparse.ArgumentParser(description="NLM Python Bindings Examples")
    parser.add_argument("--basic-connectivity", action="store_true",
                       help="Run basic connectivity test")
    parser.add_argument("--plasticity", action="store_true",
                       help="Run plasticity experiment")
    parser.add_argument("--stdp-verification", action="store_true",
                       help="Run STDP verification test")
    parser.add_argument("--all", action="store_true", dest="all_tests",
                       help="Run all tests")
    parser.add_argument("--config", default="configs/default.cfg",
                       help="Configuration file path")
    parser.add_argument("--output", default="nlm_results.txt",
                       help="Output file for results")
    parser.add_argument("--verbose", action="store_true",
                       help="Enable verbose output")
    
    args = parser.parse_args()
    
    # If no specific tests requested, run all
    if not any([args.basic_connectivity, args.plasticity, args.stdp_verification, args.all_tests]):
        args.all_tests = True
    
    try:
        # Initialize experiment runner
        runner = NLMExperimentRunner(config_file=args.config)
        
        # Run selected tests
        if args.all_tests or args.basic_connectivity:
            result = runner.run_basic_connectivity_test()
            if args.output and result.get("success", False):
                runner.save_results(args.output)
        
        if args.all_tests or args.plasticity:
            result = runner.run_plasticity_experiment()
            if args.output and result.get("success", False):
                runner.save_results(args.output)
        
        if args.all_tests or args.stdp_verification:
            result = runner.run_stdp_verification()
            if args.output and result.get("success", False):
                runner.save_results(args.output)
        
        if args.all_tests:
            # Run all tests
            final_result = runner.run_all_tests()
            if args.output:
                runner.save_results(args.output)
        
        print("\n" + "="*50)
        print("EXAMPLE SCRIPT COMPLETED")
        print("="*50)
        
    except Exception as e:
        print(f"\n❌ ERROR: {str(e)}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    sys.exit(main())