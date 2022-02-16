Install the command line tools by going here.

https://developer.android.com/studio#command-tools

The extracted folder is something like cmdline-tools.

Place this directory into something like android-sdk (name can be anything). Should place into a higher level directory because downloading subsequent tools will place these tools into whatever directory is containing cmdline-tools. 

Also make sure to put the directory structure like so: \
cmdline-tools/latest/bin/...

Here are the commands to run.

```bash
$ sdkmanager "platform-tools" "platforms;android-30"
$ sdkmanager "system-images;android-30;google_apis;x86"
```

Note that the 30 in this instance is the API level. This can be whatever you would like....okay. Now we want to create an Android virtual device.

```bash
$ avdmanager create avd -n Android30 -k "system-images;android-28;google_apis;x86"
```

Again, we can name this whatever we like. In this case we are naming it Android30.

Finally, we can run this is the emulator like so.

```bash
$ emulator -avd Android30 
```

As for getting an app running, let's start by trying to build something in C...

We can the following link for this: https://hackaday.com/2020/05/13/writing-android-apps-in-c-no-java-required/

# 2022.02.16

Starting today by taking a gander at the makefile and
trying to get a sense of the commands that are run.

Seems the NDK is being used (the native development kit).

What is this doing for us?
- Main NDK website quotes that we can develop parts of an Android app using native code.

...what is the primary structure of an apk?

## Structure of APK

An apk is just a ZIP folder. Quite similar to an app bundle for iOS.

- classes.dex. Compiled DEX bytecode. Then compiled on the device into OAT files when installed.
- res/
  - folder contains images and the sort
- resources.arsc
  - resourced are "compiled and flattened" into this file.
- AndroidManifest.xml
- libs/*/
  - native libraries (C and C++ code).
- assets/   
  - Not android resources. Niche and depending on the application. Opended directly via file stream.
- META-INF/
  - Present in all APKs and contains a list of all files in the APK with their signatures.

Now we cross reference the known structure and analyze what is going on with the C to android project.

- The C source is compiled by the NDK and placed in the libs/ folder.

So it would seem that the application is fully just compiled C/C++ code that lives in the libs/ part of the apk.

Then in the manifest the application is specified as a native activity.

You compile the C code to APPNAME.so and you specify this APPNAME as metadata in the AndroidManifest.

Cool!

Okay, so it is seeming that I will not nede to fiddle around with the DEX bytecode at all. I can just write full-blown binaries.

After some more googling around on the NDK webpage, it seems that I have found a really compelling page talking about Android ABIs. This is EXACTLY what I want.

https://developer.android.com/ndk/guides/abis

Right, and there exists something called the JNI. This is the Java Native Interface. Java and Kotlin code are compiled down to bytecode (DEX). If we want to communicate with these, we use the JNI. 
- And apparently there is something called "marshalling"?


JNI:
- The java VM is multithreaded. So we told to compile with -mt.
- many complexities...

# Roadmap Forward

Want to strip down the example C app into a minimal app.

Then transform it into assembly language for some specific target ABI of the Android platform.

Calling into the NDK, we just need to figure out how the C headers get mangled. At the end of the day, symbols are symbols.

The safest bet is to use the compilers shipped by the android people. Here's to hoping that they take asm as input...