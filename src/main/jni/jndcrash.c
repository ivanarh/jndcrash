#include <android/log.h>
#include <ndcrash.h>
#include <jni.h>

JNIEXPORT void JNICALL Java_ru_ivanarh_jndcrash_NDCrash_nativeInitialize(
        JNIEnv *env,
        jclass type,
        jstring jCrashReportPath,
        jint backend) {
    const char *crashReportPath = NULL;
    if (jCrashReportPath) {
        crashReportPath = (*env)->GetStringUTFChars(env, jCrashReportPath, NULL);
    }
    __android_log_print(ANDROID_LOG_DEBUG, "JNDCRASH", "JNDCrash initialization, path: %s, backend: %d",
                        crashReportPath ? crashReportPath : "null",
                        backend);
    ndcrash_in_init((enum ndcrash_backend)backend, crashReportPath);
    if (jCrashReportPath) {
        (*env)->ReleaseStringUTFChars(env, jCrashReportPath, crashReportPath);
    }
}
