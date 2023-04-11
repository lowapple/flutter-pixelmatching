#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint flutter_pixelmatching.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'flutter_pixelmatching'
  s.version          = '0.0.1'
  s.summary          = 'Pixelmatching plugin with OpenCV'
  s.description      = <<-DESC
Pixelmatching plugin with OpenCV
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files = 'Classes/**/*{.h,.cpp,.swift}'
  s.dependency 'Flutter'
  s.platform = :ios, '11.0'

  s.preserve_paths = 'opencv2.framework'

  s.xcconfig = {
    'OTHER_LDFLAGS' => '-framework opencv2',
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++20',
  }
  s.vendored_frameworks = 'opencv2.framework'
  s.frameworks = 'AVFoundation'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386' }
  s.swift_version = '5.0'

  # Pixelmatching
  # s.dependency 'OpenCV', '4.3.0'
  s.library = 'c++'
  s.module_map = 'flutter_pixelmatching.modulemap'
end
