import 'dart:developer';

import 'package:flutter/material.dart';

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
  @override
  void initState() {
    super.initState();

    log('pixelmatching : ${FlutterPixelMatching().version()}');
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('OpenCV PixelMatching Plugin Sample'),
        ),
        body: const Center(
          child: Text(
            'OpenCV PixelMatching',
          ),
        ),
      ),
    );
  }
}
