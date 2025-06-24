#include <queue>
#include <climits>
#include "bfs.hpp"
#include "../model/PentagonType.hpp"
#include "../model/Pentagono.hpp"
#include "../algorithm/bfs.hpp"

vector<int> findShortestPath(
  int start, int end,
  const vector<vector<int>>& adj,
  const vector<Pentagono>& pentagonos,
  const map<Pentagono,bool>& switches
) {
    int n = adj.size();
    vector<int> dist(n, INT_MAX), parent(n, -1);
    queue<int> q;

    if(start - 1 < 0){
        start = 0;
    }
    else{
        start = start - 1;
    }

    if(end - 1 < 0){
        end = 0;
    }
    else{
        end = end - 1;
    }
    
    auto esBloqueado = [&](int u) {
        auto &p = pentagonos[u];
        bool wall = p.type == returnPentagonTypeAsInt(PentagonType::WALL);
        bool eWall = p.type == returnPentagonTypeAsInt(PentagonType::ELECTRIC_WALL)
                     && !all_of(switches.begin(), switches.end(),
                         [](auto &pr){ return pr.second; });
        return wall || eWall;
    };
    
    dist[start] = 0;
    q.push(start);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == end) break;
        for (int v : adj[u]) {
            if (dist[v] == INT_MAX && !esBloqueado(v)) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    if (dist[end] == INT_MAX) return {}; // Sin ruta

    vector<int> path;
    for (int cur = end; cur != -1; cur = parent[cur])
        path.push_back(cur+1);
    reverse(path.begin(), path.end());

    // En path.push_back(cur+1); estamos sumando uno
    // Realizamos esto para mostrar el camino segun los numeros mostrados
    // Acordemonos que realmente el primer nodo es 0
    
    return path;
}