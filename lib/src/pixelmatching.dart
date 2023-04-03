import 'dart:async';
import 'dart:isolate';
import 'dart:typed_data';
import 'package:camera/camera.dart';
import 'package:flutter_pixelmatching/flutter_pixelmatching.dart';

import 'pixelmatching_client.dart' as client;

class PixelMatching {
  bool isReady = false;
  //
  late Isolate _thread;
  late SendPort _client;

  var _id = 0;
  final Map<int, Completer> _completers = {};
  // =================
  // Initialize Completer & Params
  bool _isInitialize = false;
  late Map<String, dynamic> _initParams;
  Completer<bool>? _initCompleter;
  // =================
  PixelMatching() {
    _initThread();
  }

  // *Public*
  Future<bool> initialize(Uint8List bytes, int width, int height, {int rotation = 0}) async {
    if (_isInitialize) return Future.value(true);
    // 초기화 변수 설정
    _initParams = {
      'image': bytes,
      'width': width,
      'height': height,
      'rotation': rotation,
    };
    // 초기화 준비가 되어있다면 초기화 처리를 바로 진행한다.
    if (isReady) {
      return _initialize();
    }
    // 아직 초기화 준비가 되어있지 않다면 초기화 요청을 기다리고, 초기화가 완료되면 초기화 요청을 처리한다.
    else {
      _initCompleter = Completer<bool>();
      return _initCompleter!.future;
    }
  }

  Future<double> query(Uint8List image, int width, int height, {int rotation = 0}) async {
    if (!isReady) return Future.value(0.0);
    final id = ++_id;
    var res = Completer<double>();
    _completers[id] = res;
    _client.send(
      client.Request(
        id: id,
        method: 'query',
        params: {
          'image': image,
          'width': width,
          'height': height,
          'rotation': rotation,
        },
      ),
    );
    return res.future;
  }

  Future<PixelMatchingState> getState() async {
    if (!isReady) return Future.value(PixelMatchingState.notInitialized);
    final id = ++_id;
    var res = Completer<PixelMatchingState>();
    _completers[id] = res;
    _client.send(
      client.Request(
        id: id,
        method: 'getState',
      ),
    );
    return res.future;
  }

  // *Private*
  Future<bool> _initialize() async {
    final id = ++_id;
    var res = Completer<bool>();
    _completers[id] = res;
    var req = client.Request(
      id: id,
      method: 'initialize',
      params: _initParams,
    );
    _client.send(req);
    return res.future;
  }

  void _initThread() async {
    final fromThread = ReceivePort();
    fromThread.listen(_handleMessage, onDone: () {
      isReady = false;
    });

    _thread = await Isolate.spawn(
      client.init,
      fromThread.sendPort,
    );
  }

  void _handleMessage(message) {
    if (message is SendPort) {
      _client = message;
      isReady = true;
      // 초기화 요청이 있다면 초기화 처리 진행
      if (_initCompleter != null) {
        _initialize().then(
          (value) {
            _initCompleter!.complete(value);
            _initCompleter = null;
          },
        );
      }
      return;
    }

    if (message is client.Response) {
      final id = message.id;
      _completers[id]?.complete(message.data);
      _completers.remove(id);
      return;
    }
  }

  void dispose() async {
    if (isReady == false) return;
    final id = ++_id;
    final res = Completer();
    _completers[id] = res;
    final req = client.Request(
      id: id,
      method: 'dispose',
      params: null,
    );
    _client.send(req);
    await res.future;
    _thread.kill();
    isReady = false;
  }
}
