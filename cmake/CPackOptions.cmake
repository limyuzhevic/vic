# CPack configuration options

# Package information
set(CPACK_PACKAGE_NAME "nlm")
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine Framework")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A neural network simulation and learning framework")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Repository information
set(CPACK_PACKAGE_URL "https://github.com/example/nlm")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/example/nlm")

# Package contacts
set(CPACK_PACKAGE_CONTACT "nlm-dev@example.com")
set(CPACK_PACKAGE_MAINTAINER "NLM Development Team")

# Package summary
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_DESCRIPTION "A comprehensive neural network simulation and learning framework")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neural Learning Machine (NLM) Framework")

# Distribution information
set(CPACK_DISTRIBUTION_DESCRIPTION "Neural Learning Machine framework")
set(CPACK_COPYRIGHT "Copyright 2026 NLM Contributors")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

# Component packaging (future)
set(CPACK_COMPONENTS_ALL
    core
    neural
    plasticity
    memory
    neuromodulation
    development
    cognition
    sensory
    prediction
    motor
    environment
    experiment
    agent
)

# Component details
set(CPACK_COMPONENT_CORE_DESCRIPTION "Core types and utilities")
set(CPACK_COMPONENT_NEURAL_DESCRIPTION "Neural network components")
set(CPACK_COMPONENT_PLASTICITY_DESCRIPTION "Synaptic plasticity mechanisms")
set(CPACK_COMPONENT_MEMORY_DESCRIPTION "Memory systems")
set(CPACK_COMPONENT_NEUROMODULATION_DESCRIPTION "Neuromodulation signals")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Neural development")
set(CPACK_COMPONENT_COGNITION_DESCRIPTION "Cognitive functions")
set(CPACK_COMPONENT_SENSORY_DESCRIPTION "Sensory processing")
set(CPACK_COMPONENT_PREDICTION_DESCRIPTION "Prediction and inference")
set(CPACK_COMPONENT_MOTOR_DESCRIPTION "Motor control")
set(CPACK_COMPONENT_ENVIRONMENT_DESCRIPTION "Environment simulation")
set(CPACK_COMPONENT_EXPERIMENT_DESCRIPTION "Experimental systems")
set(CPACK_COMPONENT_AGENT_DESCRIPTION "Agent systems")

# Installation paths
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/packages/")

# Packaging format options
include(CPackRPM)
include(CPackDEB)
include(CPackTGZ)
include(CPackNSIS)

# RPM options
set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_GROUP science/biology)
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})
set(CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_URL})
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_REQUIRES "gcc-c++ >= 9.1")

# DEB options
set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")
set(CPACK_DEBIAN_PACKAGE_PROVIDES ${CPACK_PACKAGE_NAME})

# TGZ options
set(CPACK_GENERATOR "TGZ;DEB;RPM")

# Control panel options (Windows)
if(WIN32)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${WIN32})")
endif()