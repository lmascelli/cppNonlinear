clear

if ($IsLinux) {
  $python = 'python3'
  $VCPKG_DIR = "~/vcpkg"
} else {
  $python = 'python'
  $VCPKG_DIR = "D:/vcpkg"
}

if ($args[0] -eq '--test-py') {
  Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
  Invoke-Expression ($python + ' test/test.py')
#  Remove-Item -Recurse -Force -Path "test\nonlinear"
}
else {
  if (Test-Path -Path build/CmakeCache.txt -PathType leaf) {
  }
  else {
    mkdir build
    Push-Location build
    $VCPKG_CMAKE = $VCPKG_DIR + "/scripts/buildsystems/vcpkg.cmake"
    cmake .. -G"Ninja" -DCMAKE_TOOLCHAIN_FILE="$VCPKG_CMAKE" -DCMAKE_EXPORT_COMPILE_COMMANDS=True
    Pop-Location
  }

  Push-Location build
  cmake --build . --config "Release"
  Pop-Location

  if ($IsLinux) {
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    Copy-Item -Force build\libsystem_test.so test\system_test.so
    Copy-Item -Force build\cppLib\libpynl.so test\pynl.so
    Invoke-Expression ($python + ' test/analisys.py')
#    Remove-Item -Recurse -Force -Path "test\nonlinear"
  }
  else {
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    Copy-Item -Force build\Release\system_test.dll test\system_test.pyd
    Copy-Item -Force build\cppLib\Release\pynl.dll test\pynl.pyd
    Invoke-Expression ($python + ' test\analisys.py')
#    Remove-Item -Recurse -Force -Path "test\nonlinear"
  }
}
