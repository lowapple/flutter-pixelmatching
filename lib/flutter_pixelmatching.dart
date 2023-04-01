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

  // convert CameraImage to extract feature and encoded to jpeg
  imglib.Image? extractFeaturesAndEncodeToJpeg(CameraImage image) {
    // get rotation
    final rotation = image.planes[0].bytesPerRow / image.planes[0].bytesPerPixel!;
    // input pointer
    final ip = malloc.allocate<Uint8>(image.planes[0].bytes.length);
    final ipl = ip.asTypedList(image.planes[0].bytes.length);
    ipl.setRange(0, image.planes[0].bytes.length, image.planes[0].bytes);
    // output pointer
    Pointer<Uint8> op = binding.extractFeaturesAndEncodeToJpeg(ip.cast(), image.width, image.height).cast();
    // convert to imglib.Image
    final imgBytes = op.asTypedList(image.width * image.height);
    final img = imglib.decodeJpg(imgBytes);
    return img;
  }

  // final imglibImage = imglib.Image.
  // imglib.Image? grayscale(CameraImage image) {
  //   // input pointer
  //   final ip = malloc.allocate<Uint8>(image.planes[0].bytes.length);
  //   final ipl = ip.asTypedList(image.planes[0].bytes.length);
  //   ipl.setRange(0, image.planes[0].bytes.length, image.planes[0].bytes);
  //   // output pointer
  //   final op = malloc.allocate<Uint8>(image.width * image.height);
  //   binding.grayscale(ip.cast(), image.width, image.height, op.cast());
  //   // convert to imglib.Image
  //   final imgBytes = op.asTypedList(image.width * image.height);
  //   final img = imglib.decodeJpg(imgBytes);
  //   // final imglibImage = imglib.Image.fromBytes(width: image.width, height: image.height, bytes: imgBytes.buffer);

  //   // final imglibImage = imglib.Image.fromBytes(width: image.width, height: image.height, bytes: imgBytes.buffer);
  //   // imglib.Image.from
  //   malloc.free(ip);
  //   malloc.free(op);
  //   return img;
  // }

  // imglib.Image? yuv2rgb(CameraImage image) {
  //   // Allocate memory for the 3 planes of the image
  //   final p1 = malloc.allocate<Uint8>(image.planes[0].bytes.length);
  //   final p2 = malloc.allocate<Uint8>(image.planes[1].bytes.length);
  //   final p3 = malloc.allocate<Uint8>(image.planes[2].bytes.length);

  //   final p1l = p1.asTypedList(image.planes[0].bytes.length);
  //   final p2l = p2.asTypedList(image.planes[1].bytes.length);
  //   final p3l = p3.asTypedList(image.planes[2].bytes.length);

  //   p1l.setRange(0, image.planes[0].bytes.length, image.planes[0].bytes);
  //   p2l.setRange(0, image.planes[1].bytes.length, image.planes[1].bytes);
  //   p3l.setRange(0, image.planes[2].bytes.length, image.planes[2].bytes);

  //   Pointer<Uint8> imgPointer = binding.yuv2rgb(
  //     p1,
  //     p2,
  //     p3,
  //     image.planes[1].bytesPerRow,
  //     image.planes[1].bytesPerPixel!,
  //     image.width,
  //     image.height,
  //   );

  //   final imgBytes = imgPointer.asTypedList((image.width * image.height));

  //   final img = imglib.decodeJpg(imgBytes);
  //   // final img = imglib.Image.fromBytes(
  //   //   width: image.width,
  //   //   height: image.height,
  //   //   bytes: imgBytes.buffer,
  //   //   order: imglib.ChannelOrder.bgr,
  //   // );
  //   // img data printing
  //   // print('img data: ${img.data}');
  //   // print('img w/h : ${img.width} / ${img.height}');
  //   // print('img bytes: ${img.getBytes()}');
  //   // print('img format : ${img.format}');

  //   malloc.free(p1);
  //   malloc.free(p2);
  //   malloc.free(p3);
  //   malloc.free(imgPointer);

  //   return img;
  // }
}
