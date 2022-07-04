#include "Parser.h"
#include "../Exception/DARICException.h"
#include <tuple>
#include <circle/timer.h>
extern CTimer *timer;

std::map<TokenType, std::list<ComplexType>> Parser::generic_functions;
std::map<TokenType, void *> Parser::generic_functions_ptr;

void Parser::Error(std::string message, Token *token)
{
    throw DARICException(ExceptionType::PARSER, filenames->at(token->file_number), token->line_number,
                         token->char_number, message);
}

void Parser::SyntaxError(Token *token)
{
    throw DARICException(ExceptionType::PARSER, filenames->at(token->file_number), token->line_number,
                         token->char_number, "Syntax error");
}

void Parser::TypeError(Token *token)
{
    throw DARICException(ExceptionType::PARSER, filenames->at(token->file_number), token->line_number,
                         token->char_number, "Unexpected type");
}

void Parser::AddFuncTokenType(TokenType type, std::list<ComplexType> types, void *func)
{
    generic_functions.insert(std::make_pair(type, types));
    generic_functions_ptr.insert(std::make_pair(type, func));
}

void Parser::Init()
{
    // Strings
    AddFuncTokenType(TokenType::ASC, fp{TypeInteger(), TypeString()}, (void *)&call_STRING_asc);
    AddFuncTokenType(TokenType::CHRS, fp{TypeString(), TypeInteger()}, (void *)&call_STRING_chrs);
    AddFuncTokenType(TokenType::MIDS, fp{TypeString(), TypeString(), TypeInteger(), TypeInteger()},
                     (void *)&call_STRING_mids);
    AddFuncTokenType(TokenType::INSTR, fp{TypeInteger(), TypeString(), TypeString(), TypeInteger()},
                     (void *)&call_STRING_instr);
    AddFuncTokenType(TokenType::LEFTS, fp{TypeString(), TypeString(), TypeInteger()}, (void *)&call_STRING_lefts);
    AddFuncTokenType(TokenType::RIGHTS, fp{TypeString(), TypeString(), TypeInteger()}, (void *)&call_STRING_rights);
    AddFuncTokenType(TokenType::LEN, fp{TypeInteger(), TypeString()}, (void *)&call_STRING_len);
    AddFuncTokenType(TokenType::STRINGS, fp{TypeString(), TypeString(), TypeInteger()}, (void *)&call_STRING_strings);

    // 2D
    AddFuncTokenType(TokenType::MODE, fp{TypeNone(), TypeInteger(), TypeInteger()}, (void *)&call_2D_mode);
    AddFuncTokenType(TokenType::SCREENWIDTH, fp{TypeInteger()}, (void *)&call_2D_screenwidth);
    AddFuncTokenType(TokenType::SCREENHEIGHT, fp{TypeInteger()}, (void *)&call_2D_screenheight);
    AddFuncTokenType(TokenType::COLOUR, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_colour);
    AddFuncTokenType(TokenType::COLOURBG, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_colourbg);
    AddFuncTokenType(TokenType::SHADOW, fp{TypeNone()}, (void *)&call_2D_shadow);
    AddFuncTokenType(TokenType::FLIP, fp{TypeNone()}, (void *)&call_2D_flip);
    AddFuncTokenType(TokenType::CLS, fp{TypeNone()}, (void *)&call_2D_cls);
    AddFuncTokenType(TokenType::SHADOW, fp{TypeNone()}, (void *)&call_2D_shadow);
    AddFuncTokenType(TokenType::PLOT, fp{TypeNone(), TypeInteger(), TypeInteger()}, (void *)&call_2D_plot);
    AddFuncTokenType(TokenType::LINE,
                     fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_line);
    AddFuncTokenType(TokenType::TRIANGLE,
                     fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(),
                        TypeInteger(), TypeInteger()},
                     (void *)&call_2D_triangle);
    AddFuncTokenType(TokenType::TRIANGLEFILLED,
                     fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(),
                        TypeInteger(), TypeInteger()},
                     (void *)&call_2D_trianglefilled);
    AddFuncTokenType(TokenType::CIRCLE, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_circle);
    AddFuncTokenType(TokenType::CIRCLEFILLED,
                     fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_circlefilled);
    AddFuncTokenType(TokenType::RECTANGLE,
                     fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_rectangle);
    AddFuncTokenType(TokenType::RECTANGLEFILLED,
                     fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_rectanglefilled);
    AddFuncTokenType(TokenType::CLIPOFF, fp{TypeNone()}, (void *)&call_2D_clipoff);
    AddFuncTokenType(TokenType::CLIPON, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger()},
                     (void *)&call_2D_clipon);
    AddFuncTokenType(TokenType::TEXT, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeString()},
                     (void *)&call_2D_text);
    AddFuncTokenType(TokenType::TEXTCENTRE, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeString()},
                     (void *)&call_2D_textcentre);
    AddFuncTokenType(TokenType::TEXTRIGHT, fp{TypeNone(), TypeInteger(), TypeInteger(), TypeString()},
                     (void *)&call_2D_textright);
    AddFuncTokenType(TokenType::SETFONT, fp{TypeNone(), TypeString(), TypeString(), TypeInteger()},
                     (void *)&call_2D_font);

    // Keyboard
    AddFuncTokenType(TokenType::INKEY, fp{TypeInteger(), TypeInteger()}, (void *)&call_INKEY);

    // Random
    AddFuncTokenType(TokenType::RND, fp{TypeInteger(), TypeInteger()}, (void *)&call_RNDI);
    AddFuncTokenType(TokenType::RNDF, fp{TypeFloat(), TypeFloat()}, (void *)&call_RNDF);
}

