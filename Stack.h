#include"Vector.h"

template <typename T> class Stack : public Vector<T>
{
   public:
      void push(T const &e) { insert(this->Size(), e); } // 入栈：等效于将新元素作为向量元素的末元素插入
      T pop() { return remove(this->Size() - 1); }       // 出栈：等效于删除向量的末元素
      T &top() { return (*this)[this->Size() - 1]; }     // 取顶：直接返回向量的末元素
};