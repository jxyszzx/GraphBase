# openMP
## 编译
* 指令
    * `g++ test.cpp -o main.exe -fopenmp`
    * `g++ test.cpp -fopenmp`
## 循环的并行
* OpenMP设计时希望提供一种简单的方式让程序员不需要懂得创建和销毁线程就能写出多线程化程序。
    * 为此他们设计了一些pragma指令和函数来让编译器能够在合适的地方插入线程，大多数的循环只需要在for之前插入一个pragma就可以实现并行化
    * `#pragma omp parallel for`
* 对可以多线程的循环要求
    1. 循环的变量（就是i）必须是有符号整形，其他的都不行。
    2. 循环的比较条件必须是< <= > >=中的一种
    3. 循环的增量部分必须是增减一个不变的值（即每次循环是不变的）。
    4. 如果比较符号是< <=，那每次循环i应该增加，反之应该减小
    5. 循环必须是没有奇奇怪怪的东西，不能从内部循环跳到外部循环，goto和break只能在循环内部跳转，异常必须在循环内部被捕获。
* 检测是否支持OpenMP
    ```
    #ifndef _OPENMP
        fprintf(stderr, "OpenMP not supported");
    #endif
    ```