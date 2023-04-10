import 'dart:async';
import 'dart:isolate';
import 'dart:typed_data';
import 'pixelmatching_client.dart' as client;
import 'pixelmatching_state.dart';

class PixelMatching {
  bool isReady = false;
  //
  late Isolate _thread;
  late SendPort _client;

  var _id = 0;
  final _completers = <int, Completer>{};
  final _initCompleter = Completer();

  PixelMatching() {
    _initThread();
  }

  // *Public*
  Future<bool> initialize(Uint8List bytes, int width, int height, {int rotation = 0}) async {
    if (!isReady) {
      await _initCompleter.future;
    }
    final id = ++_id;
    var res = Completer<bool>();
    _completers[id] = res;
    var req = client.Request(
      id: id,
      method: 'initialize',
      params: {
        'image': bytes,
        'width': width,
        'height': height,
        'rotation': rotation,
      },
    );
    _client.send(req);
    return res.future;
  }

  Future<double> setQuery(Uint8List image, int width, int height, {int rotation = 0}) async {
    if (!isReady) {
      await _initCompleter.future;
    }
    final id = ++_id;
    var res = Completer<double>();
    _completers[id] = res;
    _client.send(
      client.Request(
        id: id,
        method: 'setQuery',
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

  Future<PixelMatchingState> getStateCode() async {
    if (!isReady) return Future.value(PixelMatchingState.notInitialized);
    final id = ++_id;
    var res = Completer<PixelMatchingState>();
    _completers[id] = res;
    _client.send(
      client.Request(
        id: id,
        method: 'getStateCode',
      ),
    );
    return res.future;
  }

  Future<Uint8List?> getMarkerQueryDifferenceImage() async {
    if (!isReady) {
      await _initCompleter.future;
    }
    final id = ++_id;
    var res = Completer<Uint8List?>();
    _completers[id] = res;
    _client.send(
      client.Request(
        id: id,
        method: 'getMarkerQueryDifferenceImage',
      ),
    );
    return res.future;
  }

  // *Private*
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
      _initCompleter.complete();
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
