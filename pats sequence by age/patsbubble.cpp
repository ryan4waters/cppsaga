struct pats
{
  string name;
  int age;
  string pos;
};

void patsbubble (pats patsarry[],int len){
  for (int i=0;i<len-1;i++){
    for (int j=0;j<len-i-1;j++){
      if (patsarry[j].age>patsarry[j+1].age){
        pats tmp=patsarry[j+1];
        patsarry[j+1]=patsarry[j];
        patsarry[j]=tmp;
      }
    }
  }
}

void patsprint (pats patsarry[],int len){
for (int i=0;i<len;i++){
  cout<<patsarry[i].name<<patsarry[i].age<<patsarry[i].pos<<endl;
}
}

int main(){
pats patsarry[5]={
  {"matthewjudon",29,"DE"},
  {"dontahightower",31,"LB"},
  {"nickfolk",37,"K"},
  {"ronnieperkins",22,"LB"},
  {"macjones",23,"QB"}
};
int len=sizeof(patsarry)/sizeof(patsarry[0]);
patsprint(patsarry,len);//before the sequence
patsbubble(patsarry,len);//sequence
patsprint(patsarry,len);//after the sequence
  system ("pause");
}
