#include <stdio.h>
#include <stdlib.h>
#include "des.h"

void deskey(key, edf)
unsigned char *key;
short edf;
{
  register int i, j, l, m, n;
  unsigned char pc1m[56], pcr[56];
  unsigned long kn[32];

  for (j=0; j<56; j++) {
    l = pc1[j];
    m = l & 07;
    pc1m[j] = (key[l>>3] & bytebit[m]) ? 1 : 0;
  }

  for (i=0; i<16; i++) {
    if (edf==DE1)
      m = (15-i) << 1;
    else
      m = i << 1;
    n = m + 1;
    kn[m] = kn[n] = 0L;
    for (j=0; j<28; j++) {
      l = j + totrot[i];
      if (l<28)
        pcr[j] = pc1m[l];
      else
        pcr[j] = pc1m[l-28];
    }
    for (j=28; j<56; j++) {
      l = j + totrot[i];
      if (l<56)
        pcr[j] = pc1m[l];
      else
        pcr[j] = pc1m[l-28];
    }
    for (j=0; j<24; j++) {
      if (pcr[pc2[j]])
        kn[m] |= bigbyte[j];
      if (pcr[pc2[j+24]])
        kn[n] |= bigbyte[j];
    }
  }
  cookey(kn);
  return;
}

static void cookey(raw1)
register unsigned long *raw1;
{
  register unsigned long *cook, *raw0;
  unsigned long dough[32];
  register int i;

  cook = dough;
  for (i=0; i<16; i++, raw1++) {
    raw0 = raw1++;
    *cook    = (*raw0 & 0x00fc0000L) << 6;
    *cook   |= (*raw0 & 0x00000fc0L) << 10;
    *cook   |= (*raw1 & 0x00fc0000L) >> 10;
    *cook++ |= (*raw1 & 0x00000fc0L) >> 6;
    *cook    = (*raw0 & 0x0003f000L) << 12;
    *cook   |= (*raw0 & 0x0000003fL) << 16;
    *cook   |= (*raw1 & 0x0003f000L) >> 4;
    *cook++ |= (*raw1 & 0x0000003fL);
  }
  usekey(dough);
  return;
}

void cpkey(into)
register unsigned long *into;
{
  register unsigned long *from, *endp;

  from = KnL, endp = &KnL[32];
  while (from < endp)
    *into++ = *from++;
  return;
}

void usekey(from)
register unsigned long *from;
{
  register unsigned long *to, *endp;

  to = KnL, endp = &KnL[32];
  while (to < endp)
    *to++ = *from++;
  return;
}

void des(inblock, outblock)
unsigned char *inblock, *outblock;
{
  unsigned long work[2];

  scrunch(inblock, work);
  desfunc(work, KnL);
  unscrun(work, outblock);
  return;
}

static void scrunch(outof, into)
register unsigned char *outof;
register unsigned long *into;
{
  *into    = (*outof++ & 0xffL) << 24;
  *into   |= (*outof++ & 0xffL) << 16;
  *into   |= (*outof++ & 0xffL) << 8;
  *into++ |= (*outof++ & 0xffL);
  *into    = (*outof++ & 0xffL) << 24;
  *into   |= (*outof++ & 0xffL) << 16;
  *into   |= (*outof++ & 0xffL) << 8;
  *into   |= (*outof   & 0xffL);
  return;
}

static void unscrun(outof, into)
register unsigned long *outof;
register unsigned char *into;
{
  *into++ = (*outof >> 24) & 0xffL;
  *into++ = (*outof >> 16) & 0xffL;
  *into++ = (*outof >>  8) & 0xffL;
  *into++ = *outof++       & 0xffL;
  *into++ = (*outof >> 24) & 0xffL;
  *into++ = (*outof >> 16) & 0xffL;
  *into++ = (*outof >>  8) & 0xffL;
  *into   = *outof         & 0xffL;
  return;
}

static unsigned long SP1[64] = {
	0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
	0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
	0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
	0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
	0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
	0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
	0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
	0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
	0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
	0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
	0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
	0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
	0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
	0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L };
 
static unsigned long SP2[64] = {
	0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
	0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
	0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
	0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
	0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
	0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
	0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
	0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
	0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
	0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
	0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
	0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
	0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
	0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
	0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
	0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L };
 
static unsigned long SP3[64] = {
	0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
	0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
	0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
	0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
	0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
	0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
	0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
	0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
	0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
	0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
	0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
	0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
	0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
	0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
	0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
	0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L };
 
static unsigned long SP4[64] = {
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
	0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
	0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
	0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
	0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
	0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
	0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
	0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
	0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L };
 
static unsigned long SP5[64] = {
	0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
	0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
	0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
	0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
	0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
	0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
	0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
	0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
	0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
	0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
	0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
	0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
	0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
	0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
	0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
	0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L };
 
