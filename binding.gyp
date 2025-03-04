{
  "targets": [
    {
      "target_name": "drivelists",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7"
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1,
          "AdditionalOptions": [ "/EHsc" ]
        },
        "VCLinkerTool": {
          "SetChecksum": "true"
        }
      },
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<!@(pkg-config --cflags libusb-1.0 | sed s/-I//g)",
        "."
      ],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "defines": [ "NAPI_VERSION=9" ],
      "sources": [
        "src/drivelists.cpp",
        "src/device-descriptor.cpp",
        "src/usb_device.cpp",
      ],
      "conditions": [
        [ 'OS=="mac"', {
          "xcode_settings": {
            "OTHER_CPLUSPLUSFLAGS": [ "-stdlib=libc++" ],
            "OTHER_LDFLAGS": [ "-stdlib=libc++" ]
          },
          "sources": [
            "src/darwin/list.mm",
            "src/darwin/REDiskList.m"
          ],
          "link_settings": {
            "libraries": [ "-framework Carbon,DiskArbitration" ]
          }
        }],
        [ 'OS=="win"', {
          "sources": [ "src/windows/list.cpp" ],
          "libraries": [
            "-lKernel32.lib",
            "-lShell32.lib",
            "-lSetupAPI.lib"
          ]
        }],
        [ 'OS=="linux"', {
          "sources": [ "src/linux/list.cpp" ]
        }]
      ]
    }
  ]
}
