// Programa para teste na junta 5 do ur3////////////////////////////////
// compilar : g++ open_socket.cpp socket_ur3_server.cpp -o server///////
////////////////////////////////////////////////////////////////////////
#include "ros/ros.h"
#include "ur3/join0.h"
#include "ur3/join1.h"
#include "ur3/join2.h"
#include "ur3/join3.h"
#include "ur3/join4.h"
#include "ur3/join5.h"
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
	ros::Publisher join0_pub = n.advertise<ur3::join0>("join0", 0);
	ros::Publisher join1_pub = n.advertise<ur3::join1>("join1", 0);
	ros::Publisher join2_pub = n.advertise<ur3::join2>("join2", 0);
	ros::Publisher join3_pub = n.advertise<ur3::join3>("join3", 0);
	ros::Publisher join4_pub = n.advertise<ur3::join4>("join4", 0);
	ros::Publisher join5_pub = n.advertise<ur3::join5>("join5", 0);
	ros::Rate loop_rate(125);
	//Declaração das estruturas de dados para as publicações
	ur3::join5 data_join0;
	ur3::join5 data_join1;
	ur3::join5 data_join2;
	ur3::join5 data_join3;
	ur3::join5 data_join4;
	ur3::join5 data_join5;
	
    while (ros::ok()){

		// data for join 5
		data_join5.pose = pose_float;
		data_join5.velocity = vel_float;
		data_join5.torque = vel_float;
		//////////////////////////////

		referencia = sin ((conta*PI)/180);
		buffer_in = (int)(referencia*norma_float);
		buffer_in = converter(buffer_in);
		send(new_socket, &buffer_in, sizeof buffer_in, 0);
    	recv(new_socket, &buffer_out, sizeof buffer_out, 0);

		memcpy(&vel_int32, &buffer_out[0], sizeof(int32_t));
		memcpy(&pose_int32, &buffer_out[4], sizeof(int32_t));
		memcpy(&torque_int32, &buffer_out[8], sizeof(int32_t));

		vel_int32 = converter(vel_int32);
		pose_int32 = converter(pose_int32);
		torque_int32 = converter(torque_int32);

		vel_float = ((double)vel_int32)/norma_float;
		pose_float = ((double)pose_int32)/norma_float;
		torque_float = ((double)torque_int32)/norma_float;
		fprintf(fp, "\n%10.5f, %10.5f, %10.5f, %10.5f, %10.5f", tempo, referencia, vel_float,
			 pose_float, torque_float);
		//printf("\n%f, %f, %f\n", vel_float, pose_float, torque_float);
		tempo = tempo + 0.008;
		conta = conta + 0.5;
	
		join5_pub.publish(data_join5);
		ros::spinOnce();
		loop_rate.sleep();
			
	}
	fclose(fp);
	return 0;
} 
