#!/usr/bin/env python3
"""Debug and monitoring utilities for NLM (熙然) neural simulation framework

This script provides enhanced debugging and monitoring capabilities for the NLM project,
addressing the issues mentioned in the repository documentation:

1. Checkpointing improvements and validation
2. Reward system enhancements
3. Advanced debugging and monitoring commands
4. Performance profiling
"""

import sys
import os
import json
import time
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add the NLM Python bindings to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some functionality will be limited.")

def print_header(title: str, width: int = 80) -> None:
    """Print a formatted header"""
    print("\n" + "=" * width)
    print(f"{title:^{width}}")
    print("=" * width + "\n")

def print_section(title: str) -> None:
    """Print a section header"""
    print(f"\n--- {title} ---")

def check_checkpoint_system() -> Dict[str, Any]:
    """Check and validate the checkpoint system"""
    print_section("Checkpoint System Validation")
    
    if not PYNLM_AVAILABLE:
        return {"status": "ERROR", "message": "Python bindings not available"}
    
    try:
        # Test basic checkpoint functionality
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test save operation
        test_file = "/tmp/test_checkpoint.bin"
        save_success = brain.save(test_file)
        
        if save_success:
            file_size = os.path.getsize(test_file) if os.path.exists(test_file) else 0
            
            # Test load operation
            brain.reset()
            brain.initialize()
            load_success = brain.load(test_file)
            
            if load_success:
                print("✓ Checkpoint save/load operations successful")
                print(f"  File size: {file_size} bytes")
                print(f"  Neurons after load: {brain.getTotalNeuronCount()}")
                print(f"  Synapses after load: {brain.getTotalSynapseCount()}")
                
                # Clean up
                os.unlink(test_file)
                
                return {"status": "SUCCESS", "file_size": file_size}
            else:
                print("✗ Checkpoint load operation failed")
                return {"status": "ERROR", "message": "Load operation failed"}
        else:
            print("✗ Checkpoint save operation failed")
            return {"status": "ERROR", "message": "Save operation failed"}
            
    except Exception as e:
        print(f"✗ Checkpoint validation failed with exception: {e}")
        return {"status": "ERROR", "message": str(e)}

