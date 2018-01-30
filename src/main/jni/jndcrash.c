#include <android/log.h>
#include <ndcrash.h>
#include <jni.h>

JNIEXPORT void JNICALL Java_ru_ivanarh_jndcrash_NDCrash_nativeInitialize(JNIEnv *env, jclass type, jstring jCrashReportPath) {
    const char * crashReportPath = NULL;
    if (jCrashReportPath) {
        crashReportPath = (*env)->GetStringUTFChars(env, jCrashReportPath, NULL);
    }
    __android_log_print(ANDROID_LOG_DEBUG, "JNDCRASH", "JNDCrash initialization, path: %s", crashReportPath ? crashReportPath : "null");
    ndcrash_in_init(ndcrash_backend_libcorkscrew, crashReportPath);
    if (jCrashReportPath) {
        (*env)->ReleaseStringUTFChars(env, jCrashReportPath, crashReportPath);
    }
}
