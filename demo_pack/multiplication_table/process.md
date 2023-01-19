The first time I saw this problem, I come up with the for-loop as below:

  ```for  (int i=1;i<=9;i++){
    for (int j = i; j <= 9; ++j){
      cout<<i<<"x"<<j<<"="<<i*j;
    }
  ```

However I found it seems impossible to convert the print into multi-columns.

So, it should rather be printed by rows.

Then it's not difficult to figure that `a * b = c `in every row, with the number of column added, a gains one by one, and of course `b` stays.

OK, the final for-loop shall be as follow:

```for  (int i=1;i<=9;i++){
    for (int j = 1; j <= i; ++j){
      cout<<j<<"x"<<i<<"="<<i*j;
    }
    cout<<endl;
```
