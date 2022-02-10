//
//  AppDelegate.h
//  myFirstIOSApp
//
//  Created by Noah Cabral on 2022-02-06.
//

#import <UIKit/UIKit.h>

// My app delegate is a UIResponder that implements the UIApplicationDelegate protocol.
// okay but like what does this mean in terms of the objective-C language?
//    literally just this. That it conforms to the protocol...so it defines the methods
//    that one might expect?

// Yes. A protocol is actually a first class citizen in the objective-C language.
// @protocol ProtocolName
//     list of methods and properties
// @end
// It defines a set of properties and methods that are independent of any class.

// UIApplicationDelegate is a protocol. The protocol itself conforms to NSObject.


@interface AppDelegate : UIResponder <UIApplicationDelegate>


@end

