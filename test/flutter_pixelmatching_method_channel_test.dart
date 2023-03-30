import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_pixelmatching/flutter_pixelmatching_method_channel.dart';

void main() {
  MethodChannelFlutterPixelmatching platform = MethodChannelFlutterPixelmatching();
  const MethodChannel channel = MethodChannel('flutter_pixelmatching');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
