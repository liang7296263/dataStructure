#ifndef GRAPH_H
#define GRAPH_H

typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus; //顶点状态
typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EType; //边在遍历树中所属的类型
#include"Stack.h"
#include <climits>
#include"Queue.h"

/*****图ADT*****/
template <typename Tv, typename Te> // 顶点类型、边类型
class Graph{
private:
    void reset()
    { // 所有顶点、边的辅劣信息复位
        for (int i = 0; i < n; i++)
        { // 所有顶点的
            status(i) = UNDISCOVERED;
            dTime(i) = fTime(i) = -1; // 状态，时间标签
            parent(i) = -1;
            priority(i) = INT_MAX;    // （在遍历树中的）父节点，优先级数
            for(int j = 0; j < n; j++) // 所有边的
                if (exists(i, j)) type(i, j) = UNDETERMINED; // 类型
        }
    }
    void BFS(int, int &);                       // （连通域）广度优先搜索算法
    void DFS(int, int &);                       // （连通域）深度优先搜索算法
    void BCC(int, int &, Stack<int> &);         // （连通域）基于DFS的双连通分量分解算法
    bool TSort(int, int &, Stack<Tv> *);        // （连通域）基于DFS的拓扑排序算法
    template < typename PU > void PFS(int, PU); // （连通域）优先级搜索框架
public:
// 顶点
    int n;                                // 顶点总数
    virtual int insert(Tv const &) = 0; // 插入顶点，返回编号
    virtual Tv remove(int) = 0;         // 删除顶点及其兲联边，返回该顶点信息
    virtual Tv &vertex(int) = 0;        // 顶点v的数据（该顶点的确存在）
    virtual int inDegree(int) = 0;      // 顶点v的入度（该顶点的确存在）
    virtual int outDegree(int) = 0;     // 顶点v的出度（该顶点的确存在）
    virtual int firstNbr(int) = 0;      // 顶点v的首个邻接顶点
    virtual int nextNbr(int, int) = 0;  // 顶点v的（相对于顶点j的）下一邻接顶点
    virtual VStatus &status(int) = 0;   // 顶点v的状态
    virtual int &dTime(int) = 0;        // 顶点v的时间标签dTime
    virtual int &fTime(int) = 0;        // 顶点v的时间标签fTime
    virtual int &parent(int) = 0;       // 顶点v在遍历树中的父亲
    virtual int &priority(int) = 0;     // 顶点v在遍历树中的优先级数
// 边（这里约定，无向边均统一转化为方向互逆的一对有向边，从而将无向图规作有向图的特例）
    int e;                              // 边总数
    virtual bool exists(int, int) = 0;  // 边(v, u)是否存在
    virtual void insert(Te const &, int, int, int) = 0; // 在顶点v和u之间插入权重为w的边e
    virtual Te remove(int, int) = 0;    // 删除顶点v和u之间的边e，返回该边信息
    virtual EType &type(int, int) = 0;
    virtual Te &edge(int, int) = 0;     // 边(v, u)的数据（该边的确存在）
    virtual int &weight(int, int) = 0;  // 边(v, u)的权重
// 算法
    void bfs(int);                              // 广度优先搜索算法
    void dfs(int);                              // 深度优先搜索算法
    void bcc(int);                              // 基于DFS的双连通分量分解算法
    Stack < Tv > *tSort(int);                   // 基于DFS的拓扑排序算法
    void prim(int);                             // 最小支撑树Prim算法
    void dijkstra(int);                         // 最短路径Dijkstra算法
    template < typename PU > void pfs(int, PU); // 优先级搜索框架
};

/*****广度优先搜索算法*****/
//全图算法bfs
template <typename Tv, typename Te>
void Graph<Tv, Te>::bfs(int s){
    reset(); int clock = 0; int v = s; //初始化
    do
    {
        if (UNDISCOVERED == status (v)) //一旦遇到尚未发现的顶点
            BFS( v, clock); //即从该顶点处罚启动一次BFS 
    } while ( s != ( v = ( ++v % n))); //按序号检查，不漏不重
}
//单个连通域算法BFS
template <typename Tv, typename Te>
void Graph<Tv, Te>::BFS(int v, int &clock){
    Queue<int> Q;
    status (v) = DISCOVERED; Q.enqueue(v); //初始化起点
    while (!Q.empty()) {
        int v = Q.dequeue(); dTime ( v ) = ++clock; //叏出队首顶点v
        for (int u = firstNbr(v); -1 < u; u = nextNbr(v, u)) // 构丼v癿所有邻屁u
            if (UNDISCOVERED == status(u)) {
                status ( u ) = DISCOVERED; Q.enqueue ( u ); //収现诠顶点
                type ( v, u ) = TREE; parent ( u ) = v; //引入树边拓展支撑树
            } else{ //若u已被収现，戒者甚至已讵问完毕，则
                type(v, u) = CROSS; // 将(v, u)弻类亍跨边
            }
        status(v) = VISITED; // 至此，弼前顶点讵问完毕
    }
}

/*****深度优先搜索算法*****/
// 全图算法dfs
template <typename Tv, typename Te> // 深度优先搜索DFS算法（全图）
void Graph<Tv, Te>::dfs(int s)
{ // assert: 0 <= s < n
    reset(); int clock = 0; int v = s;  // 初始化
    do                                  // 逐一检查所有顶点
        if (UNDISCOVERED == status(v))  // 一旦遇到尚未发现的顶点
           DFS(v, clock);               // 即从该顶点出发启动一次DFS
    while (s != (v = (++v % n)));       // 按序号检查，故不漏不重
}
#endif
