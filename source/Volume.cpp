/******************************************************************************
This file is part of a voxel plugin for OGRE
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/

#include "IntegralVector3.h"
#include "Volume.h"
#include "VolumeIterator.h"

#include "OgreVector3.h"

#include <iostream> //FIXME - remove this...

using namespace boost;

namespace Ogre
{

	Volume::Volume()
	{
		//FIXME - Add checking...
		/*SharedPtr<Block>& homogeneousBlock = mHomogeneousBlocks[0];
		if(homogeneousBlock.isNull())
		{
			homogeneousBlock = SharedPtr<Block>(new Block);
			homogeneousBlock->fillWithValue(0);
		}*/

		/*for(uint16_t i = 0; i < OGRE_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			mBlocks[i] = mHomogeneousBlocks[0];
		}*/

		for(uint16_t i = 0; i < OGRE_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			mBlocks[i] = new Block;
		}
	}

	Volume::Volume(const Volume& rhs)
	{
		std::cout << "Warning - Copying Volume" << std::endl;
		*this = rhs;
	}

	Volume::~Volume()
	{
	}

	Volume& Volume::operator=(const Volume& rhs)
	{
		std::cout << "Warning - Assigning Volume" << std::endl;
		if (this == &rhs)
		{
			return *this;
		}

		/*for(uint16_t i = 0; i < OGRE_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			//FIXME - Add checking...
			mBlocks[i] = SharedPtr<Block>(new Block);
		}*/

		for(uint16_t i = 0; i < OGRE_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			//I think this is OK... If a block is in the homogeneous array it's ref count will be greater
			//than 1 as there will be the pointer in the volume and the pointer in the static homogeneous array.
			/*if(rhs.mBlocks[i].unique())
			{
				mBlocks[i] = SharedPtr<Block>(new Block(*(rhs.mBlocks[i])));
			}
			else
			{*/
				//we have a block in the homogeneous array - just copy the pointer.
				mBlocks[i] = rhs.mBlocks[i];
			//}
		}

		return *this;
	}

	/*uint8_t Volume::getVoxelAt(const uint16_t xPosition, const uint16_t yPosition, const uint16_t zPosition) const
	{
	const uint16_t blockX = xPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
	const uint16_t blockY = yPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
	const uint16_t blockZ = zPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;

	const uint16_t xOffset = xPosition - (blockX << OGRE_BLOCK_SIDE_LENGTH_POWER);
	const uint16_t yOffset = yPosition - (blockY << OGRE_BLOCK_SIDE_LENGTH_POWER);
	const uint16_t zOffset = zPosition - (blockZ << OGRE_BLOCK_SIDE_LENGTH_POWER);

	Block* block = mBlocks
	[
	blockX + 
	blockY * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
	blockZ * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS
	];

	return block->getVoxelAt(xOffset,yOffset,zOffset);
	}*/

	/*void Volume::setVoxelAt(const uint16_t xPosition, const uint16_t yPosition, const uint16_t zPosition, const uint8_t value)
	{
	const uint16_t blockX = xPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
	const uint16_t blockY = yPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;
	const uint16_t blockZ = zPosition >> OGRE_BLOCK_SIDE_LENGTH_POWER;

	const uint16_t xOffset = xPosition - (blockX << OGRE_BLOCK_SIDE_LENGTH_POWER);
	const uint16_t yOffset = yPosition - (blockY << OGRE_BLOCK_SIDE_LENGTH_POWER);
	const uint16_t zOffset = zPosition - (blockZ << OGRE_BLOCK_SIDE_LENGTH_POWER);

	Block* block = mBlocks
	[
	blockX + 
	blockY * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
	blockZ * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS * OGRE_VOLUME_SIDE_LENGTH_IN_BLOCKS
	];

	block->setVoxelAt(xOffset,yOffset,zOffset, value);
	}*/	

	Block* Volume::getBlock(uint16_t index)
	{
		return mBlocks[index];
	}

	bool Volume::containsPoint(Vector3 pos, float boundary)
	{
		return (pos.x < OGRE_VOLUME_SIDE_LENGTH - 1 - boundary)
			&& (pos.y < OGRE_VOLUME_SIDE_LENGTH - 1 - boundary) 
			&& (pos.z < OGRE_VOLUME_SIDE_LENGTH - 1 - boundary)
			&& (pos.x > boundary)
			&& (pos.y > boundary)
			&& (pos.z > boundary);
	}

	bool Volume::containsPoint(IntVector3 pos, uint16_t boundary)
	{
		return (pos.x < OGRE_VOLUME_SIDE_LENGTH - 1 - boundary)
			&& (pos.y < OGRE_VOLUME_SIDE_LENGTH - 1 - boundary) 
			&& (pos.z < OGRE_VOLUME_SIDE_LENGTH - 1 - boundary)
			&& (pos.x > boundary)
			&& (pos.y > boundary)
			&& (pos.z > boundary);
	}

	bool Volume::loadFromFile(const std::string& sFilename)
	{
		//Open the file
		std::ifstream file;
		file.open(sFilename.c_str(), std::ios::in | std::ios::binary);
		if(!file.is_open())
		{
			//LogManager::getSingleton().logMessage("Failed to open volume file " + sFilename);
			return false;
		}

		//Read volume dimensions
		uint8_t volumeWidth = 0;
		uint8_t volumeHeight = 0;
		uint8_t volumeDepth = 0;
		file.read(reinterpret_cast<char*>(&volumeWidth), sizeof(volumeWidth));
		file.read(reinterpret_cast<char*>(&volumeHeight), sizeof(volumeHeight));
		file.read(reinterpret_cast<char*>(&volumeDepth), sizeof(volumeDepth));
		if(file.fail())
		{
			//LogManager::getSingleton().logMessage("Failed to read dimentions");
			return false;
		} 

		//Read data
		VolumeIterator volIter(*this);
		for(uint16_t z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint16_t y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint16_t x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					uint8_t value;
					file.read(reinterpret_cast<char*>(&value), sizeof(value)); //FIXME - check for error here
					volIter.setVoxelAt(x,y,z,value);
				}
			}

			//Periodically see if we can tidy the memory to avoid excessive usage during loading.
			if(z%OGRE_BLOCK_SIDE_LENGTH == OGRE_BLOCK_SIDE_LENGTH-1)
			{
				tidy(); //FIXME - we don't actually have to tidy the whole volume here - just the part we loaded since the last call to tidy.
			}
		}
		return true;
	}

	bool Volume::saveToFile(const std::string& sFilename)
	{
		//Open the file
		std::ofstream file;

		file.open(sFilename.c_str(), std::ios::out | std::ios::binary);
		if(!file.is_open())
		{
			//LogManager::getSingleton().logMessage("Failed to open file for saving volume");
			return false;
		}

		//Read volume dimensions
		uint8_t volumeWidth = 0;
		uint8_t volumeHeight = 0;
		uint8_t volumeDepth = 0;
		file.write(reinterpret_cast<char*>(&volumeWidth), sizeof(volumeWidth));
		file.write(reinterpret_cast<char*>(&volumeHeight), sizeof(volumeHeight));
		file.write(reinterpret_cast<char*>(&volumeDepth), sizeof(volumeDepth));
		if(file.fail())
		{
			//LogManager::getSingleton().logMessage("Failed to write dimensions");
			return false;
		} 

		//Write data
		VolumeIterator volIter(*this);
		for(uint16_t z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint16_t y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint16_t x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					uint8_t value = volIter.getVoxelAt(x,y,z);
					file.write(reinterpret_cast<char*>(&value), sizeof(value));	//FIXME - check for error here				
				}
			}
		}
		return true;
	}

	void Volume::regionGrow(uint16_t xStart, uint16_t yStart, uint16_t zStart, uint8_t value)
	{
		//FIXME - introduce integrer 'isInVolume' function
		if((xStart > OGRE_VOLUME_SIDE_LENGTH-1) || (yStart > OGRE_VOLUME_SIDE_LENGTH-1) || (zStart > OGRE_VOLUME_SIDE_LENGTH-1)
			|| (xStart < 0) || (yStart < 0) || (zStart < 0))
		{
            //FIXME - error message..
			return;
		}

		VolumeIterator volIter(*this);
		const uint8_t uSeedValue = volIter.getVoxelAt(xStart,yStart,zStart);

		if(value == uSeedValue)
		{
			return; //FIXME - Error message? Exception?
		}

		std::queue<UIntVector3> seeds;
		seeds.push(UIntVector3(xStart,yStart,zStart));

		while(!seeds.empty())
		{
			UIntVector3 currentSeed = seeds.front();
			seeds.pop();

			//std::cout << "x = " << currentSeed.x << " y = " << currentSeed.y << " z = " << currentSeed.z << std::endl;

			//FIXME - introduce 'safe' function which tests this?
			if((currentSeed.x > OGRE_VOLUME_SIDE_LENGTH-2) || (currentSeed.y > OGRE_VOLUME_SIDE_LENGTH-2) || (currentSeed.z > OGRE_VOLUME_SIDE_LENGTH-2)
				|| (currentSeed.x < 1) || (currentSeed.y < 1) || (currentSeed.z < 1))
			{
				continue;
			}

			if(volIter.getVoxelAt(currentSeed.x, currentSeed.y, currentSeed.z+1) == uSeedValue)
			{
				volIter.setVoxelAt(currentSeed.x, currentSeed.y, currentSeed.z+1, value);
				seeds.push(UIntVector3(currentSeed.x, currentSeed.y, currentSeed.z+1));
			}

			if(volIter.getVoxelAt(currentSeed.x, currentSeed.y, currentSeed.z-1) == uSeedValue)
			{
				volIter.setVoxelAt(currentSeed.x, currentSeed.y, currentSeed.z-1, value);
				seeds.push(UIntVector3(currentSeed.x, currentSeed.y, currentSeed.z-1));
			}

			if(volIter.getVoxelAt(currentSeed.x, currentSeed.y+1, currentSeed.z) == uSeedValue)
			{
				volIter.setVoxelAt(currentSeed.x, currentSeed.y+1, currentSeed.z, value);
				seeds.push(UIntVector3(currentSeed.x, currentSeed.y+1, currentSeed.z));
			}

			if(volIter.getVoxelAt(currentSeed.x, currentSeed.y-1, currentSeed.z) == uSeedValue)
			{
				volIter.setVoxelAt(currentSeed.x, currentSeed.y-1, currentSeed.z, value);
				seeds.push(UIntVector3(currentSeed.x, currentSeed.y-1, currentSeed.z));
			}

			if(volIter.getVoxelAt(currentSeed.x+1, currentSeed.y, currentSeed.z) == uSeedValue)
			{
				volIter.setVoxelAt(currentSeed.x+1, currentSeed.y, currentSeed.z, value);
				seeds.push(UIntVector3(currentSeed.x+1, currentSeed.y, currentSeed.z));
			}

			if(volIter.getVoxelAt(currentSeed.x-1, currentSeed.y, currentSeed.z) == uSeedValue)
			{
				volIter.setVoxelAt(currentSeed.x-1, currentSeed.y, currentSeed.z, value);
				seeds.push(UIntVector3(currentSeed.x-1, currentSeed.y, currentSeed.z));
			}
		}
	}

	void Volume::tidy(void)
	{
		//Check for homogeneous blocks
		/*for(uint32_t ct = 0; ct < OGRE_NO_OF_BLOCKS_IN_VOLUME; ++ct)
		{
			if(mBlocks[ct]->isHomogeneous())
			{
				//LogManager::getSingleton().logMessage("Got homogeneous block with value " + StringConverter::toString(mBlocks[ct]->getVoxelAt(0,0,0)));

				const uint8_t homogeneousValue = mBlocks[ct]->getVoxelAt(0,0,0);
				SharedPtr<Block>& homogeneousBlock = mHomogeneousBlocks[homogeneousValue];
				if(homogeneousBlock.isNull())
				{
					homogeneousBlock = SharedPtr<Block>(new Block);
					homogeneousBlock->fillWithValue(homogeneousValue);
				}
				mBlocks[ct] = homogeneousBlock;
			}
		}*/
	}
}
