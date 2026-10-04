"""
NLM Python Command Line Interface

This module provides a command-line interface for the NLM Python bindings,
allowing users to interact with the neural simulation framework from the command line.
"""

import argparse
import sys
import os
from pathlib import Path

try:
    import pynlm_logging as logging
    import pynlm_agent
    from pynlm_agent import AgentBrain, SensoryPercept, MotorCommand, AgentBody
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: NLM Python bindings not available: {e}")


def version():
    """Print NLM Python bindings version"""
    try:
        from .. import __version__
        print(f"NLM Python Bindings v{__version__}")
    except ImportError:
        print("NLM Python Bindings v0.1.0")


def init():
    """Initialize NLM Python bindings"""
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        return 1
    
    print("Initializing NLM Python bindings...")
    
    # Initialize logging
    logger = logging.getGlobalLogger()
    logger.setLevel(logging.LogLevel.INFO)
    logger.enableConsoleOutput(True)
    
    print("NLM Python bindings initialized successfully")
    return 0


def create_agent():
    """Create a sample agent"""
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        return 1
    
    print("Creating sample agent...")
    
    try:
        # Note: In a real implementation, we would need a Brain instance
        # For this example, we'll just create the agent structure
        body = AgentBody()
        percept = SensoryPercept()
        
        print("Sample agent created:")
        print(f"  - Body at ({body.x}, {body.y})")
        print(f"  - Vision grid: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
        print(f"  - Touch sensors: {len(percept.getTouch())}")
        print(f"  - Internal signals: {len(percept.getInternal())}")
        print(f"  - Proprioception: {len(percept.getProprioception())}")
        
        return 0
        
    except Exception as e:
        print(f"Error creating agent: {e}")
        return 1


def configure_logging():
    """Configure logging settings"""
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        return 1
    
    parser = argparse.ArgumentParser(description="Configure logging")
    parser.add_argument("--level", choices=["TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"],
                       default="INFO", help="Log level")
    parser.add_argument("--console", action="store_true", default=True,
                       help="Enable console output")
    parser.add_argument("--file", help="Log file path")
    
    args = parser.parse_args()
    
    logger = logging.getGlobalLogger()
    
    # Set log level
    level_map = {
        "TRACE": logging.LogLevel.TRACE,
        "DEBUG": logging.LogLevel.DEBUG,
        "INFO": logging.LogLevel.INFO,
        "WARN": logging.LogLevel.WARN,
        "ERROR": logging.LogLevel.ERROR,
        "FATAL": logging.LogLevel.FATAL
    }
    
    logger.setLevel(level_map[args.level])
    
    # Configure output
    logger.enableConsoleOutput(args.console)
    
    if args.file:
        logger.enableFileOutput(args.file)
    elif logger.getLogFilePath():
        logger.disableFileOutput()
    
    print(f"Logging configured: level={args.level}, console={args.console}, file={args.file or 'disabled'}")
    return 0


def log_status():
    """Log agent/system status"""
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        return 1
    
    logger = logging.getGlobalLogger()
    
    print("NLM System Status:")
    print(f"  - Log level: {logger.getLevel()}")
    print(f"  - Console output: {'enabled' if logger.isLevelEnabled(logging.LogLevel.INFO) else 'disabled'}")
    print(f"  - File output: {'enabled' if logger.getLogFilePath() else 'disabled'}")
    if logger.getLogFilePath():
        print(f"  - Log file: {logger.getLogFilePath()}")
    
    print(f"  - Total log messages: {logger.getLogCount()}")
    
    return 0


def run_example():
    """Run the agent example"""
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        return 1
    
    print("Running NLM agent example...")
    
    try:
        # Import and run the example
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'examples'))
        
        from agent_example import main as run_examples
        return run_examples()
        
    except Exception as e:
        print(f"Error running example: {e}")
        return 1


def main():
    """Main command-line interface"""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli --version                    # Show version
  nlm-cli init                         # Initialize bindings
  nlm-cli create-agent                 # Create a sample agent
  nlm-cli logging --level DEBUG --file nlm.log    # Configure logging
  nlm-cli status                       # Show system status
  nlm-cli example                      # Run usage examples
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Version command
    subparsers.add_parser("version", help="Show version information")
    
    # Init command
    subparsers.add_parser("init", help="Initialize NLM Python bindings")
    
    # Create agent command
    subparsers.add_parser("create-agent", help="Create a sample agent")
    
    # Logging command
    logging_parser = subparsers.add_parser("logging", help="Configure logging")
    logging_parser.add_argument("--level", choices=["TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"],
                               default="INFO", help="Log level")
    logging_parser.add_argument("--console", action="store_true", default=True,
                               help="Enable console output")
    logging_parser.add_argument("--file", help="Log file path")
    
    # Status command
    subparsers.add_parser("status", help="Show system status")
    
    # Example command
    subparsers.add_parser("example", help="Run usage examples")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle commands
    if args.command == "version":
        version()
        return 0
    elif args.command == "init":
        return init()
    elif args.command == "create-agent":
        return create_agent()
    elif args.command == "logging":
        return configure_logging()
    elif args.command == "status":
        return log_status()
    elif args.command == "example":
        return run_example()
    else:
        parser.print_help()
        return 1


if __name__ == "__main__":
    sys.exit(main())