static unsigned long SP6[64] = {
	0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
	0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
	0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
	0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
	0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
	0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
	0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
	0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
	0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
	0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
	0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
	0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
	0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
	0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L };
 
static unsigned long SP7[64] = {
	0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
	0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
	0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
	0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
	0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
	0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
	0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
	0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
	0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
	0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
	0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
	0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
	0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
	0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
	0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
	0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L };
 
static unsigned long SP8[64] = {
	0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
	0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
	0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
	0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
	0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
	0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
	0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
	0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
	0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
	0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
	0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
	0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
	0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
	0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
	0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
	0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L };

static void desfunc(block, keys)
register unsigned long *block, *keys;
{
  register unsigned long fval, work, right, leftt;
  register int round;

  leftt = block[0];
  right = block[1];
  work = ((leftt>>4) ^ right) & 0x0f0f0f0fL;
  right ^= work;
  leftt ^= (work<<4);
  work = ((leftt>>16) ^ right) & 0x0000ffffL;
  right ^= work;
  leftt ^= (work<<16);
  work = ((right>>2) ^ leftt) & 0x33333333L;
  leftt ^= work;
  right ^= (work<<2);
  work = ((right>>8) ^ leftt) & 0x00ff00ffL;
  leftt ^= work;
  right ^= (work<<8);
  right = ((right<<1) | ((right>>31) & 1L)) & 0xffffffffL;
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = ((leftt<<1) | ((leftt>>31) & 1L)) & 0xffffffffL;

  for (round=0; round<8; round++) {
    work  = (right<<28) | (right>>4);
    work ^= *keys++;
    fval  = SP7[work       & 0x3fL];
    fval |= SP5[(work>> 8) & 0x3fL];
    fval |= SP3[(work>>16) & 0x3fL];
    fval |= SP1[(work>>24) & 0x3fL];
    work  = right ^ *keys++;
    fval |= SP8[work       & 0x3fL];
    fval |= SP6[(work>> 8) & 0x3fL];
    fval |= SP4[(work>>16) & 0x3fL];
    fval |= SP2[(work>>24) & 0x3fL];
    leftt ^= fval;
    work  = (leftt<<28) | (leftt>>4);
    work ^= *keys++;
    fval  = SP7[work       & 0x3fL];
    fval |= SP5[(work>> 8) & 0x3fL];
    fval |= SP3[(work>>16) & 0x3fL];
    fval |= SP1[(work>>24) & 0x3fL];
    work  = leftt ^ *keys++;
    fval |= SP8[work       & 0x3fL];
    fval |= SP6[(work>> 8) & 0x3fL];
    fval |= SP4[(work>>16) & 0x3fL];
    fval |= SP2[(work>>24) & 0x3fL];
    right ^= fval;
  }

  right = (right<<31) | (right>>1);
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = (leftt<<31) | (leftt>>1);
  work = ((leftt>>8) ^ right) & 0x00ff00ffL;
  right ^= work;
  leftt ^= (work<<8);
  work = ((leftt>>2) ^right) & 0x33333333L;
  right ^= work;
  leftt ^= (work<<2);
  work = ((right>>16) ^ leftt) & 0x0000ffffL;
  leftt ^= work;
  right ^= (work<<16);
  work = ((right>>4) ^ leftt) & 0x0f0f0f0fL;
  leftt ^= work;
  right ^= (work<<4);
  *block++ = right;
  *block = leftt;
  return;
}

void des_key(des_ctx *dc, unsigned char *key) {
  deskey(key, EN0);
  cpkey(dc->ek);
  deskey(key, DE1);
  cpkey(dc->dk);
}

void des_enc(des_ctx *dc, unsigned char *data, int blocks) {
  unsigned long work[2];
  int i;
  unsigned char *cp;

  cp = data;
  for (i=0; i<blocks; i++) {
    scrunch(cp, work);
    desfunc(work, dc->ek);
    unscrun(work, cp);
    cp += 8;
  }
}

void des_dec(des_ctx *dc, unsigned char *data, int blocks) {
  unsigned long work[2];
  int i;
  unsigned char *cp;

  cp = data;
  for (i=0; i<blocks; i++) {
    scrunch(cp, work);
    desfunc(work, dc->dk);
    unscrun(work, cp);
    cp += 8;
  }
}

/* Triple DES: C = E_k3(D_k2(E_k1(P))) */
void triple_des_enc(des_ctx *dc1, des_ctx *dc2, des_ctx *dc3, unsigned char *block) {
    des_enc(dc1, block, 1);
    des_dec(dc2, block, 1);
    des_enc(dc3, block, 1);
}

/* Triple DES: P = D_k1(E_k2(D_k3(C))) */
void triple_des_dec(des_ctx *dc1, des_ctx *dc2, des_ctx *dc3, unsigned char *block) {
    des_dec(dc3, block, 1);
    des_enc(dc2, block, 1);
    des_dec(dc1, block, 1);
}

