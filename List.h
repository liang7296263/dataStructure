typedef int Rank;
#define ListNodePosi(T) ListNode<T>*
#define NULL nullptr
#include<cstdlib>

/*****列表节点模板类*****/
template <typename T> struct ListNode
{
    //成员
    T data; ListNodePosi(T) pred; ListNodePosi(T) succ;              //数值/前驱/后继

    //构造函数
    ListNode() {}
    ListNode(T e, ListNodePosi(T) p = NULL, ListNodePosi(T) s = NULL)
       :data(e), pred(p), succ(s) {} //默认构造函数
    
    //操作接口
    ListNodePosi(T) insertAsPred (T const& e);
    ListNodePosi(T) insertAsSucc (T const &e);
};

/*****List模板类*****/
template <typename T> class List{

private:
    int _size; //规模
    ListNodePosi(T) header; //头哨兵
    ListNodePosi(T) trailer; //尾哨兵

protected:
    void init(); // 列表创建时的初始化始化
    int clear(); // 清除所有节点
    void copyNodes(ListNodePosi(T), int); // 复制列表中自位置p起的n项
    void merge(ListNodePosi(T) &, int, List<T> &, ListNodePosi(T), int); // 归并
    void mergeSort(ListNodePosi(T) &, int); // 对从p开始连续的n个节点归并排序
    void selectionSort(ListNodePosi(T), int); // 对从p开始连续的n个节点选择排序
    void insertionSort(ListNodePosi(T), int); // 对从p开始连续的n个节点插入排序

public:
    //构造函数
    List() { init(); } // 默认构造函数
    List(List<T> const &L); // 整体复制列表L
    List(List<T> const &L, Rank r, int n); // 复制列表L中自第r项起的n项
    List(ListNodePosi(T) p, int n); // 复制列表中自位置p起的n项

    //析构函数
    ~List();
    
    //只读访问接口
    Rank size() const { return _size; } // 规模
    bool empty() const { return _size <= 0; } // 判空
    T &operator[] (Rank r) const;  // 重载，支持循秩访问（效率低）
    ListNodePosi(T) first() const { return header->succ; } // 首节点位置
    ListNodePosi(T) last() const { return trailer->pred; } // 末节点位置
    bool valid(ListNodePosi(T) p)  // 判断位置p是否对外合法
    {return p && (trailer != p) && (header != p);} // 将头、尾节点等同于NULL
    int disordered() const; // 判断列表是否已排序
    ListNodePosi(T) find(T const &e) const // 无序列表查找
    { return find(e, _size, trailer); }
    ListNodePosi(T) find(T const &e, int n, ListNodePosi(T) p) const; // 无序区间查找
    ListNodePosi(T) search(T const &e) const  // 有序列表查找
    {return search(e, _size, trailer);}
    ListNodePosi(T) search(T const &e, int n, ListNodePosi(T) p) const;
    ListNodePosi(T) selectMax(ListNodePosi(T) p, int n);   // 在p及其n-1个后继中选出最大者
    ListNodePosi(T) selectMax() { return selectMax(header->succ, _size); } // 整体最大者

    //可写访问接口
    ListNodePosi(T) insertAsFirst(T const &e); // 将e当作首节点插入
    ListNodePosi(T) insertAsLast(T const &e);  // 将e当作末节点插入
    ListNodePosi(T) insertA(ListNodePosi(T) p, T const &e); // 将e当作p的后继插入
    ListNodePosi(T) insertB(ListNodePosi(T) p, T const &e); // 将e当作p的前驱插入
    T remove(ListNodePosi(T) p);  // 删除合法位置p处的节点,返回被删除节点
    void merge(List<T> &L) { merge(first(), size, L, L.first(), L._size); } // 全列表归并
    void sort(ListNodePosi(T) p, int n);  // 列表区间排序
    void sort() { sort(first(), _size); } // 列表整体排序
    int deduplicate();                    // 无序去重
    int uniquify();                       // 有序去重
    void reverse();                       // 前后倒置（习题）

    //遍历
    void traverse(void (* )(T &)); // 遍历，依次实施visit操作（函数指针，只读或局部性修改
    template <typename VST>        // 操作器
    void traverse(VST &);          // 遍历，依次实施visit操作（函数对象，可全局性修改）
};

/*****构造函数*****/

