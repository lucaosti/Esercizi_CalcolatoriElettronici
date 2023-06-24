import re
import sys
import gdb
import gdb.printing
import struct
import fcntl
import termios
from gdb.FrameDecorator import FrameDecorator

def esc(c = ''):
    return '\x1b[' + str(c) + 'm';

# colors
class Color:
    black   = esc(30)
    red     = esc(31)
    green   = esc(32)
    yellow  = esc(33)
    blue    = esc(34)
    magenta = esc(35)
    cyan    = esc(36)
    white   = esc(37)
    reverse = esc(7)
    normal  = esc()

    conf = {
            'col_file'     : green,
            'col_function' : yellow,
            'col_proc_elem': green,
            'col_curline'  : reverse,
            'col_proc_hdr' : white,
            'col_var'      : cyan,
            'col_index'    : yellow,
            'col_pag_frame': green,
            'col_tab_frame': yellow,
            'col_res_frame': reverse,
            'col_usermode' : white,
            'col_sysmode'  : reverse,
            'col_intr_dis' : red,
            'col_intr_en'  : green,
            'col_io_dis'   : red,
            'col_io_en'    : green,
            'col_access_no': red,
            'col_access_ro': yellow,
            'col_access_rw': green,
            'col_idt_noint': red,
            'col_idt_okint': green
            }

def colorize(name, value):
    if name in Color.conf.keys():
        return Color.conf[name] + str(value) + Color.normal
    return value

# cache the inferior
qemu = gdb.selected_inferior()

# init rows and columns
rows, columns = 0, 0

def get_row_col():
    global rows, columns
    rows, columns, _, _ = struct.unpack('HHHH', fcntl.ioctl(0, termios.TIOCGWINSZ, struct.pack('HHHH', 0, 0, 0, 0)))
get_row_col()

# cache some types
ulong_type = gdb.lookup_type('unsigned long')
void_ptr_type = gdb.Type.pointer(gdb.lookup_type('void'))

def toi(v):
    """convert from gdb.Type to unsigned long"""
    try:
        vi = int(v.cast(ulong_type))
    except:
        vi = v
    if vi < 0:
        vi += 1 << 64
    return vi

res_sym = re.compile('^(\w+)(?:\(.*\))? in section \.text(?: of .*/(.*))?$')
def resolve_function(f):
    global res_sym
    s = gdb.execute('info symbol 0x{:x}'.format(f), False, True)
    m = res_sym.match(s)
    if not m:
        return (s.rstrip(), '???')
    fun = m.group(1)
    mod = m.group(2) or ''
    return (fun, mod)

# get the address of the gdt
gdt = toi(gdb.parse_and_eval("&gdt"))

# cache the idt
idt = toi(gdb.parse_and_eval("&idt"))

# cache apic registers
ioregsel = toi(gdb.parse_and_eval("apic::IOREGSEL"))
iowin    = toi(gdb.parse_and_eval("apic::IOWIN"))

# cache some constants
try:
    max_liv  = int(gdb.parse_and_eval('MAX_LIV'))
except:
    max_liv = 4
try:
    sc_desc  = int(gdb.parse_and_eval('$SEL_CODICE_SISTEMA'))
    uc_desc  = int(gdb.parse_and_eval('$SEL_CODICE_UTENTE'))
    ud_desc  = int(gdb.parse_and_eval('$SEL_DATI_UTENTE'))
except:
    sc_desc = 0x8
    uc_desc = 0x13
    ud_desc = 0x1b
m_parts = []
m_ini = []
m_names = []

def readfis(addr):
    """read an unsigned long from qemu memory"""
    return struct.unpack('Q', bytes(qemu.read_memory(addr, 8)))[0]

def v2p(tab, addr):
    """translate addr in the vm of proc"""
    for i in range(max_liv, 0, -1):
        #gdb.write("--> tab{} @ {:16x}\n".format(i, tab))
        shift = 12 + (i - 1) * 9
        addr2 = tab + ((addr >> shift) & 0x1ff) * 8
        #gdb.write("--> ent{} @ {:16x}\n".format(i, addr2))
        entry = readfis(addr2)
        if not (entry & 0x1):
            return None
        tab = entry & ~0xfff
    return tab | (addr & 0xfff)

