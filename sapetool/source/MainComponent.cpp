#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() {
  getTracksAndMarkersFromDawButton.setButtonText("Get DAW Configuration");
  getTracksAndMarkersFromDawButton.onClick = [this] {
    getTracksAndMarkersFromDaw();
    getTracksAndMarkersFromDawButton.setVisible(false);
  };
  addAndMakeVisible(getTracksAndMarkersFromDawButton);

  juce::StringArray testTypes = {"MUSHRA", "Tone color", "Externalization", "Localizability", "Level of Reverberation",
                                "Loudness", "Clarity", "Naturalness", "Degree of Liking"};

  for (auto test : testTypes) {
    testTypeSelectionButtons.add(new juce::ToggleButton());
    testTypeSelectionButtons.getLast()->setButtonText(test);
    testTypeSelectionButtons.getLast()->setRadioGroupId(1);
    testTypeSelectionButtons.getLast()->setColour(
        juce::ToggleButton::textColourId, juce::Colours::black);
    testTypeSelectionButtons.getLast()->setColour(
        juce::ToggleButton::tickColourId, juce::Colours::black);
    addAndMakeVisible(testTypeSelectionButtons.getLast());
  }

  initGuiButton.setButtonText("Launch Test");
  initGuiButton.onClick = [this] {
    launchTest();
  };

  saveResultsButton.setButtonText("Save Full Report");
  saveResultsButton.onClick = [this] {
    mc.saveResults();
  };

  saveRawResultsButton.setButtonText("Save Results as CSV");
  saveRawResultsButton.onClick = [this] {
      mc.saveRawResults();
  };

  returnButton.setButtonText("Return");
  returnButton.onClick = [this] {
      returnToMain();
  };

  dawTxIpLabel.setEditable(false, true, false);
  dawTxPortLabel.setEditable(false, true, false);
  dawRxPortLabel.setEditable(false, true, false);

  dawTxIpLabel.setText("127.0.0.1", juce::dontSendNotification);
  dawTxPortLabel.setText("9696", juce::dontSendNotification);
  dawRxPortLabel.setText("9797", juce::dontSendNotification);
  addAndMakeVisible(dawTxIpLabel);
  addAndMakeVisible(dawTxPortLabel);
  addAndMakeVisible(dawRxPortLabel);

  // OSC sender and receiver connect
  DBG("Connecting OSC ...");
  dawTxRx.connectSender(dawTxIpLabel.getText(), dawTxPortLabel.getText().getIntValue());
  dawTxRx.connectReceiver(dawRxPortLabel.getText().getIntValue());
  dawTxRx.addListener(this);

  setSize(1280, 600);

}

MainComponent::~MainComponent() {
  dawTxRx.disconnectSender();
  dawTxRx.disconnectReceiver();
}

void MainComponent::paint(juce::Graphics& g) {
  // BACKGROUND
  g.fillAll(juce::Colours::gainsboro);

  // RECTANGULAR OUTLINE
  g.setColour(juce::Colours::black);
  g.drawRect(getLocalBounds(), 1);
  g.drawRect(juce::Rectangle<int>(0, 0, menuWidth, getHeight()), 1);
  g.drawRect(juce::Rectangle<int>(0, getHeight() - 110, menuWidth, 110), 1);

  g.setFont(juce::Font(16.0f));
  g.setColour(juce::Colours::black);
  g.drawText("Listening Test App", 25, 20, 200, 25,
             juce::Justification::centredLeft, true);

  g.setFont(juce::Font(14.0f));

  if (dawReady) {
    g.drawText("Number of Trials: " + juce::String(numberOfTrials), 25, 50, 200, 25, juce::Justification::centredLeft, true);
    g.drawText("Number of Conditions: " + juce::String(numberOfConditions), 25, 70, 200, 25, juce::Justification::centredLeft, true);
  }

  int labelXPos = 25;
  int labelYPos = getHeight() - 85 - 25;

  g.drawText("IP", labelXPos + 42, labelYPos, 50, 25,
             juce::Justification::centredLeft, true);
  g.drawText("Tx Port", labelXPos + 115, labelYPos, 50, 25,
             juce::Justification::centredLeft, true);
  g.drawText("Rx Port", labelXPos + 165, labelYPos, 50, 25,
             juce::Justification::centredLeft, true);
  g.drawText("DAW", labelXPos, labelYPos + 25, 50, 25,
             juce::Justification::centredLeft, true);
}

