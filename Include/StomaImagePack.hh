#ifndef STOMAIMAGEPACK_HEAD_INCLUDE_BARRIER
#define STOMAIMAGEPACK_HEAD_INCLUDE_BARRIER

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

namespace StomaImagePack {
// clang-format off
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
GlyphArray					| GlyphCount
	CharCount				| uint32	| 4
		Name				| CharCount
	Size					| 1
		Width				| uint32	| 4
		Height				| uint32	| 4
	Offset					| 1
		Width				| uint32	| 4
		Height				| uint32	| 4	
*/
V2,
/*
// NOTE: replaces GlyphCount onwards
GlyphGroupCount				| uint32	| 4
GlyphGroupArray				| GlyphGroupCount
	GroupNameCharCount		| uint32	| 4
		GroupNameChar		| CharCount
	GroupType				| uint32	| 4
	GlyphCount				| uint32	| 4
	Glyph Array				| GlyphCount
		CharCount			| uint32	| 4
			Name			| CharCount
		Size				| 1
			Width			| uint32	| 4
			Height			| uint32	| 4
		Offset				| 1
			Width			| uint32	| 4
			Height			| uint32	| 4	
*/
ENDOF
};
// clang-format on

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

enum class GroupType{
	REGULAR,
	FONT,
	NORMALMAP,

