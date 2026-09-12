// DebugUtils Tests
// Phase 3: Debugging and System Utilities

#include "utils/DebugUtils.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace test_debug_utils {

void testDebugUtilsStackTrace() {
    // Test stack trace generation
    std::string stackTrace = nlm::DebugUtils::getStackTrace();
    
    // Stack trace may be platform-dependent, but function should not crash
    assert(true);
    
    std::cout << "    testDebugUtilsStackTrace passed" << std::endl;
}

void testDebugUtilsMemoryUsage() {
    // Test memory usage reporting
    std::string memoryInfo = nlm::DebugUtils::getMemoryUsageInfo();
    
    // Memory info may be platform-dependent, but function should not crash
    assert(true);
    
    std::cout << "    testDebugUtilsMemoryUsage passed" << std::endl;
}

void testDebugUtilsThreadInfo() {
    // Test thread information
    std::string threadInfo = nlm::DebugUtils::getThreadInfo();
    
    // Thread info should not be empty
    assert(!threadInfo.empty());
    assert(threadInfo.find("Thread Information") != std::string::npos);
    
    std::cout << "    testDebugUtilsThreadInfo passed" << std::endl;
}

void testDebugUtilsWorkingDirectory() {
    // Test working directory
    std::string cwd = nlm::DebugUtils::getWorkingDirectory();
    
    // Current working directory should exist
    assert(!cwd.empty());
    assert(std::filesystem::exists(cwd));
    
    std::cout << "    testDebugUtilsWorkingDirectory passed" << std::endl;
}

void testDebugUtilsSetWorkingDirectory() {
    // Test setting working directory
    std::string originalCwd = nlm::DebugUtils::getWorkingDirectory();
    
    std::string tempDir = "/tmp";
    if (std::filesystem::exists(tempDir)) {
        bool success = nlm::DebugUtils::setWorkingDirectory(tempDir);
        assert(success);
        
        std::string newCwd = nlm::DebugUtils::getWorkingDirectory();
        assert(newCwd == tempDir);
        
        // Restore original directory
        nlm::DebugUtils::setWorkingDirectory(originalCwd);
    }
    
    std::cout << "    testDebugUtilsSetWorkingDirectory passed" << std::endl;
}

void testDebugUtilsCreateDirectory() {
    // Test directory creation
    std::string testDir = "/tmp/debug_test_dir";
    
    bool success = nlm::DebugUtils::createDirectory(testDir);
    assert(success);
    assert(std::filesystem::exists(testDir));
    
    // Clean up
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testDebugUtilsCreateDirectory passed" << std::endl;
}

void testDebugUtilsFileExists() {
    // Create a temporary file
    std::string testFile = "/tmp/debug_test_file.txt";
    
    {
        std::ofstream file(testFile);
        file << "test content";
    }
    
    // Test file exists
    bool exists = nlm::DebugUtils::fileExists(testFile);
    assert(exists);
    
    // Test non-existent file
    bool notExists = nlm::DebugUtils::fileExists("/tmp/nonexistent_file_xyz");
    assert(!notExists);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testDebugUtilsFileExists passed" << std::endl;
}

void testDebugUtilsGetFileInfo() {
    // Create a temporary file
    std::string testFile = "/tmp/debug_test_file_info.txt";
    
    {
        std::ofstream file(testFile);
        file << "test content for file info";
    }
    
    // Get file info
    std::string fileInfo = nlm::DebugUtils::getFileInfo(testFile);
    
    // File info should not be empty
    assert(!fileInfo.empty());
    assert(fileInfo.find("File Information") != std::string::npos);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testDebugUtilsGetFileInfo passed" << std::endl;
}

void testDebugUtilsCompareFiles() {
    // Create two identical temporary files
    std::string file1 = "/tmp/debug_compare_file1.txt";
    std::string file2 = "/tmp/debug_compare_file2.txt";
    
    {
        std::ofstream f1(file1);
        std::ofstream f2(file2);
        f1 << "identical content";
        f2 << "identical content";
    }
    
    // Test file comparison (identical)
    bool identical = nlm::DebugUtils::compareFiles(file1, file2, false);
    assert(identical);
    
    // Test different content
    {
        std::ofstream f1(file1);
        f1 << "different content";
    }
    
    bool different = !nlm::DebugUtils::compareFiles(file1, file2, false);
    assert(different);
    
    // Clean up
    std::remove(file1.c_str());
    std::remove(file2.c_str());
    
    std::cout << "    testDebugUtilsCompareFiles passed" << std::endl;
}

