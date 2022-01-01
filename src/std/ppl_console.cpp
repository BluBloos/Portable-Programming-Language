#include "ppl_console.h"
// NOTE(NOah): printf works with whatever the system locale is.
// my locale is set to utf8. Thus, utf8 is supported!!
void ppl::print(char *fmt, ...) {

        // need to handle %c and interpret params from this as unicode 
        // code points.

        // when passing to printf, %c -> %s and we convert the unicode code
        // points into utf-8 multi-byte strings.

        // How it works:
        // go through the fmt string. Note each non %% thing. 
                // param offset.
        // if you find a %c, 
                // use param offset to get the arg.
                // conver the argument and store back in args (va_list).
                // then also convert in place the %c to %s in the utf8 formatted fmt string.
        
        /*int paramCount = 0;
        int i = 0; // pointer into fmt char array
        for (
                char *pChar = fmt; 
                pChar != 0; 
                ( u8_inc(pChar, &i), pChar = &fmt[i] ) 
        ) {
                int j = i; char *pCharNext;
                u8_inc(pChar, &j); pCharNext = fmt[j];
                if (*pChar == '%' && &pCharNext != '%') {
                        // get the next char
                        if (*pCharNext == 'c') {
                                // convert from code point to utf8 string
                                // and convert the arg in va_list.
                                char dest[5];
                                int result = u8_wc_toutf8(dest, u_int32_t ch);

                                *pCharNext = 's'; // write into fmt buffer.
                        }

                        // increase the counter for params
                        paramCount++;
                }


        }*/

        // cannot directly modify the va_args list...
        // but can construct a new set of parameters dynamically
                // but in order to do this we need to handle every % case in printf...
        // and pass these to vprintf directly instead of passing args...

        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
}
void ppl::scan(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vscanf(fmt, args);
        va_end (args);
}
void ppl::print_color(ppl::Color color, char *fmt, ...) {
        // Set color
        switch(color) {
                case ppl::RED:
                printf("\033[0;31m");
                break;
                case ppl::GREEN:
                printf("\033[0;32m");
                break;
        }
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        // reset the color
        printf("\033[0m");
}