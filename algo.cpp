#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int32_t Word; // shift instruction does srav

enum {
    CIR,
    SQU,
    EXS,
    TRI,
};
const char * const stringified[] = { "\u25EF", "\u25FB", "\u00D7", "\u25B3" };

const uint8_t codes[][24] = {
    { /* Super password */
        TRI, TRI, TRI, TRI, EXS, SQU, TRI, TRI,
        TRI, TRI, SQU, EXS, TRI, CIR, TRI, TRI,
        TRI, CIR, SQU, TRI, EXS, EXS, EXS, EXS,
    },
    { /* 52%, 21/32 levels, 0/2 keys, 4/26 gems */
        TRI, TRI, CIR, EXS, EXS, SQU, SQU, CIR,
        SQU, TRI, TRI, EXS, EXS, CIR, CIR, SQU,
        CIR, SQU, TRI, SQU, EXS, EXS, EXS, CIR,
    },
    { /* 56%, 23/32 levels, 0/2 keys, 4/26 gems */
        TRI, TRI, TRI, TRI, TRI, EXS, SQU, SQU,
        SQU, TRI, TRI, EXS, EXS, TRI, SQU, CIR,
        TRI, SQU, CIR, EXS, CIR, CIR, TRI, EXS,
    },
    { /* 57%, 23/32 levels, 0/2 keys, 5/26 gems */
        TRI, TRI, TRI, SQU, SQU, SQU, SQU, CIR,
        SQU, TRI, TRI, EXS, EXS, TRI, TRI, EXS,
        CIR, SQU, SQU, EXS, SQU, TRI, EXS, CIR
    },
};

Word mixAndPermute(Word v)
{
    Word g_0d = 1;
    Word g_0e = 0x40000000;

    for (Word g_0f = 0; g_0f < 31; g_0f++) {
        g_0d = (g_0d * g_0d) % 0x7e5;

        if (g_0e & 0x5) {
            g_0d = (g_0d * v) % 0x7e5;
        }
        g_0e >>= 1;
    }

    return g_0d;
}

bool algo(const uint8_t* code)
{
    Word v_13 = 0, v_14 = 0;

    // Built bitmap into v_13 and v_14.
    for (int i = 0; i < 24; i++) {
        if (i < 13)
            v_13 |= code[i] << (2 * i);
        else
            v_14 |= code[i] << (2 * (i - 13));
    }

    // For a super password, the first two are always triangles. Discard them.
    v_13 >>= 4;
    v_13 &= 0x3fffff;
    v_14 &= 0x3fffff;

    // printf("v_13: 0x%08x v_14: 0x%08x\n", v_13, v_14);

    Word g_05 = v_13 & 0x7ff;
    Word g_06 = (v_13 >> 11) & 0x7ff;
    Word g_07 = v_14 & 0x7ff;
    Word g_08 = (v_14 >> 11) & 0x7ff;

    Word g_09 = g_05 ^ g_07 ^ g_08;
    Word g_0a = g_05 ^ g_06 ^ g_08;
    Word g_0b = g_05 ^ g_06 ^ g_07;
    Word g_0c = g_06 ^ g_07 ^ g_08;

    g_09 ^= 0x18d;
    g_0a ^= 0x24e;
    g_0b ^= 0x3e4;
    g_0c ^= 0x139;

    // printf("Before mix:\n");
    // printf("g_09: 0x%08x, ", g_09);
    // printf("g_0a: 0x%08x, ", g_0a);
    // printf("g_0b: 0x%08x, ", g_0b);
    // printf("g_0c: 0x%08x\n", g_0c);

    g_05 = mixAndPermute(g_09);
    g_06 = mixAndPermute(g_0a);
    g_07 = mixAndPermute(g_0b);
    g_08 = mixAndPermute(g_0c);

    // printf("After mix:\n");
    // printf("g_05: 0x%08x, ", g_05);
    // printf("g_06: 0x%08x, ", g_06);
    // printf("g_07: 0x%08x, ", g_07);
    // printf("g_08: 0x%08x\n", g_08);

    bool valid = g_05 <= 0x1ff &&
                 g_06 <= 0x1ff &&
                 g_07 <= 0x1ff &&
                 g_08 <= 0x3f;

    printf("Code");
    for (int i = 0; i < 24; i++) {
        printf(" %s", stringified[code[i]]);
    }
    printf(": %s (0x%08x 0x%08x 0x%08x 0x%08x)\n", valid ? "yes" : "no",
            g_05, g_06, g_07, g_08);

    return valid;
}

int main()
{
    for (int i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
        algo(codes[i]);
}
