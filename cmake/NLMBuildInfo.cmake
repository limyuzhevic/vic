# NLM Build Information Module
# Generates build information files for the NLM project

# Generate NLM configuration file
set(NLM_CONFIG_FILE "${CMAKE_BINARY_DIR}/NLMConfig.cmake")

# Generate build info file
set(BUILD_INFO_FILE "${CMAKE_BINARY_DIR}/build_info.txt")

# Add custom target to generate build info
add_custom_target(nlm_build_info
    COMMAND ${CMAKE_COMMAND} -E echo "Build Type: ${CMAKE_BUILD_TYPE}" > ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Platform: ${CMAKE_SYSTEM_NAME}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Python Bindings: ${NLM_ENABLE_PYTHON_BINDINGS}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Tests: ${NLM_ENABLE_TESTS}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation: ${NLM_ENABLE_DOCS}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Coverage: ${NLM_ENABLE_COVERAGE}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Sanitizers: ${NLM_ENABLE_SANITIZERS}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Performance Profiling: ${NLM_ENABLE_PERFORMANCE_PROFILING}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Distributed Build: ${NLM_ENABLE_DISTRIBUTED_BUILD}" >> ${BUILD_INFO_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Shared Libraries: ${NLM_BUILD_SHARED_LIBS}" >> ${BUILD_INFO_FILE}
    COMMENT "Generating build information"
    VERBATIM
)

# Generate dependency graph
set(DEPENDENCY_GRAPH "${CMAKE_BINARY_DIR}/dependency_graph.dot")

file(WRITE ${DEPENDENCY_GRAPH}
"digraph NLM_Dependencies {
    rankdir=LR;
    node [shape=box, style=filled];
    
    nlm_core [fillcolor=lightblue];
    nlm_agent [fillcolor=lightgreen];
    nlm_world [fillcolor=lightcoral];
    nlm_phase3 [fillcolor=lightyellow];
    nlm_phase4 [fillcolor=lightpink];
    
    nlm_core -> nlm_agent;
    nlm_core -> nlm_world;
    nlm_agent -> nlm_phase3;
    nlm_world -> nlm_phase3;
    nlm_phase3 -> nlm_phase4;
}
""
)

# Create Python script to convert DOT to PNG
file(WRITE ${CMAKE_BINARY_DIR}/generate_graph.py
"#!/usr/bin/env python3
import subprocess
import sys
import os

def generate_graph():
    dot_file = '${DEPENDENCY_GRAPH}'
    png_file = '${CMAKE_BINARY_DIR}/dependency_graph.png'
    
    try:
        subprocess.run(['dot', '-Tpng', dot_file, '-o', png_file], check=True)
        print(f'Dependency graph generated: {png_file}')
    except subprocess.CalledProcessError:
        print('Error generating dependency graph. Make sure Graphviz is installed.')
    except FileNotFoundError:
        print('Graphviz not found. Install with: sudo apt-get install graphviz')

if __name__ == '__main__':
    generate_graph()
""
)

# Make script executable
execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/generate_graph.py)

# Add custom target to generate dependency graph
add_custom_target(dependency_graph
    COMMAND python ${CMAKE_BINARY_DIR}/generate_graph.py
    COMMENT "Generating dependency graph"
    VERBATIM
)
