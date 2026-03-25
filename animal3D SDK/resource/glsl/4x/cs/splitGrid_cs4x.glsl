#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(binding = 1) buffer buf_in {

	float data_in[];
};

layout(binding = 2) buffer buf_out {

	int data_out[];
};
layout(binding = 3) buffer buf_1out {

	int data_out1[];
};
layout(binding = 4) buffer buf_2out {

	int data_out2[];
};
layout(binding = 5) buffer buf_3out {

	int data_out3[];
};


void main()
{
	uint this_index = gl_GlobalInvocationID.x;

	int offset = int((40 * 40) * 0.25);

	/*data_out[this_index] = data_in[this_index];
	data_out1[this_index] = data_in[this_index + offset];
	data_out2[this_index] = data_in[this_index + offset * 2];
	data_out3[this_index] = data_in[this_index + offset * 3];*/

		data_out[0] = 1;
		data_out1[0] = 1;
		data_out2[0] = 1;
		data_out3[0] = 1;


}