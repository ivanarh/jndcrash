package ru.ivanarh.jndcrash;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

/**
 * Main binding class for NDCrash functionality.
 */
public class NDCrash {

    /**
     * Initializes NDCrash library signal handler using in-process mode.
     *
     * @param crashReportPath Path where a crash report is saved.
     * @param unwinder        Used unwinder. See ndcrash_unwinder type in ndcrash.h.
     * @return Error status.
     */
    public static Error initializeInProcess(@Nullable String crashReportPath, Unwinder unwinder) {
        return Error.values()[nativeInitializeInProcess(crashReportPath, unwinder.ordinal())];
    }

    /// Native implementation method.
    private static native int nativeInitializeInProcess(@Nullable String crashReportPath, int unwinder);

    /**
     * De-initializes NDCrash library signal handler using in-process mode.
     */
    public static native void deInitializeInProcess();

    /**
     * Initializes NDCrash library signal handler using out-of-process mode.
     *
     * @param context Context instance. Used to determine a socket name.
     * @return Error status.
     */
    public static Error initializeOutOfProcess(Context context) {
        return Error.values()[nativeInitializeOutOfProcess(getSocketName(context))];
    }

    /// Native implementation method.
    private static native int nativeInitializeOutOfProcess(@NonNull String socketName);

    /**
     * De-initializes NDCrash library signal handler using out-of-process mode.
     */
    public static native void deInitializeOutOfProcess();

    /**
     * Starts NDCrash out-of-process unwinding daemon. This is necessary for out of process crash
     * handling. This method should be run from a service that works in separate process.
     *
     * @param context Context instance. Used to determine a socket name.
     * @param crashReportPath Path where to save a crash report.
     * @param unwinder Unwinder to use.
     */
    public static Error startOutOfProcessDaemon(
            Context context,
            @Nullable String crashReportPath,
            Unwinder unwinder,
            @Nullable OnCrashCallback callback) {
        mOnCrashCallback = callback;
        final Error result = Error.values()[startOutOfProcessDaemon(getSocketName(context), crashReportPath, unwinder.ordinal())];
        if (result != Error.ok) {
            mOnCrashCallback = null;
        }
        return result;
    }

    /// Native implementation method.
    private static native int startOutOfProcessDaemon(
            @NonNull String socketName,
            @Nullable String crashReportPath,
            int unwinder);

    /**
     * Stops NDCrash out-of-process unwinding daemon.
     */
    public static boolean stopOutOfProcessDaemon() {
        final boolean result = nativeStopOutOfProcessDaemon();
        mOnCrashCallback = null;
        return result;
    }

    /// Native implementation method.
    private static native boolean nativeStopOutOfProcessDaemon();

    /**
     * Instance of crash callback.
     */
    @Nullable
    private static volatile OnCrashCallback mOnCrashCallback = null;

    /**
     * Runs on crash callback if it was set. This method is called from native code.
     *
     * @param reportPath Path to file containing crash report.
     */
    private static void runOnCrashCallback(String reportPath) {
        final OnCrashCallback callback = mOnCrashCallback;
        if (callback != null) {
            callback.onCrash(reportPath);
        }
    }

    /**
     * Retrieves a socket name from Context instance. We use a package name with additional suffix
     * to make sure that socket name doesn't intersect with another application.
     *
     * @param context Context to use.
     * @return Socket name.
     */
    private static String getSocketName(Context context) {
        return context.getPackageName() + ".ndcrash";
    }

    /**
     * Crash callback that allows to process a report immediately after crash. Works only in out of
     * process mode.
     */
    public interface OnCrashCallback {

        /**
         * Runs when crash is detected. This method is run from background (daemon) thread.
         * This method allows to process a report immediately after crash.
         *
         * @param reportPath Path to file containing crash report.
         */
        void onCrash(String reportPath);

    }

    static {
        System.loadLibrary("jndcrash");
    }
}
