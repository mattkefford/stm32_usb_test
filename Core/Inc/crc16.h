
int crc16_init_saga_settings(void);
int crc16_init(int order, unsigned long poly, int direct, unsigned long crcinit, unsigned long crcxor, int refin, int refout);
unsigned long crc16_bit_by_bit_fast(unsigned char* p, unsigned long len);
