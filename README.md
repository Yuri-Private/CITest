# CITest

Test bed for building embedded device software (an Android UI app and STM32 firmware) on GitHub-hosted runners.
The projects here are small examples written for this repository.
They use common toolchains for such devices, so the workflows can be reused for real projects.

| Folder | Example | Toolchain | Workflow |
|---|---|---|---|
| `android/` | One-screen Android app | Gradle 8.13, Android Gradle Plugin 8.13.2, compileSdk 36, minSdk 29, Java 11, JDK 17 | `.github/workflows/android.yml`: debug and release (unsigned) APKs |
| `firmware/` | LED blink for the STM32F767ZI (NUCLEO-F767ZI, LD2 on PB7), bare metal | Arm GNU Toolchain 13.3 with STM32CubeIDE Release options (Cortex-M7, hard float, newlib-nano, `-O1`), CMake | `.github/workflows/firmware.yml`: `.elf`, `.bin`, `.hex`, `.map` |

Each workflow runs on pushes and pull requests that change its folder, and can be started by hand
(**Actions** → workflow → **Run workflow**). The build outputs are attached to each run as artifacts.

## Building by hand

Android (JDK 17 and the Android SDK):

```
cd android
./gradlew assembleDebug assembleRelease
```

Firmware (CMake and the Arm GNU Toolchain on the path):

```
cmake -S firmware -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
