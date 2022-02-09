pushd bin

rm -R myFirstIOSApp.app

mkdir myFirstIOSApp.app

DIR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk
clang -isysroot $DIR -march=ios ../myFirstIOSApp/myFirstIOSApp/*.m -framework Foundation \
    -framework UIKit -o myFirstIOSApp.app/myFirstIOSApp

cp  Info.plist myFirstIOSApp.app/Info.plist

#xcrun simctl install booted myFirstIOSApp.app
#open -a Simulator.app --args -CurrentDeviceUDID D04DA895-C6A6-43DB-A517-2E28FB9C40C2

popd