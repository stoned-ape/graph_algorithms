#include <iostream>
#include <fstream>
#include <vector>
#include <array>
// #include <utility>                   // for std::pair
// #include <algorithm>                 // for std::for_each
// #include <boost/graph/graph_traits.hpp>
// #include <boost/graph/adjacency_list.hpp>
// #include <boost/graph/dijkstra_shortest_paths.hpp>
// #include <boost/graph/betweenness_centrality.hpp>

// typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::bidirectionalS> boost_graph;

//prims algorithm
//kruskal algrothm
//union find
//cytoscape
//networkx
//snap graph database

using namespace std;


struct edge{
	int from_idx; 
	int to_idx;
	int weight;
};

template <typename T>
ostream &operator<<(ostream &os,const vector<T> &v){
	for(auto i:v) os<<i<<" ";
	return os<<'\n';
}
template <typename T,size_t n>
ostream &operator<<(ostream &os,const array<T,n> a){
	for(auto i:a) os<<i<<" ";
	return os<<'\n';
}

template <typename T,size_t n>
ostream &operator<<(ostream &os,const T a[n]){
	for(int i=0;i<n;i++) os<<a[i]<<" ";
	return os<<'\n';
}



void print(){
	cout<<"\n";
}

template<typename T,typename ...Ts>
void print(T t,Ts ...ts){
	cout<<t;
	print(ts...);
}

void shuffle(vector<edge> &a){
	int n=a.size();
	srand(time(NULL));
	for(int i=0;i<4*n;i++){
		int j=rand()%n,k=rand()%n;
		auto t=a[j];
		a[j]=a[k];
		a[k]=t;
	}
}




