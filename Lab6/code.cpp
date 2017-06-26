#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <stack>
#include <queue>
using namespace std;

class State{
	private:	
		bool final;
		map<char,vector<State*> > transiation_table;
		int name;
	public:
		static int name_counter;
		State(){
			final=false;
			name= State::name_counter;
			++(State::name_counter);
		}
		inline map<char,vector<State*> >& getTransitionTable(){return transiation_table;}
		inline bool& getFinal(){return final;}
		inline int& getName(){return name;}
};

class NFA{
	private:
		State* start_state;
		State* end_state;
	public:
		NFA();
		NFA(State* s_state, State* e_state);
		inline State* getStartState(){return start_state;}
		inline State* getEndState(){return end_state;}
		inline void setStartState(State* state){start_state= state;}
		inline void setEndState(State* state){end_state= state;}
		void toDFA();
		void getStatesOnTransition(State*,char symbol,set<State*>&, set<State*>&);
		set<char> getTransitionSymbolsOnState(set<State*>);
		void bfs();
		void util(queue<State*>, set<State*>&);
};

// assuming that the special characters are:
// *,+,|,(,) and ?
class RegEx{
	private:
		string regex;
		map<char,int> priority_map;
		NFA nfa;
	public:
		RegEx(string str_):regex(str_){
			priority_map['(']=priority_map[')']=1,
			priority_map['*']= priority_map['+']=
			priority_map['?']=2,
			priority_map['.']=3,
			priority_map['|']=4;
		};
		inline NFA &getNFA(){return nfa;}
		void compile(string str);
		inline void traverseStates(char,auto,auto,State*);
		inline string getRegex(){return regex;}
		string toPostfix(string str);
		void handleConcatenation(stack<NFA>&);
		void handleAlternation(stack<NFA>&);
		void addEpsilonTransition(State*, State*);
		void handleOption(stack<NFA>&);
		void handleKleeneStar(stack<NFA>&);
		void handleKleenePlus(stack<NFA>&);
		string toStandardRegex(string);
		int getPrecedence(char ch);
		bool match(string);
};

void NFA::util(queue<State*> que, set<State*>& visited_states){
	if(que.empty())
		return;
	queue<State*> temp_que;
	while(!que.empty()){
		State* state= que.front();
		cout <<"curr state:" << state->getName();
		if(state->getFinal())
			cout <<" -final";
		cout <<"\n";
		for(auto &iter: state->getTransitionTable()){
			vector<State*> vect= iter.second;
			cout << iter.first <<": ";
			for(auto &iter: vect){
				if(visited_states.find(iter)== visited_states.end()){
					visited_states.insert(iter);
					temp_que.push(iter);
				}
				cout << iter->getName()<<" ";
			}
			cout <<endl;
		}
		// cout << endl <<endl;
		que.pop();
	}
	util(temp_que,visited_states);
}

void NFA::bfs(){
	State* state= getStartState();
	queue<State*> que; que.push(state);
	set<State*> visited_states{state};
	util(que,visited_states);
}

NFA::NFA(){
	start_state= new State();
	end_state= new State();
}

NFA::NFA(State* s_state, State* e_state){
	start_state= s_state;
	end_state= e_state;
}

inline int RegEx::getPrecedence(char ch){
	return (priority_map.find(ch)!=priority_map.end()?priority_map[ch]:
			5);
}

void RegEx::handleConcatenation(stack<NFA>& nfa_stack){
	NFA nfa1= nfa_stack.top(); nfa_stack.pop();
	NFA nfa2= nfa_stack.top(); nfa_stack.pop();
	map<char, vector<State*> > & map_= nfa2.getEndState()->getTransitionTable();
	if(map_.find('e')== map_.end())
		map_['e']= vector<State*>{};
	map_['e'].push_back(nfa1.getStartState());
	nfa= NFA(nfa2.getStartState(), nfa1.getEndState());
	nfa_stack.push(nfa);
}

void RegEx::addEpsilonTransition(State* state1, State* state2){
	map<char, vector<State*> > & map1= state1->getTransitionTable();
	if(map1.find('e')==map1.end())
		map1['e']= vector<State*>{state2};
	else map1['e'].push_back(state2);
}

void RegEx::handleAlternation(stack<NFA>& nfa_stack){
	NFA nfa1= nfa_stack.top(); nfa_stack.pop();
	NFA nfa2= nfa_stack.top(); nfa_stack.pop();
	NFA nfa;
	addEpsilonTransition(nfa.getStartState(), nfa1.getStartState());
	addEpsilonTransition(nfa.getStartState(), nfa2.getStartState());
	addEpsilonTransition(nfa1.getEndState(), nfa.getEndState());
	addEpsilonTransition(nfa2.getEndState(), nfa.getEndState());
	nfa_stack.push(nfa);
}

void RegEx::handleOption(stack<NFA>& nfa_stack){
	addEpsilonTransition(nfa_stack.top().getStartState(),
							nfa_stack.top().getEndState());
}

void RegEx::handleKleeneStar(stack<NFA>& nfa_stack){
	NFA nfa1= nfa_stack.top(),nfa; nfa_stack.pop();
	addEpsilonTransition(nfa1.getEndState(),nfa1.getStartState());
	addEpsilonTransition(nfa1.getEndState(),nfa.getEndState());
	addEpsilonTransition(nfa.getStartState(), nfa1.getStartState());
	addEpsilonTransition(nfa.getStartState(),nfa.getEndState());
	nfa_stack.push(nfa);
}

void RegEx::handleKleenePlus(stack<NFA>& nfa_stack){
	handleKleeneStar(nfa_stack);
	vector<State*> &vect= nfa_stack.top().getStartState()
								->getTransitionTable()['e'];
	vect.erase(find(vect.begin(),vect.end(),nfa_stack.top().getEndState()));
}

