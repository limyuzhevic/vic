# Sphinx configuration file for NLM documentation
# See https://www.sphinx-doc.org/en/master/usage/configuration.html

project = 'NLM - Neural Learning Machine'
copyright = '2026, NLM Authors'
author = 'NLM Authors'

# The full version, including alpha/beta/rc tags
release = 'Phase 6'
version = '0.1.0'

# Supported languages
language = 'en'

# List of patterns, relative to the documentation directory
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# Extensions to enable
templates_path = ['_templates']
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    'sphinx.ext.intersphinx',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.todo',
    'sphinx.ext.githubpages',
    'sphinx.ext.mathjax',
    'sphinx.ext.ifconfig',
    'sphinxcontrib.bibtex',
    'sphinx.ext.graphviz',
    'sphinx.ext.inheritance_diagram',
]

# The suffix of source filenames
master_doc = 'index'

# HTML options
html_theme = 'sphinx_rtd_theme'  # Read the Docs theme
html_title = 'NLM Neural Learning Machine Documentation'
html_short_title = 'NLM Documentation'
html_logo = '../docs/logo.png'
html_favicon = '../docs/favicon.ico'
html_static_path = ['_static']
html_extra_paths = []

# Theme-specific options
html_theme_options = {
    'analytics_id': 'UA-XXXXXXX-1',  # Google Analytics ID
    'display_version': True,
    'prev_next_buttons_location': 'bottom',
    'style_external_links': False,
    'vcs_pageview_mode': 'edit',
}

# Autodoc options
autodoc_default_options = {
    'members': True,
    'member-order': 'bysource',
    'special-members': '__init__',
    'undoc-members': True,
    'exclude-members': '__weakref__',
}

autodoc_member_order = 'bysource'
autodoc_typehints = 'description'
autodoc_typehints_format = 'short'
autodoc_mock_imports = []

# Autosummary options
autosummary_generate = True
autosummary_imported_members = True

# Napoleon options (Google/NumPy style docstrings)
napoleon_google_docstring = True
napoleon_numpy_docstring = True
napoleon_include_init_with_doc = False
napoleon_include_private_with_doc = False
napoleon_include_special_with_doc = True
napoleon_use_admonition_for_examples = False
napoleon_use_admonition_for_notes = False
napoleon_use_admonition_for_references = False
napoleon_use_ivar = False
napoleon_use_param = True
napoleon_use_rtype = True
napoleon_preprocess_types = False
napoleon_type_aliases = None
napoleon_attr_annotations = True

# Intersphinx options
intersphinx_mapping = {
    'python': ('https://docs.python.org/3', None),
    'numpy': ('https://numpy.org/doc/stable/', None),
    'pybind11': ('https://pybind11.readthedocs.io/en/stable/', None),
}

# Todo extension options
todo_include_todos = True

# MathJax options
mathjax_path = None
mathjax_options = {}

# Graphviz options
graphviz_dot = 'dot'
graphviz_output_format = 'png'

# Bibtex extension (if used)
bibtex_bibfiles = []
bibtex_default_style = 'alpha'
bibtex_person_separator = ', '

# Custom settings
pygments_style = 'sphinx'
pygments_dark_style = 'monokai'

# Define custom roles and directives
import re
from docutils import nodes
from docutils.parsers.rst import Directive

class CodeBlockDirective(Directive):
    """Custom directive for highlighting code blocks with language specifiers."""
    has_content = True
    
    def run(self):
        # Get language from first line or default to cpp
        language = 'cpp'
        if self.content and self.content[0]:
            lang = self.content[0].strip()
            if lang and lang != '#':
                language = lang
        
        # Create literal block
        block = nodes.literal_block(text='\n'.join(self.content[1:] if language != 'cpp' else self.content))
        block['language'] = language
        self.add_target_and_index(block, '', '')
        return [block]

# Add custom CSS for better syntax highlighting
# This can be placed in _static/custom.css

# API documentation settings
modindex_common_prefix = ['nlm.']
