#include "cli/util/FileUtils.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

using namespace nlm::cli::util;

bool FileUtils::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileUtils::createDirectory(const std::string& path) {
    return std::filesystem::create_directories(path);
}

bool FileUtils::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return !file.fail();
}

bool FileUtils::readFile(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return !file.fail();
}

std::vector<std::string> FileUtils::listFiles(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    if (!std::filesystem::exists(directory)) {
        return files;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            if (extension.empty() || getFileExtension(entry.path().string()) == extension) {
                files.push_back(entry.path().string());
            }
        }
    }
    return files;
}

std::string FileUtils::getFileExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    std::string ext = path.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

std::string FileUtils::getFileName(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}
