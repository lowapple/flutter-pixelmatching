import 'dart:developer';
import 'dart:ffi';
import 'dart:io';
import 'package:camera/camera.dart';
import 'package:ffi/ffi.dart';
import 'flutter_pixelmatching_bindings.dart' as bindings;
import 'package:image/image.dart' as imglib;

final DynamicLibrary _lib = Platform.isAndroid ? DynamicLibrary.open("libopencv_pixelmatching.so") : DynamicLibrary.process();

class FlutterPixelMatching {
  final binding = bindings.PixelMatchingBindings(_lib);
  var _lastQueryRes = false;

  void initialize() {
    binding.initialize();
  }

  Pointer<Uint8> _createImagePointer(CameraImage cameraImage) {
    final ip = malloc.allocate<Uint8>(cameraImage.planes[0].bytes.length);
    final ipl = ip.asTypedList(cameraImage.planes[0].bytes.length);
    ipl.setRange(0, cameraImage.planes[0].bytes.length, cameraImage.planes[0].bytes);

    return ip;
  }

  bool setTargetImage(CameraImage cameraImage) {
    final image = _createImagePointer(cameraImage);
    final res = binding.setTargetImage(image.cast(), cameraImage.width, cameraImage.height);
    return res;
  }

  bool setQueryImage(CameraImage cameraImage) {
    final image = _createImagePointer(cameraImage);
    final res = binding.setQueryImage(image.cast(), cameraImage.width, cameraImage.height);
    _lastQueryRes = res;
    return res;
  }

  // 0.87 정도 나오면 일치하는 것으로 판단해도 됨
  double getQueryConfidenceRate() {
    if (_lastQueryRes) {
      return binding.getQueryConfidenceRate();
    } else {
      return 0.0;
    }
  }

  void dispose() => binding.dispose();
}
