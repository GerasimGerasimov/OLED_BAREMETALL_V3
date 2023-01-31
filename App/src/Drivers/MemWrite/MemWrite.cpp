#include "MemWrite.h"
#include "FlashData.h"
#include "RAMdata.h"

void MemWrite::writeValueToFlash(TInternalMemAddress& Addr, std::string& value) {
	u32 v = std::stol(value, nullptr, 16);
	u8* d = (u8*)&FLASH_DATA + Addr.Offset;
	if (Addr.BitNumber == -1) {
		memcpy(d, (u8*)&v, Addr.Lenght);
	}
	else {//сюда пришёл Offset, BitNumber и Lenght
		u8 data = *d;
		data = (v != 0) ? (data | (1<< Addr.BitNumber)) : ( data & ~(1 << Addr.BitNumber));
		memcpy(d, &data, Addr.Lenght);
	}
}
/*TODO адаптировать для записи FLASH CD RAM*/