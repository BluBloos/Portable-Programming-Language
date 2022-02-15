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