bool Parser::Parse(bool optimise, std::list<Token> *tokens, std::vector<std::string> *filenames)
{
    this->optimise = optimise;
    it = tokens->begin();
    this->filenames = filenames;

    // Fast scan of all DEFs for forward lookup
    // Top level
    Token *t;
    do
    {
        t = GetToken();
        switch (t->type)
        {
        case TokenType::DEF:
            Parser_DEFFastScan(t);
            break;
        default:
            break;
        }
    } while (t->type != TokenType::NONE);
    it = tokens->begin();

    // Top level
    while (1)
    {
        Token *t = GetToken();
        switch (t->type)
        {
        case TokenType::COLON:
        case TokenType::NEWLINE:
            // Ignore
            break;
        case TokenType::NONE:
            // We are done
            return true;

            // PROCs
        case TokenType::DEF:
            Parser_DEF(t, &final_tokens);
            break;

            // Constants
        case TokenType::CONST:
            Parser_CONST(t, &final_tokens);
            break;

            // DATA
        case TokenType::DATA:
            Parser_DATA(t, &final_tokens);
            break;
        case TokenType::DATALABEL:
            Parser_DATALABEL(t, &final_tokens);
            break;

        default:
            ParseStatement(t, &final_tokens);
        }
    }
    return false;
}

void Parser::ParseSequenceOfStatements(Token *t, std::set<TokenType> block_terminator)
{
    // Do we have any leading colons or newlines?
    auto tt = GetToken();
    bool single_line = block_terminator.count(TokenType::NEWLINE) > 0;
    if (!single_line)
    {
        while (tt->type == TokenType::COLON || tt->type == TokenType::NEWLINE)
            tt = GetToken();
    }
    else
    {
        while (tt->type == TokenType::COLON)
            tt = GetToken();
    }
    PushTokenBack();

    // Create new branch
    t->branches.push_back(std::list<Token *>());
    auto to = &t->branches[t->branches.size() - 1];
    do
    {
        tt = GetToken();
        ParseStatement(tt, to);

        // Trailing colons??
        tt = GetToken();
        if (single_line)
        {
            while (tt->type == TokenType::COLON)
                tt = GetToken();
        }
        else
        {
            while (tt->type == TokenType::COLON || tt->type == TokenType::NEWLINE)
                tt = GetToken();
        }
        PushTokenBack();
    } while (single_line ? !IsStatementTerminator(false, tt) : !IsBlockTerminator(false, tt, block_terminator));

    // Trailing colons or new lines??
    while (tt->type == TokenType::COLON || tt->type == TokenType::NEWLINE)
        tt = GetToken();

    PushTokenBack();
}

