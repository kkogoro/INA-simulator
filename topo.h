#pragma once
#include <iostream>
#include <random>
#include <cstdio>
#include <cstddef>
#include <set>
#include <algorithm>

const double eps = 1e-2;
struct Edge;
struct Node;

struct Node
{
    Edge *firstEdge;
    Node()
    {
        firstEdge = NULL;
    }
    ~Node()
    {
        delete firstEdge;
    }
};

typedef std::pair<int, int> PII;
typedef std::set<PII, std::less<PII>> Heap;
const int INF = 0x7fffffff;

struct Edge
{
    double capacity, flow;
    int total_flow;
    Edge *nextEdge;
    int from, to;
    bool blocked()
    {
        return capacity - flow < eps;
    }
    Edge()
    {
        capacity = .0;
        flow = .0;
        from = to = 0;
        total_flow = 0;
        nextEdge = NULL;
    }
    ~Edge()
    {
        if (nextEdge)
        {
            delete nextEdge;
        }
    }
};

struct Topo
{
    int n;
    Node *nodes;
    int *dis;
    bool *vis;
    Edge **last_edge;
    Heap min_heap;
    Topo(int n) : n(n)
    {
        last_edge = new Edge *[n];
        nodes = new Node[n];
        dis = new int[n];
        vis = new bool[n];
    }
    ~Topo()
    {
        delete[] nodes;
    }
    void addEdge(int from, int to, double capacity)
    {
        Edge *edge = new Edge();
        edge->from = from;
        edge->to = to;
        edge->capacity = capacity;
        edge->flow = .0;
        edge->nextEdge = nodes[from].firstEdge;
        nodes[from].firstEdge = edge;
    }
    std::vector<Edge *> get_path(int from, int to)
    {
        std::random_device rd;              // 随机数种子
        std::minstd_rand gen(rd());         // 随机数生成器
        std::bernoulli_distribution B(0.5); // B(1,0.5)
        std::vector<Edge *> path;
        for (int i = 0; i < n; i++)
        {
            vis[i] = false;
            dis[i] = INF;
            last_edge[i] = NULL;
        }
        dis[from] = 0;
        min_heap.clear();
        min_heap.insert(PII(0, from));
        while (!min_heap.empty())
        {
            Heap::iterator it = min_heap.begin();
            int from = it->second;
            min_heap.erase(it);
            if (vis[from])
            {
                continue;
            }
            vis[from] = true;
            for (Edge *edge = nodes[from].firstEdge; edge != NULL; edge = edge->nextEdge)
            {
                int to = edge->to;
                if (vis[to])
                    continue;
                if (dis[to] > dis[from] + 1) // 先按最小跳数来，等价于BFS
                {
                    dis[to] = dis[from] + 1;
                    min_heap.insert(PII(dis[to], to));
                    last_edge[to] = edge;
                }
                else if (dis[to] == dis[from] + 1) // 如果等dis，随机选路径
                {
                    if (B(gen)) // check if int
                    {
                        last_edge[to] = edge;
                    }
                }
            }
        }
        if (dis[to] == INF) // 不可达
        {
            return std::vector<Edge *>();
        }

        int now = to;
        while (last_edge[now] != NULL) // 没回溯到起点
        {
            path.push_back(last_edge[now]);
            now = last_edge[now]->from;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};