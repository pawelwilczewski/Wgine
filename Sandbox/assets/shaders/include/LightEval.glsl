vec3 EvaluateAmbientLight(AmbientLight light, Material mat, vec3 matDiffuse, float matSpecular, vec3 normal)
{
	return matDiffuse * light.Color * mat.Ambient * light.Intensity;
}

vec3 EvaluateDirectionalLight(DirectionalLight light, Material mat, vec3 matDiffuse, float matSpecular, vec3 normal)
{
	// diffuse lighting
	float diff = max(dot(normal, -light.Direction), 0.0);
	vec3 diffuse = matDiffuse * diff * light.Color;

	// specular lighting
	vec3 viewDir = normalize(u_CameraLocation - io_WorldPos);
	vec3 reflectDir = reflect(light.Direction, normal); // TODO: is this correct in the end?
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.Shininess);
	vec3 specular = matSpecular * spec * light.Color;

	return (diffuse + specular) * light.Intensity;
}

vec3 EvaluatePointLight(PointLight light, Material mat, vec3 matDiffuse, float matSpecular, vec3 normal)
{
	// ambient lighting
    vec3 ambient = matDiffuse * mat.Ambient * light.Color;

	// diffuse lighting
	vec3 lightDir = normalize(io_WorldPos - light.Location);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = matDiffuse * diff * light.Color;

	// specular lighting
	vec3 viewDir = normalize(u_CameraLocation - io_WorldPos);
	vec3 reflectDir = reflect(lightDir, normal); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.Shininess);
	vec3 specular = matSpecular * spec * light.Color;

	// attenuation
    float d = max(length(light.Location - io_WorldPos) - light.Radius, 0.0) / light.CutoffRadius; // normalized distance respecting the light radius
	float attenuation = light.Intensity / (d * d); // inverse square, div by 0 possible but the perf hit of removing that is not worth it

	return (ambient + diffuse + specular) * attenuation;
}

vec3 EvaluateSpotLight(SpotLight light, Material mat, vec3 matDiffuse, float matSpecular, vec3 normal)
{
	// ambient lighting
    vec3 ambient = matDiffuse * mat.Ambient * light.Color;

	// diffuse lighting
	vec3 lightDir = normalize(io_WorldPos - light.Location);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = matDiffuse * diff * light.Color;

	// specular lighting
	vec3 viewDir = normalize(u_CameraLocation - io_WorldPos);
	vec3 reflectDir = reflect(lightDir, normal); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.Shininess);
	vec3 specular = matSpecular * spec * light.Color;

	// attenuation
    float d = max(length(light.Location - io_WorldPos) - light.Radius, 0.0) / light.CutoffRadius; // normalized distance respecting the light radius
	float attenuation = light.Intensity / (d * d); // inverse square, div by 0 possible but the perf hit of removing that is not worth it

	// spot light mask
	float angle = degrees(dot(normalize(io_WorldPos - light.Location), light.Direction));
	attenuation *= clamp((angle - light.CutoffAngleInner) / (light.CutoffAngle - light.CutoffAngleInner), 0.0, 1.0);

	return (ambient + diffuse + specular) * attenuation;
}
