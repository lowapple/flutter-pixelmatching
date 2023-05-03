import 'dart:io';
import 'dart:typed_data';

import 'package:camera/camera.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';

// ignore: depend_on_referenced_packages
import 'package:image/image.dart' as imglib;
import 'package:flutter_pixelmatching/flutter_pixelmatching.dart';
import 'package:image_picker/image_picker.dart';

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
    if (_isRunning || !mounted || DateTime.now().millisecondsSinceEpoch - _lastRun < 50) return;
    this.cameraImage = cameraImage;
    final status = await matching?.getStateCode();
    print(status);
    print(cameraImage.toString());
    if (status == PixelMatchingState.noQuery) {
      _isRunning = true;
      final imgType = Platform.isAndroid ? "jpeg" : "bgra8888";
      final img = cameraImage.planes[0].bytes;
      final w = cameraImage.width;
      final h = cameraImage.height;
      final confidence = await matching?.setQuery(imgType, img, w, h, rotation: rotation);
      print(confidence);
      final preview = await matching?.getMarkerQueryDifferenceImage();
      if (preview != null) {
        setState(() {
          previewImage = imglib.encodeJpg(preview);
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
            title: const Text('PixelMatching Sample'),
          ),
          body: controller == null
              ? Column(
                  crossAxisAlignment: CrossAxisAlignment.center,
                  mainAxisAlignment: MainAxisAlignment.center,
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    TextButton(
                      onPressed: () async {
                        final imagePicker = ImagePicker();
                        final xImage = await imagePicker.pickImage(source: ImageSource.gallery);
                        if (xImage != null) {
                          final image = await imglib.decodeJpgFile(xImage.path);
                          matching?.initialize("jpeg", imglib.encodeJpg(image!), image.width, image.height).then((value) {
                            init();
                          });
                        }
                      },
                      child: const Text(
                        "From Gallery",
                        style: TextStyle(fontSize: 18.0),
                      ),
                    ),
                    TextButton(
                      onPressed: () async {
                        final imagePicker = ImagePicker();
                        final xImage = await imagePicker.pickImage(source: ImageSource.camera);
                        if (xImage != null) {
                          final image = await imglib.decodeJpgFile(xImage.path);
                          matching?.initialize("jpeg", imglib.encodeJpg(image!), image.width, image.height).then((value) {
                            if (value) {
                              init();
                            }
                          });
                        }
                      },
                      child: const Text(
                        "From Camera",
                        style: TextStyle(fontSize: 18.0),
                      ),
                    ),
                  ],
                )
              : Column(
                  children: [
                    CameraPreview(controller!),
                    if (previewImage != null)
                      Image.memory(
                        previewImage!,
                        gaplessPlayback: true,
                      )
                  ],
                ),
          floatingActionButton: IconButton(
            onPressed: () async {
              matching?.dispose();
              await controller?.dispose();
              setState(() {
                controller = null;
              });
            },
            icon: const Icon(
              Icons.clear_all,
            ),
          ),
        ),
      ),
    );
  }
}