def show_lines(sal, indent=0):
    curline = sal.line
    fname = sal.symtab.filename
    function = gdb.block_for_pc(sal.pc).function
    gdb.write("file: {} function: {}\n".format(
        colorize('col_file', fname),
        colorize('col_function', function)))
    lines = gdb.execute("list {}:{}".format(fname, curline), False, True)
    found = False
    for l in lines.splitlines():
        if not found:
            w = l.split()
            if len(w) > 0 and int(w[0]) == curline:
                gdb.write(" " * indent + colorize('col_curline', "*") + l + "\n")
                found = True
                continue
        gdb.write(" " * (indent + 1) + l + "\n")

def write_key(k, v, indent=0):
    gdb.write("{}{:16s}: {}\n".format(" " * indent, k, v))

def dump_flags(rflags):
    flags = { 14: "NT", 11: "OF", 10: "DF", 9: "IF", 8: "TF", 7: "SF", 6: "ZF", 4: "AF", 2: "PF", 0: "CF" }
    active = []
    for f in flags:
        if rflags & (1 << f):
            active.append(flags[f])
    return "[{} IOPL={}]".format(" ".join(active), "utente" if rflags & 0x3000 == 0x3000 else "sistema")

def dump_selector(sel):
    if sel == sc_desc:
        name = "SEL_CODICE_SISTEMA"
    elif sel == uc_desc:
        name = "SEL_CODICE_UTENTE"
    elif sel == ud_desc:
        name = "SEL_DATI_UTENTE"
    else:
        name = "sconosciuto"
    return "[{}]".format(name)

flags  = { 1: 'W', 2: 'U', 3: 'w', 4: 'c', 5: 'A', 6: 'D', 7: 's' }
nflags = { 1: 'R', 2: 'S', 3: '-', 4: '-', 5: '-', 6: '-', 7: '-' }

def vm_decode(indent, f, liv, stop=max_liv, nonpresent=False, rngs=[range(512)]*max_liv):
    if liv > 0 and stop > 0:
        for i in rngs[liv - 1]:
            e = readfis(f + i * 8)
            if e & 1 or (nonpresent and e):
                f1 = e & ~0xFFF
                fl = []
                for j in flags:
                    fl.append(flags[j] if e & (1 << j) else nflags[j])
                gdb.write(" " * indent + "{:03o}: {} ".format(i, "".join(fl)))
                if e & 1:
                     gdb.write(" \u279e {}\n".format(vm_paddr_to_str(f1)))
                     if not e & (1<<7):
                         vm_decode(indent + 10, f1, liv - 1, stop - 1, nonpresent, rngs)
                elif nonpresent:
                    gdb.write("b: {}\n".format(f1 >> 12))

def vm_show_tree(f, liv=max_liv):
    gdb.write(vm_paddr_to_str(f) + "\n")
    vm_decode(0, f, liv, max_liv, False)

def vm_show_table(f, liv=max_liv):
    gdb.write(vm_paddr_to_str(f) + "\n")
    vm_decode(0, f, liv, 1, True)

def vm_vaddr_split(v):
    off = v & 0xfff
    v >>= 12
    idx = [ (v >> (i * 9)) & 0o777 for i in range(max_liv) ]
    h = v >> (9 * max_liv)
    return [ off ] + idx + [h]

def vm_show_path(v, cr3):
    vi = vm_vaddr_split(v)[1:max_liv+1]
    rngs = [ range(i, i + 1) for i in vi ]
    gdb.write("cr3: {}\n".format(vm_paddr_to_str(cr3)))
    vm_decode(0, cr3, max_liv, max_liv, False, rngs)

page_size = { 5: '128PiB', 4: '512GiB', 3: '1GiB', 2: '2MiB', 1: '4KiB' }

def vm_access_byte_to_str(a):
    if not a & 1:
        return "unmapped"
    fl = []
    if a & (1 << 2):
        fl.append("U")
    else:
        fl.append("S")
    if a & (1 << 8):
        fl.append("G")
    if a & (1 << 1):
        fl.append("W")
    else:
        fl.append("R")
    if a & (1 << 3):
        fl.append("PWT")
    if a & (1 << 4):
        fl.append("PCD")
    if a & (1 << 7):
        fl.append("PS")
    if a & (1 << 5):
        fl.append("A")
    if a & (1 << 6):
        fl.append("D")
    return " ".join(fl)

