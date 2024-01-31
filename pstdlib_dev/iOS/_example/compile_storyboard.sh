/Applications/Xcode.app/Contents/Developer/usr/bin/ibtool --errors --warnings --notices \
    --module Test_App --output-partial-info-plist ./spartial.plist \
    --auto-activate-custom-fonts --target-device iphone --target-device ipad \
    --minimum-deployment-target 15.2 --output-format human-readable-text \
    --compilation-directory minimalApp.app/Base.lproj \
    ../minimalApp/LaunchScreen.storyboard