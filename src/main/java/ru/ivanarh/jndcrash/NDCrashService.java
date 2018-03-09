package ru.ivanarh.jndcrash;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

public class NDCrashService extends Service
{
    private static final String TAG = "JNDCRASH";

    public static final String EXTRA_REPORT_FILE = "report_file";
    public static final String EXTRA_BACKEND = "backend";

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        if (!mNativeStarted && intent != null) {
            mNativeStarted = true;
            Log.i(TAG, "Starting native crash service...");
            startNativeServer(intent.getStringExtra(EXTRA_REPORT_FILE), intent.getIntExtra(EXTRA_BACKEND, 0));
        }
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        if (mNativeStarted) {
            mNativeStarted = false;
            Log.i(TAG, "Stopping native crash service...");
            stopNativeServer();
        }
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    private static boolean mNativeStarted = false;
    private static native void startNativeServer(@Nullable String crashReportPath, int backend);
    private static native void stopNativeServer();
}