void stringToByteArray(const char *string, unsigned char *byteArray) {
  char hexByte[3];
  hexByte[2] = '\0';

  for(int i = 0; i < 8; i++) {
    hexByte[0] = string[i*2];
    hexByte[1] = string[(i*2) + 1];
    byteArray[i] = (unsigned char) strtoul(hexByte, NULL, 16);
  }
}

void byteArrayToString(unsigned char *byteArray, char *string) {
    for(int i = 0; i < 8; i++) {
        sprintf(&string[i*2], "%02X", byteArray[i]);
    }
    string[16] = '\0'; 
}

int main(void) {
    des_ctx dc[5][3];
    char keyStr[17], ptStr[17], ctInStr[17];
    unsigned char keyBytes[8];
    unsigned char pt[5][8];
    unsigned char ctIn[5][8];
    unsigned char tempBlock[8];
    char outStr[17], filename[32];
    FILE *fp;

    /* 1. Read 5 sets of keys (3 keys per set = 15 keys total) from keys.txt */
    fp = fopen("keys.txt", "r");
    if (!fp) {
        printf("Error: Could not open keys.txt\n");
        return 1;
    }
    for (int s = 0; s < 5; s++) {
        for (int k = 0; k < 3; k++) {
            if (fscanf(fp, "%16s", keyStr) != 1) {
                printf("Error: Incomplete keys.txt (expected 15 keys across 5 sets)\n");
                fclose(fp);
                return 1;
            }
            stringToByteArray(keyStr, keyBytes);
            des_key(&dc[s][k], keyBytes);
        }
    }
    fclose(fp);

    /* 2. Read 5 plaintext blocks from plaintextin.txt */
    fp = fopen("plaintextin.txt", "r");
    if (!fp) {
        printf("Error: Could not open plaintextin.txt\n");
        return 1;
    }
    for (int i = 0; i < 5; i++) {
        if (fscanf(fp, "%16s", ptStr) != 1) {
            printf("Error reading plaintextin.txt at line %d\n", i + 1);
            fclose(fp);
            return 1;
        }
        stringToByteArray(ptStr, pt[i]);
    }
    fclose(fp);

    /* 3. Encrypt: 5 sets * 5 plaintexts = 25 outputs into ciphertextout1.txt ... ciphertextout5.txt */
    printf("=== Starting Triple DES Encryption ===\n");
    for (int s = 0; s < 5; s++) {
        sprintf(filename, "ciphertextout%d.txt", s + 1);
        fp = fopen(filename, "w");
        if (!fp) {
            printf("Error: Could not create %s\n", filename);
            return 1;
        }

        printf("Set %d Encryption:\n", s + 1);
        for (int p = 0; p < 5; p++) {
            for (int b = 0; b < 8; b++) {
                tempBlock[b] = pt[p][b];
            }

            triple_des_enc(&dc[s][0], &dc[s][1], &dc[s][2], tempBlock);
            byteArrayToString(tempBlock, outStr);
            fprintf(fp, "%s\n", outStr);
            printf("  PT[%d] -> CT: %s\n", p + 1, outStr);
        }
        fclose(fp);
    }
    printf("Encryption complete. Generated ciphertextout1.txt through ciphertextout5.txt\n\n");

    /* 4. Read 5 ciphertext test cases from ciphertextin.txt */
    fp = fopen("ciphertextin.txt", "r");
    if (!fp) {
        printf("ciphertext.in next\n");
        return 0;
    }
    for (int i = 0; i < 5; i++) {
        stringToByteArray(ctInStr, ctIn[i]);
    }
    fclose(fp);

    /* 5. Decrypt: 5 sets * 5 ciphertexts = 25 outputs into plaintextout1.txt ... plaintextout5.txt */
    printf("=== Starting Triple DES Decryption ===\n");
    for (int s = 0; s < 5; s++) {
        sprintf(filename, "plaintextout%d.txt", s + 1);
        fp = fopen(filename, "w");
        if (!fp) {
            printf("Error: Could not create %s\n", filename);
            return 1;
        }

        printf("Set %d Decryption:\n", s + 1);
        for (int c = 0; c < 5; c++) {
            for (int b = 0; b < 8; b++) {
                tempBlock[b] = ctIn[c][b];
            }

            triple_des_dec(&dc[s][0], &dc[s][1], &dc[s][2], tempBlock);
            byteArrayToString(tempBlock, outStr);
            fprintf(fp, "%s\n", outStr);
            printf("  CT[%d] -> PT: %s\n", c + 1, outStr);
        }
        fclose(fp);
    }
    printf("Decryption complete. Generated plaintextout1.txt through plaintextout5.txt\n");

    return 0;
}