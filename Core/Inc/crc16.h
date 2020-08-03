// Module to calculate the CRC16 checksum
// Add the .h/.c files to your project
// Call the init function first, in your main
// When you need a checksum, pass a buffer and length
// to the bit_by_bit_fast function

int crc16_init_saga_settings(void);
int crc16_init(int order, unsigned long poly, int direct, unsigned long crcinit, unsigned long crcxor, int refin, int refout);
unsigned long crc16_bit_by_bit_fast(unsigned char* p, unsigned long len);
