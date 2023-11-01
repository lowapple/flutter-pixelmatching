import 'dart:async';
import 'dart:isolate';
import 'dart:typed_data';
import 'package:camera/camera.dart';
import 'package:image/image.dart' as imglib;
import 'pixelmatching_client.dart' as client;
import 'pixelmatching_state.dart';

class PixelMatching {
  /// Whether the isolate is ready to receive messages.
  bool _clientReady = false;

  /// The isolate instance.
  Isolate? _clientIsolate;

  /// The isolate send port.
  SendPort? _clientSender;

  /// The id of the next request.
  var _id = 0;

  /// The map of pending requests.
  final Map<int, Completer> _completers = <int, Completer>{};

  /// The completer for the initialization.
  Completer? _clientInitCompleter;

  bool get isInitialized => _clientReady;

  /// Initialize the isolate.
  Future<void> _initialize() async {
    if (_clientReady) return;
    final ReceivePort receivePort = ReceivePort();
    receivePort.listen(_clientMessageHandler, onDone: () {
      _clientReady = false;
    });
    _clientInitCompleter = Completer();
    _clientIsolate = await Isolate.spawn(
      client.init,
      receivePort.sendPort,
    );
    await _clientInitCompleter?.future;
  }

  /// Handle messages from the isolate.
  ///
  /// This function is called whenever the isolate sends a message to the main
  void _clientMessageHandler(message) {
    // initialize the sender port
    if (message is SendPort) {
      _clientSender = message;
      _clientReady = true;
      _clientInitCompleter?.complete();
      return;
    }
    // handle responses
    if (message is client.Response) {
      final id = message.id;
      _completers[id]?.complete(message.data);
      _completers.remove(id);
      return;
    }
  }

  // Future<double> similarity(
  //   dynamic image1,
  //   dynamic image2,
  // ) async {
  //   assert(image1 != null, 'image1 is null');
  //   assert(image2 != null, 'image2 is null');
  //   if (!(image1 is CameraImage || image1 is imglib.Image)) {
  //     throw Exception('image1 is not CameraImage or Image');
  //   }
  //   if (!(image2 is CameraImage || image2 is imglib.Image)) {
  //     throw Exception('image2 is not CameraImage or Image');
  //   }
  //   await _initialize();
  //   final id = ++_id;
  //   final res = Completer<double>();
  //   _completers[id] = res;
  //   final client.Request body = client.Request(
  //     id: id,
  //     method: 'similarity',
  //     params: {
  //       'image1': image1,
  //       'image2': image2,
  //     },
  //   );
  //   _clientSender?.send(body);
  //   return res.future;
  // }

  /// Initialize PixelMatching
  ///
  /// Methods used to initialize the PixelMatching library
  /// Passes the image comparison target as an initialization value
  /// and must be called again to change the image comparison target
  ///
  /// [image] is a CameraImage or Image or Uint8List
  ///
  Future<bool> initialize({
    required dynamic image,
  }) async {
    assert(image != null, 'image is null');
    assert(image is CameraImage || image is imglib.Image || image is Uint8List,
        'image is not CameraImage or Image or Uint8List');
    await _initialize();
    Uint8List? img;
    late int imgW;
    late int imgH;
    if (image is CameraImage) {
      img = await _cameraImageToQuery(image);
      imgW = image.width;
      imgH = image.height;
      assert(img == null,
          "image is not ImageFormatGroup.jpeg or ImageFormatGroup.bgra8888 or ImageFormatGroup.yuv420");
    } else if (image is imglib.Image) {
      img = imglib.encodeJpg(image);
      imgW = image.width;
      imgH = image.height;
    } else if (image is Uint8List) {
      final imglib.Image? tmpImg = imglib.decodeImage(image);
      if (tmpImg != null) {
        img = image;
        imgW = tmpImg.width;
        imgH = tmpImg.height;
      } else {
        assert(false, "image is not a valid jpeg, png");
      }
    } else {
      throw Exception('image is not CameraImage or Image or Uint8List');
    }

    final id = ++_id;
    var res = Completer<bool>();
    _completers[id] = res;
    var req = client.Request(
      id: id,
      method: 'initialize',
      params: {
        'imageType': "jpeg",
        'image': img,
        'width': imgW,
        'height': imgH,
        'rotation': 0,
      },
    );
    _clientSender?.send(req);
    return res.future;
  }