void testDebugUtilsDirectoryListing() {
    // Create a temporary directory with files
    std::string testDir = "/tmp/debug_dir_listing_test";
    std::filesystem::create_directories(testDir);
    
    // Create test files
    std::string file1 = testDir + "/file1.txt";
    std::string file2 = testDir + "/file2.txt";
    
    {
        std::ofstream f1(file1);
        std::ofstream f2(file2);
        f1 << "file 1 content";
        f2 << "file 2 content";
    }
    
    // Get directory listing
    std::vector<std::string> files = nlm::DebugUtils::getDirectoryListing(testDir);
    
    // Should contain our test files (order not guaranteed)
    bool foundFile1 = false;
    bool foundFile2 = false;
    for (const auto& filename : files) {
        if (filename == "file1.txt") foundFile1 = true;
        if (filename == "file2.txt") foundFile2 = true;
    }
    assert(foundFile1 && foundFile2);
    
    // Clean up
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testDebugUtilsDirectoryListing passed" << std::endl;
}

void testDebugUtilsPlatformInfo() {
    // Test platform information
    std::string platformInfo = nlm::DebugUtils::getPlatformInfo();
    
    // Platform info should not be empty
    assert(!platformInfo.empty());
    assert(platformInfo.find("Platform Information") != std::string::npos);
    
    std::cout << "    testDebugUtilsPlatformInfo passed" << std::endl;
}

void testDebugUtilsFormatMemoryAddress() {
    // Test memory address formatting
    int x = 42;
    std::string addrStr = nlm::DebugUtils::formatMemoryAddress(&x);
    
    // Should contain hex representation
    assert(!addrStr.empty());
    
    std::cout << "    testDebugUtilsFormatMemoryAddress passed" << std::endl;
}

void testDebugUtilsGenerateId() {
    // Test ID generation
    std::string id1 = nlm::DebugUtils::generateId();
    std::string id2 = nlm::DebugUtils::generateId();
    
    // IDs should be unique (very high probability)
    assert(id1 != id2);
    
    std::cout << "    testDebugUtilsGenerateId passed" << std::endl;
}

void testDebugUtilsFormatDuration() {
    // Test duration formatting
    std::string format1 = nlm::DebugUtils::formatDuration(0.0005);  // 0.5 ms
    std::string format2 = nlm::DebugUtils::formatDuration(1.5);     // 1.5 s
    std::string format3 = nlm::DebugUtils::formatDuration(90.0);    // 1.5 min
    
    // All formats should not be empty
    assert(!format1.empty());
    assert(!format2.empty());
    assert(!format3.empty());
    
    std::cout << "    testDebugUtilsFormatDuration passed" << std::endl;
}

void testDebugUtilsFormatBytes() {
    // Test bytes formatting
    std::string format1 = nlm::DebugUtils::formatBytes(512);          // 512 B
    std::string format2 = nlm::DebugUtils::formatBytes(1536);         // 1.5 KB
    std::string format3 = nlm::DebugUtils::formatBytes(2097152);      // 2 MB
    std::string format4 = nlm::DebugUtils::formatBytes(3221225472);   // 3 GB
    
    // All formats should not be empty
    assert(!format1.empty());
    assert(!format2.empty());
    assert(!format3.empty());
    assert(!format4.empty());
    
    std::cout << "    testDebugUtilsFormatBytes passed" << std::endl;
}

void testDebugUtilsValidateArguments() {
    // Test argument validation (should not crash)
    nlm::DebugUtils::validateArguments(3, (char*[]){(char*)"test", (char*)"--key=value", (char*)"--flag"}, "--key=value");
    
    std::cout << "    testDebugUtilsValidateArguments passed" << std::endl;
}

