import 'dart:io';
import 'dart:typed_data';

import 'package:camera/camera.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';

// ignore: depend_on_referenced_packages
import 'package:image/image.dart' as imglib;
import 'package:flutter_pixelmatching/flutter_pixelmatching.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  CameraController? controller;
  PixelMatching? matching;
  bool process = false;

  // capturing images
  Uint8List? previewImage;
  CameraImage? cameraImage;
  bool isTarget = false;

  bool _isRunning = false;
  int _lastRun = 0;
  int rotation = 0;

  @override
  void initState() {
    matching = PixelMatching();
    init();
    super.initState();
  }

  @override
  dispose() {
    matching?.dispose();
    matching = null;
    super.dispose();
  }

  init() async {
    final cameras = await availableCameras();
    final camera = cameras[0];
    rotation = Platform.isAndroid ? camera.sensorOrientation : 0;
    controller = CameraController(
      camera,
      ResolutionPreset.medium,
      enableAudio: false,
      imageFormatGroup: Platform.isAndroid ? ImageFormatGroup.jpeg : ImageFormatGroup.bgra8888,
    );
    controller!.initialize().then(
      (_) {
        if (!mounted) {
          return;
        }
        controller!.startImageStream((CameraImage cameraImage) {
          cameraStream(cameraImage);
        });
        setState(() {});
      },
    );
  }

  cameraStream(CameraImage cameraImage) async {
    this.cameraImage = cameraImage;
    if (_isRunning || !mounted || DateTime.now().millisecondsSinceEpoch - _lastRun < 50) return;
    final status = await matching?.getStateCode();
    print(status);
    if (status == PixelMatchingState.noQuery) {
      _isRunning = true;
      final img = cameraImage.planes[0].bytes;
      final w = cameraImage.width;
      final h = cameraImage.height;
      final confidence = await matching?.setQuery(img, w, h, rotation: rotation);
      print(confidence);
      final preview = await matching?.getMarkerQueryDifferenceImage();
      if (preview != null) {
        setState(() {
          previewImage = preview;
        });
      }
      _isRunning = false;
      _lastRun = DateTime.now().millisecondsSinceEpoch;
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: WillPopScope(
        onWillPop: () async {
          controller?.stopImageStream();
          controller?.dispose();
          controller = null;

          return true;
        },
        child: Scaffold(
          appBar: AppBar(
            title: const Text('OpenCV PixelMatching Plugin Sample'),
          ),
          body: Column(children: [
            controller != null ? CameraPreview(controller!) : Container(),
            if (previewImage != null)
              Image.memory(
                previewImage!,
                gaplessPlayback: true,
              )
          ]),
          floatingActionButton: IconButton(
            onPressed: () async {
              final state = await matching?.getStateCode();
              // image bytes
              final img = cameraImage!.planes[0].bytes;
              final w = cameraImage!.width;
              final h = cameraImage!.height;
              if (state == PixelMatchingState.notInitialized) {
                await matching?.initialize(img, w, h, rotation: rotation);
              }
              if (kDebugMode) {
                print(state);
              }

              // if (state == PixelMatchingState.noMarker) {
              //   Uint8List bytes;
              //   if (Platform.isAndroid) {
              //     bytes = cameraImage!.planes[0].bytes;
              //   } else {
              //     var img = imglib.Image.fromBytes(
              //       width: cameraImage!.width,
              //       height: cameraImage!.height,
              //       bytes: cameraImage!.planes[0].bytes.buffer,
              //       order: imglib.ChannelOrder.bgra,
              //     );
              //     bytes = imglib.encodeJpg(img);
              //   }
              //   setState(() {
              //     isTarget = true;
              //   });
              //   return;
              // }
            },
            icon: const Icon(
              Icons.camera,
            ),
          ),
        ),
      ),
    );
  }
}
