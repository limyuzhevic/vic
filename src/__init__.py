"""
NLM Python Package - Enhanced Python Bindings for Neural Learning Machine

This package provides Python bindings for the NLM (Neural Learning Machine) framework,
enhancing the original C++ implementation with comprehensive Python interfaces,
logging capabilities, and easy-to-use APIs for agent development and simulation.

Main Modules:
    pynlm_agent: Core agent functionality (AgentBrain, SensoryPercept, etc.)
    pynlm_logging: Logging and monitoring utilities
    pynlm: High-level API and command-line interface

Usage Examples:
    # Import core agent functionality
    from pynlm import Agent, SensoryPercept, MotorCommand
    
    # Create and configure an agent
    agent = Agent()
    agent.configure(exploration_rate=0.3)
    
    # Run simulation
    for step in range(1000):
        percept = agent.get_sensory_input()
        action = agent.get_action(percept)
        reward = agent.execute_action(action)
    
    # Access logs and monitoring
    logs = agent.get_logs()
    stats = agent.get_performance_stats()
"""

from . import pynlm_agent, pynlm_logging, __main__

__all__ = ['pynlm_agent', 'pynlm_logging', '__main__']

# Package version
__version__ = "0.1.0"

# Package metadata
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
__license__ = "MIT"

# Initialize logging when package is imported
import logging as std_logging
std_logging.basicConfig(level=std_logging.INFO)

print(f"NLM Python Bindings v{__version__} loaded successfully")
print("Available modules: pynlm_agent, pynlm_logging")
