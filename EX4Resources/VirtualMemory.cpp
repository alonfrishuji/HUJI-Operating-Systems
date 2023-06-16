#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <algorithm>
#include <stdlib.h>
#include <cassert>


uint64_t getCurOffset(uint64_t virtualAddress, unsigned int curDepth) {
    unsigned int rightIndex = (TABLES_DEPTH - curDepth) * OFFSET_WIDTH;
    unsigned int size = curDepth == 0 ? VIRTUAL_ADDRESS_WIDTH - (TABLES_DEPTH * OFFSET_WIDTH) : OFFSET_WIDTH;
    unsigned int leftIndex = rightIndex + size;
    unsigned int leftShift = 64 - leftIndex;
    unsigned int rightShift = 64 - size;
    uint64_t curOffset = (virtualAddress << leftShift) >> rightShift;
    return curOffset;
}


uint64_t getPageIdx(uint64_t virtualAddress) {
    uint64_t pageIdx = virtualAddress >> OFFSET_WIDTH;
    return pageIdx;
}


void setTableZero(uint64_t frameIdx) {
    uint64_t frameBase = frameIdx * PAGE_SIZE;
    for (int row = 0; row < PAGE_SIZE; row++) {
        PMwrite(frameBase + row, 0);
    }
}


uint64_t getCyclicDistance(uint64_t page1, uint64_t page2) {
    uint64_t pageDistance = std::abs((signed)(page1 - page2));
    uint64_t cyclicDistance = std::min(pageDistance, (uint64_t)(NUM_PAGES - pageDistance));
    return cyclicDistance;
}


void getNextFrameRecursive(uint64_t frameIdx, uint64_t originFrame, unsigned int curDepth, uint64_t *maxFrameIdx, uint64_t *farthestPage,
    uint64_t *adressUnlink, uint64_t addressFromParent, uint64_t page, uint64_t targetPage, bool *foundEmptyTable) {
    *maxFrameIdx = std::max(*maxFrameIdx, frameIdx);
    if (curDepth < TABLES_DEPTH) {
        bool emptyTable = true;
        for (int row = 0; row < PAGE_SIZE; row++) {
            word_t nextFrameIdx;
            uint64_t rowAddress = frameIdx * PAGE_SIZE + row;
            PMread(rowAddress, &nextFrameIdx);
            if (nextFrameIdx != 0) {
                emptyTable = false;
                uint64_t childPage = page + (row << ((TABLES_DEPTH - 1 - curDepth) * OFFSET_WIDTH));
                getNextFrameRecursive(nextFrameIdx, originFrame, curDepth+1, maxFrameIdx, farthestPage, adressUnlink, 
                    rowAddress, childPage, targetPage, foundEmptyTable);
                if (*foundEmptyTable) {
                    return;
                }
            }
        }
        if (emptyTable && frameIdx != originFrame) {
            *foundEmptyTable = true;
            *adressUnlink = addressFromParent;
            return;
        }
    }
    else if (getCyclicDistance(page, targetPage) > getCyclicDistance(*farthestPage, targetPage)) {
            *farthestPage = page;
            *adressUnlink = addressFromParent;
            return;
    }
}



int getNextFrame(uint64_t originFrame, int originDepth, uint64_t targetPage, word_t *nextFrame) {
    uint64_t maxFrameIdx = 0, farthestPage = targetPage, adressUnlink = 0;
    bool foundEmptyTable = false;
    getNextFrameRecursive(0, originFrame, 0, &maxFrameIdx, &farthestPage, &adressUnlink, 0, 0, 
        targetPage, &foundEmptyTable);
    if (foundEmptyTable) {
        PMread(adressUnlink, nextFrame);
        PMwrite(adressUnlink, 0);
    }
    else if (maxFrameIdx < NUM_FRAMES - 1) {
        *nextFrame = maxFrameIdx + 1;
    }
    else if (farthestPage != targetPage) {
        PMread(adressUnlink, nextFrame);
        PMevict(*nextFrame, farthestPage);
        PMwrite(adressUnlink, 0);
    }
    else {
        return 0;
    }

    if (originDepth == TABLES_DEPTH - 1) {
        PMrestore(*nextFrame, targetPage);
    }
    else if (!foundEmptyTable) {
        setTableZero(*nextFrame);
    }
    assert((*nextFrame) != 0);
    return 1;
}


int getPhysicalAddress(uint64_t virtualAddress, uint64_t *physicalAddress) {
    if (virtualAddress >= VIRTUAL_MEMORY_SIZE) {
        return 0;
    }
    unsigned int curDepth = 0;
    uint64_t curAddress = 0;
    word_t nextFrame;
    uint64_t pageIdx = getPageIdx(virtualAddress);
    while (curDepth < TABLES_DEPTH) {
        uint64_t curOffset = getCurOffset(virtualAddress, curDepth);
        PMread(curAddress + curOffset, &nextFrame);
        if (nextFrame == 0) {
            if (!getNextFrame(curAddress / PAGE_SIZE, curDepth, pageIdx, &nextFrame)) {
                return 0;
            }
            PMwrite(curAddress + curOffset, nextFrame);
        }
        curAddress = nextFrame * PAGE_SIZE;
        curDepth++;
    }
    uint64_t curOffset = getCurOffset(virtualAddress, curDepth);
    *physicalAddress = curAddress + curOffset;
    return 1;
} 


void VMinitialize() {
    setTableZero(0);
}


int VMread(uint64_t virtualAddress, word_t* value) {
    uint64_t physhicalAddress;
    if (!getPhysicalAddress(virtualAddress, &physhicalAddress)) {
        return 0;
    }
    PMread(physhicalAddress, value);
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t physhicalAddress;
    if (!getPhysicalAddress(virtualAddress, &physhicalAddress)) {
        return 0;
    }
    PMwrite(physhicalAddress, value);
    return 1;
}

int main(int argc, char const *argv[])
{
    //110
    //1101
    //0101
    //1001
    //0101
    // uint64_t virtualAddress = 447893;
    // for (int i = 0; i < 5; i++) {
    //     std::bitset<64> binaryNumber(getCurOffset(virtualAddress, i));
    //     std::cout << binaryNumber << std::endl;
    // }
    VMinitialize();
    int ret = VMwrite(VIRTUAL_MEMORY_SIZE - 1, 5);
    return 0;
}
