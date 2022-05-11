#include "Tokeniser.h"

#ifndef CLION
#include <circle/timer.h>
extern CTimer *timer;
#endif

void Tokeniser::PrintTokensPtr(std::list<Token *> *tokens, int depth, std::list<std::string> *output) {
    for (auto tk = tokens->begin(); tk != tokens->end(); ++tk) {
        auto token = *tk;

        // Spacing
        std::string spacing;
        for (int i = 0; i < depth; i++) {
            spacing += " ";
        }

        // Header
        char l[256];
        sprintf(l, "%sLine %4d:%3d [%3d]: ", spacing.c_str(), token->line_number, token->char_number,
                (int) token->type);

        // Token stuff
        char m[256];
        switch (token->type) {
            case TokenType::NONE:
                sprintf(m, "None/End of tokens");
                break;
            case TokenType::NEWLINE:
                sprintf(m, "Newline");
                break;
            case TokenType::STATEMENT_START:
                sprintf(m, "Statement start");
                break;
            case TokenType::SUB_EXPRESSION:
                sprintf(m, "Sub-expression");
                break;
            case TokenType::LINE_NUMBER:
                sprintf(m, "Line No.");
                break;
            case TokenType::VARIABLE:
                sprintf(m, "Identifier '%s'/%lld", token->name.c_str(), token->index);
                break;
            case TokenType::ASSIGNMENT:
                if (token->vtype.IsInteger())
                    sprintf(m, "Integer assignment '%s', index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsFloat())
                    sprintf(m, "Float assignment '%s', index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsString())
                    sprintf(m, "String assignment '%s', index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsType()) {
                    sprintf(m, "Type assignment '%s' (type %s), index %lld", token->name.c_str(),
                            token->vtype.GetType()->name.c_str(),
                            token->index);
                }
                break;
            case TokenType::FIELD:
                if (token->vtype.IsInteger())
                    sprintf(m, "Integer field '%s', offset %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsFloat())
                    sprintf(m, "Float field '%s', offset %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsString())
                    sprintf(m, "String field '%s', offset %lld", token->name.c_str(), token->index);
                break;
            case TokenType::FOR:
                if (token->vtype.IsInteger())
                    sprintf(m, "FOR, integer '%s'/%lld", token->name.c_str(), token->index);
                else if (token->vtype.IsFloat())
                    sprintf(m, "FOR, float '%s'/%lld", token->name.c_str(), token->index);
                break;
            case TokenType::NEXT:
                sprintf(m, "NEXT %lld", token->index);
                break;
            case TokenType::CONST:
                if (token->vtype.IsInteger())
                    sprintf(m, "Integer constant '%s'=%lld", token->name.c_str(), token->integer);
                else if (token->vtype.IsFloat())
                    sprintf(m, "Float constant '%s'=%f", token->name.c_str(), token->real);
                if (token->vtype.IsString())
                    sprintf(m, "String constant '%s'='%s'", token->name.c_str(), token->text.c_str());
                break;
            case TokenType::DEF:
                sprintf(m, "DEF '%s'", token->text.c_str());
                break;
            case TokenType::CONSTANT:
                sprintf(m, "Constant '%s'", token->name.c_str());
                break;
            case TokenType::LITERAL_INTEGER:
                sprintf(m, "Integer %lld", token->integer);
                break;
            case TokenType::LITERAL_FLOAT:
                sprintf(m, "Float %f", token->real);
                break;
            case TokenType::LITERAL_STRING:
                sprintf(m, "String '%s'", token->text.c_str());
                break;
            case TokenType::PROC_CALL: {
                auto top = token->return_types.front();
                if (top.IsNone()) {
                    sprintf(m, "PROC call [%s], no return", token->text.c_str());
                } else if (top.IsInteger()) {
                    sprintf(m, "PROC call [%s], integer return", token->text.c_str());
                } else if (top.IsFloat()) {
                    sprintf(m, "PROC call [%s], float return", token->text.c_str());
                } else if (top.IsString()) {
                    sprintf(m, "PROC call [%s], string return", token->text.c_str());
                }
                break;
            }
            case TokenType::PROC_CALL_EXPR: {
                auto top = token->return_types.front();
                if (top.IsNone()) {
                    sprintf(m, "PROC call (expression) [%s], no return", token->text.c_str());
                } else if (top.IsInteger()) {
                    sprintf(m, "PROC call (expression) [%s], integer return", token->text.c_str());
                } else if (top.IsFloat()) {
                    sprintf(m, "PROC call (expression) [%s], float return", token->text.c_str());
                } else if (top.IsString()) {
                    sprintf(m, "PROC call (expression) [%s], string return", token->text.c_str());
                }
                break;
            }
            case TokenType::LOCAL:
                if (token->vtype.IsInteger())
                    sprintf(m, "LOCAL '%s' Integer, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsFloat())
                    sprintf(m, "LOCAL '%s' Float, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsString())
                    sprintf(m, "LOCAL '%s' String, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsType()) {
                    sprintf(m, "LOCAL '%s' Type (%s), index %lld", token->name.c_str(),
                    		token->vtype.GetType()->name.c_str(),
                            token->index);
                }
                break;
            case TokenType::GLOBAL:
                if (token->vtype.IsInteger())
                    sprintf(m, "GLOBAL '%s' Integer, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsFloat())
                    sprintf(m, "GLOBAL '%s' Float, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsString())
                    sprintf(m, "GLOBAL '%s' String, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsType()) {
                    sprintf(m, "GLOBAL '%s' Type (%s), index %lld", token->name.c_str(),
                    		token->vtype.GetType()->name.c_str(),
                            token->index);
                }
                break;
            case TokenType::LOCAL_NOINIT:
                if (token->vtype.IsInteger())
                    sprintf(m, "LOCAL-NOINIT '%s' Integer, index %lld", token->name.c_str(), token->index);
                if (token->vtype.IsFloat())
                    sprintf(m, "LOCAL-NOINIT '%s' Float, index %lld", token->name.c_str(), token->index);
                if (token->vtype.IsString())
                    sprintf(m, "LOCAL-NOINIT '%s' String, index %lld", token->name.c_str(), token->index);
                break;
            case TokenType::GLOBAL_NOINIT:
                if (token->vtype.IsInteger())
                    sprintf(m, "GLOBAL-NOINIT '%s' Integer, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsFloat())
                    sprintf(m, "GLOBAL-NOINIT '%s' Float, index %lld", token->name.c_str(), token->index);
                else if (token->vtype.IsString())
                    sprintf(m, "GLOBAL-NOINIT '%s' String, index %lld", token->name.c_str(), token->index);
                break;
            case TokenType::PRINTNL:
                sprintf(m, "PRINT newline");
                break;
            case TokenType::PRINTTABBEDOFF:
                sprintf(m, "PRINT tabbed mode off");
                break;
            case TokenType::PRINTTABBED:
                sprintf(m, "PRINT tabbed mode on");
                break;
            case TokenType::RETURN: {
                auto top = token->return_types.front();
                if (top.IsNone()) {
                    sprintf(m, "RETURN (no value)");
                } else if (top.IsInteger()) {
                    sprintf(m, "RETURN integer");
                } else if (top.IsFloat()) {
                    sprintf(m, "RETURN float");
                } else if (top.IsString()) {
                    sprintf(m, "RETURN string");
                }
                break;
            }
            default:
                if (token->type < TokenType::NONE) {
                    // ASCII one
                    sprintf(m, "%c", (char) token->type);
                } else {
                    auto kw = keyword_lookup.find(token->type);
                    if (kw != keyword_lookup.end()) {
                        sprintf(m, "%s", kw->second.name.c_str());
                    } else {
                        sprintf(m, "Unknown: %d", (int) token->type);
                    }
                }
                break;
        }
        output->push_back(std::string(l) + std::string(m));

        // Do we have any expected types?
        if (!token->required_types.empty()) {
            for (auto &e: token->required_types) {
                if (e.IsInteger())
                    output->push_back(spacing + "  Required type: integer");
                else if (e.IsFloat())
                    output->push_back(spacing + "  Required type: float");
                else if (e.IsString())
                    output->push_back(spacing + "  Required type: string");
            }
        }

        // Do we have any expressions?
        if (!token->expressions.empty()) {
            int i = 0;
            for (auto &e: token->expressions) {
                output->push_back(spacing + "  Expression " + std::to_string(i) + ":");
                PrintTokensPtr(&e, depth + 4, output);
                i++;
            }
        }

        // Do we have any branches?
        if (!token->branches.empty()) {
            int i = 0;
            for (auto &b: token->branches) {
                output->push_back(spacing + "  Branch " + std::to_string(i) + ":");
                PrintTokensPtr(&b, depth + 4, output);
                i++;
            }
        }

        // Do we have any sub-tokens, such as an expression?
        if (!token->stack.empty()) {
            output->push_back(spacing + "  Stack:");
            PrintTokensPtr(&token->stack, depth + 4, output);
        }
    }

}