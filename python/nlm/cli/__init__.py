"""NLM CLI package"""

from .nlm_cli import NLMCli, create_parser, main, __version__
from .config_manager import NLMConfigManager, get_default_template, save_default_template, load_config_with_env
from .batch import NLMBatchProcessor, BatchUtils
from .validation import NLMValidator
from .comparison import NLMComparison
from .presets import NLMPresets, BuiltInPresets

__all__ = [
    # Main CLI
    'NLMCli',
    'create_parser', 
    'main',
    '__version__',
    # Configuration management
    'NLMConfigManager',
    'get_default_template',
    'save_default_template',
    'load_config_with_env',
    # Batch processing
    'NLMBatchProcessor',
    'BatchUtils',
    # Validation
    'NLMValidator',
    # Comparison
    'NLMComparison',
    # Presets
    'NLMPresets',
    'BuiltInPresets'
]
