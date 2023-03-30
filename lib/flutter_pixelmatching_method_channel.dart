import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'flutter_pixelmatching_platform_interface.dart';

/// An implementation of [FlutterPixelmatchingPlatform] that uses method channels.
class MethodChannelFlutterPixelmatching extends FlutterPixelmatchingPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('flutter_pixelmatching');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
