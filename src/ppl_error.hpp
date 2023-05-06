#if !defined(PPL_ERROR_HPP)
#define PPL_ERROR_HPP

// TODO: this needs fixing.
struct ppl_error
{
    const char *codeContext;
    uint32_t c;
    uint32_t line;
    const char *errMsg;

    // used to rate this error over others when deciding what error to give the user.
    uint32_t errorScore;
};

#endif // PPL_ERROR_HPP