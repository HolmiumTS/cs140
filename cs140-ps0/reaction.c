#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	// FILL ME IN
	struct lock lock;
	struct condition cond_o;
	struct condition cond_h;
	int num_o;
	int num_h;
	int num_w;
};

void
reaction_init(struct reaction *reaction)
{
	// FILL ME IN
	lock_init(&reaction->lock);
	cond_init(&reaction->cond_o);
	cond_init(&reaction->cond_h);
	reaction->num_o=0;
	reaction->num_h=0;
	reaction->num_w=0;
}

void
reaction_h(struct reaction *reaction)
{
	// FILL ME IN
	lock_acquire(&reaction->lock);
	reaction->num_h++;
	//fprintf(stderr,"^^%d %d\n",reaction->num_h,reaction->num_o);
	while(reaction->num_w<=0){
		cond_signal(&reaction->cond_o,&reaction->lock);
		cond_wait(&reaction->cond_h,&reaction->lock);
	}
	reaction->num_w--;
	cond_signal(&reaction->cond_o,&reaction->lock);
	lock_release(&reaction->lock);
}

void
reaction_o(struct reaction *reaction)
{
	// FILL ME IN
	lock_acquire(&reaction->lock);
	fprintf(stderr,"&&%d\n",reaction->num_h);
	while(reaction->num_h<2){
		cond_broadcast(&reaction->cond_h,&reaction->lock);
		cond_wait(&reaction->cond_o,&reaction->lock);
	}
	reaction->num_h-=2;
	reaction->num_w+=2;
	make_water();
	while(reaction->num_w>0){
		cond_broadcast(&reaction->cond_h,&reaction->lock);
		cond_wait(&reaction->cond_o,&reaction->lock);
	}
	cond_broadcast(&reaction->cond_o,&reaction->lock);
	lock_release(&reaction->lock);
}
