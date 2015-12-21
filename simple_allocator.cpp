#include <iostream>
#include <forward_list>
#include <math.h>
#include <string.h>

const int BLOCK_SIZE = 8;
const int BUFFER_SIZE = 2097152;

typedef struct Block {
	uint8_t* ptr;
	bool busy;
	int busy_blocks_count;
	Block(uint8_t* ptr) : ptr(ptr), busy(false), busy_blocks_count(0) {}
} Block;

class SmallAllocator {
private:
	uint8_t Memory[BUFFER_SIZE];
	std::forward_list<Block> block_list;

	std::forward_list<Block>::iterator FindFreeZone(int need_blocks) {
		int found_blocks = 0;
		std::forward_list<Block>::iterator pos_it = block_list.end();

		for (auto it = block_list.begin(); it != block_list.end(); it++) {
			if ((*it).busy) {
				found_blocks = 0;
				pos_it = block_list.end();
			} else if (pos_it == block_list.end()) {
				++found_blocks;
				pos_it = it;
			} else {
				++found_blocks;
			}
			if (found_blocks == need_blocks) {
				break;
			}
		}

		if (found_blocks < need_blocks) {
			block_list.end();
		}

		return pos_it;
	}
public:
	SmallAllocator() {
		int block_count = BUFFER_SIZE / BLOCK_SIZE;
		for (int i = block_count - 1; i >= 0; i--) {
			block_list.push_front(Block(Memory + i * BLOCK_SIZE));
		}
	}

	void *Alloc(unsigned int Size) {
		int blocks = ceil(Size / (double)BLOCK_SIZE);
		auto it = FindFreeZone(blocks);
		if (it == block_list.end()) {
			return NULL;
		}
		uint8_t* ptr = (*it).ptr;
		(*it).busy_blocks_count = blocks;
		for (int i = 0; i < blocks; i++, it++) {
			(*it).busy = true;
		}
		return ptr;
	};

	void *ReAlloc(void *Pointer, unsigned int Size) {
		int blocks = ceil(Size / (double)BLOCK_SIZE);
		auto it = FindFreeZone(blocks);
		if (it == block_list.end()) {
			return NULL;
		}
		uint8_t *ptr = (*it).ptr;

		bool found = false;
		auto _it = block_list.begin();
		for (; _it != block_list.end(); _it++) {
			if ((*_it).ptr == Pointer) {
				found = true;
				break;
			}
		}
		if (!found) {
			return NULL;
		}

		(*it).busy_blocks_count = blocks;
		(*_it).busy_blocks_count = 0;
		for (int i = 0; i < blocks; i++, it++, _it++) {
			memmove((*it).ptr, (*_it).ptr, BLOCK_SIZE);
			(*it).busy = true;
			(*_it).busy = false;
		}

		return ptr;
	};

	void Free(void *Pointer) {
		int blocks = 0;
		auto it = block_list.begin();
		for (; it != block_list.end(); it++) {
			if ((*it).ptr == Pointer) {
				blocks = (*it).busy_blocks_count;
				(*it).busy_blocks_count = 0;
				break;
			}
		}
		for (int i = 0; i < blocks; i++, it++) {
			(*it).busy = false;
		}
	};
};

int main(int argc, char *argv[]) {
	SmallAllocator allocator;
	
	char* memory = (char*)allocator.Alloc(6 * sizeof(char));
	memory[0] = 'm'; memory[1] = 'e'; memory[2] = 'm';
	memory[3] = 'o'; memory[4] = 'r'; memory[5] = 'y';

	printf("0x%x %s\n", memory, memory);

	char* new_memory = (char*)allocator.ReAlloc(memory, 6 * sizeof(char));

	printf("0x%x %s\n", new_memory, new_memory);

	allocator.Free(new_memory);

	return 0;
}