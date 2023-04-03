import 'dart:developer';
import 'dart:io';

import 'package:camera/camera.dart';
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
  imglib.Image? capture;
  CameraImage? cameraImage;
  bool isTarget = false;

  bool _isRunning = false;
  int _lastRun = 0;

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
    controller = CameraController(
      cameras[0],
      ResolutionPreset.medium,
      enableAudio: false,
      imageFormatGroup: Platform.isAndroid ? ImageFormatGroup.jpeg : ImageFormatGroup.bgra8888,
    );
    controller!.initialize().then((_) {
      if (!mounted) {
        return;
      }
      controller!.startImageStream((CameraImage cameraImage) {
        cameraStream(cameraImage);
      });
      setState(() {});
    });
  }

  cameraStream(CameraImage cameraImage) async {
    this.cameraImage = cameraImage;
    if (_lastRun == 0) {
      _lastRun = DateTime.now().millisecondsSinceEpoch;
    }

    if (_isRunning || !mounted || DateTime.now().millisecondsSinceEpoch - _lastRun < 30) return;
    final status = await matching?.getState();
    if (status == PixelMatchingState.readyToProcess) {
      _isRunning = true;
      var res = await matching?.query(cameraImage);
      print(res);
      _isRunning = false;
    }
    _lastRun = DateTime.now().millisecondsSinceEpoch;
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
          body: controller != null ? CameraPreview(controller!) : Container(),
          floatingActionButton: IconButton(
            onPressed: () async {
              final state = await matching?.getState();
              if (state == PixelMatchingState.notInitialized) {
                await matching?.initialize();
              }
              if (state == PixelMatchingState.waitingForTarget) {
                matching?.setTargetImage(cameraImage!).then(
                  (value) {
                    setState(
                      () {
                        isTarget = true;
                      },
                    );
                  },
                );
                return;
              }
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
