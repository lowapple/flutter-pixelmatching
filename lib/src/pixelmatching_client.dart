// ignore_for_file: public_member_api_docs, sort_constructors_first
import 'dart:ffi';
import 'dart:io';
import 'dart:isolate';
import 'dart:typed_data';

import 'package:camera/camera.dart';
import 'package:ffi/ffi.dart';
import 'package:image/image.dart' as imglib;

import 'pixelmatching_bindings.dart' as bindings;
import 'pixelmatching_state.dart';

final DynamicLibrary _lib = Platform.isAndroid ? DynamicLibrary.open("libopencv_pixelmatching.so") : DynamicLibrary.process();

late _PixelMatchingClient _client;

late SendPort _caller;

class Request {
  int id;
  String method;
  dynamic params;

  Request({
    required this.id,
    required this.method,
    this.params,
  });
}

class Response {
  int id;
  dynamic data;

  Response({
    required this.id,
    this.data,
  });
}

void init(
  SendPort caller,
) {
  _client = _PixelMatchingClient();
  _caller = caller;

  // 메인 스레드에 메시지를 보낼 포트를 저장합니다.
  final fromMainThread = ReceivePort();
  // 메인 스레드가 메시지를 보내고 수신할 수 있는 포트를 만듭니다.
  fromMainThread.listen(_handleMessage);
  // 메인 스레드에 메시지를 보낼 수 있는 포트를 보냅니다.
  _caller.send(fromMainThread.sendPort);
}

// 클라이언트로 요청온 데이터들을 처리합니다.
void _handleMessage(message) {
  if (message is Request) {
    // initialize
    switch (message.method) {
      case "initialize":
        _client.initialize();
        var status = _client.getState();
        // initialize
        if (status != PixelMatchingState.waitingForTarget) {
          _caller.send(Response(id: message.id, data: false));
          return;
        }
        // set target image
        final image = message.params['image'] as Uint8List;
        final w = message.params['width'] as int;
        final h = message.params['height'] as int;
        final rotation = message.params['rotation'] as int;
        final res = _client.setTargetImageFromBytes(image, w, h, rotation: rotation);
        _caller.send(Response(id: message.id, data: res));
        break;
      case "getState":
        var status = _client.getState();
        _caller.send(Response(id: message.id, data: status));
        break;
      case "query":
        if (message.params is Map<String, dynamic>) {
          final image = message.params['image'] as Uint8List;
          final width = message.params['width'] as int;
          final height = message.params['height'] as int;
          final rotation = message.params['rotation'] as int;
          final res = _client.query(image, width, height, rotation: rotation);
          _caller.send(Response(id: message.id, data: res));
        }
        break;
      case "dispose":
        _client.dispose();
        _caller.send(Response(id: message.id));
        break;
    }
  }
}

class _PixelMatchingClient {
  final binding = bindings.PixelMatchingBindings(_lib);
  var _lastQueryRes = false;

  void initialize() {
    binding.initialize();
  }

  // Create Image Pointer from Uint8List
  Pointer<Uint8> _createImagePointerFromBytes(Uint8List bytes) {
    final ip = malloc.allocate<Uint8>(bytes.length);
    final ipl = ip.asTypedList(bytes.length);
    ipl.setRange(0, bytes.length, bytes);
    return ip;
  }

  // Create Image Pointer from CameraImage
  // Pointer<Uint8> _createImagePointer(CameraImage cameraImage) {
  //   late Uint8List imgBytes;
  //   if (cameraImage.format.group == ImageFormatGroup.bgra8888) {
  //     var img = imglib.Image.fromBytes(
  //       width: cameraImage.width,
  //       height: cameraImage.height,
  //       bytes: cameraImage.planes[0].bytes.buffer,
  //       order: imglib.ChannelOrder.bgra,
  //     );
  //     imgBytes = imglib.encodeJpg(img);
  //   } else if (cameraImage.format.group == ImageFormatGroup.jpeg) {
  //     // rotate image 90 degree
  //     imgBytes = cameraImage.planes[0].bytes;
  //   } else {
  //     throw Exception("Unsupported image format");
  //   }
  //   return _createImagePointerFromBytes(imgBytes);
  // }

  // Set Marker Image from Uint8List
  bool setTargetImageFromBytes(Uint8List bytes, int w, int h, {int rotation = 0}) {
    final image = _createImagePointerFromBytes(bytes);
    final res = binding.setTargetImage(image.cast(), w, h, rotation);
    return res;
  }

  double query(Uint8List bytes, int w, int h, {int rotation = 0}) {
    final image = _createImagePointerFromBytes(bytes);
    final res = binding.setQueryImage(image.cast(), w, h, rotation);
    _lastQueryRes = res;
    return getQueryConfidenceRate();
  }

  // Set Marker Image from CameraImage
  // bool setTargetImage(CameraImage cameraImage) {
  //   final image = _createImagePointer(cameraImage);
  //   final res = binding.setTargetImage(image.cast(), cameraImage.width, cameraImage.height);
  //   return res;
  // }

  // double query(CameraImage cameraImage) {
  //   final image = _createImagePointer(cameraImage);
  //   final res = binding.setQueryImage(image.cast(), cameraImage.width, cameraImage.height);
  //   _lastQueryRes = res;
  //   return getQueryConfidenceRate();
  // }

  PixelMatchingState getState() {
    final state = binding.getStatusCode();
    if (state == -1) {
      return PixelMatchingState.notInitialized;
    } else if (state == 0) {
      return PixelMatchingState.waitingForTarget;
    } else if (state == 1) {
      return PixelMatchingState.readyToProcess;
    } else if (state == 2) {
      return PixelMatchingState.processing;
    } else {
      throw Exception("Unknown state");
    }
  }

  // 0.87 정도 나오면 일치하는 것으로 판단해도 됨
  double getQueryConfidenceRate() {
    if (_lastQueryRes) {
      return binding.getQueryConfidenceRate();
    } else {
      return 0.0;
    }
  }

  void dispose() {
    binding.dispose();
  }
}
