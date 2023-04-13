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
  s.source_files     = 'Classes/**/*{.h,.cpp,.swift}'
  s.dependency       'Flutter'
  s.platform         = :ios, '11.0'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386' }
  s.swift_version = '5.0'

  # Pixelmatching
  s.dependency 'OpenCV', '4.3.0'
  s.module_map = 'flutter_pixelmatching.modulemap'
end
