# Path Forward

Current Progress:
- objective-C to iOS simulator build working (without ever opening xCode).

The task remains to implement a small "Hello, World!" application. While doing so, it is necessary to understand generally how development for this device works.

This is all so that when it comes time to write the print function for this target, we know what we are doing.

In doing so, we must learn objective-C.

On the date of 2022.02.11, we were hastily trying to do this. Taking shortcuts. This proved detrimental and progress slowed to a near snail pace.

In order to effectively create a minimal iOS application, we have to take the "slow road". This is in fact the fastest road.

Once we finalize the minimal iOS app, we must strip the build system to keep just the CFLAGS that we need. Then slowly replace the app until the entire thing is written in assembly.
- We further note that the iOS simulator is x86_64 assembly while actual iPhones are ARM.

# Reference
So, there exists many commands that we can run via the xcode command line utility.

Open the IOS simulator.
- open -a Simulator.app --args -CurrentDeviceUDID D04DA895-C6A6-43DB-A517-2E28FB9C40C2 \
(this opens the iPhone 8 for my device)

Install and launch an app.
- xcrun simctl install booted \<PATH-TO-APPLICATION-BUNDLE>
- xcrun simctl launch booted \<BUNDLE-ID-OF-APP-BUNDLE>