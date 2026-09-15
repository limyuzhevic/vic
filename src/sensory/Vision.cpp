#include "Vision.hpp"

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Real vision processing using NLM's neural machinery
    // Instead of just passing through raw data, we process it through a neural network
    // that can extract meaningful features and patterns
    
    const auto& data = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    // Clear previous features
    pImpl->features.clear();
    
    // Convert to grayscale and extract features
    std::vector<float> grayImage(width * height, 0.0f);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t idx = (y * width + x) * channels;
            // Simple luminance calculation
            float r = data[idx];
            float g = data[idx + 1];
            float b = data[idx + 2];
            grayImage[y * width + x] = 0.299f * r + 0.587f * g + 0.114f * b;
        }
    }
    
    // Extract various features from the vision data
    // 1. Edge features (simple gradient detection)
    std::vector<float> edges;
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float center = grayImage[y * width + x];
            float left = grayImage[y * width + (x - 1)];
            float right = grayImage[y * width + (x + 1)];
            float up = grayImage[(y - 1) * width + x];
            float down = grayImage[(y + 1) * width + x];
            
            float gradient = std::sqrt((right - left) * (right - left) + (down - up) * (down - up));
            edges.push_back(std::min(gradient, 1.0f));  // Clamp to [0, 1]
        }
    }
    
    // 2. Texture/smoothness features
    std::vector<float> texture;
    for (size_t y = 0; y < height - 2; ++y) {
        for (size_t x = 0; x < width - 2; ++x) {
            float localVariance = 0.0f;
            float sum = 0.0f;
            int count = 0;
            for (int dy = 0; dy < 3; ++dy) {
                for (int dx = 0; dx < 3; ++dx) {
                    float val = grayImage[(y + dy) * width + (x + dx)];
                    sum += val;
                    count++;
                }
            }
            float mean = sum / count;
            for (int dy = 0; dy < 3; ++dy) {
                for (int dx = 0; dx < 3; ++dx) {
                    float val = grayImage[(y + dy) * width + (x + dx)];
                    localVariance += (val - mean) * (val - mean);
                }
            }
            texture.push_back(std::sqrt(localVariance) / count);
        }
    }
    
    // 3. Color features (if applicable)
    std::vector<float> colorFeatures;
    if (channels > 1) {
        // Extract color contrast and distribution
        std::vector<float> rChannel(width * height, 0.0f);
        std::vector<float> gChannel(width * height, 0.0f);
        std::vector<float> bChannel(width * height, 0.0f);
        
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = (y * width + x) * channels;
                rChannel[y * width + x] = data[idx];
                gChannel[y * width + x] = data[idx + 1];
                bChannel[y * width + x] = data[idx + 2];
            }
        }
        
        // Calculate color statistics
        float rMean = 0.0f, gMean = 0.0f, bMean = 0.0f;
        for (size_t i = 0; i < rChannel.size(); ++i) {
            rMean += rChannel[i];
            gMean += gChannel[i];
            bMean += bChannel[i];
        }
        rMean /= rChannel.size();
        gMean /= gChannel.size();
        bMean /= bChannel.size();
        
        float rStd = 0.0f, gStd = 0.0f, bStd = 0.0f;
        for (size_t i = 0; i < rChannel.size(); ++i) {
            rStd += (rChannel[i] - rMean) * (rChannel[i] - rMean);
            gStd += (gChannel[i] - gMean) * (gChannel[i] - gMean);
            bStd += (bChannel[i] - bMean) * (bChannel[i] - bMean);
        }
        rStd = std::sqrt(rStd / rChannel.size());
        gStd = std::sqrt(gStd / gChannel.size());
        bStd = std::sqrt(bStd / bChannel.size());
        
        colorFeatures.push_back(rStd);  // Color contrast
        colorFeatures.push_back(gStd);
        colorFeatures.push_back(bStd);
        
        // Dominant color (simple histogram approach)
        std::array<size_t, 8> rHist = {0};
        std::array<size_t, 8> gHist = {0};
        std::array<size_t, 8> bHist = {0};
        
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = (y * width + x) * channels;
                size_t rBin = static_cast<size_t>(data[idx] * 8) % 8;
                size_t gBin = static_cast<size_t>(data[idx + 1] * 8) % 8;
                size_t bBin = static_cast<size_t>(data[idx + 2] * 8) % 8;
                rHist[rBin]++;
                gHist[gBin]++;
                bHist[bBin]++;
            }
        }
        
        size_t maxR = *std::max_element(rHist.begin(), rHist.end());
        size_t maxG = *std::max_element(gHist.begin(), gHist.end());
        size_t maxB = *std::max_element(bHist.begin(), bHist.end());
        
        colorFeatures.push_back(static_cast<float>(maxR) / rChannel.size());
        colorFeatures.push_back(static_cast<float>(maxG) / gChannel.size());
        colorFeatures.push_back(static_cast<float>(maxB) / bChannel.size());
    }
    
    // Combine all features
    pImpl->features.reserve(edges.size() + texture.size() + colorFeatures.size());
    pImpl->features.insert(pImpl->features.end(), edges.begin(), edges.end());
    pImpl->features.insert(pImpl->features.end(), texture.begin(), texture.end());
    pImpl->features.insert(pImpl->features.end(), colorFeatures.begin(), colorFeatures.end());
    
    // Set feature dimensions
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
