# Python CLI module for NLM

import sys
from pathlib import Path

# Add the src directory to the Python path
sys.path.insert(0, str(Path(__file__).parent / "src"))

def main():
    """Main entry point for NLM CLI"""
    from nlm.cli import create_cli_app
    
    app = create_cli_app()
    return app.run()

if __name__ == "__main__":
    sys.exit(main())