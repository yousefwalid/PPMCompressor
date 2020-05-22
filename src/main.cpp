#include <iostream>
#include <fstream>
#include <string>
#include "trie.cpp"

using namespace std;

struct countStats
{
  int count, cumCount, totalCount;
  bool isEsc;
};

uint8_t l = 0, u = 255;
int wordCnt = 0;
int wordSize = 0;

ofstream binaryStream;

string stream = "";

TrieNode *zeroContext;
const int maxContext = 2; // The maximum number of contexts

string getSmallerContext(string context)
{
  return context.erase(0, 1);
}

/**
 * Traverses to a given node in the trie,
 * If the node does not exist in the trie, it will return nullptr
 * @param symbol {int} The symbol to traverse to
 * @param context {string} The context the string exists in
 */

TrieNode *traverseToNode(int symbol, string context)
{
  auto curr = zeroContext;
  for (int i = 0; i < context.size(); i++)
  {
    while (curr != nullptr && curr->symbol != (int)context[i])
      curr = curr->nextNode;

    if (curr == nullptr)
      return curr;

    curr = curr->nextContextHead;
  }

  while (curr != nullptr && curr->symbol != symbol)
    curr = curr->nextNode;

  return curr;
}

/**
 * Increases the count of the given node and all its lower contexts,
 * also updates the cumCount of next nodes of those nodes
 * @param node {TrieNode *} Pointer to the node to be increased
 * @param cnt {int} The value to increase the count with
 */

void increaseCountAndCumCount(TrieNode *node, int cnt = 1)
{
  while (node != nullptr)
  {
    node->count += cnt;
    auto node2 = node->nextNode;
    while (node2 != nullptr)
      node2->cumCount += cnt, node2 = node2->nextNode;
    node = node->prevContext;
  }
}

/**
 * Adds a node to the trie given the context, and also adds it to all the smaller contexts,
 * it also updates the count of the parent nodes and all their following nodes
 * @param symbol {int} The symbol of the node to be added
 * @param context {string} The context of the node to be added
 * @return addedNodePointer {TrieNode *} The address of the node just added
 */

TrieNode *recursivelyAddNode(int symbol, string context)
{
  if (context == "")
  {
    if (zeroContext == nullptr)
    {
      auto firstNode = new TrieNode(symbol);
      zeroContext = firstNode;
      return firstNode;
    }
    else
    {
      auto curr = zeroContext;
      while (curr->nextNode && curr->symbol != symbol)
        curr = curr->nextNode;

      if (curr->symbol == symbol)
        return curr;

      auto newNode = new TrieNode(symbol);
      curr->nextNode = newNode;
      newNode->cumCount = curr->cumCount + curr->count;
      return newNode;
    }
  }
  else
  {
    string previousContext = context.substr(1);

    auto foundNode = traverseToNode(symbol, previousContext);
    auto lowerContextNode = foundNode;
    if (!foundNode)
      lowerContextNode = recursivelyAddNode(symbol, previousContext);
    else
      increaseCountAndCumCount(foundNode);

    auto newNode = new TrieNode(symbol);
    newNode->prevContext = lowerContextNode;
    auto parNode = traverseToNode(context[context.length() - 1], context.substr(0, context.length() - 1));
    auto currentContextHead = parNode->nextContextHead;
    if (currentContextHead == nullptr)
    {
      parNode->nextContextHead = newNode;
    }
    else
    {
      while (currentContextHead->nextNode != nullptr)
        currentContextHead = currentContextHead->nextNode;
      currentContextHead->nextNode = newNode;
      newNode->cumCount = currentContextHead->cumCount + currentContextHead->count;
    }
    return newNode;
  }
}

void createInitialNodes(string initialWord)
{
  string context = "";
  for (int i = 0; i < initialWord.length(); i++)
  {
    recursivelyAddNode(initialWord[i], context);
    context = context + initialWord[i];
    if (context.length() > maxContext)
      context.erase(0, 1);
  }
}

void handleRanges(int count, int cumCount, int totalCount)
{
  int diff = (u - l + 1);
  uint8_t new_l = (diff * cumCount) / totalCount;
  uint8_t new_u = (diff * (cumCount + count)) / totalCount - 1;
  l = new_l;
  u = new_u;

  while (l <= 127 && u <= 127 || l >= 128 && u >= 128)
  {
    stream += (((l >> 7) & 1) + '0');
    l <<= 1;
    u <<= 1;
    u += 1;
  }
}

void encodeNegativeContext(int symbol)
{
  handleRanges(1, symbol, 256);
}

/**
 * Returns nullptr if symbol not found on the context given
 * and pointer to the node if found
 */

TrieNode *handleByte(int symbol, string context)
{
  auto curr = zeroContext;
  for (int i = 0; i < context.size(); i++) // traverse to context
  {
    while (curr != nullptr && curr->symbol != (int)context[i])
      curr = curr->nextNode;

    curr = curr->nextContextHead;
  }

  if (curr == nullptr) // parent context does not have any children, encode <ESC> and go to lower context
  {
    handleRanges(1, 0, 1);

    if (context == "")
      encodeNegativeContext(symbol);
    else
      handleByte(symbol, getSmallerContext(context));

    return nullptr;
  }
  else
  {
    while (curr->nextNode != nullptr && curr->symbol != symbol) // look for symbol or reach end
      curr = curr->nextNode;

    int count, cumCount, totalCount;

    if (curr->symbol == symbol) // if found symbol, get stats and encode ranges
    {
      auto originalNode = curr;
      count = curr->count;
      cumCount = curr->cumCount;
      while (curr->nextNode != nullptr)
        curr = curr->nextNode;
      totalCount = curr->cumCount + curr->count + 1;
      handleRanges(count, cumCount, totalCount);
      return originalNode;
    }
    else // if reached end, encode <ESC> and go check in lower context
    {
      count = 1;
      cumCount = curr->count + curr->cumCount;
      totalCount = cumCount + 1;
      handleRanges(count, cumCount, totalCount);
      if (context == "")
        encodeNegativeContext(symbol);
      else
        handleByte(symbol, getSmallerContext(context));

      return nullptr;
    }
  }
}

void encode(const char *inputFileName, const char *binaryFileName)
{
  ifstream inputStream(inputFileName);
  binaryStream.open(binaryFileName);

  char currentByte;
  string initialWord = "";
  string context = "";
  for (int i = 0; i < 7; i++)
  {
    inputStream.get(currentByte);
    initialWord.push_back(currentByte);
    context += currentByte;
    if (context.length() > maxContext)
      context.erase(0, 1);
  }

  createInitialNodes(initialWord);

  while (inputStream.get(currentByte))
  {
    auto foundNode = handleByte(currentByte, context);
    if (!foundNode)
      recursivelyAddNode(currentByte, context);
    else
      increaseCountAndCumCount(foundNode);

    context += currentByte;
    if (context.length() > maxContext)
      context.erase(0, 1);
  }
}

void decode()
{
}

// remember to increase range of l and u

int main()
{
  encode("enwik8", "binfile");
}