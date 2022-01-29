# Hello-World-SDK-3

[![Build all Platform](https://github.com/TwinFan/HelloWorld-XP/actions/workflows/build.yml/badge.svg)](https://github.com/TwinFan/HelloWorld-XP/actions/workflows/build.yml)

This is a "Hello World" example plugin for use with X-Plane and can serve as a basis for your own plugin development. Its code is originally taken from [X-Plane's developer site](https://developer.x-plane.com/code-sample/hello-world-sdk-3/).

Added are IDE projects for XCode and MS Visual Studio as well as a `CMakeList.txt` setup for use with Visual Studio and the provided Docker environment for cross platform build.


## Credits
LiveTraffic is based on a number of other great libraries and APIs, most notably:
- [X-Plane SDK](https://developer.x-plane.com/sdk/plugin-sdk-documents/) to integrate with X-Plane

## Build

- GitHub Actions build setup is provided in `actions/workflows/build.yml`
- MacOS: Just open `HelloWorld.xcodeproj` in Xcode.
- For Cross Compile for Linux, Mac, and Windows see `docker/README.md`.
