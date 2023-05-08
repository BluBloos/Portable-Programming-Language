#if !defined(PPL_ERROR_HPP)
#define PPL_ERROR_HPP

class RawFileReader;
struct ppl_error_context;

void GenerateCodeContextFromFilePos(ppl_error_context &ctx, uint32_t line, uint32_t c, char *buf, uint32_t bufLen);

static constexpr uint32_t PPL_ERROR_MESSAGE_MAX_LENGTH = 256;
static constexpr uint32_t PPL_ERROR_PARTIAL_AST_MAX_LENGTH = 4096;

enum ppl_error_kind
{
    PPL_ERROR_KIND_LEXER,
    PPL_ERROR_KIND_PARSER,
    PPL_ERROR_KIND_SEMA,
};

// TODO: we maybe want to have info for many errors in flight.
struct ppl_error_context
{

    // -------- details on a single error -------- 
    uint32_t c;
    uint32_t line;
    ppl_error_kind kind;
    char codeContext[PPL_ERROR_MESSAGE_MAX_LENGTH];
    char errMsg[PPL_ERROR_MESSAGE_MAX_LENGTH];
    // TODO: originally I tried to "save" the tree.
    // it was looking kind of complicated. so I decided to simply serialize
    // the tree to string at the time of error.
    //
    // maybe there is something less "dumb" here?
    char almostParsedTree[PPL_ERROR_PARTIAL_AST_MAX_LENGTH] = {};
    // used to rate this error over others when deciding what error to give the user.
    uint32_t m_errorScore;
    // -------- details on a single error -------- 


    struct tree_node *currTopLevelTree;

    // TODO: I guess when we start having those #import ideas this kind of thing is going to
    // change.
    RawFileReader *pTokenBirthplace;

    bool SubmitError(ppl_error_kind kind, uint32_t line, uint32_t c, uint32_t errorScore)
    {
        if (errorScore > m_errorScore)
        {
            this->kind = kind;
            this->line = line;
            this->c = c;
            if (pTokenBirthplace != nullptr)
            {
                GenerateCodeContextFromFilePos(*this, line, c, this->codeContext, PPL_ERROR_MESSAGE_MAX_LENGTH);
            }
            m_errorScore = errorScore;
            return true;
        }
        return false;
    }
};

#endif // PPL_ERROR_HPP