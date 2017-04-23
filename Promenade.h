#include<iostream>
using namespace std;

int num_below=0;

class Person{
public:
	Person();
	Person(int i, int e);
	~Person();

private:
	Person *parent, *child, *sibling; // the pointers to his parent, child, sibling
	int id; // record Person's id
	int energy; // record Person's energy
	int degree;// record how many levels of this Person's children
	friend class Promenade;//you could access all valuables in class Promenade
};

//ctor
Person::Person(){
	parent = child = sibling = NULL;
	id = 0;
	energy = 0;
	degree = 0;
}

//ctor
Person::Person(int i, int e){// define a new Person with id = i and energy = e
	parent = child = sibling = NULL;
	id = i;
	energy = e;
	degree = 0;
}

//dtor
Person::~Person(){

}

class Promenade{

public:
	Promenade();
	~Promenade();
	void one_person_joined(int id, int energy);
	void one_group_joined(Promenade *g);
	void absorb_energy(int id, int energy);
	void msheap(Promenade *g);//***
	void mergeTree(Person *y,Person *z);//***
	int postorder(int eng);//****
	void _postorder(Person* node,int eng);//***
	int  calculate_people_below(int energy);
	int  get_weakest_person();
	int  size();
	Person* search(Person *head, int k);
private:
	Person *head;
	int num;//caculate how many people in the promenade
};


//ctor
Promenade::Promenade(){
	head = NULL;
	num = 0;
}

//dtor
Promenade::~Promenade(){

}

void Promenade::one_person_joined(int id, int energy){
	//TODO: fill in the code to simulate a person with his id and energy to join the Promenade
	Person *the_person = new Person(id,energy);
	Person *tmp = head;
	num+=1;
	//head->degree == 0 可併入子樹中
	if(!tmp){
	    head = the_person;
	}else{
            if( head->degree == 0)
            {
                //step 1 加入heap
                if( head->energy > the_person->energy){
                    the_person->sibling = head->sibling;
                    head->sibling = NULL;
                    the_person->child = head;
                    head->parent = the_person;
                    the_person->degree += 1 ;
                    head = the_person;
                }else{
                    head->child = the_person;
                    the_person->parent = head;
                    head->degree += 1 ;
                }
                //step 2 若可以合併則head和下一個手足合併(root list degree increase),直到沒有手足或head無法與下一個合併
                Person *next_root = head->sibling;
                while( next_root!= NULL && head->degree == next_root->degree ){
                    if( head->energy > next_root->energy){
                        head->sibling = next_root->child;
                        next_root->child = head;
                        head->parent = next_root;
                        next_root->degree += 1 ;
                        head = next_root;
                    }else{
                        head->sibling = next_root->sibling;
                        next_root->sibling =head->child;
                        head->child = next_root;
                        next_root->parent = head;
                        head->degree += 1 ;
                    }
                    next_root = head->sibling;
                }
            }else{
                the_person->sibling = head;
                head = the_person;
            }
	}
}


void Promenade::one_group_joined(Promenade *g){
	//TODO: fill in the code to simulate one group to join the full promenade
    num+=g->num;
//    g->postorder(); 
    msheap(g);
	Person *prev = NULL;
	Person *current = head;
	Person *next = current->sibling;
	while(next){
        //current 和 next 不可合 or 當前和下兩個都同degree
        //以上情況都直接往下移動不做合併
        if( (current->degree!=next->degree) || (next->sibling&&next->sibling==current->sibling)){
            prev = current;
            current = next;
        }else if(current->energy <= next->energy){
            current->sibling = next->sibling;
           mergeTree(next,current);
        }else{
            if(!prev){
                head = next;
                mergeTree(current,next);
                current=next;
            }else{
                prev->sibling = next;
               mergeTree(current,next);
                current=next;
            }
        }
        next = next->sibling;
	}


}

