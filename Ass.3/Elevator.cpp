#define Max_Floor 163
#define Min_Floor 1 
#include<bits/stdc++.h>         
using namespace std;

int main(int argc, char const *argv[]){

  int dir=0;
  string dir_str=(char *)argv[2];
  string filename=(char*)argv[1];
  if(dir_str=="upward")
    dir=1;  
  vector <vector <string> > data;
  ifstream infile(filename);
  std::vector<int> v1;
  std::vector<std::vector<int>> v2;
  int temp=0;
  int i=0;
  while (infile)
  {    
    string s,s1;
    infile>>temp;       
    if (!getline( infile, s )) break;
    //cout<<temp; 
    //cout<<s<<endl;
    v1.push_back(temp);
    std::stringstream ss( s );        
    v2.push_back(std::vector<int>());
    while (getline( ss, s1, ',' ))
    {       
      temp=stoi(s1);
      //cout<<temp<<" ";            
      v2[i].push_back(temp) ;
    }//cout<<endl;
    i++;
  }


  if (!infile.eof())
  {
    cerr << "Fooey!\n";
    return 0;
  }
  cout<<endl<<endl<<"**************************"<<endl<<endl; 
  for (int it1 = 0; it1 < v1.size(); ++it1)
  {
    cout<<v1[it1]<<"  ";
    for (int j=0 ; j < v2[it1].size(); ++j)
    {
      cout<<v2[it1][j]<<",";
    }
      cout<<endl;
  }
  cout<<endl<<endl<<"**************************"<<endl<<endl; 
  int flag,floor,ptr,start=1,steps=0;
  std::vector<int> seq,req_vec;
  std::vector<int>::iterator it1,it2;

  while(true){

    if(start){

      floor=v1[0];
      seq.push_back(floor);
      start=0;
      cout<<"Initial Elevator floor position: "<<floor<<endl;
    }
    it1=find(v1.begin(),v1.end(),floor);
    if( it1 != v1.end() ){
      ptr = it1 - v1.begin();
      for (int i = 0; i < v2[ptr].size(); ++i)
      {
        it2=find(req_vec.begin(),req_vec.end(),v2[ptr][i]);
        if(it2 == req_vec.end()){
          req_vec.push_back(v2[ptr][i]);
        }
        else{
          //Request already exists.
        }
      }        
    }
   //**********************************************OUTPUT THING******
    cout<<"Request Queue at Floor "<<floor<<":  ";
    for (int i = 0; i < req_vec.size(); ++i)
      {
        cout<<req_vec[i]<<" ";
      }
    cout<<endl<<endl; 
  //***********************************************

    if(dir){
      dir=0;
      flag=1;
      for (int i = 0; i < req_vec.size(); ++i)
      {
        if(req_vec[i]>floor && req_vec[i] <= Max_Floor){
          dir=1;
          flag=0;
          break;          
        }
      }
      if(flag){
        steps+=Max_Floor-floor;
        floor=Max_Floor;
      }
    }
    else{
     dir=1;
     flag=1;
      for (int i = 0; i < req_vec.size(); ++i)
      {
        if(req_vec[i]<floor && req_vec[i] >= Min_Floor){
          dir=0;
          flag=0;
          break;
        }
      }
      if(flag){
        steps+=floor- Min_Floor;
        floor=Min_Floor; 
      }
    }
    if(dir){
      temp=Max_Floor;
      for (int i = 0; i < req_vec.size(); ++i)
      {
        if (req_vec[i] > floor && req_vec[i] <= temp)
        {
          temp = req_vec[i];
        }
      }
      if(req_vec.size()) steps+=(temp-floor);
    }
    else{
      temp=Min_Floor;
      for (int i = 0; i < req_vec.size(); ++i)
      {
        if (req_vec[i] < floor && req_vec[i] >= temp)
        {
          temp = req_vec[i];
        }
      }
     if(req_vec.size()) steps+=(floor - temp);
    }

    it1 = find(req_vec.begin(),req_vec.end(),temp);
    if (it1 != req_vec.end())
    {
      //it1 = it1 - req_vec.begin();
      req_vec.erase(it1);
      seq.push_back(temp);
      floor = temp;
      cout<<"Reached Floor "<<floor<<endl;
      cout<<"Steps covered till now "<<steps<<endl;
    }
    else{
      cout<<"**************Lift Stop***************"<<endl<<endl;
      cout<<"Total movement of the Elevator: "<<steps<<endl;
      cout<<"Order in which the floor requests are serviced:"<<endl;
      for (int i = 0; i < seq.size(); ++i)
      {
        cout<<seq[i];
        if(i != seq.size()+1)
          cout<<" - ";
      }
      return 0;
    }

  }


    return 0;
}
