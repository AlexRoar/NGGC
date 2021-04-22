#ifndef NGG_NGG_H
#define NGG_NGG_H

#include <MachOBuilder.h>
#include "VarTable.h"
#include "helpers/ParamsParser.h"
#include "compiler/CompileError.h"
#include "ASTLoader/ASTLoader.h"
#include "lexicalAnalysis/LexParser.h"
#include "helpers/ByteContainer.h"
#include "../src/core/bgen/FastStackController.h"
#include "core/eloquent/ASMStructure/ElCommand.h"

namespace NGGC {
    struct offset{
        size_t value;
        void dest(){}
        offset(size_t val): value(val) {}
        offset(int val): value(val) {}

        explicit operator size_t(){
            return value;
        }
    };
    struct functionDefinition {
        ClassicStack <offset> usages;
        size_t definitionOffset;

        void init() {
            const int startSize = 8;
            usages.init(startSize);
            definitionOffset = 0;
        }

        void dest() {
            usages.dest();
        }
    };

    class NGGCompiler {
        HashMasm<ClassicStack < size_t>> globalVars;
        HashMasm<functionDefinition> functions;
        ClassicStack <CompileError> *cErrors;
        FastStackController stack;
        FastList<Lexeme> *parsed;
        ByteContainer *compiled;
        size_t lastDescription;
        const char* fileName;
        StrContainer listing;
        VarTable table;
        AST tree;

        void addInstructions(const unsigned *arr, const char* description) {
            addDescription(description);
            while (*arr != COMMANDEND) {
                compiled->append((char) *arr);
                arr++;
            }
        }

        void addInstructions(const unsigned *arr) {
            while (*arr != COMMANDEND) {
                compiled->append((char) *arr);
                arr++;
            }
        }

        void addDescription(const char *msg) {
            printAddedBytes();
            listing.sEndPrintf("%05d | %s\n", compiled->getLen(), msg);
        }

        void addDescription(const char *msg, const char *additional) {
            printAddedBytes();
            listing.sEndPrintf("%05d | %s %s\n", compiled->getLen(), msg, additional);
        }

        void printAddedBytes() {
            if (lastDescription == compiled->getLen())
                return;
            listing.sEndPrintf("%05d |    ", lastDescription);
            for (; lastDescription < compiled->getLen(); lastDescription++)
                listing.sEndPrintf("0x%x ",  ((unsigned char *) compiled->getStorage())[lastDescription]);
            listing.sEndPrintf("\n");
        }

        void printImm32(int32_t offset) {
            compiled->append((const char*)&offset, sizeof(offset));
        }

        void printImm8(int8_t offset) {
            compiled->append((const char*)&offset, sizeof(offset));
        }

        void storeRaxToOffsetRbp(int32_t offset){
            offset *= -1;
            static const unsigned movrbx[] = {
                    MOV_MEM_RBP_DISPL32RAX,
                    COMMANDEND};
            addInstructions(movrbx, "Storing rax to rbp - offset");
            printImm32(offset);
        }

        void storeRaxToOffsetRsp(int32_t offset){
            offset *= -1;
            static const unsigned movrbx[] = {
                    MOV_MEM_RSP_DISPL32RAX,
                    COMMANDEND};
            addInstructions(movrbx, "Storing rax to rsp - offset");
            printImm32(offset);
        }

        void leave(){
            static const unsigned leave[] = {POP_RBP, RET, COMMANDEND};
            addInstructions(leave, "Leave");
        }

