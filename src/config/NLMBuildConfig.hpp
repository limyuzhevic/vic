#pragma once

// NLM Build Configuration
// 
// This file contains build configuration options for the NLM project.
// It demonstrates the advanced build system and configuration options.

// Build configuration options
#define NLM_BUILD_TYPE_RELEASE 1
#define NLM_BUILD_TYPE_DEBUG 2
#define NLM_BUILD_TYPE_RELWITHDEBINFO 3
#define NLM_BUILD_TYPE_MINSIZEREL 4

#ifndef NLM_BUILD_TYPE
#define NLM_BUILD_TYPE NLM_BUILD_TYPE_RELEASE
#endif

// Feature configuration
#ifndef ENABLE_ADVANCED_FEATURES
#define ENABLE_ADVANCED_FEATURES 1
#endif

#ifndef ENABLE_MEMORY_POOLS
#define ENABLE_MEMORY_POOLS 0
#endif

#ifndef ENABLE_SIMD_VECTORIZATION
#define ENABLE_SIMD_VECTORIZATION 0
#endif

#ifndef ENABLE_PARALLEL_PROCESSING
#define ENABLE_PARALLEL_PROCESSING 0
#endif

#ifndef ENABLE_CHECKPOINTING
#define ENABLE_CHECKPOINTING 0
#endif

#ifndef ENABLE_REPLAY
#define ENABLE_REPLAY 0
#endif

#ifndef ENABLE_DEVELOPMENT
#define ENABLE_DEVELOPMENT 0
#endif

#ifndef ENABLE_PREDICTION
#define ENABLE_PREDICTION 0
#endif

#ifndef ENABLE_MEMORY_INTEGRATION
#define ENABLE_MEMORY_INTEGRATION 0
#endif

#ifndef ENABLE_COGNITION_INTEGRATION
#define ENABLE_COGNITION_INTEGRATION 0
#endif

#ifndef ENABLE_PERFORMANCE_MONITORING
#define ENABLE_PERFORMANCE_MONITORING 0
#endif

#ifndef ENABLE_ADVANCED_LOGGING
#define ENABLE_ADVANCED_LOGGING 0
#endif

#ifndef ENABLE_METALEARNING
#define ENABLE_METALEARNING 0
#endif

#ifndef ENABLE_TRANSFER_LEARNING
#define ENABLE_TRANSFER_LEARNING 0
#endif

#ifndef ENABLE_CURRICULUM_LEARNING
#define ENABLE_CURRICULUM_LEARNING 0
#endif

#ifndef ENABLE_PLUGIN_SYSTEM
#define ENABLE_PLUGIN_SYSTEM 0
#endif

#ifndef ENABLE_EXPERIMENTAL_FEATURES
#define ENABLE_EXPERIMENTAL_FEATURES 0
#endif

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
#define NLM_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define NLM_PLATFORM_LINUX 1
#elif defined(__APPLE__) || defined(__MACH__)
#define NLM_PLATFORM_MACOS 1
#else
#define NLM_PLATFORM_UNKNOWN 1
#endif

// Compiler detection
#if defined(_MSC_VER)
#define NLM_COMPILER_MSVC 1
#elif defined(__GNUC__) && !defined(__clang__)
#define NLM_COMPILER_GCC 1
#elif defined(__clang__)
#define NLM_COMPILER_CLANG 1
#else
#define NLM_COMPILER_UNKNOWN 1
#endif

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64)
#define NLM_ARCH_X64 1
#elif defined(__i386__) || defined(_M_IX86)
#define NLM_ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define NLM_ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
#define NLM_ARCH_ARM 1
#else
#define NLM_ARCH_UNKNOWN 1
#endif

// Build configuration macros
#if NLM_BUILD_TYPE == NLM_BUILD_TYPE_RELEASE
#define NLM_DEBUG 0
#define NLM_OPTIMIZE_FOR_SPEED 1
#define NLM_OPTIMIZE_FOR_SIZE 0
#elif NLM_BUILD_TYPE == NLM_BUILD_TYPE_DEBUG
#define NLM_DEBUG 1
#define NLM_OPTIMIZE_FOR_SPEED 0
#define NLM_OPTIMIZE_FOR_SIZE 0
#elif NLM_BUILD_TYPE == NLM_BUILD_TYPE_RELWITHDEBINFO
#define NLM_DEBUG 1
#define NLM_OPTIMIZE_FOR_SPEED 1
#define NLM_OPTIMIZE_FOR_SIZE 0
#elif NLM_BUILD_TYPE == NLM_BUILD_TYPE_MINSIZEREL
#define NLM_DEBUG 0
#define NLM_OPTIMIZE_FOR_SPEED 0
#define NLM_OPTIMIZE_FOR_SIZE 1
#endif

