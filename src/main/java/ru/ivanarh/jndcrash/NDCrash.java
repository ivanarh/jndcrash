package ru.ivanarh.jndcrash;

import android.support.annotation.Nullable;

public class NDCrash {

    public static void initialize(@Nullable String crashReportPath, int backend, boolean outOfProcess) {
        nativeInitialize(crashReportPath, backend, outOfProcess);
    }

    private static native void nativeInitialize(@Nullable String crashReportPath, int backend, boolean outOfProcess);

    static {
        System.loadLibrary("jndcrash");
    }
}
