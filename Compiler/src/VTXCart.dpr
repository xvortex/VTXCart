//-----------------------------------------------------------------------------
// SNK MultiCart Compiler v1.00
// project started 05.19.2023                                  (c) Vortex '2023
//-----------------------------------------------------------------------------

{$APPTYPE CONSOLE}
program VTXCart;

uses
  Windows, Sysutils, IdGlobal, IdHashSHA, IdHashCRC;

type
  TARR = packed array of byte;
  TPName = packed array [0..15] of byte;

  TROM = record
    index: byte;
    ngh:   word;
    name:  string;
    mname: string;
    pname: TPname;
    mode_bsw: integer; // bankswitching mode
    mode_gra: integer; // graphics mode
    mode_aud: integer; // audio mode
    vblk_addr: cardinal;
    crom_addr: cardinal;
    prom_addr: cardinal;
    mrom_addr: cardinal;
    srom_addr: cardinal;
    vrom_addr: cardinal;
    crom: TARR;
    prom: TARR;
    mrom: TARR;
    srom: TARR;
    vrom: TARR;
    bram: TARR;
  end;

const
  crom_max = $80000000; // 2048 Mb
  prom_max = $18000000; // 384 Mb
  mrom_max = $4000000;  // 64 Mb
  srom_max = $4000000;  // 64 Mb (32 Mb act.)
  vrom_max = $40000000; // 1024 Mb

  crom_mask = $100000;  // 1 Mb
  prom_mask = $100000;  // 1 Mb
  mrom_mask = $40000;   // 256 Kb
  srom_mask = $20000;   // 128 Kb
  vrom_mask = $200000;  // 2 Mb

  type_prom = 0;
  type_prom1 = 1;
  type_crom = 2;
  type_vrom = 3;
  type_srom = 4;
  type_mrom = 5;
  type_bram = 6;

  BIT0 = $0001;
  BIT1 = $0002;
  BIT2 = $0004;
  BIT3 = $0008;
  BIT4 = $0010;
  BIT5 = $0020;
  BIT6 = $0040;
  BIT7 = $0080;
  BIT8 = $0100;
  BIT9 = $0200;
  BIT10 = $0400;
  BIT11 = $0800;
  BIT12 = $1000;
  BIT13 = $2000;
  BIT14 = $4000;
  BIT15 = $8000;

var
  ROM: array of TROM;
  fn, sys: string;
  i: integer;
  crom_extend: cardinal;
  crom_pos: cardinal;
  prom_pos: cardinal;
  mrom_pos: cardinal;
  srom_pos: cardinal;
  vrom_pos: cardinal;

procedure WriteLog (s: string);
var
  f: textfile;
  fn: string;
begin
  fn := changefileext (extractfilename (paramstr (0)), '.log');
  assign (f, fn);
  if (fileexists (fn)) then
    append (f)
  else
    rewrite (f);
  writeln (f, s);
  writeln (s);
  closefile (f);
end;

function inttobin (p_nb_int: uint64; p_nb_digits: byte=64): string;
begin
  SetLength(Result, p_nb_digits);
  while p_nb_digits > 0 do
  begin
    if odd(p_nb_int) then
      Result[p_nb_digits] := '1'
    else
      Result[p_nb_digits] := '0';
    p_nb_int := p_nb_int shr 1;
    dec(p_nb_digits);
  end;
end;

function GetMenu (sn: string): string;
var
  fn: string;
  f: textfile;
begin
  fn := '..\Games\' + sn + '\' + 'menu';
  if (fileexists (fn)) then
  begin
    assign (f, fn);
    reset (f);
    readln (f, result);
    closefile (f);
    result := trim (result);
  end else
  begin
    WriteLog ('Warning: menu not found for ' + sn + ' using defaults');
    result := sn;
  end;
end;

function GetVBlank (sn: string): cardinal;
var
  fn, s: string;
  f: textfile;
begin
  fn := '..\Games\' + sn + '\' + 'vblank';
  if (fileexists (fn)) then
  begin
    assign (f, fn);
    reset (f);
    readln (f, s);
    closefile (f);
    s := lowercase (trim (s));
    try
      result := strtoint ('$' + s);
    except
      WriteLog ('Warning: vblank addr is not recognized ' + sn);
      result := 0;
    end;
  end else
  begin
    result := 0;
  end;
end;

procedure GetMode (sn: string; var bsw: integer; var gra: integer; var aud: integer);
var
  fn, s: string;
  f: textfile;
begin
  fn := '..\Games\' + sn + '\' + 'fpga';
  if (fileexists (fn)) then
  begin
    assign (f, fn);
    reset (f);
    readln (f, s);
    closefile (f);
    s := trim (s);
    s := s + '0';
    bsw := strtoint (s [1]);
    gra := strtoint (s [2]);
    aud := strtoint (s [3]);
  end else
  begin
    WriteLog ('Warning: fpga not found for ' + sn + ' using defaults');
    bsw := 1; // standard bankswitching
    gra := 0; // CROM full address
    aud := 0; // VROM+PCM mode
  end;
