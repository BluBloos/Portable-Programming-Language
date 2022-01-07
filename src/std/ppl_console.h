#ifndef PPL_CONSOLE
#define PPL_CONSOLE
#include <stdio.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <memory.h>
#include <ppl_core.h>

// NOTE(Noah): LMAOOOOO. Love how my string type is char *. BEAUT.
#define string char * 

namespace ppl {
    enum Color {
        RED,
        GREEN
    };

    // Waits until the given file stream is ready.
    // ready means that the call will not block (like, someone buffered some input in stdin).
    // We are GOOD TO GO.
    bool _wait_until_fready(FILE *fin) {
        // pass in a file descriptor set to watch.
        timeval timeout; timeout.tv_sec = 0; timeout.tv_usec = 0;
        int nfds = 1;
        fd_set rfds; // file descript set.
        FD_ZERO(&rfds);
        // TODO(Noah): Add more of these.
        if (fin == stdin) {
            FD_SET(STDIN_FILENO, &rfds);
            select(nfds, &rfds, NULL, NULL, &timeout);
            if (FD_ISSET(STDIN_FILENO, &rfds)) 
                return true;
        }
        /* After select() has returned, readfds will be cleared of
        all file descriptors except for those that are ready for
        reading. */
        return false;
    }

    FILE *STDIN = stdin;

    struct terminal_attr {
            unsigned long input_speed;
            unsigned long output_speed;
            unsigned long local_modes;
            unsigned long  input_modes;     //Input modes. 
                unsigned long  output_modes;     //Output modes. 
                unsigned long  control_modes;     // Control modes. 
                unsigned char  special_characters[NCCS];  // Control characters. 
    };

    void __copy_termios_to_tattr(termios &t, terminal_attr &a) {
        a.local_modes = t.c_lflag;
        a.input_modes = t.c_iflag;
        a.output_modes = t.c_oflag;
        a.control_modes = t.c_cflag;
        a.input_speed = t.c_ispeed;
        a.output_speed = t.c_ospeed;
        memcpy(a.special_characters, t.c_cc, NCCS * sizeof(unsigned char));
    }

    void __copy_tattr_to_termios(terminal_attr &a, termios &t) {
        t.c_lflag = a.local_modes;
        t.c_iflag = a.input_modes;
        t.c_oflag = a.output_modes;
        t.c_cflag = a.control_modes;
        t.c_ispeed = a.input_speed;
        t.c_ospeed = a.output_speed;
        memcpy(t.c_cc, a.special_characters, NCCS * sizeof(unsigned char));
    }    

    class terminal {
        public:
        terminal() {
            this->_attr = this->_get_attr();
            this->_attr_save = this->_attr;
        }
        terminal_attr _attr_save;
        terminal_attr _attr;
        terminal_attr _get_attr() {
            // update _attr with underlying terminal API.
            terminal_attr attr;
            termios t;
            tcgetattr( STDIN_FILENO, &t );
            __copy_termios_to_tattr(t, attr);    
            return attr; 
        }
        void _set_attr(terminal_attr attr) {
            this->_attr = attr;
            termios t;
            __copy_tattr_to_termios(this->_attr, t);
            // TCSANOW means the change will occur immediately.
            tcsetattr( STDIN_FILENO, TCSANOW, &t );
        }
        void _reset_attr() {
                _set_attr(this->_attr_save);
        }
    };

    terminal _get_attached_term() {
        return terminal();
    };

    // NOTE(NOah): printf works with whatever the system locale is.
    // my locale is set to utf8. Thus, utf8 is supported!!
    void _print(char *fmt, ...) {
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
    void _scan(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vscanf(fmt, args);
        va_end (args);
    }
    void _print_color(Color color, char *fmt, ...) {
        // Set color
        switch(color) {
            case RED:
            printf("\033[0;31m");
            break;
            case GREEN:
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

    uint32 _getchar() {
        return (uint32)getchar();
    }

}
#endif