  Future<double> similarity(dynamic query) async {
    assert(query != null, 'image is null');
    assert(query is CameraImage || query is imglib.Image || query is Uint8List,
        'image is not CameraImage or Image or Uint8List');
    if (!_clientReady) {
      if (_clientInitCompleter == null) {
        assert(false, "PixelMatching is not initialized");
      } else {
        await _clientInitCompleter?.future;
      }
    }
    Uint8List? img;
    late int imgW;
    late int imgH;
    if (query is CameraImage) {
      img = await _cameraImageToQuery(query);
      imgW = query.width;
      imgH = query.height;
      assert(img != null,
          "image is not ImageFormatGroup.jpeg or ImageFormatGroup.bgra8888 or ImageFormatGroup.yuv420");
    } else if (query is imglib.Image) {
      img = imglib.encodeJpg(query);
      imgW = query.width;
      imgH = query.height;
    } else if (query is Uint8List) {
      final imglib.Image? tmpImg = imglib.decodeImage(query);
      if (tmpImg != null) {
        img = query;
        imgW = tmpImg.width;
        imgH = tmpImg.height;
      } else {
        assert(false, "image is not a valid jpeg, png");
      }
    } else {
      throw Exception('image is not CameraImage or Image or Uint8List');
    }

    final id = ++_id;
    var res = Completer<double>();
    _completers[id] = res;
    _clientSender?.send(
      client.Request(
        id: id,
        method: 'setQuery',
        params: {
          'imageType': 'jpeg',
          'image': img,
          'width': imgW,
          'height': imgH,
          'rotation': 0,
        },
      ),
    );
    return res.future;
  }

  @Deprecated("""Use similarity instead
  Using the [PixelMatching.similarity] method
  Same functionality, but with different parameter values
  """)
  Future<double> setQuery(
      String imageType, Uint8List image, int width, int height,
      {int rotation = 0}) async {
    if (!_clientReady) {
      await _clientInitCompleter?.future;
    }
    final id = ++_id;
    var res = Completer<double>();
    _completers[id] = res;
    _clientSender?.send(
      client.Request(
        id: id,
        method: 'setQuery',
        params: {
          'imageType': imageType,
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
    if (!_clientReady) return Future.value(PixelMatchingState.notInitialized);
    final id = ++_id;
    var res = Completer<PixelMatchingState>();
    _completers[id] = res;
    _clientSender?.send(
      client.Request(
        id: id,
        method: 'getStateCode',
      ),
    );
    return res.future;
  }

  Future<imglib.Image?> getMarkerQueryDifferenceImage() async {
    if (!_clientReady) {
      await _clientInitCompleter?.future;
    }
    final id = ++_id;
    var res = Completer<imglib.Image?>();
    _completers[id] = res;
    _clientSender?.send(
      client.Request(
        id: id,
        method: 'getMarkerQueryDifferenceImage',
      ),
    );
    return res.future;
  }

  Future<Uint8List?> _cameraImageToQuery(CameraImage cameraImage) async {
    if (!_clientReady) {
      await _clientInitCompleter?.future;
    }
    final id = ++_id;
    var res = Completer<Uint8List?>();
    _completers[id] = res;
    _clientSender?.send(
      client.Request(
        id: id,
        method: 'cameraImageToQuery',
        params: cameraImage,
      ),
    );
    return res.future;
  }

  void dispose() async {
    if (_clientReady == false) return;
    final id = ++_id;
    final res = Completer();
    _completers[id] = res;
    final req = client.Request(
      id: id,
      method: 'dispose',
      params: null,
    );
    _clientSender?.send(req);
    await res.future;
    _clientInitCompleter = null;
    _clientIsolate?.kill();
    _clientIsolate = null;
    _clientReady = false;
  }
}