end;

function GetBramSize (var bram: TARR): word;
var
  ix: integer;
begin
  ix := $320 + $1000;
  repeat
    ix := ix - 1;
    if (ix = $320) then break;
    if (bram [ix] <> 0) then break;
  until false;
  ix := ix + 1 - $320;
  if ((ix mod 16) <> 0) then ix := ix + 16;
  ix := ix div 16;
  if (ix < 1) then ix := 1;
  result := ix;
end;

procedure PSwap (var rom, res: TARR);
var
  i, l: cardinal;
  tmp: TARR;
begin
  l := length (rom);
  setlength (tmp, l);
  for i := 0 to ((l div 2) - 1) do
  begin
    tmp [i * 2 + 0] := rom [i * 2 + 1];
    tmp [i * 2 + 1] := rom [i * 2 + 0];
  end;
  res := tmp;
  setlength (tmp, 0);
end;

procedure CSwap (var rom, res: TARR);
var
  i, l: cardinal;
  tmp: TARR;
begin
  l := length (rom);
  setlength (tmp, l);
  for i := 0 to ((l div 4) - 1) do
  begin
    tmp [i * 4 + 0] := rom [i * 4 + 0];
    tmp [i * 4 + 1] := rom [i * 4 + 2];
    tmp [i * 4 + 2] := rom [i * 4 + 1];
    tmp [i * 4 + 3] := rom [i * 4 + 3];
  end;
  res := tmp;
  setlength (tmp, 0);
end;

procedure VSplit (var rom, roma, romb: TARR);
var
  i, l: cardinal;
begin
  l := length (rom);
  if (l < $400000) then l := $400000;
  setlength (roma, $200000);
  setlength (romb, l - $200000);
  for i := 0 to length (roma) - 1 do roma [i] := 0;
  for i := 0 to length (romb) - 1 do romb [i] := 0;
  for i := 0 to $200000 - 1 do
  begin
    if (i < length (rom)) then roma [i] := rom [i];
  end;
  for i := 0 to (l - $200000) - 1 do
  begin
    if ((i + $200000) < length (rom)) then romb [i] := rom [i + $200000];
  end;
end;

procedure GetPName (n: integer);
var
  i, ix: integer;
  dw: dword;
  prom: TARR;
begin
  for i := 0 to 15 do ROM [n].pname [i] := byte (' ');
  PSwap (ROM [n].prom, prom);
  //ix := $116; // JP lauout
  //ix := $11A; // US lauout
  ix := $11E; // EU lauout
  dw := (prom [ix + 0] shl 24) + (prom [ix + 1] shl 16) +
        (prom [ix + 2] shl 8) + prom [ix + 3];
  if (dw >= $200000) then dw := dw - $100000;
  if (dw < length (ROM [n].prom) - 16) then
  begin
    ix := dw;
    for i := 0 to 15 do ROM [n].pname [i] := prom [ix + i];
  end;
  SetLength (prom, 0);
end;

procedure POP (fn: string; var pos: cardinal; mask: cardinal; var rom: TARR; typ: integer);
var
  f: file of byte;
  i, l, fs, ix: cardinal;
  ff, dp: boolean;
begin
  if (fileexists (fn)) then
  begin
    ff := true;
    assignfile (f, fn);
    reset (f);
    fs := filesize (f);
  end else
  begin
    ff := false;
    fs := mask;
  end;

  l := fs;

  if ((not ff) or (fs = 0)) then
  begin
    if (typ = type_crom) then exit;
    if (typ = type_vrom) then exit;
    if (typ = type_prom1) then exit;
    if (typ = type_prom) then
    begin
      WriteLog ('Error: ' + fn + ' not found!');
      Halt(1);
    end;
    if ((typ = type_bram) and (length (rom) > 0)) then
    begin
      WriteLog ('Warning: ' + fn + ' not found!');
    end;
  end;

  if ((l and (mask - 1)) <> 0) then
  begin
    l := (l + mask) and (not (mask - 1)); // shrink to mask
  end;

  if ((typ = type_prom) and (l = $100000)) then // double prom (if no bs)
  begin
    l := l + $100000;
    dp := true;
  end
  else dp := false;

  ix := length (rom);
  SetLength (rom, ix + l);
  if ((typ <> type_bram) and (typ <> type_vrom)) then
    for i := 0 to (l - 1) do rom [ix + i] := $FF
  else
    for i := 0 to (l - 1) do rom [ix + i] := $00;

  if (ff) then
  begin
    blockread (f, &rom [ix], fs);
    if (dp) then
    begin
      seek (f, 0);
      blockread (f, &rom [ix + $100000], fs);
    end;
    closefile (f);
  end;

  pos := pos + l;
