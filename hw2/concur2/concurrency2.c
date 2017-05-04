#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

typedef int bool;
#define true 1
#define false 0

typedef struct guy{
	int num_fork;
	int time_spent_hungry;
	bool eating;
} guy;

typedef struct philosophers{ //true is a philosopher that will eat, false is a hungry philosopher
	guy aristotle;
	guy confucious;
	guy plato;
	guy pythagoras;
	guy ayn_rand;
} philosophers;

void think(philosophers hungry_guys); //decides which philosopher will eat, sets identity in the philosopher struct
void get_forks(philosophers hungry_guys);
void eat(philosophers hungry_guys);
void put_forks(philosophers hungry_guys);
void print_status(philosophers hungry_guys);

//openMP splits into threads to coordinate fork acquisition/placement
//3 barriers coordinate timings between threads

main() {
	philosophers hungry_guys;
	
	hungry_guys.aristotle.num_fork = 0;
	hungry_guys.confucious.num_fork = 0;
	hungry_guys.plato.num_fork = 0;
	hungry_guys.pythagoras.num_fork = 0;
	hungry_guys.ayn_rand.num_fork = 0;
	
	int seconds_past;
	seconds_past = 0;
	
	hungry_guys.aristotle.time_spent_hungry = 0;
	hungry_guys.confucious.time_spent_hungry = 0;
	hungry_guys.plato.time_spent_hungry = 0;
	hungry_guys.pythagoras.time_spent_hungry = 0;
	hungry_guys.ayn_rand.time_spent_hungry = 0;
	
	
	hungry_guys.aristotle.eating = true;
	hungry_guys.confucious.eating = false;
	hungry_guys.plato.eating = false;
	hungry_guys.pythagoras.eating = true;
	hungry_guys.ayn_rand.eating = false;
	
	int done;
	done = 0;
	
	omp_set_num_threads( 4 );
	do {	
		#pragma omp parallel sections 
		{
			#pragma omp section 
			{
				think(hungry_guys);
			}
			
			#pragma omp section 
			{
				get_forks(hungry_guys);
			}
			
			#pragma omp section 
			{
				eat(hungry_guys);
			}
			
			#pragma omp section 
			{
				put_forks(hungry_guys);
			}
		}
		seconds_past++;
		printf("%d seconds have passed\n", seconds_past);
		printf("Continue running? Type 1 to continue or 0 to quit\n");
		scanf("%d", &done);
	} while (!done);
	return 0;
}

void think(philosophers hungry_guys) {
	//start think
	if (hungry_guys.aristotle.eating) {
		hungry_guys.confucious.eating = true;
		hungry_guys.aristotle.eating = false;
	}
	
	if (hungry_guys.confucious.eating) {
		hungry_guys.plato.eating = true;
		hungry_guys.confucious.eating = false;
	}
	
	if (hungry_guys.plato.eating) {
		hungry_guys.pythagoras.eating = true;
		hungry_guys.plato.eating = false;
	}
	
	if (hungry_guys.pythagoras.eating) {
		hungry_guys.ayn_rand.eating = true;
		hungry_guys.pythagoras.eating = false;
	}
	
	if (hungry_guys.ayn_rand.eating) {
		hungry_guys.aristotle.eating = true;
		hungry_guys.ayn_rand.eating = false;
	}
	
	srand(time(NULL));
	printf("Thinking time!\n");
	sleep((rand() % 20) + 1); //Waiting/Thinking is between 1 and 20 seconds
	
	print_status(hungry_guys);
	
	#pragma omp barrier
	#pragma omp barrier
	#pragma omp barrier
}

