# ScaleSequence

ScaleSequence is a prototype [MTS-ESP](https://github.com/ODDSound/MTS-ESP) master plugin, which allows the dynamic switching of the current scale according to a set sequence.

![ScaleSequence](https://github.com/eventual-recluse/ScaleSequence/blob/main/plugins/ScaleSequence/ScaleSequence_screenshot.png "ScaleSequence")<br/>

# Settings

Each of the four scales can be set by loading a either a Scala scale file (.scl), keymapping file (.kbm) file, or both. Click "Open SCL File" or "Open KBM File" to choose the file.

The sequence has 16 steps. Set the scale for each step by clicking the sequence buttons.

More parameters:

**Step Multi:** Multiplies the length of the step. e.g. if the step type is beats, 2 will set each step to 2 beats.<br>
**Step Type:** The steps can be in beats or bars.<br>
**Glide:** The glide amount for smoothly switching between scales. The higher the glide amount, the longer it will take to switch completely. (1.00 means no glide.)<br>
**Offset:** This setting allows the timing of the scale switching be moved a little earlier or later. Up to -1 or +1 beat or bar (depending on the step type chosen).<br>

# Build instructions

## Build Instructions: Ubuntu
Install dependencies, clone this repository, enter the cloned repository, then make.
```
sudo apt-get install build-essential git pkg-config freeglut3-dev
git clone --recursive https://github.com/eventual-recluse/ScaleSequence.git
cd ScaleSequence
make
```
After building, the plugins can be found in the 'bin' folder.

## Build Instructions: Windows.
ScaleSequence can be built using [msys2](https://www.msys2.org/)
After installing msys2, launch the MinGW64 shell and enter the following commands to install dependencies, clone this repository, enter the cloned repository, then make.
```
pacman -S base-devel git mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut
git clone --recursive https://github.com/eventual-recluse/ScaleSequence.git
cd ScaleSequence
make
```
After building, the plugins can be found in the 'bin' folder.

## Building on macOS.
ScaleSequence can be built on macOS using the Xcode Command Line Tools.

Install Xcode Command Line Tools from the Terminal:
```
xcode-select --install
```
Then enter the following commands in the Terminal to clone this repository, enter the cloned repository, then make.
```
git clone --recursive https://github.com/eventual-recluse/ScaleSequence.git
cd ScaleSequence
make
```
After building, the plugins can be found in the 'bin' folder.

# Credits
[DISTRHO Plugin Framework.](https://github.com/DISTRHO/DPF) ISC license.

[MTS-ESP.](https://github.com/ODDSound/MTS-ESP) 0BSD license.

[Surge Synthesizer Tuning Library.](https://github.com/surge-synthesizer/tuning-library) MIT license.

[Dear ImGui.](https://github.com/ocornut/imgui) MIT license.

[Bruno Ace Font](https://fonts.google.com/specimen/Bruno+Ace) designed by Astigmatic. [Open Font License.](https://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL)

[Bruno Ace SC Font](https://fonts.google.com/specimen/Bruno+Ace+SC) designed by Astigmatic. [Open Font License.](https://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL)

[Lekton Font](https://fonts.google.com/specimen/Lekton) designed by ISIA Urbino. [Open Font License.](https://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL)
