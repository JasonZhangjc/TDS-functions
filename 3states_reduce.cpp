#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

/*
The short program is used to generate the flattened version 
of the discrete-event systems in state tree structures.
*/


class state
{
public:
	string         m_name;
	vector<string> m_members;

public:
	state(string name = "M_0", vector<string> members = {"M_0"}):
	m_name(name), m_members(members)
	{}

};


class transition
{
private:
	string m_source;
	int m_event;
	string m_target;

public:
	// Constructor.
	transition(string source = "M_0", int event = 1, string target = "M_1"):
	m_source(source), m_event(event), m_target(target)
	{}

	string getSource()
	{	return m_source;	}

	int getEvent()
	{	return m_event;		}

	string getTarget()
	{	return m_target;	}
};


class automaton
{
public:
	string             m_name;
	vector<state>      m_states;
	vector<int>        m_alphabet;
	vector<transition> m_transitions;

public:
	automaton(string name, vector<state> states, vector<int> alphabet, 
	          vector<transition> transitions):
	m_name(name), m_states(states), m_alphabet(alphabet), 
	m_transitions(transitions)
	{}

	void print()
	{
		cout << m_name << "******************" << endl;

		int countState = m_states.size();
		cout << "State space: ";
		for (int i = 0; i < countState; i++)
		{	cout << m_states.at(i).m_name << " ";	}
		cout << endl;

		int countAlphabet = m_alphabet.size();
		cout << "Alphabet: ";
		for (int i = 0; i < countAlphabet; i++)
		{	cout << m_alphabet.at(i) << " ";	}
		cout << endl;

		int countTransitions = m_transitions.size();
		cout << "Transitions: " << endl;
		for (int i = 0; i < countTransitions; i++)
		{	cout << m_transitions.at(i).getSource() << " - " 
		         << m_transitions.at(i).getEvent()
			     << " - " << m_transitions.at(i).getTarget() << endl;	}
			
		cout << endl;
	}
};


class level
{
public:
	string m_name;
	string m_relation;
	string m_component1;
	string m_component2;

public:
	level(string name, string relation, string component1, string component2):
	m_name(name), m_relation(relation), m_component1(component1), 
	m_component2(component2)
	{}
};


// Determine whether the events of A are in the alphabet of B or not.
bool notIn(int elem, vector<int> s)
{
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == elem)
            return false;
    }
    return true;
}

// Return the alphabet of the synchronous product.
vector<int> syncAlphabet(automaton &A, automaton &B)
{
    vector<int> alphabet = B.m_alphabet;
    for (int i = 0; i < A.m_alphabet.size(); i++)
    {
        if (notIn(A.m_alphabet.at(i),B.m_alphabet))
            alphabet.push_back(A.m_alphabet.at(i));
    }

    cout << "The new sync alphabet: " << endl;
    for (int j = 0; j < alphabet.size(); j++)
        cout << alphabet.at(j) << " ";
    cout << endl << endl;
    return alphabet;
}

// Catenate a string and a integer.
string cat(string a, int b)
{
    stringstream ss;
    ss << a << b;
    //cout << ss.str() << endl;
	return ss.str();
}

// Generate the state space of the synchronous product.
vector<state> syncStates(automaton &A, automaton &B, string AB)
{
	// Fisrt generate a vector to store new states
	// The number of elements in this vector is initialized as 
	// the product of the numbers of elements in the two automaton
	vector<state> product;
	int count = 0;
	for (int i = 0; i < A.m_states.size(); i++)
		for (int j = 0; j < B.m_states.size(); j++)
		{
			// e.g. if A1 & B1 -> AB1, then m_members should contain A1,B1,AB1.
			vector<string> members;
			for (int x = 0; x < A.m_states.at(i).m_members.size(); x++)
			{
				members.push_back(A.m_states.at(i).m_members.at(x));
			}
			for (int y = 0; y < B.m_states.at(j).m_members.size(); y++)
			{
				members.push_back(B.m_states.at(j).m_members.at(y));
			}
			members.push_back(cat(AB,count));
			
			state current(cat(AB,count), members);
			count++;
			product.push_back(current);
		}

    cout << "The new sync state space: " << endl;
	for (int k = 0; k < product.size(); k++)
	{
	    cout << product.at(k).m_name << " ";
	    for (int r = 0; r < product.at(k).m_members.size(); r++)
            cout << product.at(k).m_members.at(r) << " ";
        cout << endl;
    }
    cout << endl;
	return product;
}
// State trace is done.

