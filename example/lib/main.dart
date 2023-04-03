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
  FlutterPixelMatching pixelmatching = FlutterPixelMatching();
  bool process = false;
  // capturing images
  imglib.Image? capture;
  CameraImage? cameraImage;
  bool isTarget = false;

  @override
  void initState() {
    super.initState();
    init();
  }

  init() async {
    pixelmatching.initialize();

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

  cameraStream(CameraImage cameraImage) {
    this.cameraImage = cameraImage;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('OpenCV PixelMatching Plugin Sample'),
        ),
        body: capture == null
            ? Column(
                children: [
                  controller != null ? CameraPreview(controller!) : Container(),
                ],
              )
            : Image.memory(imglib.encodeJpg(capture!)),
        floatingActionButton: IconButton(
          onPressed: () {
            if (process) return;
            process = true;
            var state = pixelmatching.getState();
            if (state == PixelMatchingState.waitingForTarget) {
              pixelmatching.setTargetImage(cameraImage!);
            }
            if (state == PixelMatchingState.readyToProcess) {
              pixelmatching.setQueryImage(cameraImage!);
              final confidence = pixelmatching.getQueryConfidenceRate();
              log('[pixelmatching] result : $confidence');
            }
            process = false;
          },
          icon: const Icon(
            Icons.camera,
          ),
        ),
      ),
    );
  }
}
