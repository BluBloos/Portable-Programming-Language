#!/bin/bash

APPNAME="MinimalApp"
LABEL="$APPNAME"
APKFILE="$APPNAME.apk"
PACKAGENAME="org.ncabral.$APPNAME"
SRC="main.c"

ANDROIDVERSION="30"
ANDROIDTARGET="$ANDROIDVERSION"

ADB="adb"
UNAME=$(uname)

# NOTE(Noah): Yep. This bullcrap is just meant for testing anyways. No need to
# worry about any cross-platform junk.
OS_NAME="darwin-x86_64"

ANDROIDSDK="/Users/noahcabral/dev/android-sdk"
NDK="/Users/noahcabral/dev/android-sdk/ndk-bundle"
BUILD_TOOLS="/Users/noahcabral/dev/android-sdk/build-tools/30.0.0"


CFLAGS=-"ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden -Os -DANDROID \
    -DAPPNAME=$APPNAME -DANDROID_FULLSCREEN -I$NDK/sysroot/usr/include -I$NDK/sysroot/usr/include/android \
    -I$NDK/toolchains/llvm/prebuilt/$OS_NAME/sysroot/usr/include/android -fPIC \
    -DANDROIDVERSION=$ANDROIDVERSION"

LDFLAGS="-Wl,--gc-sections -s -lm -lGLESv3 -lEGL -landroid -llog -shared -uANativeActivity_onCreate"

CC_ARM64="$NDK/toolchains/llvm/prebuilt/$OS_NAME/bin/aarch64-linux-android$ANDROIDVERSION-clang"
CC_ARM32="$NDK/toolchains/llvm/prebuilt/$OS_NAME/bin/armv7a-linux-androideabi$ANDROIDVERSION-clang"
CC_x86="$NDK/toolchains/llvm/prebuilt/$OS_NAME/bin/i686-linux-android$ANDROIDVERSION-clang"
CC_x86_64="$NDK/toolchains/llvm/prebuilt/$OS_NAME/bin/x86_64-linux-android$ANDROIDVERSION-clang"
AAPT="$BUILD_TOOLS/aapt"

CFLAGS_ARM64="-m64"
CFLAGS_ARM32="-mfloat-abi=softfp -m32"
CFLAGS_x86="-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32"
CFLAGS_x86_64="-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel"

########### MAKE KEYSTORE ###########
STOREPASS="password"
KEYSTOREFILE="my-release-key.keystore"
ALIASNAME="standkey"
if [ -f "$KEYSTOREFILE" ]; then
    echo "Keystore already exists, skipping keystore generation"
else
    keytool -genkey -v -keystore $KEYSTOREFILE -alias $ALIASNAME -keyalg RSA \
        -keysize 2048 -validity 10000 -storepass $STOREPASS -keypass $STOREPASS \
        -dname "CN=example.com, OU=ID, O=Example, L=Doe, S=John, C=GB"
fi
########### MAKE KEYSTORE ###########

############### BUILD NATIVE APP #############
mkdir -p makecapk/lib/arm64-v8a
mkdir -p makecapk/lib/armeabi-v7a
	
$CC_ARM64 $CFLAGS $CFLAGS_ARM64 -o makecapk/lib/arm64-v8a/lib$APPNAME.so \
    $SRC $NDK/sources/android/native_app_glue/android_native_app_glue.c \
    -I$NDK/sources/android/native_app_glue \
    -L$NDK/toolchains/llvm/prebuilt/$OS_NAME/sysroot/usr/lib/aarch64-linux-android/$ANDROIDVERSION $LDFLAGS

$CC_ARM32 $CFLAGS $CFLAGS_ARM32 -o makecapk/lib/armeabi-v7a/lib$APPNAME.so \
    $SRC $NDK/sources/android/native_app_glue/android_native_app_glue.c \
    -I$NDK/sources/android/native_app_glue \
    -L$NDK/toolchains/llvm/prebuilt/$OS_NAME/sysroot/usr/lib/arm-linux-androideabi/$ANDROIDVERSION \
    $LDFLAGS
############### BUILD NATIVE APP #############

############### CREATE MANIFEST ##############
rm -rf AndroidManifest.xml
export ANDROIDTARGET
export ANDROIDVERSION
export APPNAME
export PACKAGENAME
export LABEL
envsubst '$ANDROIDTARGET $$ANDROIDVERSION $$APPNAME $$PACKAGENAME $$LABEL' \
    < AndroidManifest.xml.template > AndroidManifest.xml
############### CREATE MANIFEST ##############

############### MAKE APK ###############
#makecapk.apk : $(TARGETS) $(EXTRA_ASSETS_TRIGGER) AndroidManifest.xml	
# mkdir -p makecapk/assets
# cp -r Sources/assets/* makecapk/assets
rm -rf temp.apk
$AAPT package -f -F temp.apk -I $ANDROIDSDK/platforms/android-$ANDROIDVERSION/android.jar \
    -M AndroidManifest.xml -v --target-sdk-version $ANDROIDTARGET
unzip -o temp.apk -d makecapk
rm -rf makecapk.apk

# NOTE(Noah): This part we are unsure of...
pushd makecapk
zip -D9r ../makecapk.apk . && zip -D0r ../makecapk.apk ./resources.arsc ./AndroidManifest.xml
popd

jarsigner -sigalg SHA1withRSA -digestalg SHA1 -verbose -keystore $KEYSTOREFILE \
#    -storepass $STOREPASS makecapk.apk $ALIASNAME
#rm -rf $APKFILE
#$BUILD_TOOLS/zipalign -v 4 makecapk.apk $APKFILE
# Using the apksigner in this way is only required on Android 30+
#$BUILD_TOOLS/apksigner sign --key-pass pass:$STOREPASS --ks-pass pass:$STOREPASS --ks $KEYSTOREFILE $APKFILE
#rm -rf temp.apk
#rm -rf makecapk.apk
#@ls -l $APKFILE
############### MAKE APK ###############

############### RUN IN EMULATOR ###########
#@echo "Installing" $PACKAGENAME
#$ADB install -r $APKFILE
#$(eval ACTIVITYNAME:=$(shell $(AAPT) dump badging $(APKFILE) | grep "launchable-activity" | cut -f 2 -d"'"))
#$ADB shell am start -n $PACKAGENAME/$ACTIVITYNAME
############### RUN IN EMULATOR ###########

