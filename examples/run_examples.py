import os
import sys

def run_examples():
    """Run all examples in the examples directory."""
    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    examples_dir = os.path.join(script_dir, 'examples')
    
    print("NLM Python Examples - Running All Examples")
    print("=" * 80)
    print(f"Examples directory: {examples_dir}")
    print(f"Script directory: {script_dir}")
    
    # List all Python files in examples directory
    example_files = []
    for filename in os.listdir(examples_dir):
        if filename.endswith('.py') and not filename.startswith('__'):
            example_files.append(filename)
    
    print(f"\nFound {len(example_files)} example files:")
    for filename in sorted(example_files):
        print(f"  • {filename}")
    
    # Try to run examples
    for filename in sorted(example_files):
        print(f"\n{'=' * 80}")
        print(f"Running: {filename}")
        print(f"{'=' * 80}")
        
        try:
            # Import and run the example
            module_name = filename[:-3]  # Remove .py extension
            spec = __import__(f"examples.{module_name}", fromlist=[''])
            
            # Try to find a main function or test function
            if hasattr(spec, 'main'):
                spec.main()
            elif hasattr(spec, 'test_complete_agent_simulation'):
                spec.test_complete_agent_simulation(100)
            elif hasattr(spec, 'analyze_brain_activity'):
                # For visualization example, create a simple brain
                import pynlm
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                spec.analyze_brain_activity(brain, 50)
            else:
                # Try to find any function that starts with 'test_'
                test_functions = [name for name in dir(spec) 
                                if name.startswith('test_')]
                
                if test_functions:
                    func_to_run = test_functions[0]
                    print(f"Running function: {func_to_run}")
                    getattr(spec, func_to_run)()
                else:
                    print(f"No test functions found in {filename}")
            
            print(f"✓ {filename} completed successfully")
            
        except Exception as e:
            print(f"✗ Error running {filename}: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'=' * 80}")
    print("All examples processed!")
    print(f"{'=' * 80}")

if __name__ == '__main__':
    # Add the script directory to Python path
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if script_dir not in sys.path:
        sys.path.insert(0, script_dir)
    
    run_examples()