void get_forks(philosophers hungry_guys) {
	#pragma omp barrier
	//start get_forks
	printf("Eating time!\n");
	print_status(hungry_guys);
	if (hungry_guys.aristotle.eating) {
		hungry_guys.aristotle.num_fork = 2;
	}
	if (hungry_guys.confucious.eating) {
		hungry_guys.confucious.num_fork = 2;
	}
	if (hungry_guys.plato.eating) {
		hungry_guys.plato.num_fork = 2;
	}
	if (hungry_guys.pythagoras.eating) {
		hungry_guys.pythagoras.num_fork = 2;
	}
	if (hungry_guys.ayn_rand.eating) {
		hungry_guys.ayn_rand.num_fork = 2;
	}

	print_status(hungry_guys);
	#pragma omp barrier
	#pragma omp barrier
}

void eat(philosophers hungry_guys) {
	#pragma omp barrier
	#pragma omp barrier
	//start eat
	int forks_on_table = 5 - (hungry_guys.aristotle.num_fork + hungry_guys.confucious.num_fork + hungry_guys.plato.num_fork + hungry_guys.pythagoras.num_fork + hungry_guys.ayn_rand.num_fork);
	
	if (hungry_guys.aristotle.num_fork < 2) {
		hungry_guys.aristotle.time_spent_hungry++;
	}else{
		hungry_guys.aristotle.time_spent_hungry = 0;
	}
	if (hungry_guys.confucious.num_fork < 2) {
		hungry_guys.confucious.time_spent_hungry++;
	}else{
		hungry_guys.confucious.time_spent_hungry = 0;
	}
	if (hungry_guys.plato.num_fork < 2) {
		hungry_guys.plato.time_spent_hungry++;
	}else{
		hungry_guys.plato.time_spent_hungry = 0;
	}
	if (hungry_guys.pythagoras.num_fork < 2) {
		hungry_guys.pythagoras.time_spent_hungry++;
	}else{
		hungry_guys.pythagoras.time_spent_hungry = 0;
	}
	if (hungry_guys.ayn_rand.num_fork < 2) {
		hungry_guys.ayn_rand.time_spent_hungry++;
	}else{
		hungry_guys.ayn_rand.time_spent_hungry = 0;
	}

	
	//Print fork locations
	print_status(hungry_guys);
	
	sleep((rand() % 8) + 2); //Eating is between 2 and 9 seconds
	#pragma omp barrier
}

void put_forks(philosophers hungry_guys) {
	#pragma omp barrier
	#pragma omp barrier
	#pragma omp barrier
	//start put_forks
	hungry_guys.aristotle.num_fork = 0;
	hungry_guys.confucious.num_fork = 0;
	hungry_guys.plato.num_fork = 0;
	hungry_guys.pythagoras.num_fork = 0;
	hungry_guys.ayn_rand.num_fork = 0;
	
	//Print the fork count for each philosopher as well as how many are on the table
	print_status(hungry_guys);
}

void print_status (philosophers hungry_guys) {
	printf("Forks currently on the table: %d \n", 5 - (hungry_guys.aristotle.num_fork + hungry_guys.confucious.num_fork + hungry_guys.plato.num_fork + hungry_guys.pythagoras.num_fork + hungry_guys.ayn_rand.num_fork));
	printf("Aristotle's forks: %d \n", hungry_guys.aristotle.num_fork);
	printf("Confucious' forks: %d \n", hungry_guys.confucious.num_fork);
	printf("Plato's forks: %d \n", hungry_guys.plato.num_fork);
	printf("Pythagoras's forks: %d \n", hungry_guys.pythagoras.num_fork);
	printf("Ayn Rand's forks: %d \n\n", hungry_guys.ayn_rand.num_fork);
	
	printf("Aristotle has been hungry for %d seconds \n", hungry_guys.aristotle.time_spent_hungry);
	printf("Confucious has been hungry for %d seconds \n", hungry_guys.confucious.time_spent_hungry);
	printf("Plato has been hungry for %d seconds \n", hungry_guys.confucious.time_spent_hungry);
	printf("Pythagoras has been hungry for %d seconds \n", hungry_guys.confucious.time_spent_hungry);
	printf("Ayn Rand has been hungry for %d seconds \n\n", hungry_guys.confucious.time_spent_hungry);
}