end;

procedure Import (games: string);
var
  f: textfile;
  s, fn: string;
  ix: integer;

  rom_index: byte;
  dummy_pos: cardinal;

procedure import_menu;
begin
  ix := length (ROM);
  setlength (ROM, length (ROM) + 1);

  ROM [ix].index := rom_index;
  ROM [ix].name := 'menu';
  ROM [ix].mname := 'menu';
  ROM [ix].vblk_addr := 0;
  ROM [ix].mode_bsw := 1;
  ROM [ix].mode_gra := 0;
  ROM [ix].mode_aud := 0;
  ROM [ix].crom_addr := crom_pos;
  ROM [ix].prom_addr := prom_pos;
  ROM [ix].mrom_addr := mrom_pos;
  ROM [ix].srom_addr := srom_pos;
  ROM [ix].vrom_addr := vrom_pos;
  SetLength (ROM [ix].crom, 0);
  SetLength (ROM [ix].prom, 0);
  SetLength (ROM [ix].mrom, 0);
  SetLength (ROM [ix].srom, 0);
  SetLength (ROM [ix].vrom, 0);

  fn := '..\MEnu\' + sys + '\menu-p1.bin';
  POP (fn, prom_pos, prom_mask, ROM [ix].prom, type_prom);

  ROM [ix].ngh := (ROM [ix].prom [$109] shl 8) + ROM [ix].prom [$108];

  fn := '..\MEnu\' + sys + '\menu-s1.bin';
  POP (fn, srom_pos, srom_mask, ROM [ix].srom, type_srom);

  fn := '..\MEnu\' + sys + '\menu-m1.bin';
  POP (fn, mrom_pos, mrom_mask, ROM [ix].mrom, type_mrom);

  //WriteLn ('Info: ', ROM [ix].mname + ' imported');
  rom_index := rom_index + 1;
end;

procedure import1 (s: string);
begin
  ix := length (ROM);
  setlength (ROM, length (ROM) + 1);

  ROM [ix].index := rom_index;
  ROM [ix].name := s;
  ROM [ix].mname := GetMenu (s);
  ROM [ix].vblk_addr := GetVBlank (s);
  ROM [ix].crom_addr := crom_pos;
  ROM [ix].prom_addr := prom_pos;
  ROM [ix].mrom_addr := mrom_pos;
  ROM [ix].srom_addr := srom_pos;
  ROM [ix].vrom_addr := vrom_pos;
  SetLength (ROM [ix].crom, 0);
  SetLength (ROM [ix].prom, 0);
  SetLength (ROM [ix].mrom, 0);
  SetLength (ROM [ix].srom, 0);
  SetLength (ROM [ix].vrom, 0);

  GetMode(s, ROM [ix].mode_bsw, ROM [ix].mode_gra, ROM [ix].mode_aud);

  fn := '..\Games\' + ROM [ix].name + '\' + 'prom';
  POP (fn, prom_pos, prom_mask, ROM [ix].prom, type_prom);
  fn := '..\Games\' + ROM [ix].name + '\' + 'prom1';
  POP (fn, prom_pos, prom_mask, ROM [ix].prom, type_prom1);

  ROM [ix].ngh := (ROM [ix].prom [$109] shl 8) + ROM [ix].prom [$108];
  GetPName (ix);

  fn := '..\Games\' + ROM [ix].name + '\' + 'crom0';
  POP (fn, crom_pos, crom_mask, ROM [ix].crom, type_crom);

  fn := '..\Games\' + ROM [ix].name + '\' + 'vroma0';
  POP (fn, vrom_pos, vrom_mask, ROM [ix].vrom, type_vrom);

  fn := '..\Games\' + ROM [ix].name + '\' + 'srom';
  POP (fn, srom_pos, srom_mask, ROM [ix].srom, type_srom);

  fn := '..\Games\' + ROM [ix].name + '\' + 'm1rom';
  POP (fn, mrom_pos, mrom_mask, ROM [ix].mrom, type_mrom);

  fn := '..\Games\' + ROM [ix].name + '\' + 'bram';
  POP (fn, dummy_pos, $10000, ROM [ix].bram, type_bram);

  //WriteLn ('Info: ', ROM [ix].mname + ' imported');
  rom_index := rom_index + 1;
end;

begin
  Write ('Import: ');

  setlength (ROM, 0);

  rom_index := 0;
  crom_pos := 0;
  prom_pos := 0;
  mrom_pos := 0;
  srom_pos := 0;
  vrom_pos := 0;

  import_menu;

  assignfile (f, games);
  reset (f);
  while not eof (f) do
  begin
    readln (f, s);
    s := trim (s);
    if (s = '') then continue;
    if (s[1] = '#') then continue;
    import1 (s);
    Write ('.');
  end;
  closefile (f);
  WriteLn ('');
  WriteLn ('');
