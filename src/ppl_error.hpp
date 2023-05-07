#if !defined(PPL_ERROR_HPP)
#define PPL_ERROR_HPP

class RawFileReader;
struct ppl_error_context;

void GenerateCodeContextFromFilePos(ppl_error_context &ctx, uint32_t line, uint32_t c, char *buf, uint32_t bufLen);

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
    uint32_t m_errorScore;

    // TODO: I guess when we start having those #import ideas this kind of thing is going to
    // change.
    RawFileReader *pTokenBirthplace;

    void SubmitError(const char *errMsg, uint32_t line, uint32_t c, uint32_t errorScore)
    {
        if (errorScore > m_errorScore)
        {
            this->errMsg = errMsg;
            this->line = line;
            this->c = c;
            if (pTokenBirthplace != nullptr)
            {
                GenerateCodeContextFromFilePos(*this, line, c, this->codeContext, PPL_ERROR_MESSAGE_MAX_LENGTH);
            }
            m_errorScore = errorScore;
        }
    }
};

#endif // PPL_ERROR_HPP