package ru.ivanarh.jndcrash;

import android.support.annotation.Nullable;

public class NDCrash {

    public static void initialize(@Nullable String crashReportPath) {
        nativeInitialize(crashReportPath);
    }

    private static native void nativeInitialize(@Nullable String crashReportPath);

    static {
        System.loadLibrary("jndcrash");
    }
}
