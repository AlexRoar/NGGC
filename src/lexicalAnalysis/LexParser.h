//
// Created by Александр Дремов on 09.12.2020.
//

#ifndef NGG_LEXPARSER_H
#define NGG_LEXPARSER_H

#include <cstdlib>
#include <cctype>
#include "helpers/Optional.h"
#include "Lexeme.h"
#include "LexemeType.h"

namespace NGGC {
    class LexParser {
        struct LexAlias{
            const char* alias;
            LexemeType  type;
        };

        constexpr static LexAlias lexAliases[] = {
            #define LEXEME(str, type) {str, Lex_ ## type},
                #include "LexemeType.mpl"
            #undef LEXEME
        };

        static Optional<NGGC::Lexeme> tryParseLex_FDecl(char *&ptr) {
            return parsePhrase(ptr, Lex_FDecl);
        };

        static Optional<NGGC::Lexeme> tryParseLex_LPA(char *&ptr) {
            return parsePhrase(ptr, Lex_LPA);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Input(char *&ptr) {
            return parsePhrase(ptr, Lex_Input);
        };

        static Optional<NGGC::Lexeme> tryParseLex_RPA(char *&ptr) {
            return parsePhrase(ptr, Lex_RPA);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Comma(char *&ptr) {
            return parsePhrase(ptr, Lex_Comma);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Pow(char *&ptr) {
            return parsePhrase(ptr, Lex_Pow);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Eq(char *&ptr) {
            return parsePhrase(ptr, Lex_Eq);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Leq(char *&ptr) {
            return parsePhrase(ptr, Lex_Leq);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Geq(char *&ptr) {
            return parsePhrase(ptr, Lex_Geq);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Neq(char *&ptr) {
            return parsePhrase(ptr, Lex_Neq);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Gr(char *&ptr) {
            return parsePhrase(ptr, Lex_Gr);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Le(char *&ptr) {
            return parsePhrase(ptr, Lex_Le);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Plus(char *&ptr) {
            return parsePhrase(ptr, Lex_Plus);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Minus(char *&ptr) {
            return parsePhrase(ptr, Lex_Minus);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Mul(char *&ptr) {
            return parsePhrase(ptr, Lex_Mul);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Div(char *&ptr) {
            return parsePhrase(ptr, Lex_Div);
        };

        static Optional<NGGC::Lexeme> tryParseLex_BStart(char *&ptr) {
            return parsePhrase(ptr, Lex_BStart);
        };

        static Optional<NGGC::Lexeme> tryParseLex_BEnd(char *&ptr) {
            return parsePhrase(ptr, Lex_BEnd);
        };

        static Optional<NGGC::Lexeme> tryParseLex_StEnd(char *&ptr) {
            return parsePhrase(ptr, Lex_StEnd);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Return(char *&ptr) {
            return parsePhrase(ptr, Lex_Return);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Assg(char *&ptr) {
            return parsePhrase(ptr, Lex_Assg);
        };

        static Optional<NGGC::Lexeme> tryParseLex_AdAssg(char *&ptr) {
            return parsePhrase(ptr, Lex_AdAssg);
        };

        static Optional<NGGC::Lexeme> tryParseLex_MiAssg(char *&ptr) {
            return parsePhrase(ptr, Lex_MiAssg);
        };

        static Optional<NGGC::Lexeme> tryParseLex_MuAssg(char *&ptr) {
            return parsePhrase(ptr, Lex_MuAssg);
        };

        static Optional<NGGC::Lexeme> tryParseLex_DiAssg(char *&ptr) {
            return parsePhrase(ptr, Lex_DiAssg);
        };

        static Optional<NGGC::Lexeme> tryParseLex_VDecl(char *&ptr) {
            return parsePhrase(ptr, Lex_VDecl);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Print(char *&ptr) {
            return parsePhrase(ptr, Lex_Print);
        };

        static Optional<NGGC::Lexeme> tryParseLex_If(char *&ptr) {
            return parsePhrase(ptr, Lex_If);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Else(char *&ptr) {
            return parsePhrase(ptr, Lex_Else);
        };

        static Optional<NGGC::Lexeme> tryParseLex_While(char *&ptr) {
            return parsePhrase(ptr, Lex_While);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Sin(char *&ptr) {
            return parsePhrase(ptr, Lex_Sin);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Cos(char *&ptr) {
            return parsePhrase(ptr, Lex_Cos);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Abs(char *&ptr) {
            return parsePhrase(ptr, Lex_Abs);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Sqrt(char *&ptr) {
            return parsePhrase(ptr, Lex_Sqrt);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Setpix(char *&ptr) {
            return parsePhrase(ptr, Lex_Setpix);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Exp(char *&ptr) {
            return parsePhrase(ptr, Lex_Exp);
        };

        static Optional<NGGC::Lexeme> tryParseLex_Tan(char *&ptr) {
            return parsePhrase(ptr, Lex_Tan);
        };

        static Optional<NGGC::Lexeme> tryParseLex_None(char *&ptr) {
            return parsePhrase(ptr, Lex_None);
        };

        static Optional<Lexeme> tryParseLex_NumberInt(char *&ptr) {
            Optional<Lexeme> retVal {};
            retVal.init();
            skipInvis(ptr);

            int size =   0;
            int number = 0;
            int result = sscanf(ptr, "%d%n", &number, &size);

            if (size == 0 || result <= 0)
                return retVal;

            Lexeme parsed {};

            parsed.init(Lex_NumberInt, number, ptr);
            retVal.init(parsed);
            ptr += size;
            return retVal;
        };


        static Optional<Lexeme> tryParseLex_Number(char *&ptr) {
            Optional<Lexeme> retVal {};
            retVal.init();
            skipInvis(ptr);

            int size = 0;
            double number = 0;
            int result = sscanf(ptr, "%lf%n", &number, &size);

            if (size == 0 || result <= 0)
                return retVal;

            Lexeme parsed {};
            parsed.init(Lex_Number, number, ptr);
            retVal.init(parsed);
            ptr += size;
            return retVal;
        };

        static Optional<NGGC::Lexeme> tryParseLex_Identifier(char *&ptr) {
            Optional<Lexeme> retVal {};
            retVal.init();
            skipInvis(ptr);
            char *initPtr = ptr;

            auto* identifier = StrContainer::New();
            identifier->init("");

            if (!isCharacter(*ptr))
                return retVal;
            identifier->append(*ptr);
            ptr++;

            while (*ptr != '\0') {
                if (isCharacter(*ptr)) {
                    identifier->append(*ptr);
                    ptr++;
                } else {
                    auto number = tryParseLex_NumberInt(ptr);
                    if (!number.hasValue())
                        break;
                    identifier->sEndPrintf("%d", (int) number->getDouble());
                }
            }

            Lexeme parsed {};
            parsed.init(Lex_Identifier, identifier, initPtr);
            retVal.init(parsed);
            return retVal;
        };

        static bool isCharacter(char ptr) {
            return ('a' <= ptr && ptr <= 'z') || ('A' <= ptr && ptr <= 'Z') || ptr == '_';
        }

        static bool expectPhrase(char *&ptr, const char *cmp) {
            if (ptr == strstr(ptr, cmp)) {
                ptr += strlen(cmp);
                return true;
            }
            return false;
        }

        static void getOffsets(const char *offset, const char *start, size_t &line, size_t &col) {
            if (offset == nullptr || start == nullptr)
                return;
            col = 0;
            line = 0;
            while (start <= offset) {
                if (*start == '\n') {
                    line++;
                    col = 0;
                }
                col++;
                start++;
            }
        }

        static bool isDigit(char ptr) {
            return ('0' <= ptr && ptr <= '9');
        }

        static void skipInvis(char *&ptr) {
            while (isspace(*ptr)) ptr++;
        }

        static Optional<Lexeme> parsePhrase(char *&ptr, LexemeType type) {
            Optional<Lexeme> retVal{}; retVal.init();
            skipInvis(ptr);
            auto alias = findAlias(type);
            auto phr = alias.alias;
            char* offset = ptr;
            if (!expectPhrase(ptr, phr))
                return retVal;
            Lexeme parsed{}; parsed.init(type, offset);\
            retVal.init(parsed);\
            return retVal;
        }

    public:
        static LexAlias findAlias(LexemeType type){
            for (auto lexAliase : lexAliases) {
                if (lexAliase.type == type)
                    return lexAliase;
            }
            return lexAliases[0];
        }

        static FastList<NGGC::Lexeme>* parse(StrContainer *content) {
            auto* result = FastList<NGGC::Lexeme>::New();

            char *ptr = content->begin();

            #define CHECK_SUCCESS if (parsed.hasValue()){result->pushBack(parsed.unwrap()); continue;}
            #define TRY_PARSE(type) parsed = tryParse ## type(ptr); CHECK_SUCCESS

            for (; ptr < content->end();) {
                Optional<NGGC::Lexeme> parsed {};
                parsed.init();

                #define LEXEME(str, l) TRY_PARSE(Lex_ ## l);
                    #include "LexemeType.mpl"
                #undef LEXEME

                printf("Undefined sequence at \"%s\"\n", ptr);
            }

            #undef CHECK_SUCCESS
            #undef TRY_PARSE

            char* start = content->begin();
            for(size_t it = result->begin(); it != 0; it = result->nextIterator(it)) {
                size_t col = 0, line = 0;

                Lexeme tmp{}; tmp.init();
                result->get(it, &tmp);
                getOffsets(tmp.getCodeOffset(), start, line, col);
                tmp.setLineCol(line, col);
                result->set(it, tmp);
            }

            return result;
        }

        static void dumpLexemes(FastList<NGGC::Lexeme> &list, FILE *output = stdout) {
            size_t pos = list.begin();
            size_t curLine = 0;
            for (; pos != 0; list.nextIterator(&pos)) {
                NGGC::Lexeme cur {};
                list.get(pos, &cur);
                if (cur.getLine() > curLine){
                    curLine = cur.getLine();
                    fprintf(output, "\n");
                }

                fprintf(output, "%s", lexemeTypeToString(cur.getType()));
                if (cur.isStringUsed()) {
                    fprintf(output, "<%s>", cur.getString()->begin());
                } else {
                    if (cur.getDouble() == 0) {
                        fprintf(output, "<0>");
                    } else {
                        fprintf(output, "<%lg>", cur.getDouble());
                    }

                }
                fprintf(output, " ");
            }
        }
    };
}

#endif //NGG_LEXPARSER_H
