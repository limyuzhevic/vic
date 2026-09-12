#include "DebugUtils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <thread>
#include <chrono>
#include <random>
#include <stdexcept>
#include <limits>

namespace nlm {

DebugUtils::DebugUtils() = default;

DebugUtils::~DebugUtils() = default;

DebugUtils::DebugUtils(DebugUtils&&) noexcept = default;

DebugUtils& DebugUtils::operator=(DebugUtils&&) noexcept = default;

void DebugUtils::printStackTrace() {
    #ifdef __GNUC__
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** symbols = backtrace_symbols(callstack, frames);
        
        std::cout << "=== Stack Trace (" << frames << " frames) ===" << std::endl;
        if (symbols) {
            for (int i = 0; i < frames; ++i) {
                std::cout << i << ": " << symbols[i] << std::endl;
            }
            std::free(symbols);
        }
    #else
        std::cout << "Stack trace not available on this platform" << std::endl;
    #endif
}

std::string DebugUtils::getStackTrace() {
    #ifdef __GNUC__
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** symbols = backtrace_symbols(callstack, frames);
        
        std::stringstream ss;
        ss << "=== Stack Trace (" << frames << " frames) ===" << std::endl;
        if (symbols) {
            for (int i = 0; i < frames; ++i) {
                ss << i << ": " << symbols[i] << std::endl;
            }
            std::free(symbols);
        }
        return ss.str();
    #else
        return "Stack trace not available on this platform";
    #endif
}
}

void DebugUtils::printMemoryUsage() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        std::cout << "=== Memory Usage ===" << std::endl;
        std::cout << "Max RSS: " << usage.ru_maxrss / 1024 << " MB" << std::endl;
        std::cout << "User time: " << usage.ru_utime.tv_sec << "s " << usage.ru_utime.tv_usec << "μs" << std::endl;
        std::cout << "System time: " << usage.ru_stime.tv_sec << "s " << usage.ru_stime.tv_usec << "μs" << std::endl;
    } else {
        std::cout << "Failed to get memory usage" << std::endl;
    }
}

std::string DebugUtils::getMemoryUsageInfo() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        std::stringstream ss;
        ss << "=== Memory Usage ===" << std::endl;
        ss << "Max RSS: " << usage.ru_maxrss / 1024 << " MB" << std::endl;
        ss << "User time: " << usage.ru_utime.tv_sec << "s " << usage.ru_utime.tv_usec << "μs" << std::endl;
        ss << "System time: " << usage.ru_stime.tv_sec << "s " << usage.ru_stime.tv_usec << "μs" << std::endl;
        return ss.str();
    } else {
        return "Failed to get memory usage";
    }
}
}

void DebugUtils::printThreadInfo() {
    std::cout << "=== Thread Information ===" << std::endl;
    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl;
    
    // Try to get thread IDs
    auto currentThreadId = std::this_thread::get_id();
    std::cout << "Current thread ID: " << currentThreadId << std::endl;
    
    // Note: We can't easily enumerate all threads from user space
    // This is a simplified version
}

std::string DebugUtils::getThreadInfo() {
    std::stringstream ss;
    ss << "=== Thread Information ===" << std::endl;
    ss << "Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl;
    ss << "Current thread ID: " << std::this_thread::get_id() << std::endl;
    return ss.str();
}

std::string DebugUtils::getWorkingDirectory() {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::exception& e) {
        return "Failed to get working directory: " + std::string(e.what());
    }
}