def test_reward_system() -> Dict[str, Any]:
    """Test and validate the reward system"""
    print_section("Reward System Validation")
    
    if not PYNLM_AVAILABLE:
        return {"status": "ERROR", "message": "Python bindings not available"}
    
    try:
        # Test with SimpleWorld to verify reward generation
        import tempfile
        from pathlib import Path
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a simple world for testing
        world = pynlm.createSimpleWorld()
        world.configure(20.0f, 20.0f, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(true)
        agent.enableCuriosity(true)
        
        # Run simulation and collect rewards
        total_reward = 0.0f
        steps_without_reward = 0
        max_steps_without_reward = 0
        
        for step in range(100):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get motor command
            cmd = agent.decodeMotorCommand()
            
            # Apply action to world
            result = world.applyMotorCommand(cmd, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0f)
            
            # Update world
            world.update(0.1)
            
            total_reward += result.reward
            
            if result.reward == 0.0f:
                steps_without_reward += 1
                max_steps_without_reward = max(max_steps_without_reward, steps_without_reward)
            else:
                steps_without_reward = 0
            
            # Print progress every 25 steps
            if step % 25 == 0:
                print(f"  Step {step}: Reward={result.reward:.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        avg_reward = total_reward / 100.0
        learning_efficiency = abs(avg_reward) > 0.001  # Non-zero average reward indicates learning
        
        print(f"✓ Reward system test completed")
        print(f"  Total reward over 100 steps: {total_reward:.3f}")
        print(f"  Average reward per step: {avg_reward:.5f}")
        print(f"  Maximum steps without reward: {max_steps_without_reward}")
        print(f"  Learning efficiency: {'YES' if learning_efficiency else 'NO'}")
        
        # Check if reward system is working properly
        if learning_efficiency and max_steps_without_reward < 50:
            return {
                "status": "SUCCESS",
                "total_reward": total_reward,
                "avg_reward": avg_reward,
                "learning_efficiency": learning_efficiency
            }
        else:
            return {
                "status": "WARNING",
                "total_reward": total_reward,
                "avg_reward": avg_reward,
                "learning_efficiency": learning_efficiency,
                "message": "Low reward generation detected"
            }
            
    except Exception as e:
        print(f"✗ Reward system test failed with exception: {e}")
        return {"status": "ERROR", "message": str(e)}

def check_system_integrity() -> Dict[str, Any]:
    """Check the overall system integrity"""
    print_section("System Integrity Check")
    
    integrity_score = 0
    max_score = 100
    issues = []
    
    # Check Python bindings
    if PYNLM_AVAILABLE:
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            print("✓ Python bindings functional")
            integrity_score += 25
        except Exception as e:
            issues.append(f"Python bindings error: {e}")
            print(f"✗ Python bindings error: {e}")
    else:
        issues.append("Python bindings not available")
        print("✗ Python bindings not available")
    
    # Check checkpoint functionality
    checkpoint_result = check_checkpoint_system()
    if checkpoint_result["status"] == "SUCCESS":
        integrity_score += 25
        print("✓ Checkpoint system functional")
    else:
        issues.append(f"Checkpoint system issue: {checkpoint_result.get('message', 'Unknown')}")
        print(f"✗ Checkpoint system issue: {checkpoint_result.get('message', 'Unknown')}")
    
    # Check reward system
    reward_result = test_reward_system()
    if reward_result["status"] in ["SUCCESS", "WARNING"]:
        integrity_score += 25
        print("✓ Reward system functional")
        if reward_result["status"] == "WARNING":
            issues.append(f"Reward system warning: {reward_result.get('message', 'Low reward generation')}")
    else:
        issues.append(f"Reward system error: {reward_result.get('message', 'Unknown')}")
        print(f"✗ Reward system error: {reward_result.get('message', 'Unknown')}")
    
    # Check documentation
    docs_path = Path(__file__).parent / "README.md"
    if docs_path.exists():
        print("✓ Documentation available")
        integrity_score += 25
    else:
        issues.append("Documentation not found")
        print("✗ Documentation not found")
    
    # Overall status
    status = "PASS" if integrity_score >= 75 else "WARNING" if integrity_score >= 50 else "FAIL"
    
    return {
        "status": status,
        "score": integrity_score,
        "max_score": max_score,
        "issues": issues,
        "checkpoint_result": checkpoint_result,
        "reward_result": reward_result
    }

def export_system_info(output_file: str) -> None:
    """Export system information to a JSON file"""
    print_section(f"Exporting System Information to {output_file}")
    
    try:
        system_info = {
            "timestamp": time.time(),
            "python_version": sys.version,
            "platform": sys.platform,
            "nlm_version": "1.0.0",
            "features": {
                "checkpointing": True,
                "reward_learning": True,
                "memory_systems": True,
                "prediction_systems": True,
                "cognition_systems": True
            },
            "integrity_check": check_system_integrity()
        }
        
        with open(output_file, 'w') as f:
            json.dump(system_info, f, indent=2, default=str)
        
        print(f"✓ System information exported to {output_file}")
        print(f"  File size: {os.path.getsize(output_file)} bytes")
        
    except Exception as e:
        print(f"✗ Failed to export system information: {e}")

def main():
    """Main function"""
    parser = argparse.ArgumentParser(
        description="NLM (熙然) Debug and Monitoring Utility",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python debug_monitor.py --checkpoints          # Validate checkpoint system
  python debug_monitor.py --rewards            # Test reward system
  python debug_monitor.py --integrity          # Check overall system integrity
  python debug_monitor.py --export system_info.json  # Export system information
  python debug_monitor.py --all               # Run all checks
        ""
    )
    
    parser.add_argument(
        "--checkpoints", "-c",
        action="store_true",
        help="Validate checkpoint system"
    )
    
    parser.add_argument(
        "--rewards", "-r",
        action="store_true",
        help="Test reward system"
    )
    
    parser.add_argument(
        "--integrity", "-i",
        action="store_true",
        help="Check overall system integrity"
    )
    
    parser.add_argument(
        "--export", "-e",
        metavar="FILE",
        help="Export system information to JSON file"
    )
    
    parser.add_argument(
        "--all", "-a",
        action="store_true",
        help="Run all checks"
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    if not any([args.checkpoints, args.rewards, args.integrity, args.export, args.all]):
        parser.print_help()
        return 1
    
    print_header("NLM Debug and Monitoring Utility")
    
    if args.all or args.checkpoints:
        check_checkpoint_system()
    
    if args.all or args.rewards:
        test_reward_system()
    
    if args.all or args.integrity:
        integrity_result = check_system_integrity()
        print(f"\nOverall System Integrity: {integrity_result['status']}")
        print(f"Score: {integrity_result['score']}/{integrity_result['max_score']}")
        
        if integrity_result['issues']:
            print(f"\nIssues found ({len(integrity_result['issues'])}):")
            for i, issue in enumerate(integrity_result['issues'], 1):
                print(f"  {i}. {issue}")
    
    if args.export:
        export_system_info(args.export)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())