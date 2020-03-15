[![Arduino Logo](https://www.vectorlogo.zone/logos/arduino/arduino-icon.svg)](https://arduino.cc/) [![Flutter Logo](https://www.vectorlogo.zone/logos/flutterio/flutterio-icon.svg)](https://flutter.dev/)

![Maintained][maintained-badge]
[![Travis Build Status][build-badge]][build]
[![Make a pull request][prs-badge]][prs]
<!-- [![License](http://img.shields.io/badge/Licence-MIT-brightgreen.svg)](LICENSE.md) -->

[![Watch on GitHub][github-watch-badge]][github-watch]
[![Star on GitHub][github-star-badge]][github-star]
[![Tweet][twitter-badge]][twitter]

# Barqino

Small boat driven by an on-board ESP8266, connected to a smartphone app through WiFi.

ESP8266 programmed using Arduino IDE first and PlatformIO for VS Code then. Smartphone app built with Flutter.

## Getting Started

Clone this repository locally :

``` bash
git clone https://github.com/marcodellemarche/barcuino.git
```

## Arduino

If you use PlatformIO, dependencies will be automatically downloaded. Otherwise, please look for these dependencies:

[WebSockets](https://github.com/Links2004/arduinoWebSockets/blob/master/src/WebSocketsServer.h)

## Flutter

Install dependencies for flutter :

``` bash
cd flutter
flutter pub get
```

If you want to generate Flutter components you **MUST** install `flutter` globally.
Please follow [flutter documentation](https://flutter.dev/docs/get-started/install).
