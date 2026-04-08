#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_osc/juce_osc.h>

#include <random>
#include <vector>
#include "OscTransceiver.h"
#include "SampleRegion.h"

using std::vector;

class MushraComponent : public juce::Component,
                        public juce::Button::Listener,
                        public juce::Slider::Listener,
                        public juce::OSCReceiver,
                        public juce::OSCReceiver::Listener<
                            juce::OSCReceiver::MessageLoopCallback> {
 public:
  //==============================================================================
  MushraComponent();
  ~MushraComponent();

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

  const int infoAreaHeight = 80;
  const int titleHeight = 50;
  const int sliderButtonPanelHeight = 200;
  const int rateLabelWidth = 120;

  enum TestType {
	  standardMushra,
    tonecolor,
    externalization,
    localizability,
    reverberation,
    loudness,
    distortion,
    clarity,
    naturalness,
    degreeOfLiking
  };

  void setupRatingLabels(TestType type) {
    testType = type;
    switch (type) {
		case MushraComponent::standardMushra:
        testTypeText = "MUSHRA: Rate the quality of sound";
        ratingScaleLabels = {"Excellent", "Good", "Fair", "Poor", "Bad"};
        break; 
      case MushraComponent::tonecolor:
        testTypeText = "Tone color: Rate the timbral impression which is determined by the ratio\nof high to low frequency components.";
        ratingScaleLabels = {"Brighter", "Slightly brighter", "Neutral", "Slightly darker", "Darker"};
        break;        
      case MushraComponent::externalization:
        testTypeText =
            "Rate how you perceive the sound sources, whether they seem to be coming from within your head\nor outside of it, regardless of their distance.";
        ratingScaleLabels = {"More Externalized", "Slightly more externalized", "Neutral",
                             "Sligthly more internalized", "More internalized"};
        break;
      case MushraComponent::localizability:
        testTypeText = "Localizability: Rate if localizability spatial extent and location"
        " of a sound source are difficult to estimate or appear diffuse,"
        " or if a sound source is clearly delimited.";
        ratingScaleLabels = {"Easier", "Slightly easier", "Neutral",
                             "Sligthly more difficult", "More difficult"};
        break;
      case MushraComponent::reverberation:
        testTypeText = "Level of reverberation: Rate how strong is the perception of a reverberant sound field,\ncaused by a high ratio of reflected to direct sound energy.";
        ratingScaleLabels = {"More", "Slightly more", "Neutral",
                             "Slightly less", "Less"};
        break;
      case MushraComponent::loudness:
        testTypeText = "Loudness: Rate the Perceived loudness of a sound source.";
        ratingScaleLabels = {"Louder", "Slightly louder", "Neutral",
                             "Slightly quieter", "Quieter"};
        break;
      case MushraComponent::clarity:
        testTypeText = "Clarity: Impression of how clearly different elements in a scene\ncan be distinguished from each other, how well various properties of individual\nscene elements can be detected. ";
        ratingScaleLabels = {"More pronounced", "Slightly more pronounced", "Neutral",
                             "Slightly less pronounced", "Less pronounced"};
        break;
      case MushraComponent::naturalness:
        testTypeText = "Naturalness: Rate the impression that a signal is in accordance\nwith the expectation/former experience of an equivalent signal.";
        ratingScaleLabels = {"Higher", "Slightly higher", "Neutral",
                             "Slightly lower", "Lower"};
        break;
      case MushraComponent::degreeOfLiking:
        testTypeText =
            "Degree of Liking: Rate how much you like or dislike the scene\nbased on its overall pleasantness or unpleasantness.";
        ratingScaleLabels = {"Higher", "Slightly higher", "Neutral",
                             "Slightly lower", "Lower"};
        break;

      default:
        break;
    }
    repaint();
  }

  void setupSampleLabels()
  {
      sampleLabels = { "REFERENCE (ambix)", "KOTWICA (mono and low-pass filtered)", "hodirac", "IEM", "cropac", "sparta" };
  }
  
  void setupTestTrialLabels()
  {
      testTrialLabels = { "Clock test", "SFX tapping", "Libertango by orchestra", "SFX Bells", "SFX Bells and drums", "Seashore", "Scooter and people", "Birds"};
  }

  juce::OwnedArray<SampleRegion> regionArray;
  vector<vector<float>> scoresArray;
  vector<int> trialRandomizationArray;
  vector<vector<int>> sampleRandomizationArray;
  juce::Array<juce::String> trackNameArray;
  bool randomizeSamples = true;
  int numberOfSamplesPerRegion = 0;

  juce::String hostIp;

  void createGui();
  void connectOsc(OscTransceiver* oscTxRx, juce::String dawIp, int dawTxPort, int dawRxPort);
  void saveResults();
  void saveRawResults();
  void returnToMain();

 private:
  OscTransceiver* dawTxRx;

  // APP CONFIG
  TestType testType = standardMushra;
  bool isReferenceButtonVisible = true;

  // APP STATE
  int currentRegion = 0, currentTrack = 0;
  int currentTrialIndex = 0;
  float currentPosition = 0.0f;
  bool playback = false;
  bool loop = true;
  juce::Label dawTimeLabel, sampleTimeLabel;
  bool showInfo = false;

  // test type specific text
  juce::String testTypeText;
  juce::StringArray ratingScaleLabels;
  juce::StringArray testTrialLabels;
  juce::StringArray sampleLabels;

  // MUSHRA CONTROLS
  juce::TextButton playReference;
  juce::TextButton stopPlaybackB;
  juce::TextButton goToPrevious, goToNext;
  juce::OwnedArray<juce::Slider> rateSampleSliderArray;
  juce::OwnedArray<juce::Label> rateSampleSliderLabelArray;
  juce::OwnedArray<juce::Button> playSampleButtonArray;
  juce::Slider loopSlider;
  juce::ToggleButton loopTB;
  juce::ToggleButton showInfoTB;

  // METHODS
  void playSample(int track, bool randomize);
  void playSampleLoop();
  void stopPlayback();
  void buttonClicked(juce::Button* buttonThatWasClicked) override;
  void sliderValueChanged(juce::Slider* sliderThatWasChanged) override;
  void oscBundleReceived(const juce::OSCBundle& bundle) override;
  void oscMessageReceived(const juce::OSCMessage& message) override;
  void updateTransportSlider(bool updateLoop);
  void updateRatingSliders();
  void updatePrevNextButtons();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MushraComponent)
};
