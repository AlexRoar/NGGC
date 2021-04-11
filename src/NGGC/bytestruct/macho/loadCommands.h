//
// Created by Александр Дремов on 10.04.2021.
//

#ifndef NGGC_LOADCOMMANDS_H
#define NGGC_LOADCOMMANDS_H

struct segmentCommand64{
    segment_command_64 segment;
};

struct entryPointCommand{
    entry_point_command segment;
    unsigned sectionIndex;
};

struct unixThread{
    thread_command thread;
    uint32_t flavor;
    uint32_t count;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cs;
    uint64_t fs;
    uint64_t gs;
};

struct unixThreadCommand {
    unixThread segment;
    unsigned sectionIndex;
};

struct segmentSection {
    section_64 section;
    size_t offset;

    void binWrite(BinFile *out) {
        BINFILE_WRITE_STRUCT(section);
    }

    static segmentSection code() {
        segmentSection sec = {};
        strcpy(sec.section.segname, SEG_TEXT);
        strcpy(sec.section.sectname, SECT_TEXT);
        sec.section.align = 3;
        sec.section.flags = S_REGULAR | S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS;
        return sec;
    }
};


#endif //NGGC_LOADCOMMANDS_H
