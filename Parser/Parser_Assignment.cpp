#include "Parser.h"

void Parser::FindType(Token *t) {
    auto f = local_variables.find(t->name);
    if (f == local_variables.end()) {
        auto f = global_variables.find(t->name);
        if (f == global_variables.end()) {
            Error("Unknown variable", t);
        } else {
            if (!f->second->vtype.IsType())
                Error("Expected structured type", t);
            t->index = f->second->index;
            t->vtype = TypeStruct(f->second->vtype.GetType());
        }
    } else {
        if (!f->second->vtype.IsType())
            Error("Expected structured type", t);
        t->index = f->second->index;
        t->vtype = TypeStruct(f->second->vtype.GetType());
    }
}

void Parser::FindField(Token *t, Token *tt) {
    tt->name = std::move(tt->text);
    auto ff = t->vtype.GetType()->fields.find(tt->name);
    if (ff == t->vtype.GetType()->fields.end())
        Error("Field not found in structured type", t);

    // Set field type
    switch (tt->type) {
        case TokenType::IDENTIFIER_INTEGER:
            tt->vtype = TypeInteger();
            break;
        case TokenType::IDENTIFIER_FLOAT:
            tt->vtype = TypeFloat();
            break;
        case TokenType::IDENTIFIER_STRING:
            tt->vtype = TypeString();
            break;
        default:
            SyntaxError(tt);
    }
    tt->type = TokenType::FIELD;
    tt->index = ff->second.position;
}

void Parser::FindOrCreateVariable(Token *t, std::list<Token *> *tokens_out, bool local_override) {
    if (inside_function) {
        auto f = local_variables.find(t->name);
        if (f == local_variables.end()) {
            auto f = global_variables.find(t->name);
            if (local_override || f == global_variables.end()) {
                if (t->vtype.IsType())
                    Error("Unknown type", t);

                // Create local implicitly
                Token tt = CreateToken(t, TokenType::LOCAL);
                tt.text = t->name;
                tt.vtype = t->vtype;
                Token *ttt = CreateNewLocalVariableNoInit(tt);
                t->index = ttt->index;
                tokens_out->push_back(ttt);
            } else {
                t->index = f->second->index;
            }
        } else {
            t->index = f->second->index;
        }
    } else {
        auto f = global_variables.find(t->name);
        if (f == global_variables.end()) {
            if (t->vtype.IsType())
                Error("Unknown type", t);

            // Create global implicitly
            Token tt = CreateToken(t, TokenType::GLOBAL);
            tt.text = t->name;
            tt.vtype = t->vtype;
            Token *ttt = CreateNewGlobalVariableNoInit(tt);
            t->index = ttt->index;
            tokens_out->push_back(ttt);
        } else
            t->index = f->second->index;
    }
}

void Parser::ParserAssignment(Token *t, std::list<Token *> *tokens_out, bool local_override) {
    switch (t->type) {
        case TokenType::IDENTIFIER_INTEGER:
            t->vtype = TypeInteger();
            t->name = std::move(t->text);
            break;
        case TokenType::IDENTIFIER_FLOAT:
            t->vtype = TypeFloat();
            t->name = std::move(t->text);
            break;
        case TokenType::IDENTIFIER_STRING:
            t->vtype = TypeString();
            t->name = std::move(t->text);
            break;
        case TokenType::IDENTIFIER_TYPE: {

            // Equals??
            auto tt2 = GetToken();
            if (tt2->type == TokenType::EQUAL) {

                // Type name
                tt2 = GetToken();
                if (tt2->type != TokenType::IDENTIFIER)
                    SyntaxError(tt2);

                // Do we have it?
                auto f = types.find(tt2->text);
                if (f == types.end())
                    Error("Can't find structured type", tt2);

                t->vtype = TypeStruct(&f->second);

                // Parens, any initialisers?
                t->expressions.push_back(std::list<Token *>());
                ParseExpression(false, false, &t->expressions[t->expressions.size() - 1]);

                if (inside_function) {
                    CreateLocalVariable(t, false);
                } else {
                    CreateGlobalVariable(t, false);
                }
                tokens_out->push_back(t);
                return;
            } else {
                t->name = std::move(t->text);
                FindType(t);
                FindField(t, tt2);
                t->stack.push_back(tt2);
            }
            break;
        }
        default:
            Error("Identifier with type expected", t);
    }
    t->type = TokenType::ASSIGNMENT;
    FindOrCreateVariable(t, tokens_out, local_override);

    // Next token is the expression
    t->expressions.push_back(std::list<Token *>());

    // In case we need it for += etc.
    auto nc = CreateToken(t, TokenType::VARIABLE);
    nc.index = t->index;
    nc.vtype = t->vtype;
    nc.name = std::move(t->text);
    auto nc2 = CreateToken(t, TokenType::NONE);

    // Is this a regular equals?
    auto tt = GetToken();
    switch (tt->type) {
        case TokenType::EQUAL:
            ParseExpression(false, false, &t->expressions[0]);
            break;
        case TokenType::ADD_EQUAL:
            new_tokens.push_back(std::move(nc));
            t->expressions[0].push_back(&new_tokens.back());
            ParseExpression(false, false, &t->expressions[0]);
            nc2.type = TokenType::ADD;
            new_tokens.push_back(std::move(nc2));
            t->expressions[0].push_back(&new_tokens.back());
            break;
        case TokenType::SUBTRACT_EQUAL:
            new_tokens.push_back(std::move(nc));
            t->expressions[0].push_back(&new_tokens.back());
            ParseExpression(false, false, &t->expressions[0]);
            nc2.type = TokenType::SUBTRACT;
            new_tokens.push_back(std::move(nc2));
            t->expressions[0].push_back(&new_tokens.back());
            break;
        case TokenType::MULTIPLY_EQUAL:
            new_tokens.push_back(std::move(nc));
            t->expressions[0].push_back(&new_tokens.back());
            ParseExpression(false, false, &t->expressions[0]);
            nc2.type = TokenType::MULTIPLY;
            new_tokens.push_back(std::move(nc2));
            t->expressions[0].push_back(&new_tokens.back());
            break;
        case TokenType::SHIFT_LEFT_EQUAL:
            new_tokens.push_back(std::move(nc));
            t->expressions[0].push_back(&new_tokens.back());
            ParseExpression(false, false, &t->expressions[0]);
            nc2.type = TokenType::SHIFT_LEFT;
            new_tokens.push_back(std::move(nc2));
            t->expressions[0].push_back(&new_tokens.back());
            break;
        case TokenType::SHIFT_RIGHT_EQUAL:
            new_tokens.push_back(std::move(nc));
            t->expressions[0].push_back(&new_tokens.back());
            ParseExpression(false, false, &t->expressions[0]);
            nc2.type = TokenType::SHIFT_RIGHT;
            new_tokens.push_back(std::move(nc2));
            t->expressions[0].push_back(&new_tokens.back());
            break;
        default:
            SyntaxError(tt);
    }

    tokens_out->push_back(t);
}
