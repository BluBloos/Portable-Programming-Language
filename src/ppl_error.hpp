#if !defined(PPL_ERROR_HPP)
#define PPL_ERROR_HPP

class RawFileReader;

static constexpr uint32_t PPL_ERROR_MESSAGE_MAX_LENGTH = 256;

// TODO: we maybe want to have info for many errors in flight.
struct ppl_error_context
{
 
    // TODO:
    const char *errMsg; 
 
    uint32_t c;
    uint32_t line;

    char codeContext[PPL_ERROR_MESSAGE_MAX_LENGTH];

    // used to rate this error over others when deciding what error to give the user.
    uint32_t errorScore;

    RawFileReader *pTokenBirthplace;
};

#endif // PPL_ERROR_HPP