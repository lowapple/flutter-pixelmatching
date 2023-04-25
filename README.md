# Flutter PixelMatching

| Module to perform feature matching using OpenCV
## Example


## How to install
1. Navigate to the root directory of your project.
2. Clone the Flutter Pixelmatching repository using the following command: `git clone https://github.com/lowapple/flutter_pixelmatching`
3. Change your current directory to the Flutter project directory using the command `cd $flutter_pixelmatching`.
4. Run the initialization script by typing `. ./init.sh` in your terminal.
5. Open the pubspec.yaml file in your Flutter project.
6. Append the following lines to the dependencies section of the pubspec.yaml file:
```yaml
dependencies:
  ...
  flutter_pixelmatching:
    path: ../flutter_pixelmatching
```
The reason we're adding plugins like this is because it's a work in progress and there are a lot of gaps, so we apologize for any inconvenience.

## How to use the module 
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

## And...
If there's anything you'd like us to fix or add, please feel free to email us or request a PR. 