        void processFurther(ASTNode *head, bool valueNeeded = false, bool noScope = false) {
            if (head == nullptr)
                return;
            switch (head->getKind()) {
                case Kind_Linker:
                    c_Linker(head, valueNeeded, noScope);
                    break;
                case Kind_FuncDecl:
                    c_FuncDecl(head);
                    break;
                case Kind_Identifier:
                    c_Identifier(head);
                    break;
                case Kind_Number:
                    c_Number(head);
                    break;
                case Kind_AssignExpr:
                    c_AssignExpr(head);
                    break;
                case Kind_VarDef:
                    c_VarDef(head);
                    break;
                case Kind_MaUnOperator:
                    c_MaUnOperator(head);
                    break;
                case Kind_MaOperator:
                    c_MaOperator(head);
                    break;
                case Kind_Statement:
                    c_Statement(head);
                    break;
                case Kind_FuncCall:
                    c_FuncCall(head, valueNeeded);
                    break;
                case Kind_CmpOperator:
                    c_CmpOperator(head);
                    break;
                case Kind_ReturnStmt:
                    c_ReturnStmt(head);
                    break;
                case Kind_Print:
                    c_Print(head);
                    break;
                case Kind_IfStmt:
                    c_IfStmt(head);
                    break;
                case Kind_Input:
                    c_Input(head);
                    break;
                case Kind_None:
                    c_None();
                    break;
                case Kind_WhileStmt:
                    c_WhileStmt(head);
                    break;
                case Kind_BasicFunction:
                    c_BasicFunction(head, valueNeeded);
                    break;
                case Kind_Setpix:
                    c_Setpix(head);
                    break;
                default: {
                    CompileError err {};
                    err.init("Undefined sequence: ", head->getLexeme());
                    err.msg->sEndPrintf("%s", ASTNodeKindToString(head->getKind()));
                    cErrors->push(err);
                    break;
                }
            }
        }

        void c_Linker(ASTNode *head, bool valueNeeded, bool noScope = false) {
            bool genNewScope = (head->getLinkKind() == Kind_Link_NewScope) && !noScope;

            if (genNewScope) {
                table.addNewLevel();
                addDescription("New scope created");
            }
            processFurther(head->getLeft(), valueNeeded);
            if (head->getRight())
                processFurther(head->getRight(), valueNeeded);
            if (genNewScope) {
                table.deleteLocal();
                addDescription("Exited scope");
            }
        }

        void c_FuncDecl(ASTNode *head) {
            StrContainer label = {};
            label.init(head->getLexeme().getString()->begin());
            size_t argCount = 0;
            ASTNode *arg = head->getLeft();
            while (arg != nullptr && arg->getKind() != Kind_None) {
                argCount++;
                arg = arg->getRight();
            }
            label.sEndPrintf("%zu", argCount);

            if (functions.find(label.getStorage()) != functions.end()) {
                if (functions[label.begin()].definitionOffset != (size_t)-1) {
                    CompileError err {};
                    err.init("Duplicate function declaration", head->getLexeme());
                    err.msg->sEndPrintf("%s", label.getStorage());
                    cErrors->push(err);
                    label.dest();
                    return;
                } else
                    functions[label.begin()].definitionOffset = compiled->getLen();
            } else {
                functions[label.begin()].init();
                functions[label.begin()].definitionOffset = compiled->getLen();
            }

            addDescription("Function declaration start:", label.getStorage());
            table.addNewLevel();
            stack.clear();
            if (head->getLeft() != nullptr && head->getLeft()->getKind() != Kind_None) {
                ASTNode *cur = head->getLeft();
                while (cur != nullptr && cur->getKind() != Kind_None && cur->getLeft() != nullptr) {
                    Lexeme name = cur->getLeft()->getLexeme();
                    table.def(name.getString());
                    cur = cur->getRight();
                }
            }
            static const unsigned moverbp[] = {PUSH_RBP, MOV_RBPRSP, COMMANDEND};
            addInstructions(moverbp, "Function enter");
            processFurther(head->getRight(), false, true);
            leave();
            table.deleteLocal();
            addDescription("Function declaration end");
            label.dest();
        }

        void c_Identifier(ASTNode *head) {
            StrContainer *label = head->getLexeme().getString();
            Optional <VarSingle> found = table.get(label);
            if (!found.hasValue()) {
                CompileError err {};
                err.init("Identifier is not defined: ", head->getLexeme());
                err.msg->sEndPrintf("%s", label->begin());
                cErrors->push(err);
                return;
            }
            addDescription("Load variable from memory:", label->begin());
            static const unsigned movOperation[] = {MOV_RAXRBP_MEM_DISPL32, COMMANDEND};
            addInstructions(movOperation);
            int32_t offset = found->rbpOffset * -1;
            printImm32(offset);
        }

