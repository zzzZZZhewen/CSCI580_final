#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec2 ndcPos;

uniform vec3 lightPos;
uniform vec3 objPos;
uniform float radius;

uniform vec3 camPos;
uniform vec3 camFront;
uniform vec3 camRight;
uniform vec3 camUp;

void main()
{
	vec2 clipPos = ndcPos * tan(radians(45.0f/2.0f))*0.1f;
	clipPos.x = clipPos.x * 16.0f / 9.0f;
	vec3 texInWorld = camPos + camFront * 0.1f + camRight * clipPos.x + camUp * clipPos.y;
	vec3 rayDirec = normalize(texInWorld - camPos);
	vec3 rayCurrPos = texInWorld;
	vec3 litToObj = objPos - lightPos;
	float colBright = 0;
	float stepSize = 1;
	int steps = 0;
	while((length(rayCurrPos-objPos) > radius) && (steps < 100))
	{
		rayCurrPos += rayDirec * stepSize;
		vec3 litToRayPoint = rayCurrPos - lightPos;
		float sin = length(cross(normalize(litToRayPoint),normalize(litToObj)));
		float cos = dot(normalize(litToRayPoint),normalize(litToObj));
		if((( sin > radius/length(litToObj)) && (cos > 0.0f))||(sin < radius/length(litToObj) && (length(litToRayPoint)<length(litToObj)))) // don't skip
		{
			float cosTheta = dot(normalize(rayCurrPos-lightPos),-rayDirec);
			float g =  0.5f;
			float result = 1/(4*3.14)* (1 - pow(g,2))/ pow(1 + pow(g,2) -2*g* cosTheta, 1.5);
			colBright += result;
		}
		steps++;
	}
	 
	vec3 col = vec3(colBright/20.0f,colBright/20.0f,colBright/20.0f);
	FragColor = vec4(col, 1.0);	
	
}
	 
	