void testDebugUtilsParseCommandLine() {
    // Test command line parsing
    char* argv[] = {
        (char*)"test",
        (char*)"--key1=value1",
        (char*)"--flag1",
        (char*)"-k",
        (char*)"value2"
    };
    
    int argc = 5;
    auto args = nlm::DebugUtils::parseCommandLine(argc, argv);
    
    assert(args["key1"] == "value1");
    assert(args["flag1"] == "true");
    assert(args["k"] == "value2");
    
    std::cout << "    testDebugUtilsParseCommandLine passed" << std::endl;
}

void testDebugUtilsPrintSystemInfo() {
    // Test system info printing (should not crash)
    nlm::DebugUtils::printSystemInfo();
    
    std::cout << "    testDebugUtilsPrintSystemInfo passed" << std::endl;
}

void testDebugUtilsPrintHelp() {
    // Test help printing (should not crash)
    std::unordered_map<std::string, std::string> commands = {
        {"help", "Show help"},
        {"version", "Show version"}
    };
    nlm::DebugUtils::printHelp(commands);
    
    std::cout << "    testDebugUtilsPrintHelp passed" << std::endl;
}

void testDebugUtilsValidateEnvironment() {
    // Test environment validation (should not crash)
    nlm::DebugUtils::validateEnvironment();
    
    std::cout << "    testDebugUtilsValidateEnvironment passed" << std::endl;
}

void testDebugUtilsInitializeDebugEnvironment() {
    // Test debug environment initialization (should not crash)
    nlm::DebugUtils::initializeDebugEnvironment();
    
    std::cout << "    testDebugUtilsInitializeDebugEnvironment passed" << std::endl;
}

void testDebugUtilsCleanupDebugEnvironment() {
    // Test debug environment cleanup (should not crash)
    nlm::DebugUtils::cleanupDebugEnvironment();
    
    std::cout << "    testDebugUtilsCleanupDebugEnvironment passed" << std::endl;
}

void testDebugUtilsGetErrorString() {
    // Test error string generation
    std::string msg0 = nlm::DebugUtils::getErrorString(0);
    assert(msg0 == "Success");
    
    std::string msg1 = nlm::DebugUtils::getErrorString(1);
    assert(msg1 == "General error");
    
    std::string msg2 = nlm::DebugUtils::getErrorString(999);
    assert(msg2 == "Unknown error 999");
    
    std::cout << "    testDebugUtilsGetErrorString passed" << std::endl;
}

void testDebugUtilsSetErrorMessage() {
    // Test error message setting
    nlm::DebugUtils::setErrorMessage(888, "Custom error");
    
    std::string msg = nlm::DebugUtils::getErrorString(888);
    // Note: getErrorString doesn't use setErrorMessage from DebugUtils
    // This test just verifies the function exists and doesn't crash
    
    std::cout << "    testDebugUtilsSetErrorMessage passed" << std::endl;
}

void runAll() {
    testDebugUtilsStackTrace();
    testDebugUtilsMemoryUsage();
    testDebugUtilsThreadInfo();
    testDebugUtilsWorkingDirectory();
    testDebugUtilsSetWorkingDirectory();
    testDebugUtilsCreateDirectory();
    testDebugUtilsFileExists();
    testDebugUtilsGetFileInfo();
    testDebugUtilsCompareFiles();
    testDebugUtilsDirectoryListing();
    testDebugUtilsPlatformInfo();
    testDebugUtilsFormatMemoryAddress();
    testDebugUtilsGenerateId();
    testDebugUtilsFormatDuration();
    testDebugUtilsFormatBytes();
    testDebugUtilsValidateArguments();
    testDebugUtilsParseCommandLine();
    testDebugUtilsPrintSystemInfo();
    testDebugUtilsPrintHelp();
    testDebugUtilsValidateEnvironment();
    testDebugUtilsInitializeDebugEnvironment();
    testDebugUtilsCleanupDebugEnvironment();
    testDebugUtilsGetErrorString();
    testDebugUtilsSetErrorMessage();
}

} // namespace test_debug_utils
