// My device (the iPhone 6s), uses the Apple A9 chip...
// which seems to use the ARMv8-A architecture...
// which also seems to be specified by the AArch64 architecture.

// maybe one of the most relevant resources is the following:
// https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms#//apple_ref/doc/uid/TP40009020-SW1

// NOTE, this work is an adaptation of https://github.com/richardjrossiii/iOSAppInAssembly

// WHAT IS THE CALLING CONVENTION?
/*
    arm 64 calling convention: 
    https://github.com/ARM-software/abi-aa/releases

    Sourced from: https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms#//apple_ref/doc/uid/TP40009020-SW1
    The stack pointer on Apple platforms follows the ARM64 standard ABI and requires 16-byte alignment. When passing 
    arguments to functions, Apple platforms diverge from the ARM64 standard ABI in the following ways:
        - Function arguments may consume slots on the stack that are not multiples of 8 bytes. 
        If the total number of bytes for stack-based arguments is not a multiple of 8 bytes, 
        insert padding on the stack to maintain the 8-byte alignment requirements.
        - When passing an argument with 16-byte alignment in integer registers, 
        Apple platforms allow the argument to start in an odd-numbered xN register. 
        The standard ABI requires it to begin in an even-numbered xN register.
        - The caller of a function is responsible for signing or zero-extending any argument with fewer 
        than 32 bits. The standard ABI expects the callee to sign or zero-extend those arguments.
        - Functions may ignore parameters that contain empty struct types. This behavior applies to 
        the GNU extension in C and, where permitted by the language, in C++. The AArch64 documentation 
        doesn’t address the issue of empty structures as parameters, but Apple chose this path for its implementation.
    */

// ---------------------------------------------------------------------------
// THIS FILE IS FOR ENTRY POINT INTO APPLICATION.

.section __TEXT,__cstring
s_delegateClassNameCStr:
    .asciz "AppDelegate" // .asciz will append null byte at end of string

// For main function, r0: error code to return to OS.
.section __TEXT,__text,regular,pure_instructions
.global _main
.align 4 // NOTE(Noah): I presume that this is to byte-align the start of the code?
_main:

    // NOTE(Noah): Standard modular programming procedure. For all the registers that are modified by 
    // this subroutine, we must save them and restore at the end of the subroutine.
    // we can find documentation for these exact instructions here: 
    // https://developer.apple.com/documentation/xcode/writing-armv7-code-for-ios
    push     {r4-r7, lr}           // save LR, R7, R4-R6
    add      r7, sp, #12           // adjust R7 to point to saved R7
    push     {r8, r10, r11}        // save remaining GPRs (R8, R10, R11)
    vstmdb   sp!, {d8-d15}         // save VFP/Advanced SIMD registers D8

    // Setup the autorelease pool.
    // _objc_autoreleasePoolPush is part of the Apple objective-C runtime.
    // maybe the best online reference to the existence of this function is the following Github issue:
    // https://github.com/gfx-rs/gfx/issues/2190
    //     takes no params, returns no params.
    bl _objc_autoreleasePoolPush // bl is branch to label.

    // Setup our app delegate's class. This is done using our function AppDelegate_Setup.
    //     It takes no parameters.
    //     It returns no results.
    bl AppDelegate_Setup

    // Setup our custom view's class. This is done using our function View_Setup.
    //     It takes no parameters.
    //     It returns no results.
    bl View_Setup

    /* Next, we need to actually start up the visual application. This is done in 2 steps.
       1: Create a Objective-C string from a C-String for our App Delegate's name.
       2: Start the visual application.  */
   
    movw r0, #:lower16:s_delegateClassNameCStr
    movt r0, #:upper16:s_delegateClassNameCStr
    bl util_getCFString // Turn our C string into a NSString, using our function util_getCFString.
    // The newly created string object is stored in r0 after calling the function.

    // The next step is to start the visual application. This is done using the function UIApplicationMain.
    //     Parameter 1 (r0) is the argument count passed to main. For the purposes of this demo, it is irrelevant.
    //     Parameter 2 (r1) is the argument list passed to main. For the purposes of this demo, it is irrelevant.
    //     Parameter 3 (r2) is the name of the principle class. This would be a subclass of UIApplication if we had one.
    //     Parameter 4 (r3) is the name of the delegate class. This is our AppDelegate string.
    mov r3, r0
    mov r2, #0 ; #0 in this instance is an immediate value.
    mov r1, #0
    mov r0, #0
    bl _UIApplicationMain
    // The result code of the application is stored in r0 after calling the function.

    // Finally, we must clean up our auto-release pool. This is done using the function objc_autoreleasePoolPop.
    //     It takes no parameters.
    //     It returns no usable results. However, it clobbers r0, so we save and re-load that variable around it.
    push {r0}
    bl _objc_autoreleasePoolPop
    pop {r0}

    // restore clobbered registers of the main subroutine.
    vldmia   sp!, {d8-d15}         // restore VFP/Advanced SIMD registers
    pop      {r8, r10, r11}        // restore R8-R11
    pop      {r4-r7, pc}           // restore R4-R6, saved R7, return to saved LR