void MainComponent::resized() {
  getTracksAndMarkersFromDawButton.setBounds(28, 50, 200, 25);

  for (int i = 0; i < testTypeSelectionButtons.size(); i++) {
    testTypeSelectionButtons[i]->setBounds(25, testTypeSelectionTop + i * testTypeSelectionSpacing, 200, 25);
  }

  initGuiButton.setBounds(28, testTypeSelectionTop + testTypeSelectionButtons.size() * testTypeSelectionSpacing + 10, 100, 25);

  saveRawResultsButton.setBounds(28, testTypeSelectionTop + 10, 200, 25);
  saveResultsButton.setBounds(28, testTypeSelectionTop + 40, 200, 25);
  returnButton.setBounds(28, testTypeSelectionTop + 70, 200, 25);

  int x;
  x = getHeight();

  // OSC LABELS
  int labelXPos = 60;
  int labelYPos = getHeight() - 60 - 25;
  dawTxIpLabel.setBounds(labelXPos, labelYPos, 75, 25);
  dawTxPortLabel.setBounds(labelXPos + 75, labelYPos, 50, 25);
  dawRxPortLabel.setBounds(labelXPos + 125, labelYPos, 50, 25);

  dawTxIpLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  dawTxPortLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  dawRxPortLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  dawTxIpLabel.setJustificationType(juce::Justification::centredLeft);
  dawTxPortLabel.setJustificationType(juce::Justification::centredLeft);
  dawRxPortLabel.setJustificationType(juce::Justification::centredLeft);

  // MC SIZE CONFIG
  int w, h;
  w = getWidth(); h = getHeight();
  mc.setSize(w - menuWidth, h);
  mc.setTopLeftPosition(menuWidth, 0);
}

void MainComponent::getTracksAndMarkersFromDaw() {

  DBG("Getting tracks / markers ...");
  countMarkers = true;
  countTracks = true;

  dawTxRx.sendOscMessage("/time", 0);
  // deselect tracks......

  markerTimeArray.clear();

  markerIndex = 1;
  trackIndex = 1;

  lastMarkerIndex = 0;
  lastTrackIndex = 0;
  trackArrayIndex = 0;

  startTimer(250);
}

void MainComponent::timerCallback() {
  if (markerIndex == lastMarkerIndex + 2) countMarkers = false;
  if (trackIndex == lastTrackIndex + 2) countTracks = false;

  if (countMarkers)
  {
    DBG("Jump to marker " + juce::String(markerIndex));
    dawTxRx.sendOscMessage("/marker",markerIndex);
    markerIndex++;
  }

  if (countTracks)
  {
    DBG("Jump to track " + juce::String(trackIndex));
    dawTxRx.sendOscMessage("/device/track/select", trackIndex);
    trackIndex++;
  }

  if (countMarkers == false && countTracks == false)
  {
    stopTimer();
    dawTxRx.sendOscMessage("/time", 0);

    for (int i = 0; i < markerTimeArray.size(); i++)
    {
      DBG("Marker " + juce::String(i+1) + " position: " + juce::String(markerTimeArray[i]));
    }

    for (int i = 0; i < trackNameArray.size(); i++)
    {
      DBG("Track " + juce::String(i+1) + " name: " + trackNameArray[i]);
    }

    if (markerTimeArray.size() >= 2 && trackNameArray.size() >= 2)
    {
      numberOfTrials = floor(markerTimeArray.size() / 2);
      numberOfConditions = trackNameArray.size();
      dawReady = true;

      addAndMakeVisible(initGuiButton);
    }
    else 	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "DAW error",
        "Not enough markers or tracks",
        "OK");
  }

  repaint();
}

void MainComponent::oscBundleReceived(const juce::OSCBundle& bundle) {
  for (int i =0; i < bundle.size(); ++i)
    oscMessageReceived(bundle[i].getMessage());
}

