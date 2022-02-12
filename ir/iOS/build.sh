pushd bin

rm -R minimalApp.app

mkdir minimalApp.app

CFLAGS="-x objective-c -target x86_64-apple-ios15.2-simulator -fmessage-length=0 \
-fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0 -std=gnu11 -fobjc-arc \
-fobjc-weak -Wnon-modular-include-in-framework-module -Werror=non-modular-include-in-framework-module \
-Wno-trigraphs -fpascal-strings -O0 -fno-common -Wno-missing-field-initializers -Wno-missing-prototypes \
-Werror=return-type -Wdocumentation -Wunreachable-code -Wquoted-include-in-framework-header \
-Wno-implicit-atomic-properties -Werror=deprecated-objc-isa-usage -Wno-objc-interface-ivars \
-Werror=objc-root-class -Wno-arc-repeated-use-of-weak -Wimplicit-retain-self -Wduplicate-method-match \
-Wno-missing-braces -Wparentheses -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter \
-Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wconditional-uninitialized -Wno-unknown-pragmas \
-Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion \
-Wenum-conversion -Wno-float-conversion -Wnon-literal-null-conversion -Wobjc-literal-conversion \
-Wshorten-64-to-32 -Wpointer-sign -Wno-newline-eof -Wno-selector -Wno-strict-selector-match \
-Wundeclared-selector -Wdeprecated-implementations -DDEBUG=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0 \
-fasm-blocks -fstrict-aliasing -Wprotocol -Wdeprecated-declarations -g -Wno-sign-conversion \
-Winfinite-recursion -Wcomma -Wblock-capture-autoreleasing -Wstrict-prototypes \
-Wno-semicolon-before-method-body -Wunguarded-availability -fobjc-abi-version=2 -fobjc-legacy-dispatch \
-MMD" 

DIR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator15.2.sdk
clang -isysroot $DIR $CFLAGS ../minimalApp/*.m -framework Foundation \
    -framework UIKit -o minimalApp.app/minimalApp

cp ../minimalApp/Info.plist minimalApp.app/Info.plist

cp -R ../minimalApp/assets/ minimalApp.app/

#open -a Simulator.app --args -CurrentDeviceUDID D04DA895-C6A6-43DB-A517-2E28FB9C40C2
#xcrun simctl install booted bin/minimalApp.app


popd