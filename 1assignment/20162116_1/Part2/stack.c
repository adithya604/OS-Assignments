/* stack used in program to implement split+tac of Unix */

struct Position	/* holds position and offset of lines */
{
	int current_pos;
	int cur_offset;
};

struct Stack 
{
    struct Position data;
    struct Stack *next;
}*top;
 
void initialize() 
{
    top = NULL;
} 
 
int isEmptyStack()
{
    if (top == NULL) 
        return 1;
    else
        return 0;
}


void push(struct Position pos) 
{
    struct Stack *temp;
    temp =(struct Stack *)malloc(1*sizeof(struct Stack));
    if(!temp)
    	return;
    temp->data = pos;
    temp->next = top;
    top = temp;
}
 
struct Position pop() 
{
	struct Position pos;
    struct Stack *temp;
    if (isEmptyStack())
    {
    	pos.current_pos = -1;
    	pos.cur_offset = -1;
        return pos;
    }
    temp = top;
    top = top->next;
    pos = temp->data;   
    free(temp); 
    return pos;
}