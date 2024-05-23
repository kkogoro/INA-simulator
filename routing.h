#pragma once
#include <iostream>
#include <cstdio>
#include <vector>
#include "topo.h"

struct JobFlow
{
    std::vector<Edge *> path;
    int job_id;
    int from, to;
    bool blocked;
    double speed;       // 总速率
    double delta_speed; // 单次增量
    double data_size;
    JobFlow()
    {
        from = to = 0;
        speed = .0;
        blocked = false;
        data_size = .0;
        path.clear();
    }
    JobFlow(int job_id, int _from, int _to, double _data_size)
        : job_id(job_id), from(_from), to(_to), data_size(_data_size)
    {
        speed = .0;
        blocked = false;
        path.clear();
    }
};

class Routing
{
public:
    void addJob(int job_id, int from, int to, double data_size)
    {
        JobFlows.push_back(new JobFlow(job_id, from, to, data_size));
    }
    void get_path(Topo &topo)
    {
        for (auto &JobFlow : JobFlows)
        {
            {
                std::vector<Edge *> tmp;
                JobFlow->path.swap(tmp); // 释放内存
            }
            JobFlow->path = topo.get_path(JobFlow->from, JobFlow->to);
        }

        for (auto &JobFlow : JobFlows)
        {
            std::cout << "job" << JobFlow->job_id << " path: ";
            for (auto &edge : JobFlow->path)
            {
                std::cout << edge->from << "->" << edge->to << " ";
            }
            std::cout << std::endl;
        }
    }
    void max_min_fair(Topo *topo) // water filling
    {
        for (int u = 0; u < topo->n; ++u)
        {
            for (auto edge = topo->nodes[u].firstEdge; edge; edge = edge->nextEdge)
            {
                edge->flow = .0;
                edge->total_flow = 0;
            }
        }

        for (auto &Job_flow : JobFlows)
        {
            Job_flow->blocked = false;
            Job_flow->speed = .0;
            for (auto &edge : Job_flow->path)
            {
                edge->total_flow += 1;
            }
        }

        bool all_blocked = false;
        while (!all_blocked)
        {
            all_blocked = true;
            // 计算每个流的speed增量（路上瓶颈）
            for (auto &Job_flow : JobFlows)
            {
                if (Job_flow->blocked)
                    continue;

                double bottle_flow_delta = 1e9; // speed增量

                for (auto &edge : Job_flow->path)
                {
                    bottle_flow_delta = std::min(bottle_flow_delta,
                                                 (edge->capacity - edge->flow) / (double)edge->total_flow);
                }

                Job_flow->delta_speed = bottle_flow_delta;
            }

            // 更新每个flow的speed 并 计算链路剩余带宽
            for (auto &Job_flow : JobFlows)
            {
                if (Job_flow->blocked)
                    continue;

                Job_flow->speed += Job_flow->delta_speed;
                for (auto &edge : Job_flow->path)
                {
                    edge->flow += Job_flow->delta_speed;
                }
            }

            // 更新block情况
            for (auto &Job_flow : JobFlows)
            {
                if (Job_flow->blocked)
                    continue;

                for (auto &edge : Job_flow->path)
                {
                    if (edge->blocked())
                    {
                        Job_flow->blocked = true;
                        break;
                    }
                }

                // 存在一个flow没有被block，调通之后改用每轮一个vector存没block的流
                if (Job_flow->blocked == false)
                {
                    all_blocked = false;
                }
                else // 当前流被阻塞，把路径上所有edge的flow计数器-1
                {
                    for (auto &edge : Job_flow->path)
                    {
                        edge->total_flow -= 1;
                    }
                }
            }
        }
    }
    void step(Topo *topo, double t, double t_eps)
    {
        max_min_fair(topo);

        std::vector<JobFlow *> new_Jobs;

        for (auto &JobFlow : JobFlows)
        {
            std::cout << "time" << t << ": job " << JobFlow->job_id << "speed is" << JobFlow->speed << std::endl;
            JobFlow->data_size -= JobFlow->speed * t_eps;
            if (JobFlow->data_size > 0)
            {
                new_Jobs.push_back(JobFlow);
            }
            else
            {
                std::cout << "time" << t << ": job " << JobFlow->job_id << "is completed" << std::endl;
                delete JobFlow;
            }
        }

        JobFlows.swap(new_Jobs);
    }
    void init(Topo *topo)
    {
        get_path(*topo); // 路由
    }

private:
    std::vector<JobFlow *> JobFlows;
};