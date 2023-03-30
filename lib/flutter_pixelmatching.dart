import 'dart:ffi';
import 'dart:io';
import 'package:ffi/ffi.dart';
import 'flutter_pixelmatching_bindings.dart' as bindings;

final DynamicLibrary _lib = Platform.isAndroid ? DynamicLibrary.open("libopencv_pixelmatching.so") : DynamicLibrary.process();

class FlutterPixelMatching {
  final binding = bindings.PixelMatchingBindings(_lib);
  String version() {
    Pointer<Utf8> version = binding.version();
    return version.toDartString();
  }
}
