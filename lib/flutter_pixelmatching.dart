import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';
import 'package:camera/camera.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:flutter_pixelmatching/utils/camera_image.dart';
import 'flutter_pixelmatching_bindings.dart' as bindings;

final DynamicLibrary _lib = Platform.isAndroid ? DynamicLibrary.open("libopencv_pixelmatching.so") : DynamicLibrary.process();

class FlutterPixelMatching {
  final binding = bindings.PixelMatchingBindings(_lib);

  void init() {
    binding.init();
  }

  /// Returns the version of the OpenCV library.
  String version() {
    Pointer<Utf8> version = binding.version().cast<Utf8>();
    return version.toDartString();
  }

  Image? grayscale(CameraImage image, int width, int height) {
    var yBuffer = image.planes[0].bytes;
    Uint8List? uBuffer;
    Uint8List? vBuffer;
    if (Platform.isAndroid) {
      uBuffer = image.planes[1].bytes;
      vBuffer = image.planes[2].bytes;
    }
    final ySize = yBuffer.lengthInBytes;
    final uSize = uBuffer?.lengthInBytes ?? 0;
    final vSize = vBuffer?.lengthInBytes ?? 0;
    // image buffer size
    final imgSize = ySize + uSize + vSize;
    final buf = malloc.allocate<Uint8>(imgSize);
    final outBuf = malloc.allocate<Uint8>(imgSize);
    final bufSize = malloc.allocate<Uint32>(1);
    bufSize[0] = imgSize;
    // We always have at least 1 plane, on Android it si the yPlane on iOS its the rgba plane
    if (Platform.isAndroid) {
      buf.asTypedList(bufSize[0]).setRange(0, ySize, yBuffer);
      buf.asTypedList(bufSize[0]).setRange(ySize, ySize + uSize, uBuffer!);
      buf.asTypedList(bufSize[0]).setRange(ySize + uSize, imgSize, vBuffer!);
    } else {
      buf.asTypedList(bufSize[0]).setRange(0, imgSize, yBuffer);
    }

    binding.grayscale(buf, width, height, Platform.isAndroid, outBuf);
    final bytes = outBuf.asTypedList(bufSize[0]);
    final memoryImage = Image.memory(bytes);
    // malloc.free(buf);
    // malloc.free(outBuf);
    // malloc.free(bufSize);
    return memoryImage;
  }
}
