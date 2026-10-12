# Python CLI interface for NLM

import sys
from pathlib import Path

# Add the src directory to the Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

def create_cli_app():
    """Create and return a CLI application"""
    try:
        from nlm_cli import createCLIApp
        return createCLIApp()
    except ImportError:
        print("Error: CLI library not properly built. Please build the NLM CLI first.")
        sys.exit(1)

def main():
    """Main entry point for NLM CLI"""
    app = create_cli_app()
    
    if not app.parse(len(sys.argv), sys.argv):
        return 1
    
    return app.run()

if __name__ == "__main__":
    sys.exit(main())