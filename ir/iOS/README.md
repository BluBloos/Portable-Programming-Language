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

# 2022.03.09

So we are yet again running into issues. This repo does NOT work out of the gate. 

So now I need to think of another way to move forward here. There are actually many different
components of what I am trying to do.

Maybe one of the most important ones is to do the following: 
- "How to compile and objective-C app for xCode."

Because if I cannot even do something as simple as this, then how do I expect to do the same for 
a full-blown assembly project? 

All I can say is....what the fuck have I gotten myself into...

Here is a useful article I found in the last moments as I scrambled to try to get something working,
https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art024