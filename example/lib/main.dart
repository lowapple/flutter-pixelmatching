import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_pixelmatching_example/matching_view.dart';

// ignore: depend_on_referenced_packages
import 'package:image_picker/image_picker.dart';

void main() {
  runApp(const MaterialApp(home: MyApp()));
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('PixelMatching Sample'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            buildFromGallery(),
            buildFromCamera(),
          ],
        ),
      ),
    );
  }

  /// Build a button to fetch from the gallery, and set `image1` on image selection.
  buildFromGallery() {
    return TextButton.icon(
      icon: const Icon(Icons.photo),
      label: const Text("From Gallery"),
      onPressed: () async {
        ImagePicker().pickImage(source: ImageSource.gallery).then(
          (value) async {
            final Uint8List? target = await value?.readAsBytes();
            if (target != null) {
              openMatchingView(target);
            }
          },
        );
      },
    );
  }

  /// Build a button to fetch from the camera, and set `image1` on image selection.
  buildFromCamera() {
    return TextButton.icon(
      label: const Text("From Camera"),
      icon: const Icon(Icons.camera),
      onPressed: () async {
        ImagePicker().pickImage(source: ImageSource.camera).then(
          (value) async {
            final Uint8List? target = await value?.readAsBytes();
            if (target != null) {
              openMatchingView(target);
            }
          },
        );
      },
    );
  }

  openMatchingView(Uint8List target) {
    Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) {
          return MatchingView(target: target);
        },
      ),
    );
  }
}
