/*
 * Version numbers major.minor.revision.build.
 * Edit FW_VERSION_MAJOR / _MINOR / _REVISION by hand. CI (ci/version.sh) increments FW_VERSION_BUILD
 * on every firmware change merged into dev, and writes UI_VERSION_* whenever the Android app is built.
 */
#ifndef VERSION_H
#define VERSION_H

/* Firmware version */
#define FW_VERSION_MAJOR     1
#define FW_VERSION_MINOR     0
#define FW_VERSION_REVISION  0
#define FW_VERSION_BUILD     2

/* Version of the Android app this firmware is paired with (written by the Android CI build) */
#define UI_VERSION_MAJOR     1
#define UI_VERSION_MINOR     0
#define UI_VERSION_REVISION  0
#define UI_VERSION_BUILD     2

#define VERSION_STR_(x)  #x
#define VERSION_STR(x)   VERSION_STR_(x)

/* "x.y.z.b" strings */
#define FW_VERSION  VERSION_STR(FW_VERSION_MAJOR) "." VERSION_STR(FW_VERSION_MINOR) "." \
                    VERSION_STR(FW_VERSION_REVISION) "." VERSION_STR(FW_VERSION_BUILD)
#define UI_VERSION  VERSION_STR(UI_VERSION_MAJOR) "." VERSION_STR(UI_VERSION_MINOR) "." \
                    VERSION_STR(UI_VERSION_REVISION) "." VERSION_STR(UI_VERSION_BUILD)

#endif /* VERSION_H */
