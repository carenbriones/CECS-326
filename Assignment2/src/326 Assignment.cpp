//============================================================================
// Name        : Assignment.cpp
// Author      : Caren Briones <carenpbriones@gmail.com>
// Description : CECS 326, Assignment 2
//============================================================================

#include <iostream>
#include <stdlib.h>
using namespace std;

struct PCB
{
	int PID;
	int* pageTable;
	PCB *next;
};

struct MBT
{
	int emptyBlocks;
	bool memoryBlocks[128];
};

// Declare prototypes
void displayMenu();
void PrintMBT(MBT *blocks);
void AddNode(PCB *head, PCB *node);
void PrintList(PCB *head);
int GetEmptyBlocks(MBT *blocks);
int GetPCBSize(PCB *node);

int main() {
	// Include a description of what this program does
	cout << "This program simulates the creation and use of a Memory Block Table." << endl;
	cout << "It can create an MBT, and add/delete processes to the table." << endl << endl;

	// Creates MBT, initializes first ten blocks to "occupied"
	MBT *memoryBlockTable = new MBT;
	memoryBlockTable->emptyBlocks = 118;

	for (int i = 0; i < 10; i++)
	{
		memoryBlockTable->memoryBlocks[i] = true;
	}
	for (int i = 10; i < 128; i++)
	{
		memoryBlockTable->memoryBlocks[i] = false;
	}

	int choice = 1;
	int PID = 1000;
	PCB *head = new PCB;
	head = NULL;

	while (choice > 0 && choice < 4)
	{
		displayMenu();
		cin >> choice;

		// Input validation
		while (std::cin.fail())
		{
			cout << endl;
			std::cout << "Invalid input. Enter a valid number." << std::endl;
			std::cin.clear();
			std::cin.ignore(256, '\n');
			cin >> choice;
		}
		if (choice > 4 || choice < 1)
		{
			cout << "Please enter a number from 1 to 4.\n";
			cin >> choice;
		}

		// Menu choices
		// Initiate a process
		else if (choice == 1)
		{
			srand (time(NULL));
			PCB *process = new PCB;
			process->PID = PID;
			process->next = NULL;
			int size = rand() % 50 + 1;

			// Increments PID each time to give new PID for next PCB
			PID = PID + 1;
			cout << endl << size << endl;

			if (size < GetEmptyBlocks(memoryBlockTable))
			{
				int blockCounter = 0;
				int* pageTable = new int[size];
				for (int i = 10; i < 128; i++)
				{
					if (blockCounter < size && memoryBlockTable->memoryBlocks[i] == false)
					{
						memoryBlockTable->memoryBlocks[i] = true;
						*pageTable = i;
						pageTable++;
						blockCounter++;
					}
				}

				pageTable = pageTable - size;
				process->pageTable = pageTable;

				// Prints out the indexes stored in the pageTable

				for (int i = 0; i < size; i++)
				{
					cout << *process->pageTable << endl;
					*process->pageTable++;
				}

				cout << GetPCBSize(head);

				cout << "The process has successfully been added to the MBT." << endl;
				PrintMBT(memoryBlockTable);

				PCB *current = new PCB;
				current = head;

				if (head == NULL)
				{
					head = process;
				}
				else
				{
					while(current->next != NULL)
					{
						current = current->next;
					}
					current->next = process;
				}

				cout << GetPCBSize(head);
			}

			else
			{
				cout << "There is not enough space in the MBT for this process." << endl;
				cout << "The process has not been added to the MBT." << endl << endl;
			}
		}

		// Print all processes in ready queue
		else if (choice == 2)
		{
			cout << "Here are the PIDs of the PCBs currently in the ready queue: " << endl;
			PrintList(head);
		}

		// Terminate processes in ready queue
		else if (choice == 3)
		{
			int PID;
			cout << "Enter the PID of the process";
			cout << " that you would like to terminate" << endl;
			cin >> PID;

			// Input validation
			while (std::cin.fail())
			{
				cout << endl;
				std::cout << "Invalid input. Enter a valid PID." << std::endl;
				std::cin.clear();
				std::cin.ignore(256, '\n');
				cin >> PID;
			}
		}
		else if (choice == 4)
		{
			delete memoryBlockTable;
		}
	}
	return 0;
}

/**
 * Displays the menu choices
 */
void displayMenu()
{
	cout << endl;
	cout << "Choose an option:\n";
	cout << "1.  Initiate a process\n";
	cout << "2.  Print all processes in ready queue\n";
	cout << "3.  Terminate processes in ready queue\n";
	cout << "4.  Exit\n";
}

/**
 * Prints all blocks in the MBT, displaying whether they are full or not.
 *
 * @param blocks Memory Block Table to be displayed
 */
void PrintMBT(MBT *blocks)
{
	cout << endl;
	for (int i = 0; i < 128; i++)
	{
		cout << "Memory Block " << (i + 1) << ": ";
		if (blocks->memoryBlocks[i] == true)
		{
			cout << "\tfull" << endl;
		}
		else
		{
			cout << "\tempty" << endl;
		}
	}
}

/**
 * Adds a node to a linked list
 *
 * @param head root of the list
 * @param n size of the PCB
 */
void AddNode(struct PCB *head, struct PCB *node)
{
	node->next = NULL; //initially it will point to nothing
	PCB *current = head; //create a pointer to a Node structure that is set equal to the head Node

	// Sets root PCB if necessary
	if(current == NULL)
	{
		head = node;
		current = node->next;
	}

	while (current) //while loop runs as long as current is not NULL
	{
		if (current->next == NULL)
		{
			current->next = node;
		}
		current = current->next;
	}
}

/**
 * Returns the number of empty memory blocks in the MBT
 *
 * @param blocks the MBT
 */
int GetEmptyBlocks(MBT *blocks)
{
	int counter = 0;
	// Starts at index ten because the first ten blocks will always be occupied
	for (int i = 10; i < 128; i++)
	{
		if (blocks->memoryBlocks[i] == false)
		{
			counter = counter + 1;
		}
	}
	return counter;
}

/**
 * Deletes a node from a linked list if it matches the PID linked to the PCB
 * given in the parameters
 *
 * @param head root of the list of page tables
 * @param PID PID of corresponding PCB that will be removed
 */
void DeleteNode(PCB *head, int PID)
{
	PCB *current = head;
	PCB *previous = new PCB;

	// Checks if the root is the node to be deleted
	if (current->PID == PID)
	{
		delete current;
	}

	while(current)
	{
		// Updates page tables by deleting the node once it is found
		if (current->next->PID == PID)
		{
			previous = current;
			previous->next = current->next->next;
			delete current->next;
		}
	}
}

void PrintList(PCB *head)
{
	PCB *current = head;
	while (current)
	{
		cout << current->PID << endl;
		current = current->next;
	}
}

int GetPCBSize(PCB *node)
{
	PC
}
