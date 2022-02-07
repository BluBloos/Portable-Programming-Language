//
//  main.m
//  myFirstIOSApp
//
//  Created by Noah Cabral on 2022-02-06.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#import "myClass.h"

// here we are going to define a class thing.
@implementation myClass

- (instancetype) initWithName:(NSString *)defaultName
{
    self = [super init];
    if (self) {
        self.name = defaultName;
    }
    return self;
}

@end

// objective-c is a superset of the C language, provides ojbect oriented features,
// and gives a dynamic runtime,

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName; // A static, plain-text Unicode string object.
    
    // will destory our objects.
    // this is done by automatic reference counting.
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
        appDelegateClassName = NSStringFromClass([AppDelegate class]);
        
        // NOTE(Noah): This is where I am doing my learning.
        //NSLog(@"%s", appDelegateClassName);
        
        NSString *myStr = @"This is a string";
        NSLog(@"Size of string: %d", (int)[myStr length]); // wierd obj-c way of calling methods.
        NSLog(@"Char: %c", [myStr characterAtIndex:5]); // woah
        
        char *name = "Derek";
        NSString *myName = [NSString stringWithFormat:@"- %s", name];
        
        BOOL isStringEqual = [myStr isEqualToString:myName];
        printf("Are string equal: %d\n", isStringEqual);
        
        // NOTE(Noah): The [] are called messages in objective-c.
        // we can nest them.
        const char *uCString = [[myName uppercaseString]UTF8String];
        
        // instantiating a class.
        myClass *myClassInstance = [[myClass alloc] initWithName:@"myClass lol"];
        
        NSLog(@"Name of myClass is: %@", [myClassInstance name]); // here we are using the automatically generated getter
        
        // Some other interesting things about Objective-C:
        // there seems to be a runtime library for obj-c and we directly call those functions?
        
    }
    
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
}
