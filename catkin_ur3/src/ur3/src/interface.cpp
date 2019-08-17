// Programa para teste na junta 5 do ur3////////////////////////////////
// compilar : g++ open_socket.cpp socket_ur3_server.cpp -o server///////
////////////////////////////////////////////////////////////////////////
#include "ros/ros.h"
#include "sensor_msgs/JointState.h"
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
#include "reverse_word.h"
#include "join_data.h"

#define PORT 5000
#define PI 3.1415
float referencia = 0;
/// little endian <-> big endian ///////
 int reverse(int32_t num){
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
	float* data_join_out;
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
	int8_t buffer_out[2024]; 
	float tempo = 0;
   	/////////////////////////////
	// criação de um arquivo .csv para armazenar os dados
	FILE *fp;
    char filename[20]= "dados.csv";
	fp=fopen(filename,"w+");
	fprintf(fp,"     t     ,   pj0    ,    vj0    ,    tj0    ,    pj1    ,    vj1    ,    tj1    ,    pj2    ,    vj2    ,    tj2    ,    pj3    ,    vj3    ,    tj3    ,    pj4    ,    vj4    ,    tj4    ,    pj5    ,    vj5    ,    tj5    \n");
    
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
	ros::Publisher arm_pub = n.advertise<sensor_msgs::JointState>("arm",0);
	ros::Rate loop_rate(55);
	//Declaração das estruturas de dados para as publicações
	sensor_msgs::JointState arm;
	arm.header.frame_id = "Base";
	arm.name.resize(6);
	arm.position.resize(6);
	arm.velocity.resize(6);
	arm.effort.resize(6); 
	arm.name[0] = "Base";
	arm.name[1] = "Shoulder";
	arm.name[2] = "Elbow";
	arm.name[3] = "Wrist 1";
	arm.name[4] = "Wrist 2";
	arm.name[5] = "Wrist 3";
	
	int number = 0;
	
	//////////////////////////////////////////////////////////
    while (ros::ok()){
		/////////////////////////////////////////////////////
		// arm.header.seq = con
		referencia = 1*sin ((conta*PI)/180);
		buffer_in = (int)(referencia*norma_float);
		buffer_in = reverse(buffer_in);
		send(new_socket, &buffer_in, sizeof buffer_in, 0);
		number = 0;
		///////////////////////////////////////////////////////////
		number = recv(new_socket, &buffer_out, 128, 0);
		
		/////////////////////////////////////////////////////
		data_join_out = join_data(buffer_out);
		/////////////////////////////////////////////////

		/////////////////////////////////////////
		arm.position[5] = data_join_out[15];
		arm.velocity[5] = data_join_out[16];
		arm.effort[5] = data_join_out[17];
		///////////////////////////////////////
		arm.position[4] = data_join_out[12];
		arm.velocity[4] = data_join_out[13];
		arm.effort[4] = data_join_out[14];
		///////////////////////////////////////
		arm.position[3] = data_join_out[9];
		arm.velocity[3] = data_join_out[10];
		arm.effort[3] = data_join_out[11];
		////////////////////////////////////////
		arm.position[2] = data_join_out[6];
		arm.velocity[2] = data_join_out[7];
		arm.effort[2] = data_join_out[8];
		//////////////////////////////////////
		arm.position[1] = data_join_out[3];
		arm.velocity[1] = data_join_out[4];
		arm.effort[1] = data_join_out[5];
		//////////////////////////////////////
		arm.position[0] = data_join_out[0];
		arm.velocity[0] = data_join_out[1];
		arm.effort[0] = data_join_out[2];
		//////////////////////////////////////
		fprintf(fp, "\n%10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f, %10.5f", tempo, data_join_out[15], data_join_out[16], data_join_out[17], data_join_out[12], data_join_out[13], data_join_out[14], data_join_out[9], data_join_out[10], data_join_out[11], data_join_out[6], data_join_out[7], data_join_out[8], data_join_out[3], data_join_out[4], data_join_out[5], data_join_out[0], data_join_out[1], data_join_out[2]);
		
		tempo = tempo + 0.02;
		conta = conta + 1;
	
		arm.header.stamp = ros::Time::now();
		arm_pub.publish(arm);
		ros::spinOnce();
		loop_rate.sleep();		
		

	}
	fclose(fp);
	return 0;
} 
