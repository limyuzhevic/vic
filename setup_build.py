#!/usr/bin/env python3
import subprocess
import sys
import os

# Install pip in venv
def install_pip():
    print("Installing pip in virtual environment...")
    # First, let's install python3-pip system-wide
    subprocess.run(["apt", "install", "-y", "python3-pip"], check=True)
    
    # Now install pip in venv using get-pip.py
    print("Downloading get-pip.py...")
    import urllib.request
    urllib.request.urlretrieve("https://bootstrap.pypa.io/get-pip.py", "get-pip.py")
    
    print("Installing pip in venv...")
    result = subprocess.run(["venv/bin/python", "get-pip.py"], 
                          cwd="/workspace/app",
                          capture_output=True, text=True)
    if result.returncode != 0:
        print("Error installing pip:", result.stderr)
        return False
    print("pip installed successfully")
    return True

if __name__ == "__main__":
    install_pip()
