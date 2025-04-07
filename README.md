# Key Quest
Key Quest is an interactive piano learning and practice software for the Raspberry Pi, designed to help users develop their musical skills through guided exercises, real-time feedback, and adaptive quizzes. It offers practice for chords, scales, and octaves with visual aids and scoring, plus a freestyle mode and multiplayer options. The AI-driven quiz adapts to the user's previous performance for personalized challenges, making learning engaging and effective with expansion potential.

# Required Libraries and Tools
To build and run the KeyQuest, ensure you have the following tools installed:

- Qt Creator with Qt Framework:
Qt 5.15 or higher (Minimum Requirement): [Get and Install Qt | Qt 5.15](https://doc.qt.io/qt-5/get-and-install-qt.html)
Qt 6.8.2 (recommended): [Get and Install Qt | Qt 6.8](https://doc.qt.io/qt-6/get-and-install-qt.html)
- C++ compiler (MSVC or G++): 
	Version: 13.2.0  [GCC Releases - GNU Project](https://gcc.gnu.org/releases.html)
- OS:
Raspberry Pi OS, which is based on Debian. This is the intended platform.
Windows (requires Chocolatey)
MacOS
Ubuntu, Debian, and their derivatives (such as Linux Mint).
- FluidSynth:
The FluidSynth libraries can be installed on Raspberry Pi OS and Debian/Ubuntu distributions by running “sudo apt update” and then “sudo apt install fluidsynth libfluidsynth-dev” in the terminal. Consult https://github.com/FluidSynth/fluidsynth/wiki/Download for other platforms.
- CMake (optional for command-line builds):
	https://cmake.org/download/

# Building the Software
### Step 1: Clone the Repository
Open a terminal/command prompt and clone the project in your desired location.

Clone with ssh:
"git clone ssh://git@gitlab.sci.uwo.ca:7999/courses/2025/01/COMPSCI3307/group30.git"

Clone with HTTPS:
“git clone https://gitlab.sci.uwo.ca/courses/2025/01/COMPSCI3307/group30.git”

### Step 2: Build the Project
Navigate to the “group30/KeyQuest” directory.

Run From QT Creator:
1. Open **Qt Creator**.
2. Load the `KeyQuest.pro` file from the root directory.
3. Select your build kit (e.g., Desktop Qt 6.5.0).
4. Click **Build** → **Run**.

Run From Terminal:
Run “qmake” or “qmake6,” depending on your installed version of Qt.
Run “make”
Run “./KeyQuest”


**Note:** Please make sure FluidSynth is installed and accessible on your system. The application depends on it for MIDI playback.

# Using the Software
The software is very straightforward to use. Run the application by building the project, running the .exe file, and navigate your way in the game using the buttons provided.
