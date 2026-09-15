#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>

namespace nlm {

// Diff result structure
struct DiffResult {
    std::set<std::string> added;
    std::set<std::string> removed;
    std::map<std::string, std::pair<std::string, std::string>> changed;
};

// Configuration diff tool
class ConfigDiff {
public:
    ConfigDiff();
    virtual ~ConfigDiff() = default;
    
    // Compare two configuration objects
    std::string compare(const std::string& file1, const std::string& file2,
                        const std::string& format = "auto");
    
    // Generate configuration diff report
    std::string generateDiffReport(const std::map<std::string, std::string>& changes);
    
    // Highlight differences in output
    std::string highlightChanges(const std::string& content,
                                 const std::vector<std::pair<size_t, size_t>>& diffs);
    
    // Export configuration
    bool exportConfig(const std::string& filepath, const CLIOptions& options);
    
    // Import configuration
    bool importConfig(const std::string& filepath, CLIOptions& options);
    
    // Configuration validation
    bool validateConfig(const std::string& filepath, const std::string& format = "auto");
    
private:
    std::map<std::string, std::string> parseConfigFile(const std::string& filepath, const std::string& format);
    DiffResult findDifferences(const std::map<std::string, std::string>& config1,
                               const std::map<std::string, std::string>& config2);
    std::string trim(const std::string& str);
    std::string detectFormatFromContent(const std::string& content);
};

} // namespace nlm