// Feature detection macros
#if ENABLE_ADVANCED_FEATURES
#define NLM_FEATURE_ADVANCED_CONFIG 1
#else
#define NLM_FEATURE_ADVANCED_CONFIG 0
#endif

#if ENABLE_MEMORY_POOLS
#define NLM_FEATURE_MEMORY_POOLS 1
#else
#define NLM_FEATURE_MEMORY_POOLS 0
#endif

#if ENABLE_SIMD_VECTORIZATION
#define NLM_FEATURE_SIMD 1
#else
#define NLM_FEATURE_SIMD 0
#endif

#if ENABLE_PARALLEL_PROCESSING
#define NLM_FEATURE_PARALLEL 1
#else
#define NLM_FEATURE_PARALLEL 0
#endif

#if ENABLE_CHECKPOINTING
#define NLM_FEATURE_CHECKPOINTING 1
#else
#define NLM_FEATURE_CHECKPOINTING 0
#endif

#if ENABLE_REPLAY
#define NLM_FEATURE_REPLAY 1
#else
#define NLM_FEATURE_REPLAY 0
#endif

#if ENABLE_DEVELOPMENT
#define NLM_FEATURE_DEVELOPMENT 1
#else
#define NLM_FEATURE_DEVELOPMENT 0
#endif

#if ENABLE_PREDICTION
#define NLM_FEATURE_PREDICTION 1
#else
#define NLM_FEATURE_PREDICTION 0
#endif

#if ENABLE_MEMORY_INTEGRATION
#define NLM_FEATURE_MEMORY_INTEGRATION 1
#else
#define NLM_FEATURE_MEMORY_INTEGRATION 0
#endif

#if ENABLE_COGNITION_INTEGRATION
#define NLM_FEATURE_COGNITION_INTEGRATION 1
#else
#define NLM_FEATURE_COGNITION_INTEGRATION 0
#endif

#if ENABLE_PERFORMANCE_MONITORING
#define NLM_FEATURE_PERFORMANCE_MONITORING 1
#else
#define NLM_FEATURE_PERFORMANCE_MONITORING 0
#endif

#if ENABLE_ADVANCED_LOGGING
#define NLM_FEATURE_ADVANCED_LOGGING 1
#else
#define NLM_FEATURE_ADVANCED_LOGGING 0
#endif

#if ENABLE_METALEARNING
#define NLM_FEATURE_METALEARNING 1
#else
#define NLM_FEATURE_METALEARNING 0
#endif

#if ENABLE_TRANSFER_LEARNING
#define NLM_FEATURE_TRANSFER_LEARNING 1
#else
#define NLM_FEATURE_TRANSFER_LEARNING 0
#endif

#if ENABLE_CURRICULUM_LEARNING
#define NLM_FEATURE_CURRICULUM_LEARNING 1
#else
#define NLM_FEATURE_CURRICULUM_LEARNING 0
#endif

#if ENABLE_PLUGIN_SYSTEM
#define NLM_FEATURE_PLUGIN_SYSTEM 1
#else
#define NLM_FEATURE_PLUGIN_SYSTEM 0
#endif

#if ENABLE_EXPERIMENTAL_FEATURES
#define NLM_FEATURE_EXPERIMENTAL 1
#else
#define NLM_FEATURE_EXPERIMENTAL 0
#endif

// Platform-specific configuration
#if NLM_PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#endif

#if NLM_PLATFORM_LINUX || NLM_PLATFORM_MACOS
#include <unistd.h>
#include <sys/types.h>
#endif

// Memory configuration
#define NLM_DEFAULT_MEMORY_POOL_SIZE (1024 * 1024)  // 1MB
#define NLM_MAX_MEMORY_POOL_SIZE (64 * 1024 * 1024)  // 64MB
#define NLM_DEFAULT_MEMORY_LIMIT 0.9f  // 90% of system memory

// Performance configuration
#if NLM_ARCH_X64 || NLM_ARCH_X86
#define NLM_DEFAULT_THREAD_COUNT std::thread::hardware_concurrency()
#elif NLM_ARCH_ARM64 || NLM_ARCH_ARM
#define NLM_DEFAULT_THREAD_COUNT 4
#else
#define NLM_DEFAULT_THREAD_COUNT 2
#endif

