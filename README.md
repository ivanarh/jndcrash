# JNDCrash #

**JNDCrash** is a Java wrapper over [NDCrash C library](https://github.com/ivanarh/ndcrash) which significantly simplifies its usage. It includes **NDCrash** library as a submodule. See key concepts here https://github.com/ivanarh/ndcrash documentation.

## Integration ##

**JNDCrash** currently has experimental status an isn't published to any Maven repository. A recommended way to include it to a project is to add a submodule. Please note that 3 version of gradle is used. It's integrated to a project as a usual library, see [documentation](https://developer.android.com/studio/projects/android-library.html)
For example integraion please take a look at [ndcrashdemo application](https://github.com/ivanarh/ndcrashdemo).

## Usage ##

All **JNDCrash** method are wrappers over corresponding **NDCrash** functions.

### In-process ###

To initialize in-process signal handler you need to call `NDCrash.initializeInProcess` method. A good place to do this is `onCreate` method of Application subclass. Example with *libunwind* unwinder and crash report path inside getAbsolutePath() result:

```
@Override
public void onCreate() {
	super.onCreate();
	...
	final String reportPath = getFilesDir().getAbsolutePath() + "/crash.txt"; // Example.
	final NDCrashError error = NDCrash.initializeInProcess(reportPath, NDCrashUnwinder.libunwind);
	if (error == NDCrashError.ok) {
		// Initialization is successful.
	} else {
		// Initialization failed, check error value.
	}
	...
}
```

### Out-of-process ###

First you need to declare a new service that will work in a parallel process, please add a following line to your AndroidManifest.xml, application element:

```
<application
	...>
	...
    <service android:name="ru.ivanarh.jndcrash.NDCrashService" android:process=":reportprocess"/>
    ...
</application>
```

Note that ":reportprocess" is just string used for a process name, it doesn't affect library work but should be set.

Next you need to add some code that initializes a signal handler and starts out service. A recommended place for it is `onCreate` method of Application subclass. **One important detail:** Code that initializes a signal handler must be run only from main application process, not from background service process. The problem is that `onCreate` method is called for both processes and we need a way to determine if it's being run in a main process. For this purpose `NDCrashUtils.isMainProcess` method may be used.

For a signal handler initialization please call `NDCrash.initializeOutOfProcess` method. You need to pass a Context instance and don't need to specify a socket name, a package name with additional ".ndcrash" suffix will be used for this.
To specify a crash report output file path and an unwinder you need to pass this data to extras of Intent that will be used to start a service. A report passed as a string, an unwinder should be passed as an ordinal integer value of `NDCrashUnwinder` enum.

An example initialization code is below, assuming it's located in `onCreate` method of Application subclass.

```
    @Override
    public void onCreate() {
		super.onCreate();
		...
		if (NDCrashUtils.isMainProcess(this)) {
			final NDCrashError error = NDCrash.initializeOutOfProcess(this);
			if (error == NDCrashError.ok) {

			} else {

			}

			final Intent serviceIntent = new Intent(context, NDCrashService.class);
			final String reportPath = getFilesDir().getAbsolutePath() + "/crash.txt"; // Example.
	        serviceIntent.putExtra(NDCrashService.EXTRA_REPORT_FILE, reportPath);
	        serviceIntent.putExtra(NDCrashService.EXTRA_UNWINDER, NDCrashUnwinder.libunwind.ordinal());
	        startService(serviceIntent);
		}
		...
	}
```

### Immediate crash handling ###

You can access a crash report immediately, for example you can send it to your server straight after it's generated. It's supported **only in Out-of-process mode**. To do this you need to subclass `NDCrashService` class and override `onCrash` method:

```
public class CrashService extends NDCrashService {
    @Override
    public void onCrash(String reportPath) {
        // Read a file at reportPath.
    }
```

Also a service declaration in AndroidManifest.xml should be updated:

```
<service android:name=".CrashService" android:process=":reportprocess"/>
```

And, or course, you need to update a service starting code:

```
			final Intent serviceIntent = new Intent(context, CrashService.class);
			// Set extras...
	        startService(serviceIntent);
```

Please keep in mind that onCrash is run from background thread created by *pthread*. It means it doesn't have a Looper instance. Also note that when `onCrash` method is running other crash report can't be created, it means a very long blocking operation in it is unwanted.
