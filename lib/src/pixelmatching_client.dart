import 'dart:ffi';
import 'dart:io';
import 'dart:isolate';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

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
        var status = _client.getStateCode();
        // initialize
        if (status != PixelMatchingState.noMarker) {
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
      case "getStateCode":
        var status = _client.getStateCode();
        _caller.send(Response(id: message.id, data: status));
        break;
      case "setQuery":
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
  final _native = bindings.PixelMatchingBindings(_lib);
  var _lastQueryRes = false;
  // Image Buffer
  Pointer<Uint8>? _imageBuffer;

  void initialize() {
    _native.initialize();
  }

  bool setTargetImageFromBytes(Uint8List bytes, int w, int h, {int rotation = 0}) {
    var totalSize = bytes.length;
    var imgBuffer = malloc.allocate<Uint8>(totalSize);
    var imgBufferList = imgBuffer.asTypedList(totalSize);
    imgBufferList.setAll(0, bytes);
    var res = _native.setMarker(imgBuffer.cast(), w, h, rotation);
    malloc.free(imgBuffer);
    return res;
  }

  double query(Uint8List bytes, int w, int h, {int rotation = 0}) {
    _imageBuffer ??= malloc.allocate<Uint8>(bytes.length);
    // 이미지 내용 복사
    var imageBufferList = _imageBuffer!.asTypedList(bytes.length);
    imageBufferList.setAll(0, bytes);
    var res = _native.setQuery(_imageBuffer!.cast(), w, h, rotation);
    _lastQueryRes = res;
    return _confidence();
  }

  PixelMatchingState getStateCode() {
    final state = _native.getStateCode();
    if (state == -1) {
      return PixelMatchingState.notInitialized;
    } else if (state == 0) {
      return PixelMatchingState.noMarker;
    } else if (state == 1) {
      return PixelMatchingState.noQuery;
    } else if (state == 2) {
      return PixelMatchingState.processing;
    } else {
      throw Exception("Unknown state");
    }
  }

  // 0.87 정도 나오면 일치하는 것으로 판단해도 됨
  double _confidence() {
    if (_lastQueryRes) {
      return _native.getQueryConfidenceRate();
    } else {
      return 0.0;
    }
  }

  void dispose() {
    if (_imageBuffer != null) {
      malloc.free(_imageBuffer!);
    }
    _imageBuffer = null;
    _native.dispose();
  }
}
