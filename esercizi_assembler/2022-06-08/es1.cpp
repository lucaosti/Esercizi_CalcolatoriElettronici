#include "s/cc.h"
cl::cl(char c, st1& s2)
{
	for (int i = 0; i < 2; i++) {
		s.vc[i] = c;
		v[i] = s2.vc[i] - c;
	}
}
void cl::elab1(st1& s1)
{	
	cl cla('p', s1);
	for (int i = 0; i < 2; i++) {
		if (s.vc[i] < s1.vc[i]) {
			s.vc[i] = cla.s.vc[i];
			v[i] = cla.v[i] + i;
		}
	}
}
