import 'dart:async';
import 'dart:isolate';
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
  //
  bool _initialize = false;

  bool get isInitialize => _initialize;

  PixelMatching() {
    _initThread();
  }

  // *Public*
  Future<bool> initialize() async {
    if (_initialize) {
      return Future.value(true);
    }
    if (!isReady) return Future.value(false);

    final id = ++_id;
    var res = Completer<bool>();
    _completers[id] = res;
    var req = client.Request(
      id: id,
      method: 'initialize',
    );
    _client.send(req);
    _initialize = await res.future;
    return _initialize;
  }

  Future<bool> setTargetImage(CameraImage image) async {
    if (!isReady) return Future.value(false);
    return _createJob(
      client.Request(
        id: ++_id,
        method: 'setTargetImage',
        params: {
          'image': image,
          'w': image.width,
          'h': image.height,
        },
      ),
    );
  }

  Future<double> query(CameraImage image) async {
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
          'w': image.width,
          'h': image.height,
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
  Future<bool> _createJob(client.Request req) {
    final id = ++_id;
    var res = Completer<bool>();
    _completers[id] = res;
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
      initialize();
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
