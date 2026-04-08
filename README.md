# Spatial Sound Quality Assessment

This repository contains a spatial sound quality assessment project developed as part of the engineer thesis "Multidimensional Evaluation of Spatial Sound Quality Using MUSHRA Tests and SAQI Descriptors" at the Warsaw University of Technology.

It is based on the SAPETool (Spatial Audio Perceptual Evaluation Tool) application created by Tomasz Rudzki (@trsonic), designed for conducting MUSHRA-type listening tests. This version extends the original tool with additional functionality and integrates selected aspects from the SAQI (Spatial Audio Quality Inventory) dictionary [1].

The application enables real-time preparation and execution of listening tests within the REAPER DAW environment, connected with an intuitive user interface.


## Features

- MUSHRA-based listening test  
- Integration with REAPER DAW in real-time 
- Perceptual evaluation by aspects (SAQI)  
- Improved UI (responsiveness, playing sample highlighting)
- Export of test results to CSV and TXT report 

## Credits

This project is based on the work of Tomasz Rudzki.

Original repository:  
https://github.com/trsonic/sapetool

### Changes introduced in this version:
- UI improvements, including responsiveness and highlighting of the currently playing sample  
- Added export of results to CSV format  
- Performance optimizations (including marker reading)


## Requirements
To build the sapetool app Visual Studio and CMake are necessary. 

- Visual Studio: https://visualstudio.microsoft.com/pl/downloads/

- CMake: https://cmake.org/download/ 

Make sure your CMake version matches the one specified in `CMakeLists.txt`.

---

## Build Instructions

Run the following commands in the `sapetool` directory:

    cmake -B build -G "Visual Studio 18 2026" -A x64
    cmake --build build --config Release

You will find the `sapetool.exe` in the new folder: `sapetool\build\sapetool_artefacts\Release`.

## REAPER setup

See: `README_original.md`


## Test Aspects Configuration

The current version supports following aspects:

- (1) MUSHRA - standard MUSHRA test ("basic audio quality")

(8) Aspects from the SAQI dictionary:
- Tone color
- Externalization
- Localizability
- Reverberation
- Loudness
- Clarity
- Naturalness
- Degree of liking

You can change the aspects directly in the source code as described below.

### `MainComponent.cpp`

Change the titles of the aspects that will be presented to the user:

    juce::StringArray testTypes = {"aspect title 1", "aspect title 2", "aspect title 3", ...};

Assign aspect types to UI buttons:

    void MainComponent::launchTest() {
        MushraComponent::TestType typeToLaunch;

        if (testTypeSelectionButtons[0]->getToggleState())
            typeToLaunch = MushraComponent::TestType::aspect1;
        else if (testTypeSelectionButtons[1]->getToggleState())
            typeToLaunch = MushraComponent::TestType::aspect2;
        else if (testTypeSelectionButtons[2]->getToggleState())
            typeToLaunch = MushraComponent::TestType::aspect3;
        ...
        else
            return;  // no test type selected
    }


### `MushraComponent.h`

Define available aspects:

    enum TestType {
        aspect1,
        aspect2,
        aspect3,
        ...
    };

Customize labels and descriptions:

    void setupRatingLabels(TestType type) {
        testType = type;
        switch (type) {
        case MushraComponent::aspect1:
            testTypeText = "Aspect 1: you can describe the aspect here";
            ratingScaleLabels = {"Top scale label", "Good", "Fair", "Poor", "Bottom scale label"};
            break; 
        case MushraComponent::aspect2:
            testTypeText = "Aspect 2: you can describe the aspect here and break line like \n this";
            ratingScaleLabels = {"Top scale label", "Good", "Fair", "Poor", "Bottom scale label"};
            break;        
        ...

        default:
            break;
        }
        repaint();
    }

## References
[1] Lindau A; Erbes V; Lepa S; Maempel H-J; Brinkmann F; Weinzierl S (2014): "A Spatial Audio Quality Inventory for Virtual Acoustic Environments (SAQI)", in: Acta Acustica united with Acustica, 100(5): 984-994
