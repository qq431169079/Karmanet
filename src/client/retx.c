#include "retx.h"

pthread_mutex_t retx_mutex = PTHREAD_MUTEX_INITIALIZER;

void retx_func(void *args) {
	struct retx_info *rtxinfo = (struct retx_info *)args;
	struct timeval now;
	struct timespec timeout;
	int to;
	int rc, errors = 0;
	int indx = rtxinfo->index;	
	int tmp;
	
	while (1) {
		to = DEFAULT_TO;
		sem_wait(&rtx_sem);
		rtxinfo->snd_info.socksd = packet_to_send.socksd;
		addrcpy(&rtxinfo->snd_info.addrto, &packet_to_send.addrto);
		pckcpy(&rtxinfo->snd_info.pck, &packet_to_send.pck);
		sem_post(&pck_sem);	
		tmp = rtxinfo->snd_info.pck.index;
		errors = 0;
		while (1) {
			printf("Thread %d: data: %s, index: %d timeout: %d sec\n", indx, rtxinfo->snd_info.pck.cmd, tmp, to);
			pthread_mutex_lock(&retx_mutex);
			if (send_packet(rtxinfo->snd_info.socksd, &rtxinfo->snd_info.addrto, &rtxinfo->snd_info.pck) < 0) {
				pthread_mutex_unlock(&retx_mutex);
				perror("errore in sendto");
				errors ++;
				if (errors > 5 ) {
					break;
				} else { 
					continue;
				}
			}
			pthread_mutex_unlock(&retx_mutex); 
			gettimeofday(&now, NULL);
			timeout.tv_sec = now.tv_sec + to;
			timeout.tv_nsec = now.tv_usec * 1000;
			if ((rc = sem_timedwait(&rtxinfo->sem, &timeout)) != 0) {
				if (errno == ETIMEDOUT) {
					to ++;
					printf("RITRASMETTO\n");
					continue;					
				} else {
					perror("errore in sem_timedwait");
				}
			} else {
				pthread_mutex_lock(&retx_mutex); 
				rtxinfo->snd_info.pck.index = -1;
				pthread_mutex_unlock(&retx_mutex); 			
				break;
			}
		}
	}

}

int retx_send(int socksd, const struct sockaddr_in *addr, const struct packet *pck) {
//	printf("============ENTRO IN RETX_SEND=========\n");
	
	sem_wait(&pck_sem);
	packet_to_send.socksd = socksd;
	addrcpy(&packet_to_send.addrto, addr);
	pckcpy(&packet_to_send.pck, pck);
	sem_post(&rtx_sem);
	
//	printf("============ESCO DA RETX_SEND=========\n");
	return 0;
	
}

int retx_stop(int pck_index) {
//	printf("\n============ENTRO IN RETX_STOP: pck_index:%d =========\n", pck_index);
	int i;
	
	for (i = 0; i < NTHREADS; i ++) {
		if (retx_threads[i].snd_info.pck.index == pck_index) {
			break;
		} 
	}
	
	if (i == NTHREADS) {
		printf("NO THREAD FOUND FOR %d\n", pck_index);
		return -1;
	}
	
	sem_post(&retx_threads[i].sem);
//	printf("============ESCO DA RETX_STOP=========\n");
	return 0;
}

int retx_init(int wr_pipe, pthread_mutex_t *pipe_mutex) {
	int i;
	sem_init(&rtx_sem, 0, 0);
	sem_init(&pck_sem, 0, 1);
	for (i = 0; i < NTHREADS; i ++) {
		sem_init(&retx_threads[i].sem, 0, 0);
		retx_threads[i].index = i;
		retx_threads[i].retx_wr_pipe = wr_pipe;
		retx_threads[i].pipe_mutex = pipe_mutex;
		if (pthread_create(&retx_threads[i].thread, NULL, (void *)&retx_func, (void *)&retx_threads[i]) != 0) {
			fprintf(stderr,  "errore in pthread_create\n");
		 	return -1;
		} 
	}
	return 0;
}

int retx_recvfrom(int socksd, struct sockaddr_in *addr, struct packet *pck, int *addr_len) {
	if (recvfrom_packet(socksd, addr, pck, addr_len) < 0) {
		return -1;
	}
	
	retx_stop(pck->index);
	
	return 0;
}

int retx_recv(int socksd, struct packet *pck) {
	return retx_recvfrom(socksd, NULL, pck, NULL);
}

int mutex_send(int socksd, const struct sockaddr_in *addr, const struct packet *pck) {
	pthread_mutex_lock(&retx_mutex);
	if (send_packet(socksd, addr, pck) < 0) {
		pthread_mutex_unlock(&retx_mutex);
		perror("mutex_send error");
		return -1;
	}
	pthread_mutex_unlock(&retx_mutex);
	return 0;
}

