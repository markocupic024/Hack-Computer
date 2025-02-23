#include "code.h"
#include <stdio.h>

const char *code_dest(char *dest)
{
    if (!dest)
        return NULL;

    if (!strcmp(dest, "null"))
        return DEST_NULL;
    if (!strcmp(dest, "M"))
        return DEST_M;
    if (!strcmp(dest, "D"))
        return DEST_D;
    if (!strcmp(dest, "MD"))
        return DEST_MD;
    if (!strcmp(dest, "A"))
        return DEST_A;
    if (!strcmp(dest, "AM"))
        return DEST_AM;
    if (!strcmp(dest, "AD"))
        return DEST_AD;
    if (!strcmp(dest, "AMD"))
        return DEST_AMD;

    return DEST_NULL;
}

const char *code_comp(char *comp)
{
    if (!comp)
        return NULL;

    if (!strcmp(comp, "0"))
        return COMP_ZERO;
    if (!strcmp(comp, "1"))
        return COMP_ONE;
    if (!strcmp(comp, "-1"))
        return COMP_MINUS_ONE;
    if (!strcmp(comp, "D"))
        return COMP_D;
    if (!strcmp(comp, "A"))
        return COMP_A;
    if (!strcmp(comp, "M"))
        return COMP_M;
    if (!strcmp(comp, "!D"))
        return COMP_NOT_D;
    if (!strcmp(comp, "!A"))
        return COMP_NOT_A;
    if (!strcmp(comp, "!M"))
        return COMP_NOT_M;
    if (!strcmp(comp, "-D"))
        return COMP_MINUS_D;
    if (!strcmp(comp, "-A"))
        return COMP_MINUS_A;
    if (!strcmp(comp, "-M"))
        return COMP_MINUS_M;
    if (!strcmp(comp, "D+1"))
        return COMP_D_PLUS_ONE;
    if (!strcmp(comp, "A+1"))
        return COMP_A_PLUS_ONE;
    if (!strcmp(comp, "M+1"))
        return COMP_M_PLUS_ONE;
    if (!strcmp(comp, "D-1"))
        return COMP_D_MINUS_ONE;
    if (!strcmp(comp, "A-1"))
        return COMP_A_MINUS_ONE;
    if (!strcmp(comp, "M-1"))
        return COMP_M_MINUS_ONE;
    if (!strcmp(comp, "D+A"))
        return COMP_D_PLUS_A;
    if (!strcmp(comp, "D+M"))
        return COMP_D_PLUS_M;
    if (!strcmp(comp, "D-A"))
        return COMP_D_MINUS_A;
    if (!strcmp(comp, "D-M"))
        return COMP_D_MINUS_M;
    if (!strcmp(comp, "A-D"))
        return COMP_A_MINUS_D;
    if (!strcmp(comp, "M-D"))
        return COMP_M_MINUS_D;
    if (!strcmp(comp, "D&A"))
        return COMP_D_AND_A;
    if (!strcmp(comp, "D&M"))
        return COMP_D_AND_M;
    if (!strcmp(comp, "D|A"))
        return COMP_D_OR_A;
    if (!strcmp(comp, "D|M"))
        return COMP_D_OR_M;

    return NULL;
}

const char *code_jump(char *jump)
{
    if (!jump)
        return NULL;

    if (!strcmp(jump, "null"))
        return JUMP_NULL;
    if (!strcmp(jump, "JGT"))
        return JUMP_JGT;
    if (!strcmp(jump, "JEQ"))
        return JUMP_JEQ;
    if (!strcmp(jump, "JGE"))
        return JUMP_JGE;
    if (!strcmp(jump, "JLT"))
        return JUMP_JLT;
    if (!strcmp(jump, "JNE"))
        return JUMP_JNE;
    if (!strcmp(jump, "JLE"))
        return JUMP_JLE;
    if (!strcmp(jump, "JMP"))
        return JUMP_JMP;

    return NULL;
}
