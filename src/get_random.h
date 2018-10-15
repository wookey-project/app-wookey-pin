#ifndef GET_RANDOM_H_
#define GET_RANDOM_H_

void get_random(unsigned char * a,unsigned size)
{
	*(unsigned int*)a=size;
}

#endif
