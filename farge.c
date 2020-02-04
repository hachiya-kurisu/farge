#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>

enum { FARGE_ANSI, FARGE_HTML, FARGE_PALETTE };

struct color {
  int r, g, b;
};

int wild = 0;

struct color palette[94] = {
  { -1, -1, -1 }, { 32, 32, 32 }, { 64, 64, 64 }, { 96, 96, 96 }, //  !"#
  { 128, 128, 128 }, { 160, 160, 160 }, { 192, 192, 192 }, { 224, 224, 224 }, // $%&'
  { 240, 240, 240 }, { 255, 255, 255 }, { 255, 255, 224 }, { 255, 224, 224 }, // ()*+
  { 44, 15, 82 }, { 174, 31, 99 }, { 255, 79, 104 }, { 255, 247, 248 }, // ,-./
  { 255, 129, 70 }, { 255, 218, 74 }, { 46, 104, 217 }, { 52, 231, 236 }, // 0123
  { 112, 171, 104 }, { 126, 188, 117 }, { 151, 205, 143 }, { 90, 165, 120 }, // 4567
  { 107, 188, 139 }, { 135, 205, 161 }, { 72, 144, 146 }, { 101, 172, 174 }, // 89:;
  { 124, 185, 187 }, { 39, 123, 158 }, { 50, 135, 170 }, { 66, 153, 189 }, // <=?>
  { 57, 66, 144 }, { 74, 83, 160 }, { 92, 101, 175 }, { 107, 64, 152 }, // @ABC
  { 128, 76, 183 }, { 148, 95, 202 }, { 136, 55, 120 }, { 157, 68, 140 }, // DEFG
  { 180, 89, 162 }, { 152, 59, 80 }, { 175, 68, 92 }, { 193, 97, 118 }, // HIJK
  { 149, 52, 52 }, { 178, 65, 65 }, { 207, 90, 90 }, { 162, 60, 51 }, // LMNO
  { 188, 72, 62 }, { 214, 102, 92 }, { 212, 135, 169 }, { 234, 156, 192 }, // PQRS
  { 245, 193, 217 }, { 178, 150, 108 }, { 208, 182, 142 }, { 226, 204, 170 }, // TUVW
  { 161, 108, 68 }, { 200, 183, 90 }, { 217, 157, 110 }, { 88, 65, 47 }, // XYZ[
  { 110, 81, 58 }, { 157, 116, 84 }, { 65, 47, 35 }, { 81, 59, 43 }, // \]^_
  { 114, 85, 61 }, { 82, 50, 36 }, { 108, 62, 44 }, { 142, 83, 58 }, // `abc
  { 57, 40, 33 }, { 70, 48, 39 }, { 99, 69, 57 }, { 32, 24, 23 }, // defg
  { 50, 41, 40 }, { 81, 70, 67 }, { 41, 47, 50 }, { 73, 78, 81 }, // hijk
  { 99, 101, 103 }, { 117, 86, 153 }, { 143, 251, 202 }, { 247, 255, 179 }, // lmno
  { 255, 179, 202 }, { 217, 191, 215 }, { 117, 70, 76 }, { 174, 86, 116 }, // pqrs
  { 242, 106, 123 }, { 255, 167, 165 }, { 255, 224, 132 }, { 255, 231, 215 }, // tuvw
  { 106, 220, 189 }, { 36, 172, 185 }, { 255, 153, 121 }, { 225, 218, 189 }, // xyz{
  { 225, 118, 163 }, { 108, 193, 140 } // |}
};

void parsepalette(FILE *rd) {
  char buffer[LINE_MAX];
  char *meta = fgets(buffer, LINE_MAX, rd);
  int c, r, g, b;
  struct color *clr;
  while(*meta) {
    c = *meta;
    if(!isdigit(*(meta + 1))) return;
    r = strtol(meta + 1, &meta, 10);
    if(!isdigit(*(meta + 1))) return;
    g = strtol(meta + 1, &meta, 10);
    if(!isdigit(*(meta + 1))) return;
    b = strtol(meta + 1, &meta, 10);
    if(c >= 32 && c < 126) {
      clr = &palette[c - 32];
      clr->r = r;
      clr->g = g;
      clr->b = b;
    }
    if(*meta != ';') return;
    meta++;
  }
}

void farge(FILE *rd, FILE *wr, int mode) {
  int ch;
  int meta = 0;
  struct color *c;
  if(mode == FARGE_HTML) fprintf(wr, "<pre>");

  while((ch = fgetc(rd)) != EOF) {
    if(!meta && ch == '#') {
      parsepalette(rd);
      meta = 1;
      continue;
    }

    if(mode == FARGE_PALETTE) return;


    meta = 1;
    if(ch >= 32 && ch < 126) {
      c = &palette[ch - 32];
      if(c->r == -1 && c->g == -1 && c->b == -1) {
        printf("  ");
        continue;
      }
      switch(mode) {
        case FARGE_ANSI:
          fprintf(wr, "\033[48;2;%d;%d;%dm  ", c->r, c->g, c->b);
          fprintf(wr, "\033[0m");
          break;
        case FARGE_HTML:
          fprintf(wr, "<span style='background: rgb(%d,%d,%d);'>  </span>",
              c->r, c->g, c->b);
          break;
      }
    } else {
      switch(ch) {
        case '\n':
        case '\r':
          if(wild) continue;
          mode == FARGE_ANSI ? putc(ch, wr) : fprintf(wr, "<br>");
          break;
      }
    }
  }
  if(mode == FARGE_HTML) fprintf(wr, "</pre>");
  printf("\n");
}

int usage() {
  printf("farge [-hnpw] [file ...]\n");
  return 0;
}

int main(int argc, char *argv[]) {
  int c;
  int mode = FARGE_ANSI;

  while((c = getopt(argc, argv, "hnpw")) != -1) {
    switch(c) {
      case 'h': return usage();
      case 'w': mode = FARGE_HTML; break;
      case 'p': mode = FARGE_PALETTE; break;
      case 'n': wild = 1;
    }
  }
  argc -= optind; argv += optind;

  for(int i = 0; i < argc; i++) {
    FILE *rd;

    rd = !strncmp(argv[i], "-", strlen(argv[i])) ? stdin : fopen(argv[i], "r");
    if(!rd) fprintf(stderr, "%s: %s: no such file\n", argv[0], argv[i]), exit(1);

    farge(rd, stdout, mode);
    fclose(rd);

    if(mode == FARGE_PALETTE) {
      int line = 0;
      struct color *c;
      for(int i = 0; i < 94; i++) {
        c = &palette[i];
        if(c->r == -1 && c->g == -1 && c->b == -1) continue;
        printf("%c\033[48;2;%d;%d;%dm  ", i + 32, c->r, c->g, c->b);
        printf("\033[0m (%.3d,%.3d,%.3d)  ", c->r, c->g, c->b);
        line += 14;
        if(line > 52) {
          line = 0; printf("\n");
        }
      }
      printf("\n");
    }
  }
  return 0;
}