void Parser::ParserStatementGeneric(Token *t, std::list<Token *> *tokens_out, fp *parameters, bool expression)
{
    // Parentheses are optional
    auto tt = GetToken();
    bool optional_parens = true;
    if (tt->type != TokenType::LPARENS)
    {
        PushTokenBack();
        optional_parens = false;
    }

    // Create space for all expressions
    bool first = true;
    for (auto p : *parameters)
    {
        if (first)
        {
            // Return type
            if (expression)
            {
                t->return_types.push_back(p);
            }
            first = false;
        }
        else
        {
            t->required_types.push_back(p);
            t->expressions.push_back(std::list<Token *>());
        }
    }

    // Get each parameter in turn (working backwards)
    size_t done = 0;
    int i = t->required_types.size() - 1;
    for (auto p : t->required_types)
    {

        // Get expression
        ParseExpression(false, optional_parens, &t->expressions[i--]);

        // Do we have a comma (and need one)?
        done++;
        if (done != t->required_types.size())
        {
            tt = GetToken();
            if (tt->type != TokenType::COMMA)
                Error("Comma expected", tt);
        }
    }

    // Parentheses are optional
    if (optional_parens)
    {
        tt = GetToken();
        if (tt->type != TokenType::RPARENS)
            Error("Closing parentheses expected", tt);
    }

    tokens_out->push_back(t);
}

void Parser::ParseStatement(Token *t, std::list<Token *> *tokens_out)
{
    if (!optimise)
    {
        auto tt = CreateToken(t, TokenType::STATEMENT_START);
        new_tokens.push_back(std::move(tt));
        tokens_out->push_back(&new_tokens.back());
    }

    switch (t->type)
    {
    case TokenType::CASE:
        Parser_CASE(t, tokens_out);
        break;
    case TokenType::END:
        tokens_out->push_back(t);
        break;
    case TokenType::ENDDEF:
        // Ignore here
        PushTokenBack();
        break;
    case TokenType::FOR:
        Parser_FOR(t, tokens_out);
        break;
    case TokenType::IDENTIFIER:
    case TokenType::IDENTIFIER_FLOAT:
    case TokenType::IDENTIFIER_INTEGER:
    case TokenType::IDENTIFIER_STRING:
    case TokenType::IDENTIFIER_TYPE: {
        // Either a PROC call or an assignment, depending on whether there are parentheses or not
        auto tt = GetToken();
        PushTokenBack();
        if (tt->type == TokenType::LPARENS)
        {
            Parser_PROCCall(t, tokens_out);
        }
        else
        {
            ParserAssignment(t, tokens_out, false);
        }
        break;
    }
    case TokenType::IF:
        Parser_IF(t, tokens_out);
        break;
    case TokenType::LET:
        t = GetToken();
        ParserAssignment(t, tokens_out, false);
        break;
    case TokenType::LOCAL:
        if (!inside_function)
            Error("LOCAL only valid within a DEF", t);
        t = GetToken();
        ParserAssignment(t, tokens_out, true);
        break;
    case TokenType::NEXT:
        Parser_NEXT(t, tokens_out);
        break;
    case TokenType::PRINT:
        Parser_PRINT(t, tokens_out);
        break;
    case TokenType::READ:
        Parser_READ(t, tokens_out);
        break;
    case TokenType::REM:
        break;
    case TokenType::REPEAT:
        Parser_REPEAT(t, tokens_out);
        break;
    case TokenType::RESTORE:
        Parser_RESTORE(t, tokens_out);
        break;
    case TokenType::RETURN:
        Parser_RETURN(t, tokens_out);
        break;
    case TokenType::SWAP:
        Parser_SWAP(t, tokens_out);
        break;
    case TokenType::TYPE:
        Parser_TYPE(t, tokens_out);
        break;
    case TokenType::WHILE:
        Parser_WHILE(t, tokens_out);
        break;
    case TokenType::YIELD:
        tokens_out->push_back(t);
        break;

    default: {
        // A generic function?
        auto f = generic_functions.find(t->type);
        if (f != generic_functions.end())
        {
            ParserStatementGeneric(t, tokens_out, &f->second, false);
            break;
        }

        // Not, so error
        Error("Syntax error", t);
    }
    }
}

Token *Parser::GetToken()
{
    Token *t = &*it;
    previous = it;
    it++;
    /*	if (t->line_number == 27 && t->char_number == 10) {
            int a = 1;
        }*/
    return t;
}

void Parser::PushTokenBack()
{
    it = previous;
}

bool Parser::IsStatementTerminator(bool assignment, Token *t)
{
    switch (t->type)
    {
    case TokenType::NEWLINE:
    case TokenType::COLON:
    case TokenType::THEN:
    case TokenType::ELSE:
        return true;
    default:
        return false;
    }
    return false;
}

bool Parser::IsBlockTerminator(bool assignment, Token *t, std::set<TokenType> block_terminator)
{
    return block_terminator.count(t->type) > 0;
}