void MainComponent::oscMessageReceived(const juce::OSCMessage& message) {
  juce::String msg;
  msg += "OSC received: ";
  msg += message.getAddressPattern().toString();
  msg += "; ";
  for (int i = 0; i < message.size(); ++i) {
    if (message[i].isString())
      msg += message[i].getString();
    else if(message[i].isFloat32())
      msg += juce::String(message[i].getFloat32());
    else if(message[i].isInt32())
      msg += juce::String(message[i].getInt32());
    msg += "; ";
  }

  DBG(msg);

  if (message.size() == 1 &&
      message.getAddressPattern() == "/lastmarker/number/str" &&
      message[0].isString()) {
    lastMarkerIndex = message[0].getString().getIntValue();
    DBG("Last marker index: " + juce::String(lastMarkerIndex));
  }

  if (message.size() == 1 &&
      message.getAddressPattern() == "/time" &&
      message[0].isFloat32() && message[0].getFloat32() != 0.0f) {
    currentPosition = message[0].getFloat32();
    markerTimeArray.set(markerIndex, currentPosition);
    DBG("Last marker position: " + juce::String(currentPosition));
  }

  if (message.size() == 1 &&
      message.getAddressPattern() == "/track/number/str" &&
      message[0].isString()) {
    lastTrackIndex = message[0].getString().getIntValue();
    DBG("Last track index: " + juce::String(lastTrackIndex));
  }

  if (message.size() == 1 &&
      message.getAddressPattern() == "/track/name" &&
      message[0].isString()) {
    juce::String trackName = message[0].getString();
    DBG("Last track name: " + trackName);
    if (trackName.startsWith("##")) {
      trackNameArray.set(trackArrayIndex, trackName);
      trackArrayIndex++;
    }
  }
}

void MainComponent::launchTest() {
  MushraComponent::TestType typeToLaunch;

  if (testTypeSelectionButtons[0]->getToggleState())
    typeToLaunch = MushraComponent::TestType::standardMushra;
  else if (testTypeSelectionButtons[1]->getToggleState())
    typeToLaunch = MushraComponent::TestType::tonecolor;
  else if (testTypeSelectionButtons[2]->getToggleState())
    typeToLaunch = MushraComponent::TestType::externalization;
  else if (testTypeSelectionButtons[3]->getToggleState())
    typeToLaunch = MushraComponent::TestType::localizability;
  else if (testTypeSelectionButtons[4]->getToggleState())
    typeToLaunch = MushraComponent::TestType::reverberation;
  else if (testTypeSelectionButtons[5]->getToggleState())
    typeToLaunch = MushraComponent::TestType::loudness;
  else if (testTypeSelectionButtons[6]->getToggleState())
    typeToLaunch = MushraComponent::TestType::clarity;
  else if (testTypeSelectionButtons[7]->getToggleState())
    typeToLaunch = MushraComponent::TestType::naturalness;
  else if (testTypeSelectionButtons[8]->getToggleState())
    typeToLaunch = MushraComponent::TestType::degreeOfLiking;
  else
    return;  // no test type selected

  mc.setupRatingLabels(typeToLaunch);

  for (auto testTypeSelectionButton : testTypeSelectionButtons) {
    testTypeSelectionButton->setVisible(false);
    testTypeSelectionButton->setToggleState(false, juce::dontSendNotification);
  }

  mc.regionArray.clear();
  for (int i = 0; i < numberOfTrials; ++i) {
    mc.regionArray.add(new SampleRegion());
    mc.regionArray[i]->dawStartTime = markerTimeArray[i * 2];       // 0 2 4 6
    mc.regionArray[i]->dawStopTime = markerTimeArray[(i * 2) + 1];  // 1 3 5 7
    mc.regionArray[i]->calculateStartEndTimes();
  }
  mc.numberOfSamplesPerRegion = numberOfConditions;
  mc.trackNameArray.addArray(trackNameArray);
  mc.connectOsc(&dawTxRx,
    dawTxIpLabel.getText(),
    dawTxPortLabel.getText().getIntValue(),
    dawRxPortLabel.getText().getIntValue());
  mc.createGui();
  addAndMakeVisible(&mc);
  mushraComponentConnected = true;
  initGuiButton.setVisible(false);
  addAndMakeVisible(&saveResultsButton);
  addAndMakeVisible(&saveRawResultsButton);
  addAndMakeVisible(&returnButton);
  repaint();
}

void MainComponent::returnToMain() {
    for (auto testTypeSelectionButton : testTypeSelectionButtons) {
        testTypeSelectionButton->setVisible(true);
        testTypeSelectionButton->setToggleState(false, juce::dontSendNotification);
    }

    mc.setVisible(false);
    initGuiButton.setVisible(true);
    saveResultsButton.setVisible(false);
    saveRawResultsButton.setVisible(false);
    returnButton.setVisible(false);

    repaint();
}