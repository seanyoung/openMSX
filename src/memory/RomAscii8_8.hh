#ifndef ROMASCII8_8_HH
#define ROMASCII8_8_HH

#include "RomBlocks.hh"

namespace openmsx {

class RomAscii8_8 final : public Rom8kBBlocks
{
public:
	enum SubType { ASCII8_8, KOEI_8, KOEI_32, WIZARDRY, ASCII8_2 };
	RomAscii8_8(const DeviceConfig& config,
	            Rom&& rom, SubType subType);

	void reset(EmuTime::param time) override;
	byte readMem(word address, EmuTime::param time) override;
	void writeMem(word address, byte value, EmuTime::param time) override;
	const byte* getReadCacheLine(word start) const override;
	byte* getWriteCacheLine(word address) const override;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const byte sramEnableBit;
	const byte sramPages;
	byte sramEnabled;
	byte sramBlock[NUM_BANKS];
};

} // namespace openmsx

#endif
