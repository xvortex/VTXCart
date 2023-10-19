#-----------------------------------------------------------------------------
# SNK MultiCart Compiler v1.01 - python
# (c) Vortex '2023. Python version by Fluffy.
#-----------------------------------------------------------------------------

import hashlib
import argparse
import os.path
import sys
import zlib

class TROM:
    def __init__(self):
        self.index = 0
        self.ngh = 0
        self.name = ""
        self.mname = ""
        self.pname = ""
        self.mode_bsw = 0
        self.mode_gra = 0 # graphics mode
        self.mode_aud = 0 # audio mode
        self.vblk_addr = -1
        self.crom_addr = -1
        self.prom_addr = -1
        self.mrom_addr = -1
        self.srom_addr = -1
        self.vrom_addr = -1
        self.crom = bytes()
        self.prom = bytes()
        self.mrom = bytes()
        self.srom = bytes()
        self.vrom = bytes()
        self.bram = bytes()

crom_max = 0x80000000 # 2048 Mb
prom_max = 0x18000000 # 384 Mb
mrom_max = 0x4000000  # 64 Mb
srom_max = 0x4000000  # 64 Mb (32 Mb act.)
vrom_max = 0x40000000 # 1024 Mb

crom_mask = 0x100000  # 1 Mb
prom_mask = 0x100000  # 1 Mb
mrom_mask = 0x40000   # 256 Kb
srom_mask = 0x20000   # 128 Kb
vrom_mask = 0x200000  # 2 Mb

type_prom = 0
type_prom1 = 1
type_crom = 2
type_vrom = 3
type_srom = 4
type_mrom = 5
type_bram = 6

BIT0 = 0x0001
BIT1 = 0x0002
BIT2 = 0x0004
BIT3 = 0x0008
BIT4 = 0x0010
BIT5 = 0x0020
BIT6 = 0x0040
BIT7 = 0x0080
BIT8 = 0x0100
BIT9 = 0x0200
BIT10 = 0x0400
BIT11 = 0x0800
BIT12 = 0x1000
BIT13 = 0x2000
BIT14 = 0x4000
BIT15 = 0x8000

ROM = []
fn = ""
_sys = 'mvs'
i = 0
crom_extend = 0
crom_pos = 0
prom_pos = 0
mrom_pos = 0
srom_pos = 0
vrom_pos = 0

logname = None

def WriteLog (s: str):
    print (s)
    with open(logname, "at", encoding='utf-8') as f:
        f.write("%s\n" % s)

def inttobin (p_nb_int: int, p_nb_digits: int=64) -> str:
    return format(p_nb_int, '0%ib' % p_nb_digits)

def GetMenu (sn: str) -> str:
  fn = os.path.join('../Games', sn, 'menu')
  if os.path.exists(fn):
    with open(fn, "rt") as f:
      return f.read().strip()
  else:
    WriteLog ('Warning: menu not found for %s using defaults' % sn)
    return sn

def GetVBlank (sn: str) -> int:
  fn = os.path.join('../Games', sn, 'vblank')
  if os.path.exists(fn):
    with open(fn, "rt") as f:
      s = f.read().strip().lower()
    try:
      result = int(s, 16)
    except ValueError:
      WriteLog ('Warning: vblank addr is not recognized %s' % sn)
      result = 0
    return result
  else:
    return 0      

def GetMode (sn: str):
    fn = os.path.join('../Games', sn, 'fpga')
    if (os.path.exists (fn)):
        with open (fn, "rt") as f:
          s = f.read().strip()
        s += "0"

        bsw = int (s [0])
        gra = int (s [1])
        aud = int (s [2])
    else:
        WriteLog ('Warning: fpga not found for %s using defaults' % sn)
        bsw = 1  # standard bankswitching
        gra = 0  # CROM full address
        aud = 0  # VROM+PCM mode
    return (bsw, gra, aud)