// The transition relation is the most complex part in this short program :)
// The function returns all relations, which may contain blocking transitions.
vector<transition> syncTransitions(automaton &A, automaton &B, vector<state> s, 
                                   vector<transition> &t)
{
    vector<transition> t_a = A.m_transitions;
    vector<transition> t_b = B.m_transitions;
    vector<int>        a_a = A.m_alphabet;
    vector<int>        a_b = B.m_alphabet;

    vector<transition> newTransitions;

    for (int i = 0; i < s.size(); i++)
    {
        // string c = s.at(i).m_members.at(0);
        string c = s.at(i).m_members.at(s.at(i).m_members.size() - 3);
        // string d = s.at(i).m_members.at(1);
        string d = s.at(i).m_members.at(s.at(i).m_members.size() - 2);

        for (int j = 0; j < t_a.size(); j++)
        {
            if (t_a.at(j).getSource() == c)                    
			// If in A we find transitions exit from c.
            {
                int mark = 0;                                  
				// mark is to mark transition is generated or not.
                int current_event = t_a.at(j).getEvent();      
				// current_event is the event of current transition.
                
                // To determine whether the current event is shared event.
                vector<int>::iterator ita = find(a_a.begin(), a_a.end(), 
                                                 current_event);
                vector<int>::iterator itb = find(a_b.begin(), a_b.end(), 
                                                 current_event); 
				if (ita != a_a.end()) 
				{
					// cout << *ita << endl;
					// cout << "Current_event is found in A's alphabet" << endl;
					if (itb != a_b.end())
					{
						// cout << *itb << endl;
						// cout << "Shared event is found" << endl;
						
						for (int k = 0; k < t_b.size(); k++)           
						// Now go to B to find this transition.
				        {
				            if (t_b.at(k).getEvent() == current_event) 
							// If in B we find the event of the current transition.
				            {
				                if (t_b.at(k).getSource() == d)        
								// To check the source of the transition is d or not.
				                {
				                    // If the source is d, 
				                    // then add a transition of shared event.
									string target;
				                    string e = t_a.at(j).getTarget();
				                    string f = t_b.at(k).getTarget();
				                    for (int p = 0; p < s.size(); p++)
				                    {
										// if (s.at(p).m_members.at(0) == e)
										if (s.at(p).m_members.at(s.at(i).m_members.size() - 3) == e)
											// if (s.at(p).m_members.at(1) == f)
											if (s.at(p).m_members.at(s.at(i).m_members.size() - 2) == f)
												target = s.at(p).m_name;
				                    }
				                    transition sync_transition(s.at(i).m_name, current_event, target);
				                    newTransitions.push_back(sync_transition);
									mark = 1;
				                }
				            }
				        }
					}

					// If it is not a shared event, then it is a independent transition in A.
					else
					{
						// cout << "Shared event can not find@@@@@@@@@" << endl;
						string target;
				        string e = t_a.at(j).getTarget();          
						// The transition with source c has a target e.
						for (int p = 0; p < s.size(); p++)
						{
							// if (s.at(p).m_members.at(0) == e)
							if (s.at(p).m_members.at(s.at(i).m_members.size() - 3) == e)
								// if (s.at(p).m_members.at(1) == d)
								if (s.at(p).m_members.at(s.at(i).m_members.size() - 2) == d)
								{
									target = s.at(p).m_name;
									transition current_transition(s.at(i).m_name, current_event, target);
									newTransitions.push_back(current_transition);
								}		
						}
					}  	
				}  
				
				
            }
        }
        
		// After all shared events and events only in A have been found, go to find events only in B.
        for (int k = 0; k < t_b.size(); k++)
        {
        	int indep_event = t_b.at(k).getEvent();
        	vector<int>::iterator iterator_a = find(a_a.begin(), a_a.end(), indep_event);
            vector<int>::iterator iterator_b = find(a_b.begin(), a_b.end(), indep_event);
            string target;
			string f = t_b.at(k).getTarget();
			string g = t_b.at(k).getSource();
			
			
            if (iterator_b != a_b.end())                 
			// If the event is in the alphabet of B.
            {
            	if (iterator_a == a_a.end())             
				// And meanwhile the event is not in the alphabet of A.
            	{
            		for (int q = 0; q < s.size(); q++)
					{
						if (s.at(q).m_members.at(s.at(q).m_members.size() - 2) == f)
						{
							if (s.at(q).m_members.at(s.at(q).m_members.size() - 3) == c)
							{
								target = s.at(q).m_name;
								if (s.at(i).m_members.at(s.at(i).m_members.size() - 2) == g)    
								// If the current new state is synchronized by the source of the current transition 
							    // and another state.
								{
									transition indep_transition(s.at(i).m_name, indep_event, target);
									newTransitions.push_back(indep_transition);
								}
								
							}		
						}
							
					}
            	} 
            }
            
        }
    }

    // Fisrt check the source, then check the target, in order to deal with extra transitions in real time.
	// Fisrt check the source.
	for (int i = 0; i < t.size(); i++)
	{
		string cur_source = t.at(i).getSource();
		string cur_target = t.at(i).getTarget();
		for (int j = 0; j < s.size(); j++)
		{
		
			if (s.at(j).m_members.at(s.at(j).m_members.size() - 3) == cur_source)
			{
				int cur_event = t.at(i).getEvent();
				for (int k = 0; k < t.size(); k++)        
				{
					if (t.at(k).getEvent() == cur_event)
					{
						if (t.at(k).getTarget() == cur_target)
						{
							string ano_source = t.at(k).getSource();
							for (int p = 0; p < s.size(); p++)
							{
								if (s.at(p).m_members.at(s.at(p).m_members.size() - 2) == ano_source)
								{
									if (s.at(p).m_members.at(s.at(p).m_members.size() - 3) == cur_source)
									{
										t.push_back(transition(s.at(p).m_name, cur_event, cur_target));
										transition d1(cur_source, cur_event, cur_target);
										transition d2(ano_source, cur_event, cur_target);
										for(vector<transition>::iterator del = t.begin(); del != t.end();)
										{
											if ((*del).getSource() == d1.getSource() 
												&& (*del).getEvent() == d1.getEvent() && (*del).getTarget() == d1.getTarget())
												del = t.erase(del);    
												// Delete the element, return the next position of the deleted element.    
											else if ((*del).getSource() == d2.getSource() 
												&& (*del).getEvent() == d2.getEvent() && (*del).getTarget() == d2.getTarget())
												del = t.erase(del);
											else
												++del;    
											    // Point to the next position.
										}
									}
								}
							}
						}
						
					}
				}
			}
			else if (s.at(j).m_members.at(s.at(j).m_members.size() - 2) == cur_source)
			{
				int cur_event = t.at(i).getEvent();
				for (int k = 0; k < t.size(); k++)          
				// The loop should start from 0.
				{
					if (t.at(k).getEvent() == cur_event)
					{
						if (t.at(k).getTarget() == cur_target)
						{
							string ano_source = t.at(k).getSource();
							for (int p = 0; p < s.size(); p++)
							{
								if (s.at(p).m_members.at(s.at(p).m_members.size() - 3) == ano_source)
								{
									if (s.at(p).m_members.at(s.at(p).m_members.size() - 2) == cur_source)
									{
										t.push_back(transition(s.at(p).m_name, cur_event, cur_target));
										transition d1(cur_source, cur_event, cur_target);
										transition d2(ano_source, cur_event, cur_target);
										for(vector<transition>::iterator del = t.begin(); del != t.end();)
										{
											if ((*del).getSource() == d1.getSource() 
												&& (*del).getEvent() == d1.getEvent() && (*del).getTarget() == d1.getTarget())
												del = t.erase(del);        
											else if ((*del).getSource() == d2.getSource() 
												&& (*del).getEvent() == d2.getEvent() && (*del).getTarget() == d2.getTarget())
												del = t.erase(del);
											else
												++del;    
										}
									}
								}
							}
						}
						
					}
				}
			}
		}
	}
	
	
	// Then check the target.
	for (int i = 0; i < t.size(); i++)
	{
		string cur_source = t.at(i).getSource();
		string cur_target = t.at(i).getTarget();
		for (int j = 0; j < s.size(); j++)
		{
		
			if (s.at(j).m_members.at(s.at(j).m_members.size() - 3) == cur_target)
			{
				int cur_event = t.at(i).getEvent();
				for (int k = 0; k < t.size(); k++)
				{
					if (t.at(k).getEvent() == cur_event)
					{
						if (t.at(k).getSource() == cur_source)
						{
							string ano_target = t.at(k).getTarget();
							for (int p = 0; p < s.size(); p++)
							{
								if (s.at(p).m_members.at(s.at(p).m_members.size() - 2) == ano_target)
								{
									if (s.at(p).m_members.at(s.at(p).m_members.size() - 3) == cur_target)
									{
										t.push_back(transition(cur_source, cur_event, s.at(p).m_name));
										transition d1(cur_source, cur_event, cur_target);
										transition d2(cur_source, cur_event, ano_target);
										for(vector<transition>::iterator del = t.begin(); del != t.end();)
										{
											if ((*del).getSource() == d1.getSource() 
												&& (*del).getEvent() == d1.getEvent() && (*del).getTarget() == d1.getTarget())
												del = t.erase(del);    
											else if ((*del).getSource() == d2.getSource() 
												&& (*del).getEvent() == d2.getEvent() && (*del).getTarget() == d2.getTarget())
												del = t.erase(del);
											else
												++del;    
										}
									}
								}
							}
						}
						
					}
				}
			}
			else if (s.at(j).m_members.at(s.at(j).m_members.size() - 2) == cur_target)
			{
				int cur_event = t.at(i).getEvent();
				for (int k = 0; k < t.size(); k++)
				{
					if (t.at(k).getEvent() == cur_event)
					{
						if (t.at(k).getSource() == cur_source)
						{
							string ano_target = t.at(k).getTarget();
							for (int p = 0; p < s.size(); p++)
							{
								if (s.at(p).m_members.at(s.at(p).m_members.size() - 3) == ano_target)
								{
									if (s.at(p).m_members.at(s.at(p).m_members.size() - 2) == cur_target)
									{
										t.push_back(transition(cur_source, cur_event, s.at(p).m_name));
										transition d1(cur_source, cur_event, cur_target);
										transition d2(cur_source, cur_event, ano_target);
										for(vector<transition>::iterator del = t.begin(); del != t.end();)
										{
											if ((*del).getSource() == d1.getSource() 
												&& (*del).getEvent() == d1.getEvent() && (*del).getTarget() == d1.getTarget())
												del = t.erase(del);        
											else if ((*del).getSource() == d2.getSource() 
												&& (*del).getEvent() == d2.getEvent() && (*del).getTarget() == d2.getTarget())
												del = t.erase(del);
											else
												++del;    
										}
									}
								}
							}
						}
						
					}
				}
			}
		}
	}


	cout << "The new sync transitions: " << endl;
    for (int i = 0; i < newTransitions.size(); i++)
    {
        cout << newTransitions.at(i).getSource() << " - " << newTransitions.at(i).getEvent() 
        	 << " - " << newTransitions.at(i).getTarget() << endl;
    }
	cout << endl;
	
	cout << "The extra transitions: " << endl;
	for (int i = 0; i < t.size(); i++)
	{
		cout << t.at(i).getSource() << " - " << t.at(i).getEvent() << " - " << t.at(i).getTarget() << endl;
	}
	cout << endl;
	
	return newTransitions;
}