template <int node_num>
struct graph{
	vector<edge> edges[node_num];
	string node_labels[node_num]; //for displaying purposes
	void connect(int i,int j,int weight=1){
		assert(i<node_num && j<node_num);
		assert(i!=j);
		edges[i].push_back({i,j,weight});
		edges[j].push_back({j,i,weight});
	}
	int get_weight(int i,int j) const{
		assert(i<node_num && j<node_num);
		assert(i!=j);
		int w1=0,w2=0;
		for(auto e:edges[i]) if(j==e.to_idx){
			w1=e.weight;
			break;
		}
		for(auto e:edges[j]) if(i==e.to_idx){
			w2=e.weight;
			break;
		}
		assert(w1==w2);
		return w1;
	}
	void set_weight(int i,int j,int weight){
		assert(i<node_num && j<node_num && i!=j);
		auto f=[this,weight](int x,int y){
			bool _found=false;
			for(auto &e:this->edges[x]){
				if(y==e.to_idx){ 
					_found=true;
					e.weight=weight;
					assert(e.weight=weight);
					break;
				}
			}
			assert(_found);
		};
		f(i,j);
		f(j,i);
		// for(auto e:edges[i]) if(j==e.to_idx) e.weight=weight;
		// for(auto e:edges[j]) if(i==e.to_idx) e.weight=weight;
		assert(weight==get_weight(i,j));
	}
	graph(bool empty=false){
		static_assert(node_num>0);
		for(int i=0;i<node_num;i++) node_labels[i]=to_string(i);
		if(!empty){
			for(int i=0;i<node_num;i++){
				for(int j=0;j<i;j++){
					if(rand()%2) connect(i,j);
				}
			}
		}
	}
	void copy(const graph &g){
		for(int i=0;i<node_num;i++){
			edges[i]=g.edges[i];
		}
		for(int i=0;i<node_num;i++){
			for(int j=0;j<(int)edges[i].size();j++){
				assert(edges[i][j].to_idx==g.edges[i][j].to_idx);
			}
			
		}
	}
	graph(graph &g){
		copy(g);
	}
	graph &operator=(const graph &g){
		copy(g);
		return *this;	
	}
	friend ostream &operator<<(ostream &os,const graph &g){
		os<<"graph{\n";
		for(int i=0;i<node_num;i++){
			for(auto e:g.edges[i]){
				if(e.to_idx<i) os<<"\t\""<<g.node_labels[i]<<"\"--\""<<g.node_labels[e.to_idx]
					<<"\" [label="<<e.weight<<"]"<<'\n';
			}
		}
		os<<"}"<<endl;
		return os;
	}
	void make_image(string name) const{
		ofstream f(name+".dot");
		f<<*this;
		string cmd="dot -Tpng "+name+".dot > "+name+".png";
		cout<<cmd<<'\n';
		system(cmd.c_str());
	}
	int dfs(bool *visted,int current,graph &tree){
		if(visted[current]) return 0;
		visted[current]=true;
		int sum=1;
		vector<edge> cedges=edges[current];
		shuffle(cedges);
		for(auto i:cedges){
			if(visted[i.to_idx]) continue;
			int weight=dfs(visted,i.to_idx,tree);
			tree.connect(current,i.to_idx,weight);
			sum+=weight;
		}
		return sum;
	}
	graph spanning_tree(int start){
		cout<<start<<'\n';
		graph tree(true);
		bool visted[node_num];
		memset(visted,0,node_num);
		dfs(visted,start,tree);
		return tree;
	}
	graph centrality(){
		graph trees[node_num];
		for(int i=0;i<node_num;i++){
			trees[i]=spanning_tree(i);
			cout<<trees[i];
		}
		graph centgraph(true);
		for(int i=0;i<node_num;i++){
			for(auto e:edges[i]){
				int j=e.to_idx;
				if(j<=i) continue;
				int cent=0;
				for(int tree_idx=0;tree_idx<node_num;tree_idx++){
					int weight=trees[tree_idx].get_weight(j,i);
					assert(weight>=0);
					cent+=weight;
				}
				centgraph.connect(i,e.to_idx,cent);
			}
		}
		return centgraph;
	}
	//incomplete
	//based on the floyed-warshall algorithm
	//returns the length of the shortest path
	//returns -1 if there is no path
	//there could be multiple paths with this length
	//num: the number of shortest paths between i and j returned by reference
	//numl: the number of shortest paths between i and j that contain l returned by reference
	//k: do not pass a value 
	int shortest_path(int &num,int &numl,int i,int j,int l,int k=node_num-1){
		if(i==j){
			numl=i==l?1:0;
			num=1;
			return 0;
		}
		if(k==0){
			int w=get_weight(i,j);
			if(w!=0){
				numl=(l==i || l==j)?1:0;
				num=1;
				return 1;
			}else{ //no edge between i and j
				num=0;
				numl=0;
				return -1;
			}
		}
		int num1,num2,num3;
		int numl1,numl2,numl3;
		int len1=shortest_path(num1,numl1,i,j,l,k-1);
		int len2=shortest_path(num2,numl2,i,k,l,k-1);
		int len3=shortest_path(num3,numl3,k,j,l,k-1);
		int len23=len2+len3;
		int num23=num2+num3;
		int numl23=numl2+numl3;
		if(len2==-1 || len3==-1){
			len23=-1;
			num=0;
			numl=0;
		} 
		int state=0;
		state+=(int)(bool)(len1+1)<<0;
		state+=(int)(bool)(len23+1)<<1;
		switch(state){
			case 0: //no path
				num=0;
				num1=0;
				return -1;
			case 1: //no path including k
				num=num1;
				numl=numl1;
				return len1;
			case 2: //no path not including k
				num=num23;
				numl=numl23;
				return len23;
			case 3: //there are paths with and without k
				if(len1<len23){
					num=num1;
					numl=numl1;
					return len1;
				}
				if(len1>len23){
					num=num23;
					numl=numl23;
					return len23;
				}
				if(len1==len23){
					num=num1+num23;
					numl=numl1+numl23;
					return len1;
				}
			default: assert(1==0);
		}
		assert(1==0);
		return -1;
	}
	//incomplete
	//the betweenness centrality of a node k is:
	// the sum for all (i,j), of the number of shortest paths between i and j that contain k
	// divided by the total number of shortest paths between i and j
	array<double,node_num> node_centrality(){
		array<double,node_num> cent;
		for(auto i:cent) i=0;
		for(int i=0;i<node_num;i++){
			for(int j=0;j<i;j++){
				for(int k=0;k<node_num;k++){
					if(k==i || k==j) continue;
					int num;
					int numk;
					if(!shortest_path(num,numk,i,j,k)) continue;
					cent[k]+=(double)numk/num;
				}
			}
		}
		for(int i=0;i<node_num;i++){
			char *ret;
			asprintf(&ret,"%.2f",cent[i]);
			node_labels[i]+=": "+string(ret);
		}
		return cent;
	}
	bool is_cyclic_rec(int *vstack,int sp,int node){
		for(int i=0;i<sp;i++){
			if(vstack[i]==node) return true;
		}
		vstack[sp]=node;
		for(auto e:edges[node]){
			if(sp>0 && vstack[sp-1]==e.to_idx) continue;
			if(is_cyclic_rec(vstack,sp+1,e.to_idx)) return true;
		}
		return false;
	}
	bool is_cyclic(){
		int vstack[node_num];
		return is_cyclic_rec(vstack,0,0);
	}

