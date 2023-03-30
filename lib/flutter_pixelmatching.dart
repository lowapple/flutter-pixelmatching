import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
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
}
