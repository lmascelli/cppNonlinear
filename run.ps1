Clear-Host

# SYSTEM SPECIFIC VARIABLES

if ($IsLinux) {
  $python = 'python3'
  $VCPKG_DIR = "~/vcpkg"
  $GENERATOR = "Ninja"
}
else {
  $python = 'python'
  $VCPKG_DIR = "D:/vcpkg"
  $GENERATOR = 'Visual Studio 17 2022'
}

# ARGUMENT PARSING

$TestPy = $false;
$Clear = $false;
$Build = $false;
$Run = $false;

foreach ($arg in $args) {
  switch ($arg) {
    'test-py' { $TestPy = $true }
    'clear' { $Clear = $true }
    'build' { $Build = $true }
    'run' { $Run = $true }
    default { Write-Output "$arg is not a valid command." }
  }
}

# CLEANING BUILD FILES

if ($Clear) {
  Write-Output "Cleaning"
  Remove-Item -Recurse -Path "build"
}

# RUN JUST PYTHON TEST FILE

if ($TestPy) {
  Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
  Invoke-Expression ($python + ' test/test.py')
}

# BUILD AND RUN

if ($Build -or $Run) {
  if (Test-Path -Path build/CmakeCache.txt -PathType leaf) {
  }
  else {
    mkdir build
    Push-Location build
    $VCPKG_CMAKE = $VCPKG_DIR + "/scripts/buildsystems/vcpkg.cmake"
    cmake .. -G"$GENERATOR" -DCMAKE_TOOLCHAIN_FILE="$VCPKG_CMAKE" -DCMAKE_EXPORT_COMPILE_COMMANDS=True
    Pop-Location
  }

  Push-Location build
  cmake --build . --config "Release"
  Pop-Location

  if ($IsLinux) {
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    Copy-Item -Force build\libsystem_test.so test\system_test.so
    Copy-Item -Force build\cppLib\libpynl.so test\pynl.so
  }
  else {
    Copy-Item -Recurse -Force -Path "pyLib\nonlinear" -Destination "test"
    Copy-Item -Force build\Release\system_test.dll test\system_test.pyd
    Copy-Item -Force build\cppLib\Release\pynl.dll test\pynl.pyd
  }
}

if ($Run) {
  if ($IsLinux) {
    Invoke-Expression ($python + ' test/analisys.py')
    #    Remove-Item -Recurse -Force -Path "test\nonlinear"
  }
  else {
    Invoke-Expression ($python + ' test\analisys.py')
    #    Remove-Item -Recurse -Force -Path "test\nonlinear"
  }
}
