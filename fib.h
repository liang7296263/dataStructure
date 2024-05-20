#ifndef FIB_H
#define FIB_H

class Fib
{
private:
    int f, g; //两个紧邻的私有成员变量
public:
    //初始化不小于n的最小Fibonacci项
    Fib(int n){f = 1; g = 0; while(g < n) next();} //反复调用next直到不超过n
    int get() {return g;}
    int next(){g += f; f = g - f; return g;}
    int prev(){f = g - f; g -= f; return g;}
};

#endif