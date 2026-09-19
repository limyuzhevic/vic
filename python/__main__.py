#!/usr/bin/env python3
"""Main entry point for the NLM Python package."""

import sys
from typing import List, Any

# Import all bindings
from .bindings import *

def main():
    """Entry point for command-line usage."""
    print("NLM Python Bindings")
    print("===================")
    print("This package provides Python bindings for the Neural Learning Machine (NLM)")
    print("simulation framework. See the documentation for more information.")
    print()
    print("Available modules and classes:")
    print(f"  NeuronId, SynapseId, RegionId, PopulationId")
    print(f"  NeuronType, SynapseType, DevelopmentalStage, FiringState")
    print(f"  ActionType, MotorCommand, WorldObjectType")
    print(f"  Config, SensoryInput, Vision, Audio, InternalSignals")
    print(f"  Action, WorldObject, AgentBody, ActionResult")
    print(f"  SensoryPercept, SimpleWorld, Brain, AgentBrain")
    print(f"  INVALID_NEURON_ID, INVALID_SYNAPSE_ID")
    print(f"  INVALID_REGION_ID, INVALID_POPULATION_ID")
    print(f"  createDefaultConfig, createBrain, createSimpleWorld")
    print(f"  createAgentBrain, createAction, createActionWithParams")
    print(f"  createMotorCommand, createDefaultMotorCommand")
    print(f"  createWorldObject, createDefaultWorldObject")
    print(f"  createActionResult, createDefaultActionResult")
    print()
    print("Example usage:")
    print("  from nlm import Config, SimpleWorld, createDefaultConfig")
    print("  config = createDefaultConfig()")
    print("  world = createSimpleWorld()")

if __name__ == "__main__":
    main()
