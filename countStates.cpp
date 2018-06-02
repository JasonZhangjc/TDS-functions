#include <vector> 
#include <string> 
#include <fstream> 
#include <iostream> 
 
using namespace std; 

//这个函数是用来将我的txt的每一行用逗号隔开的，接下来的任务就是对于分割好的string，数一下其中/的个数。
//This algorithm is so elegant and efficient!
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}

int main() 
{ 
    ifstream myfile("no_spec.txt"); 
    //ofstream outfile("output.txt", ios::app); 
    string temp;             // Store the content of every line.
    string temp_temp;
    int states = 0;
    int statesThisLine = 1;  // Record the number of states of every line.
    int statesTemp = 1;
    string::size_type position;
    
    if (!myfile.is_open()) 
    { 
        cout << "Couldn't open the file!" << endl; 
    } 
    
    // Count from the right place of the file.
    while(getline(myfile,temp)) 
    { 
        //cout << temp << endl;
        //Unnecessary to cout;
        if (temp == "Applying the control patterns above, the system under control can reach the supremal state sets:")
		{
			//cout << temp << endl; 
			//Unnecessary to cout;
			while(getline(myfile,temp)) 
			{ 
				if(temp.size() != 0)
					statesThisLine = 1;
					
				//cout << temp << endl;
				
				vector<string> v;
				SplitString(temp, v, ","); //三个输入，待分割string，分割后string，分隔符;
			
				//对于分割后的所有clause进行操作;
				for(vector<string>::size_type i = 0; i != v.size(); ++i)
				{
					//cout << v[i] << " ";
					while (position != v[i].npos)  //对于每一个clause进行操作;
					{
						position = v[i].find("/"); //find a “/”
					
						if(position != v[i].npos)
						{
							statesTemp += 1; //更新每个clause里“/”的数量
							v[i] = v[i].substr(position + 1);	
						}
					}
					statesThisLine *= statesTemp;
					statesTemp = 1;
					position = 0;
				}
				
				//outfile << temp; 
				//outfile << endl;
				states += statesThisLine;
				statesThisLine = 0;
				//cout << endl;
			} 
		}
	}
	 
    // Count the number of '/', 
    // though it is not precisely the number of states under all circumstances.
    
    cout << "!!!!!!!!!!!!! " << states << " !!!!!!!!!!!!" << endl;
    
    //***************************************************************
    myfile.close(); 
    //outfile.close();
    getchar();
    return 0; 
}  



