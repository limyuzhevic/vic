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

// FileSystem implementation
FileSystem::FileSystem() : pImpl(std::make_unique<Impl>()) {}

FileSystem::~FileSystem() = default;

FileSystem& FileSystem::getInstance() {
    static FileSystem instance;
    return instance;
}

std::string FileSystem::normalizePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.make_preferred().string();
}

std::string FileSystem::absolutePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.make_absolute().string();
}

std::string FileSystem::relativePath(const std::string& path, const std::string& base) {
    std::filesystem::path fsPath(path);
    std::filesystem::path fsBase(base);
    return fsPath.lexically_relative(fsBase).string();
}

std::string FileSystem::extension(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.extension().string();
}

std::string FileSystem::filename(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.filename().string();
}

std::string FileSystem::stem(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.stem().string();
}

std::string FileSystem::directory(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.parent_path().string();
}

bool FileSystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileSystem::isSymlink(const std::string& path) {
    return std::filesystem::is_symlink(path);
}

uintmax_t FileSystem::fileSize(const std::string& path) {
    if (!exists(path) || !isFile(path)) {
        return 0;
    }
    return std::filesystem::file_size(path);
}

std::chrono::system_clock::time_point FileSystem::lastModified(const std::string& path) {
    if (!exists(path)) {
        return std::chrono::system_clock::time_point();
    }
    
    auto ftime = std::filesystem::last_write_time(path);
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
}

std::chrono::system_clock::time_point FileSystem::created(const std::string& path) {
    if (!exists(path)) {
        return std::chrono::system_clock::time_point();
    }
    
    auto ftime = std::filesystem::status(path).last_write_time();
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
}

bool FileSystem::readFile(const std::string& path, std::string& content) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        return false;
    }
    
    content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

bool FileSystem::readFileBinary(const std::string& path, std::vector<uint8_t>& data) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(size);
    if (file.read(reinterpret_cast<char*>(data.data()), size)) {
        return true;
    }
    
    return false;
}

