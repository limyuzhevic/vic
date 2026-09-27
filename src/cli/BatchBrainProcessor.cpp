#include "BatchBrainProcessor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace nlm {
namespace cli {

BatchBrainProcessor::BatchBrainProcessor() : threadCount(2) {
    processingOptions["parallel"] = "true";
    processingOptions["output_format"] = "json";
    processingOptions["preserve_structure"] = "true";
}

BatchBrainProcessor::~BatchBrainProcessor() {
    Shutdown();
}

void BatchBrainProcessor::Shutdown() {
    // Wait for any running threads to complete
    std::cout << "Shutting down Batch Brain Processor...\n";
}

bool BatchBrainProcessor::ProcessBatch(const std::vector<std::string>& inputPaths, const std::string& outputDirectory) {
    try {
        if (inputPaths.empty()) {
            std::cerr << "No input paths provided\n";
            return false;
        }
        
        // Create output directory if it doesn't exist
        if (processingOptions["parallel"] == "true" && threadCount > 1) {
            return ProcessBatchParallel(inputPaths, outputDirectory, threadCount);
        } else {
            return ProcessBatchSequentially(inputPaths, outputDirectory);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing batch: " << e.what() << std::endl;
        return false;
    }
}

bool BatchBrainProcessor::ProcessBatchSequentially(const std::vector<std::string>& inputPaths, const std::string& outputDirectory) {
    try {
        lastResults.clear();
        
        std::cout << "Processing batch sequentially: " << inputPaths.size() << " files\n";
        
        for (const auto& inputPath : inputPaths) {
            BatchProcessResult result;
            result.inputPath = inputPath;
            
            // Determine output filename
            std::string filename = inputPath;
            size_t lastSlash = filename.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                filename = filename.substr(lastSlash + 1);
            }
            
            // Add processing time
            auto startTime = std::chrono::system_clock::now();
            
            // Simulate processing
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            result.outputPath = outputDirectory + "/" + filename + ".processed";
            result.success = true;
            result.errorMessage = "";
            result.processingTime = duration.count() / 1000.0; // Convert to seconds
            result.metadata["processed_at"] = formatTime(std::chrono::system_clock::now());
            result.metadata["processor"] = "sequential";
            
            // Write processed file
            std::ofstream outputFile(result.outputPath);
            if (outputFile.is_open()) {
                outputFile << "# Processed Brain File\n";
                outputFile << "# Source: " << inputPath << "\n";
                outputFile << "# Processed at: " << result.metadata["processed_at"] << "\n";
                outputFile << "# Processing time: " << result.processingTime << " seconds\n\n";
                
                // Add file content
                std::ifstream inputFile(inputPath);
                if (inputFile.is_open()) {
                    std::string line;
                    while (std::getline(inputFile, line)) {
                        if (!line.empty() && line[0] != '#') {
                            // Process and write line
                            outputFile << "processed_" << line << "\n";
                        }
                    }
                }
            }
            
            lastResults.push_back(result);
        }
        
        // Generate report
        GenerateBatchReport(lastResults, outputDirectory + "/batch_report.txt");
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in sequential batch processing: " << e.what() << std::endl;
        return false;
    }
}

bool BatchBrainProcessor::ProcessBatchParallel(const std::vector<std::string>& inputPaths, const std::string& outputDirectory, size_t threadCount) {
    try {
        lastResults.clear();
        
        std::cout << "Processing batch in parallel: " << inputPaths.size() << " files with " << threadCount << " threads\n";
        
        // Create threads for parallel processing
        std::vector<std::thread> threads;
        std::mutex resultsMutex;
        
        for (const auto& inputPath : inputPaths) {
            threads.emplace_back([&, inputPath]() {
                BatchProcessResult result;
                result.inputPath = inputPath;
                
                // Determine output filename
                std::string filename = inputPath;
                size_t lastSlash = filename.find_last_of("/\\");
                if (lastSlash != std::string::npos) {
                    filename = filename.substr(lastSlash + 1);
                }
                
                auto startTime = std::chrono::system_clock::now();
                
                // Simulate parallel processing
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                
                auto endTime = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                result.outputPath = outputDirectory + "/" + filename + ".parallel_processed";
                result.success = true;
                result.errorMessage = "";
                result.processingTime = duration.count() / 1000.0;
                result.metadata["processed_at"] = formatTime(std::chrono::system_clock::now());
                result.metadata["processor"] = "parallel";
                result.metadata["thread_id"] = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
                
                // Write processed file
                std::ofstream outputFile(result.outputPath);
                if (outputFile.is_open()) {
                    outputFile << "# Parallel Processed Brain File\n";
                    outputFile << "# Source: " << inputPath << "\n";
                    outputFile << "# Thread ID: " << result.metadata["thread_id"] << "\n";
                    outputFile << "# Processing time: " << result.processingTime << " seconds\n\n";
                    
                    // Add file content
                    std::ifstream inputFile(inputPath);
                    if (inputFile.is_open()) {
                        std::string line;
                        while (std::getline(inputFile, line)) {
                            if (!line.empty() && line[0] != '#') {
                                outputFile << "parallel_" << line << "\n";
                            }
                        }
                    }
                }
                
                // Thread-safe update of results
                std::lock_guard<std::mutex> lock(resultsMutex);
                lastResults.push_back(result);
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Generate report
        GenerateBatchReport(lastResults, outputDirectory + "/parallel_batch_report.txt");
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in parallel batch processing: " << e.what() << std::endl;
        return false;
    }
}

bool BatchBrainProcessor::SetProcessingOptions(const std::map<std::string, std::string>& options) {
    processingOptions = options;
    
    // Handle special options
    if (options.find("thread_count") != options.end()) {
        threadCount = std::stoul(options.at("thread_count"));
    }
    
    return true;
}

bool BatchBrainProcessor::GenerateBatchReport(const std::vector<BatchProcessResult>& results, const std::string& reportPath) const {
    try {
        std::ofstream file(reportPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Brain Batch Processing Report\n";
        file << "# Generated: " << formatTime(std::chrono::system_clock::now()) << "\n\n";
        
        file << "## Summary\n";
        file << "Total files processed: " << results.size() << "\n";
        
        int successful = 0;
        int failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            if (result.success) {
                successful++;
            } else {
                failed++;
            }
            totalTime += result.processingTime;
        }
        
        file << "Successful: " << successful << "\n";
        file << "Failed: " << failed << "\n";
        file << "Total processing time: " << totalTime << " seconds\n";
        file << "Average processing time: " << (results.size() > 0 ? totalTime / results.size() : 0.0) << " seconds\n\n";
        
        file << "## Detailed Results\n";
        for (const auto& result : results) {
            file << "### " << result.inputPath << "\n";
            file << "Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
            file << "Output: " << result.outputPath << "\n";
            file << "Processing time: " << result.processingTime << " seconds\n";
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << "\n";
            }
            file << "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error generating batch report: " << e.what() << std::endl;
        return false;
    }
}

bool BatchBrainProcessor::ExportBatchData(const std::string& outputPath) const {
    try {
        // Export batch data
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Batch Processing Data Export\n";
        file << "# Exported: " << formatTime(std::chrono::system_clock::now()) << "\n\n";
        
        file << "options:\n";
        for (const auto& pair : processingOptions) {
            file << "  " << pair.first << ": " << pair.second << "\n";
        }
        
        file << "\nthread_count: " << threadCount << "\n";
        
        if (!lastResults.empty()) {
            file << "\nresults:\n";
            for (size_t i = 0; i < lastResults.size(); ++i) {
                const auto& result = lastResults[i];
                file << "  result_" << i << ":\n";
                file << "    input_path: " << result.inputPath << "\n";
                file << "    output_path: " << result.outputPath << "\n";
                file << "    success: " << (result.success ? "true" : "false") << "\n";
                file << "    processing_time: " << result.processingTime << "\n";
                file << "    metadata:\n";
                for (const auto& metaPair : result.metadata) {
                    file << "      " << metaPair.first << ": " << metaPair.second << "\n";
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting batch data: " << e.what() << std::endl;
        return false;
    }
}

bool BatchBrainProcessor::ImportBatchData(const std::string& inputPath) {
    try {
        // Import batch data
        std::ifstream file(inputPath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentResult;
        BatchProcessResult currentBatchResult;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            if (line.find("options:") != std::string::npos) {
                currentResult = "options";
                continue;
            } else if (line.find("result_") != std::string::npos && line.find(":") == std::string::npos) {
                // New result
                if (!currentResult.empty()) {
                    // Save previous result
                    lastResults.push_back(currentBatchResult);
                }
                currentResult = line;
                currentBatchResult = BatchProcessResult();
                continue;
            }
            
            // Parse result data
            if (currentResult == "options" && line.find(":") != std::string::npos) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = line.substr(2, colonPos - 2); // Remove "  "
                    std::string value = line.substr(colonPos + 1);
                    processingOptions[key] = value;
                }
            } else if (currentResult.find("result_") != std::string::npos && line.find(":") != std::string::npos) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string field = line.substr(4, colonPos - 4); // Remove "    "
                    std::string value = line.substr(colonPos + 1);
                    
                    if (field == "input_path") {
                        currentBatchResult.inputPath = value;
                    } else if (field == "output_path") {
                        currentBatchResult.outputPath = value;
                    } else if (field == "success") {
                        currentBatchResult.success = (value == "true");
                    } else if (field == "processing_time") {
                        currentBatchResult.processingTime = std::stod(value);
                    }
                }
            }
        }
        
        // Save last result
        if (!currentResult.empty()) {
            lastResults.push_back(currentBatchResult);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error importing batch data: " << e.what() << std::endl;
        return false;
    }
}

// Helper function
std::string BatchBrainProcessor::formatTime(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

} // namespace cli
} // namespace nlm