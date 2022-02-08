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

Ran into some issues today with getting the iOS build to work. Tried to compile with clang but the armv7 architecture simply does not exist on my system. Makes sense, this Github repo is WAY back. Things are much more up-to-date now. 

Current effort right now:
- Clone and try to run the repo through Xcode.
- Trying to install exact phone as used by repo owner.
- Then try command line again once we can get repo working.