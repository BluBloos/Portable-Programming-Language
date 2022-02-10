So, there exists many commands that we can run via the xcode command line utility.

We're gonna leverage xcode, hell yeah, it's the only way. But we sure as hell aren't going to
open the application.

Here are some relevant commands:

Open the IOS simulator.
- open -a Simulator.app --args -CurrentDeviceUDID D04DA895-C6A6-43DB-A517-2E28FB9C40C2

Install and launch an app.
- xcrun simctl install booted "<PATH-TO-APPLICATION-BUNDLE>"
- xcrun simctl launch booted "<BUNDLE-ID-OF-APP-BUNDLE>"

# Path Forward

iOS build seems to be working. But is displaying nothing. Since I am new to iOS development, going to learn a thing or two about this world and implement a dead-simple "Hello, World!" application.