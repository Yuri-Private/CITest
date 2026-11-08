package com.example.citest;

/** A version major.minor.revision.build, as written in version.properties and version.h. */
public final class Version implements Comparable<Version> {

    public final int major;
    public final int minor;
    public final int revision;
    public final int build;

    public Version(int major, int minor, int revision, int build) {
        if (major < 0 || minor < 0 || revision < 0 || build < 0) {
            throw new IllegalArgumentException("Version parts must not be negative");
        }
        this.major = major;
        this.minor = minor;
        this.revision = revision;
        this.build = build;
    }

    /** Parses "x.y.z.b"; throws IllegalArgumentException for anything else. */
    public static Version parse(String text) {
        if (text == null) {
            throw new IllegalArgumentException("No version");
        }
        String[] parts = text.trim().split("\\.", -1);
        if (parts.length != 4) {
            throw new IllegalArgumentException("Version must be major.minor.revision.build: " + text);
        }
        int[] numbers = new int[4];
        for (int i = 0; i < 4; i++) {
            if (!parts[i].matches("\\d{1,9}")) {
                throw new IllegalArgumentException("Not a version number: " + text);
            }
            numbers[i] = Integer.parseInt(parts[i]);
        }
        return new Version(numbers[0], numbers[1], numbers[2], numbers[3]);
    }

    /** The release part, major.minor.revision, as shown to users. */
    public String release() {
        return major + "." + minor + "." + revision;
    }

    @Override
    public int compareTo(Version other) {
        int[] a = {major, minor, revision, build};
        int[] b = {other.major, other.minor, other.revision, other.build};
        for (int i = 0; i < 4; i++) {
            if (a[i] != b[i]) {
                return Integer.compare(a[i], b[i]);
            }
        }
        return 0;
    }

    @Override
    public boolean equals(Object o) {
        return o instanceof Version && compareTo((Version) o) == 0;
    }

    @Override
    public int hashCode() {
        return ((major * 31 + minor) * 31 + revision) * 31 + build;
    }

    @Override
    public String toString() {
        return release() + "." + build;
    }
}
