/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d36b4516-9598-423b-a111-045888d2ef5f/examples/interactive_repl.py
Name: Interactive REPL example
Summary: Interactive REPL example for NLM brain control

"""
Interactive REPL example for NLM Python interface.

This example demonstrates the REPL (Read-Eval-Print Loop) functionality.
"""

import pynlm
import threading
import time

def repl_example():
    """Example REPL session"""
    print("=== NLM Brain REPL Interactive Example ===\n")
    
    # Initialize system
    print("Initializing NLM system...")
    interface = pynlm.initialize_nlm()
    
    # Create REPL interface
    print("Creating REPL interface...")
    repl = pynlm.REPL(interface)
    
    # Enable auto-complete and syntax highlighting
    repl.enable_auto_complete(True)
    repl.set_syntax_highlighting(True)
    repl.set_show_hints(True)
    
    print("\n=== REPL Commands ===")
    print("help - Show help")
    print("init - Initialize system")
    print("status - Show system status")
    print("step [dt] - Run simulation step")
    print("run [duration] [dt] - Run simulation")
    print("inject <type> [params] - Inject stimulus")
    print("save <file> - Save brain state")
    print("load <file> - Load brain state")
    print("state - Get brain state")
    print("pause - Pause simulation")
    print("resume - Resume simulation")
    print("plugins - List plugins")
    print("exit/quit - Exit REPL")
    
    print("\n=== REPL Session Started ===")
    print("Type 'help' for commands or 'exit' to quit")
    print("-" * 50)
    
    # Create background thread for REPL
    def repl_thread():
        try:
            repl.start()
        except KeyboardInterrupt:
            print("\nREPL interrupted")
        except Exception as e:
            print(f"\nREPL error: {e}")
    
    # Start REPL in background thread
    repl_thread = threading.Thread(target=repl_thread, daemon=True)
    repl_thread.start()
    
    # Simulate interactive session
    print("\nSimulating interactive REPL session...")
    
    # Small delay to let REPL start
    time.sleep(0.5)
    
    # Simulate some commands
    print("\nExecuting simulated commands...")
    
    # Command: help
    print("Command: help")
    repl._cmd_help()
    
    # Command: init
    print("\nCommand: init")
    repl._cmd_init()
    
    # Command: status
    print("\nCommand: status")
    repl._cmd_status()
    
    # Command: step
    print("\nCommand: step 0.1")
    repl._cmd_step("0.1")
    
    # Command: inject
    print("\nCommand: inject current neuron_id=0 current=1.0")
    repl._cmd_inject("current", "neuron_id=0,current=1.0")
    
    # Command: save
    print("\nCommand: save test_state.json")
    repl._cmd_save("test_state.json")
    
    # Command: load
    print("\nCommand: load test_state.json")
    repl._cmd_load("test_state.json")
    
    # Command: state
    print("\nCommand: state")
    repl._cmd_state()
    
    # Command: pause
    print("\nCommand: pause")
    repl._cmd_pause()
    
    # Command: resume
    print("\nCommand: resume")
    repl._cmd_resume()
    
    # Command: plugins
    print("\nCommand: plugins")
    repl._cmd_list_plugins()
    
    # Wait a bit for commands to complete
    time.sleep(1.0)
    
    # Final status
    print("\n" + "=" * 50)
    print("REPL session completed")
    print(f"Total commands processed: {len(repl.get_command_history())}")
    print(f"Variables set: {len(repl.variables)}")
    
    # Clean up
    print("\nCleaning up...")
    interface.reset_system()
    
    print("\n=== REPL Example Completed Successfully ===")

# Run the example
if __name__ == "__main__":
    repl_example()
