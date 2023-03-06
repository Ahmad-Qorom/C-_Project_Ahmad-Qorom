#include <iostream>
#include <../../memoryEmulator.h>
#include<../../Storage.h>
enum memoryCommandType { Write = 1, Read, Delete };
enum commandPriority { lowPriority, highPriority };

int main()
{
	std::uint16_t* Array = new std::uint16_t[10];
	static std::uint16_t ID = 0;

	std::string type[3] = { "Write","Read","Delete" };
	std::string priority[2] = { "Simple-Command","Quick-Command" };
	memoryCommandType types[3] = { Write,Read,Delete };
	commandPriority prioritys[3] = { lowPriority,highPriority };


	std::cout << "Memory Operations: ";
	for (uint8_t i = 0; i < 3; i++)
		std::cout << type[i] << " : (" << types[i] << ")\t";
	std::cout << std::endl<<"Memory Command Type: ";
	for (uint8_t i = 0; i < 2; i++)
		std::cout << priority[i] << " : (" << prioritys[i] << ")\t";


	std::cout<< "\n\n******************** Welcome with you in your Memory Card *************************" << std::endl;
	uint8_t key = 0;
	while (1)
	{
		do
		{
			std::cout << std::endl << "The type of memory operation you want to apply it in memory is: ";
			std::cin >> Array[1]; // the type of memory operation,  Write...Read....Delete
		} while (Array[1] < 49 || Array[1] > 50);

		Array[0] = ++ID;
	
		do
		{
			std::cout << std::endl << "Are you want the Command is Quick or Simple? ";
			std::cin >> Array[2]; // priority of command
		} while (!(Array[2] == 48 || Array[2] == 49));

		Array[3] = 5;
		std::cout << "Enter the start address of the data: ";
		std::cin >> Array[4];
		if (Array[1] == 49)
		{
			Array[5] = 'A';
			Array[6] = 'B';
			Array[7] = 'C';
			Array[8] = 'D';
			Array[9] = 'E';
		}
		
		MemoryEmulators s;
		s.AddCommand(Array, 10);
		std::cout << std::endl;
		std::cout << "Are want to Enter new Command? Press (1) if you want" << std::endl;
		std::cin >> key;

		if (key != 49) // ascki value for 0 is 48 in askii
			break;	
	}
}