def GetBramSize (bram: bytes) -> int:
  ix = 0x320 + 0x1000
  if len(bram) < ix:
     return 0
  while True:
    ix -= 1
    if ix == 0x320:
        break
    if bram [ix] != 0:
        break
  
  ix = ix + 1 - 0x320
  if ((ix % 16) != 0):
    ix = ix + 16
  ix = ix // 16
  if (ix < 1):
    ix = 1
  return ix

def PSwap (rom : bytes) -> bytes:
    l = len (rom)
    tmp = bytearray([0xFF for x in range(l)])
    for i in range (l // 2):
        tmp [i * 2 + 0] = rom [i * 2 + 1]
        tmp [i * 2 + 1] = rom [i * 2 + 0]
    return bytes(tmp)

def CSwap (rom : bytes) -> bytes:
    l = len (rom)
    tmp = bytearray([0xFF for x in range(l)])
    for i in range (l // 4):
        tmp [i * 4 + 0] = rom [i * 4 + 0]
        tmp [i * 4 + 1] = rom [i * 4 + 2]
        tmp [i * 4 + 2] = rom [i * 4 + 1]
        tmp [i * 4 + 3] = rom [i * 4 + 3]
    return bytes(tmp)

def VSplit (rom : bytes):
    roma = rom[0:min(0x200000, len(rom))]
    romb = rom[0x200000:len(rom)]
    if len(romb) < 0x200000:
       romb = romb + bytes([0 for x in range(0x200000-len(romb))])
    return (bytes(roma), bytes(romb))

def GetPName (n: int):
  ROM [n].pname = " " * 16
  prom = PSwap (ROM [n].prom)
  #ix = 0x116   # JP lauout
  #ix = 0x11A   # US lauout
  ix = 0x11E    # EU lauout
  dw = (prom [ix + 0] << 24) + (prom [ix + 1] << 16) + (prom [ix + 2] << 8) + prom [ix + 3]
  if (dw >= 0x200000):
        dw = dw - 0x100000
  if (dw < len (ROM [n].prom) - 16):
    ix = dw
    ROM [n].pname = prom [ix: ix + 16]

def POP (fn: str, pos: int, mask: int, rom:bytes, typ: int):
    if os.path.exists (fn):
        ff = True
        fs = os.path.getsize(fn)
    else:
        ff = False
        fs = mask

    l = fs

    if (not ff) or (fs == 0):
        if typ == type_crom:
           return (pos, rom)
        if typ == type_vrom: 
           return (pos, rom)
        if typ == type_prom1: 
           return (pos, rom)
        if typ == type_prom:
          raise Exception('Error: %s not found!' % fn)
        if (typ == type_bram) and (len(rom) > 0):
          WriteLog ('Warning: %s not found!' % fn)

    if (l & (mask - 1)) != 0:
        l = (l + mask) & (~(mask - 1)) # shrink to mask

    ix = len(rom)
    if ((typ != type_bram) and (typ != type_vrom)):
      tmp = bytes([0xFF for x in range(l)])
    else:
      tmp = bytes([0x00 for x in range(l)])
    rom = bytearray(rom + tmp)

    if ff:
        with open(fn, "rb") as f:
          tmp = f.read()
          rom[ix:ix+fs] = tmp

    pos = pos + l

    return (pos, rom)

def Import (games: str):
    global ROM
    rom_index = 0

    def import_menu():
        nonlocal rom_index
        global crom_pos
        global prom_pos
        global mrom_pos
        global srom_pos
        global vrom_pos

        ix = len(ROM)
        ROM.append(TROM())

        ROM [ix].index = rom_index
        ROM [ix].name = 'menu'
        ROM [ix].mname = 'menu'
        ROM [ix].vblk_addr = 0
        ROM [ix].mode_bsw = 1
        ROM [ix].mode_gra = 0
        ROM [ix].mode_aud = 0
        ROM [ix].crom_addr = crom_pos
        ROM [ix].prom_addr = prom_pos
        ROM [ix].mrom_addr = mrom_pos
        ROM [ix].srom_addr = srom_pos
        ROM [ix].vrom_addr = vrom_pos
        ROM [ix].crom = bytes()
        ROM [ix].prom = bytes()
        ROM [ix].mrom = bytes()
        ROM [ix].srom = bytes()
        ROM [ix].vrom = bytes()

        fn = os.path.join('../Menu', _sys, 'menu-p1.bin')
        (prom_pos, ROM[ix].prom) = POP (fn, prom_pos, prom_mask, ROM[ix].prom, type_prom)

        ROM [ix].ngh = (ROM [ix].prom [0x109] << 8) + ROM [ix].prom [0x108]

        fn = os.path.join('../Menu', _sys, 'menu-s1.bin')
        (srom_pos, ROM[ix].srom) = POP (fn, srom_pos, srom_mask, ROM[ix].srom, type_srom)

        fn = os.path.join('../Menu', _sys, 'menu-m1.bin')
        (mrom_pos, ROM[ix].mrom) = POP (fn, mrom_pos, mrom_mask, ROM[ix].mrom, type_mrom)

        #print ('Info: ', ROM [ix].mname + ' imported')
        rom_index += 1

    def import1 (s: str):
        nonlocal rom_index
        global crom_pos
        global prom_pos
        global mrom_pos
        global srom_pos
        global vrom_pos

        ix = len(ROM)
        ROM.append(TROM())

        ROM [ix].index = rom_index
        ROM [ix].name = s
        ROM [ix].mname = GetMenu (s)
        ROM [ix].vblk_addr = GetVBlank (s)
        ROM [ix].crom_addr = crom_pos
        ROM [ix].prom_addr = prom_pos
        ROM [ix].mrom_addr = mrom_pos
        ROM [ix].srom_addr = srom_pos
        ROM [ix].vrom_addr = vrom_pos
        ROM [ix].crom = bytes()
        ROM [ix].prom = bytes()
        ROM [ix].mrom = bytes()
        ROM [ix].srom = bytes()
        ROM [ix].vrom = bytes()

        ROM [ix].mode_bsw, ROM [ix].mode_gra, ROM [ix].mode_aud = GetMode(s)

        dummy_pos = 0

        fn = os.path.join('../Games', ROM [ix].name, 'prom')
        (prom_pos, ROM[ix].prom) = POP (fn, prom_pos, prom_mask, ROM[ix].prom, type_prom)
        fn = os.path.join('../Games', ROM [ix].name, 'prom1')
        (prom_pos, ROM[ix].prom) = POP (fn, prom_pos, prom_mask, ROM[ix].prom, type_prom1)

        ROM [ix].ngh = (ROM [ix].prom [0x109] << 8) + ROM [ix].prom [0x108]
        GetPName (ix)

        fn = os.path.join('../Games', ROM [ix].name, 'crom0')
        (crom_pos, ROM[ix].crom) = POP (fn, crom_pos, crom_mask, ROM[ix].crom, type_crom)

        fn = os.path.join('../Games', ROM [ix].name, 'vroma0')
        (vrom_pos, ROM[ix].vrom) = POP (fn, vrom_pos, vrom_mask, ROM[ix].vrom, type_vrom)

        fn = os.path.join('../Games', ROM [ix].name, 'srom')
        (srom_pos, ROM[ix].srom) = POP (fn, srom_pos, srom_mask, ROM[ix].srom, type_srom)

        fn = os.path.join('../Games', ROM [ix].name, 'm1rom')
        (mrom_pos, ROM[ix].mrom) = POP (fn, mrom_pos, mrom_mask, ROM[ix].mrom, type_mrom)

        fn = os.path.join('../Games', ROM [ix].name, 'bram')
        (_,ROM[ix].bram) = POP (fn, dummy_pos, 0x10000, ROM[ix].bram, type_bram)

        #print ('Info: ', ROM [ix].mname + ' imported')
        rom_index = rom_index + 1

    print ('Import: ', end="")

    import_menu()

    with open(games, "rt") as f:
       for s in f:
          s = s.strip()
          if len(s) == 0:
             continue
          if s[0] == '#':
             continue
          import1(s)
          print('.', end='', flush=True)
    print ()
    print ()

def Report():
  global crom_pos
  global prom_pos
  global mrom_pos
  global srom_pos
  global vrom_pos

  s = 'no\tngh\t\tFPG\tprom_addr\tcrom_addr\tvrom_addr\tsrom_addr\tmrom_addr\tMenu name'
  WriteLog (s)
  s = '------------------------------------------------------------------------------------------'
  WriteLog (s)
  for i in range(len(ROM)):
    s = '%i' % (ROM[i].index) + '\t0x%04X' % (ROM[i].ngh) + '\t%i' % (ROM[i].mode_bsw) + '%i' % (ROM[i].mode_gra) + '%i' % (ROM[i].mode_aud) + '\t0x%08X' % (ROM[i].prom_addr) + '\t0x%08X' % (ROM[i].crom_addr) + '\t0x%08X' % (ROM[i].vrom_addr) + '\t0x%08X' % (ROM[i].srom_addr) + '\t0x%08X' % (ROM[i].mrom_addr) + '\t' + ROM[i].mname
    WriteLog (s)

  s = '------------------------------------------------------------------------------------------'
  WriteLog (s)
  s = '\t\t\t\t0x%08X' % (prom_pos) + '\t0x%08X' % (crom_pos) + '\t0x%08X' % (vrom_pos) + '\t0x%08X' % (srom_pos) + '\t0x%08X' % (mrom_pos)
  WriteLog (s)
  WriteLog ('')

def GenIX():
  os.makedirs ('Verilog', exist_ok=True)

  with open('Verilog\ix_c.inc', "wt", encoding='utf-8') as f:
    for i in range(len (ROM)):
      if (len (ROM[i].crom) > 0):
        mask = 0x3F
        if (len (ROM[i].crom) <= 0x4000000): mask = 0x3F
        if (len (ROM[i].crom) <= 0x2000000): mask = 0x1F
        if (len (ROM[i].crom) <= 0x1000000): mask = 0x0F
        if (len (ROM[i].crom) <= 0x0800000): mask = 0x07
        if (len (ROM[i].crom) <= 0x0400000): mask = 0x03
        if (len (ROM[i].crom) <= 0x0200000): mask = 0x01
        if (len (ROM[i].crom) <= 0x0100000): mask = 0x00
        w = ROM[i].crom_addr // crom_mask
        s = '%i' % (i) + ': begin MASK <= 6\'b' + inttobin (mask, 6) + '; IX <= 12\'b' + inttobin (w, 12) + '; end // ' + ROM[i].name
        f.write("%s\n" % s)

  with open('Verilog\ix_p.inc', "wt", encoding='utf-8') as f:
    for i in range(len (ROM)):
      p2_mirror = 0
      banks = (len (ROM[i].prom) // 0x100000) - 2
      if (banks < 0): banks = 0
      if (banks > 7):
        raise Exception ('Error: up to 8 pbanks is supported')
      if (len (ROM[i].prom) == 0x100000): p2_mirror = 1
      w = ROM[i].prom_addr // prom_mask
      s = '%i' % (i) + ': begin BANKS <= 3\'d%i' % (banks) + '; IX <= 9\'b' + inttobin (w, 9) + '; P2_MIRROR <= 1\'b' + inttobin ((p2_mirror), 1) + '; end // ' + ROM[i].name
      f.write("%s\n" % s)

  with open('Verilog\ix_v.inc', "wt", encoding='utf-8') as f:
    for i in range(len (ROM)):
      mode = ROM[i].mode_aud and 1
      if (len (ROM[i].vrom) > 0):
        w = ROM[i].vrom_addr // vrom_mask
        s = '%i' % (i) + ': begin MODE <= 1\'b' + inttobin (mode, 1) + '; IX <= 9\'b' + inttobin (w, 9) + '; end // ' + ROM[i].name
        f.write("%s\n" % s)

def GenROM():
  def SaveROM (fn: str, rom_1 : int, rom_max: int, typ: int):
    ff = False
    rom_arr = bytearray([0xFF for x in range(rom_max)])
    
    ix = 0
    for i in range(len (ROM)):
      if (ROM [i].name != 'menu'):
        if (ROM [i].mode_bsw > 1):
          raise Exception('Error: bankswitching mode %i' % (ROM [i].mode_bsw) + ' in ' + ROM [i].name + ' is not supported!')
        if (ROM [i].mode_gra > 4):
          raise Exception('Error: graphics mode %i' % (ROM [i].mode_gra) + ' in ' + ROM [i].name + ' is not supported!')
        if (ROM [i].mode_aud > 1):
          raise Exception('Error: audio mode %i' % (ROM [i].mode_aud) + ' in ' + ROM [i].name + ' is not supported!')
        if (typ == type_srom) and (len (ROM [i].srom) > srom_mask):
          raise Exception('Error: ' + ROM [i].name + ' srom size 0x%x' % (len (ROM [i].srom)) + ' bigger than 0x%x' % (srom_mask))
        if (typ == type_mrom) and (len (ROM [i].mrom) > mrom_mask):
          raise Exception('Error: ' + ROM [i].name + ' mrom size 0x%x' % (len (ROM [i].mrom)) + ' bigger than 0x%x' % (mrom_mask))
      in_arr = None
      if typ == type_prom: in_arr = ROM [i].prom
      if typ == type_crom: in_arr = ROM [i].crom
      if typ == type_vrom: in_arr = ROM [i].vrom
      if typ == type_srom: in_arr = ROM [i].srom
      if typ == type_mrom: in_arr = ROM [i].mrom
      l1 = len (in_arr)
      if (l1 > 0):
        if ((ix + l1) <= rom_max):
          rom_arr [ix:ix+l1] = in_arr
        else:
          l2 = min(ix+l1, rom_max) - ix
          rom_arr [ix:ix+l2] = in_arr[0:l2]
          if (not ff):
            WriteLog ('Error: ' + fn + ' is full!')
            ff = True
      ix = ix + l1
      print ('.', end='', flush=True)
    if (rom_1 != rom_max):
      ix = 0
      fx = 1
      while ix < rom_max:
        with open(fn + '-%i' % (fx), "wb") as f:
          f.write(rom_arr [ix:ix+rom_1])
        ix = ix + rom_1
        fx = fx + 1
    else:
      with open (fn, "wb") as f:
        f.write(rom_arr)
    rom_arr = bytes()

  print ('GenROM: ', end="")

  os.makedirs ('ROM', exist_ok=True)

  SaveROM ('ROM/prom', prom_max // 3, prom_max, type_prom)
  SaveROM ('ROM/crom', crom_max // 2, crom_max + crom_extend, type_crom)
  SaveROM ('ROM/vrom', vrom_max, vrom_max, type_vrom)
  SaveROM ('ROM/srom', srom_max, srom_max, type_srom)
  SaveROM ('ROM/mrom', mrom_max, mrom_max, type_mrom)

  print ()
  print ()

def PatchMenu():
  def EncodeChar (c: int) -> int:
    if (c >= ord ('0')) and (c <= ord ('9')):
      return  0x0280 + (c - ord ('0'))
    elif ((c >= ord ('A')) and (c <= ord ('P'))):
      return  0x02A0 + (c - ord ('A'))
    elif ((c >= ord ('Q')) and (c <= ord ('Z'))):
      return  0x02C0 + (c - ord ('Q'))
    elif (c == ord ('.')):
      return  0x02CA
    elif (c == ord ('\'')):
      return  0x02CB
    elif (c == ord ('!')):
      return  0x02CC
    elif (c == ord ('+')):
      return  0x02CD
    elif (c == ord ('-')):
      return  0x02CE
    else:
      if (_sys == 'aes'):
        return  0x02CF  # space AES
      else:
        return  0x026E  # space MVS

  prom = bytearray(PSwap (ROM [0].prom))

  # Menu Text
  ix = 0x80000; # menu sprites area
  iy = 1;      # skip menu
  for i in range(256): # total 256 elements
    if (iy >= len (ROM)):
      s = '                    '
    else:
      s = '  ' + (ROM[iy].mname.upper()) + '                  '
      s = s[:20]
    for j in range(20):
      if ((j < 2) and (iy < len (ROM))) :
         continue
      w1 = EncodeChar (ord(s [j]))
      w2 = w1 + 0x10
      prom [ix + j * 2 + 0] = w1 >> 8
      prom [ix + j * 2 + 1] = w1 & 255
      prom [ix + j * 2 + 40] = w2 >> 8
      prom [ix + j * 2 + 41] = w2 & 255
    ix = ix + 80 # next line;
    iy = iy + 1

  WriteLog ('ngh\t\tgname\t\t\t\tbram_len bram_addr')
  WriteLog ('---------------------------------------------')

  # bram
  ix1 = 0xF0000 # ngh's
  ix2 = 0xF0400 # bram block's cat
  ix3 = 0xF1000 # name's
  ix4 = 0x90000 # bram block's
  iy = 1        # skip menu
  gn = bytearray([0 for x in range(16)])
  for i in range(256): # total 256 elements
    if (iy >= len (ROM)):
      w1 = 0xFFFF
      w2 = 0xFFFF
      dw2 = 0xFFFFFFFF
      for j in range(16):
         gn [j] = 0xFF
    else:
      w1 = ROM[iy].ngh
      w2 = GetBramSize (ROM [iy].bram)
      dw2 = ix4
      for j in range(16):
        gn [j] = (ROM[iy].pname [j])

    # ngh
    prom [ix1 + 0] = w1 >> 8
    prom [ix1 + 1] = w1 & 255

    # bram cat
    prom [ix2 + 0] = w2 >> 8
    prom [ix2 + 1] = w2 & 255
    prom [ix2 + 2] = dw2 >> 24
    prom [ix2 + 3] = (dw2 >> 16) & 255
    prom [ix2 + 4] = (dw2 >> 8) & 255
    prom [ix2 + 5] = dw2 & 255

    # bram block
    # TODO

    # gname
    s = bytearray([0 for x in range(16)])
    for j in range(16):
      prom [ix3 + j] = gn [j]
      s[j] = gn[j]
    s = s.decode("shift-jis", errors="ignore")

    if (iy < len (ROM)):
      WriteLog ('0x%04X' % (w1) + ': [' + s + '] - 0x%04X' % (w2 * 16) + ', 0x%06x' % (dw2))

    ix1 = ix1 + 2
    ix2 = ix2 + 6
    ix3 = ix3 + 16
    iy = iy + 1

  # Limits
  i = len (ROM)
  w1 = i
  prom [0xE2629] = w1
  prom [0xFF04D] = w1

  i = len (ROM) - 1 - 12
  if (i < 0):
    i = 0
  w1 = i
  prom [0xE04BD] = w1
  prom [0xE0587] = w1
  prom [0xE05E7] = w1
  prom [0xE068F] = w1

  i = len (ROM) - 1
  w1 = i
  if (w1 < 12):
    prom [0xE04A7] = w1
    prom [0xE04D5] = w1 - 1
    prom [0xE058F] = w1 - 1
    prom [0xE05E3] = w1
    prom [0xE05EF] = w1
    prom [0xE05FF] = w1 - 1
    prom [0xE0669] = w1
    prom [0xE0671] = w1
    prom [0xE0697] = w1 - 1

  ROM [0].prom = PSwap (prom)

  del prom

  WriteLog ('')

def GenMAME():
  def SaveMAME (fn: str, arr: bytes):
    with open(fn, "wb") as f:
      f.write(arr)

  print ('SaveMAME: ', end="")

  os.makedirs ('MAME', exist_ok=True)
  os.makedirs ('MAME/roms', exist_ok=True)
  os.makedirs ('MAME/hash', exist_ok=True)

  with open('MAME/hash/neogeo.xml', "wt", encoding='utf-8') as f:
    f.write( '<?xml version="1.0"?>\n')
    f.write( '<!DOCTYPE softwarelist SYSTEM "softwarelist.dtd">\n')
    f.write( '<softwarelist name="neogeo" description="SNK Neo-Geo cartridges">\n')
    f.write( '\n')

    for i  in range (len (ROM)):
      fn = os.path.join('MAME/roms', ROM [i].name)
      os.makedirs (fn, exist_ok=True)

      vroma = bytearray()
      vromb = bytearray()
      if (ROM [i].mode_aud == 1): 
        (vroma, vromb) = VSplit (ROM [i].vrom)

      SaveMAME (os.path.join(fn, 'prom'), ROM [i].prom)
      SaveMAME (os.path.join(fn, 'srom'), ROM [i].srom)
      SaveMAME (os.path.join(fn, 'mrom'), ROM [i].mrom)
      if (len (vroma) > 0):
        SaveMAME (os.path.join(fn, 'vroma'), vroma)
        SaveMAME (os.path.join(fn, 'vromb'), vromb)
      elif (len (ROM [i].vrom) > 0):
        SaveMAME (os.path.join(fn, 'vrom'), ROM [i].vrom)
      if (len (ROM [i].crom) > 0):
        crom = CSwap (ROM [i].crom)
        SaveMAME (os.path.join(fn, 'crom'), crom)
      else:
        crom = bytes()

      size_c = 0x4000000
      if (len (crom) <= 0x4000000): size_c = 0x4000000
      if (len (crom) <= 0x2000000): size_c = 0x2000000
      if (len (crom) <= 0x1000000): size_c = 0x1000000
      if (len (crom) <= 0x0800000): size_c = 0x0800000
      if (len (crom) <= 0x0400000): size_c = 0x0400000
      if (len (crom) <= 0x0200000): size_c = 0x0200000
      if (len (crom) <= 0x0100000): size_c = 0x0100000

      crc_p = zlib.crc32( (ROM [i].prom))
      crc_c = zlib.crc32( (crom))
      crc_v = zlib.crc32( (ROM [i].vrom))
      crc_va = zlib.crc32( (vroma))
      crc_vb = zlib.crc32( (vromb))
      crc_s = zlib.crc32( (ROM [i].srom))
      crc_m = zlib.crc32( (ROM [i].mrom))

      sha_p = hashlib.sha1( (ROM [i].prom)).hexdigest().upper()
      sha_c = hashlib.sha1( (crom)).hexdigest().upper()
      sha_v = hashlib.sha1( (ROM [i].vrom)).hexdigest().upper()
      sha_va = hashlib.sha1( (vroma)).hexdigest().upper()
      sha_vb = hashlib.sha1( (vromb)).hexdigest().upper()
      sha_s = hashlib.sha1( (ROM [i].srom)).hexdigest().upper()
      sha_m = hashlib.sha1( (ROM [i].mrom)).hexdigest().upper()

      f.write( '\t<software name="' + ROM [i].name + '">\n')
      f.write( '\t\t<description>' + ROM [i].mname + '</description>\n')
      f.write( '\t\t<year>2023</year>\n')
      f.write( '\t\t<publisher>vortex</publisher>\n')
      f.write( '\t\t<sharedfeat name="release" value="MVS,AES" />\n')
      f.write( '\t\t<sharedfeat name="compatibility" value="MVS,AES" />\n')
      f.write( '\t\t<part name="cart" interface="neo_cart">\n')
      f.write( '\t\t\t<dataarea name="maincpu" width="16" endianness="big" size="0x%08X' % (len (ROM [i].prom)) + '">\n')
      f.write( '\t\t\t\t<rom loadflag="load16_word_swap" name="prom" offset="0x000000" size="0x%08X"' % (len (ROM [i].prom)) + ' crc="%08X"' % crc_p + ' sha1="' + sha_p + '" />\n')
      f.write( '\t\t\t</dataarea>\n')
      f.write( '\t\t\t<dataarea name="fixed" size="0x040000">\n')
      f.write( '\t\t\t\t<rom name="srom" offset="0x000000" size="0x%08X' % (len (ROM [i].srom)) + '" crc="%08X"' % crc_s + ' sha1="' + sha_s + '" />\n')
      f.write( '\t\t\t</dataarea>\n')
      f.write( '\t\t\t<dataarea name="audiocpu" size="0x%08X"' % (len (ROM [i].mrom)) + '>\n')
      f.write( '\t\t\t\t<rom name="mrom" offset="0x000000" size="0x%08X' % (len (ROM [i].mrom)) + '" crc="%08X"' % crc_m + ' sha1="' + sha_m + '" />\n')
      f.write( '\t\t\t</dataarea>\n')
      if (len (vroma) > 0):
        f.write( '\t\t\t<dataarea name="ymsnd:adpcma" size="0x%08X">\n' % (len (vroma)))
        f.write( '\t\t\t\t<rom name="vroma" offset="0x000000" size="0x%08X' % (len (vroma)) + '" crc="%08X"' % crc_va + ' sha1="' + sha_va + '" />\n')
        f.write( '\t\t\t</dataarea>\n')
        f.write( '\t\t\t<dataarea name="ymsnd:adpcmb" size="0x%08X">\n' % (len (vromb)))
        f.write( '\t\t\t\t<rom name="vromb" offset="0x000000" size="0x%08X' % (len (vromb)) + '" crc="%08X"' % crc_vb + ' sha1="' + sha_vb + '" />\n')
        f.write( '\t\t\t</dataarea>\n')
      else:
        if (len (ROM [i].vrom) > 0):
          f.write( '\t\t\t<dataarea name="ymsnd:adpcma" size="0x%08X">\n' % (len (ROM [i].vrom)))
          f.write( '\t\t\t\t<rom name="vrom" offset="0x000000" size="0x%08X' % (len (ROM [i].vrom)) + '" crc="%08X"' % crc_v + ' sha1="' + sha_v + '" />\n')
          f.write( '\t\t\t</dataarea>\n')
      if (len (ROM [i].crom) > 0):
        f.write( '\t\t\t<dataarea name="sprites" size="0x%08X">\n' % (size_c))
        f.write( '\t\t\t\t<rom loadflag="load8_byte" name="crom" offset="0x000000" size="0x%08X' % (len (ROM [i].crom)) + '" crc="%08X"' % crc_c + ' sha1="' + sha_c + '" />\n')
        f.write( '\t\t\t</dataarea>\n')
      else:
        f.write( '\t\t\t<dataarea name="sprites" size="0x%08X">\n' % (size_c))
        f.write( '\t\t\t</dataarea>\n')
      f.write( '\t\t</part>\n')
      f.write( '\t</software>\n')
      f.write( '\t\n')

      del crom

      print ('.', end='', flush=True)

    f.write( '</softwarelist>\n')

  print ()
  print ()

def main():
    global _sys
    global crom_extend
    global logname

    parser = argparse.ArgumentParser(prog='VTXCart', description='SNK MultiCart Compiler v1.01 (c) Vortex ''2023')
    parser.add_argument('filename')
    parser.add_argument('sys', choices=['mvs', 'aes'], default='mvs')
    parser.add_argument('--c3g', action='store_true')
    parser.add_argument('--genix', action='store_true')
    parser.add_argument('--patchmenu', action='store_true')
    parser.add_argument('--genmame', action='store_true')
    parser.add_argument('--genrom', action='store_true')

    args = parser.parse_args()

    _sys = args.sys
    crom_extend = 0x40000000 if args.c3g else 0

    fn = args.filename
    if not os.path.exists(fn):
        print ('Error: %s not exists!' % fn)
        exit()

    logname = "%s.log" % os.path.splitext(sys.argv[0])[0]

    Import (fn)
    Report()

    if args.genix:
        GenIX()
    if args.patchmenu:
        PatchMenu()
    if args.genmame:
        GenMAME()
    if args.genrom:
        GenROM()

    WriteLog ('Done.')

if __name__ == "__main__":
    main()
