// Programa para teste na junta 5 do ur3////////////////////////////////
// compilar : g++ open_socket.cpp socket_ur3_server.cpp -o server///////
////////////////////////////////////////////////////////////////////////
#include "ros/ros.h"
#include "ur3/arm_msg.h"
#include <X11/keysymdef.h>
#include <sys/socket.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <iostream>
#include <stdio.h>   
#include <string.h>  
#include <string> 
#include <stdlib.h>
#include <sstream>
#include <inttypes.h>
#include <math.h>
#include <ctime>
#include "open_socket.h"
#include "send_script.h"

#define PORT 5000
#define PI 3.1415
float referencia = 0;
/// little endian <-> big endian ///////
int converter(int32_t num){
	uint32_t b0,b1,b2,b3;
	uint32_t res;
	b0 = (num & 0x000000ff) << 24u;
	b1 = (num & 0x0000ff00) << 8u;
	b2 = (num & 0x00ff0000) >> 8u;
	b3 = (num & 0xff000000) >> 24u;
	res = b0 | b1 | b2 | b3;
	return res;
}
///////////////////////////////////////

int main(int argc, char **argv){ 
	// primeira coisa:
	// tem que enviar o arquivo urscript
	send_script(); // a função send_script envia o arquivo para o robô
	///////////////////////////////////
	int new_socket;
	// abrindo a comunicaçção tcp socket
	new_socket = open_socket();
	////////////////////////////////////
	// Declaração dos buffers de entrada e saida 
    int32_t buffer_in  = 0;
	int8_t buffer_out[127]; 
   	/////////////////////////////
	// criação de um arquivo .csv para armazenar os dados
	FILE *fp;
    char filename[20]= "dados.csv";
	fp=fopen(filename,"w+");
	fprintf(fp,"     t     ,     r     ,     v     ,     p     ,     m     \n");
    float tempo = 0;
	//////////////////////
	double vel_float = 0;
	int32_t vel_int32 = 0;
	int64_t vel_int64 = 0;
	//////////////////////
	double pose_float = 0;
	int32_t pose_int32 = 0;
	/////////////////////
	double torque_float = 0;
	int32_t torque_int32 = 0;
	/////////////////////
	float norma_float = 1000000.0;
	
	float conta = 0;
	///////////////////////
	//ROS 
	ros::init(argc, argv, "talker");
	ros::NodeHandle n;
	//Declaração das publicões 
	ros::Publisher arm_pub = n.advertise<ur3::arm_msg>("arm",0);
	ros::Rate loop_rate(125);
	//Declaração das estruturas de dados para as publicações
	ur3::arm_msg arm;
	//////////////////////////////////////////////////////////
    while (ros::ok()){

		// data for join 5
		arm.pose[5] = pose_float*180/PI;
		arm.velocity[5] = vel_float;
		arm.torque[5] = torque_float;
		//////////////////////////////

		referencia = 3*sin ((conta*PI)/180);
		buffer_in = (int)(referencia*norma_float);
		buffer_in = converter(buffer_in);
		send(new_socket, &buffer_in, sizeof buffer_in, 0);
    	recv(new_socket, &buffer_out, sizeof buffer_out, 0);

		//memcpy(&pose_int32, &buffer_out[0], sizeof(int32_t));
		//memcpy(&vel_int32, &buffer_out[4], sizeof(int32_t));
		//memcpy(&torque_int32, &buffer_out[8], sizeof(int32_t));
		memcpy(&pose_int32, &buffer_out[60], sizeof(int32_t));
		memcpy(&vel_int32, &buffer_out[64], sizeof(int32_t));
		memcpy(&torque_int32, &buffer_out[68], sizeof(int32_t));

		pose_int32 = converter(pose_int32);
		vel_int32 = converter(vel_int32);
		torque_int32 = converter(torque_int32);

		pose_float = ((double)pose_int32)/norma_float;
		vel_float = ((double)vel_int32)/norma_float;
		torque_float = ((double)torque_int32)/norma_float;
		fprintf(fp, "\n%10.5f, %10.5f, %10.5f, %10.5f, %10.5f", tempo, referencia, vel_float,
			 pose_float, torque_float);
		//printf("\n%f, %f, %f\n", vel_float, pose_float, torque_float);
		tempo = tempo + 0.008;
		conta = conta + 0.5;
	
		//join5_pub.publish(data_join5);
		arm_pub.publish(arm);
		ros::spinOnce();
		loop_rate.sleep();
			
	}
	fclose(fp);
	return 0;
} 
