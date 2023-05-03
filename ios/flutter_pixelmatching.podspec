Pod::Spec.new do |s|
  s.name                  = 'flutter_pixelmatching'
  s.version               = '0.0.1'
  s.summary               = 'Pixelmatching plugin with OpenCV'
  s.description           = <<-DESC
This project is a Flutter plugin that compares two images and outputs their similarity using Native C++ with OpenCV. It provides an efficient way to integrate image comparison features into Flutter applications.
                       DESC
  s.homepage              = 'https://github.com/lowapple/flutter_pixelmatching'
  s.license               = { :file => '../LICENSE', :type => 'MIT' }
  s.author                = { 'lowapple' => 'imyounjs@gmail.com' }
  s.source                = { :path => '.' }
  s.source_files          = 'Classes/**/*'
  s.dependency              'Flutter'
  s.platform              = :ios, '11.0'
  s.ios.deployment_target = '11.0'
  s.swift_version         = '5.0'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig   = {
    'DEFINES_MODULE'      => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386',
    'OTHER_CFLAGS'        => '-Wno-documentation',
    'CODE_SIGN_IDENTITY'  => 'iPhone Developer', 'CODE_SIGNING_REQUIRED' => 'NO'
  }

  # Pixelmatching
  s.dependency              'OpenCV', '4.3.0'
  s.static_framework      = true
end
