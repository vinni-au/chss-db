#include "index.h"
#include "noindex.h"

NoindexIterator::NoindexIterator(Index* index, int key) : IndexIterator(index, key), m_current_position(0), current_record(0) {
}

Record* NoindexIterator::getNextRecord() {

}

bool NoindexIterator::hasNextRecord() {

}

void Noindex::createIndex() {
}

void Noindex::addKey(int key, uint32 page) {
}

NoindexIterator* Noindex::findKey(int key) {
}

void Noindex::deleteKey(int key, uint32 page) {

}