def vm_tab_entry_to_str(d):
    s = vm_access_byte_to_str(d & 0xfff)
    f = d & ~0xfff
    ds = "{:x}: {}".format(toi(d), s)
    if d:
        if d & 1:
            ds += " \u2192 {}".format(vm_paddr_to_str(f))
    return ds

class VaddrParam(gdb.Parameter):
    def __init__(self):
        super (VaddrParam, self).__init__(
                'vaddr-decode',
                gdb.COMMAND_DATA,
                gdb.PARAM_ENUM,
                [ 'hex', 'oct', 'path', 'both' ])
        self.value = 'both'

vaddr_verbose = VaddrParam()

def vm_vaddr_to_str(v, liv=0, verbose='default'):
    r = []
    if verbose == 'default':
        verbose = vaddr_verbose.value
    if verbose in ('hex', 'both'):
        r.append(format(v, '016x'))
    if verbose in ('oct', 'path', 'both'):
        ov = vm_vaddr_split(v)[::-1]
        sov = [ format(v, '03o') for v in ov ][1:-1]
        if verbose == 'oct':
            sov += [ format(ov[-1], '04o') ]
        else:
            liv -= 1
        if ov[0] == 0:
            t = 'S'
        elif ov[0] == (1 << (64 - (max_liv*9+12))) - 1:
            t = 'U'
        else:
            t = "{:o}?".format(ov[0])
        if liv > 0:
            p = "-".join(sov[:-liv]) + "|" + "-".join(sov[-liv:])
        else:
            p = "-".join(sov)
        r.append(t + '-' + p)
    return " ".join(r)

class FrameParam(gdb.Parameter):
    def __init__(self):
        super (FrameParam, self).__init__(
                'show-frame-descriptors',
                gdb.COMMAND_DATA,
                gdb.PARAM_BOOLEAN)
        self.value = True

frame_verbose = FrameParam()

def vm_paddr_to_str(f):
    s = "0x{:08x}".format(toi(f))
    return s

cs_cur = 0
wp_cur = 0
def vm_dump_map(v, a):
    vv = v[:]
    while len(vv) < max_liv:
        vv.append(0)
    vs = []
    if vv[0] >= 256:
        addr = 0xffff
        vs.append("U")
    else:
        addr = 0
        vs.append("S")
    for i in range(max_liv):
        addr = (addr << 9) | vv[i]
        vs.append("{:03o}".format(vv[i]))
    addr <<= 12
    col = 'col_access_rw'
    if not a & 1:
        col = 'col_access_no'
    elif cs_cur and not a & (1 << 2):
        col = 'col_access_no'
    elif (cs_cur or wp_cur) and not a & (1 << 1):
        col = 'col_access_ro'
    gdb.write(colorize(col, "{:016x}  {}: {}\n".format(addr, "-".join(vs), vm_access_byte_to_str(a))))

vm_last = 0xffff
def vm_show_maps_rec(tab, liv, virt, cur):
    global vm_last, m_ini, max_liv
    cur_reg = 0
    for i in range(512):
        if liv == max_liv and cur_reg < len(m_ini) and i == m_ini[cur_reg]:
            gdb.write(colorize('col_proc_hdr', "---" + m_names[cur_reg] + "---\n"))
            cur_reg += 1
        e = readfis(tab + i * 8)
        a = e & 0xfff
        if liv > 1 and not a & (1 << 7):
            a &= ~(1 << 5)  # A
            a &= ~(1 << 6)  # D
        if not cur & (1 << 1): # R/W
            a &= ~(1 << 1)
        if not cur & (1 << 2): # U/S
            a &= ~(1 << 2)
        virt.append(i)
        if a & 1 and liv > 1 and not a & (1 << 7):
            f = e & ~0xfff
            vm_show_maps_rec(f, liv - 1, virt, a)
        elif a != vm_last:
            vm_dump_map(virt, a)
            vm_last = a
        virt.pop()

