#include <iostream>

int main()
{
  for  (int i = 1; i <= 9; ++i){
    for (int j = 1; j <= i; ++j){
      std::cout << j << "x" << i << "=" << i * j << " ";
    }
    std::cout << std::endl;
  }
  system ("pause");
}
