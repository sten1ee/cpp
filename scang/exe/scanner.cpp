#include "scanner.h"

// \x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xa\xb\xc\xd\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !"#$%&'()*+,-./:;<=>?@[\\]^`{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff
static const char isgroup0[256] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
// 123456789
static const char isgroup1[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1};
// true
// 0123456789
static const char isgroup3[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1};
// 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
static const char isgroup4[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int action[235] = {0,52,51,51,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,51,50,50,50,50,50,50,50,50,50,50,12,50,50,50,50,50,50,20,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,18,50,50,23,50,50,50,29,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,41,50,50,50,50,50,3,6,50,8,50,50,50,50,13,50,50,50,50,50,50,50,50,50,26,50,30,50,50,50,50,50,50,50,50,50,50,43,50,50,50,42,48,50,50,50,2,5,7,50,50,50,50,15,16,17,50,50,50,50,50,50,50,50,50,50,50,38,50,40,50,50,50,44,50,49,50,50,50,50,11,50,50,50,21,50,50,27,28,50,50,50,34,35,39,36,50,50,45,50,50,4,50,10,14,19,50,50,50,31,32,50,50,50,50,1,9,50,50,50,50,50,50,50,50,50,24,33,50,50,47,22,25,50,46,50,37};

int scanner::next_token()
{
  scan_buffer& b = *buffer;
  b.flush_to_mark();
  int s = 0;
  int a = 0;
  int c;
  while (1) {
    if (action[s]) {
      a = action[s];
      b.mark();
    }
    if ((c = b.get()) < 0) {
      break;
    }
    switch (s) {
    case 0:
      switch (c) {
      case '0': s = 2; break;
      case 'a': s = 5; break;
      case 'b': s = 6; break;
      case 'c': s = 7; break;
      case 'd': s = 8; break;
      case 'e': s = 9; break;
      case 'f': s = 10; break;
      case 'i': s = 11; break;
      case 'l': s = 12; break;
      case 'n': s = 13; break;
      case 'p': s = 14; break;
      case 'r': s = 15; break;
      case 's': s = 16; break;
      case 't': s = 17; break;
      case 'v': s = 18; break;
      case 'w': s = 19; break;
      default:
        if (isgroup0[c]) s = 1;
        else if (isgroup1[c]) s = 3;
        else  s = 4;
      }
      break;
    case 1:
      goto _A_52;
    case 2:
      goto _A_51;
    case 3:
      switch (c) {
      default:
        if (isgroup3[c]) s = 20;
        else goto _A_51;
      }
      break;
    case 4:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 5:
      switch (c) {
      case 'b': s = 22; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 6:
      switch (c) {
      case 'o': s = 23; break;
      case 'r': s = 24; break;
      case 'y': s = 25; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 7:
      switch (c) {
      case 'a': s = 26; break;
      case 'h': s = 27; break;
      case 'l': s = 28; break;
      case 'o': s = 29; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 8:
      switch (c) {
      case 'e': s = 30; break;
      case 'o': s = 31; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 9:
      switch (c) {
      case 'l': s = 32; break;
      case 'x': s = 33; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 10:
      switch (c) {
      case 'a': s = 34; break;
      case 'i': s = 35; break;
      case 'l': s = 36; break;
      case 'o': s = 37; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 11:
      switch (c) {
      case 'f': s = 38; break;
      case 'm': s = 39; break;
      case 'n': s = 40; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 12:
      switch (c) {
      case 'e': s = 41; break;
      case 'o': s = 42; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 13:
      switch (c) {
      case 'a': s = 43; break;
      case 'e': s = 44; break;
      case 'u': s = 45; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 14:
      switch (c) {
      case 'a': s = 46; break;
      case 'r': s = 47; break;
      case 'u': s = 48; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 15:
      switch (c) {
      case 'e': s = 49; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 16:
      switch (c) {
      case 'h': s = 50; break;
      case 't': s = 51; break;
      case 'u': s = 52; break;
      case 'w': s = 53; break;
      case 'y': s = 54; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 17:
      switch (c) {
      case 'h': s = 55; break;
      case 'r': s = 56; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 18:
      switch (c) {
      case 'o': s = 57; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 19:
      switch (c) {
      case 'h': s = 58; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 20:
      switch (c) {
      default:
        if (isgroup3[c]);
        else goto _A_51;
      }
      break;
    case 21:
      switch (c) {
      default:
        if (isgroup4[c]);
        else goto _A_50;
      }
      break;
    case 22:
      switch (c) {
      case 's': s = 59; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 23:
      switch (c) {
      case 'o': s = 60; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 24:
      switch (c) {
      case 'e': s = 61; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 25:
      switch (c) {
      case 't': s = 62; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 26:
      switch (c) {
      case 's': s = 63; break;
      case 't': s = 64; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 27:
      switch (c) {
      case 'a': s = 65; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 28:
      switch (c) {
      case 'a': s = 66; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 29:
      switch (c) {
      case 'n': s = 67; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 30:
      switch (c) {
      case 'f': s = 68; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 31:
      switch (c) {
      case 'u': s = 69; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_12;
      }
      break;
    case 32:
      switch (c) {
      case 's': s = 70; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 33:
      switch (c) {
      case 't': s = 71; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 34:
      switch (c) {
      case 'l': s = 72; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 35:
      switch (c) {
      case 'n': s = 73; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 36:
      switch (c) {
      case 'o': s = 74; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 37:
      switch (c) {
      case 'r': s = 75; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 38:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_20;
      }
      break;
    case 39:
      switch (c) {
      case 'p': s = 76; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 40:
      switch (c) {
      case 's': s = 77; break;
      case 't': s = 78; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 41:
      switch (c) {
      case 'n': s = 79; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 42:
      switch (c) {
      case 'n': s = 80; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 43:
      switch (c) {
      case 't': s = 81; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 44:
      switch (c) {
      case 'w': s = 82; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 45:
      switch (c) {
      case 'l': s = 83; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 46:
      switch (c) {
      case 'c': s = 84; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 47:
      switch (c) {
      case 'i': s = 85; break;
      case 'o': s = 86; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 48:
      switch (c) {
      case 'b': s = 87; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 49:
      switch (c) {
      case 't': s = 88; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 50:
      switch (c) {
      case 'o': s = 89; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 51:
      switch (c) {
      case 'a': s = 90; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 52:
      switch (c) {
      case 'p': s = 91; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 53:
      switch (c) {
      case 'i': s = 92; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 54:
      switch (c) {
      case 'n': s = 93; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 55:
      switch (c) {
      case 'i': s = 94; break;
      case 'r': s = 95; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 56:
      switch (c) {
      case 'a': s = 96; break;
      case 'u': s = 97; break;
      case 'y': s = 98; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 57:
      switch (c) {
      case 'i': s = 99; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 58:
      switch (c) {
      case 'i': s = 100; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 59:
      switch (c) {
      case 't': s = 101; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 60:
      switch (c) {
      case 'l': s = 102; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 61:
      switch (c) {
      case 'a': s = 103; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 62:
      switch (c) {
      case 'e': s = 104; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 63:
      switch (c) {
      case 'e': s = 105; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 64:
      switch (c) {
      case 'c': s = 106; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 65:
      switch (c) {
      case 'r': s = 107; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 66:
      switch (c) {
      case 's': s = 108; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 67:
      switch (c) {
      case 't': s = 109; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 68:
      switch (c) {
      case 'a': s = 110; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 69:
      switch (c) {
      case 'b': s = 111; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 70:
      switch (c) {
      case 'e': s = 112; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 71:
      switch (c) {
      case 'e': s = 113; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 72:
      switch (c) {
      case 's': s = 114; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 73:
      switch (c) {
      case 'a': s = 115; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 74:
      switch (c) {
      case 'a': s = 116; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 75:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_18;
      }
      break;
    case 76:
      switch (c) {
      case 'l': s = 117; break;
      case 'o': s = 118; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 77:
      switch (c) {
      case 't': s = 119; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 78:
      switch (c) {
      case 'e': s = 120; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_23;
      }
      break;
    case 79:
      switch (c) {
      case 'g': s = 121; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 80:
      switch (c) {
      case 'g': s = 122; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 81:
      switch (c) {
      case 'i': s = 123; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 82:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_29;
      }
      break;
    case 83:
      switch (c) {
      case 'l': s = 124; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 84:
      switch (c) {
      case 'k': s = 125; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 85:
      switch (c) {
      case 'v': s = 126; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 86:
      switch (c) {
      case 't': s = 127; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 87:
      switch (c) {
      case 'l': s = 128; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 88:
      switch (c) {
      case 'u': s = 129; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 89:
      switch (c) {
      case 'r': s = 130; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 90:
      switch (c) {
      case 't': s = 131; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 91:
      switch (c) {
      case 'e': s = 132; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 92:
      switch (c) {
      case 't': s = 133; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 93:
      switch (c) {
      case 'c': s = 134; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 94:
      switch (c) {
      case 's': s = 135; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 95:
      switch (c) {
      case 'e': s = 136; break;
      case 'o': s = 137; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 96:
      switch (c) {
      case 'n': s = 138; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 97:
      switch (c) {
      case 'e': s = 139; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 98:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_41;
      }
      break;
    case 99:
      switch (c) {
      case 'd': s = 140; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 100:
      switch (c) {
      case 'l': s = 141; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 101:
      switch (c) {
      case 'r': s = 142; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 102:
      switch (c) {
      case 'e': s = 143; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 103:
      switch (c) {
      case 'k': s = 144; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 104:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_3;
      }
      break;
    case 105:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_6;
      }
      break;
    case 106:
      switch (c) {
      case 'h': s = 145; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 107:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_8;
      }
      break;
    case 108:
      switch (c) {
      case 's': s = 146; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 109:
      switch (c) {
      case 'i': s = 147; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 110:
      switch (c) {
      case 'u': s = 148; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 111:
      switch (c) {
      case 'l': s = 149; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 112:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_13;
      }
      break;
    case 113:
      switch (c) {
      case 'n': s = 150; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 114:
      switch (c) {
      case 'e': s = 151; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 115:
      switch (c) {
      case 'l': s = 152; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 116:
      switch (c) {
      case 't': s = 153; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 117:
      switch (c) {
      case 'e': s = 154; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 118:
      switch (c) {
      case 'r': s = 155; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 119:
      switch (c) {
      case 'a': s = 156; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 120:
      switch (c) {
      case 'r': s = 157; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 121:
      switch (c) {
      case 't': s = 158; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 122:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_26;
      }
      break;
    case 123:
      switch (c) {
      case 'v': s = 159; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 124:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_30;
      }
      break;
    case 125:
      switch (c) {
      case 'a': s = 160; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 126:
      switch (c) {
      case 'a': s = 161; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 127:
      switch (c) {
      case 'e': s = 162; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 128:
      switch (c) {
      case 'i': s = 163; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 129:
      switch (c) {
      case 'r': s = 164; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 130:
      switch (c) {
      case 't': s = 165; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 131:
      switch (c) {
      case 'i': s = 166; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 132:
      switch (c) {
      case 'r': s = 167; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 133:
      switch (c) {
      case 'c': s = 168; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 134:
      switch (c) {
      case 'h': s = 169; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 135:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_43;
      }
      break;
    case 136:
      switch (c) {
      case 'a': s = 170; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 137:
      switch (c) {
      case 'w': s = 171; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 138:
      switch (c) {
      case 's': s = 172; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 139:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_42;
      }
      break;
    case 140:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_48;
      }
      break;
    case 141:
      switch (c) {
      case 'e': s = 173; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 142:
      switch (c) {
      case 'a': s = 174; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 143:
      switch (c) {
      case 'a': s = 175; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 144:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_2;
      }
      break;
    case 145:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_5;
      }
      break;
    case 146:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_7;
      }
      break;
    case 147:
      switch (c) {
      case 'n': s = 176; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 148:
      switch (c) {
      case 'l': s = 177; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 149:
      switch (c) {
      case 'e': s = 178; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 150:
      switch (c) {
      case 'd': s = 179; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 151:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_15;
      }
      break;
    case 152:
      switch (c) {
      case 'l': s = 180; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_16;
      }
      break;
    case 153:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_17;
      }
      break;
    case 154:
      switch (c) {
      case 'm': s = 181; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 155:
      switch (c) {
      case 't': s = 182; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 156:
      switch (c) {
      case 'n': s = 183; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 157:
      switch (c) {
      case 'f': s = 184; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 158:
      switch (c) {
      case 'h': s = 185; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 159:
      switch (c) {
      case 'e': s = 186; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 160:
      switch (c) {
      case 'g': s = 187; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 161:
      switch (c) {
      case 't': s = 188; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 162:
      switch (c) {
      case 'c': s = 189; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 163:
      switch (c) {
      case 'c': s = 190; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 164:
      switch (c) {
      case 'n': s = 191; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 165:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_38;
      }
      break;
    case 166:
      switch (c) {
      case 'c': s = 192; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 167:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_40;
      }
      break;
    case 168:
      switch (c) {
      case 'h': s = 193; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 169:
      switch (c) {
      case 'r': s = 194; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 170:
      switch (c) {
      case 'd': s = 195; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 171:
      switch (c) {
      case 's': s = 196; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_44;
      }
      break;
    case 172:
      switch (c) {
      case 'i': s = 197; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 173:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_49;
      }
      break;
    case 174:
      switch (c) {
      case 'c': s = 198; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 175:
      switch (c) {
      case 'n': s = 199; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 176:
      switch (c) {
      case 'u': s = 200; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 177:
      switch (c) {
      case 't': s = 201; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 178:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_11;
      }
      break;
    case 179:
      switch (c) {
      case 's': s = 202; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 180:
      switch (c) {
      case 'y': s = 203; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 181:
      switch (c) {
      case 'e': s = 204; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 182:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_21;
      }
      break;
    case 183:
      switch (c) {
      case 'c': s = 205; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 184:
      switch (c) {
      case 'a': s = 206; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 185:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_27;
      }
      break;
    case 186:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_28;
      }
      break;
    case 187:
      switch (c) {
      case 'e': s = 207; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 188:
      switch (c) {
      case 'e': s = 208; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 189:
      switch (c) {
      case 't': s = 209; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 190:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_34;
      }
      break;
    case 191:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_35;
      }
      break;
    case 192:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_39;
      }
      break;
    case 193:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_36;
      }
      break;
    case 194:
      switch (c) {
      case 'o': s = 210; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 195:
      switch (c) {
      case 's': s = 211; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 196:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_45;
      }
      break;
    case 197:
      switch (c) {
      case 'e': s = 212; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 198:
      switch (c) {
      case 't': s = 213; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 199:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_4;
      }
      break;
    case 200:
      switch (c) {
      case 'e': s = 214; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 201:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_10;
      }
      break;
    case 202:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_14;
      }
      break;
    case 203:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_19;
      }
      break;
    case 204:
      switch (c) {
      case 'n': s = 215; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 205:
      switch (c) {
      case 'e': s = 216; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 206:
      switch (c) {
      case 'c': s = 217; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 207:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_31;
      }
      break;
    case 208:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_32;
      }
      break;
    case 209:
      switch (c) {
      case 'e': s = 218; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 210:
      switch (c) {
      case 'n': s = 219; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 211:
      switch (c) {
      case 'a': s = 220; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 212:
      switch (c) {
      case 'n': s = 221; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 213:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_1;
      }
      break;
    case 214:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_9;
      }
      break;
    case 215:
      switch (c) {
      case 't': s = 222; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 216:
      switch (c) {
      case 'o': s = 223; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 217:
      switch (c) {
      case 'e': s = 224; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 218:
      switch (c) {
      case 'd': s = 225; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 219:
      switch (c) {
      case 'i': s = 226; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 220:
      switch (c) {
      case 'f': s = 227; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 221:
      switch (c) {
      case 't': s = 228; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 222:
      switch (c) {
      case 's': s = 229; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 223:
      switch (c) {
      case 'f': s = 230; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 224:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_24;
      }
      break;
    case 225:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_33;
      }
      break;
    case 226:
      switch (c) {
      case 'z': s = 231; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 227:
      switch (c) {
      case 'e': s = 232; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 228:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_47;
      }
      break;
    case 229:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_22;
      }
      break;
    case 230:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_25;
      }
      break;
    case 231:
      switch (c) {
      case 'e': s = 233; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 232:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_46;
      }
      break;
    case 233:
      switch (c) {
      case 'd': s = 234; break;
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_50;
      }
      break;
    case 234:
      switch (c) {
      default:
        if (isgroup4[c]) s = 21;
        else goto _A_37;
      }
      break;
    }
  }
 _A:
  if (a == 0) {
    b.mark();
    return (0 <= c ? 0 : -1);
  }
  else {
    b.reset_to_mark();
  }
  switch (a) {
      _A_1: b.reset_to_mark();
    case 1: return 1;
      _A_2: b.reset_to_mark();
    case 2: return 2;
      _A_3: b.reset_to_mark();
    case 3: return 3;
      _A_4: b.reset_to_mark();
    case 4: return 4;
      _A_5: b.reset_to_mark();
    case 5: return 5;
      _A_6: b.reset_to_mark();
    case 6: return 6;
      _A_7: b.reset_to_mark();
    case 7: return 7;
      _A_8: b.reset_to_mark();
    case 8: return 8;
      _A_9: b.reset_to_mark();
    case 9: return 9;
      _A_10: b.reset_to_mark();
    case 10: return 10;
      _A_11: b.reset_to_mark();
    case 11: return 11;
      _A_12: b.reset_to_mark();
    case 12: return 12;
      _A_13: b.reset_to_mark();
    case 13: return 13;
      _A_14: b.reset_to_mark();
    case 14: return 14;
      _A_15: b.reset_to_mark();
    case 15: return 15;
      _A_16: b.reset_to_mark();
    case 16: return 16;
      _A_17: b.reset_to_mark();
    case 17: return 17;
      _A_18: b.reset_to_mark();
    case 18: return 18;
      _A_19: b.reset_to_mark();
    case 19: return 19;
      _A_20: b.reset_to_mark();
    case 20: return 20;
      _A_21: b.reset_to_mark();
    case 21: return 21;
      _A_22: b.reset_to_mark();
    case 22: return 22;
      _A_23: b.reset_to_mark();
    case 23: return 23;
      _A_24: b.reset_to_mark();
    case 24: return 24;
      _A_25: b.reset_to_mark();
    case 25: return 25;
      _A_26: b.reset_to_mark();
    case 26: return 26;
      _A_27: b.reset_to_mark();
    case 27: return 27;
      _A_28: b.reset_to_mark();
    case 28: return 28;
      _A_29: b.reset_to_mark();
    case 29: return 29;
      _A_30: b.reset_to_mark();
    case 30: return 30;
      _A_31: b.reset_to_mark();
    case 31: return 31;
      _A_32: b.reset_to_mark();
    case 32: return 32;
      _A_33: b.reset_to_mark();
    case 33: return 33;
      _A_34: b.reset_to_mark();
    case 34: return 34;
      _A_35: b.reset_to_mark();
    case 35: return 35;
      _A_36: b.reset_to_mark();
    case 36: return 36;
      _A_37: b.reset_to_mark();
    case 37: return 37;
      _A_38: b.reset_to_mark();
    case 38: return 38;
      _A_39: b.reset_to_mark();
    case 39: return 39;
      _A_40: b.reset_to_mark();
    case 40: return 40;
      _A_41: b.reset_to_mark();
    case 41: return 41;
      _A_42: b.reset_to_mark();
    case 42: return 42;
      _A_43: b.reset_to_mark();
    case 43: return 43;
      _A_44: b.reset_to_mark();
    case 44: return 44;
      _A_45: b.reset_to_mark();
    case 45: return 45;
      _A_46: b.reset_to_mark();
    case 46: return 46;
      _A_47: b.reset_to_mark();
    case 47: return 47;
      _A_48: b.reset_to_mark();
    case 48: return 48;
      _A_49: b.reset_to_mark();
    case 49: return 49;
      _A_50: b.reset_to_mark();
    case 50: return 50;
      _A_51: b.reset_to_mark();
    case 51: return 51;
      _A_52: b.reset_to_mark();
    case 52: return 52;
  }
  return -1;
}
