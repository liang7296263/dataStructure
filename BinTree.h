#include"BinNode.h"

template <typename T> class BinTree
{
protected:
    int _size; //规模
    BinNodePosi(T) _root; //根节点
    virtual int updateHeight(BinNodePosi(T) x); // 更新节点x的高度
    void updateHeightAbove(BinNodePosi(T) x);   // 更新节点x及其祖先的高度
public:
    BinTree() : _size (0), _root (nullptr) {}; // 构造函数
    ~BinTree() {if (0 < _size) remove(_root);};
    int size() const { return _size; } //返回规模
    bool empty() const { return !_root; } // 判空
    BinNodePosi(T) root() const { return _root; } // 返回树根
    BinNodePosi(T) insertAsRoot(T const &e);      // 插入根节点
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x, T const &e); // e作为x的左孩子（原无）插入
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x, T const &e); // e作为x的右孩子（原无）插入
    BinNodePosi(T) attachAsLC(BinNodePosi(T) x, BinTree<T> *&T); // T作为x左子树接入
    BinNodePosi(T) attachAsRC(BinNodePosi(T) x, BinTree<T> *&T); // T作为x右子树接入
    int remove(BinNodePosi(T) x); // 删除以位置x处节点为根的子树，返回该子树原先的规模
    BinTree<T> *secede(BinNodePosi(T) x); // 将子树x从当前树中摘除，并将其转换为一棵独立子树
    template <typename VST> // 操作器
    void travLevel(VST &visit){ if (_root) _root->travLevel(visit);} // 局次遍历
    template <typename VST>   // 操作器
    void travPre(VST &visit){ if (_root) _root->travPre(visit);} // 先序遍历
    template <typename VST>                                      // 操作器
    void travIn(VST &visit){ if (_root) _root->travIn(visit);} // 中序遍历
    template < typename VST > // 操作器
    void travPost(VST &visit){ if (_root) _root->travPost(visit);}  // 后序遍历
    bool operator<(BinTree<T> const &t) // 比较器
    {
        return _root && t._root && lt(_root, t._root);
    }
    bool operator==(BinTree<T> const &t) // 判等器
    {
        return _root && t._root && (_root == t._root);
    }
};

/*****更新高度*****/
//更新节点X的高度
template <typename T> int BinTree<T>::updateHeight (BinNodePosi(T) x){
    return x->height = 1 + max (stature (x->lc), stature (x->rc));
}
//更新高度
template <typename T> void BinTree<T>::updateHeightAbove(BinNodePosi(T) x){
    while (x) { updateHeight(x); x = x->parent;}//从x出发，覆盖历代祖先
}

/*****节点插入*****/
template <typename T> 
BinNodePosi(T) BinTree<T>::insertAsLC ( BinNodePosi(T) x, T const& e )
{ _size++; x->insertAsLC ( e ); updateHeightAbove ( x ); return x->lc; } //e插入为x的左孩子
template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, T const &e)
{_size++; x->insertAsRC ( e ); updateHeightAbove ( x ); return x->rc;}//e插入为x的右孩子

/*****子树接入*****/
template <typename T> // 二叉树子树接入算法：将S当作节点x的左子树接入，S本身置空
BinNodePosi(T) BinTree<T>::attachAsLC(BinNodePosi(T) x, BinTree<T> *&S)
{                                            // x->lc == NULL
    if(x->lc = S->_root) x->lc->parent = x; // 接入
    _size += S->_size; updateHeightAbove(x); // 更新全树规模不x所有祖先的高度
    S->_root = NULL;
    S->_size = 0; release(S); S = NULL; return x; // 释放原树，返回接入位置
}

template <typename T> // 二叉树子树接入算法：将S当作节点x的右子树接入，S本身置空
BinNodePosi(T) BinTree<T>::attachAsRC(BinNodePosi(T) x, BinTree<T> *&S)
{                                             // x->rc == NULL
    if(x->rc = S->_root) x->rc->parent = x; // 接入
    _size += S->_size; updateHeightAbove(x); // 更新全树规模不x所有祖先的高度
    S->_root = NULL; S->_size = 0; release(S); S = NULL; return x; // 释放原树，返回接入位置
}

/*****子树删除*****/
template <typename T> // 删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
int BinTree<T>::remove(BinNodePosi(T) x)
{                                  // assert: x为二叉树中的合法位置
    FromParentTo(*x) = NULL;      // 切断来自父节点的指针
    updateHeightAbove(x->parent); // 更新祖先高度
    int n = removeAt(x); _size -= n; return n; // 删除子树x，更新规模，返回删除节点总数
}
template <typename T> // 删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
static int removeAt(BinNodePosi(T) x)
{                                                    // assert: x为二叉树中的合法位置
    if(!x) return 0;                                // 递归基：空树
    int n = 1 + removeAt(x->lc) + removeAt(x->rc); // 递归释放左、右子树
    release(x->data); release(x); return n; // 释放被摘除节点，并返回删除节点总数
}

/*****子树分离*****/
template <typename T> // 二叉树子树分离算法：将子树x从当前树中摘除，将其封装为一棵独立子树返回
BinTree<T> *BinTree<T>::secede(BinNodePosi(T) x)
{                                  // assert: x为二叉树中的合法位置
    FromParentTo(*x) = NULL;      // 切断来自父节点的指针
    updateHeightAbove(x->parent); // 更新原树中所有祖先的高度
    BinTree < T > *S = new BinTree<T>; S->_root = x; x->parent = NULL; // 新树以x为根
    S->_size = x->size(); _size -= S->_size; return S; // 更新规模，返回分离出来的子树
}