#define NLM_MAX_PARALLEL_TASKS 8
#define NLM_DEFAULT_CPU_AFFINITY 1.0f

// Learning configuration
#define NLM_DEFAULT_LEARNING_RATE 0.01f
#define NLM_DEFAULT_DECAY_RATE 0.999f
#define NLM_DEFAULT_EXPLORATION_RATE 0.1f
#define NLM_DEFAULT_EXPLOITATION_RATE 0.9f
#define NLM_DEFAULT_CURRICULUM_STEPS 10

// Safety configuration
#define NLM_DEFAULT_SAFETY_THRESHOLD 0.9f
#define NLM_DEFAULT_MAX_RESOURCE_USAGE 0.8f
#define NLM_DEFAULT_ROLLBACK_TIMEOUT 60.0f  // seconds

// Monitoring configuration
#define NLM_DEFAULT_MONITORING_INTERVAL 1.0  // seconds
#define NLM_DEFAULT_METRICS_RETENTION 1000  // data points
#define NLM_DEFAULT_API_TIMEOUT 30.0f  // seconds

// Integration configuration
#define NLM_DEFAULT_MAX_RETRY_ATTEMPTS 3
#define NLM_DEFAULT_PLUGIN_DIRECTORY "plugins"

// Advanced learning configuration
#define NLM_DEFAULT_HEBBIAN_STRENGTH 0.1f
#define NLM_DEFAULT_HOMEOSTATIC_TARGET 1.0f
#define NLM_DEFAULT_SYNAPTIC_TAG_WINDOW 10.0f  // minutes
#define NLM_DEFAULT_REPLAY_STRENGTH 0.5f

// Cognitive configuration
#define NLM_DEFAULT_REASONING_DEPTH 3
#define NLM_DEFAULT_ANALOGY_THRESHOLD 0.7f
#define NLM_DEFAULT_METACOGNITION_INTERVAL 60.0f  // seconds
#define NLM_DEFAULT_CONSCIOUSNESS_THRESHOLD 0.8f

// Social learning configuration
#define NLM_DEFAULT_IMITATION_CONFIDENCE 0.7f
#define NLM_DEFAULT_CULTURAL_TRANSMISSION_RATE 0.3f
#define NLM_DEFAULT_COOPERATION_THRESHOLD 0.5f

// Adaptive configuration
#define NLM_DEFAULT_ADAPTATION_RATE 0.1f
#define NLM_DEFAULT_RESOURCE_REEVALUATION_INTERVAL 30.0f  // seconds
#define NLM_DEFAULT_PERFORMANCE_THRESHOLD 0.95f

// Configuration management
#define NLM_DEFAULT_CONFIG_VERSION 2
#define NLM_DEFAULT_CONFIG_PROFILE "standard"

// Optimization configuration
#define NLM_DEFAULT_PRUNING_THRESHOLD 0.01f
#define NLM_DEFAULT_EFFICIENCY_TARGET 0.9f
#define NLM_DEFAULT_COMPRESSION_LEVEL 6  // 1-9

// Experimental configuration
#define NLM_DEFAULT_EXPERIMENTAL_FEATURE_PROBABILITY 0.1f
#define NLM_DEFAULT_RESEARCH_MODE_SENSITIVITY 0.7f

// Build feature detection
#if NLM_COMPILER_GCC || NLM_COMPILER_CLANG
#define NLM_COMPILER_SUPPORTS_CXX20 1
#elif NLM_COMPILER_MSVC
#define NLM_COMPILER_SUPPORTS_CXX20 1
#else
#define NLM_COMPILER_SUPPORTS_CXX20 0
#endif

#if NLM_COMPILER_SUPPORTS_CXX20
#define NLM_CXX20_SUPPORT 1
#else
#define NLM_CXX20_SUPPORT 0
#endif

// SIMD feature detection
#if NLM_ARCH_X64
#if NLM_COMPILER_GCC || NLM_COMPILER_CLANG
#define NLM_SUPPORTS_AVX 1
#define NLM_SUPPORTS_SSE 1
#elif NLM_COMPILER_MSVC
#define NLM_SUPPORTS_SSE 1
#endif
#elif NLM_ARCH_ARM64
#define NLM_SUPPORTS_NEON 1
#endif