bool DebugUtils::setWorkingDirectory(const std::string& path) {
    try {
        std::filesystem::current_path(path);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugUtils::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugUtils::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

std::string DebugUtils::getFileInfo(const std::string& path) {
    try {
        auto fileInfo = std::filesystem::status(path);
        
        std::stringstream ss;
        ss << "=== File Information ===" << std::endl;
        ss << "Path: " << path << std::endl;
        ss << "Size: " << std::filesystem::file_size(path) << " bytes" << std::endl;
        ss << "Permissions: " << std::oct << fileInfo.permissions() << std::dec << std::endl;
        ss << "Last modified: " << std::filesystem::last_write_time(path) << std::endl;
        
        return ss.str();
    } catch (const std::exception& e) {
        return "Failed to get file info: " + std::string(e.what());
    }
}

bool DebugUtils::compareFiles(const std::string& path1, const std::string& path2, bool binary) {
    try {
        if (binary) {
            return std::filesystem::equivalent(path1, path2);
        } else {
            std::ifstream file1(path1);
            std::ifstream file2(path2);
            
            if (!file1.is_open() || !file2.is_open()) {
                return false;
            }
            
            return std::equal(std::istreambuf_iterator<char>(file1),
                            std::istreambuf_iterator<char>(),
                            std::istreambuf_iterator<char>(file2));
        }
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> DebugUtils::getDirectoryListing(const std::string& path) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        // Return empty list on error
        std::cerr << "Failed to get directory listing for path: " << path << ". Error: " << e.what() << std::endl;
    }
    
    return files;
}

std::string DebugUtils::getPlatformInfo() {
    std::stringstream ss;
    ss << "=== Platform Information ===" << std::endl;
    ss << "Operating System: " << getOSInfo() << std::endl;
    ss << "Architecture: " << getArchitectureInfo() << std::endl;
    ss << "Compiler Info: " << getCompilerInfo() << std::endl;
    ss << "Standard Library: " << getSTLInfo() << std::endl;
    ss << "Runtime Info: " << getRuntimeInfo() << std::endl;
    return ss.str();
}

std::string DebugUtils::getOSInfo() {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#elif __linux__
    return "Linux";
#elif __FreeBSD__
    return "FreeBSD";
#elif __NetBSD__
    return "NetBSD";
#elif __OpenBSD__
    return "OpenBSD";
#else
    return "Unknown";
#endif
}

std::string DebugUtils::getArchitectureInfo() {
#ifdef _WIN64
    return "x64 (Windows)";
#elif _WIN32
    return "x86 (Windows)";
#elif __x86_64__
    return "x64 (Unix-like)";
#elif __i386__
    return "x86 (Unix-like)";
#elif __aarch64__
    return "ARM64 (Unix-like)";
#elif __arm__
    return "ARM (Unix-like)";
#elif __powerpc64__
    return "PowerPC64 (Unix-like)";
#elif __powerpc__
    return "PowerPC (Unix-like)";
#else
    return "Unknown";
#endif
}

std::string DebugUtils::getCompilerInfo() {
#ifdef _MSC_VER
    return "Microsoft Visual C++ " + std::to_string(_MSC_VER);
#elif __GNUC__
    return "GCC " + std::string(__VERSION__);
#elif __clang__
    return "Clang " + std::to_string(__clang_version__);
#elif __INTEL_COMPILER
    return "Intel C++ Compiler " + std::to_string(__INTEL_COMPILER);
#elif __HPCC__
    return "HP C/C++ Compiler";
#elif __SUNPROCC
    return "Sun C Compiler";
#else
    return "Unknown";
#endif
}

std::string DebugUtils::getSTLInfo() {
#if __cplusplus >= 202002L
    return "C++20";
#elif __cplusplus >= 201703L
    return "C++17";
#elif __cplusplus >= 201402L
    return "C++14";
#elif __cplusplus >= 199711L
    return "C++98";
#else
    return "Unknown";
#endif
}

std::string DebugUtils::getRuntimeInfo() {
#ifdef __cplusplus
    return "C++ Runtime";
#else
    return "C Runtime";
#endif
}

std::string DebugUtils::formatMemoryAddress(void* ptr) {
    std::stringstream ss;
    ss << std::hex << std::showbase << ptr;
    return ss.str();
}

std::string DebugUtils::generateId() {
    std::stringstream ss;
    
    // Generate a random ID using current time and process ID
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 0xFFFFFFFF);
    
    ss << std::hex << timestamp << "-" << dis(gen) << "-" << getpid() << "-" 
       << std::thread::hardware_concurrency();
    
    return ss.str();
}

std::string DebugUtils::formatDuration(double duration) {
    std::stringstream ss;
    
    if (duration < 0.001) {
        ss << std::fixed << std::setprecision(3) << (duration * 1000000) << " μs";
    } else if (duration < 1.0) {
        ss << std::fixed << std::setprecision(3) << (duration * 1000) << " ms";
    } else if (duration < 60.0) {
        ss << std::fixed << std::setprecision(3) << duration << " s";
    } else if (duration < 3600.0) {
        ss << std::fixed << std::setprecision(3) << (duration / 60.0) << " min";
    } else {
        ss << std::fixed << std::setprecision(3) << (duration / 3600.0) << " h";
    }
    
    return ss.str();
}

std::string DebugUtils::formatBytes(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024.0) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    } else if (bytes < 1024.0 * 1024.0 * 1024.0 * 1024.0) {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
    } else {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0)) + " TB";
    }
}

