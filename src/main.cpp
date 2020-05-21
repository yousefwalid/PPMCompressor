#include <iostream>
#include <fstream>
#include <string>
#include "trie.cpp"
using namespace std;

TrieNode *zeroContext;
const int maxContext = 2; // The maximum number of contexts

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
 * Adds a node to the trie given the context, and also adds it to all the smaller contexts
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
      context.erase(0);
  }
}

void encode(const char *inputFileName)
{
  ifstream inputStream(inputFileName);
  char currentByte;
  string initialWord = "";
  for (int i = 0; i < 8; i++)
  {
    inputStream.get(currentByte);
    initialWord.push_back(currentByte);
  }

  createInitialNodes(initialWord);
}

void decode()
{
}

int main()
{
  //encode("enwik8");
  createInitialNodes("thisbis");
  int x;
  cin >> x;
}