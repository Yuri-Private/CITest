# CITest

Test bed for building embedded device software (an Android UI app and STM32 firmware) on GitHub-hosted runners.
The projects here are small examples written for this repository.
They use common toolchains for such devices, so the workflows can be reused for real projects.

| Folder | Example | Toolchain | Workflow |
|---|---|---|---|
| `android/` | One-screen Android app | Gradle 8.13, Android Gradle Plugin 8.13.2, compileSdk 36, minSdk 29, Java 11, JDK 17 | `.github/workflows/android.yml`: debug and release (unsigned) APKs |
| `firmware/` | LED blink for the STM32F767ZI (NUCLEO-F767ZI, LD2 on PB7), bare metal: 2 s of fast blinking after reset, then one blink every 500 ms | Arm GNU Toolchain 13.3 with STM32CubeIDE Release options (Cortex-M7, hard float, newlib-nano, `-O1`), CMake | `.github/workflows/firmware.yml`: `.elf`, `.bin`, `.hex`, `.map` |

## Branches and builds

`dev` is the integration branch: work is done on feature branches and merged into `dev` by pull request.
`main` gets `dev`'s commits when a release is due, and releases are tagged on `main`.

| Event | Workflow | Version bump |
|---|---|---|
| Pull request (into any branch) | `android.yml` / `firmware.yml` for the folder it changes; the result is shown as a check on the pull request | No |
| Push or merge to `dev` | `ci.yml`: builds what changed, plus release notes | Yes (below) |
| Push or merge to `main` or another branch | `android.yml` / `firmware.yml` for the folder it changes | No |
| Tag `v*` | `release.yml` (see Releases) | No |

All of them can also be started by hand (**Actions** → workflow → **Run workflow**).
The build outputs are attached to each run as artifacts.

## Versions

Versions have the form `major.minor.revision.build` and live in the source code:

| File | Content |
|---|---|
| `android/version.properties` | App version |
| `firmware/Core/Inc/version.h` | Firmware version (`FW_VERSION_*`) and the app version the firmware is paired with (`UI_VERSION_*`) |

Edit `major`, `minor` and `revision` by hand. The build number is CI's: on every push (merge) to `dev`, `ci.yml` runs
`ci/version.sh` and commits the result back to `dev` before building:

- the app changed: app build + 1, the new app version is written into `UI_VERSION_*`, and so the firmware
  (whose source changed) gets build + 1 too;
- only the firmware changed: firmware build + 1.

Build numbers never reset, so the app's `versionCode` (= its build number) always increases.
Only `dev` changes build numbers, so merging `dev` into `main` never conflicts on the version files;
`main` carries the version of the `dev` build it was merged from. After the bump, pull before committing on `dev`.
The firmware keeps `"CITest FW <fw version> for app <app version>"` in flash.

## Commit messages and release notes

Release notes are generated from git for every build of `dev` (artifact `ReleaseNotes-…`, also shown on the
run page) and for every release (release text, plus `.md` and `.docx` files). They list the features and the
defect fixes since the baseline: the previous `v*` tag, or the tag or commit written in a file `release-baseline`.

For that, start each commit message with its type and, optionally, the part it changes
([Conventional Commits](https://www.conventionalcommits.org/)):

| Message | Listed under |
|---|---|
| `feat(app): Add PIN screen` | Features → App |
| `fix(fw): Tip timeout at 0 s (#42)` | Defects fixed → Firmware, linked to issue 42 |
| `feat(fw)!: New UI protocol v2` | Features → Firmware, marked breaking |
| `docs: …`, `refactor(app): …`, `ci: …`, `test: …`, `chore: …` | Other changes |

Scopes: `app` (or `android`, `ui`), `fw` (or `firmware`), `ci`. A `BREAKING CHANGE:` line in the message body also
marks a change as breaking, and issue numbers (`#42`) in the body are linked too. Commits without a type are listed
under Other changes, and CI shows a warning for each of them.

## Releases

Merge `dev` into `main`, then push a tag `v*` on that commit of `main` to publish a release built from it:

```
git switch main
git pull
git merge --ff-only origin/dev
git push origin main
git tag -a v1.0 -m "CITest 1.0"
git push origin v1.0
```

`.github/workflows/release.yml` builds the app and the firmware, checks that the firmware is paired with this app
version, and creates a GitHub Release with:

- `CITestApp-<app version>-debug.apk`, `CITestApp-<app version>-release-unsigned.apk`
- `CITest_FW-<fw version>.elf`, `.bin`, `.hex`, `.map`
- `build-info.txt` (versions, tag, commit, build time, run link) and `SHA256SUMS`

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
