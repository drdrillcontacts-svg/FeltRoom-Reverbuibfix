#pragma once
#include "PluginProcessor.h"

class FeltRoomEditor final : public juce::AudioProcessorEditor
{
public:
    explicit FeltRoomEditor(FeltRoomProcessor&);
    ~FeltRoomEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FeltRoomProcessor& proc;
    juce::Image piano;
    struct CopperLook final : juce::LookAndFeel_V4
    {
        void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
        void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    } look;

    std::array<std::unique_ptr<juce::Slider>, 7> knobs;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 7> attachments;
    std::array<std::unique_ptr<juce::Label>, 7> knobLabels;
    std::array<std::unique_ptr<juce::Label>, 7> valueLabels;
    std::array<std::unique_ptr<juce::TextButton>, 6> effectTabs;
    std::array<std::unique_ptr<juce::TextButton>, 10> categoryButtons;
    std::array<std::unique_ptr<juce::TextButton>, 3> presetColumns;
    std::vector<std::unique_ptr<juce::TextButton>> presetButtons;
    juce::TextEditor search;
    juce::ComboBox presetSelector, mode;
    juce::TextButton save{"▣  SAVE"}, load{"▱  LOAD"}, settings{"⚙"}, random{"⤨  RANDOM"}, previous{"‹"}, next{"›"}, menuButton{"☰  PRESETS"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    std::unique_ptr<juce::FileChooser> chooser;
    juce::String selectedCategory{"ALL PRESETS"};
    juce::String selectedPreset{"Intimate Room"};
    juce::String searchText;
    bool settingsOpen = false;

    void savePreset();
    void loadPreset();
    void rebuildPresetList();
    void applyFactoryPreset(const juce::String& name);
    void selectMode(int index);
    juce::String formatValue(int index, double value) const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeltRoomEditor)
};
