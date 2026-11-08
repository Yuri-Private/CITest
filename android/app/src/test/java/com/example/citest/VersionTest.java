package com.example.citest;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

public class VersionTest {

    @Test
    public void parsesFourParts() {
        Version v = Version.parse("1.2.3.45");
        assertEquals(1, v.major);
        assertEquals(2, v.minor);
        assertEquals(3, v.revision);
        assertEquals(45, v.build);
    }

    @Test
    public void printsTheSameText() {
        assertEquals("1.0.0.2", Version.parse("1.0.0.2").toString());
        assertEquals("1.0.0", Version.parse("1.0.0.2").release());
    }

    @Test
    public void rejectsOtherFormats() {
        for (String bad : new String[] {null, "", "1.2.3", "1.2.3.4.5", "1.2.x.4", "1.2.-3.4", "1..3.4"}) {
            assertThrows(bad, IllegalArgumentException.class, () -> Version.parse(bad));
        }
    }

    @Test
    public void comparesPartByPartNotAsText() {
        assertTrue(Version.parse("1.0.0.10").compareTo(Version.parse("1.0.0.9")) > 0);
        assertTrue(Version.parse("2.0.0.1").compareTo(Version.parse("1.9.9.99")) > 0);
        assertEquals(0, Version.parse("1.2.3.4").compareTo(Version.parse("1.2.3.4")));
        assertEquals(Version.parse("1.2.3.4"), new Version(1, 2, 3, 4));
    }

    @Test
    public void theAppsOwnVersionIsValid() {
        Version v = Version.parse(BuildConfig.VERSION_NAME);
        assertEquals(BuildConfig.VERSION_CODE, Math.max(1, v.build));
    }
}
