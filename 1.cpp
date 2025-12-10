#include <bits/stdc++.h>
using namespace std;
struct Student
{
    string name;
    int age;
    int score;
};  
int main()
{ 
    vector<Student> v;
    for(int i=0;i<5;i++)
    {
        Student s;
        cin>>s.name>>s.age>>s.score;
        v.push_back(s);
    }
    sort(v.begin(),v.end(),[](Student a,Student b){
        if(a.score!=b.score)
            return a.score>b.score;
        else
            return a.age<b.age;
    });
    auto ele=max_element(v.begin(),v.end(),[](Student a,Student b){
        return a.age<b.age;
    });
    for(auto i:v)
    {
        cout<<i.name<<" "<<i.age<<" "<<i.score<<endl;
    }
    cout<<ele->name<<" "<<ele->age<<" "<<ele->score<<endl;
}