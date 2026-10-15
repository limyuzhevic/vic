#include "Reporter.hpp"
#include <memory>
#include <stdexcept>

namespace nlm {

std::unique_ptr<Reporter> ReportFactory::createReporter(const std::string& name, 
                                                        const std::map<std::string, std::string>& options) {
    if (name == "PlainText" || name == "plaintext") {
        return std::make_unique<PlainTextReporter>();
    } else if (name == "JSON" || name == "json") {
        return std::make_unique<JSONReporter>();
    } else if (name == "XML" || name == "xml") {
        return std::make_unique<XMLReporter>();
    } else if (name == "CSV" || name == "csv") {
        return std::make_unique<CSVReporter>();
    } else if (name == "Markdown" || name == "markdown") {
        return std::make_unique<MarkdownReporter>();
    } else {
        throw std::runtime_error("Unknown reporter type: " + name);
    }
}

std::vector<std::unique_ptr<Reporter>> ReportFactory::createReporters(
    const std::vector<std::string>& reporterNames,
    const std::map<std::string, std::string>& globalOptions) {
    
    std::vector<std::unique_ptr<Reporter>> reporters;
    
    for (const auto& name : reporterNames) {
        try {
            auto reporter = createReporter(name, globalOptions);
            reporters.push_back(std::move(reporter));
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to create reporter '" << name << "': " << e.what() << std::endl;
        }
    }
    
    return reporters;
}

std::vector<std::string> ReportFactory::getAvailableReporters() {
    return {"PlainText", "JSON", "XML", "CSV", "Markdown"};
}

} // namespace nlm
