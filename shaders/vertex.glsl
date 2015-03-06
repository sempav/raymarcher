#version 120

attribute vec3 coord;

varying vec2 coord2d;

void main(void) 
{
	gl_Position = vec4(coord, 1.0);

	coord2d = vec2(coord);

	// vec3 world_pos = vec3(model * vec4(coord, 1.0));

	// mat3 normal_matrix = mat3(1.0f);
	// vec3 n = normalize(normal_matrix * normal);

	// vec3 dir_to_camera = normalize(camera_pos - world_pos);

	// for (int i = 0; i < LIGHTS_NUM; i++) {

	// 	vec3 dir_to_light = normalize(light_pos[i] - world_pos);

	// 	float diffuseCoefficient = max(0.0, dot(n, dir_to_light));
	// 	vec3 diffuse = diffuseCoefficient * light_diffuse[i];

	// 	float specularCoefficient = 0.0;
	// 	if(diffuseCoefficient > 0.0)
	// 	    specularCoefficient = pow(max(0.0, dot(dir_to_camera, reflect(-dir_to_light, n))), shininess);
	// 	vec3 specular = specularCoefficient * specularColor * light_diffuse[i];

	// 	f_color += diffuse + specular;
	// }
}