end;

procedure Report;
var
  i: integer;
  s: string;
begin
  s := 'no' + #9 + 'ngh' + #9 + #9 + 'FPG' + #9 + 'prom_addr' + #9 + 'crom_addr' + #9 + 'vrom_addr' + #9 + 'srom_addr' + #9 + 'mrom_addr' + #9 + 'Menu name';
  WriteLog (s);
  s := '------------------------------------------------------------------------------------------';
  WriteLog (s);
  for i := 0 to length (ROM) - 1 do
  begin
    s := inttostr (ROM[i].index)               + #9 +
         '0x' + inttohex (ROM[i].ngh, 4)       + #9 +
          inttostr(ROM[i].mode_bsw) + inttostr(ROM[i].mode_gra) + inttostr(ROM[i].mode_aud) + #9 +
         '0x' + inttohex (ROM[i].prom_addr, 8) + #9 +
         '0x' + inttohex (ROM[i].crom_addr, 8) + #9 +
         '0x' + inttohex (ROM[i].vrom_addr, 8) + #9 +
         '0x' + inttohex (ROM[i].srom_addr, 8) + #9 +
         '0x' + inttohex (ROM[i].mrom_addr, 8) + #9 + ROM[i].mname;
    WriteLog (s);
  end;
  s := '------------------------------------------------------------------------------------------';
  WriteLog (s);
  s := #9 + #9 + #9 + #9 +
       '0x' + inttohex (prom_pos, 8) + #9 +
       '0x' + inttohex (crom_pos, 8) + #9 +
       '0x' + inttohex (vrom_pos, 8) + #9 +
       '0x' + inttohex (srom_pos, 8) + #9 +
       '0x' + inttohex (mrom_pos, 8);
  WriteLog (s);
  WriteLog ('');
end;

procedure GenIX;
var
  f: textfile;
  i: integer;
  s: string;
  w: word;
  banks, mask, mode: integer;
begin
  CreateDir ('Verilog');

  assignfile (f, 'Verilog\ix_c.inc');
  rewrite (f);
  for i := 0 to length (ROM) - 1 do
  begin
    if (length (ROM[i].crom) > 0) then
    begin
      mask := $3F;
      if (length (ROM[i].crom) <= $4000000) then mask := $3F;
      if (length (ROM[i].crom) <= $2000000) then mask := $1F;
      if (length (ROM[i].crom) <= $1000000) then mask := $0F;
      if (length (ROM[i].crom) <= $0800000) then mask := $07;
      if (length (ROM[i].crom) <= $0400000) then mask := $03;
      if (length (ROM[i].crom) <= $0200000) then mask := $01;
      if (length (ROM[i].crom) <= $0100000) then mask := $00;
      w := ROM[i].crom_addr div crom_mask;
      s := inttostr (i) + ': begin MASK <= 6''b' + inttobin (mask, 6) + '; IX <= 12''b' + inttobin (w, 12) + '; end // ' + ROM[i].name;
      writeln (f, s);
    end;
  end;
  closefile (f);

  assignfile (f, 'Verilog\ix_p.inc');
  rewrite (f);
  for i := 0 to length (ROM) - 1 do
  begin
    banks := (length (ROM[i].prom) div $100000) - 2;
    if (banks < 0) then banks := 0;
    if (banks > 7) then
    begin
      WriteLog ('Error: up to 8 pbanks is supported');
      Halt;
    end;
    w := ROM[i].prom_addr div prom_mask;
    s := inttostr (i) + ': begin BANKS <= 3''d' + inttostr (banks) + '; IX <= 9''b' + inttobin (w, 9) + '; end // ' + ROM[i].name;
    writeln (f, s);
  end;
  closefile (f);

  assignfile (f, 'Verilog\ix_v.inc');
  rewrite (f);
  for i := 0 to length (ROM) - 1 do
  begin
    mode := ROM[i].mode_aud and 1;
    if (length (ROM[i].vrom) > 0) then
    begin
      w := ROM[i].vrom_addr div vrom_mask;
      s := inttostr (i) + ': begin MODE <= 1''b' + inttobin (mode, 1) + '; IX <= 9''b' + inttobin (w, 9) + '; end // ' + ROM[i].name;
      writeln (f, s);
    end;
  end;
  closefile (f);
end;

procedure GenROM;

procedure SaveROM (fn: string; rom_1, rom_max, typ: cardinal);
var
  i, j, ix, fx, l1: cardinal;
  in_arr: TARR;
  rom_arr: TARR;
  f: file of byte;
  ff: boolean;
