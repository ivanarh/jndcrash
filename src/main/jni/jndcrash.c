#include <ndcrash.h>
#include <jni.h>
#include <malloc.h>

static const char LOG_TAG[] = "JNDCRASH";

// NDCrash.java methods

JNIEXPORT jint JNICALL Java_ru_ivanarh_jndcrash_NDCrash_nativeInitializeInProcess(
        JNIEnv *env,
        jclass type,
        jstring jCrashReportPath,
        jint unwinder) {
    const char *crashReportPath = NULL;
    if (jCrashReportPath) {
        crashReportPath = (*env)->GetStringUTFChars(env, jCrashReportPath, NULL);
    }
    const enum ndcrash_error error = ndcrash_in_init((enum ndcrash_unwinder) unwinder, crashReportPath);
    if (jCrashReportPath) {
        (*env)->ReleaseStringUTFChars(env, jCrashReportPath, crashReportPath);
    }
    return (jint) error;
}

JNIEXPORT void JNICALL Java_ru_ivanarh_jndcrash_NDCrash_deInitializeInProcess(
        JNIEnv *env,
        jclass type) {
    ndcrash_in_deinit();
}

JNIEXPORT jint JNICALL Java_ru_ivanarh_jndcrash_NDCrash_nativeInitializeOutOfProcess(
        JNIEnv *env,
        jclass type) {
    const enum ndcrash_error error = ndcrash_out_init();
    return (jint) error;
}

JNIEXPORT void JNICALL Java_ru_ivanarh_jndcrash_NDCrash_deInitializeOutOfProcess(
        JNIEnv *env,
        jclass type) {
    ndcrash_out_deinit();
}

JavaVM * jndcrash_javavm = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jndcrash_javavm = vm;
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    jndcrash_javavm = NULL;
}

// Special struct where we save values that we need to run a crash callback.
typedef struct {

    /// Global reference to NDCrash class.
    jclass jc_NDCrash;

    /// Method ID of runOnCrashCallback.
    jmethodID jm_runOnCrashCallback;

    /// JNI environment for daemon background thread.
    JNIEnv *daemon_thread_env;

} jndcrash_callback_arg_t;

/// Called on daemon start from its background thread.
static void jndcrash_daemon_start(void *argvoid) {
    jndcrash_callback_arg_t * const arg = (jndcrash_callback_arg_t *) argvoid;
    (*jndcrash_javavm)->AttachCurrentThread(jndcrash_javavm, &arg->daemon_thread_env, NULL);
}

/// Called when a crash report is generated. From background thread.
static void jndcrash_on_crash(const char *report_file, void *argvoid) {
    jndcrash_callback_arg_t * const arg = (jndcrash_callback_arg_t *) argvoid;
    const jstring j_report_file = (*arg->daemon_thread_env)->NewStringUTF(arg->daemon_thread_env, report_file);
    (*arg->daemon_thread_env)->CallStaticVoidMethod(
            arg->daemon_thread_env, arg->jc_NDCrash, arg->jm_runOnCrashCallback, j_report_file);
    (*arg->daemon_thread_env)->DeleteLocalRef(arg->daemon_thread_env, j_report_file);
}

/// Called on daemon stop from its background thread.
static void jndcrash_daemon_stop(void *argvoid) {
    jndcrash_callback_arg_t * const arg = (jndcrash_callback_arg_t *) argvoid;
    (*jndcrash_javavm)->DetachCurrentThread(jndcrash_javavm);
}

JNIEXPORT jint JNICALL Java_ru_ivanarh_jndcrash_NDCrash_startOutOfProcessDaemon(
        JNIEnv *env,
        jclass type,
        jstring crashReportPath_,
        jint unwinder) {
    const char *crashReportPath = NULL;
    if (crashReportPath_) {
        crashReportPath = (*env)->GetStringUTFChars(env, crashReportPath_, 0);
    }

    jndcrash_callback_arg_t * const arg = calloc(1, sizeof(jndcrash_callback_arg_t));
    arg->jc_NDCrash = (*env)->NewGlobalRef(env, type);
    arg->jm_runOnCrashCallback = (*env)->GetStaticMethodID(env, arg->jc_NDCrash, "runOnCrashCallback", "(Ljava/lang/String;)V");

    const enum ndcrash_error error = ndcrash_out_start_daemon(
            (enum ndcrash_unwinder) unwinder,
            crashReportPath,
            &jndcrash_daemon_start,
            &jndcrash_on_crash,
            &jndcrash_daemon_stop,
            arg);

    if (crashReportPath) {
        (*env)->ReleaseStringUTFChars(env, crashReportPath_, crashReportPath);
    }

    return (jint) error;
}

JNIEXPORT jboolean JNICALL Java_ru_ivanarh_jndcrash_NDCrash_nativeStopOutOfProcessDaemon(JNIEnv *env, jclass type) {
    jndcrash_callback_arg_t * const arg = (jndcrash_callback_arg_t *) ndcrash_out_get_daemon_callbacks_arg();
    if (arg) {
        (*env)->DeleteGlobalRef(env, arg->jc_NDCrash);
        free(arg);
    }
    return (jboolean) ndcrash_out_stop_daemon();
}