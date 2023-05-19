<h1 align="center">image feature matching for Flutter</h1>

<p align="center">
<a href="https://pub.dev/packages/flutter_pixelmatching"><img src="https://img.shields.io/pub/v/flutter_pixelmatching.svg" alt="Pub"></a>
<a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/license-MIT-purple.svg" alt="License: MIT"></a>

[//]: # (<a href="https://github.com/lowapple/flutter_pixelmatching/actions"><img src="https://github.com/lowapple/flutter_pixelmatching/actions/workflows/main.yml/badge.svg" alt="build"></a>)
[//]: # (<a href="https://codecov.io/gh/lowapple/flutter_pixelmatching">)
[//]: # (  <img src="https://codecov.io/gh/lowapple/flutter_pixelmatching/branch/main/graph/badge.svg"/>)
[//]: # (</a>)
</p>

> Plugin to perform feature matching using OpenCV

An image comparison module written as an exercise in Flutter FFI and out of a need for that functionality. It is called PixelMatching, but it is actually a FeatureMatching module utilizing OpenCV. Isolate was utilized for asynchronous processing.
For the internal functionality, it was written in C++. 

The matching algorithm is using **FLannBasedMatcher**, and for the detector, **Andorid uses SIFT** **iOS uses KAZE**.

There is currently no method to change the algorithm, so if you want to change the algorithm and test it, please refer to `ios/Classes/src/ImageProcessor.cpp` and change it.
```c++
ImageProcessor::ImageProcessor() : stateCode(NotInitialized) {
    compare.setMatchers(cv::DescriptorMatcher::create(DescriptorMatcher::MatcherType::FLANNBASED));
#ifdef __ANDROID__
    compare.setDetector(cv::SIFT::create());
#elif __APPLE__
    compare.setDetector(cv::KAZE::create());
#endif
}
```

## Demo Screenshots

![sample](https://user-images.githubusercontent.com/26740046/234154847-d3199f18-b262-45f1-8b9f-4153e11b5f80.png)

## Features
* [x] Plugins utilizing OpenCV
* [x] Perform image comparisons on camera streams (mobile platforms only), image files.
* [x] Compares image features to calculate and return similarity

## Todo
* [ ] Write plugin test code
* [ ] Reduce the overall size of the plugin 
  * [x] Do not build x86 [Flutter Building the app for release](https://docs.flutter.dev/deployment/android#build-an-app-bundle)
  * [ ] Find a way to avoid importing third-party library sources directly inside the plugin

## Supported platforms
Flutter PixelMatching is available for Android and iOS.

* Android (min SDK 24)
* iOS (min iOS version 11.0)

flutter_pixelmatching is currently written to be available only on mobile and there are no plans for other platforms.

## Getting started
### Installing dependencies
```yaml
flutter pub add flutter_pixelmatching
```
or
```yaml
dependencies:
  flutter_pixelmatching: ^0.1.5
```
#### XCode 
Symbolic errors in the archive when using the FFi module.<br/>
For this issue, check out the following link https://github.com/dart-lang/ffi/issues/41#issuecomment-645550964

**Runner -> Build Settings -> Strip Style -> change from "All Symbols" to "Non-Global Symbols"**

![XCode-Settings](https://github.com/lowapple/flutter_pixelmatching/assets/26740046/929dc7f7-40c4-4d49-95fe-d5b57346a931)


### Usage 
```dart
final matching = PixelMatching();
```
```dart
// An enum value indicating the processing status of the module. If a marker is registered after initialization, it will change to the 'noQuery' state. 
enum PixelMatchingState {
  // The pixel matching is not initialized.
  notInitialized,
  // target image is not set.
  noMarker,
  // query image is not set.
  noQuery,
  // is processing.
  processing,
}
```

### Initialize
Initialize while registering the marker image.<br/>
**Support ImageType jpeg/bgra8888**<br/>
For ease of image handling, we currently support two image types. 
```dart
// from Camera Stream 
final imgType = Platform.isAndroid ? "jpeg" : "bgra8888";
final img = cameraImage.planes[0].bytes;
final w = cameraImage.width;
final h = cameraImage.height;
matching?.initialize(imgType, img, w, h);
```
```dart
// from ImagePicker
final imagePicker = ImagePicker();
final xImage = await imagePicker.pickImage(source: ImageSource.camera);
if (xImage != null) {
  final image = await imglib.decodeJpgFile(xImage.path);
  matching?.initialize("jpeg", imglib.encodeJpg(image!), image.width, image.height);
}
```
### Query
Calculates the current similarity compared to the registered image at initialization.
```dart
final status = await matching.getStateCode();
if(status == PixelMatchingState.noQuery) {
  final imgType = Platform.isAndroid ? "jpeg" : "bgra8888";
  final img = cameraImage.planes[0].bytes;
  final w = cameraImage.width;
  final h = cameraImage.height;
  final confidence = await matching?.setQuery(imgType, img, w, h, rotation: rotation);
  print(confidence);
  setState(() {
    similarity = confidence ?? 0.0;
  });
}
```
### MarkerQueryDifferenceImage
Functions to visually see which images are currently being processed. This is an optional function that is provided separately to ensure that images are being processed well during development.

```dart
// Use package:image for functional convenience
imglib.Image? preview = await matching?.getMarkerQueryDifferenceImage();
if (preview != null) {
  setState(() {
    previewImage = imglib.encodeJpg(preview);
  });
}
```

## ⚠️ Issue

Currently, for ease of use, the libopencv_java4.so binary file is included inside the plugin. I know it's not recommended, but it's there so you can try it out quickly. If you are using the OpenCV module in another plugin, it may cause conflicts. Please note that.

## License
MIT License. See [LICENSE](LICENSE) for details.
