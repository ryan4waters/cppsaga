# Multiplication Table of Nine

## What is this table

As educated in China, Multiplication Table of Nine is everyone supposed to learn to recite at very young age. It looks like blow:

![image](https://github.com/ryan4waters/cppsaga/blob/main/demo_pack/multiplication_table/Multiplication_Table_of_Nine.jpeg)

So our task is to print this table.

## Problem I faced

The first time I saw this problem, I come up with the for-loop as below:

  ```for  (int i=1;i<=9;i++){
  for (int i = 1; i <= 9; ++i){
  	for (int j = i; j <= 9; ++j) {
  		std::cout << i << "x" << j << "=" << i * j << " ";
  	}
  	std::cout<<std::endl;
  }
  ```

However I found it seems impossible to print the table in which each multiplier factor keeps still in each column.

So, it should rather be printed by rows.

Then it's not difficult to figure that `a * b = c `in every row, with the number of column added, a gains one by one, and of course `b` stays.

OK, the final answer shall be as follow:

```for  (int i=1;i<=9;i++){
for  (int i = 1; i <= 9; ++i){
    for (int j = 1; j <= i; ++j){
      std::cout << j << "x" << i << "=" << i * j << " ";
    }
    std::cout << std::endl;
  }
```