begin
  ff := false;
  SetLength (rom_arr, rom_max);
  for i := 0 to (rom_max - 1) do rom_arr [i] := $FF;
  ix := 0;
  for i := 0 to length (ROM) - 1 do
  begin
    if (ROM [i].name <> 'menu') then
    begin
      if (ROM [i].mode_bsw > 1) then
      begin
        WriteLog ('Error: bankswitching mode ' + inttostr (ROM [i].mode_bsw) + ' in ' + ROM [i].name + ' is not supported!');
        Halt;
      end;
      if (ROM [i].mode_gra > 4) then
      begin
        WriteLog ('Error: graphics mode ' + inttostr (ROM [i].mode_gra) + ' in ' + ROM [i].name + ' is not supported!');
        Halt;
      end;
      if (ROM [i].mode_aud > 1) then
      begin
        WriteLog ('Error: audio mode ' + inttostr (ROM [i].mode_aud) + ' in ' + ROM [i].name + ' is not supported!');
        Halt;
      end;
      if ((typ = type_srom) and (length (ROM [i].srom) > srom_mask)) then
      begin
        WriteLog ('Error: ' + ROM [i].name + ' srom size 0x' + inttohex (length (ROM [i].srom)) + ' bigger than 0x' + inttohex (srom_mask));
        Halt;
      end;
      if ((typ = type_mrom) and (length (ROM [i].mrom) > mrom_mask)) then
      begin
        WriteLog ('Error: ' + ROM [i].name + ' mrom size 0x' + inttohex (length (ROM [i].mrom)) + ' bigger than 0x' + inttohex (mrom_mask));
        Halt;
      end;
    end;
    in_arr := nil;
    if typ = type_prom then in_arr := ROM [i].prom;
    if typ = type_crom then in_arr := ROM [i].crom;
    if typ = type_vrom then in_arr := ROM [i].vrom;
    if typ = type_srom then in_arr := ROM [i].srom;
    if typ = type_mrom then in_arr := ROM [i].mrom;
    l1 := length (in_arr);
    if (l1 > 0) then
    begin
      for j := 0 to (l1 - 1) do
      begin
        if ((ix + j) < rom_max) then
        begin
          rom_arr [ix + j] := in_arr [j];
        end else
        begin
          if (not ff) then
          begin
            WriteLog ('Error: ' + fn + ' is full!');
            ff := true;
          end;
        end;
      end;
    end;
    ix := ix + l1;
    Write ('.');
  end;
  if (rom_1 <> rom_max) then
  begin
    ix := 0;
    fx := 1;
    repeat
      assignfile (f, fn + '-' + inttostr (fx));
      rewrite (f);
      blockwrite (f, &rom_arr [ix], rom_1);
      closefile (f);
      ix := ix + rom_1;
      fx := fx + 1;
    until (ix >= rom_max);
  end else
  begin
    assignfile (f, fn);
    rewrite (f);
    blockwrite (f, &rom_arr [0], rom_max);
    closefile (f);
  end;
  SetLength (rom_arr, 0);
end;

begin
  Write ('GenROM: ');

  CreateDir ('ROM');

  SaveROM ('ROM\prom', prom_max div 3, prom_max, type_prom);
  SaveROM ('ROM\crom', crom_max div 2, crom_max + crom_extend, type_crom);
  SaveROM ('ROM\vrom', vrom_max, vrom_max, type_vrom);
  SaveROM ('ROM\srom', srom_max, srom_max, type_srom);
  SaveROM ('ROM\mrom', mrom_max, mrom_max, type_mrom);

  WriteLn ('');
  WriteLn ('');
end;

procedure PatchMenu;
var
  i, j, ix, iy: integer;
  s: string;
  w1, w2: word;
  dw2: dword;
  ix1, ix2, ix3, ix4: integer;
  gn: array [0..15] of byte;
  prom: TARR;