// This part is to find states that have no source.
bool noSource(state &s, vector<transition> &t)
{
	if (s.m_name[s.m_name.length()-1] != '0')                
	// First rule out the initial state.
	{
		for (int j = 0; j < t.size(); j++)
		{
			if (s.m_name == t.at(j).getTarget())
			{	
				// cout << "No headless states!" << endl;
				return false;	
			}
		}
		cout << "Found headless states!" << endl;
		return true;
	}
	return false;

}

// Function trim: Every time find the transition and with no source and the corresponding states, 
// delete the transition and states and find the next transition with no source.
// Rule out the initail state.
int syncTrim(vector<state> &states, vector<transition> &transitions)
{
	int mark = 0;
	// cout << "Enter syncTrim!" << endl;
	for (int i = 0; i < states.size(); i++)
	{
		bool no_source = noSource(states.at(i), transitions);
		if (no_source)
		{
			mark = 1;
			cout << "Mark is turned to 1 ******************************************" << endl;
			vector<state>::iterator it_s = states.begin() + i;         
			string name = states.at(i).m_name;
			states.erase(it_s);
			for (int j = 0; j < transitions.size(); j++)
			{
				if (transitions.at(j).getSource() == name)
				{
					vector<transition>::iterator it_t = transitions.begin() + j;
					transitions.erase(it_t);
				}
			}
		}
	}
	return mark;
}

