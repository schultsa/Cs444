#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include "mt19937ar.c"
#include <time.h>


void producer(void*);
void consumer(void*);
unsigned long gen_rendom_num();



int rdrand64_step(uint64_t *arand){
	unsigned char ok;
	__asm__ __volatile__("rdrand %0; setc %1"
			: "=r" (*arand), "=qm" (ok));
	return (int)ok;
}

unsigned long gen_random_num()
{
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	
	uint64_t x;
	
	char vendor[13];
	unsigned long seed_val = (unsigned long) pthread_self() ;
	eax = 0x01;

	__asm__ __volatile__(
			"cpuid;"
			: "=a"(eax)," =b"(ebx), "=c"(ecx), "=d"(edx)
			: "a"(eax)
			);

	if(ecx & 0x40000000){
		rdrand64_step(&x);
		return x;
	}
	
	else{
	//	init_genrand(seed_val);
		return abs(genrand_int32());
	}
}

struct buffer_item{
	int value;
	int wait_time;
};

struct proc_buffer{
	struct buffer_item list[32];
	pthread_mutex_t lock;
	int count;
};
//pthread_mutex_t lock;
 
pthread_cond_t cond;
//pthread cond_t empty;
//


struct proc_buffer process_buffer;

struct buffer_item gen_producer(){
	struct 	buffer_item buff_item;
	int sleep_time = gen_random_num();
	 	
	buff_item.wait_time = sleep_time % 7 + 2;
	buff_item.value = gen_random_num();
	printf("DEBUG: Gen values, WaitTime: %d, Value: %d\n", buff_item.wait_time, buff_item.value);
	return buff_item;
}

void producer(void* item){
	int wait_time;
	while(1){
		struct buffer_item new_item = gen_producer();		
		wait_time = gen_random_num() % 5 + 3;
		printf("producer waiting %d seconds to generate\n", wait_time);
		(void)sleep(wait_time);
		pthread_mutex_lock(&(process_buffer.lock));
		 while(process_buffer.count == 31){
                        pthread_cond_wait(&cond, &(process_buffer.lock));
                        //thread_mutex_unlock(&lock);
                       // return;
                 }
                                                                                        
		
                

		process_buffer.list[process_buffer.count+1] = new_item;
		//process_buffer.list[process_buffer.count] = gen_producer();
                process_buffer.count++;

		printf("process_buffer.count = %d\n", process_buffer.count);
		

		
		
		printf("producer value = %d, wait time = %d\n", process_buffer.list[process_buffer.count].value, process_buffer.list[process_buffer.count].wait_time);
		pthread_mutex_unlock(&(process_buffer.lock));
	}
	pthread_exit(0);
	
}


void consumer(void* item){
	//(struct buffer_item) item;
//	int sleepTime;
	 while(1){
		//sleepTime = 
		//pthread_mutex_lock(&process_buffer.lock);
		if(process_buffer.count > -1){
			//pthread_mutex_unlock(&process_buffer.lock);
			//return;
		
		
		

			printf("sleeing %d seconds \n", process_buffer.list[process_buffer.count].wait_time);
               		(void)sleep(process_buffer.list[process_buffer.count].wait_time);
			printf("Item %d consumed, wait time = %d\n", process_buffer.count,process_buffer.list[process_buffer.count].wait_time );

			pthread_mutex_lock(&(process_buffer.lock));
			process_buffer.list[process_buffer.count].value = 0;
			process_buffer.list[process_buffer.count].wait_time = 0;
			process_buffer.count--;
		//struct buffer_item item = process_buffer.list[process_buffer.count];
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&(process_buffer.lock));
	//	printf("sleeing %d seconds \n", process_buffer.list[process_buffer.count].wait_time);
//		
//		(void)sleep(process_buffer.list[process_buffer.count].wait_time);
		//printf("Sleeping %d seconds\n", item.wait_time);
//		printf("Item %d consumed, wait time = %d\n", process_buffer.count,process_buffer.list[process_buffer.count].wait_time );

		}
	}
	pthread_exit(NULL);

}



			




 	
			
	

int main(int argc, char **argv){
	int threads;
	int loop_counter;
	pthread_t *prods;
	pthread_t *cons;
//	pthread_cond_init(&cond, NULL);
	process_buffer.count = 0 ;
	pthread_mutex_init(&(process_buffer.lock), NULL);
	init_genrand((unsigned long)time(NULL));
	if(argc <= 1){
		threads = 2;
	}
	else{
		threads = atoi(argv[1]);
	}
	
	
	
	prods = malloc(sizeof(pthread_t) * threads);
	cons = malloc(sizeof(pthread_t) * threads);
	
	for (loop_counter = 0; loop_counter < 32; loop_counter++){
		process_buffer.list[loop_counter].value = 0;
		process_buffer.list[loop_counter].wait_time = 0;
	}
	for(loop_counter = 0; loop_counter < threads; loop_counter++){

		pthread_create(&prods[loop_counter], NULL, (void*)producer, NULL);
		pthread_create(&cons[loop_counter], NULL, (void*)consumer, NULL);
		pthread_join(cons[loop_counter], NULL);
		pthread_join(prods[loop_counter], NULL);
	}

	pthread_mutex_destroy(&(process_buffer.lock));
	pthread_cond_destroy(&cond);

	free(prods);
	free(cons);
	return 0;
}		 
