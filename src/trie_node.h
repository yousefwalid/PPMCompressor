class TrieNode
{
public:
  TrieNode
      *prevContext,     // Points at the same node in the previous context
      *nextContextHead, // Points at the head node of the next context
      *nextNode;        // Points at the next node in the same context
  int
      symbol,   // The symbol of the current node
      count,    // The frequency of the node's symbol in the current context
      cumCount; // The cumulative count of all the nodes previous to this node

  /**
   * The default constructor for TrieNode 
   */
  TrieNode()
  {
    this->count = 1, this->cumCount = 0;
    prevContext = nextContextHead = nextNode = nullptr;
  }

  /**
   * Constructor with symbol parameter
   * @param symbol {int} The symbol of this node
   */
  TrieNode(int symbol)
  {
    this->symbol = symbol;
    this->count = 1, this->cumCount = 0;
    prevContext = nextContextHead = nextNode = nullptr;
  }

  /**
   * Increments the cumCount of the current node and all the following nodes
   * and returns the totalCount of the whole context including the ESC symbol
   * @return totalCount {int} The totalCount of this context
   */
  int updateCumCountAndGetTotalCount()
  {
    auto curr = this;
    while (curr->nextNode)
    {
      cumCount++;
      curr = curr->nextNode;
    }
    return curr->cumCount + count + 1;
  }
};