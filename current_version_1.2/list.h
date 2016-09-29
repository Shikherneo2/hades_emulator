struct data{
       unsigned char keybrd[24];
       unsigned char mouse[10];
       WORD id;
}temp45;

struct node{
       data inst;
       node *next;
}start, *curr = new node, *user = new node;

void lList(int nai = 0){
     if( nai == 1 ){
         curr = new node;
         user = new node;
         start.next = NULL;
         start.inst= temp45;
         curr = &start;
     }
     
     else{
        user = new node;
        user->inst = temp45;
        user->next = NULL;
        curr->next = user;
        curr = user;
     }
}

void closeList(){
     node *temp, *temp2;
     temp = &start;
     while( temp != NULL){
            temp2 = temp;
            temp = temp->next;
            delete temp2;
     }
}
