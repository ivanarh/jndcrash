# JNDCrash #

**JNDCrash** is a Java wrapper over [NDCrash C library](https://github.com/ivanarh/ndcrash) which significantly simplifies its usage. It includes **NDCrash** library as a submodule. See key concepts here https://github.com/ivanarh/ndcrash documentation. Minimum Android version is 4.0.3.

## Integration ##

### Quick integration ###

**JNDCrash** is currently published to [bintray repository](https://bintray.com/ivanarh/ndcrash/jndcrash-libunwind) and accessible in jcenter. Currently only one version with *libunwind* unwinder is accessible, it means other unwinders are disabled during compilation and can't be used. You should initialize a library with `NDCrashUnwinder.libunwind` argument, otherwise it will return an error. This is done for optimization purposes, this unwinder is a good choice for majority of users and it's recommended to use. If you need another unwinder please go to "Advanced integration" and "Customization" sections.

To add a library to a project please add this line to your application's build.gradle, `dependencies` section:

```
compile 'ru.ivanarh.ndcrash:jndcrash-libunwind:0.1'
```

Also make sure that `jcenter()` is included to `repositories` section (it's already done in default project template). Run "Sync" operation and verify that no error has occured.

### Advanced integration ###

A more advanced way to include it to a project is to a project hierarchy. Please note that gradle version 3 is used. It's integrated to a project as a usual library, see [documentation](https://developer.android.com/studio/projects/android-library.html) For example integraion you can take a look at [ndcrashdemo application](https://github.com/ivanarh/ndcrashdemo). The following instructions assume that you use a default project template provided by Android Studio. A fresh version of Android NDK with clang toolchain should be installed.

First you need to add **JNDCrash** to a project root. If you use **git** in your project it's a good idea integrate **JNDCrash** as a submodule. To do this please run a following console command in a project root:

```
git submodule add git@github.com:ivanarh/jndcrash.git jndcrash
cd jndcrash
git submodule update --init --recursive
```

If you don't use git please replace `git submodule add` command by `git clone` with the same arguments. Verify that no error has occured. After that please change `settings.gradle` file, replace a line:

```
include ':app'
```

to

```
include ':app', ':jndcrash'
```

After that please add this line to your application's build.gradle, `dependencies` section:

```
implementation project(':jndcrash')
```

Run "Sync" operation and verify that no error has occured.

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

## Customization ##

For optimization purposes you can customize **NDCrash** library, see *Customization* section in [NDCrash docs](https://github.com/ivanarh/ndcrash)
Since **JNDCrash** is a wrapper, it's customized along with underlying library with the same parameters passed by CMake variables. You always can create a fork of these libraries and set parameters, for example, to build.gradle file of **JNDCrash**:

```
android {
	...
    defaultConfig {
    	...
		externalNativeBuild {
		    cmake {
		        arguments "-DANDROID_STL=c++_static", "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
		    }
		}
	}
}
```

If you don't wish to create a fork you can use a following ability to customize enabled modules set of **JNDCrash**: You can create `jndcrash.cmake` file in the same directory where **JNDCrash** submodule is cloned, for example see this file in [demo application](https://github.com/ivanarh/ndcrashdemo). Here is example contents of this file:

```
# Modes.
set(ENABLE_INPROCESS ON)
set(ENABLE_OUTOFPROCESS ON)

# Unwinders.
set(ENABLE_LIBCORKSCREW ON)
set(ENABLE_LIBUNWIND ON)
set(ENABLE_LIBUNWINDSTACK ON)
set(ENABLE_CXXABI ON)
set(ENABLE_STACKSCAN ON)
```

By default, if this file is absent, all modes all modes and unwinders are switched on. Don't forget that you should initialize crash reporting library with supported mode and supported unwinder, otherwise an initialization will fail.