	bool is_path_rec(bool *visted,int i,int j){
		if(visted[i]) return false;
		visted[i]=true;
		if(i==j) return true;
		for(auto e:edges[i]){
			if(is_path_rec(visted,e.to_idx,j)) return true;
		}
		return false;
	}
	bool is_path(int i,int j){
		bool visted[node_num];
		memset(visted,0,node_num);
		return is_path_rec(visted,i,j);
	}
	static int find(int *nodes,int i){
		if(nodes[i]==i) return i;
		return nodes[i]=find(nodes,nodes[i]);
	}
	static void _union(int *nodes,int i,int j){
		int x=find(nodes,i);
		int y=find(nodes,j);
		nodes[x]=nodes[y]=min(x,y);
	}
	auto get_edge_list(){
		vector<edge> el;
		for(int i=0;i<node_num;i++){
			for(auto e:edges[i]){
				if(e.to_idx>i) el.push_back(e);
			}
		}
		return el;
	}
	graph kruskal(){
		graph forrest(true);
		int set[node_num];
		for(int i=0;i<node_num;i++) set[i]=i;
		auto edgelist=get_edge_list();
		shuffle(edgelist);
		for(auto [i,j,w]:edgelist){
			if(find(set,i)!=find(set,j)){
				_union(set,i,j);
				forrest.connect(i,j);
			}
		}
		return forrest;
	}
	//only works for trees
	int node_count(int a,int b){
		int c=1;
		for(auto &e:edges[a]){
			if(e.to_idx!=b) c+=node_count(e.to_idx,a);
		}
		return c;
	}
	void weighted_tree(){
		for(int i=0;i<node_num;i++){
			for(auto &e:edges[i]){
				int j=e.to_idx;
				assert(get_weight(i,j));
				int r=node_count(i,j);
				int l=node_count(j,i);
				int weight=10*l+r;//min(l,r);
				set_weight(i,j,weight);
			
			}
		}
	}
	
};











int main(){
	srand(time(NULL));
	graph<6> g;

	
	// boost_graph gg(6);
	// for (int i=0;i<6;i++){
	// 	for(auto e:g.edges[i]){
	// 		add_edge(i,e.to_idx,gg);
	// 	}
	// }
	// boost::CentralityMap cmap;
	// boost::brandes_betweenness_centrality(gg,1);
	// boost::brandes_betweenness_centrality(1,1);

	
	cout<<g;
	g.make_image("graph");
	// auto tree=g.spanning_tree(1);
	// for(int i=0;i<5;i++){
	// 	for(auto e:tree.edges[i]) assert(e.weight==tree.get_weight(i,e.to_idx));
	// }
	// cout<<tree;
	// tree.make_image("tree");
	// assert(!tree.is_cyclic());
	// auto c=g.centrality();
	// cout<<c;
	// c.make_image("centrality");

	// auto cent=g.node_centrality();
	// cout<<cent;
	// g.make_image("node_centrality");

	auto ktree=g.kruskal();
	ktree.weighted_tree();
	cout<<ktree;
	ktree.make_image("kruskal_tree");
	assert(!ktree.is_cyclic());
}