// Until cannot find transitions and states with no source.
void syncNonblocking(vector<state> &states, vector<transition> &transitions)
{
/*
	do
	{
		int trim = syncTrim(states, transitions);
	}
	while(syncTrim(states, transitions) == 0);
*/
	int trim = syncTrim(states, transitions);
	while(trim != 0)
	{
		trim = syncTrim(states, transitions);
	}
	
	// int trim = syncTrim(states, transitions);
	
	// Output all nonblokcing states.
	cout << "The sync nonblocking states: " << endl;
    for (int i = 0; i < states.size(); i++)
    {
        cout << states.at(i).m_name << " - " << states.at(i).m_members.at(states.at(i).m_members.size() - 3) 
        	 << " & " << states.at(i).m_members.at(states.at(i).m_members.size() - 2) << " -> " 
        	 << states.at(i).m_members.at(states.at(i).m_members.size() - 1) << endl;
    }
    cout << endl;
    
    // Output all nonblocking transitions.
	cout << "The sync nonblocking transitions: " << endl;
    for (int i = 0; i < transitions.size(); i++)
    {
        cout << transitions.at(i).getSource() << " - " << transitions.at(i).getEvent() 
        	 << " - " << transitions.at(i).getTarget() << endl;
    }
    cout << endl;
}

