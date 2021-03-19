#include <types.h>
#include <pid_system.h>
#include <lib.h>
#include <machine/spl.h>

struct pid* create_pid_list(){

    struct pid *pid = kmalloc(sizeof(struct pid));
	if (pid==NULL) {
		return NULL;
	}

    //first process has pid of 1
    pid->pidValue = 1;

    //the pid of 1 is being used so it is unavailable
    pid->isUsed = 1;

    pid->next = NULL;

    return pid;
}

void append_pid(struct pid* head, int pidValue) 
{   
    int s;
    s = splhigh();

    struct pid* new_pid = (struct pid*) kmalloc(sizeof(struct pid)); 
  
    struct pid *last = head;
   
    new_pid->pidValue  = pidValue;
    new_pid->isUsed = 1;
    new_pid->next = NULL;
       
    while (last->next != NULL) 
        last = last->next; 
    
    last->next = new_pid;

    splx(s);
    return;     
} 

int assign_pid(struct pid* head){

    int s;
    s = splhigh();

    struct pid *current = head;
    struct pid *next = current->next;

    while(current != NULL){
        if(next == NULL){
            append_pid(head, current->pidValue+1);
            splx(s);
            return current->pidValue+1;
        }
        if(next->isUsed == 0){
            next->isUsed = 1;
            splx(s);
            return next->pidValue;
        }
        current = next;
        next = current->next;
    }

    splx(s);
    return -1;
}

void release_pid(struct pid* head, int pidValue){

    int s;
    s = splhigh();

    struct pid *current = head;

    while(current != NULL){
        if(current->pidValue == pidValue){
            current->isUsed = 0;

            splx(s);
            return;
        }
        current = current->next;
    }

    splx(s);
}

void delete_pid_list(struct pid* head){
       /* deref head_ref to get the real head */
   struct pid* current = head;
   struct pid* next;
 
   while (current != NULL) 
   {
       next = current->next;
       kfree(current);
       current = next;
   }
   
   head = NULL;
}
