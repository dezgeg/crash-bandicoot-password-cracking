var stringified = ["\u25EF", "\u25FB", "\u00D7", "\u25B3"];
function stringify(code) {
    var s = '';
    for (var i = 0; i < code.length; i++) {
        if (i == 8 || i == 16)
            s += "\n"
        s += stringified[code[i]];
    }
    return s;
}

function mixAndPermute(v) {
  var r;
  r = (v * v) % 0x7e5;
  r = (r * r) % 0x7e5;
  r = (v * r) % 0x7e5;

  return r;
}

var inverseMixAndPermute = new Array(0x7e5);
for (var i = 0; i < inverseMixAndPermute.length; i++)
    inverseMixAndPermute[i] = [];

for (var i = 0; i < 0x7ff; i++)
    inverseMixAndPermute[mixAndPermute(i)].push(i);

function check(code)
{
    var v_13 = 0, v_14 = 0;

    // Build bitmap into v_13 and v_14.
    for (var i = 0; i < 24; i++) {
        if (i < 13)
            v_13 |= code[i] << (2 * i);
        else
            v_14 |= code[i] << (2 * (i - 13));
    }

    // For a super password, the first two are always triangles. Discard them.
    v_13 >>= 4;
    v_13 &= 0x3fffff;
    v_14 &= 0x3fffff;

    var g_05 = v_13 & 0x7ff;
    var g_06 = (v_13 >> 11) & 0x7ff;
    var g_07 = v_14 & 0x7ff;
    var g_08 = (v_14 >> 11) & 0x7ff;

    var g_09 = g_05 ^ g_07 ^ g_08;
    var g_0a = g_05 ^ g_06 ^ g_08;
    var g_0b = g_05 ^ g_06 ^ g_07;
    var g_0c = g_06 ^ g_07 ^ g_08;

    g_09 ^= 0x18d;
    g_0a ^= 0x24e;
    g_0b ^= 0x3e4;
    g_0c ^= 0x139;

    g_05 = mixAndPermute(g_09);
    g_06 = mixAndPermute(g_0a);
    g_07 = mixAndPermute(g_0b);
    g_08 = mixAndPermute(g_0c);

    var valid = g_05 <= 0x1ff &&
                g_06 <= 0x1ff &&
                g_07 <= 0x1ff &&
                g_08 <= 0x3f;

    return valid ? [g_05, g_06, g_07, g_08] : false;
}

function make(g_05, g_06, g_07, g_08) {
    var results = [];

    var g_09 = inverseMixAndPermute[g_05][0];
    var g_0a = inverseMixAndPermute[g_06][0];
    var g_0b = inverseMixAndPermute[g_07][0];
    var g_0c = inverseMixAndPermute[g_08][0];

    g_09 ^= 0x18d;
    g_0a ^= 0x24e;
    g_0b ^= 0x3e4;
    g_0c ^= 0x139;

    g_05 = g_09 ^ g_0a ^ g_0b;
    g_06 = g_0a ^ g_0b ^ g_0c;
    g_07 = g_09 ^ g_0b ^ g_0c;
    g_08 = g_0a ^ g_09 ^ g_0c;

    var v_13 = g_05 | (g_06 << 11);
    var v_14 = g_07 | (g_08 << 11);

    v_13 <<= 4;
    v_13 |= 0xf;

    var result = [];
    for (var i = 0; i < 24; i++) {
        if (i < 13)
            result[i] = (v_13 >> (2 * i)) & 0x3;
        else
            result[i] = (v_14 >> (2 * (i - 13))) & 0x3;
    }

    return result;
}
