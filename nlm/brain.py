#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Brain Module

This module provides the Brain class and related functionality for the NLM neural simulation framework.
It includes neuron management, synapse dynamics, neural populations, regions, and advanced brain control capabilities.

License: MIT
Author: NLM Authors
Version: 0.1.0
"""

import os
import sys
import numpy as np
import numpy.matlib as matlib
import time
import threading
from typing import Dict, List, Any, Optional, Union, Tuple, Callable
from dataclasses import dataclass, field
from enum import Enum
import logging
import random
import math

# Import NLM core types
from nlm.core.types import *
from nlm.core.config import Config

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Version
__version__ = "0.1.0"