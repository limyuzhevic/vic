# NLM Documentation Module
# Provides documentation generation and examples

# Doxygen configuration
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
"PROJECT_NAME           = \"NLM\"
PROJECT_NUMBER        = \"0.1.0\"
PROJECT_BRIEF        = \"Neuromodular Learning Machine - A modular neural network simulation framework\"
PROJECT_URL          = \"https://github.com/NLMinc/NLM\"
PROJECT_LOGO         = \"${CMAKE_CURRENT_SOURCE_DIR}/docs/images/nlm_logo.png\"

INPUT                 = \"${CMAKE_CURRENT_SOURCE_DIR}/docs\"
INPUT                 += \"${CMAKE_CURRENT_SOURCE_DIR}/src\"
EXCLUDE               = \"${CMAKE_CURRENT_SOURCE_DIR}/build\"
EXCLUDE_PATTERNS     = \"*.bak\"

OUTPUT_DIRECTORY      = \"${CMAKE_BINARY_DIR}/docs\"
GENERATE_HTML         = YES
GENERATE_LATEX        = NO
GENERATE_MAN          = NO
GENERATE_RTF         = NO
GENERATE_XML         = YES
GENERATE_CHI         = NO
GENERATE_HTMLHELP    = NO
GENERATE_WORD_DOC   = NO
GENERATE_CUSTOM_TAGS = NO

HTML_OUTPUT          = \"html\"
HTML_COLORSTYLE_HUE = 220
HTML_COLORSTYLE_SATURATION = 100
HTML_EXTRA_FILES    = \"${CMAKE_CURRENT_SOURCE_DIR}/docs/images/*\"

GENERATE_TAGFILE      = \"${CMAKE_BINARY_DIR}/NLM.tag\"
CLASS_GRAPH          = YES
COLLABORATION_GRAPH  = YES
GROUP_GRAPHS         = YES
UML_LOOK             = YES
UML_LIMIT_RECURSIVITY = NO

DOCSET_FEEDNAME      = \"NLM\"
DOCSET_BUNDLE_ID    = \"org.nlminc.NLM\"

XML_OUTPUT           = \"xml\"
XML_PROGRAMLISTING  = NO

GENERATE_RTF        = NO
GENERATE_MAN       = NO
GENERATE_HTMLHELP  = NO
GENERATE_PHP       = NO
GENERATE_XML       = YES

ALLEXTERNALS        = NO
EXTERNAL_GROUPS     = YES
PERL_PATH           = \"/usr/bin/perl\"

JAVADOC_URL          = \"https://docs.oracle.com/javase/8/docs/api/\"
JAVADOC_LINKS        = \"https://docs.oracle.com/javase/8/docs/api/\"

MACRO_EXPANSION     = NO
EXPAND_ONLY_PREDEF  = NO
SEARCH_INCLUDES    = YES
INCLUDE_FILE_PATTERNS = \"*.h\"
PREDEFINED          = \"NLM_API=\" \"NLM_EXPORT=\" \"NLM_DEPRECATED=\" \"NLM_THREAD_SAFE=\"
""
)

# Sphinx configuration for Python documentation
set(SPHINX_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/python")
file(WRITE ${SPHINX_DIR}/conf.py
"# Sphinx configuration file for NLM Python bindings

import sys
import os

# Add the bindings directory to sys.path
sys.path.insert(0, os.path.abspath('../bin'))

# -- Project information -----------------------------------------------------
project = 'NLM Python Bindings'
copyright = '2024, NLM Team'
author = 'NLM Team'
release = '0.1.0'

# -- General configuration ---------------------------------------------------
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.intersphinx',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
""
)

# Create examples directory
file(WRITE ${CMAKE_BINARY_DIR}/examples/NLMBuildExamples.md
"# NLM Build System Examples

## Example 1: Basic Debug Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

## Example 2: Release Build with Python Bindings

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j$(nproc)
```

## Example 3: Build with Tests

```bash
mkdir build && cd build
cmake .. -DNLM_ENABLE_TESTS=ON
cmake --build . --target nlm_build_options
```

## Example 4: Dependency Graph Generation

```bash
mkdir build && cd build
cmake ..
cmake --build . --target dependency_graph
```

## Example 5: Package Installation

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Example 6: Custom Build Configuration

Create a config.cmake file:

```cmake
# config.cmake
include_directories(${NLM_SOURCE_DIR}/custom)
target_link_libraries(nlm_core PRIVATE custom_lib)
```

Place this in ~/.nlmc/ directory.
""
)

# Create documentation targets
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation..."
    COMMAND doxygen ${CMAKE_BINARY_DIR}/Doxyfile
    COMMAND ${PYTHON_EXECUTABLE} -m sphinx.cmd.build -b html ${SPHINX_DIR} ${CMAKE_BINARY_DIR}/docs/python/_build/html
    COMMENT "Generating documentation"
    VERBATIM
)

add_custom_target(examples
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/examples ${CMAKE_BINARY_DIR}/docs/examples
    COMMENT "Creating examples documentation"
)
