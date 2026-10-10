#include "LayoutManager.hpp"
#include "ColorManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace nlm {

Widget::Widget(Type type, const std::string& id, const Position& pos, const Size& size)
    : id_(id), type_(type), position_(pos), size_(size), visible_(true), parent_(nullptr) {
}

void Widget::addChild(std::unique_ptr<Widget> child) {
    children_.push_back(std::move(child));
    children_.back()->setParent(this);
}

Widget* Widget::getChild(const std::string& id) const {
    for (auto& child : children_) {
        if (child->getId() == id) {
            return child.get();
        }
        Widget* found = child->getChild(id);
        if (found) return found;
    }
    return nullptr;
}

int Widget::getScreenX() const {
    int x = position_.x;
    Widget* current = parent_;
    while (current) {
        x += current->position_.x;
        current = current->parent_;
    }
    return x;
}

int Widget::getScreenY() const {
    int y = position_.y;
    Widget* current = parent_;
    while (current) {
        y += current->position_.y;
        current = current->parent_;
    }
    return y;
}

void Widget::applyLayout(const std::map<std::string, Constraint>& constraints) {
    // Apply layout constraints
    for (const auto& constraint : constraints) {
        Widget* widget = getChild(constraint.first);
        if (widget) {
            widget->calculateGeometry();
        }
    }
}

void Widget::calculateGeometry() {
    // Calculate widget geometry based on parent size
    if (parent_) {
        position_.x = static_cast<int>(parent_->size_.width * 0.1f); // 10% margin from left
        position_.y = static_cast<int>(parent_->size_.height * 0.1f); // 10% margin from top
    }
    onPositionChanged();
    onSizeChanged();
}

Panel::Panel(const std::string& id, const Position& pos, const Size& size, BorderStyle border)
    : Widget(Type::Panel, id, pos, size), borderStyle_(border) {
    borderChars_[0] = "+"; // Corners
    borderChars_[1] = "-"; // Horizontal
    borderChars_[2] = "|"; // Vertical
    borderChars_[3] = "."; // Junctions
}

void Panel::render() {
    if (!isVisible()) return;
    
    ColorManager colorManager;
    std::string borderColor = colorManager.getColorCode(ColorManager::Color::White);
    std::string titleColor = colorManager.getColorCode(ColorManager::Color::Yellow);
    
    // Draw panel background
    for (int y = position_.y; y < position_.y + size_.height; ++y) {
        for (int x = position_.x; x < position_.x + size_.width; ++x) {
            std::cout << backgroundColor_ << " ";
        }
        std::cout << "\033[0m\n";
    }
    
    // Draw title
    if (!title_.empty()) {
        std::cout << titleColor << " " << title_ << " " << "\033[0m";
    }
    
    // Draw border if requested
    if (borderStyle_ != BorderStyle::None) {
        // Draw corners
        std::cout << borderColor << borderChars_[0];
        for (int x = 1; x < size_.width - 1; ++x) std::cout << borderChars_[1];
        std::cout << borderChars_[0] << "\033[0m\n";
        
        // Draw sides
        for (int y = 1; y < size_.height - 1; ++y) {
            std::cout << borderColor << borderChars_[2] << "\033[0m";
            for (int x = 1; x < size_.width - 1; ++x) std::cout << " ";
            std::cout << borderColor << borderChars_[2] << "\033[0m\n";
        }
        
        // Draw bottom border
        std::cout << borderColor << borderChars_[0];
        for (int x = 1; x < size_.width - 1; ++x) std::cout << borderChars_[1];
        std::cout << borderChars_[0] << "\033[0m\n";
    }
}

Label::Label(const std::string& text, const std::string& id, const Position& pos, const Size& size)
    : Widget(Type::Label, id, pos, size), text_(text), alignment_(1) {
}

void Label::render() {
    if (!isVisible()) return;
    
    std::cout << color_ << text_ << "\033[0m";
}

Chart::Chart(const std::string& id, const Position& pos, const Size& size)
    : Widget(Type::Chart, id, pos, size), colorScheme_("default") {
    colors_[0] = "\033[31m"; // Red
    colors_[1] = "\033[32m"; // Green
    colors_[2] = "\033[34m"; // Blue
    colors_[3] = "\033[33m"; // Yellow
}

void Chart::render() {
    if (!isVisible() || data_.empty()) return;
    
    // Calculate chart dimensions
    int chartWidth = size_.width - 2;
    int chartHeight = size_.height - 2;
    
    // Draw chart background
    for (int y = position_.y; y < position_.y + size_.height; ++y) {
        for (int x = position_.x; x < position_.x + size_.width; ++x) {
            std::cout << " ";
        }
        std::cout << "\n";
    }
    
    // Draw data points
    if (!data_.empty() && chartWidth > 0 && chartHeight > 0) {
        float minValue = *std::min_element(data_.begin(), data_.end());
        float maxValue = *std::max_element(data_.begin(), data_.end());
        float range = maxValue - minValue;
        if (range == 0.0f) range = 1.0f;
        
        for (size_t i = 0; i < data_.size() && i < static_cast<size_t>(chartWidth); ++i) {
            float normalized = (data_[i] - minValue) / range;
            int height = static_cast<int>(normalized * (chartHeight - 1));
            
            if (i < colors_.size()) {
                std::cout << colors_[i % colors_.size()];
            }
            
            for (int y = chartHeight - 1; y >= 0; --y) {
                if (y < height) {
                    std::cout << "*";
                } else {
                    std::cout << " ";
                }
            }
            
            std::cout << "\033[0m";
        }
    }
}

