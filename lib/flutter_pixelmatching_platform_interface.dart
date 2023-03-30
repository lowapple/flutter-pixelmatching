import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'flutter_pixelmatching_method_channel.dart';

abstract class FlutterPixelmatchingPlatform extends PlatformInterface {
  /// Constructs a FlutterPixelmatchingPlatform.
  FlutterPixelmatchingPlatform() : super(token: _token);

  static final Object _token = Object();

  static FlutterPixelmatchingPlatform _instance = MethodChannelFlutterPixelmatching();

  /// The default instance of [FlutterPixelmatchingPlatform] to use.
  ///
  /// Defaults to [MethodChannelFlutterPixelmatching].
  static FlutterPixelmatchingPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FlutterPixelmatchingPlatform] when
  /// they register themselves.
  static set instance(FlutterPixelmatchingPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
