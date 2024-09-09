#ifndef STOMAIMAGEPACK_HEAD_INCLUDE_BARRIER
#define STOMAIMAGEPACK_HEAD_INCLUDE_BARRIER

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

namespace StomaImagePack {
enum class StimpacVer {
	V1,
	/*
	stimpac spec for V1
	Version						| uint32 	| 4
	Resolution					| 1
		Width					| uint32 	| 4
		Height					| uint32 	| 4
	RGBA Pixel Array			| (Width * Height)
		Red						| uint8		| 1
		Green					| uint8		| 1
		Blue					| uint8		| 1
		Alpha					| uint8		| 1
	GlyphCount					| uint32	| 4
	Glyph Array					| GlyphCount
		CharCount				| uint32	| 4
			Name				| CharCount
		Size					| 1
			Width				| uint32	| 4
			Height				| uint32	| 4
		Offset					| 1
			Width				| uint32	| 4
			Height				| uint32	| 4	
	*/
	ENDOF
};

struct Resolution {
	uint32_t Width = 0;
	uint32_t Height = 0;

	Resolution(void){}
	Resolution(uint32_t E){Width = E;Height = E;}
	Resolution(uint32_t I,uint32_t Q){Width = I;Height = Q;}
	// ^ only exist to prevent garbage errors
};
struct Colour {
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
	uint8_t A = 0;
};
struct Glyph {
	std::string Name = "Unnamed";
	StomaImagePack::Resolution Size = {64, 64};
	StomaImagePack::Resolution Offset = {0, 0};
};
struct Image {
	StomaImagePack::Resolution Size = {64, 64};
	std::vector<StomaImagePack::Colour> Pixels{};
	std::vector<StomaImagePack::Glyph> Glyphs{};
};


Image ReadStimpac(std::string NAM);
void WriteStimpac(StomaImagePack::Image IMG, std::string NAM);

} // namespace StomaImagePack

#endif // RGBKS_HEAD_INCLUDE_BARRIER

#ifdef STOMAIMAGEPACK_IMPLEM
#ifndef STOMAIMAGEPACK_BODY_INCLUDE_BARRIER
#define STOMAIMAGEPACK_BODY_INCLUDE_BARRIER

namespace StomaImagePack {
// TODO: fix stimpac read/write resulting in mangled image sizes

void WriteStimpac(Image IMG, std::string NAM) {
	std::vector<uint8_t> CharVector;
	uint32_t GlyphCount;
	uint32_t CharCount;
	uint8_t *CharVoodoo;

	// clang-format off
	uint32_t Version = ((uint32_t)StimpacVer::V1);
	CharVoodoo = (uint8_t*)&Version;
	for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	CharVoodoo = (uint8_t*)&IMG.Size.Width;
	for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	CharVoodoo = (uint8_t*)&IMG.Size.Height;
	for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	for(uint32_t i=0;i<IMG.Pixels.size();i++){
		CharVector.push_back(IMG.Pixels[i].R);
		CharVector.push_back(IMG.Pixels[i].G);
		CharVector.push_back(IMG.Pixels[i].B);
		CharVector.push_back(IMG.Pixels[i].A);
	}
	GlyphCount = IMG.Glyphs.size();
	CharVoodoo = (uint8_t*)&GlyphCount;
	for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	for(uint32_t q=0;q<IMG.Glyphs.size();q++){
		CharCount = IMG.Glyphs[q].Name.size();
		CharVoodoo = (uint8_t*)&CharCount;
		for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		for(uint32_t i=0;i<CharCount;i++){CharVector.push_back(IMG.Glyphs[q].Name[i]);}
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Size.Width;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Size.Height;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Offset.Width;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Offset.Height;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	}
	// write char vector to disk
	std::ofstream WriteFile(NAM + ".stimpac",std::ios::binary);
	WriteFile.write((char*)CharVector.data(),CharVector.size());
	WriteFile.close();
	// clang-format on
}

Image ReadStimpac(std::string NAM) {
	Image OutputImage;
	uint32_t CurrentStimpacVersion;
	std::ifstream ReadFile;
	std::vector<uint8_t> CharVector;
	uint8_t *CharVoodoo;
	char HoldChar;
	uint32_t CurrentPosition = 0;
	uint32_t GlyphCount;
	uint32_t CharCount;

	// clang-format off
	
	// read file into char vector
	ReadFile.open(NAM + ".stimpac",std::ios::binary);
	if(!ReadFile.is_open()) {
		printf("could not read %s.\n", NAM.c_str());
		exit(1);
	}
	while(ReadFile.get(HoldChar)) {
		CharVector.push_back(HoldChar);
	}
	if(!ReadFile.eof()){exit(10);}
	ReadFile.close();
	CurrentPosition = 0;
	CharVoodoo = (uint8_t*)&CurrentStimpacVersion;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	CharVoodoo = (uint8_t*)&OutputImage.Size.Width;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	CharVoodoo = (uint8_t*)&OutputImage.Size.Height;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	OutputImage.Pixels.resize(OutputImage.Size.Width * OutputImage.Size.Height);
	for(uint32_t i=0;i<OutputImage.Pixels.size();i++) {
		OutputImage.Pixels[i] = {
		CharVector[CurrentPosition+0],
		CharVector[CurrentPosition+1],
		CharVector[CurrentPosition+2],
		CharVector[CurrentPosition+3]
		};CurrentPosition+=4;
	}
	CharVoodoo = (uint8_t*)&GlyphCount;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
	OutputImage.Glyphs.resize(GlyphCount);
	for (uint32_t q=0;q<OutputImage.Glyphs.size();q++) {
		CharVoodoo = (uint8_t*)&CharCount;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
		OutputImage.Glyphs[q].Name.resize(CharCount);
		for (uint32_t i=0;i<OutputImage.Glyphs[q].Name.size();i++) {
			OutputImage.Glyphs[q].Name[i]=CharVector[CurrentPosition];CurrentPosition++;
		}
		CharVoodoo = (uint8_t*)&OutputImage.Glyphs[q].Size.Width;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
		CharVoodoo = (uint8_t*)&OutputImage.Glyphs[q].Size.Height;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
		CharVoodoo = (uint8_t*)&OutputImage.Glyphs[q].Offset.Width;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
		CharVoodoo = (uint8_t*)&OutputImage.Glyphs[q].Offset.Height;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	}
	return OutputImage;
	// clang-format on
}

} // namespace StomaImagePack

#endif // STOMAIMAGEPACK_BODY_INCLUDE_BARRIER
#endif // STOMAIMAGEPACK_IMPLEM