        void c_Number(ASTNode *head) {
            static const unsigned movOperation[] = {MOV_RAXIMM32, COMMANDEND};
            addInstructions(movOperation, "Load number");
            printImm32((int)head->getLexeme().getDouble());
        }

        void c_Setpix(ASTNode *head) {
        }

        void c_MaOperator(ASTNode *head) {
            auto type = head->getLexeme().getType();
            addDescription("Processing math operation:", lexemeTypeToString(type));
            processFurther(head->getLeft(), true);
            addDescription("Processed left of math operator, saving");
            stack.push(*compiled, REG_RAX);
            processFurther(head->getRight(), true);
            stack.pop(*compiled, REG_RBX);
            addDescription("Processed right of math operator,results in rax and rbx now");

            switch (type) {
                case Lex_Plus: {
                    static const unsigned processed[] = {ADD_RAXRBX, COMMANDEND};
                    addInstructions(processed, "add rax, rbx");
                    break;
                }
                case Lex_Minus: {
                    static const unsigned processed[] = {SUB_RAXRBX, COMMANDEND};
                    addInstructions(processed, "sub rax, rbx");
                    break;
                }
                case Lex_Mul:{
                    static const unsigned processed[] = {IMUL_RAXRBX, COMMANDEND};
                    addInstructions(processed, "imul rax, rbx");
                    break;
                }
                case Lex_Div:{
                    static const unsigned processed[] = {
                            XOR_RDXRDX,
                            IDIV_RBX,
                            COMMANDEND};
                    addInstructions(processed, "idiv rax, rbx");
                    break;
                }
                case Lex_Pow:
                default: {
                    CompileError err {};
                    err.init("Unknown operation in c_MaOperator: ", head->getLexeme());
                    err.msg->sEndPrintf("%s", lexemeTypeToString(type));
                    cErrors->push(err);
                    return;
                }
            }
        }

        void c_AssignExpr(ASTNode *head) {
            auto type = head->getLexeme().getType();

            ASTNode *idNode = head->getLeft();
            ASTNode *valNode = head->getRight();

            StrContainer *name = idNode->getLexeme().getString();
            Optional<VarSingle> found = table.get(name);

            if (!found.hasValue()) {
                CompileError err {};
                err.init("Identifier was not declared", head->getLexeme());
                err.msg->sEndPrintf("%s", idNode->getLexeme().getString()->begin());
                cErrors->push(err);
                return;
            }
            addDescription("Processing assignment type node. Evaluating rvalue");
            processFurther(valNode, true);
            static const unsigned movrbx[] = {MOV_RAXRBX, COMMANDEND};
            addInstructions(movrbx, "mov rax, rbx");
            addDescription("Evaluated. Modifying stored value");
            switch (type) {
                case Lex_AdAssg: {
                    processFurther(idNode, true);
                    static const unsigned processed[] = {
                            ADD_RAXRBX,
                            COMMANDEND
                    };
                    addInstructions(processed, "add rax, rbx - += operation");
                    storeRaxToOffsetRbp(found->rbpOffset);
                    break;
                }
                case Lex_MiAssg:{
                    processFurther(idNode, true);
                    static const unsigned processed[] = {
                            SUB_RAXRBX,
                            COMMANDEND
                    };
                    addInstructions(processed, "sub rax, rbx - -= operation");
                    storeRaxToOffsetRbp(found->rbpOffset);
                    break;
                }
                case Lex_MuAssg:{
                    processFurther(idNode, true);
                    static const unsigned processed[] = {
                            IMUL_RAXRBX,
                            COMMANDEND
                    };
                    addInstructions(processed, "imul rax, rbx - *= operation");
                    storeRaxToOffsetRbp(found->rbpOffset);
                    break;
                }
                case Lex_DiAssg: {
                    processFurther(idNode, true);
                    static const unsigned processed[] = {
                            XOR_RDXRDX,
                            IDIV_RBX,
                            COMMANDEND
                    };
                    addInstructions(processed, "idiv rax, rbx - /= operation");
                    storeRaxToOffsetRbp(found->rbpOffset);
                    break;
                }
                case Lex_Assg: {
                    static const unsigned processed[] = {
                            MOV_RBXRAX,
                            COMMANDEND
                    };
                    addInstructions(processed, "mov rbx, rax - = operation");
                    storeRaxToOffsetRbp(found->rbpOffset);
                    break;
                }
                default: {
                    CompileError err {};
                    err.init("Unknown operation in c_AssignExpr: ", head->getLexeme());
                    err.msg->sEndPrintf("%s", lexemeTypeToString(type));
                    cErrors->push(err);
                    return;
                }
            }
        }

