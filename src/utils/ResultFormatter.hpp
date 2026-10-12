#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <string>

namespace nlm {

class ResultFormatter {
public:
    ResultFormatter();
    ~ResultFormatter();
    
    void format(const std::map<std::string, std::string>& data, const std::string& format);
    
private:
    void formatAsJson(const std::map<std::string, std::string>& data);
    void formatAsYaml(const std::map<std::string, std::string>& data);
    void formatAsText(const std::map<std::string, std::string>& data);
};

} // namespace nlm

ResultFormatter::ResultFormatter() {
}

ResultFormatter::~ResultFormatter() {
}

void ResultFormatter::format(const std::map<std::string, std::string>& data, const std::string& format) {
    if (format == "json") {
        formatAsJson(data);
    } else if (format == "yaml") {
        formatAsYaml(data);
    } else {
        formatAsText(data);
    }
}

void ResultFormatter::formatAsJson(const std::map<std::string, std::string>& data) {
    nlohmann::json jsonData;
    
    for (const auto& pair : data) {
        jsonData[pair.first] = pair.second;
    }
    
    std::cout << jsonData.dump(2) << std::endl;
}

void ResultFormatter::formatAsYaml(const std::map<std::string, std::string>& data) {
    YAML::Emitter out;
    out << YAML::BeginDoc;
    
    for (const auto& pair : data) {
        out << YAML::Key << pair.first;
        out << YAML::Value << pair.second;
    }
    
    out << YAML::EndDoc;
    std::cout << out.c_str();
}

void ResultFormatter::formatAsText(const std::map<std::string, std::string>& data) {
    for (const auto& pair : data) {
        std::cout << std::left << std::setw(20) << pair.first << ": " << pair.second << std::endl;
    }
}

} // namespace nlm