function EncodeChar (c: byte): word;
begin
  if ((c >= byte ('0')) and (c <= byte ('9'))) then
  begin
    result := $0280 + (c - byte ('0'));
  end else
  if ((c >= byte ('A')) and (c <= byte ('P'))) then
  begin
    result := $02A0 + (c - byte ('A'));
  end else
  if ((c >= byte ('Q')) and (c <= byte ('Z'))) then
  begin
    result := $02C0 + (c - byte ('Q'));
  end else
  if (c = byte ('.')) then
  begin
    result := $02CA;
  end else
  if (c = byte ('''')) then
  begin
    result := $02CB;
  end else
  if (c = byte ('!')) then
  begin
    result := $02CC;
  end else
  if (c = byte ('+')) then
  begin
    result := $02CD;
  end else
  if (c = byte ('-')) then
  begin
    result := $02CE;
  end else
  begin
    if (sys = 'aes') then
      result := $02CF  // space AES
    else
      result := $026E; // space MVS
  end;
end;

begin
  PSwap (ROM [0].prom, prom);

  // Menu Text
  ix := $80000; // menu sprites area
  iy := 1;      // skip menu
  for i := 0 to 255 do // total 256 elements
  begin
    if (iy >= length (ROM)) then
    begin
      s := '                    ';
    end else
    begin
      s := '  ' + uppercase (ROM[iy].mname) + '                  ';
      s := copy (s, 1, 20);
    end;
    for j := 0 to 19 do begin
      if ((j < 2) and (iy < length (ROM))) then continue;
      w1 := EncodeChar (byte (s [j + 1]));
      w2 := w1 + $10;
      prom [ix + j * 2 + 0] := w1 shr 8;
      prom [ix + j * 2 + 1] := w1;
      prom [ix + j * 2 + 40] := w2 shr 8;
      prom [ix + j * 2 + 41] := w2;
    end;
    ix := ix + 80; // next line;
    iy := iy + 1;
  end;

  s := 'ngh' + #9 + #9 + 'gname' + #9 + #9 + #9 + #9 + 'bram_len bram_addr';
  WriteLog (s);
  s := '---------------------------------------------';
  WriteLog (s);

  // bram
  ix1 := $F0000; // ngh's
  ix2 := $F0400; // bram block's cat
  ix3 := $F1000; // name's
  ix4 := $90000; // bram block's
  iy := 1;       // skip menu
  for i := 0 to 255 do // total 256 elements
  begin
    if (iy >= length (ROM)) then
    begin
      w1 := $FFFF;
      w2 := $FFFF;
      dw2 := $FFFFFFFF;
      for j := 0 to 15 do gn [j] := $FF;
    end else
    begin
      w1 := ROM[iy].ngh;
      w2 := GetBramSize (ROM [iy].bram);
      dw2 := ix4;
      for j := 0 to 15 do gn [j] := byte (ROM[iy].pname [j]);
    end;

    // ngh
    prom [ix1 + 0] := w1 shr 8;
    prom [ix1 + 1] := w1;

    // bram cat
    prom [ix2 + 0] := w2 shr 8;
    prom [ix2 + 1] := w2;
    prom [ix2 + 2] := dw2 shr 24;
    prom [ix2 + 3] := dw2 shr 16;
    prom [ix2 + 4] := dw2 shr 8;
    prom [ix2 + 5] := dw2;

    // bram block
    // TODO

    // gname
    s := '';
    for j := 0 to 15 do
    begin
      prom [ix3 + j] := gn [j];
      s := s + chr (gn [j]);
    end;

    if (iy < length (ROM)) then
    begin
      WriteLog ('0x' + inttohex (w1, 4) + ': [' + s + '] - 0x' + inttohex (w2 * 16, 4) + ', 0x' + inttohex (dw2, 6));
    end;

    ix1 := ix1 + 2;
    ix2 := ix2 + 6;
    ix3 := ix3 + 16;
    iy := iy + 1;
  end;

  // Limits
  i := length (ROM);
  w1 := i;
  prom [$E2629] := w1;
  prom [$FF04D] := w1;

  i := length (ROM) - 1 - 12;
  if (i < 0) then i := 0;
  w1 := i;
  prom [$E04BD] := w1;
  prom [$E0587] := w1;
  prom [$E05E7] := w1;
  prom [$E068F] := w1;

  i := length (ROM) - 1;
  w1 := i;
  if (w1 < 12) then
  begin
    prom [$E04A7] := w1;
    prom [$E04D5] := w1 - 1;
    prom [$E058F] := w1 - 1;
    prom [$E05E3] := w1;
    prom [$E05EF] := w1;
    prom [$E05FF] := w1 - 1;
    prom [$E0669] := w1;
    prom [$E0671] := w1;
    prom [$E0697] := w1 - 1;
  end;

  // double prom
  for i := 0 to $100000 - 1 do
  begin
    prom [i + $100000] := prom [i];
  end;

  PSwap (prom, ROM [0].prom);

  setlength (prom, 0);

  WriteLog ('');
end;

procedure GenMAME;
var
  i: integer;
  fn: string;
  f: textfile;
  crom, vroma, vromb: TARR;
  crc_c, crc_v, crc_va, crc_vb, crc_s, crc_m, crc_p: string;
  sha_c, sha_v, sha_va, sha_vb, sha_s, sha_m, sha_p: string;
  IdHashCRC32: TIdHashCRC32;
  IdHashSHA1: TIdHashSHA1;
  size_c: integer;

procedure SaveMAME (fn: string; arr: TARR);
var
  f: file of byte;
begin
  assignfile (f, fn);
  rewrite (f);
  blockwrite (f, &arr [0], length (arr));
  closefile (f);
end;

begin
  Write ('SaveMAME: ');

  CreateDir ('MAME');
  CreateDir ('MAME\roms');
  CreateDir ('MAME\hash');

  assign (f, 'MAME\hash\neogeo.xml');
  rewrite (f);
	writeln (f, '<?xml version="1.0"?>');
	writeln (f, '<!DOCTYPE softwarelist SYSTEM "softwarelist.dtd">');
	writeln (f, '<softwarelist name="neogeo" description="SNK Neo-Geo cartridges">');
 	writeln (f, '');
  closefile (f);

  for i := 0 to (length (ROM) - 1) do
  begin
    fn := 'MAME\roms\' + ROM [i].name;
    CreateDir (fn);

    SetLength (vroma, 0);
    SetLength (vromb, 0);
    if (ROM [i].mode_aud = 1) then VSplit (ROM [i].vrom, vroma, vromb);

    SaveMAME (fn + '\prom', ROM [i].prom);
    SaveMAME (fn + '\srom', ROM [i].srom);
    SaveMAME (fn + '\mrom', ROM [i].mrom);
    if (length (vroma) > 0) then
    begin
      SaveMAME (fn + '\vroma', vroma);
      SaveMAME (fn + '\vromb', vromb);
    end
    else
    if (length (ROM [i].vrom) > 0) then
    begin
      SaveMAME (fn + '\vrom', ROM [i].vrom);
    end;
    if (length (ROM [i].crom) > 0) then
    begin
      CSwap (ROM [i].crom, crom);
      SaveMAME (fn + '\crom', crom);
    end;

    assign (f, 'MAME\hash\neogeo.xml');
    append (f);

    size_c := $4000000;
    if (length (crom) <= $4000000) then size_c := $4000000;
    if (length (crom) <= $2000000) then size_c := $2000000;
    if (length (crom) <= $1000000) then size_c := $1000000;
    if (length (crom) <= $0800000) then size_c := $0800000;
    if (length (crom) <= $0400000) then size_c := $0400000;
    if (length (crom) <= $0200000) then size_c := $0200000;
    if (length (crom) <= $0100000) then size_c := $0100000;

    IdHashCRC32 := TIdHashCRC32.Create;
    crc_p := IdHashCRC32.HashBytesAsHex(TIdBytes (ROM [i].prom));
    crc_c := IdHashCRC32.HashBytesAsHex(TIdBytes (crom));
    crc_v := IdHashCRC32.HashBytesAsHex(TIdBytes (ROM [i].vrom));
    crc_va := IdHashCRC32.HashBytesAsHex(TIdBytes (vroma));
    crc_vb := IdHashCRC32.HashBytesAsHex(TIdBytes (vromb));
    crc_s := IdHashCRC32.HashBytesAsHex(TIdBytes (ROM [i].srom));
    crc_m := IdHashCRC32.HashBytesAsHex(TIdBytes (ROM [i].mrom));
    IdHashCRC32.Destroy;

    IdHashSHA1 := TIdHashSHA1.Create;
    sha_p := IdHashSHA1.HashBytesAsHex(TIdBytes (ROM [i].prom));
    sha_c := IdHashSHA1.HashBytesAsHex(TIdBytes (crom));
    sha_v := IdHashSHA1.HashBytesAsHex(TIdBytes (ROM [i].vrom));
    sha_va := IdHashSHA1.HashBytesAsHex(TIdBytes (vroma));
    sha_vb := IdHashSHA1.HashBytesAsHex(TIdBytes (vromb));
    sha_s := IdHashSHA1.HashBytesAsHex(TIdBytes (ROM [i].srom));
    sha_m := IdHashSHA1.HashBytesAsHex(TIdBytes (ROM [i].mrom));
    IdHashSHA1.Destroy;

  	writeln (f, #9 + '<software name="' + ROM [i].name + '">');
  	writeln (f, #9 + #9 + '<description>' + ROM [i].mname + '</description>');
  	writeln (f, #9 + #9 + '<year>2023</year>');
  	writeln (f, #9 + #9 + '<publisher>vortex</publisher>');
  	writeln (f, #9 + #9 + '<sharedfeat name="release" value="MVS,AES" />');
  	writeln (f, #9 + #9 + '<sharedfeat name="compatibility" value="MVS,AES" />');
  	writeln (f, #9 + #9 + '<part name="cart" interface="neo_cart">');
  	writeln (f, #9 + #9 + #9 + '<dataarea name="maincpu" width="16" endianness="big" size="0x' + inttohex (length (ROM [i].prom), 8) + '">');
  	writeln (f, #9 + #9 + #9 + #9 + '<rom loadflag="load16_word_swap" name="prom" offset="0x000000" size="0x' + inttohex (length (ROM [i].prom), 8) + '" crc="' + crc_p + '" sha1="' + sha_p + '" />');
  	writeln (f, #9 + #9 + #9 + '</dataarea>');
  	writeln (f, #9 + #9 + #9 + '<dataarea name="fixed" size="0x040000">');
  	writeln (f, #9 + #9 + #9 + #9 + '<rom name="srom" offset="0x000000" size="0x' + inttohex (length (ROM [i].srom), 8) + '" crc="' + crc_s + '" sha1="' + sha_s + '" />');
  	writeln (f, #9 + #9 + #9 + '</dataarea>');
  	writeln (f, #9 + #9 + #9 + '<dataarea name="audiocpu" size="0x' + inttohex (length (ROM [i].mrom), 8) + '">');
  	writeln (f, #9 + #9 + #9 + #9 + '<rom name="mrom" offset="0x000000" size="0x' + inttohex (length (ROM [i].mrom), 8) + '" crc="' + crc_m + '" sha1="' + sha_m + '" />');
  	writeln (f, #9 + #9 + #9 + '</dataarea>');
    if (length (vroma) > 0) then
    begin
    	writeln (f, #9 + #9 + #9 + '<dataarea name="ymsnd:adpcma" size="0x' + inttohex (length (vroma), 8) + '">');
    	writeln (f, #9 + #9 + #9 + #9 + '<rom name="vroma" offset="0x000000" size="0x' + inttohex (length (vroma), 8) + '" crc="' + crc_va + '" sha1="' + sha_va + '" />');
    	writeln (f, #9 + #9 + #9 + '</dataarea>');
    	writeln (f, #9 + #9 + #9 + '<dataarea name="ymsnd:adpcmb" size="0x' + inttohex (length (vromb), 8) + '">');
    	writeln (f, #9 + #9 + #9 + #9 + '<rom name="vromb" offset="0x000000" size="0x' + inttohex (length (vromb), 8) + '" crc="' + crc_vb + '" sha1="' + sha_vb + '" />');
    	writeln (f, #9 + #9 + #9 + '</dataarea>');
    end
    else
    if (length (ROM [i].vrom) > 0) then
    begin
    	writeln (f, #9 + #9 + #9 + '<dataarea name="ymsnd:adpcma" size="0x' + inttohex (length (ROM [i].vrom), 8) + '">');
    	writeln (f, #9 + #9 + #9 + #9 + '<rom name="vrom" offset="0x000000" size="0x' + inttohex (length (ROM [i].vrom), 8) + '" crc="' + crc_v + '" sha1="' + sha_v + '" />');
     	writeln (f, #9 + #9 + #9 + '</dataarea>');
    end;
    if (length (ROM [i].crom) > 0) then
    begin
    	writeln (f, #9 + #9 + #9 + '<dataarea name="sprites" size="0x' + inttohex (size_c, 8) + '">');
    	writeln (f, #9 + #9 + #9 + #9 + '<rom loadflag="load8_byte" name="crom" offset="0x000000" size="0x' + inttohex (length (ROM [i].crom), 8) + '" crc="' + crc_c + '" sha1="' + sha_c + '" />');
    	writeln (f, #9 + #9 + #9 + '</dataarea>');
    end else
    begin
    	writeln (f, #9 + #9 + #9 + '<dataarea name="sprites" size="0x' + inttohex (size_c, 8) + '">');
      writeln (f, #9 + #9 + #9 + '</dataarea>');
    end;
  	writeln (f, #9 + #9 + '</part>');
  	writeln (f, #9 + '</software>');
  	writeln (f, #9 + '');

    SetLength (crom, 0);

    closefile (f);

    Write ('.');
  end;

  assign (f, 'MAME\hash\neogeo.xml');
  append (f);
	writeln (f, '</softwarelist>');
  closefile (f);

  WriteLn ('');
  WriteLn ('');
end;

begin
  Writeln ('SNK MultiCart Compiler v1.01 (c) Vortex ''2023');
  sys := 'mvs';
  crom_extend := 0;
  if (paramcount < 1) then
  begin
    WriteLn ('Usage: ', changefileext (extractfilename (paramstr (0)), ''), ' <filename> <command> .. <command> ..');
    Halt;
  end;
  fn := paramstr (1);
  if (not fileexists (fn)) then
  begin
    WriteLog ('Error: ' + fn + ' not exists!');
    Halt;
  end;
  for i := 2 to paramcount do
  begin
    if (lowercase (paramstr (i)) = 'mvs') then sys := 'mvs';
    if (lowercase (paramstr (i)) = 'aes') then sys := 'aes';
    if (lowercase (paramstr (i)) = 'c3g') then crom_extend := $40000000;
  end;
  Import (fn);
  Report;
  for i := 2 to paramcount do
  begin
    if (lowercase (paramstr (i)) = 'genix') then GenIX;
    if (lowercase (paramstr (i)) = 'patchmenu') then PatchMenu;
    if (lowercase (paramstr (i)) = 'genmame') then GenMAME;
    if (lowercase (paramstr (i)) = 'genrom') then GenROM;
  end;
  WriteLog ('Done.');
end.