def vm_show_maps(cr3):
    global vm_last, cs_cur, wp_cur
    cs_cur = toi(gdb.parse_and_eval('$cs')) & 0x3
#        vstack = toi(proc['contesto'][4])
#        stack = v2p(proc, vstack)
#        cs_cur = readfis(stack + 8) & 0x3
    wp_cur = toi(gdb.parse_and_eval('$cr0')) & (1 << 16)
    vm_last = 0xffff
    vm_show_maps_rec(cr3, max_liv, [], 0x7)

class Vm(gdb.Command):
    """info about virtual memory"""

    def __init__(self):
        super(Vm, self).__init__("vm", gdb.COMMAND_DATA, prefix=True)

class VmTree(gdb.Command):
    """show a translation tree.
The command has an optional argument, which is the address of a frame
containing a table. It shows the translation whose root is the given table.
Note that only entries with P=1 are followed.

If called without arguments, it shows the tree that start at the table4 of
'esecuzione->id'."""

    def __init__(self):
        super(VmTree, self).__init__("vm tree", gdb.COMMAND_DATA, gdb.COMPLETE_EXPRESSION)

    def invoke(self, arg, from_tty):
        if arg:
            f = toi(gdb.parse_and_eval(arg))
        else:
            f = toi(gdb.parse_and_eval('$cr3'))
        vm_show_tree(f)

class VmTable(gdb.Command):
    """show the entries of a translation table.
The command has a non-optional argument, which is the address of a frame
containing a table. It shows all the entries of the table that are not
completely null."""

    def __init__(self):
        super(VmTable, self).__init__("vm table", gdb.COMMAND_DATA, gdb.COMPLETE_EXPRESSION)

    def invoke(self, arg, from_tty):
        if arg:
            f = toi(gdb.parse_and_eval(arg))
        else:
            f = toi(gdb.parse_and_eval('$cr3'))
        vm_show_table(f)

class VmPath(gdb.Command):
    """show the translation path of a virtual address.
The command has a non-optional argument, which is a virtual address,
and an optional argument which is the address of a root table
($cr3 is assumed by default)
If the optional argument is present it must be separated by '@'

The command shows the table entries that the MMU must follow to
translate the given virtual address, starting from the table4 of the
given process."""

    def __init__(self):
        super(VmPath, self).__init__("vm path", gdb.COMMAND_DATA, gdb.COMPLETE_EXPRESSION)

    def invoke(self, arg, from_tty):
        a = list(arg.split("@"))
        addr = a[0]
        if len(a) > 1:
            tab = toi(gdb.parse_and_eval(a[1]))
        else:
            tab = toi(gdb.parse_and_eval('$cr3'))
        v = toi(gdb.parse_and_eval(addr))
        vm_show_path(v, tab)

class VmMaps(gdb.Command):
    """show the mappings of an address space.
The command acceptes an optional argument which is a process id
('esecuzione->id' is assumed by default).
The command shows a condensed view of the address space of the process,
grouped by sequential addresses which have the same access byte."""

    def __init__(self):
        super(VmMaps, self).__init__("vm maps", gdb.COMMAND_DATA, gdb.COMPLETE_EXPRESSION)

    def invoke(self, arg, from_tty):
        if arg:
            f = toi(gdb.parse_and_eval(arg))
        else:
            f = toi(gdb.parse_and_eval('$cr3'))
        vm_show_maps(f)

Vm()
VmTree()
VmTable()
VmPath()
VmMaps()

def print_hdr(hdr):
    l = (columns - len(hdr) - 2)
    s = l // 2
    gdb.write("\u2500" * s + "[" + hdr + "]" + "\u2500" * (s + (l & 1)))

def print_footer():
    gdb.write("\u2550" * columns + "\n")

def cpu_mode(cs):
    if cs & 0x3:
        return colorize('col_usermode', "utente")
    return colorize('col_sysmode', "sistema")

def cpu_intr(eflags):
    if eflags & (1 << 9):
        return colorize('col_intr_en', "abilitati   ")
    return colorize('col_intr_dis',    "disabilitati")

