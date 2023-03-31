import 'dart:developer';
import 'dart:io';

import 'package:camera/camera.dart';
import 'package:flutter/material.dart';
import 'package:flutter_pixelmatching/flutter_pixelmatching.dart';
import 'package:image/image.dart' as imglib;

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

  @override
  void initState() {
    super.initState();
    init();
  }

  init() async {
    pixelmatching.init();
    log('pixelmatching : ${pixelmatching.version()}');

    final cameras = await availableCameras();
    controller = CameraController(
      cameras[0],
      ResolutionPreset.medium,
      enableAudio: false,
      imageFormatGroup: Platform.isAndroid ? ImageFormatGroup.yuv420 : ImageFormatGroup.bgra8888,
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
    if (process) return;
    process = true;
    // final h = Platform.isAndroid ? cameraImage.width : cameraImage.height;
    // final w = Platform.isAndroid ? cameraImage.height : cameraImage.width;
    capture = pixelmatching.yuv2rgb(cameraImage);
    if (capture != null) {
      setState(() {});
    }
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
      ),
    );
  }
}