void Promenade::absorb_energy(int id, int energy){
	//TODO: Elizabeth absord the energy of the person with this id, this Person's energy will decrease by this number.
	//		If his energy becomes lower than or equal to zero, he will die and should be thrown out automatically

	Person *target = search(head,id);
	if(!target)return;
	target->energy -= energy;
	int tmp_id,tmp_eng;
	while( target->parent && target->parent->energy > target->energy){
			tmp_eng = target->energy;
			tmp_id = target->id;
			target->energy = target->parent->energy;
			target->id = target->parent->id;
			target->parent->id = tmp_id;
			target->parent->energy = tmp_eng;
			target = target->parent;
	}
	if(target->energy > 0){
		return;
	}else{
		num-=1;
	  	Person *target_pre =NULL , *target_cur = head, *tmp = head;
		while(tmp->sibling){  //find the target and record pre
			if(target_cur == target)break;
			target_pre =tmp;
			target_cur =tmp->sibling;
			tmp = tmp->sibling;
		}
		if(!target_pre)head = target_cur->sibling;
		else target_pre->sibling = target_cur->sibling;
		
		Person *pre,*cur;
		pre =NULL;pre=NULL;
		cur = target_cur->child;
		while(cur){
			tmp = cur->sibling;
			cur->sibling = pre;
			cur->parent = NULL;
			pre = cur;
			cur = tmp;
		}
		Promenade *n = new Promenade();
		n->head = pre;
		one_group_joined(n);
	}

}

int Promenade::calculate_people_below(int energy){
	//TODO: You should calculate how many people's energy are below the number (include the number) and return it
	num_below=0;
	return postorder(energy);
}

int Promenade::get_weakest_person(){
	//TODO: return the id of the one with the least energy left. If there are more than one solution, just return one of them
    Person *current = head;
    int least_id = head->id;
    int least_eng = head->energy;
    while(current->sibling){
        if(least_eng > current->sibling->energy){least_id = current->sibling->id;least_eng= current->sibling->energy;}
        current = current->sibling;
    }
	return least_id;
}

int Promenade::size(){
//    postorder();
    //cout<<head->energy<<" "<<head->child->energy<<" "<<head->child->sibling->energy<<" "<<head->child->child->energy<<endl;
    //cout<<head->child->sibling->parent->energy<<endl;
//	Person *tmp=head;
//	cout<<head->id<<" "<<head->energy<<endl;
//	while(tmp->sibling){cout<<tmp->sibling->id<<" "<<tmp->sibling->energy<<endl;tmp=tmp->sibling;}
//	cout<<head->sibling->child->energy<<endl;
	return num;
}

//msheap : merge and sort two heap
void Promenade::msheap(Promenade *g){
    //記下兩BH的head
    Person *head1 = head;
    Person *head2 = g->head;
    if(!head1){
        head = head2;  // 原本空的->head指向g.head
	return;
    }else if(!head2){
        return;         //g是空的
    }
    //找比較小的當新的head
    Person *new_head;
    Person *current;
    if(head1->degree < head2->degree){
        new_head = head1;
        current = head1;
        head1 = head1->sibling;
    }else{
        new_head = head2;
        current = head2;
        head2 = head2->sibling;
    }
    //先確保head1&&head2 (皆存在) 排列
    while(head1&&head2){
        if(head1->degree < head2->degree){
            current->sibling = head1;
            current = head1;
            head1 = head1->sibling;
        }else{
            current->sibling = head2;
            current = head2;
            head2 = head2->sibling;
        }
    }
    //if head1 not finish => finish it
    while(head1){
        current->sibling = head1;
        current = head1;
        head1 = head1->sibling;
    }
    //if head2 not finish => done it
    while(head2){
        current->sibling = head2;
        current = head2;
        head2 = head2->sibling;
    }
    //all finish
    //把新的串列給head
    head = new_head;
}
//已知y > z
void Promenade::mergeTree(Person *y,Person *z){
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree++;
}

//debug
int Promenade::postorder(int eng){
  int num;
  Person* tmp = head;
  while(tmp){
    _postorder(tmp,eng);
    tmp = tmp->sibling;
  }
   num = num_below;
//    printf("\n");
  return num;
}

//用parent判斷是不是root,避免root跑去呼叫到別的BT
void Promenade::_postorder(Person* node,int eng){
    if(!node)
        return;
    _postorder(node->child,eng);
    if(node->parent)
        _postorder(node->sibling,eng);
//    printf("%d ",node->energy);
    if(node->energy <= eng)++num_below;
}
//serch k
Person* Promenade::search(Person *head, int k){
	Person *x = head;
	Person *p =NULL;
	if (x->id == k)
    {
        p = x;
        return p;
    }
    if (x->child != NULL && p == NULL)
        p = search(x->child, k);
    if (x->sibling != NULL && p == NULL)
        p = search(x->sibling, k);
    return p;
}
