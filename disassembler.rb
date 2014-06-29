CONSTANT_POOL = IO.binread('constant_pool.bin').unpack('L<*')
f = File.open('crashlog.txt')

insns = {}

KNOWN_OPCODES = {
  0x00 => 'add',
  0x01 => 'sub',
  0x02 => 'mul',
  0x03 => 'div',
  0x04 => 'eq',
  0x05 => 'land',
  0x06 => 'lor',
  0x07 => 'band',
  0x08 => 'bor',
  0x09 => 'slt?',
  0x0a => 'sgt?',
  0x0b => 'sle?',
  0x0c => 'sge?',
  0x0d => 'mod',
  0x0e => 'xor',
  0x10 => 'rand',
  0x11 => 'sw',
  0x12 => 'setnz',
  0x15 => 'sll',
  0x16 => 'load',
  0x17 => 'setneg',
  0x18 => 'lpool',
  0x1a => 'upcall',
  0x1f => 'loadobj',
  0x20 => 'storeobj',
  0x26 => 'lea',
}

def decodeOpcode(opc)
  KNOWN_OPCODES[opc] || sprintf('ins%-2X', opc)
end

def decodeOperand(op)
  bin = op.to_s(2).rjust(12, '0')
  if op == 0xe1f
    'TOS'
  elsif op == 0xbe0
    'SKIP'
  elsif op & 0xe00 == 0xe00
    sprintf 'v_%02x', op & ~0xe00
  elsif bin =~ /^00/
    #sprintf 'cp0/%X=0x%x', op, CONSTANT_POOL[op]
    sprintf '0x%x', CONSTANT_POOL[op]
  elsif bin =~ /^01/
    op &= ~0b1100_0000_0000
    #sprintf 'cp4/%X=0x%x', op, CONSTANT_POOL[op]
    sprintf '0x%x', CONSTANT_POOL[op]
  elsif bin =~ /^100/
    ((op & 0x1FF) << 8).to_s
  elsif bin =~ /^1010/
    ((op & 0xFF) << 4).to_s
  elsif bin =~ /^10110/
    sprintf 'g_%02x', op & 0x7f
  else
    sprintf '0x%03x', op
  end
end

def twos_comp(val, bits)
  if (val & (1 << (bits - 1))) != 0
    val = val - (1 << bits)
  end
  return val
end

f.readlines.each do |line|
  line.chomp!

  if line =~ /^HACK VmInstruction:.*PC=(.{10}) insn=(.{10})/
    pc = Integer($1)
    insn = Integer($2)

    opc = insn >> 24
    op1 = (insn >> 12) & 0xfff
    op2 = insn & 0xfff

    sop = decodeOpcode(opc)
    s = nil
    if opc == 0x82
      a = (insn >> 22) & 3
      b = (insn >> 20) & 3
      c = (insn >> 14) & 0x3f
      d = (insn >> 10) & 0xf
      e = insn & 0x3ff # ??? where's the rest of bits?
      de = insn & 0x3fff

      # Sign extend
      if (e & 0x200) != 0
        e -= 0x400
      end

      suffix = ['al', 'nz', 'ez', 'UND'][b]
      sc = c == 0x1f ? 'TOS,' : sprintf('v_%X', c) + ','
      if a == 0
        sop = 'br.' + suffix

        s = sprintf('%-7s    %-8s %d, %d', sop, sc, d, e).rstrip
      else
        sop = sprintf('ins82/%d.%s,', a, suffix)
        s = sprintf('%-7s    %-8s 0x%04x', sop, sc, de).rstrip
      end
    else
      s1 = decodeOperand(op1) + ','
      s2 = decodeOperand(op2)

      s = sprintf('%-7s    %-8s %-8s', sop, s1, s2).rstrip
    end
    line += '    DECODED: ' + s
  end
  puts line
end