void Parser::CreateLocalVariable(Token *tt, bool init)
{
    tt->index = local_index++;
    tt->type = TokenType::LOCAL;
    tt->name = std::move(tt->text);
    tt->text = "";
    local_variables.insert(std::make_pair(tt->name, tt));

    // Set default expression to zero out
    if (init)
    {
        auto tn = CreateToken(tt, TokenType::NONE);
        if (tt->vtype.IsInteger())
        {
            tn.type = TokenType::LITERAL_INTEGER;
            tn.integer = 0;
        }
        else if (tt->vtype.IsFloat())
        {
            tn.type = TokenType::LITERAL_FLOAT;
            tn.real = 0;
        }
        else if (tt->vtype.IsString())
        {
            tn.type = TokenType::LITERAL_STRING;
            tn.text = "";
        }
        else
        {
            TypeError(tt);
        }
        new_tokens.push_back(std::move(tn));
        tt->expressions.push_back(std::list<Token *>());
        tt->expressions[0].push_back(&new_tokens.back());
    }
    // CLogger::Get()->Write("Parser", LogNotice, "Create local: %d %d %s", tt->line_number, tt->index,
    // tt->name.c_str());
}

void Parser::CreateGlobalVariable(Token *tt, bool init)
{
    tt->index = global_index--;
    tt->type = TokenType::GLOBAL;
    tt->name = std::move(tt->text);
    tt->text = "";
    global_variables.insert(std::make_pair(tt->name, tt));

    // Set default expression to zero out
    if (init)
    {
        auto tn = CreateToken(tt, TokenType::NONE);
        if (tt->vtype.IsInteger())
        {
            tn.type = TokenType::LITERAL_INTEGER;
            tn.integer = 0;
        }
        else if (tt->vtype.IsFloat())
        {
            tn.type = TokenType::LITERAL_FLOAT;
            tn.real = 0;
        }
        else if (tt->vtype.IsString())
        {
            tn.type = TokenType::LITERAL_STRING;
            tn.text = "";
        }
        else
            TypeError(tt);
        new_tokens.push_back(std::move(tn));
        tt->expressions.push_back(std::list<Token *>());
        tt->expressions[0].push_back(&new_tokens.back());
    }
    //    CLogger::Get()->Write("Parser", LogNotice, "Create global: %d %d %s", tt->line_number, tt->index,
    //    tt->name.c_str());
}

Token *Parser::CreateNewLocalVariableNoInit(Token *t, std::string name, ComplexType type)
{
    Token tt = CreateToken(t, TokenType::LOCAL_NOINIT);
    tt.name = name;
    tt.vtype = type;
    tt.index = local_index++;
    new_tokens.push_back(std::move(tt));
    Token *tg = &new_tokens.back();
    local_variables.insert(std::make_pair(tg->name, tg));
    //    CLogger::Get()->Write("Parser", LogNotice, "New local: %d %d %s", tg->line_number, tg->index,
    //    tg->name.c_str());
    return tg;
}

Token *Parser::CreateNewGlobalVariableNoInit(Token *t, std::string name, ComplexType type)
{
    Token tt = CreateToken(t, TokenType::GLOBAL_NOINIT);
    tt.name = name;
    tt.vtype = type;
    tt.index = global_index--;
    new_tokens.push_back(std::move(tt));
    Token *tg = &new_tokens.back();
    global_variables.insert(std::make_pair(tg->name, tg));
    //    CLogger::Get()->Write("Parser", LogNotice, "New global: %d %d %s", tg->line_number, tg->index,
    //    tg->name.c_str());
    return tg;
}

Token Parser::CreateToken(Token *parent, TokenType t)
{
    Token nc;
    nc.type = t;
    nc.line_number = parent->line_number;
    nc.char_number = parent->char_number;
    nc.file_number = parent->file_number;
    return nc;
}

ComplexType TypeNone()
{
    ComplexType t;
    t.None();
    return t;
}

ComplexType TypeInteger()
{
    ComplexType t;
    t.Integer();
    return t;
}

ComplexType TypeFloat()
{
    ComplexType t;
    t.Float();
    return t;
}

ComplexType TypeString()
{
    ComplexType t;
    t.String();
    return t;
}

ComplexType TypeStruct(Type *type)
{
    ComplexType t;
    t.Struct(type);
    return t;
}

void *Parser::GetAddressForFunc(TokenType tt)
{
    return generic_functions_ptr.find(tt)->second;
}
