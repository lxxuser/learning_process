#include "user_defined.h"

char Json_Buf[1024];	

void JSON_Upper(void) //模拟阶段，现在只有水的重量
{
	char buf[50];
	
	memset(Json_Buf,0,1024);
	
	strncpy(Json_Buf,"{params:",strlen("{params:"));
	
	sprintf(buf,"{\"%s\":%.1f}",Param1,(float)(Weight_Shiwu) );
	
	strcat(Json_Buf,buf);
	
	strcat(Json_Buf,"}");
}