#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct edge{
	int to_idx;
	int weight;
};

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
	void connect(int i,int j,int weight=1){
		assert(i<node_num && j<node_num);
		assert(i!=j);
		edges[i].push_back({j,weight});
		edges[j].push_back({i,weight});
	}
	int get_weight(int i,int j) const{
		assert(i<node_num && j<node_num && i!=j);
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
		puts("weight not found");
		return w1;
	}
	void set_weight(int i,int j,int weight){
		assert(i<node_num && j<node_num && i!=j);
		for(auto e:edges[i]) if(j==e.to_idx) e.weight=weight;
		for(auto e:edges[j]) if(i==e.to_idx) e.weight=weight;
		assert(weight==get_weight(i,j));
	}
	
	graph(bool empty=false){
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
				if(e.to_idx<i) os<<'\t'<<i<<"--"<<e.to_idx<<" [label="<<e.weight<<"]"<<'\n';
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
					print("\t",i," ",j," ",tree_idx," ",weight);
					assert(weight>=0);
					cent+=weight;
				}
				// assert(cent);
				centgraph.connect(i,e.to_idx,cent);
				print(i," ",j," ",cent);

			}
		}
		return centgraph;
	}
	
};





template <typename T>
ostream &operator<<(ostream &os,const vector<T> &v){
	for(auto i:v) os<<i;
	return os<<'\n';
}





int main(){
	srand(time(NULL));
	cout<<"bruh\n";
	graph<6> g;
	
	cout<<g;
	g.make_image("graph");
	auto tree=g.spanning_tree(1);
	for(int i=0;i<5;i++){
		for(auto e:tree.edges[i]) assert(e.weight==tree.get_weight(i,e.to_idx));
	}
	cout<<tree;
	tree.make_image("tree");
	auto c=g.centrality();
	cout<<c;
	c.make_image("centrality");
}





