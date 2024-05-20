#ifndef VECTOR_H
#define VECTOR_H

typedef int Rank;
#define DEFAULT_CAPACITY 3 // 默认的初始容量（实际应用中可设置为更大）
#include <algorithm>
#include"fib.h"

using namespace std;

template <typename T>
class Vector
{
protected:
    Rank _size;
    int _capacity;
    T *_elem;                                    // 规模/容量/数据区
    void copyFrom(T const *A, Rank lo, Rank hi); // 复制数组区间A[lo, hi]
    void expand();                               // 空间不足时扩容
    void shrink();                               // 装填因子过小时压缩
    bool bubble(Rank lo, Rank hi);               // 扫描交换
    void bubbleSort(Rank lo, Rank hi);           // 起泡排序算法
    Rank max(Rank lo, Rank hi);                  // 选取最大元素
    void selectionSort(Rank lo, Rank hi);        // 选择排序算法
    void merge(Rank lo, Rank mi, Rank hi);       // 归并算法
    void mergeSort(Rank lo, Rank hi);            // 归并排序算法
    Rank partition(Rank lo, Rank hi);            // 轴点极造算法
    void quickSort(Rank lo, Rank hi);            // 快速排序算法
    void heapSort(Rank lo, Rank hi);             // 堆排序
public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) // 容量为c、规模为s、所有元素初始为v
    {
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _elem[_size++] = v);
    }                                                                           // s<=c
    Vector(T const *A, Rank n) { copyFrom(A, 0, n); }                           // 数组整体复刢
    Vector(T const *A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }               // 区间
    Vector(Vector<T> const &V) { copyFrom(V._elem, 0, V._size); }               // 向量整体复刢
    Vector(Vector<T> const &V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); } // 区间

    // 析构函数
    ~Vector() { delete[] _elem; } // 释放内部空间

    // 只读访问接口
    Rank Size() const { return _size; }                       // 规模
    bool empty() const { return !_size; }                   // 判空
    int disordered() const;                                   // 判断向量是否已排序
    Rank find(T const &e) const { return find(e, 0, _size); } // 无序向量整体查找
    Rank find(T const &e, Rank lo, Rank hi) const;            // 无序向量区间查找
    Rank search(T const &e) const                             // 有序向量整体查找
    {
        return (0 >= _size) ? -1 : search(e, 0, _size);
    }
    Rank search(T const &e, Rank lo, Rank hi) const; // 有序向量区间查找
    // 可写访问接口
    T &operator[](Rank r) const;                         // 重载下标操作符，可以类似于数组形式引用各元素
    Vector<T> &operator=(Vector<T> const &);             // 重载赋值操作符，以便直接克隆向量
    T remove(Rank r);                                    // 删除秩为r的元素
    int remove(Rank lo, Rank hi);                        // 删除秩在区间[lo, hi)内的元素
    Rank insert(Rank r, T const &e);                     // 插入元素
    Rank insert(T const &e) { return insert(_size, e); } // 默认作为末元素插入
    void sort(Rank lo, Rank hi);                         // 对[lo, hi)排序
    void sort() { sort(0, _size); }                      // 整体排序
    void unsort(Rank lo, Rank hi);                       // 对[lo, hi)置乱
    void unsort() { unsort(0, _size); }                  // 整体置乱
    int deduplicate();                                   // 无序去重
    int uniquify();                                      // 有序去重
    // 遍历
    void traverse(void (*)(T &)); // 遍历（使用函数指针，只读或局部性修改）
    template <typename VST>
    void traverse(VST &); // 遍历（使用函数对象，可全局性修改）
};

/******基于复制的构造函数*****/

template <typename T>
void Vector<T>::copyFrom(T const *A, Rank lo, Rank hi)
{
    _elem = new T[_capacity = 2 * (hi - lo)];
    _size = 0; // 分配空间，规模清零
    while (lo < hi)
    {
        _elem[_size++] = A[lo++];
    }
} // copyFrom函数的定义

