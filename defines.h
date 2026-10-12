//defines.h
#define SETBIT(WORD,BIT)  (WORD|=(1<< BIT))
#define CLRBIT(WORD,BIT)  (WORD&=~(1<<BIT))
#define SSETBIT(WORD,BIT) (WORD= (1<<BIT))
#define SCLRBIT(WORD,BIT) (WORD=1<<BIT)

#define WRITEBYTE(WORD,POS,DATA) \
    WORD = (((WORD)&~(0XFF <<POS))|((DATA & 0XFF) << (POS)))
