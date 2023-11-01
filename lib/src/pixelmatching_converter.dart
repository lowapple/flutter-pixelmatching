import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';
import 'package:camera/camera.dart';
import 'package:ffi/ffi.dart';
import 'package:image/image.dart' as imglib;
import 'package:image/image.dart';
import 'pixelmatching_bindings.dart' as bindings;

Uint8List? cameraImageToQuery(
    bindings.PixelMatchingBindings native, CameraImage cameraImage) {
  if (!(Platform.isAndroid || Platform.isIOS)) {
    throw Exception("Not supported on this platform");
  }
  switch (cameraImage.format.group) {
    case ImageFormatGroup.yuv420:
      Pointer<Uint8> p0 = malloc.allocate(cameraImage.planes[0].bytes.length);
      Pointer<Uint8> p1 = malloc.allocate(cameraImage.planes[1].bytes.length);
      Pointer<Uint8> p2 = malloc.allocate(cameraImage.planes[2].bytes.length);
      // Assign the planes data to the pointers of the image
      Uint8List pointerList0 =
          p0.asTypedList(cameraImage.planes[0].bytes.length);
      Uint8List pointerList1 =
          p1.asTypedList(cameraImage.planes[1].bytes.length);
      Uint8List pointerList2 =
          p2.asTypedList(cameraImage.planes[2].bytes.length);
      pointerList0.setRange(
          0, cameraImage.planes[0].bytes.length, cameraImage.planes[0].bytes);
      pointerList1.setRange(
          0, cameraImage.planes[1].bytes.length, cameraImage.planes[1].bytes);
      pointerList2.setRange(
          0, cameraImage.planes[2].bytes.length, cameraImage.planes[2].bytes);
      Pointer<Uint8> imagePointer = native.YuvToRGB(
        p0,
        p1,
        p2,
        cameraImage.planes[1].bytesPerRow,
        cameraImage.planes[1].bytesPerPixel!,
        cameraImage.width,
        cameraImage.height,
      );
      final imageBuffer = imagePointer
          .asTypedList((cameraImage.width * cameraImage.height * 3))
          .buffer;
      final image = imglib.Image.fromBytes(
        width: cameraImage.width,
        height: cameraImage.height,
        bytes: imageBuffer,
        order: ChannelOrder.rgb,
        numChannels: 3,
      );
      malloc.free(p0);
      malloc.free(p1);
      malloc.free(p2);
      malloc.free(imagePointer);
      return _toJpeg(image);
    case ImageFormatGroup.bgra8888:
      if (Platform.isIOS) {
        final image = imglib.Image.fromBytes(
          width: cameraImage.width,
          height: cameraImage.height,
          bytes: cameraImage.planes[0].bytes.buffer,
          order: ChannelOrder.bgra,
        );
        return _toJpeg(image);
      }
      throw Exception(
          "Not supported ImageFormatGroup.bgra8888 on this platform");
    case ImageFormatGroup.jpeg:
      return cameraImage.planes[0].bytes;
    case ImageFormatGroup.nv21:
      throw Exception("Not supported ImageFormatGroup.nv21 on this platform");
    case ImageFormatGroup.unknown:
      throw Exception(
          "Not supported ImageFormatGroup.unknown on this platform");
  }
}

Uint8List _toJpeg(imglib.Image image) {
  return imglib.encodeJpg(image);
}
