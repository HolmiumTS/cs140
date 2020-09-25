#include "pintos_thread.h"
#include <stdio.h>

struct station {
	// FILL ME IN
	struct lock lock;
	int seat_num;
	int board_num;
	int wait_num;
	// for both wait_num and seat_num
	struct condition pass_cond;
	struct condition train_cond;
};

void
station_init(struct station *station)
{
	// FILL ME IN
	lock_init(&station->lock);
	cond_init(&station->pass_cond);
	cond_init(&station->train_cond);
	station->seat_num=0;
	station->board_num=0;
	station->wait_num=0;
}

void
station_load_train(struct station *station, int count)
{
	// FILL ME IN
	lock_acquire(&station->lock);
	if(count==0||station->wait_num==0){
		lock_release(&station->lock);
		return;
	}
	station->seat_num=count;
	station->board_num=station->seat_num<station->wait_num?station->seat_num:station->wait_num;
	//fprintf(stderr,"board:%d\n",station->board_num);
	//fprintf(stderr,"wait:%d\n",station->wait_num);
	//fprintf(stderr,"seat:%d\n",station->seat_num);
	do{
		cond_broadcast(&station->pass_cond,&station->lock);
		cond_wait(&station->train_cond,&station->lock);
	}while(station->board_num!=0);
	station->seat_num=0;
	lock_release(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
	// FILL ME IN
	lock_acquire(&station->lock);
	station->wait_num++;
	while(station->seat_num<=0){
		cond_wait(&station->pass_cond,&station->lock);
	}
	station->wait_num--;
	station->seat_num--;
	//fprintf(stderr,"##%d\n",station->board_num);
	cond_broadcast(&station->pass_cond,&station->lock);
	lock_release(&station->lock);
}

void
station_on_board(struct station *station)
{
	// FILL ME IN
	lock_acquire(&station->lock);
	station->board_num--;
	cond_broadcast(&station->train_cond,&station->lock);
	lock_release(&station->lock);
}

