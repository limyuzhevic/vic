#include "DocumentationGenerator.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

struct DocumentationGenerator::Impl {
    std::vector<DocEntry> entries;
    std::string formattingStyle;
    bool includeExamples;
    bool includeParameters;
};

DocumentationGenerator::DocumentationGenerator() : pImpl(std::make_unique<Impl>()) {
    pImpl->formattingStyle = "markdown";
    pImpl->includeExamples = true;
    pImpl->includeParameters = true;
    
    // Add default sections
    addSection(DocSectionType::OVERVIEW, "Overview", DocumentationContent::getOverviewContent());
    addSection(DocSectionType::USAGE, "Usage", DocumentationContent::getOverviewUsage());
    addSection(DocSectionType::API_REFERENCE, "API Reference", DocumentationContent::getExperimentAPIContent());
    addSection(DocSectionType::CONFIGURATION, "Configuration", DocumentationContent::getPhase6ConfigContent());
    addSection(DocSectionType::EXPERIMENTS, "Experiments", DocumentationContent::getPhase6ExperimentContent());
    addSection(DocSectionType::EXTENSIONS, "Extensions", DocumentationContent::getFactoryPatternContent());
    addSection(DocSectionType::EXAMPLES, "Examples", DocumentationContent::getBasicExperimentExample());
    addSection(DocSectionType::TROUBLESHOOTING, "Troubleshooting", DocumentationContent::getTroubleshootingContent());
}

DocumentationGenerator::~DocumentationGenerator() = default;

std::string DocumentationGenerator::generateFullDocumentation() const {
    std::ostringstream oss;
    
    oss << "# NLM Enhanced Experiment System Documentation" << std::endl;
    oss << "Generated: " << getCurrentTimestamp() << std::endl;
    oss << std::endl;
    
    // Generate table of contents
    oss << "## Table of Contents" << std::endl;
    oss << DocHelpers::generateTOC(pImpl->entries) << std::endl;
    oss << std::endl;
    
    // Generate each section
    for (const auto& entry : pImpl->entries) {
        oss << "## " << entry.title << "" << std::endl;
        oss << entry.content;
        
        if (pImpl->includeExamples && !entry.examples.empty()) {
            oss << "\n### Examples" << std::endl;
            for (const auto& example : entry.examples) {
                oss << DocHelpers::formatCodeBlock(example) << std::endl;
            }
        }
        
        if (pImpl->includeParameters && !entry.parameters.empty()) {
            oss << "\n### Parameters" << std::endl;
            oss << DocHelpers::formatTable({{ "Parameter", "Type", "Description" }},
                                         getParameterTableData(entry.parameters));
        }
        
        oss << std::endl;
    }
    
    return oss.str();
}

std::string DocumentationGenerator::generateSection(DocSectionType section) const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.section == section) {
            oss << "# " << entry.title << std::endl;
            oss << entry.content;
            
            if (pImpl->includeExamples && !entry.examples.empty()) {
                oss << "\n## Examples" << std::endl;
                for (const auto& example : entry.examples) {
                    oss << DocHelpers::formatCodeBlock(example) << std::endl;
                }
            }
            
            if (pImpl->includeParameters && !entry.parameters.empty()) {
                oss << "\n## Parameters" << std::endl;
                oss << DocHelpers::formatTable({{ "Parameter", "Type", "Description" }},
                                             getParameterTableData(entry.parameters));
            }
            
            break;
        }
    }
    
    return oss.str();
}

void DocumentationGenerator::addSection(DocSectionType section, const std::string& title, const std::string& content) {
    pImpl->entries.emplace_back(section, title, content);
}

void DocumentationGenerator::addExample(DocSectionType section, const std::string& example) {
    for (auto& entry : pImpl->entries) {
        if (entry.section == section) {
            entry.examples.push_back(example);
            break;
        }
    }
}

std::vector<DocSectionType> DocumentationGenerator::getAvailableSections() const {
    std::vector<DocSectionType> sections;
    std::unordered_set<DocSectionType> uniqueSections;
    
    for (const auto& entry : pImpl->entries) {
        if (uniqueSections.find(entry.section) == uniqueSections.end()) {
            sections.push_back(entry.section);
            uniqueSections.insert(entry.section);
        }
    }
    
    return sections;
}

std::string DocumentationGenerator::getSectionTitle(DocSectionType section) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.section == section) {
            return entry.title;
        }
    }
    return "";
}

void DocumentationGenerator::setFormattingStyle(const std::string& style) {
    pImpl->formattingStyle = style;
}

