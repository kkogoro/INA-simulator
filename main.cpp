#include <iostream>
#include <cstdio>
#include "topo.h"
#include "routing.h"

int main()
{
    int n, m, job_tot;
    Routing *routing = new Routing();
    std::cout << "input number of nodes" << std::endl;
    std::cin >> n;
    Topo *graph = new Topo(n);
    std::cout << "input number of edges" << std::endl;
    std::cin >> m;

    std::cout << "input from , to, and capacity of each edge" << std::endl;
    while (m--)
    {
        int u, v;
        double c;
        std::cin >> u >> v >> c;
        graph->addEdge(u, v, c);
    }

    std::cout << "input number of jobs" << std::endl;
    std::cin >> job_tot;
    std::cout << "input worker node id, PS node id and data size" << std::endl;
    for (int i = 0; i < job_tot; ++i)
    {
        int u, v;
        double data_size;
        std::cin >> u >> v >> data_size;
        routing->addJob(i, u, v, data_size);
    }

    routing->init(graph);
    double max_T = 100000000;
    double t_eps = 1.0;
    for (double t = 0; t < max_T; t += t_eps)
    {
        routing->step(graph, t, t_eps);
    }
    return 0;
}