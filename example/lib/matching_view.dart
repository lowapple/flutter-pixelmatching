import 'package:camera/camera.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
// ignore: depend_on_referenced_packages
import 'package:image/image.dart' as imglib;
import 'package:flutter_pixelmatching/flutter_pixelmatching.dart';

class MatchingView extends StatefulWidget {
  final Uint8List target;

  const MatchingView({
    super.key,
    required this.target,
  });

  @override
  State<MatchingView> createState() => MatchingViewState();
}

class MatchingViewState extends State<MatchingView> {
  CameraController? controller;
  PixelMatching? matching;

  late Uint8List image = widget.target;

  /// PixelMatching processing
  bool _processing = false;

  /// PixelMatching last processed timestamp
  int _processTimestamp = 0;

  double _similarity = 0;

  @override
  void initState() {
    WidgetsBinding.instance.addPostFrameCallback((timeStamp) async {
      // resize target image
      // Larger sizes don't significantly impact recognition performance
      final imglib.Image? img = imglib.decodeImage(image);
      final imglib.Image resizedImg = imglib.copyResize(img!, width: 720);
      final imgBytes = imglib.encodeJpg(resizedImg);

      // initialize PixelMatching and Camera
      image = imgBytes;
      initializePixelMatching();
      initializeCamera();
    });
    super.initState();
  }

  @override
  dispose() {
    matching?.dispose();
    matching = null;
    controller?.stopImageStream();
    controller?.dispose();
    controller = null;
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("PixelMatching Sample"),
      ),
      body: controller != null &&
              controller!.value.isInitialized &&
              matching != null &&
              matching!.isInitialized
          ? Center(
              child: Column(
                children: [
                  Container(
                    color: Colors.black,
                    child: Row(
                      children: [
                        Expanded(
                          child: Image.memory(
                            image,
                          ),
                        ),
                        Expanded(
                          child: CameraPreview(
                            controller!,
                          ),
                        ),
                      ],
                    ),
                  ),
                  Row(
                    children: [
                      Expanded(
                        child: Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          mainAxisSize: MainAxisSize.min,
                          children: [
                            IconButton.outlined(
                                onPressed: () => imageRotation(-90),
                                icon: const Icon(
                                    Icons.rotate_90_degrees_ccw_outlined)),
                            IconButton.outlined(
                                onPressed: () => imageRotation(90),
                                icon: const Icon(
                                    Icons.rotate_90_degrees_cw_outlined)),
                          ],
                        ),
                      ),
                      Expanded(
                        child: Center(
                          child: Text(
                            "${_similarity.toStringAsFixed(2)} Similar",
                          ),
                        ),
                      )
                    ],
                  ),
                ],
              ),
            )
          : const Center(
              child: CircularProgressIndicator(),
            ),
    );
  }

  /// Setup PixelMatching
  initializePixelMatching() async {
    matching = PixelMatching();
    await matching?.initialize(image: image);
    setState(() {});
  }

  /// Setup Camera
  ///
  initializeCamera() async {
    final cameras = await availableCameras();
    final camera = cameras[0];
    controller =
        CameraController(camera, ResolutionPreset.high, enableAudio: false);
    await controller?.initialize();
    controller?.startImageStream(cameraStream);
    setState(() {});
  }

  /// PixelMatching similarity check
  ///
  /// 1. Check if PixelMatching is processing
  ///    - If processing, skip
  /// 2. Check if PixelMatching is delayed
  ///    - If not delayed, skip
  /// 3. Process PixelMatching
  cameraStream(CameraImage cameraImage) async {
    final bool isProcessed = _processing;
    final bool isDelayed =
        DateTime.now().millisecondsSinceEpoch - _processTimestamp > 50;
    if (isProcessed || !isDelayed) return;
    _processing = true;
    _similarity = await matching?.similarity(cameraImage) ?? 0.0;
    if (mounted) {
      setState(() {});
    }
    if (kDebugMode) {
      print(_similarity);
    }
    _processing = false;
    _processTimestamp = DateTime.now().millisecondsSinceEpoch;
  }

  /// image rotation
  imageRotation(int angle) async {
    final imglib.Image? img = imglib.decodeImage(image);
    final imglib.Image rotatedImg = imglib.copyRotate(img!, angle: angle);
    final imgBytes = imglib.encodeJpg(rotatedImg);
    image = imgBytes;
    initializePixelMatching();
  }
}
