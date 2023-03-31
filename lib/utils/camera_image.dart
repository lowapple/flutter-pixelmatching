import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';

import 'package:camera/camera.dart';
import 'package:ffi/ffi.dart';

extension CameraImageExtension on CameraImage {
  Pointer<Uint8>? imgBufferTo() {
    var yBuffer = planes[0].bytes;
    Uint8List? uBuffer;
    Uint8List? vBuffer;
    if (Platform.isAndroid) {
      uBuffer = planes[1].bytes;
      vBuffer = planes[2].bytes;
    }
    var ySize = yBuffer.lengthInBytes;
    var uSize = uBuffer?.lengthInBytes ?? 0;
    var vSize = vBuffer?.lengthInBytes ?? 0;
    var totalSize = ySize + uSize + vSize;

    final imageBuffer = malloc.allocate<Uint8>(totalSize);

    // We always have at least 1 plane, on Android it si the yPlane on iOS its the rgba plane
    Uint8List imgBytes = imageBuffer.asTypedList(totalSize);
    imgBytes.setAll(0, yBuffer);
    if (Platform.isAndroid) {
      // Swap u&v buffer for opencv
      imgBytes.setAll(ySize, vBuffer!);
      imgBytes.setAll(ySize + vSize, uBuffer!);
    }
    return imageBuffer;
  }
}
