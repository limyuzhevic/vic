#!/usr/bin/env python3

import re

# Read the bindings file
with open('python/bindings.cpp', 'r') as f:
    content = f.read()

# Extract all py::class_ declarations
class_pattern = r'py::class_<([^,]+),\s*\w*\s*\)'
classes_found = re.findall(class_pattern, content)
print("BOUND CLASSES:")
for cls in sorted(classes_found):
    print(f"  {cls}")

print(f"\nTotal bound classes: {len(classes_found)}")

# Extract all py::enum_ declarations
enum_pattern = r'py::enum_<([^,]+),\s*\w*\s*\)'
enums_found = re.findall(enum_pattern, content)
print(f"\nBOUND ENUMS ({len(enums_found)}):")
for enum in sorted(enums_found):
    print(f"  {enum}")

# Extract key functions from Brain class (from the README and easy_usage.md)
brain_methods = [
    "initialize", "step", "reset", "save", "load",
    "receiveSensoryInput", "injectCurrent", 
    "produceAction", "addRegion", "getRegion", "getRegionCount",
    "getRegionIds", "getRegions", "getTotalNeuronCount",
    "getTotalSynapseCount", "getActiveNeuronCount",
    "getFiringNeuronCount", "getAverageFiringRate",
    "getExcitationInhibitionRatio", "getTotalSpikeCount",
    "getDevelopmentalStage", "setDevelopmentalStage",
    "getConfig", "logStatus"
]

print(f"\n\nIMPORTANT BRAIN METHODS CHECKED AGAINST current bindings:")
print("(These should be callable from Python)")
for method in brain_methods:
    if f".def(\"{method}\", &Brain::{method})" in content:
        print(f"  ✓ {method}")
    else:
        print(f"  ✗ {method}")

# Check AgentBrain methods
agentbrain_methods = [
    "initialize", "getSensoryInputSize", "getMotorOutputSize",
    "processSensoryInput", "decodeMotorCommand",
    "applyRewardModulation", "updateDevelopment",
    "getDevelopmentalStage", "getNeuromodulationLevel",
    "getCuriosityLevel", "getNoveltyLevel", "getPredictionError",
    "reset", "getBrain", "enableRewardModulation",
    "enableStructuralPlasticity", "enableDevelopment", "enableCuriosity",
    "isRewardModulationEnabled", "isStructuralPlasticityEnabled",
    "isDevelopmentEnabled", "isCuriosityEnabled"
]

print(f"\n\nIMPORTANT AGENTBRAIN METHODS CHECKED AGAINST current bindings:")
print("(These should be callable from Python)")
for method in agentbrain_methods:
    if f".def(\"{method}\", &AgentBrain::{method})" in content:
        print(f"  ✓ {method}")
    else:
        print(f"  ✗ {method}")

# Check Config methods
config_methods = [
    "loadFromFile", "loadFromArgs", "saveToFile", "has",
    "getKeys", "clear", "summary"
]

print(f"\n\nIMPORTANT CONFIG METHODS CHECKED AGAINST current bindings:")
print("(These should be callable from Python)")
for method in config_methods:
    if f".def(\"{method}\", &Config::{method})" in content:
        print(f"  ✓ {method}")
    else:
        print(f"  ✗ {method}")

# Check SimpleWorld methods
simpleworld_methods = [
    "configure", "reset", "setAgentStart", "update",
    "applyMotorCommand", "getSensoryPercept", "getAgentBody",
    "addObject", "removeObject", "isValidPosition", "getWidth",
    "getHeight", "getMaxEnergy", "setMaxEnergy", "getEnergyDecayRate",
    "setEnergyDecayRate", "getSimulationTime", "setRandomSeed",
    "getRandomSeed"
]

print(f"\n\nIMPORTANT SIMPLEWORLD METHODS CHECKED AGAINST current bindings:")
print("(These should be callable from Python)")
for method in simpleworld_methods:
    if f".def(\"{method}\", &SimpleWorld::{method})" in content:
        print(f"  ✓ {method}")
    else:
        print(f"  ✗ {method}")