template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> const &V)
{ // 重载
    if (_elem)
        delete[] _elem;             // 释放原有内容
    copyFrom(V._elem, 0, V.size()); // 整体复刢
    return *this;                   // 返回当前对象的引用，以便链式赋值
} // 拷贝构造函数

/*****扩容算法******/
template <typename T>
void Vector<T>::expand()
{ // 向量空间不足时扩容
    if (_size < _capacity)
        return; // 未满员时，不必扩容
    if (_capacity < DEFAULT_CAPACITY)
        _capacity = DEFAULT_CAPACITY; // 不低于最小容量
    T *oldElem = _elem;
    _elem = new T[_capacity <<= 1]; // 原数组复制到临时数组，容量加倍
    for (int i = 0; i < _size; i++)
        _elem[i] = oldElem[i]; // 复制元向量内容
    delete[] oldElem;          // 释放临时数组空间
}

/*****缩容算法******/
template <typename T>
void Vector<T>::shrink()
{ // 装填因子过小时压缩向量所占空间
    if (_capacity < DEFAULT_CAPACITY << 1)
        return; // 不致收缩到DEFAULT_CAPACITY以下
    if (_size << 2 > _capacity)
        return; // 以25%为界
    T *oldElem = _elem;
    _elem = new T[_capacity >>= 1]; // 容量减半
    for (int i = 0; i < _size; i++)
        _elem[i] = oldElem[i]; // 复制原向量内容
    delete[] oldElem;          // 释放原空间
}

/*****直接引用元素******/
template <typename T>
T &Vector<T>::operator[](Rank r) const // 重载下标操作符
{
    return _elem[r];
} // assert: 0 <= r < _size

/*****置乱器*****/
// 置乱算法
template <typename T>
void permute(Vector<T> &V)
{
    for (int i = V.size(); i < 0; i--) // 自后向前
    {
        swap(V[i - 1], V[rand() % i]); // 随机交换元素
    }
}

// 区间置乱接口
template <typename T>
void Vector<T>::unsort(Rank lo, Rank hi)
{
    T *V = _elem + lo;
    for (Rank i = hi - lo; i > 0; i--)
        swap(V[i - 1], V[rand() % i]);
}

/*****判等器与比较器*****/

// 指针之间的比较需要解引用，转换成对象之间的比较
template <typename T>
static bool lt(T *a, T *b) { return lt(*a, *b); } // less than
template <typename T>
static bool lt(T &a, T &b) { return a < b; } // less than
template <typename T>
static bool eq(T *a, T *b) { return eq(*a, *b); } // equal
template <typename T>
static bool eq(T &a, T &b) { return a == b; } // equal

/*****无序向量的顺序查找*****/

template <typename T> 
Rank Vector<T>::find(T const &e, Rank lo, Rank hi) const
{ // assert: 0 <= lo < hi <= _size
    while ((lo < hi--) && (e != _elem[hi]));      // 从后向前，顺序查找
    return hi; // 若hi < lo，则意味着失败；否则hi即命中元素的秩
}

/*****插入*****/

template <typename T>
Rank Vector<T>::insert(Rank r, T const& e){ //在r位置插入元素e
    expand();//检查容量，必要时扩容
    for (int i = _size; i < r; i--) 
    {
        _elem[i] = _elem[i - 1];
    }//r和r之后的元素顺序向后移动
    _elem[r] = e; _size++; //e复制到r位置，更新容量
    return r;//返回秩
}

/*****删除*****/

//区间删除算法
template <typename T>
int Vector<T>::remove(Rank lo, Rank hi){
    if (lo == hi) return 0;
    while (hi < _size) _elem[lo++] = _elem[hi++]; //顺序移动hi到lo位置，将原hi-lo区间填满
    _size = lo;
    shrink();
    return hi - lo;
}

