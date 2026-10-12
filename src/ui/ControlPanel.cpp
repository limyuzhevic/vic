#include "ControlPanel.hpp"
#include <iostream>

namespace nlm {

struct ControlPanel::Impl {
    std::string title;
    int width;
    int height;
    bool visible;
    std::vector<std::string> buttons;
    std::vector<std::string> sliders;
    std::vector<std::string> comboBoxes;
    std::vector<std::string> checkboxes;
    std::vector<std::string> textFields;
    std::function<void(const std::string&, const std::string&)> callback;
    std::string theme;
    std::string dockPosition;
    bool autoHide;
    
    Impl() : width(200), height(400), visible(true), autoHide(false) {}
};

ControlPanel::ControlPanel(const std::string& title) : pImpl(std::make_unique<Impl>()) {
    pImpl->title = title;
}

void ControlPanel::setTitle(const std::string& title) {
    pImpl->title = title;
}

std::string ControlPanel::getTitle() const {
    return pImpl->title;
}

void ControlPanel::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void ControlPanel::getSize(int& width, int& height) const {
    width = pImpl->width;
    height = pImpl->height;
}

void ControlPanel::show() {
    pImpl->visible = true;
    std::cout << "Panel shown: " << pImpl->title << std::endl;
}

void ControlPanel::hide() {
    pImpl->visible = false;
    std::cout << "Panel hidden: " << pImpl->title << std::endl;
}

bool ControlPanel::isVisible() const {
    return pImpl->visible;
}

void ControlPanel::addButton(const std::string& label, std::function<void()> callback) {
    pImpl->buttons.push_back(label);
    // In a real implementation, would store the callback
    std::cout << "Button added: " << label << std::endl;
}

void ControlPanel::addSlider(const std::string& label, double min, double max, double value) {
    pImpl->sliders.push_back(label + " (" + std::to_string(min) + " - " + std::to_string(max) + ", current: " + std::to_string(value) + ")");
    std::cout << "Slider added: " << label << std::endl;
}

void ControlPanel::addComboBox(const std::string& label, const std::vector<std::string>& options) {
    pImpl->comboBoxes.push_back(label + " (" + std::to_string(options.size()) + " options)");
    std::cout << "ComboBox added: " << label << std::endl;
}

void ControlPanel::addCheckbox(const std::string& label, bool value) {
    pImpl->checkboxes.push_back(label + " (" + (value ? "checked" : "unchecked") + ")");
    std::cout << "Checkbox added: " << label << std::endl;
}

void ControlPanel::addTextField(const std::string& label, const std::string& value) {
    pImpl->textFields.push_back(label + " (value: " + value + ")");
    std::cout << "TextField added: " << label << std::endl;
}

void ControlPanel::setCallback(std::function<void(const std::string&, const std::string&)> callback) {
    pImpl->callback = callback;
}

void ControlPanel::setTheme(const std::string& theme) {
    pImpl->theme = theme;
}

void ControlPanel::setDockPosition(const std::string& position) {
    pImpl->dockPosition = position;
}

void ControlPanel::setAutoHide(bool autoHide) {
    pImpl->autoHide = autoHide;
}

} // namespace nlm