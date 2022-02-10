//
//  main.m
//  myFirstIOSApp
//
//  Created by Noah Cabral on 2022-02-06.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

// objective-c is a superset of the C language, provides ojbect oriented features,
// and gives a dynamic runtime,

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName; // A static, plain-text Unicode string object.
    
    // will destory our objects.
    // this is done by automatic reference counting.
    @autoreleasepool {
        appDelegateClassName = NSStringFromClass([AppDelegate class]);  
    }
    
    // Creates the application object and the application delegate and sets up the event cycle.
    //     UIApplication is the app process.
    //     UIApplicationDelegate is an obj that responds to events happening within the process.
    // Must provide a Launch storyboard to display while our app is loading.
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
}
