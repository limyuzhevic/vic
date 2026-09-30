# Enhanced NLM Plugin System

This module extends the existing Python plugin system with comprehensive plugin management capabilities, security sandboxing, dependency management, and multi-language support.

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType
from pynlm.plugin_system import Plugin, PluginInfo, PluginManager, PluginType, PluginStatus
import importlib.util
import sys
import os
import time
import threading
import json
import hashlib
import shutil
from typing import Dict, List, Any, Optional, Callable, Type, Set, Union
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod
import pkgutil
import tempfile
import subprocess

# Import enhanced plugin system components
from .core_plugin_framework import EnhancedPluginManager
from .plugin_categories import (
    BrainPlugin, PlasticityPlugin, NeuromodulationPlugin,
    MemoryPlugin, SensoryPlugin, MotorPlugin, DevelopmentPlugin,
    IntegrationPlugin, AnalysisPlugin, UtilityPlugin
)
from .plugin_development_kit import (
    PluginSDK, TemplateGenerator, TestingFramework, DocumentationGenerator,
    PluginPackager
)
from .runtime_features import (
    HotPlugManager, StateManager, EventSystem, CoordinationManager,
    LifecycleMonitor
)
from .plugin_marketplace import (
    PluginMarketplace, RatingSystem, CertificationSystem,
    MetadataSystem
)

# Export enhanced plugin classes
__all__ = [
    'EnhancedPluginManager',
    'Plugin',
    'PluginInfo',
    'PluginManager',
    'PluginType',
    'PluginStatus',
    'BrainPlugin',
    'PlasticityPlugin',
    'NeuromodulationPlugin',
    'MemoryPlugin',
    'SensoryPlugin',
    'MotorPlugin',
    'DevelopmentPlugin',
    'IntegrationPlugin',
    'AnalysisPlugin',
    'UtilityPlugin',
    'PluginSDK',
    'TemplateGenerator',
    'TestingFramework',
    'DocumentationGenerator',
    'PluginPackager',
    'HotPlugManager',
    'StateManager',
    'EventSystem',
    'CoordinationManager',
    'LifecycleMonitor',
    'PluginMarketplace',
    'RatingSystem',
    'CertificationSystem',
    'MetadataSystem',
    'MultiLanguagePluginLoader',
    'BackwardCompatibilityManager',
    'PluginPerformanceMonitor',
    'PluginManagementInterface'
]

# Version information
__version__ = '1.0.0'
__author__ = 'NLM Research Team'
__license__ = 'MIT'

# Global plugin manager instance
_global_plugin_manager = None


def get_plugin_manager():
    """Get the global plugin manager instance"""
    return _global_plugin_manager


def set_plugin_manager(manager):
    """Set the global plugin manager instance"""
    global _global_plugin_manager
    _global_plugin_manager = manager


def initialize_plugin_system(interface, config=None):
    """Initialize the enhanced plugin system"""
    global _global_plugin_manager
    _global_plugin_manager = EnhancedPluginManager(interface, config)
    return _global_plugin_manager


def shutdown_plugin_system():
    """Shutdown the plugin system"""
    global _global_plugin_manager
    if _global_plugin_manager:
        _global_plugin_manager.shutdown()
        _global_plugin_manager = None


# Convenience functions for common plugin operations
def load_plugin(plugin_name, plugin_path=None, config=None):
    """Load a plugin using the global plugin manager"""
    if _global_plugin_manager:
        return _global_plugin_manager.load_plugin(plugin_name, plugin_path, config)
    raise RuntimeError("Plugin system not initialized")


def unload_plugin(plugin_name):
    """Unload a plugin using the global plugin manager"""
    if _global_plugin_manager:
        return _global_plugin_manager.unload_plugin(plugin_name)
    raise RuntimeError("Plugin system not initialized")


def list_plugins():
    """List all loaded plugins using the global plugin manager"""
    if _global_plugin_manager:
        return _global_plugin_manager.list_plugins()
    raise RuntimeError("Plugin system not initialized")


def search_plugins(query, filters=None):
    """Search plugins using the global plugin manager"""
    if _global_plugin_manager:
        return _global_plugin_manager.search_plugins(query, filters)
    raise RuntimeError("Plugin system not initialized")
