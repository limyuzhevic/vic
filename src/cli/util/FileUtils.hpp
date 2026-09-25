#pragma once

#include <string>
#include <vector>

namespace nlm {
namespace cli {
namespace util {

class FileUtils {
public:
    static bool fileExists(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool readFile(const std::string& path, std::string& content);
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& extension = "");
    static std::string getFileExtension(const std::string& path);
    static std::string getFileName(const std::string& path);
};

} // namespace util
} // namespace cli
} // namespace nlm
