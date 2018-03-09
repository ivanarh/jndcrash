package ru.ivanarh.jndcrash;

import android.support.annotation.Nullable;

public class NDCrash {

    public static void initialize(@Nullable String crashReportPath, int backend) {
        nativeInitialize(crashReportPath, backend);
    }

    private static native void nativeInitialize(@Nullable String crashReportPath, int backend);

    static {
        System.loadLibrary("jndcrash");
    }
}