// Generate the synchronous product.
automaton sync(automaton &A, automaton &B, string AB, vector<transition> &t)
{
	// How to generate the synchronous product:
	// Fisrt create a vector, the size should be A.size() * B.size().
	// Then generate the state space.
    // Then generate the alphabet.
	// Then generate transitions according to rules.
	// Delete blocking states and transitions.

	// Generate the state space.
    vector<state> new_States   = syncStates(A,B,AB);
    // Generate the alphabet.
    vector<int>   new_Alphabet = syncAlphabet(A,B);
    // Generate transitions according to rules.
    vector<transition> new_Transitions = syncTransitions(A,B,new_States,t);
    // Delete blocking states and transitions.
    syncNonblocking(new_States, new_Transitions);
    // Return an automaton
    return automaton(AB, new_States, new_Alphabet, new_Transitions);
   
}

// Deal with states when the synchronous product is generated via an OR relation.
vector<state> unionStates(automaton &A, automaton &B)
{
	// Generate the union of the states.
	vector<state> states;
	for (int i = 0; i < A.m_states.size(); i++)
	{
		states.push_back(A.m_states.at(i));
	}
	for (int i = 0; i < B.m_states.size(); i++)
	{
		states.push_back(B.m_states.at(i));
	}
	
	cout << "The new union states: " << endl;
	for (int i = 0; i < states.size(); i++)
	{
		cout << states.at(i).m_name << " -> ";
		for (int j = 0; j < states.at(i).m_members.size(); j++)
		{
			cout << states.at(i).m_members.at(j) << " ";
		}
		cout << endl;
	}
	cout << endl;
	
	return states;
}

// Generate the alphabet when the synchronous product is generated via an OR relation.
vector<int> unionAlphabet(automaton &A, automaton &B, vector<transition> &t)
{
	vector<int> alphabet;
	for (int i = 0; i < A.m_alphabet.size(); i++)
	{
		alphabet.push_back(A.m_alphabet.at(i));
	}
	
	for (int i = 0; i < B.m_alphabet.size(); i++)
	{
		vector<int>::iterator it = find(A.m_alphabet.begin(), A.m_alphabet.end(), B.m_alphabet.at(i));
		if (it == A.m_alphabet.end())
		{
			alphabet.push_back(B.m_alphabet.at(i));
		}
	}
	
	vector<int> temp_alphabet;
	for (int i = 0; i < t.size(); i++)
	{
		temp_alphabet.push_back(t.at(i).getEvent());
	}
	
	for (int i = 0; i < temp_alphabet.size(); i++)
	{
		vector<int>::iterator it = find(alphabet.begin(), alphabet.end(), temp_alphabet.at(i));
		if (it == alphabet.end())
		{
			alphabet.push_back(temp_alphabet.at(i));
		}
	}
	
	cout << "The new union alphabet: " << endl;
	for (int i = 0; i < alphabet.size(); i++)
	{
		cout << alphabet.at(i) << " ";
	}
	cout << endl << endl;
	return alphabet;
}