void DebugUtils::validateArguments(int argc, char** argv, const std::string& expectedFormat) {
    std::cout << "=== Command Line Arguments ===" << std::endl;
    std::cout << "Argument count: " << argc << std::endl;
    
    for (int i = 0; i < argc; ++i) {
        std::cout << i << ": " << argv[i] << std::endl;
    }
    
    // Basic validation
    if (expectedFormat == "--key=value") {
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-') {
                if (strchr(argv[i], '=') == nullptr) {
                    std::cout << "Warning: Argument " << i << " does not use expected format " << expectedFormat << std::endl;
                }
            }
        }
    } else if (expectedFormat == "--key value") {
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-') {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // Next argument is the value
                    i++;
                } else {
                    std::cout << "Warning: Argument " << i << " does not have a value" << std::endl;
                }
            }
        }
    }
}

std::unordered_map<std::string, std::string> DebugUtils::parseCommandLine(int argc, char** argv) {
    std::unordered_map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args[key] = value;
            } else {
                // --key format (flag)
                args[arg.substr(2)] = "true";
            }
        } else if (arg[0] == '-') {
            // -key value format
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args[key] = argv[++i];
            } else {
                args[key] = "true";
            }
        } else {
            // Positional argument
            args[std::to_string(i - 1)] = arg;
        }
    }
    
    return args;
}

void DebugUtils::printSystemInfo() {
    std::cout << getPlatformInfo();
}

void DebugUtils::printHelp(const std::unordered_map<std::string, std::string>& commands) {
    std::cout << "=== Available Commands ===" << std::endl;
    for (const auto& [cmd, desc] : commands) {
        std::cout << cmd << ": " << desc << std::endl;
    }
}

void DebugUtils::validateEnvironment() {
    std::cout << "=== Environment Validation ===" << std::endl;
    
    // Check file system
    try {
        std::filesystem::path tempPath = std::filesystem::temp_directory_path();
        std::cout << "Temp directory: " << tempPath.string() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not get temp directory: " << e.what() << std::endl;
    }
    
    // Check memory
    printMemoryUsage();
    
    // Check CPU
    std::cout << "CPU cores: " << std::thread::hardware_concurrency() << std::endl;
    
    // Check platform
    std::cout << "Platform: " << getOSInfo() << std::endl;
    
    // Check compiler
    std::cout << "Compiler: " << getCompilerInfo() << std::endl;
    
    std::cout << "=== Environment validation complete ===" << std::endl;
}

void DebugUtils::initializeDebugEnvironment() {
    // Set up debug flags, logging, etc.
    
    // Set up signal handlers
    // Set up logging
    // Set up performance monitoring
    // Set up memory tracking
    
    std::cout << "Debug environment initialized" << std::endl;
}

void DebugUtils::cleanupDebugEnvironment() {
    // Clean up debug resources
    // Flush logs
    // Stop monitoring
    // Free memory
    
    std::cout << "Debug environment cleaned up" << std::endl;
}

std::string DebugUtils::getErrorString(int code) {
    // Common error codes
    switch (code) {
        case 0:
            return "Success";
        case 1:
            return "General error";
        case 2:
            return "File not found";
        case 3:
            return "Permission denied";
        case 4:
            return "Invalid argument";
        case 5:
            return "Out of memory";
        case 6:
            return "Not implemented";
        case 7:
            return "Network error";
        case 8:
            return "Timeout";
        case 9:
            return "Resource busy";
        case 10:
            return "Invalid format";
        default:
            return "Unknown error " + std::to_string(code);
    }
}

void DebugUtils::setErrorMessage(int code, const std::string& message) {
    // Store error message in a map
    // In a real implementation, this would use a thread-safe map
    static std::unordered_map<int, std::string> errorMessages;
    errorMessages[code] = message;
}

} // namespace nlm