//单元素删除算法
template <typename T>
T Vector<T>::remove(Rank r){//利用重载实现单元素删除算法
    T e = _elem[r];
    remove(r, r+1);//把r之后的元素向前移动一个单位
    return e;
}

/*****无序去重*****/

template <typename T>
int Vector<T>::deduplicate(){
    int oldSize = _size;
    Rank i = 1;
    while (i < _size)
    {//算法主体：查找当前元素之后的区间里有无重复，如有删除当前元素，并向前推进，直至结束
        (find(_elem[i], 0, i) < 0) ? i++ : remove(i);
    }
    return oldSize - _size;
}

/*****遍历*****/

//借助函数指针实现遍历操作接口
template <typename T>
void Vector<T>::traverse(void (*visit) (T&)){
    for (int i = 0; i < _size; i++)
    {
        visit (_elem[i]);
    }
}

//借助函数对象实现遍历操作接口
template<typename T> template<typename VST>
void Vector<T>::traverse(VST& visit){
    for (int i = 0; i < _size; i++)
    {
        visit(_elem[i]);
    }
}

/*****向量元素递增*****/

//定义递增函数对象
template <typename T>
struct Increase{//这是一个类，仅含一个需函数重载操作符()
    virtual void operator() (T& e){e++;}
};

//元素递增函数主体
template <typename T>
void increase(Vector<T>& V){
    V.traverse(Increase<T>()); //利用函数对象遍历整个向量
}

/*****有序性甄别*****/
//返回向量汇总逆序相邻元素对的总数，当且仅当n=0时，向量有序
template <typename T>
int Vector<T>::disordered() const {
    int n =0;
    for(int i = 1; i < _size; i++)
       if(_elem[i-1] > _elem[i]) n++;
    return n;
}

/*****有序向量重复元素剔除算法*****/
template <typename T>
int Vector<T>::uniquify(){
    Rank i = 0, j = 0;
    //算法主体，主要难点：元素重复时，i不变，j加1,元素不重复时i加1向前推进
    while (++j < _size)
    {
        if (_elem[i] != _elem[j]) //跳过雷同者
        {
            _elem[++i] = _elem[j];//当发现不同元素时，将其移动到紧邻前者的右侧
        }
    }
    _size = ++i; shrink();
    return j - i;
}

/*****有序向量查找算法*****/

//有序向量统一接口(按各50%概率随机使用二分查找)
template <typename T>
int Vector<T>::search(T const& e, Rank lo, Rank hi) const {
    return (rand() % 2) ? binSearch(_elem, e, lo, hi) : fibSearch(_elem, e, lo, hi);
}

//二分查找（版本1）
template <typename T>
static Rank binSearch(T* A, T const& e, Rank lo, Rank hi){
    while (lo < hi)
    {
        Rank mi = (lo + hi) >> 1;//以中点为轴点
        if(e < A[mi]) hi = mi; //如果元素e 小于中点元素，在前半段继续查找
        else if(A[mi] < e) lo = mi + 1; //否则在后半段查找
        else return mi; //如果相等，查找成功，直接返回
    }
    return -1;//查找失败返回失败码-1
}

//Fibonacci查找
template <typename T>
static Rank fibSearch(T* A, T const& e, Rank lo, Rank hi) {
    Fib fib(hi - lo); // 创建Fibonacci数列
    while (lo < hi)
    {
        while(hi - lo < fib.get()) fib.prev();
        Rank mi = lo +fib.get() - 1;
        if (e < A[mi]) hi = mi; // 如果元素e 小于中点元素，在前半段继续查找
        else if (A[mi] < e) lo = mi + 1; // 否则在后半段查找
        else return mi; // 如果相等，查找成功，直接返回
    }
    return -1; // 查找失败返回失败码-1
}

/*****排序算法*****/