def cpu_iopl(cs, eflags):
    iopl = (eflags & 0x3000) >> 12
    pl = cs & 2
    if pl <= iopl:
        return colorize('col_io_en', "permesso")
    return colorize('col_io_dis',    "vietato ")

def show_cpu_status():
    cs=toi(gdb.parse_and_eval('$cs'))
    eflags=toi(gdb.parse_and_eval('$eflags'))
    cr3=toi(gdb.parse_and_eval('$cr3'))
    gdb.write("modo: " + cpu_mode(cs) + "  intr: " + cpu_intr(eflags) + "  io: " + cpu_iopl(cs, eflags) + "  cr3: " + vm_paddr_to_str(cr3) + "\n")

registers = [ 'rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15' ]
def show_registers():
    global registers
    for i in range(8):
        gdb.write('{:>3s}: {:016x}                {:>3s}: {:016x}\n'.format(registers[i], toi(gdb.parse_and_eval('$'+registers[i])), registers[i+8], toi(gdb.parse_and_eval('$'+registers[i+8]))))
    gdb.write('RIP: {}\n'.format(gdb.parse_and_eval('$rip')))
    gdb.write('RFLAGS: {}\n'.format(gdb.parse_and_eval('$eflags')))
    #gdb.execute('print $eflags')

def dump_error(e):
    s = ''
    if e & 1:
        s += 'EXT'
        e & ~1
    if e:
        if s:
            s += ' '
        if e & 2:
            s += 'IDT'
        else:
            s += 'LDT' if e & 4 else 'GDT'
        s += '[' + str(e & 7) + ']'
    return '[' + s + ']'

def dump_fault(e):
    s  = 'prot' if e & 1 else 'pg/tab'
    s += ' w'   if e & 2 else ' r'
    s += ' usr' if e & 4 else ' sys'
    if e & 8:
        s += ' resvd'
    return '[' + s + ']'

def dump_rip(e):
    rip_s = "{}".format(gdb.Value(e).cast(void_ptr_type)).split()
    return " ".join(rip_s[1:])

int_entrypoint = re.compile("^((a|exc|driver|handler)_\w+) in section \.text")
stack_min_lines = 10
stack_max_lines = 10
stack_interp = dict()
def show_stack():
    global m_ini, m_parts, max_liv, stack_interp
    stack=toi(gdb.parse_and_eval('$rsp'))
    base=toi(gdb.parse_and_eval('$rbp'))
    stack_i = stack >> ((max_liv - 1)*9 + 12) & 0o777
    for i in range(len(m_parts)):
        if stack_i < m_ini[i + 1]:
            break
    if len(m_parts):
        stack_type = '??? (' + m_parts[i] + ')'
    else:
        i = 0
        stack_type = ''
    stop = (1<<64)-1
    if i == 1:
        stack_type = 'sistema'
        stop = m_ini[2] << (max_liv - 1)*9 + 12
    elif i == 4:
        stack_type = 'utente'
        stop = 1 << 64
    gdb.write('pila ' + stack_type + ':\n')
    stop1 = max(base+16, stack+stack_min_lines*8)
    stop = min(stop1, stop, stack+stack_max_lines*8)
    todelete = [ k for k in stack_interp if k < stack ]
    for k in todelete:
            del stack_interp[k]
    cur_rip = toi(gdb.parse_and_eval('$rip'))
    sym=gdb.execute('info symbol $rip', False, True)
    m = int_entrypoint.match(sym)
    if m or struct.unpack('H', qemu.read_memory(cur_rip, 2))[0] == 0xcf48:
        stackp = stack
        if m:
            for j in (8, 10, 11, 12, 13, 14, 17):
                access, offset, _ = read_idt(j)
                if (access & (1<<7)) and cur_rip == offset:
                    stack_interp[stackp] = dump_fault if j == 14 else dump_error
                    stackp += 8
        stack_interp[stackp] = dump_rip
        stackp += 8
        stack_interp[stackp] = dump_selector
        stackp += 8
        stack_interp[stackp] = dump_flags
        stackp += 8
        # print saved rsp at it is
        stackp += 8
        stack_interp[stackp] = dump_selector
    for s in range(stack, stop, 8):
        k = ''
        v = readfis(s)
        if s in stack_interp:
            k = stack_interp[s](v)
        d = s - base
        if abs(d) > 4096:
            d = 0
        gdb.write("{:18x}:   {:016x} {:4d} {}\n".format(s, v, d, k))