void RegEx::compile(string regex){
	//nfa=NFA();
	stack<NFA> nfa_stack;
	for(auto &c: regex){
		if(priority_map.find(c)!=priority_map.end()){
			switch(c){
				case '.': handleConcatenation(nfa_stack);
					break;
				case '|': handleAlternation(nfa_stack);
					break;
				case '?': handleOption(nfa_stack);
					break;
				case '*': handleKleeneStar(nfa_stack);
					break;
				case '+': handleKleenePlus(nfa_stack);
					break;
				default: break;
			}
		}else{
			NFA nfa_;
			nfa_.getStartState()->getTransitionTable()[c]= 
					vector<State*>{nfa_.getEndState()}; 
			nfa_stack.push(nfa_);
		}
	}
	nfa= nfa_stack.top(); nfa_stack.pop();
	nfa.getEndState()->getFinal()= true;
}

bool RegEx::match(string str){
	State* start_state= getNFA().getStartState();
	for(auto c:str){
		if(c!='e'){
			map<char,vector<State*> > map_= start_state->getTransitionTable();
			if(map_.find(c)==map_.end())
				return false;
			start_state= map_[c][0];
		}
	}
	if(start_state->getFinal())
		return true;
	return false;
}

string RegEx::toStandardRegex(string str){
	string stdRegex;
	int len= str.size();
	for(int i=0;i<len-1;++i){
		char curr_char= str[i];
		char next_char= str[i+1];

		stdRegex+=curr_char;

		if(curr_char!='(' && curr_char!='|' &&
			(getPrecedence(next_char)>4 || next_char=='('))
			stdRegex+='.';
	}
	stdRegex+=str[len-1];
	return stdRegex;
}

string RegEx::toPostfix(string str){
	str= toStandardRegex(str);
	string postfix_exp;
	stack<char> symbol_stack;
	for(auto &c:str){
		if(getPrecedence(c)>4)
			postfix_exp+=c;
		else if(c=='(')
			symbol_stack.push(c);
		else if(c==')'){
			while(!symbol_stack.empty() && symbol_stack.top()!='(')
				postfix_exp+=symbol_stack.top(), symbol_stack.pop();
			symbol_stack.pop();
		}else{
			while(!symbol_stack.empty() && getPrecedence(symbol_stack.top())<= 
					getPrecedence(c) && symbol_stack.top()!='(')
				postfix_exp+=symbol_stack.top(), symbol_stack.pop();
			symbol_stack.push(c);
		}
	}
	while(!symbol_stack.empty())
		postfix_exp+=symbol_stack.top(),symbol_stack.pop();
	cout << postfix_exp <<endl;
	return postfix_exp;
}

void NFA::getStatesOnTransition(State* state, char symbol, set<State*>& states_set,
								set<State*>& visited_states){
	map<char,vector<State*> >map_= state->getTransitionTable();
	visited_states.insert(state);
	if(map_.find(symbol)!=map_.end()){
		vector<State*> next_states= map_[symbol];
		for(auto &states: next_states)
			if(visited_states.find(states)==visited_states.end())
				getStatesOnTransition(states,symbol,states_set,visited_states);
	}
	states_set.insert(state);
}

set<char> NFA::getTransitionSymbolsOnState(set<State*> states){
	set<char> symbols;
	for(auto &state:states){
		for(auto &iter: state->getTransitionTable())
			if(iter.first!='e')
				symbols.insert(iter.first);
	}
	return symbols;
}

void printStates(set<State*> states){
	for(auto & iter: states)
		cout << iter->getName() <<" ";
	cout << endl;
}
void NFA::toDFA(){
	State::name_counter=0;
	map<set<State*>,State*> mapped_states;
	set<State*> states_set,visited_states;
	getStatesOnTransition(getStartState(),'e',states_set,visited_states);
	mapped_states[states_set]= new State();
	NFA nfa(mapped_states[states_set], new State());
	// printStates(states_set);
	queue<set<State*> > current_states;
	current_states.push(states_set);
	while(!current_states.empty()){		
		set<State*> curr_states= current_states.front();
		//printStates(curr_states);
		set<char> trans_symbols= getTransitionSymbolsOnState(curr_states);
		for(auto &symbol: trans_symbols){
			// cout << symbol <<"\n";
			states_set.clear(); visited_states.clear();
			for(auto &state: curr_states)
				if(state->getTransitionTable().find(symbol)!= state->getTransitionTable().end())
					for(auto &nxt_states:state->getTransitionTable()[symbol])
						getStatesOnTransition(nxt_states,symbol,states_set,visited_states);
		
			set<State*> epsilonClosure;
			for(auto &state: states_set)
				getStatesOnTransition(state,'e',epsilonClosure,visited_states);
			//printStates(epsilonClosure);
			if(mapped_states.find(epsilonClosure)==mapped_states.end()){
				mapped_states[epsilonClosure]= new State();
				current_states.push(epsilonClosure);
			}
			//printStates(epsilonClosure);
			mapped_states[curr_states]->getTransitionTable()[symbol]= 
						vector<State*>{mapped_states[epsilonClosure]};
		}
		current_states.pop(); 
	}
	for(auto &iter: mapped_states)
		if(iter.first.find(getEndState())!= iter.first.end())
			iter.second->getFinal()= true;
	*this= nfa;
}

int State::name_counter=0;
int main(){
	string pattern,str1;
	cin >> pattern;
	RegEx regex(pattern);
    regex.compile(regex.toPostfix(regex.getRegex()));
    regex.getNFA().toDFA();
    //regex.getNFA().bfs();
	while(cin >> str1){
		if(regex.match(str1))
			cout <<"matched\n";
		else cout <<"not matched\n";
	}
}
// 31602951022