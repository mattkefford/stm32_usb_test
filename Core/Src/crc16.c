#include "crc16.h"

unsigned long crcinit_direct;
unsigned long crcinit_nondirect;

static int 						_order = 0;
static unsigned long 	_poly = 0;
static int 						_refin = 0;
static int 						_refout = 0;

static unsigned long _crcmask;
static unsigned long _crcxor;
static unsigned long _crchighbit;

static unsigned long reflect(unsigned long crc, int bitnum);

int crc16_init_saga_settings(void)
{
	//	Saga CRC parameters
	//	order = 16
	//	poly = 0x8408
	//	direct = 1
	//	crcinit = 0
	//	crcxor = 0
	//	refin = 0
	//	refout = 0
	return(crc16_init(16, 0x8408, 1, 0, 0, 0, 0));
}

int crc16_init(int order, unsigned long poly, int direct, unsigned long crcinit, unsigned long crcxor, int refin, int refout)
{	
	int i;
	unsigned long bit, crc;
	
	// save params to module variables
	_order = order;
	_poly = poly;
	_refin = refin;
	_refout = refout;
	_crcxor = crcxor;
	
	_crcmask = ((((unsigned long)1<<(order-1))-1)<<1)|1;
	_crchighbit = (unsigned long)1<<(order-1);

	// check params
	if (order < 1 || order > 32)
	{
		return 0;
	}

	if (poly != (poly & _crcmask))
	{
		return 0;
	}

	if (crcinit != (crcinit & _crcmask))
	{
		return 0;
	}

	if (crcxor != (crcxor & _crcmask))
	{
		return 0;
	}
	
	if (!direct)
	{
		crcinit_nondirect = crcinit;
		crc = crcinit;
		for (i=0; i<order; i++)
		{
			bit = crc & _crchighbit;
			crc<<= 1;
			if (bit) crc ^= poly;
		}
		crc &= _crcmask;
		crcinit_direct = crc;
	}
	else
	{
		crcinit_direct = crcinit;
		crc = crcinit;
		for (i=0; i<order; i++)
		{
			bit = crc & 1;
			if (bit) crc ^= poly;
			crc >>= 1;
			if (bit) crc |= _crchighbit;
		}	
		crcinit_nondirect = crc;
	}
	return 1;
}

unsigned long crc16_bit_by_bit_fast(unsigned char* p, unsigned long len)
{
	// fast bit by bit algorithm without augmented zero bytes
	// does not use lookup table, suited for polynom orders between 1...32

	unsigned long i, j, c, bit;
	unsigned long crc = crcinit_direct;

	for (i=0; i<len; i++) {

		c = (unsigned long)*p++;
		if (_refin) c = reflect(c, 8);

		for (j=0x80; j; j>>=1) {

			bit = crc & _crchighbit;
			crc<<= 1;
			if (c & j) bit^= _crchighbit;
			if (bit) crc^= _poly;
		}
	}	

if (_refout) crc=reflect(crc, _order);
	crc^= _crcxor;
	crc &= _crcmask;

	return(crc);
}

unsigned long reflect(unsigned long crc, int bitnum)
{
	unsigned long i, j=1, crcout=0;

	for (i=(unsigned long)1<<(bitnum-1); i; i>>=1) {
		if (crc & i) crcout|=j;
		j <<= 1;
	}
	return (crcout);
}