	ENDOF
};
struct Glyph {
	std::string Name = "";
	StomaImagePack::Resolution Size = {64, 64};
	StomaImagePack::Resolution Offset = {0, 0};
};
struct GlyphGroup{
	std::string Name = "";
	StomaImagePack::GroupType Type;
	std::vector<StomaImagePack::Glyph> Glyphs{};
};
struct Image {
	StomaImagePack::Resolution Size = {64, 64};
	std::vector<StomaImagePack::GlyphGroup> Groups{};
	std::vector<StomaImagePack::Colour> Pixels{};
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
	//Version					| uint32 	| 4
	// NOTE: version should always be latest when writing 
	uint32_t Version = ((uint32_t)StimpacVer::ENDOF - 1);
	CharVoodoo = (uint8_t*)&Version;
	for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	// Resolution				| 1
	// Width					| uint32 	| 4
	CharVoodoo = (uint8_t*)&IMG.Size.Width;
	for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	// Height					| uint32 	| 4
	CharVoodoo = (uint8_t*)&IMG.Size.Height;
	for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	// RGBA Pixel Array			| (Width * Height)
	for(uint32_t i=0;i<IMG.Pixels.size();i++){
		// Red					| uint8		| 1
		CharVector.push_back(IMG.Pixels[i].R);
		// Green				| uint8		| 1
		CharVector.push_back(IMG.Pixels[i].G);
		// Blue					| uint8		| 1
		CharVector.push_back(IMG.Pixels[i].B);
		// Alpha				| uint8		| 1
		CharVector.push_back(IMG.Pixels[i].A);
	}
	// v1 
	#ifdef FORCE_OUTPUT_VERSION_1
	// GlyphCount					| uint32	| 4
	GlyphCount = IMG.Glyphs.size();
	CharVoodoo = (uint8_t*)&GlyphCount;
	for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	// Glyph Array				| GlyphCount
	for(uint32_t q=0;q<IMG.Glyphs.size();q++){
		// CharCount			| uint32	| 4
		CharCount = IMG.Glyphs[q].Name.size();
		CharVoodoo = (uint8_t*)&CharCount;
		for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// Name					| CharCount
		for(uint32_t i=0;i<CharCount;i++){CharVector.push_back(IMG.Glyphs[q].Name[i]);}
		// Size					| 1
		// Width				| uint32	| 4
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Size.Width;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// Height				| uint32	| 4
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Size.Height;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// Offset				| 1
		// Width				| uint32	| 4
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Offset.Width;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// Height				| uint32	| 4
		CharVoodoo = (uint8_t*)&IMG.Glyphs[q].Offset.Height;
		for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	}
	#else
	// v2
	// glpyh group count
	// GlyphGroupCount			| uint32	| 4
	uint32_t GlyphGroupCount = IMG.Groups.size();
	CharVoodoo = (uint8_t*)&GlyphGroupCount;
	for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
	// GlyphGroupArray			| GlyphGroupCount
	for (uint32_t gr = 0;gr<GlyphGroupCount;gr++) {
		// GroupNameCharCount	| uint32	| 4
		uint32_t GlyphGroupCharCount = IMG.Groups[gr].Name.size();
		CharVoodoo = (uint8_t*)&GlyphGroupCharCount;
		for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// GroupName		| CharCount
		for(uint32_t i=0;i<GlyphGroupCharCount;i++){CharVector.push_back(IMG.Groups[gr].Name[i]);}
		// GroupType		| uint32	| 4
		uint32_t GroupType = (uint32_t)IMG.Groups[gr].Type;
		CharVoodoo = (uint8_t*)&GroupType;
		for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// GlyphCount		| uint32	| 4
		uint32_t GlyphCount = IMG.Groups[gr].Glyphs.size();
		CharVoodoo = (uint8_t*)&GlyphCount;
		for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		// Glyph Array		| GlyphCount
		for(uint32_t gl=0;gl<IMG.Groups[gr].Glyphs.size();gl++){
			// CharCount		| uint32	| 4
			CharCount = IMG.Groups[gr].Glyphs[gl].Name.size();
			CharVoodoo = (uint8_t*)&CharCount;
			for(uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
			// Name				| CharCount
			for(uint32_t i=0;i<CharCount;i++){CharVector.push_back(IMG.Groups[gr].Glyphs[gl].Name[i]);}
			// Size				| 1
			// Width			| uint32	| 4
			CharVoodoo = (uint8_t*)&IMG.Groups[gr].Glyphs[gl].Size.Width;
			for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
			// Height			| uint32	| 4
			CharVoodoo = (uint8_t*)&IMG.Groups[gr].Glyphs[gl].Size.Height;
			for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
			// Offset			| 1
			// Width			| uint32	| 4
			CharVoodoo = (uint8_t*)&IMG.Groups[gr].Glyphs[gl].Offset.Width;
			for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
			// Height			| uint32	| 4
			CharVoodoo = (uint8_t*)&IMG.Groups[gr].Glyphs[gl].Offset.Height;
			for (uint32_t i=0;i<4;i++){CharVector.push_back(CharVoodoo[i]);}
		}
	}
	#endif
	//
	// write char vector to disk
	std::ofstream WriteFile(NAM + ".stimpac",std::ios::binary);
	WriteFile.write((char*)CharVector.data(),CharVector.size());
	WriteFile.close();
	// clang-format on
}

Image ReadStimpac(std::string NAM) {
	Image ReturnImage;
	uint32_t FileVersion;
	std::ifstream ReadFile;
	std::vector<uint8_t> CharVector;
	uint8_t *CharVoodoo;
	char HoldChar;
	uint32_t CurrentPosition = 0;
	uint32_t GlyphCount;
	uint32_t GroupCount;
	uint32_t CharCount;
	uint32_t GroupTypeData;

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
	// Version						| uint32 	| 4
	CharVoodoo = (uint8_t*)&FileVersion;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	// Resolution					| 1
	// Width						| uint32 	| 4
	CharVoodoo = (uint8_t*)&ReturnImage.Size.Width;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	// Height						| uint32 	| 4
	CharVoodoo = (uint8_t*)&ReturnImage.Size.Height;
	for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
	// RGBA Pixel Array				| (Width * Height)
	ReturnImage.Pixels.resize(ReturnImage.Size.Width * ReturnImage.Size.Height);
	for(uint32_t i=0;i<ReturnImage.Pixels.size();i++) {
		ReturnImage.Pixels[i] = {
	//	Red							| uint8		| 1
		CharVector[CurrentPosition+0],
	//	Green						| uint8		| 1
		CharVector[CurrentPosition+1],
	//	Blue						| uint8		| 1
		CharVector[CurrentPosition+2],
	//	Alpha						| uint8		| 1
		CharVector[CurrentPosition+3]
		};CurrentPosition+=4;
	}
	switch ((StimpacVer)FileVersion) {
	case(StimpacVer::V1):{
		ReturnImage.Groups.resize(1);
		ReturnImage.Groups[0].Name = "Stimpac Version 1 converted file";
		// GlyphCount					| uint32	| 4
		CharVoodoo = (uint8_t*)&GlyphCount;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
		ReturnImage.Groups[0].Glyphs.resize(GlyphCount);
		// Glyph Array					| GlyphCount
		for (uint32_t q=0;q<ReturnImage.Groups[0].Glyphs.size();q++) {
			//	CharCount				| uint32	| 4
			CharVoodoo = (uint8_t*)&CharCount;
			for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
			//	Name					| CharCount
			ReturnImage.Groups[0].Glyphs[q].Name.resize(CharCount);
			for (uint32_t i=0;i<ReturnImage.Groups[0].Glyphs[q].Name.size();i++) {
				ReturnImage.Groups[0].Glyphs[q].Name[i]=CharVector[CurrentPosition];CurrentPosition++;
			}
			//	Size					| 1
			//	Width					| uint32	| 4
			CharVoodoo = (uint8_t*)&ReturnImage.Groups[0].Glyphs[q].Size.Width;
			for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
			//	Height					| uint32	| 4
			CharVoodoo = (uint8_t*)&ReturnImage.Groups[0].Glyphs[q].Size.Height;
			for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
			//	Offset					| 1
			//	Width					| uint32	| 4
			CharVoodoo = (uint8_t*)&ReturnImage.Groups[0].Glyphs[q].Offset.Width;
			for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
			//	Height					| uint32	| 4
			CharVoodoo = (uint8_t*)&ReturnImage.Groups[0].Glyphs[q].Offset.Height;
			for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
		}
		break;
	}
	case(StimpacVer::V2):{
		// GlyphGroupCount				| uint32	| 4
		CharVoodoo = (uint8_t*)&GroupCount;
		for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
		ReturnImage.Groups.resize(GroupCount);
		// GlyphGroupArray				| GlyphGroupCount
			for(uint32_t z=0;z<ReturnImage.Groups.size();z++){
				// GroupNameCharCount	| uint32	| 4
				CharVoodoo = (uint8_t*)&CharCount;
				for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
				// GroupNameChar		| CharCount
				ReturnImage.Groups[z].Name.resize(CharCount);
				for (uint32_t i=0;i<ReturnImage.Groups[z].Name.size();i++) {
					ReturnImage.Groups[z].Name[i] = CharVector[CurrentPosition];CurrentPosition++;
				}
				// GroupType				| uint32	| 4
				CharVoodoo = (uint8_t*)&GroupTypeData;
				for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
				ReturnImage.Groups[z].Type = (GroupType)GroupTypeData;
				// GlyphCount				| uint32	| 4
				CharVoodoo = (uint8_t*)&GlyphCount;
				for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
				ReturnImage.Groups[z].Glyphs.resize(GlyphCount);
				// Glyph Array				| GlyphCount
				for (uint32_t q=0;q<ReturnImage.Groups[z].Glyphs.size();q++) {
					// CharCount			| uint32	| 4
					CharVoodoo = (uint8_t*)&CharCount;
					for(uint32_t i=0;i<4;i++) {CharVoodoo[i] = CharVector[CurrentPosition];CurrentPosition++;}
					ReturnImage.Groups[z].Glyphs[q].Name.resize(CharCount);
					// Name				| CharCount
					for (uint32_t i=0;i<ReturnImage.Groups[0].Glyphs[q].Name.size();i++) {
						ReturnImage.Groups[z].Glyphs[q].Name[i]=CharVector[CurrentPosition];CurrentPosition++;
					}
					// Size				| 1
					// Width			| uint32	| 4
					CharVoodoo = (uint8_t*)&ReturnImage.Groups[z].Glyphs[q].Size.Width;
					for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
					// Height			| uint32	| 4
					CharVoodoo = (uint8_t*)&ReturnImage.Groups[z].Glyphs[q].Size.Height;
					for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
					// Offset			| 1
					// Width			| uint32	| 4
					CharVoodoo = (uint8_t*)&ReturnImage.Groups[z].Glyphs[q].Offset.Width;
					for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
					// Height			| uint32	| 4
					CharVoodoo = (uint8_t*)&ReturnImage.Groups[z].Glyphs[q].Offset.Height;
					for(uint32_t i=0;i<4;i++) {CharVoodoo[i]=CharVector[CurrentPosition];CurrentPosition++;}
			}
		}

		break;
	}
	default:{
		printf("invalid or corrupted stimpac version\n");
		exit(43);
	}
	
	}
	return ReturnImage;
	// clang-format on
}

} // namespace StomaImagePack

#endif // STOMAIMAGEPACK_BODY_INCLUDE_BARRIER
#endif // STOMAIMAGEPACK_IMPLEM