// Generate the transitions when the synchronous product is generated via an OR relation.
vector<transition> unionTransition(automaton &A, automaton &B, vector<transition> &t)
{
	vector<transition> trans;
	for (int i = 0; i < A.m_transitions.size(); i++)
	{
		trans.push_back(A.m_transitions.at(i));
	}
	for (int i = 0; i < B.m_transitions.size(); i++)
	{
		trans.push_back(B.m_transitions.at(i));
	}
	
	// The extra transitions here only contain those transitions from one state to another state.	
	// Include the transitions whose source and target are both in union components.	
	for (int i = 0; i < t.size(); i++)
	{
		for (int j = 0; j < A.m_states.size(); j++)
		{
			if (t.at(i).getSource() == A.m_states.at(j).m_name)
			{
				for (int k = 0; k < B.m_states.size(); k++)
				{
					if (t.at(i).getTarget() == B.m_states.at(k).m_name)
					{
						trans.push_back(t.at(i));
					}
				}
			}
		}
	}
	
	for (int i = 0; i < t.size(); i++)
	{
		for (int j = 0; j < B.m_states.size(); j++)
		{
			if (t.at(i).getSource() == B.m_states.at(j).m_name)
			{
				for (int k = 0; k < A.m_states.size(); k++)
				{
					if (t.at(i).getTarget() == A.m_states.at(k).m_name)
					{
						trans.push_back(t.at(i));
					}
				}
			}
		}
	}
	
/*	
	cout << "The new extra transitions: " << endl;
	for (int i = 0; i < t.size(); i++)
	{
		cout << t.at(i).getSource() << " " << t.at(i).getEvent() << " " << t.at(i).getTarget() << endl;
		
		trans.push_back(t.at(i));
	}
	cout << endl;
*/	
	
	cout << "The new union transitions: " << endl;
	for (int i = 0; i < trans.size(); i++)
	{
		cout << trans.at(i).getSource() << " " << trans.at(i).getEvent() << " " << trans.at(i).getTarget() << endl;
	}
	cout << endl;
	
	return trans;
}


// To deal with the holon union via OR relation.
automaton union_holons(automaton &A, automaton &B, string s, vector<transition> &t)   
// s is the name of automaton after combination.
{
	vector<state> states   = unionStates(A, B);
	vector<int>   alphabet = unionAlphabet(A, B, t);
	vector<transition> trans = unionTransition(A, B, t);
	return automaton(s, states, alphabet, trans);
}


// To generate the final automaton.
automaton DES_generate(vector<automaton> &holons, vector<level> &levels, vector<transition> &transHolon)
{
	for (int i = levels.size()-1; i >= 0; i--)
	{
		if (levels.at(i).m_relation == "AND")
		{
			for (int j = 0; j < holons.size(); j++)
			{
				if (holons.at(j).m_name == levels.at(i).m_component1)
				{
					for (int k = 0; k < holons.size(); k++)
					{
						if (holons.at(k).m_name == levels.at(i).m_component2)
						{
							automaton DES = sync(holons.at(j), holons.at(k), levels.at(i).m_name, transHolon);
							holons.push_back(DES);
							// DES.print();
						}
						// else
							// cout << "Component not found!!!!" << endl;
					}
				}
				/*
				else if (holons.at(j).m_name == levels.at(i).m_component2)
				{
					for (int k = 0; k < holons.size(); k++)
					{
						if (holons.at(k).m_name == levels.at(i).m_component1)
						{
							automaton DES = sync(holons.at(k), holons.at(j), levels.at(i).m_name, transHolon);
							holons.push_back(DES);
							DES.print();
						}
						else
							cout << "Component not found!!!!" << endl;
					}
				}
				*/
			}
		}
		
		else if (levels.at(i).m_relation == "OR")
		{
			for (int j = 0; j < holons.size(); j++)
			{
				if (holons.at(j).m_name == levels.at(i).m_component1)
				{
					for (int k = 0; k < holons.size(); k++)
					{
						if (holons.at(k).m_name == levels.at(i).m_component2)
						{
							automaton DES = union_holons(holons.at(j), holons.at(k), levels.at(i).m_name, transHolon);
							holons.push_back(DES);
							// DES.print();
						}
						// else
							// cout << "Component not found!!!!" << endl;
					}
				}
				/*
				else if (holons.at(j).m_name == levels.at(i).m_component2)
				{
					for (int k = 0; k < holons.size(); k++)
					{
						if (holons.at(k).m_name == levels.at(i).m_component1)
						{
							automaton DES = union_holons(holons.at(k), holons.at(j), levels.at(i).m_name, transHolon);
							holons.push_back(DES);
							DES.print();
						}
						else
							cout << "Component not found!!!!" << endl;
					}
				}
				*/
			}
		}
		
		else continue;
	}
	
	return holons.at(holons.size()-1);
}