//默认构造函数
template <typename T> void List<T>::init(){//通过调用init函数初始化列表
    //创建首尾哨兵
    header = new ListNode<T>; 
    trailer = new ListNode<T>;
    //设置首尾哨兵前驱后继
    header->succ = trailer; header->pred = NULL;
    trailer->pred = header; trailer->succ = NULL;
    _size = 0;
}
//节点复制函数
template <typename T> void List<T>::copyNodes(ListNodePosi(T) p, int n){
    init();
    //将起自p的n项依次作为末节点插入
    while(n--) {insertAsLast(p->data); p = p->succ;}
}
//复制构造函数
template <typename T> List<T>::List(ListNodePosi(T) p, int n){
    copyNodes(p, n);
}//复制列表中自位置P起的n项
template <typename T> List<T>::List(List<T> const& L)
{
    copyNodes(L.first(), L._size);
} // 整体复制列表L
template <typename T> List<T>::List(List<T> const &L, int r, int n)
{
    copyNodes(L[r], n);
} // 复制列表L中第r项起的N项

/*****[]操作符重载*****/
template<typename T>
T& List<T>::operator[](Rank r) const {//寻秩访问（效率低下）
    ListNodePosi(T) p = first();
    while(0 < r--) p = p->succ;//顺数第r个节点
    return p->data;
}

/*****无序列表元素查找接口find*****/
template <typename T> // 在无序列表内节点p（可能是trailer）的n个（真）前驱中，找到等于e的最后者
ListNodePosi(T) List<T>::find(T const &e, int n, ListNodePosi(T) p) const
{
    while(0 < n--)                             // （0 <= n <= rank(p) < _size）对于p的最近的n个前驱，从右向左
       if(e == (p = p->pred)->data) return p; // 逐个比对，直至命中或范围越界
    return NULL;                              // p越出左边界意味着区间内不含e，查找失败
} // 失败时，返回NULL

/*****节点插入操作*****/

//统一接口
template <typename T> 
ListNodePosi(T) List<T>::insertAsFirst( T const& e)
{_size++; return header->insertAsSucc(e); } //e当作首节点插入
template <typename T>
ListNodePosi(T) List<T>::insertAsLast(T const &e)
{_size++; return trailer->insertAsPred(e);} //e当作末节点插入
template <typename T>
ListNodePosi(T) List<T>::insertA (ListNodePosi(T) p, T const& e)
{_size++; return p->insertAsSucc(e);} //e当作p的后继插入
template <typename T>
ListNodePosi(T) List<T>::insertB (ListNodePosi(T) p, T const &e)
{size++; return p->insertAsPred(e); } //e当作p的前驱插入

//前插入函数
template <typename T>
ListNodePosi(T) ListNode<T>::insertAsPred(T const& e){
    ListNodePosi(T) x = new ListNode (e, pred, this);
    pred->succ = x; pred = x;
    return x;
} //主要步骤：new出新节点，设置前后驱连接

//后插入函数
template <typename T>
ListNodePosi(T) ListNode<T>::insertAsSucc(T const &e){
    ListNodePosi(T) x = new ListNode (e, this, succ);
    succ->pred = x; succ = x;
    return x;
} //主要步骤：new出新节点，设置前后驱连接

/*****节点删除操作*****/
template <typename T> T List<T>::remove (ListNodePosi(T) p){
    T e = p->data;
    p->pred->succ = p->succ; p->succ->pred = p->pred;
    delete p; _size--;
    return e;
}//主要步骤：设置前后驱节点的前后驱链接，释放本节点

/*****析构函数*****/
//列表清空函数
template <typename T> int List<T>::clear(){
    int oldSize = _size;
    while (_size > 0) remove(header->succ);//反复删除首节点
    return oldSize;
}
// 析构函数
template <typename T> List<T>::~List(){
    clear(); delete header; delete trailer; //删除所有节点和哨兵
}

/*****无序列表去重*****/
template <typename T>
int List<T>::deduplicate()
{                            // 剔除无序列表中的重复节点
    if(_size < 2) return 0; // 平凡列表自然无重复
    int oldSize = _size;    // 记住原规模
    ListNodePosi(T) p = header;
    Rank r = 0; // p从首节点开始
    while(trailer != (p = p->succ))
    {                                            // 依次直到末节点
        ListNodePosi(T) q = find(p->data, r, p); // 在p的r个（真）前驱中查找雷同者
        q ? remove(q) : r++;                    // 若的确存在，则删除；否则秩加一
    }                        // assert: 循环过程中的任意时刻，p的所有前驱互不相同
    return oldSize - _size; // 列表规模发化量，即被删除元素总数
}

/*****有序列表去重*****/
template <typename T>
int List<T>::uniquify()
{                            // 成批剔除重复元素，效率更高
    if(_size < 2) return 0; // 平凡列表自然无重复
    int oldSize = _size;    // 记住原规模
    ListNodePosi(T)p = first();
    ListNodePosi(T) q;                 // p为各区殌起点，q为其后继
    while(trailer != (q = p->succ))   // 反复考查紧邻的节点对(p, q)
        if(p->data != q->data) p = q; // 若互异，则转向下一区殌
    else remove(q);                   // 否则（雷同），删除后者
    return oldSize - _size;          // 列表规模发化量，即被删除元素总数
}

/*****遍历*****/
template <typename T>
void List<T>::traverse(void (*visit)(T &)) // 借助函数指针机制遍历
{
    for (ListNodePosi(T) p = header->succ; p != trailer; p = p->succ)
        visit(p->data);
}
template <typename T>
template <typename VST>              // 元素类型、操作器
void List<T>::traverse(VST &visit) // 借助函数对象机制遍历
{
    for (ListNodePosi(T) p = header->succ; p != trailer; p = p->succ)
        visit(p->data);
}

/*****有序列表查找*****/
template <typename T>
ListNodePosi(T) List<T>::search(T const &e, int n, ListNodePosi(T) p) const
{
    while (0 <= n--)
        if (((p = p->pred)->data) <= e) break; //直至命中或者范围越界
    return p;
} //失败时返回左边届的前驱，调用者可通过valid(),判断是否成功


/*****排序****/
template <typename T>
void List<T>::sort(ListNodePosi(T) p, int n)
{ // 列表区间排序
    switch ( rand() % 3)
    { // 随机选取排序算法。可根据具体问题的特点灵活选取或扩充
       case 1 : insertionSort(p, n); break; // 插入排序
       case 2 : selectionSort(p, n); break; // 选择排序
       default : mergeSort(p, n); break; // 归并排序
    }
}

//插入排序
template <typename T> // 列表的插入排序算法：对起始于位置p的n个元素排序
void List<T>::insertionSort(ListNodePosi(T) p, int n)
{ // valid(p) && rank(p) + n <= size
    for (int r = 0; r < n; r++)
    {                                              // 逐一为各节点
        insertA(search(p->data, r, p), p->data); // 查找适当的位置并插入
        p = p->succ; remove(p->pred); // 转向下一节点
    }
}

//选择排序
template <typename T> // 列表的选择排序算法：对起始于位置p的n个元素排序
void List<T>::selectionSort(ListNodePosi(T) p, int n)
{ // valid(p) && rank(p) + n <= size
    ListNodePosi(T)head = p->pred;
    ListNodePosi(T) tail = p;
    for(int i = 0; i < n; i++) tail = tail->succ; // 待排序区间为(head, tail)
    while(1 < n)
    {                                                   // 在至少还剩两个节点之前，在待排序区间内
        ListNodePosi(T)max = selectMax(head->succ, n); // 找出最大者（歧义时后者优先）
        insertB(tail, remove(max));                    // 将其移至无序区间末尾（作为有序区间新的首元素）
        tail = tail->pred;
        n--;
    }
}

//归并排序
template <typename T>
void List<T>::merge (ListNodePosi(T) &p, int n, List<T> &L, ListNodePosi(T) q, int m){
    ListNodePosi(T) pp = p->pred;
    while (m > 0)
    {
        if ((n > 0) && (p->data <= q->data))
        {
            if (q == (p = p->succ)) break; n--;
        }
        else
        {
            insertB(p, L.remove((q = q->succ)->pred));
            m--;
        }
    }
    p = pp->succ;
}//二路归并

template <typename T> // 列表的归并排序算法：对起始于位置p的n个元素排序
void List<T>::mergeSort(ListNodePosi(T) & p, int n)
{                      // valid(p) && rank(p) + n <= size
    if(n < 2) return; // 若待排序范围已足够小，则直接返回；否则...
    int m = n >> 1;   // 以中点为界
    ListNodePosi(T)q = p;
    for (int i = 0; i < m; i++)
        q = q->succ; // 均分列表
    mergeSort(p, m);
    mergeSort(q, n - m);           // 对前、后子列表分删排序
    merge(p, m, *this, q, n - m); // 归并
}