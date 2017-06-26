#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cstring>
#include <climits>

using namespace std;
int N,E;
int min_cost;
vector<bool>visited;
typedef struct Edge{
    int src,dest;
    int cost;
    Edge(int src_, int dest_, int cost_):
        src(src_),dest(dest_),cost(cost_){};
}Edge;

bool compare(const Edge& edge1,const Edge &edge2){
    return (edge1.cost <= edge2.cost);
}

class UnionFind{
    private:
        vector<int> parent, rank;
    public:
        UnionFind(int N){
            rank.assign(N,0);
            parent.assign(N,0);
            for(int i=0;i<N;++i)
                parent[i]=i;
        }
        int findSet(int i){ 
            return (parent[i]==i)? i: (parent[i]=findSet(parent[i])); 
        }

        bool isSameSet(int i, int j){
            return findSet(i) == findSet(j);
        }

        void unionSet(int i, int j){
            int x= findSet(i);
            int y= findSet(j);
            if(x!=y){
                if(rank[x]>rank[y])
                    parent[y]= x;
                else{
                    parent[x]=y;
                    if(rank[x]==rank[y])
                        ++rank[y];
                }
            }
        }
};

vector<Edge> kruskal_get_min_spanning(vector<Edge>& edgeList){
    sort(edgeList.begin(),edgeList.end(), compare);
    UnionFind uf(N+1);
    vector<Edge> mst;
    int edge_count=0;
    for(auto &edge:edgeList){
        if(!uf.isSameSet(edge.src,edge.dest)){
            ++edge_count;
            uf.unionSet(edge.src,edge.dest);
            mst.push_back(edge);
            if(edge_count==N-1)
                break;
        }
    }
    return mst;
}

void dfs(vector<vector<pair<int,int> > > &ad_list,int src,int max_cost){
    if(src==N){
        if(min_cost>max_cost)
            min_cost=max_cost;
        return;
    }
    visited[src]=true;
    for(auto &nbrs:ad_list[src]){
        if(!visited[nbrs.first]){
            dfs(ad_list,nbrs.first,max_cost>nbrs.second?max_cost:nbrs.second);
        }
    }
}

int main() {
    
    scanf("%d%d",&N,&E);
    vector<Edge> edgeList;
    int n1,n2,cost;
    for(int i=0;i<E;++i){
        scanf("%d%d%d",&n1,&n2,&cost);
        edgeList.push_back(Edge(n1,n2,cost));
    }
    vector<Edge> mst= kruskal_get_min_spanning(edgeList);
    vector<vector<pair<int,int> > > ad_list(N+1);
    for(auto &edge: mst){
        ad_list[edge.src].push_back(make_pair(edge.dest,edge.cost));
        ad_list[edge.dest].push_back(make_pair(edge.src,edge.cost));
    }

    min_cost=INT_MAX;
    int start_edge=1;
    visited.assign(N+1,false);
    dfs(ad_list,start_edge,INT_MIN);
    if(min_cost==INT_MAX)
        cout <<"NO PATH EXISTS"<<endl;
    else
        cout << min_cost<<endl;
    return 0;
}
