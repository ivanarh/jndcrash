package ru.ivanarh.jndcrash;

import android.support.annotation.Nullable;

public class NDCrash {

    public static void initialize(@Nullable String crashReportPath, int unwinder, boolean outOfProcess) {
        nativeInitialize(crashReportPath, unwinder, outOfProcess);
    }

    private static native void nativeInitialize(@Nullable String crashReportPath, int unwinder, boolean outOfProcess);

    static {
        System.loadLibrary("jndcrash");
    }
}