class ptrPrinter:
    """print a pointer"""

    def __init__(self, val, omit=False):
        self.val = val
        self.omit = omit

    def to_string(self):
        p = toi(self.val)
        if p == 0:
            return 'null'
        try:
            e = self.val.dereference()
        except:
            pe = "???"
        else:
            pe = e
        if self.omit:
            return pe
        return "0x{:x} \u279e {}".format(p, pe)

class tab_entryPrinter:
    """print a tab_entry"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return vm_tab_entry_to_str(self.val)

def tab_entryLookup(val):
    if str(val.type.unqualified()) == 'tab_entry':
        return tab_entryPrinter(val)
    return None

gdb.pretty_printers.append(tab_entryLookup)

class vaddrPrinter:
    """print a vaddr"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return vm_vaddr_to_str(self.val)

def vaddrLookup(val):
    if str(val.type.unqualified()) == 'vaddr':
        return vaddrPrinter(toi(val))
    return None

gdb.pretty_printers.append(vaddrLookup)

class paddrPrinter:
    """print a paddr"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return vm_paddr_to_str(self.val)

def paddrLookup(val):
    if str(val.type.unqualified()) == 'paddr':
        return paddrPrinter(toi(val))
    return None

gdb.pretty_printers.append(paddrLookup)

class tab_iterPrinter:
    """print a tab_iter"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        s = self.val['s']
        f =  '{\n'
        f += '  range: [{}, {})\n'.format(s[max_liv]['cur'], s[max_liv]['end'])
        l = self.val['l']
        if l == max_liv + 1:
            f+= '  done\n'
        else:
            for i in range(max_liv, l - 1, -1):
                f+= '  {}:     [{}, {}) tab: {}\n'.format(i, s[i-1]['cur'], s[i-1]['end'], s[i-1]['tab'])
        f += '}'
        return f

def tab_iterLookup(val):
    if str(val.type.unqualified()) == 'tab_iter':
        return tab_iterPrinter(val)
    return None

gdb.pretty_printers.append(tab_iterLookup)

class natPrinter:
    """print an natl/q"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return str(self.val) + " " + hex(self.val)

def natlLookup(val):
    t = str(val.type.unqualified())
    if t == 'natl' or t == 'natq':
        return natPrinter(toi(val))
    return None

gdb.pretty_printers.append(natlLookup)

class ioaddrPrinter:
    """print an ioaddr"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "0x{:04x}".format(toi(self.val))

def ioaddrLookup(val):
    t = str(val.type.unqualified())
    if t == 'ioaddr':
        return ioaddrPrinter(toi(val))
    return None

gdb.pretty_printers.append(ioaddrLookup)

def read_idt(i):
        off0, sel, access, off1, off2 = struct.unpack('HHxBHI', bytes(qemu.read_memory(idt + i * 16, 12)))
        if not (access & (1<<7)):
            return 0, 0, 0
        offset = off2 << 32 | off1 << 16 | off0
        return access, offset, sel

