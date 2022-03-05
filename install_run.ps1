clear

pushd build
cmake --build . --config "Release"
popd

if ( $IsLinux){
    move -Force build\libsystem_test.so system_test.so
    move -Force build\src\libpynl.so pynl.so
    python3 pynl_test.py
} else {
    move -Force build\Release\system_test.dll system_test.pyd
    move -Force build\src\Release\pynl.dll pynl.pyd
    python pynl_test.py
}