bool FileSystem::writeFile(const std::string& path, const std::string& content, bool append) {
    std::ios_base::openmode mode = std::ios::out;
    if (append) {
        mode |= std::ios::app;
    }
    
    std::ofstream file(path, mode);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

bool FileSystem::writeFileBinary(const std::string& path, const std::vector<uint8_t>& data, bool append) {
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;
    if (append) {
        mode |= std::ios::app;
    }
    
    std::ofstream file(path, mode);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

bool FileSystem::createDirectory(const std::string& path, bool recursive) {
    std::error_code ec;
    if (recursive) {
        return std::filesystem::create_directories(path, ec);
    } else {
        return std::filesystem::create_directory(path, ec);
    }
}

bool FileSystem::removeFile(const std::string& path) {
    std::error_code ec;
    return std::filesystem::remove(path, ec);
}

bool FileSystem::removeDirectory(const std::string& path, bool recursive) {
    std::error_code ec;
    if (recursive) {
        return std::filesystem::remove_all(path, ec);
    } else {
        return std::filesystem::remove(path, ec);
    }
}

bool FileSystem::copyFile(const std::string& source, const std::string& destination) {
    std::error_code ec;
    return std::filesystem::copy_file(source, destination, ec);
}

bool FileSystem::moveFile(const std::string& source, const std::string& destination) {
    std::error_code ec;
    return std::filesystem::rename(source, destination, ec);
}

bool FileSystem::renameFile(const std::string& oldPath, const std::string& newPath) {
    return moveFile(oldPath, newPath);
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path, bool recursive) {
    std::vector<std::string> files;
    
    if (recursive) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

bool FileSystem::createDirectories(const std::string& path) {
    return createDirectory(path, true);
}

bool FileSystem::removeDirectories(const std::string& path, bool recursive) {
    return removeDirectory(path, recursive);
}

bool FileSystem::validatePath(const std::string& path, bool mustExist) {
    if (path.length() > MAX_PATH_LENGTH) return false;
    
    if (mustExist && !exists(path)) return false;
    
    return true;
}

bool FileSystem::validatePathLength(const std::string& path, size_t maxLength) {
    return path.length() <= maxLength;
}

bool FileSystem::validatePathSecurity(const std::string& path) {
    return isSafePath(path);
}

bool FileSystem::isTextFile(const std::string& path) {
    if (!exists(path) || !isFile(path)) return false;
    
    std::ifstream file(path);
    char ch;
    int binaryCount = 0;
    int textCount = 0;
    
    while (file.get(ch)) {
        if (std::iscntrl(ch) && ch != '\t' && ch != '\n' && ch != '\r') {
            binaryCount++;
        } else {
            textCount++;
        }
    }
    
    return (binaryCount == 0) || (textCount > binaryCount * 2);
}

bool FileSystem::isBinaryFile(const std::string& path) {
    return !isTextFile(path);
}

std::string FileSystem::detectFileType(const std::string& path) {
    if (!exists(path) || !isFile(path)) return "unknown";
    
    std::string extension = toLowerCase(extension(path));
    
    if (extension == ".json" || extension == ".yaml" || extension == ".yml") {
        return "config";
    } else if (extension == ".txt" || extension == ".log" || extension == ".md") {
        return "text";
    } else if (extension == ".bin" || extension == ".dat") {
        return "binary";
    } else if (extension == ".zip" || extension == ".tar" || extension == ".gz") {
        return "archive";
    } else if (extension == ".jpg" || extension == ".png" || extension == ".gif") {
        return "image";
    }
    
    return "unknown";
}

bool FileSystem::hasTextContent(const std::string& content) {
    return hasTextContent(content.c_str(), content.size());
}

bool FileSystem::hasTextContent(const char* data, size_t size) {
    int binaryCount = 0;
    int textCount = 0;
    
    for (size_t i = 0; i < size; ++i) {
        if (std::iscntrl(data[i]) && data[i] != '\t' && data[i] != '\n' && data[i] != '\r') {
            binaryCount++;
        } else {
            textCount++;
        }
    }
    
    return (binaryCount == 0) || (textCount > binaryCount * 2);
}

bool FileSystem::hasBinaryContent(const std::vector<uint8_t>& data) {
    return !hasTextContent(data.data(), data.size());
}

bool FileSystem::saveStringToFile(const std::string& path, const std::string& content) {
    return writeFile(path, content);
}

bool FileSystem::loadStringFromFile(const std::string& path, std::string& content) {
    return readFile(path, content);
}

bool FileSystem::saveBinaryDataToFile(const std::string& path, const std::vector<uint8_t>& data) {
    return writeFileBinary(path, data);
}

bool FileSystem::loadBinaryDataFromFile(const std::string& path, std::vector<uint8_t>& data) {
    return readFileBinary(path, data);
}

std::string FileSystem::resolvePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.make_absolute().string();
}

bool FileSystem::isSafePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    
    // Check for path traversal attacks
    if (fsPath.is_absolute()) {
        // Check if path contains suspicious components
        for (const auto& component : fsPath) {
            if (component.string() == ".." || component.string() == ".") {
                return false;
            }
        }
    }
    
    return true;
}

bool FileSystem::isPathWithinDirectory(const std::string& path, const std::string& directory) {
    std::filesystem::path fsPath(path);
    std::filesystem::path fsDirectory(directory);
    
    try {
        auto relative = fsPath.lexically_relative(fsDirectory);
        return !relative.empty() && relative.string().find("..") == std::string::npos;
    } catch (const std::exception&) {
        return false;
    }
}

bool FileSystem::lockFile(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool FileSystem::unlockFile(const std::string& path) {
    return true; // Placeholder - implementation depends on OS
}

bool FileSystem::tryLockFile(const std::string& path) {
    return lockFile(path);
}

std::string FileSystem::createTempFile(const std::string& prefix, const std::string& suffix) {
    std::string tempDir = getTempDirectory();
    std::filesystem::path tempPath = std::filesystem::temp_directory_path();
    
    std::string filename = prefix + "-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + suffix;
    std::filesystem::path fullPath = tempDir / filename;
    
    if (!exists(tempDir)) {
        createDirectories(tempDir);
    }
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        return "";
    }
    
    return fullPath.string();
}

bool FileSystem::deleteTempFile(const std::string& path) {
    return removeFile(path);
}

bool FileSystem::compressFile(const std::string& source, const std::string& destination) {
    // Placeholder - would use compression library
    return copyFile(source, destination);
}

bool FileSystem::extractArchive(const std::string& archive, const std::string& destination) {
    // Placeholder - would use archive library
    return true;
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> files;
    std::regex regex(pattern);
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, regex)) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

std::vector<std::string> FileSystem::findFilesRecursive(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> files;
    std::regex regex(pattern);
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, regex)) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

std::string FileSystem::readFileContent(const std::string& path) {
    std::string content;
    if (readFile(path, content)) {
        return content;
    }
    return "";
}

bool FileSystem::writeFileContent(const std::string& path, const std::string& content) {
    return writeFile(path, content);
}

std::string FileSystem::getFileExtension(const std::string& filename) {
    return extension(filename);
}

std::string FileSystem::getFileNameWithoutExtension(const std::string& filename) {
    return stem(filename);
}

bool FileSystem::fileExists(const std::string& path) {
    return exists(path);
}

bool FileSystem::directoryExists(const std::string& path) {
    return isDirectory(path);
}

bool FileSystem::createDirectoryRecursive(const std::string& path) {
    return createDirectories(path);
}

std::string FileSystem::getTempDirectory() {
    return std::filesystem::temp_directory_path().string();
}

std::string FileSystem::getCurrentDirectory() {
    return std::filesystem::current_path().string();
}

bool FileSystem::changeDirectory(const std::string& path) {
    std::error_code ec;
    std::filesystem::current_path(path, ec);
    return !ec;
}

bool FileSystem::compareFiles(const std::string& path1, const std::string& path2) {
    std::vector<uint8_t> data1, data2;
    if (readFileBinary(path1, data1) && readFileBinary(path2, data2)) {
        return data1 == data2;
    }
    return false;
}

bool FileSystem::compareFileContents(const std::string& path1, const std::string& path2) {
    std::string content1, content2;
    if (readFile(path1, content1) && readFile(path2, content2)) {
        return content1 == content2;
    }
    return false;
}

bool FileSystem::compareFilesByChecksum(const std::string& path1, const std::string& path2) {
    // Placeholder - would implement checksum
    return compareFileContents(path1, path2);
}

std::string FileSystem::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm