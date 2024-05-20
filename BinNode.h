#define BinNodePosi(T) BinNode<T>* //节点位置
#define stature(p) ((p) ? (p)->heighi : -1) //节点高度（空树高度为-1）
typedef enum{ RB_RED, RB_BLACK} RBColor; // 节点颜色
#include"Queue.h"

/*****BinNode状态与性质判断宏*****/
#define IsRoot(x) ( ! ( (x).parent ) )
#define IsLChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->lc ) )
#define IsRChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->rc ) )
#define HasParent(x) ( ! IsRoot(x) )
#define HasLChild(x) ( (x).lc )
#define HasRChild(x) ( (x).rc )
#define HasChild(x) ( HasLChild(x) || HasRChild(x) ) //至少拥有一个孩子
#define HasBothChild(x) ( HasLChild(x) && HasRChild(x) ) //同时拥有两个孩子
#define IsLeaf(x) (!HasChild(x))

/*****与BinNode具有特定关系的节点及指针*****/
#define sibling(p) /*兄弟*/ \
   (IsLChild(*(p)) ? (p)->parent->rc : (p)->parent->lc)
#define uncle(x) /*叔叔*/ \
    (IsLChild(*((x)->parent)) ? (x)->parent->parent->rc : (x)->parent->parent->lc)
#define FromParentTo(x) /*来自父亲的引用*/ \
    (IsRoot(x) ? _root : (IsLChild(x) ? (x).parent->lc : (x).parent->rc))

/*****BinNode模板类*****/
template <typename T> struct BinNode 
{
    // 成员（为简化描述起见统一开放，读者可根据需要迕一步封装）
    T data; // 数值
    BinNodePosi(T) parent; // 父节点
    BinNodePosi(T) lc;     // 左孩子
    BinNodePosi(T) rc;     // 右孩子
    int height;            // 高度（通用）
    int npl;               // nullptr Path Length（左式堆，也可直接用height代替）
    RBColor color;         // 颜色（红黑树）

    // 构造函数
    BinNode() : 
       parent( nullptr ), lc(nullptr), rc(nullptr), height(0), npl(1), color(RB_RED) {}
    BinNode(T e, BinNodePosi(T) p = nullptr, BinNodePosi(T) lc = nullptr, BinNodePosi(T) rc = nullptr,
            int h = 0, int l = 1, RBColor c = RB_RED) : 
        data(e), parent(p), lc(lc), rc(rc), height(h), npl(l), color(c) {}
    
    // 操作接口
    int size();                                        // 统计当前节点后代总数，亦即以其为根的子树的归模
    BinNodePosi(T)insertAsLC(T const &);             // 作为当前节点的左孩子插入新节点
    BinNodePosi(T)insertAsRC(T const &);             // 作为当前节点的右孩子插入新节点
    BinNodePosi(T)succ();                            // 取当前节点的直接后继
    template < typename VST > void travLevel(VST &); // 子树局次遍历
    template <typename VST> void travPre(VST &);             // 子树先序遍历
    template < typename VST > void travIn(VST &);    // 子树中序遍历
    template < typename VST > void travPost(VST &);  // 子树后序遍历

    // 比较器、判等器（各列其一，其余自行补充）
    bool operator<(BinNode const &bn){ return data < bn.data;}  // 小于
    bool operator==(BinNode const &bn) { return data == bn.data; } // 等于
};

/*****插入操作*****/
template <typename T> BinNodePosi(T) BinNode<T>::insertAsLC(T const &e) 
{ return lc = new BinNode(e, this); } // 将e作为当前节点的左孩子插入二叉树

template <typename T> BinNodePosi(T) BinNode<T>::insertAsRC(T const &e)
{ return rc = new BinNode(e, this); } //将e作为当前节点的右孩子插入二叉树



/*****遍历操作*****/
// 先序遍历
template <typename T>
template <typename VST>
inline void BinNode<T>::travPre(VST& visit)
{
    if ( !this ) return;
    visit(this->data);
    travPre(this->lc, visit);
    travPre(this->rc, visit);
}

// 中序遍历
template <typename T>
template <typename VST>
inline void BinNode<T>::travIn(VST &visit)
{
    travPre(this->lc, visit);
    visit(this->data);
    travPre(this->rc, visit);
}

// 后序遍历
template <typename T>
template <typename VST>
inline void BinNode<T>::travPost(VST &visit)
{
    travPre(this->lc, visit);
    travPre(this->rc, visit);
    visit(this->data);
}

// 层次遍历
template <typename T>
template <typename VST>
inline void BinNode<T>::travLevel(VST &visit)
{
    Queue<BinNodePosi(T)> Q; // 辅助队列
    Q.enqueue(this);         // 根节点入队
    while (!Q.empty())
    {
        BinNodePosi(T) x = Q.dequeue(); visit (x->data);
        if (HasLChild (*x) ) Q.enqueue (x->lc);
        if (HasRChild (*x) ) Q.enqueue (x->rc);
    }
    
}
