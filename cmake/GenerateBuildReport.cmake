# CMake script to generate build report
# This script generates a comprehensive report of the build configuration

cmake_minimum_required(VERSION 3.16)

message("Generating NLM build report...")

# Get project information
project_info = "NLM (熙然) - Neural Learning Machine"
set(PROJECT_VERSION ${PROJECT_VERSION})

# Build configuration
build_config = {
    "build_type": "${CMAKE_BUILD_TYPE}",
    "cxx_standard": "${CMAKE_CXX_STANDARD}",
    "cxx_extensions": "${CMAKE_CXX_EXTENSIONS}",
    "build_dir": "${CMAKE_BINARY_DIR}",
    "source_dir": "${CMAKE_CURRENT_SOURCE_DIR}",
    "generator": "${CMAKE_GENERATOR}"
}

# Compiler information
compiler_info = {
    "cxx_compiler": "${CMAKE_CXX_COMPILER}",
    "cxx_compiler_version": "${CMAKE_CXX_COMPILER_VERSION}",
    "cxx_flags": "${CMAKE_CXX_FLAGS}",
    "cxx_flags_${CMAKE_BUILD_TYPE}": "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
}

# Libraries found
libraries = {}

# Python
if(Python_FOUND)
    set(libraries["python"] {
        "found": true,
        "version": "${Python_VERSION}",
        "executable": "${Python_EXECUTABLE}",
        "include_dir": "${Python_INCLUDE_DIRS}",
        "library_dir": "${Python_LIBRARIES}"
    })
endif()

if(pybind11_FOUND)
    set(libraries["pybind11"] {
        "found": true,
        "version": "${pybind11_VERSION}",
        "include_dir": "${pybind11_INCLUDE_DIR}",
        "library_dir": "${pybind11_LIBRARIES}"
    })
endif()

# Targets
function(list_targets output_var)
    set(targets)
    foreach(target ${PROJECT_NAME} ${PROJECT_NAME}_targets)
        if(TARGET ${target})
            list(APPEND targets ${target})
        endif()
    endforeach()
    
    set(${output_var} "${targets}" PARENT_SCOPE)
endfunction()

list_targets(all_targets)

# Source files
if(DEFINED SOURCES)
    set(source_count "${SOURCES}")
else()
    file(GLOB_RECURSE all_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
    set(source_count "${all_sources}")
endif()

# Generate HTML report
report_content = "<!DOCTYPE html>\n<html>\n<head>\n"
report_content += "<title>NLM Build Report</title>\n"
report_content += "<style>\n"
report_content += "body { font-family: Arial, sans-serif; margin: 20px; }\n"
report_content += "h1 { color: #2c3e50; }\n"
report_content += "h2 { color: #34495e; border-bottom: 2px solid #3498db; padding-bottom: 5px; }\n"
report_content += ".config { background-color: #f8f9fa; padding: 10px; border-radius: 5px; }\n"
report_content += ".target { background-color: #e8f4f8; padding: 5px; margin: 5px 0; border-radius: 3px; }\n"
report_content += "</style>\n"
report_content += "</head>\n<body>\n"

report_content += "<h1>${project_info}</h1>\n"
report_content += "<p><strong>Version:</strong> ${PROJECT_VERSION}</p>\n"
report_content += "<p><strong>Generated:</strong> ${CMAKE_CURRENT_TIME}</p>\n"

report_content += "<h2>Build Configuration</h2>\n"
report_content += "<div class=\"config\">\n"
for(key, value in build_config.items()):
    report_content += f"<p><strong>{key}:</strong> {value}</p>\n"
report_content += "</div>\n"

report_content += "<h2>Compiler Information</h2>\n"
report_content += "<div class=\"config\">\n"
for(key, value in compiler_info.items()):
    report_content += f"<p><strong>{key}:</strong> {value}</p>\n"
report_content += "</div>\n"

report_content += "<h2>Libraries</h2>\n"
report_content += "<div class=\"config\">\n"
for(lib_name, lib_info in libraries.items()):
    report_content += f"<h3>{lib_name}</h3>\n"
    if lib_info["found"]:
        report_content += "<p><strong>Status:</strong> Found</p>\n"
        for(k, v) in lib_info.items():
            if k != "found":
                report_content += f"<p><strong>{k}:</strong> {v}</p>\n"
    else:
        report_content += "<p><strong>Status:</strong> Not Found</p>\n"
report_content += "</div>\n"

report_content += "<h2>Targets</h2>\n"
for(target in all_targets):
    report_content += f"<div class=\"target\"><strong>{target}</strong></div>\n"

report_content += "<h2>Source Files</h2>\n"
report_content += f"<p>Total source files: {len(source_count)}</p>\n"

report_content += "</body>\n</html>\n"

# Write report to file
report_file = "${CMAKE_CURRENT_BINARY_DIR}/build_report.html"
file(WRITE "${report_file}" "${report_content}")

message("Build report generated: ${report_file}")

# Also create a simple text summary
summary_content = "NLM Build Report\n"
summary_content += "==================\n\n"
summary_content += f"Project: {project_info}\n"
summary_content += f"Version: {PROJECT_VERSION}\n"
summary_content += f"Generated: {CMAKE_CURRENT_TIME}\n\n"
summary_content += "Build Configuration:\n"
for(key, value in build_config.items()):
    summary_content += f"  {key}: {value}\n"
summary_content += "\nTargets:\n"
for(target in all_targets):
    summary_content += f"  - {target}\n"
summary_content += f"\nSource Files: {len(source_count)}\n"

summary_file = "${CMAKE_CURRENT_BINARY_DIR}/build_report.txt"
file(WRITE "${summary_file}" "${summary_content}")

message("Build summary generated: ${summary_file}")
