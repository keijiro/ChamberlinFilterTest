#include "../JuceLibraryCode/JuceHeader.h"
#include "ChamberlinFilter.h"

class MainContentComponent : public AudioAppComponent
{
public:
    MainContentComponent()
    {
        filterTypeLabel.setText("Filter Type", dontSendNotification);
        filterTypeBox.addItem("Low-pass", 1);
        filterTypeBox.addItem("Band-pass", 2);
        filterTypeBox.addItem("High-pass", 3);
        filterTypeBox.setSelectedId(1);

        cutoffFreqLabel.setText("Cutoff Frequency", dontSendNotification);
        cutoffFreqSlider.setRange(10, 4000);
        cutoffFreqSlider.setTextBoxStyle(Slider::TextBoxRight, false, 100, 20);

        addAndMakeVisible(filterTypeLabel);
        addAndMakeVisible(filterTypeBox);
        addAndMakeVisible(cutoffFreqLabel);
        addAndMakeVisible(cutoffFreqSlider);

        setSize(600, 100);
        setAudioChannels(1, 1);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        filter.initialize(static_cast<float>(cutoffFreqSlider.getValue()), 1, static_cast<float>(sampleRate));
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        auto device = deviceManager.getCurrentAudioDevice();

        const auto activeInputChannels = device->getActiveInputChannels();
        const auto activeOutputChannels = device->getActiveOutputChannels();
        const auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
        const auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

        if (maxInputChannels == 0 || maxOutputChannels == 0 || !activeInputChannels[0] || !activeOutputChannels[0])
        {
            bufferToFill.clearActiveBufferRegion();
        }
        else
        {
            auto filterType = filterTypeBox.getSelectedId();
            auto filterFunc = &ChamberlinFilter::getLowPassOut;
            if (filterType == 2) filterFunc = &ChamberlinFilter::getBandPassOut;
            if (filterType == 3) filterFunc = &ChamberlinFilter::getHighPassOut;

            filter.setCutoffFreq(static_cast<float>(cutoffFreqSlider.getValue()), 1);

            const auto inBuffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
            auto outBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples; ++i)
            {
                filter.feedSample(inBuffer[i]);
                outBuffer[i] = (filter.*filterFunc)();
            }
        }
    }

    void releaseResources() override
    {
    }

    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        filterTypeLabel.setBounds(10, 10, 190, 20);
        cutoffFreqLabel.setBounds(10, 30, 190, 40);
        filterTypeBox.setBounds(200, 10, 300, 20);
        cutoffFreqSlider.setBounds(200, 30, getWidth() - 210, 40);
    }

private:
    ChamberlinFilter filter;
    ComboBox filterTypeBox;
    Label filterTypeLabel;
    Slider cutoffFreqSlider;
    Label cutoffFreqLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};

Component* createMainContentComponent() { return new MainContentComponent(); }
