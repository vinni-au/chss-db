#include "BtreeIndex.h"

Record* BTreeIterator::getNextRecord() {
    Record* current_result = current_record;
    if(current_record) {
        index->BTree_search(column, page, pos + 1, key);
    }
    return current_result;
}

bool BTreeIterator::hasNextRecord() {
    if(current_record)
        return true;
    return false;
}
