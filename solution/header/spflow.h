#pragma once

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "graph.h"
#include "matrix.h"
using namespace std;

struct spflow {
  unordered_set<vtx_t> tS, tT;
  vtx_t curN;
  graph G;
  unordered_map<vtx_t, pair<tick_t, vtx_t>> last;
  vector<pair<tick_t, vtx_t>> sS, sT;
  unordered_set<vtx_t> V;
  spflow(unordered_set<vtx_t> tS, unordered_set<vtx_t> tT)
      : tS(tS), tT(tT), curN(0) {}
  matrix result;
  void add_edge(vtx_t u, vtx_t v, tick_t t, flow_t w) {
    if (tS.count(v)) return;
    if (tT.count(u)) return;
    auto nnode = [&](int u) -> int {
      if (!last.count(u)) return ++curN;
      if (last[u].first == t) return last[u].second;
      int uu = ++curN;
      pair<tick_t, vtx_t> tu{t, uu};
      V.insert(uu);
      if (tS.count(u))
        sS.push_back(tu);
      else if (tT.count(u))
        sT.push_back(tu);
      else
        G.adde(last[u].second, uu, inf_flow);
      return uu;
    };
    vtx_t uu = nnode(u), vv = nnode(v);
    G.adde(uu, vv, w);
    last[u] = {uu, t}, last[v] = {vv, t};
  }
  vector<pair<tick_t, vtx_t>> subarr(vector<pair<tick_t, vtx_t>> a, tick_t ts,
                                     tick_t te) {
    auto itl = lower_bound(a.begin(), a.end(), make_pair(ts, 0));
    auto itr = lower_bound(a.begin(), a.end(), make_pair(te, 0));
    return {itl, itr};
  }
  map<pair<tick_t, tick_t>, flow_t> query(tick_t ts, tick_t te) {
    cout << ts << ' ' << te << endl;
    vector<pair<tick_t, vtx_t>> S = subarr(sS, ts, te), T = subarr(sT, ts, te);
    for (auto s : S) cout << s.second << ' ';
    cout << " ; ";
    for (auto t : T) cout << t.second << ' ';
    cout << endl;
    int n = S.size(), m = T.size();
    reverse(S.begin(), S.end());
    V;
    for (auto [ts, s] : S)
      for (auto [tt, t] : T)
        if (!result.get(ts, tt)) result.add(ts, tt, G.maxflow({s}, {t}, V));
    return result.query(ts, te);
  }
};