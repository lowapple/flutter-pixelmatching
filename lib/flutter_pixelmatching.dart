import 'dart:ffi';
import 'dart:io';
import 'package:camera/camera.dart';
import 'package:ffi/ffi.dart';
import 'flutter_pixelmatching_bindings.dart' as bindings;
import 'package:image/image.dart' as imglib;

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

  imglib.Image? yuv2rgb(CameraImage image) {
    // Allocate memory for the 3 planes of the image
    final p1 = malloc.allocate<Uint8>(image.planes[0].bytes.length);
    final p2 = malloc.allocate<Uint8>(image.planes[1].bytes.length);
    final p3 = malloc.allocate<Uint8>(image.planes[2].bytes.length);

    final p1l = p1.asTypedList(image.planes[0].bytes.length);
    final p2l = p2.asTypedList(image.planes[1].bytes.length);
    final p3l = p3.asTypedList(image.planes[2].bytes.length);

    p1l.setRange(0, image.planes[0].bytes.length, image.planes[0].bytes);
    p2l.setRange(0, image.planes[1].bytes.length, image.planes[1].bytes);
    p3l.setRange(0, image.planes[2].bytes.length, image.planes[2].bytes);

    Pointer<Uint32> imgPointer = binding
        .yuv2rgb(
          p1.cast(),
          p2.cast(),
          p3.cast(),
          image.planes[1].bytesPerRow,
          image.planes[1].bytesPerPixel!,
          image.planes[0].bytesPerRow,
          image.height,
        )
        .cast();

    final imgBytes = imgPointer.asTypedList((image.planes[0].bytesPerRow * image.height));
    final img = imglib.Image.fromBytes(width: image.height, height: image.planes[0].bytesPerRow, bytes: imgBytes.buffer);

    malloc.free(p1);
    malloc.free(p2);
    malloc.free(p3);
    malloc.free(imgPointer);

    return img;
  }
}
