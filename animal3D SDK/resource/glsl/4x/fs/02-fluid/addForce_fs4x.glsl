#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;
uniform float dt; 
uniform float add;
uniform bool velocity;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //current divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

//dan's
uint rand_seed_xorshift = 0;
void srand_xorshift(uint seed)
{
	rand_seed_xorshift = seed;
}
uint rand_xorshift()
{
	rand_seed_xorshift ^= rand_seed_xorshift << 13;
	rand_seed_xorshift ^= rand_seed_xorshift >> 17;
	rand_seed_xorshift ^= rand_seed_xorshift <<  5;
	return rand_seed_xorshift;
}

uint rand_seed_parkmiller = 0;
void srand_parkmiller(uint seed)
{
	rand_seed_parkmiller = seed;
}
uint rand_parkmiller()
{
	const uint M = 0x7fffffff;
	const uint A = 48271;
	const uint Q = M / A;
	const uint R = M % A;
	uint div = rand_seed_parkmiller / Q;
	uint rem = rand_seed_parkmiller % Q;
	int s = int(rem * A);
	int t = int(div * R);
	int result = s - t;
	if (result < 0)
		result += int(M);
	rand_seed_parkmiller = uint(result);
	return rand_seed_parkmiller;
}

const uint rand_max_open   = 1<<7;
const uint rand_max_closed = rand_max_open-1;
uint rand_seed = 0;
void srand(uint seed)
{
	rand_seed = seed;
	srand_xorshift(seed);
	srand_parkmiller(seed);
}
uint rand()
{
	rand_seed = rand_xorshift();
	rand_seed = rand_parkmiller();
	rand_seed %= rand_max_open;
	return rand_seed;
}

float randf_open()
{
	return float(rand()) / float(rand_max_open);
}
float randf_closed()
{
	return float(rand()) / float(rand_max_closed);
}
vec3 randv_open()
{
	return vec3(randf_open(), randf_open(), randf_open()) * 2.0 - 1.0;
}
vec3 randv_closed()
{
	return vec3(randf_closed(), randf_closed(), randf_closed()) * 2.0 - 1.0;
}

void main()
{
	if (velocity)
	{
		current.rgb = randv_closed();
	}
	else
	{
		if (vTexcoord_atlas.x >= 0.48 && vTexcoord_atlas.x <= 0.48 && vTexcoord_atlas.y >= 0.48 && vTexcoord_atlas.y <= 0.48)
		{
			current.a = texture(uImage00, vTexcoord_atlas.xy).a * dt * add; 
		}
	}
}
	
