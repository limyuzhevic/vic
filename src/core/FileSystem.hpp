#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

namespace nlm {

// FileSystem utility for file operations and path handling
class FileSystem {
public:
    FileSystem();
    ~FileSystem();
    
    // Disable copying
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;
    
    // Get global file system instance
    static FileSystem& getInstance();
    
    // Path operations
    static std::string normalizePath(const std::string& path);
    static std::string absolutePath(const std::string& path);
    static std::string relativePath(const std::string& path, const std::string& base = "");
    static std::string extension(const std::string& path);
    static std::string filename(const std::string& path);
    static std::string stem(const std::string& path);
    static std::string directory(const std::string& path);
    
    // File operations
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static bool isSymlink(const std::string& path);
    static uintmax_t fileSize(const std::string& path);
    static std::chrono::system_clock::time_point lastModified(const std::string& path);
    static std::chrono::system_clock::time_point created(const std::string& path);
    
    // File read/write operations
    static bool readFile(const std::string& path, std::string& content);
    static bool readFileBinary(const std::string& path, std::vector<uint8_t>& data);
    static bool writeFile(const std::string& path, const std::string& content, bool append = false);
    static bool writeFileBinary(const std::string& path, const std::vector<uint8_t>& data, bool append = false);
    
    // File system operations
    static bool createDirectory(const std::string& path, bool recursive = false);
    static bool removeFile(const std::string& path);
    static bool removeDirectory(const std::string& path, bool recursive = false);
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool moveFile(const std::string& source, const std::string& destination);
    static bool renameFile(const std::string& oldPath, const std::string& newPath);
    
    // Directory operations
    static std::vector<std::string> listDirectory(const std::string& path, bool recursive = false);
    static bool createDirectories(const std::string& path);
    static bool removeDirectories(const std::string& path, bool recursive = false);
    
    // Path validation
    static bool validatePath(const std::string& path, bool mustExist = false);
    static bool validatePathLength(const std::string& path, size_t maxLength = 1024);
    static bool validatePathSecurity(const std::string& path);
    
    // File content operations
    static bool isTextFile(const std::string& path);
    static bool isBinaryFile(const std::string& path);
    static std::string detectFileType(const std::string& path);
    static bool hasTextContent(const std::string& content);
    static bool hasBinaryContent(const std::vector<uint8_t>& data);
    
    // Serialization utilities
    static bool saveStringToFile(const std::string& path, const std::string& content);
    static bool loadStringFromFile(const std::string& path, std::string& content);
    static bool saveBinaryDataToFile(const std::string& path, const std::vector<uint8_t>& data);
    static bool loadBinaryDataFromFile(const std::string& path, std::vector<uint8_t>& data);
    
    // Path resolution and security
    static std::string resolvePath(const std::string& path);
    static bool isSafePath(const std::string& path);
    static bool isPathWithinDirectory(const std::string& path, const std::string& directory);
    
    // File locking
    static bool lockFile(const std::string& path);
    static bool unlockFile(const std::string& path);
    static bool tryLockFile(const std::string& path);
    
    // Temporary file operations
    static std::string createTempFile(const std::string& prefix = "tmp", const std::string& suffix = "");
    static bool deleteTempFile(const std::string& path);
    
    // Archive operations
    static bool compressFile(const std::string& source, const std::string& destination);
    static bool extractArchive(const std::string& archive, const std::string& destination);
    
    // File search operations
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    static std::vector<std::string> findFilesRecursive(const std::string& directory, const std::string& pattern);
    
    // File monitoring
    class FileWatcher {
    public:
        using FileChangeCallback = std::function<void(const std::string& path, const std::string& action)>;
        
        FileWatcher(const std::string& path, FileChangeCallback callback);
        ~FileWatcher();
        
        bool isWatching() const;
        void stopWatching();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Utility functions
    static std::string readFileContent(const std::string& path);
    static bool writeFileContent(const std::string& path, const std::string& content);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static bool createDirectoryRecursive(const std::string& path);
    static std::string getTempDirectory();
    static std::string getCurrentDirectory();
    static bool changeDirectory(const std::string& path);
    
    // File comparison
    static bool compareFiles(const std::string& path1, const std::string& path2);
    static bool compareFileContents(const std::string& path1, const std::string& path2);
    static bool compareFilesByChecksum(const std::string& path1, const std::string& path2);
    
    // File utility constants
    static constexpr size_t MAX_PATH_LENGTH = 4096;
    static constexpr size_t MAX_FILENAME_LENGTH = 255;
    static constexpr char PATH_SEPARATOR = '/';
    static constexpr char FILE_EXTENSION_SEPARATOR = '.';
    
    // File type constants
    static constexpr const char* TEXT_FILE_TYPE = "text";
    static constexpr const char* BINARY_FILE_TYPE = "binary";
    static constexpr const char* ARCHIVE_FILE_TYPE = "archive";
    static constexpr const char* IMAGE_FILE_TYPE = "image";
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm