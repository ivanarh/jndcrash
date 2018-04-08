package ru.ivanarh.jndcrash;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Process;

/**
 * Contains some utility code.
 */
public class NDCrashUtils {

    /**
     * Checks if a current process is a main process of application. Returns false if this code
     * is running in a service with android:process attribute in manifest.
     *
     * @param context Current context.
     * @return Flag whether it's a main process.
     */
    public static boolean isMainProcess(Context context) {
        final int pid = Process.myPid();
        final String packageName = context.getPackageName();
        final ActivityManager manager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        if (manager != null) {
            for (final ActivityManager.RunningAppProcessInfo info : manager.getRunningAppProcesses()) {
                if (info.pid == pid) {
                    return packageName.equals(info.processName);
                }
            }
        }
        return true;
    }

}
