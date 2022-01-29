# Hello-World-SDK-3

[![Build all Platforms](https://github.com/TwinFan/HelloWorld-XP/actions/workflows/build.yml/badge.svg)](https://github.com/TwinFan/HelloWorld-XP/actions/workflows/build.yml)

This is a "Hello World" example plugin for use with X-Plane and can serve as a basis for your own plugin development. Its code is originally taken from [X-Plane's developer site](https://developer.x-plane.com/code-sample/hello-world-sdk-3/).

Added are an IDE project for XCode as well as a `CMakeList.txt` setup for use with MS Visual Studio, the provided Docker environment for cross platform build, and GitHub Actions.

## Credits
LiveTraffic is based on a number of other great libraries and APIs, most notably:
- [X-Plane SDK](https://developer.x-plane.com/sdk/plugin-sdk-documents/) to integrate with X-Plane

## Build

- GitHub Actions build setup is provided in `actions/workflows/build.yml`
- MacOS: Just open `HelloWorld.xcodeproj` in Xcode.
- MS Visual Studio: Do "File > Open > Folder..." on the project's main folder. VS will initialize based on `CMakeList.txt` and `CMakeSettings.json`.
- For Cross Compile in a Docker environment for Linux, Mac, and Windows see `docker/README.md`.
