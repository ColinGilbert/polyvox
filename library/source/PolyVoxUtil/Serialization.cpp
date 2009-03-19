#include "PolyVoxUtil/Serialization.h"

#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/VolumeIterator.h"
#include "PolyVoxCore/Utility.h"

using namespace std;

namespace PolyVox
{
	//Note: we don't do much error handling in here - exceptions will simply be propergated up to the caller.
	//FIXME - think about pointer ownership issues. Or could return volume by value if the copy constructor is shallow
	Volume<uint8>* loadVolumeRaw(istream& stream)
	{
		//Read volume dimensions
		uint8 volumeWidthPower = 0;
		uint8 volumeHeightPower = 0;
		uint8 volumeDepthPower = 0;
		stream.read(reinterpret_cast<char*>(&volumeWidthPower), sizeof(volumeWidthPower));
		stream.read(reinterpret_cast<char*>(&volumeHeightPower), sizeof(volumeHeightPower));
		stream.read(reinterpret_cast<char*>(&volumeDepthPower), sizeof(volumeDepthPower));
		
		//FIXME - need to support non cubic volumes
		Volume<uint8>* volume = new Volume<uint8>(volumeWidthPower);

		uint16 volumeWidth = 0x0001 << volumeWidthPower;
		uint16 volumeHeight = 0x0001 << volumeHeightPower;
		uint16 volumeDepth = 0x0001 << volumeDepthPower;

		//Read data
		for(uint16 z = 0; z < volumeDepth; ++z)
		{
			for(uint16 y = 0; y < volumeHeight; ++y)
			{
				for(uint16 x = 0; x < volumeWidth; ++x)
				{
					uint8 value = 0;
					stream.read(reinterpret_cast<char*>(&value), sizeof(value));
			
					volume->setVoxelAt(x,y,z,value);
				}
			}
		}

		return volume;
	}

	void saveVolumeRaw(std::ostream& stream, Volume<uint8>& volume)
	{
		//Write volume dimensions
		uint16 volumeWidth = volume.getSideLength();
		uint16 volumeHeight = volume.getSideLength();
		uint16 volumeDepth  = volume.getSideLength();

		uint8 volumeWidthPower = logBase2(volumeWidth);
		uint8 volumeHeightPower = logBase2(volumeHeight);
		uint8 volumeDepthPower = logBase2(volumeDepth);

		stream.write(reinterpret_cast<char*>(&volumeWidthPower), sizeof(volumeWidthPower));
		stream.write(reinterpret_cast<char*>(&volumeHeightPower), sizeof(volumeHeightPower));
		stream.write(reinterpret_cast<char*>(&volumeDepthPower), sizeof(volumeDepthPower));

		//Write data
		VolumeIterator<uint8> volIter(volume);
		for(uint16 z = 0; z < volumeDepth; ++z)
		{
			for(uint16 y = 0; y < volumeHeight; ++y)
			{
				for(uint16 x = 0; x < volumeWidth; ++x)
				{
					volIter.setPosition(x,y,z);
					uint8 value = volIter.getVoxel();
					stream.write(reinterpret_cast<char*>(&value), sizeof(value));
				}
			}
		}
	}

	//Note: we don't do much error handling in here - exceptions will simply be propergated up to the caller.
	//FIXME - think about pointer ownership issues. Or could return volume by value if the copy constructor is shallow
	Volume<uint8>* loadVolumeRle(istream& stream)
	{
		//Read volume dimensions
		uint8 volumeWidthPower = 0;
		uint8 volumeHeightPower = 0;
		uint8 volumeDepthPower = 0;
		stream.read(reinterpret_cast<char*>(&volumeWidthPower), sizeof(volumeWidthPower));
		stream.read(reinterpret_cast<char*>(&volumeHeightPower), sizeof(volumeHeightPower));
		stream.read(reinterpret_cast<char*>(&volumeDepthPower), sizeof(volumeDepthPower));
		
		//FIXME - need to support non cubic volumes
		Volume<uint8>* volume = new Volume<uint8>(volumeWidthPower);

		uint16 volumeWidth = 0x0001 << volumeWidthPower;
		uint16 volumeHeight = 0x0001 << volumeHeightPower;
		uint16 volumeDepth = 0x0001 << volumeDepthPower;

		//Read data
		bool firstTime = true;
		uint32 runLength = 0;
		uint8 value = 0;
		stream.read(reinterpret_cast<char*>(&value), sizeof(value));
		stream.read(reinterpret_cast<char*>(&runLength), sizeof(runLength));
		for(uint16 z = 0; z < volumeDepth; ++z)
		{
			for(uint16 y = 0; y < volumeHeight; ++y)
			{
				for(uint16 x = 0; x < volumeWidth; ++x)
				{	
					if(runLength != 0)
					{
						volume->setVoxelAt(x,y,z,value);
						runLength--;
					}
					else
					{
						stream.read(reinterpret_cast<char*>(&value), sizeof(value));
						stream.read(reinterpret_cast<char*>(&runLength), sizeof(runLength));

						volume->setVoxelAt(x,y,z,value);
						runLength--;
					}
				}
			}
		}

		return volume;
	}

	void saveVolumeRle(std::ostream& stream, Volume<uint8>& volume)
	{
		//Write volume dimensions
		uint16 volumeWidth = volume.getSideLength();
		uint16 volumeHeight = volume.getSideLength();
		uint16 volumeDepth  = volume.getSideLength();

		uint8 volumeWidthPower = logBase2(volumeWidth);
		uint8 volumeHeightPower = logBase2(volumeHeight);
		uint8 volumeDepthPower = logBase2(volumeDepth);

		stream.write(reinterpret_cast<char*>(&volumeWidthPower), sizeof(volumeWidthPower));
		stream.write(reinterpret_cast<char*>(&volumeHeightPower), sizeof(volumeHeightPower));
		stream.write(reinterpret_cast<char*>(&volumeDepthPower), sizeof(volumeDepthPower));

		//Write data
		VolumeIterator<uint8> volIter(volume);
		uint8 current = 0;
		uint32 runLength = 0;
		bool firstTime = true;
		for(uint16 z = 0; z < volumeDepth; ++z)
		{
			for(uint16 y = 0; y < volumeHeight; ++y)
			{
				for(uint16 x = 0; x < volumeWidth; ++x)
				{		
					volIter.setPosition(x,y,z);
					uint8 value = volIter.getVoxel();
					if(firstTime)
					{
						current = value;
						runLength = 1;
						firstTime = false;
					}
					else
					{
						if(value == current)
						{
							runLength++;
						}
						else
						{
							stream.write(reinterpret_cast<char*>(&current), sizeof(current));
							stream.write(reinterpret_cast<char*>(&runLength), sizeof(runLength));
							current = value;
							runLength = 1;
						}
					}					
				}
			}
		}
		stream.write(reinterpret_cast<char*>(&current), sizeof(current));
		stream.write(reinterpret_cast<char*>(&runLength), sizeof(runLength));
	}
}