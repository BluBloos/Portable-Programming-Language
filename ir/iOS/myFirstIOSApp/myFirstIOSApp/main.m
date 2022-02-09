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
        // Setup code that might create autoreleased objects goes here.
        appDelegateClassName = NSStringFromClass([AppDelegate class]);  
    }
    
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
}
