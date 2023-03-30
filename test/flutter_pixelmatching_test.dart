import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_pixelmatching/flutter_pixelmatching.dart';
import 'package:flutter_pixelmatching/flutter_pixelmatching_platform_interface.dart';
import 'package:flutter_pixelmatching/flutter_pixelmatching_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockFlutterPixelmatchingPlatform
    with MockPlatformInterfaceMixin
    implements FlutterPixelmatchingPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final FlutterPixelmatchingPlatform initialPlatform = FlutterPixelmatchingPlatform.instance;

  test('$MethodChannelFlutterPixelmatching is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFlutterPixelmatching>());
  });

  test('getPlatformVersion', () async {
    FlutterPixelmatching flutterPixelmatchingPlugin = FlutterPixelmatching();
    MockFlutterPixelmatchingPlatform fakePlatform = MockFlutterPixelmatchingPlatform();
    FlutterPixelmatchingPlatform.instance = fakePlatform;

    expect(await flutterPixelmatchingPlugin.getPlatformVersion(), '42');
  });
}
