#include <iostream>
using namespace std;

void print(uint8_t x)
{
  for (int i = 7; i >= 0; i--)
  {
    cout << ((x >> (i)) & 1);
  }
  cout << endl;
}

int main()
{
  uint8_t x = 54;
  print(x);

  x <<= 1;
  x += 1;
  print(x);
}