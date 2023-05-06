#if !defined(PPL_ERROR_HPP)
#define PPL_ERROR_HPP

class RawFileReader;

// TODO: this needs fixing.
struct ppl_error_context
{
    const char *codeContext;
    uint32_t c;
    uint32_t line;
    const char *errMsg;

    // used to rate this error over others when deciding what error to give the user.
    uint32_t errorScore;

    RawFileReader *pTokenBirthplace;
};

#endif // PPL_ERROR_HPP