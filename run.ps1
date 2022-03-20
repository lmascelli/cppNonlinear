clear

pushd build
cmake --build . --config "Release"
popd

if ( $IsLinux){
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    copy -Force build\libsystem_test.so test\system_test.so
    copy -Force build\cppLib\libpynl.so test\pynl.so
    python3 test/test.py
    Remove-Item -Recurse -Force -Path "test\nonlinear"
} else {
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    copy -Force build\Release\system_test.dll test\system_test.pyd
    copy -Force build\cppLib\Release\pynl.dll test\pynl.pyd
    python test\test.py
    Remove-Item -Recurse -Force -Path "test\nonlinear"
}