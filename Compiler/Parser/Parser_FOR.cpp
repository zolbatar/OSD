#include "Parser.h"

void Parser::Parser_FOR(Token *t, std::list<Token *> *tokens_out)
{
    //    CLogger::Get()->Write("Parser", LogNotice, "For: %d/%d", t->line_number, inside_function);

    // Get variable
    Token *tt = GetToken();
    t->name = tt->text;

    switch (tt->type)
    {
    case TokenType::IDENTIFIER_INTEGER:
        t->vtype = TypeInteger();
        break;
    case TokenType::IDENTIFIER_FLOAT:
        t->vtype = TypeFloat();
        break;
    case TokenType::IDENTIFIER_STRING:
        TypeError(tt);
    default:
        Error("Identifier with type expected", tt);
    }

    // Find variable
    if (inside_function)
    {
        auto f = local_variables.find(t->name);
        if (f == local_variables.end())
        {
            Token *tt = CreateNewLocalVariableNoInit(t, t->name, t->vtype);
            //            CLogger::Get()->Write("Parser", LogNotice, "1: %d %d", tt->line_number, tt->index);
            t->index = tt->index;
            tokens_out->push_back(tt);
        }
        else
        {
            t->vtype = f->second->vtype;
            t->index = f->second->index;
        }

        // Create variable: iterations
        auto name = t->name + " #iterations";
        f = local_variables.find(name);
        if (f == local_variables.end())
        {
            Token *tt = CreateNewLocalVariableNoInit(t, name, TypeInteger());
            //            CLogger::Get()->Write("Parser", LogNotice, "2: %d %d", tt->line_number, tt->index);
            t->index2 = tt->index;
            tokens_out->push_back(tt);
        }
        else
        {
            t->index2 = f->second->index;
            //            CLogger::Get()->Write("Parser", LogNotice, "2i: %d '%s'", t->index2, f->second->name.c_str());
        }

        // Create variable: step
        name = t->name + " #step";
        f = local_variables.find(name);
        if (f == local_variables.end())
        {
            Token *tt = CreateNewLocalVariableNoInit(t, name, t->vtype);
            t->index3 = tt->index;
            //            CLogger::Get()->Write("Parser", LogNotice, "3: %d %d", tt->line_number, tt->index);
            tokens_out->push_back(tt);
        }
        else
        {
            t->index3 = f->second->index;
            //            CLogger::Get()->Write("Parser", LogNotice, "3i: %d", t->index3);
        }
    }
    else
    {
        auto f = global_variables.find(t->name);
        if (f == global_variables.end())
        {
            Token *tt = CreateNewGlobalVariableNoInit(t, t->name, t->vtype);
            //            CLogger::Get()->Write("Parser", LogNotice, "1g: %d %d", tt->line_number, tt->index);
            t->index = tt->index;
            tokens_out->push_back(tt);
        }
        else
        {
            t->vtype = f->second->vtype;
            t->index = f->second->index;
        }

        // Create variable: iterations
        auto name = t->name + " #iterations";
        f = global_variables.find(name);
        if (f == global_variables.end())
        {
            Token *tt = CreateNewGlobalVariableNoInit(t, name, TypeInteger());
            //            CLogger::Get()->Write("Parser", LogNotice, "2g: %d %d", tt->line_number, tt->index);
            t->index2 = tt->index;
            tokens_out->push_back(tt);
        }
        else
        {
            t->index2 = f->second->index;
        }

        // Create variable: step
        name = t->name + " #step";
        f = global_variables.find(name);
        if (f == global_variables.end())
        {
            Token *tt = CreateNewGlobalVariableNoInit(t, name, t->vtype);
            t->index3 = tt->index;
            //            CLogger::Get()->Write("Parser", LogNotice, "3g: %d %d", tt->line_number, tt->index);
            tokens_out->push_back(tt);
        }
        else
        {
            t->index3 = f->second->index;
        }
    }

    // Next token should be an equal
    auto tt2 = GetToken();
    if (tt2->type != TokenType::EQUAL)
        SyntaxError(tt);

    // Value expression is FROM
    t->expressions.push_back(std::list<Token *>());
    ParseExpression(false, false, &t->expressions[0]);

    // Next token should be a TO
    tt2 = GetToken();
    if (tt2->type != TokenType::TO)
        SyntaxError(tt2);

    // Value expression is TO
    t->expressions.push_back(std::list<Token *>());
    ParseExpression(false, false, &t->expressions[1]);

    // Next token should be STEP or end of statement
    tt2 = GetToken();
    if (tt2->type == TokenType::STEP)
    {
        // Value expression is STEP
        t->expressions.push_back(std::list<Token *>());
        ParseExpression(false, false, &t->expressions[2]);
    }
    else if (tt2->type == TokenType::COLON || tt2->type == TokenType::NEWLINE)
    {
        // Fine, end
    }
    else
        SyntaxError(tt2);
    tokens_out->push_back(t);
    for_loop_variables.push(t);
}

void Parser::Parser_NEXT(Token *t, std::list<Token *> *tokens_out)
{
    auto tt = GetToken();

    if (tt->type == TokenType::COLON || tt->type == TokenType::NEWLINE)
    {
        // This is fine, we don't support named variables here
    }
    else
    {
        Error("NEXT doesn't support named variables", tt);
    }

    // Find last variable
    if (for_loop_variables.empty())
    {
        Error("No FOR loop found", t);
    }
    auto var = for_loop_variables.top();
    for_loop_variables.pop();
    t->index = var->index;
    t->index2 = var->index2;
    t->index3 = var->index3;
    t->vtype = var->vtype;
    tokens_out->push_back(t);
}
