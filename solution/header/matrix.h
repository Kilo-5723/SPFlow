#pragma once

#include <map>
#include <optional>
#include <set>
#include <vector>

#include "config.h"
using namespace std;
const double eps = 1e-9;
struct matrix {
  set<tick_t> S, T;
  map<pair<tick_t, tick_t>, flow_t> mtx;
  optional<flow_t> get(int s, int t) {
    if (mtx.count({s, t})) return mtx[{s, t}];
    return nullopt;
  }
  void add(int s, int t, flow_t f) {
    S.insert(s);
    T.insert(t);
    mtx[{s, t}] = f;
  }
  map<pair<tick_t, tick_t>, flow_t> query(tick_t s, tick_t t) {
    vector<vtx_t> ss, tt;
    for (auto i = S.lower_bound(s); i != S.end() && *i <= t; i++)
      ss.push_back(*i);
    for (auto j = T.lower_bound(t); j != T.end() && *j <= t; j++)
      tt.push_back(*j);
    int n = ss.size(), m = tt.size();
    cout<<n<<' '<<m<<endl;
    vector a(n, vector<flow_t>(m));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        if (mtx.count({ss[i], tt[j]})) a[i][j] = mtx[{ss[i], tt[j]}];
    for (int i = 1; i < n; i++)
      for (int j = 0; j < m; j++) a[i][j] += a[i - 1][j];
    for (int i = 0; i < n; i++)
      for (int j = 1; j < m; j++) a[i][j] += a[i][j - 1];
    map<pair<tick_t, tick_t>, flow_t> res;
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++) res[{ss[i], tt[j]}] = a[i][j];
    return res;
  }
};