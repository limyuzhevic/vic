#!/usr/bin/env python3
"""
NLM Python Bindings - Command Line Interface

This script provides a command-line interface for testing and demonstrating
the NLM Python bindings.
"""

import sys
import os
import argparse

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def quick_test(args):
    """Run a quick test of the bindings."""
    print("Running quick test...")
    try:
        brain = pynlm.quick_test()
        print(f"✓ Quick test passed! Created brain with {brain.getTotalNeuronCount()} neurons")
        return True
    except Exception as e:
        print(f"✗ Quick test failed: {e}")
        return False

def basic_test(args):
    """Run basic functionality tests."""
    print("Running basic functionality tests...")
    print("=" * 50)
    
    tests_passed = 0
    tests_total = 0
    
    # Test 1: Create default config
    tests_total += 1
    try:
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() - PASSED")
        tests_passed += 1
    except Exception as e:
        print(f"✗ createDefaultConfig() - FAILED: {e}")
    
    # Test 2: Create brain
    tests_total += 1
    try:
        brain = pynlm.createBrain()
        brain.initialize()
        print(f"✓ createBrain() - PASSED ({brain.getTotalNeuronCount()} neurons)")
        tests_passed += 1
    except Exception as e:
        print(f"✗ createBrain() - FAILED: {e}")
    
    # Test 3: Run simulation
    tests_total += 1
    try:
        brain = pynlm.createBrain()
        brain.initialize()
        for step in range(5):
            brain.step(step)
        print(f"✓ Brain simulation - PASSED (spikes: {brain.getTotalSpikeCount()})")
        tests_passed += 1
    except Exception as e:
        print(f"✗ Brain simulation - FAILED: {e}")
    
    # Test 4: Create world
    tests_total += 1
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        print("✓ createSimpleWorld() - PASSED")
        tests_passed += 1
    except Exception as e:
        print(f"✗ createSimpleWorld() - FAILED: {e}")
    
    # Test 5: Create agent
    tests_total += 1
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(pynlm.createSimpleWorld())
        print("✓ createAgentBrain() - PASSED")
        tests_passed += 1
    except Exception as e:
        print(f"✗ createAgentBrain() - FAILED: {e}")
    
    # Test 6: Convenience functions
    tests_total += 1
    try:
        sim = pynlm.create_simulation()
        assert 'brain' in sim
        assert 'world' in sim
        assert 'agent' in sim
        print("✓ create_simulation() - PASSED")
        tests_passed += 1
    except Exception as e:
        print(f"✗ create_simulation() - FAILED: {e}")
    
    print(f"\nBasic tests: {tests_passed}/{tests_total} passed")
    return tests_passed == tests_total

def demo_test(args):
    """Run a demo with controlled output."""
    print(f"\nRunning demo test ({args.steps} steps)...")
    print("=" * 50)
    
    try:
        # Create simulation
        sim = pynlm.create_simulation()
        brain = sim['brain']
        world = sim['world']
        agent = sim['agent']
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(args.steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 5 == 0:
                print(f"  Step {step:2d}: "
                      f"Firing={brain.getFiringNeuronCount():4d}, "
                      f"Curiosity={agent.getCuriosityLevel():5.3f}")
        
        print(f"\nDemo completed: {args.steps} simulation steps")
        return True
        
    except Exception as e:
        print(f"✗ Demo test failed: {e}")
        return False

def agent_test(args):
    """Run agent-specific demo."""
    print(f"\nRunning agent test ({args.steps} steps)...")
    print("=" * 50)
    
    try:
        # Create simulation
        sim = pynlm.create_simulation()
        brain = sim['brain']
        world = sim['world']
        agent = sim['agent']
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run simulation with detailed logging
        for step in range(args.steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            if step % 5 == 0:
                print(f"  Step {step:2d}: "
                      f"Firing={brain.getFiringNeuronCount():4d}, "
                      f"Curiosity={agent.getCuriosityLevel():5.3f}, "
                      f"Novelty={agent.getNoveltyLevel():5.3f}, "
                      f"Dev={brain.getDevelopmentalStage()}")
        
        print(f"\nAgent test completed: {args.steps} steps")
        print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent test failed: {e}")
        return False

def main():
    """Main CLI function."""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings - Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Quick test command
    parser_quick = subparsers.add_parser('quick', help='Run quick test')
    
    # Basic test command
    parser_basic = subparsers.add_parser('basic', help='Run basic tests')
    
    # Demo test command
    parser_demo = subparsers.add_parser('demo', help='Run demo test')
    parser_demo.add_argument('--steps', type=int, default=20,
                           help='Number of simulation steps (default: 20)')
    
    # Agent test command
    parser_agent = subparsers.add_parser('agent', help='Run agent test')
    parser_agent.add_argument('--steps', type=int, default=20,
                             help='Number of simulation steps (default: 20)')
    
    # Test command (default)
    parser_test = subparsers.add_parser('test', help='Run comprehensive tests')
    
    # Help command
    parser_help = subparsers.add_parser('help', help='Show help information')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    print(f"NLM Python Bindings - Command Line Interface")
    print("=" * 60)
    print(f"Command: {args.command}")
    
    success = False
    
    if args.command == 'quick':
        success = quick_test(args)
    elif args.command == 'basic':
        success = basic_test(args)
    elif args.command == 'demo':
        success = demo_test(args)
    elif args.command == 'agent':
        success = agent_test(args)
    elif args.command == 'test':
        success = (basic_test(args) and demo_test(args) and agent_test(args))
    elif args.command == 'help':
        print("\nAvailable commands:")
        print("  quick  - Run quick test")
        print("  basic  - Run basic functionality tests")
        print("  demo   - Run demo test")
        print("  agent  - Run agent-specific test")
        print("  test   - Run all comprehensive tests")
        print("  help   - Show this help message")
        print("\nExamples:")
        print("  python -m pynlm.cli quick")
        print("  python -m pynlm.cli basic")
        print("  python -m pynlm.cli demo --steps 50")
        print("  python -m pynlm.cli agent --steps 30")
        print("  python -m pynlm.cli test")
        return 0
    
    if success:
        print("\n" + "=" * 60)
        print("CLI execution completed successfully!")
        return 0
    else:
        print("\n" + "=" * 60)
        print("CLI execution completed with errors!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