// Memory pool feature detection
#if NLM_FEATURE_MEMORY_POOLS
#define NLM_SUPPORTS_MEMORY_POOLS 1
#else
#define NLM_SUPPORTS_MEMORY_POOLS 0
#endif

// Parallel processing feature detection
#if NLM_FEATURE_PARALLEL
#define NLM_SUPPORTS_PARALLEL 1
#else
#define NLM_SUPPORTS_PARALLEL 0
#endif

// Configuration validation
#define NLM_VALIDATE_CONFIGURATION 1
#define NLM_CONFIGURATION_STRICT_VALIDATION 0

// Logging configuration
#define NLM_DEFAULT_LOG_LEVEL "info"
#define NLM_DEFAULT_LOG_FILE "nlm.log"
#define NLM_DEFAULT_LOG_ROTATION_SIZE 10 * 1024 * 1024  // 10MB
#define NLM_DEFAULT_LOG_RETENTION 7  // days

// Performance monitoring
#define NLM_ENABLE_PERFORMANCE_PROFILING 1
#define NLM_ENABLE_REAL_TIME_MONITORING 1
#define NLM_ENABLE_METRICS_COLLECTION 1
#define NLM_ENABLE_ANOMALY_DETECTION 0

// Safety features
#define NLM_ENABLE_SAFETY_CONSTRAINTS 1
#define NLM_ENABLE_BOUNDS_CHECKING 1
#define NLM_ENABLE_RESOURCE_LIMITS 1
#define NLM_ENABLE_FAIL_SAFE_MODE 1
#define NLM_ENABLE_ROLLBACK_PROTECTION 1

// Integration features
#define NLM_ENABLE_CROSS_SYSTEM_INTEGRATION 1
#define NLM_ENABLE_EXTERNAL_APIS 0
#define NLM_ENABLE_PLUGIN_SYSTEM 1
#define NLM_ENABLE_EXTENSIBILITY 1
#define NLM_ENABLE_MODULAR_ARCHITECTURE 1

// Advanced learning features
#define NLM_ENABLE_HEBBIAN_LEARNING 1
#define NLM_ENABLE_HOMEOSTATIC_PLASTICITY 1
#define NLM_ENABLE_SYNAPTIC_TAGGING 0
#define NLM_ENABLE_REPLAY_CONSOLIDATION 1
#define NLM_ENABLE_SLEEP_CONSOLIDATION 0

// Cognitive features
#define NLM_ENABLE_ABSTRACT_REASONING 0
#define NLM_ENABLE_ANALOGICAL_THINKING 0
#define NLM_ENABLE_METACOGNITION 0
#define NLM_ENABLE_CONSCIOUSNESS_SIMULATION 0
#define NLM_ENABLE_THEORY_OF_MIND 0

// Social learning features
#define NLM_ENABLE_SOCIAL_LEARNING 0
#define NLM_ENABLE_IMITATION_LEARNING 0
#define NLM_ENABLE_CULTURAL_TRANSMISSION 0
#define NLM_ENABLE_COOPERATIVE_LEARNING 0

// Adaptive features
#define NLM_ENABLE_ADAPTIVE_ARCHITECTURE 1
#define NLM_ENABLE_DYNAMIC_RESOURCE_ALLOCATION 1
#define NLM_ENABLE_AUTO_SCALING 0
#define NLM_ENABLE_SELF_OPTIMIZATION 0

// Configuration management features
#define NLM_ENABLE_CONFIGURATION_MANAGEMENT 1
#define NLM_ENABLE_CONFIGURATION_VERSIONING 1
#define NLM_ENABLE_CONFIGURATION_ROLLBACK 1
#define NLM_ENABLE_CONFIGURATION_EXPORT 1

// Optimization features
#define NLM_ENABLE_NEURAL_PRUNING 1
#define NLM_ENABLE_EFFICIENCY_OPTIMIZATION 1
#define NLM_ENABLE_COMPRESSION 1
#define NLM_ENABLE_ASYNCHRONOUS_PROCESSING 1

// Experimental features
#define NLM_ENABLE_EXPERIMENTAL_FEATURES 0
#define NLM_ENABLE_RESEARCH_MODE 0
#define NLM_ENABLE_DEBUG_FEATURES 0