void DocumentationGenerator::setIncludeExamples(bool include) {
    pImpl->includeExamples = include;
}

void DocumentationGenerator::setIncludeParameters(bool include) {
    pImpl->includeParameters = include;
}

bool DocumentationGenerator::exportToFile(const std::string& filepath, const std::string& format) {
    std::string content = exportToString(format);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

std::string DocumentationGenerator::exportToString(const std::string& format) const {
    if (format == "markdown") {
        return generateFullDocumentation();
    } else if (format == "html") {
        // Generate HTML documentation
        std::ostringstream oss;
        oss << "<!DOCTYPE html>" << std::endl;
        oss << "<html>" << std::endl;
        oss << "<head>" << std::endl;
        oss << "    <title>NLM Enhanced Experiment System Documentation</title>" << std::endl;
        oss << "</head>" << std::endl;
        oss << "<body>" << std::endl;
        oss << "    <h1>NLM Enhanced Experiment System Documentation</h1>" << std::endl;
        oss << "    <p>Generated: " << getCurrentTimestamp() << "</p>" << std::endl;
        oss << generateSection(DocSectionType::OVERVIEW);
        oss << "</body>" << std::endl;
        oss << "</html>" << std::endl;
        return oss.str();
    } else if (format == "pdf") {
        // For PDF, you'd typically use a library like iText or similar
        // For now, return a message indicating PDF generation
        return "PDF generation requires additional libraries";
    }
    
    return generateFullDocumentation();
}

std::string DocumentationGenerator::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string DocumentationGenerator::formatTimestamp(const std::string& timestamp, const std::string& format) {
    // Parse timestamp and reformat
    return timestamp; // Simplified - in production, you'd parse and reformat
}

// Helper functions implementation
namespace DocHelpers {
    std::string formatCodeBlock(const std::string& code, const std::string& language) {
        std::ostringstream oss;
        oss << "```" << language << std::endl;
        oss << code;
        oss << "```" << std::endl;
        return oss.str();
    }
    
    std::string formatList(const std::vector<std::string>& items, const std::string& style) {
        if (style == "bullet") {
            std::ostringstream oss;
            for (const auto& item : items) {
                oss << "- " << item << std::endl;
            }
            return oss.str();
        } else if (style == "numbered") {
            std::ostringstream oss;
            for (size_t i = 0; i < items.size(); ++i) {
                oss << (i + 1) << ". " << items[i] << std::endl;
            }
            return oss.str();
        }
        return "";
    }
    
    std::string formatTable(const std::vector<std::vector<std::string>>& data,
                           const std::vector<std::string>& headers) {
        if (data.empty() || headers.empty()) {
            return "";
        }
        
        std::ostringstream oss;
        
        // Write header
        oss << "|";
        for (const auto& header : headers) {
            oss << " " << header << " |";
        }
        oss << std::endl;
        
        // Write separator
        oss << "|";
        for (size_t i = 0; i < headers.size(); ++i) {
            oss << " --- |";
        }
        oss << std::endl;
        
        // Write data rows
        for (const auto& row : data) {
            oss << "|";
            for (const auto& cell : row) {
                oss << " " << cell << " |";
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    std::string formatLink(const std::string& text, const std::string& url) {
        return "[" + text + "](" + url + ")";
    }
    
    std::string formatWarning(const std::string& message) {
        return "!!! warning\"\"\n" + message + "\n\"\"";
    }
    
    std::string formatNote(const std::string& message) {
        return "!!! note\"\"\n" + message + "\n\"\"";
    }
    
    std::string formatImportant(const std::string& message) {
        return "!!! important\"\"\n" + message + "\n\"\"";
    }
    
    std::string generateTOC(const std::vector<DocEntry>& entries) {
        std::ostringstream oss;
        
        for (const auto& entry : entries) {
            oss << "- [" << entry.title << "](" << "#" << entry.title << ")" << std::endl;
        }
        
        return oss.str();
    }
    
    std::vector<std::vector<std::string>> getParameterTableData(const std::map<std::string, std::string>& parameters) {
        std::vector<std::vector<std::string>> data;
        
        for (const auto& pair : parameters) {
            std::vector<std::string> row;
            row.push_back(pair.first);
            
            // Parse parameter to get type and description
            size_t colonPos = pair.second.find(":");
            if (colonPos != std::string::npos) {
                row.push_back(pair.second.substr(0, colonPos));
                row.push_back(pair.second.substr(colonPos + 1));
            } else {
                row.push_back("string");
                row.push_back(pair.second);
            }
            
            data.push_back(row);
        }
        
        return data;
    }
}

} // namespace nlm
