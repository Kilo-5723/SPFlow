#include <bits/stdc++.h>

#include "header/config.h"
#include "header/spflow.h"
using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 5) {
    puts("Usage: ./solution.out graph test mode output");
    return 0;
  }
  string graph = argv[1], test = argv[2], output = argv[4];
  int mode = atoi(argv[3]);
  vector<tuple<vtx_t, vtx_t, tick_t, flow_t>> edges;
  ifstream gin(graph);
  while (true) {
    vtx_t u, v;
    tick_t t;
    flow_t w;
    if (!(gin >> u >> v >> t >> w)) break;
    edges.push_back({u, v, t, w});
  }
  sort(edges.begin(), edges.end(),
       [&](auto a, auto b) { return get<2>(a) < get<2>(b); });
  for (int i = 0; i < edges.size(); i++) get<2>(edges[i]) = i;
  cout << "Edge count: " << edges.size() << endl;
  unordered_set<vtx_t> tS, tT;
  ifstream tin(argv[2]);
  int k;
  vtx_t v;
  tin >> k;
  while (k--) tin >> v, tS.insert(v);
  tin >> k;
  while (k--) tin >> v, tT.insert(v);
  for (auto s : tS) cout << s << ' ';
  cout << endl;
  for (auto t : tT) cout << t << ' ';
  cout << endl;

  spflow G(tS, tT);
  ofstream lout(output);
  lout << setprecision(6);

  pair<double, tick_t> ans;
  double sum = 0;
  int cntE = 0, cntT = 0;
  for (auto [u, v, t, w] : edges) G.add_edge(u, v, t, w);
  cout << "Prepare done" << endl;
  auto ts = get<2>(edges[0]), te = get<2>(edges.back());
  tick_t wid = (te - ts) / 12;
  auto begin = clock();
  double maxb = 0;
  if (mode == 1) {
    auto mp = G.query(ts, te);
    for (auto [pr, f] : mp) {
      auto [ts, te] = pr;
      maxb = max(maxb, f / (te - ts + 1.0));
    }
  } else {
    for (auto [u, v, t, w] : edges)
      if (tT.count(v)) {
        auto mp = G.query(t - wid, t);
        for (auto [pr, f] : mp) {
          auto [ts, te] = pr;
          maxb = max(maxb, f / (te - ts + 1.0));
          cout << pr.first << ' ' << pr.second << ' ' << f << endl;
        }
      }
  }
  auto end = clock();

  ofstream fout(output);
  fout << "Time : " << (double)(end - begin) / CLOCKS_PER_SEC << endl;
  fout << "Burstiness : " << maxb << endl;
  return 0;
}