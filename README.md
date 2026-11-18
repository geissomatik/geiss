[![Build for Windows using MSVC](https://github.com/geissomatik/geiss/actions/workflows/windows-msvc.yml/badge.svg)](https://github.com/geissomatik/geiss/actions/workflows/windows-msvc.yml)


# Geiss Screensaver and Winamp plug-in

This repository holds the latest code of the Geiss Screensaver and Winamp plug-in;
for general information about Geiss please see:

- [At a glance: Geiss](https://www.geisswerks.com/about_geiss.html)
- [Official Homepage of the Geiss Screensaver and Winamp plug-in](https://www.geisswerks.com/geiss/)
- [Screenshots from Geiss](https://www.geisswerks.com/geiss/shots.html)
- [How 'Geiss' Worked](https://www.geisswerks.com/geiss/secrets.html)
- [Geiss @ WinampHeritage.com](https://winampheritage.com/visualizations/geiss)
- [Geisswerks — home page of Ryan M. Geiss](https://www.geisswerks.com/)

For a YouTube video showing Geiss in action, please click:

[![Video "Windows 95 + Winamp + Geiss = Aesthetic"](https://img.youtube.com/vi/MfFcRsUZCxw/hqdefault.jpg)](https://youtu.be/MfFcRsUZCxw?t=15)


# macOS app

[`macos/`](macos/README.md) is a standalone window for Apple Silicon, on macOS 14.2 or later. It draws a few Geiss maps and waveforms, and the picture follows the sound coming out of the Mac.

- The looks are a horizontal tunnel, a vortex, a fast swirl, and a smooth zoom, with one line, two lines, or an X-Y oscilloscope. The colors are one cycling palette. The picture changes about every 20 seconds.
- Sensitivity is remembered. Pause, lock, and next visual are the other controls.
- Visual > Tone plays a steady note. Visual > Silence lets the line fade. Otherwise the window listens to the Mac's output.
- The picture engine is portable C++ in [`engine/`](engine). Metal displays it. The Windows screensaver and plug-in sources stay on their own build.

Build steps, the license, and troubleshooting are in [`macos/README.md`](macos/README.md). The product brief is [`docs/spec/macos-port.md`](docs/spec/macos-port.md).

# Compiling locally

In order to compile the Geiss Winamp plug-in and/or the Geiss Screensaver,
you need:

- A Git clone or snapshot of this repository
- CMake >=3.15
- Microsoft Visual Studio >=9/2008 (due to the current use of MSVC style assembly)

Then, the code can be built using:

```console
# cmake -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=RelWithDebInfo .
# cmake --build .
```
