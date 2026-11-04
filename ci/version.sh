#!/usr/bin/env bash
# Reads and bumps the versions major.minor.revision.build kept in the source code:
#   android/version.properties    app version
#   firmware/Core/Inc/version.h   firmware version (FW_VERSION_*) and the paired app version (UI_VERSION_*)
#
#   ci/version.sh                    print the versions only
#   ci/version.sh android            app build + 1, and the firmware's UI_VERSION_* = the new app version
#   ci/version.sh firmware           firmware build + 1
#   ci/version.sh android firmware   both, in this order
#
# Prints app_version=, fw_version= and ui_version= lines (ready for $GITHUB_OUTPUT).
set -euo pipefail
cd "$(dirname "$0")/.."

PROPS=android/version.properties
HEADER=firmware/Core/Inc/version.h

prop()     { sed -n "s/^$1=\([0-9][0-9]*\)[[:space:]]*\$/\1/p" "$PROPS"; }
set_prop() { sed -i "s/^$1=[0-9][0-9]*/$1=$2/" "$PROPS"; }
def()      { sed -n "s/^#define $1 [[:space:]]*\([0-9][0-9]*\).*/\1/p" "$HEADER"; }
set_def()  { sed -i "s/^\(#define $1 [[:space:]]*\)[0-9][0-9]*/\1$2/" "$HEADER"; }

app_version() { echo "$(prop major).$(prop minor).$(prop revision).$(prop build)"; }
fw_version()  { echo "$(def FW_VERSION_MAJOR).$(def FW_VERSION_MINOR).$(def FW_VERSION_REVISION).$(def FW_VERSION_BUILD)"; }
ui_version()  { echo "$(def UI_VERSION_MAJOR).$(def UI_VERSION_MINOR).$(def UI_VERSION_REVISION).$(def UI_VERSION_BUILD)"; }

for part in "$@"; do
  case "$part" in
    android)
      set_prop build $(( $(prop build) + 1 ))
      set_def UI_VERSION_MAJOR    "$(prop major)"
      set_def UI_VERSION_MINOR    "$(prop minor)"
      set_def UI_VERSION_REVISION "$(prop revision)"
      set_def UI_VERSION_BUILD    "$(prop build)"
      ;;
    firmware)
      set_def FW_VERSION_BUILD $(( $(def FW_VERSION_BUILD) + 1 ))
      ;;
    *)
      echo "unknown part '$part' (use android and/or firmware)" >&2
      exit 2
      ;;
  esac
done

# Every number must have been found: an edited file that no longer matches would give "1..0." here.
for v in "$(app_version)" "$(fw_version)" "$(ui_version)"; do
  [[ "$v" =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]] || { echo "cannot read a version from $PROPS / $HEADER: '$v'" >&2; exit 3; }
done

echo "app_version=$(app_version)"
echo "fw_version=$(fw_version)"
echo "ui_version=$(ui_version)"
