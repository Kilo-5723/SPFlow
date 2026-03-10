#pragma once

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "config.h"
using namespace std;

struct graph {
  static int cmaxf;
  static const int inf_dep = 1e9;
  unordered_map<vtx_t, unordered_map<vtx_t, flow_t>> E;
  void delv(vtx_t u) {
    for (auto [v, w] : E[u]) E[v].erase(u);
    E.erase(u);
  }
  void adde(vtx_t u, vtx_t v, flow_t w) {
    E[u][v] += w, E[v][u] += 0;  // add reverse edge
  }
  // maximum flow from S to T on the subgraph V
  flow_t maxflow(unordered_set<vtx_t> S, unordered_set<vtx_t> T,
                 unordered_set<vtx_t> V) {
    V = reachable({S.begin(), S.end()}, {T.begin(), T.end()}, V);
#ifdef DISTRIBUTION
    auto begin = clock();
#endif
    cmaxf++;
    unordered_map<vtx_t, vtx_t> depth;
    auto bfs = [&]() {
      depth.clear();
      queue<vtx_t> Q;
      for (auto s : S) depth[s] = 0, Q.push(s);
      while (Q.size()) {
        auto u = Q.front();
        Q.pop();
        if (T.count(u)) break;
        for (auto [v, w] : E[u])
          if (w && V.count(v) && !depth.count(v))
            Q.push(v), depth[v] = depth[u] + 1;
      }
      auto mxm = inf_dep;
      for (auto t : T)
        if (depth.count(t)) mxm = min(mxm, depth[t]);
      for (auto t : T)
        if (depth.count(t) && depth[t] > mxm) depth.erase(t);
      return mxm < inf_dep;
    };
    auto dfs = [&](auto&& self, vtx_t u, flow_t f) -> flow_t {
      // cout << u << ' ' << f << endl;
      if (T.count(u)) return f;
      if (!depth.count(u)) return 0;
      auto du = depth[u];
      // cout << u << ' ' << f << ' ' << du << ' ' << depth[f] << endl;
      auto left = f;
      for (auto& [v, w] : E[u])
        if (auto it = depth.find(v);
            w && it != depth.end() && it->second == du + 1) {
          flow_t delta = self(self, v, min(left, w));
          left -= delta, w -= delta, E[v][u] += delta;
          if (!left) break;
        }
      if (left) depth.erase(u);
      return f - left;
    };
    flow_t result = 0;
    // cout << "BFS Start" << endl;
    while (bfs()) {
      // cout << "DFS Start" << endl;
      for (auto s : S) {
        // cout << "Calculating " << s << " | ";
        // for (auto v : T) cout << ' ' << v;
        // cout << endl;
        result += dfs(dfs, s, inf_flow);
      }
      // cout << "current flow : " << result << endl;
    }
#ifdef DISTRIBUTION
    static char s[200];
    if (V.size()) {
      sprintf(s, "%d %.12f\n", V.size(),
              (double)(clock() - begin) / CLOCKS_PER_SEC);
      lout << s;
    }
#endif
    return result;
  }
  // flow-out of sources
  // It assumes that no edges points to s in the original network
  flow_t flowout(const unordered_set<vtx_t>& S) {
    flow_t res = 0;
    for (auto s : S)
      for (auto [v, w] : E[s]) res += E[v][s];
    return res;
  }
  // flow-in of sinks
  // It assumes that no edges points from t in the original network
  flow_t flowin(const unordered_set<vtx_t>& T) {
    flow_t res = 0;
    for (auto t : T)
      for (auto [v, w] : E[t]) res += w;
    return res;
  }
  unordered_set<vtx_t> reachable(vector<vtx_t> S,
                                 const unordered_set<vtx_t>& V) {
    unordered_set<vtx_t> forward, reverse;
    auto dfs_fwd = [&](auto&& self, vtx_t u) {
      if (forward.count(u)) return;
      forward.insert(u);
      for (auto& [v, w] : E[u])
        if (w && V.count(v)) self(self, v);
    };
    auto dfs_rev = [&](auto&& self, vtx_t u) {
      if (reverse.count(u)) return;
      reverse.insert(u);
      for (auto& [v, w] : E[u])
        if (E[v][u] && V.count(v)) self(self, v);
    };
    for (auto s : S) dfs_fwd(dfs_fwd, s), dfs_rev(dfs_rev, s);
    unordered_set<vtx_t> result;
    for (auto v : forward)
      if (reverse.count(v)) result.insert(v);
    return result;
  }
  unordered_set<vtx_t> reachable(vector<vtx_t> S, vector<vtx_t> T,
                                 const unordered_set<vtx_t>& V) {
    unordered_set<vtx_t> forward, reverse;
    auto dfs_fwd = [&](auto&& self, vtx_t u) {
      if (forward.count(u)) return;
      forward.insert(u);
      for (auto& [v, w] : E[u])
        if (w && V.count(v)) self(self, v);
    };
    auto dfs_rev = [&](auto&& self, vtx_t u) {
      if (reverse.count(u)) return;
      reverse.insert(u);
      for (auto& [v, w] : E[u])
        if (E[v][u] && V.count(v)) self(self, v);
    };
    for (auto s : S) dfs_fwd(dfs_fwd, s);
    for (auto t : T) dfs_rev(dfs_rev, t);
    unordered_set<vtx_t> result;
    for (auto v : forward)
      if (reverse.count(v)) result.insert(v);
    return result;
  }
  void peel(vtx_t s, unordered_set<vtx_t> T) {
    map<vtx_t, flow_t> excess;
    excess[s] = flowout({s});
    while (excess.size()) {
      auto [u, f] = *excess.begin();
      excess.erase(excess.begin());
      for (auto& [v, w] : E[u]) {
        if (!f) break;
        if (v < u) continue;
        auto& rev = E[v][u];
        auto det = min(f, rev);
        f -= det, rev -= det, w += det;
        if (!T.count(v)) excess[v] += det;
      }
    }
  }
};
int graph::cmaxf = 0;