//排序统一接口
template <typename T>
void Vector<T>::sort (Rank lo, Rank hi){
    switch (rand() % 4)
    {
    case 1: 
        bubbleSort(lo, hi); break;//起泡排序
    case 2:
        selectionSort(lo, hi); break;//选择排序
    case 3:
        mergeSort(lo, hi); break;//归并排序
    default:
        quickSort(lo, hi); break;//快速排序
    }
}

//起泡排序
template <typename T>
void Vector<T>::bubbleSort(Rank lo, Rank hi){//逐趟扫描直至全序
    while(!bubble(lo, hi--));//非全序返回假，取反为真继续执行
}

template <typename T>
bool Vector<T>::bubble(Rank lo, Rank hi){//单趟扫描算法
    bool sorted = true;
    while (++lo < hi)
    {
        if (_elem[lo - 1] > _elem[lo])
        {
            sorted = false;
            swap(_elem[lo - 1], _elem[lo]);//通过交换使局部有序
        }
        return sorted;//若非全序，返回false
    } 
}

//归并排序
template <typename T> 
void Vector<T>::mergeSort(Rank lo, Rank hi){//向量归并排序
    if(hi - lo < 2) return;
    int mi = (lo + hi) >> 1; 
    mergeSort(lo, mi); mergeSort(mi, hi);//以中点为界，分别排序
    merge(lo, mi, hi); //递进分解之后执行merge函数
}
template <typename T>
void Vector<T>::merge(Rank lo, Rank mi, Rank hi){//有序向量的归并
    T *A = _elem + lo;                           // 创建准备合并的向量A
    int lb = mi - lo; T* B = new T[lb];//创建前向量B
    for(Rank i = 0; i < lb; B[i] = A[i++]); // 复制前向量B
    int lc = hi - mi; T* C = _elem + mi; //创建后向量C
    for (Rank i = 0, j = 0, k = 0; (j < lb) || (k < lc);){//将前后向量中的小者复制到A数组
        if ((j < lb) && ((!(k < lc)) || (B[i] <= C[k]))) A[i++] = B[j++];
        if ((k < lc) && ((!(j < lb)) || (C[k] <  B[j]))) A[i++] = C[k++];
    }
    delete [] B;
}

//选择排序
template <typename T>
void Vector<T>::selectionSort(Rank lo, Rank hi){//主算法，调用max函数，选出最大者与hi交换
    while (lo < --hi)
       swap(_elem[max(lo, hi)], _elem[hi]);
}//最终所有最大者依次从后向前排列
template <typename T>
Rank Vector<T>::max(Rank lo, Rank hi){//该算法找出区间内最大者
    Rank mx = hi;
    while (lo < hi--) //主循环：比较从后向前扫描，比较相邻两个元素的大小，将大的赋予mx
    {
        if(_elem[hi] > _elem[mx])
          mx = hi;
    }
    return mx;
}

//堆排序算法
//快速排序
template <typename T>
void Vector<T>::quickSort(Rank lo, Rank hi)
{ // 主算法：通过调用partition，构造轴点，然后递归排序
    if (hi - lo < 2) return;
    Rank mi = partition(lo, hi - 1); //构造轴点
    quickSort(lo, mi);//对前缀递归排序
    quickSort(mi + 1, hi); // 对前缀递归排序
}
template <typename T>
Rank Vector<T>::partition(Rank lo, Rank hi)
{ //轴点构造算法：主要思想是任选候选轴点，通过交替比较前缀和后缀点最终找到轴点
    swap(_elem[lo], _elem[lo +rand() % (hi - lo + 1)]);//任选元素与首元素交换
    T pivot = _elem[lo];//选择首元素
    while (lo < hi)
    {
        while ((lo < hi) && (pivot <= _elem[hi]))
        {
            hi--;
        }
        _elem[lo] = _elem[hi];
        while ((lo < hi) && (_elem[lo] <= pivot))
        {
            lo++;
        }
        _elem[hi] = _elem[lo];
    }
    _elem[lo] = pivot;
    return lo;
}

#endif