Graph::Graph(const std::string& id, const Position& pos, const Size& size)
    : Widget(Type::Graph, id, pos, size) {
}

void Graph::render() {
    if (!isVisible() || series_.empty()) return;
    
    // Simple graph rendering - draw axes and data points
    ColorManager colorManager;
    std::string axisColor = colorManager.getColorCode(ColorManager::Color::White);
    
    // Draw axes
    std::cout << axisColor << "+";
    for (int x = 1; x < size_.width - 1; ++x) std::cout << "-";
    std::cout << "+\033[0m\n";
    
    for (int y = 1; y < size_.height - 1; ++y) {
        std::cout << axisColor << "|" << "\033[0m";
        for (int x = 1; x < size_.width - 1; ++x) std::cout << " ";
        std::cout << axisColor << "|" << "\033[0m\n";
    }
    
    // Draw data points (simplified)
    if (!series_.empty() && size_.width > 10 && size_.height > 10) {
        size_t seriesIndex = 0;
        std::string seriesColor = "\033[31m";
        
        for (size_t i = 0; i < series_.size() && i < static_cast<size_t>(size_.width - 2); ++i) {
            if (!series_[0].empty()) {
                float value = series_[0][i % series_[0].size()];
                float normalized = (value - 0.0f) / 1.0f; // Assume range [0,1]
                int y = size_.height - static_cast<int>(normalized * (size_.height - 3)) - 1;
                
                std::cout << seriesColor;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int py = y + dy;
                        int px = static_cast<int>(i) + 1 + dx;
                        if (py >= position_.y && py < position_.y + size_.height &&
                            px >= position_.x && px < position_.x + size_.width) {
                            std::cout << "*";
                        }
                    }
                }
                std::cout << "\033[0m";
            }
        }
    }
}

ListWidget::ListWidget(const std::string& id, const Position& pos, const Size& size)
    : Widget(Type::List, id, pos, size), selectedIndex_(-1), scrollOffset_(0) {
}

void ListWidget::render() {
    if (!isVisible() || items_.empty()) return;
    
    // Draw visible items
    int visibleCount = std::min(static_cast<int>(items_.size()) - scrollOffset_, size_.height);
    
    for (int i = 0; i < visibleCount; ++i) {
        int itemIndex = i + scrollOffset_;
        std::string itemText = items_[itemIndex];
        
        if (itemIndex == selectedIndex_) {
            std::cout << "\033[7m"; // Reverse video for selection
        }
        
        // Truncate long items
        if (itemText.length() > size_.width - 2) {
            itemText = itemText.substr(0, size_.width - 5) + "...";
        }
        
        std::cout << itemText;
        
        // Fill rest of line with spaces
        for (size_t j = itemText.length(); j < static_cast<size_t>(size_.width - 2); ++j) {
            std::cout << " ";
        }
        
        std::cout << "\033[0m\n";
    }
}

bool ListWidget::handleInput(const std::string& input) {
    if (input == "UP") {
        if (selectedIndex_ > 0) {
            selectedIndex_--;
            if (selectedIndex_ < scrollOffset_) {
                scrollOffset_ = selectedIndex_;
            }
            return true;
        }
    } else if (input == "DOWN") {
        if (selectedIndex_ < static_cast<int>(items_.size()) - 1) {
            selectedIndex_++;
            if (selectedIndex_ >= scrollOffset_ + size_.height) {
                scrollOffset_ = selectedIndex_ - size_.height + 1;
            }
            return true;
        }
    }
    return false;
}

LayoutManager::LayoutManager()
    : paddingLeft_(2), paddingTop_(1), paddingRight_(2), paddingBottom_(1),
      marginLeft_(1), marginTop_(1), marginRight_(1), marginBottom_(1),
      horizontalSpacing_(1), verticalSpacing_(1), autoLayoutEnabled_(true),
      theme_("default"), colorScheme_("default") {
    initializeColorArrays();
}

LayoutManager::~LayoutManager() {
}

void LayoutManager::setScreenSize(int width, int height) {
    screenInfo_.width = width;
    screenInfo_.height = height;
    screenInfo_.availableWidth = width - paddingLeft_ - paddingRight_;
    screenInfo_.availableHeight = height - paddingTop_ - paddingBottom_;
    
    if (rootWidget_) {
        updateLayout();
    }
}

void LayoutManager::addWidget(std::unique_ptr<Widget> widget) {
    widgetMap_[widget->getId()] = std::move(widget);
    if (rootWidget_) {
        rootWidget_->addChild(std::move(widgetMap_[widget->getId()]));
    }
}

void LayoutManager::removeWidget(const std::string& id) {
    widgetMap_.erase(id);
}

Widget* LayoutManager::getWidget(const std::string& id) const {
    auto it = widgetMap_.find(id);
    if (it != widgetMap_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void LayoutManager::updateLayout() {
    if (!autoLayoutEnabled_ || !rootWidget_) return;
    
    // Simple layout algorithm - center root widget
    rootWidget_->setPosition(Position(paddingLeft_, paddingTop_));
    rootWidget_->setSize(Size(screenInfo_.availableWidth, screenInfo_.availableHeight));
    
    // Update all child widgets
    for (auto& child : rootWidget_->getChildren()) {
        child->calculateGeometry();
    }
}

void LayoutManager::render() const {
    if (!rootWidget_) return;
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Render root widget and all children
    rootWidget_->render();
}

bool LayoutManager::handleInput(const std::string& input) {
    if (!rootWidget_) return false;
    
    return rootWidget_->handleInput(input);
}

void LayoutManager::setTheme(const std::string& theme) {
    theme_ = theme;
}

void LayoutManager::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm
