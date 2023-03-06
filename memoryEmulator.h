#pragma once
#include <iostream>
#include <../../Storage.h>

class MemoryEmulators
{
	Memory* memory;
public:
	MemoryEmulators() = default;
	void AddCommand(std::uint16_t* data, uint16_t size);
	void ExecuteCommand();
	void Abort();
	void Remove();
	void Remove(uint16_t id);
	virtual ~MemoryEmulators();
};

MemoryEmulators::MemoryEmulators()
{
	memory = memory->createObject();
}


void MemoryEmulators::AddCommand(std::uint16_t* data,uint16_t size)
{
	requestCommand* cmd = reinterpret_cast<requestCommand*>(data); // casting from dynamic array to structure
	//requestCommand* request = (requestCommand*)&data;
	requestCommand newCommnad; // structure of request command
	newCommnad.ID = data[0];
	newCommnad.Type = data[1];
	newCommnad.priority = data[2];
	newCommnad.dataLength = data[3];
	newCommnad.dataAddress = data[4];

	if (data[1] == 49)   //if the array contain write command
	{
		for (uint8_t i = 0; i < data[3]; i++)
			newCommnad.Array[i] = data[5 + i];	
		bool result = memory->addtoQueue(newCommnad);
	}
	else
	{   
		delete newCommnad.Array;
		bool result = memory->addtoQueue(newCommnad);
	}
}

void MemoryEmulators::Remove()
{
	bool result = memory->removeFromHead();
}

void MemoryEmulators::Remove(uint16_t id)
{
	bool result = memory->removebyID(id);
}

void MemoryEmulators::ExecuteCommand()
{
	std::queue<responseCommand*> responseQueue;
  	responseQueue = memory->Execute();
}

void MemoryEmulators::Abort()
{
	memory->Abort();
}