        void c_VarDef(ASTNode *head) {
            auto type = head->getLexeme();
            bool res = table.def(type.getString());
            if (!res) {
                CompileError err {};
                err.init("Can't redeclare variable: ", head->getLexeme());
                err.msg->sEndPrintf("%s", head->getLexeme().getString()->begin());
                cErrors->push(err);
                return;
            }

            if (head->getLeft() == nullptr || (head->getLeft() != nullptr && head->getLeft()->getKind() == Kind_None)){
                return;
            }

            processFurther(head->getLeft(), true);
            Optional<VarSingle> found = table.get(type.getString());
            storeRaxToOffsetRbp(found->rbpOffset);
        }

        void c_MaUnOperator(ASTNode *head) {
            auto type = head->getLexeme().getType();
            processFurther(head->getLeft(), true);

            if (type == Lex_Minus) {
                static const unsigned processed[] = {
                        IMUL_RAXIMM32,
                        COMMANDEND
                };
                addInstructions(processed, "unary * (-1)");
                printImm32(-1);
            }
        }

        void c_Statement(ASTNode *head) {
            processFurther(head->getLeft());
            processFurther(head->getRight());
        }

        void c_FuncCall(ASTNode *head, bool valueNeeded = false) {
            ASTNode *lastNode = head->getLeft();

            if (table.getLocalOffset() != 0) {
                static const unsigned moversp[] = {SUB_RSPIMM32, COMMANDEND};
                addInstructions(moversp, "sub rsp, imm32 - moving rsp before function call");
                printImm32(table.getLocalOffset());
            }

            int argOffset = 1; // return address
            while (lastNode != nullptr && lastNode->getKind() != Kind_None) {
                processFurther(lastNode->getLeft(), true);
                static const unsigned movearg[] = {MOV_MEM_RSP_DISPL8RAX, COMMANDEND};
                addInstructions(movearg, "preparing argument");
                printImm8(argOffset * -1);
                lastNode = lastNode->getRight();
                argOffset++;
            }

            StrContainer label {};
            label.init(head->getLexeme().getString()->begin());
            label.sEndPrintf("%d", argOffset - 1);

            call(label);
        }

        void call(const StrContainer &label) {
            stack.saveStack(*compiled);

            auto foundFunc = functions.find(label.begin());
            if (foundFunc == functions.end()){
                functions[label.begin()].init();
                functions[label.begin()].definitionOffset = -1;
                foundFunc = functions.find(label.begin());
            }
            foundFunc->value.usages.push(compiled->getLen() + 1);
            static const unsigned call[] = {CALL_REL, COMMANDEND};
            addInstructions(call, "call");
            printImm32(0);

            stack.restoreStack(*compiled);
        }

        void c_CmpOperator(ASTNode *head) {
        }

        void c_ReturnStmt(ASTNode *head) {
            leave();
            static const unsigned ret[] = {RET, COMMANDEND};
            addInstructions(ret, "return");
        }

        void c_Print(ASTNode *head) {
            processFurther(head->getLeft(), true);
            static const unsigned prepareArgs[] = {MOV_RDIRAX, COMMANDEND};
            addInstructions(prepareArgs, "preparing SystemV args");
            call("__Z5printi");
        }

        void c_Input(ASTNode *head) {
            call("__Z2inv");
        }

        void c_IfStmt(ASTNode *head) {
        }

        void c_None() {
        }

        void c_WhileStmt(ASTNode *head) {
        }

        void c_BasicFunction(ASTNode *head, bool valueNeeded = false) {
        }

