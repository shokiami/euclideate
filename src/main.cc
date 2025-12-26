#include "types.h"

int main() {
  QuadNum x = QuadNum(1, 2, 3);
  QuadNum y = QuadNum(1, -2, 3);
  cout << x / y << endl;
  return 0;
}
