#include <android/log.h>
#include <ndcrash.h>
#include <jni.h>

static const char LOG_TAG[] = "JNDCRASH";

JNIEXPORT void JNICALL Java_ru_ivanarh_jndcrash_NDCrash_nativeInitialize(
        JNIEnv *env,
        jclass type,
        jstring jCrashReportPath,
        jint backend,
        jboolean outOfProcess) {
    const char *crashReportPath = NULL;
    if (jCrashReportPath) {
        crashReportPath = (*env)->GetStringUTFChars(env, jCrashReportPath, NULL);
    }
    enum ndcrash_error error;
    if (outOfProcess) {
        error = ndcrash_out_init();
    } else {
        error = ndcrash_in_init((enum ndcrash_backend) backend, crashReportPath);
    }
    __android_log_print(ANDROID_LOG_DEBUG,
                        LOG_TAG,
                        "JNDCrash initialization, path: %s, backend: %d, out of process: %s, result: %d",
                        crashReportPath ? crashReportPath : "null",
                        backend,
                        outOfProcess ? "true" : "false",
                        (int) error);
    if (jCrashReportPath) {
        (*env)->ReleaseStringUTFChars(env, jCrashReportPath, crashReportPath);
    }
}

JNIEXPORT void JNICALL
Java_ru_ivanarh_jndcrash_NDCrashService_startNativeServer(JNIEnv *env, jclass type,
                                                          jstring crashReportPath_, jint backend) {
    const char *crashReportPath = (*env)->GetStringUTFChars(env, crashReportPath_, 0);
    const enum ndcrash_error error = ndcrash_out_start_daemon((enum ndcrash_backend) backend, crashReportPath);
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,
                        "JNDCrash out-of-process daemon starting, path: %s, backend: %d, result: %d",
                        crashReportPath ? crashReportPath : "null",
                        backend,
                        (int) error);
    (*env)->ReleaseStringUTFChars(env, crashReportPath_, crashReportPath);
}

JNIEXPORT void JNICALL
Java_ru_ivanarh_jndcrash_NDCrashService_stopNativeServer(JNIEnv *env, jclass type) {
    const bool success = ndcrash_out_stop_daemon();
    __android_log_print(
            ANDROID_LOG_DEBUG,
            LOG_TAG,
            "JNDCrash out-of-process daemon stopped, success: %s",
            success ? "true" : "false"
    );
}