// Build feature macros
#if NLM_BUILD_TYPE == NLM_BUILD_TYPE_DEBUG
#define NLM_BUILD_CONFIG_NAME "Debug"
#elif NLM_BUILD_TYPE == NLM_BUILD_TYPE_RELEASE
#define NLM_BUILD_CONFIG_NAME "Release"
#elif NLM_BUILD_TYPE == NLM_BUILD_TYPE_RELWITHDEBINFO
#define NLM_BUILD_CONFIG_NAME "RelWithDebInfo"
#elif NLM_BUILD_TYPE == NLM_BUILD_TYPE_MINSIZEREL
#define NLM_BUILD_CONFIG_NAME "MinSizeRel"
#endif

// Version information
#define NLM_VERSION_MAJOR 0
#define NLM_VERSION_MINOR 1
#define NLM_VERSION_PATCH 0
#define NLM_VERSION_STRING "0.1.0"

// Project information
#define NLM_PROJECT_NAME "NLM"
#define NLM_PROJECT_DESCRIPTION "An Experimental Artificial Developmental Brain"
#define NLM_PROJECT_HOMEPAGE "https://github.com/yourusername/nlm"
#define NLM_PROJECT_AUTHOR "Research Project"

// Copyright information
#define NLM_COPYRIGHT_YEAR 2024
#define NLM_COPYRIGHT_HOLDER "NLM Research Team"

// License information
#define NLM_LICENSE "MIT"
#define NLM_LICENSE_TEXT "MIT License\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."

// Build configuration summary
#if defined(NLM_BUILD_CONFIG_SUMMARY)
const char* getBuildConfigSummary() {
    std::string summary = "NLM Build Configuration\n";
    summary += "========================\n\n";
    summary += "Build Type: " + std::string(NLM_BUILD_CONFIG_NAME) + "\n";
    summary += "Platform: ";
#if NLM_PLATFORM_WINDOWS
    summary += "Windows\n";
#elif NLM_PLATFORM_LINUX
    summary += "Linux\n";
#elif NLM_PLATFORM_MACOS
    summary += "macOS\n";
#else
    summary += "Unknown\n";
#endif
    summary += "Compiler: ";
#if NLM_COMPILER_MSVC
    summary += "MSVC\n";
#elif NLM_COMPILER_GCC
    summary += "GCC\n";
#elif NLM_COMPILER_CLANG
    summary += "Clang\n";
#else
    summary += "Unknown\n";
#endif
    summary += "Architecture: ";
#if NLM_ARCH_X64
    summary += "x86_64\n";
#elif NLM_ARCH_X86
    summary += "x86\n";
#elif NLM_ARCH_ARM64
    summary += "ARM64\n";
#elif NLM_ARCH_ARM
    summary += "ARM\n";
#else
    summary += "Unknown\n";
#endif
    summary += "Features:\n";
#if NLM_FEATURE_ADVANCED_CONFIG
    summary += "  - Advanced Configuration\n";
#endif
#if NLM_FEATURE_MEMORY_POOLS
    summary += "  - Memory Pools\n";
#endif
#if NLM_FEATURE_SIMD
    summary += "  - SIMD Vectorization\n";
#endif
#if NLM_FEATURE_PARALLEL
    summary += "  - Parallel Processing\n";
#endif
#if NLM_FEATURE_CHECKPOINTING
    summary += "  - Checkpointing\n";
#endif
#if NLM_FEATURE_REPLAY
    summary += "  - Memory Replay\n";
#endif
#if NLM_FEATURE_DEVELOPMENT
    summary += "  - Development\n";
#endif
#if NLM_FEATURE_PREDICTION
    summary += "  - Prediction\n";
#endif
#if NLM_FEATURE_MEMORY_INTEGRATION
    summary += "  - Memory Integration\n";
#endif
#if NLM_FEATURE_COGNITION_INTEGRATION
    summary += "  - Cognition Integration\n";
#endif
#if NLM_FEATURE_PERFORMANCE_MONITORING
    summary += "  - Performance Monitoring\n";
#endif
#if NLM_FEATURE_ADVANCED_LOGGING
    summary += "  - Advanced Logging\n";
#endif
#if NLM_FEATURE_METALEARNING
    summary += "  - Meta Learning\n";
#endif
#if NLM_FEATURE_TRANSFER_LEARNING
    summary += "  - Transfer Learning\n";
#endif
#if NLM_FEATURE_CURRICULUM_LEARNING
    summary += "  - Curriculum Learning\n";
#endif
#if NLM_FEATURE_PLUGIN_SYSTEM
    summary += "  - Plugin System\n";
#endif
#if NLM_FEATURE_EXPERIMENTAL
    summary += "  - Experimental Features\n";
#endif
    summary += "\n";
    return summary.c_str();
}
#endif