    public:
        void init() {
            parsed = FastList<Lexeme>::New();
            compiled = ByteContainer::New();
            cErrors = ClassicStack<CompileError>::New();
            functions.init();
            globalVars.init();
            listing.init();
            table.init();
            stack.init();
            lastDescription = 0;
        }

        void init(ASTNode *head) {
            init();
            tree.dest();
            tree.init(head);
        }

        void dest() {
            ClassicStack<CompileError>::Delete(cErrors);
            ByteContainer::Delete(compiled);
            functions.dest();
            globalVars.dest();
            parsed->Delete();
            listing.dest();
            table.dest();
            stack.dest();
            tree.dest();
        }

        static NGGCompiler *New() {
            auto *ob = static_cast<NGGCompiler *>(calloc(1, sizeof(NGGCompiler)));
            ob->init();
            return ob;
        }

        static void Delete(NGGCompiler *ob) {
            ob->dest();
            free(ob);
        }

        void compile() {
            processFurther(tree.getHead());
            printAddedBytes();
        }

        bool loadFile(const char *filePath) {
            StrContainer content {};
            content.init();
            fileName = filePath;

            FILE *sourceCode = fopen(filePath, "rb");
            if (!sourceCode) {
                return false;
            }

            content.readFromFile(sourceCode);
            fclose(sourceCode);

            auto *res = NGGC::LexParser::parse(&content);

            auto ASTParser = NGGC::AST {};
            ASTParser.init();
            ASTParser.parse(res);
            this->tree = ASTParser;
            this->parsed = res;
            content.dest();
            return true;
        }

        void printLexemes(const char *lexfileName) {
            FILE *file = fopen(lexfileName, "w");
            LexParser::dumpLexemes(*parsed, file);
            fclose(file);
        }

        bool isParseSuccessful() {
            return !tree.hasError();
        }

        bool isCompileSuccessful() {
            return cErrors->isEmpty();
        }

        void dumpCompileErrorStack(const char *inputFileName) {
            for (unsigned i = 0; i < cErrors->getSize(); ++i) {
                const CompileError &err = cErrors->get(i);
                printf("%s:%zu:%zu: error: %s\n", inputFileName, err.line + 1, err.col, err.msg->begin());
            }
        }

        void dumpErrorStack(const char *inputFileName) {
            tree.dumpParseErrorStack(inputFileName);
        };

        void dumpGraph() {
            FILE *file = fopen("graph.gv", "wb");
            tree.dumpTree(file);
            fclose(file);
            system("dot -Tsvg graph.gv -o code.svg");
            system("dot -Tpng graph.gv -o code.png && rm graph.gv");
        }

        void dumpTree() {
            FILE *file = fopen("treestruct.txt", "w");
            ASTLoader::dump(tree.getHead(), file);
        }

        const StrContainer& getListing() {
            return listing;
        }

        void genObject(const char* file){
            FILE *res = fopen(file, "wb");
            binaryFile binary = {};
            binary.init(res);

            ObjectMachOGen mgen = {};
            mgen.init();

            unsigned char data[] = {
                    0xDE, 0xD3, 0x2D, 0xED, 0x32, 0xDE, 0xD3, 0x2D, 0xED, 0x32,
                    0xDE, 0xD3, 0x2D, 0xED, 0x32, 0xDE, 0xD3, 0x2D, 0xED, 0x32,
            };

            mgen.addCode(compiled->begin(), compiled->getLen());
            mgen.addData(data, sizeof(data));

            for(auto& elem: functions){
                StrContainer label = {};
                label.init("_");
                label.sEndPrintf("%s", elem.key);
                if(elem.value.definitionOffset != size_t (-1)){
                    mgen.addInternalCodeSymbol(label.begin(), elem.value.definitionOffset);
                    for (auto& usage: elem.value.usages)
                        mgen.bindSignedOffset(label.begin(), usage.value);
                } else {
                    for (auto& usage: elem.value.usages)
                        mgen.bindBranchExt(elem.key, usage.value);
                }
                label.dest();
            }

            mgen.dumpFile(binary);

            mgen.dest();
            binary.dest();
        }
    };
}

#endif //NGG_NGG_H