pushd bin

rm -R minimalApp.app

mkdir minimalApp.app

# NOTE(Noah): All W compiler flags are not needed. For me, these are like training wheels.
CFLAGS="-x objective-c -target x86_64-apple-ios15.2-simulator \
-fmacro-backtrace-limit=0 -std=gnu11 -fobjc-arc -fobjc-weak -fpascal-strings -O0 \
-fno-common -g -DDEBUG=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0 -fobjc-abi-version=2 -fobjc-legacy-dispatch \
\
-Werror=return-type -Werror=non-modular-include-in-framework-module -Werror=deprecated-objc-isa-usage \
-Werror=objc-root-class \
\
-Wdocumentation -Wunreachable-code -Wquoted-include-in-framework-header \
-Wimplicit-retain-self -Wduplicate-method-match \
-Wparentheses -Wswitch -Wunused-function   \
-Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wconditional-uninitialized  \
-Wconstant-conversion -Wint-conversion -Wbool-conversion \
-Wenum-conversion  -Wnon-literal-null-conversion -Wobjc-literal-conversion \
-Wshorten-64-to-32 -Wpointer-sign   \
-Wundeclared-selector -Wdeprecated-implementations -Wprotocol -Wdeprecated-declarations   \
-Winfinite-recursion -Wcomma -Wblock-capture-autoreleasing -Wstrict-prototypes \
-Wunguarded-availability"

DIR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator15.2.sdk
clang -isysroot $DIR $CFLAGS ../minimalApp/*.m -framework Foundation \
    -framework UIKit -o minimalApp.app/minimalApp

cp ../minimalApp/Info.plist minimalApp.app/Info.plist

cp -R ../minimalApp/assets/ minimalApp.app/

open -a Simulator.app --args -CurrentDeviceUDID D04DA895-C6A6-43DB-A517-2E28FB9C40C2
xcrun simctl install booted minimalApp.app

popd