# IDT entry layout
#
#     7     6     5     4     3     2     1     0  
#  +-----+-----+-----+-----+-----+-----+-----+-----+
#  | off 16-31 | acb |  0  |  SELECTOR |  off 0-15 |  0
#  +-----+-----+-----+-----+-----+-----+-----+-----+
#  |                       |       off 32-63       | +8
#  +-----+-----+-----+-----+-----+-----+-----+-----+
#                                                                                     
# access byte (byte 5 above:
#
#     7     6     5     4     3     2     1     0  
#  +-----+-----+-----+-----+-----+-----+-----+-----+
#  |  P  |    DPL    |  0  |  1  |  1  |  1  | I/T |
#  +-----+-----+-----+-----+-----+-----+-----+-----+
#
class IDT(gdb.Command):
    """show the status of the IDT"""

    def __init__(self):
        super(IDT, self).__init__("idt", gdb.COMMAND_DATA, prefix=False)

    def invoke(self, arg, from_tty):
        present=True
        filt=[]
        r = range(256)
        if arg:
            if arg[0] == '/':
                filt = arg[1:].split(',')
                if 'all' in filt:
                    present=False
            else:
                i = toi(gdb.parse_and_eval(arg))
                r = range(i, i+1)
        for i in r:
            access, offset, sel = read_idt(i)
            if not (access & (1<<7)) and present:
                continue
            cs_cur = toi(gdb.parse_and_eval('$cs')) & 0x3
            accessible = ((access >> 5) & 0x3) >= cs_cur
            if ('open' in filt and not accessible) or ('closed' in filt and accessible):
                continue
            col = 'col_idt_okint' if accessible else 'col_idt_noint'
            su  = 'sys' if sel == sc_desc else 'usr'
            it  = 'trap' if access & 1 else 'intr'
            if ('trap' in filt and it == 'intr') or ('intr' in filt and it == 'trap'):
                continue
            function, module = resolve_function(offset)
            if ('sistema' in filt and module != 'sistema') or ('io' in filt and module != 'io'):
                continue
            gdb.write(colorize(col, "[{:2x}]: {} {} {:16x} ({}:{})\n".format(i, su, it, offset, module, function))) 

IDT()

#    63          16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
#  +---/   /---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
#  |    ...    |msk|tgm|   |   |   |pol|   |   |   |           vector              |
#  +---/   /---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
#
#

def apic_read_rtl(irq):
    global ioregsel, iowin
    qemu.write_memory(ioregsel, struct.pack('I', 0x10 + irq * 2), 4)
    return toi(struct.unpack('I', qemu.read_memory(iowin, 4))[0])

def apic_read_isr(i):
    return toi(struct.unpack('I', qemu.read_memory(0xfee00100 + i * 16, 4))[0])

def apic_read_irr(i):
    return toi(struct.unpack('I', qemu.read_memory(0xfee00200 + i * 16, 4))[0])


class Apic(gdb.Command):
    """show the status of the APIC"""

    def __init__(self):
        super(Apic, self).__init__("apic", gdb.COMMAND_DATA, prefix=False)

    def invoke(self, arg, from_tty):
        for i in range(24):
            rtl = apic_read_rtl(i)
            vector = rtl & 0xff
            if not vector:
                continue
            pol = 'low ' if rtl & (1<<11) else 'high'
            tgm = 'level' if rtl & (1<<15) else 'edge '
            masked = '(masked)' if rtl & (1<<16) else ''
            gdb.write("[{:2d}]: polarity={} mode={} vector={:2x} {}\n".format(i, pol, tgm, vector, masked))
        isr = [ apic_read_isr(i) for i in range(7, -1, -1) ]
        gdb.write("ISR: " + ":".join([ "{:04x}:{:04x}".format(x >> 16, x & 0xFFFF) for x in isr ]) + "\n")
        irr = [ apic_read_irr(i) for i in range(7, -1, -1) ]
        gdb.write("IRR: " + ":".join([ "{:04x}:{:04x}".format(x >> 16, x & 0xFFFF) for x in irr ]) + "\n")

Apic()

def context_backtrace():
    print_hdr("backtrace")
    gdb.execute("backtrace")

source = 'Source language is '
def context_sorgente():
    global stack_interp
    print_hdr("sorgente")
    show_lines(gdb.newest_frame().find_sal())
    s = gdb.execute("info source", False, True)
    if (s[s.index(source) + len(source):].startswith('asm')):
        print_hdr("registri")
        show_registers()
        print_hdr("pila")
        show_stack()
    else:
    #    stack_interp = dict()
        print_hdr("variabili")
        gdb.execute("info args")
        gdb.execute("info locals")

def context_protezione():
    print_hdr("protezione")
    show_cpu_status()

def print_context():
    get_row_col()
    context_backtrace()
    context_sorgente()
    context_protezione()
    print_footer()

class Context(gdb.Command):
    """print context info"""

    def __init__(self):
        super(Context, self).__init__("context", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        print_context()

Context()

def context_event(event):
    print_context()

gdb.events.stop.connect(context_event)
