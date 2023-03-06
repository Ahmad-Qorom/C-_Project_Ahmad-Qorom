#pragma once
#include <iostream>
#include <queue>

struct requestCommand
{
	std::uint16_t ID;
	std::uint16_t Type;
	std::uint16_t priority;
	std::uint16_t dataLength;
	std::uint16_t dataAddress;	
	std::uint16_t* Array;
};
struct responseCommand
{
	std::uint16_t ID;
	std::uint16_t Type;
	std::uint16_t priority;
	std::uint16_t status;
	std::uint16_t* Array;
};

class commnandFactory
{
public:
	commands* getCommand(std::uint16_t commandType)
	{
		if (commandType == 1)
			return new write;
		else if (commandType == 2)
			return new read();
		else if (commandType == 3)
			return new cut();
	}
};

class commands
{
public:
	static uint8_t ID;
	Memory* memory;
	responseCommand* response;

	commands()
	{
		memory = memory->createObject();
	}
	virtual ~commands() = default;
	virtual responseCommand* Execute(requestCommand& writeCommnd) = 0; // pure virtual function 
	virtual responseCommand* generateResponse(std::uint16_t isStored, std::uint16_t priority) {};
};

class write :virtual public commands
{
	responseCommand* generateResponse(std::uint16_t isStored, std::uint16_t priority) override
	{
		responseCommand writeResponse;
		writeResponse.ID = ++ID;
		writeResponse.Type = 4;
		writeResponse.priority = priority;
		writeResponse.status = isStored;
		delete writeResponse.Array;
		return&writeResponse;
	}
public:
	responseCommand* Execute(requestCommand& writeCommnd) override
	{
		ID = writeCommnd.ID;
		uint8_t startPoint = writeCommnd.dataAddress;
		uint8_t endPoint = writeCommnd.dataLength + startPoint;
		uint8_t index = 0;
		if (endPoint <= 999 && startPoint >= 0)
		{
			for (; startPoint < endPoint; startPoint++)
			{
				memory->memoryData[startPoint] = writeCommnd.Array[index++];
			}
			response = generateResponse(true, writeCommnd.priority);
		}
		else
			return generateResponse(false, writeCommnd.priority);
	}
};

class read :virtual  public commands
{
	responseCommand* generateResponse(std::uint16_t isStored, std::uint16_t priority) override
	{
		responseCommand readResponse;
		readResponse.ID = ++ID;
		readResponse.Type = 5;
		readResponse.priority = priority;
		readResponse.status = isStored;
		return&readResponse;
	}
public:
	responseCommand* Execute(requestCommand& readCommnd) override
	{
		ID = readCommnd.ID;
		uint16_t startPoint = readCommnd.dataAddress;
		uint16_t endPoint = readCommnd.dataLength + startPoint;
		uint16_t index = 0;
		uint16_t* readData = new uint16_t[readCommnd.dataLength];
		if (endPoint <= 999 && startPoint >= 0)
		{
			response = generateResponse(true, readCommnd.priority);
			for (; startPoint < endPoint; startPoint++)
			{
				readData[index++] = memory->memoryData[startPoint];
			}
			response->Array = readData;
		}
		else
		{
			response = generateResponse(false, readCommnd.priority);
			response->status = false;
			delete response->Array;
		}
		return response;
	}
};

class cut :virtual  public commands
{
	responseCommand* generateResponse(std::uint16_t isStored, std::uint16_t priority) override
	{
		responseCommand cutResponse;
		cutResponse.ID = ++ID;
		cutResponse.Type = 4;
		cutResponse.priority = priority;
		cutResponse.status = isStored;
		delete cutResponse.Array;
		return&cutResponse;
	}
public:
	responseCommand* Execute(requestCommand& cutCommand) override 
	{
		ID = cutCommand.ID;
		uint16_t startPoint = cutCommand.dataAddress;
		uint16_t endPoint = cutCommand.dataLength + startPoint;
		uint16_t index = 0;
		if (endPoint <= 999 && startPoint >= 0)
		{
			response = generateResponse(true, cutCommand.priority);
			for (; startPoint < endPoint; startPoint++)
			{
				memory->memoryData[startPoint] = NULL;
			}
		}
		else
		{
			response = generateResponse(false, cutCommand.priority);
			response->status = false;
			delete response->Array;
		}
		return response;
	}
};

uint8_t commands::ID = 0;

struct compare
{
	bool operator()(requestCommand& first, requestCommand& second)
	{
		return first.priority < second.priority;
	}
};

class Memory
{
private:
	static Memory* Object;
	static uint16_t ID;
	Memory() = default;  // Singelton Pattern
	std::priority_queue<requestCommand, std::vector<requestCommand>, compare> QueueManager;
public:
	std::uint16_t memoryData[1000] = { 0 };
	responseCommand* response;

	// singelton method for create single object 
	static Memory* createObject()
	{
		if (Object == NULL)
		{
			Object = new Memory();
			return Object;
		}
		else
		{
			return Object;
		}
	}

	bool addtoQueue(requestCommand& packet)
	{
		QueueManager.push(packet);
		ID = packet.ID;
		return true;
	}

	bool removeFromHead()
	{
		if (QueueManager.empty())
		{
			std::cout << "The Queue is Empty" << std::endl;
			return false;
		}
		QueueManager.pop();
		return true;
	}

	bool removebyID(std::uint16_t id)
	{
		if (QueueManager.empty())
		{
			std::cout << "The Queue is Empty!" << std::endl;
			return false;
		}
		else
		{
			std::queue<requestCommand> temp;
			for (uint16_t i = 0; i < QueueManager.size(); i++)
			{
				if (QueueManager.top().ID == id)
				{
					QueueManager.pop();
					return true;
				}
				else
				{
					temp.push(QueueManager.top());
					QueueManager.pop();
				}
			}
			for (uint16_t i = 0; i < temp.size(); i++)
			{
				QueueManager.push(temp.front());
				temp.pop();
			}
			return false;
		}
	}

	std::queue<responseCommand*> Execute()
	{
		std::queue<responseCommand*> responseQueue;
		if (QueueManager.size() == 0)
		{
			std::cout << "There is no Command in Queue\n";
		}
		else
		{
			requestCommand comm;
			commnandFactory* factory = new commnandFactory();
			commands* commandToExecute;
			for (uint16_t i = 0; i < QueueManager.size(); i++)
			{
				comm = QueueManager.top();
				commandToExecute = factory->getCommand(comm.Type);   // polymorphism here
				response = commandToExecute->Execute(comm);
				responseQueue.push(response);
				QueueManager.pop();
			}
		}
		return responseQueue;
	}

	void Abort()
	{
		for (uint16_t i = 0; i < QueueManager.size(); i++)
		{
			QueueManager.pop();
		}
		std::cout << "All Commands are Stopped\n";
	}
};

Memory* Memory::Object = NULL;