int main()
{
	// Use example to test our short program.
	automaton A("A", {state("A0",{"A0"}),state("A1",{"A1"}),state("A2",{"A2"})}, 
				{11,12,14}, {transition("A0",11,"A1"),transition("A1",12,"A2"),transition("A2",14,"A0")});
	automaton B("B", {state("B0",{"B0"}),state("B1",{"B1"}),state("B2",{"B2"})}, 
				{21,12,14}, {transition("B0",21,"B1"),transition("B1",12,"B2"),transition("B2",14,"B0")});
	automaton C("C", {state("C0",{"C0"}),state("C1",{"C1"})}, 
				{15,17}, {transition("C0",15,"C1"),transition("C1",17,"C0")});

	vector<transition> transHolon;
	transHolon.push_back(transition("C1",25,"A0"));
	transHolon.push_back(transition("C1",25,"B0"));
	transHolon.push_back(transition("A2",27,"C0"));
	transHolon.push_back(transition("B2",27,"C0"));
	
	vector<automaton> holons;
	holons.push_back(A);
	holons.push_back(B);
	holons.push_back(C);
	
	vector<level> levels;	
	levels.push_back(level("ABC", "OR", "AB", "C"));
	levels.push_back(level("AB", "AND", "A", "B"));
	
	automaton DES1 = DES_generate(holons, levels, transHolon);
	
	DES1.print();





	automaton E("E", {state("E0",{"E0"}),state("E1",{"E1"})}, {1,2}, {transition("E0",1,"E1"),transition("E1",2,"E0")});
	automaton F("F", {state("F0",{"F0"}),state("F1",{"F1"})}, {3,2}, {transition("F0",3,"F1"),transition("F1",2,"F0")});
	automaton G("G", {state("G0",{"G0"}),state("G1",{"G1"})}, {5,6}, {transition("G0",5,"G1"),transition("G1",6,"G0")});
	automaton H("H", {state("H0",{"H0"}),state("H1",{"H1"})}, {7,6}, {transition("H0",7,"H1"),transition("H1",6,"H0")});
	
	vector<transition> trans_EFGH;
	trans_EFGH.push_back(transition("E1",9,"G0"));
	trans_EFGH.push_back(transition("E1",9,"H0"));
	trans_EFGH.push_back(transition("F1",9,"G0"));
	trans_EFGH.push_back(transition("F1",9,"H0"));
	trans_EFGH.push_back(transition("G1",8,"E0"));
	trans_EFGH.push_back(transition("G1",8,"F0"));
	trans_EFGH.push_back(transition("H1",8,"E0"));
	trans_EFGH.push_back(transition("H1",8,"F0"));
	
	vector<automaton> holons_EFGH;
	holons_EFGH.push_back(E);
	holons_EFGH.push_back(F);
	holons_EFGH.push_back(G);
	holons_EFGH.push_back(H);
	
	vector<level> levels_EFGH;
	levels_EFGH.push_back(level("EFGH","OR","EF","GH"));
	levels_EFGH.push_back(level("GH","AND","G","H"));
	levels_EFGH.push_back(level("EF","AND","E","F"));
	
	automaton DES2 = DES_generate(holons_EFGH, levels_EFGH, trans_EFGH);
	
	DES2.print();

	
	return 0;
}




