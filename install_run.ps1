clear

pushd build
cmake --build . --config "Release"
popd

if ( $IsLinux){
    # TODO
    python3 pynl_test.py
} else {
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    copy -Force build\Release\system_test.dll test\system_test.pyd
    copy -Force build\cppLib\Release\pynl.dll test\pynl.pyd
    python test\test.py
}